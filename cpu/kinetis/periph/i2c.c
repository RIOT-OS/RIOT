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
#include "thread.h"
#include "thread_flags.h"
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
 * @brief   Thread flag used internally for signaling between ISR and user thread
 */
#define THREAD_FLAG_KINETIS_I2C (1u << 8)

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
 * @brief   I2C IRQ reception state
 */
typedef struct {
    uint8_t *datap; /**< pointer to output buffer */
    size_t bytes_left; /**< how many bytes left to receive */
} i2c_rx_t;

/**
 * @brief   I2C IRQ transmission state
 */
typedef struct {
    const uint8_t *datap; /**< pointer to input buffer */
    size_t bytes_left; /**< how many bytes left to transmit */
} i2c_tx_t;
/**
 * @brief   Driver internal state
 */
typedef struct {
    mutex_t mtx; /**< Mutex preventing multiple users of the same bus */
    kernel_pid_t pid; /**< PID of thread waiting for a transfer to complete */
    uint8_t active; /**< State variable to help catch user mistakes */
    union {
        i2c_tx_t tx; /**< State of ongoing transmission sequence */
        i2c_rx_t rx; /**< State of ongoing reception sequence */
    };
    int retval; /**< return value from ISR */
} i2c_state_t;


static i2c_state_t i2c_state[I2C_NUMOF];

int i2c_acquire(i2c_t dev)
{
    assert((unsigned)dev < I2C_NUMOF);
    mutex_lock(&i2c_state[dev].mtx);
    i2c_state[dev].pid = thread_getpid();
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
    /* cppcheck-suppress selfAssignment
     * reason: intentional self assignment to clear all pending IRQs */
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
    i2c_state[dev].pid = KERNEL_PID_UNDEF;
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
    I2C_Type *i2c = i2c_config[dev].i2c;
    TRACE("i2c: txa: %02x\n", (unsigned)byte);
    i2c->D = byte;
    uint8_t S;
    uint16_t timeout = UINT16_MAX;
    do {
        S = i2c->S;
    } while (!(S & I2C_S_IICIF_MASK) && --timeout);

    i2c_clear_irq_flags(i2c);

    int res = 0;
    if (S & I2C_S_ARBL_MASK) {
        DEBUG("i2c: arbitration lost\n");
        res = -EAGAIN;
    }
    else if (timeout == 0) {
        /* slave stretches the clock for too long */
        DEBUG("i2c: tx timeout\n");
        res = -ETIMEDOUT;
    }
    else if (S & I2C_S_RXAK_MASK) {
        DEBUG("i2c: NACK\n");
        res = -ENXIO;
    }
    if (res < 0) {
        bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
        i2c_state[dev].active = 0;
        i2c_clear_irq_flags(i2c);
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
        uint16_t timeout = UINT16_MAX;
        while ((i2c->S & I2C_S_BUSY_MASK) && --timeout) {}
        if (timeout == 0) {
            /* Someone else is using the bus for a long time, try again later */
            DEBUG("i2c: start timeout\n");
            return -EAGAIN;
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
        /* We need IRQs enabled for interrupt based transfers */
        assert(!__get_PRIMASK());
        assert(!irq_is_in());
        bit_clear8(&i2c->C1, I2C_C1_TX_SHIFT);
        /* Configure IRQ transfer */
        i2c_state[dev].rx.datap = data;
        i2c_state[dev].rx.bytes_left = len;
        i2c_state[dev].retval = 0;
        bit_set8(&i2c->C1, I2C_C1_IICIE_SHIFT);
        /* Initiate master receive mode by reading the data register once when
         * the C1[TX] bit is cleared and C1[MST] is set */
        volatile uint8_t dummy;
        dummy = i2c->D;
        ++dummy;
        /* Wait until the ISR signals back */
        TRACE("i2c: read C1=%02x S=%02x\n", (unsigned)i2c->C1, (unsigned)i2c->S);
        thread_flags_t tflg = thread_flags_wait_any(THREAD_FLAG_KINETIS_I2C | THREAD_FLAG_TIMEOUT);
        TRACE("i2c: rx done, %u left, ret: %d\n",
            i2c_state[dev].rx.bytes_left, i2c_state[dev].retval);
        if (!(tflg & THREAD_FLAG_KINETIS_I2C)) {
            bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
            i2c_state[dev].active = 0;
            return -ETIMEDOUT;
        }
        if (i2c_state[dev].retval < 0) {
            /* An error occurred */
            /* The module has been stopped from the ISR, no need to clear the MST bit */
            return i2c_state[dev].retval;
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
    if (len > 0) {
        /* We need IRQs enabled for interrupt based transfers */
        assert(!__get_PRIMASK());
        assert(!irq_is_in());
        /* Configure IRQ transfer */
        bit_set8(&i2c->C1, I2C_C1_TX_SHIFT);
        i2c_state[dev].tx.datap = data;
        i2c_state[dev].tx.bytes_left = len;
        i2c_state[dev].retval = 0;
        bit_set8(&i2c->C1, I2C_C1_IICIE_SHIFT);
        TRACE("i2c: write C1=%02x S=%02x\n", (unsigned)i2c->C1, (unsigned)i2c->S);
        /* Initiate transfer by writing the first byte, the remaining bytes will
         * be fed by the ISR */
        i2c->D = *((const uint8_t *)data);
        /* Wait until the ISR signals back */
        thread_flags_t tflg = thread_flags_wait_any(THREAD_FLAG_KINETIS_I2C | THREAD_FLAG_TIMEOUT);
        TRACE("i2c: rx done, %u left, ret: %d\n",
            i2c_state[dev].rx.bytes_left, i2c_state[dev].retval);
        if (!(tflg & THREAD_FLAG_KINETIS_I2C)) {
            bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
            i2c_state[dev].active = 0;
            return -ETIMEDOUT;
        }
        if (i2c_state[dev].retval < 0) {
            /* An error occurred */
            /* The module has been stopped from the ISR, no need to clear the MST bit */
            return i2c_state[dev].retval;
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

static inline void i2c_irq_signal_done(I2C_Type *i2c, kernel_pid_t pid)
{
    thread_flags_set((thread_t *)thread_get(pid), THREAD_FLAG_KINETIS_I2C);
    bit_clear8(&i2c->C1, I2C_C1_IICIE_SHIFT);
}

/**
 * @brief   Master transmit mode IRQ handler
 */
static void i2c_irq_mst_tx_handler(I2C_Type *i2c, i2c_state_t *state, uint8_t S)
{
    size_t len = state->tx.bytes_left;
    assert(len != 0); /* This only happens if this periph driver is broken */
    --len;
    state->tx.bytes_left = len;
    if (len == 0) {
        /* We are done, NACK on the last byte is OK */
        DEBUG("i2c: TX done\n");
        i2c_irq_signal_done(i2c, state->pid);
    }
    else {
        if (S & I2C_S_RXAK_MASK) {
            /* NACK */
            /* Abort master transfer */
            DEBUG("i2c: NACK\n");
            bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
            state->active = 0;
            state->retval = -EIO;
            i2c_irq_signal_done(i2c, state->pid);
        }
        /* transmit the next byte */
        /* Increment first, datap points to the last byte transmitted */
        ++state->tx.datap;
        i2c->D = *state->tx.datap;
        TRACE("i2c: tx: %02x\n", (unsigned)*(state->tx.datap));
    }
}

/**
 * @brief   Master receive mode IRQ handler
 */
static void i2c_irq_mst_rx_handler(I2C_Type *i2c, i2c_state_t *state)
{
    size_t len = state->rx.bytes_left;
    assert(len != 0); /* This only happens if this periph driver is broken */
    --len;
    if (len == 1) {
        /* Send NACK after the next byte */
        bit_set8(&i2c->C1, I2C_C1_TXAK_SHIFT);
    }
    else if (len == 0) {
        /* Switching the module to TX mode lets us read the data register
         * without triggering a new reception */
        bit_set8(&i2c->C1, I2C_C1_TX_SHIFT);
        i2c_irq_signal_done(i2c, state->pid);
    }
    state->rx.bytes_left = len;
    /* Read data reception buffer, this will trigger reception of the following
     * byte iff TX and MST bits in the C1 register are set */
    *state->rx.datap = i2c->D;
    TRACE("i2c: rx: %02x\n", (unsigned)*(state->rx.datap));
    ++state->rx.datap;
}

void i2c_irq_handler(i2c_t dev)
{
    I2C_Type *i2c = i2c_config[dev].i2c;
    i2c_state_t *state = &i2c_state[dev];
    uint8_t S = i2c->S;
    /* Clear IRQ flags */
    i2c->S = S;
    if (i2c->C1 & I2C_C1_MST_MASK) {
        /* Master mode handler */
        if (S & I2C_S_ARBL_MASK) {
            DEBUG("i2c: arbitration lost\n");
            /* Abort master transfer */
            bit_clear8(&i2c->C1, I2C_C1_MST_SHIFT);
            state->active = 0;
            state->retval = -EAGAIN;
            i2c_irq_signal_done(i2c, state->pid);
        }
        if (i2c->C1 & I2C_C1_TX_MASK) {
            /* Transmit mode */
            i2c_irq_mst_tx_handler(i2c, state, S);
        }
        else {
            /* Receive mode */
            i2c_irq_mst_rx_handler(i2c, state);
        }
    }
    else {
        /* TODO: Slave mode handler goes here */
    }
    cortexm_isr_end();
}

#ifdef I2C_0_ISR
void I2C_0_ISR(void)
{
    i2c_irq_handler(I2C_DEV(0));
}
#endif /* I2C_0_ISR */

#ifdef I2C_1_ISR
void I2C_1_ISR(void)
{
    i2c_irq_handler(I2C_DEV(1));
}
#endif /* I2C_1_ISR */
