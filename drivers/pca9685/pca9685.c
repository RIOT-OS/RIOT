/*
 * Copyright (C) 2019 Marius <marius@twostairs.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca9685
 * @{
 *
 * @file
 * @brief       Device driver implementation for the PCA9685 16-channel, 12-bit PWM Fm+ I²C-bus controller.
 * Implementation based on https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
 *
 * @author      Marius <marius@twostairs.com>
 *
 * @}
 */

#include <math.h>

#include "pca9685.h"

#include "xtimer.h"
#include "periph_conf.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define DEV_I2C      (dev->params.i2c_dev)
#define DEV_ADDR     (dev->params.i2c_addr)

int pca9685_init(pca9685_t *dev, const pca9685_params_t *params) {
    dev->params = *params;

    DEBUG("Acquiring I2C ...\n");
    i2c_acquire(DEV_I2C);

    DEBUG("Reading reg ...\n");
    uint8_t oldmode;
    if(i2c_read_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, &oldmode, 0) != 0) {
        DEBUG("Failed ...\n");
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_NODEV;
    }

    DEBUG("Releasing I2C ...\n");
    i2c_release(DEV_I2C);

    DEBUG("Setting PWM ...\n");
    int setfreq = pca9685_set_pwm_freq(dev, dev->params.pwm_freq);
    if(setfreq != PCA9685_OK) {
        return setfreq;
    }

    return PCA9685_OK;
}

int pca9685_set_ext_clk(pca9685_t *dev, uint8_t prescale) {

    i2c_acquire(DEV_I2C);

    uint8_t oldmode;
    if(i2c_read_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, &oldmode, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETEXTCLK;
    }
    uint8_t newmode = (oldmode & 0x7F) | 0x10; // sleep
    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, newmode, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETEXTCLK;
    }

    // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
    // use the external clock.
    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, (newmode|=0x40), 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETEXTCLK;
    }

    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_PRESCALE, prescale, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETEXTCLK;
    }

    xtimer_usleep(5);
    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, (newmode & ~(0x10)) | 0xa0, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETEXTCLK;
    }

    i2c_release(DEV_I2C);

    return PCA9685_OK;
}

int pca9685_set_pwm_freq(pca9685_t *dev, float freq) {
    freq *= 0.9; //Correct for overshoot in the frequency setting (see issue https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library/issues/11)
    float prescaleval = 25000000;
    prescaleval /= 4096;
    prescaleval /= freq;
    prescaleval -= 1;
    uint8_t prescale = floor(prescaleval + 0.5);

    i2c_acquire(DEV_I2C);

    uint8_t oldmode;
    if(i2c_read_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, &oldmode, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETFREQ;
    }

    uint8_t newmode = (oldmode & 0x7F) | 0x10;
    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, newmode, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETFREQ;
    }

    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_PRESCALE, prescale, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETFREQ;
    }
    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, oldmode, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETFREQ;
    }

    xtimer_usleep(5);
    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, oldmode|0xa0, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETFREQ;
    }

    i2c_release(DEV_I2C);
    return PCA9685_OK;
}

int pca9685_set_output_mode(pca9685_t *dev, bool totempole) {
    uint8_t oldmode;

    i2c_acquire(DEV_I2C);

    if(i2c_read_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE2, &oldmode, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETFREQ;
    }

    uint8_t newmode;
    if (totempole) {
        newmode = (oldmode&0x7F) | 0x04;
    }
    else {
        newmode = (oldmode&0x7F) & ~0x04;
    }

    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE2, newmode, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SETFREQ;
    }

    i2c_release(DEV_I2C);
    return PCA9685_OK;
}

int pca9685_set_pwm(pca9685_t *dev, uint8_t num, uint16_t on, uint16_t off, bool acquire, bool keep_acquired) {
    uint8_t onoff[4];
    onoff[0] = (uint8_t)on;
    onoff[1] = (uint8_t)((uint16_t)on >> 8);
    onoff[2] = (uint8_t)off;
    onoff[3] = (uint8_t)((uint16_t)off >> 8);

    if(acquire == true) {
        i2c_acquire(DEV_I2C);
    }

    if(i2c_write_regs(DEV_I2C, DEV_ADDR, LED0_ON_L + 4 * num, &onoff, 4, 0) != 0) {
        if(keep_acquired == false) {
            i2c_release(DEV_I2C);
        }

        return -PCA9685_ERR_SETPWM;
    }

    if(keep_acquired == false) {
        i2c_release(DEV_I2C);
    }
    return PCA9685_OK;
}

uint16_t pca9685_get_pwm(pca9685_t *dev, uint8_t num) {
    uint8_t buffer[4] = {0, 0, 0, 0};

    i2c_acquire(DEV_I2C);

    i2c_read_regs(DEV_I2C, DEV_ADDR, LED0_ON_L + 4 * num, &buffer, 4, 0);

    i2c_release(DEV_I2C);
    return ((buffer[0] & 0xff) | (buffer[1] << 8));
}

int pca9685_wakeup(pca9685_t *dev) {
    uint8_t sleep;

    i2c_acquire(DEV_I2C);

    if(i2c_read_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, &sleep, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_WAKEUP;
    }

    uint8_t wakeup = sleep & ~0x10; // set sleep bit low
    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, wakeup, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_WAKEUP;
    }

    i2c_release(DEV_I2C);
    return PCA9685_OK;
}

int pca9685_sleep(pca9685_t *dev) {
    uint8_t awake;

    i2c_acquire(DEV_I2C);

    if(i2c_read_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, &awake, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SLEEP;
    }

    uint8_t sleep = awake | 0x10; // set sleep bit high
    if(i2c_write_reg(DEV_I2C, DEV_ADDR, PCA9685_MODE1, sleep, 0) != 0) {
        i2c_release(DEV_I2C);
        return -PCA9685_ERR_SLEEP;
    }

    xtimer_usleep(5);
    i2c_release(DEV_I2C);
    return PCA9685_OK;
}
