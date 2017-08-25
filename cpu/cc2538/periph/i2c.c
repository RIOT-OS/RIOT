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

#include "mutex.h"
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#ifdef MODULE_XTIMER
#include "xtimer.h"
#else
#include "thread.h"
#include "timex.h" /* for US_PER_SEC */
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard this file in case no I2C device is defined */
#ifdef I2C_NUMOF

/* pin short cuts */
#define SCL_PIN(x) (i2c_config[x].scl_pin)
#define SDA_PIN(x) (i2c_config[x].sda_pin)

#define SDA_TRY_LIMIT   (200U)

#undef BIT
#define BIT(n)     ( 1 << (n) )

/* Standard I2C Parameters */
#define DATA_BITS  (8U)
#define ACK_BITS   (1U)

/* I2CM_DR Bits */
#define RS         BIT(0)

/* I2CM_CTRL Bits */
#define ACK        BIT(3)
#define STOP       BIT(2)
#define START      BIT(1)
#define RUN        BIT(0)

/* I2CM_STAT Bits */
#define BUSBSY     BIT(6)
#define IDLE       BIT(5)
#define ARBLST     BIT(4)
#define DATACK     BIT(3)
#define ADRACK     BIT(2)
#define ERROR      BIT(1)
#define BUSY       BIT(0)

/* I2CM_CR Bits */
#define SFE        BIT(5)
#define MFE        BIT(4)
#define LPBK       BIT(0)

#define ANY_ERROR  (ARBLST | DATACK | ADRACK | ERROR)

#define SCL_LP     6 /**< SCL Low Period (fixed at 6). */
#define SCL_HP     4 /**< SCL High Period (fixed at 4). */

static mutex_t mutex = MUTEX_INIT;
static mutex_t i2c_wait_mutex = MUTEX_INIT;
static uint32_t speed_hz;
static uint32_t scl_delay;

#define WARN_IF(cond) \
        if (cond) { \
            DEBUG("%s at %s:%u\n", #cond, RIOT_FILE_NOPATH, __LINE__); \
        }

static void cc2538_i2c_init_master(i2c_t dev, uint32_t speed_hz);


static inline void i2cm_ctrl_write(uint_fast8_t value) {
    I2CM_CTRL = value;
}

#ifdef MODULE_XTIMER
static void _timer_cb(void *arg)
{
    (void)arg;
    mutex_unlock(&i2c_wait_mutex);
}
#endif

static uint_fast8_t i2c_ctrl_blocking(i2c_t dev, uint_fast8_t flags)
{
#ifdef MODULE_XTIMER
    const unsigned int xtimer_timeout = 3 * (DATA_BITS + ACK_BITS) * US_PER_SEC / speed_hz;
#endif

    mutex_trylock(&i2c_wait_mutex);

    assert(I2CM_IMR & 1);
    i2cm_ctrl_write(flags);

#ifdef MODULE_XTIMER
    /* Set a timeout at double the expected time to transmit a byte: */
    xtimer_t xtimer = { .callback = _timer_cb, .arg = NULL };
    xtimer_set(&xtimer, xtimer_timeout);
#endif

    mutex_lock(&i2c_wait_mutex);

#ifdef MODULE_XTIMER
    xtimer_remove(&xtimer);
#endif

    if (I2CM_STAT & BUSY) {
        /* If the controller is still busy, it probably will be forever */
#ifdef MODULE_XTIMER
        DEBUG("Master is still BUSY after %u usec. Resetting.\n", xtimer_timeout);
#endif
        cc2538_i2c_init_master(dev, speed_hz);
    }

    WARN_IF(I2CM_STAT & BUSY);

    return I2CM_STAT;
}

void isr_i2c(void)
{
    /* Clear the interrupt flag */
    I2CM_ICR = 1;

    /* Unlock the wait mutex */
    mutex_unlock(&i2c_wait_mutex);

    cortexm_isr_end();
}

void cc2538_i2c_init_master(i2c_t dev, uint32_t speed_hz)
{
    SYS_CTRL_RCGCI2C |= 1; /**< Enable the I2C0 clock. */
    SYS_CTRL_SCGCI2C |= 1; /**< Enable the I2C0 clock. */
    SYS_CTRL_DCGCI2C |= 1; /**< Enable the I2C0 clock. */

    /* Reset I2C peripheral */
    SYS_CTRL_SRI2C |= 1;
    /* wait shortly for reset */
    for (unsigned delay = 0; delay < 32; ++delay) {};
    /* clear periph reset trigger */
    SYS_CTRL_SRI2C &= ~1;

    gpio_init_af(SCL_PIN(dev), I2C_CMSSCL, GPIO_IN_PU);
    gpio_init_af(SDA_PIN(dev), I2C_CMSSDA, GPIO_IN_PU);
    IOC_I2CMSSCL = gpio_pp_num(SCL_PIN(dev));
    IOC_I2CMSSDA = gpio_pp_num(SDA_PIN(dev));

    /* Initialize the I2C master by setting the Master Function Enable bit */
    I2CM_CR |= MFE;

    /* Set the SCL clock speed */
    uint32_t ps = sys_clock_freq();
    uint32_t denom = 2 * (SCL_LP + SCL_HP) * speed_hz;
    ps += denom / 2;
    ps /= denom;
    I2CM_TPR = ps - 1;

    /* Enable I2C master interrupts */
    NVIC_SetPriority(I2C_IRQn, I2C_IRQ_PRIO);
    NVIC_EnableIRQ(I2C_IRQn);

    i2cm_ctrl_write(STOP);

    /* Enable I2C master interrupts */
    I2CM_IMR = 1;
}

int i2c_init_master(i2c_t dev, i2c_speed_t speed)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }

    switch (speed) {
        case I2C_SPEED_LOW:
            speed_hz = 10000;
            break;

        case I2C_SPEED_NORMAL:
            speed_hz = 100000;
            break;

        case I2C_SPEED_FAST:
            speed_hz = 400000;
            break;

        case I2C_SPEED_FAST_PLUS:
            speed_hz = 1000000;
            break;

        case I2C_SPEED_HIGH:
            speed_hz = 3400000;
            break;

        default:
            return -2;
    }

    cc2538_i2c_init_master(dev, speed_hz);

    /* Pre-compute an SCL delay in microseconds */
    scl_delay = US_PER_SEC;
    scl_delay += speed_hz;
    scl_delay /= 2 * speed_hz;

    return 0;
}

int i2c_acquire(i2c_t dev)
{
    if (dev < I2C_NUMOF) {
        mutex_lock(&mutex);
        return 0;
    }
    return -1;
}

int i2c_release(i2c_t dev)
{
    if (dev < I2C_NUMOF) {
        mutex_unlock(&mutex);
        return 0;
    }
    return -1;
}

static bool i2c_busy(i2c_t dev) {
    if (I2CM_STAT & BUSY) {
        cc2538_i2c_init_master(dev, speed_hz);
        return (I2CM_STAT & BUSY) != 0;
    }

    return false;
}

int i2c_read_byte(i2c_t dev, uint8_t address, void *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

static int i2c_read_bytes_dumb(i2c_t dev, uint8_t address, uint8_t *data, int length)
{
    int n = 0;
    uint_fast8_t stat;

    switch (length) {
        case 0:
            break;

        case 1:
            if (i2c_busy(dev)) {
                break;
            }

            I2CM_SA = (address << 1) | RS;
            stat = i2c_ctrl_blocking(dev, (STOP | START | RUN));
            if (stat & ANY_ERROR) {
                break;
            }
            data[n] = I2CM_DR;
            n++;
            break;

        default:
            if (i2c_busy(dev)) {
                break;
            }

            I2CM_SA = (address << 1) | RS;
            stat = i2c_ctrl_blocking(dev, (ACK | START | RUN));

            if (stat & ARBLST) {
                 break;
            }
            else if (stat & ANY_ERROR) {
                i2cm_ctrl_write(STOP);
                break;
            }

            data[n] = I2CM_DR;
            n++;

            while (n < length) {
                stat = i2c_ctrl_blocking(dev, (n < length - 1) ? (ACK | RUN) : (STOP | RUN));

                if (stat & ARBLST) {
                     break;
                }
                else if (stat & ANY_ERROR) {
                    i2cm_ctrl_write(STOP);
                    break;
                }

                data[n] = I2CM_DR;
                n++;
            }

            break;
    }

    return n;
}

int i2c_read_bytes(i2c_t dev, uint8_t address, void *data, int length)
{
    if (dev >= I2C_NUMOF) {
        return -1;
    }
    if (length <= 0) {
        DEBUG("i2c_read_bytes: invalid length!\n");
        return 0;
    }
    if (i2c_busy(dev)) {
        DEBUG("i2c_read_bytes: device busy!\n");
        return 0;
    }
    if (I2CM_STAT & BUSBSY) {
        DEBUG("i2c_read_bytes: bus busy!\n");
        return 0;
    }

    return i2c_read_bytes_dumb(dev, address, data, length);
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, void *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, void *data, int length)
{
    uint_fast8_t stat;

    if (dev >= I2C_NUMOF) {
        return -1;
    }
    if (i2c_busy(dev)) {
        DEBUG("i2c_read_regs: device busy!\n");
        return 0;
    }
    if (I2CM_STAT & BUSBSY) {
        DEBUG("i2c_read_regs: bus busy!\n");
        return 0;
    }

    /* Transmit reg byte to slave */
    I2CM_SA = address << 1;
    I2CM_DR = reg;
    stat = i2c_ctrl_blocking(dev, (START | RUN));

    if (stat & ARBLST) {
        return 0;
    }
    else if (stat & ANY_ERROR) {
        i2cm_ctrl_write(STOP);
        return 0;
    }
    else {
        /* Receive data from slave */
        return i2c_read_bytes_dumb(dev, address, data, length);
    }
}

int i2c_write_byte(i2c_t dev, uint8_t address, uint8_t data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, const void *data, int length)
{
    const uint8_t *my_data = data;

    if (dev >= I2C_NUMOF) {
        return -1;
    }
    if (i2c_busy(dev)) {
        DEBUG("i2c_read_regs: device busy!\n");
        return 0;
    }
    if (I2CM_STAT & BUSBSY) {
        DEBUG("i2c_read_regs: bus busy!\n");
        return 0;
    }

    I2CM_SA = address << 1;

    uint_fast8_t flags = START | RUN;
    int n = 0;

    for (n = 0; n < length; n++) {
        if (n >= length - 1) {
            flags |= STOP;
        }
        WARN_IF(I2CM_STAT & BUSY);
        I2CM_DR = my_data[n];
        i2c_ctrl_blocking(dev, flags);

        WARN_IF(I2CM_STAT & ARBLST);
        WARN_IF(I2CM_STAT & DATACK);
        WARN_IF(I2CM_STAT & ADRACK);
        WARN_IF(I2CM_STAT & ERROR);

        if (I2CM_STAT & ARBLST) {
             break;
        }
        else if (I2CM_STAT & ANY_ERROR) {
            i2cm_ctrl_write(STOP);
            break;
        }

        flags = RUN;
    }

    DEBUG("%s(%u, %p, %u): %u/%u bytes delivered.\n", __FUNCTION__,
          address, (void *)my_data, length, n, length);

    return n;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, uint8_t data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, const void *data, int length)
{
    const uint8_t *my_data = data;

    if (dev >= I2C_NUMOF) {
        return -1;
    }
    if (i2c_busy(dev)) {
        DEBUG("i2c_read_regs: device busy!\n");
        return 0;
    }
    if (I2CM_STAT & BUSBSY) {
        DEBUG("i2c_read_regs: bus busy!\n");
        return 0;
    }

    /* Transmit reg byte to slave */
    I2CM_SA = address << 1;
    I2CM_DR = reg;

    uint_fast8_t flags = (length > 0) ? (START | RUN) : (STOP | START | RUN);
    uint_fast8_t stat = i2c_ctrl_blocking(dev, flags);

    if (stat & ARBLST) {
        return 0;
    }
    else if (stat & ANY_ERROR) {
        i2cm_ctrl_write(STOP);
        return 0;
    }

    /* Transmit data to slave */
    int n = 0;

    flags &= ~START;
    for (n = 0; n < length; n++) {
        if (n >= length - 1) {
            flags |= STOP;
        }

        WARN_IF(I2CM_STAT & BUSY);
        I2CM_DR = my_data[n];

        i2c_ctrl_blocking(dev, flags);

        WARN_IF(I2CM_STAT & ARBLST);
        WARN_IF(I2CM_STAT & DATACK);
        WARN_IF(I2CM_STAT & ADRACK);
        WARN_IF(I2CM_STAT & ERROR);

        if (I2CM_STAT & ARBLST) {
             break;
        }
        else if (I2CM_STAT & ANY_ERROR) {
            i2cm_ctrl_write(STOP);
            break;
        }
    }

    DEBUG("%s(%u, %u, %u, %p, %u): %u/%u bytes delivered.\n", __FUNCTION__,
          dev, address, reg, data, length, n, length);

    return n;
}

void i2c_poweron(i2c_t dev)
{
    if (dev < I2C_NUMOF) {
        SYS_CTRL_RCGCI2C |= 1; /**< Enable the I2C0 clock. */
        SYS_CTRL_SCGCI2C |= 1; /**< Enable the I2C0 clock. */
        SYS_CTRL_DCGCI2C |= 1; /**< Enable the I2C0 clock. */

        I2CM_CR |= MFE;        /**< I2C master function enable. */

        /* Enable I2C master interrupts */
        NVIC_SetPriority(I2C_IRQn, I2C_IRQ_PRIO);
        NVIC_EnableIRQ(I2C_IRQn);
        I2CM_IMR = 1;
    }
}

void i2c_poweroff(i2c_t dev)
{
    if (dev < I2C_NUMOF) {
        /* Disable I2C master interrupts */
        I2CM_IMR = 0;
        NVIC_DisableIRQ(I2C_IRQn);

        I2CM_CR &= ~MFE;        /**< I2C master function disable. */

        SYS_CTRL_RCGCI2C &= ~1; /**< Disable the I2C0 clock. */
        SYS_CTRL_SCGCI2C &= ~1; /**< Disable the I2C0 clock. */
        SYS_CTRL_DCGCI2C &= ~1; /**< Disable the I2C0 clock. */
    }
}

#endif /* I2C_NUMOF */
