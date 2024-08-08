#pragma once

#include <concepts>
#include "vec.hpp"
#include "mat.hpp"

// chaos math
namespace cm {
template<std::floating_point T>
struct Quaternion
{
  constexpr Quaternion() = default;
  constexpr explicit Quaternion(const T vals[4])
  {
    for (uint32_t i = 0; i < 4; i++) values[i] = vals[i];
  }
  constexpr explicit Quaternion(T w, T x, T y, T z) : w(w), x(x), y(y), z(z)
  {}
  constexpr explicit Quaternion(T w, const Vec<T, 3>& xyz) : w(w), x(xyz.x), y(xyz.y), z(xyz.z)
  {}
  constexpr Quaternion(const std::initializer_list<T>& vals)
  {
    assert(vals.size() == 4);
    uint32_t i = 0;
    for (const auto& val : vals)
    {
      values[i++] = val;
    }
  }

  union
  {
    T values[4];
    struct
    {
      T w, x, y, z;
    };
  };
};

template<typename T>
Quaternion<T> angle_axis(T angle, const cm::Vec<T, 3>& axis)
{
  return Quaternion<T>(std::cos(angle * 0.5), axis * std::sin(angle * 0.5));
}

template<typename T1, typename T2>
Quaternion<T1> operator*(const Quaternion<T1>& a, const Quaternion<T2>& b)
{
  Quaternion<T1> result;
  result.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
  result.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
  result.y = a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z;
  result.z = a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x;
  return result;
}

// only the operators that are needed are implemented
// so, not every expected overload might be there already
template<typename T1, typename T2>
Quaternion<T1> operator+(const Quaternion<T1>& a, const Quaternion<T2>& b)
{
  T1 vals[4];
  for (uint32_t i = 0; i < 4; i++) vals[i] = a.values[i] + T1(b.values[i]);
  return Quaternion<T1>(vals);
}

template<typename T1, typename T2>
Quaternion<T1> operator+(const Quaternion<T1>& a, const T2 b)
{
  T1 vals[4];
  for (uint32_t i = 0; i < 4; i++) vals[i] = a.values[i] + T1(b);
  return Quaternion<T1>(vals);
}

template<typename T>
Quaternion<T> operator-(const Quaternion<T>& a, const Quaternion<T>& b)
{
  return operator+(a, -b);
}

template<typename T1, typename T2>
Quaternion<T1> operator-(const Quaternion<T1>& a, const T2 b)
{
  return operator+(a, -b);
}

template<typename T>
Quaternion<T> operator-(const Quaternion<T>& a)
{
  T vals[4];
  for (uint32_t i = 0; i < 4; i++) vals[i] = -a.values[i];
  return Quaternion<T>(vals);
}

template<typename T1, typename T2>
Quaternion<T1> operator/(const Quaternion<T1>& a, const T2 b)
{
  T1 vals[4];
  for (uint32_t i = 0; i < 4; i++) vals[i] = a.values[i] / T1(b);
  return Quaternion<T1>(vals);
}

template<typename T1, typename T2>
Quaternion<T1> operator*(const Quaternion<T1>& a, const T2 b)
{
  T1 vals[4];
  for (uint32_t i = 0; i < 4; i++) vals[i] = a.values[i] * T1(b);
  return Quaternion<T1>(vals);
}

template<typename T1, typename T2>
Quaternion<T1> operator*(const T2 a, const Quaternion<T1>& b)
{
  return operator*(b, a);
}

template<typename T>
Quaternion<T> conjugate(const Quaternion<T>& q)
{
  return Quaternion({q.w, -q.x, -q.y, -q.z});
}

template<typename T>
Quaternion<T> normalize(const Quaternion<T>& q)
{
  T result = 0.0;
  for (uint32_t i = 0; i < 4; i++) result += q.values[i] * q.values[i];
  result = std::sqrt(result);
  return Quaternion<T>(q.w / result, q.x / result, q.y / result, q.z / result);
}

template<typename T>
T dot(const Quaternion<T>& a, const Quaternion<T>& b)
{
  T result = 0.0;
  for (uint32_t i = 0; i < 4; i++) result += a.values[i] * b.values[i];
  return result;
}

// taken from glm library: https://github.com/g-truc/glm
template<typename T>
Quaternion<T> quat_cast(const Mat<T, 3, 3>& m)
{
  T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
  T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
  T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
  T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

  int biggestIndex = 0;
  T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
  if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)
  {
    fourBiggestSquaredMinus1 = fourXSquaredMinus1;
    biggestIndex = 1;
  }
  if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)
  {
    fourBiggestSquaredMinus1 = fourYSquaredMinus1;
    biggestIndex = 2;
  }
  if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)
  {
    fourBiggestSquaredMinus1 = fourZSquaredMinus1;
    biggestIndex = 3;
  }

  T biggestVal = std::sqrt(fourBiggestSquaredMinus1 + static_cast<T>(1)) * static_cast<T>(0.5);
  T mult = static_cast<T>(0.25) / biggestVal;

  switch(biggestIndex)
  {
  case 0:
    return normalize(Quaternion<T>(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult));
  case 1:
    return normalize(Quaternion<T>((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult));
  case 2:
    return normalize(Quaternion<T>((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult));
  case 3:
    return normalize(Quaternion<T>((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal));
  default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
    assert(false);
    return Quaternion<T>(1.0, 0.0, 0.0, 0.0);
  }
}

template<typename T>
inline Quaternion<T> quat_look_at(const cm::Vec<T, 3>& view_dir, const cm::Vec<T, 3>& up = cm::Vec3(0.0, 1.0, 0.0))
{
  return quat_cast<T>(look_at(view_dir, up));
}

template<typename T>
Quaternion<T> slerp(const Quaternion<T>& a, Quaternion<T> b, T weight)
{
  T cos_theta = dot(a, b);

  // ensure shortest path by negating one quat if necessary
  if (cos_theta < 0.0)
  {
    b = -b;
    cos_theta = -cos_theta;
  }

  // lerp for almost identical quats to prevent divison by zero
  if(cos_theta > 0.999)
  {
    return normalize(Quaternion<T>(
      (1 - weight) * a.w + weight * b.w,
      (1 - weight) * a.x + weight * b.x,
      (1 - weight) * a.y + weight * b.y,
      (1 - weight) * a.z + weight * b.z));
  }
  else
  {
    const T angle = acos(cos_theta);
    return (std::sin((1.0 - weight) * angle) * a + sin(weight * angle) * b) / std::sin(angle);
  }
}
using Quatf = Quaternion<float>;
using Quatd = Quaternion<double>;
} // namespace cm
