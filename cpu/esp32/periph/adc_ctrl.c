/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       ADC controller functions used by ADC and DAC peripherals
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "board.h"

#include "adc_ctrl.h"
#include "esp_common.h"
#include "soc/rtc_io_struct.h"
#include "soc/rtc_cntl_struct.h"
#include "soc/sens_reg.h"
#include "soc/sens_struct.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   RTC hardware map
 *
 * The index corresponds to RTC pin type _rtcio_pin_t (Table 19 in Technical
 * Reference)
 */
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

/* flags to indicate whether the according controller is already initialized */
static bool _adc1_ctrl_initialized = false;
static bool _adc2_ctrl_initialized = false;

void _adc1_ctrl_init(void)
{
    /* return if already initialized */
    if (_adc1_ctrl_initialized) {
        return;
    }

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

void _adc2_ctrl_init(void)
{
    /* return if already initialized */
    if (_adc2_ctrl_initialized) {
        return;
    }

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
