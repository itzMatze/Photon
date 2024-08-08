#pragma once
#include "glm/vec3.hpp"
#include <ostream>

template<typename T>
std::ostream& operator<<(std::ostream& out, const glm::vec<3, T>& a)
{
  out << std::fixed << "(" << a.x << ", " << a.y << ", " << a.z << ")";
  return out;
}

template<typename T>
glm::vec<3, T> operator+(const glm::vec<3, T>& a, T b)
{
  return glm::vec3(a.x + b, a.y + b, a.z + b);
}

template<typename T>
glm::vec<3, T> operator-(const glm::vec<3, T>& a, T b)
{
  return glm::vec3(a.x - b, a.y - b, a.z - b);
}

template<typename T>
glm::vec<3, T> operator*(const glm::vec<3, T>& a, T b)
{
  return glm::vec3(a.x * b, a.y * b, a.z * b);
}

template<typename T>
glm::vec<3, T> operator/(const glm::vec<3, T>& a, T b)
{
  return glm::vec3(a.x / b, a.y / b, a.z / b);
}
