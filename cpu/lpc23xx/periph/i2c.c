/*
 * SPDX-FileCopyrightText: 2020 Beuth Hochschule für Technik Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lpc23xx
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation for lpc23xx
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <errno.h>

#include "cpu.h"
#include "board.h"
#include "byteorder.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#include "sched.h"
#include "thread.h"
#include "mutex.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if I2C_NUMOF > 0
static void I2C0_IRQHandler(void) __attribute__((interrupt("IRQ")));
#endif
#if I2C_NUMOF > 1
static void I2C1_IRQHandler(void) __attribute__((interrupt("IRQ")));
#endif
#if I2C_NUMOF > 2
static void I2C2_IRQHandler(void) __attribute__((interrupt("IRQ")));
#endif

/**
 * We only ever need two buffers for read/write reg
 */
#define TRX_BUFS_MAX    (2)
static struct i2c_ctx {
    mutex_t lock;
    mutex_t tx_done;
    uint8_t *buf[TRX_BUFS_MAX];
    uint8_t *buf_end[TRX_BUFS_MAX];
    uint8_t *cur;
    uint8_t *end;
    int res;
    uint8_t addr[TRX_BUFS_MAX];
    uint8_t buf_num;
    uint8_t buf_cur;
} ctx[I2C_NUMOF] = {
#if I2C_NUMOF > 0
    { .lock = MUTEX_INIT, .tx_done = MUTEX_INIT_LOCKED },
#endif
#if I2C_NUMOF > 1
    { .lock = MUTEX_INIT, .tx_done = MUTEX_INIT_LOCKED },
#endif
#if I2C_NUMOF > 2
    { .lock = MUTEX_INIT, .tx_done = MUTEX_INIT_LOCKED },
#endif
};

static void poweron(lpc23xx_i2c_t *i2c)
{
    switch ((uint32_t)i2c) {
    case I2C0_BASE_ADDR:
        PCONP |= PCI2C0;
        break;
    case I2C1_BASE_ADDR:
        PCONP |= PCI2C1;
        break;
    case I2C2_BASE_ADDR:
        PCONP |= PCI2C2;
        break;
    }
}

static void poweroff(lpc23xx_i2c_t *i2c)
{
    switch ((uint32_t)i2c) {
    case I2C0_BASE_ADDR:
        PCONP &= ~PCI2C0;
        break;
    case I2C1_BASE_ADDR:
        PCONP &= ~PCI2C1;
        break;
    case I2C2_BASE_ADDR:
        PCONP &= ~PCI2C2;
        break;
    }
}

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&ctx[dev].lock);
    poweron(i2c_config[dev].dev);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    poweroff(i2c_config[dev].dev);
    mutex_unlock(&ctx[dev].lock);
}

static void _set_baud(lpc23xx_i2c_t *i2c, uint32_t baud)
{
    uint32_t pclksel, prescale;
    lpc23xx_pclk_scale(CLOCK_CORECLOCK, baud, &pclksel, &prescale);

    switch ((uint32_t)i2c) {
    case I2C0_BASE_ADDR:
        PCLKSEL0 &= ~(BIT14 | BIT15);
        PCLKSEL0 |= pclksel << 14;
        I20SCLL = prescale / 2;
        I20SCLH = prescale / 2;
        break;
    case I2C1_BASE_ADDR:
        PCLKSEL1 &= ~(BIT6 | BIT7);
        PCLKSEL1 |= pclksel << 6;
        I21SCLL = prescale / 2;
        I21SCLH = prescale / 2;
        break;
    case I2C2_BASE_ADDR:
        PCLKSEL1 &= ~(BIT20 | BIT21);
        PCLKSEL1 |= pclksel << 20;
        I22SCLL = prescale / 2;
        I22SCLH = prescale / 2;
        break;
    }
}

static unsigned _get_irq(i2c_t dev)
{
    switch ((uint32_t)i2c_config[dev].dev) {
    case I2C0_BASE_ADDR:
        return I2C0_INT;
    case I2C1_BASE_ADDR:
        return I2C1_INT;
    case I2C2_BASE_ADDR:
        return I2C2_INT;
    }

    return 0;
}

static void _install_irq(i2c_t dev)
{
    switch (dev) {
#if I2C_NUMOF > 0
    case 0:
        install_irq(_get_irq(dev), I2C0_IRQHandler, i2c_config[dev].irq_prio);
        break;
#endif
#if I2C_NUMOF > 1
    case 1:
        install_irq(_get_irq(dev), I2C1_IRQHandler, i2c_config[dev].irq_prio);
        break;
#endif
#if I2C_NUMOF > 2
    case 2:
        install_irq(_get_irq(dev), I2C2_IRQHandler, i2c_config[dev].irq_prio);
        break;
#endif
    }
}

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    const i2c_conf_t *cfg = &i2c_config[dev];
    lpc23xx_i2c_t *i2c = cfg->dev;

    poweron(i2c);

    /* configure SDA & SCL pins */
    *(&PINSEL0 + cfg->pinsel_sda) |= cfg->pinsel_msk_sda;
    *(&PINSEL0 + cfg->pinsel_scl) |= cfg->pinsel_msk_scl;

    /* clear control register */
    i2c->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC
                | I2CONCLR_I2ENC;

    _set_baud(i2c, cfg->speed);

    _install_irq(dev);

    /* enable the interface */
    i2c->CONSET = I2CONSET_I2EN;

    poweroff(i2c);
}

static void _end_tx(i2c_t dev, unsigned res)
{
    ctx[dev].res = res;
    mutex_unlock(&ctx[dev].tx_done);
}

static void _next_buffer(i2c_t dev)
{
    /* We only need two buffers max.
       This can only be called for the first buffer
       as there is no next buffer for the second buffer */
    assert(ctx[dev].buf_cur == 0);

    lpc23xx_i2c_t *i2c = i2c_config[dev].dev;

    /* if mode (read/write) changed, send START again */
    if (ctx[dev].addr[0] != ctx[dev].addr[1]) {
        i2c->CONSET = I2CONSET_STA;
    }

    ctx[dev].cur = ctx[dev].buf[1];
    ctx[dev].end = ctx[dev].buf_end[1];
    ctx[dev].buf_cur = 1;
}

static void irq_handler(i2c_t dev)
{
    lpc23xx_i2c_t *i2c = i2c_config[dev].dev;

    unsigned stat = i2c->STAT;

    DEBUG("[i2c] STAT: %x\n", stat);

    switch (stat) {
    case 0x00: /* Bus Error */
        DEBUG("[i2c] Bus Error\n");
        _end_tx(dev, -EIO);
        break;

    case 0x08: /* A Start Condition is issued. */
    case 0x10: /* A repeated Start Condition is issued */
        ctx[dev].cur = ctx[dev].buf[ctx[dev].buf_cur];
        i2c->DAT  = ctx[dev].addr[ctx[dev].buf_cur];
        i2c->CONSET = I2CONSET_AA;
        i2c->CONCLR = I2CONCLR_STAC;
        break;

    case 0x20:  /* Address NACK (write) */
    case 0x48:  /* Address NACK (read)  */
        /* slave did not ACK address - send STOP */
        i2c->CONSET = I2CONSET_STO | I2CONSET_AA;
        _end_tx(dev, -ENXIO);
        break;

    case 0x18: /* Master Transmit, SLA_R has been sent */
        i2c->DAT = *ctx[dev].cur++;
        i2c->CONSET = I2CONSET_AA;
        break;

    case 0x28: /* Data byte has been transmitted */

        if (ctx[dev].cur == ctx[dev].end) {
            /* we transmitted all buffers */
            if (ctx[dev].buf_cur == ctx[dev].buf_num) {
                i2c->CONSET = I2CONSET_STO | I2CONSET_AA;
                _end_tx(dev, 0);
                break;
            } else {
                _next_buffer(dev);
            }
        }

        i2c->DAT = *ctx[dev].cur++;
        i2c->CONSET = I2CONSET_AA;
        break;

    case 0x30: /* Data NACK */
        i2c->CONSET = I2CONSET_STO | I2CONSET_AA;
        _end_tx(dev, 0);
        break;

    case 0x38: /* Arbitration has been lost */
        i2c->CONSET = I2CONSET_STA | I2CONSET_AA;
        break;

    case 0x40: /* Master Receive, SLA_R has been sent */

        /* if we only want to read one byte, send NACK already */
        if (ctx[dev].cur + 1 == ctx[dev].end) {
            i2c->CONCLR = I2CONCLR_AAC;
        } else {
            i2c->CONSET = I2CONSET_AA;
        }
        break;

    case 0x50: /* Data byte has been received */

        *ctx[dev].cur++ = i2c->DAT;

        if (ctx[dev].cur == ctx[dev].end) {
            i2c->CONCLR = I2CONCLR_AAC;
        } else {
            i2c->CONSET = I2CONSET_AA;
        }

        break;

    case 0x58: /* Data received, NACK */
        *ctx[dev].cur = i2c->DAT;

        if (ctx[dev].buf_cur != ctx[dev].buf_num) {
            i2c->CONSET = I2CONSET_AA;
            _next_buffer(dev);
        } else {
            i2c->CONSET = I2CONSET_AA | I2CONSET_STO;
            _end_tx(dev, 0);
        }
        break;

    }

    /* clear interrupt flag */
    /* cppcheck-suppress redundantAssignment
     * (reason: writing 1 to volatile register to clear the interrupt) */
    i2c->CONCLR = I2CONCLR_SIC;
}

static void _init_buffer(i2c_t dev, uint8_t idx, uint8_t addr,
                         uint8_t *data, size_t len)
{
    ctx[dev].addr[idx]    = addr;
    ctx[dev].buf[idx]     = data;
    ctx[dev].buf_end[idx] = data + len;

    ctx[dev].buf_num = idx;

    ctx[dev].buf_cur = 0;
    ctx[dev].cur     = ctx[dev].buf[0];
    ctx[dev].end     = ctx[dev].buf_end[0];
}

int i2c_read_bytes(i2c_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    lpc23xx_i2c_t *i2c = i2c_config[dev].dev;

    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    /* TODO: 10 bit addresses */
    if (flags) {
        return -ENOTSUP;
    }

    _init_buffer(dev, 0, 1 | (addr << 1), (void*)data, len);

    /* set Start flag */
    i2c->CONSET = I2CONSET_STA;

    mutex_lock(&ctx[dev].tx_done);
    return ctx[dev].res;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    lpc23xx_i2c_t *i2c = i2c_config[dev].dev;

    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    /* TODO: 10 bit addresses */
    if (flags) {
        return -ENOTSUP;
    }

    _init_buffer(dev, 0, addr << 1, (void*)data, len);

    /* set Start flag */
    i2c->CONSET = I2CONSET_STA;

    mutex_lock(&ctx[dev].tx_done);
    return ctx[dev].res;
}

int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    lpc23xx_i2c_t *i2c = i2c_config[dev].dev;

    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    /* TODO: 10 bit addresses */
    if (flags & ~I2C_REG16) {
        return -ENOTSUP;
    }

    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg = htons(reg); /* Make sure register is in big-endian on I2C bus */
    }

    _init_buffer(dev, 0, addr << 1, (void*)&reg, (flags & I2C_REG16) ? 2 : 1);
    _init_buffer(dev, 1, 1 | (addr << 1), (void*)data, len);

    /* set Start flag */
    i2c->CONSET = I2CONSET_STA;

    mutex_lock(&ctx[dev].tx_done);
    return ctx[dev].res;
}

int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                   const void *data, size_t len, uint8_t flags)
{
    assert(dev < I2C_NUMOF);
    lpc23xx_i2c_t *i2c = i2c_config[dev].dev;

    /* Check for wrong arguments given */
    if (data == NULL || len == 0) {
        return -EINVAL;
    }

    /* TODO: 10 bit addresses */
    if (flags & ~I2C_REG16) {
        return -ENOTSUP;
    }

    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg = htons(reg); /* Make sure register is in big-endian on I2C bus */
    }

    _init_buffer(dev, 0, addr << 1, (void*)&reg, (flags & I2C_REG16) ? 2 : 1);
    _init_buffer(dev, 1, addr << 1, (void*)data, len);

    /* set Start flag */
    i2c->CONSET = I2CONSET_STA;

    mutex_lock(&ctx[dev].tx_done);
    return ctx[dev].res;
}

#if I2C_NUMOF > 0
static void I2C0_IRQHandler(void)
{
    irq_handler(0);
    VICVectAddr = 0;                    /* Acknowledge Interrupt */
}
#endif
#if I2C_NUMOF > 1
static void I2C1_IRQHandler(void)
{
    irq_handler(1);
    VICVectAddr = 0;                    /* Acknowledge Interrupt */
}
#endif
#if I2C_NUMOF > 2
static void I2C2_IRQHandler(void)
{
    irq_handler(2);
    VICVectAddr = 0;                    /* Acknowledge Interrupt */
}
#endif
