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

#include <sstream>

struct stringbuilder {
  std::stringstream ss;
  template <typename T> stringbuilder &operator<<(const T &data) {
    ss << data;
    return *this;
  }
  operator std::string() { return ss.str(); }
};

#include <chrono>
#include <iostream>

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
    stringbuilder sb;
    double _elapsed = elapsed();
    sb << "[" << name
       << "] elapsed: " << format_time_to_human_readable(_elapsed);
    if (m_counts > 0) {
      sb << " avg: ";
      sb << format_time_to_human_readable(_elapsed / m_counts);
      sb << "/it";
    }
    return std::string(sb);
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

// variadic print function
template <typename T1> void print(T1 first) { std::cout << first; }

template <typename T1, typename... T2> void print(T1 first, T2... rest) {
  std::cout << first;
  print(rest...);
}
