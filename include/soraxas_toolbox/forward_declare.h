#pragma once

#include <vector>
#include <iostream>

// template for printing vector container
template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v);

