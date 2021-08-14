/*
 * Copyright (C) 2018 Gunar Schorcht
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
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "board.h"
#include "periph/adc.h"

#include "adc_arch.h"
#include "adc_ctrl.h"
#include "esp_common.h"
#include "gpio_arch.h"
#include "soc/rtc_io_struct.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* declaration of external functions */
extern void _adc1_ctrl_init(void);
extern void _adc2_ctrl_init(void);

/* forward declarations of internal functions */
static bool _adc_conf_check(void);
static void _adc_module_init(void);
static bool _adc_module_initialized  = false;

/* external variable declarations */
extern const gpio_t _gpio_rtcio_map[];

int adc_init(adc_t line)
{
    CHECK_PARAM_RET (line < ADC_NUMOF, -1)

    if (!_adc_module_initialized) {
        /* do some configuration checks */
        if (!_adc_conf_check()) {
            return -1;
        }
        _adc_module_init();
        _adc_module_initialized = true;
    }

    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];

    if (_adc_hw[rtcio].adc_ctrl == ADC1_CTRL) {
        _adc1_ctrl_init();
    }
    if (_adc_hw[rtcio].adc_ctrl == ADC2_CTRL) {
        _adc2_ctrl_init();
    }

    /* try to initialize the pin as ADC input */
    if (gpio_get_pin_usage(_adc_hw[rtcio].gpio) != _GPIO) {
        LOG_TAG_ERROR("adc", "GPIO%d is used for %s and cannot be used as "
                      "ADC input\n", _adc_hw[rtcio].gpio,
                      gpio_get_pin_usage_str(_adc_hw[rtcio].gpio));
        return -1;
    }

    uint8_t idx;

    /* disable the pad output */
    RTCIO.enable_w1tc.val = BIT(_adc_hw[rtcio].rtc_gpio);

    /* route pads to RTC and if possible, disable input, pull-up/pull-down */
    switch (rtcio) {
        case RTCIO_SENSOR_SENSE1: /* GPIO36, RTC0 */
                RTCIO.sensor_pads.sense1_mux_sel = 1; /* route to RTC */
                RTCIO.sensor_pads.sense1_fun_sel = 0; /* function ADC1_CH0 */
                break;
        case RTCIO_SENSOR_SENSE2: /* GPIO37, RTC1 */
                RTCIO.sensor_pads.sense2_mux_sel = 1; /* route to RTC */
                RTCIO.sensor_pads.sense2_fun_sel = 0; /* function ADC1_CH1 */
                break;
        case RTCIO_SENSOR_SENSE3: /* GPIO38, RTC2 */
                RTCIO.sensor_pads.sense3_mux_sel = 1; /* route to RTC */
                RTCIO.sensor_pads.sense3_fun_sel = 0; /* function ADC1_CH2 */
                break;
        case RTCIO_SENSOR_SENSE4: /* GPIO39, RTC3 */
                RTCIO.sensor_pads.sense4_mux_sel = 1; /* route to RTC */
                RTCIO.sensor_pads.sense4_fun_sel = 0; /* function ADC1_CH3 */
                break;

        case RTCIO_TOUCH0: /* GPIO4, RTC10 */
        case RTCIO_TOUCH1: /* GPIO0, RTC11 */
        case RTCIO_TOUCH2: /* GPIO2, RTC12 */
        case RTCIO_TOUCH3: /* GPIO15, RTC13 */
        case RTCIO_TOUCH4: /* GPIO13, RTC14 */
        case RTCIO_TOUCH5: /* GPIO12, RTC15 */
        case RTCIO_TOUCH6: /* GPIO14, RTC16 */
        case RTCIO_TOUCH7: /* GPIO27, RTC17 */
        case RTCIO_TOUCH8: /* GPIO33, RTC8 */
        case RTCIO_TOUCH9: /* GPIO32, RTC9 */
                idx = rtcio - RTCIO_TOUCH0;
                RTCIO.touch_pad[idx].mux_sel = 1; /* route to RTC */
                RTCIO.touch_pad[idx].fun_sel = 0; /* function ADC2_CH0..ADC2_CH9 */
                RTCIO.touch_pad[idx].fun_ie = 0;  /* input disabled */
                RTCIO.touch_pad[idx].rue = 0;     /* pull-up disabled */
                RTCIO.touch_pad[idx].rde = 0;     /* pull-down disabled */
                RTCIO.touch_pad[idx].xpd = 0;     /* touch sensor powered off */
                break;

        case RTCIO_ADC_ADC1: /* GPIO34, RTC4 */
                RTCIO.adc_pad.adc1_mux_sel = 1; /* route to RTC */
                RTCIO.adc_pad.adc1_fun_sel = 0; /* function ADC1_CH6 */
                break;
        case RTCIO_ADC_ADC2: /* GPIO35, RTC5 */
                RTCIO.adc_pad.adc2_mux_sel = 1; /* route to RTC */
                RTCIO.adc_pad.adc2_fun_sel = 0; /* function ADC1_CH7 */
                break;

        case RTCIO_DAC1: /* GPIO25, RTC6 */
        case RTCIO_DAC2: /* GPIO26, RTC7 */
                idx = rtcio - RTCIO_DAC1;
                RTCIO.pad_dac[idx].mux_sel = 1; /* route to RTC */
                RTCIO.pad_dac[idx].fun_sel = 0; /* function ADC2_CH8, ADC2_CH9 */
                RTCIO.pad_dac[idx].fun_ie = 0;  /* input disabled */
                RTCIO.pad_dac[idx].rue = 0;     /* pull-up disabled */
                RTCIO.pad_dac[idx].rde = 0;     /* pull-down disabled */
                RTCIO.pad_dac[idx].xpd_dac = 0; /* DAC powered off */
                break;

        default: return -1;
    }

    /* set pin usage type  */
    gpio_set_pin_usage(_adc_hw[rtcio].gpio, _ADC);

    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    CHECK_PARAM_RET (line < ADC_NUMOF, -1)
    CHECK_PARAM_RET (res <= ADC_RES_12BIT, -1)

    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];

    if (_adc_hw[rtcio].adc_ctrl == ADC1_CTRL) {
        /* set the resolution for the measurement */
        SENS.sar_start_force.sar1_bit_width = res;
        SENS.sar_read_ctrl.sar1_sample_bit = res;

        /* enable the pad in the pad enable bitmap */
        SENS.sar_meas_start1.sar1_en_pad = (1 << _adc_hw[rtcio].adc_channel);
        while (SENS.sar_slave_addr1.meas_status != 0) {}

        /* start measurement by toggling the start bit and wait until the
           measurement has been finished */
        SENS.sar_meas_start1.meas1_start_sar = 0;
        SENS.sar_meas_start1.meas1_start_sar = 1;
        while (SENS.sar_meas_start1.meas1_done_sar == 0) {}

        /* read out the result and return */
        return SENS.sar_meas_start1.meas1_data_sar;
    }
    else {
        /* set the resolution for the measurement */
        SENS.sar_start_force.sar2_bit_width = res;
        SENS.sar_read_ctrl2.sar2_sample_bit = res;

        /* enable the pad in the pad enable bitmap */
        SENS.sar_meas_start2.sar2_en_pad = (1 << _adc_hw[rtcio].adc_channel);

        /* start measurement by toggling the start bit and wait until the
           measurement has been finished */
        SENS.sar_meas_start2.meas2_start_sar = 0;
        SENS.sar_meas_start2.meas2_start_sar = 1;
        while (SENS.sar_meas_start2.meas2_done_sar == 0) {}

        /* read out the result and return */
        return SENS.sar_meas_start2.meas2_data_sar;
    }
}

int adc_set_attenuation(adc_t line, adc_attenuation_t atten)
{
    CHECK_PARAM_RET (line < ADC_NUMOF, -1)

    uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];

    if (_adc_hw[rtcio].adc_ctrl == ADC1_CTRL) {
        SENS.sar_atten1 &= ~(0x3 << (_adc_hw[rtcio].adc_channel << 1));
        SENS.sar_atten1 |= (atten << (_adc_hw[rtcio].adc_channel << 1));
    }
    else {
        SENS.sar_atten2 &= ~(0x3 << (_adc_hw[rtcio].adc_channel << 1));
        SENS.sar_atten2 |= (atten << (_adc_hw[rtcio].adc_channel << 1));
    }
    return 0;
}

int adc_vref_to_gpio25 (void)
{
    /* determine ADC line for GPIO25 */
    adc_t line = ADC_UNDEF;
    for (unsigned i = 0; i < ADC_NUMOF; i++) { \
        if (adc_channels[i] == GPIO25) { \
            line = i;
            break;
        }
    }

    if (line == ADC_UNDEF) {
        LOG_TAG_ERROR("adc", "Have no ADC line for GPIO25\n");
        return -1;
    }

    if (adc_init(line) == 0)
    {
        uint8_t rtcio = _gpio_rtcio_map[adc_channels[line]];
        RTCCNTL.bias_conf.dbg_atten = 0;
        RTCCNTL.test_mux.dtest_rtc = 1;
        RTCCNTL.test_mux.ent_rtc = 1;
        SENS.sar_start_force.sar2_en_test = 1;
        SENS.sar_meas_start2.sar2_en_pad = (1 << _adc_hw[rtcio].adc_channel);
        LOG_TAG_INFO("adc", "You can now measure Vref at GPIO25\n");
        return 0;
    }
    else {
        LOG_TAG_ERROR("adc", "Could not init GPIO25 as Vref output\n");
        return -1;
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

static void _adc_module_init(void)
{
    RTCIO.enable_w1tc.val = ~0x0;

    /* always power on */
    SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;

    /* disable temperature sensor */
    SENS.sar_tctrl.tsens_power_up_force = 1; /* controlled by SW */
    SENS.sar_tctrl.tsens_power_up = 0;       /* power down */
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
