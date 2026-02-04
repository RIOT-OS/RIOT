/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_paa5100je
 * @{
 *
 * @file
 * @brief       Device driver implementation for the PAA5100JE/PMW3901 optical flow sensor
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 *
 * @}
 */

#include "paa5100je.h"
#include "paa5100je_constants.h"
#include "paa5100je_params.h"
#include "ztimer.h"
#include "macros/utils.h"
#include "log.h"
#include <stdio.h>

#define PAA5100JE_CMD_READ   (0b00000000) /**< Mask applied to a register address when reading */
#define PAA5100JE_CMD_WRITE  (0b10000000) /**< Mask applied to a register address when writing */

/* Prototypes of private functions */
static uint8_t _read_reg(const paa5100je_t *dev, uint8_t reg);
static void _read_reg_burst(const paa5100je_t *dev, uint8_t reg, size_t num, uint8_t *buf);
static void _write_reg(const paa5100je_t *dev, uint8_t reg, uint8_t value);
static void _prop_init(const paa5100je_t *dev);
static void _init_paa5100je(const paa5100je_t *dev);
static void _init_pmw3901(const paa5100je_t *dev);

/* Public API */

int paa5100je_init(paa5100je_t *dev, const paa5100je_params_t *params)
{
    assert(dev && params);
    dev->params = params;

    int res = spi_init_cs(params->spi, params->cs);
    if (res < 0) {
        return -ENXIO;
    }
    _write_reg(dev, REG_POWER_UP_RESET, 0x5A);

    for (uint8_t offset = 0; offset < 5; offset++) {
        (void) _read_reg(dev, REG_DATA_READY + offset);
    }

    _prop_init(dev);

    uint8_t id = _read_reg(dev, REG_ID);
    uint8_t inv_id = _read_reg(dev, REG_INV_ID);
    uint8_t rev = _read_reg(dev, REG_REV);
    if ((id ^ inv_id) != 0xFF || id != 0x49 || rev != 0x00) {
        LOG_ERROR("[PAA5100JE] Wrong id, inverted id, or unknown revision.\n");
        return -ENODEV;
    }
    return 0;
}

int paa5100je_get_motion_burst(const paa5100je_t *dev, int16_t *x, int16_t *y)
{
    uint8_t data[12];
    ztimer_now_t start = ztimer_now(ZTIMER_MSEC);

    do {
        _read_reg_burst(dev, REG_MOTION_BURST, 12, data);
        /* PixArt who designed the sensor is pretty secretive. The default threshold for the quality
         * and for the shutter are taken from a reference implementation. */
        uint8_t data_ready = data[0];
        uint8_t quality = data[6];
        uint8_t shutter_upper = data[10];
        if ((data_ready & 0b10000000)
            && quality >= CONFIG_PAA5100JE_QUALITY_THRESHOLD
            && shutter_upper != 0x1F) {
            *x = (int16_t)(data[3] << 8 | data[2]);
            *y = (int16_t)(data[5] << 8 | data[4]);
            /* Apply scaling factor */
            *x *= 100;
            *x /= CONFIG_PAA5100JE_SCALE_DENOMINATOR;
            *y *= 100;
            *y /= CONFIG_PAA5100JE_SCALE_DENOMINATOR;
            return 0;
        }
        ztimer_sleep(ZTIMER_MSEC, 1);
    } while (CONFIG_PAA5100JE_TIMEOUT_MS
             && ztimer_now(ZTIMER_MSEC) < start + CONFIG_PAA5100JE_TIMEOUT_MS);
    return -ETIME;
}

int paa5100je_set_led_brightness(const paa5100je_t *dev, const paa5100je_led_brightness_t level)
{
    assert(dev);
    uint8_t reg_val;

    switch (level) {
    case PAA5100JE_LED_OFF:
        reg_val = PAA5100JE_LED_OFF_VAL;
        break;
    case PAA5100JE_LED_MEDIUM:
        reg_val = PAA5100JE_LED_MEDIUM_VAL;
        break;
    case PAA5100JE_LED_MAX:
        reg_val = PAA5100JE_LED_MAX_VAL;
        break;
    default:
        return -EINVAL;
    }

    _write_reg(dev, 0x7F, 0x14);
    _write_reg(dev, REG_LED_BRIGHTNESS, reg_val);
    _write_reg(dev, 0x7F, 0x00);

    return 0;
}

/* Private API */

static uint8_t _read_reg(const paa5100je_t *dev, uint8_t reg)
{
    assert(dev);
    assert(!(reg & 0x10000000));

    uint8_t value;

    spi_acquire(dev->params->spi, dev->params->cs, SPI_MODE_3, dev->params->clk);

    value = spi_transfer_reg(dev->params->spi, dev->params->cs, reg | PAA5100JE_CMD_READ, 0);

    spi_release(dev->params->spi);

    return value;
}

static void _read_reg_burst(const paa5100je_t *dev, uint8_t reg, size_t num, uint8_t *buf)
{
    assert(dev);
    assert(!(reg & 0x10000000));
    spi_acquire(dev->params->spi, dev->params->cs, SPI_MODE_3, dev->params->clk);
    spi_transfer_regs(dev->params->spi, dev->params->cs, reg | PAA5100JE_CMD_READ, NULL, buf, num);
    spi_release(dev->params->spi);
}

static void _write_reg(const paa5100je_t *dev, uint8_t reg, uint8_t value)
{
    assert(dev);
    assert(!(reg & 0x10000000));

    spi_acquire(dev->params->spi, dev->params->cs, SPI_MODE_3, dev->params->clk);

    (void) spi_transfer_reg(dev->params->spi, dev->params->cs, (reg | PAA5100JE_CMD_WRITE), value);

    spi_release(dev->params->spi);
}

/**
 * @brief Writes a set of magic values to the sensors registers.
 *
 * The datasheet does not explain this.
 * These values and registers are taken from a reference implementation.
 *
 * @param[in]   dev    device descriptor
 */
static void _prop_init(const paa5100je_t *dev)
{
    _write_reg(dev, 0x7F, 0x00);
    _write_reg(dev, 0x55, 0x01);
    _write_reg(dev, 0x50, 0x07);
    _write_reg(dev, 0x7F, 0x0E);
    _write_reg(dev, 0x43, 0x10);

    int tmp = _read_reg(dev, 0x67);
    if (tmp & 0b10000000){
        _write_reg(dev, 0x48, 0x04);
    }
    else {
        _write_reg(dev, 0x48, 0x02);
    }

    _write_reg(dev, 0x7F, 0x00);
    _write_reg(dev, 0x51, 0x7B);
    _write_reg(dev, 0x50, 0x00);
    _write_reg(dev, 0x55, 0x00);
    _write_reg(dev, 0x7F, 0x0E);

    tmp = _read_reg(dev, 0x73);
    if (tmp == 0x00){
        int c1 = _read_reg(dev, 0x70);
        int c2 = _read_reg(dev, 0x71);
        if (c1 <= 28) {
            c1 += 14;
        }
        if (c1 > 28) {
            c1 += 11;
        }
        c1 = MAX(0, MIN(0x3F, c1));
        c2 = (c2 * 45);
        _write_reg(dev, 0x7F, 0x00);
        _write_reg(dev, 0x61, 0xAD);
        _write_reg(dev, 0x51, 0x70);
        _write_reg(dev, 0x7F, 0x0E);
        _write_reg(dev, 0x70, c1);
        _write_reg(dev, 0x71, c2);
    }

    if (dev->params->var == PAA5100JE) {
        _init_paa5100je(dev);
    }
    else {
        _init_pmw3901(dev);
    }
}

/**
 * @brief Writes a set of PAA5100JE specific magic values to the sensors registers.
 *
 * @param[in]   dev    device descriptor
 */
static void _init_paa5100je(const paa5100je_t *dev)
{
    _write_reg(dev, 0x7F, 0x00);
    _write_reg(dev, 0x61, 0xAD);

    _write_reg(dev, 0x7F, 0x03);
    _write_reg(dev, 0x40, 0x00);

    _write_reg(dev, 0x7F, 0x05);
    _write_reg(dev, 0x41, 0xB3);
    _write_reg(dev, 0x43, 0xF1);
    _write_reg(dev, 0x45, 0x14);

    _write_reg(dev, 0x5F, 0x34);
    _write_reg(dev, 0x7B, 0x08);
    _write_reg(dev, 0x5E, 0x34);
    _write_reg(dev, 0x5B, 0x11);
    _write_reg(dev, 0x6D, 0x11);
    _write_reg(dev, 0x45, 0x17);
    _write_reg(dev, 0x70, 0xE5);
    _write_reg(dev, 0x71, 0xE5);

    _write_reg(dev, 0x7F, 0x06);
    _write_reg(dev, 0x44, 0x1B);
    _write_reg(dev, 0x40, 0xBF);
    _write_reg(dev, 0x4E, 0x3F);

    _write_reg(dev, 0x7F, 0x08);
    _write_reg(dev, 0x66, 0x44);
    _write_reg(dev, 0x65, 0x20);
    _write_reg(dev, 0x6A, 0x3A);
    _write_reg(dev, 0x61, 0x05);
    _write_reg(dev, 0x62, 0x05);

    _write_reg(dev, 0x7F, 0x09);
    _write_reg(dev, 0x4F, 0xAF);
    _write_reg(dev, 0x5F, 0x40);
    _write_reg(dev, 0x48, 0x80);
    _write_reg(dev, 0x49, 0x80);
    _write_reg(dev, 0x57, 0x77);
    _write_reg(dev, 0x60, 0x78);
    _write_reg(dev, 0x61, 0x78);
    _write_reg(dev, 0x62, 0x08);
    _write_reg(dev, 0x63, 0x50);

    _write_reg(dev, 0x7F, 0x0A);
    _write_reg(dev, 0x45, 0x60);

    _write_reg(dev, 0x7F, 0x00);
    _write_reg(dev, 0x4D, 0x11);
    _write_reg(dev, 0x55, 0x80);
    _write_reg(dev, 0x74, 0x21);
    _write_reg(dev, 0x75, 0x1F);
    _write_reg(dev, 0x4A, 0x78);
    _write_reg(dev, 0x4B, 0x78);
    _write_reg(dev, 0x44, 0x08);

    _write_reg(dev, 0x45, 0x50);
    _write_reg(dev, 0x64, 0xFF);
    _write_reg(dev, 0x65, 0x1F);

    _write_reg(dev, 0x7F, 0x14);
    _write_reg(dev, 0x65, 0x67);
    _write_reg(dev, 0x66, 0x08);
    _write_reg(dev, 0x63, 0x70);
    _write_reg(dev, 0x6F, 0x1C);

    _write_reg(dev, 0x7F, 0x15);
    _write_reg(dev, 0x48, 0x48);

    _write_reg(dev, 0x7F, 0x07);
    _write_reg(dev, 0x41, 0x0D);
    _write_reg(dev, 0x43, 0x14);
    _write_reg(dev, 0x4B, 0x0E);
    _write_reg(dev, 0x45, 0x0F);
    _write_reg(dev, 0x44, 0x42);
    _write_reg(dev, 0x4C, 0x80);

    _write_reg(dev, 0x7F, 0x10);
    _write_reg(dev, 0x5B, 0x02);

    _write_reg(dev, 0x7F, 0x07);
    _write_reg(dev, 0x40, 0x41);

    ztimer_sleep(ZTIMER_MSEC, 100);

    _write_reg(dev, 0x7F, 0x00);
    _write_reg(dev, 0x32, 0x00);

    _write_reg(dev, 0x7F, 0x07);
    _write_reg(dev, 0x40, 0x40);

    _write_reg(dev, 0x7F, 0x06);
    _write_reg(dev, 0x68, 0xF0);
    _write_reg(dev, 0x69, 0x00);

    _write_reg(dev, 0x7F, 0x0D);
    _write_reg(dev, 0x48, 0xC0);
    _write_reg(dev, 0x6F, 0xD5);

    _write_reg(dev, 0x7F, 0x00);
    _write_reg(dev, 0x5B, 0xA0);
    _write_reg(dev, 0x4E, 0xA8);
    _write_reg(dev, 0x5A, 0x90);
    _write_reg(dev, 0x40, 0x80);
    _write_reg(dev, 0x73, 0x1F);

    ztimer_sleep(ZTIMER_MSEC, 100);

    _write_reg(dev, 0x73, 0x00);
}

/**
 * @brief Writes a set of PMW301 specific magic values to the sensors registers.
 *
 * @param[in]   dev    device descriptor
 */
static void _init_pmw3901(const paa5100je_t *dev)
{
    _write_reg(dev, 0x7F, 0x00);
    _write_reg(dev, 0x61, 0xAD);
    _write_reg(dev, 0x7F, 0x03);
    _write_reg(dev, 0x40, 0x00);
    _write_reg(dev, 0x7F, 0x05);

    _write_reg(dev, 0x41, 0xB3);
    _write_reg(dev, 0x43, 0xF1);
    _write_reg(dev, 0x45, 0x14);
    _write_reg(dev, 0x5B, 0x32);
    _write_reg(dev, 0x5F, 0x34);
    _write_reg(dev, 0x7B, 0x08);
    _write_reg(dev, 0x7F, 0x06);
    _write_reg(dev, 0x44, 0x1B);
    _write_reg(dev, 0x40, 0xBF);
    _write_reg(dev, 0x4E, 0x3F);
    _write_reg(dev, 0x7F, 0x08);
    _write_reg(dev, 0x65, 0x20);
    _write_reg(dev, 0x6A, 0x18);

    _write_reg(dev, 0x7F, 0x09);
    _write_reg(dev, 0x4F, 0xAF);
    _write_reg(dev, 0x5F, 0x40);
    _write_reg(dev, 0x48, 0x80);
    _write_reg(dev, 0x49, 0x80);

    _write_reg(dev, 0x57, 0x77);
    _write_reg(dev, 0x60, 0x78);
    _write_reg(dev, 0x61, 0x78);
    _write_reg(dev, 0x62, 0x08);
    _write_reg(dev, 0x63, 0x50);
    _write_reg(dev, 0x7F, 0x0A);
    _write_reg(dev, 0x45, 0x60);
    _write_reg(dev, 0x7F, 0x00);
    _write_reg(dev, 0x4D, 0x11);

    _write_reg(dev, 0x55, 0x80);
    _write_reg(dev, 0x74, 0x1F);
    _write_reg(dev, 0x75, 0x1F);
    _write_reg(dev, 0x4A, 0x78);
    _write_reg(dev, 0x4B, 0x78);

    _write_reg(dev, 0x44, 0x08);
    _write_reg(dev, 0x45, 0x50);
    _write_reg(dev, 0x64, 0xFF);
    _write_reg(dev, 0x65, 0x1F);
    _write_reg(dev, 0x7F, 0x14);
    _write_reg(dev, 0x65, 0x60);
    _write_reg(dev, 0x66, 0x08);
    _write_reg(dev, 0x63, 0x78);
    _write_reg(dev, 0x7F, 0x15);
    _write_reg(dev, 0x48, 0x58);
    _write_reg(dev, 0x7F, 0x07);
    _write_reg(dev, 0x41, 0x0D);
    _write_reg(dev, 0x43, 0x14);

    _write_reg(dev, 0x4B, 0x0E);
    _write_reg(dev, 0x45, 0x0F);
    _write_reg(dev, 0x44, 0x42);
    _write_reg(dev, 0x4C, 0x80);
    _write_reg(dev, 0x7F, 0x10);
    _write_reg(dev, 0x5B, 0x02);
    _write_reg(dev, 0x7F, 0x07);
    _write_reg(dev, 0x40, 0x41);
    _write_reg(dev, 0x70, 0x00);

    ztimer_sleep(ZTIMER_MSEC, 100);
    _write_reg(dev, 0x32, 0x44);
    _write_reg(dev, 0x7F, 0x07);
    _write_reg(dev, 0x40, 0x40);
    _write_reg(dev, 0x7F, 0x06);
    _write_reg(dev, 0x62, 0xF0);
    _write_reg(dev, 0x63, 0x00);
    _write_reg(dev, 0x7F, 0x0D);
    _write_reg(dev, 0x48, 0xC0);
    _write_reg(dev, 0x6F, 0xD5);
    _write_reg(dev, 0x7F, 0x00);

    _write_reg(dev, 0x5B, 0xA0);
    _write_reg(dev, 0x4E, 0xA8);
    _write_reg(dev, 0x5A, 0x50);
    _write_reg(dev, 0x40, 0x80);

    ztimer_sleep(ZTIMER_MSEC, 100);
    _write_reg(dev, 0x7F, 0x14);
    _write_reg(dev, 0x6F, 0x1C);
    _write_reg(dev, 0x7F, 0x00);
}
