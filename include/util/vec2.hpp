#pragma once
#include "glm/vec2.hpp"
#include <ostream>

template<typename T>
std::ostream& operator<<(std::ostream& out, const glm::vec<2, T>& a)
{
  out << std::fixed << "(" << a.x << ", " << a.y << ", " << ")";
  return out;
}

template<typename T>
glm::vec<2, T> operator+(const glm::vec<2, T>& a, T b)
{
  return glm::vec2(a.x + b, a.y + b);
}

template<typename T>
glm::vec<2, T> operator-(const glm::vec<2, T>& a, T b)
{
  return glm::vec2(a.x - b, a.y - b);
}

template<typename T>
glm::vec<2, T> operator*(const glm::vec<2, T>& a, T b)
{
  return glm::vec2(a.x * b, a.y * b);
}

template<typename T>
glm::vec<2, T> operator/(const glm::vec<2, T>& a, T b)
{
  return glm::vec2(a.x / b, a.y / b);
}
