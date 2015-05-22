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
 * A list of integers (wraps a long... template parameter pack).
 */
template <long... Is>
struct int_list {};

/**
 * Creates indices for from `Pos` to `Max`.
 */
template <long Max, long Pos = 0, typename Indices = int_list<>>
struct il_indices;

template <long Pos, long... Is>
struct il_indices<Pos, Pos, int_list<Is...>> {
  using type = int_list<Is...>;
};

template <long Max, long Pos, long... Is>
struct il_indices<Max, Pos, int_list<Is...>> {
  using type = typename il_indices<Max, Pos + 1, int_list<Is..., Pos>>::type;
};

template <long To, long From = 0>
typename il_indices<To, From>::type get_indices() {
  return {};
}

/**
 * apply arguments to function
 */
template <class F, long... Is, class Tuple>
inline auto apply_args(F& f, detail::int_list<Is...>, Tuple&& tup)
  -> decltype(f(std::get<Is>(tup)...)) {
  return f(std::get<Is>(tup)...);
}

template <class F, class Tuple, class... Ts>
inline auto apply_args_prefixed(F& f, detail::int_list<>, Tuple&, Ts&&... args)
  -> decltype(f(std::forward<Ts>(args)...)) {
  return f(std::forward<Ts>(args)...);
}

template <class F, long... Is, class Tuple, class... Ts>
inline auto apply_args_prefixed(F& f, detail::int_list<Is...>, Tuple& tup,
                                Ts&&... args)
  -> decltype(f(std::forward<Ts>(args)..., std::get<Is>(tup)...)) {
  return f(std::forward<Ts>(args)..., std::get<Is>(tup)...);
}

template <class F, long... Is, class Tuple, class... Ts>
inline auto apply_args_suffxied(F& f, detail::int_list<Is...>, Tuple& tup,
                                Ts&&... args)
  -> decltype(f(std::get<Is>(tup)..., std::forward<Ts>(args)...)) {
  return f(std::get<Is>(tup)..., std::forward<Ts>(args)...);
}

} // namespace detail
} // namespace riot

#endif // RIOT_THREAD_UTILS_HPP
