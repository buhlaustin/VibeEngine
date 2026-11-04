#pragma once

#include <string_view>

namespace vibe {

enum class LogLevel { Trace, Debug, Info, Warn, Error };

void log(LogLevel level, std::string_view message);

}  // namespace vibe
