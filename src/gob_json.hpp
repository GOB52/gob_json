/*!
  @file gob_json.hpp
  @brief JSON streaming parser

  @mainpage gob_json
  Based on mrfaptastic/json-streaming-parser2, modified and extended independently.
  @sa https://github.com/mrfaptastic/json-streaming-parser2/ (Forked from squix78/json-streaming-parser)
  @sa https://github.com/squix78/json-streaming-parser
  
  @copyright 2023 GOB
  @copyright Licensed under the MIT license. See LICENSE file in the project root for full license information.
*/
#ifndef GOB_JSON_HPP
#define GOB_JSON_HPP

#include "gob_json_typedef.hpp"
#include "gob_json_handler.hpp"
#include "gob_json_element_path.hpp"
#include "gob_json_element_value.hpp"

/*!
  @namespace goblib
  @brief Top level namespace of mine
*/
namespace goblib {
/*!
  @namespace json
  @brief JSON related
*/
namespace json {

#ifndef GOB_JSON_STRINGIFY
# define GOB_JSON_STRINGIFY(x) GOB_JSON_STRINGIFY_AGAIN(x)
#endif
#ifndef GOB_JSON_STRINGIFY_AGAIN
# define GOB_JSON_STRINGIFY_AGAIN(x) #x
#endif

#ifndef GOB_JSON_PARSER_BUFFER_MAX_LENGTH
# pragma message "Buffer length as default"
# define GOB_JSON_PARSER_BUFFER_MAX_LENGTH  (256)
#else
# pragma message "Defined buffer length=" GOB_JSON_STRINGIFY(GOB_JSON_PARSER_BUFFER_MAX_LENGTH)
#endif

/*!
  @class StreamingParser
  @brief JSON streaming parser
 */
class StreamingParser
{
  public:
    /*!
      @brief Constructor
      @param handler Handler
     */
    explicit StreamingParser(Handler* h = nullptr) { reset(); setHandler(h); }

    /*! @brief Set handler */
    void setHandler(Handler* h) { handler = h; }
    /*! @brief Reset inner state.*/
    void reset();

    /*! @brief Parse 1 character */
    void parse(const char ch);
    /*! @brief Parse buffer */
    void parse(const char* buf, size_t len)
    {
        while(len--) { parse(*buf++); }
    }
    /*! @brief Parsing JSON documents recursively */
    void setRecursively(const bool b) { recursive = b; }

    /*! @brief Any errors? */
    bool hasError() const { return state == State::ERROR; }
    
  protected:
    void startArray();
    void endArray();

    void startObject();
    void endObject();
    
    void startString();
    void endString();

    void startNumber(char c);
    void endNumber();
    
    void startValue(char c);
    void startKey();

    void endNull();
    void endFalse();
    void endTrue();
    void endDocument();
    void endUnicodeSurrogateInterstitial();
    void endUnicodeCharacter(int codepoint);

    void increaseBufferPointer();
    void processEscapeCharacters(char c);

    bool isDigit(char c);
    bool isHexCharacter(char c);

    char convertCodepointToCharacter(int num);
    int convertDecimalBufferToInt(char myArray[], int length);
    bool doesCharArrayContain(char myArray[], int length, char c);
    int getHexArrayAsDecimal(char hexArray[], int length);
    void processUnicodeCharacter(char c);

    enum class State : int8_t
    {
        ERROR = -1,
        DONE,
        START_DOCUMENT,
        IN_ARRAY,
        IN_OBJECT,
        END_KEY,
        AFTER_KEY,
        IN_STRING,
        START_ESCAPE,
        UNICODE,
        IN_NUMBER,
        IN_TRUE,
        IN_FALSE,
        IN_NULL,
        AFTER_VALUE,
        UNICODE_SURROGATE,
    };
    enum class Stack : uint8_t
    {
        OBJECT,
        ARRAY,
        KEY,
        STRING,
    };

    Handler* handler{nullptr};
    ElementValue elementValue{};
    ElementPath path{};

    State state{State::START_DOCUMENT};
    Stack stack[GOB_JSON_PARSER_STACK_MAX_DEPTH]{};
    int stackPos{0};
    
    bool recursive{false};
    bool doEmitWhitespace{false};

    char buffer[GOB_JSON_PARSER_BUFFER_MAX_LENGTH]{};
    int bufferPos{0};

    char unicodeEscapeBuffer[10];
    int unicodeEscapeBufferPos{0};;
    char unicodeBuffer[10];
    int unicodeBufferPos{0};
    int unicodeHighSurrogate{0};

    size_t characterCounter{0};
    int curCh{}; // for error information.
};
//
}}
#endif
