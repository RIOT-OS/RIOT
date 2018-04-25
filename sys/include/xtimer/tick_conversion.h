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

/*
 * As tick_conversion.h is included in multiple files ENABLE_DEBUG can not be
 * used as it gets redefined "[-Werror]"
 * To use  the debug print enable module fmt in the project makefile
 *
 * USEMODULE += fmt
 * CFLAGS += -DENABLE_DEBUG_TICKS=1
 * */
#ifndef ENABLE_DEBUG_TICKS
#define ENABLE_DEBUG_TICKS   (0)
#endif
#if ENABLE_DEBUG_TICKS
#ifndef MODULE_FMT
#error "Module fmt is necessary to ENABLE_DEBUG_TICKS set 'USEMODULE += fmt' \
in the makefile."
#endif
#include <stdio.h>
#include <string.h>
#include "fmt.h"
#define DEBUG_PRINT_TICKS_32( CLOCK_HZ, EQUAL, VALUE1, VALUE2, BASE2) ({\
char buf_1[21];                                                         \
char buf_2[21];                                                         \
size_t size = fmt_u32_dec(buf_1, VALUE1);                               \
buf_1[size] = '\0';                                                     \
size = fmt_u32_dec(buf_2, VALUE2);                                      \
buf_2[size] = '\0';                                                     \
printf("32bit %" PRIu32 " "EQUAL" 1MHz : %s "#VALUE1" = %s "BASE2"\n",  \
       (uint32_t)CLOCK_HZ , buf_1, buf_2);                              \
})

#define DEBUG_PRINT_TICKS_64( CLOCK_HZ, EQUAL, VALUE1, VALUE2, BASE2) ({\
char buf_1[21];                                                         \
char buf_2[21];                                                         \
size_t size = fmt_u64_dec(buf_1, VALUE1);                               \
buf_1[size] = '\0';                                                     \
size = fmt_u64_dec(buf_2, VALUE2);                                      \
buf_2[size] = '\0';                                                     \
printf("64bit %" PRIu32 " "EQUAL" 1MHz : %s "#VALUE1" = %s "BASE2"\n",  \
       (uint32_t)CLOCK_HZ , buf_1, buf_2);                              \
})
#else
#define DEBUG_PRINT_TICKS_32(CLOCK_HZ, EQUAL, VALUE1, VALUE2, BASE2) {}
#define DEBUG_PRINT_TICKS_64(CLOCK_HZ, EQUAL, VALUE1, VALUE2, BASE2) {}
#endif

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
/* XTIMER_HZ is a power-of-two multiple of 1 MHz and is bigger than 1000MHz */
/* e.g. cc2538 uses a 16 MHz timer */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t us) {
    DEBUG_PRINT_TICKS_32(XTIMER_HZ, ">", us, us << XTIMER_SHIFT, "ticks");
    return (us << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t us) {
    DEBUG_PRINT_TICKS_64(XTIMER_HZ, ">", us, us << XTIMER_SHIFT, "ticks");
    return (us << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks) {
    DEBUG_PRINT_TICKS_32(XTIMER_HZ, ">", ticks, ticks >> XTIMER_SHIFT, "us");
    return (ticks >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks) {
    DEBUG_PRINT_TICKS_64(XTIMER_HZ, ">", ticks, ticks >> XTIMER_SHIFT, "us");
    return (ticks >> XTIMER_SHIFT); /* divide by power of two */
}

#else /* !(XTIMER_HZ > 1000000ul) */
#if ((XTIMER_HZ << XTIMER_SHIFT) != 1000000ul)
#error (XTIMER_HZ << XTIMER_SHIFT) != 1000000ul
#endif
/* 1 MHz is a power-of-two multiple of XTIMER_HZ and is smaller than 1000MHz */
/* e.g. ATmega2560 uses a 250 kHz timer */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t us) {
    DEBUG_PRINT_TICKS_32(XTIMER_HZ, "<", us, us >> XTIMER_SHIFT, "ticks");
    return (us >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t us) {
    DEBUG_PRINT_TICKS_64(XTIMER_HZ, "<", us, us >> XTIMER_SHIFT, "ticks");
    return (us >> XTIMER_SHIFT); /* divide by power of two */
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks) {
    DEBUG_PRINT_TICKS_32(XTIMER_HZ, "<", ticks, ticks << XTIMER_SHIFT, "us");
    return (ticks << XTIMER_SHIFT); /* multiply by power of two */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks) {
    DEBUG_PRINT_TICKS_64(XTIMER_HZ, "<", ticks, ticks << XTIMER_SHIFT, "us");
    return (ticks << XTIMER_SHIFT); /* multiply by power of two */
}
#endif /* defined(XTIMER_SHIFT) && (XTIMER_SHIFT != 0) */
#elif XTIMER_HZ == (1000000ul)
/* This is the most straightforward as the xtimer API is based around
 * microseconds for representing time values. */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t us) {
    DEBUG_PRINT_TICKS_32(XTIMER_HZ, "=", us, us, "ticks");
    return us; /* no-op */
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t us) {
    DEBUG_PRINT_TICKS_64(XTIMER_HZ, "=", us, us, "ticks");
    return us; /* no-op */
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks) {
    DEBUG_PRINT_TICKS_32(XTIMER_HZ, "=", ticks, ticks, "us");
    return ticks; /* no-op */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks) {
    DEBUG_PRINT_TICKS_64(XTIMER_HZ, "=", ticks, ticks , "us");
    return ticks; /* no-op */
}
#elif XTIMER_HZ == (32768ul)
/* This is a common frequency for RTC crystals. We use the fact that the
 * greatest common divisor between 32768 and 1000000 is 64, so instead of
 * multiplying by the fraction (32768 / 1000000), we will instead use
 * (512 / 15625), which reduces the truncation caused by the integer widths */
static inline uint32_t _xtimer_ticks_from_usec(uint32_t us) {
    DEBUG_PRINT_TICKS_32(XTIMER_HZ, "<", us, div_u64_by_15625div512(us),
            "ticks");
    return div_u32_by_15625div512(us);
}

static inline uint64_t _xtimer_ticks_from_usec64(uint64_t us) {
    DEBUG_PRINT_TICKS_64(XTIMER_HZ, "<", us, div_u64_by_15625div512(us),
            "ticks");
    return div_u64_by_15625div512(us);
}

static inline uint32_t _xtimer_usec_from_ticks(uint32_t ticks) {
    /* return (us * 15625) / 512; */
    /* Using 64 bit multiplication to avoid truncating the top 9 bits */
    uint64_t us = (uint64_t)ticks * 15625ul;
    DEBUG_PRINT_TICKS_32XTIMER_HZ, "<", ticks, (us >> 9) , "us");
    return (us >> 9); /* equivalent to (us / 512) */
}

static inline uint64_t _xtimer_usec_from_ticks64(uint64_t ticks) {
    /* return (us * 15625) / 512; */
    uint64_t us = (uint64_t)ticks * 15625ul;
    DEBUG_PRINT_TICKS_64(XTIMER_HZ, "<", ticks, (us >> 9) , "us");
    return (us >> 9); /* equivalent to (us / 512) */
}

#else
/* No matching implementation found, try to give meaningful error messages */
#if ((XTIMER_HZ % 15625) == 0)
#error "Unsupported hardware timer frequency (XTIMER_HZ), missing XTIMER_SHIFT \
in board.h? See xtimer.h documentation for more info"
#else
#error "Unknown hardware timer frequency (XTIMER_HZ), check board.h and/or add \
an implementation in sys/include/xtimer/tick_conversion.h"
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* XTIMER_TICK_CONVERSION_H */
