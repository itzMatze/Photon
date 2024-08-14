#pragma once
#include <string>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"

#define PH_ASSERTIONS

#define PH_THROW(...)                   \
{                                       \
  spdlog::critical(__VA_ARGS__);        \
  std::string s(__FILE__);              \
  s.append(": ");                       \
  s.append(std::to_string(__LINE__));   \
  spdlog::throw_spdlog_ex(s);           \
}

#if defined(PH_ASSERTIONS)
#define PH_ASSERT(X, ...) if (!(X)) PH_THROW(__VA_ARGS__);
#else
#define PH_ASSERT(X, ...) X
#endif

namespace phlog = spdlog;
