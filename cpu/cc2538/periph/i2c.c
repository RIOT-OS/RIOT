/*
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "mutex.h"
#include "periph/i2c.h"
#include "sched.h"
#include "thread.h"
#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif
#include "timex.h" /* for SEC_IN_USEC */

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard this file in case no I2C device is defined */
#if I2C_NUMOF

#ifndef I2C_0_SCL_PIN
#define I2C_0_SCL_PIN i2c_config[I2C_0].scl_pin
#endif

#ifndef I2C_0_SDA_PIN
#define I2C_0_SDA_PIN i2c_config[I2C_0].sda_pin
#endif

#undef BIT
#define BIT(n)     ( 1 << (n) )

/* Standard I2C Parameters */
#define DATA_BITS  8
#define ACK_BITS   1

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

#define bus_quiet()            ( cc2538_gpio_read(I2C_0_SCL_PIN) && cc2538_gpio_read(I2C_0_SDA_PIN) )
#define WARN_IF(cond) \
        if (cond) { \
            DEBUG("%s at %s:%u\n", #cond, RIOT_FILE_NOPATH, __LINE__); \
        }

void cc2538_i2c_init_master(uint32_t speed_hz);

static void i2cm_ctrl_write(uint_fast8_t value) {
    WARN_IF(I2CM_STAT & BUSY);
    I2CM_CTRL = value;
}

static void assert_scl(void) {
    cc2538_gpio_clear(I2C_0_SCL_PIN);
    IOC_PXX_OVER[I2C_0_SCL_PIN] |= IOC_OVERRIDE_OE;
    gpio_dir_output(I2C_0_SCL_PIN);
    gpio_software_control(I2C_0_SCL_PIN);
}

static void release_scl(void) {
    IOC_PXX_OVER[I2C_0_SCL_PIN] &= ~(IOC_OVERRIDE_OE | IOC_OVERRIDE_PDE);
    gpio_dir_input(I2C_0_SCL_PIN);
    gpio_software_control(I2C_0_SCL_PIN);
}

static void release_sda(void) {
    IOC_PXX_OVER[I2C_0_SDA_PIN] &= ~(IOC_OVERRIDE_OE | IOC_OVERRIDE_PDE);
    gpio_dir_input(I2C_0_SDA_PIN);
    gpio_software_control(I2C_0_SDA_PIN);
}

static void recover_i2c_bus(void) {
    /* Switch to software GPIO mode for bus recovery */
    release_sda();
    release_scl();

    if (!bus_quiet()) {
        const uint_fast8_t try_limit = 200;
        uint_fast8_t n;
        for (n = 0; n < try_limit; n++) {
            if (bus_quiet()) {
                DEBUG("%s(): SDA released after%4u SCL pulses.\n", __FUNCTION__, n);
                break;
            }

            assert_scl();

#ifdef MODULE_XTIMER
            xtimer_usleep(scl_delay);
#else
            thread_yield();
#endif

            release_scl();

#ifdef MODULE_XTIMER
            xtimer_usleep(scl_delay);
#else
            thread_yield();
#endif
        }

        if (n >= try_limit) {
            DEBUG("%s(): Failed to release SDA after%4u SCL pulses.\n", __FUNCTION__, n);
        }
    }

    /* Return to hardware mode for the I2C pins */
    gpio_hardware_control(I2C_0_SCL_PIN);
    gpio_hardware_control(I2C_0_SDA_PIN);
}

#ifdef MODULE_XTIMER
static void callback(void *arg)
{
    mutex_unlock(&i2c_wait_mutex);
}
#endif

static uint_fast8_t i2c_ctrl_blocking(uint_fast8_t flags)
{
#ifdef MODULE_XTIMER
    const unsigned int xtimer_timeout = 3 * (DATA_BITS + ACK_BITS) * SEC_IN_USEC / speed_hz;
#endif

    mutex_trylock(&i2c_wait_mutex);

    assert(I2CM_IMR & 1);
    i2cm_ctrl_write(flags);

#ifdef MODULE_XTIMER
    /* Set a timeout at double the expected time to transmit a byte: */
    xtimer_t xtimer = {.callback = callback};
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
        cc2538_i2c_init_master(speed_hz);
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

        if (sched_context_switch_request) {
            thread_yield();
        }
}

void cc2538_i2c_init_master(uint32_t speed_hz)
{
    SYS_CTRL_RCGCI2C |= 1; /**< Enable the I2C0 clock. */
    SYS_CTRL_SCGCI2C |= 1; /**< Enable the I2C0 clock. */
    SYS_CTRL_DCGCI2C |= 1; /**< Enable the I2C0 clock. */

    /* Reset I2C peripheral */
    SYS_CTRL_SRI2C |= 1;

#ifdef MODULE_XTIMER
    xtimer_usleep(50);
#else
    thread_yield();
#endif

    SYS_CTRL_SRI2C &= ~1;

    /* Clear all pin override flags except PUE (Pull-Up Enable) */
    IOC_PXX_OVER[I2C_0_SCL_PIN] &= IOC_OVERRIDE_PUE;
    IOC_PXX_OVER[I2C_0_SDA_PIN] &= IOC_OVERRIDE_PUE;

    IOC_PXX_SEL[I2C_0_SCL_PIN] = I2C_CMSSCL;
    IOC_PXX_SEL[I2C_0_SDA_PIN] = I2C_CMSSDA;

    IOC_I2CMSSCL = I2C_0_SCL_PIN;
    IOC_I2CMSSDA = I2C_0_SDA_PIN;

    gpio_hardware_control(I2C_0_SCL_PIN);
    gpio_hardware_control(I2C_0_SDA_PIN);

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
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            break;
#endif
        default:
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

    cc2538_i2c_init_master(speed_hz);

    /* Pre-compute an SCL delay in microseconds */
    scl_delay = SEC_IN_USEC;
    scl_delay += speed_hz;
    scl_delay /= 2 * speed_hz;

    return 0;
}

int i2c_init_slave(i2c_t dev, uint8_t address)
{
    /* Slave mode is not (yet) supported. */
    return -1;
}

int i2c_acquire(i2c_t dev)
{
    if (dev == I2C_0) {
        mutex_lock(&mutex);
        return 0;
    }
    else {
        return -1;
    }
}

int i2c_release(i2c_t dev)
{
    if (dev == I2C_0) {
        mutex_unlock(&mutex);
        return 0;
    }
    else {
        return -1;
    }
}

static bool i2c_busy(void) {
    if (I2CM_STAT & BUSY) {
        cc2538_i2c_init_master(speed_hz);
        return (I2CM_STAT & BUSY) != 0;
    }

    return false;
}

int i2c_read_byte(i2c_t dev, uint8_t address, char *data)
{
    return i2c_read_bytes(dev, address, data, 1);
}

static int i2c_read_bytes_dumb(uint8_t address, char *data, int length)
{
    int n = 0;
    uint_fast8_t stat;

    switch (length) {
        case 0:
            break;

        case 1:
            if (i2c_busy()) {
                break;
            }

            I2CM_SA = (address << 1) | RS;
            stat = i2c_ctrl_blocking(STOP | START | RUN);
            if (stat & ANY_ERROR) {
                break;
            }
            data[n] = I2CM_DR;
            n++;
            break;

        default:
            if (i2c_busy()) {
                break;
            }

            I2CM_SA = (address << 1) | RS;
            stat = i2c_ctrl_blocking(ACK | START | RUN);

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
                stat = i2c_ctrl_blocking((n < length - 1) ? (ACK | RUN) : (STOP | RUN));

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

int i2c_read_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    switch (dev) {
#if I2C_0_EN
        case I2C_0:
            break;
#endif

        default:
            return -1;
    }

    WARN_IF(I2CM_STAT & BUSY);

    if ( (length <= 0) || i2c_busy() ) {
        return 0;
    }

    WARN_IF(I2CM_STAT & BUSBSY);

    if (I2CM_STAT & BUSBSY) {
        recover_i2c_bus();

        if (I2CM_STAT & BUSBSY) {
            return 0;
        }
    }

    return i2c_read_bytes_dumb(address, data, length);
}

int i2c_read_reg(i2c_t dev, uint8_t address, uint8_t reg, char *data)
{
    return i2c_read_regs(dev, address, reg, data, 1);
}

int i2c_read_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
    uint_fast8_t stat;

    if (dev != I2C_0) {
        return -1;
    }

    /* Transmit reg byte to slave */
    if (i2c_busy()) {
        return 0;
    }

    WARN_IF(I2CM_STAT & BUSBSY);

    if (I2CM_STAT & BUSBSY) {
        recover_i2c_bus();

        if (I2CM_STAT & BUSBSY) {
            return 0;
        }
    }

    I2CM_SA = address << 1;
    I2CM_DR = reg;
    stat = i2c_ctrl_blocking(START | RUN);

    if (stat & ARBLST) {
        return 0;
    }
    else if (stat & ANY_ERROR) {
        i2cm_ctrl_write(STOP);
        return 0;
    }
    else {
        /* Receive data from slave */
        return i2c_read_bytes_dumb(address, data, length);
    }
}

int i2c_write_byte(i2c_t dev, uint8_t address, char data)
{
    return i2c_write_bytes(dev, address, &data, 1);
}

int i2c_write_bytes(i2c_t dev, uint8_t address, char *data, int length)
{
    int n = 0;

    if (dev != I2C_0) {
        return -1;
    }

    WARN_IF(I2CM_STAT & BUSBSY);

    if (I2CM_STAT & BUSBSY) {
        recover_i2c_bus();

        if (I2CM_STAT & BUSBSY) {
            return 0;
        }
    }

    I2CM_SA = address << 1;
    uint_fast8_t flags = START | RUN;

    for (n = 0; n < length; n++) {
        if (n >= length - 1) flags |= STOP;
        WARN_IF(I2CM_STAT & BUSY);
        I2CM_DR = data[n];
        i2c_ctrl_blocking(flags);

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

        if (n < length) {
            DEBUG("%s(%u, %p, %u): %u/%u bytes delivered.\n",
                  __FUNCTION__, address, (void *)data, length, n, length);
        }

    return n;
}

int i2c_write_reg(i2c_t dev, uint8_t address, uint8_t reg, char data)
{
    return i2c_write_regs(dev, address, reg, &data, 1);
}

int i2c_write_regs(i2c_t dev, uint8_t address, uint8_t reg, char *data, int length)
{
    uint_fast8_t stat;

    if (dev != I2C_0) {
        return -1;
    }

    /* Transmit reg byte to slave */
    if (i2c_busy()) {
        return 0;
    }

    WARN_IF(I2CM_STAT & BUSBSY);

    if (I2CM_STAT & BUSBSY) {
        recover_i2c_bus();

        if (I2CM_STAT & BUSBSY) {
            return 0;
        }
    }

    I2CM_SA = address << 1;
    I2CM_DR = reg;

    uint_fast8_t flags = (length > 0)? (START | RUN) : (STOP | START | RUN);
    stat = i2c_ctrl_blocking(flags);

    if (stat & ARBLST) {
        return 0;
    }
    else if (stat & ANY_ERROR) {
        i2cm_ctrl_write(STOP);
        return 0;
    }
    else {
        /* Transmit data to slave */
        int n = 0;

        flags &= ~START;

        for (n = 0; n < length; n++) {
            if (n >= length - 1) flags |= STOP;
            WARN_IF(I2CM_STAT & BUSY);
            I2CM_DR = data[n];

            i2c_ctrl_blocking(flags);

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

        if (n < length) {
            DEBUG(
                "%s(%u, %u, %u, %p, %u): %u/%u bytes delivered.\n",
                __FUNCTION__,
                dev,
                address,
                reg,
                (void *)data,
                length,
                n,
                length
            );
        }

        return n;
    }
}

void i2c_poweron(i2c_t dev)
{
    if (dev == I2C_0) {
        SYS_CTRL_RCGCI2C |= 1; /**< Enable the I2C0 clock. */
        SYS_CTRL_SCGCI2C |= 1; /**< Enable the I2C0 clock. */
        SYS_CTRL_DCGCI2C |= 1; /**< Enable the I2C0 clock. */

        I2CM_CR |= MFE;        /**< I2C master function enable. */

        /* Enable I2C master interrupts */
        I2CM_IMR = 1;
    }
}

void i2c_poweroff(i2c_t dev)
{
    if (dev == I2C_0) {
        /* Disable I2C master interrupts */
        I2CM_IMR = 0;
        NVIC_DisableIRQ(I2C_IRQn);

        I2CM_CR &= ~MFE;        /**< I2C master function enable. */

        SYS_CTRL_RCGCI2C &= ~1; /**< Disable the I2C0 clock. */
        SYS_CTRL_SCGCI2C &= ~1; /**< Disable the I2C0 clock. */
        SYS_CTRL_DCGCI2C &= ~1; /**< Disable the I2C0 clock. */
    }
}

#endif /* I2C_NUMOF */
