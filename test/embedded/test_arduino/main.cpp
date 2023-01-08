/*
  main for embedeed test (arduino)
*/
#include <gtest/gtest.h>
#include <M5Unified.h>

#if __has_include (<esp_idf_version.h>)
#include <esp_idf_version.h>
#else // esp_idf_version.h has been introduced in Arduino 1.0.5 (ESP-IDF3.3)
#define ESP_IDF_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))
#define ESP_IDF_VERSION ESP_IDF_VERSION_VAL(3,2,0)
#endif

namespace
{
void test()
{
    M5.Lcd.fillRect( 0,0,32,240, RUN_ALL_TESTS() ? TFT_RED : TFT_GREEN );
}
//
}
void setup()
{
    M5.begin();

    printf("ESP-IDF Version %d.%d.%d\n",
           (ESP_IDF_VERSION>>16) & 0xFF, (ESP_IDF_VERSION>>8)&0xFF, ESP_IDF_VERSION & 0xFF);

    printf("CPP %ld\n", __cplusplus);

    printf("char is %s\n", std::is_signed<char>::value ? "signed" : "unsigned");
    printf("sizeof uintmax_t %zu, sizeof uint64_t %zu\n", sizeof(uintmax_t), sizeof(uint64_t));
    printf(" intmax_t: %jd / %jd\n", INTMAX_MIN, INTMAX_MAX);
    printf("uintmax_t: %ju\n", UINTMAX_MAX);
    printf("double:%le / %le\n", -DBL_MAX, DBL_MAX);
  
    ::testing::InitGoogleTest();
    
#ifdef GTEST_FILTER
    ::testing::GTEST_FLAG(filter) = GTEST_FILTER;
#endif
}
    
void loop()
{
    test();
    delay(10 * 1000);
}
