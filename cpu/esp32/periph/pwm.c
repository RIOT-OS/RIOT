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

#define ENABLE_DEBUG 0
#include "debug.h"

#include "board.h"
#include "cpu.h"
#include "log.h"
#include "irq_arch.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#include "esp_common.h"
#include "gpio_arch.h"

#include "driver/periph_ctrl.h"
#include "soc/gpio_struct.h"
#include "soc/gpio_sig_map.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#if defined(PWM0_GPIOS) || defined(PWM1_GPIOS)

#define PWM_CLK       (160000000UL) /* base clock of PWM devices */
#define PWM_CPS_MAX   (10000000UL)  /* maximum cycles per second supported */
#define PWM_CPS_MIN   (2500UL)      /* minimum cycles per second supported */

#define PWM_TIMER_MOD_FREEZE          0  /* timer is disabled */
#define PWM_TIMER_MOD_UP              1  /* timer counts up */
#define PWM_TIMER_MOD_DOWN            2  /* timer counts down */
#define PWM_TIMER_MOD_UP_DOWN         3  /* timer counts up and then down */

#define PWM_TIMER_STOPS_AT_TEZ        0  /* PWM starts, then stops at next TEZ */
#define PWM_TIMER_STOPS_AT_TEP        1  /* PWM starts, then stops at next TEP */
#define PWM_TIMER_RUNS_ON             2  /* PWM runs on */
#define PWM_TIMER_STARTS_STOPS_AT_TEZ 3  /* PWM starts and stops at next TEZ */
#define PWM_TIMER_STARTS_STOPS_AT_TEP 4  /* PWM starts and stops at next TEP */

#define PWM_TIMER_UPDATE_IMMIDIATE    0  /* update period immediately */
#define PWM_TIMER_UPDATE_AT_TEZ       1  /* update period at TEZ */
#define PWM_TIMER_UPDATE_AT_SYNC      2  /* update period at sync */
#define PWM_TIMER_UPDATE_AT_TEZ_SYNC  3  /* update period at TEZ and sync */

#define PWM_OP_ACTION_NO_CHANGE       0  /* do not change output */
#define PWM_OP_ACTION_LOW             1  /* set the output to high */
#define PWM_OP_ACTION_HIGH            2  /* set the output to low */
#define PWM_OP_ACTION_TOGGLE          3  /* toggle the output */

#define PWM_OP_CHANNEL_A              0  /* operator channel A */
#define PWM_OP_CHANNEL_B              0  /* operator channel B */

/* forward declaration of internal functions */
static void _pwm_start(pwm_t pwm);
static void _pwm_stop(pwm_t pwm);
static bool _pwm_configuration(void);

/* data structure for static configuration of PWM devices */
struct _pwm_hw_t {
    mcpwm_dev_t*  regs;         /* PWM's registers set address */
    uint8_t       mod;          /* PWM's hardware module */
    uint8_t       int_src;      /* PWM's peripheral interrupt source */
    uint32_t      signal_group; /* PWM's base peripheral signal index */
    uint8_t       gpio_num;     /* number of GPIOs used as channels outputs */
    const gpio_t* gpios;        /* GPIOs used as channel outputs */
};

#ifdef PWM0_GPIOS
static const gpio_t _pwm_channel_gpios_0[] = PWM0_GPIOS;
#endif

#ifdef PWM1_GPIOS
static const gpio_t _pwm_channel_gpios_1[] = PWM1_GPIOS;
#endif

/* static configuration of PWM devices */
static const struct _pwm_hw_t _pwm_hw[] =
{
    #ifdef PWM0_GPIOS
    {
        .regs = &MCPWM0,
        .mod = PERIPH_PWM0_MODULE,
        .int_src = ETS_PWM0_INTR_SOURCE,
        .signal_group = PWM0_OUT0A_IDX,
        .gpio_num = ARRAY_SIZE(pwm0_channels),
        .gpios = pwm0_channels,
    },
    #endif
    #ifdef PWM1_GPIOS
    {
        .regs = &MCPWM1,
        .mod = PERIPH_PWM1_MODULE,
        .int_src = ETS_PWM1_INTR_SOURCE,
        .signal_group = PWM1_OUT0A_IDX,
        .gpio_num = ARRAY_SIZE(pwm1_channels),
        .gpios = pwm1_channels,
    },
    #endif
};

/* data structure dynamic channel configuration */
typedef struct {
    bool used;
    uint32_t duty;
} _pwm_chn_t;

/* data structure for dynamic configuration of PWM devices */
struct _pwm_dev_t {
    uint16_t    res;
    uint32_t    freq;
    pwm_mode_t  mode;
    uint8_t     chn_num;
    _pwm_chn_t  chn[PWM_CHANNEL_NUM_DEV_MAX];
};

/* dynamic configuration of PWM devices */
static struct _pwm_dev_t _pwm_dev[PWM_NUMOF_MAX] = {};

/* if pwm_init is called first time, it checks the overall pwm configuration */
static bool _pwm_init_first_time = true;

/* Initialize PWM device */
uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    DEBUG ("%s pwm=%u mode=%u freq=%u, res=%u\n", __func__, pwm, mode, freq, res);

    CHECK_PARAM_RET (pwm < PWM_NUMOF, 0);
    CHECK_PARAM_RET (freq > 0, 0);

    if (_pwm_init_first_time) {
        if (!_pwm_configuration())
            return 0;
    }

    if (_pwm_hw[pwm].gpio_num == 0) {
        LOG_TAG_ERROR("pwm", "PWM device %d has no assigned pins\n", pwm);
        return 0;
    }

    /* reset by disabling and enable the PWM module */
    periph_module_disable(_pwm_hw[pwm].mod);
    periph_module_enable(_pwm_hw[pwm].mod);

    _pwm_dev[pwm].res = res;
    _pwm_dev[pwm].freq = freq;
    _pwm_dev[pwm].mode = mode;
    _pwm_dev[pwm].chn_num = _pwm_hw[pwm].gpio_num;

    for (int i = 0; i < _pwm_dev[pwm].chn_num; i++) {
        /* initialize channel data */
        _pwm_dev[pwm].chn[i].used = false;
        _pwm_dev[pwm].chn[i].duty = 0;

         /* reset GPIO usage type if the pins were used already for PWM before
            to make it possible to reinitialize PWM with new parameters */
        if (gpio_get_pin_usage(_pwm_hw[pwm].gpios[i]) == _PWM) {
            gpio_set_pin_usage(_pwm_hw[pwm].gpios[i], _GPIO);
        }

       if (gpio_get_pin_usage(_pwm_hw[pwm].gpios[i]) != _GPIO) {
            LOG_TAG_ERROR("pwm", "GPIO%d is used for %s and cannot be used as PWM output\n", i,
                      gpio_get_pin_usage_str(_pwm_hw[pwm].gpios[i]));
            return 0;
        }

        if (gpio_init(_pwm_hw[pwm].gpios[i], GPIO_OUT) < 0) {
            return 0;
        }

        /* initialize the GPIO and route the PWM signal output to the GPIO */
        gpio_set_pin_usage(_pwm_hw[pwm].gpios[i], _GPIO);
        gpio_clear (_pwm_hw[pwm].gpios[i]);
        GPIO.func_out_sel_cfg[_pwm_hw[pwm].gpios[i]].func_sel = _pwm_hw[pwm].signal_group + i;
    }

    /* start the PWM device */
    _pwm_start(pwm);

    return freq;
}

uint8_t pwm_channels(pwm_t pwm)
{
    CHECK_PARAM_RET (pwm < PWM_NUMOF, 0);

    return _pwm_hw[pwm].gpio_num;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    DEBUG("%s pwm=%u channel=%u value=%u\n", __func__, pwm, channel, value);

    CHECK_PARAM (pwm < PWM_NUMOF);
    CHECK_PARAM (channel < _pwm_dev[pwm].chn_num);
    CHECK_PARAM (value <= _pwm_dev[pwm].res);

    uint32_t state = irq_disable();

    _pwm_dev[pwm].chn[channel].duty = value;
    _pwm_dev[pwm].chn[channel].used = true;

    /* determine used operator and operator output */
    uint8_t op_idx = channel >> 1;
    uint8_t op_out = channel & 0x01;

    /* compute and set shadow register (compare) )value of according channel */
    uint16_t cmp = 0;
    switch (_pwm_dev[pwm].mode) {
        case PWM_LEFT:   cmp = value;
                         break;
        case PWM_RIGHT:  cmp = value - 1;
                         break;
        case PWM_CENTER: cmp = _pwm_hw[pwm].regs->timer[0].period.period - value;
                         break;
    }
    _pwm_hw[pwm].regs->channel[op_idx].cmpr_value[op_out].cmpr_val = cmp;

    /* set actions for timing events (reset all first) */
    _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].val = 0;

    if (op_out == 0)
    {
        /* channel/output A is used -> set actions for channel A */
        switch (_pwm_dev[pwm].mode)
        {
            case PWM_LEFT:
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].utez = PWM_OP_ACTION_HIGH;
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].utea = PWM_OP_ACTION_LOW;
                break;

            case PWM_RIGHT:
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].dtea = PWM_OP_ACTION_HIGH;
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].dtep = PWM_OP_ACTION_LOW;
                break;

            case PWM_CENTER:
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].utea = PWM_OP_ACTION_HIGH;
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].dtea = PWM_OP_ACTION_LOW;
                break;
        }
    }
    else {
        /* channel/output B is used -> set actions for channel B */
        switch (_pwm_dev[pwm].mode)
        {
            case PWM_LEFT:
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].utez = PWM_OP_ACTION_HIGH;
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].uteb = PWM_OP_ACTION_LOW;
                break;

            case PWM_RIGHT:
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].dteb = PWM_OP_ACTION_HIGH;
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].dtep = PWM_OP_ACTION_LOW;
                break;

            case PWM_CENTER:
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].uteb = PWM_OP_ACTION_HIGH;
                _pwm_hw[pwm].regs->channel[op_idx].generator[op_out].dteb = PWM_OP_ACTION_LOW;
                break;
        }
    }

    irq_restore(state);
}

void pwm_poweron(pwm_t pwm)
{
    CHECK_PARAM (pwm < PWM_NUMOF);
    periph_module_enable(_pwm_hw[pwm].mod);
    _pwm_start(pwm);
}

void pwm_poweroff(pwm_t pwm)
{
    CHECK_PARAM (pwm < PWM_NUMOF);
    _pwm_stop (pwm);
    periph_module_disable(_pwm_hw[pwm].mod);
}

static void _pwm_start(pwm_t pwm)
{
    pwm_mode_t mode = _pwm_dev[pwm].mode;
    uint16_t   res  = _pwm_dev[pwm].res;
    uint32_t   freq = _pwm_dev[pwm].freq;
    uint32_t   period = 0;

    /* set timer mode */
    switch (mode) {
        case PWM_LEFT:
            period = res;
            _pwm_hw[pwm].regs->timer[0].mode.mode = PWM_TIMER_MOD_UP;
            break;
        case PWM_RIGHT:
            period = res;
            _pwm_hw[pwm].regs->timer[0].mode.mode = PWM_TIMER_MOD_DOWN;
            break;
        case PWM_CENTER:
            period = res * 2;
            _pwm_hw[pwm].regs->timer[0].mode.mode = PWM_TIMER_MOD_UP_DOWN;
            break;
    }

    uint32_t cps = period * freq;
    /* maximum number of timer clock cycles per second (freq*period) must not
       be greater than PWM_CPS_MAX, reduce the freq if necessary and keep
       the resolution */
    if (cps > PWM_CPS_MAX) {
        freq = PWM_CPS_MAX / period;
        _pwm_dev[pwm].freq = freq;
        DEBUG("%s freq*res was to high, freq was reduced to %d Hz\n",
              __func__, freq);
    }
    /* minimum number of timer clock cycles per second (freq*period) must not
       be less than PWM_CPS_MIN, increase the freq if necessary and keep
       the resolution */
    else if (cps < PWM_CPS_MIN) {
        freq = PWM_CPS_MIN / period;
        _pwm_dev[pwm].freq = freq;
        DEBUG("%s freq*res was to low, freq was increased to %d Hz\n",
              __func__, freq);
    }

    /* determine a suitable pwm clock prescale */
    uint32_t prescale;
    if (cps > 1000000) {
        /* pwm clock is not scaled,
           8 bit timer prescaler can scale down timer clock to 625 kHz */
        prescale = 1;
    }
    else if (cps > 100000) {
        /* pwm clock is scaled down to 10 MHz,
           8 bit timer prescaler can scale down timer clock to 39,0625 kHz */
        prescale = 16;
    }
    else if (cps > 10000) {
        /* pwm clock is scaled down to 1 MHz
           8 bit timer prescaler can scale down timer clock to 3,90625 kHz */
        prescale = 160;
    }
    else {
        /* pwm clock is scaled down to 640 kHz
           8 bit timer prescaler can scale down timer clock to 2,5 kHz */
        prescale = 250;
    }
    _pwm_hw[pwm].regs->clk_cfg.prescale = prescale - 1;

    /* set timing parameters (only timer0 is used) */
    _pwm_hw[pwm].regs->timer[0].period.prescale = (PWM_CLK / prescale / cps) - 1;
    _pwm_hw[pwm].regs->timer[0].period.period = (mode == PWM_CENTER) ? res : res - 1;
    _pwm_hw[pwm].regs->timer[0].period.upmethod = PWM_TIMER_UPDATE_IMMIDIATE;

    /* start the timer */
    _pwm_hw[pwm].regs->timer[0].mode.start = PWM_TIMER_RUNS_ON;

    /* set timer sync phase and enable timer sync input */
    _pwm_hw[pwm].regs->timer[0].sync.timer_phase = 0;
    _pwm_hw[pwm].regs->timer[0].sync.in_en = 1;

    /* set the duty for all channels to start them */
    for (int i = 0; i < _pwm_dev[pwm].chn_num; i++) {
        if (_pwm_dev[pwm].chn[i].used)
            pwm_set(pwm, i, _pwm_dev[pwm].chn[i].duty);
    }

    /* sync all timers */
    for (unsigned i = 0; i < PWM_NUMOF; i++) {
        _pwm_hw[i].regs->timer[0].sync.sync_sw = ~_pwm_hw[i].regs->timer[0].sync.sync_sw;
    }
}

static void _pwm_stop(pwm_t pwm)
{
    /* disable the timer */
    _pwm_hw[pwm].regs->timer[0].mode.mode = PWM_TIMER_MOD_FREEZE;
}

/* do some static initialization and configuration checks */
static bool _pwm_configuration(void)
{
    if (PWM_NUMOF > PWM_NUMOF_MAX) {
        LOG_TAG_ERROR("pwm", "%d PWM devices were defined, only %d PWM are "
                      "supported\n", PWM_NUMOF, PWM_NUMOF_MAX);
        return false;
    }

    for (unsigned i = 0; i < PWM_NUMOF; i++) {
        if (_pwm_hw[i].gpio_num > PWM_CHANNEL_NUM_DEV_MAX) {
            LOG_TAG_ERROR("pwm", "Number of PWM channels of device %d is %d, "
                          "at maximum only %d channels per PWM device are "
                          "supported\n",
                      i, _pwm_hw[i].gpio_num, PWM_CHANNEL_NUM_DEV_MAX);
            return false;
        }
    }
    bool multiple_used = false;
    for (unsigned i = 0; i < PWM_NUMOF; i++) {
        for (unsigned j = 0; j < PWM_NUMOF; j++) {
            if (i != j) {
                for (unsigned k = 0; k < _pwm_hw[i].gpio_num >> 2; k++) {
                    for (unsigned l = 0; l < _pwm_hw[j].gpio_num >> 2; l++) {
                        if (_pwm_hw[i].gpios[k] == _pwm_hw[j].gpios[l]) {
                            LOG_TAG_ERROR("pwm", "GPIO%d is used multiple times in "
                                          "PWM devices %d and %d\n",
                                          _pwm_hw[i].gpios[k], i, j);
                            multiple_used = true;
                        }
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

void pwm_print_config(void)
{
    for (unsigned pwm = 0; pwm < PWM_NUMOF; pwm++) {
        printf("\tPWM_DEV(%d)\tchannels=[ ", pwm);
        for (int i = 0; i < _pwm_hw[pwm].gpio_num; i++) {
            printf("%d ", _pwm_hw[pwm].gpios[i]);
        }
        printf("]\n");
    }
}

#else /* defined(PWM0_GPIOS) || defined(PWM1_GPIOS) */

void pwm_print_config(void)
{
    LOG_TAG_INFO("pwm", "no PWM devices\n");
}

#endif /* defined(PWM0_GPIOS) || defined(PWM1_GPIOS) */
