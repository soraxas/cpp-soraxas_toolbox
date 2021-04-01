/*
 * This should be compiled using C++17 because of the use of inline variable
 */

#ifndef SXS_GLOBALS
#define SXS_GLOBALS

#include "main.h"
#include <variant>

namespace sxs {
namespace globals {

extern Stats stats;
extern std::map<std::string, std::shared_ptr<void>> storage;
// inline static Stats stats = Stats();
// inline static std::map<std::string, std::shared_ptr<void>> storage;

#ifdef SXS_GLOBALS_SETUP_STORAGE
sxs::Stats stats = sxs::Stats();
std::map<std::string, std::shared_ptr<void>> storage;
#endif

} // namespace globals
} // namespace sxs

namespace sxs {
namespace globals {

template <typename T> std::shared_ptr<T> create(const std::string &key, T arg) {
  // create a shared ptr object in storage, then return the ptr
  std::shared_ptr<T> ptr = std::make_shared<T>(std::move(arg));
  storage[key] = ptr;
  return ptr;
}

template <typename T> std::shared_ptr<T> get_ptr(const std::string &key) {
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