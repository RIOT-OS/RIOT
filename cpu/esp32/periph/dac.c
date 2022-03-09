/*
 * Copyright (C) 2019 Gunar Schorcht
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
 * @brief       Low-level DAC driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "board.h"
#include "periph/dac.h"

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
extern void _adc2_ctrl_init(void);

/* forward declarations of internal functions */
static bool _dac_conf_check(void);
static bool _dac_module_initialized  = false;

/* external variable declarations */
extern const gpio_t _gpio_rtcio_map[];

int8_t dac_init (dac_t line)
{
    CHECK_PARAM_RET (line < DAC_NUMOF, DAC_NOLINE)

    if (!_dac_module_initialized) {
        /* do some configuration checks */
        if (!_dac_conf_check()) {
            return DAC_NOLINE;
        }
        _dac_module_initialized = true;
    }

    _adc2_ctrl_init();

    uint8_t rtcio = _gpio_rtcio_map[dac_channels[line]];
    uint8_t idx;

    /* try to initialize the pin as DAC output */
    if (gpio_get_pin_usage(_adc_hw[rtcio].gpio) != _GPIO) {
        LOG_TAG_ERROR("dac", "GPIO%d is used for %s and cannot be used as "
                      "DAC output\n", _adc_hw[rtcio].gpio,
                      gpio_get_pin_usage_str(_adc_hw[rtcio].gpio));
        return DAC_NOLINE;
    }

    /* disable the output of the pad */
    RTCIO.enable_w1tc.val = BIT(_adc_hw[rtcio].rtc_gpio);

    switch (rtcio) {
        case RTCIO_DAC1: /* GPIO25, RTC6 */
        case RTCIO_DAC2: /* GPIO26, RTC7 */
                idx = rtcio - RTCIO_DAC1;
                RTCIO.pad_dac[idx].mux_sel = 1; /* route to RTC */
                RTCIO.pad_dac[idx].fun_sel = 0; /* function ADC2_CH8, ADC2_CH9 */
                RTCIO.pad_dac[idx].fun_ie = 0;  /* input disabled */
                RTCIO.pad_dac[idx].rue = 0;     /* pull-up disabled */
                RTCIO.pad_dac[idx].rde = 0;     /* pull-down disabled */

                RTCIO.pad_dac[idx].dac_xpd_force = 1; /* use RTC pad not the FSM*/
                RTCIO.pad_dac[idx].xpd_dac = 1;       /* DAC powered on */
                break;

        default: return DAC_NOLINE;
    }

    /* set pin usage type  */
    gpio_set_pin_usage(_adc_hw[rtcio].gpio, _DAC);

    /* don't use DMA */
    SENS.sar_dac_ctrl1.dac_dig_force = 0;

    /* disable CW generators and invert DAC signal */
    SENS.sar_dac_ctrl1.sw_tone_en = 0;
    SENS.sar_dac_ctrl2.dac_cw_en1 = 0;
    SENS.sar_dac_ctrl2.dac_cw_en2 = 0;

    return DAC_OK;
}

void dac_set (dac_t line, uint16_t value)
{
    CHECK_PARAM (line < DAC_NUMOF);
    RTCIO.pad_dac[_gpio_rtcio_map[dac_channels[line]] - RTCIO_DAC1].dac = value >> 8;
}

void dac_poweroff (dac_t line)
{
    CHECK_PARAM (line < DAC_NUMOF);
}

void dac_poweron (dac_t line)
{
    CHECK_PARAM (line < DAC_NUMOF);
}

static bool _dac_conf_check(void)
{
    for (unsigned i = 0; i < DAC_NUMOF; i++) {
        if (_gpio_rtcio_map[dac_channels[i]] != RTCIO_DAC1 &&
            _gpio_rtcio_map[dac_channels[i]] != RTCIO_DAC2) {
            LOG_TAG_ERROR("dac", "GPIO%d cannot be used as DAC line\n",
                          dac_channels[i]);
            return false;
        }
    }

    return true;
}

void dac_print_config(void)
{
    printf("\tDAC\t\tpins=[ ");
#if defined(DAC_GPIOS)
    for (unsigned i = 0; i < DAC_NUMOF; i++) {
        printf("%d ", dac_channels[i]);
    }
#endif /* defined(DAC_GPIOS) */
    printf("]\n");
}
