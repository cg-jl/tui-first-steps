#pragma once

namespace vt::input {
constexpr char control(char c) { return c & 0x1f; }
}  // namespace vt::input