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

static inline int _unlock(void)
{
    int state;
    state = irq_disable();
    FCTL3 = FWKEY;
    while (FCTL3 & BUSY) {}
    return state;
}

static inline void _lock(int state)
{
    FCTL3 = (FWKEY | LOCK);
    irq_restore(state);
}

static inline void _erase(uint16_t *page_addr)
{
    /* disable interrupts and unlock flash */
    int state = _unlock();

    /* erase page */
    FCTL1 = (FWKEY | ERASE);
    *page_addr = 0;
    while (FCTL3 & BUSY) {}

    /* lock flash and re-enable interrupts */
    _lock(state);
}

void flashpage_write_raw(void *target_addr, const void *data, size_t len)
{
    /* assert multiples of FLASHPAGE_RAW_BLOCKSIZE are written and no less of
       that length. */
    assert(!(len % FLASHPAGE_RAW_BLOCKSIZE));

    /* ensure writes are aligned */
    assert(!(((unsigned)target_addr % FLASHPAGE_RAW_ALIGNMENT) ||
            ((unsigned)data % FLASHPAGE_RAW_ALIGNMENT)));

    /* ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF) - 1));

    uint8_t *page_addr = target_addr;
    const uint8_t *data_addr = data;

   /* disable interrupts and unlock flash */
    int state = _unlock();

   /* enable write access, and write*/
    FCTL1 = (FWKEY | WRT);
    for (unsigned i = 0; i < len; i++) {
        *(page_addr++) = *(data_addr++);
        while (!(FCTL3 & WAIT)) {}
    }
   /* disable write access */
    FCTL1 = (FWKEY);

    /* lock flash and re-enable interrupts */
    _lock(state);
}

void flashpage_write(int page, const void *data)
{
    assert((unsigned) page < FLASHPAGE_NUMOF);

    uint16_t *page_addr = (uint16_t *)flashpage_addr(page);

    /* erase page */
    _erase(page_addr);

    /* write page */
    if (data != NULL) {
        flashpage_write_raw(page_addr, data, FLASHPAGE_SIZE);
    }
}
