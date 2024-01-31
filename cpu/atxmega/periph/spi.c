/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "cpu_pm.h"
#include "mutex.h"
#include "periph/spi.h"
#include "pm_layered.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _print_buffer(const char* s, const uint8_t* buffer, uint16_t len)
{
    uint16_t i;

    if (buffer == NULL) {
        DEBUG("%s - no buffer\n", s);
        return;
    }

    DEBUG("%s\n", s);

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0 && i != 0) {
            DEBUG("\n");
        }
        else if ((i % 8) == 0) {
            DEBUG(" ");
        }

        DEBUG("%02x ", buffer[i]);
    }

    DEBUG("\n");
}

/**
 * @brief   Allocation device locks
 */
static mutex_t locks[SPI_NUMOF];

/**
 * @brief   Get the pointer to the base register of the given SPI device
 *
 * @param[in] dev       SPI device identifier
 *
 * @return              base register address
 */
static inline SPI_t* dev(spi_t bus)
{
    return (SPI_t*)spi_config[bus].dev;
}

void spi_init(spi_t bus)
{
    mutex_init(&locks[bus]);
    spi_init_pins(bus);

    DEBUG("init\n");
}

void spi_init_pins(spi_t bus)
{
    /* SS pin always should be output even when it is not used */
    spi_init_cs(bus, spi_config[bus].ss_pin);
    gpio_init(spi_config[bus].sck_pin, GPIO_OUT);
    gpio_init(spi_config[bus].miso_pin, GPIO_IN);
    gpio_init(spi_config[bus].mosi_pin, GPIO_OUT);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    (void)clk;

    DEBUG("acquire\n");

    pm_block(4); /* Require clkPer */
    mutex_lock(&locks[bus]);
    pm_periph_enable(spi_config[bus].pwr);

    dev(bus)->CTRL = SPI_CLK2X_bm
                   | SPI_ENABLE_bm
                   | SPI_MASTER_bm
                   | (mode << SPI_MODE_gp)
                   | SPI_PRESCALER0_bm;

    (void)dev(bus)->STATUS;
    (void)dev(bus)->DATA;
}

void spi_release(spi_t bus)
{
    dev(bus)->CTRL &= ~SPI_ENABLE_bm;
    pm_periph_disable(spi_config[bus].pwr);
    mutex_unlock(&locks[bus]);
    pm_unblock(4);

    DEBUG("release\n");
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    uint8_t *out_buf = (uint8_t *)out;
    uint8_t *in_buf = (uint8_t *)in;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_buffer("bytes - out", out, len);
    }

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t) cs);
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;

        dev(bus)->DATA = tmp;

        while (!(dev(bus)->STATUS & SPI_IF_bm)){}

        tmp = dev(bus)->DATA;

        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t) cs);
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_buffer("bytes - in", in, len);
    }
}
