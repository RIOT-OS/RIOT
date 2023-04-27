/*
 * Copyright (C) 2023 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include <errno.h>
#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"

/**
 * @brief   Lock to prevent concurrency issues when used from different threads
 */
static mutex_t lock;

static inline void _prep(void)
{
    mutex_lock(&lock);
}

static inline void _done(void)
{
    mutex_unlock(&lock);
}

static void _disable_digital_func(gpio_t pin)
{

    const gpio_pad_ctrl_t disable_digital_pad = {
        .input_enable = 0,
        .output_disable = 1,
    };

    gpio_set_pad_config(pin, disable_digital_pad);
}

int adc_init(adc_t line)
{
    if (line >= ADC_NUMOF) {
        return -ENODEV;
    }
    /* Lock mutex for exclusive access */
    _prep();
    /* Reset ADC peripheral */
    periph_reset(RESETS_RESET_adc_Msk);
    periph_reset_done(RESETS_RESET_adc_Msk);

    /* Enable ADC peripheral and its clock */
    io_reg_atomic_set(&ADC->CS, 1 << ADC_CS_EN_Pos);
    /* Disable associated GPIO functionality as requested by datasheet */
    _disable_digital_func(adc_config[line].pin);
    /* Unlock mutex */
    _done();
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    int val = 0;

    /* rpx0xx MCU only supports 12 bits resolution */
    if (res != ADC_RES_12BIT) {
        return -EINVAL;
    }
    /* Lock mutex for exclusive access */
    _prep();
    /* Select the channel */
    io_reg_write_dont_corrupt(&ADC->CS, adc_config[line].chan << ADC_CS_AINSEL_Pos,
                              ADC_CS_AINSEL_Msk);
    /* Wait for ADC to be ready */
    while (!(ADC->CS & ADC_CS_READY_Msk)) {}
    /* Trigger one-shot sample */
    io_reg_atomic_set(&ADC->CS, 1 << ADC_CS_START_ONCE_Pos);
    /* Wait for completion */
    while (!(ADC->CS & ADC_CS_READY_Msk)) {}
    /* Get the result */
    val = ADC->RESULT & ADC_RESULT_RESULT_Msk;
    /* Unlock mutex */
    _done();
    return val;
}
