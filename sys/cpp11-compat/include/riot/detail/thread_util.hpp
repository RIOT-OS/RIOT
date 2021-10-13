/*
 * Copyright (C) 2015 Hamburg University of Applied Sciences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpp11-compat
 * @{
 *
 * @file
 * @brief   utility functions
 *
 * @author  Dominik Charousset <dominik.charousset (at) haw-hamburg.de>
 * @author  Raphael Hiesgen <raphael.hiesgen (at) haw-hamburg.de>
 *
 * @}
 */

#ifndef RIOT_THREAD_UTILS_HPP
#define RIOT_THREAD_UTILS_HPP

#include <tuple>
#include <utility>

namespace riot {
namespace detail {

/**
 * @brief A list of integers (wraps a long... template parameter pack).
 */
template <long... Is>
struct int_list {};

/**
 * @brief Creates indices from `Pos` to `Max`.
 */
template <long Max, long Pos = 0, typename Indices = int_list<>>
struct il_indices;

/**
 * @brief End of recursion, `Pos` reached `Max`.
 */
template <long Pos, long... Is>
struct il_indices<Pos, Pos, int_list<Is...>> {
  /**
   * @brief Result is the list containing `Is...`.
   */
  using type = int_list<Is...>;
};

/**
 * @brief Recursion step.
 */
template <long Max, long Pos, long... Is>
struct il_indices<Max, Pos, int_list<Is...>> {
  /**
   * @brief Append `Pos` to list and increment for the next step.
   */
  using type = typename il_indices<Max, Pos + 1, int_list<Is..., Pos>>::type;
};

/**
 * @brief Function to create a list of indices from `From` to `To`.
 */
template <long To, long From = 0>
typename il_indices<To, From>::type get_indices() {
  return {};
}

/**
 * @brief Apply arguments in a tuple to function.
 */
template <class F, long... Is, class Tuple>
inline auto apply_args(F& f, detail::int_list<Is...>, Tuple&& tup)
  -> decltype(f(std::get<Is>(tup)...)) {
  return f(std::get<Is>(tup)...);
}

/**
 * @brief Prefix the argument tuple with additional arguments.
 *        In this case the tuple is empty.
 */
template <class F, class Tuple, class... Ts>
inline auto apply_args_prefixed(F& f, detail::int_list<>, Tuple&, Ts&&... args)
  -> decltype(f(std::forward<Ts>(args)...)) {
  return f(std::forward<Ts>(args)...);
}

/**
 * @brief Prefix the argument tuple with additional arguments.
 *        In this case the tuple is contains arguments.
 */
template <class F, long... Is, class Tuple, class... Ts>
inline auto apply_args_prefixed(F& f, detail::int_list<Is...>, Tuple& tup,
                                Ts&&... args)
  -> decltype(f(std::forward<Ts>(args)..., std::get<Is>(tup)...)) {
  return f(std::forward<Ts>(args)..., std::get<Is>(tup)...);
}

/**
 * @brief Suffix the tuple with additional arguments.
 */
template <class F, long... Is, class Tuple, class... Ts>
inline auto apply_args_suffxied(F& f, detail::int_list<Is...>, Tuple& tup,
                                Ts&&... args)
  -> decltype(f(std::get<Is>(tup)..., std::forward<Ts>(args)...)) {
  return f(std::get<Is>(tup)..., std::forward<Ts>(args)...);
}

} // namespace detail
} // namespace riot

#endif // RIOT_THREAD_UTILS_HPP
