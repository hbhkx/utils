#ifndef UTILS_MACROS_H
#define UTILS_MACROS_H

#if __cplusplus >= 201402L
#define UTILS_CONSTEXPR14 constexpr
#else
#define UTILS_CONSTEXPR14
#endif

#if __cplusplus >= 201703L
#define UTILS_CONSTEXPR17 constexpr
#define UTILS_CONSTEXPR_IF if constexpr
#else
#define UTILS_CONSTEXPR17
#define UTILS_CONSTEXPR_IF if
#endif

#define __forceinline__ inline __attribute__((always_inline, internal_linkage))

#endif  // UTILS_MACROS_H
