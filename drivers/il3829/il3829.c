/*
 * Copyright (C) 2018 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_il3829
 *
 * @{
 * @file
 * @brief       Device driver implementation for the il3829 display controller
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */
#include <string.h>
#include "byteorder.h"
#include "spi_display.h"

#include "il3829.h"
#include "il3829_params.h"
#include "il3829_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int il3829_init(il3829_t *dev, const spi_display_params_t *params,
                uint8_t size_x, uint16_t size_y, il3829_entry_mode_t entry_mode)
{
    memcpy(&dev->params, params, sizeof(spi_display_params_t));
    dev->size_x = size_x;
    dev->size_y = size_y;
    dev->entry_mode = entry_mode;
    dev->params.dummy = false;

    return spi_display_init(&dev->params);
}

void il3829_display_init(il3829_t *dev)
{
    uint16_t y_data[2] = { 0 };

    y_data[0] = (dev->size_y - 1) & 0x1FF;
    spi_display_write_cmd(&dev->params, IL3829_CMD_DRIVER_OUTPUT_CONTROL, (uint8_t *)y_data, 3);

    uint8_t data[3];
    data[0] = 0xD7; /* Phase 1: 30 ms phase, sel 3, 6.58 us off */
    data[1] = 0xD6; /* Phase 2: 30 ms phase, sel 3, 3.34 us off */
    data[2] = 0x9D; /* Phase 3: 10 ms phase, sel 4, 1.54 us off */
    spi_display_write_cmd(&dev->params, IL3829_CMD_BOOSTER_SOFT_START_CONTROL, data, 3);

    data[0] = IL3829_VCOM;
    spi_display_write_cmd(&dev->params, IL3829_CMD_WRITE_VCOM_REGISTER, data, 1);

    data[0] = 0x1A; /* 4 dummy line per gate */
    spi_display_write_cmd(&dev->params, IL3829_CMD_SET_DUMMY_LINE_PERIOD, data, 1);

    data[0] = 0x08; /* 2 µs per line */
    spi_display_write_cmd(&dev->params, IL3829_CMD_SET_GATE_LINE_WIDTH, data, 1);

    data[0] = (uint8_t)dev->entry_mode;
    spi_display_write_cmd(&dev->params, IL3829_CMD_DATA_ENTRY_MODE_SETTING, data, 1);
}

void il3829_activate(il3829_t *dev)
{
    uint8_t data[] = { 0xC0 }; /* Enable Clock Signal then Enable CP */

    spi_display_write_cmd(&dev->params, IL3829_CMD_DISPLAY_UPDATE_CONTROL_2, data, sizeof data);
    spi_display_write_cmd(&dev->params, IL3829_CMD_MASTER_ACTIVATION, NULL, 0);
    spi_display_wait(&dev->params, IL3829_WAIT_ACTIVATION);
}

void il3829_deactivate(il3829_t *dev)
{
    uint8_t data[] = { 0x03 }; /* Enable and disable (TODO: check C3 or 03) */

    spi_display_write_cmd(&dev->params, IL3829_CMD_DISPLAY_UPDATE_CONTROL_2, data, sizeof data);
    spi_display_write_cmd(&dev->params, IL3829_CMD_MASTER_ACTIVATION, NULL, 0);
    spi_display_wait(&dev->params, IL3829_WAIT_ACTIVATION);
}

void il3829_init_full(il3829_t *dev)
{
    il3829_display_init(dev);
    il3829_set_area(dev, 0, 200, 0, 200);
    spi_display_write_cmd(&dev->params, IL3829_CMD_WRITE_LUT_REGISTER,
                          il3829_lut_default_full, sizeof il3829_lut_default_full);
}

void il3829_update_full(il3829_t *dev)
{
    uint8_t data[] = { 0xC4 }; /* Enable Clock Signal then Enable CP | display pattern */

    spi_display_write_cmd(&dev->params, IL3829_CMD_DISPLAY_UPDATE_CONTROL_2, data, sizeof data);
    spi_display_write_cmd(&dev->params, IL3829_CMD_MASTER_ACTIVATION, NULL, 0);
    spi_display_wait(&dev->params, IL3829_WAIT_UPDATE_FULL);
    spi_display_write_cmd(&dev->params, IL3829_CMD_NOP, NULL, 0);
}

void il3829_init_part(il3829_t *dev)
{
    il3829_display_init(dev);
    il3829_set_area(dev, 0, 200, 0, 200);
    spi_display_write_cmd(&dev->params, IL3829_CMD_WRITE_LUT_REGISTER,
                          il3829_lut_default_part, sizeof il3829_lut_default_part);
}

void il3829_update_part(il3829_t *dev)
{
    uint8_t data[] = { 0x04 }; /* To display pattern */

    spi_display_write_cmd(&dev->params, IL3829_CMD_DISPLAY_UPDATE_CONTROL_2, data, sizeof data);
    spi_display_write_cmd(&dev->params, IL3829_CMD_MASTER_ACTIVATION, NULL, 0);
    spi_display_wait(&dev->params, IL3829_WAIT_UPDATE_PART);
    spi_display_write_cmd(&dev->params, IL3829_CMD_NOP, NULL, 0);
}

void il3829_write_ram(il3829_t *dev)
{
    spi_display_write_cmd(&dev->params, IL3829_CMD_WRITE_RAM, NULL, 0);
}

void il3829_clear(il3829_t *dev)
{
    il3829_fill(dev, 0, dev->size_x, 0, dev->size_y, IL3829_COLOR_WHITE);
}

void il3829_fill(il3829_t *dev, uint8_t x1, uint8_t x2, uint16_t y1, uint16_t y2, uint8_t color)
{
    il3829_set_area(dev, x1, x2, y1, y2);

    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0, dev->params.spi_clk);
    spi_display_cmd_start(&dev->params, IL3829_CMD_WRITE_RAM, true);

    uint16_t size = ((x2 - x1) >> 3) * (y2 - y1);
    for (uint16_t i = 0; i < size - 1; i++) {
        spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true, color);
    }
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, false, color);

    spi_release(dev->params.spi);
}

void il3829_fill_pixels(il3829_t *dev, uint8_t x1, uint8_t x2, uint16_t y1, uint16_t y2,
                        uint8_t *px)
{
    il3829_set_area(dev, x1, x2, y1, y2);
    il3829_write_buffer(dev, px, (x2 - x1) * (y2 - y1));
}

void il3829_set_area(il3829_t *dev, uint8_t x1, uint8_t x2, uint16_t y1, uint16_t y2)
{
    /* Set X bounds */
    uint8_t x_data[] = {
        (x1 >> 3) & 0x1F,
        ((x2 - 1) >> 3) & 0x1F,
    };
    spi_display_write_cmd(&dev->params, IL3829_CMD_SET_RAM_X, x_data, sizeof x_data);

    /* Set Y bounds */
    le_uint16_t y_data[] = {
        byteorder_btols(byteorder_htons(y1 & 0x01FF)),
        byteorder_btols(byteorder_htons((y2 - 1) & 0x01FF)),
    };
    spi_display_write_cmd(&dev->params, IL3829_CMD_SET_RAM_Y, (uint8_t *)y_data, sizeof y_data);

    /* Set counters to start positions */
    spi_display_write_cmd(&dev->params, IL3829_CMD_SET_RAM_X_ADDR_COUNTER, x_data, 1);
    spi_display_write_cmd(&dev->params, IL3829_CMD_SET_RAM_Y_ADDR_COUNTER, (uint8_t *)y_data, 2);
}

void il3829_write_buffer(il3829_t *dev, const uint8_t *buf, size_t len)
{
    spi_display_write_cmd(&dev->params, IL3829_CMD_WRITE_RAM, buf, len);
}

void il3829_sleep(il3829_t *dev)
{
    uint8_t data = 0x01;

    spi_display_write_cmd(&dev->params, IL3829_CMD_DEEP_SLEEP_MODE, &data, 1);
}

/* TODO: test this */
void il3829_wake(il3829_t *dev)
{
    /* Give a low pulse on the reset pin */
    if (dev->params.rst_pin != GPIO_UNDEF) {
        gpio_clear(dev->params.rst_pin);
        spi_display_wait(&dev->params, IL3829_WAIT_RESET);
        gpio_set(dev->params.rst_pin);
    }

    /* Turn off sleep mode */
    uint8_t data = 0x00;
    spi_display_write_cmd(&dev->params, IL3829_CMD_DEEP_SLEEP_MODE, &data, 1);
}

void il3829_swreset(il3829_t *dev)
{
    spi_display_write_cmd(&dev->params, IL3829_CMD_SWRESET, NULL, 0);
}
