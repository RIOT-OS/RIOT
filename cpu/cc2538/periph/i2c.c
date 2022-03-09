/*
 * Copyright (C) 2015 Loci Controls Inc.
 *               2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#include "mutex.h"
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* short cuts macros */
#define SCL_PIN(x)  (i2c_config[x].scl_pin)
#define SDA_PIN(x)  (i2c_config[x].sda_pin)
#define SPEED(x)    (i2c_config[x].speed)

/* I2CM_CTRL Bits */
#define ACK         (0x08)
#define STOP        (0x04)
#define START       (0x02)
#define RUN         (0x01)

/* I2CM_STAT Bits */
#define BUSBSY      (0x40)
#define IDLE        (0x20)
#define ARBLST      (0x10)
#define DATACK      (0x08)
#define ADRACK      (0x04)
#define ERROR       (0x02)
#define BUSY        (0x01)

/* I2CM_CR Bits */
#define SFE         (0x20)  /**< I2C slave function enable */
#define MFE         (0x10)  /**< I2C master function enable */
#define LPBK        (0x01)  /**< I2C loopback */

#define SCL_LP      (6U)    /**< SCL Low Period (fixed at 6). */
#define SCL_HP      (4U)    /**< SCL High Period (fixed at 4). */

#define CMD_WAIT    (16U)   /**< small delay to wait for I2C command */
#define RST_WAIT    (32U)   /**< reset delay */

#define INVALID_SPEED_MASK  (0x0f)

static mutex_t lock = MUTEX_INIT;

void isr_i2c(void)
{
    /* Clear the interrupt flag */
    I2CM_ICR = 0x1;
    I2CM_MIS = 0x1;

    cortexm_isr_end();
}

static inline void _i2c_reset(void)
{
    DEBUG("%s\n", __FUNCTION__);
    /* Reset I2C peripheral */
    SYS_CTRL_SRI2C |= 1;
    /* wait shortly for reset */
    for (unsigned delay = 0; delay < RST_WAIT; ++delay) {}
    /* clear periph reset trigger */
    SYS_CTRL_SRI2C &= ~1;
}

static inline void _i2c_clock_enable(bool enable)
{
    DEBUG("%s\n", __FUNCTION__);
    if (enable) {
        SYS_CTRL_RCGCI2C |= 1;
        SYS_CTRL_SCGCI2C |= 1;
        SYS_CTRL_DCGCI2C |= 1;
    }
    else {
        SYS_CTRL_RCGCI2C &= ~1;
        SYS_CTRL_SCGCI2C &= ~1;
        SYS_CTRL_DCGCI2C &= ~1;
    }
}

static inline void _i2c_master_enable(bool enable)
{
    DEBUG("%s: %s\n", __FUNCTION__, (enable ? "yes" : "no"));
    if (enable) {
        /* enable I2C master function */
        I2CM_CR |= MFE;
        /* Enable I2C master interrupts */
        NVIC_SetPriority(I2C_IRQn, I2C_IRQ_PRIO);
        NVIC_EnableIRQ(I2C_IRQn);
        /* Enable I2C master interrupts */
        I2CM_IMR = 1;
    }
    else {
        /* Disable I2C master interrupts */
        I2CM_IMR = 0;
        NVIC_DisableIRQ(I2C_IRQn);
        /* disable master function */
        I2CM_CR &= ~(MFE);
    }
}

static inline void _i2c_master_frequency(i2c_speed_t speed)
{
    assert ((speed == I2C_SPEED_NORMAL) || (speed == I2C_SPEED_FAST));
    DEBUG("%s (%" PRIu32 ")\n", __FUNCTION__, (uint32_t)speed);
    /* if selected speed is not applicable fall back to normal */
    if (speed & INVALID_SPEED_MASK) {
        DEBUG("! invalid speed setting, fall back to normal !\n");
        speed = I2C_SPEED_NORMAL;
    }
    /* Set the SCL clock speed */
    uint32_t denom = 2 * (SCL_LP + SCL_HP) * (uint32_t)speed;
    uint32_t ps = (sys_clock_freq() + denom - 1) / denom;
    I2CM_TPR = (ps - 1);
}

static uint_fast8_t _i2c_master_stat_get(void)
{
    return I2CM_STAT;
}

static bool _i2c_master_busy(void)
{
    return ((I2CM_STAT & BUSY) ? true : false);
}

static bool _i2c_master_busbusy(void)
{
    return ((I2CM_STAT & BUSBSY) ? true : false);
}

static void _i2c_master_slave_addr(uint16_t addr, bool receive)
{
    DEBUG("%s (%" PRIx16 ", %d)\n", __FUNCTION__, addr, (int)receive);
    assert(!(addr & 0x80));
    I2CM_SA = (addr << 1) | (receive ? 0x1 : 0x0);
}

static void _i2c_master_data_put(uint8_t data)
{
    DEBUG("%s (0x%x)\n", __FUNCTION__, data);
    I2CM_DR = data;
}

static uint_fast8_t _i2c_master_data_get(void)
{
    uint_fast8_t data = I2CM_DR;
    DEBUG("%s (0x%x)\n", __FUNCTION__, data);
    return data;
}

static inline void _i2c_master_ctrl(uint_fast8_t cmd)
{
    DEBUG("%s (%" PRIx32 ")\n", __FUNCTION__, (uint32_t)cmd);
    I2CM_CTRL = cmd;
}

static inline int _i2c_master_status(void)
{
    DEBUG("%s\n", __FUNCTION__);
    uint_fast8_t stat = _i2c_master_stat_get();
    DEBUG(" - I2C master status (0x%x): ", stat);
    if (stat & BUSY) {
        DEBUG("busy!\n");
        return 0;
    }
    else if (stat & (ERROR | ARBLST)) {
        _i2c_master_ctrl(STOP | RUN);
        unsigned cw = CMD_WAIT;
        while (_i2c_master_busy() || (cw--)) {}
        if (stat & ADRACK) {
            DEBUG("addr ack lost!\n");
            return -ENXIO;
        }
        if (stat & DATACK) {
            DEBUG("data ack lost!\n");
            return -EIO;
        }
        if (stat & ARBLST) {
            DEBUG("lost bus arbitration!\n");
            return -EAGAIN;
        }
        DEBUG("unknown!\n");
        return -EAGAIN;
    }
    DEBUG("okay.\n");
    return 0;
}

void i2c_init(i2c_t dev)
{
    DEBUG("%s (%i)\n", __FUNCTION__, (int)dev);
    assert(dev < I2C_NUMOF);
    /* enable i2c clock */
    _i2c_clock_enable(true);
    /* reset i2c periph */
    _i2c_reset();
    /* init pins */
    gpio_init_mux(SCL_PIN(dev), OVERRIDE_PULLUP, I2C_SCL_OUT, I2C_SCL_IN);
    gpio_init_mux(SDA_PIN(dev), OVERRIDE_PULLUP, I2C_SDA_OUT, I2C_SDA_IN);
    /* enable master mode */
    _i2c_master_enable(true);
    /* set bus frequency */
    _i2c_master_frequency(SPEED(dev));
    DEBUG(" - I2C master status (0x%x).\n", _i2c_master_stat_get());
}

void i2c_acquire(i2c_t dev)
{
    (void)dev;
    assert(dev < I2C_NUMOF);
    DEBUG("%s\n", __FUNCTION__);
    mutex_lock(&lock);
}

void i2c_release(i2c_t dev)
{
    (void)dev;
    assert(dev < I2C_NUMOF);
    DEBUG("%s\n", __FUNCTION__);
    mutex_unlock(&lock);
}

int i2c_read_bytes(i2c_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags)
{
    DEBUG("%s\n", __FUNCTION__);
    DEBUG(" - I2C master status (0x%x).\n", _i2c_master_stat_get());

    if ((dev >= I2C_NUMOF) || (data == NULL) || (len == 0)) {
        return -EINVAL;
    }
    if (flags & (I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }
    if (_i2c_master_busy()) {
        DEBUG("i2c_read_bytes: device busy!\n");
        return -EAGAIN;
    }
    if (!(_i2c_master_busbusy()) && (flags & I2C_NOSTART)) {
        DEBUG("i2c_read_bytes: bus not busy!\n");
        return -EAGAIN;
    }

    /* set slave address for receive */
    _i2c_master_slave_addr(addr, true);
    int rc = 0;
    uint8_t *buf = data;

    for (size_t n = 0; n < len; n++) {
        uint_fast8_t cmd = RUN;
        if ((n == 0) && !(flags & I2C_NOSTART)) {
            cmd |= START;
        }
        if (((len - n) == 1) && !(flags & I2C_NOSTOP)) {
            cmd |= STOP;
        }
        else {
            cmd |= ACK;
        }
        /* run command */
        _i2c_master_ctrl(cmd);
        /* wait until master is done transferring */
        DEBUG ("%s: wait for master...\n", __FUNCTION__);
        unsigned cw = CMD_WAIT;
        while (_i2c_master_busy() || (cw--)) {}

        /* check master status */
        if ((rc = _i2c_master_status()) != 0) {
            break;
        }
        /* read data into buffer */
        buf[n] = _i2c_master_data_get();
    }

    return rc;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags)
{
    DEBUG("%s\n", __FUNCTION__);
    DEBUG(" - I2C master status (0x%x).\n", _i2c_master_stat_get());

    if ((dev >= I2C_NUMOF) || (data == NULL) || (len == 0)) {
        return -EINVAL;
    }
    if (flags & (I2C_REG16 | I2C_ADDR10)) {
        return -EOPNOTSUPP;
    }
    if (_i2c_master_busy()) {
        DEBUG("i2c_write_bytes: device busy!\n");
        return -EAGAIN;
    }
    if (!(_i2c_master_busbusy()) && (flags & I2C_NOSTART)) {
        DEBUG("i2c_read_bytes: bus not busy!\n");
        return -EAGAIN;
    }

    /* set slave address for write */
    _i2c_master_slave_addr(addr, false);

    int rc = 0;
    const uint8_t *buf = data;

    for (size_t n = 0; n < len; n++) {
        uint_fast8_t cmd = RUN;
        if ((n == 0) && !(flags & I2C_NOSTART)) {
            cmd |= START;
        }
        if (((len - n) == 1) && !(flags & I2C_NOSTOP)) {
            cmd |= STOP;
        }
        /* write byte to mem */
        _i2c_master_data_put(buf[n]);
        /* run command */
        _i2c_master_ctrl(cmd);
        /* wait until master is done transferring */
        DEBUG ("%s: wait for master...\n", __FUNCTION__);
        unsigned cw = CMD_WAIT;
        while (_i2c_master_busy() || (cw--)) {}

        /* check master status */
        if ((rc = _i2c_master_status()) != 0) {
            break;
        }
    }

    return rc;
}
