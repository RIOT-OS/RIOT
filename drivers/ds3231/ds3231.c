/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ds3231
 * @{
 *
 * @file
 * @brief       DS3231 RTC driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "bcd.h"
#include "mutex.h"
#include "ds3231.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/* some metadata about the devices registers */
#define DATE_REG_NUMOF      7U
#define A1_REG_NUMOF        4U
#define A2_REG_NUMOF        3U

/* register addresses */
#define REG_SEC             0x00
#define REG_MIN             0x01
#define REG_HOUR            0x02
#define REG_DAY             0x03
#define REG_DATE            0x04
#define REG_MONTH           0x05
#define REG_YEAR            0x06
#define REG_A1_SEC          0x07
#define REG_A1_MIN          0x08
#define REG_A1_HOUR         0x09
#define REG_A1_DAYDATE      0x0a
#define REG_A2_MIN          0x0b
#define REG_A2_HOUR         0x0c
#define REG_A2_DAYDATE      0x0d
#define REG_CTRL            0x0e
#define REG_STATUS          0x0f
#define REG_AGING_OFFSET    0x10
#define REG_TEMP_MSB        0x11
#define REG_TEMP_LSB        0x12

/* general register bitmasks */
#define MASK_SEC10          0x70
#define MASK_SEC            0x0f
#define MASK_MIN10          0x70
#define MASK_MIN            0x0f
#define MASK_H10            0x10
#define MASK_AMPM_H20       0x20
#define MASK_H12_H24        0x40
#define MASK_H20H10         (MASK_H10 | MASK_AMPM_H20)
#define MASK_HOUR           0x0f
#define MASK_DAY            0x07
#define MASK_DATE10         0x30
#define MASK_DATE           0x0f
#define MASK_MONTH10        0x10
#define MASK_MONTH          0x0f
#define MASK_CENTURY        0x80
#define MASK_DY_DT          0x60

/* control register bitmaps */
#define CTRL_EOSC           0x80
#define CTRL_BBSQW          0x40
#define CTRL_CONV           0x20
#define CTRL_RS2            0x10
#define CTRL_RS1            0x08
#define CTRL_RS             (CTRL_RS2 | CTRL_RS1)
#define CTRL_INTCN          0x04
#define CTRL_A2IE           0x02
#define CTRL_A1IE           0x01
#define CTRL_AIE            (CTRL_A2IE | CTRL_A1IE)

/* status register bitmaps */
#define STAT_OSF            0x80
#define STAT_EN32KHZ        0x08
#define STAT_BSY            0x04
#define STAT_A2F            0x02
#define STAT_A1F            0x01
#define STAT_AF             (STAT_A2F | STAT_A1F)

static int _read(const ds3231_t *dev, uint8_t reg, uint8_t *buf, size_t len,
                 int acquire, int release)
{
    int res;

    if (acquire) {
        i2c_acquire(dev->bus);
    }
    res = i2c_read_regs(dev->bus, DS3231_I2C_ADDR, reg, buf, len, 0);
    if (res < 0) {
        i2c_release(dev->bus);
        return -EIO;
    }
    if (release) {
        i2c_release(dev->bus);
    }
    return 0;
}

static int _write(const ds3231_t *dev, uint8_t reg, uint8_t *buf, size_t len,
                  int acquire, int release)
{
    if (acquire) {
        i2c_acquire(dev->bus);
    }
    if (i2c_write_regs(dev->bus, DS3231_I2C_ADDR, reg, buf, len, 0) < 0) {
        i2c_release(dev->bus);
        return -EIO;
    }
    if (release) {
        i2c_release(dev->bus);
    }
    return 0;
}

static int _clrset(const ds3231_t *dev, uint8_t reg,
                   uint8_t clr_mask, uint8_t set_mask, int acquire, int release)
{
    uint8_t old;
    uint8_t new;

    if (_read(dev, reg, &old, 1, acquire, 0) < 0) {
        return -EIO;
    }
    new = ((old &= ~clr_mask) | set_mask);
    if (_write(dev, reg, &new, 1, 0, release) < 0) {
        return -EIO;
    }
    return 0;
}

#if IS_USED(MODULE_DS3231_INT)
static void _unlock(void *m)
{
    mutex_unlock(m);
}
#endif

int ds3231_init(ds3231_t *dev, const ds3231_params_t *params)
{
    int res;

    /* write device descriptor */
    memset(dev, 0, sizeof(ds3231_t));
    dev->bus = params->bus;

#if IS_USED(MODULE_DS3231_INT)
    /* write interrupt pin configuration */
    dev->int_pin = params->int_pin;
#endif

    /* en or disable 32KHz output */
    if (params->opt & DS3221_OPT_32KHZ_ENABLE) {
        res = _clrset(dev, REG_STATUS, 0, STAT_EN32KHZ, 1, 0);
    }
    else {
        res = _clrset(dev, REG_STATUS, STAT_EN32KHZ, 0, 1, 0);
    }
    if (res != 0) {
        return -EIO;
    }

    /* Configure backup battery */
    uint8_t clr = 0;
    uint8_t set = 0;
    /* if configured, start the oscillator */
    if (params->opt & DS3231_OPT_BAT_ENABLE) {
        clr |= CTRL_EOSC;
    }
    else {
        set = CTRL_EOSC;
    }

    /* if configured, enable the interrupts (no SQW output) */
    if (params->opt & DS3231_OPT_INTER_ENABLE) {
        set |= CTRL_INTCN;
    }
    else {
        clr |= CTRL_INTCN;
    }

    return _clrset(dev, REG_CTRL, clr, set, 0, 1);
}

int ds3231_get_time(const ds3231_t *dev, struct tm *time)
{
    uint8_t raw[DATE_REG_NUMOF];

    /* read date registers */
    if (_read(dev, REG_SEC, raw, DATE_REG_NUMOF, 1, 1) < 0) {
        return -EIO;
    }

    /* convert data to struct tm */
    time->tm_sec = bcd_to_byte(raw[REG_SEC]);
    time->tm_min = bcd_to_byte(raw[REG_MIN]);
    if (raw[REG_HOUR] & MASK_H12_H24) {
        time->tm_hour = bcd_to_byte(raw[REG_HOUR] & (MASK_HOUR | MASK_H10));
        if (raw[REG_HOUR] & MASK_AMPM_H20) {
            time->tm_hour += 12;
        }
    }
    else {
        time->tm_hour = bcd_to_byte(raw[REG_HOUR] & (MASK_HOUR | MASK_H20H10));
    }
    time->tm_mday = bcd_to_byte(raw[REG_DATE]);
    time->tm_mon = bcd_to_byte(raw[REG_MONTH] & ~MASK_CENTURY) - 1;
    time->tm_year = bcd_to_byte(raw[REG_YEAR]);
    if (raw[REG_MONTH] & MASK_CENTURY) {
        time->tm_year += 100;
    }
    time->tm_wday = bcd_to_byte(raw[REG_DAY]) - 1;

    return 0;
}

int ds3231_set_time(const ds3231_t *dev, const struct tm *time)
{
    uint8_t raw[DATE_REG_NUMOF];

    /* some validity checks */
    if (time->tm_year > 200) {
        return -ERANGE;
    }

    /* convert struct tm to raw BDC data */
    raw[REG_SEC] = bcd_from_byte(time->tm_sec);
    raw[REG_MIN] = bcd_from_byte(time->tm_min);
    /* note: we always set the hours in 24-hour format */
    raw[REG_HOUR] = bcd_from_byte(time->tm_hour);
    raw[REG_DAY] = bcd_from_byte(time->tm_wday + 1);
    raw[REG_DATE] = bcd_from_byte(time->tm_mday);
    raw[REG_MONTH] = bcd_from_byte(time->tm_mon + 1);
    raw[REG_YEAR] = bcd_from_byte(time->tm_year % 100);
    if (time->tm_year > 100) {
        raw[REG_MONTH] |= MASK_CENTURY;
    }

    /* write time to device */
    if (_write(dev, REG_SEC, raw, DATE_REG_NUMOF, 1, 1) < 0) {
        return -EIO;
    }

    return 0;
}

#if IS_USED(MODULE_DS3231_INT)
int ds3231_await_alarm(ds3231_t *dev)
{
    mutex_t mutex = MUTEX_INIT_LOCKED;

    assert(dev != NULL);
    assert(gpio_is_valid(dev->int_pin));

    if (gpio_init_int(dev->int_pin, GPIO_IN, GPIO_FALLING, _unlock, &mutex) < 0) {
            return -EIO;
    }

    /* wait for alarm */
    mutex_lock(&mutex);

    gpio_irq_disable(dev->int_pin);

    uint8_t status, tmp;
    _read(dev, REG_STATUS, &status, 1, 1, 0);

    /* clear interrupt flags */
    tmp = status & ~(STAT_A1F | STAT_A2F);
    if (_write(dev, REG_STATUS, &tmp, 1, 0, 1) < 0) {
        return -EIO;
    }

    return status & (STAT_A1F | STAT_A2F);
}
#endif

int ds3231_set_alarm_1(const ds3231_t *dev, struct tm *time,
                       ds3231_alm_1_mode_t trigger)
{
    uint8_t raw[A1_REG_NUMOF];
    uint8_t a1mx_mask[A1_REG_NUMOF];

    /* A1M1, A1M2, A1M3 and A1M4 are set accordingly to the trigger type */
    a1mx_mask[0] = (trigger & 0x01) << 7;
    a1mx_mask[1] = (trigger & 0x02) << 6;
    a1mx_mask[2] = (trigger & 0x04) << 5;
    a1mx_mask[3] = (trigger & 0x08) << 4;

    raw[0] = ((bcd_from_byte(time->tm_sec)  & (MASK_SEC10  | MASK_SEC))
            | a1mx_mask[0]);
    raw[1] = ((bcd_from_byte(time->tm_min)  & (MASK_MIN10  | MASK_MIN))
            | a1mx_mask[1]);
    /* note: we always set the hours in 24-hour format */
    raw[2] = ((bcd_from_byte(time->tm_hour) & (MASK_H20H10 | MASK_HOUR))
            | a1mx_mask[2]);
    raw[3] = ((bcd_from_byte(time->tm_wday  + 1) & (MASK_DATE10 | MASK_DATE))
            | a1mx_mask[3]);

    /* write alarm configuration to device */
    if (_write(dev, REG_A1_SEC, raw, A1_REG_NUMOF, 1, 1) < 0) {
        return -EIO;
    }

    /* activate alarm 1 in case it was not */
    if (_clrset(dev, REG_CTRL, 0, CTRL_A1IE, 1, 1) < 0){
        return -EIO;
    }

    return 0;
}

int ds3231_set_alarm_2(const ds3231_t *dev, struct tm *time,
                       ds3231_alm_2_mode_t trigger)
{
    uint8_t raw[A2_REG_NUMOF];
    uint8_t a2mx_mask[A2_REG_NUMOF];

    /*A2M2, A2M3 and A2M4 are set accordingly to the trigger type */
    a2mx_mask[0] = (trigger & 0x01) << 7;
    a2mx_mask[1] = (trigger & 0x02) << 6;
    a2mx_mask[2] = (trigger & 0x04) << 5;

    raw[0] = ((bcd_from_byte(time->tm_min)  & (MASK_MIN10  | MASK_MIN))
            | a2mx_mask[0]);
    /* note: we always set the hours in 24-hour format */
    raw[1] = ((bcd_from_byte(time->tm_hour) & (MASK_H20H10 | MASK_HOUR))
            | a2mx_mask[1]);
    raw[2] = ((bcd_from_byte(time->tm_wday  + 1) & (MASK_DATE10 | MASK_DATE))
            | a2mx_mask[2]);

    /* write alarm configuration to device */
    if (_write(dev, REG_A2_MIN, raw, A2_REG_NUMOF, 1, 1) < 0) {
        return -EIO;
    }

    /* activate alarm 2 in case it was not */
    if (_clrset(dev, REG_CTRL, 0, CTRL_A2IE, 1, 1) < 0){
        return -EIO;
    }

    return 0;
}

int ds3231_toggle_alarm_1(const ds3231_t *dev, bool enable)
{
    if (enable){
        return _clrset(dev, REG_CTRL, 0, CTRL_A1IE, 1, 1);
    }
    else{
        return _clrset(dev, REG_CTRL, CTRL_A1IE, 0, 1, 1);
    }
}

int ds3231_toggle_alarm_2(const ds3231_t *dev, bool enable)
{
    if (enable){
        return _clrset(dev, REG_CTRL, 0, CTRL_A2IE, 1, 1);
    }
    else{
        return _clrset(dev, REG_CTRL, CTRL_A2IE, 0, 1, 1);
    }
}

int ds3231_clear_alarm_1_flag(const ds3231_t *dev)
{
    return _clrset(dev, REG_STATUS, STAT_A1F, 0, 1, 1);
}

int ds3231_clear_alarm_2_flag(const ds3231_t *dev)
{
    return _clrset(dev, REG_STATUS, STAT_A2F, 0, 1, 1);

}

int ds3231_get_alarm_1_flag(const ds3231_t *dev, bool *flag)
{
    uint8_t raw;
    int res = _read(dev, REG_STATUS, &raw, 1, 1, 1);
    if (res != 0) {
        return -EIO;
    }

    *flag = (raw & STAT_A1F);

    return res;
}

int ds3231_get_alarm_2_flag(const ds3231_t *dev, bool *flag)
{
    uint8_t raw;
    int res = _read(dev, REG_STATUS, &raw, 1, 1, 1);
    if (res != 0) {
        return -EIO;
    }

    *flag = (raw & STAT_A2F) >> 1;

    return res;
}

int ds3231_get_aging_offset(const ds3231_t *dev, int8_t *offset)
{
    return _read(dev, REG_AGING_OFFSET, (uint8_t *)offset, 1, 1, 1);
}

int ds3231_set_aging_offset(const ds3231_t *dev, int8_t offset)
{
    return _write(dev, REG_AGING_OFFSET, (uint8_t *)&offset, 1, 1, 1);
}

int ds3231_get_temp(const ds3231_t *dev, int16_t *temp)
{
    uint8_t raw[2];
    int res = _read(dev, REG_TEMP_MSB, raw, 2, 1, 1);
    if (res != 0) {
        return -EIO;
    }
    *temp = ((((int16_t)raw[0] << 8) | raw[1]) >> 6) * 25;
    return 0;
}

int ds3231_enable_bat(const ds3231_t *dev)
{
    return _clrset(dev, REG_CTRL, CTRL_EOSC, 0, 1, 1);
}

int ds3231_disable_bat(const ds3231_t *dev)
{
    return _clrset(dev, REG_CTRL, 0, CTRL_EOSC, 1, 1);
}
