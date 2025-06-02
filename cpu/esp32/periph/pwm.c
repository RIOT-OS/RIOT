/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "bitarithm.h"
#include "board.h"
#include "cpu.h"
#include "gpio_arch.h"
#include "kernel_defines.h"
#include "log.h"
#include "irq_arch.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#include "esp_cpu.h"
#include "esp_common.h"
#include "esp_private/periph_ctrl.h"
#include "esp_rom_gpio.h"
#include "hal/ledc_hal.h"
#include "soc/ledc_struct.h"
#include "soc/rtc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if defined(PWM0_GPIOS) || defined(PWM1_GPIOS) || defined(PWM2_GPIOS) || defined(PWM3_GPIOS)

/* Ensure that the SPIn_* symbols define SPI_DEV(n) */
#if defined(PWM1_GPIOS) && !defined(PWM0_GPIOS)
#error "PWM1_GPIOS is used but PWM0_GPIOS is not defined"
#elif defined(PWM2_GPIOS) && !defined(PWM1_GPIOS)
#error "PWM2_GPIOS is used but PWM1_GPIOS is not defined"
#elif defined(PWM3_GPIOS) && !defined(PWM2_GPIOS)
#error "PWM3_GPIOS is used but PWM2_GPIOS is not defined"
#endif

#define SOC_LEDC_CLK_DIV_BIT_NUM        18
#define SOC_LEDC_CLK_DIV_INT_BIT_NUM    10  /* integral part of CLK divider */
#define SOC_LEDC_CLK_DIV_FRAC_BIT_NUM   8   /* fractional part of CLK divider */

#define PWM_HW_RES_MAX  ((uint32_t)1 << SOC_LEDC_TIMER_BIT_WIDTH)
#define PWM_HW_RES_MIN  ((uint32_t)1 << 1)

#define _DEV     _pwm_dev[pwm]      /* shortcut for PWM device descriptor */
#define _CFG     pwm_config[pwm]    /* shortcut for PWM device configuration */

/* data structure for dynamic channel parameters */
typedef struct {
    uint32_t duty;      /* actual duty value */
    uint32_t hpoint;    /* actual hpoing value */
    bool used;          /* true if the channel is set by pwm_set */
    uint8_t ch;         /* actual channel index within used channel group */
} _pwm_ch_t;

/* data structure for device handling at runtime */
typedef struct {
    uint32_t freq;                  /* specified frequency parameter */
    uint32_t res;                   /* specified resolution parameter */
    uint32_t hw_freq;               /* used hardware frequency */
    uint32_t hw_res;                /* used hardware resolution */
    uint32_t hw_clk_div;            /* used hardware clock divider */
    _pwm_ch_t ch[PWM_CH_NUMOF_MAX]; /* dynamic channel parameters at runtime */
    ledc_hal_context_t hw;          /* used hardware device context */
    pwm_mode_t mode;                /* specified mode */
    ledc_timer_bit_t hw_res_bit;    /* used hardware resolution in bit */
    bool enabled;                   /* true if the device is used (powered on) */
} _pwm_dev_t;

static _pwm_dev_t _pwm_dev[PWM_NUMOF] = { };

/* if pwm_init is called first time, it checks the pwm configuration */
static bool _pwm_initialized = false;

/* static configuration checks and initialization on first pwm_init */
static bool _pwm_initialize(void);

/* Initialize PWM device */
uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    _Static_assert(PWM_NUMOF <= PWM_NUMOF_MAX, "Too many PWM devices defined");

    if (!_pwm_initialized) {
        if (!_pwm_initialize()) {
            return 0;
        }
        _pwm_initialized = true;
    }

    assert(pwm < PWM_NUMOF);
    assert(freq > 0);

    _DEV.enabled = true;
    _DEV.res = res;
    _DEV.freq = freq;
    _DEV.mode = mode;

    if ((res < PWM_HW_RES_MIN) || (_DEV.res > PWM_HW_RES_MAX)) {
        LOG_TAG_ERROR("pwm", "Resolution of PWM device %u to be in "
                      "range [%"PRIu32", %"PRIu32"]\n",
                      pwm, PWM_HW_RES_MIN, PWM_HW_RES_MAX);
        return 0;
    }

    /*
     * The hardware resolution must be a power of two, so we determine the
     * next power of two, which covers the desired resolution
     */
    ledc_timer_bit_t hw_res_bit = bitarithm_msb(res - 1);
    if (hw_res_bit < SOC_LEDC_TIMER_BIT_WIDTH) {
        hw_res_bit++;
    }

    uint32_t hw_res = 1 << hw_res_bit;

    uint32_t hw_ticks_max = rtc_clk_apb_freq_get();
    uint32_t hw_ticks_min = hw_ticks_max / (1 << SOC_LEDC_CLK_DIV_INT_BIT_NUM);
    uint32_t hw_freq_min = hw_ticks_min / (1 << SOC_LEDC_TIMER_BIT_WIDTH) + 1;

    if (freq < hw_freq_min) {
        LOG_TAG_ERROR("pwm", "Frequency of %"PRIu32" Hz is too less, minimum "
                      "frequency is %"PRIu32" Hz\n", freq, hw_freq_min);
        return 0;
    }

    /* number of hardware ticks required, at maximum it can be APB clock */
    uint32_t hw_ticks = MIN(freq * hw_res, rtc_clk_apb_freq_get());

    /*
     * if the number of required ticks is less than minimum ticks supported by
     * the hardware supports, we have to increase the resolution.
     */
    while (hw_ticks < hw_ticks_min) {
        hw_res_bit++;
        hw_res = 1 << hw_res_bit;
        hw_ticks = freq * hw_res;
    }

    /* LEDC_CLK_DIV is given in Q10.8 format */
    uint32_t hw_clk_div =
        ((uint64_t)rtc_clk_apb_freq_get() << SOC_LEDC_CLK_DIV_FRAC_BIT_NUM) / hw_ticks;

    _DEV.freq = freq;
    _DEV.res = res;
    _DEV.hw_freq = hw_ticks / hw_res;
    _DEV.hw_res = hw_res;
    _DEV.hw_res_bit = hw_res_bit;
    _DEV.hw_clk_div = hw_clk_div;

    DEBUG("%s hw_freq=%"PRIu32" hw_res=%"PRIu32" hw_ticks=%"PRIu32
          " hw_clk_div=%"PRIu32"\n", __func__,
          _DEV.hw_freq, _DEV.hw_res, hw_ticks, _DEV.hw_clk_div);

    for (int i = 0; i < _CFG.ch_numof; i++) {
        /* initialize channel data */
        _DEV.ch[i].used = false;
        _DEV.ch[i].duty = 0;

         /* reset GPIO usage type if the pins were used already for PWM before
            to make it possible to reinitialize PWM with new parameters */
        if (gpio_get_pin_usage(_CFG.gpios[i]) == _PWM) {
            gpio_set_pin_usage(_CFG.gpios[i], _GPIO);
        }

       if (gpio_get_pin_usage(_CFG.gpios[i]) != _GPIO) {
            LOG_TAG_ERROR("pwm", "GPIO%d is used for %s and cannot be used as "
                          "PWM output\n",
                          i, gpio_get_pin_usage_str(_CFG.gpios[i]));
            return 0;
        }

        /* initialize the GPIOs and route the PWM signal output to the GPIO */
        if (gpio_init(_CFG.gpios[i], GPIO_OUT) < 0) {
            return 0;
        }

        gpio_set_pin_usage(_CFG.gpios[i], _PWM);
        gpio_clear(_CFG.gpios[i]);

        esp_rom_gpio_connect_out_signal(
            _CFG.gpios[i],
            ledc_periph_signal[_CFG.group].sig_out0_idx + _DEV.ch[i].ch, 0, 0);

    }

    pwm_poweron(pwm);

    return _DEV.hw_freq;
}

uint8_t pwm_channels(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    return _CFG.ch_numof;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    DEBUG("%s pwm=%u channel=%u value=%u\n", __func__, pwm, channel, value);

    assert(pwm < PWM_NUMOF);
    assert(channel < _CFG.ch_numof);

    value = MIN(value, _DEV.res);

    _DEV.ch[channel].used = true;
    _DEV.ch[channel].duty = value * _DEV.hw_res / _DEV.res;
    _DEV.ch[channel].hpoint = 0;

    switch (_DEV.mode) {
        case PWM_LEFT:
            _DEV.ch[channel].hpoint = 0;
            break;
        case PWM_RIGHT:
            _DEV.ch[channel].hpoint = _DEV.hw_res - 1 - _DEV.ch[channel].duty;
            break;
        case PWM_CENTER:
            _DEV.ch[channel].hpoint = (_DEV.hw_res - _DEV.ch[channel].duty) >> 1;
            break;
    }

    DEBUG("%s pwm=%u duty=%"PRIu32" hpoint=%"PRIu32"\n",
          __func__, pwm, _DEV.ch[channel].duty, _DEV.ch[channel].hpoint);

    unsigned ch = _DEV.ch[channel].ch;  /* internal channel mapping */

    critical_enter();
    ledc_hal_set_duty_int_part(&_DEV.hw, ch, _DEV.ch[channel].duty);
    ledc_hal_set_hpoint(&_DEV.hw, ch, _DEV.ch[channel].hpoint);
    ledc_hal_set_sig_out_en(&_DEV.hw, ch, true);
    ledc_hal_ls_channel_update(&_DEV.hw, ch);
    ledc_hal_set_duty_start(&_DEV.hw, ch, true);
    critical_exit();
}

void pwm_poweron(pwm_t pwm)
{
    DEBUG("%s pwm=%u\n", __func__, pwm);

    /* enable and init the module and select the right clock source */
    periph_module_enable(_CFG.module);
    ledc_hal_init(&_DEV.hw, _CFG.group);
    ledc_hal_set_slow_clk_sel(&_DEV.hw, LEDC_SLOW_CLK_APB);
#if SOC_LEDC_HAS_TIMER_SPECIFIC_MUX
    ledc_hal_set_clock_source(&_DEV.hw, _CFG.timer, LEDC_APB_CLK);
#endif

    /* update the timer according to determined parameters */
    ledc_hal_set_clock_divider(&_DEV.hw, _CFG.timer, _DEV.hw_clk_div);
    ledc_hal_set_duty_resolution(&_DEV.hw, _CFG.timer, _DEV.hw_res_bit);
    ledc_hal_ls_timer_update(&_DEV.hw, _CFG.timer);
    ledc_hal_timer_rst(&_DEV.hw, _CFG.timer);

    critical_enter();
    for (unsigned i = 0; i < _CFG.ch_numof; i++) {
        /* static configuration of the channel, no fading */
        ledc_hal_set_fade_param(&_DEV.hw, _DEV.ch[i].ch, 0, 1, 1, 0, 1);
        ledc_hal_set_fade_end_intr(&_DEV.hw, _DEV.ch[i].ch, 0);

        /* bind the channel to the timer and disable the output for now */
        ledc_hal_bind_channel_timer(&_DEV.hw, _DEV.ch[i].ch, _CFG.timer);

        /* restore used parameters */
        ledc_hal_set_duty_int_part(&_DEV.hw, _DEV.ch[i].ch, _DEV.ch[i].duty);
        ledc_hal_set_hpoint(&_DEV.hw, _DEV.ch[i].ch, _DEV.ch[i].hpoint);
        ledc_hal_set_sig_out_en(&_DEV.hw, _DEV.ch[i].ch, _DEV.ch[i].used);
        ledc_hal_ls_channel_update(&_DEV.hw, _DEV.ch[i].ch);
        ledc_hal_set_duty_start(&_DEV.hw, _DEV.ch[i].ch, _DEV.ch[i].used);
    }
    _DEV.enabled = true;
    critical_exit();
}

void pwm_poweroff(pwm_t pwm)
{
    DEBUG("%s pwm=%u\n", __func__, pwm);

    if (!_pwm_dev[pwm].enabled) {
        return;
    }

    unsigned i;

    /* disable the signal of all channels */
    critical_enter();
    for (i = 0; i < _CFG.ch_numof; i++) {
        ledc_hal_set_idle_level(&_DEV.hw, _DEV.ch[i].ch, 0);
        ledc_hal_set_sig_out_en(&_DEV.hw, _DEV.ch[i].ch, false);
        ledc_hal_set_duty_start(&_DEV.hw, _DEV.ch[i].ch, false);
        ledc_hal_ls_channel_update(&_DEV.hw, _DEV.ch[i].ch);
    }
    _DEV.enabled = false;
    critical_exit();

    /* check whether all devices of the same hardware module are disabled */
    for (i = 0; i < PWM_NUMOF; i++) {
        if ((_CFG.module == pwm_config[i].module) && _pwm_dev[i].enabled) {
            break;
        }
    }

    /* if all devices of the same hardware module are disable, it is powered off */
    if (i == PWM_NUMOF) {
        periph_module_disable(_CFG.module);
    }
}

void pwm_print_config(void)
{
    for (unsigned pwm = 0; pwm < PWM_NUMOF; pwm++) {
        printf("\tPWM_DEV(%u)\tchannels=[ ", pwm);
        for (int i = 0; i < _CFG.ch_numof; i++) {
            printf("%d ", _CFG.gpios[i]);
        }
        printf("]\n");
    }
}

/* do static configuration checks */
static bool _pwm_initialize(void)
{
    unsigned ch_numof[2] = {};

    for (unsigned pwm = 0; pwm < PWM_NUMOF; pwm++) {
        /* compute the channel indices */
        for (unsigned i = 0; i < _CFG.ch_numof; i++) {
            _pwm_dev[pwm].ch[i].ch = ch_numof[_CFG.group] + i;
        }
        ch_numof[_CFG.group] += _CFG.ch_numof;
        if (_CFG.ch_numof > PWM_CH_NUMOF_MAX) {
            LOG_TAG_ERROR("pwm", "Number of PWM channels of device %d is %d, "
                          "only %d channels per PWM device are supported\n",
                          pwm, _CFG.ch_numof, PWM_CH_NUMOF_MAX);
            return false;
        }
    }

    unsigned total_ch_numof = ch_numof[0] + ch_numof[1];

    if (total_ch_numof > (SOC_LEDC_CHANNEL_NUM * ARRAY_SIZE(ledc_periph_signal))) {
        LOG_TAG_ERROR("pwm", "Total number of PWM channels is %d, only "
                      "%d channels are supported at maximum\n", total_ch_numof,
                       PWM_CH_NUMOF_MAX * ARRAY_SIZE(ledc_periph_signal));
        return false;
    }

    bool multiple_used = false;
    for (unsigned pi = 0; pi < PWM_NUMOF; pi++) {
        for (unsigned ci = 0; ci < pwm_config[pi].ch_numof; ci++) {
            for (unsigned pj = 0; pj < PWM_NUMOF; pj++) {
                if (pi == pj) {
                    continue;
                }
                for (unsigned cj = 0; cj < pwm_config[pj].ch_numof; cj++) {
                    if (pwm_config[pi].gpios[ci] == pwm_config[pj].gpios[cj]) {
                        LOG_TAG_ERROR("pwm", "GPIO%d is used multiple times in "
                                      "PWM devices %d and %d\n",
                                      pwm_config[pi].gpios[ci], pi, pj);
                        multiple_used = true;
                    }
                }
            }
        }
    }
    if (multiple_used) {
        return false;
    }

    return true;
}

#else

void pwm_print_config(void)
{
    LOG_TAG_INFO("pwm", "no PWM devices\n");
}

#endif
