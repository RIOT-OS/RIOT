/*
 * Copyright (C) 2018 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_spi_display
 *
 * @{
 * @file
 * @brief       Common device driver elements for SPI driven displays
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */
#include "periph/spi.h"
#include "xtimer.h"
#include "spi_display.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void spi_display_cmd_start(spi_display_params_t *p, uint8_t cmd, bool cont)
{
    if (p->busy_pin != GPIO_UNDEF) {
        while (gpio_read(p->busy_pin)) {}
    }
    gpio_clear(p->dc_pin);
    spi_transfer_byte(p->spi, p->cs_pin, cont, (uint8_t)cmd);
    gpio_set(p->dc_pin);
}

void spi_display_write_cmd(spi_display_params_t *p, uint8_t cmd, const uint8_t *params, size_t plen)
{
    spi_acquire(p->spi, p->cs_pin, SPI_MODE_0, p->spi_clk);
    spi_display_cmd_start(p, cmd, plen ? true : false);
    if (plen) {
        spi_transfer_bytes(p->spi, p->cs_pin, false, params, NULL, plen);
    }
    spi_release(p->spi);
}

void spi_display_read_cmd(spi_display_params_t *p, uint8_t cmd, uint8_t *params, size_t plen)
{
    spi_acquire(p->spi, p->cs_pin, SPI_MODE_0, p->spi_clk);
    spi_display_cmd_start(p, cmd, true);
    if (p->dummy) {
        spi_transfer_byte(p->spi, p->cs_pin, true, 0x00);
    }
    spi_transfer_bytes(p->spi, p->cs_pin, false, NULL, params, plen);
    spi_release(p->spi);
}

int spi_display_init(spi_display_params_t *p)
{
    if (p->rst_pin != GPIO_UNDEF) {
        if (gpio_init(p->rst_pin, GPIO_OUT) != 0) {
            DEBUG("[spi_display] init: error initializing the RST pin\n");
            return SPI_DISPLAY_RST_FAIL;
        }
        gpio_set(p->rst_pin);
    }

    if (p->busy_pin != GPIO_UNDEF) {
        if (gpio_init(p->busy_pin, GPIO_IN) != 0) {
            DEBUG("[spi_display] init: error initializing the BUSY pin\n");
            return SPI_DISPLAY_BUSY_FAIL;
        }
    }

    if (gpio_init(p->dc_pin, GPIO_OUT) != 0) {
        DEBUG("[spi_display] init: error initializing the DC pin\n");
        return SPI_DISPLAY_DC_FAIL;
    }
    gpio_set(p->dc_pin);

    int res = spi_init_cs(p->spi, p->cs_pin);
    if (res != SPI_OK) {
        DEBUG("[spi_display] init: error initializing the CS pin [%i]\n", res);
        return res;
    }

    return 0;
}

void spi_display_wait(spi_display_params_t *p, uint32_t usec)
{
    if (p->busy_pin != GPIO_UNDEF) {
        while (gpio_read(p->busy_pin)) {}
    }
    else {
        xtimer_usleep(usec);
    }
}
