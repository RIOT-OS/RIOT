/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_msp430
 *
 * @{
 *
 * @file
 * @brief       Implementation of fast atomic utility functions
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

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

#define HAS_ATOMIC_LOAD_U16
static inline uint16_t atomic_load_u16(const volatile uint16_t *var)
{
    return __atomic_load_2(var, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U8
static inline void atomic_store_u8(volatile uint8_t *dest, uint8_t val)
{
    __atomic_store_1(dest, val, __ATOMIC_SEQ_CST);
}

#define HAS_ATOMIC_STORE_U16
static inline void atomic_store_u16(volatile uint16_t *dest, uint16_t val)
{
    __atomic_store_2(dest, val, __ATOMIC_SEQ_CST);
}

#endif /* __clang__ */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
/** @} */
