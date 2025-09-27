/*
 * SPDX-FileCopyrightText: 2023-2024 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "cpu.h"
#include "assert.h"
#include "periph/pwm.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include <stdio.h>

#define ENABLE_DEBUG    0
#include "debug.h"

/* Vendor files don't offer a convenient way to access these registers
   through a dedicated struct, thus create one for this purpose */
typedef struct {
    uint32_t csr;
    uint32_t div;
    uint32_t ctr;
    uint32_t cc;
    uint32_t top;
} pwm_slice_reg_t;

/* Structure holding all PWM slices registers */
struct pwm_reg {
    pwm_slice_reg_t slices[PWM_SLICE_NUMOF];
};

/* Start address of PWM slices */
#define PWM_REG ((struct pwm_reg *)PWM_BASE)

/* Helper to get slice register */
static inline pwm_slice_reg_t *pwm_slice(unsigned slice_idx)
{
    return &PWM_REG->slices[slice_idx];
}

/* PWM block is feed by RP2040 sysclk (CLOCK_CORECLOCK) */
uint32_t pwm_init(pwm_t pwm, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    uint8_t div_int;
    uint8_t div_frac;
    uint32_t val;
    uint32_t ret;
    uint8_t slice = pwm_config[pwm].pwm_slice;

    (void)mode;

    const gpio_io_ctrl_t pwm_io_config = {
        .function_select = FUNCTION_SELECT_PWM,
    };

    /* Initialize associated GPIO pin */
    if (pwm_config[pwm].chan[0].pin != GPIO_UNDEF) {
        gpio_set_io_config(pwm_config[pwm].chan[0].pin, pwm_io_config);
    }
    if (pwm_config[pwm].chan[1].pin != GPIO_UNDEF) {
        gpio_set_io_config(pwm_config[pwm].chan[1].pin, pwm_io_config);
    }

    /* Compute DIV register value to get closest match for
       freq and res variables */
    val = (((uint32_t)CLOCK_CORECLOCK) << 4) / (freq * res);
    /* If the value is above 4095, we will not be able to reach the desired
       frequency so set the divisor value to maximum to get to the closest
       possible value for the PWM frequency */
    if (val > 4095) {
        div_frac = 0x0F;
        div_int = 0xFF;
    } else {
        div_frac = val % 16;
        div_int = val / 16;
    }
    /* Compute the real frequency we will get */
    ret = CLOCK_CORECLOCK / (res * (div_int + (div_frac / 16)));

    DEBUG("[pwm]: div_int:%d, div_frac:%d\n", div_int, div_frac);
    /* Set the slice divider to reach the desired frequency */
    pwm_slice(slice)->div = ((div_int << PWM_CH0_DIV_INT_Pos) | div_frac);

    /* Let PWM slice run in free running mode */
    pwm_slice(slice)->csr = PWM_CH0_CSR_DIVMODE_div;

    /* Set PWM slice TOP value */
    pwm_slice(slice)->top = res-1;

    /* Enable PWM slice */
    io_reg_atomic_set(&PWM->EN, 1 << slice);

    DEBUG("[pwm]: Init done, frequency set to %ld\n", ret);
    return ret;

}

uint8_t pwm_channels(pwm_t pwm)
{
    (void)pwm;  /* avoid compilation problems with NDEBUG */
    assert(pwm < PWM_NUMOF);
    return PWM_CHANNEL_NUMOF;
}

void pwm_set(pwm_t pwm, uint8_t channel, uint16_t value)
{
    assert((pwm < PWM_NUMOF) && (channel < PWM_CHANNEL_NUMOF));
    uint8_t slice = pwm_config[pwm].pwm_slice;

    /* Set channel compare value */
    if (channel) {
        io_reg_write_dont_corrupt(&pwm_slice(slice)->cc,
                                  (value << PWM_CH0_CC_B_Pos),
                                  PWM_CH0_CC_B_Msk);
    }
    else {
        io_reg_write_dont_corrupt(&pwm_slice(slice)->cc,
                                  (value << PWM_CH0_CC_A_Pos),
                                  PWM_CH0_CC_A_Msk);
    }
}

void pwm_poweron(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    uint8_t slice = pwm_config[pwm].pwm_slice;
    io_reg_atomic_set(&PWM->EN, 1 << slice);
}

void pwm_poweroff(pwm_t pwm)
{
    assert(pwm < PWM_NUMOF);
    uint8_t slice = pwm_config[pwm].pwm_slice;
    io_reg_atomic_clear(&PWM->EN, 1 << slice);
}
