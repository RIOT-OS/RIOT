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

#define ENABLE_DEBUG 0
#include "debug.h"

#define READ_PADDING_BYTE (0x00)
/**
 * @brief   Allocate one lock per SPI device
 */
static mutex_t locks[sizeof soft_spi_config];

static inline bool soft_spi_bus_is_valid(soft_spi_t bus)
{
    unsigned int soft_spi_num = (unsigned int) bus;

    if (ARRAY_SIZE(soft_spi_config) < soft_spi_num) {
        return false;
    }
    return true;
}

void soft_spi_init(soft_spi_t bus)
{
    DEBUG("Soft SPI init\n");

    assert(soft_spi_bus_is_valid(bus));

    /* initialize device lock */
    mutex_init(&locks[bus]);
    soft_spi_init_pins(bus);
}

void soft_spi_init_pins(soft_spi_t bus)
{
    DEBUG("Soft SPI soft_spi_init_pins\n");

    assert(soft_spi_bus_is_valid(bus));

    /* check that miso is not mosi is not clk*/
    assert(!gpio_is_equal(soft_spi_config[bus].mosi_pin, soft_spi_config[bus].miso_pin));
    assert(!gpio_is_equal(soft_spi_config[bus].mosi_pin, soft_spi_config[bus].clk_pin));
    assert(!gpio_is_equal(soft_spi_config[bus].miso_pin, soft_spi_config[bus].clk_pin));
    /* mandatory pins */
    assert(gpio_is_valid(soft_spi_config[bus].mosi_pin) ||
           gpio_is_valid(soft_spi_config[bus].miso_pin));
    assert(gpio_is_valid(soft_spi_config[bus].clk_pin));

    /* initialize clock pin */
    gpio_init(soft_spi_config[bus].clk_pin, GPIO_OUT);
    /* initialize optional pins */
    if (gpio_is_valid(soft_spi_config[bus].mosi_pin)) {
        gpio_init(soft_spi_config[bus].mosi_pin, GPIO_OUT);
        gpio_clear(soft_spi_config[bus].mosi_pin);
    }
    if (gpio_is_valid(soft_spi_config[bus].miso_pin)) {
        gpio_init(soft_spi_config[bus].miso_pin, GPIO_IN);
    }
}

int soft_spi_init_cs(soft_spi_t bus, soft_spi_cs_t cs)
{
    DEBUG("Soft SPI init CS\n");
    if (!soft_spi_bus_is_valid(bus)) {
        DEBUG("Soft SPI bus not valid\n");
        return SOFT_SPI_NODEV;
    }

    if (gpio_is_valid(cs) && !gpio_is_equal(cs, SOFT_SPI_CS_UNDEF)) {
        DEBUG("Soft SPI set user CS line\n");
        gpio_init(cs, GPIO_OUT);
        gpio_set(cs);
    }

    return SOFT_SPI_OK;
}

int soft_spi_acquire(soft_spi_t bus, soft_spi_cs_t cs, soft_spi_mode_t mode, soft_spi_clk_t clk)
{
    (void) cs;
    assert(soft_spi_bus_is_valid(bus));

    /* lock bus */
    mutex_lock(&locks[bus]);

    if ((mode != SOFT_SPI_MODE_0) && (mode != SOFT_SPI_MODE_1) &&
        (mode != SOFT_SPI_MODE_2) && (mode != SOFT_SPI_MODE_3)) {
        return SOFT_SPI_NOMODE;
    }
    soft_spi_config[bus].soft_spi_mode = mode;
    switch (mode) {
        case SOFT_SPI_MODE_0:
        case SOFT_SPI_MODE_1:
            /* CPOL=0 */
            gpio_clear(soft_spi_config[bus].clk_pin);
            break;
        case SOFT_SPI_MODE_2:
        case SOFT_SPI_MODE_3:
            /* CPOL=1 */
            gpio_set(soft_spi_config[bus].clk_pin);
            break;
    }
    soft_spi_config[bus].soft_spi_clk = clk;
    return SOFT_SPI_OK;
}

void soft_spi_release(soft_spi_t bus)
{
    assert(soft_spi_bus_is_valid(bus));
    mutex_unlock(&locks[bus]);
}

static inline uint8_t _transfer_one_byte(soft_spi_t bus, uint8_t out)
{
    uint8_t i = 8;
    if (SOFT_SPI_MODE_1 == soft_spi_config[bus].soft_spi_mode ||
        SOFT_SPI_MODE_3 == soft_spi_config[bus].soft_spi_mode) {
        /* CPHA = 1*/
        gpio_toggle(soft_spi_config[bus].clk_pin);
    }

    do {
        uint8_t bit = out >> 7;
        gpio_write(soft_spi_config[bus].mosi_pin, bit);

        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        gpio_toggle(soft_spi_config[bus].clk_pin);

        out <<= 1; /*shift transfer register*/

        bit = gpio_read(soft_spi_config[bus].miso_pin);
        out = bit ? (out | 0x01) : (out & 0xfe); /*set or delete bit 0*/

        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        --i;
        if (i > 0) {
            gpio_toggle(soft_spi_config[bus].clk_pin);
        }
    } while (i > 0);

    if (SOFT_SPI_MODE_0 == soft_spi_config[bus].soft_spi_mode ||
        SOFT_SPI_MODE_2 == soft_spi_config[bus].soft_spi_mode) {
        /* CPHA = 0 */
        xtimer_nanosleep(soft_spi_config[bus].soft_spi_clk);
        gpio_toggle(soft_spi_config[bus].clk_pin);
    }

    return out;
}

uint8_t soft_spi_transfer_byte(soft_spi_t bus, soft_spi_cs_t cs, bool cont, uint8_t out)
{
    DEBUG("Soft SPI soft_spi_transfer_bytes\n");
    assert(soft_spi_bus_is_valid(bus));

    uint8_t retval = 0;

    /* activate the given chip select line */
    if (gpio_is_valid(cs) && !gpio_is_equal(cs, SOFT_SPI_CS_UNDEF)) {
        gpio_clear((gpio_t)cs);
    }

    retval = _transfer_one_byte(bus, out);

    if (!cont) {
        if (gpio_is_valid(cs) && !gpio_is_equal(cs, SOFT_SPI_CS_UNDEF)) {
            gpio_set((gpio_t)cs);
        }
    }

    return retval;
}

void soft_spi_transfer_bytes(soft_spi_t bus, soft_spi_cs_t cs, bool cont,
                             const void *out, void *in, size_t len)
{
    DEBUG("Soft SPI soft_spi_transfer_bytes\n");
    assert(soft_spi_bus_is_valid(bus));
    /* make sure at least one input or one output buffer is given */
    assert(out || in);

    const uint8_t *outbuf = out;
    uint8_t *inbuf = in;

    /* activate the given chip select line */
    if ((cs != GPIO_UNDEF) && (cs != SOFT_SPI_CS_UNDEF)) {
        gpio_clear((gpio_t)cs);
    }

    if (!inbuf) {
        for (size_t i = 0; i < len; i++) {
            _transfer_one_byte(bus, outbuf[i]);
        }
    }
    else if (!outbuf) {
        for (size_t i = 0; i < len; i++) {
            inbuf[i] = _transfer_one_byte(bus, READ_PADDING_BYTE);
        }
    }
    else  {
        for (size_t i = 0; i < len; i++)  {
            inbuf[i] = _transfer_one_byte(bus, outbuf[i]);
        }
    }

    if (!cont)  {
        if ((cs != GPIO_UNDEF) && (cs != SOFT_SPI_CS_UNDEF))  {
            gpio_set((gpio_t)cs);
        }
    }
}
