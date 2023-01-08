/*
  main for native test.
*/
#include <gtest/gtest.h>

// C++ version
#if   __cplusplus >= 202002L
# pragma message "C++20 or later"
#elif __cplusplus >= 201703L
# pragma message "C++17 or later"
#elif __cplusplus >= 201402L
# pragma message "C++14 or later"
#elif __cplusplus >= 201103L
# pragma message "C++11 or later"
#else
# error "Need C++11 or later"
# endif
// Compiler
#if defined(__clang__)
# pragma message "Clang"
#elif defined(_MSC_VER)
# pragma message "MSVC"
#elif defined(__BORLANDC__)
# pragma message "BORANDC"
#elif defined(__MINGW32__) || defined(__MINGW64__)
# pragma message "MINGW"
#elif defined(__INTEL_COMPILER)
# pragma message "ICC"
#elif defined(__GNUG__)
# pragma message "GCC"
#else
# pragma message "Unknown compiler"
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result" 

__attribute__((weak)) int main(int argc, char **argv)
{
    printf("CPP %ld\n", __cplusplus);

    printf("char is %s\n", std::is_signed<char>::value ? "signed" : "unsigned");
    printf("sizeof uintmax_t %zu, sizeof uint64_t %zu\n", sizeof(uintmax_t), sizeof(uint64_t));
    printf(" intmax_t: %jd / %jd\n", INTMAX_MIN, INTMAX_MAX);
    printf("uintmax_t: %ju\n", UINTMAX_MAX);
    printf("double:%le / %le\n", -DBL_MAX, DBL_MAX);

    testing::InitGoogleTest(&argc, argv);

#ifdef GTEST_FILTER
    ::testing::GTEST_FLAG(filter) = GTEST_FILTER;
#endif
    
    RUN_ALL_TESTS();
    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}

#pragma GCC diagnostic pop
