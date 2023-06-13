/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @ingroup     drivers_periph_i2c
 *
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"
#include "mutex.h"

#include "periph_conf.h"
#include "periph/i2c.h"

#include "vendor/drivers/fsl_clock.h"
#include "vendor/drivers/fsl_iocon.h"

#include "gpio_mux.h"
#include "flexcomm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static mutex_t locks[I2C_NUMOF];

/**
 * @name Definitions for MSTSTATE bits in I2C Status register STAT
 * @{
 */
/* Controller Idle State Code */
#define I2C_STAT_MSTSTATE_IDLE      (0)
/* Controller Receive Ready State Code */
#define I2C_STAT_MSTSTATE_RXREADY   (1)
/* Controller Transmit Ready State Code */
#define I2C_STAT_MSTSTATE_TXREADY   (2)
/* Controller NACK by peripheral on address State Code */
#define I2C_STAT_MSTSTATE_NACKADR   (3)
/* Controller NACK by peripheral on data State Code */
#define I2C_STAT_MSTSTATE_NACKDAT   (4)
/** @} */

/**
 * @brief Limit value I2C CLKDIV register.
 */
#define I2C_CLKDIV_MAX 0xffffu

/**
 * @brief Set the I2C controller mode clock speed.
 */
static void _i2c_controller_set_speed(I2C_Type *i2c_dev, uint32_t speed_hz)
{
    /* The I2C clock source is based on the FLEXCOMM clock with the following
     * formula:
     *   i2c freq := flexcomm freq / ((CLKDIV + 1) * (MSTTIME + 2) * 2)
     * Where MSTTIME is a number between 0 and 7, and CLKDIV is between 0 and
     * 0xffff.
     * The MSTTIME register controls for how many cycles does the clock stay
     * low and high, allowing to use different values for each one but we only
     * use symmetric ones here, which is why there's a * 2 in the formula above.
     */
    assert(speed_hz > 0);
    uint32_t bus_freq = CLOCK_GetFreq(kCLOCK_BusClk);
    uint32_t target = bus_freq / (2 * speed_hz);

    uint32_t best_error = UINT_MAX;
    uint32_t best_clkdiv = 0;
    uint32_t best_msttime = 0;

    for (uint32_t msttime_p2 = 9; msttime_p2 >= 2; msttime_p2--) {
        uint32_t clkdiv_p1 = (target + msttime_p2 / 2) / msttime_p2;
        if (clkdiv_p1 >= I2C_CLKDIV_MAX + 1) {
            clkdiv_p1 = I2C_CLKDIV_MAX + 1;
        }
        uint32_t error =
            abs((int32_t)target - (int32_t)(clkdiv_p1 * msttime_p2));
        if (error < best_error) {
            best_error = error;
            best_clkdiv = clkdiv_p1 - 1;
            best_msttime = msttime_p2 - 2;
        }
    }
    i2c_dev->CLKDIV = I2C_CLKDIV_DIVVAL(best_clkdiv);
    i2c_dev->MSTTIME = I2C_MSTTIME_MSTSCLLOW(best_msttime) |
                       I2C_MSTTIME_MSTSCLHIGH(best_msttime);
    DEBUG("[i2c]: bus_fq=%" PRIu32" target_freq=%" PRIu32" msttime=%" PRIu32
          " clkdiv=%" PRIu32 " error=%" PRIu32 "\n",
          bus_freq, speed_hz, best_msttime, best_clkdiv, best_error);
}

static void _i2c_init_pins(i2c_t dev)
{
    const i2c_conf_t *conf = &i2c_config[dev];

    /* Configure SDA and SCL pins, the function value depends on the pin:
     * FUNC4: A6, A7, A8, A9, A12, A13, A22, A23, A26, A27
     * FUNC5: A2, A3, A4, A5, A20, A21
     */
    static const uint32_t func5_mask =
        (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 20) | (1 << 21);

    uint32_t sda_mode = IOCON_FUNC4;
    if ((1u << GPIO_T_PIN(conf->pin_sda)) & func5_mask) {
        sda_mode = IOCON_FUNC5;
    }

    uint32_t scl_mode = IOCON_FUNC4;
    if ((1u << GPIO_T_PIN(conf->pin_scl)) & func5_mask) {
        sda_mode = IOCON_FUNC5;
    }

    /* Using the internal pull up will make sure the I2C bus is also working
     * when no external pull up is present. It can waste power by when both
     * external and internal pull up are present. If a real world example
     * shows up where disabling the internal pull up is helpful, we can extend
     * i2c_conf_t to make it configurable */
    scl_mode |= IOCON_MODE_PULLUP;

    gpio_init_mux(conf->pin_sda, sda_mode);
    gpio_init_mux(conf->pin_scl, scl_mode);

    mutex_unlock(&locks[dev]);
}

static void _enable(I2C_Type *const i2c_dev)
{
    i2c_dev->CFG = I2C_CFG_MSTEN_MASK;
    while ((i2c_dev->STAT & I2C_STAT_MSTPENDING_MASK) == 0) {
        /* I2C peripheral not yet available, spin more ... */
    }

    /* I2C peripheral is online, check state is indeed idle  */
    assert((i2c_dev->STAT & I2C_STAT_MSTSTATE_MASK) == I2C_STAT_MSTSTATE_IDLE);
}

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    const i2c_conf_t *conf = &i2c_config[dev];
    I2C_Type *const i2c_dev = conf->dev;

    int flexcomm_num = flexcomm_init((FLEXCOMM_Type *)i2c_dev, FLEXCOMM_ID_I2C);
    assert(flexcomm_num >= 0);

    if (flexcomm_num == 1) {
        /* Disable the FLECOMM1 MULT1/DIV1 divisor. FLEXCOMM2 doesn't have a
         * fractional divisor. This divides the clock by (1 + N / 256) where
         * N is set to 0 here, so the fractional divisor only divides by one. */
        CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0u);
    }

    /* Enable controller mode, no timeout, no monitor, no clock stretching. */
    _enable(i2c_dev);
    _i2c_controller_set_speed(i2c_dev, conf->speed);
    locks[dev] = (mutex_t)MUTEX_INIT_LOCKED;

    /* This also unlocks the mutex. */
    _i2c_init_pins(dev);
}

#ifdef MODULE_PERIPH_I2C_RECONFIGURE
void i2c_init_pins(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    i2c_config[dev].dev->CFG |= I2C_CFG_MSTEN_MASK;
    _i2c_init_pins(dev);
}

void i2c_deinit_pins(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    const i2c_conf_t *conf = &i2c_config[dev];
    mutex_lock(&locks[dev]);
    conf->dev->CFG &= ~I2C_CFG_MSTEN_MASK;
    gpio_init_mux(conf->pin_sda, 0);
    gpio_init_mux(conf->pin_scl, 0);
}
#endif /* MODULE_PERIPH_I2C_RECONFIGURE */

void i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_lock(&locks[dev]);
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);
    mutex_unlock(&locks[dev]);
}

static uint32_t _i2c_wait_idle(I2C_Type *i2c_dev)
{
    uint32_t status;

    do {
        status = i2c_dev->STAT;
    } while ((status & I2C_STAT_MSTPENDING_MASK) == 0);

    i2c_dev->STAT = I2C_STAT_MSTARBLOSS_MASK | I2C_STAT_MSTSTSTPERR_MASK;
    return status;
}

static void _i2c_start(I2C_Type *i2c_dev, uint8_t addr_dir)
{
    _i2c_wait_idle(i2c_dev);
    i2c_dev->MSTDAT = addr_dir;
    i2c_dev->MSTCTL = I2C_MSTCTL_MSTSTART_MASK; /* start */
}

static uint32_t _i2c_stop(I2C_Type *i2c_dev)
{
    uint32_t status = _i2c_wait_idle(i2c_dev);

    i2c_dev->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK; /* stop */
    return status;
}


static int _i2c_transfer_blocking(i2c_t dev, uint32_t addr_dir, uint8_t *data,
                                  size_t len, uint8_t flags)
{
    assert((data != NULL) || (len == 0));
    I2C_Type *i2c_dev = i2c_config[dev].dev;

    uint32_t status = 0;
    uint32_t controller_state;

    if ((flags & I2C_NOSTART) == 0) {
        if ((flags & I2C_ADDR10) != 0) {
            _i2c_start(i2c_dev,
                       0xf0 | (addr_dir & 1) | ((addr_dir >> 8) & 0x6));
            /* The second call to start sends a repeated start */
            _i2c_start(i2c_dev, (addr_dir >> 1) & 0xff);
        }
        else {
            _i2c_start(i2c_dev, addr_dir);
        }
    }

    while (len) {
        status = _i2c_wait_idle(i2c_dev);
        if (status & (I2C_STAT_MSTARBLOSS_MASK | I2C_STAT_MSTSTSTPERR_MASK)) {
            break;
        }
        controller_state = (status & I2C_STAT_MSTSTATE_MASK) >>
                           I2C_STAT_MSTSTATE_SHIFT;
        switch (controller_state) {
        case I2C_STAT_MSTSTATE_TXREADY:
            /* I2C write case. */
            if ((addr_dir & 1) != 0) {
                /* This means that the direction requested in the transfer
                 * was to read, without a start condition and following
                 * a previous request to write, so the block is ready to
                 * write but we should be reading. */
                _i2c_stop(i2c_dev);
                return -EINVAL;
            }
            i2c_dev->MSTDAT = *(data++);
            len--;
            i2c_dev->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK;
            break;
        case I2C_STAT_MSTSTATE_RXREADY:
            /* I2C read case. */
            if ((addr_dir & 1) != 1) {
                /* Analog to the write case, we should be trying to writing
                 * in this case. */
                i2c_dev->MSTDAT;
                _i2c_stop(i2c_dev);
                return -EINVAL;
            }

            *(data++) = i2c_dev->MSTDAT;
            if (--len) {
                i2c_dev->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK;
            }
            else {
                if ((flags & I2C_NOSTOP) == 0) {
                    /* initiate NAK and stop */
                    i2c_dev->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
                    status = _i2c_wait_idle(i2c_dev);
                }
            }
            break;

        case I2C_STAT_MSTSTATE_NACKADR:
            /* NACK on the address means no device. */
            i2c_dev->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
            return -ENXIO;

        case I2C_STAT_MSTSTATE_NACKDAT:
            /* NACK on the data means that the device didn't ACK the last
             * byte. */
            i2c_dev->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
            return -EIO;

        default:
            return -EINVAL;
        }
    }
    /* In the write case we didn't send a stop condition yet. */
    if ((flags & I2C_NOSTOP) == 0 && (addr_dir & 1) == 0) {
        status = _i2c_stop(i2c_dev);
        /* The status will contain the last ACK/NACK */
        controller_state = (status & I2C_STAT_MSTSTATE_MASK) >>
                           I2C_STAT_MSTSTATE_SHIFT;
        if (controller_state == I2C_STAT_MSTSTATE_NACKDAT) {
            return -EIO;
        }
    }
    if (status & I2C_STAT_MSTARBLOSS_MASK) {
        DEBUG("[i2c] Controller arbitration loss error flag, status 0x%.2"
              PRIx32 "\n", status);
        return -EAGAIN;
    }

    if (status & I2C_STAT_MSTSTSTPERR_MASK) {
        /* Controller start/stop error flag. */
        DEBUG("[i2c] Controller start/stop error flag\n");
        return -EINVAL;
    }
    return 0;
}

#define I2C_ADDR_READ(addr) (((uint32_t)(addr) << 1u) | 1u)
#define I2C_ADDR_WRITE(addr) ((uint32_t)(addr) << 1u)

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data,
                   size_t len, uint8_t flags)
{
    DEBUG("[i2c] R a=%.2x len=%2u f=%.2x, data:", addr, (unsigned)len, flags);
    int ret =
        _i2c_transfer_blocking(dev, I2C_ADDR_READ(addr), data, len, flags);
#if ENABLE_DEBUG
    for (size_t i = 0; i < len; i++) {
        DEBUG(" %.2x", ((uint8_t *)data)[i]);
    }
    DEBUG(", ret=%d\n", ret);
#endif
    return ret;
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len,
                    uint8_t flags)
{
    /* The write transfer should technically only use a const uint8_t* buffer
     * but we are re-using the same function to save on code here. It will not
     * be written to in the I2C write case. */
#if ENABLE_DEBUG
    DEBUG("[i2c] W a=%.2x len=%2u f=%.2x, data:", addr, (unsigned)len, flags);
    for (size_t i = 0; i < len; i++) {
        DEBUG(" %.2x", ((uint8_t *)data)[i]);
    }
#endif
    int ret = _i2c_transfer_blocking(
        dev, I2C_ADDR_WRITE(addr), (uint8_t *)data, len, flags);
    DEBUG(", ret=%d\n", ret);
    return ret;
}

void isr_flexcomm_i2c(USART_Type *dev, uint32_t flexcomm_num)
{
    // TODO: Set up async mode with interrupts.
    (void)dev;
    (void)flexcomm_num;

    cortexm_isr_end();
}
