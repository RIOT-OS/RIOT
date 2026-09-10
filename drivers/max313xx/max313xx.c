/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       MAX313xx RTC driver implementation
 *
 * @author      Jakob Müller <ja.mueller@tuhh.de>
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "bcd.h"
#include "rtc_utils.h"
#include "periph/i2c.h"
#include "kernel_defines.h"

#include "max313xx.h"
#include "max313xx_internal.h"
#include "max313xx_params.h"

static int _read_regs(const max313xx_t *dev, uint8_t reg, void *buf, size_t len)
{
    return i2c_read_regs(dev->i2c, MAX313XX_I2C_ADDR, reg, buf, len, 0);
}

static int _write_regs(const max313xx_t *dev, uint8_t reg, const void *buf, size_t len)
{
    return i2c_write_regs(dev->i2c, MAX313XX_I2C_ADDR, reg, buf, len, 0);
}

static int _validate_tm_for_set(const struct tm *t)
{
    if (!rtc_tm_valid(t)) {
        return -ERANGE;
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

static int _alarm1_set_enable(const max313xx_t *dev, bool en)
{
    uint8_t ie = 0;
    int res = _read_regs(dev, MAX313XX_REG_INT_EN, &ie, 1);
    if (res != 0) {
        return res;
    }

    if (en) {
        ie |= MAX313XX_INTEN_A1IE;
    } else {
        ie &= ~MAX313XX_INTEN_A1IE;
    }

    return _write_regs(dev, MAX313XX_REG_INT_EN, &ie, 1);
}

int max313xx_init(max313xx_t *dev, const max313xx_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }

    dev->i2c = params->i2c;

    uint8_t status;
    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MAX313XX_REG_STATUS, &status, 1);
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    bool time_lost = (status & MAX313XX_STATUS_OSF) != 0;

    res = max313xx_poweron(dev);
    if (res != 0) {
        return res;
    }

    return time_lost ? -ENODATA : 0;
}

int max313xx_get_time(const max313xx_t *dev, struct tm *time)
{
    assert(dev);
    assert(time);

    uint8_t raw[MAX313XX_TIME_LEN];
    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MAX313XX_REG_TIME0, raw, sizeof(raw));
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    memset(time, 0, sizeof(*time));

    time->tm_sec  = bcd_to_byte(raw[0] & 0x7FU);
    time->tm_min  = bcd_to_byte(raw[1] & 0x7FU);
    time->tm_hour = bcd_to_byte(raw[2] & 0x3FU);
    int wday = (raw[3] & 0x07U);
    /* Map weekday: chip 1..7 -> tm 0..6 */
    time->tm_wday = (wday >= 1 && wday <= 7) ? (wday - 1) : 0;

    time->tm_mday = bcd_to_byte(raw[4] & 0x3FU);
    time->tm_mon  = bcd_to_byte(raw[5] & 0x1FU) - 1;
    int year = bcd_to_byte(raw[6]);
    bool century = (raw[5] & MAX313XX_MONTH_CENTURY);

    int full_year = (century ? 2000 : 1900) + year;
    time->tm_year = full_year - 1900;

    time->tm_yday = 0;
    time->tm_isdst = -1;

    if (rtc_tm_valid(time)) {
        return 0;
    }
    else {
        return -EIO;
    }
}

int max313xx_set_time(const max313xx_t *dev, const struct tm *time)
{
    assert(dev);
    assert(time);

    int res = _validate_tm_for_set(time);
    if (res != 0) {
        return res;
    }

    uint8_t raw[MAX313XX_TIME_LEN] = {0};

    raw[0] = bcd_from_byte(time->tm_sec);
    raw[1] = bcd_from_byte(time->tm_min);
    raw[2] = bcd_from_byte(time->tm_hour);
    raw[3] = (time->tm_wday + 1U);
    raw[4] = bcd_from_byte(time->tm_mday);

    uint8_t m_bcd = (bcd_from_byte(time->tm_mon + 1) & 0x1FU);
    raw[5] = (m_bcd | MAX313XX_MONTH_CENTURY);

    uint8_t year = (uint8_t)((time->tm_year + 1900) % 100);
    raw[6] = bcd_from_byte(year);

    i2c_acquire(dev->i2c);
    res = _write_regs(dev, MAX313XX_REG_TIME0, raw, sizeof(raw));
    i2c_release(dev->i2c);
    return res;
}

int max313xx_set_alarm(const max313xx_t *dev, const struct tm *time)
{
    assert(dev);
    assert(time);

    int res = _validate_tm_for_set(time);
    if (res != 0) {
        return res;
    }

    uint8_t alarm_reg[MAX313XX_ALM1_LEN];

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
        res = _write_regs(dev, MAX313XX_REG_ALM1_SEC, alarm_reg, sizeof(alarm_reg));
    }
    i2c_release(dev->i2c);
    return res;
}

int max313xx_get_alarm(const max313xx_t *dev, struct tm *time)
{
    assert(dev);
    assert(time);

    uint8_t alarm_reg[MAX313XX_ALM1_LEN];
    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MAX313XX_REG_ALM1_SEC, alarm_reg, sizeof(alarm_reg));
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
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

int max313xx_set_alarm_int(const max313xx_t *dev, bool enable)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    int res = _alarm1_set_enable(dev, enable);

    if (res == 0 && !enable) {
        uint8_t st;
        (void) _read_regs(dev, MAX313XX_REG_STATUS, &st, 1);
    }

    i2c_release(dev->i2c);
    return res;
}

int max313xx_poweron(const max313xx_t *dev)
{
    assert(dev);

    uint8_t cfg1 = 0;

    i2c_acquire(dev->i2c);

    int res = _read_regs(dev, MAX313XX_REG_RTC_CFG1, &cfg1, 1);
    if (res == 0) {
        cfg1 |= MAX313XX_RTC_CFG1_ENOSC;
        res = _write_regs(dev, MAX313XX_REG_RTC_CFG1, &cfg1, 1);
    }

    i2c_release(dev->i2c);

    return res;
}

int max313xx_poweroff(const max313xx_t *dev)
{
    assert(dev);

    uint8_t cfg1 = 0;

    i2c_acquire(dev->i2c);

    int res = _read_regs(dev, MAX313XX_REG_RTC_CFG1, &cfg1, 1);
    if (res == 0) {
        cfg1 &= (uint8_t)~MAX313XX_RTC_CFG1_ENOSC;
        res = _write_regs(dev, MAX313XX_REG_RTC_CFG1, &cfg1, 1);
    }

    i2c_release(dev->i2c);

    return res;
}

int max313xx_set_sqw(const max313xx_t *dev, max313xx_sqw_freq_t freq)
{
    assert(dev);

    if (!IS_USED(MODULE_MAX31343)) {
        return -ENOTSUP;
    }

    if ((unsigned) freq > 7U) {
        return -ERANGE;
    }

    uint8_t cfg2 = 0;
    i2c_acquire(dev->i2c);

    int res = _read_regs(dev, MAX313XX_REG_RTC_CFG2, &cfg2, 1);
    if (res == 0) {
        cfg2 = (uint8_t)((cfg2 & ~MAX31343_RTC_CFG2_SQW_HZ_MASK) |
                          ((uint8_t)freq & MAX31343_RTC_CFG2_SQW_HZ_MASK));
        res = _write_regs(dev, MAX313XX_REG_RTC_CFG2, &cfg2, 1);
    }

    i2c_release(dev->i2c);
    return res;
}

int max313xx_get_temp(const max313xx_t *dev, int16_t *temp_centi)
{
    assert(dev);
    assert(temp_centi);

    if (!IS_USED(MODULE_MAX31343)) {
        return -ENOTSUP;
    }

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

int max313xx_trickle_charge_enable(const max313xx_t *dev, bool diode,
                                   max313xx_trickle_res_t res)
{
    assert(dev);

    uint8_t reg = 0;

    if (IS_USED(MODULE_MAX31331)) {
        reg = MAX31331_TRICKLE_ENABLE;
        reg |= (diode ? MAX313XX_TRICKLE_D_DIODE : 0x00);
        reg |= ((uint8_t)res << MAX31331_TRICKLE_RES_SHIFT) & MAX313XX_TRICKLE_D_RES_MASK;
    }
    else if (IS_USED(MODULE_MAX31343)) {
        uint8_t tche = (uint8_t)(MAX31343_TRICKLE_TCHE_ENABLE << MAX31343_TRICKLE_TCHE_SHIFT);
        uint8_t d_trickle = (uint8_t)(((uint8_t)diode << 2)
                                    | ((uint8_t)res & MAX313XX_TRICKLE_D_RES_MASK));
        reg = (uint8_t)(tche | d_trickle);
    }

    i2c_acquire(dev->i2c);
    int ret = _write_regs(dev, MAX313XX_REG_TRICKLE, &reg, 1);
    i2c_release(dev->i2c);
    return ret;
}

int max313xx_trickle_charge_disable(const max313xx_t *dev)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    uint8_t reg = 0;

    int res = _read_regs(dev, MAX313XX_REG_TRICKLE, &reg, 1);
    if (res != 0) {
        i2c_release(dev->i2c);
        return res;
    }

    if (IS_USED(MODULE_MAX31331)) {
        reg &= MAX31331_TRICKLE_ENABLE;
    }
    else if (IS_USED(MODULE_MAX31343)) {
        reg &= MAX31343_TRICKLE_TCHE_MASK;
    }

    res = _write_regs(dev, MAX313XX_REG_TRICKLE, &reg, 1);
    i2c_release(dev->i2c);
    return res;
}

int max313xx_temp_set_automode(const max313xx_t *dev, bool enable, max313xx_ttsint_t ttsint)
{
    assert(dev);

    if (!IS_USED(MODULE_MAX31343)) {
        return -ENOTSUP;
    }

    if ((unsigned) ttsint > 7U) {
        return -ERANGE;
    }

    uint8_t ts = 0;

    i2c_acquire(dev->i2c);

    int res = _read_regs(dev, MAX31343_REG_TS_CONFIG, &ts, 1);
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

        res = _write_regs(dev, MAX31343_REG_TS_CONFIG, &ts, 1);
    }

    i2c_release(dev->i2c);
    return res;
}

#if IS_USED(MODULE_WALLTIME_IMPL_MAX31331) || IS_USED(MODULE_WALLTIME_IMPL_MAX31343)
static max313xx_t _walltime_dev;
static bool _walltime_init_done;

void walltime_impl_init(void)
{
    int res = max313xx_init(&_walltime_dev, &max313xx_params[0]);
    _walltime_init_done = (res == 0 || res == -ENODATA);
}

int walltime_impl_get(struct tm *time, uint16_t *ms)
{
    if (!_walltime_init_done) {
        return -ENODEV;
    }
    *ms = 0;
    return max313xx_get_time(&_walltime_dev, time);
}

int walltime_impl_set(struct tm *time)
{
    if (!_walltime_init_done) {
        return -ENODEV;
    }
    return max313xx_set_time(&_walltime_dev, time);
}
#endif
