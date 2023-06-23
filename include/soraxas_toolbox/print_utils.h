#pragma once

#include "forward_declare.h"
#include "print_utils_core.h"
#include "string.h"
#include "string_from_stuff.h"

#include <array>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// template for printing type info
inline std::ostream &operator<<(std::ostream &out, const std::type_info &info)
{
    return out << sxs::string::simplify_type_name(sxs::string::get_type_name(info));
}

inline std::ostream &operator<<(std::ostream &out, const std::type_index &type_index)
{
    return out << sxs::string::simplify_type_name(sxs::string::get_type_name(type_index));
}

///////////////////////////////////
// template for printing pair
template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &out, const typename std::pair<T1, T2> &pair)
{
    return out << "(" << pair.first << "," << pair.second << ")";
}

///////////////////////////////////
// template for printing tuple
namespace sxs
{
    template <class Tuple, std::size_t N>
    void __print_tuple(std::ostream &out, const Tuple &t)
    {
        if constexpr (N > 1)
        {
            __print_tuple<Tuple, N - 1>(out, t);
            out << ", ";
        }
        out << std::get<N - 1>(t);
    }
}  // namespace sxs

template <class... Args>
std::ostream &operator<<(std::ostream &out, const std::tuple<Args...> &t)
{
    using namespace sxs::string;
    out << simplify_type_name(get_type_name(t)) << "(";
    sxs::__print_tuple<decltype(t), sizeof...(Args)>(out, t);
    return out << ")";
}

///////////////////////////////////

//// template for printing any iterable container
// disable this template for std::string
template <
    class IterableContainer, class Begin, class ValueType, typename
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
    ,
    typename
#endif
    >
inline std::ostream &operator<<(std::ostream &stream, const IterableContainer &vect)
{
    using namespace sxs::string;

    const char *dlm = "";
    //    std::string container_type_name =
    //            simplify_type_name(extract_container_name(get_type_name(typeid(IterableContainer))));
    std::string value_type_name = simplify_type_name(get_type_name(typeid(ValueType)));
    if (value_type_name != "double" && value_type_name != "int" && value_type_name != "float")
    {
        stream << simplify_type_name(typeid(IterableContainer));
    }
    stream << "[";
    for (const auto &i : vect)
    {
        stream << dlm << i;
        dlm = ", ";
    }
    stream << "]";

    return stream;
}

#ifdef SXS_RUN_TESTS
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */

TEST_CASE("[sxs] Test printing guard")
{
    using namespace sxs;

    SXSPrintOutputStreamGuard guard;
    std::ostringstream &oss = guard.get_ostream();
    print("hey");
    CHECK(oss.str() == "hey");
    {
        SXSPrintOutputStreamGuard guard2;
        std::ostringstream &oss2 = guard2.get_ostream();
        print("inner");
        CHECK(oss.str() == "hey");
        CHECK(oss2.str() == "inner");
    }
    print(" you");
    CHECK(oss.str() == "hey you");
}

TEST_CASE("[sxs] Test printing to ostream")
{
    using namespace sxs;

    SXSPrintOutputStreamGuard guard;
    std::ostringstream &oss = guard.get_ostream();

    std::ostream *aaa = &oss;
    SUBCASE("[sxs] no newline")
    {
        print("Hello world");
        CHECK(oss.str() == "Hello world");
    }

    SUBCASE("[sxs] newline")
    {
        println("Hello world");
        CHECK(oss.str() == "Hello world\n");
    }

    SUBCASE("[sxs] spaced print")
    {
        println_spaced("Hello world", 3, "/", 10);
        CHECK(oss.str() == "Hello world 3 / 10\n");
    }

    SUBCASE("[sxs] semantic printing")
    {
        SUBCASE("")
        {
            print(true);
            CHECK(oss.str() == "true");
        }
        SUBCASE("")
        {
            print(false);
            CHECK(oss.str() == "false");
        }
        SUBCASE("")
        {
            print(1.5);
            CHECK(oss.str() == "1.5");
        }
        SUBCASE("")
        {
            print(1.0);
            CHECK(oss.str() == "1");
        }
        SUBCASE("")
        {
            print(5.91856f);
            CHECK(oss.str() == "5.91856");
        }
    }

    SUBCASE("[sxs] container printing")
    {
        SUBCASE("trivial container")
        {
            std::vector<int> vec = {1, 6, 8};
            print(vec);
            CHECK(oss.str() == "[1, 6, 8]");
        }
        SUBCASE("trivial container")
        {
            std::vector<double> vec = {1.5, 6.9, 8.85};
            print(vec);
            CHECK(oss.str() == "[1.5, 6.9, 8.85]");
        }
        SUBCASE("non-trivial container")
        {
            std::vector<std::pair<double, int>> vec = {{1.5, 8}, {6.9, 98}, {8.85, -84}};
            print(vec);
            CHECK(oss.str() == "vec<p<double,int>>[(1.5,8), (6.9,98), (8.85,-84)]");
        }
    }

    SUBCASE("[sxs] print type")
    {
        SUBCASE("")
        {
            print(typeid(int));
            CHECK(oss.str() == "int");
        }
        SUBCASE("")
        {
            print(typeid(double));
            CHECK(oss.str() == "double");
        }
        SUBCASE("")
        {
            print(typeid(float));
            CHECK(oss.str() == "float");
        }
        SUBCASE("")
        {
            print(typeid(bool));
            CHECK(oss.str() == "bool");
        }
        SUBCASE("")
        {
            print(typeid(std::pair<int, double>));
            CHECK(oss.str() == "p<int,double>");
        }
        SUBCASE("")
        {
            print(typeid(std::string));
            CHECK(oss.str() == "str");
        }
    }

    SUBCASE("[sxs] print iterable")
    {
        SUBCASE("")
        {
            std::vector<double> container{1, 2, 5};
            print(container);
            CHECK(oss.str() == "[1, 2, 5]");
        }
    }
}

#endif  // SXS_RUN_TESTS
