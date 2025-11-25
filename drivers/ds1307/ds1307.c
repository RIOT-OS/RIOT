/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup drivers_ds1307
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <string.h>

#include "bcd.h"

#include "ds1307.h"
#include "ds1307_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _nvram_read(struct nvram *dev, uint8_t *dst, uint32_t src,
                       size_t size);
static int _nvram_write(struct nvram *dev, const uint8_t *src, uint32_t dst,
                        size_t size);

static uint8_t _convert_12_to_24(uint8_t hour)
{
    if (hour & DS1307_REG_HOUR_12H) {
        uint8_t tmp = bcd_to_byte(hour & DS1307_REG_HOUR_12H_MASK);
        if (hour & DS1307_REG_HOUR_PM) {
            if (tmp < 12) {
                tmp += 12;
            }
        }
        else {
            if (tmp == 12) {
                tmp = 0;
            }
        }
        hour = (bcd_from_byte(tmp) & DS1307_REG_HOUR_24H_MASK);
    }
    return hour;
}

int ds1307_init(ds1307_t *dev, const ds1307_params_t *params)
{
    int res;
    uint8_t hour;

    dev->i2c = params->i2c;

    i2c_acquire(dev->i2c);
    /* normalize hour format */
    res = i2c_read_reg(dev->i2c, DS1307_I2C_ADDRESS, DS1307_REG_HOUR, &hour, 0);
    if (res != 0) { /* should be 0 if device is connected */
        i2c_release(dev->i2c);
        DEBUG("ds1307: Error reading HOUR register on init: %i\n", res);
        return -1;
    }
    res = i2c_write_reg(dev->i2c, DS1307_I2C_ADDRESS, DS1307_REG_HOUR,
                        _convert_12_to_24(hour), 0);
    i2c_release(dev->i2c);

    if (res != 0) {
        DEBUG("ds1307: Error writing HOUR register on init: %i\n", res);
        return -1;
    }
    dev->nvram.read = _nvram_read;
    dev->nvram.write = _nvram_write;
    dev->nvram.size = (DS1307_REG_RAM_LAST - DS1307_REG_RAM_FIRST) + 1;
    dev->nvram.extra = dev;
    return 0;
}

int ds1307_set_time(const ds1307_t *dev, const struct tm *time)
{
    uint8_t regs[DS1307_REG_YEAR - DS1307_REG_SEC + 1];
    int res;

    regs[DS1307_REG_SEC] = (bcd_from_byte(time->tm_sec) & DS1307_REG_SEC_MASK);
    regs[DS1307_REG_MIN] = (bcd_from_byte(time->tm_min) & DS1307_REG_MIN_MASK);
    regs[DS1307_REG_HOUR] = (bcd_from_byte(time->tm_hour) &
                             DS1307_REG_HOUR_24H_MASK);
    regs[DS1307_REG_DOW] = (bcd_from_byte(time->tm_wday + DS1307_DOW_OFFSET) &
                            DS1307_REG_DOW_MASK);
    regs[DS1307_REG_DOM] = (bcd_from_byte(time->tm_mday) & DS1307_REG_DOM_MASK);
    regs[DS1307_REG_MON] = (bcd_from_byte(time->tm_mon + DS1307_MON_OFFSET) &
                            DS1307_REG_MON_MASK);
    regs[DS1307_REG_YEAR] = bcd_from_byte(time->tm_year + DS1307_YEAR_OFFSET);
    i2c_acquire(dev->i2c);
    res = i2c_write_regs(dev->i2c, DS1307_I2C_ADDRESS, DS1307_REG_SEC, regs,
                         sizeof(regs), 0);
    DEBUG("ds1307: wrote bytes %02x %02x %02x %02x %02x %02x %02x to device (result: %i)\n",
          regs[DS1307_REG_SEC], regs[DS1307_REG_MIN], regs[DS1307_REG_HOUR],
          regs[DS1307_REG_DOW], regs[DS1307_REG_DOM], regs[DS1307_REG_MON],
          regs[DS1307_REG_YEAR], res);
    i2c_release(dev->i2c);
    return (res != 0) ? -1 : 0;
}

int ds1307_get_time(const ds1307_t *dev, struct tm *time)
{
    uint8_t regs[DS1307_REG_YEAR - DS1307_REG_SEC + 1];
    int res;

    i2c_acquire(dev->i2c);
    res = i2c_read_regs(dev->i2c, DS1307_I2C_ADDRESS, DS1307_REG_SEC, regs,
                         sizeof(regs), 0);
    DEBUG("ds1307: read bytes %02x %02x %02x %02x %02x %02x %02x from device (result: %i)\n",
          regs[DS1307_REG_SEC], regs[DS1307_REG_MIN], regs[DS1307_REG_HOUR],
          regs[DS1307_REG_DOW], regs[DS1307_REG_DOM], regs[DS1307_REG_MON],
          regs[DS1307_REG_YEAR], res);
    i2c_release(dev->i2c);
    if (res != 0) {
        return -1;
    }
    time->tm_sec = bcd_to_byte(regs[DS1307_REG_SEC] & DS1307_REG_SEC_MASK);
    time->tm_min = bcd_to_byte(regs[DS1307_REG_MIN] & DS1307_REG_MIN_MASK);
    time->tm_hour = bcd_to_byte(regs[DS1307_REG_HOUR] &
                                DS1307_REG_HOUR_24H_MASK);
    time->tm_wday = (bcd_to_byte(regs[DS1307_REG_DOW] & DS1307_REG_DOW_MASK) -
                     DS1307_DOW_OFFSET);
    time->tm_mday = bcd_to_byte(regs[DS1307_REG_DOM] & DS1307_REG_DOM_MASK);
    time->tm_mon = bcd_to_byte(regs[DS1307_REG_MON] & DS1307_REG_MON_MASK) -
                     DS1307_MON_OFFSET;
    time->tm_year = (bcd_to_byte(regs[DS1307_REG_YEAR]) - DS1307_YEAR_OFFSET);
    return 0;
}

int ds1307_halt(const ds1307_t *dev)
{
    int res;
    uint8_t sec;

    i2c_acquire(dev->i2c);
    res = i2c_read_reg(dev->i2c, DS1307_I2C_ADDRESS, DS1307_REG_SEC, &sec, 0);
    if (res != 0) {
        i2c_release(dev->i2c);
        DEBUG("ds1307: Error reading SEC register on halt: %i\n", res);
        return -1;
    }
    sec |= DS1307_REG_SEC_CH;
    res = i2c_write_reg(dev->i2c, DS1307_I2C_ADDRESS, DS1307_REG_SEC, sec, 0);
    i2c_release(dev->i2c);
    return (res != 0) ? -1 : 0;
}

int ds1307_set_sqw_mode(const ds1307_t *dev, ds1307_sqw_mode_t mode)
{
    int res;

    i2c_acquire(dev->i2c);
    res = i2c_write_reg(dev->i2c, DS1307_I2C_ADDRESS, DS1307_REG_SQW_CTL,
                        (uint8_t)mode, 0);
    i2c_release(dev->i2c);
    return res;
}

int ds1307_get_sqw_mode(const ds1307_t *dev)
{
    uint8_t mode;
    int res;

    i2c_acquire(dev->i2c);
    res = i2c_read_reg(dev->i2c, DS1307_I2C_ADDRESS, DS1307_REG_SQW_CTL,
                       &mode, 0);
    i2c_release(dev->i2c);
    return (res < 0) ? res : (int)mode;
}

static int _nvram_read(struct nvram *nvram, uint8_t *dst, uint32_t src,
                       size_t size)
{
    const ds1307_t *dev = nvram->extra;
    int res;

    if ((src + size) > nvram->size) {
        return -3;
    }
    i2c_acquire(dev->i2c);
    res = i2c_read_regs(dev->i2c, DS1307_I2C_ADDRESS,
                        DS1307_REG_RAM_FIRST + src, dst, size, 0);
    i2c_release(dev->i2c);
    return (res == 0) ? (int)size : -1;
}

static int _nvram_write(struct nvram *nvram, const uint8_t *src, uint32_t dst,
                        size_t size)
{
    const ds1307_t *dev = nvram->extra;
    int res;

    if ((dst + size) > nvram->size) {
        return -3;
    }
    i2c_acquire(dev->i2c);
    res = i2c_write_regs(dev->i2c, DS1307_I2C_ADDRESS,
                         DS1307_REG_RAM_FIRST + dst, src, size, 0);
    i2c_release(dev->i2c);
    return (res == 0) ? (int)size : -1;
}

/** @} */
