/*!
  @file gob_json_typedef.cpp
  @brief Some type definition for native and embedded.
 */
#include "gob_json_typedef.hpp"

namespace goblib { namespace json {

string_t formatString(const char* fmt, ...)
{
    // calculate length
    va_list args;
    va_start(args, fmt);
    size_t sz = vsnprintf(nullptr, 0U, fmt, args); 
    va_end(args); 

    char buf[sz + 1];
    va_start(args, fmt); // Reinitiaize args (args cannot reuse because indefinite value after vsnprintf)
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    buf[sz] = '\0';

    return string_t(buf);
}
//
}}

