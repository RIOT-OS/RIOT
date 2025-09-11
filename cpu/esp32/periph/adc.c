/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <errno.h>

#include "board.h"
#include "periph/adc.h"

#include "adc_arch.h"
#include "adc_arch_private.h"
#include "esp_common.h"
#include "gpio_arch.h"

#include "esp_adc/adc_oneshot.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "hal/adc_hal_common.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* external variable declarations */
extern const gpio_t _gpio_rtcio_map[];
extern const int _adc_vrange_min_max[];

/*
 * Structure for mapping RIOT's ADC resolutions to ESP-IDF resolutions
 * of the according ESP32x SoC.
 */
typedef struct {
    adc_bitwidth_t hw_res;  /* used ESP-IDF resolution */
    unsigned       shift;   /* bit shift number for results */
} _adc_esp_res_map_t;

#define ADC_BITWIDTH_MAX   ((adc_bitwidth_t)255)

/*
 * Table for resolution mapping
 * With the exception of the ESP32, all ESP32x SoCs only support one resolution.
 * Therefore, we only use this one resolution and shift the results accordingly.
 */
static const _adc_esp_res_map_t _adc_esp_res_map[] =  {
#if SOC_ADC_RTC_MAX_BITWIDTH == 12
    { .hw_res = ADC_BITWIDTH_12, .shift = 6 },     /* ADC_RES_6BIT  */
    { .hw_res = ADC_BITWIDTH_12, .shift = 4 },     /* ADC_RES_8BIT  */
    { .hw_res = ADC_BITWIDTH_12, .shift = 2 },     /* ADC_RES_10BIT */
    { .hw_res = ADC_BITWIDTH_12, .shift = 0 },     /* ADC_RES_12BIT */
    { .hw_res = ADC_BITWIDTH_MAX },                /* ADC_RES_14BIT */
    { .hw_res = ADC_BITWIDTH_MAX },                /* ADC_RES_16BIT */
#elif SOC_ADC_RTC_MAX_BITWIDTH == 13
    { .hw_res = ADC_BITWIDTH_13, .shift = 7 },     /* ADC_RES_6BIT  */
    { .hw_res = ADC_BITWIDTH_13, .shift = 5 },     /* ADC_RES_8BIT  */
    { .hw_res = ADC_BITWIDTH_13, .shift = 3 },     /* ADC_RES_10BIT */
    { .hw_res = ADC_BITWIDTH_13, .shift = 1 },     /* ADC_RES_12BIT */
    { .hw_res = ADC_BITWIDTH_MAX },                /* ADC_RES_14BIT */
    { .hw_res = ADC_BITWIDTH_MAX },                /* ADC_RES_16BIT */
#endif
};

static adc_oneshot_unit_handle_t _adc_handle[SOC_ADC_PERIPH_NUM] = { };

static adc_oneshot_chan_cfg_t _adc_channel_cfg[ADC_NUMOF] = { };
static adc_cali_handle_t _adc_channel_cali[ADC_NUMOF] = { };
static const _adc_esp_res_map_t *_adc_channel_res[ADC_NUMOF] = { };

static bool _adc_driver_initialized = false;

/* forward declarations of internal functions */
static void _adc_driver_init(void);
static void _adc_channel_config(adc_t line, adc_bitwidth_t width, adc_atten_t att);
static void _adc_ctrl_init(adc_unit_t unit);

int adc_init(adc_t line)
{
    DEBUG("[adc] %s: line=%u\n", __func__, line);

    assert(line < ADC_NUMOF);

    /* initialize the ADC driver if needed including configuration sanity checks */
    _adc_driver_init();

    /* get the RTCIO pin number for the given GPIO defined as ADC channel */
    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];

    /* check whether the pin is not used for other purposes */
    if (gpio_get_pin_usage(_adc_hw[rtcio].gpio) != _GPIO) {
        LOG_TAG_ERROR("adc", "GPIO%d is used for %s and cannot be used as "
                      "ADC input\n", _adc_hw[rtcio].gpio,
                      gpio_get_pin_usage_str(_adc_hw[rtcio].gpio));
        return -1;
    }

    /* initialize the corresponding ADC unit if needed */
    _adc_ctrl_init(_adc_hw[rtcio].adc_ctrl);

    /* set pin usage type */
    gpio_set_pin_usage(_adc_hw[rtcio].gpio, _ADC);

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    DEBUG("[adc] %s: line=%u res=%u\n", __func__, line, res);

    assert(_adc_driver_initialized);
    assert(line < ADC_NUMOF);

    if (_adc_esp_res_map[res].hw_res == ADC_BITWIDTH_MAX) {
        return -1;
    }

    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];
    assert(rtcio != RTCIO_NA);

    if (_adc_channel_res[line] != &_adc_esp_res_map[res]) {
        /* reconfigure the channel if the resolution is changed */
        _adc_channel_res[line] = &_adc_esp_res_map[res];
        _adc_channel_config(line, _adc_esp_res_map[res].hw_res,
                                  _adc_channel_cfg[line].atten);
    }

    int raw = 0;
    if (adc_oneshot_read(_adc_handle[_adc_hw[rtcio].adc_ctrl],
                         _adc_hw[rtcio].adc_channel, &raw) != ESP_OK) {
        return -1;
    }

    return raw >> _adc_esp_res_map[res].shift;
}

int adc_set_attenuation(adc_t line, adc_atten_t atten)
{
    DEBUG("[adc] %s: line=%u atten=%u\n", __func__, line, atten);

    assert(_adc_driver_initialized);
    assert(line < ADC_NUMOF);
    assert(atten < SOC_ADC_ATTEN_NUM);

    _adc_channel_config(line, _adc_channel_cfg[line].bitwidth, atten);

    return 0;
}

int adc_raw_to_voltage(adc_t line, int raw, int *voltage)
{
    DEBUG("[adc] %s: line=%u\n", __func__, line);

    assert(line < ADC_NUMOF);
    assert(voltage);
    assert(_adc_channel_res[line]);

    /* scale the raw value to the resolution used internally */
    raw = raw << _adc_channel_res[line]->shift;

    if (_adc_channel_cali[line]) {
        adc_cali_raw_to_voltage(_adc_channel_cali[line], raw, voltage);
        return 0;
    }
    else {
        /* default values for the case the calibration does not work */
        int min = _adc_vrange_min_max[(_adc_channel_cfg[line].atten << 1)];
        int max = _adc_vrange_min_max[(_adc_channel_cfg[line].atten << 1) + 1];

        *voltage = raw * (max - min) / 4096 + min;
        return -EINVAL;
    }
}

__attribute__((__deprecated__))
int adc_line_vref_to_gpio(adc_t line, gpio_t gpio)
{
    LOG_TAG_ERROR("adc", "%s is no longer supported\n", __func__);
    assert(0);
    return 0;
}

static void _adc_driver_init(void)
{
    if (_adc_driver_initialized) {
        return;
    }

    /* do some configuration checks */
    for (unsigned i = 0; i < ADC_NUMOF; i++) {
        uint8_t rtcio = _gpio_rtcio_map[adc_channels[i]];

        /* check whether the GPIO is a valid ADC channel pin */
        if (rtcio == RTCIO_NA) {
            LOG_TAG_ERROR("adc", "GPIO%d cannot be configured as ADC line\n",
                          adc_channels[i]);
            assert(0);
        }
        if (_adc_hw[rtcio].adc_ctrl >= SOC_ADC_PERIPH_NUM) {
            LOG_TAG_ERROR("adc", "Invalid ADC unit %u for ADC line %u\n",
                          _adc_hw[rtcio].adc_ctrl, adc_channels[i]);
            assert(0);
        }
        if (_adc_hw[rtcio].adc_channel >= ADC_CHANNEL_MAX(_adc_hw[rtcio].adc_ctrl)) {
            LOG_TAG_ERROR("adc", "Invalid ADC channel %u for ADC line %u\n",
                          _adc_hw[rtcio].adc_channel, adc_channels[i]);
            assert(0);
        }
#if CPU_FAM_ESP32C3
        /* According to the ESP32-C3 Errata Sheet ADC2 does not work correctly. To
         * use ADC2 and GPIO5 as ADC channel, CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3
         * has to be set (default). */
        if (_adc_hw[rtcio].adc_ctrl == ADC_UNIT_2) {
            if (!IS_ACTIVE(CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3)) {
                LOG_TAG_ERROR("adc", "ADC2 cannot be used on ESP32-C3 if "
                                     "CONFIG_ADC_ONESHOT_FORCE_USE_ADC2_ON_C3 "
                                     "is not set (ADC line %u)\n", i);
                assert(0);
            }
            /* throw at least a warning */
            LOG_TAG_WARNING("adc", "ADC line %u on ADC2 may not work correctly "
                                   "on ESP32-C3, see errata sheet\n", i);
        }
#endif
    }

    /* set the default bitwidth and the attenuation for all channels to 12 dB */
    for (unsigned i = 0; i < ADC_NUMOF; i++) {
        _adc_channel_cfg[i].bitwidth = ADC_BITWIDTH_DEFAULT;
        _adc_channel_cfg[i].atten = ADC_ATTEN_DB_12;
    }

    _adc_driver_initialized = true;
}

static void _adc_ctrl_init(adc_unit_t unit)
{
    DEBUG("[adc] %s: unit=%u\n", __func__, unit);

    assert(unit < SOC_ADC_PERIPH_NUM);

    if (_adc_handle[unit] != NULL) {
        /* unit is already initialized */
        return;
    }

    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = unit,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    if (adc_oneshot_new_unit(&unit_cfg, &_adc_handle[unit]) != ESP_OK) {
        LOG_TAG_ERROR("adc", "Could not initialize ADC unit %u\n", unit);
    }
}

static void _adc_channel_config(adc_t line, adc_bitwidth_t width, adc_atten_t att)
{
    DEBUG("[adc] %s: line=%u width=%d atten=%u\n", __func__, line, width, att);

    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];

    _adc_channel_cfg[line].bitwidth = width;
    _adc_channel_cfg[line].atten = att;

    if (adc_oneshot_config_channel(_adc_handle[_adc_hw[rtcio].adc_ctrl],
                                   _adc_hw[rtcio].adc_channel,
                                   &_adc_channel_cfg[line]) != ESP_OK) {
        LOG_TAG_ERROR("adc", "Could not config ADC line %u\n", line);
        return;
    }

    if (_adc_channel_cali[line]) {
        /* delete allocated calibration scheme if necessary */
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
        adc_cali_delete_scheme_curve_fitting(_adc_channel_cali[line]);
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
        adc_cali_delete_scheme_line_fitting(_adc_channel_cali[line]);
#endif
        _adc_channel_cali[line] = NULL;
    }

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cfg = {
        .unit_id = _adc_hw[rtcio].adc_ctrl,
        .chan = _adc_hw[rtcio].adc_channel,
        .atten = _adc_channel_cfg[line].atten,
        .bitwidth = _adc_channel_cfg[line].bitwidth,
    };
    int res = adc_cali_create_scheme_curve_fitting(&cfg, &_adc_channel_cali[line]);
    if (res != ESP_OK) {
        DEBUG("[adc] %s: init calibration scheme failed %d\n", __func__, res);
        return;
    }
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t cfg = {
        .unit_id = _adc_hw[rtcio].adc_ctrl,
        .atten = _adc_channel_cfg[line].atten,
        .bitwidth = _adc_channel_cfg[line].bitwidth,
#if CONFIG_IDF_TARGET_ESP32
        .default_vref = 1100,
#endif
    };
    int res = adc_cali_create_scheme_line_fitting(&cfg, &_adc_channel_cali[line]);
    if (res != ESP_OK) {
        DEBUG("[adc] %s: init calibration scheme failed %d\n", __func__, res);
        return;
    }
#endif
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
