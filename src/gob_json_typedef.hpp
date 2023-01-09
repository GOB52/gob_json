/*!
  @file gob_json_typedef.hpp
  @brief Some type definition for native and embedded.
 */
#ifndef GOB_JSON_TYPEDEF_HPP
#define GOB_JSON_TYPEDEF_HPP

#ifdef ARDUINO
#include <WString.h>
#else
#include <string>
#endif
#include <cstdio>
#include <cstdarg>

namespace goblib { namespace json {

/*!
  @typedef string_t
  @brief String type. String if Arduino, std::string if other.
 */
#if defined(ARDUINO)
using string_t = String;
#else
using string_t = std::string;
#endif

string_t formatString(const char* fmt, ...);
//
}}
#endif
