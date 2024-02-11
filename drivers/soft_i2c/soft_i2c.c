/*
 * Copyright (C) 2024 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_soft_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver software implementation using GPIO-LL
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <endian.h>
#include <errno.h>
#include <stdbool.h>

#include "compiler_hints.h"
#include "macros/units.h"
#include "modules.h"
#include "mutex.h"
#include "periph/gpio_ll.h"
#include "soft_i2c.h"
#include "soft_i2c_params.h"
#include "time_units.h"
#include "tiny_strerror.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef CONFIG_SOFT_I2C_TIMEOUT_US
#  define CONFIG_SOFT_I2C_TIMEOUT_US    (10 * US_PER_MS)
#endif

#define OD_MODE IS_USED(MODULE_PERIPH_GPIO_LL_OPEN_DRAIN)

typedef struct {
    uint32_t half_period_us;
    mutex_t lock;
    gpio_port_t scl_port;
    gpio_port_t sda_port;
    gpio_conf_t pin_conf;
    uword_t scl_mask;
    uword_t sda_mask;
    bool started;
} soft_i2c_bus_t;

static soft_i2c_bus_t soft_i2c_bus[SOFT_I2C_NUMOF] = {};

/**
 * @name    Debug pins (only when ENABLE_DEBUG == 1 and not set to GPIO_PORT_UNDEF)
 * @{
 */
/**
 * @name    GPIO pin set during tx/rx of every byte
 * @{
 */
static const gpio_port_t _debug_port_frame = GPIO_PORT_UNDEF;
static const uint8_t _debug_pin_frame = 0;
/** @} */
/**
 * @name    GPIO pin toggled when sampling a bit during RX
 * @{
 */
static const gpio_port_t _debug_port_rx_sample = GPIO_PORT_UNDEF;
static const uint8_t _debug_pin_rx_sample = 0;
/** @} */
/**
 * @name    GPIO pin set to the value of the last received bit
 * @{
 */
static const gpio_port_t _debug_port__rx_bit = GPIO_PORT_UNDEF;
static const uint8_t _debug_pin__rx_bit = 0;
/** @} */
/** @} */

static int _recover_locked_bus(soft_i2c_bus_t *b);

static void _debug_print_state(soft_i2c_t dev)
{
    if (ENABLE_DEBUG) {
        const soft_i2c_bus_t *b = &soft_i2c_bus[dev];
        const soft_i2c_conf_t *c = &soft_i2c_config[dev];

        DEBUG("[soft_i2c] SCL (P%u.%u / P%c%u): ",
              (unsigned)GPIO_PORT_NUM(b->scl_port), (unsigned)gpio_get_pin_num(c->scl_pin),
              'A' + (int)GPIO_PORT_NUM(b->scl_port), (unsigned)gpio_get_pin_num(c->scl_pin));
        gpio_ll_print_conf(gpio_ll_query_conf(b->scl_port, gpio_get_pin_num(c->scl_pin)));
        DEBUG(", bus: %u\n", (unsigned)(!!(gpio_ll_read(b->scl_port) & b->scl_mask)));
        DEBUG("[soft_i2c] SDA (P%u.%u / P%c%u): ",
              (unsigned)GPIO_PORT_NUM(b->sda_port), (unsigned)gpio_get_pin_num(c->sda_pin),
              'A' + (int)GPIO_PORT_NUM(b->sda_port), (unsigned)gpio_get_pin_num(c->sda_pin));
        gpio_ll_print_conf(gpio_ll_query_conf(b->sda_port, gpio_get_pin_num(c->sda_pin)));
        DEBUG(", bus: %u\n", (unsigned)(!!(gpio_ll_read(b->sda_port) & b->sda_mask)));
    }
}

void soft_i2c_init(soft_i2c_t dev)
{
    DEBUG("[soft_i2c] Initializing device %u\n", (unsigned)dev);
    assume(dev < SOFT_I2C_NUMOF);

    soft_i2c_bus_t *b = &soft_i2c_bus[dev];
    const soft_i2c_conf_t *c = &soft_i2c_config[dev];

    b->lock = (mutex_t)MUTEX_INIT_LOCKED;
    b->scl_port = gpio_get_port(c->scl_pin);
    b->sda_port = gpio_get_port(c->sda_pin);
    b->scl_mask = 1ULL << gpio_get_pin_num(c->scl_pin);
    b->sda_mask = 1ULL << gpio_get_pin_num(c->sda_pin);
    DEBUG("[soft_i2c] scl_mask = %04x, sda_mask = %04x\n",
           (unsigned)b->scl_mask, (unsigned)b->sda_mask);
    switch (c->speed) {
    default:
    case I2C_SPEED_NORMAL:
        b->half_period_us = US_PER_SEC / KHZ(100) / 2;
        break;
    case I2C_SPEED_FAST:
        b->half_period_us = US_PER_SEC / KHZ(400) / 2;
        break;
    case I2C_SPEED_FAST_PLUS:
        /* yeah, this is 0. But chances are good that calling
         * `ztimer_spin(ZTIMER_USEC, 0)` will delay by at least
         * 0.5 us just due to function call overhead. */
        b->half_period_us = US_PER_SEC / MHZ(1) / 2;
        break;
    }

    DEBUG("[soft_i2c] waiting 2 × %" PRIu32 " μs per bit\n", b->half_period_us);

    if (OD_MODE) {
        b->pin_conf = (gpio_conf_t){
            .state = GPIO_OUTPUT_OPEN_DRAIN,
            .pull = c->enable_internal_pull_ups ? GPIO_PULL_UP : GPIO_FLOATING,
            .initial_value = true,
        };
        DEBUG_PUTS("[soft_i2c] open drain implementation");
    }
    else {
        b->pin_conf = (gpio_conf_t){
            .state = GPIO_INPUT,
            .pull = c->enable_internal_pull_ups ? GPIO_PULL_UP : GPIO_FLOATING,
        };
        DEBUG_PUTS("[soft_i2c] switch dir mode");
    }

    soft_i2c_init_pins(dev);

    if (ENABLE_DEBUG) {
        if (_debug_port_frame != GPIO_PORT_UNDEF) {
            gpio_ll_init(_debug_port_frame, _debug_pin_frame, gpio_ll_out);
        }
        if (_debug_port_rx_sample != GPIO_PORT_UNDEF) {
            gpio_ll_init(_debug_port_rx_sample, _debug_pin_rx_sample, gpio_ll_out);
        }
        if (_debug_port__rx_bit != GPIO_PORT_UNDEF) {
            gpio_ll_init(_debug_port__rx_bit, _debug_pin__rx_bit, gpio_ll_out);

        }

        DEBUG("[soft_i2c] Expected GPIO mode: ");
        gpio_ll_print_conf(b->pin_conf);
        DEBUG_PUTS("");
        _debug_print_state(dev);
    }

    if (!(gpio_ll_read(b->sda_port) & b->sda_mask)) {
        DEBUG_PUTS("[soft_i2c] I2C bus found locked during init, trying to recover");
        _recover_locked_bus(b);
    }
}

void soft_i2c_init_pins(soft_i2c_t dev)
{
    assume(dev < SOFT_I2C_NUMOF);

    soft_i2c_bus_t *b = &soft_i2c_bus[dev];
    const soft_i2c_conf_t *c = &soft_i2c_config[dev];

    assume(gpio_ll_init(b->scl_port, gpio_get_pin_num(c->scl_pin), b->pin_conf) == 0);
    assume(gpio_ll_init(b->sda_port, gpio_get_pin_num(c->sda_pin), b->pin_conf) == 0);
    mutex_unlock(&b->lock);
}

void soft_i2c_deinit_pins(soft_i2c_t dev)
{
    assume(dev < SOFT_I2C_NUMOF);

    soft_i2c_bus_t *b = &soft_i2c_bus[dev];
    const soft_i2c_conf_t *c = &soft_i2c_config[dev];

    mutex_lock(&b->lock);
    gpio_conf_t pincfg = {
        .state = GPIO_DISCONNECT,
        .pull = c->enable_internal_pull_ups ? GPIO_PULL_UP : GPIO_FLOATING,
    };
    assume(gpio_ll_init(b->scl_port, gpio_get_pin_num(c->scl_pin), pincfg) == 0);
    assume(gpio_ll_init(b->sda_port, gpio_get_pin_num(c->scl_pin), pincfg) == 0);
}

gpio_t soft_i2c_pin_sda(soft_i2c_t dev)
{
    assume(dev < SOFT_I2C_NUMOF);

    const soft_i2c_conf_t *c = &soft_i2c_config[dev];
    return c->sda_pin;
}

gpio_t soft_i2c_pin_scl(soft_i2c_t dev)
{
    assume(dev < SOFT_I2C_NUMOF);

    const soft_i2c_conf_t *c = &soft_i2c_config[dev];
    return c->scl_pin;
}

void soft_i2c_acquire(soft_i2c_t dev)
{
    assume(dev < SOFT_I2C_NUMOF);

    soft_i2c_bus_t *b = &soft_i2c_bus[dev];

    mutex_lock(&b->lock);
}

void soft_i2c_release(soft_i2c_t dev)
{
    assume(dev < SOFT_I2C_NUMOF);

    soft_i2c_bus_t *b = &soft_i2c_bus[dev];

    mutex_unlock(&b->lock);

}

/**
 * @brief   Delay execution for half a clock cycle
 */
static void _spin_half_period(const soft_i2c_bus_t *b)
{
    ztimer_spin(ZTIMER_USEC, b->half_period_us);
}

/**
 * @brief   Pull SCL up, don't wait for it to rise
 */
static void _scl_up(const soft_i2c_bus_t *b)
{
    if (OD_MODE) {
        gpio_ll_set(b->scl_port, b->scl_mask);
    }
    else {
        gpio_ll_switch_dir_input(b->scl_port, b->scl_mask);
        /* On SAM0 MCUs, this sets the pull direction. We want to pull up
         * (if pull resistors are enabled), not down */
        gpio_ll_set(b->scl_port, b->scl_mask);
    }
}

/**
 * @brief   Release control of SCL line
 *
 * Same as _scl_up, but it makes intention more obvious when calling.
 */
static void _scl_release(const soft_i2c_bus_t *b)
{
    _scl_up(b);
}

/**
 * @brief   Pull SCL up and wait for it to actually rise
 *
 * @retval  false   Timeout occurred
 * @retval  true    SCL is high
 */
static bool _scl_up_and_wait(const soft_i2c_bus_t *b)
{
    _scl_up(b);
    ztimer_acquire(ZTIMER_USEC);
    uint32_t timeout = ztimer_now(ZTIMER_USEC) + CONFIG_SOFT_I2C_TIMEOUT_US;
    /* Wait for SCL to be pulled up */
    while (!(gpio_ll_read(b->scl_port) & b->scl_mask)) {
        if (timeout - ztimer_now(ZTIMER_USEC) > CONFIG_SOFT_I2C_TIMEOUT_US) {
            ztimer_release(ZTIMER_USEC);
            return false;
        }
    }
    ztimer_release(ZTIMER_USEC);

    return true;
}

/**
 * @brief   Drive SCL line down
 */
static void _scl_down(const soft_i2c_bus_t *b)
{
    if (OD_MODE) {
        gpio_ll_clear(b->scl_port, b->scl_mask);
    }
    else {
        /* On SAM0 MCUs, the output buffer is used to control the pull
         * direction. We MUST NEVER EVER drive the SCL high, so we clear
         * the bit in the output buffer before switching to output direction */
        gpio_ll_clear(b->scl_port, b->scl_mask);
        gpio_ll_switch_dir_output(b->scl_port, b->scl_mask);
    }
}

/**
 * @brief   Pull SDA up, but don't wait for it to rise
 */
static void _sda_up(const soft_i2c_bus_t *b)
{
    if (OD_MODE) {
        gpio_ll_set(b->sda_port, b->sda_mask);
    }
    else {
        gpio_ll_switch_dir_input(b->sda_port, b->sda_mask);
        /* On SAM0 MCUs, this sets the pull direction. We want to pull up
         * (if pull resistors are enabled), not down */
        gpio_ll_set(b->sda_port, b->sda_mask);
    }
}

/**
 * @brief   Release control of SDA line
 *
 * Same as _sda_up, but it makes intention more obvious when calling.
 */
static void _sda_release(const soft_i2c_bus_t *b)
{
    _sda_up(b);
}

/**
 * @brief   Pull SDA up and wait for it to actually rise
 *
 * @retval  false   Timeout occurred
 * @retval  true    SDA is high
 */
static bool _sda_up_and_wait(const soft_i2c_bus_t *b)
{
    _sda_up(b);
    ztimer_acquire(ZTIMER_USEC);
    uint32_t timeout = ztimer_now(ZTIMER_USEC) + CONFIG_SOFT_I2C_TIMEOUT_US;
    /* Wait for SCL to be pulled up */
    while (!(gpio_ll_read(b->sda_port) & b->sda_mask)) {
        if (timeout - ztimer_now(ZTIMER_USEC) > CONFIG_SOFT_I2C_TIMEOUT_US) {
            ztimer_release(ZTIMER_USEC);
            return false;
        }
    }
    ztimer_release(ZTIMER_USEC);

    return true;
}

/**
 * @brief   Drive SDA line down
 */
static void _sda_down(const soft_i2c_bus_t *b)
{
    if (OD_MODE) {
        gpio_ll_clear(b->sda_port, b->sda_mask);
    }
    else {
        /* On SAM0 MCUs, the output buffer is used to control the pull
         * direction. We MUST NEVER EVER drive the SDA high, so we clear
         * the bit in the output buffer before switching to output direction */
        gpio_ll_clear(b->sda_port, b->sda_mask);
        gpio_ll_switch_dir_output(b->sda_port, b->sda_mask);
    }
}

/**
 * @brief   Transmit a (repeated) start condition
 *
 * @retval  -ETIMEDOUT  Timeout while waiting for SCL to rise
 * @retval  -EAGAIN     Arbitration lost
 */
static int _start_condition(soft_i2c_bus_t *b)
{
    DEBUG("[soft_i2c] %sstart condition\n", b->started ? "repeated " : "");
    if (b->started) {
        /* repeated start */
        _sda_up(b);
        _spin_half_period(b);
        if (!_scl_up_and_wait(b)) {
            return -ETIMEDOUT;
        }
        _spin_half_period(b);
    }

    if (!(gpio_ll_read(b->sda_port) & b->sda_mask)) {
        /* arbitration lost */
        return -EAGAIN;
    }

    _sda_down(b);
    _spin_half_period(b);
    _scl_down(b);
    b->started = true;

    /* arbitration won */
    return 0;
}

/**
 * @brief   Transmit a stop condition
 *
 * @retval  -ETIMEDOUT  Timeout while waiting for SCL to rise
 * @retval  -EAGAIN     Arbitration lost
 */
static int _stop_condition(soft_i2c_bus_t *b)
{
    DEBUG_PUTS("[soft_i2c] stop");
    _sda_down(b);
    _spin_half_period(b);
    if (!_scl_up_and_wait(b)) {
        return -ETIMEDOUT;
    }
    _spin_half_period(b);
    _sda_up(b);
    _spin_half_period(b);
    b->started = false;

    /* if SDA is high, arbitration was won, otherwise lost */
    if (!(gpio_ll_read(b->sda_port) & b->sda_mask)) {
        return -EAGAIN;
    }

    return 0;
}

/**
 * @brief   Transmit a bit over the I2C bus
 *
 * @retval  -ETIMEDOUT  Timeout while waiting for SCL to rise
 * @retval  -EAGAIN     Arbitration lost
 */
static int _tx_bit(const soft_i2c_bus_t *b, bool bit)
{
    if (bit) {
        _sda_up(b);
    }
    else {
        _sda_down(b);
    }

    _spin_half_period(b);
    if (!_scl_up_and_wait(b)) {
        return -ETIMEDOUT;
    }
    _spin_half_period(b);
    if (bit != (bool)(gpio_ll_read(b->sda_port) & b->sda_mask)) {
        /* arbitration lost */
        return -EAGAIN;
    }

    _scl_down(b);
    return 0;
}

/**
 * @brief   Receive a bit on the I2C bus
 *
 * @retval  -ETIMEDOUT      SCL didn't rise in time
 * @retval  0               Retrieved bit zero
 * @retval  1               Retrieved bit one
 */
static int _rx_bit(const soft_i2c_bus_t *b)
{
    _sda_release(b);
    _spin_half_period(b);
    if (!_scl_up_and_wait(b)) {
        return -ETIMEDOUT;
    }
    _spin_half_period(b);
    if (ENABLE_DEBUG && (_debug_port_rx_sample != GPIO_PORT_UNDEF)) {
        gpio_ll_toggle(_debug_port_rx_sample, 1U << _debug_pin_rx_sample);
    }
    int result = !!(gpio_ll_read(b->sda_port) & b->sda_mask);
    if (ENABLE_DEBUG && (_debug_port__rx_bit != GPIO_PORT_UNDEF)) {
        if (result) {
            gpio_ll_set(_debug_port__rx_bit, 1U << _debug_pin__rx_bit);
        }
        else {
            gpio_ll_clear(_debug_port__rx_bit, 1U << _debug_pin__rx_bit);
        }
    }
    _scl_down(b);
    return result;
}

/**
 * @brief   Transmit a byte over the I2C bus
 *
 * @retval  -ETIMEDOUT      SCL didn't rise in time
 * @retval  -EAGAIN         Arbitration lost
 * @retval  -ENXIO          NACK received
 */
static int _tx_byte(const soft_i2c_bus_t *b, uint8_t data)
{
    DEBUG("[soft_i2c] TX: 0x%02x\n", (unsigned)data);
    if (ENABLE_DEBUG && (_debug_port_frame != GPIO_PORT_UNDEF)) {
        gpio_ll_set(_debug_port_frame, 1U << _debug_pin_frame);
    }
    for (uword_t i = 0; i < 8; i++) {
        int retval = _tx_bit(b, data & 0x80);
        if (retval) {
            /* arbitration lost / timeout */
            if (ENABLE_DEBUG && (_debug_port_frame != GPIO_PORT_UNDEF)) {
                gpio_ll_clear(_debug_port_frame, 1U << _debug_pin_frame);
            }
            return retval;
        }
        data <<= 1;
    }

    int retval = _rx_bit(b);
    if (retval < 0) {
        /* timeout occured */
        if (ENABLE_DEBUG && (_debug_port_frame != GPIO_PORT_UNDEF)) {
            gpio_ll_clear(_debug_port_frame, 1U << _debug_pin_frame);
        }
        return retval;
    }

    if (retval > 0) {
        /* NACK received */
        if (ENABLE_DEBUG && (_debug_port_frame != GPIO_PORT_UNDEF)) {
            gpio_ll_clear(_debug_port_frame, 1U << _debug_pin_frame);
        }
        return -ENXIO;
    }

    if (ENABLE_DEBUG && (_debug_port_frame != GPIO_PORT_UNDEF)) {
        gpio_ll_clear(_debug_port_frame, 1U << _debug_pin_frame);
    }
    return 0;
}

/**
 * @brief   Receive a byte on the I2C bus
 * @retval  -ETIMEDOUT      SCL didn't rise
 * @retval  >=0             The received byte
 */
static int _rx_byte(const soft_i2c_bus_t *b, bool nack)
{
    uint8_t result = 0;
    int retval;
    if (ENABLE_DEBUG && (_debug_port_frame != GPIO_PORT_UNDEF)) {
        gpio_ll_set(_debug_port_frame, 1U << _debug_pin_frame);
    }
    for (uword_t i = 0; i < 8; i++) {
        result <<= 1;
        retval = _rx_bit(b);
        if (retval < 0) {
            return retval;
        }
        result |= retval;
    }

    retval = _tx_bit(b, nack);

    if (ENABLE_DEBUG && (_debug_port_frame != GPIO_PORT_UNDEF)) {
        gpio_ll_clear(_debug_port_frame, 1U << _debug_pin_frame);
    }

    if (retval) {
        /* timeout / arbitration lost */
        return retval;
    }

    DEBUG("[soft_i2c] RX: 0x%02x\n", (unsigned)result);
    return result;
}

static int _recover_locked_bus(soft_i2c_bus_t *b)
{
    DEBUG_PUTS("[soft_i2c] Trying to recover locked bus");
    for (unsigned i = 10; i > 0; i--) {
        _spin_half_period(b);
        if (!_scl_up_and_wait(b)) {
            DEBUG_PUTS("[soft_i2c] recovery timed out");
            return -ETIMEDOUT;
        }
        _spin_half_period(b);
        _scl_down(b);
    }

    if (gpio_ll_read(b->sda_port) & b->sda_mask) {
        _stop_condition(b);
        DEBUG_PUTS("[soft_i2c] SDA no longer locked\n");
    }

    return -EAGAIN;
}

/**
 * @brief   Handle the given error condition
 *
 * @param[in]   dev         I2C device the error occurred on
 * @param[in]   retval      Negative errno indicating the cause
 * @param[in]   debug_msg   Message to pass to `DEBUG()`
 *
 * @note    Some error recovery is done based on @p retval, so keep this in
 *          sync with potential causes
 *
 * @return  Passes through @retval
 */
static int _err(soft_i2c_t dev, int retval, const char *debug_msg)
{
    (void)debug_msg;
    soft_i2c_bus_t *b = &soft_i2c_bus[dev];
    b->started = false;

    switch (retval) {
    case -ENXIO:
        /* NACK, sending stop */
        _stop_condition(b);
        break;
    case -EAGAIN:
        /* arbitration lost, releasing bus */
        _scl_release(b);
        _spin_half_period(b);
        if (!_sda_up_and_wait(b)) {
            /* not an arbitration loss, but a locked I2C bus */
            return _recover_locked_bus(b);
        }
        break;
    case -ETIMEDOUT:
        break;
    default:
        /* this indicates a bug in this driver */
        assert(0);
        break;
    }
    DEBUG("[soft_i2c] %s: %s\n", debug_msg, tiny_strerror(retval));
    _debug_print_state(dev);

    return retval;
}

static int send_addr(soft_i2c_t dev, uint16_t addr, bool direction_rx, uint8_t flags)
{
    int retval;
    soft_i2c_bus_t *b = &soft_i2c_bus[dev];

    /* send start, if needed */
    if (!(flags & I2C_NOSTART)) {
        int retval = _start_condition(b);
        if (retval) {
            return _err(dev, retval, "during start condition");
        }
    }

    if (flags & I2C_ADDR10) {
        uint8_t top_byte = addr >> 9;
        top_byte &= 0x06;
        top_byte |= 0xf0 | (unsigned)direction_rx;
        uint8_t bottom_byte = addr;
        retval = _tx_byte(b, top_byte);
        if (retval) {
            return _err(dev, retval, "sending 1st byte of 10 bit addr failed");
        }
        retval = _tx_byte(b, bottom_byte);
    }
    else {
        retval = _tx_byte(b, (addr << 1) | (unsigned)direction_rx);
    }

    if (retval) {
        return _err(dev, retval, "sending addr (7bit or 2nd of 10bit) failed");
    }

    return 0;
}

int soft_i2c_read_bytes(soft_i2c_t dev, uint16_t addr, void *_data, size_t len,
                        uint8_t flags)
{
    DEBUG("[soft_i2c] soft_i2c_read_bytes(%u, %u, %p, %u, %x)\n",
          (unsigned)dev, (unsigned)addr, _data, (unsigned)len, (unsigned)flags);
    assume(dev < SOFT_I2C_NUMOF);

    int retval;
    uint8_t *data = _data;
    soft_i2c_bus_t *b = &soft_i2c_bus[dev];

    retval = send_addr(dev, addr, true, flags);
    if (retval) {
        return retval;
    }

    while (len) {
        retval = _rx_byte(b, len == 1);
        if (retval < 0) {
            _err(dev, retval, "during _rx_byte");
        }
        *data = retval;
        data++;
        len--;
    }

    if (!(flags & I2C_NOSTOP)) {
        retval = _stop_condition(b);
        if (retval) {
            return _err(dev, retval, "during stop condition");
        }
    }

    return 0;
}

int soft_i2c_write_bytes(soft_i2c_t dev, uint16_t addr, const void *_data,
                         size_t len, uint8_t flags)
{
    DEBUG("[soft_i2c] soft_i2c_write_bytes(%u, %u, %p, %u, %x)\n",
          (unsigned)dev, (unsigned)addr, _data, (unsigned)len, (unsigned)flags);
    assume(dev < SOFT_I2C_NUMOF);

    int retval;
    const uint8_t *data = _data;
    soft_i2c_bus_t *b = &soft_i2c_bus[dev];

    retval = send_addr(dev, addr, false, flags);
    if (retval) {
        return retval;
    }

    while (len) {
        retval = _tx_byte(b, *data);
        if (retval) {
            return _err(dev, retval, "sending data byte failed");
        }
        data++;
        len--;
    }

    if (!(flags & I2C_NOSTOP)) {
        retval = _stop_condition(b);
        if (retval) {
            return _err(dev, retval, "during stop condition");
        }
    }

    return 0;
}

/* convenience functions: */

int soft_i2c_read_reg(soft_i2c_t dev, uint16_t addr, uint16_t reg,
                      void *data, uint8_t flags)
{
    return soft_i2c_read_regs(dev, addr, reg, data, 1, flags);
}

int soft_i2c_read_regs(soft_i2c_t dev, uint16_t addr, uint16_t reg,
                       void *data, size_t len, uint8_t flags)
{
    uint16_t reg_end = reg;

    if (flags & (I2C_NOSTOP | I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }

    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg_end = htobe16(reg); /* Make sure register is in big-endian on I2C bus */
    }

    /* First set ADDR and register with no stop */
    int ret = soft_i2c_write_bytes(dev, addr, &reg_end, (flags & I2C_REG16) ? 2 : 1,
                                   flags | I2C_NOSTOP);
    if (ret < 0) {
        return ret;
    }
    /* Then get the data from device */
    return soft_i2c_read_bytes(dev, addr, data, len, flags);

}

int soft_i2c_read_byte(soft_i2c_t dev, uint16_t addr, void *data, uint8_t flags)
{
    return soft_i2c_read_bytes(dev, addr, data, 1, flags);
}

int soft_i2c_write_byte(soft_i2c_t dev, uint16_t addr, uint8_t data,
                        uint8_t flags)
{
    return soft_i2c_write_bytes(dev, addr, &data, 1, flags);
}

int soft_i2c_write_reg(soft_i2c_t dev, uint16_t addr, uint16_t reg,
                       uint8_t data, uint8_t flags)
{
    return soft_i2c_write_regs(dev, addr, reg, &data, 1, flags);
}

int soft_i2c_write_regs(soft_i2c_t dev, uint16_t addr, uint16_t reg,
                        const void *data, size_t len, uint8_t flags)
{
    uint16_t reg_end = reg;

    if (flags & (I2C_NOSTOP | I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }

    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg_end = htobe16(reg); /* Make sure register is in big-endian on I2C bus */
    }

    /* First set ADDR and register with no stop */
    int ret = soft_i2c_write_bytes(dev, addr, &reg_end, (flags & I2C_REG16) ? 2 : 1,
                                   flags | I2C_NOSTOP);
    if (ret < 0) {
        return ret;
    }
    /* Then write data to the device */
    return soft_i2c_write_bytes(dev, addr, data, len, flags | I2C_NOSTART);
}

void auto_init_soft_i2c(void)
{
    for (unsigned i = 0; i < SOFT_I2C_NUMOF; i++) {
        soft_i2c_init(SOFT_I2C_DEV(i));
    }
}

/* provide periph_i2c API via soft_i2c as weak symbols */
__attribute__((weak, alias("soft_i2c_init")))
void i2c_init(i2c_t dev);

__attribute__((weak, alias("soft_i2c_init_pins")))
void i2c_init_pins(i2c_t dev);

__attribute__((weak, alias("soft_i2c_deinit_pins")))
void i2c_deinit_pins(i2c_t dev);

__attribute__((weak, alias("soft_i2c_pin_sda")))
gpio_t i2c_pin_sda(i2c_t dev);

__attribute__((weak, alias("soft_i2c_pin_scl")))
gpio_t i2c_pin_scl(i2c_t dev);

__attribute__((weak, alias("soft_i2c_acquire")))
void i2c_acquire(i2c_t dev);

__attribute__((weak, alias("soft_i2c_release")))
void i2c_release(i2c_t dev);

__attribute__((weak, alias("soft_i2c_read_reg")))
int i2c_read_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                 void *data, uint8_t flags);

__attribute__((weak, alias("soft_i2c_read_regs")))
int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags);

__attribute__((weak, alias("soft_i2c_read_byte")))
int i2c_read_byte(i2c_t dev, uint16_t addr, void *data, uint8_t flags);

__attribute__((weak, alias("soft_i2c_read_bytes")))
int i2c_read_bytes(i2c_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags);

__attribute__((weak, alias("soft_i2c_write_byte")))
int i2c_write_byte(i2c_t dev, uint16_t addr, uint8_t data, uint8_t flags);

__attribute__((weak, alias("soft_i2c_write_bytes")))
int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags);

__attribute__((weak, alias("soft_i2c_write_reg")))
int i2c_write_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                  uint8_t data, uint8_t flags);

__attribute__((weak, alias("soft_i2c_write_regs")))
int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  const void *data, size_t len, uint8_t flags);
