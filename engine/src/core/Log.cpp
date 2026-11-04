#include "vibe/core/Log.hpp"

#include <iostream>

namespace vibe {

namespace {

const char* levelPrefix(LogLevel level) {
  switch (level) {
    case LogLevel::Trace:
      return "trace";
    case LogLevel::Debug:
      return "debug";
    case LogLevel::Info:
      return "info";
    case LogLevel::Warn:
      return "warn";
    case LogLevel::Error:
      return "error";
  }
  return "log";
}

}  // namespace

void log(LogLevel level, std::string_view message) {
  std::cerr << '[' << levelPrefix(level) << "] " << message << '\n';
}

}  // namespace vibe
