/*
 * Copyright (C) 2017 RWTH Aachen, Steffen Robertz, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_lc709203f
 * @{
 *
 * @file
 * @brief       lc709203f battery fuel gauge device driver
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @}
 */

#include "lc709203f.h"
#include "assert.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/*
 * does a crc check and returns the checksum
 */
static uint8_t _get_crc(uint8_t *rec_values, uint8_t len)
{
    uint8_t crc = 0x00;
    uint8_t current_byte;
    uint8_t bit;

    for (current_byte = 0; current_byte < len; current_byte++) {
        crc ^= (rec_values[current_byte]);
        for (bit = 8; bit > 0; bit--) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ LC709203F_CRC_POLYNOMIAL;
            }
            else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

int lc709203f_init(lc709203f_t *dev, const lc709203f_params_t *params)
{
    assert(dev && params);
    dev->params = *params;
    dev->bus = params->bus;
    dev->addr = params->addr;
    gpio_init_int(dev->params.alarm_pin, GPIO_IN, GPIO_FALLING, dev->cb, dev->arg);
    return LC709203F_OK;
}

int16_t lc709203f_get_voltage(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_CELL_VOLTAGE, rec_buf, 3, 0);
    i2c_release(dev->bus);
    if (control != 0) {
        DEBUG("get_voltage(): Error  reading or writing\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_CELL_VOLTAGE, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_rsoc(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_RSOC, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_rsoc(): Error  reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_RSOC, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_ite(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_ITE, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_rsoc(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_ITE, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_id(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_ID, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_id(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_ID, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_cell_temp(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_CELL_TEMP, rec_buf, 3, 0);
    i2c_release(dev->bus);
    if (control != 0) {
        DEBUG("get_cell_temp(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_CELL_TEMP, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return ((((unsigned int)rec_buf[1] << 8) | rec_buf[0]) - 2731.5);
}

lc709203f_temp_obtaining_mode_t lc709203f_get_status_bit(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_STATUS, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_status_bit(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_STATUS, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

lc709203f_power_mode_t lc709203f_get_power_mode(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_POWER_MODE, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_power_mode(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_POWER_MODE, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_alarm_low_voltage(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_ALARM_VOLTAGE, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_alarm_low_voltage(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_ALARM_VOLTAGE, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_alarm_low_rsoc(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_ALARM_RSOC, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_alarm_low_rsoc(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_ALARM_RSOC, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_change_of_parameter(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_CHANGE_PARAMETER, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_change_of_parameter(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_CHANGE_PARAMETER, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_apt(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_APT, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_apt(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_APT, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_apa(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_APA, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_apa(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_APA, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

lc709203f_current_direction_t lc709203f_get_current_direction(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_CURRENT_DIRECTION, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_current_direction(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_CURRENT_DIRECTION, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

int16_t lc709203f_get_thermistor_b(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t rec_buf[3];
    i2c_acquire(dev->bus);
    int8_t control = i2c_read_regs(dev->bus, dev->addr, LC709203F_REG_THERMISTOR, rec_buf, 3, 0);
    i2c_release(dev->bus);

    if (control != 0) {
        DEBUG("get_thermistor_b(): Error reading\n");
        return 0;
    }
    uint8_t crc_buf[5] = { dev->addr << 1, LC709203F_REG_THERMISTOR, ((dev->addr << 1) | 0x01), rec_buf[0], rec_buf[1] };
    if (_get_crc(crc_buf, 5) != rec_buf[2]) {
        DEBUG("CRC Error \n");
        return 0;
    }
    return (((unsigned int)rec_buf[1] << 8) | rec_buf[0]);
}

void lc709203f_set_rsoc_before(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_BEFORE_RSOC, 0x55, 0xAA };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_thermistor_b(const lc709203f_t *dev, const unsigned int value)
{
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_THERMISTOR, value, value << 8 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_rsoc_initial(const lc709203f_t *dev)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_INITIAL_RSOC, 0x55, 0xAA };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

int8_t lc709203f_set_cell_temp(const lc709203f_t *dev, const unsigned int value)
{
    assert(dev);
    if (!(value >= 0x9e4 && value <= 0xD04)) {
        DEBUG("gauge_set_cell_temp(): temp outside of range \n");
        return LC709203F_CELL_TEMP_INVALID;
    }
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_CELL_TEMP, value, value << 8 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };
    i2c_acquire(dev->bus);
    int8_t ret_val = i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
    return ret_val;
}

void lc709203f_set_current_direction(const lc709203f_t *dev, const lc709203f_current_direction_t direction)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_CURRENT_DIRECTION, (uint8_t)direction, (uint8_t)direction << 8 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_apa(const lc709203f_t *dev, const uint8_t value)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_APA, value, 0 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_apt(const lc709203f_t *dev, const unsigned int value)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_APT, value, value << 8 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_change_of_parameter(const lc709203f_t *dev, const lc709203f_battery_profile_t value)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_CHANGE_PARAMETER, (uint8_t) value, (uint8_t) value << 8 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_alarm_low_rsoc(const lc709203f_t *dev, const uint8_t value)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_ALARM_RSOC, value, 0 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_alarm_low_cell_voltage(const lc709203f_t *dev, const unsigned int value)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_ALARM_VOLTAGE, value, value << 8 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_power_mode(const lc709203f_t *dev, const lc709203f_power_mode_t value)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_POWER_MODE, (uint8_t) value, 0 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}

void lc709203f_set_status_bit(const lc709203f_t *dev, const lc709203f_temp_obtaining_mode_t value)
{
    assert(dev);
    uint8_t crc_buf[4] = { dev->addr << 1, LC709203F_REG_STATUS, (uint8_t) value, 0 };
    uint8_t send_buf[3] = { crc_buf[2], crc_buf[3], _get_crc(crc_buf, 4) };

    i2c_acquire(dev->bus);
    i2c_write_regs(dev->bus, dev->addr, crc_buf[1], send_buf, 3, 0);
    i2c_release(dev->bus);
}
