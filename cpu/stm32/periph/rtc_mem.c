/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 * @file
 * @brief       Low-level RTC backup memory implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/rtc_mem.h"

#if defined(RTC_BKP31R) || defined(TAMP_BKP31R)
#define RTC_MEM_SIZE    32
#elif defined(RTC_BKP30R) || defined(TAMP_BKP30R)
#define RTC_MEM_SIZE    31
#elif defined(RTC_BKP29R) || defined(TAMP_BKP29R)
#define RTC_MEM_SIZE    30
#elif defined(RTC_BKP28R) || defined(TAMP_BKP28R)
#define RTC_MEM_SIZE    29
#elif defined(RTC_BKP27R) || defined(TAMP_BKP27R)
#define RTC_MEM_SIZE    28
#elif defined(RTC_BKP26R) || defined(TAMP_BKP26R)
#define RTC_MEM_SIZE    27
#elif defined(RTC_BKP25R) || defined(TAMP_BKP25R)
#define RTC_MEM_SIZE    26
#elif defined(RTC_BKP24R) || defined(TAMP_BKP24R)
#define RTC_MEM_SIZE    25
#elif defined(RTC_BKP23R) || defined(TAMP_BKP23R)
#define RTC_MEM_SIZE    24
#elif defined(RTC_BKP22R) || defined(TAMP_BKP22R)
#define RTC_MEM_SIZE    23
#elif defined(RTC_BKP21R) || defined(TAMP_BKP21R)
#define RTC_MEM_SIZE    22
#elif defined(RTC_BKP20R) || defined(TAMP_BKP20R)
#define RTC_MEM_SIZE    21
#elif defined(RTC_BKP19R) || defined(TAMP_BKP19R)
#define RTC_MEM_SIZE    20
#elif defined(RTC_BKP18R) || defined(TAMP_BKP18R)
#define RTC_MEM_SIZE    19
#elif defined(RTC_BKP17R) || defined(TAMP_BKP17R)
#define RTC_MEM_SIZE    18
#elif defined(RTC_BKP16R) || defined(TAMP_BKP16R)
#define RTC_MEM_SIZE    17
#elif defined(RTC_BKP15R) || defined(TAMP_BKP15R)
#define RTC_MEM_SIZE    16
#elif defined(RTC_BKP14R) || defined(TAMP_BKP14R)
#define RTC_MEM_SIZE    15
#elif defined(RTC_BKP13R) || defined(TAMP_BKP13R)
#define RTC_MEM_SIZE    14
#elif defined(RTC_BKP12R) || defined(TAMP_BKP12R)
#define RTC_MEM_SIZE    13
#elif defined(RTC_BKP11R) || defined(TAMP_BKP11R)
#define RTC_MEM_SIZE    12
#elif defined(RTC_BKP10R) || defined(TAMP_BKP10R)
#define RTC_MEM_SIZE    11
#elif defined(RTC_BKP9R) || defined(TAMP_BKP9R)
#define RTC_MEM_SIZE    10
#elif defined(RTC_BKP8R) || defined(TAMP_BKP8R)
#define RTC_MEM_SIZE    9
#elif defined(RTC_BKP7R) || defined(TAMP_BKP7R)
#define RTC_MEM_SIZE    8
#elif defined(RTC_BKP6R) || defined(TAMP_BKP6R)
#define RTC_MEM_SIZE    8
#elif defined(RTC_BKP5R) || defined(TAMP_BKP5R)
#define RTC_MEM_SIZE    6
#elif defined(RTC_BKP4R) || defined(TAMP_BKP4R)
#define RTC_MEM_SIZE    5
#elif defined(RTC_BKP3R) || defined(TAMP_BKP3R)
#define RTC_MEM_SIZE    4
#elif defined(RTC_BKP2R) || defined(TAMP_BKP2R)
#define RTC_MEM_SIZE    3
#elif defined(RTC_BKP1R) || defined(TAMP_BKP1R)
#define RTC_MEM_SIZE    2
#elif defined(RTC_BKP0R) || defined(TAMP_BKP0R)
#define RTC_MEM_SIZE    1
#else
#define RTC_MEM_SIZE    0
#endif

extern void rtc_lock(void);
extern void rtc_unlock(void);

size_t rtc_mem_size(void)
{
    return RTC_MEM_SIZE * __SIZEOF_POINTER__;
}

void rtc_mem_write(unsigned offset, const void *data, size_t len)
{
    if (offset + len > rtc_mem_size()) {
        return;
    }

    const uint8_t *in = data;

#if defined(TAMP_BKP0R)
    volatile uint32_t *rtc_regs = &TAMP->BKP0R + (offset / __SIZEOF_POINTER__);
#else
    volatile uint32_t *rtc_regs = &RTC->BKP0R + (offset / __SIZEOF_POINTER__);
#endif

    offset %= __SIZEOF_POINTER__;

    rtc_unlock();

    while (len) {
        unsigned to_copy = (len >= __SIZEOF_POINTER__) ?
                           __SIZEOF_POINTER__ - offset : len;
        uint32_t tmp = *rtc_regs;
        memcpy(((uint8_t *)&tmp) + offset, in, to_copy);
        offset = 0;
        *rtc_regs++ = tmp;
        len -= to_copy;
        in += to_copy;
    }

    rtc_lock();
}

void rtc_mem_read(unsigned offset, void *data, size_t len)
{
    if (offset + len > rtc_mem_size()) {
        return;
    }

    uint8_t *out = (uint8_t *)data;

#if defined(TAMP_BKP0R)
    volatile uint32_t *rtc_regs = &TAMP->BKP0R + (offset / __SIZEOF_POINTER__);
#else
    volatile uint32_t *rtc_regs = &RTC->BKP0R + (offset / __SIZEOF_POINTER__);
#endif

    offset %= __SIZEOF_POINTER__;

    while (len) {
        unsigned to_copy = (len >= __SIZEOF_POINTER__) ?
                           __SIZEOF_POINTER__ - offset : len;
        uint32_t tmp = *rtc_regs++;
        memcpy(out, ((uint8_t *)&tmp) + offset, to_copy);
        offset = 0;
        len -= to_copy;
        out += to_copy;
    }
}
