/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Tin Yiu Lai (@soraxas)
 *
 * This file is part of the project soraxas_toolbox, a collections of utilities
 * for developing c++ applications.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#if defined __has_include
#if __has_include("soraxas_toolbox/external/enum.hpp")
#include "soraxas_toolbox/external/enum.hpp"
// a shortcut to define everything we need for a good enum token
#define SXS_DEFINE_ENUM_AND_TRAITS(Enum, Type, Fields)                                             \
    ENUM_HPP_DECL(Enum, Type, Fields)                                                              \
    ENUM_HPP_REGISTER_TRAITS(Enum)
#define SXS_HAS_ENUM_HPP
#endif
#endif

#include "soraxas_toolbox/metaprogramming.h"

#include <string>

namespace sxs
{
    namespace stats
    {
        using std::to_string;
        CREATE_FUNCTION_EXISTS_CHECKER(to_string)
        CREATE_FUNCTION_EXISTS_CHECKER(enum_hpp_adl_find_registered_traits)

        /*
         * only enable this funciton if token has a defined to_string function.
         */
        template <typename Token>
        constexpr inline auto get_token_name(const Token &token)
        {
#ifdef SXS_HAS_ENUM_HPP
            if constexpr (
                std::is_enum<Token>::value &&
                // make sure it's defined by enum-hpp
                has_function_enum_hpp_adl_find_registered_traits<Token>::value
                )
            {
                auto ret = enum_hpp::to_string_or_empty(token);
                // the enum might failed to extract string out of it.
                return (ret == "") ? to_string(token) : ret;
            }
            else
#endif
            {
                /* if this fails, please provide a custom to_string implementation. */
                return to_string(token);
            }
        }

        // template <typename Token>
        // constexpr inline
        //     typename std::enable_if<true, std::string>::type
        //     get_token_name(const Token &token)
        // {
        //     using std::to_string;

        //     return to_string(token); /* if this fails, please provide a custom to_string
        //     implementation. */
        // }

        // /*
        // * only enable this funciton if token has a defined to_string function.
        // */
        // template <typename Token>
        // constexpr inline
        //     typename std::enable_if<has_function_to_string<Token>::value, std::string_view>::type
        //     get_token_name(const Token &token)
        // {
        //     using std::to_string;

        //     return to_string(token); /* if this fails, please provide a custom to_string
        //     implementation. */
        // }

        inline std::string get_token_name(const std::string &token)
        {
            return token;
        }

    }  // namespace stats
}  // namespace sxs

#ifdef SXS_RUN_TESTS
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */
namespace __sxs_token
{

#ifdef SXS_HAS_ENUM_HPP
    SXS_DEFINE_ENUM_AND_TRAITS(
        my_smart_enum, char,  //
        (null_value)          //
        (val1)                //
        (val2)                //
    )

    class MyClass
    {
    };

    std::string_view to_string(const MyClass &cls)
    {
        return "MyClass!";
    }

    enum MyUnnamedEnum : char
    {
        other_val1,
        other_val2,
    };

    TEST_CASE("[sxs] Check stats token to_string")
    {
        using namespace sxs::stats;
        {
            CHECK(get_token_name(std::string{"my_name"}) == "my_name");

            CHECK(get_token_name(static_cast<uint>(1)) == "1");
            CHECK(get_token_name(static_cast<int>(1)) == "1");
            CHECK(get_token_name(static_cast<int>(185)) == "185");

            CHECK(get_token_name(MyClass{}) == "MyClass!");

            CHECK(get_token_name(my_smart_enum::val1) == "val1");
            CHECK(get_token_name(my_smart_enum::null_value) == "null_value");

            CHECK(get_token_name(MyUnnamedEnum::other_val1) == "0");
            CHECK(get_token_name(MyUnnamedEnum::other_val2) == "1");
        }
    }
#endif  // SXS_HAS_ENUM_HPP

}  // namespace __sxs_token

#endif  // SXS_RUN_TESTS