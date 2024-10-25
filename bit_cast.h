#ifndef UTILS_BIT_CAST_H
#define UTILS_BIT_CAST_H

#include <type_traits>

#include "macros.h"

namespace utils {
// alternative to reinterpret_cast a value obeying strict aliasing rule
template <typename To, typename From>
__forceinline__ constexpr To bit_cast(const From& from) {
  static_assert(!std::is_pointer<typename std::decay<From>::type>::value &&
                    !std::is_pointer<typename std::decay<To>::type>::value &&
                    sizeof(From) == sizeof(To),
                "cannot cast through pointers or through different bits!");
  // typename std::remove_cv<To>::type to;
  // std::memcpy(&to, &from, sizeof(From));
  // return to;
  return __builtin_bit_cast(To, from);
}

// alternative to reinterpret_cast a pointer obeying strict aliasing rule
template <typename To, typename From>
__forceinline__ constexpr To pointer_cast(From&& from) {
  static_assert(
      std::is_pointer<typename std::decay<From>::type>::value &&
          std::is_pointer<To>::value &&
          sizeof(typename std::decay<From>::type) == sizeof(To),
      "pointer_cast supports only casting from one pointer type to another!!");

  static_assert(
      !std::is_const<typename std::remove_pointer<
              typename std::remove_reference<From>::type>::type>::value ||
          std::is_const<typename std::remove_pointer<To>::type>::value,
      "cannot cast from const pointer to non-const pointer!!");

  using TempPtr = typename std::conditional<
      std::is_const<typename std::remove_pointer<
          typename std::remove_reference<From>::type>::type>::value,
      const void*, void*>::type;

  return static_cast<To>(static_cast<TempPtr>(from));
}
}  // namespace utils
#endif  // UTILS_BIT_CAST_H
