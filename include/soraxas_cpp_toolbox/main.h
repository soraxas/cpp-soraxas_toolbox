#pragma once

#include <cxxabi.h>
#include <iterator> // needed for std::ostram_iterator

// template for printing vector container
template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
  int status;
  char *demangled = abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status);

  out << "std::vec<" << demangled << ">";
  out << "(" << v.size() << ")[";
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
  if (!v.empty())
    out << "\b\b";
  out << "]";
  if (status)
    free(demangled);
  return out;
}

////////////////////////////////////////////////////////////////////////
// extend a vector with a vector
template <typename T>
void vector_extend_vector(std::vector<T> &container, const std::vector<T> &v2) {

  // container.reserve(container.size() + distance(v2.begin(), v2.end()));  //
  // more generic
  container.reserve(container.size() + v2.size());
  container.insert(container.end(), v2.begin(), v2.end());
}

#include <algorithm>
#include <vector>

template <typename T>
long long int indexOf(const std::vector<T> &vector, const T &data,
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
double compute_sum(std::vector<double> nums) {
  return std::accumulate(nums.begin(), nums.end(), 0.);
}

std::pair<double, double> compute_mean_and_stdev(std::vector<double> nums) {
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

std::pair<double, std::string>
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

std::string format_time2readable(double elapsed, int precision = 3) {
  auto factor_and_unit = _get_time_factor_and_unit(elapsed, true);
  std::stringstream ss;
  ss << _FIX_WIDTH_DECIMAL(precision) << (elapsed * factor_and_unit.first)
     << factor_and_unit.second;
  return ss.str();
}

std::string format_time2readable(std::vector<double> all_elapsed,
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

class Timer {
public:
  Timer(const std::string &name = "", bool print_starter = false)
      : beg_(clock_::now()), name(name), m_autoprint(false), m_counts(0),
        m_print_starter(print_starter) {
    if (m_print_starter)
      std::cout << "[" << name << "] (computing...)" << std::flush;
  }
  ~Timer() {
    if (m_autoprint) {
      if (m_print_starter)
        std::cout << '\r'; // erase line
      if (!stamped.empty())
        print_stamped_stats();
      else
        std::cout << std::string(*this) << std::endl;
    }
  }
  void reset() { beg_ = clock_::now(); }
  double elapsed() const {
    return std::chrono::duration_cast<second_>(clock_::now() - beg_).count();
  }

  void stamp(const std::string &stamp_string) {
    if (!_last_stamped_string.empty()) {
      //      std::stringstream ss;
      //      ss << _last_stamped_string << " -> " << stamp_string;
      stamped[{_last_stamped_string, stamp_string}].push_back(
          std::chrono::duration_cast<second_>(clock_::now() -
                                              _last_stamped_clock)
              .count());
    }
    _last_stamped_clock = clock_::now();
    _last_stamped_string = stamp_string;
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

#if __cplusplus >= 201703L
#include <variant>
namespace sxs {
namespace variant {
using std::get;
using std::variant;
using std::visit;
} // namespace variant
} // namespace sxs
#else
#include "external/variant.hpp"
namespace sxs {
namespace variant {
using mpark::get;
using mpark::variant;
using mpark::visit;
} // namespace variant
} // namespace sxs
#endif

namespace sxs {

using stats_internal_variant = sxs::variant::variant<long, int, double, float>;
using stats_aggregate_internal_variant =
    sxs::variant::variant<std::vector<long>, std::vector<int>,
                          std::vector<double>, std::vector<float>>;

class Stats {
  /* A class that stores stats for performing basic performance test */
public:
  // double is the default type
  template <
      class Numeric = double,
      typename = std::enable_if_t<std::is_arithmetic<Numeric>::value, Numeric>>
  Numeric &of(const std::string &key) {
    auto val_it = data.find(key);
    if (val_it != data.end()) {
      // return stored reference
      return sxs::variant::get<Numeric>(val_it->second);
    }

    // not found. Default to zero.
    data[key] = stats_internal_variant((Numeric)0); // cast to ensure correct
                                                    // type
    return sxs::variant::get<Numeric>(data[key]);
  }

  // return the actual std::variant
  stats_internal_variant &get(const std::string &key) { return data[key]; }

  // nicely format the contained items to the input stream
  template <typename T> void format_item(T &stream) const {
    // this works with anything that accepts << operator (and returns itself)
    bool firstitem = true;
    for (auto &&item : data) {
      if (firstitem)
        firstitem = false;
      else
        stream << ", ";
      stream << item.first << ": ";
      sxs::variant::visit([&stream](const auto &x) { stream << x; },
                          item.second);
    }
  }

  operator std::string() const {
    std::stringstream ss;
    ss << "{";
    format_item(ss);
    ss << "}";
    return ss.str();
  }

  friend std::ostream &operator<<(std::ostream &_stream, Stats const &t) {
    _stream << std::string(t);
    return _stream;
  }

  void reset() { data.clear(); }

  std::map<std::string, stats_internal_variant> data;
};

class StatsAggregate {
  /* Given the Stats object, aggregate results into a std list */
public:
  StatsAggregate() = default;

  StatsAggregate(const Stats &stats) { append(stats); }

  void append(const Stats &stats) {
    for (auto &&item : stats.data) {
      // always push the item as a double (easier...)
      // this will always down-cast int/long/float within Stats to double
      sxs::variant::visit(
          [this, &item](const auto &x) { data[item.first].push_back(x); },
          item.second);
    }
  }

  operator std::string() const {
    bool firstitem = true;
    std::stringstream ss;
    ss << "{";
    for (auto &&item : data) {
      if (firstitem)
        firstitem = false;
      else
        ss << ", ";
      ss << item.first << ":[";
      for (size_t i = 0; i < item.second.size(); ++i) {
        if (i > 0)
          ss << ",";
        ss << item.second[i];
      }
      ss << "]";
    }
    ss << "}";
    return ss.str();
  }

  friend std::ostream &operator<<(std::ostream &_stream,
                                  StatsAggregate const &t) {
    _stream << std::string(t);
    return _stream;
  }

  const std::map<std::string, std::vector<double>> &get() const { return data; }

  std::map<std::string, std::vector<double>> data;
};

// variadic print function
template <typename T1> void print(T1 first) { std::cout << first; }

template <typename T1, typename... T2> void print(T1 first, T2... rest) {
  std::cout << first;
  print(rest...);
}

template <typename... T> void println(T... rest) {
  print(rest...);
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
} // end of namespace sxs
