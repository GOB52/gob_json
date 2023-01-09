/*!
  @file gob_json_arduino.hpp
  @brief Comatible class of Arduino Stream.
 */
#ifndef GOB_JSON_ARDUINO_HPP
#define GOB_JSON_ARDUINO_HPP

#ifdef ARDUINO

#include <Stream.h>
#include "gob_json.hpp"

namespace goblib { namespace json {
/*! @namespace arduino For arduino */
namespace arduino {

/*!
  @class Stream
  @brief Write stream derived from Arduino Stream
 */
class Stream : public ::Stream, public StreamingParser
{
  public:
    static constexpr int ERROR_PARSE_JSON = 52;

    explicit Stream(goblib::json::Handler* h = nullptr) : ::Stream(), StreamingParser(h) {}

    ///@name override class Print
    ///@{
    virtual size_t write(uint8_t v) override
    {
        if(getWriteError()==0)
        {
            parse(v);
            if(hasError()) { setWriteError(ERROR_PARSE_JSON); }
        }
        return (hasError() == false);
    }
    ///@}

    using Print::write;
    
    ///@name override class Stream
    ///@{
    inline virtual int available() override { return (state != State::ERROR) || (state != State::DONE); }
    inline virtual int peek() override { return 0; }
    inline virtual int read() override { return 0; }
    ///@}
};
//
}}}
#endif

#endif
