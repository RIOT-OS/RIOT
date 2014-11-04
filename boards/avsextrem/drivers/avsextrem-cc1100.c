/*
 * avsextrem-cc1100.c - CC100 Transceiver Driver for the AVSEXTREM-BOARD.
 * Copyright (C) 2013 Heiko Will <hwill@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @file
 * @ingroup LPC2387
 * @brief   CC1100 LPC2387 dependend functions
 *
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @version     $Revision: 1781 $
 *
 * @note        $Id: avsextrem-cc1100.c 1781 2013-08-14 13:39:36Z kasmi $
 */

#include <stdio.h>
#include <stddef.h>
// cpu
#include "cpu.h"
// sys
#include "cc110x_legacy.h"
#include "gpioint.h"

#define CC1100_GDO0   (FIO2PIN & BIT6)  // read serial I/O (GDO0)
#define CC1100_GDO1   (FIO0PIN & BIT8)  // read serial I/O (GDO1)
#define CC1100_GDO2   (FIO0PIN & BIT28) // read serial I/O (GDO2)

#define SPI_TX_EMPTY  (SSP1SR & SSPSR_TFE)
#define SPI_BUSY      (SSP1SR & SSPSR_BSY)
#define SPI_RX_AVAIL  (SSP1SR & SSPSR_RNE)

#define CC1100_GDO1_LOW_RETRY   (100)   // max. retries for GDO1 to go low
#define CC1100_GDO1_LOW_COUNT   (2700)  // loop count (timeout ~ 500 us) to wait
// for GDO1 to go low when CS low

//#define DEBUG
#ifdef DEBUG

static unsigned long time_value;

static void set_time(void)
{
    time_value = 0;
}

static int test_time(int code)
{
    time_value++;

    if (time_value > 10000000) {
        printf("CC1100 SPI alarm: %d!\n", code);
        time_value = 0;
        return 1;
    }

    return 0;
}
#endif

int cc110x_get_gdo0(void)
{
    return CC1100_GDO0;
}

int cc110x_get_gdo1(void)
{
    return CC1100_GDO1;
}

int cc110x_get_gdo2(void)
{
    return CC1100_GDO2;
}

void cc110x_spi_init(void)
{
    // configure chip-select
    FIO0DIR |= BIT6;
    FIO0SET = BIT6;

    // Power
    PCONP |= PCSSP1;    // Enable power for SSP1 (default is on)

    // PIN Setup
    PINSEL0 |= BIT15;   // Set CLK function to SSP1
    PINSEL0 &= ~BIT14;
    PINSEL0 |= BIT17 ;  // Set MISO function to SSP1
    PINSEL0 &= ~BIT16;
    PINSEL0 |= BIT19;   // Set MOSI function to SSP1
    PINSEL0 &= ~BIT18;
    // Interface Setup
    SSP1CR0 = 7;

    // Clock Setup
    uint32_t pclksel;
    uint32_t cpsr;
    lpc2387_pclk_scale(F_CPU / 1000, 6000, &pclksel, &cpsr);
    PCLKSEL0 &= ~(BIT21 | BIT20);   // CCLK to PCLK divider
    PCLKSEL0 |= pclksel << 20;
    SSP1CPSR = cpsr;

    // Enable
    SSP1CR1 |= BIT1;    // SSP-Enable
    int dummy;

    // Clear RxFIFO:
    while (SPI_RX_AVAIL) {   // while RNE (Receive FIFO Not Empty)...
        dummy = SSP1DR;      // read data
    }

    /* to suppress unused-but-set-variable */
    (void) dummy;
}

uint8_t
cc110x_txrx(uint8_t c)
{
    uint8_t result;
    SSP1DR = c;
#ifdef DEBUG
    set_time();
#endif

    while (!SPI_TX_EMPTY) {
#ifdef DEBUG
        test_time(0);
#endif
    }

#ifdef DEBUG
    set_time();
#endif

    while (SPI_BUSY) {
#ifdef DEBUG
        test_time(1);
#endif
    }

#ifdef DEBUG
    set_time();
#endif

    while (!SPI_RX_AVAIL) {
#ifdef DEBUG
        test_time(2);
#endif
    }

    result = (uint8_t)SSP1DR;
    return result;
}

void cc110x_spi_cs(void)
{
    FIO0CLR = BIT6;
}

void
cc110x_spi_select(void)
{
    volatile int retry_count = 0;
    volatile int abort_count;
    // Switch to GDO mode input
    PINSEL0 &= ~(BIT17 + BIT16);   // Set MISO function to GPIO
    FIO0DIR &= ~BIT8;
cs_low:
    // CS to low
    abort_count = 0;
    FIO0CLR = BIT6;
    // Wait for SO to go low (voltage regulator
    // has stabilized and the crystal is running)
loop:
    asm volatile("nop");

    if (CC1100_GDO1) {
        abort_count++;

        if (abort_count > CC1100_GDO1_LOW_COUNT) {
            retry_count++;

            if (retry_count > CC1100_GDO1_LOW_RETRY) {
                puts("[CC1100 SPI] fatal error\n");
                goto final;
            }

            FIO0SET = BIT6;   // CS to high
            goto cs_low;      // try again
        }

        goto loop;
    }

final:
    // Switch to SPI mode
    PINSEL0 |= BIT17;   // Set MISO function to SSP1
    PINSEL0 &= ~BIT16;
}

void
cc110x_spi_unselect(void)
{
    FIO0SET = BIT6;
}

void cc110x_gdo2_disable(void)
{
    gpioint_set(0, BIT28, GPIOINT_DISABLE, NULL);
}

void cc110x_gdo2_enable(void)
{
    gpioint_set(0, BIT28, GPIOINT_FALLING_EDGE, &cc110x_gdo2_irq);
}

void cc110x_before_send(void)
{
    // Disable GDO2 interrupt before sending packet
    cc110x_gdo2_disable();
}

void cc110x_after_send(void)
{
    // Enable GDO2 interrupt after sending packet
    cc110x_gdo2_enable();
}

void cc110x_gdo0_enable(void)
{
    gpioint_set(2, BIT6, GPIOINT_RISING_EDGE, &cc110x_gdo0_irq);
}

void cc110x_gdo0_disable(void)
{
    gpioint_set(2, BIT6, GPIOINT_DISABLE, NULL);
}

void cc110x_init_interrupts(void)
{
    // Enable external interrupt on low edge (for GDO2)
    FIO0DIR &= ~BIT28;
    cc110x_gdo2_enable();
    // Enable external interrupt on low edge (for GDO0)
    FIO2DIR &= ~BIT6;
}
