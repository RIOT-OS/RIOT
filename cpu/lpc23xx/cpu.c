/*
 * Copyright (C) 2013, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc23xx
 * @{
 */

#include <stdint.h>
#include "cpu.h"
#include "kernel_init.h"
#include "irq.h"
#include "VIC.h"

#include "stdio_base.h"
#include "periph/init.h"

void lpc23xx_pclk_scale(uint32_t source, uint32_t target, uint32_t *pclksel, uint32_t *prescale)
{
    uint32_t pclkdiv;
    *prescale = source / target;

    if ((*prescale % 16) == 0) {
        *pclksel = 3;
        pclkdiv = 8;
    }
    else if ((*prescale % 8) == 0) {
        *pclksel = 0;
        pclkdiv = 4;
    }
    else if ((*prescale % 4) == 0) {
        *pclksel = 2;
        pclkdiv = 2;
    }
    else {
        *pclksel = 1;
        pclkdiv = 1;
    }

    *prescale /= pclkdiv;

    if (*prescale % 2) {
        (*prescale)++;
    }
}

void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale)
{
    uint32_t pclksel;

    lpc23xx_pclk_scale(source, target, &pclksel, prescale);

    PCLKSEL0 = (PCLKSEL0 & ~(BIT2 | BIT3)) | (pclksel << 2);    /* timer 0 */
    PCLKSEL0 = (PCLKSEL0 & ~(BIT4 | BIT5)) | (pclksel << 4);    /* timer 1 */
    PCLKSEL1 = (PCLKSEL1 & ~(BIT12 | BIT13)) | (pclksel << 12); /* timer 2 */
}

/******************************************************************************
** Function name:       install_irq
**
** Descriptions:        Install interrupt handler
** parameters:          Interrupt number, interrupt handler address,
**                      interrupt priority
** Returned value:      true or false, return false if IntNum is out of range
**
******************************************************************************/
#define VIC_BASE_ADDR   0xFFFFF000

bool install_irq(int IntNumber, void (*HandlerAddr)(void), int Priority)
{
    VICIntEnClr = 1 << IntNumber;   /* Disable Interrupt */

    if (IntNumber >= VIC_SIZE) {
        return (false);
    }
    else {
        /* find first un-assigned VIC address for the handler */
        int *vect_addr = (int *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + IntNumber * 4);
        int *vect_cntl = (int *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + IntNumber * 4);
        *vect_addr = (int)HandlerAddr;  /* set interrupt vector */
        *vect_cntl = Priority;
        VICIntEnable = 1 << IntNumber;  /* Enable Interrupt */
        return(true);
    }
}

void arm_reset(void)
{
    /*
     * We abuse the watchdog timer for a reset.
     */
    irq_disable();
    /* Set the watchdog timeout constant to 0xFFFF */
    WDTC   = 0xFFFF;
    /*
     * Enable watchdog interrupt and enable reset on watchdog timeout.
     * (The reset on watchdog timeout flag is ignored, if interrupt on watchdog
     * timeout is not set. Thus, we set both. The reset takes precedence over
     * the interrupt, anyway.)
     */
    WDMOD  = 0x03;
    /*
     * Feed the watchdog by writing 0xAA followed by 0x55:
     * Reload the watchdog timer with the value in WDTC (0xFFFF)
     */
    WDFEED = 0xAA;
    WDFEED = 0x55;
    /* Wait for the watchdog timer to expire, thus performing a reset */
    while(1) {}
}

/**
 * @brief Initialize the CPU, set IRQ priorities, clocks
 */
void cpu_init(void)
{
    extern void board_init(void);

    /* configure CPU clock */
    cpu_init_clks();

    /* set up GPIOs */
    gpio_init_ports();

    /* board specific setup of i/o pins */
    board_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}

/* RSIR will only have POR bit set even when waking up from Deep Power Down
 * Use signature in battery RAM to discriminate between Deep Power Down and POR
 */
bool cpu_backup_ram_is_initialized(void)
{
    static char signature[] __attribute__((section(".backup.data"))) = {
        'R', 'I', 'O', 'T'
    };

    /* Only in case when a reset occurs and the POR = 0, the BODR bit
     * indicates if the V_DD (3V3) voltage was below 2.6 V or not.
     */
    if ((RSIR & (RSIR_BODR | RSIR_POR)) == (RSIR_BODR | RSIR_POR)) {
        RSIR |= RSIR_BODR;
    }

    /* external reset */
    if (RSIR & RSIR_EXTR) {
        return false;
    }

    if (signature[0] != 'R') {
        return false;
    }

    if (signature[1] != 'I') {
        return false;
    }

    if (signature[2] != 'O') {
        return false;
    }

    if (signature[3] != 'T') {
        return false;
    }

    /* When we wake from Deep Sleep only POR is set, just like in the real
     * POR case. Clear the bit to create a new, distinct state.
     */
    RSIR |= RSIR_POR;

    return true;
}

/** @} */
