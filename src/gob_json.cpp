/*!
  @file gob_json.cpp
  @brief JSON streaming parser
 */
#ifdef ARDUINO_ARCH_ESP8266
#include <Arduino.h>
#ifdef min
#undef min
#endif
#endif
#include "gob_json.hpp"
#include <cstring>
#include <cassert>
#include <algorithm>

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

void StreamingParser::parse(const char ch)
{
    assert(handler && "handler must be set");
    if(!handler) { return; }

    const int c = std::is_signed<char>::value ? (unsigned char)ch : ch; // Handling the case where char is signed.
    
#ifdef ARDUINO_ARCH_ESP8266	
    yield(); // reduce crashes
#endif
    //System.out.print(c);
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
            //throw new RuntimeException("Start of string expected for object key. Instead got: " + c + " at position" + characterCounter);
        }
        break;
    case State::END_KEY:
        if (c != ':') {
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
        // not safe for size == 0!!!
        auto within = stack[stackPos - 1];
        if (within == Stack::OBJECT) {
            if (c == '}') {
                endObject();
            } else if (c == ',') {
                state = State::IN_OBJECT;
            } else {
                //throw new RuntimeException("Expected ',' or '}' while parsing object. Got: " + c + ". " + characterCounter);
            }
        } else if (within == Stack::ARRAY) {
            if (c == ']') {
                endArray();
            } else if (c == ',') {
                state = State::IN_ARRAY;
            } else {
                //throw new RuntimeException("Expected ',' or ']' while parsing array. Got: " + c + ". " + characterCounter);

            }
        } else {
            //throw new RuntimeException("Finished a literal, but unclear what state to move to. Last state: " + characterCounter);
        }
    }break;
    case State::IN_NUMBER:
        if (c >= '0' && c <= '9') {
            buffer[bufferPos] = c;
            increaseBufferPointer();
        } else if (c == '.') {
            if (doesCharArrayContain(buffer, bufferPos, '.')) {
                //throw new RuntimeException("Cannot have multiple decimal points in a number. " + characterCounter);
            } else if (doesCharArrayContain(buffer, bufferPos, 'e')) {
                //throw new RuntimeException("Cannot have a decimal point in an exponent." + characterCounter);
            }
            buffer[bufferPos] = c;
            increaseBufferPointer();
        } else if (c == 'e' || c == 'E') {
            if (doesCharArrayContain(buffer, bufferPos, 'e')) {
                //throw new RuntimeException("Cannot have multiple exponents in a number. " + characterCounter);
            }
            buffer[bufferPos] = c;
            increaseBufferPointer();
        } else if (c == '+' || c == '-') {
            char last = buffer[bufferPos - 1];
            if (!(last == 'e' || last == 'E')) {
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
    bufferPos = std::min(bufferPos + 1, JSON_PARSER_BUFFER_MAX_LENGTH - 1);
}

void StreamingParser::endString() {
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
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Unexpected end of string.");
    }
    bufferPos = 0;
}
void StreamingParser::startValue(char c) {
	
#ifdef ARDUINO_ARCH_ESP8266	
    yield();
#endif
	
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
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Unexpected character for value: ".$c);
    }
}

bool StreamingParser::isDigit(char c) {
    // Only concerned with the first character in a number.
    return (c >= '0' && c <= '9') || c == '-';
}

void StreamingParser::endArray() {
    auto popped = stack[stackPos - 1];
    stackPos--;
    path.pop();
    if (popped != Stack::ARRAY) {
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
    stack[stackPos] = Stack::KEY;
    stackPos++;
    state = State::IN_STRING;
}

void StreamingParser::endObject() {
    auto popped = stack[stackPos];
    stackPos--;
    path.pop();
    if (popped != Stack::OBJECT) {
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
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected escaped character after backslash. Got: ".$c);
    }
    if (state != State::UNICODE) {
        state = State::IN_STRING;
    }
}

void StreamingParser::processUnicodeCharacter(char c) {
    if (!isHexCharacter(c)) {
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
    if (strchr(buffer, '.') != NULL) {
        double floatValue;
        sscanf(buffer, "%lf", &floatValue);
        handler->value(path, elementValue.with(floatValue));
    } else {
        long long intValue;
        sscanf(buffer, "%lld", &intValue);
        handler->value(path, elementValue.with(intValue));
    }
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
        // throw new ParsingError($this->_line_number, $this->_char_number,
        // "Expected 'true'. Got: ".$true);
    }
    bufferPos = 0;
    state = State::AFTER_VALUE;
}

void StreamingParser::startArray() {
    handler->startArray(path);
    state = State::IN_ARRAY;
    stack[stackPos] = Stack::ARRAY;
    path.push(); 
    stackPos++;
}

void StreamingParser::startObject() {
    handler->startObject(path);
    state = State::IN_OBJECT;
    stack[stackPos] = Stack::OBJECT;
    path.push(); 
    stackPos++;
}

void StreamingParser::startString() {
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
