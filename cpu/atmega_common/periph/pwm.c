/*
 * SPDX-FileCopyrightText: 2017 Víctor Ariño
 * SPDX-FileCopyrightText: 2023 Hugues Larrive
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Hugues Larrive <hugues.larrive@pm.me>
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
#define COMB0 4
#define COMB1 5
#define COMA0 6
#define COMA1 7

static struct {
#if (defined(TCCR0A) && defined(TCCR0B)) || (defined(TCCR2A) && defined(TCCR2B))
    uint8_t CRA;
    uint8_t CRB;
#elif defined(TCCR2)
    uint8_t CR;
#endif
    uint8_t res;
} state[PWM_NUMOF];

static inline uint8_t get_prescaler(pwm_t dev, uint32_t *scale)
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

#if (defined(TCCR0A) && defined(TCCR0B)) || (defined(TCCR2A) && defined(TCCR2B))
static inline void compute_cra_and_crb(pwm_t dev, uint8_t pre)
{
    uint8_t cra = (1 << WGM1) | (1 << WGM0);
    uint8_t crb = pre;

    if (pwm_conf[dev].pin_ch[0] != GPIO_UNDEF) {
        cra |= (1 << COMA1);
    }
    else {
        crb |= (1 << WGM2);
    }

    if (pwm_conf[dev].pin_ch[1] != GPIO_UNDEF) {
        cra |= (1 << COMB1);
    }

    state[dev].CRA = cra;
    state[dev].CRB = crb;
}
#endif

static inline void apply_config(pwm_t dev)
{
#if (defined(TCCR0A) && defined(TCCR0B)) || (defined(TCCR2A) && defined(TCCR2B))
    pwm_conf[dev].dev->CRA = state[dev].CRA;
    pwm_conf[dev].dev->CRB = state[dev].CRB;
#elif defined(TCCR2)
    pwm_conf[dev].dev->CR = state[dev].CR;
#endif

    if (pwm_conf[dev].pin_ch[0] == GPIO_UNDEF) {
        /* If channel 0 is not used, variable resolutions can be used for
         * channel 1 */
#if (defined(OCR0A) && defined(OCR0B)) || (defined(OCR2A) && defined(OCR2B))
        pwm_conf[dev].dev->OCR[0] = state[dev].res;
#elif defined(OCR2)
        pwm_conf[dev].dev->OCR = state[dev].res;
#endif
    }
}

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    (void)mode;
    /* only left implemented, max resolution 256 */
    assert(dev < PWM_NUMOF && mode == PWM_LEFT && res <= 256);
    /* resolution != 256 only valid if ch0 not used */
    assert(!(res != 256 && pwm_conf[dev].pin_ch[0] != GPIO_UNDEF));

    /* disable PWM */
#if (defined(TCCR0A) && defined(TCCR0B)) || (defined(TCCR2A) && defined(TCCR2B))
    pwm_conf[dev].dev->CRA = 0x00;
    pwm_conf[dev].dev->CRB = 0x00;
    pwm_conf[dev].dev->OCR[0] = 0;
    pwm_conf[dev].dev->OCR[1] = 0;
#elif defined(TCCR2)
    pwm_conf[dev].dev->CR = 0x00;
    pwm_conf[dev].dev->OCR = 0;
#endif

#if defined(PRT2) || defined(PRTIM2) || defined(PRT0) || defined(PRTIM0)
    /* disable power reduction */
    if (dev) {
        power_timer2_enable();
    }
    else {
        power_timer0_enable();
    }
#endif

    /* find out prescaler */
    uint32_t scale = (CLOCK_CORECLOCK / (freq * (uint32_t)res));
    uint8_t pre = get_prescaler(dev, &scale);
    freq = (CLOCK_CORECLOCK / (scale * (uint32_t)res));

    /* Compute configuration and store it in the state. (The state is needed
     * for later calls to pwm_poweron().)*/
#if (defined(TCCR0A) && defined(TCCR0B)) || (defined(TCCR2A) && defined(TCCR2B))
    compute_cra_and_crb(dev, pre);
#elif defined(TCCR2)
    uint8_t cr = (1 << WGM21) | (1 << WGM20) | (1 << COM21) | pre;
    state[dev].CR = cr;
#endif
    state[dev].res = res - 1;

    /* Apply configuration stored in state */
    apply_config(dev);

    /* Enable outputs */
    if (pwm_conf[dev].pin_ch[0] != GPIO_UNDEF) {
        gpio_init(pwm_conf[dev].pin_ch[0], GPIO_OUT);
    }
    if (pwm_conf[dev].pin_ch[1] != GPIO_UNDEF) {
        gpio_init(pwm_conf[dev].pin_ch[1], GPIO_OUT);
    }

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
#ifdef OCR2
    (void)ch;
#endif
    assert(dev < PWM_NUMOF && ch <= 1 && pwm_conf[dev].pin_ch[ch] != GPIO_UNDEF);
    if (value > state[dev].res) {
#if (defined(OCR0A) && defined(OCR0B)) || (defined(OCR2A) && defined(OCR2B))
        pwm_conf[dev].dev->OCR[ch] = state[dev].res;
#elif defined(OCR2)
        pwm_conf[dev].dev->OCR = state[dev].res;
#endif
    }
    else {
#if (defined(OCR0A) && defined(OCR0B)) || (defined(OCR2A) && defined(OCR2B))
        pwm_conf[dev].dev->OCR[ch] = value;
#elif defined(OCR2)
        pwm_conf[dev].dev->OCR = value;
#endif
    }
}

void pwm_poweron(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
#if defined(PRT2) || defined(PRTIM2) || defined(PRT0) || defined(PRTIM0)
    /* disable power reduction */
    if (dev) {
        power_timer2_enable();
    }
    else {
        power_timer0_enable();
    }
#endif

    apply_config(dev);
}

void pwm_poweroff(pwm_t dev)
{
    assert(dev < PWM_NUMOF);
#if (defined(TCCR0A) && defined(TCCR0B)) || (defined(TCCR2A) && defined(TCCR2B))
    pwm_conf[dev].dev->CRA = 0x00;
    pwm_conf[dev].dev->CRB = 0x00;
    /* disable timers to lower power consumption */
    if (dev) {
        power_timer2_disable();
    }
    else {
        power_timer0_disable();
    }
#elif defined(TCCR2)
    pwm_conf[dev].dev->CR = 0x00;
#endif

    if (pwm_conf[dev].pin_ch[0] != GPIO_UNDEF) {
        gpio_clear(pwm_conf[dev].pin_ch[0]);
    }

    if (pwm_conf[dev].pin_ch[1] != GPIO_UNDEF) {
        gpio_clear(pwm_conf[dev].pin_ch[1]);
    }
}
#endif /* PWM_NUMOF */
