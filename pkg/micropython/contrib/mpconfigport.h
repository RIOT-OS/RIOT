/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython port configuration for RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdint.h>

#include "irq.h"

// RIOT's compiler_hints.h (pulled in above) defines NORETURN, and MicroPython's
// mpconfig.h redefines it (as MP_NORETURN) further down. Both expand to
// __attribute__((noreturn)), but the differing macro text trips -Werror on
// redefinition, so drop RIOT's definition and let MicroPython provide its own.
#undef NORETURN

// Usually passed from Makefile
#ifndef MICROPY_HEAP_SIZE
#  define MICROPY_HEAP_SIZE (16 * 1024)
#endif

#define MICROPY_ENABLE_SOURCE_LINE          (1)
#define MICROPY_STACK_CHECK                 (1)
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_ENABLE_SCHEDULER            (0)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_REPL_AUTO_INDENT            (1)
#define MICROPY_KBD_EXCEPTION               (0)
#define MICROPY_CPYTHON_COMPAT              (0)
#define MICROPY_PY_ASYNC_AWAIT              (0)
#define MICROPY_USE_INTERNAL_PRINTF         (0)
#define MICROPY_PY_ATTRTUPLE                (1)
#define MICROPY_PY_BUILTINS_ENUMERATE       (1)
#define MICROPY_PY_BUILTINS_FILTER          (1)
#define MICROPY_PY_BUILTINS_MIN_MAX         (1)
#define MICROPY_PY_BUILTINS_PROPERTY        (1)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS     (1)
#define MICROPY_PY_BUILTINS_REVERSED        (1)
#define MICROPY_PY_BUILTINS_SET             (1)
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       riot_help_text
#define MICROPY_PY_ARRAY                    (1)
#define MICROPY_PY_COLLECTIONS              (1)
#define MICROPY_PY_CMATH                    (1)
#define MICROPY_PY_IO                       (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     (1)
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_PY_STRUCT                   (1)
#ifdef MODULE_PERIPH_SPI
#  define MICROPY_PY_MACHINE_SPI            (1)
#endif
#ifdef MODULE_PERIPH_ADC
#  define MICROPY_PY_MACHINE_ADC            (1)
#endif
#define MICROPY_PY_MACHINE_SPI_MAKE_NEW machine_hw_spi_make_new
#ifdef CONFIG_NETWORKING
#define MICROPY_PY_UERRNO                   (1)
#  define MICROPY_PY_USOCKET                (1)
#endif
#define MICROPY_EPOCH_IS_1970                       (1)
#define MICROPY_PY_TIME                             (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME     (1)
#define MICROPY_PY_TIME_TIME_TIME_NS                (1)
#define MICROPY_PY_TIME_INCLUDEFILE                 "modtime_riot.h"
#define MICROPY_PY_RIOT                             (1)
#define MICROPY_PY_XTIMER                           (1)
#define MICROPY_PY_SYS_MODULES                      (1)
#define MICROPY_LONGINT_IMPL                        (MICROPY_LONGINT_IMPL_LONGLONG)
#define MICROPY_FLOAT_IMPL                          (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_COMPLEX                 (0)

// Saving extra crumbs to make sure binary fits in 128K
#define MICROPY_COMP_CONST_FOLDING          (0)
#define MICROPY_COMP_CONST                  (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN    (0)

#define MICROPY_PY_SYS_PLATFORM             "RIOT"

#define MICROPY_HW_BOARD_NAME               "riot-" RIOT_BOARD
#define MICROPY_HW_MCU_NAME                 RIOT_CPU

#define MICROPY_MODULE_FROZEN_STR           (0)

typedef intptr_t mp_int_t;   /* must be pointer size */
typedef uintptr_t mp_uint_t; /* must be pointer size */
typedef long mp_off_t;

#define MICROPY_BEGIN_ATOMIC_SECTION()      irq_disable()
#define MICROPY_END_ATOMIC_SECTION(state)   irq_restore(state)

#define MICROPY_PORT_ROOT_POINTERS          const char *readline_hist[8];

#define MP_STATE_PORT                       MP_STATE_VM
