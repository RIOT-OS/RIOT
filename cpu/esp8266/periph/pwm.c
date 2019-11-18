/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "cpu.h"
#include "log.h"
#include "irq_arch.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#include "esp_common.h"
#include "esp/iomux_regs.h"
#include "esp/timer_regs.h"
#include "gpio_arch.h"
#include "sdk/sdk.h"
#include "xtensa/xtensa_api.h"

#define TIMER_FRC1_CLKDIV_16    BIT(2)
#define TIMER_FRC1_CLKDIV_256   BIT(3)

#define ETS_FRC1_INT_ENABLE()       xt_ints_on(BIT(ETS_FRC_TIMER1_INUM))
#define ETS_FRC1_INT_DISABLE()      xt_ints_off(BIT(ETS_FRC_TIMER1_INUM))
#define ETS_FRC1_INT_ATTACH(f, a)   xt_set_interrupt_handler(ETS_FRC_TIMER1_INUM, f, a)

typedef struct
{
    uint16_t    duty;
    uint32_t    next_on;
    uint32_t    next_off;
    gpio_t      gpio;

} _pwm_chn_t;

typedef struct
{
    pwm_mode_t  mode;
    uint16_t    res;
    uint32_t    load;
    uint32_t    cycles;
    uint8_t     chn_num;
    _pwm_chn_t  chn[PWM_CHANNEL_NUM_MAX];

} _pwm_dev_t;

static _pwm_dev_t _pwm_dev;

static void _pwm_timer_handler (void* arg)
{
    irq_isr_enter ();

    _pwm_dev.cycles++;

    for (int i = 0; i < _pwm_dev.chn_num; i++) {
        if (_pwm_dev.chn[i].duty != 0 &&
            _pwm_dev.chn[i].next_on == _pwm_dev.cycles) {
            gpio_set (_pwm_dev.chn[i].gpio);
            _pwm_dev.chn[i].next_on += _pwm_dev.res;
        }
        else if (_pwm_dev.chn[i].duty < _pwm_dev.res &&
                 _pwm_dev.chn[i].next_off == _pwm_dev.cycles) {
            gpio_clear (_pwm_dev.chn[i].gpio);
            _pwm_dev.chn[i].next_off += _pwm_dev.res;
        }
    }

    irq_isr_exit ();
}

static void _pwm_start(void)
{
    /* enable the timer and the interrupt and load the counter */
    TIMER_FRC1.CTRL = TIMER_FRC1_CLKDIV_16 | TIMER_CTRL_RELOAD | TIMER_CTRL_RUN;
    TM1_EDGE_INT_ENABLE();
    ETS_FRC1_INT_ENABLE();
    TIMER_FRC1.LOAD = _pwm_dev.load;

    _pwm_dev.cycles = 0;

    /* set the duty for all channels to start them */
    for (int i = 0; i < _pwm_dev.chn_num; i++) {
        pwm_set(PWM_DEV(0), i, _pwm_dev.chn[i].duty);
    }
}

static void _pwm_stop(void)
{
    /* disable the interrupt and the timer */
    ETS_FRC1_INT_DISABLE();
    TM1_EDGE_INT_DISABLE();
    TIMER_FRC1.CTRL &= ~TIMER_CTRL_RUN;
}

#define PWM_MAX_CPS 100000UL  /* maximum cycles per second */

uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    DEBUG ("%s pwm=%u mode=%u freq=%u, res=%u\n", __func__, pwm, mode, freq, res);

    uint8_t _pwm_channel_gpio_num = sizeof(pwm0_channels) >> 2;

    assert(pwm < PWM_NUMOF_MAX);
    assert(pwm < PWM_NUMOF);
    assert(freq > 0);
    assert(_pwm_channel_gpio_num <= PWM_CHANNEL_NUM_MAX);

    /* maximum number of cycles per second (freq*res) should not be greater than */
    /* 100.000 (period of 10 us), reduce freq if necessary and keep resolution */
    if (res * freq > PWM_MAX_CPS) {
        freq = PWM_MAX_CPS / res;
    }

    _pwm_dev.load = 5e6 / freq / res;   /* load value for FRC1 at TIMER_FRC1_CLKDIV_16 */
    _pwm_dev.res = res;
    _pwm_dev.chn_num = 0;
    _pwm_dev.cycles = 0;
    _pwm_dev.mode = mode;

    for (int i = 0; i < _pwm_channel_gpio_num; i++) {
        if (gpio_get_pin_usage(pwm0_channels[i]) != _GPIO) {
            LOG_ERROR("GPIO%d is used for something else and cannot be used as PWM output\n", i);
            return 0;
        }

        if (gpio_init(pwm0_channels[i], GPIO_OUT) < 0) {
            return 0;
        }

        gpio_clear (pwm0_channels[i]);

        _pwm_dev.chn[_pwm_dev.chn_num].duty = 0;
        _pwm_dev.chn[_pwm_dev.chn_num].next_on = 0;
        _pwm_dev.chn[_pwm_dev.chn_num].next_off = 0;
        _pwm_dev.chn[_pwm_dev.chn_num].gpio = pwm0_channels[i];

        _pwm_dev.chn_num++;
    }

    TIMER_FRC1.CTRL = TIMER_FRC1_CLKDIV_16 | TIMER_CTRL_RELOAD | TIMER_CTRL_RUN;
    ETS_FRC1_INT_ATTACH(_pwm_timer_handler,0);
    TM1_EDGE_INT_ENABLE();
    ETS_FRC1_INT_ENABLE();

    TIMER_FRC1.LOAD = _pwm_dev.load;

    return freq;
}

uint8_t pwm_channels(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);

    return _pwm_dev.chn_num;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    DEBUG("%s pwm=%u channel=%u value=%u\n", __func__, pwm, channel, value);

    assert(pwm < PWM_NUMOF);
    assert(channel < _pwm_dev.chn_num);
    assert(value <= _pwm_dev.res);

    uint32_t state = irq_disable();
    uint32_t phase = _pwm_dev.cycles - _pwm_dev.cycles % _pwm_dev.res;
    uint32_t next_on = phase;
    uint32_t next_off;

    switch (_pwm_dev.mode) {
        case PWM_LEFT:
            next_on = phase;
            break;

        case PWM_RIGHT:
            next_on = phase + _pwm_dev.res - value;
            break;

        case PWM_CENTER:
            next_on = phase + (_pwm_dev.res - value) / 2;
            break;
    }

    next_off = next_on + value;

    if (_pwm_dev.cycles >= next_on) {
        next_on += _pwm_dev.res;
    }
    if (_pwm_dev.cycles >= next_off) {
        next_off += _pwm_dev.res;
    }

    _pwm_dev.chn[channel].next_on = next_on;
    _pwm_dev.chn[channel].next_off = next_off;
    _pwm_dev.chn[channel].duty = value;

    irq_restore(state);
}

void pwm_poweron(pwm_t pwm)
{
    CHECK_PARAM (pwm < PWM_NUMOF);

    _pwm_start();
}

void pwm_poweroff(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);

    _pwm_stop ();
}

void pwm_print_config(void)
{
    printf("\tPWM_DEV(0)\tchannels=[ ");
    for (unsigned i = 0; i < sizeof(pwm0_channels) >> 2; i++) {
        printf("%d ", pwm0_channels[i]);
    }
    printf("]\n");
}
