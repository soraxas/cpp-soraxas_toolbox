//
// Created by tin on 2/08/22.
//

#ifndef SXS_COMPILE_TIME_STRING_H
#define SXS_COMPILE_TIME_STRING_H

// adopted from
// https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c/15863804#15863804
namespace sxs
{
    template <char... letters>
    struct string_t
    {
        static char const *c_str()
        {
            static constexpr char string[] = {letters..., '\0'};
            return string;
        }
    };

    template <typename... Args>
    struct is_compile_time_string
    {
        enum
        {
            value = false
        };
    };

    template <char... letters>
    struct is_compile_time_string<sxs::string_t<letters...>>
    {
        enum
        {
            value = true
        };
    };
}  // namespace sxs

#define MACRO_GET_1(str, i) (sizeof(str) > (i) ? str[(i)] : 0)

#define MACRO_GET_4(str, i)                                                                        \
    MACRO_GET_1(str, i + 0), MACRO_GET_1(str, i + 1), MACRO_GET_1(str, i + 2),                     \
        MACRO_GET_1(str, i + 3)

#define MACRO_GET_16(str, i)                                                                       \
    MACRO_GET_4(str, i + 0), MACRO_GET_4(str, i + 4), MACRO_GET_4(str, i + 8),                     \
        MACRO_GET_4(str, i + 12)

#define MACRO_GET_64(str, i)                                                                       \
    MACRO_GET_16(str, i + 0), MACRO_GET_16(str, i + 16), MACRO_GET_16(str, i + 32),                \
        MACRO_GET_16(str, i + 48)

// CT_STR means Compile-Time_String
#define CT_STR(str) sxs::string_t<MACRO_GET_64(str, 0), 0>  // guard for longer strings

/**
 * Test with this
 *
 * template<typename Name>
 * void test_print() {
 * //String as template parameter
 * std::cout << Name::c_str();
 * }
 *
 * test_print<CT_STR("Hello World!")>();
 *
 */

#endif  // SXS_COMPILE_TIME_STRING_H
