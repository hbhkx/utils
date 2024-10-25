#ifndef UTILS_MATH_UTILS_H
#define UTILS_MATH_UTILS_H

#include <algorithm>
#include <type_traits>
#include <utility>

#include "limits_16bit.h"
#include "macros.h"

// namespace std {  // extend negate operator for __fp16 and __bf16
// __forceinline__ constexpr __fp16 operator-(__fp16 a) {
//   return utils::bit_cast<__fp16>(
//       static_cast<uint16_t>(utils::bit_cast<uint16_t>(a) ^ 0x8000));
// }

// __forceinline__ constexpr __bf16 operator-(__bf16 a) {
//   return utils::bit_cast<__bf16>(
//       static_cast<uint16_t>(utils::bit_cast<uint16_t>(a) ^ 0x8000));
// }
// }  // namespace std

namespace utils {

// judge whether a number is power of 2
template <typename Int_Type>
__forceinline__ constexpr bool IsPowerOf2(Int_Type n) {
  static_assert(std::is_integral<Int_Type>::value,
                "Cannot judge non-integral type as power of 2!");
  return (n > 0) && !(n & (n - 1));
}

// ceiling division of two integers
template <typename Int_Type, typename Int_Type2>
__forceinline__ constexpr auto CeilDiv(Int_Type x,
                                       Int_Type2 y) -> decltype(x / y) {
  static_assert(
      std::is_integral<Int_Type>::value && std::is_integral<Int_Type2>::value,
      "CeilDiv only supports integral types!");
  return (x + y - 1) / y;
}

// align to a multiple of rhs no less than lhs
template <typename Int_Type, typename Int_Type2>
__forceinline__ constexpr auto AlignUp(Int_Type x,
                                       Int_Type2 y) -> decltype(x / y) {
  static_assert(
      std::is_integral<Int_Type>::value && std::is_integral<Int_Type2>::value,
      "AlignUp only supports integral types!");

#if __cplusplus >= 201402L
  if (IsPowerOf2(y)) {
    return (x + y - 1) &
           (~(static_cast<typename std::make_unsigned<Int_Type>::type>(y) - 1));
  }
#endif  // __cplusplus >= 201402L
  return CeilDiv(x, y) * y;
}

// align to a multiple of rhs no more than lhs
template <typename Int_Type, typename Int_Type2>
__forceinline__ constexpr auto AlignDown(Int_Type x,
                                         Int_Type2 y) -> decltype(x / y) {
  static_assert(
      std::is_integral<Int_Type>::value && std::is_integral<Int_Type2>::value,
      "AlignDown only supports integral types!");

#if __cplusplus >= 201402L
  if (IsPowerOf2(y)) {
    return x &
           (~(static_cast<typename std::make_unsigned<Int_Type>::type>(y) - 1));
  }
#endif  // __cplusplus >= 201402L
  return (x / y) * y;
}

// count leading zeros of an integer
__forceinline__ constexpr int32_t CountLeadingZeros(uint32_t x) {
  return __builtin_clz(x);
}

// align up to a power of 2
__forceinline__ constexpr uint32_t AlignUpPow2(uint32_t x) {
  return 1U << (32 - CountLeadingZeros(x - 1));
}

// get greatest common divisor of two intergers
// complexity: O(log(min(x, y)))
template <typename Int_Type, typename Int_Type2>
__forceinline__ auto GetGCD(Int_Type x, Int_Type2 y) -> decltype(x % y) {
  using ReturnType = decltype(x % y);

  ReturnType a = std::max(x, y);
  ReturnType b = std::min(x, y);

  while (b != 0) {
    ReturnType temp = b;
    b = a % b;
    a = temp;
  }

  return a;
}

// get max factor not greater than upper_bound
// complexity: linear, worst O(N)
template <typename Int_Type, typename Ari_Type,
          typename std::enable_if<std::is_integral<Int_Type>::value,
                                  bool>::type = true>
__forceinline__ UTILS_CONSTEXPR14 Int_Type GetMaxFactor(Int_Type number,
                                                        Ari_Type upper_bound) {
  static_assert(std::is_arithmetic<Ari_Type>::value, "non-supported type!");
  if (number <= 0) {
    return 1;
  }

  if (number <= upper_bound) {
    return number;
  }

  for (Int_Type factor = upper_bound; factor > 1; factor--) {
    if (number % factor == 0) return factor;
  }

  // for prime, return 1
  return 1;
}

// struct containing magic number for uint32 divide-by-constant optimization
typedef struct {
  unsigned M;  // Magic number,
  int a;       // "add" indicator,
  int s;       // and shift amount.
} MagicUnsigned;

// Calculate the magic numbers required to implement an unsigned integer
// division by a constant as a sequence of multiplies, adds and shifts.
// Taken from "Hacker's Delight", Henry S. Warren, Jr., chapter 10.
__forceinline__ MagicUnsigned GetDivMagicU(uint32_t d) {
  // Must have 1 <= d <= 2**32-1.
  MagicUnsigned magu;
  magu.a = 0;  // Initialize "add" indicator.
  if (d == 0) {
    magu.M = 0;  // Bad divisor, so just return.
    magu.s = 0;
    return magu;
  }

  int32_t p;
  uint32_t nc, delta, q1, r1, q2, r2;
  nc = -1 - (-d) % d;         // Unsigned arithmetic here.
  p = 31;                     // Init. p.
  q1 = 0x80000000 / nc;       // Init. q1 = 2**p/nc.
  r1 = 0x80000000 - q1 * nc;  // Init. r1 = rem(2**p, nc).
  q2 = 0x7FFFFFFF / d;        // Init. q2 = (2**p - 1)/d.
  r2 = 0x7FFFFFFF - q2 * d;   // Init. r2 = rem(2**p - 1, d).
  do {
    p = p + 1;
    if (r1 >= nc - r1) {
      q1 = 2 * q1 + 1;   // Update q1.
      r1 = 2 * r1 - nc;  // Update r1.
    } else {
      q1 = 2 * q1;
      r1 = 2 * r1;
    }
    if (r2 + 1 >= d - r2) {
      if (q2 >= 0x7FFFFFFF) magu.a = 1;
      q2 = 2 * q2 + 1;      // Update q2.
      r2 = 2 * r2 + 1 - d;  // Update r2.
    } else {
      if (q2 >= 0x80000000) magu.a = 1;
      q2 = 2 * q2;
      r2 = 2 * r2 + 1;
    }
    delta = d - 1 - r2;
  } while (p < 64 && (q1 < delta || (q1 == delta && r1 == 0)));
  magu.M = q2 + 1;  // Magic number
  magu.s = p - 32;  // and shift amount to return
  return magu;      // (magu.a was set above).
}
}  // namespace utils

#endif  // UTILS_UTILS_H
