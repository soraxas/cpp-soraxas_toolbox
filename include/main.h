#pragma once

#include <iterator> // needed for std::ostram_iterator

// template for printing vector container
template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
  int status;
  char *demangled = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);

  out << "std::vec<" << demangled << '>';
  out << "(" << v.size() << ")[";
  std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
  if (!v.empty())
    out << "\b\b";
  out << "]";
  return out;
}

////////////////////////////////////////////////////////////////////////
// extend a vector with a vector
template <typename T>
void vector_extend_vector(std::vector<T> v1, std::vector<T> v2) {
  
  // v1.reserve(v1.size() + distance(v2.begin(), v2.end()));  // more generic
  v1.reserve(v1.size() + v2.size());
  v1.insert(v1.end(),v2.begin(),v2.end());
}


#include <algorithm>
#include <vector>

template <typename T> 
long long int indexOf(const std::vector<T> &vector , const T &data, bool throw_exception=false){

    auto find_result = std::find(vector.begin(), vector.end(), data);
     if(find_result != vector.end()){
         return std::distance(vector.begin(), find_result);
    }
    if (throw_exception)
      throw std::runtime_error("Unable to find target from the given vector");
    return -1;
}


#include <sstream>

struct stringbuilder
{
  std::stringstream ss;
  template <typename T>
  stringbuilder &operator<<(const T &data)
  {
    ss << data;
    return *this;
  }
  operator std::string() { return ss.str(); }
};


#include <iostream>
#include <chrono>

class Timer
{
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const { 
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};

// variadic print function

template <typename T1>
void print(T1 first) {
  std::cout << first;
}

template <typename T1, typename... T2>
void print(T1 first, T2... rest) {
  std::cout << first;
  print(rest...);
}
