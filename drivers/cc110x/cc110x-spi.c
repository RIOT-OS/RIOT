/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       TI Chipcon CC110x SPI driver
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @}
 */

#include <stdio.h>

#include "cc110x.h"
#include "cc110x-internal.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "hwtimer.h"
#include "irq.h"

/**********************************************************************
 *                      CC110x SPI access
 **********************************************************************/

void cc110x_cs(void)
{
    volatile int retry_count = 0;
    /* Switch MISO/GDO1 to GPIO input mode */
    gpio_init(CC110X_GDO1, GPIO_DIR_IN, GPIO_NOPULL);
    /* CS to low */
    gpio_clear(CC110X_CS);
    /* Wait for SO to go low (voltage regulator
     * has stabilized and the crystal is running) */
    while (gpio_read(CC110X_GDO1)) {
        /* Wait ~500us and try again */
        hwtimer_wait(CS_SO_WAIT_TIME);

        if (gpio_read(CC110X_GDO1)) {
            retry_count++;

            if (retry_count > CC1100_GDO1_LOW_RETRY) {
                puts("[CC1100 SPI] fatal error\n");
                break;
            }

            gpio_set(CC110X_CS);
            gpio_clear(CC110X_CS);
        }
    }
    /* Switch MISO/GDO1 to SPI mode */
    spi_conf_pins(CC110X_SPI);
}

void cc110x_writeburst_reg(uint8_t addr, char *src, uint8_t count)
{
    unsigned int cpsr;
    spi_acquire(CC110X_SPI);
    cpsr = disableIRQ();
    cc110x_cs();
    spi_transfer_regs(CC110X_SPI, addr | CC1100_WRITE_BURST, src, 0, count);
    gpio_set(CC110X_CS);
    restoreIRQ(cpsr);
    spi_release(CC110X_SPI);
}

void cc110x_readburst_reg(uint8_t addr, char *buffer, uint8_t count)
{
    int i = 0;
    unsigned int cpsr;
    spi_acquire(CC110X_SPI);
    cpsr = disableIRQ();
    cc110x_cs();
    spi_transfer_byte(CC110X_SPI, addr | CC1100_READ_BURST, 0);
    while (i < count) {
        spi_transfer_byte(CC110X_SPI, CC1100_NOBYTE, &buffer[i]);
        i++;
    }
    gpio_set(CC110X_CS);
    restoreIRQ(cpsr);
    spi_release(CC110X_SPI);
}

void cc110x_write_reg(uint8_t addr, uint8_t value)
{
    unsigned int cpsr;
    spi_acquire(CC110X_SPI);
    cpsr = disableIRQ();
    cc110x_cs();
    spi_transfer_reg(CC110X_SPI, addr, value, 0);
    gpio_set(CC110X_CS);
    restoreIRQ(cpsr);
    spi_release(CC110X_SPI);
}

uint8_t cc110x_read_reg(uint8_t addr)
{
    char result;
    unsigned int cpsr;
    spi_acquire(CC110X_SPI);
    cpsr = disableIRQ();
    cc110x_cs();
    spi_transfer_reg(CC110X_SPI, addr | CC1100_READ_SINGLE, CC1100_NOBYTE, &result);
    gpio_set(CC110X_CS);
    restoreIRQ(cpsr);
    spi_release(CC110X_SPI);
    return (uint8_t) result;
}

uint8_t cc110x_read_status(uint8_t addr)
{
    char result;
    unsigned int cpsr;
    spi_acquire(CC110X_SPI);
    cpsr = disableIRQ();
    cc110x_cs();
    spi_transfer_reg(CC110X_SPI, addr | CC1100_READ_BURST, CC1100_NOBYTE, &result);
    gpio_set(CC110X_CS);
    restoreIRQ(cpsr);
    spi_release(CC110X_SPI);
    return (uint8_t) result;
}

uint8_t cc110x_strobe(uint8_t c)
{
    char result;
    unsigned int cpsr;
    spi_acquire(CC110X_SPI);
    cpsr = disableIRQ();
    cc110x_cs();
    spi_transfer_byte(CC110X_SPI, c, &result);
    gpio_set(CC110X_CS);
    restoreIRQ(cpsr);
    spi_release(CC110X_SPI);
    return (uint8_t) result;
}
