/*!
  @file gob_json.cpp
  @brief JSON streaming parser
 */
#include "gob_json.hpp"
#include "internal/gob_json_log.hpp"
#include <cstring>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <cinttypes> 

namespace goblib { namespace json {

void StreamingParser::reset()
{
    state = State::START_DOCUMENT;
    bufferPos = 0;
    unicodeEscapeBufferPos = 0;
    unicodeBufferPos = 0;
    characterCounter = 0;
    stackPos = 0;
}

#define PARSE_ERROR(estr, ch, pos, path) \
do \
{  \
    GOB_JSON_LOGE("%s at <%c>:0x%x pos:%zu [%s]", estr, ch, ch, pos, path.toString().c_str()); \
    state = State::ERROR; \
}while(0)

void StreamingParser::parse(const char ch)
{
    assert(handler && "handler must be set");
    if(!handler || state == State::ERROR) { return; }

    const int c = curCh = std::is_signed<char>::value ? (unsigned char)ch : ch; // Handling the case where char is signed.

    //GOB_JSON_LOGI("stack[%d]:%d <%c>0x%x", stackPos, (stackPos > 0) ? (int)stack[stackPos-1] : -1, ch, ch);

    // valid whitespace characters in JSON (from RFC4627 for JSON) include:
    // space, horizontal tab, line feed or new line, and carriage return.
    // thanks:
    // http://stackoverflow.com/questions/16042274/definition-of-whitespace-in-json
    if ((c == ' ' || c == '\t' || c == '\n' || c == '\r')
        && !(state == State::IN_STRING || state == State::UNICODE || state == State::START_ESCAPE || state == State::IN_NUMBER)) {
        return;
    }

    switch (state) {
    case State::IN_STRING:
        if (c == '"') {
            endString();
        } else if (c == '\\') {
            state = State::START_ESCAPE;
        } else if ((c < 0x1f) || (c == 0x7f)) {
            PARSE_ERROR("Unescaped control character encountered", c, characterCounter, path);
            break;
            //throw new RuntimeException("Unescaped control character encountered: " + c + " at position" + characterCounter);
        } else {
            buffer[bufferPos] = c;
            increaseBufferPointer();
        }
        break;
    case State::IN_ARRAY:
        if (c == ']') {
            endArray();
        } else {
            path.getCurrent()->step();
            startValue(c);
        }
        break;
    case State::IN_OBJECT:
        if (c == '}') {
            endObject();
        } else if (c == '"') {
            startKey();
        } else {
            PARSE_ERROR("Start of string expected for object key. Instead got", c, characterCounter, path);
            break;
            //throw new RuntimeException("Start of string expected for object key. Instead got: " + c + " at position" + characterCounter);
        }
        break;
    case State::END_KEY:
        if (c != ':') {
            PARSE_ERROR("Expected ':' after key. Instead got", c, characterCounter, path);
            break;
            //throw new RuntimeException("Expected ':' after key. Instead got " + c + " at position" + characterCounter);
        }
        state = State::AFTER_KEY;
        break;
    case State::AFTER_KEY:
        startValue(c);
        break;
    case State::START_ESCAPE:
        processEscapeCharacters(c);
        break;
    case State::UNICODE:
        processUnicodeCharacter(c);
        break;
    case State::UNICODE_SURROGATE:
        unicodeEscapeBuffer[unicodeEscapeBufferPos] = c;
        unicodeEscapeBufferPos++;
        if (unicodeEscapeBufferPos == 2) {
            endUnicodeSurrogateInterstitial();
        }
        break;
    case State::AFTER_VALUE: {
        if(stackPos <= 0)
        {
            PARSE_ERROR("stackPos <= 0", c, characterCounter, path);
            break;
        }
        // not safe for size == 0!!!
        auto within = stack[stackPos - 1];
        if (within == Stack::OBJECT) {
            if (c == '}') {
                endObject();
            } else if (c == ',') {
                state = State::IN_OBJECT;
            } else {
                //                PARSE_ERROR("Expected ',' or '}' while parsing object. Got: <%c> pos:%zu", c, characterCounter);
                //                break;
                //throw new RuntimeException("Expected ',' or '}' while parsing object. Got: " + c + ". " + characterCounter);
            }
        } else if (within == Stack::ARRAY) {
            if (c == ']') {
                endArray();
            } else if (c == ',') {
                state = State::IN_ARRAY;
            } else {
                PARSE_ERROR("Expected ',' or ']' while parsing array. Got", c, characterCounter, path);
                break;
                //throw new RuntimeException("Expected ',' or ']' while parsing array. Got: " + c + ". " + characterCounter);

            }
        } else {
            PARSE_ERROR("Finished a literal, but unclear what state to move to. Last state:", c, characterCounter, path);
            break;
            //throw new RuntimeException("Finished a literal, but unclear what state to move to. Last state: " + characterCounter);
        }
    }break;
    case State::IN_NUMBER:
        if (c >= '0' && c <= '9') {
            buffer[bufferPos] = c;
            increaseBufferPointer();
        } else if (c == '.') {
            if (doesCharArrayContain(buffer, bufferPos, '.')) {
                PARSE_ERROR("Cannot have multiple decimal points in a number", c, characterCounter, path);
                break;
                //throw new RuntimeException("Cannot have multiple decimal points in a number. " + characterCounter);
            } else if (doesCharArrayContain(buffer, bufferPos, 'e')) {
                PARSE_ERROR("Cannot have a decimal point in an exponent", c, characterCounter, path);
                break;
                //throw new RuntimeException("Cannot have a decimal point in an exponent." + characterCounter);
            }
            buffer[bufferPos] = c;
            increaseBufferPointer();
        } else if (c == 'e' || c == 'E') {
            if (doesCharArrayContain(buffer, bufferPos, 'e')) {
                PARSE_ERROR("Cannot have multiple exponents in a number", c, characterCounter, path);
                break;
                //throw new RuntimeException("Cannot have multiple exponents in a number. " + characterCounter);
            }
            buffer[bufferPos] = c;
            increaseBufferPointer();
        } else if (c == '+' || c == '-') {
            char last = buffer[bufferPos - 1];
            if (!(last == 'e' || last == 'E')) {
                PARSE_ERROR("Can only have '+' or '-' after the 'e' or 'E' in a number", c, characterCounter, path);
                break;
                //throw new RuntimeException("Can only have '+' or '-' after the 'e' or 'E' in a number." + characterCounter);
            }
            buffer[bufferPos] = c;
            increaseBufferPointer();
        } else {
            endNumber();
            // we have consumed one beyond the end of the number
            parse(c);
        }
        break;
    case State::IN_TRUE:
        buffer[bufferPos] = c;
        increaseBufferPointer();
        if (bufferPos == 4) {
            endTrue();
        }
        break;
    case State::IN_FALSE:
        buffer[bufferPos] = c;
        increaseBufferPointer();
        if (bufferPos == 5) {
            endFalse();
        }
        break;
    case State::IN_NULL:
        buffer[bufferPos] = c;
        increaseBufferPointer();
        if (bufferPos == 4) {
            endNull();
        }
        break;
    case State::START_DOCUMENT:
        if (c == '[') {
            handler->startDocument();
            startArray();
        } else if (c == '{') {
            handler->startDocument();
            startObject();
        } else {
            // throw new ParsingError($this->_line_number,
            // $this->_char_number,
            // "Document must start with object or array.");
        }
        break;
        //case State::DONE:
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected end of document.");
        //default:
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Internal error. Reached an unknown state: ".$this->_state);
    default: break;
    }
    characterCounter++;
}

void StreamingParser::increaseBufferPointer() {
    bufferPos = std::min((size_t)bufferPos + 1, sizeof(buffer) - 1);
}

void StreamingParser::endString() {
    if(stackPos <= 0)
    {
        PARSE_ERROR("stackPos <= 0", curCh, characterCounter, path);
        return;
    }
    auto popped = stack[stackPos - 1];
    stackPos--;
    if (popped == Stack::KEY) {
        buffer[bufferPos] = '\0';
        path.getCurrent()->set(buffer);
        state = State::END_KEY;
    } else if (popped == Stack::STRING) {
        buffer[bufferPos] = '\0';
        handler->value(path, elementValue.with(buffer));
        state = State::AFTER_VALUE;
    } else {
        PARSE_ERROR("Unexpected end of string", curCh, characterCounter, path);
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Unexpected end of string.");
    }
    bufferPos = 0;
}
void StreamingParser::startValue(char c) {
    if (c == '[') {
        startArray();
    } else if (c == '{') {
        startObject();
    } else if (c == '"') {
        startString();
    } else if (isDigit(c)) {
        startNumber(c);
    } else if (c == 't') {
        state = State::IN_TRUE;
        buffer[bufferPos] = c;
        increaseBufferPointer();
    } else if (c == 'f') {
        state = State::IN_FALSE;
        buffer[bufferPos] = c;
        increaseBufferPointer();
    } else if (c == 'n') {
        state = State::IN_NULL;
        buffer[bufferPos] = c;
        increaseBufferPointer();
    } else {
        PARSE_ERROR("Unexpected character for value", c, characterCounter, path);
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Unexpected character for value: ".$c);
    }
}

bool StreamingParser::isDigit(char c) {
    // Only concerned with the first character in a number.
    return (c >= '0' && c <= '9') || c == '-';
}

void StreamingParser::endArray() {
    if(stackPos <= 0)
    {
        PARSE_ERROR("stackPos <= 0", curCh, characterCounter, path);
        return;
    }
    auto popped = stack[stackPos - 1];
    stackPos--;
    path.pop();
    if (popped != Stack::ARRAY) {
        PARSE_ERROR("Unexpected end of array encountered", curCh, characterCounter, path);
        return;
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Unexpected end of array encountered.");
    }
    handler->endArray(path);
    state = State::AFTER_VALUE;
    if (stackPos == 0) {
        endDocument();
    }
}

void StreamingParser::startKey() {
    if(stackPos >= (int)sizeof(stack))
    {
        PARSE_ERROR("stack overflow", curCh, characterCounter, path);
        return;
    }
    stack[stackPos] = Stack::KEY;
    stackPos++;
    state = State::IN_STRING;
}

void StreamingParser::endObject() {
    auto popped = stack[stackPos - 1];
    stackPos--;
    path.pop();
    if (popped != Stack::OBJECT) {
        PARSE_ERROR("Unexpected end of object encountered", curCh, characterCounter, path);
        return;
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Unexpected end of object encountered.");
    }
    handler->endObject(path);
    state = State::AFTER_VALUE;
    if (stackPos == 0) {
        endDocument();
    }
}

void StreamingParser::processEscapeCharacters(char c) {
    if (c == '"') {
        buffer[bufferPos] = '"';
        increaseBufferPointer();
    } else if (c == '\\') {
        buffer[bufferPos] = '\\';
        increaseBufferPointer();
    } else if (c == '/') {
        buffer[bufferPos] = '/';
        increaseBufferPointer();
    } else if (c == 'b') {
        buffer[bufferPos] = 0x08;
        increaseBufferPointer();
    } else if (c == 'f') {
        buffer[bufferPos] = '\f';
        increaseBufferPointer();
    } else if (c == 'n') {
        buffer[bufferPos] = '\n';
        increaseBufferPointer();
    } else if (c == 'r') {
        buffer[bufferPos] = '\r';
        increaseBufferPointer();
    } else if (c == 't') {
        buffer[bufferPos] = '\t';
        increaseBufferPointer();
    } else if (c == 'u') {
        state = State::UNICODE;
    } else {
        PARSE_ERROR("Expected escaped character after backslash. Got:", c, characterCounter, path);
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected escaped character after backslash. Got: ".$c);
    }
    if (state != State::UNICODE) {
        state = State::IN_STRING;
    }
}

void StreamingParser::processUnicodeCharacter(char c) {
    if (!isHexCharacter(c)) {
        PARSE_ERROR("Expected hex character for escaped Unicode character. Unicode parsed", c, characterCounter, path);
        return;
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected hex character for escaped Unicode character. Unicode parsed: "
        // . implode($this->_unicode_buffer) . " and got: ".$c);
    }

    unicodeBuffer[unicodeBufferPos] = c;
    unicodeBufferPos++;

    if (unicodeBufferPos == 4) {
        int codepoint = getHexArrayAsDecimal(unicodeBuffer, unicodeBufferPos);
        endUnicodeCharacter(codepoint);
        return;
        /*if (codepoint >= 0xD800 && codepoint < 0xDC00) {
          unicodeHighSurrogate = codepoint;
          unicodeBufferPos = 0;
          state = State::UNICODE_SURROGATE;
          } else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
          if (unicodeHighSurrogate == -1) {
          // throw new ParsingError($this->_line_number,
          // $this->_char_number,
          // "Missing high surrogate for Unicode low surrogate.");
          }
          int combinedCodePoint = ((unicodeHighSurrogate - 0xD800) * 0x400) + (codepoint - 0xDC00) + 0x10000;
          endUnicodeCharacter(combinedCodePoint);
          } else if (unicodeHighSurrogate != -1) {
          // throw new ParsingError($this->_line_number,
          // $this->_char_number,
          // "Invalid low surrogate following Unicode high surrogate.");
          endUnicodeCharacter(codepoint);
          } else {
          endUnicodeCharacter(codepoint);
          }*/
    }
}
bool StreamingParser::isHexCharacter(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int StreamingParser::getHexArrayAsDecimal(char hexArray[], int length) {
    int result = 0;
    for (int i = length; i >= 0; i--) {
        char current = hexArray[length - i - 1];
        int value = 0;
        if (current >= 'a' && current <= 'f') {
            value = current - 'a' + 10;
        } else if (current >= 'A' && current <= 'F') {
            value = current - 'A' + 10;
        } else if (current >= '0' && current <= '9') {
            value = current - '0';
        }
        result = (result << 4) | value;
    }
    return result;
}

bool StreamingParser::doesCharArrayContain(char myArray[], int length, char c) {
    for (int i = 0; i < length; i++) {
        if (myArray[i] == c) {
            return true;
        }
    }
    return false;
}

void StreamingParser::endUnicodeSurrogateInterstitial() {
    char unicodeEscape = unicodeEscapeBuffer[unicodeEscapeBufferPos - 1];
    if (unicodeEscape != 'u') {
        PARSE_ERROR("Expected '\\u' following a Unicode high surrogate", curCh, characterCounter, path);
        return;
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected '\\u' following a Unicode high surrogate. Got: " .
        // $unicode_escape);
    }
    unicodeBufferPos = 0;
    unicodeEscapeBufferPos = 0;
    state = State::UNICODE;
}

void StreamingParser::endNumber() {
    buffer[bufferPos] = '\0';
#if 1
    if (strchr(buffer, '.') != NULL) {
        double floatValue;
        sscanf(buffer, "%lf", &floatValue);
        handler->value(path, elementValue.with(static_cast<ElementValue::fp_t>(floatValue)));
    }
    else if(buffer[0] == '-')
    {
        intmax_t intValue;
        sscanf(buffer, "%jd", &intValue);
        handler->value(path, elementValue.with(static_cast<ElementValue::number_t>(intValue)));
    }
    else
    {
        uintmax_t intValue;
        sscanf(buffer, "%ju", &intValue);
        handler->value(path, elementValue.with(static_cast<ElementValue::number_t>(intValue)));
    }
#else
    // Floating-point
    if(strchr(buffer, '.') != nullptr)
    {
        handler->value(path, elementValue.with(static_cast<ElementValue::fp_t>(std::strtod(buffer, nullptr))) );
    }
    // Signed integer
    else if(buffer[0] == '-')
    {
        handler->value(path,
                       elementValue.with(static_cast<ElementValue::number_t>(std::strtoimax(buffer, nullptr, 10))) );
    }
    // Assume unsigned integarer
    else
    {
        handler->value(path,
                       elementValue.with(static_cast<ElementValue::number_t>(std::strtoumax(buffer, nullptr, 10))) );
    }
#endif

    bufferPos = 0;
    state = State::AFTER_VALUE;
}

int StreamingParser::convertDecimalBufferToInt(char myArray[], int length) {
    int result = 0;
    for (int i = 0; i < length; i++) {
        char current = myArray[length - i - 1];
        result += (current - '0') * 10;
    }
    return result;
}

void StreamingParser::endDocument() {
    handler->endDocument();
    if(recursive) { reset(); }
    else { state = State::DONE; }
}

void StreamingParser::endTrue() {
    buffer[bufferPos] = '\0';
    if(strcmp(buffer, "true") == 0) {
        handler->value(path, elementValue.with(true));
    } else {
        PARSE_ERROR("Expected 'true'", curCh, characterCounter, path);
        return;
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected 'true'. Got: ".$true);
    }
    bufferPos = 0;
    state = State::AFTER_VALUE;
}

void StreamingParser::endFalse() {
    buffer[bufferPos] = '\0';
    if(strcmp(buffer, "false") == 0) {
        handler->value(path, elementValue.with(false));
    } else {
        PARSE_ERROR("Expected 'false'", curCh, characterCounter, path);
        return;
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected 'true'. Got: ".$true);
    }
    bufferPos = 0;
    state = State::AFTER_VALUE;
}

void StreamingParser::endNull() {
    buffer[bufferPos] = '\0';
    if(strcmp(buffer, "null") == 0) {
        handler->value(path, elementValue.with());
    } else {
        PARSE_ERROR("Expected 'null'", curCh, characterCounter, path);
        return;
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected 'true'. Got: ".$true);
    }
    bufferPos = 0;
    state = State::AFTER_VALUE;
}

void StreamingParser::startArray() {
    if(stackPos >= (int)sizeof(stack))
    {
        PARSE_ERROR("stack overflow", curCh, characterCounter, path);
        return;
    }
    handler->startArray(path);
    state = State::IN_ARRAY;
    stack[stackPos] = Stack::ARRAY;
    path.push(); 
    stackPos++;
}

void StreamingParser::startObject() {
    if(stackPos >= (int)sizeof(stack))
    {
        PARSE_ERROR("stack overflow", curCh, characterCounter, path);
        return;
    }
    handler->startObject(path);
    state = State::IN_OBJECT;
    stack[stackPos] = Stack::OBJECT;
    path.push(); 
    stackPos++;
}

void StreamingParser::startString() {
    if(stackPos >= (int)sizeof(stack))
    {
        PARSE_ERROR("stack overflow", curCh, characterCounter, path);
        return;
    }
    stack[stackPos] = Stack::STRING;
    stackPos++;
    state = State::IN_STRING;
}

void StreamingParser::startNumber(char c) {
    state = State::IN_NUMBER;
    buffer[bufferPos] = c;
    increaseBufferPointer();
}

void StreamingParser::endUnicodeCharacter(int codepoint) {
    if (codepoint < 0x80){
        buffer[bufferPos] = (char) (codepoint);
    } else if (codepoint <= 0x800){
        buffer[bufferPos] = (char) ((codepoint >> 6) | 0b11000000);
        increaseBufferPointer();
        buffer[bufferPos] = (char) ((codepoint & 0b00111111) | 0b10000000);
    } else if (codepoint == 0x2019){
        buffer[bufferPos] = '\''; // \u2019 ’
    } else {
        buffer[bufferPos] = ' ';
    }
    increaseBufferPointer();
    unicodeBufferPos = 0;
    unicodeHighSurrogate = -1;
    state = State::IN_STRING;
}

//
}}
