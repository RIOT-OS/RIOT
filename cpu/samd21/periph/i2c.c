/*
 * Copyright (C) 2014 CLENET Baptiste
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup  cpu_samd21
 * @{
 * @file
 * @brief       Low-level I2C driver implementation
 * @author      Baptiste Clenet <baptiste.clenet@xsoen.com>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "board.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"
#include "instance/instance_sercom3.h"

#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* guard file in case no I2C device is defined */
#if I2C_NUMOF

#define SAMD21_I2C_TIMEOUT  (65535)

/* static function definitions */
static inline void _start(SercomI2cm *dev, uint8_t address, uint8_t rw_flag);
static inline void _write(SercomI2cm *dev, char *data, int length);
static inline void _read(SercomI2cm *dev, char *data, int length);
static inline void _stop(SercomI2cm *dev);

/**
 * @brief Array holding one pre-initialized mutex for each I2C device
 */
static mutex_t locks[] = {
#if I2C_0_EN
    [I2C_0] = MUTEX_INIT,
#endif
#if I2C_1_EN
    [I2C_1] = MUTEX_INIT,
#endif
#if I2C_2_EN
    [I2C_2] = MUTEX_INIT
#endif
#if I2C_3_EN
    [I2C_3] = MUTEX_INIT
#endif
};

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    SercomI2cm *I2CSercom = 0;
    PortGroup *port_group;
    uint8_t pin_scl = 0;
    uint8_t pin_sda = 0;
    uint32_t i2c_pins = 0;
    uint32_t clock_source_speed = 0;
    uint8_t sercom_core = 0;
    uint8_t sercom_gclk_id_slow = 0;

    uint32_t timeout_counter = 0;
    int32_t tmp_baud;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            I2CSercom = &I2C_0_DEV;
            port_group = &I2C_0_PORT;
            pin_scl = I2C_SDA;
            pin_sda = I2C_SCL;
            i2c_pins = I2C_0_PINS;
            clock_source_speed = I2C_0_REF_F;
            sercom_core = SERCOM3_GCLK_ID_CORE;
            sercom_gclk_id_slow = SERCOM3_GCLK_ID_SLOW ;
            break;
#endif
        default:
            DEBUG("I2C FALSE VALUE\n");
            return -1;
    }
    /* DISABLE I2C MASTER */
    i2c_poweroff(dev);

    /* Reset I2C */
    I2CSercom->CTRLA.reg = SERCOM_I2CS_CTRLA_SWRST;
    while(I2CSercom->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);

    /* Turn on power manager for sercom */
    PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0 << (sercom_core - 20));

    /* I2C using CLK GEN 0 */
    GCLK->CLKCTRL.reg = (uint32_t)(GCLK_CLKCTRL_CLKEN
                                   | GCLK_CLKCTRL_GEN_GCLK0 << GCLK_CLKCTRL_GEN_Pos
                                   | (sercom_core << GCLK_CLKCTRL_ID_Pos));
    while (GCLK->STATUS.bit.SYNCBUSY);

    GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN
                                   | GCLK_CLKCTRL_GEN_GCLK0 << GCLK_CLKCTRL_GEN_Pos
                                   | (sercom_gclk_id_slow << GCLK_CLKCTRL_ID_Pos));
    while (GCLK->STATUS.bit.SYNCBUSY);


    /* Check if module is enabled. */
    if (I2CSercom->CTRLA.reg & SERCOM_I2CM_CTRLA_ENABLE) {
        DEBUG("STATUS_ERR_DENIED\n");
        return -3;
    }
    /* Check if reset is in progress. */
    if (I2CSercom->CTRLA.reg & SERCOM_I2CM_CTRLA_SWRST) {
        DEBUG("STATUS_BUSY\n");
        return -3;
    }

    /************ SERCOM PAD0 - SDA and SERCOM PAD1 - SCL *************/
    /* DIR + INEN at one: in/out pin. DIRSET modifies DIR in order to set I2C SDA/SCL on output */
    port_group->DIRSET.reg = (1 << pin_scl);
    port_group->DIRSET.reg = (1 << pin_sda);

    /* The Write Configuration register (WRCONFIG) requires the
     * pins to to grouped into two 16-bit half-words (PIN are set up to 31) */
    uint32_t lower_pin_mask = (i2c_pins & 0xFFFF);
    uint32_t upper_pin_mask = (i2c_pins >> 16);

    port_group->WRCONFIG.reg =
          (lower_pin_mask << PORT_WRCONFIG_PINMASK_Pos) \
        | PORT_WRCONFIG_PMUXEN \
        | PORT_WRCONFIG_PMUX(0x3) \
        | PORT_WRCONFIG_INEN \
        | PORT_WRCONFIG_WRPMUX \
        | PORT_WRCONFIG_WRPINCFG;

    port_group->WRCONFIG.reg =
          (upper_pin_mask << PORT_WRCONFIG_PINMASK_Pos) \
        | PORT_WRCONFIG_PMUXEN \
        | PORT_WRCONFIG_PMUX(0x3) \
        | PORT_WRCONFIG_INEN \
        | PORT_WRCONFIG_WRPMUX \
        | PORT_WRCONFIG_WRPINCFG \
        | PORT_WRCONFIG_HWSEL;


    /* I2C CONFIGURATION */
    while(I2CSercom->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);

    /* Set sercom module to operate in I2C master mode. */
    I2CSercom->CTRLA.reg = SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;

    /* Enable Smart Mode (ACK is sent when DATA.DATA is read) */
    I2CSercom->CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;

    /* Find and set baudrate. Read speed configuration. Set transfer
     * speed: SERCOM_I2CM_CTRLA_SPEED(0): Standard-mode (Sm) up to 100
     * kHz and Fast-mode (Fm) up to 400 kHz */
    switch (speed) {
        case I2C_SPEED_NORMAL:
            tmp_baud = (int32_t)(((clock_source_speed + (2*(100000)) - 1) / (2*(100000))) - 5);
            if (tmp_baud < 255 && tmp_baud > 0) {
                I2CSercom->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(0);
                I2CSercom->BAUD.reg = SERCOM_I2CM_BAUD_BAUD(tmp_baud);
            }
            break;
        case I2C_SPEED_FAST:
            tmp_baud = (int32_t)(((clock_source_speed + (2*(400000)) - 1) / (2*(400000))) - 5);
            if (tmp_baud < 255 && tmp_baud > 0) {
                I2CSercom->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(0);
                I2CSercom->BAUD.reg = SERCOM_I2CM_BAUD_BAUD(tmp_baud);
            }
            break;
        case I2C_SPEED_HIGH:
            tmp_baud = (int32_t)(((clock_source_speed + (2*(3400000)) - 1) / (2*(3400000))) - 1);
            if (tmp_baud < 255 && tmp_baud > 0) {
                I2CSercom->CTRLA.reg |= SERCOM_I2CM_CTRLA_SPEED(2);
                I2CSercom->BAUD.reg =SERCOM_I2CM_BAUD_HSBAUD(tmp_baud);
            }
            break;
        default:
            DEBUG("BAD BAUDRATE\n");
            return -2;
    }

    /* ENABLE I2C MASTER */
    i2c_poweron(dev);

    /* Start timeout if bus state is unknown. */
    while (!(I2CSercom->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(1))) {
        if(timeout_counter++ >= SAMD21_I2C_TIMEOUT) {
            /* Timeout, force bus state to idle. */
            I2CSercom->STATUS.reg = SERCOM_I2CM_STATUS_BUSSTATE(1);
        }
    }
    return 0;
}

int i2c_init_slave(i2c_t dev, uint8_t address)
{
    /* TODO */
    return 0;
}

int i2c_acquire(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, char *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    SercomI2cm *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = &I2C_0_DEV;
            break;
#endif
        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_READ);
    /* read data to register */
    _read(i2c, data, length);
    _stop(i2c);
    /* return number of bytes sent */
    return length;
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
    SercomI2cm *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = &I2C_0_DEV;
            break;
#endif
        default:
            return -1;
    }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_WRITE);
    /* send register address/command and wait for complete transfer to
     * be finished */
    _write(i2c, (char *)(&reg), 1);
    return i2c_read_bytes(dev, address, data, length);
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    SercomI2cm *I2CSercom;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            I2CSercom = &I2C_0_DEV;
            break;
#endif
        default:
            return -1;
    }

    _start(I2CSercom, address, I2C_FLAG_WRITE);
    _write(I2CSercom, data, length);
    _stop(I2CSercom);
    return length;
}


int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
    SercomI2cm *i2c;

    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            i2c = &I2C_0_DEV;
            break;
#endif
        default:
            return -1;
        }

    /* start transmission and send slave address */
    _start(i2c, address, I2C_FLAG_WRITE);
    /* send register address and wait for complete transfer to be finished */
    _write(i2c, (char *)(&reg), 1);
    /* write data to register */
    _write(i2c, data, length);
    /* finish transfer */
    _stop(i2c);
    return length;
}

void i2c_poweron(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            while(I2C_0_DEV.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
            I2C_0_DEV.CTRLA.bit.ENABLE = 1;
            break;
#endif
        default:
            return;
    }
}

void i2c_poweroff(i2c_t dev)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            while(I2C_0_DEV.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
            I2C_0_DEV.CTRLA.reg |= SERCOM_I2CM_CTRLA_ENABLE;
            break;
#endif
        default:
            return;
    }
}

static void _start(SercomI2cm *dev, uint8_t address, uint8_t rw_flag)
{
    uint32_t timeout_counter = 0;

    /* Wait for hardware module to sync */
    DEBUG("Wait for device to be ready\n");
    while(dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);

    /* Set action to ACK. */
    dev->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

    /* Send Start | Address | Write/Read */
    DEBUG("Generate start condition by sending address\n");
    dev->ADDR.reg = (address << 1) | rw_flag | (0 << SERCOM_I2CM_ADDR_HS_Pos);

    /* Wait for response on bus. */
    while (!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)
           && !(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) {
        if (++timeout_counter >= SAMD21_I2C_TIMEOUT) {
            DEBUG("STATUS_ERR_TIMEOUT\n");
            return;
        }
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
            return;
        }
    }
    /* Check that slave responded with ack. */
    else if (dev->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
        /* Slave busy. Issue ack and stop command. */
        dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
        DEBUG("STATUS_ERR_BAD_ADDRESS\n");
        return;
    }
}

static inline void _write(SercomI2cm *dev, char *data, int length)
{
    uint16_t tmp_data_length = length;
    uint32_t timeout_counter = 0;
    uint16_t buffer_counter = 0;

    /* Write data buffer until the end. */
    DEBUG("Looping through bytes\n");
    while (tmp_data_length--) {
        /* Check that bus ownership is not lost. */
        if (!(dev->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(2))) {
            DEBUG("STATUS_ERR_PACKET_COLLISION\n");
            return;
        }

        /* Wait for hardware module to sync */
        while(dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);

        DEBUG("Written %i byte to data reg, now waiting for DR to be empty again\n", buffer_counter);
        dev->DATA.reg = data[buffer_counter++];

        DEBUG("Wait for response.\n");
        while (!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)
               && !(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) {
            if (++timeout_counter >= SAMD21_I2C_TIMEOUT) {
                DEBUG("STATUS_ERR_TIMEOUT\n");
                return;
            }
        }

        /* Check for NACK from slave. */
        if (dev->STATUS.reg & SERCOM_I2CM_STATUS_RXNACK) {
            DEBUG("STATUS_ERR_OVERFLOW\n");
            return;
        }
    }
}

static inline void _read(SercomI2cm *dev, char *data, int length)
{
    uint32_t timeout_counter = 0;
    uint8_t count = 0;

    /* Set action to ack. */
    dev->CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

    /* Read data buffer. */
    while (count != length) {
        /* Check that bus ownership is not lost. */
        if (!(dev->STATUS.reg & SERCOM_I2CM_STATUS_BUSSTATE(2))) {
            DEBUG("STATUS_ERR_PACKET_COLLISION\n");
            return;
        }

        /* Wait for hardware module to sync */
        while(dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
        /* Save data to buffer. */
        data[count] = dev->DATA.reg;

        /* Wait for response. */
        timeout_counter = 0;
        while (!(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)
               && !(dev->INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB)) {
            if (++timeout_counter >= SAMD21_I2C_TIMEOUT) {
                DEBUG("STATUS_ERR_TIMEOUT\n");
                return;
            }
        }
        count++;
    }
    /* Send NACK before STOP */
    dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
}

static inline void _stop(SercomI2cm *dev)
{
    /* Wait for hardware module to sync */
    while(dev->SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_MASK);
    /* Stop command */
    dev->CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
    DEBUG("Stop sent\n");
}

#endif /* I2C_NUMOF */
