#pragma once
#include "vec.hpp"
#include <ostream>

// chaos math
namespace cm {
template<typename T>
struct Vec<T, 2>
{
  constexpr Vec() = default;
  constexpr explicit Vec(const T value) : x(value), y(value) {}
  constexpr Vec(const T x, const T y) : x(x), y(y) {}
  constexpr Vec(const Vec& other) : x(other.x), y(other.y) {}
  constexpr Vec(const T vals[2]) : x(vals[0]), y(vals[1]) {}
  constexpr Vec(const std::initializer_list<T>& vals)
  {
    assert(vals.size() == 2);
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
  operator Vec<T2, 2>() const
  {
    Vec<T2, 2> result;
    for (uint32_t i = 0; i < 2; i++) result[i] = T2(values[i]);
    return result;
  }

  union {
    T values[2];
    struct {
      T x, y;
    };
    struct {
      T u, v;
    };
  };
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const Vec<T, 2>& a)
{
  out << std::fixed << "(" << a.x << ", " << a.y << ")";
  return out;
}

using Vec2 = Vec<float, 2>;
using Vec2i = Vec<int32_t, 2>;
using Vec2u = Vec<uint32_t, 2>;
} // namespace cm
