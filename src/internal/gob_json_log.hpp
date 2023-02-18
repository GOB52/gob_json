/*!
  @brief Logging
  @file gob_json_log.hpp

  @note Display level is controlled by GOB_JSON_LOG_LEVEL
  @note (0:None 1:Error 2:Warning 3:Information 4:Debug 5:Verbose)
  @note If GOB_JSON_LOG_LEVEL is not specified in the compile options,
  @note it is assumed that LOG_LOCAL_LEVEL or CORE_DEBUG_LEVEL is specified.
*/
#ifndef GOBLIB_JSON_LOG_HPP
#define GOBLIB_JSON_LOG_HPP

#if !defined(NDEBUG) || defined(DOXYGEN_PROCESS)

# if defined(ARDUINO)
#   include <esp32-hal-log.h>
#   ifndef GOB_JSON_LOG_LEVEL
#     if defined(LOG_LOCAL_LEVEL)
#       pragma message "[gob_json Using LOG_LOCAL_LEVEL"
#       define GOB_JSON_LOG_LEVEL (LOG_LOCAL_LEVEL)
#     elif defined(CORE_DEBUG_LEVEL)
#       pragma message "[gob_json] Using CORE_DEBUG_LEVEL"
#       define GOB_JSON_LOG_LEVEL (CORE_DEBUG_LEVEL)
#     else
#       pragma message "[gob_json] Using loglevel 3"
#       define GOB_JSON_LOG_LEVEL (3))
#     endif
#   else
#     pragma message "[gob_json] Using defined log level"
#   endif
/*! @brief Error */
#   define GOB_JSON_LOGE(format, ...) do { if(GOB_JSON_LOG_LEVEL >= ESP_LOG_ERROR)   { log_printf(ARDUHAL_LOG_FORMAT(E, format), ##__VA_ARGS__); } } while(0)
/*! @brief Warning */
#   define GOB_JSON_LOGW(format, ...) do { if(GOB_JSON_LOG_LEVEL >= ESP_LOG_WARN)    { log_printf(ARDUHAL_LOG_FORMAT(W, format), ##__VA_ARGS__); } } while(0)
/*! @brief Information */
#   define GOB_JSON_LOGI(format, ...) do { if(GOB_JSON_LOG_LEVEL >= ESP_LOG_INFO)    { log_printf(ARDUHAL_LOG_FORMAT(I, format), ##__VA_ARGS__); } } while(0)
/*! @brief Debug */
#   define GOB_JSON_LOGD(format, ...) do { if(GOB_JSON_LOG_LEVEL >= ESP_LOG_DEBUG)   { log_printf(ARDUHAL_LOG_FORMAT(D, format), ##__VA_ARGS__); } } while(0)
/*! @brief Verbose */
#   define GOB_JSON_LOGV(format, ...) do { if(GOB_JSON_LOG_LEVEL >= ESP_LOG_VERBOSE) { log_printf(ARDUHAL_LOG_FORMAT(V, format), ##__VA_ARGS__); } } while(0)

# else

#   include <cstdio>
#   ifndef GOB_JSON_LOG_LEVEL
#     pragma message "[gob_json] Using loglevel 3"
#     define GOB_JSON_LOG_LEVEL (3)
#   else
#     pragma message "[gob_json] Using defined log level"
#   endif

#   define GOB_JSON_LOG(fmt, ...) do { printf("%s:%d ", __FILE__, __LINE__); printf(fmt, ##__VA_ARGS__); putchar('\n'); }while(0)
#   define GOB_JSON_LOGE(format, ...) do { if(GOB_JSON_LOG_LEVEL >= 1) { GOB_JSON_LOG("[E] "#format, ##__VA_ARGS__); } } while(0)
#   define GOB_JSON_LOGW(format, ...) do { if(GOB_JSON_LOG_LEVEL >= 2) { GOB_JSON_LOG("[W] "#format, ##__VA_ARGS__); } } while(0)
#   define GOB_JSON_LOGI(format, ...) do { if(GOB_JSON_LOG_LEVEL >= 3) { GOB_JSON_LOG("[I] "#format, ##__VA_ARGS__); } } while(0)
#   define GOB_JSON_LOGD(format, ...) do { if(GOB_JSON_LOG_LEVEL >= 4) { GOB_JSON_LOG("[D] "#format, ##__VA_ARGS__); } } while(0)
#   define GOB_JSON_LOGV(format, ...) do { if(GOB_JSON_LOG_LEVEL >= 5) { GOB_JSON_LOG("[V] "#format, ##__VA_ARGS__); } } while(0)

# endif

#else

# define GOB_JSON_LOGE(format, ...)  /**/
# define GOB_JSON_LOGW(format, ...)  /**/
# define GOB_JSON_LOGI(format, ...)  /**/
# define GOB_JSON_LOGD(format, ...)  /**/
# define GOB_JSON_LOGV(format, ...)  /**/

#endif

//
#endif
