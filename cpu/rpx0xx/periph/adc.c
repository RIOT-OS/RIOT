/*
 * SPDX-FileCopyrightText: 2023 Mesotic SAS
 * SPDX-License-Identifier: LGPL-2.1-only
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
    periph_reset(RESETS_RESET_ADC_BITS);
    periph_reset_done(RESETS_RESET_ADC_BITS);

    /* Enable ADC peripheral and its clock */
    io_reg_atomic_set(&ADC->CS, 1 << ADC_CS_EN_LSB);
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
    io_reg_write_dont_corrupt(&ADC->CS, adc_config[line].chan << ADC_CS_AINSEL_LSB,
                              ADC_CS_AINSEL_BITS);
    /* Wait for ADC to be ready */
    while (!(ADC->CS & ADC_CS_READY_BITS)) {}
    /* Trigger one-shot sample */
    io_reg_atomic_set(&ADC->CS, 1 << ADC_CS_START_ONCE_LSB);
    /* Wait for completion */
    while (!(ADC->CS & ADC_CS_READY_BITS)) {}
    /* Get the result */
    val = ADC->RESULT & ADC_RESULT_BITS;
    /* Unlock mutex */
    _done();
    return val;
}
