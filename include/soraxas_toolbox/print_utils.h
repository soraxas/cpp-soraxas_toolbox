//
// Created by tin on 22/07/22.
//

#ifndef GLOBALPLANNER_PRINT_UTILS_H
#define GLOBALPLANNER_PRINT_UTILS_H

#include <array>
#include <cxxabi.h>
#include <iostream>
#include <string>
#include <vector>

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

// template for printing array
template <typename T, size_t N>
std::ostream &operator<<(std::ostream &out,
                         const typename std::array<T, N> arr) {
  out << "[";
  for (int i = 0; i < N; ++i) {
    out << arr.at(i);
    if (i < N - 1)
      out << ",";
  }
  out << "]";
  return out;
}

// template for printing pair
template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &out,
                         const typename std::pair<T1, T2> pair) {
  out << "[";
  out << pair.first << "," << pair.second;
  out << "]";
  return out;
}

#endif // GLOBALPLANNER_PRINT_UTILS_H
