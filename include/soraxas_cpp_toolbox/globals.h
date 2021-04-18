/*
 * This should be compiled using C++17 because of the use of inline variable
 */

#ifndef SXS_GLOBALS
#define SXS_GLOBALS

#include "main.h"
#include <thread>
#include <variant>

namespace sxs {
namespace globals {

namespace {
// static storage of arbitary things
std::map<std::string, std::shared_ptr<void>> &_get_static_storage() {
  static std::map<std::string, std::shared_ptr<void>> storage{};
  return storage;
}

// static storage across thread
static std::mutex &_get_lock() {
  static std::mutex thread_lock;
  return thread_lock;
}

static std::map<std::thread::id, Stats> &_get_stats_container() {
  static std::map<std::thread::id, Stats> stats_container;
  return stats_container;
}

static void _allocate_stats_for_this_thread(std::thread::id thread_id) {
  auto &thread_lock = _get_lock();
  thread_lock.lock();
  _get_stats_container()[thread_id] = Stats{};
  thread_lock.unlock();
}
} // unnamed namespace

static Stats &get_stats() {
  auto &container = _get_stats_container();
  auto thread_id = std::this_thread::get_id();
  auto container_iterator = container.find(thread_id);

  if (container_iterator == container.end()) { /* Not found */
    _allocate_stats_for_this_thread(thread_id);
    container_iterator = container.find(thread_id); // need to search again
    assert(container_iterator != container.end());
  }
  return (*container_iterator).second;
}

static void print_all_stored_stats() {
  std::cout << "========= StoredStats =========" << std::endl;
  for (auto &&item : _get_stats_container()) {
    std::cout << "Thread id: " << item.first << std::endl;
    std::cout << std::string(item.second) << std::endl;
  }
  std::cout << "===============================" << std::endl;
}

} // namespace globals
} // namespace sxs

namespace sxs {
namespace globals {

template <typename T> std::shared_ptr<T> create(const std::string &key, T arg) {
  // create a shared ptr object in storage, then return the ptr
  std::shared_ptr<T> ptr = std::make_shared<T>(std::move(arg));
  _get_static_storage()[key] = ptr;
  return ptr;
}

template <typename T> std::shared_ptr<T> get_ptr(const std::string &key) {
  auto &storage = _get_static_storage();
  auto iter = storage.find(key);
  if (iter == storage.end()) {
    // not found
    throw std::runtime_error("Given key '" + key + "' does not exists.");
  }
  // obtain the stored shared ptr as a specified type
  return std::static_pointer_cast<T>((*iter).second);
}

template <typename T> T &get(const std::string &key) {
  return *get_ptr<T>(key);
}
} // namespace globals

// namespace alias
namespace g = globals;
} // namespace sxs

#endif // SXS_GLOBALS