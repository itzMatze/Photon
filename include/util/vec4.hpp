#pragma once
#include "glm/vec4.hpp"
#include <ostream>

template<typename T>
std::ostream& operator<<(std::ostream& out, const glm::vec<4, T>& a)
{
  out << std::fixed << "(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")";
  return out;
}

template<typename T>
glm::vec<4, T> operator+(const glm::vec<4, T>& a, T b)
{
  return glm::vec4(a.x + b, a.y + b, a.z + b, a.w + b);
}

template<typename T>
glm::vec<4, T> operator-(const glm::vec<4, T>& a, T b)
{
  return glm::vec4(a.x - b, a.y - b, a.z - b, a.w - b);
}

template<typename T>
glm::vec<4, T> operator*(const glm::vec<4, T>& a, T b)
{
  return glm::vec4(a.x * b, a.y * b, a.z * b, a.w * b);
}

template<typename T>
glm::vec<4, T> operator/(const glm::vec<4, T>& a, T b)
{
  return glm::vec4(a.x / b, a.y / b, a.z / b, a.w / b);
}
