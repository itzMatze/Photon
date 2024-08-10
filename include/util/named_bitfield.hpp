#pragma once

#include <type_traits>
#include <cstdint>

template<typename Enum>
concept UIntEnum = std::is_enum_v<Enum> &&
                      (std::is_same_v<std::underlying_type_t<Enum>, uint32_t> ||
                       std::is_same_v<std::underlying_type_t<Enum>, uint64_t>);

// wrapper class for enums to enable bitwise operations on scoped enums
template<UIntEnum Enum>
class NamedBitfield {
public:
  constexpr NamedBitfield() : value(0) {}
  constexpr NamedBitfield(std::underlying_type_t<Enum> value) : value(value) {}
  constexpr NamedBitfield(Enum value) : value(static_cast<std::underlying_type_t<Enum>>(value)) {}

  friend NamedBitfield<Enum> operator|(NamedBitfield<Enum> a, NamedBitfield<Enum> b)
  {
    return NamedBitfield<Enum>((a.value) | (b.value));
  }

  friend NamedBitfield<Enum>& operator|=(NamedBitfield<Enum> &a, NamedBitfield<Enum> b)
  {
    a.value = a.value | b.value;
    return a;
  }

  friend NamedBitfield<Enum> operator&(NamedBitfield<Enum> a, NamedBitfield<Enum> b)
  {
    return NamedBitfield<Enum>((a.value) & (b.value));
  }

  friend NamedBitfield<Enum>& operator&=(NamedBitfield<Enum> &a, NamedBitfield<Enum> b)
  {
    a.value = a.value & b.value;
    return a;
  }

  friend NamedBitfield<Enum> operator~(NamedBitfield<Enum> a)
  {
    return NamedBitfield<Enum>(~(a.value));
  }

  operator bool() const
  {
    return value != 0;
  }

private:
  std::underlying_type_t<Enum> value;
};

// enable bitwise operations directly on enum values when they cannot be implicitly casted to the wrapper class type
template<typename Enum>
struct EnumTraits
{
  static constexpr bool enable_operators = false;
};

#define ENABLE_ENUM_OPERATORS(EnumType) \
template<> \
struct EnumTraits<EnumType> { \
  static constexpr bool enable_operators = true; \
};

template<UIntEnum Enum>
inline std::enable_if<EnumTraits<Enum>::enable_operators, NamedBitfield<Enum>>::type
operator|(Enum a, Enum b)
{
  return NamedBitfield<Enum>(static_cast<std::underlying_type_t<Enum>>(a) | static_cast<std::underlying_type_t<Enum>>(b));
}

template<UIntEnum Enum>
inline std::enable_if<EnumTraits<Enum>::enable_operators, NamedBitfield<Enum>>::type
operator&(Enum a, Enum b)
{
  return NamedBitfield<Enum>(static_cast<std::underlying_type_t<Enum>>(a) & static_cast<std::underlying_type_t<Enum>>(b));
}

template<UIntEnum Enum>
inline std::enable_if<EnumTraits<Enum>::enable_operators, NamedBitfield<Enum>>::type
operator~(Enum a)
{
  return NamedBitfield<Enum>(~static_cast<std::underlying_type_t<Enum>>(a));
}
