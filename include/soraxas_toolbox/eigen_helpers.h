/*
 * MIT License
 *
 * Copyright (c) 2019-2023 Tin Yiu Lai (@soraxas)
 *
 * This file is part of the project soraxas_toolbox, a collections of utilities
 * for developing c++ applications.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <Eigen/Dense>

template <typename datatype, int dimension>
void eigen_unpack_into_separate_dim(
    const std::vector<Eigen::Matrix<datatype, dimension, 1>> &vec,
    std::array<std::vector<datatype> *, dimension> vec_to_store_unpacked
)
{
    /*
     * Given a std::vector<ArrayXd>, split it into X many std::vector<double>
     *
     * Usage:
     *   std::vector<Eigen::Vector3d> input = {
     *     {1, 1.1, 1.2}, {2, 2.1, 2.2}, {3, 3.1, 3.2}};
     *   std::vector<double> x, y, z;
     *   eigen_unpack_into_separate_dim(input, {&x, &y, &z});
     */
    auto vec_as_matrix =
        Eigen::Map<const Eigen::Array<datatype, Eigen::Dynamic, dimension, Eigen::RowMajor>>(
            vec[0].data(), vec.size(), dimension
        );
    for (int i = 0; i < dimension; ++i)
    {
        vec_to_store_unpacked[i]->resize(vec.size());
        // The following block first maps our output vector to an eigen array,
        // then assign it with a mapped eigen array of our input with a specific
        // dimension. It needs to be RowMajor because it's a std::vector of
        // Vector
        Eigen::Array<datatype, 1, Eigen::Dynamic>::Map(
            vec_to_store_unpacked[i]->data(), 1, vec.size()
        ) = vec_as_matrix.col(i);
    }
}

// std::vector<Eigen::Vector2d> eigen_convert_to(const Eigen::ArrayX2d& arr) {
template <typename datatype, int col>
auto eigen_convert_to(const Eigen::Array<datatype, Eigen::Dynamic, col> &arr)
{
    /*
     * Given ArrayX2d, unpack it into a vector of vector2d
     * e.g.
     * Eigen::ArrayX2d arr = ...;
     * std::vector<Eigen::Vector2d> vec = eigen_convert_to(arr);
     */
    std::vector<Eigen::Matrix<datatype, col, 1>> vec(arr.rows());
    Eigen::Array<datatype, Eigen::Dynamic, col>::Map(vec.data()->data(), arr.rows(), col) = arr;
    return vec;
}

template <typename datatype, int col>
// auto eigen_map_to(std::vector<Eigen::Vector2d>& vec) {
auto eigen_map_to(std::vector<Eigen::Matrix<datatype, col, 1>> &vec)
{
    /*
     * Given std::vector<vectorXd>, map it into an Eigen::ArrayXd (point to same
     * memory)
     * e.g.
     * std::vector<Eigen::Vector2d> vec = ...;
     * Eigen::Map<Eigen::ArrayX2d> mapped = eigen_map_to(vec);
     */
    return Eigen::Map<Eigen::Array<datatype, Eigen::Dynamic, col>>(vec[0].data(), vec.size(), col);
}

//////////////////////////////////////////////////

template <typename T>
const auto vec_as_eigen(const std::vector<T> &v)
{
    return Eigen::Map<const Eigen::Matrix<T, Eigen::Dynamic, 1>>(v.data(), v.size());
}

template <int Num>
auto eigen_as_vec(Eigen::Matrix<double, Num, 1> &mat)
{
    return std::vector<double>(mat.data(), mat.data() + mat.size());
}

template <int Num>
auto eigen_as_vec(const Eigen::Matrix<double, Num, 1> &mat)
{
    return std::vector<double>(mat.data(), mat.data() + mat.size());
}

/**
 * Maps the memory of a std::vector<std::array<double,3>> to a writable Eigen type using Eigen::Map.
 *
 * This can convert a given MatrixXd (e.g. 100 rows x 3 cols)
 * into a std::vector<Vector3d> of 100 corresponding rows.
 *
 */
template <int Num, typename DataType>
auto map_eigen_mat_to_std_vector(const Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic> &mat)
{
    using MatXXdR = Eigen::Matrix<DataType, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

    if (mat.rows() < 1)
    {
        throw std::runtime_error("empty matrix given");
    }
    std::vector<Eigen::Matrix<DataType, Num, 1>> raw_data;

    // allocate memory for Nx3 entries
    raw_data.resize(mat.rows());

    assert(mat.cols() == Num);
    if (mat.cols() != Num)
    {
        throw std::runtime_error("Mismatch of matix column number and the requested vector "
                                 "dimension");
    }

    // Copy mat to raw_data:

    MatXXdR::Map(raw_data[0].data(), mat.rows(), mat.cols()) = mat;

    return raw_data;
}

// template <typename T>
// class PreAllocator {
// private:
//  T* memory_ptr;
//  std::size_t memory_size;
//
// public:
//  typedef std::size_t size_type;
//  typedef T* pointer;
//  typedef T value_type;
//
//  PreAllocator(T* memory_ptr, std::size_t memory_size)
//      : memory_ptr(memory_ptr), memory_size(memory_size) {}
//
//  PreAllocator(const PreAllocator& other) throw()
//      : memory_ptr(other.memory_ptr), memory_size(other.memory_size){};
//
//  template <typename U>
//  PreAllocator(const PreAllocator<U>& other) throw()
//      : memory_ptr(other.memory_ptr), memory_size(other.memory_size){};
//
//  template <typename U>
//  PreAllocator& operator=(const PreAllocator<U>& other) {
//    return *this;
//  }
//  PreAllocator<T>& operator=(const PreAllocator& other) { return *this; }
//  ~PreAllocator() {}
//
//  pointer allocate(size_type n, const void* hint = 0) { return memory_ptr; }
//  void deallocate(T* ptr, size_type n) {}
//
//  size_type max_size() const { return memory_size; }
//};
