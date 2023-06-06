/*
 * Copyright (C) 2023 Silke Hofstra
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_uc8151d
 * @{
 *
 * @file
 * @brief       Device driver implementation for the uc8151d
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>
#include "container.h"
#include "uc8151d.h"
#include "uc8151d_constants.h"
#include "uc8151d_params.h"
#include "uc8151d_lut.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

size_t _display_byte_len(const uc8151d_t *dev)
{
    return (dev->params.size_x / 8) * dev->params.size_y;
}

static void _wait_busy(uc8151d_t *dev)
{
    /* Fast return when wait is not required */
    if ((bool)gpio_read(dev->params.busy_pin) == 1) {
        return;
    }
    /* Make sure mutex is locked */
    mutex_trylock(&dev->gpio_wait);
    /* At this point enabling the IRQ is fine */
    gpio_irq_enable(dev->params.busy_pin);
    /* In the case the rising flank already hit, skip over the wait */
    if ((bool)gpio_read(dev->params.busy_pin) == 0) {
        /* Wait for the IRQ to unlock the mutex */
        mutex_lock(&dev->gpio_wait);
    }
    /* Disable the IRQ again */
    gpio_irq_disable(dev->params.busy_pin);
}

static void _gpio_cb(void *arg)
{
    uc8151d_t *dev = arg;

    mutex_unlock(&dev->gpio_wait);
}

static void _cmd_start(uc8151d_t *dev, uint8_t cmd, bool cont)
{
    DEBUG("[uc8151d] cmd_start: command 0x%02x\n", cmd);
    gpio_clear(dev->params.dc_pin);
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, cont, (uint8_t)cmd);
    gpio_set(dev->params.dc_pin);
}

static void _write_cmd(uc8151d_t *dev, uint8_t cmd,
                       const uint8_t *params, size_t plen)
{
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0, dev->params.spi_clk);
    _cmd_start(dev, cmd, plen > 0);
    if (plen) {
        spi_transfer_bytes(dev->params.spi, dev->params.cs_pin, false, params, NULL, plen);
    }
    spi_release(dev->params.spi);
}

static void _set_frame_rate(uc8151d_t *dev, uc8151d_frequency_t freq)
{
    uint8_t data = freq;

    _write_cmd(dev, UC8151D_PLL_CTRL, &data, sizeof(data));
}

static void _reset_disp(const uc8151d_t *dev)
{
    for (unsigned i = 0; i < 3; i++) {
        gpio_clear(dev->params.rst_pin);
        ztimer_sleep(ZTIMER_MSEC, 10);
        gpio_set(dev->params.rst_pin);
        ztimer_sleep(ZTIMER_MSEC, 10);
    }
}

static void _write_lut_data(uc8151d_t *dev, uint8_t command, const uc8151d_lut_phase_t *phases,
                            size_t num)
{

    size_t padding_len = (36 - (sizeof(uc8151d_lut_phase_t) * num));

    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    _cmd_start(dev, command, true);
    spi_transfer_bytes(dev->params.spi, dev->params.cs_pin,
                       (bool)padding_len,
                       phases,
                       NULL,
                       num * sizeof(uc8151d_lut_phase_t));

    if (padding_len) {
        for (size_t i = 0; i <  (padding_len - 1); i++) {
            spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true, 0x00);
        }
        spi_transfer_byte(dev->params.spi, dev->params.cs_pin, false, 0x00);
    }
    spi_release(dev->params.spi);
}

void _write_lut_partial_refresh(uc8151d_t *dev)
{
    _write_lut_data(dev, UC8151D_VCOM_LUT, &uc8151d_lut_partial_update_vcom, 1);
    _write_lut_data(dev, UC8151D_W2W_LUT, &uc8151d_lut_partial_update_b2w, 1);
    _write_lut_data(dev, UC8151D_B2W_LUT, &uc8151d_lut_partial_update_b2w, 1);
    _write_lut_data(dev, UC8151D_W2B_LUT, &uc8151d_lut_partial_update_w2b, 1);
    _write_lut_data(dev, UC8151D_B2B_LUT, &uc8151d_lut_partial_update_w2b, 1);
}

int uc8151d_init(uc8151d_t *dev, const uc8151d_params_t *params)
{
    memcpy(&dev->params, params, sizeof(uc8151d_params_t));
    mutex_init(&dev->gpio_wait);

    if (gpio_is_valid(dev->params.rst_pin)) {
        if (gpio_init(dev->params.rst_pin, GPIO_OUT) != 0) {
            DEBUG("[uc8151d] init: error initializing the RST pin\n");
            return UC8151D_RST_FAIL;
        }
        gpio_set(dev->params.rst_pin);
    }

    if (!gpio_is_valid(dev->params.busy_pin) ||
        gpio_init_int(dev->params.busy_pin, GPIO_IN, GPIO_RISING, _gpio_cb, dev) != 0) {
        DEBUG("[uc8151d] init: error initializing the BUSY pin\n");
        return UC8151D_BUSY_FAIL;
    }

    if (!gpio_is_valid(dev->params.dc_pin) ||
        gpio_init(dev->params.dc_pin, GPIO_OUT) != 0) {
        DEBUG("[uc8151d] init: error initializing the DC pin\n");
        return UC8151D_DC_FAIL;
    }

    gpio_set(dev->params.dc_pin);

    int res = spi_init_cs(dev->params.spi, dev->params.cs_pin);
    if (res != SPI_OK) {
        DEBUG("[uc8151d] init: error initializing the CS pin [%i]\n", res);
        return res;
    }

    dev->frame_rate = CONFIG_UC8151D_FRAME_RATE_DEFAULT;

    return 0;
}

void uc8151d_wake(uc8151d_t *dev)
{
    _reset_disp(dev);
}

static void _init_common(uc8151d_t *dev, bool custom_lut)
{
    _wait_busy(dev);

    const uint8_t panel_config[] = {
        UC8151D_PANEL_SETTING_DISP_RES_160x296 |
        UC8151D_PANEL_SETTING_BW |
        (custom_lut ? UC8151D_PANEL_SETTING_LUT_FROM_REG : 0) |
        UC8151D_PANEL_SETTING_SCAN_DIR_RIGHT |
        UC8151D_PANEL_SETTING_SCAN_DIR_UP |
        UC8151D_PANEL_SETTING_BOOSTER_SWITCH |
        UC8151D_PANEL_SETTING_NO_RESET
    };

    _write_cmd(dev, UC8151D_PANEL_SETTING, panel_config, sizeof(panel_config));

    static const uint8_t power_setting[] = {
        (UC8151D_POWER_SETTING_EN_DCDC_VDHVDL | UC8151D_POWER_SETTING_EN_DCDC_VGHVGL),
        0x00,
        0x2b,   /* +15 V for VDH */
        0x2b,   /* -15 V for VDL */
        0x03    /* 3 V for VDHR */
    };
    _write_cmd(dev, UC8151D_POWER_SETTING, power_setting, sizeof(power_setting));

    uint8_t resolution[] = {
        dev->params.size_x,
        dev->params.size_y / 256,
        dev->params.size_y % 256
    };
    _write_cmd(dev, UC8151D_RESOLUTION_SETTING, resolution, sizeof(resolution));

    if (dev->frame_rate != UC8151D_FRAME_RATE_DEFAULT) {
        _set_frame_rate(dev, dev->frame_rate);
    }
}

void uc8151d_init_full(uc8151d_t *dev)
{
    _init_common(dev, true);
}

void uc8151d_init_part(uc8151d_t *dev)
{
    _init_common(dev, true);
    _write_lut_partial_refresh(dev);
    _write_cmd(dev, UC8151D_PARTIAL_IN, NULL, 0);
}

void uc8151d_set_area(uc8151d_t *dev, uint8_t x1, uint8_t x2, uint16_t y1,
                      uint16_t y2)
{
    DEBUG("[uc8151d] set_area: (%d,%d)-(%d,%d)\n", x1, y1, x2, y2);

    _write_cmd(dev, UC8151D_PARTIAL_IN, NULL, 0);
    uint16_t y_end = y2 - 1;

    uint8_t partial_window[] = {
        x1 & 0xf8,
        (x2 - 8) | 0x7,
        y1 / 256,
        y1 % 256,
        y_end / 256,
        y_end % 256,
        0x28,
    };
    _write_cmd(dev, UC8151D_PARTIAL_WINDOW, partial_window, sizeof(partial_window));
}

void uc8151d_write_buffer(uc8151d_t *dev, const uint8_t *data, size_t len)
{
    _write_cmd(dev, UC8151D_DISP_START_TRANS2, data, len);
    _write_cmd(dev, UC8151D_DATA_STOP, NULL, 0);
    _wait_busy(dev);
}

void uc8151d_update_full(uc8151d_t *dev)
{
    /* Disable partial out to refresh the full display area */
    _write_cmd(dev, UC8151D_PARTIAL_OUT, NULL, 0);
    uc8151d_update_part(dev);
}

void uc8151d_update_part(uc8151d_t *dev)
{
    _write_cmd(dev, UC8151D_POWER_ON, NULL, 0);
    _wait_busy(dev);

    uint8_t vcom_data[] = {
        (dev->invert ? 0x40 : 0x90) | 0x0C,
    };
    _write_cmd(dev, UC8151D_VCOM_DATA_INTERVAL, vcom_data, sizeof(vcom_data));

    _write_cmd(dev, UC8151D_DISP_REFRESH, NULL, 0);
    _wait_busy(dev);
    _write_cmd(dev, UC8151D_POWER_OFF, NULL, 0);
    _wait_busy(dev);
}

void uc8151d_fill(uc8151d_t *dev, uint8_t x1, uint8_t x2, uint16_t y1,
                  uint16_t y2, uint8_t color)
{
    uc8151d_set_area(dev, x1, x2, y1, y2);
    size_t size = ((x2 - x1) / 8) * (y2 - y1);
    spi_acquire(dev->params.spi, dev->params.cs_pin, SPI_MODE_0,
                dev->params.spi_clk);
    _cmd_start(dev, UC8151D_DISP_START_TRANS2, true);
    for (uint16_t i = 0; i < size - 1; i++) {
        spi_transfer_byte(dev->params.spi, dev->params.cs_pin, true, color);
    }
    spi_transfer_byte(dev->params.spi, dev->params.cs_pin, false, color);
    spi_release(dev->params.spi);
}

void uc8151d_sleep(uc8151d_t *dev)
{
    static const uint8_t vcom_data_off[] = {
        0xf7,
    };

    _write_cmd(dev, UC8151D_VCOM_DATA_INTERVAL, vcom_data_off, sizeof(vcom_data_off));

    _write_cmd(dev, UC8151D_POWER_OFF, NULL, 0);

    ztimer_sleep(ZTIMER_MSEC, 100);

    _wait_busy(dev);

    static const uint8_t deep_sleep[] = {
        0xa5,
    };
    _write_cmd(dev, UC8151D_DEEP_SLEEP, deep_sleep, sizeof(deep_sleep));
    _wait_busy(dev);
}

void uc8151d_fill_pixels(uc8151d_t *dev, uint8_t x1, uint8_t x2,
                         uint16_t y1, uint16_t y2,
                         uint8_t *px)
{
    uc8151d_set_area(dev, x1, x2, y1, y2);
    uc8151d_write_buffer(dev, px, (x2 - x1) * (y2 - y1));
}
