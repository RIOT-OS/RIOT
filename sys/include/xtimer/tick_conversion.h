/*
 * Copyright (C) 2016 Eistec AB
 *               2018 Josua Arndt
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
 *
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author  Josua Arndt <jarndt@ias.rwth-aachen.de>
 */
#ifndef XTIMER_TICK_CONVERSION_H
#define XTIMER_TICK_CONVERSION_H

#ifndef XTIMER_H
#error "Do not include this file directly! Use xtimer.h instead"
#endif

#include "div.h"
#include "fmt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * As tick_conversion.h is included in multiple files ENABLE_DEBUG can not be
 * used as it gets redefined "[-Werror]"
 * Activate the debug print and enable module fmt in the project makefile as follow
 *
 * USEMODULE += fmt
 * CFLAGS += -DENABLE_DEBUG_PRINT_TICK_CONVERSION=1
 */
#ifndef ENABLE_DEBUG_PRINT_TICK_CONVERSION
#define ENABLE_DEBUG_PRINT_TICK_CONVERSION   (0)
#endif

#if ENABLE_DEBUG_PRINT_TICK_CONVERSION
#ifndef MODULE_FMT
#error Module fmt is necessary to ENABLE_DEBUG_TICKS
#endif
#endif

inline void debug_print_ticks(uint32_t clock_hz, char *sign, uint64_t from,
                              char *base_from, uint64_t to, char *base_to)
{
    if (ENABLE_DEBUG_PRINT_TICK_CONVERSION) {
        print_str("64bit ");
        print_u32_dec(clock_hz);
        print_str(" ");
        print_str(sign);
        print_str(" 1MHz : ");
        print_u64_dec(from);
        print_str(" ");
        print_str(base_from);
        print_str(" = ");
        print_u64_dec(to);
        print_str(" ");
        print_str(base_to);
        print_str("\n");
    }
}

/* Some optimizations for common timer frequencies */
#if (XTIMER_SHIFT != 0)
#if (XTIMER_HZ % 15625 != 0)
#error XTIMER_HZ must be a multiple of 15625 (5^6) when using XTIMER_SHIFT
#endif
#if (XTIMER_HZ > 1000000ul)
#if (XTIMER_HZ != (1000000ul << XTIMER_SHIFT))
#error XTIMER_HZ != (1000000ul << XTIMER_SHIFT)
#endif
/* XTIMER_HZ is a power-of-two multiple of 1 MHz and is bigger than 1MHz */
/* e.g. cc2538 uses a 16 MHz timer */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t us)
{
    debug_print_ticks(XTIMER_HZ, ">", (uint64_t)us, "us",
                      (uint64_t)(us >> XTIMER_SHIFT), "ticks");
    return (us << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t us)
{
    debug_print_ticks(XTIMER_HZ, ">", us, "us", us >> XTIMER_SHIFT, "ticks");
    return (us << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks)
{
    debug_print_ticks(XTIMER_HZ, ">", (uint64_t)ticks, "ticks",
                      (uint64_t)(ticks >> XTIMER_SHIFT), "us");
    return (ticks >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks)
{
    debug_print_ticks(XTIMER_HZ, ">", ticks, "ticks", ticks >> XTIMER_SHIFT, "us");
    return (ticks >> XTIMER_SHIFT); /* divide by power of two */
}

#else /* !(XTIMER_HZ > 1000000ul) */
#if ((XTIMER_HZ << XTIMER_SHIFT) != 1000000ul)
#error (XTIMER_HZ << XTIMER_SHIFT) != 1000000ul
#endif
/* 1 MHz is a power-of-two multiple of XTIMER_HZ and is smaller than 1MHz */
/* e.g. ATmega2560 uses a 250 kHz timer */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t us)
{
    debug_print_ticks(XTIMER_HZ, "<", (uint64_t)us, "us",
                      (uint64_t)(us >> XTIMER_SHIFT), "ticks");
    return (us >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t us)
{
    debug_print_ticks(XTIMER_HZ, "<", us, "us", us >> XTIMER_SHIFT, "ticks");
    return (us >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks)
{
    debug_print_ticks(XTIMER_HZ, "<", (uint64_t)ticks, "ticks",
                      (uint64_t)(ticks << XTIMER_SHIFT), "us");
    return (ticks << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks)
{
    debug_print_ticks(XTIMER_HZ, "<", ticks, "ticks", ticks << XTIMER_SHIFT, "us");
    return (ticks << XTIMER_SHIFT); /* multiply by power of two */
}
#endif /* defined(XTIMER_SHIFT) && (XTIMER_SHIFT != 0) */
#elif XTIMER_HZ == (1000000ul)
/* This is the most straightforward as the xtimer API is based around
 * microseconds for representing time values. */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t us)
{
    debug_print_ticks(XTIMER_HZ, "=", (uint64_t)us, "us", (uint64_t)us, "ticks");
    return us; /* no-op */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t us)
{
    debug_print_ticks(XTIMER_HZ, "=", us, "us", us, "ticks");
    return us; /* no-op */
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks)
{
    debug_print_ticks(XTIMER_HZ, "=", (uint64_t)ticks, "ticks", (uint64_t)ticks, "us");
    return ticks; /* no-op */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks)
{
    debug_print_ticks(XTIMER_HZ, "=", ticks, "ticks", ticks, "us");
    return ticks; /* no-op */
}
#elif XTIMER_HZ == (32768ul)
/* This is a common frequency for RTC crystals. We use the fact that the
 * greatest common divisor between 32768 and 1000000 is 64, so instead of
 * multiplying by the fraction (32768 / 1000000), we will instead use
 * (512 / 15625), which reduces the truncation caused by the integer widths */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t us)
{
    debug_print_ticks(XTIMER_HZ, "<", (uint64_t)us, "us",
                      (uint64_t)div_u64_by_15625div512(us), "ticks");
    return div_u32_by_15625div512(us);
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t us)
{
    debug_print_ticks(XTIMER_HZ, "<", us, "us", div_u64_by_15625div512(us), "ticks");
    return div_u64_by_15625div512(us);
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks)
{
    /* return (us * 15625) / 512; */
    /* Using 64 bit multiplication to avoid truncating the top 9 bits */
    uint64_t us = (uint64_t)ticks * 15625ul;

    debug_print_ticks(XTIMER_HZ, "<", (uint64_t)ticks, "ticks",
                      (uint64_t)(us >> 9), "us");
    return (us >> 9); /* equivalent to (us / 512) */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks)
{
    /* return (us * 15625) / 512; */
    uint64_t us = (uint64_t)ticks * 15625ul;

    debug_print_ticks(XTIMER_HZ, "<", ticks, "ticks", (us >> 9), "us");
    return (us >> 9); /* equivalent to (us / 512) */
}

#else
/* No matching implementation found, try to give meaningful error messages */
#if ((XTIMER_HZ % 15625) == 0)
#error Unsupported hardware timer frequency (XTIMER_HZ).
/* Xtimer is not correctly configured maybe missing XTIMER_SHIFT in board.h?
 * See xtimer.h documentation for more information!
 */
#else
#error Unknown hardware timer frequency (XTIMER_HZ).
/* Check board.h and/or add an implementation in
 * sys/include/xtimer/tick_conversion.h to convert the hardware ticks
 */
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* XTIMER_TICK_CONVERSION_H */
