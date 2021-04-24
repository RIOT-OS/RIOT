/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver software implementation using for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

/*
   PLEASE NOTE:

   The implementation bases on the bit-banging I2C master implementation as
   described in [wikipedia](https://en.wikipedia.org/wiki/I%C2%B2C#Example_of_bit-banging_the_I%C2%B2C_master_protocol).
*/

#define ENABLE_DEBUG 0
#include "debug.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include "cpu.h"
#include "log.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#include "esp_attr.h"
#include "esp_common.h"
#include "gpio_arch.h"
#include "rom/ets_sys.h"

#ifdef MCU_ESP32

#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

/* max clock stretching counter */
#define I2C_CLOCK_STRETCH 200

/* gpio access macros */
#define GPIO_SET(l,h,b) if (b < 32) GPIO.l =  BIT(b); else GPIO.h.val =  BIT(b-32)
#define GPIO_GET(l,h,b) ((b < 32) ? GPIO.l & BIT(b) : GPIO.h.val & BIT(b-32))

#else /* MCU_ESP32 */

#include "esp/gpio_regs.h"
#include "sdk/ets.h"

/**
 * @brief Clock stretching counter.
 *
 * Set to 0 to disable clock stretching. This will cause SCL to be always driven
 * instead of open-drain, which allows using GPIO15 (normally pulled down to
 * boot) to be used as SCL.
 */
#ifndef I2C_CLOCK_STRETCH
/* max clock stretching counter (ca. 10 ms) */
#define I2C_CLOCK_STRETCH 40000
#endif /* I2C_CLOCK_STRETCH */

/* following functions have to be declared as extern since it is not possible */
/* to include user_interface.h due to conflicts with gpio_init */
extern uint8_t system_get_cpu_freq(void);
extern bool system_update_cpu_freq(uint8_t freq);

#endif /* MCU_ESP32 */

typedef struct
{
    i2c_speed_t speed;
    i2c_t dev;

    bool started;

    gpio_t scl;
    gpio_t sda;

    uint32_t scl_bit;   /* gpio bit mask for faster access */
    uint32_t sda_bit;   /* gpio bit mask for faster access */

    uint32_t delay;
    mutex_t  lock;

} _i2c_bus_t;

static _i2c_bus_t _i2c_bus[I2C_NUMOF] = {};

/* to ensure that I2C is always optimized with -O2 to use the defined delays */
#pragma GCC optimize ("O2")

#ifdef MCU_ESP32
static const uint32_t _i2c_delays[][3] =
{
    /* values specify one half-period and are only valid for -O2 option     */
    /* value = [period - 0.25 us (240 MHz) / 0.5us(160MHz) / 1.0us(80MHz)]  */
    /*         * cycles per second / 2                                      */
    /* 1 us = 16 cycles (80 MHz) / 32 cycles (160 MHz) / 48 cycles (240)    */
    /* values for             80,  160,  240 MHz                            */
    [I2C_SPEED_LOW]       = {790, 1590, 2390}, /*   10 kbps (period 100 us) */
    [I2C_SPEED_NORMAL]    = { 70,  150,  230}, /*  100 kbps (period 10 us)  */
    [I2C_SPEED_FAST]      = { 11,   31,   51}, /*  400 kbps (period 2.5 us) */
    [I2C_SPEED_FAST_PLUS] = {  0,    7,   15}, /*    1 Mbps (period 1 us)   */
    [I2C_SPEED_HIGH]      = {  0,    0,    0}  /*  3.4 Mbps (period 0.3 us) not working */
};
#else /* MCU_ESP32 */
static const uint32_t _i2c_delays[][2] =
{
    /* values specify one half-period and are only valid for -O2 option         */
    /* value = [period - 0.5us(160MHz) or 1.0us(80MHz)] * cycles per second / 2 */
    /* 1 us = 20 cycles (80 MHz) / 40 cycles (160 MHz)                          */
    [I2C_SPEED_LOW]       = {989, 1990}, /*   10 kbps (period 100 us)  */
    [I2C_SPEED_NORMAL]    = { 89,  190}, /*  100 kbps (period 10 us)   */
    [I2C_SPEED_FAST]      = { 15,   40}, /*  400 kbps (period 2.5 us)  */
    [I2C_SPEED_FAST_PLUS] = {  0,   13}, /*    1 Mbps (period 1 us)    */
    [I2C_SPEED_HIGH]      = {   0,   0}  /*  3.4 Mbps (period 0.3 us) is not working */
};
#endif /* MCU_ESP32 */

/* forward declaration of internal functions */

static inline void _i2c_delay(_i2c_bus_t* bus);
static inline bool _i2c_scl_read(_i2c_bus_t* bus);
static inline bool _i2c_sda_read(_i2c_bus_t* bus);
static inline void _i2c_scl_high(_i2c_bus_t* bus);
static inline void _i2c_scl_low(_i2c_bus_t* bus);
static inline void _i2c_sda_high(_i2c_bus_t* bus);
static inline void _i2c_sda_low(_i2c_bus_t* bus);
static int _i2c_start_cond(_i2c_bus_t* bus);
static int _i2c_stop_cond(_i2c_bus_t* bus);
static int _i2c_write_bit(_i2c_bus_t* bus, bool bit);
static int _i2c_read_bit(_i2c_bus_t* bus, bool* bit);
static int _i2c_write_byte(_i2c_bus_t* bus, uint8_t byte);
static int _i2c_read_byte(_i2c_bus_t* bus, uint8_t* byte, bool ack);
static int _i2c_arbitration_lost(_i2c_bus_t* bus, const char* func);
static void _i2c_abort(_i2c_bus_t* bus, const char* func);
static void _i2c_clear(_i2c_bus_t* bus);

/* implementation of i2c interface */

void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF_MAX);
    assert(dev < I2C_NUMOF);

    if (i2c_config[dev].speed == I2C_SPEED_HIGH) {
        LOG_TAG_INFO("i2c", "I2C_SPEED_HIGH is not supported\n");
        return;
    }

    mutex_init(&_i2c_bus[dev].lock);

    _i2c_bus[dev].scl   = i2c_config[dev].scl;
    _i2c_bus[dev].sda   = i2c_config[dev].sda;
    _i2c_bus[dev].speed = i2c_config[dev].speed;

    _i2c_bus[dev].dev     = dev;
    _i2c_bus[dev].scl_bit = BIT(_i2c_bus[dev].scl); /* store bit mask for faster access */
    _i2c_bus[dev].sda_bit = BIT(_i2c_bus[dev].sda); /* store bit mask for faster access */
    _i2c_bus[dev].started = false; /* for handling of repeated start condition */

    switch (ets_get_cpu_frequency()) {
        case  80: _i2c_bus[dev].delay = _i2c_delays[_i2c_bus[dev].speed][0]; break;
        case 160: _i2c_bus[dev].delay = _i2c_delays[_i2c_bus[dev].speed][1]; break;
#ifdef MCU_ESP32
        case 240: _i2c_bus[dev].delay = _i2c_delays[_i2c_bus[dev].speed][2]; break;
#endif
        default : LOG_TAG_INFO("i2c", "I2C software implementation is not "
                               "supported for this CPU frequency: %d MHz\n",
                               ets_get_cpu_frequency());
                  return;
    }

    DEBUG("%s: scl=%d sda=%d speed=%d\n", __func__,
          _i2c_bus[dev].scl, _i2c_bus[dev].sda, _i2c_bus[dev].speed);

    /* reset the GPIO usage if the pins were used for I2C before */
    if (gpio_get_pin_usage(_i2c_bus[dev].scl) == _I2C) {
        gpio_set_pin_usage(_i2c_bus[dev].scl, _GPIO);
    }
    if (gpio_get_pin_usage(_i2c_bus[dev].sda) == _I2C) {
        gpio_set_pin_usage(_i2c_bus[dev].sda, _GPIO);
    }

    /* Configure and initialize SDA and SCL pin. */
#ifdef MCU_ESP32
    /*
     * ESP32 pins are used in input/output mode with open-drain output driver.
     * Signal levels are then realized as following:
     *
     * - HIGH: Output value 1 lets the pin floating and is pulled-up to high.
     * - LOW : Output value 0 actively drives the pin to low.
     */
    if (gpio_init(_i2c_bus[dev].scl, GPIO_IN_OD_PU) ||
        gpio_init(_i2c_bus[dev].sda, GPIO_IN_OD_PU)) {
        return;
    }
#else /* MCU_ESP32 */
    /*
     * Due to critical timing required by the I2C software implementation,
     * the ESP8266 GPIOs can not be used directly in GPIO_OD_PU mode.
     * Instead, the GPIOs are configured in GPIO_IN_PU mode with open-drain
     * output driver. Signal levels are then realized as following:
     *
     * - HIGH: The GPIO is used in the configured GPIO_IN_PU mode. In this
     *         mode, the output driver is in open-drain mode and pulled-up.
     * - LOW : The GPIO is temporarily switched to GPIO_OD_PU mode. In this
     *         mode, the output value 0, which is written during
     *         initialization, actively drives the pin to low.
     *
     * When I2C_CLOCK_STRETCH is 0 (disabled), SCL is instead an always driven
     * output GPIO with no pull-up.
     */
    if (gpio_init(_i2c_bus[dev].scl, I2C_CLOCK_STRETCH ? GPIO_IN_PU : GPIO_OUT)
        || gpio_init(_i2c_bus[dev].sda, GPIO_IN_PU)) {
        return;
    }
#endif /* MCU_ESP32 */

    /* store the usage type in GPIO table */
    gpio_set_pin_usage(_i2c_bus[dev].scl, _I2C);
    gpio_set_pin_usage(_i2c_bus[dev].sda, _I2C);

    /* set SDA and SCL to be floating and pulled-up to high */
    _i2c_sda_high(&_i2c_bus[dev]);
    _i2c_scl_high(&_i2c_bus[dev]);

    /* clear the bus if necessary (SDA is driven permanently low) */
    _i2c_clear(&_i2c_bus[dev]);

    return;
}

int i2c_acquire(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_lock(&_i2c_bus[dev].lock);
    return 0;
}

void i2c_release(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    mutex_unlock(&_i2c_bus[dev].lock);
}

int IRAM_ATTR i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len, uint8_t flags)
{
    DEBUG("%s: dev=%u addr=%02x data=%p len=%d flags=%01x\n",
          __func__, dev, addr, data, len, flags);

    assert(dev < I2C_NUMOF);

    CHECK_PARAM_RET(len > 0, -EINVAL);
    CHECK_PARAM_RET(data != NULL, -EINVAL);

    _i2c_bus_t* bus = &_i2c_bus[dev];

    int res = 0;

    /* send START condition and address if I2C_NOSTART is not set */
    if (!(flags & I2C_NOSTART)) {

        /* START condition */
        if ((res = _i2c_start_cond(bus)) != 0) {
            return res;
        }

        /* send 10 bit or 7 bit address */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr1 = 0xf0 | (addr & 0x0300) >> 7 | I2C_READ;
            uint8_t addr2 = addr & 0xff;
            /* send address bytes with read flag */
            if ((res = _i2c_write_byte(bus, addr1)) != 0 ||
                (res = _i2c_write_byte(bus, addr2)) != 0) {
                /* abort transfer */
                _i2c_abort(bus, __func__);
                return -ENXIO;
            }
        }
        else {
            /* send address byte with read flag */
            if ((res = _i2c_write_byte(bus, (addr << 1 | I2C_READ))) != 0) {
                /* abort transfer */
                _i2c_abort(bus, __func__);
                return -ENXIO;
            }
        }
    }

    /* receive bytes if send address was successful */
    for (unsigned int i = 0; i < len; i++) {
        if ((res = _i2c_read_byte(bus, &(((uint8_t*)data)[i]), i < len-1)) != 0) {
            /* abort transfer */
            _i2c_abort(bus, __func__);
            return res;
        }
    }

    /* send STOP condition if I2C_NOSTOP flag is not set */
    if (!(flags & I2C_NOSTOP)) {
        res = _i2c_stop_cond(bus);
    }

    return res;
}

int IRAM_ATTR i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len, uint8_t flags)
{
    DEBUG("%s: dev=%u addr=%02x data=%p len=%d flags=%01x\n",
          __func__, dev, addr, data, len, flags);

    assert(dev < I2C_NUMOF);

    CHECK_PARAM_RET(len > 0, -EINVAL);
    CHECK_PARAM_RET(data != NULL, -EINVAL);

    _i2c_bus_t* bus = &_i2c_bus[dev];

    int res = 0;

    /*  if I2C_NOSTART is not set, send START condition and ADDR */
    if (!(flags & I2C_NOSTART)) {

        /* START condition */
        if ((res = _i2c_start_cond(bus)) != 0) {
            return res;
        }

        /* send 10 bit or 7 bit address */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr1 = 0xf0 | (addr & 0x0300) >> 7;
            uint8_t addr2 = addr & 0xff;
            /* send address bytes without read flag */
            if ((res = _i2c_write_byte(bus, addr1)) != 0 ||
                (res = _i2c_write_byte(bus, addr2)) != 0) {
                /* abort transfer */
                _i2c_abort(bus, __func__);
                return -ENXIO;
            }
        }
        else {
            /* send address byte without read flag */
            if ((res = _i2c_write_byte(bus, addr << 1)) != 0) {
                /* abort transfer */
                _i2c_abort(bus, __func__);
                return -ENXIO;
            }
        }
    }

    /* send bytes if send address was successful */
    for (unsigned int i = 0; i < len; i++) {
        if ((res = _i2c_write_byte(bus, ((uint8_t*)data)[i])) != 0) {
            /* abort transfer */
            _i2c_abort(bus, __func__);
            return res;
        }
    }

    /* send STOP condition if I2C_NOSTOP flag is not set */
    if (!(flags & I2C_NOSTOP)) {
        res = _i2c_stop_cond(bus);
    }

    return res;
}

void i2c_poweron(i2c_t dev)
{
    /* since I2C is realized in software there is no device to power on */
    /* just return */
}

void i2c_poweroff(i2c_t dev)
{
    /* since I2C is realized in software there is no device to power off */
    /* just return */
}

/* --- internal functions --- */

static inline void _i2c_delay(_i2c_bus_t* bus)
{
    /* produces a delay */
    uint32_t cycles = bus->delay;
    if (cycles) {
        __asm__ volatile ("1: _addi.n  %0, %0, -1 \n"
                          "   bnez     %0, 1b     \n" : "=r" (cycles) : "0" (cycles));
    }
}

/*
 * Please note: SDA and SDL pins are used in GPIO_OD_PU mode
 *              (open-drain with pull-ups).
 *
 * Setting a pin which is in open-drain mode leaves the pin floating and
 * the signal is pulled up to high. The signal can then be actively driven
 * to low by a slave. A read operation returns the current signal at the pin.
 *
 * Clearing a pin which is in open-drain mode actively drives the signal to
 * low.
 */

static inline bool _i2c_scl_read(_i2c_bus_t* bus)
{
    /* read SCL status (pin is in open-drain mode and set) */
#ifdef MCU_ESP32
    return GPIO_GET(in, in1, bus->scl);
#else /* MCU_ESP32 */
    return GPIO.IN & bus->scl_bit;
#endif /* MCU_ESP32 */
}

static inline bool _i2c_sda_read(_i2c_bus_t* bus)
{
    /* read SDA status (pin is in open-drain mode and set) */
#ifdef MCU_ESP32
    return GPIO_GET(in, in1, bus->sda);
#else /* MCU_ESP32 */
    return GPIO.IN & bus->sda_bit;
#endif /* MCU_ESP32 */
}

static inline void _i2c_scl_high(_i2c_bus_t* bus)
{
#ifdef MCU_ESP32
    /* set SCL signal high (pin is in open-drain mode and pulled-up) */
    GPIO_SET(out_w1ts, out1_w1ts, bus->scl);
#else /* MCU_ESP32 */
#if I2C_CLOCK_STRETCH > 0
    /*
     * set SCL signal high (switch back to GPIO_IN_PU mode, that is the pin is
     * in open-drain mode and pulled-up to high)
     */
    GPIO.ENABLE_OUT_CLEAR = bus->scl_bit;
#else /* I2C_CLOCK_STRETCH > 0 */
    /* No clock stretching supported, always drive the SCL pin. */
    GPIO.OUT_SET = bus->scl_bit;
#endif /* I2C_CLOCK_STRETCH > 0 */
#endif /* MCU_ESP32 */
}

static inline void _i2c_scl_low(_i2c_bus_t* bus)
{
#ifdef MCU_ESP32
    /* set SCL signal low (actively driven to low) */
    GPIO_SET(out_w1tc, out1_w1tc, bus->scl);
#else /* MCU_ESP32 */
#if I2C_CLOCK_STRETCH > 0
    /*
     * set SCL signal low (switch temporarily to GPIO_OD_PU where the
     * written output value 0 drives the pin actively to low)
     */
    GPIO.ENABLE_OUT_SET = bus->scl_bit;
#else /* I2C_CLOCK_STRETCH > 0 */
    /* No clock stretching supported, always drive the SCL pin. */
    GPIO.OUT_CLEAR = bus->scl_bit;
#endif /* I2C_CLOCK_STRETCH > 0 */
#endif /* MCU_ESP32 */
}

static inline void _i2c_sda_high(_i2c_bus_t* bus)
{
#ifdef MCU_ESP32
    /* set SDA signal high (pin is in open-drain mode and pulled-up) */
    GPIO_SET(out_w1ts, out1_w1ts, bus->sda);
#else /* MCU_ESP32 */
    /*
     * set SDA signal high (switch back to GPIO_IN_PU mode, that is the pin is
     * in open-drain mode and pulled-up to high)
     */
    GPIO.ENABLE_OUT_CLEAR = bus->sda_bit;
#endif /* MCU_ESP32 */
}

static inline void _i2c_sda_low(_i2c_bus_t* bus)
{
#ifdef MCU_ESP32
    /* set SDA signal low (actively driven to low) */
    GPIO_SET(out_w1tc, out1_w1tc, bus->sda);
#else /* MCU_ESP32 */
    /*
     * set SDA signal low (switch temporarily to GPIO_OD_PU where the
     * written output value 0 drives the pin actively to low)
     */
    GPIO.ENABLE_OUT_SET = bus->sda_bit;
#endif /* MCU_ESP32 */
}

static void _i2c_clear(_i2c_bus_t* bus)
{
    DEBUG("%s: dev=%u\n", __func__, bus->dev);

    /**
     * Sometimes a slave blocks and drives the SDA line permanently low.
     * Send some clock pulses in that case (10 at maximum)
     */

    /*
     * If SDA is low while SCL is high for 10 half cycles, it is not an
     * arbitration lost but a bus lock.
     */
    int count = 10;
    while (!_i2c_sda_read(bus) && _i2c_scl_read(bus) && count) {
        count--;
        _i2c_delay(bus);
    }

    if (count) {
        /* was not a bus lock */
        return;
    }

    /* send 10 clock pulses in case of bus lock */
    count = 10;
    while (!_i2c_sda_read(bus) && count--) {
        _i2c_scl_low(bus);
        _i2c_delay(bus);
        _i2c_scl_high(bus);
        _i2c_delay(bus);
    }
}

static void _i2c_abort(_i2c_bus_t* bus, const char* func)
{
    DEBUG("%s: dev=%u\n", func, bus->dev);

    /* reset SCL and SDA to passive HIGH (floating and pulled-up) */
    _i2c_sda_high(bus);
    _i2c_scl_high(bus);

    /* reset repeated start indicator */
    bus->started = false;

    /* clear the bus if necessary (SDA is driven permanently low) */
    _i2c_clear(bus);
}

static IRAM_ATTR int _i2c_arbitration_lost(_i2c_bus_t* bus, const char* func)
{
    DEBUG("%s: arbitration lost dev=%u\n", func, bus->dev);

    /* reset SCL and SDA to passive HIGH (floating and pulled-up) */
    _i2c_sda_high(bus);
    _i2c_scl_high(bus);

    /* reset repeated start indicator */
    bus->started = false;

    /* clear the bus if necessary (SDA is driven permanently low) */
    _i2c_clear(bus);

    return -EAGAIN;
}

static IRAM_ATTR int _i2c_start_cond(_i2c_bus_t* bus)
{
    /*
     * send start condition
     * on entry: SDA and SCL are set to be floating and pulled-up to high
     * on exit : SDA and SCL are actively driven to low
     */

    int res = 0;

    if (bus->started) {
        /* prepare the repeated start condition */

        /* SDA = passive HIGH (floating and pulled-up) */
        _i2c_sda_high(bus);

        /* t_VD;DAT not necessary */
        /* _i2c_delay(bus); */

        /* SCL = passive HIGH (floating and pulled-up) */
        _i2c_scl_high(bus);

        /* clock stretching, wait as long as clock is driven to low by the slave */
        uint32_t stretch = I2C_CLOCK_STRETCH;
        while (stretch && !_i2c_scl_read(bus)) {
            stretch--;
        }
        if (stretch == 0) {
            DEBUG("%s: clock stretching timeout dev=%u\n", __func__, bus->dev);
            res = -ETIMEDOUT;
        }

        /* wait t_SU;STA - set-up time for a repeated START condition */
        /* min. in us: 4.7 (SM), 0.6 (FM), 0.26 (FPM), 0.16 (HSM); no max. */
        _i2c_delay(bus);
    }

    /* if SDA is low, arbitration is lost and someone else is driving the bus */
    if (!_i2c_sda_read(bus)) {
        return _i2c_arbitration_lost(bus, __func__);
    }

    /* begin the START condition: SDA = active LOW */
    _i2c_sda_low(bus);

    /* wait t_HD;STA - hold time (repeated) START condition, */
    /* max none */
    /* min 4.0 us (SM), 0.6 us (FM), 0.26 us (FPM), 0.16 us (HSM) */
    _i2c_delay(bus);

    /* complete the START condition: SCL = active LOW */
    _i2c_scl_low(bus);

    /* needed for repeated start condition */
    bus->started = true;

    return res;
}

static IRAM_ATTR int _i2c_stop_cond(_i2c_bus_t* bus)
{
    /*
     * send stop condition
     * on entry: SCL is active low and SDA can be changed
     * on exit : SCL and SDA are set to be floating and pulled-up to high
     */

    int res = 0;

    /* begin the STOP condition: SDA = active LOW */
    _i2c_sda_low(bus);

    /* wait t_LOW - LOW period of SCL clock */
    /* min. in us: 4.7 (SM), 1.3 (FM), 0.5 (FPM), 0.16 (HSM); no max. */
    _i2c_delay(bus);

    /* SCL = passive HIGH (floating and pulled up) while SDA = active LOW */
    _i2c_scl_high(bus);

    /* clock stretching, wait as long as clock is driven to low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (stretch && !_i2c_scl_read(bus)) {
        stretch--;
    }
    if (stretch == 0) {
        DEBUG("%s: clock stretching timeout dev=%u\n", __func__, bus->dev);
        res = -ETIMEDOUT;
    }

    /* wait t_SU;STO - hold time STOP condition, */
    /* min. in us: 4.0 (SM), 0.6 (FM), 0.26 (FPM), 0.16 (HSM); no max. */
    _i2c_delay(bus);

    /* complete the STOP condition: SDA = passive HIGH (floating and pulled up) */
    _i2c_sda_high(bus);

    /* reset repeated start indicator */
    bus->started = false;

    /* wait t_BUF - bus free time between a STOP and a START condition */
    /* min. in us: 4.7 (SM), 1.3 (FM), 0.5 (FPM), 0.16 (HSM); no max. */
    _i2c_delay(bus);
    /* one additional delay */
    _i2c_delay(bus);

    /* if SDA is low, arbitration is lost and someone else is driving the bus */
    if (_i2c_sda_read(bus) == 0) {
        return _i2c_arbitration_lost(bus, __func__);
    }

    return res;
}

static IRAM_ATTR int _i2c_write_bit(_i2c_bus_t* bus, bool bit)
{
    /*
     * send one bit
     * on entry: SCL is active low, SDA can be changed
     * on exit : SCL is active low, SDA can be changed
     */

    int res = 0;

    /* SDA = bit */
    if (bit) {
        _i2c_sda_high(bus);
    }
    else {
        _i2c_sda_low(bus);
    }

    /* wait t_VD;DAT - data valid time (time until data are valid) */
    /* max. in us: 3.45 (SM), 0.9 (FM), 0.45 (FPM); no min */
    _i2c_delay(bus);

    /* SCL = passive HIGH (floating and pulled-up), SDA value is available */
    _i2c_scl_high(bus);

    /* wait t_HIGH - time for the slave to read SDA */
    /* min. in us: 4 (SM), 0.6 (FM), 0.26 (FPM), 0.09 (HSM); no max. */
    _i2c_delay(bus);

    /* clock stretching, wait as long as clock is driven low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (stretch && !_i2c_scl_read(bus)) {
        stretch--;
    }
    if (stretch == 0) {
        DEBUG("%s: clock stretching timeout dev=%u\n", __func__, bus->dev);
        res = -ETIMEDOUT;
    }

    /* if SCL is high, now data is valid */
    /* if SDA is high, check that nobody else is driving SDA low */
    if (bit && !_i2c_sda_read(bus)) {
        return _i2c_arbitration_lost(bus, __func__);
    }

    /* SCL = active LOW to allow next SDA change */
    _i2c_scl_low(bus);

    return res;
}

static IRAM_ATTR int _i2c_read_bit(_i2c_bus_t* bus, bool* bit)
{
    /* read one bit
     * on entry: SCL is active low, SDA can be changed
     * on exit : SCL is active low, SDA can be changed
     */

    int res = 0;

    /* SDA = passive HIGH (floating and pulled-up) to let the slave drive data */
    _i2c_sda_high(bus);

    /* wait t_VD;DAT - data valid time (time until data are valid) */
    /* max. in us: 3.45 (SM), 0.9 (FM), 0.45 (FPM); no min */
    _i2c_delay(bus);

    /* SCL = passive HIGH (floating and pulled-up), SDA value is available */
    _i2c_scl_high(bus);

    /* clock stretching, wait as long as clock is driven to low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (stretch && !_i2c_scl_read(bus)) {
        stretch--;
    }
    if (stretch == 0) {
        DEBUG("%s: clock stretching timeout dev=%u\n", __func__, bus->dev);
        res = -ETIMEDOUT;
    }

    /* wait t_HIGH - time for the slave to read SDA */
    /* min. in us: 4 (SM), 0.6 (FM), 0.26 (FPM), 0.09 (HSM); no max. */
    _i2c_delay(bus);

    /* SCL is high, read out bit */
    *bit = _i2c_sda_read(bus);

    /* SCL = active LOW to allow next SDA change */
    _i2c_scl_low(bus);

    return res;
}

static IRAM_ATTR int _i2c_write_byte(_i2c_bus_t* bus, uint8_t byte)
{
    /* send one byte and returns 0 in case of ACK from slave */

    /* send the byte from MSB to LSB */
    for (unsigned i = 0; i < 8; i++) {
        int res = _i2c_write_bit(bus, (byte & 0x80) != 0);
        if (res != 0) {
            return res;
        }
        byte = byte << 1;
    }

    /* read acknowledge bit (low) from slave */
    bool bit;
    int res = _i2c_read_bit(bus, &bit);
    if (res != 0) {
        return res;
    }

    return !bit ? 0 : -EIO;
}


static IRAM_ATTR int _i2c_read_byte(_i2c_bus_t* bus, uint8_t *byte, bool ack)
{
    bool bit;

    /* read the byte */
    for (unsigned i = 0; i < 8; i++) {
        int res = _i2c_read_bit(bus, &bit);
        if (res != 0) {
            return res;
        }
        *byte = (*byte << 1) | (bit ? 1 : 0);
    }

    /* write acknowledgement flag */
    _i2c_write_bit(bus, !ack);

    return 0;
}

void i2c_print_config(void)
{
    for (unsigned dev = 0; dev < I2C_NUMOF; dev++) {
        printf("\tI2C_DEV(%u)\tscl=%d sda=%d\n",
               dev, i2c_config[dev].scl, i2c_config[dev].sda);
    }
}
