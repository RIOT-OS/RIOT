/*
 * SPDX-FileCopyrightText: 2020 Locha Inc
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_gp2y10xx
 * @{
 *
 * @file
 * @brief       GP2Y10xx Compact Optical Dust Sensor device driver
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <assert.h>

#include "gp2y10xx.h"
#include "gp2y10xx_params.h"

#include "periph/adc.h"
#include "periph/gpio.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define ILED_PULSE_WAIT_US  (280U)
#define ILED_PULSE_OFF_US   (20U)
#define NO_DUST_VOLTAGE     (500)

static inline void _iled_on(const gp2y10xx_t *dev)
{
    gpio_write(dev->params.iled_pin,
               dev->params.iled_level == GP2Y10XX_ILED_LEVEL_HIGH);
}

static inline void _iled_off(const gp2y10xx_t *dev)
{
    gpio_write(dev->params.iled_pin,
               dev->params.iled_level == GP2Y10XX_ILED_LEVEL_LOW);
}

static inline int _adc_resolution(adc_res_t res)
{
    /* get the resolution the ADC can read */
    int exp = 0;
    switch (res) {
        case ADC_RES_6BIT:
            exp = 6;
            break;
        case ADC_RES_8BIT:
            exp = 8;
            break;
        case ADC_RES_10BIT:
            exp = 10;
            break;
        case ADC_RES_12BIT:
            exp = 12;
            break;
        case ADC_RES_14BIT:
            exp = 14;
            break;
        case ADC_RES_16BIT:
            exp = 16;
            break;
        default:
            assert(0);
            break;
    }

    return exp;
}

int gp2y10xx_init(gp2y10xx_t *dev, const gp2y10xx_params_t *params)
{
    assert(dev && params);

    dev->params = *params;
    if (adc_init(dev->params.aout) < 0) {
        return GP2Y10XX_ERR_ADC;
    }

    if (gpio_init(dev->params.iled_pin, GPIO_OUT) < 0) {
        return GP2Y10XX_ERR_ILED;
    }
    _iled_off(dev);

    return GP2Y10XX_OK;
}

int gp2y10xx_read_density(const gp2y10xx_t *dev, uint16_t *density)
{
    uint32_t adc_sum = 0;
    int32_t adc_value;
    uint32_t voltage;

    assert(dev && density);

    for (unsigned i = 0; i < CONFIG_GP2Y10XX_MAX_READINGS; i++) {
        /* turn ILED on/off and wait a little bit to read the ADC */
        _iled_on(dev);
        xtimer_usleep(ILED_PULSE_WAIT_US);
        if ((adc_value = adc_sample(dev->params.aout,
                                    dev->params.adc_res)) < 0) {
            _iled_off(dev);
            return GP2Y10XX_ERR_ADC;
        }
        _iled_off(dev);
        xtimer_usleep(ILED_PULSE_OFF_US);
        DEBUG("[gp2y10xx] read: unfiltered adc_value=%"PRIi32"\n", adc_value);

        adc_sum += adc_value;
    }
    /* calculate the average between all readings */
    adc_value = adc_sum / CONFIG_GP2Y10XX_MAX_READINGS;
    DEBUG("[gp2y10xx] read: filtered adc_value=%"PRIi32"\n", adc_value);

    /* convert ADC reading to a voltage */
    voltage = (dev->params.vref * adc_value);
    voltage >>= _adc_resolution(dev->params.adc_res);
    /* check if the voltage provides us meaningful data */
    if (voltage <= NO_DUST_VOLTAGE) {
        *density = 0;
    }
    else {
        voltage -= NO_DUST_VOLTAGE;
    }
    DEBUG("[gp2y10xx] read: voltage=%"PRIi32"\n", voltage);
    /* multiply by the magic eleven.
     *
     * XXX: find out why 11 is magic and the shenanigans behind it
     */
    voltage *= 11;

    /* convert "voltage" to ug/m3 */
    *density = (uint16_t)((voltage * 2) / 10);

    return GP2Y10XX_OK;
}
