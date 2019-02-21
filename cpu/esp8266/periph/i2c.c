/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_esp8266
 * @ingroup drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation using ESP8266 SDK
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

/*
   PLEASE NOTE:

   Some parts of the implementation bases on the bit-banging implementation as
   described in [wikipedia](https://en.wikipedia.org/wiki/I%C2%B2C) as well as
   its implementation in [esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos.git).
   These parts are under the copyright of their respective owners.
*/

#define ENABLE_DEBUG (0)
#include "debug.h"

#include <stdbool.h>
#include <errno.h>

#include "cpu.h"
#include "log.h"
#include "mutex.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/i2c.h"

#include "common.h"

#include "esp/gpio_regs.h"
#include "sdk/ets.h"

#if defined(I2C_NUMOF) && I2C_NUMOF > 0

/* has to be declared as extern since it is not possible to include */
/* user_interface.h due to conflicts with gpio_init */
extern uint8_t system_get_cpu_freq(void);
extern bool system_update_cpu_freq(uint8_t freq);

/* max clock stretching counter (ca. 10 ms) */
#define I2C_CLOCK_STRETCH 40000

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

} _i2c_bus_t;

static _i2c_bus_t _i2c_bus[] =
{
  #if defined(I2C0_SDA) && defined(I2C0_SCL)
  {
    .speed = I2C0_SPEED,
    .sda = I2C0_SDA,
    .scl = I2C0_SCL
  },
  #endif
  #if defined(I2C1_SDA) && defined(I2C1_SCL)
  {
    .speed = I2C1_SPEED,
    .sda = I2C1_SDA,
    .scl = I2C1_SCL
  },
  #endif
  #if defined(I2C2_SDA) && defined(I2C2_SCL)
  {
    .speed = I2C2_SPEED,
    .sda = I2C2_SDA,
    .scl = I2C2_SCL
  },
  #endif
};

/* to ensure that I2C is always optimized with -O2 to use the defined delays */
#pragma GCC optimize ("O2")

static const uint32_t _i2c_delays[][2] =
{
    /* values specify one half-period and are only valid for -O2 option */
    /* value = [period - 0.5us(160MHz) or 1.0us(80MHz)] * cycles per second / 2 */
    /* cycles per us = ca. 20 (80 MHz) / ca. 40 (160 MHz) */
    [I2C_SPEED_LOW]       = {1990, 989}, /*   10 kbps (period 100 us) */
    [I2C_SPEED_NORMAL]    = { 190,  89}, /*  100 kbps (period 10 us) */
    [I2C_SPEED_FAST]      = {  40,  16}, /*  400 kbps (period 2.5 us) */
    [I2C_SPEED_FAST_PLUS] = {  13,   0}, /*    1 Mbps (period 1 us) */
    [I2C_SPEED_HIGH]      = {   0,   0}  /*  3.4 Mbps (period 0.3 us) is not working */
};

static mutex_t i2c_bus_lock[I2C_NUMOF] = { MUTEX_INIT };

/* forward declaration of internal functions */

static inline void _i2c_delay (_i2c_bus_t* bus);
static inline bool _i2c_scl_read (_i2c_bus_t* bus);
static inline bool _i2c_sda_read (_i2c_bus_t* bus);
static inline void _i2c_scl_high (_i2c_bus_t* bus);
static inline void _i2c_scl_low (_i2c_bus_t* bus);
static inline void _i2c_sda_high (_i2c_bus_t* bus);
static inline void _i2c_sda_low (_i2c_bus_t* bus);
static int _i2c_start_cond (_i2c_bus_t* bus);
static int _i2c_stop_cond (_i2c_bus_t* bus);
static int _i2c_write_bit (_i2c_bus_t* bus, bool bit);
static int _i2c_read_bit (_i2c_bus_t* bus, bool* bit);
static int _i2c_write_byte (_i2c_bus_t* bus, uint8_t byte);
static int _i2c_read_byte (_i2c_bus_t* bus, uint8_t* byte, bool ack);
static int _i2c_arbitration_lost (_i2c_bus_t* bus, const char* func);
static void _i2c_abort (_i2c_bus_t* bus, const char* func);
static void _i2c_clear (_i2c_bus_t* bus);

/* implementation of i2c interface */
void i2c_init(i2c_t dev)
{
    if (I2C_NUMOF != sizeof(_i2c_bus)/sizeof(_i2c_bus_t)) {
        LOG_INFO("I2C_NUMOF does not match number of I2C_SDA_x/I2C_SCL_x definitions\n");
        LOG_INFO("Please check your board configuration in 'board.h'\n");
        assert(I2C_NUMOF < sizeof(_i2c_bus)/sizeof(_i2c_bus_t));

        return;
    }

    CHECK_PARAM (dev < I2C_NUMOF)

    if (_i2c_bus[dev].speed == I2C_SPEED_HIGH) {
        LOG_INFO("I2C_SPEED_HIGH is not supported\n");
        return;
    }

    i2c_acquire (dev);

    _i2c_bus[dev].dev     = dev;
    _i2c_bus[dev].delay   =_i2c_delays[_i2c_bus[dev].speed][ets_get_cpu_frequency() == 80 ? 1 : 0];
    _i2c_bus[dev].scl_bit = BIT(_i2c_bus[dev].scl); /* store bit mask for faster access */
    _i2c_bus[dev].sda_bit = BIT(_i2c_bus[dev].sda); /* store bit mask for faster access */
    _i2c_bus[dev].started = false; /* for handling of repeated start condition */

    DEBUG ("%s: scl=%d sda=%d speed=%d\n", __func__,
           _i2c_bus[dev].scl, _i2c_bus[dev].sda, _i2c_bus[dev].speed);

    /*
     * Configure and initialize SDA and SCL pin.
     * Note: Due to critical timing required by the I2C software
     * implementation, the ESP8266 GPIOs can not be used directly in GPIO_OD_PU
     * mode. Instead, the GPIOs are configured in GPIO_IN_PU mode with
     * open-drain output driver. Signal levels are then realized as following:
     *
     * - HIGH: The GPIO is used in the configured GPIO_IN_PU mode. In this
     *         mode, the output driver is in open-drain mode and pulled-up.
     * - LOW : The GPIO is temporarily switched to GPIO_OD_PU mode. In this
     *         mode, the output value 0, which is written during
     *         initialization, actively drives the output to low.
     */
    gpio_init (_i2c_bus[dev].scl, GPIO_IN_PU);
    gpio_init (_i2c_bus[dev].sda, GPIO_IN_PU);
    gpio_clear (_i2c_bus[dev].scl);
    gpio_clear (_i2c_bus[dev].sda);

    /* set SDA and SCL to be floating and pulled-up to high */
    _i2c_sda_high (&_i2c_bus[dev]);
    _i2c_scl_high (&_i2c_bus[dev]);

    /* clear the bus if necessary (SDA is driven permanently low) */
    _i2c_clear (&_i2c_bus[dev]);

    i2c_release (dev);

    return;
}

int i2c_acquire(i2c_t dev)
{
    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    mutex_lock(&i2c_bus_lock[dev]);
    return 0;
}

int i2c_release(i2c_t dev)
{
    CHECK_PARAM_RET (dev < I2C_NUMOF, -1)

    mutex_unlock(&i2c_bus_lock[dev]);
    return 0;
}

int /* IRAM */ i2c_read_bytes(i2c_t dev, uint16_t addr, void *data, size_t len, uint8_t flags)
{
    DEBUG ("%s: dev=%u addr=%02x data=%p len=%d flags=%01x\n",
           __func__, dev, addr, data, len, flags);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -EINVAL);
    CHECK_PARAM_RET (len > 0, -EINVAL);
    CHECK_PARAM_RET (data != NULL, -EINVAL);

    _i2c_bus_t* bus = &_i2c_bus[dev];

    int res = 0;

    /* send START condition and address if I2C_NOSTART is not set */
    if (!(flags & I2C_NOSTART)) {

        /* START condition */
        if ((res = _i2c_start_cond (bus)) != 0) {
            return res;
        }

        /* send 10 bit or 7 bit address */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr1 = 0xf0 | (addr & 0x0300) >> 7 | I2C_READ;
            uint8_t addr2 = addr & 0xff;
            /* send address bytes wit read flag */
            if ((res = _i2c_write_byte (bus, addr1)) != 0 ||
                (res = _i2c_write_byte (bus, addr2)) != 0) {
                /* abort transfer */
                _i2c_abort (bus, __func__);
                return res;
            }
        }
        else {
            /* send address byte with read flag */
            if ((res = _i2c_write_byte (bus, (addr << 1 | I2C_READ))) != 0) {
                /* abort transfer */
                _i2c_abort (bus, __func__);
                return res;
            }
        }
    }

    /* receive bytes if send address was successful */
    for (unsigned int i = 0; i < len; i++) {
        if ((res = _i2c_read_byte (bus, &(((uint8_t*)data)[i]), i < len-1)) != 0) {
            /* abort transfer */
            _i2c_abort (bus, __func__);
            return res;
        }
    }

    /* send STOP condition if I2C_NOSTOP flag is not set */
    if (!(flags & I2C_NOSTOP)) {
        res = _i2c_stop_cond (bus);
    }

    return res;
}

int /* IRAM */ i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data, size_t len, uint8_t flags)
{
    DEBUG ("%s: dev=%u addr=%02x data=%p len=%d flags=%01x\n",
           __func__, dev, addr, data, len, flags);

    CHECK_PARAM_RET (dev < I2C_NUMOF, -EINVAL);
    CHECK_PARAM_RET (len > 0, -EINVAL);
    CHECK_PARAM_RET (data != NULL, -EINVAL);

    _i2c_bus_t* bus = &_i2c_bus[dev];

    int res = 0;

    /*  if I2C_NOSTART is not set, send START condition and ADDR */
    if (!(flags & I2C_NOSTART)) {

        /* START condition */
        if ((res = _i2c_start_cond (bus)) != 0) {
            return res;
        }

        /* send 10 bit or 7 bit address */
        if (flags & I2C_ADDR10) {
            /* prepare 10 bit address bytes */
            uint8_t addr1 = 0xf0 | (addr & 0x0300) >> 7;
            uint8_t addr2 = addr & 0xff;
            /* send address bytes without read flag */
            if ((res = _i2c_write_byte (bus, addr1)) != 0 ||
                (res = _i2c_write_byte (bus, addr2)) != 0) {
                /* abort transfer */
                _i2c_abort (bus, __func__);
                return res;
            }
        }
        else {
            /* send address byte without read flag */
            if ((res = _i2c_write_byte (bus, addr << 1)) != 0) {
                /* abort transfer */
                _i2c_abort (bus, __func__);
                return res;
            }
        }
    }

    /* send bytes if send address was successful */
    for (unsigned int i = 0; i < len; i++) {
        if ((res = _i2c_write_byte (bus, ((uint8_t*)data)[i])) != 0) {
            /* abort transfer */
            _i2c_abort (bus, __func__);
            return res;
        }
    }

    /* send STOP condition if I2C_NOSTOP flag is not set */
    if (!(flags & I2C_NOSTOP)) {
        res = _i2c_stop_cond (bus);
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

static inline void _i2c_delay (_i2c_bus_t* bus)
{
    /* produces a delay */
    /* ca. 20 cycles = 1 us (80 MHz) or ca. 40 cycles = 1 us (160 MHz) */

    uint32_t cycles = bus->delay;
    if (cycles) {
        __asm__ volatile ("1:  _addi.n  %0, %0, -1 \n"
                          "    bnez     %0, 1b     \n" : "=r" (cycles) : "0" (cycles));
    }
}

/*
 * Note: Due to critical timing required by the I2C software implementation,
 * the ESP8266 GPIOs can not be used directly in GPIO_OD_PU mode. Instead,
 * the GPIOs are configured in GPIO_IN_PU mode with open-drain output driver.
 * Signal levels are then realized as following:
 *
 * - HIGH: The GPIO is used in the configured GPIO_IN_PU mode. In this mode,
 *         the output driver is in open-drain mode and pulled-up.
 * - LOW : The GPIO is temporarily switched to GPIO_OD_PU mode. In this mode,
 *         the output value 0, which is written during initialization,
 *         actively drives the output to low.
 */

static inline bool _i2c_scl_read(_i2c_bus_t* bus)
{
    /* read SCL status */
    return GPIO.IN & bus->scl_bit;
}

static inline bool _i2c_sda_read(_i2c_bus_t* bus)
{
    /* read SDA status */
    return GPIO.IN & bus->sda_bit;
}

static inline void _i2c_scl_low(_i2c_bus_t* bus)
{
    /*
     * set SCL signal low (switch temporarily to GPIO_OD_PU where the
     * written output value 0 drives the pin actively to low)
     */
    GPIO.ENABLE_OUT_SET = bus->scl_bit;
}

static inline void _i2c_scl_high(_i2c_bus_t* bus)
{
    /*
     * set SCL signal high (switch back to GPIO_IN_PU mode, that is the pin is
     * in open-drain mode and pulled-up to high)
     */
    GPIO.ENABLE_OUT_CLEAR = bus->scl_bit;
}

static inline void _i2c_sda_low(_i2c_bus_t* bus)
{
    /*
     * set SDA signal low (switch temporarily to GPIO_OD_PU where the
     * written output value 0 drives the pin actively to low)
     */
    GPIO.ENABLE_OUT_SET = bus->sda_bit;
}

static inline void _i2c_sda_high(_i2c_bus_t* bus)
{
    /*
     * set SDA signal high (switch back to GPIO_IN_PU mode, that is the pin is
     * in open-drain mode and pulled-up to high)
     */
    GPIO.ENABLE_OUT_CLEAR = bus->sda_bit;
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
    while (!_i2c_sda_read (bus) && _i2c_scl_read (bus) && count) {
        count--;
        _i2c_delay (bus);
    }

    if (count) {
        /* was not a bus lock */
        return;
    }

    /* send 10 clock pulses in case of bus lock */
    count = 10;
    while (!_i2c_sda_read (bus) && count--) {
        _i2c_scl_low (bus);
        _i2c_delay (bus);
        _i2c_scl_high (bus);
        _i2c_delay (bus);
    }
}

static void _i2c_abort(_i2c_bus_t* bus, const char* func)
{
    DEBUG("%s: dev=%u\n", func, bus->dev);

    /* reset SCL and SDA to passive HIGH (floating and pulled-up) */
    _i2c_sda_high (bus);
    _i2c_scl_high (bus);

    /* reset repeated start indicator */
    bus->started = false;

    /* clear the bus if necessary (SDA is driven permanently low) */
    _i2c_clear(bus);
}

static /* IRAM */ int _i2c_arbitration_lost (_i2c_bus_t* bus, const char* func)
{
    DEBUG("%s: arbitration lost dev=%u\n", func, bus->dev);

    /* reset SCL and SDA to passive HIGH (floating and pulled-up) */
    _i2c_sda_high (bus);
    _i2c_scl_high (bus);

    /* reset repeated start indicator */
    bus->started = false;

    /* clear the bus if necessary (SDA is driven permanently low) */
    _i2c_clear(bus);

    return -EAGAIN;
}

static /* IRAM */ int _i2c_start_cond(_i2c_bus_t* bus)
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
        _i2c_sda_high (bus);

        /* t_VD;DAT not neccessary */
        /* _i2c_delay (bus); */

        /* SCL = passive HIGH (floating and pulled-up) */
        _i2c_scl_high (bus);

        /* clock stretching, wait as long as clock is driven to low by the slave */
        uint32_t stretch = I2C_CLOCK_STRETCH;
        while (stretch && !_i2c_scl_read (bus)) {
            stretch--;
        }
        if (stretch == 0) {
            DEBUG("%s: clock stretching timeout dev=%u\n", __func__, bus->dev);
            res = -ETIMEDOUT;
        }

        /* wait t_SU;STA - set-up time for a repeated START condition */
        /* min. in us: 4.7 (SM), 0.6 (FM), 0.26 (FPM), 0.16 (HSM); no max. */
        _i2c_delay (bus);
    }

    /* if SDA is low, arbitration is lost and someone else is driving the bus */
    if (!_i2c_sda_read (bus)) {
        return _i2c_arbitration_lost (bus, __func__);
    }

    /* begin the START condition: SDA = active LOW */
    _i2c_sda_low (bus);

    /* wait t_HD;STA - hold time (repeated) START condition, */
    /* max none */
    /* min 4.0 us (SM), 0.6 us (FM), 0.26 us (FPM), 0.16 us (HSM) */
    _i2c_delay (bus);

    /* complete the START condition: SCL = active LOW */
    _i2c_scl_low (bus);

    /* needed for repeated start condition */
    bus->started = true;

    return res;
}

static /* IRAM */ int _i2c_stop_cond(_i2c_bus_t* bus)
{
    /*
     * send stop condition
     * on entry: SCL is active low and SDA can be changed
     * on exit : SCL and SDA are set to be floating and pulled-up to high
     */

    int res = 0;

    /* begin the STOP condition: SDA = active LOW */
    _i2c_sda_low (bus);

    /* wait t_LOW - LOW period of SCL clock */
    /* min. in us: 4.7 (SM), 1.3 (FM), 0.5 (FPM), 0.16 (HSM); no max. */
    _i2c_delay (bus);

    /* SCL = passive HIGH (floating and pulled up) while SDA = active LOW */
    _i2c_scl_high (bus);

    /* clock stretching, wait as long as clock is driven to low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (stretch && !_i2c_scl_read (bus)) {
        stretch--;
    }
    if (stretch == 0) {
        DEBUG("%s: clock stretching timeout dev=%u\n", __func__, bus->dev);
        res = -ETIMEDOUT;
    }

    /* wait t_SU;STO - hold time STOP condition, */
    /* min. in us: 4.0 (SM), 0.6 (FM), 0.26 (FPM), 0.16 (HSM); no max. */
    _i2c_delay (bus);

    /* complete the STOP condition: SDA = passive HIGH (floating and pulled up) */
    _i2c_sda_high (bus);

    /* reset repeated start indicator */
    bus->started = false;

    /* wait t_BUF - bus free time between a STOP and a START condition */
    /* min. in us: 4.7 (SM), 1.3 (FM), 0.5 (FPM), 0.16 (HSM); no max. */
    _i2c_delay (bus);
    /* one additional delay */
    _i2c_delay (bus);

    /* if SDA is low, arbitration is lost and someone else is driving the bus */
    if (_i2c_sda_read (bus) == 0) {
        return _i2c_arbitration_lost (bus, __func__);
    }

    return res;
}

static /* IRAM */ int _i2c_write_bit (_i2c_bus_t* bus, bool bit)
{
    /*
     * send one bit
     * on entry: SCL is active low, SDA can be changed
     * on exit : SCL is active low, SDA can be changed
     */

    int res = 0;

    /* SDA = bit */
    if (bit) {
        _i2c_sda_high (bus);
    }
    else {
        _i2c_sda_low (bus);
    }

    /* wait t_VD;DAT - data valid time (time until data are valid) */
    /* max. in us: 3.45 (SM), 0.9 (FM), 0.45 (FPM); no min */
    _i2c_delay (bus);

    /* SCL = passive HIGH (floating and pulled-up), SDA value is available */
    _i2c_scl_high (bus);

    /* wait t_HIGH - time for the slave to read SDA */
    /* min. in us: 4 (SM), 0.6 (FM), 0.26 (FPM), 0.09 (HSM); no max. */
    _i2c_delay (bus);

    /* clock stretching, wait as long as clock is driven low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (stretch && !_i2c_scl_read (bus)) {
        stretch--;
    }
    if (stretch == 0) {
        DEBUG("%s: clock stretching timeout dev=%u\n", __func__, bus->dev);
        res = -ETIMEDOUT;
    }

    /* if SCL is high, now data is valid */
    /* if SDA is high, check that nobody else is driving SDA low */
    if (bit && !_i2c_sda_read(bus)) {
        return _i2c_arbitration_lost (bus, __func__);
    }

    /* SCL = active LOW to allow next SDA change */
    _i2c_scl_low(bus);

    return res;
}

static /* IRAM */ int _i2c_read_bit (_i2c_bus_t* bus, bool* bit)
{
    /* read one bit
     * on entry: SCL is active low, SDA can be changed
     * on exit : SCL is active low, SDA can be changed
     */

    int res = 0;

    /* SDA = passive HIGH (floating and pulled-up) to let the slave drive data */
    _i2c_sda_high (bus);

    /* wait t_VD;DAT - data valid time (time until data are valid) */
    /* max. in us: 3.45 (SM), 0.9 (FM), 0.45 (FPM); no min */
    _i2c_delay (bus);

    /* SCL = passive HIGH (floating and pulled-up), SDA value is available */
    _i2c_scl_high (bus);

    /* clock stretching, wait as long as clock is driven to low by the slave */
    uint32_t stretch = I2C_CLOCK_STRETCH;
    while (stretch && !_i2c_scl_read (bus)) {
        stretch--;
    }
    if (stretch == 0) {
        DEBUG("%s: clock stretching timeout dev=%u\n", __func__, bus->dev);
        res = -ETIMEDOUT;
    }

    /* wait t_HIGH - time for the slave to read SDA */
    /* min. in us: 4 (SM), 0.6 (FM), 0.26 (FPM), 0.09 (HSM); no max. */
    _i2c_delay (bus);

    /* SCL is high, read out bit */
    *bit = _i2c_sda_read (bus);

    /* SCL = active LOW to allow next SDA change */
    _i2c_scl_low(bus);

    return res;
}

static /* IRAM */ int _i2c_write_byte (_i2c_bus_t* bus, uint8_t byte)
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
    int res = _i2c_read_bit (bus, &bit);
    if (res != 0) {
        return res;
    }

    return !bit ? 0 : -EIO;
}


static /* IRAM */ int _i2c_read_byte(_i2c_bus_t* bus, uint8_t *byte, bool ack)
{
    bool bit;

    /* read the byte */
    for (unsigned i = 0; i < 8; i++) {
        int res = _i2c_read_bit (bus, &bit);
        if (res != 0) {
            return res;
        }
        *byte = (*byte << 1) | bit;
    }

    /* write acknowledgement flag */
    _i2c_write_bit(bus, !ack);

    return 0;
}

void i2c_print_config(void)
{
    for (unsigned bus = 0; bus < I2C_NUMOF; bus++) {
        LOG_INFO("\tI2C_DEV(%d): scl=%d sda=%d\n",
                 bus, _i2c_bus[bus].scl, _i2c_bus[bus].sda);
    }
}

#else /* if defined(I2C_NUMOF) && I2C_NUMOF */

void i2c_print_config(void)
{
    LOG_INFO("\tI2C: no devices\n");
}

#endif /* if defined(I2C_NUMOF) && I2C_NUMOF */
