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
 *     using numdict = sxs::CompileTimeMappingTypeDict<CT_STR("yes storage");
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

#include <iostream>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

namespace sxs
{

    template <typename Tag, typename... Args>
    struct _BaseRegisterClass
    {
    protected:
        static _BaseRegisterClass instance;

        template <typename F>
        _BaseRegisterClass(const F &callback)
        {
            callback.template __register<Args...>();
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

    template <typename Tag, typename... Args>
    class _AnyTypeRegisterClass
    {
    protected:
        static _AnyTypeRegisterClass instance;

        template <typename F>
        _AnyTypeRegisterClass(const F &callback)
        {
            callback.template __register<Args...>();
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

        template <typename CompileTimeString, typename T>
        static T &of()
        {
            //            _CompileTimeAnyTypeDict_RegisterClass<Tag,
            //            CompileTimeString>::doRegister();

            _AnyTypeRegisterClass<Tag, CompileTimeString>::doRegister();

            static T thing;
            return thing;
        }

        static std::unordered_set<std::string> &keys()
        {
            static std::unordered_set<std::string> _keys;
            return _keys;
        }

        static void print_dict()
        {
            std::cout << "===== Dict: " << tag::c_str() << " =====" << std::endl;
            for (auto &&key : keys())
            {
                std::cout << "- " << key << std::endl;
            }
            std::cout << "===== ===== ===== =====" << std::endl;
        }

        template <typename Key>
        static void __register()
        {
            keys().insert(Key::c_str());
        }

    private:
        CompileTimeAnyTypeDict() = delete;
    };

    template <typename Tag, typename... Args>
    _AnyTypeRegisterClass<Tag, Args...> _AnyTypeRegisterClass<Tag, Args...>::instance{
        CompileTimeAnyTypeDict<Tag>{}};

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
    template <typename Tag, typename Key, typename NumericType = double>
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
    template <typename T, typename Key, typename NumericType>
    _CompileTimeNumericTypeDict_RegisterClass<T, Key, NumericType>
        _CompileTimeNumericTypeDict_RegisterClass<T, Key, NumericType>::instance;

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
                Tag, CompileTimeString, NumericType>::doRegister();
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
            std::cout << " >> stored types:" << std::endl;
            for (auto &&_typeid : get_stored_type_map_key())
            {
                std::cout << "- " << _typeid.name() << std::endl;
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
