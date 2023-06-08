//
// Created by tin on 3/08/22.
//

#ifndef SXS_COMPILE_TIME_DICT_H
#define SXS_COMPILE_TIME_DICT_H

/**
 * USAGE:
 *
 *      using dict = CompileTimeAnyTypeDict<CT_STR("my storage") >;
 *
 *      dict::print_dict();
 *
 *      // the second template arg is the storing type
 *     dict::of< CT_STR("name"), std::string>() = "HAHA this is a stored string";
 *     println(dict::of< CT_STR("name"), std::string>());
 *
 *     dict::of<CT_STR("my stat"), double >() = 1;
 *     dict::of<CT_STR("my stat"), double >() += 25;
 *     println(dict::of<CT_STR("my stat"), double >());
 *
 *     double & lets_keep_track_of_this = dict::of<CT_STR("my stat"), double >();
 *     lets_keep_track_of_this *= -1;
 *     println(dict::of<CT_STR("my stat"), double >());
 *
 *
 *     ================================================================
 *
 *     using numdict = sxs::CompileTimeMappingTypeDict<CT_STR("yes storage")>;
 *
 *     numdict::print_dict();
 *
 *     numdict::of<CT_STR("ok") >() += 1;
 *     numdict::increment<CT_STR("ok")>();
 *     numdict::increment<CT_STR("ok")>();
 *
 *     numdict::print_dict();
 *
 */

#include "compile_time_string.h"
#include "string_from_stuff.h"  // for printing dict
#include "vector_math.h"        // for printing dict

#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

namespace sxs
{

    template <typename Tag, typename DataType, typename... Args>
    struct _BaseRegisterClass
    {
    protected:
        static _BaseRegisterClass instance;

        template <typename F>
        _BaseRegisterClass(const F &callback)
        {
            callback.template __register<DataType, Args...>();
        }

    public:
        static const _BaseRegisterClass &doRegister()
        {
            return instance;
        }
    };

    //    template<typename Tag, typename ...Args>
    //    _BaseRegisterClass<Tag, Args...>
    //            _BaseRegisterClass<Tag, Args...>::instance{CompileTimeAnyTypeDict<Tag>{}};

    /*
     * Forward declare the actual Dict
     */
    template <typename Tag>
    class CompileTimeAnyTypeDict;

    template <typename Tag>
    class CompileTimeMappingTypeDict;

    /**
     * Helper class that register used dict tag and key at compile time.
     * The keys are being insert into the unordered map at static storage
     * allocation time, where the keys are in the map even before their
     * first usage in code (which is pretty cool eh?).
     *
     * see
     * https://stackoverflow.com/questions/28052989/can-i-have-template-instantiation-side-effects
     *
     * @tparam Tag
     * @tparam Key
     */

    //    template<typename ...Args>
    //    _BaseRegisterClass<Args...> _BaseRegisterClass<Args...>::instance;

    template <typename Tag, typename DataType, typename... Args>
    class _AnyTypeRegisterClass
    {
    protected:
        static _AnyTypeRegisterClass instance;

        template <typename F>
        _AnyTypeRegisterClass(const F &callback)
        {
            callback.template __register<DataType, Args...>();
        }

    public:
        static const _AnyTypeRegisterClass &doRegister()
        {
            return instance;
        }
    };

    /**
     * A public facing compile time dict
     *
     * @tparam Tag
     */
    template <typename Tag>
    class CompileTimeAnyTypeDict
    {
    public:
        using tag = Tag;

        template <typename T>
        using StoredValueReturnType = T &;

        using StoredMappingValue = std::pair<std::type_index, void *>;

        template <typename CompileTimeString, typename T>
        static StoredValueReturnType<T> of()
        {
            //            _CompileTimeAnyTypeDict_RegisterClass<Tag,
            //            CompileTimeString>::doRegister();

            _AnyTypeRegisterClass<Tag, T, CompileTimeString>::doRegister();

            static T thing;
            return thing;
        }

        template <typename T>
        static T runtime_retrieve(const std::string &key)
        {
            // DANGEROUS! make sure the type is correct.

            auto iter = mappings().find(key);
            if (iter == mappings().end())
                throw std::runtime_error("Key '" + key + "' does not exists");
            void *function_ptr = iter->second.second;

            // re-interpert the stored opaque pointer back to the function pointer with the
            // requested type
            return reinterpret_cast<StoredValueReturnType<T> (*)()>(function_ptr)();
        }

        static std::unordered_map<std::string, StoredMappingValue> &mappings()
        {
            static std::unordered_map<std::string, StoredMappingValue> _mapping;
            return _mapping;
        }

        static void print_dict()
        {
            std::cout << "|==== Dict: " << tag::c_str() << " ====|" << std::endl;
            for (auto &&key : mappings())
            {
                const auto &data_type = key.second.first;
                std::cout << " - " << key.first << " (" << sxs::string::get_type_name(data_type)
                          << ")";
                if (data_type == std::type_index(typeid(int)))
                {
                    std::cout << ": " << runtime_retrieve<int>(key.first);
                }
                else if (data_type == std::type_index(typeid(double)))
                {
                    std::cout << ": " << runtime_retrieve<double>(key.first);
                }
                else if (data_type == std::type_index(typeid(std::string)))
                {
                    std::cout << ": \"" << runtime_retrieve<std::string>(key.first) << "\"";
                }
                else if (data_type == std::type_index(typeid(float)))
                {
                    std::cout << ": " << runtime_retrieve<float>(key.first);
                }
                std::cout << std::endl;
            }
            std::cout << "|==== ===== ===== ====|" << std::endl;
        }

        template <typename DataType, typename Key>
        static void __register()
        {
            mappings().emplace(
                Key::c_str(),
                std::make_pair(
                    std::type_index(typeid(DataType)), reinterpret_cast<void *>(&of<Key, DataType>)
                )
            );
        }

    private:
        CompileTimeAnyTypeDict() = delete;
    };

    template <typename Tag, typename DataType, typename... Args>
    _AnyTypeRegisterClass<Tag, DataType, Args...>
        _AnyTypeRegisterClass<Tag, DataType, Args...>::instance{CompileTimeAnyTypeDict<Tag>{}};

    // ========================================================
    // ========================================================
    // ========================================================

    /**
     * Helper class that register used dict tag and key at compile time.
     * The keys are being insert into the unordered map at static storage
     * allocation time, where the keys are in the map even before their
     * first usage in code (which is pretty cool eh?).
     *
     * see
     * https://stackoverflow.com/questions/28052989/can-i-have-template-instantiation-side-effects
     *
     * @tparam Tag
     * @tparam Key
     */
    template <typename Tag, typename NumericType, typename Key>
    struct _CompileTimeNumericTypeDict_RegisterClass
    {
        static _CompileTimeNumericTypeDict_RegisterClass instance;

        _CompileTimeNumericTypeDict_RegisterClass()
        {
            CompileTimeMappingTypeDict<Tag>::template get_map<NumericType>()[Key::c_str()] = 0.;
        }

        static const _CompileTimeNumericTypeDict_RegisterClass &doRegister()
        {
            return instance;
        }
    };

    /**
     * A template static storage for each template register class
     *
     * @tparam T
     * @tparam Key
     */
    template <typename T, typename NumericType, typename Key>
    _CompileTimeNumericTypeDict_RegisterClass<T, NumericType, Key>
        _CompileTimeNumericTypeDict_RegisterClass<T, NumericType, Key>::instance;

    /**
     * A public facing compile time dict
     *
     * @tparam Tag
     */
    template <typename Tag>
    class CompileTimeMappingTypeDict
    {
    public:
        using tag = Tag;

        template <typename CompileTimeString, typename NumericType = double>
        static auto &of()
        {
            _CompileTimeNumericTypeDict_RegisterClass<
                Tag, NumericType, CompileTimeString>::doRegister();
            static NumericType &thing = get_map<NumericType>()[CompileTimeString::c_str()];
            return thing;
        }

        template <typename CompileTimeString, typename T = double>
        static auto &increment()
        {
            auto &&thing = of<CompileTimeString, T>();
            thing += 1;
            return thing;
        }

        template <typename NumericType = double>
        static auto &get_map()
        {
            static std::unordered_map<std::string, NumericType> _map;
            get_stored_type_map_key().insert(typeid(NumericType));
            return _map;
        }

        template <typename NumericType = double>
        static auto &get_stored_type_map_key()
        {
            static std::unordered_set<std::type_index> stored_types;
            return stored_types;
        }

        static void print_dict()
        {
            std::cout << "===== NumDict: " << tag::c_str() << " =====" << std::endl;
            std::cout << " >> stored types: ";
            {
                bool first_item = true;
                for (auto &&_typeid : get_stored_type_map_key())
                {
                    if (!first_item)
                        std::cout << ", ";
                    std::cout << sxs::string::simplify_type_name(sxs::string::get_type_name(_typeid)
                    );
                    first_item = false;
                }
                std::cout << std::endl;
            }
            std::cout << " ----- " << std::endl;
            std::cout << " >> storage for double:" << std::endl;
            for (auto &&key : get_map<double>())
            {
                std::cout << "- " << key.first << ": " << key.second << std::endl;
            }
            std::cout << " >> storage for int:" << std::endl;
            for (auto &&key : get_map<int>())
            {
                std::cout << "- " << key.first << ": " << key.second << std::endl;
            }
            std::cout << "===== ===== ===== =====" << std::endl;
        }

    private:
        CompileTimeMappingTypeDict() = delete;
    };
}  // namespace sxs

#endif  // SXS_COMPILE_TIME_DICT_H

#ifdef SXS_RUN_TESTS
/*
 * -------------------------------------------
 * Test cases and general usage for this file:
 * -------------------------------------------
 */

#include "print_utils.h"
#include "string.h"

TEST_CASE("[sxs] compile time dict initialise all occurance of item at the beginning")
{
    using dict = sxs::CompileTimeAnyTypeDict<CT_STR("my storage")>;
    sxs::OutputStreamGuard guard{std::cout};

    std::flush(guard.oss());
    dict::print_dict();

    // the 3 is the future inclusion
    CHECK_EQ(sxs::string::count(guard.oss().str(), "\n"),
             2 + 3);  // 2 lines means that it's empty
    std::flush(guard.oss());

    // afterwards, the item are mutated in logical order
    dict::of<CT_STR("name"), std::string>() = "HAHA string 1";
    CHECK_EQ(dict::of<CT_STR("name"), std::string>(), "HAHA string 1");
    CHECK_EQ(dict::of<CT_STR("name2"), std::string>(), "");
    dict::of<CT_STR("name2"), std::string>() = "2";
    dict::of<CT_STR("name3"), std::string>() = "my string 3";
    CHECK_EQ(dict::of<CT_STR("name2"), std::string>(), "2");
    CHECK_EQ(dict::of<CT_STR("name3"), std::string>(), "my string 3");
}

TEST_CASE("[sxs] dict returns reference")
{
    using dict = sxs::CompileTimeAnyTypeDict<CT_STR("my 2nd storage")>;
    sxs::OutputStreamGuard guard{std::cout};

    dict::of<CT_STR("my stat"), double>() = 1;
    dict::of<CT_STR("my stat"), double>() += 25;
    CHECK_EQ(dict::of<CT_STR("my stat"), double>(), 26);

    double &lets_keep_track_of_this = dict::of<CT_STR("my stat"), double>();
    lets_keep_track_of_this *= -1;

    CHECK_EQ(dict::of<CT_STR("my stat"), double>(), -26);
}

TEST_CASE("[sxs] dict in reference works")
{
    using numdict = sxs::CompileTimeMappingTypeDict<CT_STR("yes storage")>;

    CHECK_EQ(numdict::of<CT_STR("im_double")>(), 0.0);
    CHECK_EQ(numdict::of<CT_STR("im_int"), int>(), 0);

    numdict::of<CT_STR("im_double")>() += 1.8;
    numdict::of<CT_STR("im_int"), int>() += 1.8;
    numdict::of<CT_STR("im_string"), std::string>() = "hihi";

    CHECK_EQ(numdict::of<CT_STR("im_double")>(), 1.8);
    CHECK_EQ(numdict::of<CT_STR("im_int"), int>(), 1);
    CHECK_EQ(numdict::of<CT_STR("im_string"), std::string>(), "hihi");

    numdict::of<CT_STR("ok")>() += 1;
    CHECK_EQ(numdict::of<CT_STR("ok")>(), 1);

    numdict::increment<CT_STR("ok")>();
    CHECK_EQ(numdict::of<CT_STR("ok")>(), 2);

    numdict::increment<CT_STR("ok")>();
    CHECK_EQ(numdict::of<CT_STR("ok")>(), 3);
}

#endif  // SXS_RUN_TESTS
