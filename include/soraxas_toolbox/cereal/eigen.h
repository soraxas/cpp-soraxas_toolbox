#pragma once
#include <Eigen/Dense>
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>

#include <fstream>

// container and such
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>

namespace cereal
{

    template <
        class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
        int _MaxCols>
    inline void
    save(Archive &ar, const ::Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> &m)
    {
        int rows = m.rows(), cols = m.cols();
        ar(CEREAL_NVP(rows), CEREAL_NVP(cols));
        const _Scalar *data = m.data();
        for (uint32_t i = 0; i < m.size(); ++i)
        {
            ar(data[i]);
        }
    }

    template <
        class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows,
        int _MaxCols>
    inline void
    load(Archive &ar, ::Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols> &m)
    {
        int rows, cols;
        ar(CEREAL_NVP(rows), CEREAL_NVP(cols));
        if (rows * cols != m.size())
            m.resize(rows, cols);
        _Scalar value;
        _Scalar *data = m.data();
        for (uint32_t i = 0; i < m.size(); ++i)
        {
            ar(value);
            data[i] = value;
        }
    }

    template <class Archive, class _Scalar, int _AmbientDim>
    inline void serialize(Archive &ar, ::Eigen::AlignedBox<_Scalar, _AmbientDim> &bb)
    {
        ar(make_nvp("min", bb.min()), make_nvp("max", bb.max()));
    }

    template <class Archive, class _Scalar, int _AmbientDim, int _Options>
    inline void serialize(Archive &ar, ::Eigen::Hyperplane<_Scalar, _AmbientDim, _Options> &plane)
    {
        ar(make_nvp("coeffs", plane.coeffs()));
    }

    template <class Archive, class _Scalar, int _Options>
    inline void serialize(Archive &ar, ::Eigen::Quaternion<_Scalar, _Options> &quat)
    {
        ar(make_nvp("w", quat.w()), make_nvp("x", quat.x()), make_nvp("y", quat.y()),
           make_nvp("z", quat.z()));
    }

}  // namespace cereal
