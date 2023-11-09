#pragma once

template <typename underlying_type, underlying_type _epsilon>
struct basic_char_type {
  underlying_type ch;
  constexpr static underlying_type epsilon = _epsilon;
  constexpr basic_char_type(underlying_type ch): ch{ch} {}
  basic_char_type(): ch{} {}
  auto operator<=>(basic_char_type const&) const = default;
};

template <typename underlying_type, underlying_type epsilon>
auto& operator<<(auto& os, basic_char_type<underlying_type, epsilon> ch) {
  if (ch.ch == epsilon) {
    return os << "epsilon";
  }
  if (ch.ch >= 20) {
    return os << '\'' << ch.ch << '\'';
  }
  return os << +ch.ch;
}

using char_type = basic_char_type<char, 0>;


