#pragma once

#include <cxxabi.h>
#include <string>
#include <iterator> // needed for std::ostram_iterator
#include <cmath> // for std::sqrt
#include <unistd.h> // for getting home dir
#include <pwd.h> // for getting home dir

#include "soraxas_toolbox/future.h"

#include "forward_declare.h"

#define SXS_USE_PPRINT

// https://stackoverflow.com/questions/3767869/adding-message-to-assert
#ifdef NDEBUG
#define ASSERT(...) 0
#else
//https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
// The multiple macros that you would need anyway [as per: Crazy Eddie]
#define ASSERT_0()         0
#define ASSERT_1(condition)         ASSERT_2(condition, "")
#define ASSERT_2(condition, message)\
   (!(condition)) ?\
      (std::cerr << "Assertion failed: (" << #condition << "), "\
      << "function " << __FUNCTION__\
      << ", file " << __FILE__\
      << ", line " << __LINE__ << "."\
      << std::endl << message << std::endl, abort(), 0) : 1
// The interim macro that simply strips the excess and ends up with the required macro
#define ASSERT_X(x,A,B,FUNC, ...)  FUNC

// The macro that the programmer uses
#define ASSERT(...)                    ASSERT_X(,##__VA_ARGS__,\
                                          ASSERT_2(__VA_ARGS__),\
                                          ASSERT_1(__VA_ARGS__),\
                                          ASSERT_0(__VA_ARGS__)\
                                         )
#endif

// template for printing vector container
template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
  int status;
  char *demangled = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status);
  std::string demangled_str(demangled);
  if (demangled_str != "double" && demangled_str != "int" &&
      demangled_str != "float") {
    out << "<" << demangled_str << ">";
  }
  out << "[";
  bool first = true;
  for (auto &&item : v) {
    if (!first)
      out << ", ";
    else
      first = false;
    out << item;
  }
  out << "]";
  if (status)
    free(demangled);
  return out;
}

////////////////////////////////////////////////////////////////////////
namespace sxs {
namespace vec {
// extend a vector with a vector
template <typename T>
inline void extend(std::vector<T> &container, const std::vector<T> &v2) {

  // container.reserve(container.size() + distance(v2.begin(), v2.end()));  //
  // more generic
  container.reserve(container.size() + v2.size());
  container.insert(container.end(), v2.begin(), v2.end());
}

template <typename T> inline T sum(const std::vector<T> &container) {
  assert(container.size() > 0);
  T total = container[0]; // should be copy construct
  for (size_t i = 1; i < container.size(); ++i)
    total += container[i];
  return total;
}

template <typename T> inline T max(const std::vector<T> &container) {
  assert(container.size() > 0);
  T max = container[0]; // should be copy construct
  for (size_t i = 1; i < container.size(); ++i)
    max = std::max(max, container[i]);
  return max;
}

template <typename T> inline T min(const std::vector<T> &container) {
  assert(container.size() > 0);
  T min = container[0]; // should be copy construct
  for (size_t i = 1; i < container.size(); ++i)
    min = std::min(min, container[i]);
  return min;
}

} // namespace vec
} // namespace sxs

#include <algorithm>
#include <vector>

template <typename T>
inline long long int indexOf(const std::vector<T> &vector, const T &data,
                             bool throw_exception = false) {

  auto find_result = std::find(vector.begin(), vector.end(), data);
  if (find_result != vector.end()) {
    return std::distance(vector.begin(), find_result);
  }
  if (throw_exception)
    throw std::runtime_error("Unable to find target from the given vector");
  return -1;
}

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <numeric>

namespace sxs {
inline double compute_sum(std::vector<double> nums) {
  return std::accumulate(nums.begin(), nums.end(), 0.);
}

inline std::pair<double, double>
compute_mean_and_stdev(std::vector<double> nums) {
  double sum = compute_sum(nums);
  double mean = sum / nums.size();

  std::vector<double> diff(nums.size());
  std::transform(nums.begin(), nums.end(), diff.begin(),
                 [mean](double x) { return x - mean; });
  double sq_sum =
      std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  double stdev = std::sqrt(sq_sum / nums.size());
  return {mean, stdev};
}

////////////////////////////////////////////////////////////

inline std::pair<double, std::string>
_get_time_factor_and_unit(double elapsed, bool fix_width = false) {
  double factor;
  std::string unit;
  if (elapsed < 1e-6) {
    factor = 1e9;
    unit = "ns";
  } else if (elapsed < 1e-3) {
    factor = 1e6;
    unit = "µs";
  } else if (elapsed < 1) {
    factor = 1e3;
    unit = "ms";
  } else {
    factor = 1;
    unit = "s";
    if (fix_width)
      unit += " ";
  }
  return {factor, unit};
}

#define _FIX_WIDTH_DECIMAL(precision) /* +1 is for the decimal point */        \
  std::setprecision(precision) << std::left << std::setw(precision + 1)

inline std::string format_time2readable(double elapsed, int precision = 3) {
  auto factor_and_unit = _get_time_factor_and_unit(elapsed, true);
  std::stringstream ss;
  ss << _FIX_WIDTH_DECIMAL(precision) << (elapsed * factor_and_unit.first)
     << factor_and_unit.second;
  return ss.str();
}

inline std::string format_time2readable(std::vector<double> all_elapsed,
                                        int precision = 3) {
  auto mean_stdev = compute_mean_and_stdev(all_elapsed);
  // use mean to get factor and units
  auto factor_and_unit = _get_time_factor_and_unit(mean_stdev.first, true);
  std::stringstream ss;
  ss << _FIX_WIDTH_DECIMAL(precision)
     << (mean_stdev.first * factor_and_unit.first) << "±"
     << _FIX_WIDTH_DECIMAL(precision)
     << (mean_stdev.second * factor_and_unit.first) << factor_and_unit.second;
  return ss.str();
}
} // namespace sxs

#include "external/ordered-map/ordered_map.h"
#include <iostream>

namespace sxs {
class Timer {
public:
  Timer(const std::string &name = "", bool print_starter = false,
        bool auto_print = true)
      : beg_(clock_::now()), name(name), m_autoprint(auto_print), m_counts(0),
        m_finished(false), m_print_starter(print_starter) {
    if (m_print_starter)
      std::cout << "[" << name << "] (computing...)" << std::flush;
  }
  ~Timer() { finish(); }

  void finish() {
    if (m_finished)
      return;
    m_finished = true;
    if (m_autoprint) {
      if (m_print_starter)
        std::cout << '\r'; // erase line
      if (!stamped.empty())
        print_stamped_stats();
      else
        // run the current class' string operator
        std::cout << operator std::string() << std::endl;
    }
  }

  void reset() { beg_ = clock_::now(); }
  double elapsed() const {
    return std::chrono::duration_cast<second_>(clock_::now() - beg_).count();
  }

  void stamp(std::string &&stamp_string) {
    if (!_last_stamped_string.empty()) {
      //      std::stringstream ss;
      //      ss << _last_stamped_string << " -> " << stamp_string;
      stamped[{_last_stamped_string, stamp_string}].push_back(
          std::chrono::duration_cast<second_>(clock_::now() -
                                              _last_stamped_clock)
              .count());
    }
    _last_stamped_clock = clock_::now();
    _last_stamped_string = std::move(stamp_string);
  }

  void print_stamped_stats() {
    using sxs::format_time2readable;
    size_t string_1_max_len = 0;
    size_t string_2_max_len = 0;
    size_t stats_size_max_len = 0;
    double total_time_spent = 0;
    for (auto &&item : stamped) {
      string_1_max_len = std::max(string_1_max_len, item.first.first.length());
      string_2_max_len = std::max(string_2_max_len, item.first.second.length());
      stats_size_max_len = std::max(
          stats_size_max_len, std::to_string(item.second.size()).length());
      total_time_spent += sxs::compute_sum(item.second);
    }
    std::cout << "========== stamped result ==========" << std::endl;
    for (auto &&item : stamped) {
      double max = item.second[0];
      double min = item.second[0];
      std::for_each(item.second.begin(), item.second.end(),
                    [&max, &min](double val) {
                      max = std::max(val, max);
                      min = std::min(val, min);
                    });
      double sum = sxs::compute_sum(item.second);
      std::cout << std::left                                        //
                << std::setw(string_1_max_len) << item.first.first  // from
                << " -> "                                           //
                << std::setw(string_2_max_len) << item.first.second // to
                << ": " << format_time2readable(item.second) // mean, stdev
                << " (" << format_time2readable(min) << "~"  // min
                << format_time2readable(max) << ")"          // max
                << " [Σ^" << std::setw(stats_size_max_len)
                << item.second.size() // number of collected stats size
                << "=" << format_time2readable(sum) << "|" // sum
                << _FIX_WIDTH_DECIMAL(3) << (sum / total_time_spent * 100)
                << "%]" << std::endl; // percentage of time spent
    }
    std::cout << "====================================" << std::endl;
  }

  operator std::string() const {
    std::stringstream ss;
    double _elapsed = elapsed();
    ss << "[" << name << "] elapsed: " << sxs::format_time2readable(_elapsed);
    if (m_counts > 0) {
      ss << " avg: ";
      ss << sxs::format_time2readable(_elapsed / m_counts);
      ss << "/it";
      ss << " [out of " << m_counts << "]";
    }
    return ss.str();
  }

  friend std::ostream &operator<<(std::ostream &_stream, Timer const &t) {
    _stream << std::string(t); // .to_string();
    return _stream;
  }

  template <class... T> static Timer auto_print(T... args) {
    Timer timer = Timer(args...);
    timer.set_autoprint();
    return timer;
  }

  void add_count(int counts) { m_counts += counts; }

  void set_autoprint() { m_autoprint = true; }

private:
  bool m_finished;
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1>> second_;

  struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const {
      return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
  };

  tsl::ordered_map<std::pair<std::string, std::string>, std::vector<double>,
                   pair_hash>
      stamped;
  std::string _last_stamped_string;
  std::chrono::time_point<clock_> _last_stamped_clock;
  std::chrono::time_point<clock_> beg_;
  std::string name;
  bool m_autoprint;
  int m_counts;
  bool m_print_starter;
};
} // namespace sxs

namespace sxs {

// variadic print function
template <typename T1> void print(T1 first) { std::cout << first; }

template <typename T1, typename... T2> void print(T1 first, T2... rest) {
  std::cout << first;
  print(rest...);
}

template <typename T1> void print_spaced(T1 first) { print(first); }
/*
 *  Print a comma separated list of any items, with space in-between
 * */
template <typename T1, typename... T2> void print_spaced(T1 first, T2... rest) {
  print(first, " ");
  print_spaced(rest...);
}

/*
 *  Print an empty line
 * */
template <typename... T> void println() {
  std::cout << std::endl;
}

/*
 *  Print a comma separated list of any items
 * */
template <typename... T> void println(T... rest) {
  print(rest...);
  std::cout << std::endl;
}

/*
 *  Print a comma separated list of any items, with space in-between
 * */
template <typename... T> void println_spaced(T... rest) {
  print_spaced(rest...);
  std::cout << std::endl;
}

// template <class T> void doNotOptimizeAway(T &&datum) {
//  asm volatile("" : "+r"(datum));
//}
template <class T> inline auto doNotOptimizeAway(T const &datum) {
  return reinterpret_cast<char const volatile &>(datum);
}

template <typename Lambda>
void timeit(const Lambda &lambda, const std::string title = "untitled") {
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1>> second_;
  auto f = [&lambda]() {
    std::chrono::time_point<clock_> start = clock_::now();
    doNotOptimizeAway(lambda());
    return std::chrono::duration_cast<second_>(clock_::now() - start).count();
  };

  double _elapsed = 0;
  unsigned long loop_count = 0;
  double fastest, slowest;
  auto print_message = [&loop_count, &title, &_elapsed, &fastest, &slowest]() {
    std::cout << "========================================" << std::endl;
    std::cout << "[" << title << "]: Loop: " << loop_count
              << " | Total time: " << sxs::format_time2readable(_elapsed)
              << " | Avg: "
              << sxs::format_time2readable(_elapsed /
                                           static_cast<double>(loop_count))
              << " (" << sxs::format_time2readable(fastest) << " ~ "
              << sxs::format_time2readable(slowest) << ")" << std::endl;
    std::cout << "========================================" << std::endl;
  };
  // first time
  // run it one time to test how long it takes
  _elapsed += f(), ++loop_count;
  fastest = slowest = _elapsed;

  auto run_until_loop_count = [&loop_count, &f, &_elapsed, &fastest,
                               &slowest](unsigned long N) {
    while (loop_count < N) {
      double time_taken = f();
      ++loop_count;
      _elapsed += time_taken;
      fastest = std::min(fastest, time_taken);
      slowest = std::max(slowest, time_taken);
    }
  };

  if (_elapsed > 60) {
    // too long, stop right away.
    print_message();
    return;
  } else if (_elapsed > 10) {
    // run 5 times
    run_until_loop_count(5);
    print_message();
    return;
  } else if (_elapsed > 1) {
    // run 10 times
    run_until_loop_count(10);
    print_message();
    return;
  } else if (_elapsed > 0.1) {
    // run 20 times
    run_until_loop_count(20);
    print_message();
    return;
  } else if (_elapsed > 0.01) {
    // run 100 times
    run_until_loop_count(100);
    print_message();
    return;
  }

  // micro benchmark:
  run_until_loop_count(100);
  if (_elapsed < 3) {
    run_until_loop_count(1000);
    if (_elapsed < 3) {
      run_until_loop_count(10000);
      if (_elapsed < 3) {
        run_until_loop_count(100000);
        if (_elapsed < 3) {
          run_until_loop_count(1000000);
        }
      }
    }
  }
  print_message();
}


inline std::string get_home_dir() {
  /*
   * Return the current home directory
   */
  const char *homedir;
  
  // Check $HOME environment variable first before retrieving user's homedir
  if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
  }

  return homedir;
}


/*
 * Auto string stream for format string and able to cast to string
 */
class StringFormatter {
public:

  StringFormatter() {
  }

  StringFormatter(const StringFormatter& other) {
    ss << std::string(other);
  }

  template <typename T>
  StringFormatter &operator<<(T thing) {
    ss << thing;
    return *this;
  }

  operator std::string() const {
    return ss.str();
  }

  bool print() const {
    sxs::println(operator std::string());
    return false;
  }

  friend std::ostream &operator<<(std::ostream &_stream, StringFormatter const &thing) {
    _stream << std::string(thing); // .to_string();
    return _stream;
  }

private:
  std::stringstream ss;
};

} // end of namespace sxs
