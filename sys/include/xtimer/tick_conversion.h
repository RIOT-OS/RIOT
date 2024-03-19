/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_xtimer
 *
 * @{
 * @file
 * @brief   xtimer tick <-> seconds conversions for different values of XTIMER_HZ
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef XTIMER_TICK_CONVERSION_H
#define XTIMER_TICK_CONVERSION_H

#ifndef XTIMER_H
#error "Do not include this file directly! Use xtimer.h instead"
#endif

#include "div.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Some optimizations for common timer frequencies */
#if (XTIMER_SHIFT != 0)
#if (XTIMER_HZ % 15625 != 0)
#error XTIMER_HZ must be a multiple of 15625 (5^6) when using XTIMER_SHIFT
#endif
#if (XTIMER_HZ > 1000000ul)
#if (XTIMER_HZ != (1000000ul << XTIMER_SHIFT))
#error XTIMER_HZ != (1000000ul << XTIMER_SHIFT)
#endif
/* XTIMER_HZ is a power-of-two multiple of 1 MHz */
/* e.g. cc2538 uses a 16 MHz timer */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t usec) {
    return (usec << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t usec) {
    return (usec << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks) {
    return (ticks >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks) {
    return (ticks >> XTIMER_SHIFT); /* divide by power of two */
}

#else /* !(XTIMER_HZ > 1000000ul) */
#if ((XTIMER_HZ << XTIMER_SHIFT) != 1000000ul)
#error (XTIMER_HZ << XTIMER_SHIFT) != 1000000ul
#endif
/* 1 MHz is a power-of-two multiple of XTIMER_HZ */
/* e.g. ATmega2560 uses a 250 kHz timer */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t usec) {
    return (usec >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t usec) {
    return (usec >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks) {
    return (ticks << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks) {
    return (ticks << XTIMER_SHIFT); /* multiply by power of two */
}
#endif /* defined(XTIMER_SHIFT) && (XTIMER_SHIFT != 0) */
#elif (XTIMER_HZ == (1000000ul)) || defined(MODULE_XTIMER_ON_ZTIMER)
/* This is the most straightforward as the xtimer API is based around
 * microseconds for representing time values. */
static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks) {
    return ticks; /* no-op */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks) {
    return ticks; /* no-op */
}

static inline uint32_t _xtimer_ticks_from_usec(uint32_t usec) {
    return usec; /* no-op */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t usec) {
    return usec; /* no-op */
}

#elif XTIMER_HZ == (32768ul)
/* This is a common frequency for RTC crystals. We use the fact that the
 * greatest common divisor between 32768 and 1000000 is 64, so instead of
 * multiplying by the fraction (32768 / 1000000), we will instead use
 * (512 / 15625), which reduces the truncation caused by the integer widths */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t usec) {
    return div_u32_by_15625div512(usec);
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t usec) {
    return div_u64_by_15625div512(usec);
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks) {
    /* return (usec * 15625) / 512; */
    /* Using 64 bit multiplication to avoid truncating the top 9 bits */
    uint64_t usec = (uint64_t)ticks * 15625ul;
    return (usec >> 9); /* equivalent to (usec / 512) */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks) {
    /* return (usec * 15625) / 512; */
    uint64_t usec = (uint64_t)ticks * 15625ul;
    return (usec >> 9); /* equivalent to (usec / 512) */
}

#else
/* No matching implementation found, try to give meaningful error messages */
#if ((XTIMER_HZ % 15625) == 0)
#error Unsupported hardware timer frequency (XTIMER_HZ), missing XTIMER_SHIFT in board.h? See xtimer.h documentation for more info
#else
#error Unknown hardware timer frequency (XTIMER_HZ), check board.h and/or add an implementation in sys/include/xtimer/tick_conversion.h
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* XTIMER_TICK_CONVERSION_H */
/** @} */
