/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_soft_spi
 * @{
 *
 * @file
 * @brief       Software SPI implementation
 *
 * @author      Markus Blechschmidt <Markus.Blechschmidt@haw-hamburg.de>
 * @author      Peter Kietzmann     <peter.kietzmann@haw-hamburg.de>
 */

#include <stdio.h>
#include <assert.h>

#include "mutex.h"
#include "periph/gpio.h"
#include "xtimer.h"

#include "soft_spi.h"
#include "soft_spi_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef SOFT_SPI_NUMOF
#define SOFT_SPI_NUMOF (0)
#endif

/**
 * @brief   Allocate one lock per SPI device
 */
static mutex_t locks[SOFT_SPI_NUMOF];

void soft_spi_init(soft_spi_t bus)
{
    DEBUG("Soft SPI init\n");

    assert(bus < SOFT_SPI_NUMOF);

    /* initialize device lock */
    mutex_init(&locks[bus]);
    soft_spi_init_pins(bus);
}

void soft_spi_init_pins(soft_spi_t bus)
{
    DEBUG("Soft SPI soft_spi_init_pins\n");

    assert(bus < SOFT_SPI_NUMOF);

    /* mandatory pins */
    assert((GPIO_UNDEF != soft_spi_config[bus].mosi_pin) ||
           (GPIO_UNDEF != soft_spi_config[bus].miso_pin) ||
           (GPIO_UNDEF != soft_spi_config[bus].clk_pin));

    /* check that miso is not mosi is not clk*/
    assert((soft_spi_config[bus].mosi_pin != soft_spi_config[bus].miso_pin) ||
           (soft_spi_config[bus].mosi_pin != soft_spi_config[bus].clk_pin) ||
           (soft_spi_config[bus].miso_pin != soft_spi_config[bus].clk_pin));

    gpio_init(soft_spi_config[bus].clk_pin, GPIO_OUT);
    gpio_init(soft_spi_config[bus].mosi_pin, GPIO_OUT);
    gpio_init(soft_spi_config[bus].miso_pin, GPIO_IN);
}

int soft_spi_init_cs(soft_spi_t bus, soft_spi_cs_t cs)
{
    DEBUG("Soft SPI init CS\n");
    if (bus >= SOFT_SPI_NUMOF) {
        DEBUG("Soft SPI bus not valid\n");
        return SOFT_SPI_NODEV;
    }

    if ((cs == GPIO_UNDEF) || (cs == SOFT_SPI_CS_UNDEF)) {
        DEBUG("Soft SPI chip select line not valid\n");
        return SOFT_SPI_NOCS;
    }
    DEBUG("Soft SPI set user CS line\n");
    gpio_init(cs, GPIO_OUT);
    gpio_set(cs);

    return SOFT_SPI_OK;
}

int soft_spi_acquire(soft_spi_t bus, soft_spi_cs_t cs, soft_spi_mode_t mode,
                     soft_spi_clk_t clk)
{
    (void)cs;
    assert(bus < SOFT_SPI_NUMOF);

    /* lock bus */
    mutex_lock(&locks[bus]);

    if ((mode != SOFT_SPI_MODE_0) && (mode != SOFT_SPI_MODE_1) &&
        (mode != SOFT_SPI_MODE_2) && (mode != SOFT_SPI_MODE_3)) {
        return SOFT_SPI_NOMODE;
    }
    if ((clk != SOFT_SPI_CLK_100KHZ) && (clk != SOFT_SPI_CLK_400KHZ) &&
        (clk != SOFT_SPI_CLK_DEFAULT)) {
        return SOFT_SPI_NOCLK;
    }
    soft_spi_config[bus].soft_spi_mode = mode;
    soft_spi_config[bus].soft_spi_clk = clk;
    switch (mode) {
    case SOFT_SPI_MODE_0:
    case SOFT_SPI_MODE_1:
        /* CPOL=0 :clk idle state pin level is low*/
        gpio_clear(soft_spi_config[bus].clk_pin);
        break;
    case SOFT_SPI_MODE_2:
    case SOFT_SPI_MODE_3:
        /* CPOL=1 :clk idle state pin level is high*/
        gpio_set(soft_spi_config[bus].clk_pin);
        break;
    }
    return SOFT_SPI_OK;
}

void soft_spi_release(soft_spi_t bus)
{
    assert(bus < SOFT_SPI_NUMOF);
    mutex_unlock(&locks[bus]);
}

static uint8_t _transfer_one_byte(soft_spi_t bus, uint8_t out)
{
    int8_t bit = 0, i = 0;

    if (SOFT_SPI_MODE_1 == soft_spi_config[bus].soft_spi_mode ||
        SOFT_SPI_MODE_3 == soft_spi_config[bus].soft_spi_mode) {
        /* CPHA = 1 :Even edge valid*/
        gpio_toggle(soft_spi_config[bus].clk_pin);
    }

    bit = (out & (1 << 7)) >> 7;
    gpio_write(soft_spi_config[bus].mosi_pin, bit);
    for (i = 6; i >= 0; i--) {
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        gpio_toggle(soft_spi_config[bus].clk_pin);
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        gpio_toggle(soft_spi_config[bus].clk_pin);
        bit = (out & (1 << i)) >> i;
        gpio_write(soft_spi_config[bus].mosi_pin, bit);
    }
    xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
    gpio_toggle(soft_spi_config[bus].clk_pin);

    if (SOFT_SPI_MODE_0 == soft_spi_config[bus].soft_spi_mode ||
        SOFT_SPI_MODE_2 == soft_spi_config[bus].soft_spi_mode) {
        /* CPHASE = 1 */
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        gpio_toggle(soft_spi_config[bus].clk_pin);
    }

    return out;
}

static uint8_t _readin_one_byte(soft_spi_t bus)
{
    int8_t i = 0, tmp = 0;
    int bit;

    if (SOFT_SPI_MODE_1 == soft_spi_config[bus].soft_spi_mode ||
        SOFT_SPI_MODE_3 == soft_spi_config[bus].soft_spi_mode) {
        /* CPHA = 1*/
        gpio_toggle(soft_spi_config[bus].clk_pin);
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
    }
    gpio_toggle(soft_spi_config[bus].clk_pin);
    xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
    bit = gpio_read(soft_spi_config[bus].miso_pin);
    tmp |= (!!bit << 7);
    for (i = 6; i >= 0; i--) {
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        gpio_toggle(soft_spi_config[bus].clk_pin);
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        gpio_toggle(soft_spi_config[bus].clk_pin);
        bit = gpio_read(soft_spi_config[bus].miso_pin);
        tmp |= (!!bit << i);
    }

    if (SOFT_SPI_MODE_0 == soft_spi_config[bus].soft_spi_mode ||
        SOFT_SPI_MODE_2 == soft_spi_config[bus].soft_spi_mode) {
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        gpio_toggle(soft_spi_config[bus].clk_pin);
    }

    return tmp;
}

uint8_t soft_spi_transfer_byte(soft_spi_t bus, soft_spi_cs_t cs, bool cont,
                               uint8_t out)
{
    DEBUG("Soft SPI soft_spi_transfer_bytes\n");
    assert(bus < SOFT_SPI_NUMOF);

    uint8_t retval = 0;

    /* activate the given chip select line */
    if ((cs != GPIO_UNDEF) && (cs != SOFT_SPI_CS_UNDEF)) {
        gpio_clear((gpio_t)cs);
    }

    retval = _transfer_one_byte(bus, out);

    if (!cont) {
        if ((cs != GPIO_UNDEF) && (cs != SOFT_SPI_CS_UNDEF)) {
            gpio_set((gpio_t)cs);
        }
    }

    return retval;
}

uint8_t soft_spi_read_byte(soft_spi_t bus, soft_spi_cs_t cs, bool cont)
{
    DEBUG("Soft SPI soft_spi_readin_byte\n");
    assert(bus < SOFT_SPI_NUMOF);

    uint8_t retval = 0;

    /* activate the given chip select line */
    if ((cs != GPIO_UNDEF) && (cs != SOFT_SPI_CS_UNDEF)) {
        gpio_clear((gpio_t)cs);
    }

    retval = _readin_one_byte(bus);
    if (!cont) {
        gpio_set((gpio_t)cs);
    }

    return retval;
}

void soft_spi_transfer_bytes(soft_spi_t bus, soft_spi_cs_t cs, bool cont,
                             const void *out, void *in, size_t len)
{
    DEBUG("Soft SPI soft_spi_transfer_bytes\n");
    assert(bus < SOFT_SPI_NUMOF);

    const uint8_t *outbuf = out;
    uint8_t *inbuf = in;

    if (!inbuf) {
        for (size_t i = 0; i < len - 1; i++) {
            soft_spi_transfer_byte(bus, cs, true, outbuf[i]);
        }
        soft_spi_transfer_byte(bus, cs, cont, outbuf[len - 1]);
    }
    else if (!outbuf) {
        for (size_t i = 0; i < len - 1; i++) {
            inbuf[i] = soft_spi_read_byte(bus, cs, true);
        }

        inbuf[len - 1] = soft_spi_read_byte(bus, cs, cont);
    }
    else {
        for (size_t i = 0; i < len - 1; i++) {
            soft_spi_transfer_byte(bus, cs, true, outbuf[i]);
            inbuf[i] = soft_spi_read_byte(bus, cs, true);
        }
        soft_spi_transfer_byte(bus, cs, cont, outbuf[len - 1]);
        inbuf[len - 1] = soft_spi_read_byte(bus, cs, cont);
    }

}
