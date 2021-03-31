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
void vector_extend_vector(std::vector<T> v1, std::vector<T> v2) {

  // v1.reserve(v1.size() + distance(v2.begin(), v2.end()));  // more generic
  v1.reserve(v1.size() + v2.size());
  v1.insert(v1.end(), v2.begin(), v2.end());
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
#include <iostream>
#include <sstream>

std::string format_time_to_human_readable(double elapsed, int precision = 3) {
  std::stringstream ss;
  ss << std::setprecision(precision);
  if (elapsed < 1e-6) {
    ss << (elapsed * 1e9);
    ss << "ns";
  } else if (elapsed < 1e-3) {
    ss << (elapsed * 1e6);
    ss << "µs";
  } else if (elapsed < 1) {
    ss << (elapsed * 1e3);
    ss << "ms";
  } else {
    ss << (elapsed);
    ss << "s";
  }
  return ss.str();
}

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
      std::cout << std::string(*this) << std::endl;
    }
  }
  void reset() { beg_ = clock_::now(); }
  double elapsed() const {
    return std::chrono::duration_cast<second_>(clock_::now() - beg_).count();
  }

  operator std::string() const {
    std::stringstream ss;
    double _elapsed = elapsed();
    ss << "[" << name
       << "] elapsed: " << format_time_to_human_readable(_elapsed);
    if (m_counts > 0) {
      ss << " avg: ";
      ss << format_time_to_human_readable(_elapsed / m_counts);
      ss << "/it";
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
  std::string name;
  bool m_autoprint;
  bool m_print_starter;
  int m_counts;
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1>> second_;
  std::chrono::time_point<clock_> beg_;
};

#include <variant>

namespace sxs {

using stats_internal_variant = std::variant<long, int, double, float>;
using stats_aggregate_internal_variant =
    std::variant<std::vector<long>, std::vector<int>, std::vector<double>,
                 std::vector<float>>;

class Stats {
  /* A class that stores stats for performing basic performance test */
public:
  // double is the default type
  template <class T = double> T &of(const std::string &key) {
    auto val_it = data.find(key);
    if (val_it != data.end()) {
      // return stored reference
      return std::get<T>(val_it->second);
    }

    // not found. Default to zero.
    data[key] = stats_internal_variant((T)0); // cast to ensure correct type
    return std::get<T>(data[key]);
  }

  stats_internal_variant &get(const std::string &key) { return data[key]; }

  template <typename T> void format_item(T &stream) const {
    // this works with anything that accepts << operator (and returns itself)
    bool firstitem = true;
    for (auto &&item : data) {
      if (firstitem)
        firstitem = false;
      else
        stream << ", ";
      stream << item.first << ": ";
      std::visit([&stream](const auto &x) { stream << x; }, item.second);
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
      std::visit(
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
      for (int i = 0; i < item.second.size(); ++i) {
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

  std::map<std::string, std::vector<double>> data;
};

} // end of namespace sxs

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
