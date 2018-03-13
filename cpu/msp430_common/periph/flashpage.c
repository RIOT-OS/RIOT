/*
 * Copyright (C) 2014 INRIA
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fxyz
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral flashpage interface
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "irq.h"
#include "periph/flashpage.h"

void flashpage_write(int page, const void *data)
{
    assert(page < FLASHPAGE_NUMOF);

    const uint8_t *src = data;
    uint8_t *dst = (uint8_t *)flashpage_addr(page);
    unsigned istate;

    /* disable interrupts and unlock flash */
    istate = irq_disable();
    FCTL3 = FWKEY;
    while (FCTL3 & BUSY) {}

    /* erase page */
    FCTL1 = (FWKEY | ERASE);
    *dst = 0;                   /* erases the page */
    while (FCTL3 & BUSY) {}

    if (data) {
        FCTL1 = (FWKEY | WRT);
        for (unsigned i = 0; i < FLASHPAGE_SIZE; i++) {
            *(dst++) = *(src++);
            while (!(FCTL3 & WAIT)) {}
        }
    }

    /* lock flash and re-enable interrupts */
    FCTL1 = (FWKEY);
    FCTL3 = (FWKEY | LOCK);
    irq_restore(istate);
}
