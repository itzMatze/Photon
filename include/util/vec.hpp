#pragma once
#include <cstdint>
#include <cassert>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <type_traits>

// chaos math
namespace cm {
template<typename T, int N>
struct Vec
{
  constexpr Vec() = default;
  constexpr explicit Vec(const T value)
  {
    for (uint32_t i = 0; i < N; i++) values[i] = value;
  }
  constexpr explicit Vec(const T vals[N])
  {
    for (uint32_t i = 0; i < N; i++) values[i] = vals[i];
  }
  constexpr Vec(const std::initializer_list<T>& vals)
  {
    assert(vals.size() == N);
    uint32_t i = 0;
    for (const auto& val : vals)
    {
      values[i++] = val;
    }
  }

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
  operator Vec<T2, N>() const
  {
    Vec<T2, N> result;
    for (uint32_t i = 0; i < N; i++) result[i] = T2(values[i]);
    return result;
  }

  T values[N];
};

// only the operators that are needed are implemented
// so, not every expected overload might be there already
template<typename T1, typename T2, int N>
Vec<T1, N> operator+(const Vec<T1, N>& a, const Vec<T2, N>& b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = a.values[i] + T1(b.values[i]);
  return Vec<T1, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator+(const Vec<T1, N>& a, const T2 b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = a.values[i] + T1(b);
  return Vec<T1, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator-(const Vec<T1, N>& a, const Vec<T2, N>& b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = a.values[i] - T1(b.values[i]);
  return Vec<T1, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator-(const Vec<T1, N>& a, const T2 b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = a.values[i] - T1(b);
  return Vec<T1, N>(vals);
}

template<typename T, int N>
Vec<T, N> operator-(const Vec<T, N>& a)
{
  T vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = -a.values[i];
  return Vec<T, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator/(const Vec<T1, N>& a, const Vec<T2, N>& b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = a.values[i] / T1(b.values[i]);
  return Vec<T1, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator/(const Vec<T1, N>& a, const T2 b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = a.values[i] / T1(b);
  return Vec<T1, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator/(const T2 a, const Vec<T1, N>& b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = T1(a) / b.values[i];
  return Vec<T1, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator*(const Vec<T1, N>& a, const Vec<T2, N>& b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = a.values[i] * T1(b.values[i]);
  return Vec<T1, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator*(const Vec<T1, N>& a, const T2 b)
{
  T1 vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = a.values[i] * T1(b);
  return Vec<T1, N>(vals);
}

template<typename T1, typename T2, int N>
Vec<T1, N> operator*(const T2 a, const Vec<T1, N>& b)
{
  return operator*(b, a);
}

template<typename T, int N>
bool operator==(const Vec<T, N>& a, const Vec<T, N>& b)
{
  for (uint32_t i = 0; i < N; i++)
  {
    if (a[i] != b[i]) return false;
  }
  return true;
}

template<typename T, int N>
Vec<T, N> min(const Vec<T, N>& a, const Vec<T, N>& b)
{
  T vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = std::min(a[i], b[i]);
  return Vec<T, N>(vals);
}

template<typename T, int N>
Vec<T, N> max(const Vec<T, N>& a, const Vec<T, N>& b)
{
  T vals[N];
  for (uint32_t i = 0; i < N; i++) vals[i] = std::max(a[i], b[i]);
  return Vec<T, N>(vals);
}

template<typename T, int N>
T min_component(const Vec<T, N>& a)
{
  T min = std::numeric_limits<T>::max();
  for (uint32_t i = 0; i < N; i++) min = std::min(a[i], min);
  return min;
}

template<typename T, int N>
T max_component(const Vec<T, N>& a)
{
  T max = std::numeric_limits<T>::min();
  for (uint32_t i = 0; i < N; i++) max = std::max(a[i], max);
  return max;
}

template<typename T, int N>
float dot(const Vec<T, N>& a, const Vec<T, N>& b)
{
  float result = 0.0;
  for (uint32_t i = 0; i < N; i++) result += a.values[i] * b.values[i];
  return result;
}

template<typename T, int N>
float length(const Vec<T, N>& a)
{
  return std::sqrt(dot(a, a));
}

template<typename T, int N>
Vec<T, N> normalize(const Vec<T, N>& a) requires(std::is_floating_point<T>::value)
{
  return a / length(a);
}

template<typename T, int N>
Vec<T, N> abs(const Vec<T, N>& a)
{
  T values[N];
  for (uint32_t i = 0; i < N; i++) values[i] = std::abs(a.values[i]);
  return Vec<T, N>(values);
}

template<typename T, int N>
Vec<T, N> sign(const Vec<T, N>& a)
{
  T values[N];
  for (uint32_t i = 0; i < N; i++)
  {
    if (a.values[i] < T(0)) values[i] = T(-1);
    else if (a.values[i] > T(0)) values[i] = T(1);
    else values[i] = T(0);
  }
  return Vec<T, N>(values);
}

template<typename T1, typename T2, int N>
Vec<T1, N> pow(const Vec<T1, N>& a, const T2 b)
{
  Vec<T1, N> result;
  for (uint32_t i = 0; i < N; i++) result[i] = std::pow(a[i], T1(b));
  return result;
}
} // namespace cm

