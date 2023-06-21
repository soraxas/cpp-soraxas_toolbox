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
        return 3;
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

namespace sxs
{
    namespace metaprogramming
    {
/// Defines a "has_member_member_name" class template
///
/// This template can be used to check if its "T" argument
/// has a data or function member called "member_name"
#define DEFINE_HAS_MEMBER_FUNC_TRAIT(member_name)                                                  \
    template <typename T>                                                                          \
    class has_member_##member_name                                                                 \
    {                                                                                              \
        typedef char yes_type;                                                                     \
        typedef long no_type;                                                                      \
        template <typename U>                                                                      \
        static yes_type test(decltype(&U::member_name));                                           \
        template <typename U>                                                                      \
        static no_type test(...);                                                                  \
                                                                                                   \
    public:                                                                                        \
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);                      \
    };

/// Shorthand for testing if "class_" has a member called "member_name"
///
/// @note "DEFINE_HAS_MEMBER_FUNC_TRAIT(member_name)" must be used
///       before calling "has_member(class_, member_name)"
#define has_member(class_, member_name) has_member_##member_name<class_>::value

/// Defines a "has_member_member_signature_name" class template
///
/// This template can be used to check if its "T" argument
/// has a data or function member called "member_name"
#define DEFINE_HAS_MEMBER_BOOL_TRAIT(member_bool_name)                                             \
    template <typename T>                                                                          \
    class has_member_bool_##member_bool_name                                                       \
    {                                                                                              \
        typedef char yes_type;                                                                     \
        typedef long no_type;                                                                      \
        /* the following will be instantiated iff 'U::member_bool_name' exists and                 \
         * has a value of true*/                                                                   \
        template <typename U, typename = typename std::enable_if<U::member_bool_name>::type>       \
        static yes_type test(decltype(&U::member_bool_name));                                      \
        template <typename U>                                                                      \
        static no_type test(...);                                                                  \
                                                                                                   \
    public:                                                                                        \
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);                      \
    };

    }  // namespace metaprogramming
}  // namespace sxs

#ifdef SXS_RUN_TESTS
namespace __sxs_metaprogramming
{
    /*
     * -------------------------------------------
     * Test cases and general usage for this file:
     * -------------------------------------------
     */

    namespace
    {
        DEFINE_HAS_MEMBER_FUNC_TRAIT(lets_gooo)

        TEST_CASE("[sxs] meta type checker for method checking")
        {
            struct X
            {
                int lets_gooo(const std::string &)
                {
                    return 42;
                }
            };

            struct Y
            {
                int nope_lets_gooo(const std::string &)
                {
                    return 42;
                }
            };

            struct Z : X
            {
            };

            CHECK(has_member_lets_gooo<Z>::value == 1);
            CHECK(has_member_lets_gooo<Y>::value == 0);
        }
    }  // namespace

    namespace
    {

        struct X_has_true
        {
            static constexpr bool do_it = true;

            int lets_gooo(const std::string &)
            {
                return 42;
            }
        };

        struct Y_has_false
        {
            static constexpr bool do_it = false;

            int nope_lets_gooo(const std::string &)
            {
                return 42;
            }
        };

        struct Z_has_nothing
        {
            int nope_lets_gooo(const std::string &)
            {
                return 42;
            }
        };

        DEFINE_HAS_MEMBER_BOOL_TRAIT(do_it)

        TEST_CASE("[sxs] meta type checker for method checking")
        {
            CHECK(has_member_bool_do_it<X_has_true>::value == 1);
            CHECK(has_member_bool_do_it<Y_has_false>::value == 0);
            CHECK(has_member_bool_do_it<Z_has_nothing>::value == 0);
        }
    }  // namespace

}  // namespace __sxs_metaprogramming

#endif  // SXS_RUN_TESTS
