/*!
  @file gob_json_arduino.hpp
  @brief For Arduino stream
 */
#ifndef GOB_JSON_ARDUINO_HPP
#define GOB_JSON_ARDUINO_HPP
#ifdef ARDUINO

#include <Print.h>
#include <Stream.h>
#include <Arduino.h>
#include "gob_json.hpp"

namespace goblib { namespace json {

class ArudinoStreamParser: public Stream, public StreamingParser
{
  public:
    int available() override;

    int read() override;

    size_t write(const uint8_t *buffer, size_t size) override;
    size_t write(uint8_t data) override;

    void flush() override;
    int peek() override;
};
//
}}
#endif
#endif
