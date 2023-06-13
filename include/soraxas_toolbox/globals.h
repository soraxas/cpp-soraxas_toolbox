/*
 * This should be compiled using C++17 because of the use of inline variable
 */

#ifndef SXS_GLOBALS
#define SXS_GLOBALS

#include "main.h"

// #include "external/pprint.hpp"

#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#if __cplusplus >= 201703L

#include <any>

namespace sxs
{
    namespace globals
    {
        using std::any;
        using std::any_cast;
    }  // namespace globals
}  // namespace sxs
#else
#include "soraxas_toolbox/external/any.hpp"

namespace sxs
{
    namespace globals
    {
        using nonstd::any;
        using nonstd::any_cast;
    }  // namespace globals
}  // namespace sxs
#endif

namespace sxs
{
    namespace globals
    {

        enum class Flags
        {
            unified_storage,
            per_thread,
        };

        namespace
        {

            // static storage across thread
            inline static std::mutex &_get_lock()
            {
                static std::mutex thread_lock;
                return thread_lock;
            }

            template <typename T>
            inline static std::map<std::thread::id, T> &_get_stats_container()
            {
                static std::map<std::thread::id, T> stats_container;
                return stats_container;
            }

            template <typename T>
            inline static void _allocate_stats_for_this_thread(std::thread::id thread_id)
            {
                auto &thread_lock = _get_lock();
                thread_lock.lock();
                _get_stats_container<T>()[thread_id].reset();  // init
                thread_lock.unlock();
            }
        }  // unnamed namespace

        template <typename T>
        inline static T &get_stats(const std::thread::id &thread_id)
        {
            auto &container = _get_stats_container<T>();
            auto container_iterator = container.find(thread_id);

            if (container_iterator == container.end())
            { /* Not found */
                _allocate_stats_for_this_thread<T>(thread_id);
                container_iterator = container.find(thread_id);  // need to search again
                assert(container_iterator != container.end());
            }
            return (*container_iterator).second;
        }

        template <typename T>
        inline static T &get_stats()
        {
            return get_stats<T>(std::this_thread::get_id());
        }

        template <typename T>
        inline static void print_all_stored_stats()
        {
            std::cout << "========= StoredStats =========" << std::endl;

            // #define HAS_PRETTY_PRINTER
#ifdef HAS_PRETTY_PRINTER

            pprint::PrettyPrinter printer;
            for (auto &&item : _get_stats_container())
            {
                //    printer("Thread id: ");
                printer.print("Thread id: ", item.first);
                printer.print(std::string(item.second));
            }
#else
            for (auto &&item : _get_stats_container<T>())
            {
                sxs::println("Thread id: ", item.first);
                sxs::println(std::string(item.second));
            }
#endif

            std::cout << "===============================" << std::endl;
        }

        ///////////////////////////////////////////////////////

        struct GlobalStorageKeyNotExists : public std::exception
        {
            GlobalStorageKeyNotExists(const std::string &key)
              : std::exception(), message("Given key '" + key + "' does not exists.")
            {
            }

            const char *what() const noexcept override
            {
                return message.c_str();
            }

        private:
            std::string message;
        };

        ///////////////////////////////////////////////////////

        namespace storage
        {

            namespace
            {
                // hide this
                template <Flags flag>
                inline auto &_get_static_storage()
                {
                    throw std::runtime_error("Flag is not implemented");
                    // static std::map<std::string, any> container{};
                    // return container;
                }

                template <>
                inline auto &_get_static_storage<Flags::unified_storage>()
                {
                    static std::map<std::string, any> container{};
                    return container;
                }

                template <>
                inline auto &_get_static_storage<Flags::per_thread>()
                {
                    static std::map<std::thread::id, std::map<std::string, any>> container{};
                    auto thread_id = std::this_thread::get_id();
                    sxs::println(thread_id);

                    auto container_iterator = container.find(thread_id);

                    if (container_iterator == container.end())
                    { /* Not found */
                        container[thread_id] = {};
                        // _allocate_stats_for_this_thread(thread_id);
                        container_iterator = container.find(thread_id);  // need to search again
                        // assert(container_iterator != container.end());
                    }
                    return (*container_iterator).second;
                }
            }  // namespace

            template <Flags flag = Flags::unified_storage>
            inline bool has_key(const std::string &key)
            {
                auto &storage = _get_static_storage<flag>();
                return storage.find(key) != storage.end();
            }

            template <Flags flag = Flags::unified_storage>
            inline void print_stored_info()
            {
                sxs::println("========== Static Storage ==========");
                for (auto &&item : _get_static_storage<flag>())
                {
                    sxs::println(item.first, ": [type] = ", item.second.type().name());
                }
                sxs::println("====================================");
            }

            template <typename T, Flags flag = Flags::unified_storage>
            inline T &get(const std::string &key)
            {
                auto &storage = _get_static_storage<flag>();
                auto iter = storage.find(key);
                if (iter == storage.end())
                {
                    auto exc = sxs::globals::GlobalStorageKeyNotExists(key);
                    std::cerr << exc.what() << std::endl;
                    print_stored_info<flag>();
                    throw exc;
                }
                return any_cast<T &>(iter->second);
            }

            template <typename T, Flags flag = Flags::unified_storage>
            inline void store(const std::string &key, T &&obj)
            {
                _get_static_storage<flag>()[key] = std::move(obj);
            }

            template <typename T, Flags flag = Flags::unified_storage, typename... Args>
            inline void initialise_if_not_exists(const std::string &key, Args... args)
            {
                if (!has_key<flag>(key))
                {
                    store<T, flag>(key, T{args...});
                }
            }

            template <typename T, Flags flag = Flags::unified_storage, typename... Args>
            inline auto &get_or_initialise(const std::string &key, Args... args)
            {
                initialise_if_not_exists<T, flag>("timer", std::forward<Args>(args)...);
                return get<T, flag>(key);
            }

            template <Flags flag = Flags::unified_storage>
            inline void clear()
            {
                _get_static_storage<flag>().clear();
            }

        };  // namespace storage
    }       // namespace globals

    namespace g = globals;            // namespace alias
    namespace gs = globals::storage;  // namespace alias
}  // namespace sxs

#endif  // SXS_GLOBALS
