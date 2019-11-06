/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral flashpage interface
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @note        Based on AVR-libC example code at
 *              https://www.nongnu.org/avr-libc/user-manual/group__avr__boot.html#details
 *
 * @}
 */

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>

#include "cpu.h"
#include "irq.h"
#include "periph/flashpage.h"

void flashpage_write(int page, const void *data)
{
    assert(page < FLASHPAGE_NUMOF);

    uint32_t dst = page * FLASHPAGE_SIZE;
    const uint16_t *src = data;
    unsigned istate;

    /* disable interrupts */
    istate = irq_disable();
    eeprom_busy_wait();

    /* erase page and wait for completion */
    boot_page_erase(dst);
    boot_spm_busy_wait();

    if (data) {
        for (const void *end = src + FLASHPAGE_SIZE; src != end; ++src) {
            boot_page_fill(dst, *src);
            dst += sizeof(*src);
        }
    }

    boot_page_write (page);     /* Store buffer in flash page */
    boot_spm_busy_wait();       /* Wait until the memory is written */

    /* Reenable RWW-section again. We need this if we want to jump back
       to the application after bootloading. */
    boot_rww_enable();

    /* re-enable interrupts (if they were ever enabled) */
    irq_restore(istate);
}

void flashpage_read(int page, void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    uint32_t src = page * FLASHPAGE_SIZE;
    uint16_t *dst = data;

    for (uint32_t end = src + FLASHPAGE_SIZE; src != end; src += sizeof(*dst)) {
        *dst++ = pgm_read_word(src);
    }
}

int flashpage_verify(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    uint32_t src = page * FLASHPAGE_SIZE;
    const uint16_t *dst = data;

    for (uint32_t end = src + FLASHPAGE_SIZE; src != end; src += sizeof(*dst)) {
        if (*dst++ != pgm_read_word(src)) {
            return FLASHPAGE_NOMATCH;
        }
    }

    return FLASHPAGE_OK;
}
