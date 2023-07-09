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
#include "macros/math.h"
#include "mutex.h"
#include "periph/spi.h"
#include "pm_layered.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Helper function to compute a right shift value corresponding
 *          to dividers
 * @{
 */
static uint8_t _clk_shift(uint32_t freq)
{
    /* Atmega datasheets give the following table:
     * SPI2X    SPR1    SPR0    SCK Frequency
     * 0        0       0       Fosc/4
     * 0        0       1       Fosc/16
     * 0        1       0       Fosc/64
     * 0        1       1       Fosc/128
     * 1        0       0       Fosc/2
     * 1        0       1       Fosc/8
     * 1        1       0       Fosc/32
     * 1        1       1       Fosc/64
     * We can easily sort it by dividers by inverting the SPI2X bit and
     * taking it as LSB:
     * Divider  SPR1    SPR0    ~SPI2X  shift
     * 2        0       0       0       0
     * 4        0       0       1       1
     * 8        0       1       0       2
     * 16       0       1       1       3
     * 32       1       0       0       4
     * 64       1       0       1       5
     * 64       1       1       0       6
     * 128      1       1       1       7 */
    uint8_t shift;
    for (shift = 0; freq << shift < CLOCK_CORECLOCK / 2;
         shift = ++shift > 5 ? shift + 1 : shift) {}
    return shift;
}
/** @} */

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

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;
    /* bound divider to 128 */
    if (freq < DIV_ROUND_UP(CLOCK_CORECLOCK, 128)) {
        return (spi_clk_t){ .err = -EDOM };
    }

    uint8_t shift = _clk_shift(freq);
    return (spi_clk_t){ .clk = ((~shift & 1) << 7) | (shift >> 1) };
}

int32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    if (clk.err) {
        return -EINVAL;
    }
    uint8_t shift = ((~clk.clk & 0x80) >> 7) | (clk.clk << 1);
    return shift > 5 ?
        CLOCK_CORECLOCK >> shift : (CLOCK_CORECLOCK / 2) >> shift;
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;

    assert(bus < SPI_NUMOF);
    if (clk.err) {
        return;
    }

    DEBUG("acquire\n");

    pm_block(3);
    mutex_lock(&locks[bus]);
    pm_periph_enable(spi_config[bus].pwr);

    dev(bus)->CTRL = SPI_ENABLE_bm
                   | SPI_MASTER_bm
                   | (mode << SPI_MODE_gp)
                   | clk.clk;

    (void)dev(bus)->STATUS;
    (void)dev(bus)->DATA;
}

void spi_release(spi_t bus)
{
    dev(bus)->CTRL &= ~SPI_ENABLE_bm;
    pm_periph_disable(spi_config[bus].pwr);
    mutex_unlock(&locks[bus]);
    pm_unblock(3);

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
