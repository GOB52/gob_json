/*!
  @file gob_json_arduino.cpp
  @brief For Arduino stream
 */
#ifdef ARDUINO
#include "gob_json_arduino.hpp"

namespace goblib { namespace json {

size_t ArudinoStreamParser::write(const uint8_t *data, size_t size)
{
    if(size && data) 
    {
        for (size_t idx=0; idx<size; idx++)
        {
            uint8_t char_val = *(data + idx);
            parse(char_val);
        }
        return size;
    }
    return 0;
}

size_t ArudinoStreamParser::write(uint8_t data)
{
    parse(data);
    return 1;
}

int ArudinoStreamParser::available() {
    return 1;
}

int ArudinoStreamParser::read() {
    return -1;
}

int ArudinoStreamParser::peek() {
    return -1;
}

void ArudinoStreamParser::flush() {
}

//
}}
#endif
