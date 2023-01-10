# gob_json

Library for parsing potentially huge json streams on devices with scarce memory .  
Based on mrfaptastic/json-streaming-parser2, modified and extended independently.

Thanks to @mrfaptastic(https://github.com/mrfaptastic/json-streaming-parser2) ,  
@squix78(https://github.com/squix78/json-streaming-parser) and those contributors.


## Overview
When constructing some structure from JSON, it is difficult in an environment with limited memory resources once a JSON object or something similar is used.  
Therefore, there is a certain demand for streaming-type JSON parsers.  
This is because the parser of type streaming can convert JSON directly into a structure.  
(However, it takes time and effort to prepare for this)

- json-streaming-parser  
Simple but functional enough as a streaming type.
- josn-streming-parser2  
Further developed to facilitate key and value retrieval.


## Modifications from json-streaming-parser2

### Namespace management by namespace
Classes and structures now belong to namespace **goblib::json**

### To prevent unexpected substitutions and name collisions in the preprocessor, the definition of the header  has been changed to an enumerated type

### Support operation in environments where the char is signed
Improved to work in non-Arduino environment. It is important that it works with Kanji and pictogram input.  
(Remarks. In Arduino, char type is unsigned)

### Support for integral type size up to "uintmax_t"
The type was up to **long**, but now it is extended to the maximum implementation-dependent size.

### Support for floating-point number type size up to "double"

### Fixed functions causing buffer overruns
e.g.
```cpp
// ElementPath::toString(char* buffer);
void foo(ElementPath& path) {
  char buf[32];
  path.toString(buf); // So dangerous! if path string length over 32.
}
```
Changing toString arguments and return Values.
```cpp
string_t /* String or std::string */ toString() const;
```
### What can be const should be const
Arguments for situations where const is appropriate were changed to const.  
Those that can be const functions were changed to const functions.

### Enhanced internal stack range checks in the parser
Added range checks to prevent crashes.

### Added detection and output of errors in the parser
Log output when internal errors occur and added detection mechanism.

### Added a mechanism to help determine keys and retrieve values
if elseif elseif elseif elseif elseif elseif elseif elseif elseif elseif elseif elseif ... OMG!  
Added a helper for retrive values and a delegation handler for processing per JSON object.  
see also [test_element.cpp](test/test_element.cpp)

### Unit test support with GoogleTest
Even small test cases are useful.

### Add documentation to be generated from source
Documentation can be generated by Doxygen.


## Installation
git clone or download and extract zip file to your library folder.  
If you are using PlatformIO, you can write the following to download automatically.

```ini
lib_deps = https://github.com/GOB52/gob_json.git
```

## Setting
The build option allows you to set several items.

|symbol| description|default value|
|---|---|---|
|GOB_JSON_PARSER_BUFFER_MAX_LENGTH| Token buffer size| 256|
|GOB_JSON_PARSER_KEY_MAX_LENGTH| JSON key token buffer size|32|
|GOB_JSON_PARSER_STACK_MAX_DEPTH|Maximum nesting level of JSON object/array|20|

```ini
build_flags = -D GOB_JSON_PARSER_BUFFER_MAX_LENGTH=384 
  -D GOB_JSON_PARSER_KEY_MAX_LENGTH=64
  -D GOB_JSON_PARSER_STACK_MAX_DEPTH=16
```

## Usage
If you use it like json-streaming-parser2, rename the headers and types to include.

### Header
|gob_json|json-streaming-parser2|
|---|---|
|gob_json.hpp|JsonStreamingParser2.h|
|gob_json_handler.hpp|JsonHandler.h|
|gob_json_element_path.hpp|ElementPath.h|
|gob_json_element_value.hpp|ElementValue.h|
|gob_json_ardiono.hpp|ArduinoStreamParser.h|

### Class
|gob_json|json-streaming-parser2|
|---|---|
|goblib::json::StreamingParser|JsonStreamingParser|
|goblib::json::Handler|JsonHandler|
|goblib::json::ElementPath|ElementPath|
|goblib::json::ElementValue|ElementValue|
|goblib::json::arduino::Stream|ArudinoStreamParser|

About json-streaming-parser2, see also [JSON Streaming Parser 2 README](README.org.md)

I plan to expand the documentation in the future, but for now, 
please see [examples](examples/SimpleParser/SimpleParser.ino) and [test](test/test_basic.cpp).


## Documents
You can make documents by [Doxygen](https://www.doxygen.nl/index.html). Use my [Doxyfile](doc/Doxyfile).  

## UnitTest
You can run [GoogleTest](https://google.github.io/googletest/) using [platformio.ini](platformio.ini).


## Acknowledgments
I would like to thank the author and contributors of json-streaming-parser/2 again. I salute your great job.
