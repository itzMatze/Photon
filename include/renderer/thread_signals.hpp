#pragma once

#include <cstdint>
#include "util/named_bitfield.hpp"

enum class SignalFlags : uint32_t
{
  None = 0,
  Stop = (1 << 0),
  PreventOutputAccess = (1 << 1),
  Done = (1 << 2),
};

using Signals = NamedBitfield<SignalFlags>;
ENABLE_ENUM_OPERATORS(SignalFlags);
