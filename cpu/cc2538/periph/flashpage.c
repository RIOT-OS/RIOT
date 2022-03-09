/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Implementation of the peripheral flashpage interface
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "irq.h"
#include "periph/flashpage.h"

#define ENABLE_DEBUG           0
#include "debug.h"

#define FLASH_CTRL_FCTL_BUSY        0x00000080
#define FLASH_CTRL_FCTL_FULL        0x00000040
#define FLASH_CTRL_FCTL_WRITE       0x00000002
#define FLASH_CTRL_FCTL_ERASE       0x00000001
#define FLASH_CTRL_FCTL_CM_MASK     0x0000000C

__attribute__ ((section (".ramfunc")))
static inline void _erase(uint32_t *page_addr)
{
    /* wait for ongoing operations*/
    DEBUG("[flashpage] erase: wait for ongoing operations\n");
    while (FLASH_CTRL_FCTL & FLASH_CTRL_FCTL_BUSY) {}

    /* disable interrupts  */
    int state = irq_disable();

    /* Initialize Flash control register without changing the cache mode.*/
    FLASH_CTRL_FCTL &= FLASH_CTRL_FCTL_CM_MASK;

    /* set page to erase*/
    FLASH_CTRL_FADDR = (uint32_t)page_addr;

    /* starts the write-sequence state machine */
    DEBUG("[flashpage] erase: start erase sequence at %p\n", page_addr);
    FLASH_CTRL_FCTL |= FLASH_CTRL_FCTL_ERASE;

    /* wait erase to complete */
    while (FLASH_CTRL_FCTL & FLASH_CTRL_FCTL_BUSY) {}

    /* re-enable interrupts */
    irq_restore(state);
}

void flashpage_erase(unsigned page)
{
    assert((unsigned) page < FLASHPAGE_NUMOF);

    uint32_t *page_addr = (uint32_t *)flashpage_addr(page);

    _erase(page_addr);
}

__attribute__ ((section (".ramfunc")))
void flashpage_write(void *target_addr, const void *data, size_t len)
{
    /* assert multiples of FLASHPAGE_WRITE_BLOCK_SIZE are written and no less of
       that length. */
    assert(!(len % FLASHPAGE_WRITE_BLOCK_SIZE));

    /* ensure writes are aligned */
    assert(!(((unsigned)target_addr % FLASHPAGE_WRITE_BLOCK_ALIGNMENT) ||
            ((unsigned)data % FLASHPAGE_WRITE_BLOCK_ALIGNMENT)));

    /* ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <=
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)));

    uint32_t *dst = target_addr;
    const uint32_t *data_addr = data;

   /* disable interrupts and unlock flash */
    int state = irq_disable();

    DEBUG("[flashpage_raw] write: to %p \n", dst);
    /* Initialize Flash control register without changing the cache mode.*/
    FLASH_CTRL_FCTL &= FLASH_CTRL_FCTL_CM_MASK;
    /* set start address*/
    FLASH_CTRL_FADDR = (uint32_t) dst;
    /* starts the write-sequence state machine */
    DEBUG("[flashpage_raw] write: now writing the data\n");
    FLASH_CTRL_FCTL |= FLASH_CTRL_FCTL_WRITE;
    for (unsigned i = 0; i < (len / FLASHPAGE_WRITE_BLOCK_SIZE); i++) {
        FLASH_CTRL_FWDATA = (uint32_t) *(data_addr++);
        /* wait for flash operation to complete */
        while (FLASH_CTRL_FCTL & FLASH_CTRL_FCTL_FULL) {}
    }
    /* re-enable interrupts */
    irq_restore(state);
}
