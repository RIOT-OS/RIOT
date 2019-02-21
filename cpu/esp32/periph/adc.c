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

#define ENABLE_DEBUG    0
#include "debug.h"
#include "esp_common.h"

#include "board.h"
#include "cpu.h"
#include "log.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph/dac.h"
#include "periph/gpio.h"

#include "adc_arch.h"
#include "gpio_arch.h"
#include "rom/ets_sys.h"
#include "soc/rtc_io_struct.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"

#define ADC1_CTRL    0
#define ADC2_CTRL    1

/* RTC pin type (does not correspond to RTC gpio num order) */
typedef enum {

    RTCIO_TOUCH0 = 0,        /* touch sensor 0 */
    RTCIO_TOUCH1,            /* touch sensor 1 */
    RTCIO_TOUCH2,            /* touch sensor 2 */
    RTCIO_TOUCH3,            /* touch sensor 3 */
    RTCIO_TOUCH4,            /* touch sensor 4 */
    RTCIO_TOUCH5,            /* touch sensor 5 */
    RTCIO_TOUCH6,            /* touch sensor 6 */
    RTCIO_TOUCH7,            /* touch sensor 7 */
    RTCIO_TOUCH8,            /* touch sensor 8, 32K_XP */
    RTCIO_TOUCH9,            /* touch sensor 9, 32K_XN */

    RTCIO_ADC_ADC1,          /* VDET_1 */
    RTCIO_ADC_ADC2,          /* VDET_2 */

    RTCIO_SENSOR_SENSE1,     /* SENSOR_VP */
    RTCIO_SENSOR_SENSE2,     /* SENSOR_CAPP */
    RTCIO_SENSOR_SENSE3,     /* SENSOR_CAPN */
    RTCIO_SENSOR_SENSE4,     /* SENSOR_VN */

    RTCIO_DAC1,              /* DAC output */
    RTCIO_DAC2,              /* DAC output */

    RTCIO_NA,                /* RTC pad not available */
} _rtcio_pin_t;

/* ADC pin hardware information type (for internal use only) */
struct _adc_hw_t {
    gpio_t   gpio;
    uint8_t  rtc_gpio;
    uint8_t  adc_ctrl;
    uint8_t  adc_channel;
    char*    pad_name;
};

/* RTC hardware map, the index corresponds to RTC pin type _rtcio_pin_t
   (Table 19 in Technical Reference) */
const struct _adc_hw_t _adc_hw[] =
{
    /* gpio  rtc_gpio  adc_ctrl  adc_channel, pad_name */
    {  GPIO4,  10,    ADC2_CTRL, 0, "GPIO4" },       /* RTCIO_TOUCH0 */
    {  GPIO0,  11,    ADC2_CTRL, 1, "GPIO0" },       /* RTCIO_TOUCH1 */
    {  GPIO2,  12,    ADC2_CTRL, 2, "GPIO2" },       /* RTCIO_TOUCH2 */
    {  GPIO15, 13,    ADC2_CTRL, 3, "MTDO" },        /* RTCIO_TOUCH3 */
    {  GPIO13, 14,    ADC2_CTRL, 4, "MTCK" },        /* RTCIO_TOUCH4 */
    {  GPIO12, 15,    ADC2_CTRL, 5, "MTDI" },        /* RTCIO_TOUCH5 */
    {  GPIO14, 16,    ADC2_CTRL, 6, "MTMS" },        /* RTCIO_TOUCH6 */
    {  GPIO27, 17,    ADC2_CTRL, 7, "GPIO27" },      /* RTCIO_TOUCH7 */
    {  GPIO33,  8,    ADC1_CTRL, 5, "32K_XN" },      /* RTCIO_TOUCH8 */
    {  GPIO32,  9,    ADC1_CTRL, 4, "32K_XP" },      /* RTCIO_TOUCH9 */
    {  GPIO34,  4,    ADC1_CTRL, 6, "VDET_1" },      /* RTCIO_ADC_ADC1 */
    {  GPIO35,  5,    ADC1_CTRL, 7, "VDET_2" },      /* RTCIO_ADC_ADC2 */
    {  GPIO36,  0,    ADC1_CTRL, 0, "SENSOR_VP" },   /* RTCIO_SENSOR_SENSE1 */
    {  GPIO37,  1,    ADC1_CTRL, 1, "SENSOR_CAPP" }, /* RTCIO_SENSOR_SENSE2 */
    {  GPIO38,  2,    ADC1_CTRL, 2, "SENSOR_CAPN" }, /* RTCIO_SENSOR_SENSE3 */
    {  GPIO39,  3,    ADC1_CTRL, 3, "SENSOR_VN" },   /* RTCIO_SENSOR_SENSE4 */
    {  GPIO25,  6,    ADC2_CTRL, 8, "GPIO25" },      /* RTCIO_DAC1 */
    {  GPIO26,  7,    ADC2_CTRL, 9, "GPIO26" }       /* RTCIO_DAC2 */
};

/* maps GPIO pin to RTC pin, this index is used to access ADC hardware table
   (Table 19 in Technical Reference) */
const gpio_t _gpio_rtcio_map[] = {
    RTCIO_TOUCH1,        /* GPIO0 */
    RTCIO_NA     ,       /* GPIO1 */
    RTCIO_TOUCH2,        /* GPIO2 */
    RTCIO_NA,            /* GPIO3 */
    RTCIO_TOUCH0,        /* GPIO4 */
    RTCIO_NA,            /* GPIO5 */
    RTCIO_NA,            /* GPIO6 */
    RTCIO_NA,            /* GPIO7 */
    RTCIO_NA,            /* GPIO8 */
    RTCIO_NA,            /* GPIO9 */
    RTCIO_NA,            /* GPIO10 */
    RTCIO_NA,            /* GPIO11 */
    RTCIO_TOUCH5,        /* GPIO12 MTDI */
    RTCIO_TOUCH4,        /* GPIO13 MTCK */
    RTCIO_TOUCH6,        /* GPIO14 MTMS */
    RTCIO_TOUCH3,        /* GPIO15 MTDO */
    RTCIO_NA,            /* GPIO16 */
    RTCIO_NA,            /* GPIO17 */
    RTCIO_NA,            /* GPIO18 */
    RTCIO_NA,            /* GPIO19 */
    RTCIO_NA,            /* GPIO20 */
    RTCIO_NA,            /* GPIO21 */
    RTCIO_NA,            /* GPIO22 */
    RTCIO_NA,            /* GPIO23 */
    RTCIO_NA,            /* GPIO24 */
    RTCIO_DAC1,          /* GPIO25 */
    RTCIO_DAC2,          /* GPIO26 */
    RTCIO_TOUCH7,        /* GPIO27 */
    RTCIO_NA,            /* GPIO28 */
    RTCIO_NA,            /* GPIO29 */
    RTCIO_NA,            /* GPIO30 */
    RTCIO_NA,            /* GPIO31 */
    RTCIO_TOUCH9,        /* GPIO32 32K_XP */
    RTCIO_TOUCH8,        /* GPIO33 32K_XN */
    RTCIO_ADC_ADC1,      /* GPIO34 VDET_1 */
    RTCIO_ADC_ADC2,      /* GPIO35 VDET_2 */
    RTCIO_SENSOR_SENSE1, /* GPIO36 SENSOR_VP */
    RTCIO_SENSOR_SENSE2, /* GPIO37 SENSOR_CAPP */
    RTCIO_SENSOR_SENSE3, /* GPIO38 SENSOR_CAPN */
    RTCIO_SENSOR_SENSE4, /* GPIO39 SENSOR_VN */
};

/** Map of RIOT ADC and DAC lines to GPIOs */
static const uint32_t adc_pins[] = ADC_GPIOS;
static const uint32_t dac_pins[] = DAC_GPIOS;

/** number of ADC and DAC channels */
const unsigned adc_chn_num = (sizeof(adc_pins) / sizeof(adc_pins[0]));
const unsigned dac_chn_num = (sizeof(dac_pins) / sizeof(dac_pins[0]));

#if defined(ADC_GPIOS) || defined(DAC_GPIOS)
/* forward declaration of internal functions */
static void _adc1_ctrl_init(void);
static void _adc2_ctrl_init(void);

static bool _adc1_ctrl_initialized = false;
static bool _adc2_ctrl_initialized = false;
#endif /* defined(ADC_GPIOS) || defined(DAC_GPIOS) */

#if defined(ADC_GPIOS)
static bool _adc_conf_check(void);
static void _adc_module_init(void);
static bool _adc_module_initialized  = false;

int adc_init(adc_t line)
{
    CHECK_PARAM_RET (line < adc_chn_num, -1)

    if (!_adc_module_initialized) {
        /* do some configuration checks */
        if (!_adc_conf_check()) {
            return -1;
        }
        _adc_module_init();
        _adc_module_initialized = true;
    }

    uint8_t rtcio = _gpio_rtcio_map[adc_pins[line]];

    if (_adc_hw[rtcio].adc_ctrl == ADC1_CTRL && !_adc1_ctrl_initialized) {
        _adc1_ctrl_init();
    }
    if (_adc_hw[rtcio].adc_ctrl == ADC2_CTRL && !_adc2_ctrl_initialized) {
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


int adc_sample(adc_t line, adc_res_t res)
{
    CHECK_PARAM_RET (line < adc_chn_num, -1)
    CHECK_PARAM_RET (res <= ADC_RES_12BIT, -1)

    uint8_t rtcio = _gpio_rtcio_map[adc_pins[line]];

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
    CHECK_PARAM_RET (line < adc_chn_num, -1)

    uint8_t rtcio = _gpio_rtcio_map[adc_pins[line]];

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
    for (unsigned i = 0; i < adc_chn_num; i++) { \
        if (adc_pins[i] == GPIO25) { \
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
        uint8_t rtcio = _gpio_rtcio_map[adc_pins[line]];
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
    for (unsigned i = 0; i < adc_chn_num; i++) {
        if (_gpio_rtcio_map[adc_pins[i]] == RTCIO_NA) {
            LOG_TAG_ERROR("adc", "GPIO%d cannot be used as ADC line\n",
                          adc_pins[i]);
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

#endif /* defined(ADC_GPIOS) */

#if defined(DAC_GPIOS)

static bool _dac_conf_check(void);
static bool _dac_module_initialized  = false;

int8_t dac_init (dac_t line)
{
    CHECK_PARAM_RET (line < dac_chn_num, DAC_NOLINE)

    if (!_dac_module_initialized) {
        /* do some configuration checks */
        if (!_dac_conf_check()) {
            return -1;
        }
        _dac_module_initialized = true;
    }

    if (!_adc2_ctrl_initialized) {
        _adc2_ctrl_init();
    }

    uint8_t rtcio = _gpio_rtcio_map[dac_pins[line]];
    uint8_t idx;

    /* try to initialize the pin as DAC ouput */
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
    CHECK_PARAM (line < dac_chn_num);
    RTCIO.pad_dac[_gpio_rtcio_map[dac_pins[line]] - RTCIO_DAC1].dac = value >> 8;
}

void dac_poweroff (dac_t line)
{
    CHECK_PARAM (line < dac_chn_num);
}

void dac_poweron (dac_t line)
{
    CHECK_PARAM (line < dac_chn_num);
}

static bool _dac_conf_check(void)
{
    for (unsigned i = 0; i < dac_chn_num; i++) {
        if (_gpio_rtcio_map[dac_pins[i]] != RTCIO_DAC1 &&
            _gpio_rtcio_map[dac_pins[i]] != RTCIO_DAC2) {
            LOG_TAG_ERROR("dac", "GPIO%d cannot be used as DAC line\n",
                          dac_pins[i]);
            return false;
        }
    }

    return true;
}

#endif /* defined(DAC_GPIOS) */

#if defined(ADC_GPIOS) || defined(DAC_GPIOS)

static void _adc1_ctrl_init(void)
{
    /* always power on */
    SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;

    /* power off LN amp */
    SENS.sar_meas_wait2.sar2_rstb_wait = 2;
    SENS.sar_meas_ctrl.amp_rst_fb_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_gnd_fsm = 0;
    SENS.sar_meas_wait1.sar_amp_wait1 = 1;
    SENS.sar_meas_wait1.sar_amp_wait2 = 1;
    SENS.sar_meas_wait2.sar_amp_wait3 = 1;
    SENS.sar_meas_wait2.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;

    /* SAR ADC1 controller configuration */
    SENS.sar_read_ctrl.sar1_dig_force = 0;      /* SAR ADC1 controlled by RTC */
    SENS.sar_meas_start1.meas1_start_force = 1; /* SAR ADC1 started by SW */
    SENS.sar_meas_start1.sar1_en_pad_force = 1; /* pad enable bitmap controlled by SW */
    SENS.sar_touch_ctrl1.xpd_hall_force = 1;    /* XPD HALL is controlled by SW */
    SENS.sar_touch_ctrl1.hall_phase_force = 1;  /* HALL PHASE is controlled by SW */
    SENS.sar_read_ctrl.sar1_data_inv = 1;       /* invert data */
    SENS.sar_atten1 = 0xffffffff;               /* set attenuation to 11 dB for all pads
                                                   (input range 0 ... 3,3 V) */
    /* power off built-in hall sensor */
    RTCIO.hall_sens.xpd_hall = 0;

    /* set default resolution */
    SENS.sar_start_force.sar1_bit_width = ADC_RES_12BIT;
    SENS.sar_read_ctrl.sar1_sample_bit = ADC_RES_12BIT;

    _adc1_ctrl_initialized = true;
}

static void _adc2_ctrl_init(void)
{
    /* SAR ADC2 controller configuration */
    SENS.sar_read_ctrl2.sar2_dig_force = 0;     /* SAR ADC2 controlled by RTC not DIG*/
    SENS.sar_meas_start2.meas2_start_force = 1; /* SAR ADC2 started by SW */
    SENS.sar_meas_start2.sar2_en_pad_force = 1; /* pad enable bitmap controlled by SW */
    SENS.sar_read_ctrl2.sar2_data_inv = 1;      /* invert data */
    SENS.sar_atten2 = 0xffffffff;               /* set attenuation to 11 dB for all pads
                                                   (input range 0 ... 3,3 V) */
    /* set default resolution */
    SENS.sar_start_force.sar2_bit_width = ADC_RES_12BIT;
    SENS.sar_read_ctrl2.sar2_sample_bit = ADC_RES_12BIT;

    _adc2_ctrl_initialized = true;
}

#endif /* defined(ADC_GPIOS) || defined(DAC_GPIOS) */

extern const gpio_t _gpio_rtcio_map[];

int rtcio_config_sleep_mode (gpio_t pin, bool mode, bool input)
{
    CHECK_PARAM_RET(pin < GPIO_PIN_NUMOF, -1);

    uint8_t rtcio = _gpio_rtcio_map[pin];
    uint8_t idx;

    /* route pads to RTC and if possible, disable input, pull-up/pull-down */
    switch (rtcio) {
        case RTCIO_SENSOR_SENSE1: /* GPIO36, RTC0 */
                RTCIO.sensor_pads.sense1_mux_sel = 1;     /* route to RTC */
                RTCIO.sensor_pads.sense1_fun_sel = 0;     /* RTC mux function 0 */
                RTCIO.sensor_pads.sense1_slp_sel = mode;  /* sleep mode */
                RTCIO.sensor_pads.sense1_slp_ie  = input; /* input enabled */
                break;
        case RTCIO_SENSOR_SENSE2: /* GPIO37, RTC1 */
                RTCIO.sensor_pads.sense2_mux_sel = 1;     /* route to RTC */
                RTCIO.sensor_pads.sense2_fun_sel = 0;     /* RTC mux function 0 */
                RTCIO.sensor_pads.sense2_slp_sel = mode;  /* sleep mode */
                RTCIO.sensor_pads.sense2_slp_ie  = input; /* input enabled */
                break;
        case RTCIO_SENSOR_SENSE3: /* GPIO38, RTC2 */
                RTCIO.sensor_pads.sense3_mux_sel = 1;     /* route to RTC */
                RTCIO.sensor_pads.sense3_fun_sel = 0;     /* RTC mux function 0 */
                RTCIO.sensor_pads.sense3_slp_sel = mode;  /* sleep mode */
                RTCIO.sensor_pads.sense3_slp_ie  = input; /* input enabled */
                break;
        case RTCIO_SENSOR_SENSE4: /* GPIO39, RTC3 */
                RTCIO.sensor_pads.sense4_mux_sel = 1;     /* route to RTC */
                RTCIO.sensor_pads.sense4_fun_sel = 0;     /* RTC mux function 0 */
                RTCIO.sensor_pads.sense4_slp_sel = mode;  /* sleep mode */
                RTCIO.sensor_pads.sense4_slp_ie  = input; /* input enabled */
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
                RTCIO.touch_pad[idx].mux_sel = 1;      /* route to RTC */
                RTCIO.touch_pad[idx].fun_sel = 0;      /* RTC mux function 0 */
                RTCIO.touch_pad[idx].slp_sel = mode;   /* sleep mode */
                RTCIO.touch_pad[idx].slp_ie  = input;  /* input enabled */
                RTCIO.touch_pad[idx].slp_oe  = ~input; /* output enabled*/
                break;

        case RTCIO_ADC_ADC1: /* GPIO34, RTC4 */
                RTCIO.adc_pad.adc1_mux_sel = 1;     /* route to RTC */
                RTCIO.adc_pad.adc1_fun_sel = 0;     /* RTC mux function 0 */
                RTCIO.adc_pad.adc1_slp_sel = mode;  /* sleep mode */
                RTCIO.adc_pad.adc1_slp_ie  = input; /* input enabled */
                break;
        case RTCIO_ADC_ADC2: /* GPIO35, RTC5 */
                RTCIO.adc_pad.adc2_mux_sel = 1;     /* route to RTC */
                RTCIO.adc_pad.adc2_fun_sel = 0;     /* RTC mux function 0 */
                RTCIO.adc_pad.adc2_slp_sel = mode;  /* sleep mode */
                RTCIO.adc_pad.adc2_slp_ie  = input; /* input enabled */
                break;

        case RTCIO_DAC1: /* GPIO25, RTC6 */
        case RTCIO_DAC2: /* GPIO26, RTC7 */
                idx = rtcio - RTCIO_DAC1;
                RTCIO.pad_dac[idx].mux_sel = 1;      /* route to RTC */
                RTCIO.pad_dac[idx].fun_sel = 0;      /* RTC mux function 0 */
                RTCIO.pad_dac[idx].slp_sel = mode;   /* sleep mode */
                RTCIO.pad_dac[idx].slp_ie  = input;  /* input enabled */
                RTCIO.pad_dac[idx].slp_oe  = ~input; /* output enabled*/
                break;
        default:
                LOG_TAG_ERROR("gpio", "GPIO %d is not an RTCIO pin and "
                              "cannot be used in sleep mode\n", pin);
                return -1;
    }
    return 0;
}

void adc_print_config(void) {
    ets_printf("\tADC\t\tpins=[ ");
    #if defined(ADC_GPIOS)
    for (unsigned i = 0; i < adc_chn_num; i++) {
        ets_printf("%d ", adc_pins[i]);
    }
    #endif /* defined(ADC_GPIOS) */
    ets_printf("]\n");

    ets_printf("\tDAC\t\tpins=[ ");
    #if defined(DAC_GPIOS)
    for (unsigned i = 0; i < dac_chn_num; i++) {
        ets_printf("%d ", dac_pins[i]);
    }
    #endif /* defined(DAC_GPIOS) */
    ets_printf("]\n");
}
