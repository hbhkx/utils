#ifndef UTILS_NAIVE_ARRAY_H
#define UTILS_NAIVE_ARRAY_H

#include "bit_cast.h"

namespace utils {
// NaiveArray is a simple array-like class that can be used in device code.
template <typename T, int N, int alignment = sizeof(T)>
class NaiveArray {
  static_assert(alignment >= sizeof(T), "alignment must be at least sizeof(T)");

 public:
  __forceinline__ NaiveArray() = default;
  __forceinline__ NaiveArray(T* ptr) {
    for (int i = 0; i < N; i++) {
      (*this)[i] = ptr[i];
    }
  }

  __forceinline__ UTILS_CONSTEXPR14 T& operator[](size_t i) {
    return *pointer_cast<T*>(value + i * alignment);
  }
  __forceinline__ constexpr const T& operator[](size_t i) const {
    return *pointer_cast<const T*>(value + i * alignment);
  }

  __forceinline__ T* data() { return pointer_cast<T*>(value); }

 private:
  __attribute__((
      aligned(AlignUpPow2(alignment)))) unsigned char value[N * alignment];
};
}  // namespace utils

#endif  // UTILS_NAIVE_ARRAY_H
