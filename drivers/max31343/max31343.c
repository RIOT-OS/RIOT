/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       MAX31343 RTC driver implementation
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include "bcd.h"
#include "max31343.h"
#include "max31343_params.h"
#include "max31343_internal.h"
#include "periph/i2c.h"

static int _read_regs(const max31343_t *dev, uint8_t reg, void *buf, size_t len)
{
    assert(dev);
    assert(buf);
    assert(len > 0);

    return i2c_read_regs(dev->i2c, MAX31343_I2C_ADDR, reg, buf, len, 0);
}

static int _write_regs(const max31343_t *dev, uint8_t reg, const void *buf, size_t len)
{
    assert(dev);
    assert(buf);
    assert(len > 0);

    return i2c_write_regs(dev->i2c, MAX31343_I2C_ADDR, reg, buf, len, 0);
}

static inline int _read_reg(const max31343_t *dev, uint8_t reg, uint8_t *val)
{
    return _read_regs(dev, reg, val, 1);
}

static inline int _write_reg(const max31343_t *dev, uint8_t reg, uint8_t val)
{
    return _write_regs(dev, reg, &val, 1);
}

static int _validate_tm(const struct tm *t)
{
    assert(t);

    if (t->tm_sec < 0 || t->tm_sec > 59 ||
        t->tm_min < 0 || t->tm_min > 59 ||
        t->tm_hour < 0 || t->tm_hour > 23 ||
        t->tm_mday < 1 || t->tm_mday > 31 ||
        t->tm_mon < 0 || t->tm_mon > 11) {
        return -ERANGE;
    }

    return 0;
}

static int _validate_tm_for_set(const struct tm *t)
{
    int res = _validate_tm(t);
    if (res != 0) {
        return res;
    }

    int year = t->tm_year + 1900;
    if (year < 2000 || year > 2099) {
        return -ERANGE;
    }

    if (t->tm_wday < 0 || t->tm_wday > 6) {
        return -ERANGE;
    }

    return 0;
}

static void _encode_time_regs(const struct tm *t, uint8_t raw[MAX31343_TIME_LEN])
{
    int year_full = t->tm_year + 1900;
    uint8_t year2 = (uint8_t)(year_full % 100);

    raw[0] = bcd_from_byte(t->tm_sec);
    raw[1] = bcd_from_byte(t->tm_min);
    raw[2] = bcd_from_byte(t->tm_hour);
    raw[3] = (t->tm_wday + 1U);
    raw[4] = bcd_from_byte(t->tm_mday);

    uint8_t m = (t->tm_mon + 1);
    uint8_t m_bcd = (bcd_from_byte(m) & 0x1FU);
    raw[5] = (m_bcd | MAX31343_MONTH_CENTURY);
    raw[6] = bcd_from_byte(year2);
}

static void _decode_time_regs(const uint8_t raw[MAX31343_TIME_LEN], struct tm *t)
{
    memset(t, 0, sizeof(*t));

    int sec  = bcd_to_byte(raw[0] & 0x7FU);
    int min  = bcd_to_byte(raw[1] & 0x7FU);
    int hour = bcd_to_byte(raw[2] & 0x3FU);
    int wday = (raw[3] & 0x07U);
    int mday = bcd_to_byte(raw[4] & 0x3FU);
    int mon = bcd_to_byte(raw[5] & 0x1FU);
    int year = bcd_to_byte(raw[6]);
    bool century = (raw[5] & MAX31343_MONTH_CENTURY);

    t->tm_sec  = sec;
    t->tm_min  = min;
    t->tm_hour = hour;
    t->tm_mday = mday;
    t->tm_mon  = mon - 1;

    int full_year = (century ? 2000 : 1900) + year;
    t->tm_year = full_year - 1900;

    /* Map weekday: chip 1..7 -> tm 0..6 */
    t->tm_wday = (wday >= 1 && wday <= 7) ? (wday - 1) : 0;

    t->tm_yday = 0;
    t->tm_isdst = -1;
}

static int _alarm1_set_enable(const max31343_t *dev, bool en)
{
    uint8_t ie = 0;
    int res = _read_reg(dev, MAX31343_REG_INT_EN, &ie);
    if (res != 0) {
        return res;
    }

    if (en) {
        ie |= MAX31343_INTEN_A1IE;
    } else {
        ie &= (uint8_t)~MAX31343_INTEN_A1IE;
    }

    return _write_reg(dev, MAX31343_REG_INT_EN, ie);
}

int max31343_init(max31343_t *dev, const max31343_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }

    dev->i2c = params->i2c;

    uint8_t status;
    i2c_acquire(dev->i2c);
    int res = _read_reg(dev, MAX31343_REG_STATUS, &status);
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    bool time_lost = (status & MAX31343_STATUS_OSF) != 0;

    res = max31343_poweron(dev);
    if (res != 0) {
        return res;
    }

    res = max31343_set_sqw(dev, params->sqw_freq);
    if (res != 0) {
        return res;
    }

    if (params->trickle_enable) {
        res = max31343_trickle_charge_enable(dev,
                                             params->trickle_diode,
                                             params->trickle_res);
    }
    else {
        res = max31343_trickle_charge_disable(dev);
    }
    if (res != 0) {
        return res;
    }

    return time_lost ? -ENODATA : 0;
}

int max31343_get_time(const max31343_t *dev, struct tm *time)
{
    assert(dev);
    assert(time);

    uint8_t raw[MAX31343_TIME_LEN];
    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MAX31343_REG_TIME0, raw, sizeof(raw));
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    _decode_time_regs(raw, time);

    return _validate_tm(time) == 0 ? 0 : -EIO;
}

int max31343_set_time(const max31343_t *dev, const struct tm *time)
{
    assert(dev);
    assert(time);

    int res = _validate_tm_for_set(time);
    if (res != 0) {
        return res;
    }

    uint8_t raw[MAX31343_TIME_LEN] = {0};
    _encode_time_regs(time, raw);

    i2c_acquire(dev->i2c);
    res = _write_regs(dev, MAX31343_REG_TIME0, raw, sizeof(raw));
    i2c_release(dev->i2c);
    return res;
}

int max31343_set_alarm(const max31343_t *dev, const struct tm *time)
{
    assert(dev);
    assert(time);

    int res = _validate_tm_for_set(time);
    if (res != 0) {
        return res;
    }

    uint8_t alarm_reg[MAX31343_ALM1_LEN];

    alarm_reg[0] = (uint8_t)(bcd_from_byte((uint8_t) time->tm_sec) & 0x7FU);
    alarm_reg[1] = (uint8_t)(bcd_from_byte((uint8_t) time->tm_min) & 0x7FU);
    alarm_reg[2] = (uint8_t)(bcd_from_byte((uint8_t) time->tm_hour) & 0x3FU);

    alarm_reg[3] = (uint8_t)(bcd_from_byte((uint8_t) time->tm_mday) & 0x3FU);

    uint8_t mon = (uint8_t)(time->tm_mon + 1);
    alarm_reg[4] = (uint8_t)(bcd_from_byte(mon) & 0x1FU);

    int year_full = time->tm_year + 1900;
    uint8_t year2 = (uint8_t)(year_full % 100);
    alarm_reg[5] = bcd_from_byte(year2);

    i2c_acquire(dev->i2c);

    /* Disable alarm interrupt before writing registers */
    res = _alarm1_set_enable(dev, false);
    if (res == 0) {
        res = _write_regs(dev, MAX31343_REG_ALM1_SEC, alarm_reg, sizeof(alarm_reg));
    }
    i2c_release(dev->i2c);
    return res;
}

int max31343_get_alarm(const max31343_t *dev, struct tm *time)
{
    assert(dev);
    assert(time);

    uint8_t alarm_reg[MAX31343_ALM1_LEN];
    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MAX31343_REG_ALM1_SEC, alarm_reg, sizeof(alarm_reg));
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    if ((alarm_reg[0] & MAX31343_ALM_MASK_BIT) || (alarm_reg[1] & MAX31343_ALM_MASK_BIT) ||
        (alarm_reg[2] & MAX31343_ALM_MASK_BIT) || (alarm_reg[3] & MAX31343_ALM_MASK_BIT) ||
        (alarm_reg[4] & (MAX31343_ALM_MASK_BIT | MAX31343_ALM_DY_DT))) {
        return -ENOENT;
    }

    memset(time, 0, sizeof(*time));

    time->tm_sec = bcd_to_byte(alarm_reg[0] & 0x7FU);
    time->tm_min = bcd_to_byte(alarm_reg[1] & 0x7FU);
    time->tm_hour = bcd_to_byte(alarm_reg[2] & 0x3FU);
    time->tm_mday = bcd_to_byte(alarm_reg[3] & 0x3FU);
    time->tm_mon = bcd_to_byte(alarm_reg[4] & 0x1FU) - 1;

    int year = bcd_to_byte(alarm_reg[5]);
    time->tm_year = (2000 + year) - 1900;

    time->tm_isdst = -1;
    return 0;
}

int max31343_alarm_enable(const max31343_t *dev, bool enable)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    int res = _alarm1_set_enable(dev, enable);
    i2c_release(dev->i2c);
    return res;
}

int max31343_clear_alarm(const max31343_t *dev)
{
    assert(dev);

    i2c_acquire(dev->i2c);

    int res = _alarm1_set_enable(dev, false);

    if (res == 0) {
        uint8_t st;
        (void) _read_reg(dev, MAX31343_REG_STATUS, &st);
    }

    i2c_release(dev->i2c);
    return res;
}

int max31343_poweron(const max31343_t *dev)
{
    assert(dev);

    uint8_t cfg1 = 0;

    i2c_acquire(dev->i2c);

    int res = _read_reg(dev, MAX31343_REG_RTC_CFG1, &cfg1);
    if (res == 0) {
        cfg1 |= MAX31343_RTC_CFG1_ENOSC;
        res = _write_reg(dev, MAX31343_REG_RTC_CFG1, cfg1);
    }

    i2c_release(dev->i2c);

    return res;
}

int max31343_poweroff(const max31343_t *dev)
{
    assert(dev);

    uint8_t cfg1 = 0;

    i2c_acquire(dev->i2c);

    int res = _read_reg(dev, MAX31343_REG_RTC_CFG1, &cfg1);
    if (res == 0) {
        cfg1 &= (uint8_t)~MAX31343_RTC_CFG1_ENOSC;
        res = _write_reg(dev, MAX31343_REG_RTC_CFG1, cfg1);
    }

    i2c_release(dev->i2c);

    return res;
}

int max31343_set_sqw(const max31343_t *dev, max31343_sqw_freq_t freq)
{
    assert(dev);

    if ((unsigned) freq > 7U) {
        return -ERANGE;
    }

    uint8_t cfg2 = 0;
    i2c_acquire(dev->i2c);

    int res = _read_reg(dev, MAX31343_REG_RTC_CFG2, &cfg2);
    if (res == 0) {
        cfg2 = (uint8_t)((cfg2 & ~MAX31343_RTC_CFG2_SQW_HZ_MASK) |
                          ((uint8_t)freq & MAX31343_RTC_CFG2_SQW_HZ_MASK));
        res = _write_reg(dev, MAX31343_REG_RTC_CFG2, cfg2);
    }

    i2c_release(dev->i2c);
    return res;
}

int max31343_get_temp(const max31343_t *dev, int16_t *temp_centi)
{
    assert(dev);
    assert(temp_centi);

    uint8_t buf[2] = {0};
    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MAX31343_REG_TEMP_MSB, buf, sizeof(buf));
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    int16_t raw16 = (int16_t)((uint16_t) buf[0] << 8 | (uint16_t) buf[1]);

    int16_t qdeg = (int16_t)(raw16 >> 6);

    *temp_centi = (int16_t)(qdeg * 25);

    return 0;
}

int max31343_trickle_charge_enable(const max31343_t *dev,
                                   max31343_trickle_diode_t diode,
                                   max31343_trickle_res_t res)
{
    assert(dev);

    uint8_t tche = (uint8_t)(MAX31343_TRICKLE_TCHE_ENABLE << MAX31343_TRICKLE_TCHE_SHIFT);
    uint8_t d_trickle = (uint8_t)(((uint8_t)diode << 2)
                                  | ((uint8_t)res & MAX31343_TRICKLE_D_RES_MASK));
    uint8_t reg = (uint8_t)(tche | (d_trickle & MAX31343_TRICKLE_D_MASK));

    i2c_acquire(dev->i2c);
    int ret = _write_reg(dev, MAX31343_REG_TRICKLE, reg);
    i2c_release(dev->i2c);
    return ret;
}

int max31343_trickle_charge_disable(const max31343_t *dev)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    int res = _write_reg(dev, MAX31343_REG_TRICKLE, 0x00U);
    i2c_release(dev->i2c);
    return res;
}

int max31343_temp_set_automode(const max31343_t *dev, bool enable, max31343_ttsint_t ttsint)
{
    assert(dev);

    if ((unsigned) ttsint > 7U) {
        return -ERANGE;
    }

    uint8_t ts = 0;

    i2c_acquire(dev->i2c);

    int res = _read_reg(dev, MAX31343_REG_TS_CONFIG, &ts);
    if (res == 0) {
        if (enable) {
            ts |= MAX31343_TS_AUTOMODE;
        } else {
            ts &= (uint8_t)~MAX31343_TS_AUTOMODE;
        }

        /* set TTSINT field */
        ts &= (uint8_t)~MAX31343_TS_TTSINT_MASK;
        ts |= (uint8_t)(((uint8_t)ttsint << MAX31343_TS_TTSINT_SHIFT)
                & MAX31343_TS_TTSINT_MASK);

        res = _write_reg(dev, MAX31343_REG_TS_CONFIG, ts);
    }

    i2c_release(dev->i2c);
    return res;
}

#ifdef MODULE_WALLTIME_IMPL_MAX31343

static max31343_t _walltime_dev;
static bool _walltime_init_done;

void walltime_impl_init(void)
{
    int res = max31343_init(&_walltime_dev, &max31343_params[0]);
    _walltime_init_done = (res == 0 || res == -ENODATA);
}

int walltime_impl_get(struct tm *time, uint16_t *ms)
{
    if (!_walltime_init_done) {
        return -ENODEV;
    }
    *ms = 0;
    return max31343_get_time(&_walltime_dev, time);
}

int walltime_impl_set(struct tm *time)
{
    if (!_walltime_init_done) {
        return -ENODEV;
    }
    return max31343_set_time(&_walltime_dev, time);
}
#endif /* MODULE_WALLTIME_IMPL_MAX31343 */
