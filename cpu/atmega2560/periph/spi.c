/*
 * Copyright (C) 2015 Daniel Amkaer Sorensen
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega2560
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Daniel Amkaer Sorensen <daniel.amkaer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief   Extract BR0, BR1 and SPI2X bits from speed value
 * @{
 */
#define SPEED_MASK          (0x3)
#define S2X_SHIFT           (2)
/** @} */

static mutex_t lock = MUTEX_INIT;

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    /* make sure device is valid (there is only one...) */
    if (dev != 0) {
        return -1;
    }

    /* the pin configuration for this CPU is fixed:
     * - PB3: MISO (configure as input - done automatically)
     * - PB2: MOSI (configure as output)
     * - PB1: SCK  (configure as output)
     * - PB0: SS   (configure as output, but unused)
     *
     * The SS pin must be configured as output for the SPI device to work as
     * master correctly, though we do not use it for now (as we handle the chip
     * select externally for now)
     */
    DDRB |= ((1 << DDB2) | (1 << DDB1) | (1 << DDB0));

    /* make sure the SPI is not powered off */
    PRR0 &= ~(1 << PRSPI);

    /* configure as master, with given mode and clock */
    SPSR = (speed >> S2X_SHIFT);
    SPCR = ((1 << SPE) | (1 << MSTR) | conf | (speed & SPEED_MASK));

    /* clear interrupt flag */
    (void)SPSR;
    (void)SPDR;

    return 0;
}

int spi_acquire(spi_t dev)
{
    mutex_lock(&lock);
    return 0;
}

int spi_release(spi_t dev)
{
    mutex_unlock(&lock);
    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data))
{
    (void) dev;
    (void) conf;
    (void) cb;

    /* not implemented */
    return -1;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    (void)dev;
    (void)reset_val;

    /* not implemented */
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    return spi_transfer_bytes(dev, &out, in, 1);
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++) {
        char tmp = (out) ? out[i] : 0;
        SPDR = tmp;
        while (!(SPSR & (1 << SPIF))) {}
        tmp = SPDR;
        if (in) {
            in[i] = tmp;
        }
    }

    return (int)length;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    spi_transfer_bytes(dev, (char *)&reg, NULL, 1);
    return spi_transfer_bytes(dev, &out, in, 1);
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    spi_transfer_bytes(dev, (char *)&reg, NULL, 1);
    return spi_transfer_bytes(dev, out, in, length);
}

void spi_poweron(spi_t dev)
{
    SPCR |= (1 << SPE);
}

void spi_poweroff(spi_t dev)
{
    SPCR &= ~(1 << SPE);
}

#endif /* SPI_NUMOF */
