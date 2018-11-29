/*
 * Copyright (C) 2017 Víctor Ariño
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Víctor Ariño <victor@lebrush.org>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#ifdef PWM_NUMOF

#define WGM0 0
#define WGM1 1
#define WGM2 3
#define COMA0 6
#define COMA1 7

static struct {
    uint8_t CRA;
    uint8_t CRB;
    uint8_t res;
} state[PWM_NUMOF];

static inline unsigned get_prescaler(pwm_t dev, uint32_t *scale)
{
    uint16_t divmask = pwm_conf[dev].div;
    uint32_t target = *scale;
    uint8_t div, pre = 0;

    for (div = 0; divmask > 0; div++) {
        if (divmask & 0x01) {
            pre++;
            *scale = (uint32_t)(1 << div);
            if (*scale >= target) {
                break;
            }
        }
        divmask >>= 1;
    }

    return pre;
}

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    /* only left implemented, max resolution 256 */
    assert(dev < PWM_NUMOF && mode == PWM_LEFT && res <= 256);
    /* resolution != 256 only valid if ch0 not used */
    assert(!(res != 256 && pwm_conf[dev].pin_ch[0] != GPIO_UNDEF));

    /* disable PWM */
    pwm_conf[dev].dev->CRA = 0x00;
    pwm_conf[dev].dev->CRB = 0x00;
    pwm_conf[dev].dev->OCR[0] = 0;
    pwm_conf[dev].dev->OCR[1] = 0;

    /* disable power reduction */
    if (dev) {
        power_timer2_enable();
    }
    else {
        power_timer0_enable();
    }

    /* find out prescaler */
    uint32_t scale = (CLOCK_CORECLOCK / (freq * (uint32_t)res));
    unsigned pre = get_prescaler(dev, &scale);
    freq = (CLOCK_CORECLOCK / (scale * (uint32_t)res));

    /* compute register values and enable pins */
    uint8_t cra = _BV(WGM1) | _BV(WGM0);
    uint8_t crb = 0;

    res -= 1;
    /* configure pins and resolution. Output must be low at initialization.
     * Force the pin low to avoid flickering. */
    if (pwm_conf[dev].pin_ch[0] != GPIO_UNDEF) {
        gpio_init(pwm_conf[dev].pin_ch[0], GPIO_OUT);
        gpio_clear(pwm_conf[dev].pin_ch[0]);
    }
    else {
        crb |= _BV(WGM2);
        pwm_conf[dev].dev->OCR[0] = (uint8_t)res;
    }

    if (pwm_conf[dev].pin_ch[1] != GPIO_UNDEF) {
        gpio_init(pwm_conf[dev].pin_ch[1], GPIO_OUT);
        gpio_clear(pwm_conf[dev].pin_ch[1]);
    }

    pwm_conf[dev].dev->CRA = cra;
    pwm_conf[dev].dev->CRB = crb | (pre);
    state[dev].CRA = cra;
    state[dev].CRB = crb | (pre);
    state[dev].res = res;

    /* return real frequency */
    return freq;
}

uint8_t pwm_channels(pwm_t dev)
{
    assert(dev < PWM_NUMOF);

    /* a pwm with no channels enabled makes no sense. Assume at least one is
     * enabled */
    if (pwm_conf[dev].pin_ch[0] == GPIO_UNDEF ||
        pwm_conf[dev].pin_ch[1] == GPIO_UNDEF) {
        return 1;
    }

    return 2;
}

void pwm_set(pwm_t dev, uint8_t ch, uint16_t value)
{
    assert(dev < PWM_NUMOF && ch <= 1 && pwm_conf[dev].pin_ch[ch] != GPIO_UNDEF);

    /* output flickers when duty cycle is 0 or 100%. Simply force the pin
     * low or high respectively to have a clean output. */
    uint8_t bit = (_BV(COMA1) >> (ch << 1));
    if (value >= state[dev].res) {
        pwm_conf[dev].dev->CRA &= ~bit;
        gpio_set(pwm_conf[dev].pin_ch[ch]);
    }
    else if (value == 0) {
        pwm_conf[dev].dev->CRA &= ~bit;
        gpio_clear(pwm_conf[dev].pin_ch[ch]);
    }
    else {
        pwm_conf[dev].dev->OCR[ch] = value;
        pwm_conf[dev].dev->CRA |= bit;
    }
}

void pwm_poweron(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    /* disable power reduction */
    if (dev) {
        power_timer2_enable();
    }
    else {
        power_timer0_enable();
    }
    pwm_conf[dev].dev->CRA = state[dev].CRA;
    pwm_conf[dev].dev->CRB = state[dev].CRB;
}

void pwm_poweroff(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
    pwm_conf[dev].dev->CRA = 0x00;
    pwm_conf[dev].dev->CRB = 0x00;
    /* disable power reduction */
    if (dev) {
        power_timer2_disable();
    }
    else {
        power_timer0_disable();
    }

    if (pwm_conf[dev].pin_ch[0] != GPIO_UNDEF) {
        gpio_clear(pwm_conf[dev].pin_ch[0]);
    }

    if (pwm_conf[dev].pin_ch[1] != GPIO_UNDEF) {
        gpio_clear(pwm_conf[dev].pin_ch[1]);
    }
}
#endif /* PWM_NUMOF */
