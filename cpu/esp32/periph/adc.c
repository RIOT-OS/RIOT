/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * All ESP32x SoCs have two SAR ADC units each. However, these have
 * functionalities as well as specific properties that vary between the
 * ESP32x SoC and therefore require different handling for each ESP32x SoC.
 * This is already taken into account in the high-level API of the ESP-IDF.
 * To avoid having to reimplement these specifics and the different handling,
 * the high-level API of the ESP-IDF is used directly for the ADC peripherals.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>

#include "board.h"
#include "periph/adc.h"

#include "adc_arch.h"
#include "adc_arch_private.h"
#include "esp_common.h"
#include "gpio_arch.h"

#include "driver/adc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* forward declarations of internal functions */
static bool _adc_conf_check(void);
static void _adc1_ctrl_init(void);
static void _adc2_ctrl_init(void);

/* external variable declarations */
extern const gpio_t _gpio_rtcio_map[];

/*
 * Structure for mapping RIOT's ADC resolutions to ESP-IDF resolutions
 * of the according ESP32x SoC.
 */
typedef struct {
    adc_bits_width_t res;   /* used ESP-IDF resolution */
    unsigned         shift; /* bit shift number for results */
} _adc_esp_res_map_t;

/*
 * Table for resolution mapping
 */
_adc_esp_res_map_t _adc_esp_res_map[] =  {
#if defined(CPU_FAM_ESP32)
    { .res = ADC_WIDTH_BIT_9,  .shift = 3 },    /* ADC_RES_6BIT  */
    { .res = ADC_WIDTH_BIT_9,  .shift = 1 },    /* ADC_RES_8BIT  */
    { .res = ADC_WIDTH_BIT_10, .shift = 0 },    /* ADC_RES_10BIT */
    { .res = ADC_WIDTH_BIT_12, .shift = 0 },    /* ADC_RES_12BIT */
    { .res = ADC_WIDTH_MAX },                   /* ADC_RES_14BIT */
    { .res = ADC_WIDTH_MAX },                   /* ADC_RES_16BIT */
#elif SOC_ADC_MAX_BITWIDTH == 12
    { .res = ADC_WIDTH_BIT_12, .shift = 6 },    /* ADC_RES_6BIT  */
    { .res = ADC_WIDTH_BIT_12, .shift = 4 },    /* ADC_RES_8BIT  */
    { .res = ADC_WIDTH_BIT_12, .shift = 2 },    /* ADC_RES_10BIT */
    { .res = ADC_WIDTH_BIT_12, .shift = 0 },    /* ADC_RES_12BIT */
    { .res = ADC_WIDTH_MAX },                   /* ADC_RES_14BIT */
    { .res = ADC_WIDTH_MAX },                   /* ADC_RES_16BIT */
#elif SOC_ADC_MAX_BITWIDTH == 13
    { .res = ADC_WIDTH_BIT_13, .shift = 7 },    /* ADC_RES_6BIT  */
    { .res = ADC_WIDTH_BIT_13, .shift = 5 },    /* ADC_RES_8BIT  */
    { .res = ADC_WIDTH_BIT_13, .shift = 3 },    /* ADC_RES_10BIT */
    { .res = ADC_WIDTH_BIT_13, .shift = 1 },    /* ADC_RES_12BIT */
    { .res = ADC_WIDTH_MAX },                   /* ADC_RES_14BIT */
    { .res = ADC_WIDTH_MAX },                   /* ADC_RES_16BIT */
#endif
};

static bool _adc_module_initialized = false;

static inline void _adc1_ctrl_init(void)
{
    /* nothing to do for the moment */
}

static inline void _adc2_ctrl_init(void)
{
    /* nothing to do for the moment */
}

int adc_init(adc_t line)
{
    DEBUG("[adc] line=%u\n", line);

    if (line >= ADC_NUMOF) {
        return -1;
    }

    if (!_adc_module_initialized) {
        /* do some configuration checks */
        if (!_adc_conf_check()) {
            return -1;
        }
        _adc_module_initialized = true;
    }

    /* get the RTCIO pin number for the given GPIO defined as ADC channel */
    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];

    /* check whether the GPIO is avalid ADC channel pin */
    if (rtcio == RTCIO_NA) {
        return -1;
    }

    /* check whether the pin is not used for other purposes */
    if (gpio_get_pin_usage(_adc_hw[rtcio].gpio) != _GPIO) {
        LOG_TAG_ERROR("adc", "GPIO%d is used for %s and cannot be used as "
                      "ADC input\n", _adc_hw[rtcio].gpio,
                      gpio_get_pin_usage_str(_adc_hw[rtcio].gpio));
        return -1;
    }

    if (_adc_hw[rtcio].adc_ctrl == ADC_UNIT_1) {
        /* ensure compatibility of given adc_channel_t with adc1_channel_t */
        assert(_adc_hw[rtcio].adc_channel < (adc_channel_t)ADC1_CHANNEL_MAX);
        /* initialize the ADC1 unit if needed */
        _adc1_ctrl_init();
        /* set the attenuation and configure its associated GPIO pin mux */
        adc1_config_channel_atten((adc1_channel_t)_adc_hw[rtcio].adc_channel,
                                  ADC_ATTEN_DB_11);
    }
    else if (_adc_hw[rtcio].adc_ctrl == ADC_UNIT_2) {
        /* ensure compatibility of given adc_channel_t with adc2_channel_t */
        assert(_adc_hw[rtcio].adc_channel < (adc_channel_t)ADC2_CHANNEL_MAX);
        /* initialize the ADC2 unit if needed */
        _adc2_ctrl_init();
        /* set the attenuation and configure its associated GPIO pin mux */
        adc2_config_channel_atten((adc2_channel_t)_adc_hw[rtcio].adc_channel,
                                  ADC_ATTEN_DB_11);
    }
    else {
        return -1;
    }

    /* set pin usage type */
    gpio_set_pin_usage(_adc_hw[rtcio].gpio, _ADC);

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    DEBUG("[adc] line=%u res=%u\n", line, res);

    if (_adc_esp_res_map[res].res == ADC_WIDTH_MAX) {
        return -1;
    }

    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];
    int raw;

    if (_adc_hw[rtcio].adc_ctrl == ADC_UNIT_1) {
        adc1_config_width(_adc_esp_res_map[res].res);
         /* ensure compatibility of given adc_channel_t with adc1_channel_t */
        assert(_adc_hw[rtcio].adc_channel < (adc_channel_t)ADC1_CHANNEL_MAX);
        raw = adc1_get_raw((adc1_channel_t)_adc_hw[rtcio].adc_channel);
        if (raw < 0) {
            return -1;
        }
    }
    else if (_adc_hw[rtcio].adc_ctrl == ADC_UNIT_2) {
         /* ensure compatibility of given adc_channel_t with adc2_channel_t */
        assert(_adc_hw[rtcio].adc_channel < (adc_channel_t)ADC2_CHANNEL_MAX);
        if (adc2_get_raw((adc2_channel_t)_adc_hw[rtcio].adc_channel,
                         _adc_esp_res_map[res].res, &raw) < 0) {
            return -1;
        }
    }

    return raw >> _adc_esp_res_map[res].shift;
}

int adc_set_attenuation(adc_t line, adc_atten_t atten)
{
    DEBUG("[adc] line=%u atten=%u\n", line, atten);

    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];

    assert(rtcio != RTCIO_NA);

    if (_adc_hw[rtcio].adc_ctrl == ADC_UNIT_1) {
         /* ensure compatibility of given adc_channel_t with adc1_channel_t */
        assert(_adc_hw[rtcio].adc_channel < (adc_channel_t)ADC1_CHANNEL_MAX);
        return adc1_config_channel_atten((adc1_channel_t)_adc_hw[rtcio].adc_channel, atten);
    }
    else if (_adc_hw[rtcio].adc_ctrl == ADC_UNIT_2) {
         /* ensure compatibility of given adc_channel_t with adc2_channel_t */
        assert(_adc_hw[rtcio].adc_channel < (adc_channel_t)ADC2_CHANNEL_MAX);
        return adc2_config_channel_atten((adc2_channel_t)_adc_hw[rtcio].adc_channel, atten);
    }

    return -1;
}

int adc_line_vref_to_gpio(adc_t line, gpio_t gpio)
{
    uint8_t rtcio_vref = _gpio_rtcio_map[adc_channels[line]];
    uint8_t rtcio_out = _gpio_rtcio_map[gpio];

    /* both the ADC line and the GPIO for the output must be ADC channels */
    assert(rtcio_vref != RTCIO_NA);
    assert(rtcio_out != RTCIO_NA);
    /* avoid compilation problems with NDEBUG defined */
    (void)rtcio_out;

    /* the GPIO for the output must be a channel of ADC2 */
    assert(_adc_hw[rtcio_out].adc_ctrl == ADC_UNIT_2);
    /* given ADC line has to be a channel of ADC2  */
    assert(_adc_hw[rtcio_vref].adc_ctrl == ADC_UNIT_2);

    esp_err_t res = ESP_OK;

    if (_adc_hw[rtcio_vref].adc_ctrl == ADC_UNIT_1) {
        res = adc_vref_to_gpio(ADC_UNIT_1, gpio);
    }
    else if (_adc_hw[rtcio_vref].adc_ctrl == ADC_UNIT_2) {
        res = adc_vref_to_gpio(ADC_UNIT_2, gpio);
    }
    if (res != ESP_OK) {
        LOG_TAG_ERROR("adc", "Could not route Vref of ADC line %d to GPIO%d\n",
                      line, gpio);
        return -1;
    }
    else {
        LOG_TAG_ERROR("adc", "Vref of ADC%d can now be measured at GPIO %d\n",
                      _adc_hw[rtcio_vref].adc_ctrl, gpio);
        return 0;
    }
}

static bool _adc_conf_check(void)
{
    for (unsigned i = 0; i < ADC_NUMOF; i++) {
        if (_gpio_rtcio_map[adc_channels[i]] == RTCIO_NA) {
            LOG_TAG_ERROR("adc", "GPIO%d cannot be used as ADC line\n",
                          adc_channels[i]);
            return false;
        }
    }

    return true;
}

void adc_print_config(void)
{
    printf("\tADC\t\tpins=[ ");
#if defined(ADC_GPIOS)
    for (unsigned i = 0; i < ADC_NUMOF; i++) {
        printf("%d ", adc_channels[i]);
    }
#endif /* defined(ADC_GPIOS) */
    printf("]\n");
}
