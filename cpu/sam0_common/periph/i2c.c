/*
 * Copyright (C) 2014 CLENET Baptiste
 * Copyright (C) 2018 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include "cpu.h"
#include "board.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define SAMD21_I2C_TIMEOUT  (65535)

#define BUSSTATE_UNKNOWN SERCOM_I2CM_STATUS_BUSSTATE(0)
#define BUSSTATE_IDLE SERCOM_I2CM_STATUS_BUSSTATE(1)
#define BUSSTATE_OWNER SERCOM_I2CM_STATUS_BUSSTATE(2)
#define BUSSTATE_BUSY SERCOM_I2CM_STATUS_BUSSTATE(3)

#if defined(CPU_COMMON_SAML21) || defined(CPU_COMMON_SAML1X) || defined(CPU_COMMON_SAMD5X)
#define SERCOM_I2CM_CTRLA_MODE_I2C_MASTER SERCOM_I2CM_CTRLA_MODE(5)
#endif

static int _i2c_start(SercomI2cm *dev, uint16_t addr);
static inline int _write(SercomI2cm *dev, const uint8_t *data, size_t length,
                         uint8_t stop);
static inline int _read(SercomI2cm *dev, uint8_t *data, size_t length,
                        uint8_t stop);
static inline void _stop(SercomI2cm *dev);
static inline int _wait_for_response(SercomI2cm *dev,
                                     uint32_t max_timeout_counter);
static void _i2c_poweron(i2c_t dev);
static void _i2c_poweroff(i2c_t dev);

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[I2C_NUMOF];

/**
 * @brief   Shortcut for accessing the used I2C SERCOM device
 */
static inline SercomI2cm *bus(i2c_t dev)
{
    return i2c_config[dev].dev;
}

static void _syncbusy(SercomI2cm *dev)
{
#ifdef SERCOM_I2CM_STATUS_SYNCBUSY
    while (dev->STATUS.bit.SYNCBUSY) {}
#else
    while (dev->SYNCBUSY.reg) {}
#endif
}

static void _reset(SercomI2cm *dev)
{
    dev->CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
    while (dev->CTRLA.reg & SERCOM_SPI_CTRLA_SWRST) {}

#ifdef SERCOM_I2CM_STATUS_SYNCBUSY
    while (dev->STATUS.bit.SYNCBUSY) {}
#else
    while (dev->SYNCBUSY.bit.SWRST) {}
#endif
}

void i2c_init(i2c_t dev)
{
    uint32_t timeout_counter = 0;
    int32_t tmp_baud;

    assert(dev < I2C_NUMOF);

    const uint32_t fSCL = i2c_config[dev].speed;
    const uint32_t fGCLK = sam0_gclk_freq(i2c_config[dev].gclk_src);

    /* Initialize mutex */
    mutex_init(&locks[dev]);

    /* DISABLE I2C MASTER */
    _i2c_poweroff(dev);

    /* Reset I2C */
    _reset(bus(dev));

    /* Turn on power manager for sercom */
    sercom_clk_en(bus(dev));

    /* I2C using CLK GEN 0 */
    sercom_set_gen(bus(dev), i2c_config[dev].gclk_src);

    /* Check if module is enabled. */
    if (bus(dev)->CTRLA.reg & SERCOM_I2CM_CTRLA_ENABLE) {
        DEBUG("STATUS_ERR_DENIED\n");
        return;
    }
    /* Check if reset is in progress. */
    if (bus(dev)->CTRLA.reg & SERCOM_I2CM_CTRLA_SWRST) {
        DEBUG("STATUS_BUSY\n");
        return;
    }

    /************ SERCOM PAD0 - SDA and SERCOM PAD1 - SCL *************/
    gpio_init_mux(i2c_config[dev].sda_pin, i2c_config[dev].mux);
    gpio_init_mux(i2c_config[dev].scl_pin, i2c_config[dev].mux);

    /* I2C CONFIGURATION */
    _syncbusy(bus(dev));

    /* Set sercom module to operate in I2C master mode and run in Standby
       if user requests it */
    bus(dev)->CTRLA.reg = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER
                        | ((i2c_config[dev].flags & I2C_FLAG_RUN_STANDBY) ?
                           SERCOM_I2CM_CTRLA_RUNSTDBY : 0);

    /* Enable Smart Mode (ACK is sent when DATA.DATA is read) */
    bus(dev)->CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;

    /* Set SPEED */
#ifdef SERCOM_I2CM_CTRLA_SPEED
    /* > 1000 kHz */
    if (fSCL > I2C_SPEED_FAST_PLUS) {
        bus(dev)->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(2);
    /* > 400 kHz */
    } else if (fSCL > I2C_SPEED_FAST) {
        bus(dev)->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(1);
    /* ≤ 400 kHz */
    } else {
        bus(dev)->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(0);
    }
#else
    assert(fSCL < I2C_SPEED_FAST_PLUS);
#endif

    /* Get the baudrate */
    /* fSCL = fGCLK / (10 + 2 * BAUD)  -> BAUD   = fGCLK / (2 * fSCL) - 5 */
    /* fSCL = fGCLK / (2 + 2 * HSBAUD) -> HSBAUD = fGCLK / (2 * fSCL) - 1 */
    tmp_baud = (fGCLK + (2 * fSCL) - 1) /* round up */
             / (2 * fSCL)
             - (fSCL > I2C_SPEED_FAST_PLUS ? 1 : 5);

    /* Ensure baudrate is within limits */
    assert(tmp_baud < 255 && tmp_baud > 0);

#ifdef SERCOM_I2CM_BAUD_HSBAUD
    if (fSCL > I2C_SPEED_FAST_PLUS) {
        bus(dev)->BAUD.reg = SERCOM_I2CM_BAUD_HSBAUD(tmp_baud);
    } else
#endif
    {
        bus(dev)->BAUD.reg = SERCOM_I2CM_BAUD_BAUD(tmp_baud);
    }

    /* ENABLE I2C MASTER */
    _i2c_poweron(dev);

    /* Start timeout if bus state is unknown. */
    while ((bus(dev)->STATUS.reg &
          SERCOM_I2CM_STATUS_BUSSTATE_Msk) == BUSSTATE_UNKNOWN) {
        if (timeout_counter++ >= SAMD21_I2C_TIMEOUT) {
            /* Timeout, force bus state to idle. */
            bus(dev)->STATUS.reg = BUSSTATE_IDLE;
        }
    }
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_lock(&locks[dev]);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_unlock(&locks[dev]);
}

#ifdef MODULE_PERIPH_I2C_RECONFIGURE
void i2c_init_pins(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    _i2c_poweron(dev);

    gpio_init_mux(i2c_config[dev].scl_pin, i2c_config[dev].mux);
    gpio_init_mux(i2c_config[dev].sda_pin, i2c_config[dev].mux);

    mutex_unlock(&locks[dev]);
}

void i2c_deinit_pins(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&locks[dev]);
    _i2c_poweroff(dev);

    gpio_disable_mux(i2c_config[dev].sda_pin);
    gpio_disable_mux(i2c_config[dev].scl_pin);
}
#endif

int i2c_read_bytes(i2c_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags)
{
    int ret;
    assert(dev < I2C_NUMOF);

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }
    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    if (!(flags & I2C_NOSTART)) {
        /* start transmission and send slave address */
        ret = _i2c_start(bus(dev), (addr << 1) | I2C_READ);
        if (ret < 0) {
            DEBUG("Start command failed\n");
            return ret;
        }
    }
    /* read data to register and issue stop if needed */
    ret = _read(bus(dev), data, len, (flags & I2C_NOSTOP) ? 0 : 1);
    if (ret < 0) {
        DEBUG("Read command failed\n");
        return ret;
    }
    /* Ensure all bytes has been read */
    if (flags & I2C_NOSTOP) {
        while ((bus(dev)->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE_Msk)
                != BUSSTATE_OWNER) {}
    }
    else {
        while ((bus(dev)->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE_Msk)
                != BUSSTATE_IDLE) {}
    }
    /* return number of bytes sent */
    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    int ret;
    assert(dev < I2C_NUMOF);

    /* Check for unsupported operations */
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }
    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    if (!(flags & I2C_NOSTART)) {
        ret = _i2c_start(bus(dev), (addr<<1));
        if (ret < 0) {
            DEBUG("Start command failed\n");
            return ret;
        }
    }
    ret = _write(bus(dev), data, len, (flags & I2C_NOSTOP) ? 0 : 1);
    if (ret < 0) {
        DEBUG("Write command failed\n");
        return ret;
    }

    return 0;
}

void _i2c_poweron(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    if (bus(dev) == NULL) {
        return;
    }
    bus(dev)->CTRLA.bit.ENABLE = 1;
    _syncbusy(bus(dev));
}

void _i2c_poweroff(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    if (bus(dev) == NULL) {
        return;
    }
    bus(dev)->CTRLA.bit.ENABLE = 0;
    _syncbusy(bus(dev));
}

static int _i2c_start(SercomI2cm *dev, uint16_t addr)
{
    /* Wait for hardware module to sync */
    DEBUG("Wait for device to be ready\n");
    _syncbusy(dev);

    /* Set action to ACK. */
    dev->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

    /* Send Start | Address | Write/Read */
    DEBUG("Generate start condition by sending address\n");
    dev->ADDR.reg = addr;

    /* Wait for response on bus. */
    if (addr & I2C_READ) {
        /* Some devices (e.g. SHT2x) can hold the bus while
        preparing the reply */
        if (_wait_for_response(dev, 100 * SAMD21_I2C_TIMEOUT) < 0)
            return -ETIMEDOUT;
    }
    else {
        if (_wait_for_response(dev, SAMD21_I2C_TIMEOUT) < 0)
            return -ETIMEDOUT;
    }

    /* Check for address response error unless previous error is detected. */
    /* Check for error and ignore bus-error; workaround for BUSSTATE
     * stuck in BUSY */
    if (dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB) {
        /* Clear write interrupt flag */
        dev->INTFLAG.reg = SERCOM_I2CM_INTFLAG_SB;
        /* Check arbitration. */
        if (dev->STATUS.reg & SERCOM_I2CM_STATUS_ARBLOST) {
            DEBUG("STATUS_ERR_PACKET_COLLISION\n");
            return -EAGAIN;
        }
    }
    /* Check that slave responded with ack. */
    else if (dev->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
        /* Slave busy. Issue ack and stop command. */
        dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
        DEBUG("STATUS_ERR_BAD_ADDRESS\n");
        return -ENXIO;
    }
    return 0;
}

static inline int _write(SercomI2cm *dev, const uint8_t *data, size_t length,
                         uint8_t stop)
{
    size_t count = 0;

    /* Write data buffer until the end. */
    DEBUG("Looping through bytes\n");
    while (length--) {
        /* Check that bus ownership is not lost. */
        if ((dev->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE_Msk)
            != BUSSTATE_OWNER) {
            DEBUG("STATUS_ERR_PACKET_COLLISION\n");
            return -EAGAIN;
        }

        /* Wait for hardware module to sync */
        _syncbusy(dev);

        DEBUG("Written byte #%i to data reg, now waiting for DR"
              " to be empty again\n", count);
        dev->DATA.reg = data[count++];

        /* Wait for response on bus. */
        if (_wait_for_response(dev, SAMD21_I2C_TIMEOUT) < 0) {
            return -ETIMEDOUT;
        }

        /* Check for NACK from slave. */
        if (dev->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
            DEBUG("STATUS_ERR_OVERFLOW\n");
            return -EIO;
        }
    }
    if (stop) {
        /* Issue stop command */
        _stop(dev);
    }
    return 0;
}

static inline int _read(SercomI2cm *dev, uint8_t *data, size_t length,
                        uint8_t stop)
{
    size_t count = 0;

    /* Set action to ack. */
    dev->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

    /* Read data buffer. */
    while (length--) {
        /* Check that bus ownership is not lost. */
        if ((dev->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE_Msk)
            != BUSSTATE_OWNER) {
            DEBUG("STATUS_ERR_PACKET_COLLISION\n");
            return -EAGAIN;
        }

        /* Wait for hardware module to sync */
        _syncbusy(dev);

        /* Check if this is the last byte to read */
        if (length == 0 && stop) {
            /* Send NACK before STOP */
            dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
            /* Prepare stop command before read last byte otherwise
               hardware will request an extra byte to read */
            _stop(dev);
        }
        /* Save data to buffer. */
        data[count] = dev->DATA.reg;

        /* Wait for response on bus. */
        if (length > 0) {
            if (_wait_for_response(dev, SAMD21_I2C_TIMEOUT) < 0)
                return -ETIMEDOUT;
        }
        count++;
    }
    return 0;
}

static inline void _stop(SercomI2cm *dev)
{
    /* Wait for hardware module to sync */
    _syncbusy(dev);
    /* Stop command */
    dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
    DEBUG("Stop sent\n");
}

static inline int _wait_for_response(SercomI2cm *dev,
                                     uint32_t max_timeout_counter)
{
    uint32_t timeout_counter = 0;
    DEBUG("Wait for response.\n");
    while (!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)
           && !(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) {
        if (++timeout_counter >= max_timeout_counter) {
            DEBUG("STATUS_ERR_TIMEOUT\n");
            return -ETIMEDOUT;
        }
    }
    return 0;
}
