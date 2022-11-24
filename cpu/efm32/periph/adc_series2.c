/*
 * Copyright (C) 2022 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"

#include "periph_conf.h"
#include "periph/adc.h"
#include "periph/gpio.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_iadc.h"

static mutex_t adc_lock[ADC_DEV_NUMOF];

int adc_init(adc_t line)
{
    assert(line < ADC_NUMOF);
    uint8_t dev = adc_channel_config[line].dev;
    assert(dev < ADC_DEV_NUMOF);

    /* initialize lock */
    mutex_init(&adc_lock[dev]);

    /* enable clock */
    CMU_ClockEnable(adc_config[dev].cmu, true);

    /* make sure we're in a known state */
    IADC_reset(adc_config[dev].dev);

    /* init IADC periph */
    const IADC_Init_t init = IADC_INIT_DEFAULT;
    IADC_AllConfigs_t configs = { 0 };
    for (size_t i = 0; i < IADC0_CONFIGNUM; i++) {
        configs.configs[i].adcMode = iadcCfgModeNormal;
        configs.configs[i].osrHighSpeed = ADC_MODE_OSR(adc_config[dev].available_res[i]);
        configs.configs[i].digAvg = ADC_MODE_AVG(adc_config[dev].available_res[i]);
        configs.configs[i].analogGain = adc_config[dev].gain;
        configs.configs[i].reference = adc_config[dev].reference;
        configs.configs[i].vRef = adc_config[dev].reference_mV;
        configs.configs[i].twosComplement = iadcCfgTwosCompUnipolar;
    }
    IADC_init(adc_config[dev].dev, &init, &configs);

    return 0;
}

static inline GPIO_Port_TypeDef _port_num(gpio_t pin)
{
    return ((pin & 0xf0) >> 4);
}

static inline uint8_t _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

static void _setup_abus(gpio_t pin) {
    const uint32_t alloc = GPIO_ABUSALLOC_AEVEN0_ADC0
                         | GPIO_ABUSALLOC_AODD0_ADC0;
    switch (_port_num(pin)) {
        case gpioPortA:
            GPIO->ABUSALLOC = alloc;
            break;
        case gpioPortB:
            GPIO->BBUSALLOC = alloc;
            break;
        case gpioPortC:
        case gpioPortD:
            GPIO->CDBUSALLOC = alloc;
            break;
    }
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    assert(line < ADC_NUMOF);
    uint8_t dev = adc_channel_config[line].dev;

    /* find config to given resolution */
    uint8_t config;
    for (config = 0; config < IADC0_CONFIGNUM; config++) {
        if (adc_config[dev].available_res[config] == res) {
            /* we found the corresponding config */
            break;
        }
    }
    /* unsopported resolution */
    if (config >= IADC0_CONFIGNUM) {
        return -1;
    }

    /* lock device */
    mutex_lock(&adc_lock[dev]);

    /* setup channel and start sampling */
    static const IADC_InitSingle_t init = {
        .alignment = iadcAlignRight20,
        .showId = false,
        .dataValidLevel = iadcFifoCfgDvl4,
        .fifoDmaWakeup = false,
        .triggerSelect = iadcTriggerSelImmediate,
        .triggerAction = iadcTriggerActionOnce,
        .start = true
    };
    IADC_SingleInput_t input = IADC_SINGLEINPUT_DEFAULT;
    input.configId = config;
    input.posInput = IADC_portPinToPosInput(_port_num(adc_channel_config[line].input_pos),
                                            _pin_num(adc_channel_config[line].input_pos));
    _setup_abus(adc_channel_config[line].input_pos);
    if (adc_channel_config[line].input_neg != GPIO_UNDEF) {
        input.negInput = IADC_portPinToNegInput(_port_num(adc_channel_config[line].input_neg),
                                                _pin_num(adc_channel_config[line].input_neg));
        _setup_abus(adc_channel_config[line].input_neg);
    }

    IADC_initSingle(adc_config[dev].dev, &init, &input);

    /* wait for the conservation to provide the result in the fifo */
    while ((IADC_getStatus(adc_config[dev].dev) & IADC_STATUS_SINGLEFIFODV) == 0) {}
    uint32_t result = IADC_pullSingleFifoData(adc_config[dev].dev);

    /* unlock device */
    mutex_unlock(&adc_lock[dev]);

    return result >> (20 - ADC_MODE_RES(res));
}
