/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_tsl4531x
 * @{
 *
 * @file
 * @brief       Device driver for the TSL4531 Luminosity sensor.
 *
 * @author      Juan I Carrano <j.carrano@fu-berlin.de>
 *
 * This driver was derived from the one for TSL2561.
 *
 * @todo    The i2c address is hardcoded
 * @todo    The only operating mode is "Normal Operation"
 * @todo    PowerSave is not supported
 *
 * @}
 */

#include <errno.h>

#include "log.h"
#include "tsl4531x.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

static const uint16_t TSL4531_Addr = 0x29;
static const uint16_t TSL4531_PARTNO = 0xA; /* 0b1010 */

static const uint16_t Control_Reg = 0x0;
static const uint16_t Configuration_Reg = 0x01;
static const uint16_t ALSData1_Reg = 0x04;  /* contains DATALOW */
static const uint16_t ALSData2_Reg = 0x05;  /* contains DATAHIGH */
static const uint16_t ID_Reg = 0x0A;

static const uint16_t TSL4531x_MODE_Normal = 0x03; /* 0b11 */

/* Assemble the Command register */
#define TSL4531_COMMAND(addr)   ((1<<7)|(addr))

/* Assemble the Configuration register */
#define TSL4531_CONFIG(psaveskip, tcntrl)  (((!!(psaveskip)) << 3) | (tcntrl))

/* Assemble the Control register */
#define TSL4531_CONTROL(mode)  (mode)

#define TSL4531_GET_PARTNO(id_reg)  ((id_reg) >> 4)

/* Apply the multiplier corresponding to the active integration time */
/* From the manual:
 *      MULTIPLIER = 1 for TCNTRL = 00 (Tint = 400 ms),
 *      MULTIPLIER = 2 for TCNTRL = 01 (Tint = 200 ms), and
 *      MULTIPLIER = 4 for TCNTRL = 10 (Tint = 100 ms), and
 */
#define MULTIPLY_DATA(data_raw, integration_time) ((data_raw) << (integration_time))

int tsl4531x_init(tsl4531x_t *dev, i2c_t i2c_dev, uint8_t integration_time)
{
    int r;
    uint8_t id;

    dev->i2c_dev = i2c_dev;
    dev->integration_time = integration_time;

    if ((r = i2c_acquire(dev->i2c_dev)) < 0) {
        DEBUG("[Error] Cannot acquire device: %d\n", r);
        goto acquire_error;
    }

    /* Verify sensor ID */
    if ((r = i2c_read_reg(dev->i2c_dev, TSL4531_Addr, TSL4531_COMMAND(ID_Reg),
                          &id, 0)) < 0) {
        DEBUG("[Error] Cannot read ID register: %d\n", r);
        goto init_error;
    }
    DEBUG("[Info] ID ? %d\n", id);
    if (TSL4531_GET_PARTNO(id) != TSL4531_PARTNO) {
        DEBUG("[Error] not a TSL4531 sensor\n");
        r = -ENOTSUP;
        goto init_error;
    }

    /* configure device */
    if (((r = i2c_write_reg(dev->i2c_dev , TSL4531_Addr, TSL4531_COMMAND(Control_Reg),
                  TSL4531_CONTROL(TSL4531x_MODE_Normal), 0)) < 0)
         ||
         ((r = i2c_write_reg(dev->i2c_dev , TSL4531_Addr, TSL4531_COMMAND(Configuration_Reg),
                  TSL4531_CONFIG(1, integration_time), 0)) < 0)) {
        DEBUG("[Error] Cannot configure device %d\n", r);
        goto init_error;
    }

init_error:
    /* ignore errors on device release */
    i2c_release(dev->i2c_dev);

acquire_error:

    return r;
}

#define _DATALOW 0
#define _DATAHIGH 1

int tsl4531x_read(const tsl4531x_t *dev, uint16_t *result_lux)
{
    int r;
    uint8_t als_data[2]; /* = {[DATALOW], [DATAHIGH]} */

    if ((r = i2c_acquire(dev->i2c_dev)) < 0) {
        DEBUG("[Error] Cannot acquire device: %d\n", r);
        goto read_acquire_error;
    }

    if ((r = i2c_read_regs(dev->i2c_dev, TSL4531_Addr, TSL4531_COMMAND(ALSData1_Reg),
                  als_data, 2, 0)) < 0) {
        (void)ALSData2_Reg; /* suppress warning */
        DEBUG("[Error] Cannot read device: %d\n", r);
        goto read_error;
    }

    *result_lux = MULTIPLY_DATA((((uint16_t)als_data[_DATAHIGH]) << 8)
                            + als_data[_DATALOW], dev->integration_time);

read_error:
    /* ignore errors on device release */
    i2c_release(dev->i2c_dev);

read_acquire_error:
    return r;
}

