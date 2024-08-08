#pragma once
#include <algorithm>
#include <cstdint>
#include <cassert>
#include <ostream>
#include "vec.hpp"
#include "vec3.hpp"

// chaos math
namespace cm {
// uses column major layout
template<typename T, int N /* width */, int M /* height */>
struct Mat
{
  constexpr Mat() = default;
  constexpr explicit Mat(const T value)
  {
    for (uint32_t i = 0; i < std::min(N, M); i++)
    {
      for (uint32_t j = 0; j < M; j++)
      {
        if (i == j) values[i][j] = value;
        else values[i][j] = 0.0;
      }
    }
  }
  constexpr explicit Mat(const T vals[N][M])
  {
    for (uint32_t i = 0; i < N; i++)
    {
      for (uint32_t j = 0; j < M; j++)
      {
        values[i][j] = vals[i][j];
      }
    }
  }
  constexpr Mat(const std::initializer_list<std::initializer_list<T>>& vals)
  {
    assert(vals.size() == M);
    uint32_t i = 0;
    for (const auto& column : vals)
    {
      assert(column.size() == N);
      values[i++] = Vec<T, M>(column);
    }
  }
  constexpr Mat(const std::initializer_list<cm::Vec<T, N>>& vals)
  {
    assert(vals.size() == M);
    uint32_t i = 0;
    for (const auto& column : vals)
    {
      values[i++] = column;
    }
  }

  template<typename T2>
  Vec<T, M>& operator[](const T2 idx) requires(std::is_integral<T2>::value)
  {
    return values[idx];
  }

  template<typename T2>
  const Vec<T, M>& operator[](const T2 idx) const requires(std::is_integral<T2>::value)
  {
    return values[idx];
  }

  Vec<T, M> values[N];
};

template<typename T, int N, int M>
std::ostream& operator<<(std::ostream& out, const Mat<T, N, M>& a)
{
  out << std::fixed << "(";
  for (uint32_t i = 0; i < N - 1; i++) out << a[i] << "\n";
  out << a[N - 1] << ")";
  return out;
}

// only the operators that are needed are implemented
// so, not every expected overload might be there already
template<typename T1, typename T2, int N, int M>
Mat<T1, N, M> operator/(const Mat<T1, N, M>& a, const T2 b)
{
  Mat<T1, N, M> mat;
  for (uint32_t i = 0; i < N; i++)
  {
    for (uint32_t j = 0; j < N; j++)
    {
      mat[i][j] = a[i][j] / T1(b);
    }
  }
  return mat;
}

template<typename T1, typename T2, int N, int M>
Mat<T1, N, M> operator*(const Mat<T1, N, M>& a, const T2 b)
{
  Mat<T1, N, M> mat;
  for (uint32_t i = 0; i < N; i++)
  {
    for (uint32_t j = 0; j < M; j++)
    {
      mat[i][j] = a[i][j] * T1(b);
    }
  }
  return mat;
}

template<typename T, int N, int M>
Vec<T, M> operator*(const Mat<T, N, M>& a, const Vec<T, N>& b)
{
  Vec<T, M> vec(0.0);
  for (uint32_t i = 0; i < M; i++)
  {
    double sum = 0.0;
    for (uint32_t j = 0; j < N; j++)
    {
      sum += double(a[j][i]) * double(b[j]);
    }
    vec[i] = sum;
  }
  return vec;
}

template<typename T, int N, int M0, int M1>
Mat<T, M1, M0> operator*(const Mat<T, N, M0>& a, const Mat<T, M1, N>& b)
{
  Mat<T, M0, M1> result(0.0);
  for (uint32_t i = 0; i < M0; i++)
  {
    for (uint32_t j = 0; j < M1; j++)
    {
      for (uint32_t k = 0; k < N; k++)
      {
        result[j][i] += a[k][i] * b[j][k];
      }
    }
  }
  return result;
}

inline Mat<float, 3, 3> rotate(Vec3 angles, bool use_radian = false)
{
  if (!use_radian) angles = angles * (M_PI / 180.0);
  return Mat<float, 3, 3>{
    {std::cos(angles.y) * std::cos(angles.z),
      std::cos(angles.y) * std::sin(angles.z),
      -std::sin(angles.y)},

    {(std::sin(angles.x) * std::sin(angles.y) * std::cos(angles.z)) - (std::cos(angles.x) * std::sin(angles.z)),
      (std::sin(angles.x) * std::sin(angles.y) * std::sin(angles.z)) + (std::cos(angles.x) * std::cos(angles.z)),
      std::sin(angles.x) * std::cos(angles.y)},

    {(std::cos(angles.x) * std::sin(angles.y) * std::cos(angles.z)) + (std::sin(angles.x) * std::sin(angles.z)),
      (std::cos(angles.x) * std::sin(angles.y) * std::sin(angles.z)) - (std::sin(angles.x) * std::cos(angles.z)),
      std::cos(angles.x) * std::cos(angles.y)}};
}

inline Mat<float, 3, 3> look_at(const cm::Vec3& view_dir, const cm::Vec3& up = cm::Vec3(0.0, 1.0, 0.0))
{
  Mat<float, 3, 3> result;
  result[2] = -view_dir;
  result[0] = normalize(cross(up, result[2]));
  result[1] = cross(result[2], result[0]);
  return result;
}

using Mat3 = Mat<float, 3, 3>;
using Mat4 = Mat<float, 4, 4>;
} // namespace cm

