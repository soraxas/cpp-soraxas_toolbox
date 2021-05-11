/*
 * This should be compiled using C++17 because of the use of inline variable
 */

#ifndef SXS_GLOBALS
#define SXS_GLOBALS

#include "external/pprint.hpp"
#include "main.h"
#include "stats.h"
#include <map>
#include <thread>

#if __cplusplus >= 201703L
#include <any>
namespace sxs {
namespace globals {
using std::any;
using std::any_cast;
} // namespace globals
} // namespace sxs
#else
#include "soraxas_cpp_toolbox/external/any.hpp"
namespace sxs {
namespace globals {
using nonstd::any;
using nonstd::any_cast;
} // namespace globals
} // namespace sxs
#endif

namespace sxs {
namespace globals {
namespace {

// static storage across thread
inline static std::mutex &_get_lock() {
  static std::mutex thread_lock;
  return thread_lock;
}

inline static std::map<std::thread::id, Stats> &_get_stats_container() {
  static std::map<std::thread::id, Stats> stats_container;
  return stats_container;
}

inline static void _allocate_stats_for_this_thread(std::thread::id thread_id) {
  auto &thread_lock = _get_lock();
  thread_lock.lock();
  _get_stats_container()[thread_id].reset(); // init
  thread_lock.unlock();
}
} // unnamed namespace

inline static Stats &get_stats(const std::thread::id &thread_id) {
  auto &container = _get_stats_container();
  auto container_iterator = container.find(thread_id);

  if (container_iterator == container.end()) { /* Not found */
    _allocate_stats_for_this_thread(thread_id);
    container_iterator = container.find(thread_id); // need to search again
    assert(container_iterator != container.end());
  }
  return (*container_iterator).second;
}

inline static Stats &get_stats() {
  return get_stats(std::this_thread::get_id());
}

inline static void print_all_stored_stats() {
  std::cout << "========= StoredStats =========" << std::endl;
  pprint::PrettyPrinter printer;
  for (auto &&item : _get_stats_container()) {
    //    printer("Thread id: ");
    printer.print("Thread id: ", item.first);
    printer.print(std::string(item.second));
  }
  std::cout << "===============================" << std::endl;
}

///////////////////////////////////////////////////////

struct GlobalStorageKeyNotExists : public std::exception {

  GlobalStorageKeyNotExists(const std::string &key)
      : std::exception(), message("Given key '" + key + "' does not exists.") {}

  const char *what() const noexcept override { return message.c_str(); }

private:
  std::string message;
};

///////////////////////////////////////////////////////

namespace storage {

inline std::map<std::string, any> &_get_static_storage() {
  static std::map<std::string, any> container{};
  return container;
}

bool has_key(const std::string &key) {
  auto &storage = _get_static_storage();
  return storage.find(key) != storage.end();
}

template <typename T> inline T get(const std::string &key) {
  auto &storage = _get_static_storage();
  auto iter = storage.find(key);
  if (iter == storage.end()) {
    auto exc = sxs::globals::GlobalStorageKeyNotExists(key);
    std::cout << exc.what() << std::endl;
    throw exc;
  }
  return any_cast<T>(iter->second);
}

template <typename T> inline void store(const std::string &key, T obj) {
  _get_static_storage()[key] = std::move(obj);
}

inline void clear() { _get_static_storage().clear(); }

inline void print_stored_info() {
  std::cout << "========== Static Storage ==========" << std::endl;
  for (auto &&item : _get_static_storage()) {
    std::cout << item.first << ": [type] = " << item.second.type().name()
              << std::endl;
  }
  std::cout << "====================================" << std::endl;
}

}; // namespace storage
} // namespace globals

namespace g = globals; // namespace alias
} // namespace sxs

#endif // SXS_GLOBALS