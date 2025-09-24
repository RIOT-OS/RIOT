/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

#include "modules.h"
#include "macros/utils.h"

/**
 * @file
 * @brief   Utility macros for `unicoap`
 * @ingroup net_unicoap
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#ifndef DOXYGEN
#  define UNICOAP_CODE_CLASS_DETAIL_FORMAT "%u.%02u"

#  define _CONCAT3(a, b, c)                      CONCAT3(a, b, c)


/* This is best explained by the following two examples.
 *  --> denotes a proprocessor evaluation step.
 *
 *     __unicoap_bit(11)
 * --> __unicoap_create_bit_or_nothing(11, CONCAT(__unicoap_two_empty_args_, 11))
 * --> __unicoap_create_bit_or_nothing(11, __unicoap_two_empty_args_11)
 *                                         \...... __VA_ARGS__ ......./
 * --> __take_second_arg(__unicoap_two_empty_args_11, (1 << (11)) |)
 *                       \...... __VA_ARGS__ ......./
 *  --> (1 << (11)) |
 *
 *
 *     __unicoap_bit()
 * --> __unicoap_create_bit_or_nothing(, CONCAT(__unicoap_two_empty_args_,))
 * --> __unicoap_create_bit_or_nothing(, __unicoap_two_empty_args_)
                                        \.. This is not a token ../
                                            as above but a real
                                           preprocessor constant.

 * --> __unicoap_create_bit_or_nothing(,~,)
                                       \....... Three arguments: empty, tilde, empty
 * --> __take_second_arg(~,,)
 *                       \...... __VA_ARGS__ followed by empty, former first ("offset"), argument
 * -->
 *     \.... nothing/empty
 *
 *
 * The last trailing OR slash is always followed by a zero:
 *
 * UNICOAP_BITFIELD(...) __unicoap_create_bitfield(__VA_ARGS__,,,,,,,,,,,,,,,) 0
 *
 * The numerous commas ensure there are always at least 16 arguments, although they may be empty.
 * See the explainer above how an expansion of __unicoap_bit works with an empty argument.
 */

#  define __unicoap_create_bit_or_nothing(offset, ...) \
      __take_second_arg(__VA_ARGS__, (1 << (offset)) |)

#  define __unicoap_two_empty_args_ ~,

#  define __unicoap_bit(offset, ...) \
      __unicoap_create_bit_or_nothing(offset, CONCAT(__unicoap_two_empty_args_, offset))

#  define __unicoap_create_bitfield(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, \
                                    _15, _16, ...)                                               \
      __unicoap_bit(_1) __unicoap_bit(_2) __unicoap_bit(_3) __unicoap_bit(_4) __unicoap_bit(_5)  \
          __unicoap_bit(_6) __unicoap_bit(_7) __unicoap_bit(_8) __unicoap_bit(_9)                \
              __unicoap_bit(_10) __unicoap_bit(_11) __unicoap_bit(_12) __unicoap_bit(_13)        \
                  __unicoap_bit(_14) __unicoap_bit(_15) __unicoap_bit(_16)
#endif

/**
 * @brief Creates a bitfield where each argument i indicates the i-th bit is to be set
 *
 * For example, `UNICOAP_BITFIELD(11, 8, 9)` will return a bitfield with the 11th, 8th, and 9th bit
 * set, i.e. `(1 << 11) | (1 << 8) | (1 << 9)`
 */
#define UNICOAP_BITFIELD(...) __unicoap_create_bitfield(__VA_ARGS__,,,,,,,,,,,,,,,) 0

#ifdef __cplusplus
extern "C" {}
#endif
