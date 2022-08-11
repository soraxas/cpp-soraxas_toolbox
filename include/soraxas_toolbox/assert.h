#pragma once

// https://stackoverflow.com/questions/3767869/adding-message-to-assert
#ifndef ASSERT
#ifdef NDEBUG
#define ASSERT(...) 0
#else
// https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
//  The multiple macros that you would need anyway [as per: Crazy Eddie]
#define ASSERT_0() 0
#define ASSERT_1(condition) ASSERT_2(condition, "")
#define ASSERT_2(condition, message)                                                               \
    (!(condition)) ? (std::cerr << "Assertion failed: (" << #condition << "), "                    \
                                << "function " << __FUNCTION__ << ", file " << __FILE__            \
                                << ", line " << __LINE__ << "." << std::endl                       \
                                << message << std::endl,                                           \
                      abort(), 0) :                                                                \
                     1
// The interim macro that simply strips the excess and ends up with the required
// macro
#define ASSERT_X(x, A, B, FUNC, ...) FUNC

// The macro that the programmer uses
#define ASSERT(...)                                                                                \
    ASSERT_X(, ##__VA_ARGS__, ASSERT_2(__VA_ARGS__), ASSERT_1(__VA_ARGS__), ASSERT_0(__VA_ARGS__))
#endif
#endif
