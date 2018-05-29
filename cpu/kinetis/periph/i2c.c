/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015-2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_i2c
 *
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include <errno.h>

#include "cpu.h"
#include "irq.h"
#include "bit.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG    (0)
/* Define ENABLE_TRACE to 1 to enable printing of all TX/RX bytes to UART for extra verbose debugging */
#define ENABLE_TRACE    (1)
/* Define ENABLE_INIT_DEBUG to 1 to enable DEBUG prints in i2c_init. Currently
 * this causes the system to hang when running i2c_init during boot because of
 * uninitialized stdio UART */
#define ENABLE_INIT_DEBUG (0)
#include "debug.h"

#if ENABLE_TRACE
#define TRACE(...) DEBUG(__VA_ARGS__)
#else
#define TRACE(...)
#endif

/**
 * @brief   Array of I2C module clock dividers
 *
 * The index in this array is the I2C_F setting number
 */
/*
 * Divider values were copied from K22F reference manual
 */
static const uint16_t i2c_dividers[] = {
      20,   22,   24,   26,   28,   30,   34,   40, /* 0x00 ~ 0x07 */
      28,   32,   36,   40,   44,   48,   56,   68, /* 0x08 ~ 0x0f */
      48,   56,   64,   72,   80,   88,  104,  128, /* 0x10 ~ 0x17 */
      80,   96,  112,  128,  144,  160,  192,  240, /* 0x18 ~ 0x1f */
     160,  192,  224,  256,  288,  320,  384,  480, /* 0x20 ~ 0x27 */
     320,  384,  448,  512,  576,  640,  768,  960, /* 0x28 ~ 0x2f */
     640,  768,  896, 1024, 1152, 1280, 1536, 1920, /* 0x30 ~ 0x37 */
    1280, 1536, 1792, 2048, 2304, 2560, 3072, 3840, /* 0x38 ~ 0x3f */
};

/**
 * @brief   Driver internal state
 */
typedef struct {
    mutex_t mtx; /**< Mutex preventing multiple users of the same bus */
    unsigned active; /**< State variable to help catch user mistakes */
} i2c_state_t;
static i2c_state_t i2c_state[I2C_NUMOF];


int i2c_acquire(i2c_t dev)
{
    assert((unsigned)dev < I2C_NUMOF);
    mutex_lock(&i2c_state[dev].mtx);
    return 0;
}

int i2c_release(i2c_t dev)
{
    /* Check that the bus was properly stopped before releasing */
    /* It is a programming error to release the bus after sending a start
     * condition but before sending a stop condition */
    assert(i2c_state[dev].active == 0);

    mutex_unlock(&i2c_state[dev].mtx);
    return 0;
}

static uint8_t i2c_find_divider(unsigned freq, unsigned speed)
{
    unsigned diff = UINT_MAX;
    /* Use maximum divider if nothing matches */
    uint8_t F = sizeof(i2c_dividers) / sizeof(i2c_dividers[0]) - 1;
    /* We avoid using the MULT field to simplify the driver and avoid having to
     * work around hardware errata on some Kinetis parts
     *
     * See:
     * https://community.nxp.com/thread/377611
     * https://mcuoneclipse.com/2012/12/05/kl25z-and-i2c-missing-repeated-start-condition/
     *
     * e6070:   I2C: Repeat start cannot be generated if the I2Cx_F[MULT] field is set to a non-zero value
     *
     * Description:
     * If the I2Cx_F[MULT] field is written with a non-zero value, then a repeat
     * start cannot be generated
     *
     * Workaround:
     * There are two possible workarounds:
     * 1) Configure I2Cx_F[MULT] to zero if a repeat start has to be generated.
     * 2) Temporarily set I2Cx_F [MULT] to zero immediately before setting the
     *    Repeat START bit in the I2C C1 register (I2Cx_C1[RSTA]=1) and restore
     *    the I2Cx_F [MULT] field to the original value after the repeated start
     *    has occurred
     */
    for (unsigned k = 0; k < sizeof(i2c_dividers) / sizeof(i2c_dividers[0]); ++k) {
        /* Test dividers until we find one that gives a good match */
        unsigned lim = (speed * i2c_dividers[k]);
        if (lim >= freq) {
            if ((lim - freq) < diff) {
                diff = (lim - freq);
                F = k;
            }
        }
    }
    if (ENABLE_INIT_DEBUG) {
        DEBUG("i2c_divider: speed = %u, freq = %u, diff = %u, F = 0x%02x\n", speed, freq, diff, F);
    }
    return F;
}

static inline void i2c_clear_irq_flags(I2C_Type *i2c)
{
    i2c->S = i2c->S;
}

void i2c_init(i2c_t dev)
{
    if (ENABLE_INIT_DEBUG) {
        DEBUG("i2c_init: %u\n", (unsigned) dev);
    }
    assert((unsigned)dev < I2C_NUMOF);
    const i2c_conf_t *conf = &i2c_config[dev];
    I2C_Type *i2c = conf->i2c;
    /* Turn on the module clock gate */
    switch ((uintptr_t)i2c) {
#ifdef I2C0
        case (uintptr_t)I2C0:
            I2C0_CLKEN();
            break;
#endif
#ifdef I2C1
        case (uintptr_t)I2C1:
            I2C1_CLKEN();
            break;
#endif
#ifdef I2C2
        case (uintptr_t)I2C2:
            I2C2_CLKEN();
            break;
#endif
        default:
            if (ENABLE_INIT_DEBUG) {
                DEBUG("i2c_init: Unknown I2C device %p\n", (void *)i2c);
            }
            return;
    }
    i2c_state[dev].mtx = (mutex_t)MUTEX_INIT_LOCKED;
    i2c_state[dev].active = 0;
    if (ENABLE_INIT_DEBUG) {
        DEBUG("i2c_init: init SCL pin\n");
    }
    gpio_init_port(conf->scl_pin, conf->scl_pcr);
    if (ENABLE_INIT_DEBUG) {
        DEBUG("i2c_init: init SDA pin\n");
    }
    gpio_init_port(conf->sda_pin, conf->sda_pcr);
    /* Configure master settings */
    i2c->C1 = 0; /* Disable module while messing with the settings */
    /* Configure glitch filter register */
#ifdef I2C_FLT_SHEN_MASK
    i2c->FLT = I2C_FLT_SHEN_MASK;
#else
    i2c->FLT = 0;
#endif
    i2c->F = i2c_find_divider(conf->freq, (unsigned)conf->speed);
    /* Enable module */
    i2c->C1 = I2C_C1_IICEN_MASK;
    i2c_clear_irq_flags(i2c);
    NVIC_EnableIRQ(conf->irqn);
    mutex_unlock(&i2c_state[dev].mtx);
}

/* Internal helper for checking status flags after transmission */
static int i2c_tx(i2c_t dev, uint8_t byte)
{
    I2C_Type *i2c = i2c_config[dev].i2c;
    TRACE("i2c: tx: %02x\n", (unsigned)byte);
    i2c->D = byte;
    uint8_t S;
    uint16_t timeout = UINT16_MAX;
    do {
        S = i2c->S;
    } while (!(S & I2C_S_IICIF_MASK) && --timeout);

    i2c_clear_irq_flags(i2c);

    if (S & I2C_S_ARBL_MASK) {
        DEBUG("i2c: arbitration lost\n");
        bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
        i2c_state[dev].active = 0;
        return -EAGAIN;
    }
    if (timeout == 0) {
        /* slave stretches the clock for too long */
        DEBUG("i2c: tx timeout\n");
        return -ETIMEDOUT;
    }
    if (S & I2C_S_RXAK_MASK) {
        DEBUG("i2c: NACK\n");
        return -EIO;
    }
    return 0;
}

static int i2c_stop(i2c_t dev)
{
    I2C_Type *i2c = i2c_config[dev].i2c;
    /* Send stop condition */
    TRACE("i2c: stop C1=%02x S=%02x\n", (unsigned)i2c->C1, (unsigned)i2c->S);
    bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
    unsigned timeout = 0x10000;
    while ((i2c->S & I2C_S_BUSY_MASK) && --timeout) {}
    i2c_state[dev].active = 0;
    if (timeout == 0) {
        /* slave stretches the clock for too long */
        DEBUG("i2c: timeout while stopping\n");
        return -ETIMEDOUT;
    }
    TRACE("i2c: STOP C1=%02x S=%02x\n", (unsigned)i2c->C1, (unsigned)i2c->S);
    return 0;
}

static int i2c_tx_addr(i2c_t dev, uint8_t byte)
{
    int res = i2c_tx(dev, byte);
    if (res < 0) {
        if (res == -EIO) {
            /* No ACK received, remap EIO to ENXIO for missing addr ack */
            res = -ENXIO;
        }
        int stopres = i2c_stop(dev);
        if (stopres < 0) {
            return stopres;
        }
    }
    return res;
}

static int i2c_start(i2c_t dev, uint16_t addr, unsigned read_flag, uint8_t flags)
{
    I2C_Type *i2c = i2c_config[dev].i2c;
    TRACE("i2c: start C1=%02x S=%02x\n", (unsigned)i2c->C1, (unsigned)i2c->S);
    /* Send start condition */
    if (i2c->C1 & I2C_C1_MST_MASK) {
        /* Repeat start */
        i2c->C1 |= I2C_C1_IICEN_MASK | I2C_C1_RSTA_MASK | I2C_C1_TX_MASK;
    }
    else {
        /* Initial start */
        if (read_flag && (flags & I2C_ADDR10)) {
            /* 10 bit addressing does not allow reading without a repeated start */
            return -EINVAL;
        }
        /* slave -> master transition triggers the initial start condition */
        i2c->C1 |= I2C_C1_IICEN_MASK | I2C_C1_MST_MASK | I2C_C1_TX_MASK;
    }
    i2c_state[dev].active = 1;
    TRACE("i2c: Start C1=%02x S=%02x\n", (unsigned)i2c->C1, (unsigned)i2c->S);
    if (i2c->S & I2C_S_ARBL_MASK) {
        DEBUG("i2c: arbitration lost\n");
        i2c_clear_irq_flags(i2c);
        bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
        i2c_state[dev].active = 0;
        return -EAGAIN;
    }
    if (flags & I2C_ADDR10) {
        /* 10 bit addressing */
        /* Send 10 bit address tag + 2 msb + R/W flag */
        uint8_t msb = (addr >> 8);
        uint8_t lsb = (addr & 0xff);
        uint8_t addr_rw = I2C_10BIT_MAGIC | (msb << 1);
        if (read_flag) {
            addr_rw |= I2C_READ;
        }
        int res = i2c_tx_addr(dev, addr_rw);
        if (res < 0) {
            return res;
        }
        if (!read_flag) {
            /* Send lower 8 bits of address */
            res = i2c_tx_addr(dev, lsb);
        }
        return res;
    }
    /* 7 bit addressing */
    /* Send address + R/W flag */
    uint8_t addr_rw = (addr << 1);
    if (read_flag) {
        addr_rw |= I2C_READ;
    }
    return i2c_tx_addr(dev, addr_rw);
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len, uint8_t flags)
{
    assert((data != NULL) || (len == 0));
    I2C_Type *i2c = i2c_config[dev].i2c;
    DEBUG("i2c: r a:%02x, l:%u, f:%02x\n", addr, (unsigned)len, (unsigned)flags);
    if (len == 1) {
        /* Send NACK after next byte */
        bit_set8(&i2c->C1, I2C_C1_TXAK_SHIFT);
    }
    else {
        bit_clear8(&i2c->C1, I2C_C1_TXAK_SHIFT);
    }

    if (!(flags & I2C_NOSTART)) {
        /* Send start condition and address */
        int res = i2c_start(dev, addr, 1, flags);
        if (res < 0) {
            return res;
        }
    }
    if (len > 0) {
        bit_clear8(&i2c->C1, I2C_C1_TX_SHIFT);
        /* Initiate master receive mode by reading the data register once when the
         * C1[TX] bit is cleared and C1[MST] is set */
        volatile uint8_t dummy;
        dummy = i2c->D;
        ++dummy;
    }
    TRACE("i2c: read C1=%02x S=%02x\n", (unsigned)i2c->C1, (unsigned)i2c->S);
    uint8_t *datap = data;
    while (len > 0) {
        uint8_t S;
        do {
            S = i2c->S;
        } while (!(S & I2C_S_IICIF_MASK));

        i2c_clear_irq_flags(i2c);

        if (S & I2C_S_ARBL_MASK) {
            DEBUG("i2c: rx arbitration lost\n");
            bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
            i2c_state[dev].active = 0;
            return -EAGAIN;
        }
        --len;
        if (len == 1) {
            /* Send NACK after next byte */
            bit_set8(&i2c->C1, I2C_C1_TXAK_SHIFT);
        }
        else if (len == 0) {
            /* Switching the module to TX mode lets us read the data register
             * without triggering a new reception */
            bit_set8(&i2c->C1, I2C_C1_TX_SHIFT);
        }

        *datap = i2c->D;
        TRACE("i2c: rx: %02x\n", (unsigned)*datap);
        ++datap;
    }
    if (!(flags & I2C_NOSTOP)) {
        int res = i2c_stop(dev);
        if (res < 0) {
            return res;
        }
    }
    return 0;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len, uint8_t flags)
{
    assert((data != NULL) || (len == 0));
    I2C_Type *i2c = i2c_config[dev].i2c;
    DEBUG("i2c: w a:%02x, l:%u, f:%02x\n", addr, (unsigned)len, (unsigned)flags);
    if (!(flags & I2C_NOSTART)) {
        int res = i2c_start(dev, addr, 0, flags);
        if (res < 0) {
            return res;
        }
    }
    const uint8_t *datap = data;
    bit_set8(&i2c->C1, I2C_C1_TX_SHIFT);
    TRACE("i2c: write C1=%02x S=%02x\n", (unsigned)i2c->C1, (unsigned)i2c->S);
    for (size_t k = 0; k < len; ++k) {
        int res = i2c_tx(dev, datap[k]);
        if ((res == -EIO) && (k == (len - 1))) {
            /* NACK on the final byte is normal */
            break;
        }
        else if (res < 0) {
            int stopres = i2c_stop(dev);
            if (stopres < 0) {
                return stopres;
            }
            return res;
        }
    }
    if (!(flags & I2C_NOSTOP)) {
        int res = i2c_stop(dev);
        if (res < 0) {
            return res;
        }
    }
    return 0;
}
