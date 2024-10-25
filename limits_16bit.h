#ifndef UTILS_LIMITS_16BIT_H
#define UTILS_LIMITS_16BIT_H

#include <cstdint>
#include <limits>

#include "bit_cast.h"

namespace std {  // extend numeric_limits for half and bfloat
template <>
struct numeric_limits<__fp16> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = false;
  static constexpr bool is_exact = false;
  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = true;
  static constexpr bool has_denorm = true;
  static constexpr bool has_denorm_loss = false;
  static constexpr std::float_round_style round_style = std::round_to_nearest;
  static constexpr bool is_iec559 = true;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = false;
  static constexpr int digits = 11;
  static constexpr int digits10 = 3;
  static constexpr int max_digits10 = 5;
  static constexpr int radix = 2;
  static constexpr int min_exponent = -13;
  static constexpr int min_exponent10 = -4;
  static constexpr int max_exponent = 16;
  static constexpr int max_exponent10 = 4;
  static constexpr bool traps = false;
  static constexpr bool tinyness_before = false;

  static constexpr __fp16 min() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0x0400));
  }

  static constexpr __fp16 lowest() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0xfbff));
  }

  static constexpr __fp16 max() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0x7bff));
  }

  static constexpr __fp16 epsilon() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0x1400));
  }

  static constexpr __fp16 round_error() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0x3800));
  }

  static constexpr __fp16 infinity() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0x7c00));
  }

  static constexpr __fp16 quiet_NaN() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0x7fff));
  }

  static constexpr __fp16 signaling_NaN() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0x7dff));
  }

  static constexpr __fp16 denorm_min() noexcept {
    return utils::bit_cast<__fp16>(static_cast<int16_t>(0x0001));
  }
};

template <>
struct numeric_limits<__bf16> {
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = false;
  static constexpr bool is_exact = false;
  static constexpr bool has_infinity = true;
  static constexpr bool has_quiet_NaN = true;
  static constexpr bool has_signaling_NaN = true;
  static constexpr bool has_denorm = true;
  static constexpr bool has_denorm_loss = false;
  static constexpr std::float_round_style round_style = std::round_to_nearest;
  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = false;
  static constexpr int digits = 8;
  static constexpr int digits10 = 2;
  static constexpr int max_digits10 = 3;
  static constexpr int radix = 2;
  static constexpr int min_exponent = -7;
  static constexpr int min_exponent10 = -2;
  static constexpr int max_exponent = 8;
  static constexpr int max_exponent10 = 2;
  static constexpr bool traps = false;
  static constexpr bool tinyness_before = false;

  static constexpr __bf16 min() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0x0080));
  }

  static constexpr __bf16 lowest() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0xff7f));
  }

  static constexpr __bf16 max() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0x7f7f));
  }

  static constexpr __bf16 epsilon() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0x0100));
  }

  static constexpr __bf16 round_error() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0x0180));
  }

  static constexpr __bf16 infinity() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0x7f80));
  }

  static constexpr __bf16 quiet_NaN() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0x7fff));
  }

  static constexpr __bf16 signaling_NaN() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0x7dff));
  }

  static constexpr __bf16 denorm_min() noexcept {
    return utils::bit_cast<__bf16>(static_cast<int16_t>(0x0001));
  }
};
}  // namespace std
#endif  // UTILS_LIMITS_16BIT_H
