/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>

#include "cpu.h"
#include "cpu_pm.h"
#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph/pm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Device Context allocation
 */
static struct {
    mutex_t locks;
    mutex_t xfer;
    uint8_t *buffer;
    size_t pos;
    size_t len;
    uint8_t flags;
    int8_t status;
} i2c_ctx[I2C_NUMOF];

/**
 * @brief   Get the pointer to the base register of the given I2C device
 *
 * @param[in] dev       I2C device identifier
 *
 * @return              base register address
 */
static inline TWI_t* dev(i2c_t dev)
{
    return ((TWI_t*) (i2c_config[dev].dev));
}

static inline uint8_t _i2c_calc_baud(i2c_t i2c)
{
    uint16_t ftwi = (((CLOCK_CORECLOCK / (2*i2c_config[i2c].speed))-5)+1);

    if (ftwi > 255) {
        DEBUG("[i2c] init: dev speed is lower than minimal speed supported.\n");
        return 0;
    }
    if (ftwi < 1) {
        DEBUG("[i2c] init: dev speed is high than maximum speed supported.\n");
        return 0;
    }

    return ftwi & 0xff;
}

void i2c_init(i2c_t i2c)
{
    uint8_t baudrate;

    assert((unsigned)i2c < I2C_NUMOF);

    baudrate = _i2c_calc_baud(i2c);
    if (baudrate == 0) {
        DEBUG("[i2c] init: baudrate is invalid.\n");
        return;
    }

    mutex_init(&i2c_ctx[i2c].locks);
    mutex_init(&i2c_ctx[i2c].xfer);
    mutex_lock(&i2c_ctx[i2c].xfer);
    i2c_init_pins(i2c);

    pm_periph_enable(i2c_config[i2c].pwr);
    dev(i2c)->MASTER.BAUD = baudrate;
    pm_periph_disable(i2c_config[i2c].pwr);
}

void i2c_init_pins(i2c_t i2c)
{
    assert((unsigned)i2c < I2C_NUMOF);
    gpio_init(i2c_config[i2c].sda_pin, GPIO_OPC_WRD_AND_PULL);
    gpio_init(i2c_config[i2c].scl_pin, GPIO_OPC_WRD_AND_PULL);
}

void i2c_acquire(i2c_t i2c)
{
    assert((unsigned)i2c < I2C_NUMOF);
    DEBUG("acquire\n");
    pm_block(3);
    mutex_lock(&i2c_ctx[i2c].locks);
    pm_periph_enable(i2c_config[i2c].pwr);

    dev(i2c)->MASTER.CTRLA = (i2c_config[i2c].int_lvl << TWI_MASTER_INTLVL_gp)
                           | TWI_MASTER_RIEN_bm
                           | TWI_MASTER_WIEN_bm
                           | TWI_MASTER_ENABLE_bm;
    dev(i2c)->MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;
}

void i2c_release(i2c_t i2c)
{
    assert((unsigned)i2c < I2C_NUMOF);
    dev(i2c)->MASTER.CTRLA = 0;
    pm_periph_disable(i2c_config[i2c].pwr);
    mutex_unlock(&i2c_ctx[i2c].locks);
    pm_unblock(3);
    DEBUG("release\n");
}

static int _i2c_transaction(i2c_t i2c, uint16_t addr, const void *data,
                            size_t len, uint8_t flags, bool is_read)
{
    assert((unsigned)i2c < I2C_NUMOF);

    if (flags & I2C_ADDR10) {
        DEBUG("[i2c] xfer: no 10 bit address support.\n");
        return -EOPNOTSUPP;
    }
    if (data == NULL || len == 0) {
        DEBUG("[i2c] xfer: invalid parameter.\n");
        return -EINVAL;
    }

    i2c_ctx[i2c].buffer = (uint8_t *) data;
    i2c_ctx[i2c].pos = 0;
    i2c_ctx[i2c].len = len;
    i2c_ctx[i2c].flags = flags;

    dev(i2c)->MASTER.ADDR = (addr << 1) | (is_read ? 0x01 : 0x00);

    mutex_lock(&i2c_ctx[i2c].xfer);

    DEBUG("[i2c] xfer: status = %d\n", i2c_ctx[i2c].status);

    return i2c_ctx[i2c].status;
}

int i2c_write_bytes(i2c_t i2c, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    return _i2c_transaction(i2c, addr, data, len, flags, false);
}

int i2c_read_bytes(i2c_t i2c, uint16_t addr, void *data, size_t len,
                   uint8_t flags)
{
    return _i2c_transaction(i2c, addr, data, len, flags, true);
}

/**
 * @internal
 *
 * @brief TWI master write interrupt handler.
 *
 *  Handles TWI transactions (master write) and responses to (N)ACK.
 */
static inline void _i2c_write_handler(int i2c)
{
    if (i2c_ctx[i2c].pos < i2c_ctx[i2c].len) {
        const uint8_t* const data = i2c_ctx[i2c].buffer;
        dev(i2c)->MASTER.DATA = data[i2c_ctx[i2c].pos++];
    }
    else {
        /* Send STOP condition to complete the transaction. */
        if (!(i2c_ctx[i2c].flags & I2C_NOSTOP)) {
            dev(i2c)->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
        }

        i2c_ctx[i2c].status = 0;
        mutex_unlock(&i2c_ctx[i2c].xfer);
    }
}

/**
 * @internal
 *
 * @brief TWI master read interrupt handler.
 *
 * This is the master read interrupt handler that takes care of reading bytes
 * from the TWI slave device.
 */
static inline void _i2c_read_handler(int i2c)
{
    if (i2c_ctx[i2c].pos < i2c_ctx[i2c].len) {
        uint8_t* const data = i2c_ctx[i2c].buffer;
        data[i2c_ctx[i2c].pos++] = dev(i2c)->MASTER.DATA;

        /* If there is more to read, issue ACK and start a byte read.
         * Otherwise, issue NACK and STOP to complete the transaction.
         */
        if (i2c_ctx[i2c].pos < i2c_ctx[i2c].len) {
            dev(i2c)->MASTER.CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
        }
        else {
            uint8_t control_c = TWI_MASTER_ACKACT_bm;

            if (!(i2c_ctx[i2c].flags & I2C_NOSTOP)) {
                control_c |= TWI_MASTER_CMD_STOP_gc;
            }

            dev(i2c)->MASTER.CTRLC = control_c;

            i2c_ctx[i2c].status = 0;
            mutex_unlock(&i2c_ctx[i2c].xfer);
        }
    } else {
        /* Issue STOP and buffer overflow condition. */
        dev(i2c)->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;

        i2c_ctx[i2c].status = -ENOMEM;
        mutex_unlock(&i2c_ctx[i2c].xfer);
    }
}

/**
 * @internal
 *
 * @brief Common TWI master interrupt service routine.
 *
 *  Check current status and calls the appropriate handler.
 */
static inline void isr_handler(int i2c)
{
    assert((unsigned)i2c < I2C_NUMOF);

    avr8_enter_isr();

    int8_t const m_status = dev(i2c)->MASTER.STATUS;

    if (m_status & TWI_MASTER_ARBLOST_bm) {
        dev(i2c)->MASTER.STATUS = m_status | TWI_MASTER_ARBLOST_bm;
        dev(i2c)->MASTER.CTRLC  = TWI_MASTER_CMD_STOP_gc;
        i2c_ctx[i2c].status = -EBUSY;
        mutex_unlock(&i2c_ctx[i2c].xfer);
    }
    else if ((m_status & TWI_MASTER_BUSERR_bm)
         ||  (m_status & TWI_MASTER_RXACK_bm)) {

        dev(i2c)->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
        i2c_ctx[i2c].status = ((m_status & TWI_MASTER_BUSERR_bm) > 0)
                            ? -EIO
                            : -ENXIO;
        mutex_unlock(&i2c_ctx[i2c].xfer);
    }
    else if (m_status & TWI_MASTER_WIF_bm) {
        _i2c_write_handler(i2c);
    }
    else if (m_status & TWI_MASTER_RIF_bm) {
        _i2c_read_handler(i2c);
    }
    else {
        i2c_ctx[i2c].status = -EPROTO;
        mutex_unlock(&i2c_ctx[i2c].xfer);
    }

    avr8_exit_isr();
}

#ifdef I2C_0_ISR
ISR(I2C_0_ISR, ISR_BLOCK)
{
    isr_handler(0);
}
#endif /* I2C_0_ISR */

#ifdef I2C_1_ISR
ISR(I2C_1_ISR, ISR_BLOCK)
{
    isr_handler(1);
}
#endif /* I2C_1_ISR */

#ifdef I2C_2_ISR
ISR(I2C_2_ISR, ISR_BLOCK)
{
    isr_handler(2);
}
#endif /* I2C_2_ISR */

#ifdef I2C_3_ISR
ISR(I2C_3_ISR, ISR_BLOCK)
{
    isr_handler(3);
}
#endif /* I2C_3_ISR */
