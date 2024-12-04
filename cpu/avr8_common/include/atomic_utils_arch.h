/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *               2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_avr8_common
 *
 * @{
 *
 * @file
 * @brief       Implementation of fast atomic utility functions
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifndef ATOMIC_UTILS_ARCH_H
#define ATOMIC_UTILS_ARCH_H
#ifndef DOXYGEN

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* clang provides no built-in atomic access to regular variables */
#ifndef __clang__

#define HAS_ATOMIC_LOAD_U8
static inline uint8_t atomic_load_u8(const volatile uint8_t *var)
{
    return __atomic_load_1(var, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U8
static inline void atomic_store_u8(volatile uint8_t *dest, uint8_t val)
{
    __atomic_store_1(dest, val, __ATOMIC_SEQ_CST);
}

#endif /* __clang__ */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
#endif /* ATOMIC_UTILS_ARCH_H */
