#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace vibe {

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

struct Version {
  u32 major{};
  u32 minor{};
  u32 patch{};

  static Version parse(std::string_view text);
  std::string toString() const;
};

}  // namespace vibe
