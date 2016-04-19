/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu
 * @{
 *
 * @file
 * @brief       MSP430Fxyz flashrom functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "irq.h"
#include <stddef.h>
#include <stdint.h>
#include "cpu.h"
#include "irq.h"

uint8_t ie1, ie2;

static uint8_t prepare(void);
static void finish(uint8_t istate);
static inline void busy_wait(void);

/*---------------------------------------------------------------------------*/
uint8_t flashrom_erase(uint8_t *addr)
{
    uint8_t istate = prepare();

    FCTL3 = FWKEY;               /* Lock = 0 */
    busy_wait();
    FCTL1 = FWKEY | ERASE;
    *addr  = 0;                    /* erase Flash segment */
    busy_wait();
    FCTL1 = FWKEY;               /* ERASE = 0 */
    FCTL3 = FWKEY | LOCK;
    finish(istate);
    return 1;
}

uint8_t flashrom_write(uint8_t *dst, const uint8_t *src, size_t size)
{
    unsigned int i;
    FCTL3 = FWKEY;              /* Lock = 0 */
    busy_wait();

    for (i = size; i > 0; i--) {
        FCTL1 = FWKEY | WRT;
        *(dst++) = *(src++);                /* program Flash word */

        while (!(FCTL3 & WAIT)) {
            nop();
        }
    }

    busy_wait();
    FCTL1 = FWKEY;              /* WRT = 0 */
    FCTL3 = FWKEY | LOCK;              /* Lock = 1 */
    return 1;
}

/*---------------------------------------------------------------------------*/
static uint8_t prepare(void)
{
    uint8_t istate;

    /* Disable all interrupts. */

    /* Clear interrupt flag1. */
    IFG1 = 0;

    /* DCO(SMCLK) is 2,4576MHz, /6 = 409600 Hz
     select SMCLK for flash timing, divider 4+1 */
    FCTL2 = FWKEY | FSSEL_3 | FN2 | FN0;

    /* disable all interrupts to protect CPU
     during programming from system crash */
    istate = irq_disable();

    /* disable all NMI-Interrupt sources */
    ie1 = IE1;
    ie2 = IE2;
    IE1 = 0x00;
    IE2 = 0x00;
    return istate;
}
/*---------------------------------------------------------------------------*/
void finish(uint8_t istate)
{
    /* Enable interrupts. */
    IE1 = ie1;
    IE2 = ie2;
    irq_restore(istate);
}

static inline void busy_wait(void)
{
    /* Wait for BUSY = 0, not needed unless run from RAM */
    while (FCTL3 & 0x0001) {
        nop();
    }
}
