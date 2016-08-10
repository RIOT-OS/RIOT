/*
 *    Copyright (c) 2016 Thomas Stilwell <stilwellt@openlabs.co>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Thomas Stilwell <stilwellt@openlabs.co>
 *
 * @}
 */

#include "periph/i2c.h"
#include "mutex.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

mutex_t i2c_mutex = MUTEX_INIT;
mutex_t irq_wait = MUTEX_INIT;

static void _set_address(dev_t dev, uint8_t slave_address, bool read)
{
    I2C->MSA = (slave_address << 1) | (read ? 1 : 0);
}

static uint32_t _speed_t_to_scl_period(i2c_speed_t speed)
{
    uint32_t hz;
    switch (speed) {
        default:
        case I2C_SPEED_LOW:
            hz = 10000;
            break;
        case I2C_SPEED_NORMAL:
            hz = 100000;
            break;
        case I2C_SPEED_FAST:
            hz = 400000;
            break;
        case I2C_SPEED_FAST_PLUS:
            hz = 1000000;
            break;
        case I2C_SPEED_HIGH:
            hz = 3400000;
            break;
    }

    /* document SWCU117F page 1506 equation 8 */
    int32_t scl_period = CLOCK_CORECLOCK / (2 * (6 + 4) * hz) - 1;

    /* valid range is 1 to 127 inclusive */
    if (scl_period > 127) {
        /* slowest possible i2c speed for given core clock */
        scl_period = 127;
    }
    else if (scl_period < 1) {
        /* fastest possible i2c speed for given core clock */
        scl_period = 1;
    }

    /* i2c speed = core_speed / ((scl_period + 1) * 20) */
    return scl_period;
}

static void _transmit_and_yield_until_done(dev_t dev, uint32_t command)
{
#ifndef CC26X0_I2C_NO_INTERRUPTS
    /* clear interrupt flag */
    I2C->MICR = I2C_MICR_IC;

    /* enable i2c interrupt */
    I2C->MIMR = I2C_MIMR_IM;

    /* do first lock early to avoid race condition after MCTRL write */
    mutex_lock(&irq_wait);

    /* initiate i2c transaction */
    I2C->MCTRL = command;

    /* wait for interrupt to signal transaction complete */
    mutex_lock(&irq_wait);
    mutex_unlock(&irq_wait);

#else
    /* initiate i2c transaction */
    I2C->MCTRL = command;

    /* delay 4 SYSBUS clocks after MCTRL write to allow MSTAT_BUSY to be set */
    xtimer_spin(4);

    /* wait for transaction to complete */
    while(I2C->MSTAT & I2C_MSTAT_BUSY);
#endif
}

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    /* make sure dev is valid */
    if(dev >= I2C_NUMOF){
        return -1;
    }

    /* make sure serial power domain is enabled */
    PRCM->PDCTL0SERIAL = 1;
    while(!(PRCM->PDSTAT0 & PDSTAT0_SERIAL_ON));

    /* enable i2c peripheral clock */
    PRCM->I2CCLKGR = 1;
    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while(!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE));

    /* configure pins and route them to i2c peripheral */
    uint32_t pin_conf = IOC_CURRENT_2MA | IOC_STRENGTH_AUTO | IOC_IOPULL_UP |
        IOC_SLEW_DISABLE | IOC_HYST_DISABLE | IOC_NO_EDGE | IOC_INT_DISABLE |
        IOC_IOMODE_OPEN_DRAIN_NORMAL | IOC_NO_WAKE_UP | IOC_INPUT_ENABLE;
    IOC->CFG[i2c_config[dev].scl_pin] =  IOCFG_PORTID_I2C_MSSCL | pin_conf;
    IOC->CFG[i2c_config[dev].sda_pin] =  IOCFG_PORTID_I2C_MSSDA | pin_conf;

    /* initialize i2c peripheral in master mode */
    I2C->MCR = I2C_MCR_MFE;

    /* enable master mode */
    I2C->MCTRL = I2C_MCTRL_RUN;

    /* configure bus speed */
    I2C->MTPR = _speed_t_to_scl_period(speed);

    /* enable nvic irq */
    NVIC_EnableIRQ(I2C_IRQN);

    return 0;
}

int i2c_init_slave(i2c_t dev, uint8_t address)
{
    /* TODO: implement slave mode */
    return -1;
}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    if(length <= 0) {
        return -1;
    }

    /* configure slave address */
    _set_address(dev, address, false);

    /* make sure bus is idle */
    while(I2C->MSTAT & I2C_MSTAT_BUSBSY);

    /* handle each data byte */
    for(int i = 0; i < length; i++) {

        /* make sure peripheral is idle */
        while(I2C->MSTAT & I2C_MSTAT_BUSY);

        /* load data byte into transmit buffer */
        I2C->MDR = data[i];

        /* choose command */
        uint32_t command;
        if(length == 1) { /* only one byte */
            command = I2C_MASTER_CMD_SINGLE_SEND;
        }
        else if(i == 0) { /* first byte of multiple */
            command = I2C_MASTER_CMD_BURST_SEND_START;
        }
        else if(i == length - 1) { /* last byte of multiple */
            command = I2C_MASTER_CMD_BURST_SEND_FINISH;
        }
        else { /* middle byte of multiple */
            command = I2C_MASTER_CMD_BURST_SEND_CONT;
        }

        /* begin transaction and wait for completion */
        _transmit_and_yield_until_done(dev, command);

        /* quit on transaction error */
        uint32_t mstat = I2C->MSTAT;
        if(mstat & I2C_MSTAT_ERR) {
            printf("I2C error; MSTAT: 0x%02lx\n", mstat);

            /* send stop */
            _transmit_and_yield_until_done(
                dev, I2C_MASTER_CMD_BURST_SEND_ERROR_STOP
            );

            return -2;
        }
    }

    return 0;
}

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    if(length <= 0) {
        return -1;
    }

    /* configure slave address */
    _set_address(dev, address, true);

    /* make sure bus is idle */
    while(I2C->MSTAT & I2C_MSTAT_BUSBSY);

    /* handle each data byte */
    for(int i = 0; i < length; i++) {

        /* make sure peripheral is idle */
        while(I2C->MSTAT & I2C_MSTAT_BUSY);

        /* choose command */
        uint32_t command;
        if(length == 1) { /* only one byte */
            command = I2C_MASTER_CMD_SINGLE_RECEIVE;
        }
        else if(i == 0) { /* first byte of multiple */
            command = I2C_MASTER_CMD_BURST_RECEIVE_START;
        }
        else if(i == length - 1) { /* last byte of multiple */
            command = I2C_MASTER_CMD_BURST_RECEIVE_FINISH;
        }
        else { /* middle byte of multiple */
            command = I2C_MASTER_CMD_BURST_RECEIVE_CONT;
        }

        /* begin transaction and wait for completion */
        _transmit_and_yield_until_done(dev, command);

        /* quit on transaction error */
        uint32_t mstat = I2C->MSTAT;
        if(mstat & I2C_MSTAT_ERR) {
            DEBUG("I2C error; MSTAT: 0x%02lx\n", mstat);

            /* send stop */
            _transmit_and_yield_until_done(
                dev, I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP
            );

            return -2;
        }

        /* read byte from receive buffer */
        data[i] = I2C->MDR;
    }

    return 0;
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char* data, int length)
{
    /* TODO unimplemented */
    return -1;
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char* data, int length)
{
    /* TODO unimplemented */
    return -1;
}

int i2c_acquire(i2c_t dev)
{
    if(dev >= I2C_NUMOF){
        return -1;
    }
    mutex_lock(&i2c_mutex);
    return 0;
}

int i2c_release(i2c_t dev)
{
    if(dev >= I2C_NUMOF){
        return -1;
    }
    mutex_unlock(&i2c_mutex);
    return 0;
}

int i2c_read_byte(i2c_t dev, uint8_t address, char* data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char* data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

#if I2C_0_EN
void isr_i2c(void)
{
    /* disable i2c interrupt */
    I2C->MIMR = 0;

    /* clear interrupt flag */
    I2C->MICR = I2C_MICR_IC;

    /* signal transaction complete */
    mutex_unlock(&irq_wait);

    if (sched_context_switch_request) {
        thread_yield();
    }
}
#endif
