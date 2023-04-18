#pragma once

#include <type_traits>

#define CREATE_METHOD_EXISTS_CHECKER(__my_function_name)                                           \
    template <typename, typename T>                                                                \
    struct has_method_##__my_function_name                                                         \
    {                                                                                              \
        static_assert(                                                                             \
            std::integral_constant<T, false>::value, "Second template parameter needs to be of "   \
                                                     "function type."                              \
        );                                                                                         \
    };                                                                                             \
                                                                                                   \
    /* specialization that does the checking */                                                    \
    template <typename C, typename Ret, typename... Args>                                          \
    struct has_method_##__my_function_name<C, Ret(Args...)>                                        \
    {                                                                                              \
    private:                                                                                       \
        template <typename T>                                                                      \
        static constexpr auto check(T *) -> typename std::is_same<                                 \
            decltype(std::declval<T>().__my_function_name(std::declval<Args>()...)), Ret>::type;   \
        template <typename>                                                                        \
        static constexpr std::false_type check(...);                                               \
        typedef decltype(check<C>(0)) type;                                                        \
                                                                                                   \
    public:                                                                                        \
        static constexpr bool value = type::value;                                                 \
    };

#define CREATE_FUNCTION_EXISTS_CHECKER(__my_function_name)                                         \
    /* Check where a given function that takes T as argunment is defined */                        \
    template <class, class = void>                                                                 \
    struct has_function_##__my_function_name : std::false_type                                     \
    {                                                                                              \
    };                                                                                             \
                                                                                                   \
    template <typename Arg>                                                                        \
    struct has_function_##__my_function_name<                                                      \
        Arg, decltype(__my_function_name(std::declval<Arg>()), void())>                            \
    {                                                                                              \
    public:                                                                                        \
        static constexpr bool value = std::is_same<int, int>::value;                               \
    };

#ifdef SXS_RUN_TESTS
namespace __sxs_metaprogramming
{
    /*
     * -------------------------------------------
     * Test cases and general usage for this file:
     * -------------------------------------------
     */

    CREATE_METHOD_EXISTS_CHECKER(serialize)

    TEST_CASE("[sxs] meta type checker for method checking")
    {
        struct X
        {
            int serialize(const std::string &)
            {
                return 42;
            }
        };

        struct Y
        {
            int not_serialize(const std::string &)
            {
                return 42;
            }
        };

        struct Z : X
        {
        };

        CHECK(has_method_serialize<Z, int(const std::string &)>::value == 1);
        CHECK(has_method_serialize<Y, int(const std::string &)>::value == 0);
    }

    CREATE_FUNCTION_EXISTS_CHECKER(serialize)

    struct A
    {
    };

    struct B
    {
    };

    int serialize(const B &_)
    {
    }

    std::string to_string(B a)
    {
        return "ok";
    }

    namespace
    {
        using std::to_string;
        CREATE_FUNCTION_EXISTS_CHECKER(to_string)

        TEST_CASE("[sxs] meta type checker for free function")
        {
            CHECK(has_function_serialize<A>::value == false);
            CHECK(has_function_serialize<B>::value == true);

            CHECK(has_function_to_string<int>::value == true);
            CHECK(has_function_to_string<double>::value == true);
            CHECK(has_function_to_string<A>::value == false);
            CHECK(has_function_to_string<B>::value == true);
        }
    }  // namespace
}  // namespace __sxs_metaprogramming

#endif  // SXS_RUN_TESTS