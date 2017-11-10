/*
 * Copyright (C) 2014 CLENET Baptiste
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
 * @author      Dylan Laduranty  <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "board.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"
#include "periph_cpu_common.h"

#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define SAMD21_I2C_TIMEOUT  (65535)

#define BUSSTATE_UNKNOWN SERCOM_I2CM_STATUS_BUSSTATE(0)
#define BUSSTATE_IDLE SERCOM_I2CM_STATUS_BUSSTATE(1)
#define BUSSTATE_OWNER SERCOM_I2CM_STATUS_BUSSTATE(2)
#define BUSSTATE_BUSY SERCOM_I2CM_STATUS_BUSSTATE(3)

#if CPU_FAM_SAML21
#define SERCOM_I2CM_CTRLA_MODE_I2C_MASTER SERCOM_I2CM_CTRLA_MODE(5)
#endif

static int _start(SercomI2cm *dev, uint8_t address, uint8_t rw_flag);
static inline int _write(SercomI2cm *dev, const uint8_t *data, int length);
static inline int _read(SercomI2cm *dev, uint8_t *data, int length);
static inline void _stop(SercomI2cm *dev);
static inline int _wait_for_response(SercomI2cm *dev, uint32_t max_timeout_cnt);

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[I2C_NUMOF];

/**
 * @brief   Shortcut for accessing the used I2C SERCOM device
 */
static inline SercomI2cm *dev(i2c_t bus)
{
    return i2c_config[bus].dev;
}

int i2c_init_master(i2c_t bus, i2c_speed_t speed)
{

    uint32_t timeout_counter = 0;
    int32_t tmp_baud;

    if (bus >= I2C_NUMOF) {
        return -1;
    }

    /* DISABLE I2C MASTER */
    i2c_poweroff(bus);

    /* Reset I2C */
    dev(bus)->CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;
    while (dev(bus)->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK) {}

    /* Turn on power manager for sercom */
    sercom_clk_en(dev(bus));

    /* I2C using CLK GEN 0 */
    sercom_set_gen(dev(bus),i2c_config[bus].gclk_src);
#if CPU_FAM_SAML21
    /* GCLK_ID_SLOW is shared for SERCOM[0..4] */
    GCLK->PCHCTRL[(sercom_id(dev(bus)) < 5 ?
                  SERCOM0_GCLK_ID_SLOW : SERCOM5_GCLK_ID_SLOW)].reg =
    (GCLK_PCHCTRL_CHEN | i2c_config[bus].gclk_src  );
    while (GCLK->SYNCBUSY.bit.GENCTRL) {}
#else
    /* GCLK_SERCOMx_SLOW is shared for all sercom */
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN |
                         i2c_config[bus].gclk_src |
                         SERCOM0_GCLK_ID_SLOW);
    while (GCLK->STATUS.bit.SYNCBUSY) {}
#endif

    /* Check if module is enabled. */
    if (dev(bus)->CTRLA.reg & SERCOM_I2CM_CTRLA_ENABLE) {
        DEBUG("STATUS_ERR_DENIED\n");
        return -3;
    }
    /* Check if reset is in progress. */
    if (dev(bus)->CTRLA.reg & SERCOM_I2CM_CTRLA_SWRST) {
        DEBUG("STATUS_BUSY\n");
        return -3;
    }

    /************ SERCOM PAD0 - SDA and SERCOM PAD1 - SCL *************/
    gpio_init_mux(i2c_config[bus].sda_pin, i2c_config[bus].mux);
    gpio_init_mux(i2c_config[bus].scl_pin, i2c_config[bus].mux);

    /* I2C CONFIGURATION */
    while (dev(bus)->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK) {}

    /* Set sercom module to operate in I2C master mode and run in Standby
    if user requests it */
    dev(bus)->CTRLA.reg = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER |
                          (i2c_config[bus].runstdby ?
                              SERCOM_I2CM_CTRLA_RUNSTDBY : 0);

    /* Enable Smart Mode (ACK is sent when DATA.DATA is read) */
    dev(bus)->CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;

    /* Find and set baudrate. Read speed configuration. Set transfer
     * speed: SERCOM_I2CM_CTRLA_SPEED(0): Standard-mode (Sm) up to 100
     * kHz and Fast-mode (Fm) up to 400 kHz */
    switch (speed) {
        case I2C_SPEED_NORMAL:
        case I2C_SPEED_FAST:
            dev(bus)->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(0);
            break;
        case I2C_SPEED_HIGH:
            dev(bus)->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(2);
            break;
        default:
            DEBUG("BAD BAUDRATE\n");
            return -2;
    }
    /* Get the baudrate */
    tmp_baud = (int32_t)(((CLOCK_CORECLOCK +
               (2 * (speed)) - 1) / (2 * (speed))) -
               (speed == I2C_SPEED_HIGH ? 1 : 5));
    /* Ensure baudrate is within limits */
    if (tmp_baud < 255 && tmp_baud > 0) {
        dev(bus)->BAUD.reg = SERCOM_I2CM_BAUD_BAUD(tmp_baud);
    }
    /* ENABLE I2C MASTER */
    i2c_poweron(bus);

    /* Start timeout if bus state is unknown. */
    while ((dev(bus)->STATUS.reg &
          SERCOM_I2CM_STATUS_BUSSTATE_Msk) == BUSSTATE_UNKNOWN) {
        if (timeout_counter++ >= SAMD21_I2C_TIMEOUT) {
            /* Timeout, force bus state to idle. */
            dev(bus)->STATUS.reg = BUSSTATE_IDLE;
        }
    }
    return 0;
}

int i2c_acquire(i2c_t bus)
{
    if (bus >= I2C_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[bus]);
    return 0;
}

int i2c_release(i2c_t bus)
{
    if (bus >= I2C_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[bus]);
    return 0;
}

int i2c_read_byte(i2c_t bus, uint8_t address, void *data)
{
    return i2c_read_bytes(bus, address, data, 1);
}

int i2c_read_bytes(i2c_t bus, uint8_t address, void *data, int length)
{
    if (bus >= I2C_NUMOF) {
        return -1;
    }
    /* start transmission and send slave address */
    if (_start(dev(bus), address, I2C_FLAG_READ) < 0) {
        return 0;
    }
    /* read data to register */
    if (_read(dev(bus), data, length) < 0) {
        return 0;
    }
    _stop(dev(bus));
    /* return number of bytes sent */
    return length;
}

int i2c_read_reg(i2c_t bus, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(bus, address, reg, data, 1);
}

int i2c_read_regs(i2c_t bus, uint8_t address, uint8_t reg,
                  void *data, int length)
{
    if (bus >= I2C_NUMOF) {
        return -1;
    }
    /* start transmission and send slave address */
    if (_start(dev(bus), address, I2C_FLAG_WRITE) < 0) {
        return 0;
    }
    /* send register address/command and wait for complete transfer to
     * be finished */
    if (_write(dev(bus), &reg, 1) < 0) {
        return 0;
    }
    return i2c_read_bytes(bus, address, data, length);
}

int i2c_write_byte(i2c_t bus, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(bus, address, &data, 1);
}

int i2c_write_bytes(i2c_t bus, uint8_t address, const void *data, int length)
{
    if (bus >= I2C_NUMOF) {
        return -1;
    }
    if (_start(dev(bus), address, I2C_FLAG_WRITE) < 0) {
        return 0;
    }
    if (_write(dev(bus), data, length) < 0) {
        return 0;
    }
    _stop(dev(bus));
    return length;
}


int i2c_write_reg(i2c_t bus, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(bus, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t bus, uint8_t address, uint8_t reg,
                   const void *data, int length)
{
    if (bus >= I2C_NUMOF) {
        return -1;
    }
    /* start transmission and send slave address */
    if (_start(dev(bus), address, I2C_FLAG_WRITE) < 0) {
        return 0;
    }
    /* send register address and wait for complete transfer to be finished */
    if (_write(dev(bus), &reg, 1) < 0) {
        return 0;
    }
    /* write data to register */
    if (_write(dev(bus), data, length) < 0) {
        return 0;
    }
    /* finish transfer */
    _stop(dev(bus));
    return length;
}

void i2c_poweron(i2c_t bus)
{
    if (bus >= I2C_NUMOF) {
        DEBUG("i2c wrong bus selected\n");
        return;
    }
    if (dev(bus) == NULL) {
        return;
    }
    dev(bus)->CTRLA.bit.ENABLE = 1;
    while (dev(bus)->SYNCBUSY.bit.ENABLE) {}
}

void i2c_poweroff(i2c_t bus)
{
    if (bus >= I2C_NUMOF) {
        DEBUG("i2c wrong bus selected\n");
        return;
    }
    if (dev(bus) == NULL) {
        return;
    }
    dev(bus)->CTRLA.bit.ENABLE = 0;
    while (dev(bus)->SYNCBUSY.bit.ENABLE) {}
}

static int _start(SercomI2cm *dev, uint8_t address, uint8_t rw_flag)
{
    /* Wait for hardware module to sync */
    DEBUG("Wait for device to be ready\n");
    while (dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK) {}

    /* Set action to ACK. */
    dev->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

    /* Send Start | Address | Write/Read */
    DEBUG("Generate start condition by sending address\n");
    dev->ADDR.reg = (address << 1) | rw_flag | (0 << SERCOM_I2CM_ADDR_HS_Pos);

    /* Wait for response on bus. */
    if (rw_flag == I2C_FLAG_READ) {
        /* Some devices (e.g. SHT2x) can hold the bus
           while preparing the reply */
        if (_wait_for_response(dev, 100 * SAMD21_I2C_TIMEOUT) < 0)
            return -1;
    }
    else {
        if (_wait_for_response(dev, SAMD21_I2C_TIMEOUT) < 0)
            return -1;
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
            return -2;
        }
    }
    /* Check that slave responded with ack. */
    else if (dev->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
        /* Slave busy. Issue ack and stop command. */
        dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
        DEBUG("STATUS_ERR_BAD_ADDRESS\n");
        return -3;
    }
    return 0;
}

static inline int _write(SercomI2cm *dev, const uint8_t *data, int length)
{
    uint16_t tmp_data_length = length;
    uint16_t buffer_counter = 0;

    /* Write data buffer until the end. */
    DEBUG("Looping through bytes\n");
    while (tmp_data_length--) {
        /* Check that bus ownership is not lost. */
        if ((dev->STATUS.reg
            & SERCOM_I2CM_STATUS_BUSSTATE_Msk) != BUSSTATE_OWNER) {
            DEBUG("STATUS_ERR_PACKET_COLLISION\n");
            return -2;
        }

        /* Wait for hardware module to sync */
        while (dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK) {}

        DEBUG("Written byte #%i to data reg, now waiting for DR \
               to be empty again\n", buffer_counter);
        dev->DATA.reg = data[buffer_counter++];

        /* Wait for response on bus. */
        if (_wait_for_response(dev, SAMD21_I2C_TIMEOUT) < 0)
            return -1;

        /* Check for NACK from slave. */
        if (dev->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
            DEBUG("STATUS_ERR_OVERFLOW\n");
            return -4;
        }
    }
    return 0;
}

static inline int _read(SercomI2cm *dev, uint8_t *data, int length)
{
    uint8_t count = 0;

    /* Set action to ack. */
    dev->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

    /* Read data buffer. */
    while (count != length) {
        /* Check that bus ownership is not lost. */
        if ((dev->STATUS.reg
             & SERCOM_I2CM_STATUS_BUSSTATE_Msk) != BUSSTATE_OWNER) {
            DEBUG("STATUS_ERR_PACKET_COLLISION\n");
            return -2;
        }

        /* Wait for hardware module to sync */
        while (dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK) {}
        /* Save data to buffer. */
        data[count] = dev->DATA.reg;

        /* Wait for response on bus. */
        if (_wait_for_response(dev, SAMD21_I2C_TIMEOUT) < 0)
            return -1;

        count++;
    }
    /* Send NACK before STOP */
    dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
    return 0;
}

static inline void _stop(SercomI2cm *dev)
{
    /* Wait for hardware module to sync */
    while (dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK) {}
    /* Stop command */
    dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
    /* Wait for bus to be idle again */
    while ((dev->STATUS.reg
            & SERCOM_I2CM_STATUS_BUSSTATE_Msk) != BUSSTATE_IDLE) {}
    DEBUG("Stop sent\n");
}

static inline int _wait_for_response(SercomI2cm *dev, uint32_t max_timeout_cnt)
{
    uint32_t timeout_counter = 0;
    DEBUG("Wait for response.\n");
    while (!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)
           && !(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) {
        if (++timeout_counter >= max_timeout_cnt) {
            DEBUG("STATUS_ERR_TIMEOUT\n");
            return -1;
        }
    }
    return 0;
}
