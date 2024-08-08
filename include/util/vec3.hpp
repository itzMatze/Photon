#pragma once
#include "vec.hpp"
#include <ostream>
#undef SSE_ENABLED
#ifdef SSE_ENABLED
#include <xmmintrin.h>
#include <smmintrin.h>
#endif

// chaos math
namespace cm {
template<typename T>
struct Vec<T, 3>
{
  constexpr Vec() = default;
  constexpr explicit Vec(const T value) : x(value), y(value), z(value) {}
  constexpr Vec(const T x, const T y, const T z) : x(x), y(y), z(z) {}
  constexpr Vec(const Vec& other) : x(other.x), y(other.y), z(other.z) {}
  constexpr Vec(const T vals[3]) : x(vals[0]), y(vals[1]), z(vals[2]) {}
  constexpr Vec(const std::initializer_list<T>& vals)
  {
    assert(vals.size() == 3);
    uint32_t i = 0;
    for (const auto& val : vals)
    {
      values[i++] = val;
    }
  }
#if defined(SSE_ENABLED)
  constexpr Vec(const __m128& data) : data(data) {}
#endif

  template<typename T2>
  T& operator[](const T2 idx) requires(std::is_integral<T2>::value)
  {
    return values[idx];
  }

  template<typename T2>
  T operator[](const T2 idx) const requires(std::is_integral<T2>::value)
  {
    return values[idx];
  }

  template<typename T2>
  void operator*=(const T2 value)
  {
    *this = *this * value;
  }

  template<typename T2>
  void operator/=(const T2 value)
  {
    *this = *this / value;
  }

  template<typename T2>
  void operator+=(const T2 value)
  {
    *this = *this + value;
  }

  template<typename T2>
  operator Vec<T2, 3>() const
  {
    Vec<T2, 3> result;
    for (uint32_t i = 0; i < 3; i++) result[i] = T2(values[i]);
    return result;
  }

  union {
#if defined(SSE_ENABLED)
    __m128 data;
#endif
    // align data to cache lines
    T values[4];
    struct {
      T x, y, z;
    };
  };
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const Vec<T, 3>& a)
{
  out << std::fixed << "(" << a.x << ", " << a.y << ", " << a.z << ")";
  return out;
}

#if defined(SSE_ENABLED)
inline Vec<float, 3> operator+(const Vec<float, 3>& a, const Vec<float, 3>& b)
{
  return Vec<float, 3>(_mm_add_ps(a.data, b.data));
}

inline Vec<float, 3> operator-(const Vec<float, 3>& a, const Vec<float, 3>& b)
{
  return Vec<float, 3>(_mm_sub_ps(a.data, b.data));
}

inline Vec<float, 3> operator*(const Vec<float, 3>& a, const Vec<float, 3>& b)
{
  return Vec<float, 3>(_mm_mul_ps(a.data, b.data));
}

inline Vec<float, 3> operator/(const Vec<float, 3>& a, const Vec<float, 3>& b)
{
  return Vec<float, 3>(_mm_div_ps(a.data, b.data));
}

inline Vec<float, 3> min(const Vec<float, 3>& a, const Vec<float, 3>& b)
{
  return Vec<float, 3>(_mm_min_ps(a.data, b.data));
}

inline Vec<float, 3> max(const Vec<float, 3>& a, const Vec<float, 3>& b)
{
  return Vec<float, 3>(_mm_max_ps(a.data, b.data));
}

inline float dot(const Vec<float, 3>& a, const Vec<float, 3>& b)
{
  __m128 result = _mm_dp_ps(a.data, b.data, 0x71);
  return _mm_cvtss_f32(result);
}
#endif // SSE_ENABLED

inline float min_component(const Vec<float, 3>& a)
{
  if (a.x < a.y)
  {
    if (a.x < a.z) return a.x;
    else return a.z;
  }
  else
  {
    if (a.y < a.z) return a.y;
    else return a.z;
  }
}

inline float max_component(const Vec<float, 3>& a)
{
  if (a.x > a.y)
  {
    if (a.x > a.z) return a.x;
    else return a.z;
  }
  else
  {
    if (a.y > a.z) return a.y;
    else return a.z;
  }
}

template<typename T>
Vec<T, 3> cross(const Vec<T, 3>& a, const Vec<T, 3>& b) requires(std::is_floating_point<T>::value)
{
  Vec<T, 3> result;
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;
  return result;
}

template<typename T>
Vec<T, 3> reflect(const Vec<T, 3>& incident_dir, const Vec<T, 3>& normal) requires(std::is_floating_point<T>::value)
{
  return incident_dir - 2.0 * cm::dot(incident_dir, normal) * normal;
}

template<typename T>
Vec<T, 3> refract(const Vec<T, 3>& incident_dir, const Vec<T, 3>& normal, T eta) requires(std::is_floating_point<T>::value)
{
  const T k = 1.0 - eta * eta * (1.0 - dot(incident_dir, normal) * dot(incident_dir, normal));
  if (k < 0.0) return Vec<T, 3>(0.0);
  else return eta * incident_dir - (eta * dot(incident_dir, normal) + std::sqrt(k)) * normal;
}

using Vec3 = Vec<float, 3>;
using Vec3i = Vec<int32_t, 3>;
using Vec3u = Vec<uint32_t, 3>;
} // namespace cm
