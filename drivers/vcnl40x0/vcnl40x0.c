/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @brief       Device driver for the Vishay VCNL40X0 proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <string.h>
#include <stdlib.h>

#include "vcnl40x0_regs.h"
#include "vcnl40x0.h"

#include "irq.h"
#include "log.h"
#include "xtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG

#define ASSERT_PARAM(cond) \
    if (!(cond)) { \
        DEBUG("[vcnl40x0] %s: %s\n", \
              __func__, "parameter condition (" # cond ") not fulfilled"); \
        assert(cond); \
    }

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[vcnl40x0] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, VCNL40X0_I2C_ADDRESS, ## __VA_ARGS__);

#else /* ENABLE_DEBUG */

#define ASSERT_PARAM(cond) assert(cond);
#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[vcnl40x0] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.dev, VCNL40X0_I2C_ADDRESS, ## __VA_ARGS__);

#define EXEC_RET(f) { \
    int _r; \
    if ((_r = f) != VCNL40X0_OK) { \
        DEBUG("[vcnl40x0] %s: error code %d\n", __func__, _r); \
        return _r; \
    } \
}

#define EXEC_RET_CODE(f, c) { \
    int _r; \
    if ((_r = f) != VCNL40X0_OK) { \
        DEBUG("[vcnl40x0] %s: error code %d\n", __func__, _r); \
        return c; \
    } \
}

#define EXEC(f) { \
    int _r; \
    if ((_r = f) != VCNL40X0_OK) { \
        DEBUG("[vcnl40x0] %s: error code %d\n", __func__, _r); \
        return; \
    } \
}

/** Forward declaration of functions for internal use */

static int _is_available(const vcnl40x0_t *dev);
static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit);
static int _reg_read(const vcnl40x0_t *dev, uint8_t reg, uint8_t *data, uint16_t len);
static int _reg_write(const vcnl40x0_t *dev, uint8_t reg, uint8_t *data, uint16_t len);
static int _update_reg(const vcnl40x0_t *dev, uint8_t reg, uint8_t mask, uint8_t val);

int vcnl40x0_init(vcnl40x0_t *dev, const vcnl40x0_params_t *params)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);
    DEBUG_DEV("params=%p", dev, params);

    /* init sensor data structure */
    dev->params = *params;

#if MODULE_VCNL40X0_FULL
    dev->gpio_init = false;
#endif

    /* check availability of the sensor */
    EXEC_RET(_is_available(dev));

    uint8_t cmd = 0;
    uint8_t als = 0;
    uint8_t prx = 0;
    uint8_t adj = 1; /* freq = 390.625 kHz, t_dead = 1, t_delay = 0 */

    /* disable all activities to change configuration */
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_COMMAND, &cmd, 1));

    /* disable all interrupts */
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_INT_CTRL, &cmd, 1));

    /* set default ALS measurement parameters */
    _set_reg_bit(&als, VCNL40X0_REG_AUTO_OFFSET, 1);
    _set_reg_bit(&als, VCNL40X0_REG_AVG_FUNC, dev->params.als_avg);
    _set_reg_bit(&als, VCNL40X0_REG_ALS_RATE, VCNL40X0_ALS_RATE_2);

    /* configure measurement modes */
    if (dev->params.prx_rate != VCNL40X0_PRX_RATE_NONE) {
        /* periodic selftimed measurement mode */
        _set_reg_bit(&cmd, VCNL40X0_REG_SELFTIMED_EN, 1);
        _set_reg_bit(&cmd, VCNL40X0_REG_PRX_EN, 1);
        _set_reg_bit(&prx, VCNL40X0_REG_PRX_RATE_M, dev->params.prx_rate);
    }
    if (dev->params.als_rate != VCNL40X0_ALS_RATE_NONE) {
        /* periodic selftimed measurement mode */
        _set_reg_bit(&cmd, VCNL40X0_REG_SELFTIMED_EN, 1);
        _set_reg_bit(&cmd, VCNL40X0_REG_ALS_EN, 1);
        _set_reg_bit(&als, VCNL40X0_REG_ALS_RATE, dev->params.als_rate);
    }
    else {
        /* on-demand measurement mode, enable continuous conversion */
        _set_reg_bit(&als, VCNL40X0_REG_CONT_CONV, 1);
    }

    /* write registers */
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_PRX_MOD_ADJ, &adj, 1));
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_PRX_RATE, &prx, 1));
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_ALS_PARAM, &als, 1));
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_COMMAND, &cmd, 1));

    /* set LED IR current */
    EXEC_RET(_update_reg(dev, VCNL40X0_REG_IR_LED_CURR,
                              VCNL40X0_REG_IR_LED_CURR_M,
                              dev->params.prx_ir_led / 10));

    return VCNL40X0_OK;
}

int vcnl40x0_als_data_ready (const vcnl40x0_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t reg;
    _reg_read(dev, VCNL40X0_REG_COMMAND, &reg, 1);

    return (reg & VCNL40X0_REG_ALS_DATA_RDY) ? VCNL40X0_OK
                                             : VCNL40X0_ERROR_NO_DATA;
}

int vcnl40x0_prx_data_ready (const vcnl40x0_t *dev)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t reg;
    _reg_read(dev, VCNL40X0_REG_COMMAND, &reg, 1);

    return (reg & VCNL40X0_REG_PRX_DATA_RDY) ? VCNL40X0_OK
                                              : VCNL40X0_ERROR_NO_DATA;
}

int vcnl40x0_read_als(const vcnl40x0_t *dev, uint16_t *als)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(als != NULL);
    DEBUG_DEV("als=%p", dev, als);

    uint8_t data[2];
    uint8_t reg;

    if (dev->params.als_rate == VCNL40X0_ALS_RATE_NONE) {
        /* check whether on-demand ALS measurement is possible */
        if (dev->params.prx_rate != VCNL40X0_PRX_RATE_NONE) {
            DEBUG_DEV("periodic selftimed proximity measurement activated, "
                      "on-demand measurement not possible", dev);
            return -VCNL40X0_ERROR_INV_MODE;
        }
        /* enable ALS on-demand measurement */
        reg = 0;
        _set_reg_bit(&reg, VCNL40X0_REG_ALS_OD, 1);
        _set_reg_bit(&reg, VCNL40X0_REG_ALS_EN, 1);
        EXEC_RET(_reg_write(dev, VCNL40X0_REG_COMMAND, &reg, 1));
        /* wait for data */
        while (vcnl40x0_als_data_ready(dev) == VCNL40X0_ERROR_NO_DATA) {
            xtimer_usleep(1000);
        }
        /* read the data sample */
        EXEC_RET_CODE(_reg_read(dev, VCNL40X0_REG_ALS_RES_H, data, 2),
                      -VCNL40X0_ERROR_RAW_DATA);
    }
    else {
        /*
         * in selftimed periodic measurement measurement mode, simply
         * the last available data sample is read
         */
        EXEC_RET_CODE(_reg_read(dev, VCNL40X0_REG_ALS_RES_H, data, 2),
                      -VCNL40X0_ERROR_RAW_DATA);
    }

    /* data MSB @ lower address */
    *als = (data[0] << 8) | data[1];

    return VCNL40X0_OK;
}

int vcnl40x0_read_prox(vcnl40x0_t *dev, uint16_t *prox)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(prox != NULL);
    DEBUG_DEV("prox=%p", dev, prox);

    uint8_t data[2];
    uint8_t reg;

    if (dev->params.prx_rate == VCNL40X0_PRX_RATE_NONE) {
        /* check whether on-demand ALS measurement is possible */
        if (dev->params.als_rate != VCNL40X0_ALS_RATE_NONE) {
            DEBUG_DEV("periodic selftimed proximity measurement activated, "
                      "on-demand measurement not possible", dev);
            return -VCNL40X0_ERROR_INV_MODE;
        }
        /* enable proximity on-demand measurement */
        reg = 0;
        _set_reg_bit(&reg, VCNL40X0_REG_PRX_OD, 1);
        _set_reg_bit(&reg, VCNL40X0_REG_PRX_EN, 1);
        EXEC_RET(_reg_write(dev, VCNL40X0_REG_COMMAND, &reg, 1));
        /* wait for data */
        while (vcnl40x0_prx_data_ready(dev) == VCNL40X0_ERROR_NO_DATA) {
            xtimer_usleep(1000);
        }
        /* read the data sample */
        EXEC_RET_CODE(_reg_read(dev, VCNL40X0_REG_PRX_RES_H, data, 2),
                      -VCNL40X0_ERROR_RAW_DATA);
    }
    else {
        /*
         * in selftimed periodic measurement mode, read and return
         * the last available data sample
         */
        EXEC_RET_CODE(_reg_read(dev, VCNL40X0_REG_PRX_RES_H, data, 2),
                      -VCNL40X0_ERROR_RAW_DATA);
    }

    /* data MSB @ lower address */
    *prox = (data[0] << 8) | data[1];


     return VCNL40X0_OK;
}

#if MODULE_VCNL40X0_FULL

void _vcnl40x0_isr(void *arg)
{
    unsigned state = irq_disable();

    vcnl40x0_t* dev =  (vcnl40x0_t*)arg;
    uint8_t reg;

    DEBUG_DEV("", dev);

    /* get interrupt status */
    _reg_read(dev, VCNL40X0_REG_INT_STATUS, &reg, 1);

    /* call registered interrupt service routines */
    if ((reg & VCNL40X0_REG_INT_ALS_DRDY) && dev->isr_als_drdy.isr) {
        dev->isr_als_drdy.isr(dev->isr_als_drdy.arg);
    }
    if ((reg & VCNL40X0_REG_INT_PRX_DRDY) && dev->isr_prx_drdy.isr) {
        dev->isr_prx_drdy.isr(dev->isr_prx_drdy.arg);
    }
    if ((reg & VCNL40X0_REG_INT_THS_LO) && dev->isr_ths_lo.isr) {
        dev->isr_ths_lo.isr(dev->isr_ths_lo.arg);
    }
    if ((reg & VCNL40X0_REG_INT_THS_HI) && dev->isr_ths_hi.isr) {
        dev->isr_ths_hi.isr(dev->isr_ths_hi.arg);
    }

    /* clear interrupt status with value of status register */
    _reg_write(dev, VCNL40X0_REG_INT_STATUS, &reg, 1);

    irq_restore (state);
}

int vcnl40x0_enable_int(vcnl40x0_t *dev, vcnl40x0_int_t intr,
                        vcnl40x0_isr_t isr, void *isr_arg)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(isr != NULL);
    ASSERT_PARAM(dev->params.int_pin != GPIO_UNDEF);
    DEBUG_DEV("", dev);

    if (!dev->gpio_init) {
        dev->gpio_init = true;
        gpio_init_int(dev->params.int_pin, GPIO_IN_PU, GPIO_FALLING,
                      _vcnl40x0_isr, dev);
    }

    switch (intr) {

        case VCNL40X0_INT_ALS_DRDY:
                 dev->isr_als_drdy.isr = isr;
                 dev->isr_als_drdy.arg = isr_arg;
                 EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                                           VCNL40X0_REG_INT_ALS_DRDY_E, 1));
                 break;

        case VCNL40X0_INT_PRX_DRDY:
                 dev->isr_prx_drdy.isr = isr;
                 dev->isr_prx_drdy.arg = isr_arg;
                 EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                                           VCNL40X0_REG_INT_PRX_DRDY_E, 1));
                 break;

        case VCNL40X0_INT_THS_LO:
                 dev->isr_ths_lo.isr = isr;
                 dev->isr_ths_lo.arg = isr_arg;
                 EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                                           VCNL40X0_REG_INT_THS_E, 1));
                 break;

        case VCNL40X0_INT_THS_HI:
                 dev->isr_ths_hi.isr = isr;
                 dev->isr_ths_hi.arg = isr_arg;
                 EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                                           VCNL40X0_REG_INT_THS_E, 1));
                 break;

        default: break;
    }

    return VCNL40X0_OK;
}

int vcnl40x0_disable_int(vcnl40x0_t *dev, vcnl40x0_int_t intr)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    switch (intr) {

        case VCNL40X0_INT_ALS_DRDY:
                 dev->isr_als_drdy.isr = NULL;
                 dev->isr_als_drdy.arg = NULL;
                 EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                                           VCNL40X0_REG_INT_ALS_DRDY_E, 0));
                 break;

        case VCNL40X0_INT_PRX_DRDY:
                 dev->isr_prx_drdy.isr = NULL;
                 dev->isr_prx_drdy.arg = NULL;
                 EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                                           VCNL40X0_REG_INT_PRX_DRDY_E, 0));
                 break;

        case VCNL40X0_INT_THS_LO:
                 dev->isr_ths_lo.isr = NULL;
                 dev->isr_ths_lo.arg = NULL;
                 if (dev->isr_ths_hi.isr) {
                     /* disable intr if dev->isr_ths_hi.isr is also NULL */
                     EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                                               VCNL40X0_REG_INT_THS_E, 0));
                 }
                 break;

        case VCNL40X0_INT_THS_HI:
                 dev->isr_ths_hi.isr = NULL;
                 dev->isr_ths_hi.arg = NULL;
                 if (dev->isr_ths_lo.isr) {
                     /* disable intr if dev->isr_ths_lo.isr is also NULL */
                     EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                                               VCNL40X0_REG_INT_THS_E, 0));
                 }
                 break;

        default: break;
    }

    return VCNL40X0_OK;
}

int vcnl40x0_set_int_thresh(vcnl40x0_t *dev, vcnl40x0_int_ths_t sel,
                            uint16_t low, uint16_t high,
                            vcnl40x0_int_ths_t count)
{
    ASSERT_PARAM(dev != NULL);
    DEBUG_DEV("", dev);

    uint8_t data[2];

    data[0] = low >> 8;
    data[1] = low & 0xff;
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_LOW_THRESH_H, data, 2));

    data[0] = high >> 8;
    data[1] = high & 0xff;
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_HIGH_THRESH_H, data, 2));

    EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                              VCNL40X0_REG_INT_THS_SEL, sel));
    EXEC_RET(_update_reg(dev, VCNL40X0_REG_INT_CTRL,
                              VCNL40X0_REG_INT_CNT_EXC, count));
    return VCNL40X0_OK;
}

int vcnl40x0_set_prx_mod (vcnl40x0_t *dev, vcnl40x0_prx_freq_t freq,
                          uint8_t t_delay, uint8_t t_dead)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(t_delay < 8);
    ASSERT_PARAM(t_dead < 8);

    uint8_t reg = 0;
    _set_reg_bit(&reg, VCNL40X0_REG_PRX_FREQ, freq);
    _set_reg_bit(&reg, VCNL40X0_REG_PRX_DELAY, t_delay);
    _set_reg_bit(&reg, VCNL40X0_REG_PRX_DEAD, t_dead);
    EXEC_RET(_reg_write(dev, VCNL40X0_REG_PRX_MOD_ADJ, &reg, 1));

    return VCNL40X0_OK;
}

#endif /* MODULE_VCNL40X0_FULL */

/** Functions for internal use only */

/**
 * @brief   Check the chip ID to test whether sensor is available
 */
static int _is_available(const vcnl40x0_t *dev)
{
    DEBUG_DEV("", dev);

    uint8_t reg;

    /* read the chip id from VCNL40X0_REG_ID_X */
    EXEC_RET(_reg_read(dev, VCNL40X0_REG_PROD_REV_ID, &reg,1));

    if (reg != VCNL40X0_ID) {
        DEBUG_DEV("sensor is not available, wrong product/revision id %02x, "
                  "should be %02x", dev, reg, VCNL40X0_ID);
        return -VCNL40X0_ERROR_WRONG_ID;
    }

    return VCNL40X0_OK;
}

static void _set_reg_bit(uint8_t *byte, uint8_t mask, uint8_t bit)
{
    ASSERT_PARAM(byte != NULL);

    uint8_t shift = 0;
    while (!((mask >> shift) & 0x01)) {
        shift++;
    }
    *byte = ((*byte & ~mask) | ((bit << shift) & mask));
}

static int _update_reg(const vcnl40x0_t *dev, uint8_t reg, uint8_t mask, uint8_t val)
{
    DEBUG_DEV("reg=%02x mask=%02x val=%02x", dev, reg, mask, val);

    uint8_t reg_val;
    uint8_t shift = 0;

    while (!((mask >> shift) & 0x01)) {
        shift++;
    }

    /* read current register value */
    EXEC_RET(_reg_read(dev, reg, &reg_val, 1));

    /* set masked bits to the given value  */
    reg_val = (reg_val & ~mask) | ((val << shift) & mask);

    /* write back new register value */
    EXEC_RET(_reg_write(dev, reg, &reg_val, 1));

    return VCNL40X0_OK;
}

static int _reg_read(const vcnl40x0_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);

    DEBUG_DEV("read %d byte from sensor registers starting at addr 0x%02x",
              dev, len, reg);

    if (i2c_acquire(dev->params.dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -VCNL40X0_ERROR_I2C;
    }

    int res = i2c_read_regs(dev->params.dev, VCNL40X0_I2C_ADDRESS, reg, data, len, 0);
    i2c_release(dev->params.dev);

    if (res == VCNL40X0_OK) {
        if (ENABLE_DEBUG) {
            printf("[vcnl40x0] %s i2c dev=%d addr=%02x: read following bytes: ",
                   __func__, dev->params.dev, VCNL40X0_I2C_ADDRESS);
            for (int i = 0; i < len; i++) {
                printf("%02x ", data[i]);
            }
            printf("\n");
        }
    }
    else {
        DEBUG_DEV("could not read %d bytes from sensor registers "
                  "starting at addr %02x, reason %d (%s)",
                  dev, len, reg, res, strerror(res * -1));
        return -VCNL40X0_ERROR_I2C;
    }

    return res;
}

static int _reg_write(const vcnl40x0_t *dev, uint8_t reg, uint8_t *data, uint16_t len)
{
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(data != NULL);
    ASSERT_PARAM(len != 0);

    DEBUG_DEV("write %d bytes to sensor registers starting at addr 0x%02x",
              dev, len, reg);

    if (ENABLE_DEBUG) {
        printf("[vcnl40x0] %s i2c dev=%d addr=%02x: write following bytes: ",
               __func__, dev->params.dev, VCNL40X0_I2C_ADDRESS);
        for (int i = 0; i < len; i++) {
            printf("%02x ", data[i]);
        }
        printf("\n");
    }

    if (i2c_acquire(dev->params.dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -VCNL40X0_ERROR_I2C;
    }

    int res;

    if (!data || !len) {
        res = i2c_write_byte(dev->params.dev, VCNL40X0_I2C_ADDRESS, reg, 0);
    }
    else {
        res = i2c_write_regs(dev->params.dev, VCNL40X0_I2C_ADDRESS, reg, data, len, 0);
    }
    i2c_release(dev->params.dev);

    if (res != VCNL40X0_OK) {
        DEBUG_DEV("could not write %d bytes to sensor registers "
                  "starting at addr 0x%02x, reason %d (%s)",
                  dev, len, reg, res, strerror(res * -1));
        return -VCNL40X0_ERROR_I2C;
    }

    return res;
}
