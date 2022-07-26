#pragma once

#include <iostream>
#include <vector>

// template for printing vector container
template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v);
