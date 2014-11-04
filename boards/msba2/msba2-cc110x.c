/*
 * Copyright (C) 2008, Freie Universitaet Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup     LPC2387
 * @brief       CC1100 LPC2387 dependend functions
 *
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 1781 $
 *
 * @note        $Id: msba2-cc110x.c 1781 2010-01-26 13:39:36Z hillebra $
 */

#include <stdio.h>
#include <stddef.h>
/* core */
#include "irq.h"
/* cpu */
#include "cpu.h"
/* drivers  */
#include "cc110x_legacy.h"

#include "gpioint.h"

#define CC1100_GDO0         (FIO0PIN & BIT27)   // read serial I/O (GDO0)
#define CC1100_GDO1         (FIO1PIN & BIT23)   // read serial I/O (GDO1)
#define CC1100_GDO2         (FIO0PIN & BIT28)   // read serial I/O (GDO2)

#define SPI_TX_EMPTY                (SSP0SR & SSPSR_TFE)
#define SPI_BUSY                    (SSP0SR & SSPSR_BSY)
#define SPI_RX_AVAIL                (SSP0SR & SSPSR_RNE)

#define CC1100_GDO1_LOW_RETRY        (100)      // max. retries for GDO1 to go low
#define CC1100_GDO1_LOW_COUNT       (2700)      // loop count (timeout ~ 500 us) to wait
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
    return  CC1100_GDO0;
}

int cc110x_get_gdo1(void)
{
    return  CC1100_GDO1;
}

int cc110x_get_gdo2(void)
{
    return  CC1100_GDO2;
}

void cc110x_spi_init(void)
{
    // configure chip-select
    FIO1DIR |= BIT21;
    FIO1SET = BIT21;

    // Power
    PCONP |= PCSSP0;                                // Enable power for SSP0 (default is on)

    // PIN Setup
    PINSEL3 |= BIT8 + BIT9;                         // Set CLK function to SPI
    PINSEL3 |= BIT14 + BIT15;                       // Set MISO function to SPI
    PINSEL3 |= BIT16 + BIT17;                       // Set MOSI function to SPI

    // Interface Setup
    SSP0CR0 = 7;

    // Clock Setup
    uint32_t pclksel;
    uint32_t cpsr;
    lpc2387_pclk_scale(F_CPU / 1000, 6000, &pclksel, &cpsr);
    PCLKSEL1 &= ~(BIT10 | BIT11);                       // CCLK to PCLK divider
    PCLKSEL1 |= pclksel << 10;
    SSP0CPSR = cpsr;

    // Enable
    SSP0CR1 |= BIT1;                                // SSP-Enable
    int dummy;

    // Clear RxFIFO:
    while (SPI_RX_AVAIL) {                      // while RNE (Receive FIFO Not Empty)...
        dummy = SSP0DR;                         // read data
    }

    /* to suppress unused-but-set-variable */
    (void) dummy;
}

uint8_t cc110x_txrx(uint8_t c)
{
    uint8_t result;
    SSP0DR = c;
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

    result = (uint8_t)SSP0DR;
    return result;
}

void cc110x_spi_cs(void)
{
    FIO1CLR = BIT21;
}

void
cc110x_spi_select(void)
{
    volatile int retry_count = 0;
    volatile int abort_count;
    // Switch to GDO mode input
    PINSEL3 &= ~(BIT14 + BIT15);// Set MISO function to GPIO
    FIO1DIR &= ~BIT23;
cs_low:
    // CS to low
    abort_count = 0;
    FIO1CLR = BIT21;
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

            FIO1SET = BIT21;    // CS to high
            goto cs_low;        // try again
        }

        goto loop;
    }

final:
    // Switch to SPI mode
    PINSEL3 |= (BIT14 + BIT15); // Set MISO function to SPI
}

void
cc110x_spi_unselect(void)
{
    FIO1SET = BIT21;
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
    gpioint_set(0, BIT27, GPIOINT_RISING_EDGE, &cc110x_gdo0_irq);
}

void cc110x_gdo0_disable(void)
{
    gpioint_set(0, BIT27, GPIOINT_DISABLE, NULL);
}

void cc110x_gdo2_disable(void)
{
    gpioint_set(0, BIT28, GPIOINT_DISABLE, NULL);
}

void cc110x_gdo2_enable(void)
{
    gpioint_set(0, BIT28, GPIOINT_FALLING_EDGE, &cc110x_gdo2_irq);
}

void cc110x_init_interrupts(void)
{
    // Enable external interrupt on low edge (for GDO2)
    FIO0DIR &= ~BIT28;
    cc110x_gdo2_enable();
    // Enable external interrupt on low edge (for GDO0)
    FIO0DIR &= ~BIT27;
}
