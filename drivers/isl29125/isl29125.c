/*
 * Copyright 2015 Ludwig Knüpfer
 * Copyright 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_isl29125
 * @{
 *
 * @file
 * @brief       Device driver implementation for the ISL29125 RGB light sensor
 *
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @author      Martin Heusmann <martin.heusmann@haw-hamburg.de>
 * @author      Cenk Gündoğan <mail-github@cgundogan.de>
 *
 * @}
 */

#include <stdint.h>

#include "isl29125.h"
#include "isl29125-internal.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "color.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define DEV_I2C      (dev->params.i2c)
#define DEV_GPIO     (dev->params.gpio)
#define DEV_RANGE    (dev->params.range)
#define DEV_MODE     (dev->params.mode)
#define DEV_RES      (dev->params.res)

/***********************************************************************
 * public API implementation
 **********************************************************************/

int isl29125_init(isl29125_t *dev, const isl29125_params_t *params)
{
    DEBUG("isl29125_init\n");

    /* initialize device descriptor */
    dev->params = *params;

    /* configuration 1: operation mode, range, resolution */
    uint8_t conf1 = 0x00;
    conf1 |= DEV_MODE;
    conf1 |= DEV_RANGE;
    conf1 |= DEV_RES;
    conf1 |= ISL29125_CON1_SYNCOFF; /* TODO: implement SYNC mode configuration */

    /* TODO: implement configuration 2: infrared compensation configuration */

    /* acquire exclusive access to the bus */
    DEBUG("isl29125_init: i2c_acquire\n");
    (void) i2c_acquire(DEV_I2C);

    /* verify the device ID */
    DEBUG("isl29125_init: i2c_read_reg\n");
    uint8_t reg_id;
    int ret = i2c_read_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_ID, &reg_id, 0);
    if ((reg_id == ISL29125_ID) && (ret == 0)) {
        DEBUG("isl29125_init: ID successfully verified\n");
    }
    else {
        DEBUG("isl29125_init: ID could not be verified, ret: %i\n", ret);
        (void) i2c_release(DEV_I2C);
        return -1;
    }

    /* configure and enable the sensor */
    DEBUG("isl29125_init: i2c_write_reg(ISL29125_REG_RESET)\n");
    (void) i2c_write_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_RESET, ISL29125_CMD_RESET, 0);

    DEBUG("isl29125_init: i2c_write_reg(ISL29125_REG_CONF1)\n");
    (void) i2c_write_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_CONF1, conf1, 0);

    /* release the I2C bus */
    DEBUG("isl29125_init: i2c_release\n");
    (void) i2c_release(DEV_I2C);

    DEBUG("isl29125_init: success\n");
    return 0;
}

int isl29125_init_int(isl29125_t *dev, isl29125_interrupt_status_t interrupt_status,
                      isl29125_interrupt_persist_t interrupt_persist,
                      isl29125_interrupt_conven_t interrupt_conven,
                      uint16_t lower_threshold, uint16_t higher_threshold,
                      gpio_cb_t cb, void *arg)
{
    /* configuration 3: interrupt mode configuration */
    uint8_t conf3 = 0x00;
    conf3 |= interrupt_status;
    conf3 |= interrupt_persist;
    conf3 |= interrupt_conven;

    /* Lower and higher interrupt threshold registers. */
    uint8_t lthlb = 0x00;
    uint8_t lthhb = 0x00;
    uint8_t hthlb = 0x00;
    uint8_t hthhb = 0x00;
    uint16_t max_range = 10000;

    if (DEV_RANGE == 0x00) {
        max_range = 375;
    }

    if ((higher_threshold <= max_range) && (lower_threshold < higher_threshold)) {
        lower_threshold *= (uint16_t) (65535 / max_range);
        lthlb = (uint8_t)(lower_threshold & 0xff);
        lthhb = (uint8_t)(lower_threshold >> 8);
        higher_threshold *= (uint16_t) (65535 / max_range);
        hthlb = (uint8_t)(higher_threshold & 0xff);
        hthhb = (uint8_t)(higher_threshold >> 8);
    }

    DEBUG("isl29125_init: i2c_write_reg(ISL29125_REG_CONF3)\n");
    (void) i2c_write_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_CONF3, conf3, 0);

    DEBUG("isl29125_init: i2c_write_reg(ISL29125_REG_LTHLB)\n");
    (void) i2c_write_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_LTHLB, lthlb, 0);

    DEBUG("isl29125_init: i2c_write_reg(ISL29125_REG_LTHHB)\n");
    (void) i2c_write_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_LTHHB, lthhb, 0);

    DEBUG("isl29125_init: i2c_write_reg(ISL29125_REG_HTHLB)\n");
    (void) i2c_write_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_HTHLB, hthlb, 0);

    DEBUG("isl29125_init: i2c_write_reg(ISL29125_REG_HTHHB)\n");
    (void) i2c_write_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_HTHHB, hthhb, 0);

    if (gpio_init_int(DEV_GPIO, GPIO_IN, GPIO_FALLING, cb, arg) < 0) {
        DEBUG("error: gpio_init_int failed\n");
        return -1;
    }

    return 0;
}

void isl29125_read_rgb_lux(const isl29125_t *dev, isl29125_rgb_t *dest)
{
    /* acquire exclusive access to the bus */
    (void) i2c_acquire(DEV_I2C);

    /* read values */
    uint8_t bytes[6];
    (void) i2c_read_regs(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_GDLB, bytes, 6, 0);

    /* release the I2C bus */
    (void) i2c_release(DEV_I2C);

    /* possibly shift by 4 to normalize 12 to 16 bit */
    int resfactor = (DEV_RES == ISL29125_RESOLUTION_12) ? 4 : 0;
    /* parse and normalize readings */
    uint16_t green = (bytes[0] | (bytes[1] << 8)) << resfactor;
    uint16_t red   = (bytes[2] | (bytes[3] << 8)) << resfactor;
    uint16_t blue  = (bytes[4] | (bytes[5] << 8)) << resfactor;

    DEBUG("isl29125_read_rgb: adjusted, unconverted readings: (%5i / %5i / %5i) \n", red, green, blue);

    /* convert readings to lux */
    float luxfactor = (DEV_RANGE == ISL29125_RANGE_10K) ? 10000.0 / 65535.0 : 375.0 / 65535.0;
    dest->red = red * luxfactor;
    dest->green = green * luxfactor;
    dest->blue = blue * luxfactor;
}

void isl29125_read_rgb_color(const isl29125_t *dev, color_rgb_t *dest)
{
    /* acquire exclusive access to the bus */
    (void) i2c_acquire(DEV_I2C);

    /* read values */
    uint8_t bytes[6];
    (void) i2c_read_regs(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_GDLB, bytes, 6, 0);

    /* release the I2C bus */
    (void) i2c_release(DEV_I2C);

    /* factor normalize 12 or 16 bit to 8 bit */
    int normfactor = (DEV_RES == ISL29125_RESOLUTION_12) ? 4 : 8;
    /* parse and normalize readings */
    dest->g = (bytes[0] | (bytes[1] << 8)) >> normfactor;
    dest->r = (bytes[2] | (bytes[3] << 8)) >> normfactor;
    dest->b = (bytes[4] | (bytes[5] << 8)) >> normfactor;
}

void isl29125_set_mode(const isl29125_t *dev, isl29125_mode_t mode)
{
    uint8_t conf1;

    (void) i2c_acquire(DEV_I2C);

    (void) i2c_read_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_CONF1, &conf1, 0);
    conf1 &= ~ISL29125_CON1_MASK_MODE;
    conf1 |= mode;
    (void) i2c_write_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_CONF1, conf1, 0);

    (void) i2c_release(DEV_I2C);
}

int isl29125_read_irq_status(const isl29125_t *dev)
{
    /* acquire exclusive access to the bus */
    (void) i2c_acquire(DEV_I2C);

    /* read status register */
    uint8_t irq_status;
    (void) i2c_read_reg(DEV_I2C, ISL29125_I2C_ADDRESS, ISL29125_REG_STATUS, &irq_status, 0);

    /* release the I2C bus */
    (void) i2c_release(DEV_I2C);

    /* return bit 0 (RGBTHF)*/
    return (irq_status & 0x01);
}
