#include "vibe/core/Types.hpp"

#include <charconv>
#include <sstream>
#include <vector>

namespace vibe {

Version Version::parse(std::string_view text) {
  Version v{};
  std::vector<u32> parts;
  u32 current = 0;
  for (char c : text) {
    if (c == '.') {
      parts.push_back(current);
      current = 0;
    } else if (c >= '0' && c <= '9') {
      current = current * 10 + static_cast<u32>(c - '0');
    }
  }
  parts.push_back(current);
  if (parts.size() > 0) v.major = parts[0];
  if (parts.size() > 1) v.minor = parts[1];
  if (parts.size() > 2) v.patch = parts[2];
  return v;
}

std::string Version::toString() const {
  std::ostringstream oss;
  oss << major << '.' << minor << '.' << patch;
  return oss.str();
}

}  // namespace vibe
