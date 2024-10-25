#ifndef UTILS_UNROLLER_H
#define UTILS_UNROLLER_H

#include <type_traits>
#include <utility>

#include "macros.h"

#if __cplusplus >= 201402L
#define UTILS_HAS_INTEGER_SEQUENCE 1
#else
#define UTILS_HAS_INTEGER_SEQUENCE 0
#endif

#if __cplusplus >= 201703L
#define UTILS_HAS_FOLD_EXPR 1
#else
#define UTILS_HAS_FOLD_EXPR 0
#endif

#define __lambda_inline__ __attribute__((always_inline, internal_linkage))
#define UTILS_STATIC_INLINE static __forceinline__

namespace unroll_helper {
#if UTILS_HAS_INTEGER_SEQUENCE
template <int... Ints>
using integer_sequence = std::integer_sequence<int, Ints...>;

template <int Num>
using make_integer_sequence = std::make_integer_sequence<int, Num>;
#else
// a tuple of indices, used to extract the elements in a tuple.
template <int...>
struct _Index_tuple {};

// concatenates two Index_tuples.
template <typename, typename>
struct _ITuple_cat;

template <int... Ind1, int... Ind2>
struct _ITuple_cat<_Index_tuple<Ind1...>, _Index_tuple<Ind2...>> {
  using type = _Index_tuple<Ind1..., (Ind2 + sizeof...(Ind1))...>;
};

// builds an Index_tuple<0, 1, 2, ..., Num - 1>.
template <int Num>
struct _Build_index_tuple
    : _ITuple_cat<typename _Build_index_tuple<Num / 2>::type,
                  typename _Build_index_tuple<Num - Num / 2>::type> {};

template <>
struct _Build_index_tuple<1> {
  using type = _Index_tuple<0>;
};

template <>
struct _Build_index_tuple<0> {
  using type = _Index_tuple<>;
};

/// class template integer_sequence
template <int... Idx>
struct integer_sequence {
  using type = int;
  static constexpr std::size_t size() noexcept { return sizeof...(Idx); }
};

template <int Num, typename ISeq = typename _Build_index_tuple<Num>::type>
struct _Make_integer_sequence;

template <int Num, int... Idx>
struct _Make_integer_sequence<Num, _Index_tuple<Idx...>> {
  static_assert(Num >= 0, "Cannot make integer sequence of negative length");

  using type = integer_sequence<Idx...>;
};

/// Alias template make_integer_sequence
template <int Num>
using make_integer_sequence = typename _Make_integer_sequence<Num>::type;
#endif

template <class>
struct unroll_impl;

/* helper template class to manually unroll the loop */
template <int... Ints>
struct unroll_impl<unroll_helper::integer_sequence<Ints...>> {
  template <int START, int STEP, class Func, typename... Args>
  UTILS_STATIC_INLINE void call(Func&& func, Args&&... args) {
#if UTILS_HAS_FOLD_EXPR
    (func(Ints * STEP + START, std::forward<Args>(args)...), ...);
#else
    int dummy[] = {
        0,
        ((void)func(Ints * STEP + START, std::forward<Args>(args)...), 0)...};
    (void)dummy;
#endif
  }

  template <int START, int END, int STEP,
            template <int, int, int, int, class...> class F, class... Types,
            typename... Args>
  UTILS_STATIC_INLINE void call(Args&&... args) {
#if UTILS_HAS_FOLD_EXPR
    (F<Ints * STEP + START, START, END, STEP, Types...>::call(
         std::forward<Args>(args)...),
     ...);
#else
    int dummy[] = {
        0, ((void)F<Ints * STEP + START, START, END, STEP, Types...>::call(
                std::forward<Args>(args)...),
            0)...};
    (void)dummy;
#endif
  }

  template <int START, int, int STEP, template <int, class...> class F,
            class... Types, typename... Args>
  UTILS_STATIC_INLINE void call(Args&&... args) {
#if UTILS_HAS_FOLD_EXPR
    (F<Ints * STEP + START, Types...>::call(std::forward<Args>(args)...), ...);
#else
    int dummy[] = {0, ((void)F<Ints * STEP + START, Types...>::call(
                           std::forward<Args>(args)...),
                       0)...};
    (void)dummy;
#endif
  }

  template <int START, int END, int STEP,
            template <int, int, int, class...> class F, class... Types,
            typename... Args>
  UTILS_STATIC_INLINE void call(Args&&... args) {
#if UTILS_HAS_FOLD_EXPR
    (F<Ints * STEP + START, START, END, Types...>::call(
         std::forward<Args>(args)...),
     ...);
#else
    int dummy[] = {0, ((void)F<Ints * STEP + START, START, END, Types...>::call(
                           std::forward<Args>(args)...),
                       0)...};
    (void)dummy;
#endif
  }
};
}  // namespace unroll_helper

/* ========================================================================== *
 *                                 for functions                              *
 * ========================================================================== */
/* usage of this template:
 * to unroll a function body manually with index i, invoke using a lambda
 *     unroll_for<START, END, STEP>([&](int i, Args... params){...}, args...)
 * for most cases, Args... can be ignored
 * this unroll the loop for i starting from START to END with stride STEP
 * special invocable cases:
 *    a. start is 0, step is 1, NUM_UNROLL is the number of loops unrolled:
 *      unroll_for<NUM_UNROLL>([&](int i, Args... params){...}, args...)
 *    b. step is 1, specified start and end:
 *      unroll_for<START, END>([&](int i, Args... params){...}, args...)
 */
template <int START, int END, int STEP, class Func, typename... Args,
          typename std::enable_if<(STEP != 0 && (END - START) / STEP > 0),
                                  bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Func&& func, Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<(END - START) / STEP>>::template call<START,
                                                                          STEP>(
      std::forward<Func>(func), std::forward<Args>(args)...);
}

template <int END, class Func, typename... Args,
          typename std::enable_if<(END > 0), bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Func&& func, Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<END>>::template call<0, 1>(
      std::forward<Func>(func), std::forward<Args>(args)...);
}

template <int START, int END, class Func, typename... Args,
          typename std::enable_if<(END > START), bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Func&& func, Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<END - START>>::template call<START, 1>(
      std::forward<Func>(func), std::forward<Args>(args)...);
}

template <class IntSeq, class Func, typename... Args>
UTILS_STATIC_INLINE void unroll_for(Func&& func, Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<IntSeq>::template call<0, 1>(std::forward<Func>(func),
                                           std::forward<Args>(args)...);
}

template <int START, int END, int STEP, class Func, typename... Args,
          typename std::enable_if<(STEP == 0 || (END - START) / STEP <= 0),
                                  bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Func&& func, Args&&... args) {
  static_assert(STEP != 0, "step in unroll_for cannot be 0");
}

template <int END, class Func, typename... Args,
          typename std::enable_if<(END <= 0), bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Func&& func, Args&&... args) {}

template <int START, int END, class Func, typename... Args,
          typename std::enable_if<(END <= START), bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Func&& func, Args&&... args) {}

/* ========================================================================== *
 *                              for structures                                *
 * ========================================================================== */
/* usage of this template:
 * 1. construct a struct with
 *    template <int INDEX, int START, int END, int STEP, class... Types>
 * 2. implement a static member function `call' (return void) in the struct
 *    with arbitrary parameters, which contains loop body indexed by INDEX
 * 3. invoke like
 *    unroll_for<START, END, STEP, struct_name, Types...>(args...)
 *    to unroll the `call' function with (END - START) / STEP times
 * special invocable cases:
 *    a. start is 0, step is 1, NUM_UNROLL is the number of loops unrolled:
 *      template <int INDEX, class... Types> struct_name
 *      unroll_for<NUM_UNROLL, struct_name, Types...>(args...)
 *    b. step is 1, specified start and end:
 *      template <int INDEX, int START, int END, class... Types> struct_name
 *      unroll_for<START, END, struct_name, Types...>(args...)
 */
template <int START, int END, int STEP,
          template <int, int, int, int, class...> class F, class... Types,
          typename... Args,
          typename std::enable_if<(STEP != 0 && (END - START) / STEP > 0),
                                  bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<(END - START) / STEP>>::template call<
      START, END, STEP, F, Types...>(std::forward<Args>(args)...);
}

template <int START, int END, int STEP, template <int, class...> class F,
          class... Types, typename... Args,
          typename std::enable_if<(STEP != 0 && (END - START) / STEP > 0),
                                  bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<(END - START) / STEP>>::template call<
      START, END, STEP, F, Types...>(std::forward<Args>(args)...);
}

template <int START, int END, template <int, class...> class F, class... Types,
          typename... Args,
          typename std::enable_if<(END > START), bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<END - START>>::template call<START, END, 1,
                                                                 F, Types...>(
      std::forward<Args>(args)...);
}

template <int END, template <int, class...> class F, class... Types,
          typename... Args,
          typename std::enable_if<(END > 0), bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<END>>::template call<0, END, 1, F,
                                                         Types...>(
      std::forward<Args>(args)...);
}

template <int START, int END, int STEP,
          template <int, int, int, class...> class F, class... Types,
          typename... Args,
          typename std::enable_if<(STEP != 0 && (END - START) / STEP > 0),
                                  bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<(END - START) / STEP>>::template call<
      START, END, STEP, F, Types...>(std::forward<Args>(args)...);
}

template <int START, int END, template <int, int, int, class...> class F,
          class... Types, typename... Args,
          typename std::enable_if<(END > START), bool>::type = true>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<make_integer_sequence<END - START>>::template call<START, END, 1,
                                                                 F, Types...>(
      std::forward<Args>(args)...);
}

template <class IntSeq, template <int, class...> class F, class... Types,
          typename... Args>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  using namespace unroll_helper;
  unroll_impl<IntSeq>::template call<0, -1, 1, F, Types...>(
      std::forward<Args>(args)...);
}

template <int START, int END, int STEP,
          template <int, int, int, int, class...> class F, class...,
          typename... Args,
          typename std::enable_if<(STEP == 0 || (END - START) / STEP <= 0),
                                  bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  static_assert(STEP != 0, "step in unroll_for cannot be 0");
}

template <int START, int END, int STEP, template <int, class...> class F,
          class... Types, typename... Args,
          typename std::enable_if<(STEP == 0 || (END - START) / STEP <= 0),
                                  bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  static_assert(STEP != 0, "step in unroll_for cannot be 0");
}

template <int START, int END, template <int, class...> class F, class... Types,
          typename... Args,
          typename std::enable_if<(END <= START), bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {}

template <int END, template <int, class...> class F, class... Types,
          typename... Args,
          typename std::enable_if<(END <= 0), bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {}

template <int START, int END, int STEP,
          template <int, int, int, class...> class F, class... Types,
          typename... Args,
          typename std::enable_if<(STEP == 0 || (END - START) / STEP <= 0),
                                  bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {
  static_assert(STEP != 0, "step in unroll_for cannot be 0");
}

template <int START, int END, template <int, int, int, class...> class F,
          class... Types, typename... Args,
          typename std::enable_if<(END <= START), bool>::type = false>
UTILS_STATIC_INLINE void unroll_for(Args&&... args) {}

#undef UTILS_STATIC_INLINE
#endif  // UTILS_UNROLLER_H
