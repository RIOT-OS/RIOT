/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation for GD32VF103
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include "cpu.h"
#include "irq.h"
#include "mutex.h"
#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif
#include "panic.h"

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define I2C_TIMEOUT_CYCLES      1000        /* clock cycles */
#define I2C_IRQ_PRIO            (1)

#define I2C_ERROR_FLAGS_USED    (I2C_STAT0_AERR_Msk | I2C_STAT0_LOSTARB_Msk | \
                                 I2C_STAT0_BERR_Msk)
#define I2C_ERROR_FLAGS_OTHER   (I2C_STAT0_OUERR_Msk | I2C_STAT0_PECERR_Msk | \
                                 I2C_STAT0_SMBTO_Msk | I2C_STAT0_SMBALT_Msk)
#define I2C_ERROR_FLAGS         (I2C_ERROR_FLAGS_USED | I2C_ERROR_FLAGS_OTHER)

#define I2C_INT_EV_ERR_FLAGS    (I2C_CTL1_EVIE_Msk | I2C_CTL1_ERRIE_Msk)
#define I2C_INT_ALL_FLAGS       (I2C_CTL1_BUFIE_Msk | I2C_INT_EV_ERR_FLAGS)

/* static function definitions */
static void _init(i2c_t dev);
static void _init_pins(i2c_t dev);
static void _init_clk(I2C_Type *i2c, uint32_t speed);
static void _deinit_pins(i2c_t dev);
static int  _wait_for_bus(i2c_t dev);
static void _wait_for_irq(i2c_t dev);
static void _irq_handler(unsigned irqn);

typedef enum {
    I2C_OK,
    I2C_START_SENT,
    I2C_ADDR_SENT,
    I2C_ADDR10_SENT,
    I2C_BT_COMPLETE,
    I2C_TXB_EMPTY,
    I2C_RXB_NOT_EMPTY,
    I2C_RXB_NOT_EMPTY_BT_COMPLETE,
    I2C_TIMEOUT,
    I2C_ACK_ERR,
    I2C_ARB_LOST,
    I2C_BUS_ERROR,
    I2C_OTHER_ERROR,
} _i2c_state_t;

typedef struct {
    mutex_t dev_lock;
    mutex_t irq_lock;
    _i2c_state_t state;
    void (*isr)(unsigned irqn);
} _i2c_dev_t;

_i2c_dev_t _i2c_dev[] = {
    {
        .dev_lock = MUTEX_INIT,
        .irq_lock = MUTEX_INIT_LOCKED,
        .state = I2C_OK,
    },
    {
        .dev_lock = MUTEX_INIT,
        .irq_lock = MUTEX_INIT_LOCKED,
        .state = I2C_OK,
    },
};

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    assert(i2c_config[dev].dev != NULL);

    /* Configure pins in idle state as open drain outputs to keep the bus lines
     * in HIGH state */
    _deinit_pins(dev);

    periph_clk_en(APB1, i2c_config[dev].rcu_mask);

    /* enable set event interrupt handler and enable the event interrupt */
    clic_set_handler(i2c_config[dev].irqn, _irq_handler);
    clic_enable_interrupt(i2c_config[dev].irqn, CPU_DEFAULT_IRQ_PRIO);
    /* enable set error interrupt handler and enable the error interrupt */
    clic_set_handler(i2c_config[dev].irqn + 1, _irq_handler);
    clic_enable_interrupt(i2c_config[dev].irqn + 1, CPU_DEFAULT_IRQ_PRIO);

    _init(dev);

    _i2c_dev[dev].state = I2C_OK;
}

static void _init_pins(i2c_t dev)
{
    /* This is needed in case the remapped pins are used */
    if (i2c_config[dev].scl_pin == GPIO_PIN(PORT_B, 8) ||
        i2c_config[dev].sda_pin == GPIO_PIN(PORT_B, 9)) {
        /* The remapping periph clock must first be enabled */
        RCU->APB2EN |= RCU_APB2EN_AFEN_Msk;
        /* Then the remap can occur */
        AFIO->PCF0 |= AFIO_PCF0_I2C0_REMAP_Msk;
    }
    gpio_init_af(i2c_config[dev].scl_pin, GPIO_AF_OUT_OD);
    gpio_init_af(i2c_config[dev].sda_pin, GPIO_AF_OUT_OD);
}

static void _init_clk(I2C_Type *i2c, uint32_t speed)
{
    /* disable device and set ACK bit */
    i2c->CTL0 = I2C_CTL0_ACKEN_Msk;
    /* configure I2C clock */
    i2c->CTL1 = (CLOCK_APB1 / MHZ(1)) | I2C_CTL1_ERRIE_Msk;
    i2c->CKCFG = CLOCK_APB1 / (2 * speed);
    i2c->RT = (CLOCK_APB1 / 1000000) + 1;
    /* Clear flags */
    i2c->STAT0 &= ~I2C_ERROR_FLAGS;
    /* enable device */
    i2c->CTL0 |= I2C_CTL0_I2CEN_Msk;
}

static void _init(i2c_t dev)
{
    I2C_Type *i2c = i2c_config[dev].dev;

    /* make peripheral soft reset */
    i2c->CTL0 |= I2C_CTL0_SRESET_Msk;
    i2c->CTL0 &= ~I2C_CTL0_SRESET_Msk;

    /* configure I2C clock */
    _init_clk(i2c, i2c_config[dev].speed);
}

static void _deinit_pins(i2c_t dev)
{
    /* GD32V doesn't support GPIO_OD_PU mode, i.e. external pull-ups required */
    gpio_init(i2c_config[dev].scl_pin, GPIO_OD);
    gpio_init(i2c_config[dev].sda_pin, GPIO_OD);
    gpio_set(i2c_config[dev].scl_pin);
    gpio_set(i2c_config[dev].sda_pin);
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* lock the device */
    mutex_lock(&_i2c_dev[dev].dev_lock);

    /* block DEEP_SLEEP mode */
    pm_block(GD32V_PM_DEEPSLEEP);

    periph_clk_en(APB1, i2c_config[dev].rcu_mask);

    /* set the alternate function of the pins */
    _init_pins(dev);

    /* enable device */
    i2c_config[dev].dev->CTL0 |= I2C_CTL0_I2CEN_Msk;
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    /* disable device */
    i2c_config[dev].dev->CTL0 &= ~I2C_CTL0_I2CEN_Msk;

    _wait_for_bus(dev);

    /* Disabling the clock switches off the I2C controller, which results in
     * LOW bus lines. To avoid that the used GPIOs then draw some milliamps
     * of current via the pull-up resistors, the used GPIOs are set back to
     * GPIO_OD mode and HIGH. */
    _deinit_pins(dev);

    periph_clk_dis(APB1, i2c_config[dev].rcu_mask);

    /* unblock DEEP_SLEEP mode */
    pm_unblock(GD32V_PM_DEEPSLEEP);

    /* unlock the device */
    mutex_unlock(&_i2c_dev[dev].dev_lock);
}

static int _i2c_start_cmd(i2c_t dev)
{
    DEBUG("START cmd, dev=%d\n", dev);

    I2C_Type *i2c = i2c_config[dev].dev;

    /* clear error flags */
    i2c->STAT0 &= ~I2C_ERROR_FLAGS;

    /* send start condition and wait for interrupt */
    i2c->CTL0 |= I2C_CTL0_START_Msk;
    _wait_for_irq(dev);

    switch (_i2c_dev[dev].state) {
    case I2C_START_SENT:
        return 0;
    case I2C_ARB_LOST:
        return -EAGAIN;
    case I2C_TIMEOUT:
        return -ETIMEDOUT;
    default:
        /* on other errors */
        return -EINVAL;
    }
}

static int _i2c_stop_cmd(i2c_t dev)
{
    DEBUG("STOP cmd, dev=%d\n", dev);

    I2C_Type *i2c = i2c_config[dev].dev;

    /* clear error flags */
    i2c->STAT0 &= ~I2C_ERROR_FLAGS;

    /* send start condition */
    i2c->CTL0 |= I2C_CTL0_STOP_Msk;

    return 0;
}

static int _i2c_stop_cmd_and_wait(i2c_t dev)
{
    _i2c_stop_cmd(dev);
    return _wait_for_bus(dev);
}

static int _i2c_addr_cmd(i2c_t dev, uint8_t *addr, uint8_t size)
{
    if (size == 1) {
        DEBUG("ADDR cmd, dev=%d, addr=%02x\n", dev, addr[0]);
    }
    else{
        DEBUG("ADDR cmd, dev=%d, addr=%02x%02x\n", dev, addr[0], addr[1]);
    }

    I2C_Type *i2c = i2c_config[dev].dev;

    /* clear error flags */
    i2c->STAT0 &= ~I2C_ERROR_FLAGS;

    /* read STAT0 followed by writing the first address byte to the
     * DATA register to clear SBSEND and then wait for interrupt */
    i2c->STAT0;
    i2c->DATA = addr[0];
    _wait_for_irq(dev);

    if ((_i2c_dev[dev].state == I2C_ADDR10_SENT)) {
        /* first byte is sent and indicates a 10 bit address */
        assert(size == 2);
        /* read STAT0 followed by writing the second byte of the 10-bit
         * address to the DATA register to clear ADD10SEND and then wait for
         * interrupt */
        i2c->STAT0;
        i2c->DATA = addr[1];
        _wait_for_irq(dev);
    }

    switch (_i2c_dev[dev].state) {
    case I2C_ADDR_SENT:
        /* Since the ADDSEND flag is needed to control the ACK bit while
         * receiving bytes, it is intentionally not cleared here, but must
         * be explicitly cleared in `i2c_read_bytes` and `i2c_read_bytes */
        return 0;
    case I2C_ARB_LOST:
        return -EAGAIN;
    case I2C_TIMEOUT:
        return -ETIMEDOUT;
    case I2C_ACK_ERR:
        return -ENXIO;
    default:
        /* on other errors */
        return -EINVAL;
    }
}

int _i2c_write_cmd(i2c_t dev, uint8_t data)
{
    DEBUG("WRITE cmd, dev=%d\n", dev);

    I2C_Type *i2c = i2c_config[dev].dev;

    /* clear error flags */
    i2c->STAT0 &= ~I2C_ERROR_FLAGS;

    /* send data byte and wait for BTC interrupt */
    i2c->DATA = data;
    _wait_for_irq(dev);

    switch (_i2c_dev[dev].state) {
    case I2C_BT_COMPLETE:
        /* read STAT0 followed by reading DATA to clear the BTC flag */
        i2c->STAT0;
        i2c->DATA;
       return 0;
    case I2C_ARB_LOST:
        return -EAGAIN;
    case I2C_TIMEOUT:
        return -ETIMEDOUT;
    case I2C_ACK_ERR:
        return -EIO;
    default:
        /* on other errors */
        return -EINVAL;
    }
}

int _i2c_read_cmd(i2c_t dev, uint8_t *data)
{
    DEBUG("READ cmd, dev=%d\n", dev);

    I2C_Type *i2c = i2c_config[dev].dev;

    /* clear error flags */
    i2c->STAT0 &= ~I2C_ERROR_FLAGS;

    if (i2c->STAT0 & (I2C_STAT0_RBNE_Msk || I2C_STAT0_BTC_Msk)) {
        _i2c_dev[dev].state = I2C_RXB_NOT_EMPTY;
    }
    else {
        /* buffer interrupts have to be enabled for read */
        i2c_config[dev].dev->CTL1 |= I2C_CTL1_BUFIE_Msk;

        /* wait for interrupt */
        _wait_for_irq(dev);
    }

    switch (_i2c_dev[dev].state) {
    case I2C_RXB_NOT_EMPTY_BT_COMPLETE:
    case I2C_RXB_NOT_EMPTY:
        /* RBNE is cleared by reading STAT0 followed by reading the data register */
        i2c->STAT0;
        *data = i2c->DATA;
        return 0;
    case I2C_ARB_LOST:
        return -EAGAIN;
    case I2C_TIMEOUT:
        return -ETIMEDOUT;
    default:
        /* on other errors */
        return -EINVAL;
    }
}

static int _i2c_wait_rbne_btc(i2c_t dev)
{
    DEBUG("WAIT RNBE+BTC cmd, dev=%d\n", dev);

    I2C_Type *i2c = i2c_config[dev].dev;

    /* clear error flags */
    i2c->STAT0 &= ~I2C_ERROR_FLAGS;

    _wait_for_irq(dev);

    switch (_i2c_dev[dev].state) {
    case I2C_RXB_NOT_EMPTY_BT_COMPLETE:
        return 0;
    case I2C_ARB_LOST:
        return -EAGAIN;
    case I2C_TIMEOUT:
        return -ETIMEDOUT;
    default:
        /* on other errors */
        return -EINVAL;
    }
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t length,
                   uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    assert(data);
    assert(length);

    DEBUG("i2c_read_bytes, dev=%d len=%d flags=%02x\n", dev, length, flags);

    I2C_Type *i2c = i2c_config[dev].dev;

    /* Repeated START of read operations is not supported. This is exactly the
     * case if the previous transfer was a read operation (I2C_STAT1_TR == 0)
     * and was not terminated by a STOP condition (I2C_STAT1_I2CBSY == 1) and
     * the START condition is to be used (I2C_NOSTART == 0).
     */
    if (((i2c->STAT1 & (I2C_STAT1_I2CBSY_Msk | I2C_STAT1_TR_Msk)) == I2C_STAT1_I2CBSY_Msk) &&
        !(flags & I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }

    int res;

    _i2c_dev[dev].state = I2C_OK;

    /* set the ACK bit */
    i2c->CTL0 |= I2C_CTL0_ACKEN_Msk;
    /* clear the POAP bit to indicate that ACK bit controls the current byte */
    i2c->CTL0 &= ~I2C_CTL0_POAP_Msk;

    if (!(flags & I2C_NOSTOP)) {
        /* Since the I2C controller allows to receive up to two bytes before
         * the application has to react, receiving a single byte, two bytes
         * or more than two bytes needs a different handling for correct
         * reception if the reception is not to be continued (I2C_NOSTOP
         * is not set) */
        if (length == 1) {
            /* If a single byte is to be received clear the ACK bit before
             * any byte is received and keep the POAP bit cleared to indicate
             * that the ACK bit controls the ACK of the currecnt byte. */
            i2c->CTL0 &= ~I2C_CTL0_ACKEN_Msk;
        }
        else if (length == 2) {
            /* If exactly 2 bytes are to be received, keep the ACK bit set but
             * also set the POAP bit to indicate that the ACK bit controls the
             * next byte that is received in shift register. The ACK bit is then
             * cleared after the ADDSEND flag is cleared and thus the reception
             * of first byte has already been started. Thus an ACK is generated
             * for the first byte and a NACK for the second byte. */
            i2c->CTL0 |= I2C_CTL0_POAP_Msk;
        }
        /* In all other cases the ACK flag is kept set while the POAP flag
         * is kept cleared, so that the ACK bit always controls the byte
         * currently to be received in the shift register. To clear the ACK bit
         * before the last byte is received in the shift register, reading of
         * bytes from the DATA register is stopped when there are 3 bytes left
         * to receive until the BTC flag is set. The BTC flag then indicates
         * that the DATA register then contains the third last byte and the
         * shift register the second last byte. The ACK flag is then cleared
         * before reading of the bytes from the DATA register continues. */
    }

    /*  if I2C_NOSTART is not set, send START condition and ADDR */
    if (!(flags & I2C_NOSTART)) {
        res = _i2c_start_cmd(dev);
        if (res != 0) {
            _i2c_stop_cmd_and_wait(dev);
            return res;
        }

        /* address handling */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr10[2];
            addr10[0] = 0xf0 | (addr & 0x0300) >> 7 | I2C_READ;
            addr10[1] = addr & 0xff;
            /* send ADDR without read flag */
            res = _i2c_addr_cmd(dev, addr10, 2);
        }
        else {
            /* send ADDR without read flag */
            uint8_t addr7 = addr << 1 | I2C_READ;
            res = _i2c_addr_cmd(dev, &addr7, 1);
        }
        if (res != 0) {
            _i2c_stop_cmd_and_wait(dev);
            return res;
        }
    }

    /* read STAT0 followed by reading STAT1 to clear the ADDSEND flag */
    i2c->STAT0;
    i2c->STAT1;

    /* read data */
    uint8_t *buffer = (uint8_t*)data;

    for (size_t i = 0; i < length; i++) {
        if (!(flags & I2C_NOSTOP)) {
            /* if the reception is not to be continued (I2C_NOSTOP is not set) */
            if (i == 0 && (length == 2)) {
                /* The shift register already receives the first byte after
                 * the ADDSEND flag has been cleared. Since the POAP bit is
                 * set in the case that exactly two bytes are to be received,
                 * the change of the ACK bit during the reception of a byte
                 * does not affect the generated ACK of the byte currently
                 * received, but of the byte to be received next. Since the
                 * ACK bit and the POAP bit were set in this case, an ACK
                 * is generated for the first byte. Clearing the ACK bit here
                 * then generates a NACK for the next (last) byte. */
                i2c->CTL0 &= ~I2C_CTL0_ACKEN_Msk;
            }
            else if (i == (length - 3)) {
                /* To clear the ACK flag before the last byte is received
                 * in the shift register, reading of bytes from the DATA
                 * register is stopped when there are 3 bytes left to
                 * receive until the BTC flag is set. The BTC flag is then
                 * set when the third last received byte is in DATA register
                 * and the second last byte is in shift register. */
                res = _i2c_wait_rbne_btc(dev);
                if (res != 0) {
                    _i2c_stop_cmd_and_wait(dev);
                    return res;
                }
                /* The ACK flag is then cleared before reading of the bytes
                 * from the DATA register continues to generate an NACK
                 * for the last byte. */
                i2c->CTL0 &= ~I2C_CTL0_ACKEN_Msk;
            }
        }
        res = _i2c_read_cmd(dev, buffer + i);
        if (res != 0) {
            _i2c_stop_cmd_and_wait(dev);
            return res;
        }
    }

    if (!(flags & I2C_NOSTOP)) {
        /* If the transfer is not to be continued (I2C_NOSTOP is not set), set
         * the STOP bit and wait a short time until the I2CBSY bit is cleared. */
        return _i2c_stop_cmd_and_wait(dev);
    }

    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t length, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    assert(data);
    assert(length);

    DEBUG("i2c_write_bytes, dev=%d len=%d flags=%02x\n", dev, length, flags);

    int res;

    _i2c_dev[dev].state = I2C_OK;

    /*  if I2C_NOSTART is not set, send START condition and ADDR */
    if (!(flags & I2C_NOSTART)) {
        res = _i2c_start_cmd(dev);
        if (res != 0) {
            _i2c_stop_cmd_and_wait(dev);
            return res;
        }

        /* address handling */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr10[2];
            addr10[0] = 0xf0 | (addr & 0x0300) >> 7;
            addr10[1] = addr & 0xff;
            /* send ADDR without read flag */
            res = _i2c_addr_cmd(dev, addr10, 2);
        }
        else {
            /* send ADDR without read flag */
            uint8_t addr7 = addr << 1;
            res = _i2c_addr_cmd(dev, &addr7, 1);
        }
        if (res != 0) {
            _i2c_stop_cmd_and_wait(dev);
            return res;
        }
    }

    /* read STAT0 followed by reading STAT1 to clear the ADDSEND flag */
    i2c_config[dev].dev->STAT0;
    i2c_config[dev].dev->STAT1;

    /* send data */
    uint8_t *buffer = (uint8_t*)data;

    for (size_t i = 0; i < length; i++) {
        res = _i2c_write_cmd(dev, buffer[i]);
        if (res != 0) {
            _i2c_stop_cmd_and_wait(dev);
            return res;
        }
    }

    if (!(flags & I2C_NOSTOP)) {
        /* If the transfer is not to be continued (I2C_NOSTOP is not set), set
         * the STOP bit and wait a short time until the I2CBSY bit is cleared. */
        return _i2c_stop_cmd_and_wait(dev);
    }

    return 0;
}

void _i2c_transfer_timeout(void *arg)
{
    i2c_t dev = (i2c_t)(uintptr_t)arg;

    /* set result to timeout */
    _i2c_dev[dev].state = I2C_TIMEOUT;

    /* wake up the thread that is waiting for the results */
    mutex_unlock(&_i2c_dev[dev].irq_lock);
}

static void _wait_for_irq(i2c_t dev)
{
#if defined(MODULE_ZTIMER_USEC)
    ztimer_t timer = { .callback = _i2c_transfer_timeout,
                       .arg = (void *)dev };
    uint32_t timeout = ((I2C_TIMEOUT_CYCLES * MHZ(1)) / i2c_config[dev].speed) + 1;
    ztimer_set(ZTIMER_USEC, &timer, timeout);
#elif defined(MODULE_ZTIMER_MSEC)
    ztimer_t timer = { .callback = _i2c_transfer_timeout,
                       .arg = (void *)dev };
    uint32_t timeout = ((I2C_TIMEOUT_CYCLES * KHZ(1)) / i2c_config[dev].speed) + 1;
    ztimer_set(ZTIMER_MSEC, &timer, timeout);
#else
#warning "I2C timeout handling requires to use ztimer_msec or ztimer_usec"
#endif

    /* enable only event and error interrupts, buffer interrupts are only used in read */
    i2c_config[dev].dev->CTL1 |= I2C_INT_EV_ERR_FLAGS;

    /* wait for buffer, event or error interrupt */
    mutex_lock(&_i2c_dev[dev].irq_lock);

#if defined(MODULE_ZTIMER_USEC)
    ztimer_remove(ZTIMER_USEC, &timer);
#elif defined(MODULE_ZTIMER_MSEC)
    ztimer_remove(ZTIMER_MSEC, &timer);
#endif

    if (_i2c_dev[dev].state == I2C_TIMEOUT) {
        DEBUG("error: timeout, dev=%d\n", dev);
    }
}

#define TICK_TIMEOUT    0x00000fffUL

static inline int _wait_for_bus(i2c_t dev)
{
    uint16_t tick = TICK_TIMEOUT;
    /* short busy waiting for the bus becoming free (I2CBSY is cleared) */
    while ((i2c_config[dev].dev->STAT1 & I2C_STAT1_I2CBSY_Msk) && tick--) {}
    if (!tick) {
        DEBUG("error: timeout, dev=%d\n", dev);
        return -ETIMEDOUT;
    }
    return 0;
}

static void _irq_handler(unsigned irqn)
{
    static_assert(I2C0_ER_IRQn == I2C0_EV_IRQn + 1);
    static_assert(I2C1_ER_IRQn == I2C1_EV_IRQn + 1);

    i2c_t dev = ((irqn == i2c_config[0].irqn) ||
                 (irqn == (i2c_config[0].irqn + 1))) ? I2C_DEV(0) : I2C_DEV(1);

    assert(dev < I2C_NUMOF);

    I2C_Type *i2c = i2c_config[dev].dev;

    assert(i2c != NULL);

    unsigned state = i2c->STAT0;

    /* disable buffer, event and error interrupts */
    i2c->CTL1 &= ~(I2C_INT_ALL_FLAGS);

    DEBUG("STAT0: %08x, dev=%d\n", state, dev);

    if (state & I2C_ERROR_FLAGS) {
        /* any error happened */
        if (state & I2C_ERROR_FLAGS_USED) {
            if (state & I2C_STAT0_LOSTARB_Msk) {
                DEBUG("error: arbitration lost, dev=%d\n", dev);
                _i2c_dev[dev].state = I2C_ARB_LOST;
            }
            else if (state & I2C_STAT0_AERR_Msk) {
                DEBUG("error: ACK error, dev=%d\n", dev);
                _i2c_dev[dev].state = I2C_ACK_ERR;
            }
            else if (state & I2C_STAT0_BERR_Msk) {
                DEBUG("error: bus error, dev=%d\n", dev);
                _i2c_dev[dev].state = I2C_BUS_ERROR;
            }
        }
        if (state & I2C_ERROR_FLAGS_OTHER) {
            /* PEC calculation and SMBus are not used,
             * according errors are simply handled as other errors */
            DEBUG("error: other error, dev=%d\n", dev);
            _i2c_dev[dev].state = I2C_OTHER_ERROR;
        }
        /* clear interrupt flags for errors */
        i2c->CTL1 &= ~(I2C_ERROR_FLAGS);
    }
    else if (state & I2C_STAT0_SBSEND_Msk) {
        DEBUG("START sent, dev=%d\n", dev);
        _i2c_dev[dev].state = I2C_START_SENT;
    }
    else if (state & I2C_STAT0_ADDSEND_Msk) {
        DEBUG("ADDR sent, dev=%d\n", dev);
        _i2c_dev[dev].state = I2C_ADDR_SENT;
    }
    else if (state & I2C_STAT0_ADD10SEND_Msk) {
        DEBUG("ADDR10 first byte sent, dev=%d\n", dev);
        _i2c_dev[dev].state = I2C_ADDR10_SENT;
    }
    else if (state & I2C_STAT0_RBNE_Msk) {
        if (state & I2C_STAT0_BTC_Msk) {
            DEBUG("RX buffer not empty + BT completed, dev=%d\n", dev);
            _i2c_dev[dev].state = I2C_RXB_NOT_EMPTY_BT_COMPLETE;
        }
        else {
            DEBUG("RX buffer not empty, dev=%d\n", dev);
            _i2c_dev[dev].state = I2C_RXB_NOT_EMPTY;
        }
    }
    else if (state & I2C_STAT0_BTC_Msk) {
        DEBUG("BT completed, dev=%d\n", dev);
        _i2c_dev[dev].state = I2C_BT_COMPLETE;
    }
    else if (state & I2C_STAT0_TBE_Msk) {
        DEBUG("TX buffer empty, dev=%d\n", dev);
        _i2c_dev[dev].state = I2C_TXB_EMPTY;
    }
    else {
        _i2c_dev[dev].state = I2C_OK;
    }

    mutex_unlock(&_i2c_dev[dev].irq_lock);
}
