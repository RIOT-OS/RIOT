/*
 * Copyright (C) 2022 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_epd_bw_spi
 *
 * @{
 * @file
 * @brief       Device driver implementation for the epd_bw_spi display controller
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 * @}
 */
#include <string.h>
#include "byteorder.h"
#include "ztimer.h"

#include "epd_bw_spi.h"
#include "epd_bw_spi_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void epd_bw_spi_cmd_start(epd_bw_spi_params_t *p, uint8_t cmd, bool cont)
{
    DEBUG("[epd_bw_spi] cmd_start: command 0x%02x\n", cmd);
    if (gpio_is_valid(p->busy_pin)) {
        while (gpio_read(p->busy_pin) == p->busy_value) {}
    }
    gpio_clear(p->dc_pin);
    spi_transfer_byte(p->spi, p->cs_pin, cont, (uint8_t)cmd);
    gpio_set(p->dc_pin);
}

static void epd_bw_spi_write_cmd(epd_bw_spi_params_t *p, uint8_t cmd,
                                 const uint8_t *params, size_t plen)
{
    spi_acquire(p->spi, p->cs_pin, SPI_MODE_0, p->spi_clk);
    epd_bw_spi_cmd_start(p, cmd, plen > 0);
    if (plen) {
        spi_transfer_bytes(p->spi, p->cs_pin, false, params, NULL, plen);
    }
    spi_release(p->spi);
}

static void epd_bw_spi_wait(epd_bw_spi_params_t *p, uint32_t msec)
{
    if (gpio_is_valid(p->busy_pin)) {
        DEBUG("[epd_bw_spi] wait: for busy bin\n");
        while (gpio_read(p->busy_pin) == p->busy_value) {}
    }
    else {
        DEBUG("[epd_bw_spi] wait: for %" PRIu32 " milliseconds\n", msec);
        ztimer_sleep(ZTIMER_MSEC, msec);
    }
}

static void epd_bw_spi_control2(epd_bw_spi_params_t *p, uint8_t option, uint32_t wait_msec)
{
    DEBUG("[epd_bw_spi] control2: options 0x%02x, wait %" PRIu32" ms\n", option, wait_msec);

    epd_bw_spi_write_cmd(p, EPD_BW_SPI_CMD_DISPLAY_UPDATE_CONTROL_2, &option, 1);
    epd_bw_spi_write_cmd(p, EPD_BW_SPI_CMD_MASTER_ACTIVATION, NULL, 0);
    epd_bw_spi_wait(p, wait_msec);
}

int epd_bw_spi_init(epd_bw_spi_t *dev, const epd_bw_spi_params_t *params)
{
    memcpy(&dev->params, params, sizeof(epd_bw_spi_params_t));

    /* Initialize the counter to the maximum + 1 to trigger full refresh */
    dev->partial_refresh_count = dev->params.partial_refresh_max;

    if (gpio_is_valid(dev->params.rst_pin)) {
        if (gpio_init(dev->params.rst_pin, GPIO_OUT) != 0) {
            DEBUG("[epd_bw_spi] init: error initializing the RST pin\n");
            return EPD_BW_SPI_RST_FAIL;
        }
        gpio_set(dev->params.rst_pin);
    }

    if (gpio_is_valid(dev->params.busy_pin)) {
        if (gpio_init(dev->params.busy_pin, GPIO_IN) != 0) {
            DEBUG("[epd_bw_spi] init: error initializing the BUSY pin\n");
            return EPD_BW_SPI_BUSY_FAIL;
        }
    }

    if (!gpio_is_valid(dev->params.dc_pin) ||
        gpio_init(dev->params.dc_pin, GPIO_OUT) != 0) {
        DEBUG("[epd_bw_spi] init: error initializing the DC pin\n");
        return EPD_BW_SPI_DC_FAIL;
    }

    gpio_set(dev->params.dc_pin);

    int res = spi_init_cs(dev->params.spi, dev->params.cs_pin);
    if (res != SPI_OK) {
        DEBUG("[epd_bw_spi] init: error initializing the CS pin [%i]\n", res);
        return res;
    }

    return 0;
}

void epd_bw_spi_display_init(epd_bw_spi_t *dev)
{
    le_uint16_t y_data[2] = { 0 };
    uint8_t y_size;

    if (dev->params.controller.size_y <= 255) {
        y_data[0].u8[0] = dev->params.size_y - 1;
        y_size = 2;
    }
    else {
        y_data[0] =
            byteorder_btols(byteorder_htons((dev->params.size_y - 1) & 0x01FF));
        y_size = 3;
    }

    epd_bw_spi_wake(dev);
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_DRIVER_OUTPUT_CONTROL,
                         (uint8_t *)y_data, y_size);

    uint8_t data[] = {
        0xD7,   /* Phase 1: 30 ms phase, sel 3, 6.58 us off */
        0xD6,   /* Phase 2: 30 ms phase, sel 3, 3.34 us off */
        0x9D,   /* Phase 3: 10 ms phase, sel 4, 1.54 us off */
    };

    epd_bw_spi_write_cmd(&dev->params,
                         EPD_BW_SPI_CMD_BOOSTER_SOFT_START_CONTROL, data, 3);

    data[0] = dev->params.controller.vcom;
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_WRITE_VCOM_REGISTER, data,
                         1);

    data[0] = 0x1A; /* 4 dummy line per gate */
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_SET_DUMMY_LINE_PERIOD,
                         data, 1);

    data[0] = 0x08; /* 2 µs per line */
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_SET_GATE_LINE_WIDTH, data,
                         1);

    data[0] = (uint8_t)dev->params.entry_mode;
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_DATA_ENTRY_MODE_SETTING,
                         data, 1);
}

void epd_bw_spi_activate(epd_bw_spi_t *dev)
{
    uint8_t option =
          EPD_BW_SPI_DISPLAY_UPDATE_OPTION_ENABLE_CLOCK |
          EPD_BW_SPI_DISPLAY_UPDATE_OPTION_ENABLE_CP;

    epd_bw_spi_control2(&dev->params, option, EPD_BW_SPI_WAIT_ACTIVATION);
}

void epd_bw_spi_deactivate(epd_bw_spi_t *dev)
{
    uint8_t option =
          EPD_BW_SPI_DISPLAY_UPDATE_OPTION_DISABLE_CP |
          EPD_BW_SPI_DISPLAY_UPDATE_OPTION_DISABLE_OSC;

    epd_bw_spi_control2(&dev->params, option, EPD_BW_SPI_WAIT_ACTIVATION);
}

void epd_bw_spi_init_full(epd_bw_spi_t *dev)
{
    epd_bw_spi_display_init(dev);
    epd_bw_spi_set_area(dev, 0, dev->params.size_x, 0, dev->params.size_y);
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_WRITE_LUT_REGISTER,
                         dev->params.controller.lut_full,
                         dev->params.controller.lut_size);
}

void epd_bw_spi_update_full(epd_bw_spi_t *dev)
{
    uint8_t option =
          EPD_BW_SPI_DISPLAY_UPDATE_OPTION_ENABLE_CLOCK |
          EPD_BW_SPI_DISPLAY_UPDATE_OPTION_ENABLE_CP |
          EPD_BW_SPI_DISPLAY_UPDATE_OPTION_PATTERN_DISPLAY;

    epd_bw_spi_control2(&dev->params, option, EPD_BW_SPI_WAIT_UPDATE_FULL);
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_NOP, NULL, 0);
}

void epd_bw_spi_init_part(epd_bw_spi_t *dev)
{
    epd_bw_spi_display_init(dev);
    epd_bw_spi_set_area(dev, 0, dev->params.size_x, 0, dev->params.size_y);
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_WRITE_LUT_REGISTER,
                         dev->params.controller.lut_part,
                         dev->params.controller.lut_size);
}

void epd_bw_spi_update_part(epd_bw_spi_t *dev)
{
    epd_bw_spi_control2(&dev->params,
                        EPD_BW_SPI_DISPLAY_UPDATE_OPTION_PATTERN_DISPLAY,
                        EPD_BW_SPI_WAIT_UPDATE_PART);

    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_NOP, NULL, 0);
}

void epd_bw_spi_init_auto(epd_bw_spi_t *dev)
{
    if (dev->partial_refresh_count < dev->params.partial_refresh_max) {
        epd_bw_spi_init_part(dev);
    }
    else {
        epd_bw_spi_init_full(dev);
    }
}

void epd_bw_spi_update_auto(epd_bw_spi_t *dev)
{
    if (dev->partial_refresh_count < dev->params.partial_refresh_max) {
        epd_bw_spi_update_part(dev);
        dev->partial_refresh_count++;
    }
    else {
        epd_bw_spi_update_full(dev);
        dev->partial_refresh_count = 0;
    }
}

void epd_bw_spi_write_ram(epd_bw_spi_t *dev)
{
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_WRITE_RAM, NULL, 0);
}

void epd_bw_spi_clear(epd_bw_spi_t *dev)
{
    epd_bw_spi_fill(dev, 0, dev->params.size_x, 0, dev->params.size_y,
                    EPD_BW_SPI_COLOR_WHITE);
}

void epd_bw_spi_fill(epd_bw_spi_t *dev, uint8_t x1, uint8_t x2, uint16_t y1,
                     uint16_t y2, uint8_t color)
{
    epd_bw_spi_set_area(dev, x1, x2, y1, y2);

    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    epd_bw_spi_cmd_start(&dev->params, EPD_BW_SPI_CMD_WRITE_RAM, true);

    uint16_t size = ((x2 - x1) >> 3) * (y2 - y1);

    for (uint16_t i = 0; i < size - 1; i++) {
        spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true, color);
    }
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, false, color);

    spi_release(dev->params.spi);
}

void epd_bw_spi_fill_pixels(epd_bw_spi_t *dev, uint8_t x1, uint8_t x2,
                            uint16_t y1, uint16_t y2,
                            uint8_t *px)
{
    epd_bw_spi_set_area(dev, x1, x2, y1, y2);
    epd_bw_spi_write_buffer(dev, px, (x2 - x1) * (y2 - y1));
}

void epd_bw_spi_set_area(epd_bw_spi_t *dev, uint8_t x1, uint8_t x2, uint16_t y1,
                         uint16_t y2)
{
    DEBUG("[epd_bw_spi] set_area: (%d,%d)-(%d,%d)\n", x1, y1, x2, y2);

    /* Set X bounds */
    uint8_t x_data[] = {
        (x1 >> 3) & 0x1F,
        ((x2 - 1) >> 3) & 0x1F,
    };

    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_SET_RAM_X, x_data,
                         sizeof x_data);

    /* Set Y bounds */
    le_uint16_t y_data[2] = { 0 };
    uint8_t y_size;

    if (dev->params.controller.size_y <= 255) {
        y_data[0].u8[0] = y1;
        y_data[0].u8[1] = y2 - 1;
        y_size = 2;
    }
    else {
        y_data[0] = byteorder_btols(byteorder_htons(y1 & 0x01FF));
        y_data[1] = byteorder_btols(byteorder_htons((y2 - 1) & 0x01FF));
        y_size = 4;
    }
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_SET_RAM_Y,
                         (uint8_t *)y_data, y_size);

    /* Set counters to start positions */
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_SET_RAM_X_ADDR_COUNTER,
                         x_data, 1);
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_SET_RAM_Y_ADDR_COUNTER,
                         (uint8_t *)y_data, 2);
}

void epd_bw_spi_write_buffer(epd_bw_spi_t *dev, const uint8_t *buf, size_t len)
{
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_WRITE_RAM, buf, len);
}

void epd_bw_spi_sleep(epd_bw_spi_t *dev)
{
    uint8_t data[] = { 0x01 };

    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_DEEP_SLEEP_MODE, data, 1);
}

void epd_bw_spi_wake(epd_bw_spi_t *dev)
{
    /* Give a low pulse on the reset pin */
    if (gpio_is_valid(dev->params.rst_pin)) {
        gpio_clear(dev->params.rst_pin);
        ztimer_sleep(ZTIMER_MSEC, EPD_BW_SPI_WAIT_RESET);
        gpio_set(dev->params.rst_pin);
        ztimer_sleep(ZTIMER_MSEC, EPD_BW_SPI_WAIT_RESET);
    }

    /* Turn off sleep mode */
    uint8_t data[] = { 0x00 };

    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_DEEP_SLEEP_MODE, data, 1);
    epd_bw_spi_wait(&dev->params, EPD_BW_SPI_WAIT_RESET);
}

void epd_bw_spi_swreset(epd_bw_spi_t *dev)
{
    epd_bw_spi_write_cmd(&dev->params, EPD_BW_SPI_CMD_SWRESET, NULL, 0);
}
