/*
 * The Clear BSD License
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the
 * disclaimer below) provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
 * GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @ingroup     drivers_kw41zrf
 * @{
 * @file
 * @brief       NXP KW41Z XCVR module initialization and calibration of kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "bit.h"
#include "kw41zrf.h"
#include "vendor/XCVR/MKW41Z4/fsl_xcvr.h"
#include "vendor/XCVR/MKW41Z4/ifr_radio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* The implementations for these functions are taken from the vendor-provided
 * XCVR driver from mcuxpresso.nxp.com (KSDK 2.2.0, framework_5.3.5)
 * The code has been refactored to eliminate a lot of preprocessor
 * conditionals. */

#define TsettleCal 10
#define DCOC_DAC_BBF_STEP (16)
#define RX_DC_EST_SAMPLES (64)
#define RX_DC_EST_TOTAL_SAMPLES (2 * (RX_DC_EST_SAMPLES))

/* Macros used by the calibration routine */
#define SAME_SIGN(a, b) (((a) ^ (b)) >= 0)
#define ABS(x) ((x) > 0 ? (x) : -(x))

/* dumb spin delay used in the calibration functions */
static void kw41zrf_xcvr_spin(uint32_t time)
{
    time *= 32; /* Time delay is roughly in uSec. */
    while (time > 0)
    {
        __asm__ volatile  ("" ::: "memory");
        --time;
    }
}

/* Collect RX DC estimation samples */
static void rx_dc_est_samples(int32_t *i_sum, int32_t *q_sum, unsigned nsamples)
{
    /* Wait for TSM to reach the end of warmup (unless you want to capture some samples during DCOC cal phase). */
    uint32_t end_of_rx_wu = XCVR_CTRL_XCVR_STATUS_TSM_COUNT(
        (XCVR_TSM->END_OF_SEQ & XCVR_TSM_END_OF_SEQ_END_OF_RX_WU_MASK) >>
            XCVR_TSM_END_OF_SEQ_END_OF_RX_WU_SHIFT);
    while ((XCVR_MISC->XCVR_STATUS & XCVR_CTRL_XCVR_STATUS_TSM_COUNT_MASK) != end_of_rx_wu) {};

    int32_t sum_i = 0;
    int32_t sum_q = 0;
    /* Read DCOC DC EST register. */
    for (unsigned k = 0; k < nsamples; k++)
    {
        uint32_t dc_temp = XCVR_RX_DIG->DCOC_DC_EST;
        int16_t dc_meas_i = (dc_temp & XCVR_RX_DIG_DCOC_DC_EST_DC_EST_I_MASK) >> XCVR_RX_DIG_DCOC_DC_EST_DC_EST_I_SHIFT;
        dc_meas_i = (int16_t)(dc_meas_i << 4) / 16; /* Sign extend from 12 to 16 bits. */
        sum_i += dc_meas_i;

        int16_t dc_meas_q = (dc_temp & XCVR_RX_DIG_DCOC_DC_EST_DC_EST_Q_MASK) >> XCVR_RX_DIG_DCOC_DC_EST_DC_EST_Q_SHIFT;
        dc_meas_q = (int16_t)(dc_meas_q << 4) / 16; /* Sign extend from 12 to 16 bits. */
        sum_q += dc_meas_q;
    }

    *i_sum = sum_i;
    *q_sum = sum_q;
}

/* Unsigned integer division, rounded to nearest integer */
static inline uint32_t calc_div_rounded(uint32_t num, uint32_t den)
{
    return (num + (den / 2)) / den;
}

int kw41zrf_rx_bba_dcoc_dac_trim_DCest(void)
{
    /* Estimate the actual gain by measuring three points and approximating a line */
    int status = 0;

    /* Save register */
    uint32_t dcoc_ctrl_0_stack = XCVR_RX_DIG->DCOC_CTRL_0; /* Save state of DCOC_CTRL_0 for later restore */
    uint32_t dcoc_ctrl_1_stack = XCVR_RX_DIG->DCOC_CTRL_1; /* Save state of DCOC_CTRL_1 for later restore */
    uint32_t rx_dig_ctrl_stack =  XCVR_RX_DIG->RX_DIG_CTRL; /* Save state of RX_DIG_CTRL for later restore */
    uint32_t agc_ctrl_1_stack = XCVR_RX_DIG->AGC_CTRL_1; /* Save state of RX_DIG_CTRL for later restore */
    uint32_t dcoc_cal_gain_state = XCVR_RX_DIG->DCOC_CAL_GAIN; /* Save state of DCOC_CAL_GAIN for later restore */

    /* Register config */
    /* Ensure AGC, DCOC and RX_DIG_CTRL is in correct mode */
    XCVR_RX_DIG->RX_DIG_CTRL = XCVR_RX_DIG->RX_DIG_CTRL &
        ~(XCVR_RX_DIG_RX_DIG_CTRL_RX_AGC_EN_MASK | /* Turn OFF AGC */
            XCVR_RX_DIG_RX_DIG_CTRL_RX_DCOC_CAL_EN_MASK | /* Disable for SW control of DCOC */
            XCVR_RX_DIG_RX_DIG_CTRL_RX_DC_RESID_EN_MASK); /* Disable for SW control of DCOC */

    XCVR_RX_DIG->AGC_CTRL_1 = XCVR_RX_DIG_AGC_CTRL_1_USER_LNA_GAIN_EN(1) | /* Enable LNA Manual Gain */
                              XCVR_RX_DIG_AGC_CTRL_1_USER_BBA_GAIN_EN(1) | /* Enable BBA Manual Gain */
                              XCVR_RX_DIG_AGC_CTRL_1_LNA_USER_GAIN(0x0) | /* Set LNA Manual Gain */
                              XCVR_RX_DIG_AGC_CTRL_1_BBA_USER_GAIN(0x0); /* Set BBA Manual Gain */

    /* DCOC_CTRL_0 @ 4005_C02C -- Define default DCOC DAC settings in manual mode */
    XCVR_RX_DIG->DCOC_CTRL_0 = XCVR_RX_DIG->DCOC_CTRL_0 |
        XCVR_RX_DIG_DCOC_CTRL_0_DCOC_MAN(1) | /* Enable  Manual DCOC */
        XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORRECT_SRC(1) | /* Ensure DCOC Tracking is enabled */
        XCVR_RX_DIG_DCOC_CTRL_0_DCOC_TRK_EST_OVR(1) | /* Enable DC Estimator */
        XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORRECT_EN(1); /* Ensure DC correction is enabled */

    /* Use reset defaults */
    uint8_t bbf_dacinit_i = 0x20;
    uint8_t bbf_dacinit_q = 0x20;
    uint8_t tza_dacinit_i = 0x80;
    uint8_t tza_dacinit_q = 0x80;

    /* Set default DCOC DAC INIT Value */
    XCVR_RX_DIG->DCOC_DAC_INIT =
        XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(bbf_dacinit_i) |
        XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(bbf_dacinit_q) |
        XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(tza_dacinit_i) |
        XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(tza_dacinit_q);
    /* Store DCOC_DAC_INIT value */
    uint32_t dcoc_init_reg_value_dcgain = XCVR_RX_DIG->DCOC_DAC_INIT;

    kw41zrf_xcvr_spin(TsettleCal * 2);

    uint32_t meas_sum = 0;
    /* SWEEP I/Q CHANNEL */
    /* BBF NEG STEP */
    XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(bbf_dacinit_i - DCOC_DAC_BBF_STEP) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(bbf_dacinit_q - DCOC_DAC_BBF_STEP) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(tza_dacinit_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(tza_dacinit_q);
    kw41zrf_xcvr_spin(TsettleCal * 4);

    int32_t dc_meas_im = 0;
    int32_t dc_meas_qm = 0;
    rx_dc_est_samples(&dc_meas_im, &dc_meas_qm, RX_DC_EST_SAMPLES);

    /* BBF POS STEP */
    XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(bbf_dacinit_i + DCOC_DAC_BBF_STEP) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(bbf_dacinit_q + DCOC_DAC_BBF_STEP) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(tza_dacinit_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(tza_dacinit_q);
    kw41zrf_xcvr_spin(TsettleCal * 4);
    int32_t dc_meas_ip = 0;
    int32_t dc_meas_qp = 0;
    rx_dc_est_samples(&dc_meas_ip, &dc_meas_qp, RX_DC_EST_SAMPLES);
    DEBUG("dc_meas_i- = %" PRId32 "\n", dc_meas_im);
    DEBUG("dc_meas_q- = %" PRId32 "\n", dc_meas_qm);
    DEBUG("dc_meas_i+ = %" PRId32 "\n", dc_meas_ip);
    DEBUG("dc_meas_q+ = %" PRId32 "\n", dc_meas_qp);
    meas_sum += dc_meas_ip - dc_meas_im;
    DEBUG("meas_sum = %" PRIu32 "\n", meas_sum);
    meas_sum += dc_meas_qp - dc_meas_qm;
    DEBUG("meas_sum = %" PRIu32 "\n", meas_sum);
    meas_sum /= 2 * DCOC_DAC_BBF_STEP;
    DEBUG("meas_sum = %" PRIu32 "\n", meas_sum);

    XCVR_RX_DIG->DCOC_DAC_INIT = dcoc_init_reg_value_dcgain; /* Return DAC setting to initial */

    /* Compute the average sampled gain for the measured steps */

    /* Calculate BBF DCOC STEPS, RECIPROCALS */
    /* meas_sum here is the average gain multiplied by (4 * RX_DC_EST_SAMPLES) */
    /* Compute the gain average as a Q6.3 number */
    /* rounded result, Q6.3 number */
    uint16_t bbf_dcoc_gain_measured = calc_div_rounded(meas_sum, (RX_DC_EST_TOTAL_SAMPLES / (1u << 3)));

    DEBUG("temp_step = %f\n", (float)meas_sum / RX_DC_EST_TOTAL_SAMPLES);
    DEBUG("bbf_dcoc_gain_measured = %u\n", (unsigned)bbf_dcoc_gain_measured);

    /* Check the measured value for validity. Should be in the range:
     * 250 < bbf_dcoc_gain_measured < 305, according to NXP wireless framework v5.4.3 (MCUXpresso KW36 SDK)
     */
    if ((250 < bbf_dcoc_gain_measured) & (bbf_dcoc_gain_measured < 305))
    {
        /* Compute reciprocal, as Q15 number, but only the 13 lowest bits are programmable */
        /* rounded result, ((2**15) / slope) */
        uint32_t bbf_dcoc_gain_measured_rcp = calc_div_rounded((1u << 15) * RX_DC_EST_TOTAL_SAMPLES, meas_sum);
        DEBUG("bbf_dcoc_gain_measured_rcp = %"PRIu32"\n", bbf_dcoc_gain_measured_rcp);

        uint32_t bbf_dcoc_gain_default =
            (xcvr_common_config.dcoc_bba_step_init &
                XCVR_RX_DIG_DCOC_BBA_STEP_BBA_DCOC_STEP_MASK) >>
            XCVR_RX_DIG_DCOC_BBA_STEP_BBA_DCOC_STEP_SHIFT;
        /* Rescale all default TZA DCOC gains according to the measured BBF gain,
         * using (bbf_dcoc_gain_measured / bbf_dcoc_gain_default) as the implicit
         * scale factor, but rewrite it to use
         * (meas_sum / (bbf_dcoc_gain_default * RX_DC_EST_TOTAL_SAMPLES / (1u << 3))))
         * for better numeric precision */
        /* rounded result, Q9.3 number */
        bbf_dcoc_gain_default *= (RX_DC_EST_TOTAL_SAMPLES / (1u << 3));
        DEBUG("base gain = %u\n", (unsigned)bbf_dcoc_gain_default);
        /* Make the trims active */
        XCVR_RX_DIG->DCOC_BBA_STEP =
            XCVR_RX_DIG_DCOC_BBA_STEP_BBA_DCOC_STEP(bbf_dcoc_gain_measured) |
            XCVR_RX_DIG_DCOC_BBA_STEP_BBA_DCOC_STEP_RECIP(bbf_dcoc_gain_measured_rcp);
        const uint32_t *dcoc_tza_step_config_ptr = &xcvr_common_config.dcoc_tza_step_00_init;
        /* All tza_step_* configuration registers use sequential memory addresses */
        volatile uint32_t *xcvr_rx_dig_dcoc_tza_step_ptr = &XCVR_RX_DIG->DCOC_TZA_STEP_0;
        for (unsigned k = 0; k <= 10; ++k)
        {
            /* Calculate TZA DCOC STEPSIZE & its RECIPROCAL */
            uint16_t tza_gain_default =
                (dcoc_tza_step_config_ptr[k] &
                    XCVR_RX_DIG_DCOC_TZA_STEP_0_DCOC_TZA_STEP_GAIN_0_MASK) >>
                XCVR_RX_DIG_DCOC_TZA_STEP_0_DCOC_TZA_STEP_GAIN_0_SHIFT;
            /* Using meas_sum for higher precision */
            DEBUG("tza_gain_default[%u] = %u\n", k, (unsigned)tza_gain_default);
            uint32_t dcoc_step = calc_div_rounded(tza_gain_default * meas_sum, bbf_dcoc_gain_default);
            uint32_t dcoc_step_rcp = calc_div_rounded((0x8000ul << 3) * bbf_dcoc_gain_default, tza_gain_default * meas_sum);
            DEBUG("tza_dcoc_step[%u].dcoc_step = %u\n", k, (unsigned)dcoc_step);
            DEBUG("tza_dcoc_step[%u].dcoc_step_rcp = %u\n", k, (unsigned)dcoc_step_rcp);
            xcvr_rx_dig_dcoc_tza_step_ptr[k] =
                XCVR_RX_DIG_DCOC_TZA_STEP_0_DCOC_TZA_STEP_GAIN_0(dcoc_step) |
                XCVR_RX_DIG_DCOC_TZA_STEP_0_DCOC_TZA_STEP_RCP_0(dcoc_step_rcp) ;
        }
    }
    else
    {
        LOG_ERROR("!!! XCVR trim failed: bbf_dcoc_step = %u!\n", (unsigned)bbf_dcoc_gain_measured);
        status = -EAGAIN; /* Failure */
    }

    /* Restore Registers */
    XCVR_RX_DIG->DCOC_CTRL_0 = dcoc_ctrl_0_stack; /* Restore DCOC_CTRL_0 state to prior settings */
    XCVR_RX_DIG->DCOC_CTRL_1 = dcoc_ctrl_1_stack; /* Restore DCOC_CTRL_1 state to prior settings */
    XCVR_RX_DIG->RX_DIG_CTRL = rx_dig_ctrl_stack; /* Restore RX_DIG_CTRL state to prior settings */
    XCVR_RX_DIG->DCOC_CAL_GAIN = dcoc_cal_gain_state; /* Restore DCOC_CAL_GAIN state to prior setting */
    XCVR_RX_DIG->AGC_CTRL_1 = agc_ctrl_1_stack; /* Save state of RX_DIG_CTRL for later restore */

    return status;
}

static void kw41zrf_dcoc_dac_init_cal(void)
{
    uint8_t p_tza_dac_i = 0, p_tza_dac_q = 0;
    uint8_t p_bba_dac_i = 0, p_bba_dac_q = 0;
    uint8_t i = 0;
    uint8_t bba_gain = 11;
    uint8_t TZA_I_OK = 0, TZA_Q_OK = 0, BBA_I_OK = 0, BBA_Q_OK = 0;

    uint32_t temp;

    /* Save registers */
    uint32_t dcoc_ctrl_0_stack = XCVR_RX_DIG->DCOC_CTRL_0; /* Save state of DCOC_CTRL_0 for later restore */
    uint32_t dcoc_ctrl_1_stack = XCVR_RX_DIG->DCOC_CTRL_1; /* Save state of DCOC_CTRL_1 for later restore */
    uint32_t rx_dig_ctrl_stack =  XCVR_RX_DIG->RX_DIG_CTRL; /* Save state of RX_DIG_CTRL for later restore */
    uint32_t agc_ctrl_1_stack = XCVR_RX_DIG->AGC_CTRL_1; /* Save state of RX_DIG_CTRL for later restore */
    uint32_t dcoc_cal_gain_state = XCVR_RX_DIG->DCOC_CAL_GAIN; /* Save state of DCOC_CAL_GAIN for later restore */

    /* Register config */
    /* Ensure AGC, DCOC and RX_DIG_CTRL is in correct mode */
    temp = XCVR_RX_DIG->RX_DIG_CTRL;
    temp &= ~XCVR_RX_DIG_RX_DIG_CTRL_RX_AGC_EN_MASK; /* Turn OFF AGC */
    temp &= ~XCVR_RX_DIG_RX_DIG_CTRL_RX_DCOC_CAL_EN_MASK; /* Disable for SW control of DCOC */
    temp &= ~XCVR_RX_DIG_RX_DIG_CTRL_RX_DC_RESID_EN_MASK; /* Disable for SW control of DCOC */
    XCVR_RX_DIG->RX_DIG_CTRL = temp;

    XCVR_RX_DIG->AGC_CTRL_1 = XCVR_RX_DIG_AGC_CTRL_1_USER_LNA_GAIN_EN(1) | /* Enable LNA Manual Gain */
                              XCVR_RX_DIG_AGC_CTRL_1_USER_BBA_GAIN_EN(1) | /* Enable BBA Manual Gain */
                              XCVR_RX_DIG_AGC_CTRL_1_LNA_USER_GAIN(0x0) | /* Set LNA Manual Gain */
                              XCVR_RX_DIG_AGC_CTRL_1_BBA_USER_GAIN(0x0); /* Set BBA Manual Gain */

    /* DCOC_CTRL_0 @ 4005_C02C -- Define default DCOC DAC settings in manual mode */
    temp = XCVR_RX_DIG->DCOC_CTRL_0;
    temp |= XCVR_RX_DIG_DCOC_CTRL_0_DCOC_MAN(1); /* Enable  Manual DCOC */
    temp |= XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORRECT_SRC(1); /* Ensure DCOC Tracking is enabled */
    temp |= XCVR_RX_DIG_DCOC_CTRL_0_DCOC_TRK_EST_OVR(1); /* Enable DC Estimator */
    temp |= XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORRECT_EN(1); /* Ensure DC correction is enabled */
    XCVR_RX_DIG->DCOC_CTRL_0 = temp;

    kw41zrf_xcvr_spin(TsettleCal);

    /* Set default DCOC DAC INIT Value */
    /* LNA and BBA DAC Sweep */
    uint8_t curr_bba_dac_i = 0x20;
    uint8_t curr_bba_dac_q = 0x20;
    uint8_t curr_tza_dac_i = 0x80;
    uint8_t curr_tza_dac_q = 0x80;

    /* Perform a first DC measurement to ensure that measurement is not clipping */
    XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(curr_bba_dac_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(curr_bba_dac_q) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(curr_tza_dac_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(curr_tza_dac_q);

    int32_t dc_meas_i = 2000, dc_meas_i_p = 2000;
    int32_t dc_meas_q = 2000, dc_meas_q_p = 2000;
    do {
        bba_gain--;
        /* Set DAC user gain */
        XCVR_RX_DIG->AGC_CTRL_1 = XCVR_RX_DIG_AGC_CTRL_1_USER_LNA_GAIN_EN(1) |
                                  XCVR_RX_DIG_AGC_CTRL_1_LNA_USER_GAIN(0) | /* 2 */
                                  XCVR_RX_DIG_AGC_CTRL_1_USER_BBA_GAIN_EN(1) |
                                  XCVR_RX_DIG_AGC_CTRL_1_BBA_USER_GAIN(bba_gain) ; /* 10 */
        kw41zrf_xcvr_spin(TsettleCal * 2);
        rx_dc_est_samples(&dc_meas_i, &dc_meas_q, RX_DC_EST_SAMPLES);
        DEBUG("rx i=%d q=%d\n", (int)dc_meas_i, (int)dc_meas_q);
        dc_meas_i /= RX_DC_EST_SAMPLES;
        dc_meas_q /= RX_DC_EST_SAMPLES;
        DEBUG("rx i=%d q=%d\n", (int)dc_meas_i, (int)dc_meas_q);
        DEBUG("[kw41zrf] bba_gain=%u, meas I=%" PRId32 ", Q=%" PRId32 "\n", (unsigned)bba_gain, dc_meas_i, dc_meas_q);
    } while ((ABS(dc_meas_i) > 1900) || (ABS(dc_meas_q) > 1900));

    for (i = 0; i < 0x0F; i++)
    {
        DEBUG("rx i=%d q=%d\n", (int)dc_meas_i, (int)dc_meas_q);
        /* I channel :  */
        if (!TZA_I_OK) {
            if ((i > 0) && (!SAME_SIGN(dc_meas_i, dc_meas_i_p))) {
                if (ABS(dc_meas_i) > ABS(dc_meas_i_p)) {
                    curr_tza_dac_i = p_tza_dac_i;
                }

                TZA_I_OK = 1;
            }
            else {
                p_tza_dac_i = curr_tza_dac_i;

                if (dc_meas_i > 0) {
                    curr_tza_dac_i--;
                }
                else {
                    curr_tza_dac_i++;
                }
            }
        }
        else if (!BBA_I_OK) {
            /* Sweep BBA I */
            if ((curr_bba_dac_i != 0x20) && (!SAME_SIGN(dc_meas_i, dc_meas_i_p))) {
                if (ABS(dc_meas_i) > ABS(dc_meas_i_p)) {
                    curr_bba_dac_i = p_bba_dac_i;
                }

                BBA_I_OK = 1;
            }
            else {
                p_bba_dac_i = curr_bba_dac_i;
                if (dc_meas_i > 0) {
                    curr_bba_dac_i--;
                }
                else {
                    curr_bba_dac_i++;
                }
            }
        }

        /* Q channel : */
        if (!TZA_Q_OK) {
            if ((i > 0) && (!SAME_SIGN(dc_meas_q, dc_meas_q_p))) {
                if (ABS(dc_meas_q) > ABS(dc_meas_q_p)) {
                    curr_tza_dac_q = p_tza_dac_q;
                }
                TZA_Q_OK = 1;
            }
            else {
                p_tza_dac_q = curr_tza_dac_q;
                if (dc_meas_q > 0) {
                    curr_tza_dac_q--;
                }
                else {
                    curr_tza_dac_q++;
                }
            }
        }
        else if (!BBA_Q_OK) {
            /* Sweep BBA Q */
            if ((curr_bba_dac_q != 0x20) && (!SAME_SIGN(dc_meas_q, dc_meas_q_p))) {
                if (ABS(dc_meas_q) > ABS(dc_meas_q_p)) {
                    curr_bba_dac_q = p_bba_dac_q;
                }
                BBA_Q_OK = 1;
            }
            else {
                p_bba_dac_q = curr_bba_dac_q;
                if (dc_meas_q > 0) {
                    curr_bba_dac_q--;
                }
                else {
                    curr_bba_dac_q++;
                }
            }
        }

        /* DC OK break : */
        if (TZA_I_OK && TZA_Q_OK && BBA_I_OK && BBA_Q_OK) {
            break;
        }

        dc_meas_i_p = dc_meas_i; /* Store as previous value */
        dc_meas_q_p = dc_meas_q; /* Store as previous value */
        DEBUG("curr_bba_dac i=%d q=%d\n", (int)curr_bba_dac_i, (int)curr_bba_dac_q);
        DEBUG("curr_tza_dac i=%d q=%d\n", (int)curr_tza_dac_i, (int)curr_tza_dac_q);
        XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(curr_bba_dac_i) |
                                     XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(curr_bba_dac_q) |
                                     XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(curr_tza_dac_i) |
                                     XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(curr_tza_dac_q);
        kw41zrf_xcvr_spin(TsettleCal * 2);
        rx_dc_est_samples(&dc_meas_i, &dc_meas_q, RX_DC_EST_SAMPLES);
        dc_meas_i /= RX_DC_EST_SAMPLES;
        dc_meas_q /= RX_DC_EST_SAMPLES;
    }

    /* Apply optimized DCOC DAC INIT : */
    XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(curr_bba_dac_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(curr_bba_dac_q) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(curr_tza_dac_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(curr_tza_dac_q);

    /* Restore register */
    XCVR_RX_DIG->DCOC_CTRL_0 = dcoc_ctrl_0_stack; /* Restore DCOC_CTRL_0 state to prior settings */
    XCVR_RX_DIG->DCOC_CTRL_1 = dcoc_ctrl_1_stack; /* Restore DCOC_CTRL_1 state to prior settings */
    XCVR_RX_DIG->RX_DIG_CTRL = rx_dig_ctrl_stack; /* Restore RX_DIG_CTRL state to prior settings */
    XCVR_RX_DIG->DCOC_CAL_GAIN = dcoc_cal_gain_state; /* Restore DCOC_CAL_GAIN state to prior setting */
    XCVR_RX_DIG->AGC_CTRL_1 = agc_ctrl_1_stack; /* Save state of RX_DIG_CTRL for later restore */
}

static int kw41zrf_xcvr_configure(kw41zrf_t *dev,
    const xcvr_common_config_t *com_config,
    const xcvr_mode_config_t *mode_config,
    const xcvr_mode_datarate_config_t *mode_datarate_config,
    const xcvr_datarate_config_t *datarate_config)
{
    (void)dev;
    int config_status = 0;
    uint32_t temp;

    /* Turn on the module clocks before doing anything */
    SIM->SCGC5 |= mode_config->scgc5_clock_ena_bits;

    /* XCVR_ANA configs */

    /* Configure PLL Loop Filter */
    XCVR_ANA->SY_CTRL_1 &= ~com_config->ana_sy_ctrl1.mask;
    XCVR_ANA->SY_CTRL_1 |= com_config->ana_sy_ctrl1.init;

    /* Configure VCO KVM */
    XCVR_ANA->SY_CTRL_2 &= ~mode_datarate_config->ana_sy_ctrl2.mask;
    XCVR_ANA->SY_CTRL_2 |= mode_datarate_config->ana_sy_ctrl2.init;

    /* Configure analog filter bandwidth */
    XCVR_ANA->RX_BBA &= ~mode_datarate_config->ana_rx_bba.mask;
    XCVR_ANA->RX_BBA |= mode_datarate_config->ana_rx_bba.init;
    XCVR_ANA->RX_TZA &= ~mode_datarate_config->ana_rx_tza.mask;
    XCVR_ANA->RX_TZA |= mode_datarate_config->ana_rx_tza.init;

    temp = XCVR_ANA->TX_DAC_PA;
    temp &= ~XCVR_ANALOG_TX_DAC_PA_TX_PA_BUMP_VBIAS_MASK;
    temp |= XCVR_ANALOG_TX_DAC_PA_TX_PA_BUMP_VBIAS(4);
    XCVR_ANA->TX_DAC_PA = temp;

    temp = XCVR_ANA->BB_LDO_2;
    temp &= ~XCVR_ANALOG_BB_LDO_2_BB_LDO_VCOLO_TRIM_MASK;
    temp |= XCVR_ANALOG_BB_LDO_2_BB_LDO_VCOLO_TRIM(0);
    XCVR_ANA->BB_LDO_2 = temp;

    temp = XCVR_ANA->RX_LNA;
    temp &= ~XCVR_ANALOG_RX_LNA_RX_LNA_BUMP_MASK;
    temp |= XCVR_ANALOG_RX_LNA_RX_LNA_BUMP(1);
    XCVR_ANA->RX_LNA = temp;

    temp = XCVR_ANA->BB_LDO_1;
    temp &= ~XCVR_ANALOG_BB_LDO_1_BB_LDO_FDBK_TRIM_MASK;
    temp |= XCVR_ANALOG_BB_LDO_1_BB_LDO_FDBK_TRIM(1);
    XCVR_ANA->BB_LDO_1 = temp;

    /* XCVR_MISC configs */
    temp = XCVR_MISC->XCVR_CTRL;
    temp &= ~(mode_config->xcvr_ctrl.mask | XCVR_CTRL_XCVR_CTRL_REF_CLK_FREQ_MASK);
    temp |= mode_config->xcvr_ctrl.init;
    if (CLOCK_RADIOXTAL == 26000000ul) {
        temp |= XCVR_CTRL_XCVR_CTRL_REF_CLK_FREQ(1);
    }

    XCVR_MISC->XCVR_CTRL = temp;

    /* XCVR_PHY configs */
    XCVR_PHY->PHY_PRE_REF0 = mode_config->phy_pre_ref0_init;
    XCVR_PHY->PRE_REF1 = mode_config->phy_pre_ref1_init;
    XCVR_PHY->PRE_REF2 = mode_config->phy_pre_ref2_init;
    XCVR_PHY->CFG1 = mode_config->phy_cfg1_init;
    XCVR_PHY->CFG2 = mode_datarate_config->phy_cfg2_init;
    XCVR_PHY->EL_CFG = mode_config->phy_el_cfg_init | datarate_config->phy_el_cfg_init; /* EL_WIN_SIZE and EL_INTERVAL are datarate dependent, */

    /* XCVR_PLL_DIG configs */
    XCVR_PLL_DIG->HPM_BUMP = com_config->pll_hpm_bump;
    XCVR_PLL_DIG->MOD_CTRL = com_config->pll_mod_ctrl;
    XCVR_PLL_DIG->CHAN_MAP = com_config->pll_chan_map;
    XCVR_PLL_DIG->LOCK_DETECT = com_config->pll_lock_detect;
    XCVR_PLL_DIG->HPM_CTRL = com_config->pll_hpm_ctrl;
    XCVR_PLL_DIG->HPMCAL_CTRL = com_config->pll_hpmcal_ctrl;
    XCVR_PLL_DIG->HPM_SDM_RES = com_config->pll_hpm_sdm_res;
    XCVR_PLL_DIG->LPM_CTRL = com_config->pll_lpm_ctrl;
    XCVR_PLL_DIG->LPM_SDM_CTRL1 = com_config->pll_lpm_sdm_ctrl1;
    XCVR_PLL_DIG->DELAY_MATCH = com_config->pll_delay_match;
    XCVR_PLL_DIG->CTUNE_CTRL = com_config->pll_ctune_ctrl;

    /* XCVR_RX_DIG configs */

    /* Configure RF Aux PLL for proper operation based on external clock frequency */
    temp = XCVR_ANA->RX_AUXPLL;
    temp &= ~XCVR_ANALOG_RX_AUXPLL_VCO_DAC_REF_ADJUST_MASK;
    if (CLOCK_RADIOXTAL == 26000000ul) {
        temp |= XCVR_ANALOG_RX_AUXPLL_VCO_DAC_REF_ADJUST(4);
    }
    else {
        temp |= XCVR_ANALOG_RX_AUXPLL_VCO_DAC_REF_ADJUST(7);
    }
    XCVR_ANA->RX_AUXPLL = temp;

    /* Configure RX_DIG_CTRL */
    if (CLOCK_RADIOXTAL == 26000000ul) {
        temp = com_config->rx_dig_ctrl_init | /* Common portion of RX_DIG_CTRL init */
               mode_config->rx_dig_ctrl_init_26mhz | /* Mode  specific portion of RX_DIG_CTRL init */
               datarate_config->rx_dig_ctrl_init_26mhz | /* Datarate specific portion of RX_DIG_CTRL init */
               XCVR_RX_DIG_RX_DIG_CTRL_RX_SRC_EN_MASK; /* Always enable the sample rate converter for 26MHz */
    }
    else {
        temp = com_config->rx_dig_ctrl_init | /* Common portion of RX_DIG_CTRL init */
               mode_config->rx_dig_ctrl_init_32mhz | /* Mode specific portion of RX_DIG_CTRL init */
               datarate_config->rx_dig_ctrl_init_32mhz | /* Datarate specific portion of RX_DIG_CTRL init */
               0; /* Always disable the sample rate converter for 32MHz */
    }

    temp |= com_config->rx_dig_ctrl_init;  /* Common portion of RX_DIG_CTRL init */
    XCVR_RX_DIG->RX_DIG_CTRL = temp;

    /* DCOC_CAL_IIR */
    if (CLOCK_RADIOXTAL == 26000000ul) {
        XCVR_RX_DIG->DCOC_CAL_IIR = datarate_config->dcoc_cal_iir_init_26mhz;
    }
    else {
        XCVR_RX_DIG->DCOC_CAL_IIR = datarate_config->dcoc_cal_iir_init_32mhz;
    }

    /* DC_RESID_CTRL */
    if (CLOCK_RADIOXTAL == 26000000ul) {
        XCVR_RX_DIG->DC_RESID_CTRL = com_config->dc_resid_ctrl_init | datarate_config->dc_resid_ctrl_26mhz;
    }
    else {
        XCVR_RX_DIG->DC_RESID_CTRL = com_config->dc_resid_ctrl_init | datarate_config->dc_resid_ctrl_32mhz;
    }

    /* DCOC_CTRL_0  & _1 */
    if (CLOCK_RADIOXTAL == 26000000ul) {
        XCVR_RX_DIG->DCOC_CTRL_0 = com_config->dcoc_ctrl_0_init_26mhz | datarate_config->dcoc_ctrl_0_init_26mhz; /* Combine common and datarate specific settings */
        XCVR_RX_DIG->DCOC_CTRL_1 = com_config->dcoc_ctrl_1_init | datarate_config->dcoc_ctrl_1_init_26mhz; /* Combine common and datarate specific settings */

        /* customize DCOC_CTRL_0 settings for Gen2 GFSK BT=0.5, h=0.32 */
        if ((mode_config->radio_mode == ANT_MODE) || (mode_config->radio_mode == GFSK_BT_0p5_h_0p32))
        {
            if (datarate_config->data_rate == DR_1MBPS) /* only apply fix to 1Mbps data rates */
            {
                /* apply the changes to the DCOC_CTRL_0 register XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORR_DLY & XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORR_HOLD_TIME */
                temp = XCVR_RX_DIG->DCOC_CTRL_0;
                temp &= ~XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORR_DLY_MASK | XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORR_HOLD_TIME_MASK;
                temp |= XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORR_DLY(0x10) | XCVR_RX_DIG_DCOC_CTRL_0_DCOC_CORR_HOLD_TIME(0x0C);
                XCVR_RX_DIG->DCOC_CTRL_0 = temp;
            }
        }
    }
    else {
        XCVR_RX_DIG->DCOC_CTRL_0 = com_config->dcoc_ctrl_0_init_32mhz | datarate_config->dcoc_ctrl_0_init_32mhz; /* Combine common and datarate specific settings */
        XCVR_RX_DIG->DCOC_CTRL_1 = com_config->dcoc_ctrl_1_init | datarate_config->dcoc_ctrl_1_init_32mhz; /* Combine common and datarate specific settings */
    }

    /* DCOC_CAL_GAIN */
    XCVR_RX_DIG->DCOC_CAL_GAIN = com_config->dcoc_cal_gain_init;

    /* DCOC_CAL_RCP */
    XCVR_RX_DIG->DCOC_CAL_RCP = com_config->dcoc_cal_rcp_init;
    XCVR_RX_DIG->LNA_GAIN_VAL_3_0 = com_config->lna_gain_val_3_0;
    XCVR_RX_DIG->LNA_GAIN_VAL_7_4 = com_config->lna_gain_val_7_4;
    XCVR_RX_DIG->LNA_GAIN_VAL_8 = com_config->lna_gain_val_8;
    XCVR_RX_DIG->BBA_RES_TUNE_VAL_7_0 = com_config->bba_res_tune_val_7_0;
    XCVR_RX_DIG->BBA_RES_TUNE_VAL_10_8 = com_config->bba_res_tune_val_10_8;

    /* LNA_GAIN_LIN_VAL */
    XCVR_RX_DIG->LNA_GAIN_LIN_VAL_2_0 = com_config->lna_gain_lin_val_2_0_init;
    XCVR_RX_DIG->LNA_GAIN_LIN_VAL_5_3 = com_config->lna_gain_lin_val_5_3_init;
    XCVR_RX_DIG->LNA_GAIN_LIN_VAL_8_6 = com_config->lna_gain_lin_val_8_6_init;
    XCVR_RX_DIG->LNA_GAIN_LIN_VAL_9 = com_config->lna_gain_lin_val_9_init;

    /* BBA_RES_TUNE_LIN_VAL */
    XCVR_RX_DIG->BBA_RES_TUNE_LIN_VAL_3_0 = com_config->bba_res_tune_lin_val_3_0_init;
    XCVR_RX_DIG->BBA_RES_TUNE_LIN_VAL_7_4 = com_config->bba_res_tune_lin_val_7_4_init;
    XCVR_RX_DIG->BBA_RES_TUNE_LIN_VAL_10_8 = com_config->bba_res_tune_lin_val_10_8_init;

    /* BBA_STEP */
    XCVR_RX_DIG->DCOC_BBA_STEP = com_config->dcoc_bba_step_init;

    /* DCOC_TZA_STEP */
    XCVR_RX_DIG->DCOC_TZA_STEP_0 = com_config->dcoc_tza_step_00_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_1 = com_config->dcoc_tza_step_01_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_2 = com_config->dcoc_tza_step_02_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_3 = com_config->dcoc_tza_step_03_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_4 = com_config->dcoc_tza_step_04_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_5 = com_config->dcoc_tza_step_05_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_6 = com_config->dcoc_tza_step_06_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_7 = com_config->dcoc_tza_step_07_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_8 = com_config->dcoc_tza_step_08_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_9 = com_config->dcoc_tza_step_09_init;
    XCVR_RX_DIG->DCOC_TZA_STEP_10 = com_config->dcoc_tza_step_10_init;

    /* AGC_CTRL_0 .. _3 */
    XCVR_RX_DIG->AGC_CTRL_0 = com_config->agc_ctrl_0_init | mode_config->agc_ctrl_0_init;

    if (CLOCK_RADIOXTAL == 26000000ul) {
        XCVR_RX_DIG->AGC_CTRL_1 = com_config->agc_ctrl_1_init_26mhz | datarate_config->agc_ctrl_1_init_26mhz; /* Combine common and datarate specific settings */
        XCVR_RX_DIG->AGC_CTRL_2 = mode_datarate_config->agc_ctrl_2_init_26mhz;
    }
    else {
        XCVR_RX_DIG->AGC_CTRL_1 = com_config->agc_ctrl_1_init_32mhz | datarate_config->agc_ctrl_1_init_32mhz; /* Combine common and datarate specific settings */
        XCVR_RX_DIG->AGC_CTRL_2 = mode_datarate_config->agc_ctrl_2_init_32mhz;
    }

    XCVR_RX_DIG->AGC_CTRL_3 = com_config->agc_ctrl_3_init;

    /* AGC_GAIN_TBL_** */
    XCVR_RX_DIG->AGC_GAIN_TBL_03_00 = com_config->agc_gain_tbl_03_00_init;
    XCVR_RX_DIG->AGC_GAIN_TBL_07_04 = com_config->agc_gain_tbl_07_04_init;
    XCVR_RX_DIG->AGC_GAIN_TBL_11_08 = com_config->agc_gain_tbl_11_08_init;
    XCVR_RX_DIG->AGC_GAIN_TBL_15_12 = com_config->agc_gain_tbl_15_12_init;
    XCVR_RX_DIG->AGC_GAIN_TBL_19_16 = com_config->agc_gain_tbl_19_16_init;
    XCVR_RX_DIG->AGC_GAIN_TBL_23_20 = com_config->agc_gain_tbl_23_20_init;
    XCVR_RX_DIG->AGC_GAIN_TBL_26_24 = com_config->agc_gain_tbl_26_24_init;

    /* RSSI_CTRL_0 */
    XCVR_RX_DIG->RSSI_CTRL_0 = com_config->rssi_ctrl_0_init;

    /* CCA_ED_LQI_0 and _1 */
    XCVR_RX_DIG->CCA_ED_LQI_CTRL_0 = com_config->cca_ed_lqi_ctrl_0_init;
    XCVR_RX_DIG->CCA_ED_LQI_CTRL_1 = com_config->cca_ed_lqi_ctrl_1_init;

    /* Channel filter coefficients */
    if (CLOCK_RADIOXTAL == 26000000ul) {
        XCVR_RX_DIG->RX_CHF_COEF_0 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_0;
        XCVR_RX_DIG->RX_CHF_COEF_1 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_1;
        XCVR_RX_DIG->RX_CHF_COEF_2 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_2;
        XCVR_RX_DIG->RX_CHF_COEF_3 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_3;
        XCVR_RX_DIG->RX_CHF_COEF_4 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_4;
        XCVR_RX_DIG->RX_CHF_COEF_5 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_5;
        XCVR_RX_DIG->RX_CHF_COEF_6 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_6;
        XCVR_RX_DIG->RX_CHF_COEF_7 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_7;
        XCVR_RX_DIG->RX_CHF_COEF_8 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_8;
        XCVR_RX_DIG->RX_CHF_COEF_9 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_9;
        XCVR_RX_DIG->RX_CHF_COEF_10 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_10;
        XCVR_RX_DIG->RX_CHF_COEF_11 = mode_datarate_config->rx_chf_coeffs_26mhz.rx_chf_coef_11;
    }
    else {
        XCVR_RX_DIG->RX_CHF_COEF_0 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_0;
        XCVR_RX_DIG->RX_CHF_COEF_1 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_1;
        XCVR_RX_DIG->RX_CHF_COEF_2 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_2;
        XCVR_RX_DIG->RX_CHF_COEF_3 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_3;
        XCVR_RX_DIG->RX_CHF_COEF_4 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_4;
        XCVR_RX_DIG->RX_CHF_COEF_5 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_5;
        XCVR_RX_DIG->RX_CHF_COEF_6 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_6;
        XCVR_RX_DIG->RX_CHF_COEF_7 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_7;
        XCVR_RX_DIG->RX_CHF_COEF_8 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_8;
        XCVR_RX_DIG->RX_CHF_COEF_9 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_9;
        XCVR_RX_DIG->RX_CHF_COEF_10 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_10;
        XCVR_RX_DIG->RX_CHF_COEF_11 = mode_datarate_config->rx_chf_coeffs_32mhz.rx_chf_coef_11;
    }

    XCVR_RX_DIG->RX_RCCAL_CTRL0 = mode_datarate_config->rx_rccal_ctrl_0;
    XCVR_RX_DIG->RX_RCCAL_CTRL1 = mode_datarate_config->rx_rccal_ctrl_1;

    /* XCVR_TSM configs */
    XCVR_TSM->CTRL = com_config->tsm_ctrl;

    if ((mode_config->radio_mode != ZIGBEE_MODE) && (mode_config->radio_mode != BLE_MODE))
    {
        XCVR_TSM->CTRL &= ~XCVR_TSM_CTRL_DATA_PADDING_EN_MASK;
    }

    XCVR_MISC->LPPS_CTRL = com_config->lpps_ctrl_init; /* Register is in XCVR_MISC but grouped with TSM for initialization */

    XCVR_TSM->OVRD2 = com_config->tsm_ovrd2_init;
    /* TSM registers and timings - dependent upon clock frequency */
    if (CLOCK_RADIOXTAL == 26000000ul) {
        XCVR_TSM->END_OF_SEQ = com_config->end_of_seq_init_26mhz;
        XCVR_TSM->FAST_CTRL2 = com_config->tsm_fast_ctrl2_init_26mhz;
        XCVR_TSM->RECYCLE_COUNT = com_config->recycle_count_init_26mhz;
        XCVR_TSM->TIMING14 = com_config->tsm_timing_14_init_26mhz;
        XCVR_TSM->TIMING16 = com_config->tsm_timing_16_init_26mhz;
        XCVR_TSM->TIMING25 = com_config->tsm_timing_25_init_26mhz;
        XCVR_TSM->TIMING27 = com_config->tsm_timing_27_init_26mhz;
        XCVR_TSM->TIMING28 = com_config->tsm_timing_28_init_26mhz;
        XCVR_TSM->TIMING29 = com_config->tsm_timing_29_init_26mhz;
        XCVR_TSM->TIMING30 = com_config->tsm_timing_30_init_26mhz;
        XCVR_TSM->TIMING31 = com_config->tsm_timing_31_init_26mhz;
        XCVR_TSM->TIMING32 = com_config->tsm_timing_32_init_26mhz;
        XCVR_TSM->TIMING33 = com_config->tsm_timing_33_init_26mhz;
        XCVR_TSM->TIMING36 = com_config->tsm_timing_36_init_26mhz;
        XCVR_TSM->TIMING37 = com_config->tsm_timing_37_init_26mhz;
        XCVR_TSM->TIMING39 = com_config->tsm_timing_39_init_26mhz;
        XCVR_TSM->TIMING40 = com_config->tsm_timing_40_init_26mhz;
        XCVR_TSM->TIMING41 = com_config->tsm_timing_41_init_26mhz;
        XCVR_TSM->TIMING52 = com_config->tsm_timing_52_init_26mhz;
        XCVR_TSM->TIMING54 = com_config->tsm_timing_54_init_26mhz;
        XCVR_TSM->TIMING55 = com_config->tsm_timing_55_init_26mhz;
        XCVR_TSM->TIMING56 = com_config->tsm_timing_56_init_26mhz;
    }
    else {
        XCVR_TSM->END_OF_SEQ = com_config->end_of_seq_init_32mhz;
        XCVR_TSM->FAST_CTRL2 = com_config->tsm_fast_ctrl2_init_32mhz;
        XCVR_TSM->RECYCLE_COUNT = com_config->recycle_count_init_32mhz;
        XCVR_TSM->TIMING14 = com_config->tsm_timing_14_init_32mhz;
        XCVR_TSM->TIMING16 = com_config->tsm_timing_16_init_32mhz;
        XCVR_TSM->TIMING25 = com_config->tsm_timing_25_init_32mhz;
        XCVR_TSM->TIMING27 = com_config->tsm_timing_27_init_32mhz;
        XCVR_TSM->TIMING28 = com_config->tsm_timing_28_init_32mhz;
        XCVR_TSM->TIMING29 = com_config->tsm_timing_29_init_32mhz;
        XCVR_TSM->TIMING30 = com_config->tsm_timing_30_init_32mhz;
        XCVR_TSM->TIMING31 = com_config->tsm_timing_31_init_32mhz;
        XCVR_TSM->TIMING32 = com_config->tsm_timing_32_init_32mhz;
        XCVR_TSM->TIMING33 = com_config->tsm_timing_33_init_32mhz;
        XCVR_TSM->TIMING36 = com_config->tsm_timing_36_init_32mhz;
        XCVR_TSM->TIMING37 = com_config->tsm_timing_37_init_32mhz;
        XCVR_TSM->TIMING39 = com_config->tsm_timing_39_init_32mhz;
        XCVR_TSM->TIMING40 = com_config->tsm_timing_40_init_32mhz;
        XCVR_TSM->TIMING41 = com_config->tsm_timing_41_init_32mhz;
        XCVR_TSM->TIMING52 = com_config->tsm_timing_52_init_32mhz;
        XCVR_TSM->TIMING54 = com_config->tsm_timing_54_init_32mhz;
        XCVR_TSM->TIMING55 = com_config->tsm_timing_55_init_32mhz;
        XCVR_TSM->TIMING56 = com_config->tsm_timing_56_init_32mhz;
    }

    /* TSM timings independent of clock frequency */
    XCVR_TSM->TIMING00 = com_config->tsm_timing_00_init;
    XCVR_TSM->TIMING01 = com_config->tsm_timing_01_init;
    XCVR_TSM->TIMING02 = com_config->tsm_timing_02_init;
    XCVR_TSM->TIMING03 = com_config->tsm_timing_03_init;
    XCVR_TSM->TIMING04 = com_config->tsm_timing_04_init;
    XCVR_TSM->TIMING05 = com_config->tsm_timing_05_init;
    XCVR_TSM->TIMING06 = com_config->tsm_timing_06_init;
    XCVR_TSM->TIMING07 = com_config->tsm_timing_07_init;
    XCVR_TSM->TIMING08 = com_config->tsm_timing_08_init;
    XCVR_TSM->TIMING09 = com_config->tsm_timing_09_init;
    XCVR_TSM->TIMING10 = com_config->tsm_timing_10_init;
    XCVR_TSM->TIMING11 = com_config->tsm_timing_11_init;
    XCVR_TSM->TIMING12 = com_config->tsm_timing_12_init;
    XCVR_TSM->TIMING13 = com_config->tsm_timing_13_init;
    XCVR_TSM->TIMING15 = com_config->tsm_timing_15_init;
    XCVR_TSM->TIMING17 = com_config->tsm_timing_17_init;
    XCVR_TSM->TIMING18 = com_config->tsm_timing_18_init;
    XCVR_TSM->TIMING19 = com_config->tsm_timing_19_init;
    XCVR_TSM->TIMING20 = com_config->tsm_timing_20_init;
    XCVR_TSM->TIMING21 = com_config->tsm_timing_21_init;
    XCVR_TSM->TIMING22 = com_config->tsm_timing_22_init;
    XCVR_TSM->TIMING23 = com_config->tsm_timing_23_init;
    XCVR_TSM->TIMING24 = com_config->tsm_timing_24_init;
    XCVR_TSM->TIMING26 = com_config->tsm_timing_26_init;
    XCVR_TSM->TIMING34 = com_config->tsm_timing_34_init;
    XCVR_TSM->TIMING35 = com_config->tsm_timing_35_init;
    XCVR_TSM->TIMING38 = com_config->tsm_timing_38_init;
    XCVR_TSM->TIMING51 = com_config->tsm_timing_51_init;
    XCVR_TSM->TIMING53 = com_config->tsm_timing_53_init;
    XCVR_TSM->TIMING57 = com_config->tsm_timing_57_init;
    XCVR_TSM->TIMING58 = com_config->tsm_timing_58_init;

    if (CLOCK_RADIOXTAL == 26000000ul) {
        XCVR_TSM->END_OF_SEQ = XCVR_TSM_END_OF_SEQ_END_OF_TX_WU(END_OF_TX_WU) |
                               XCVR_TSM_END_OF_SEQ_END_OF_TX_WD(END_OF_TX_WD) |
                               XCVR_TSM_END_OF_SEQ_END_OF_RX_WU(END_OF_RX_WU_26MHZ) |
                               XCVR_TSM_END_OF_SEQ_END_OF_RX_WD(END_OF_RX_WD_26MHZ);
    }
    else {
        XCVR_TSM->END_OF_SEQ = XCVR_TSM_END_OF_SEQ_END_OF_TX_WU(END_OF_TX_WU) |
                               XCVR_TSM_END_OF_SEQ_END_OF_TX_WD(END_OF_TX_WD) |
                               XCVR_TSM_END_OF_SEQ_END_OF_RX_WU(END_OF_RX_WU) |
                               XCVR_TSM_END_OF_SEQ_END_OF_RX_WD(END_OF_RX_WD);
    }

    XCVR_TSM->PA_RAMP_TBL0 = com_config->pa_ramp_tbl_0_init;
    XCVR_TSM->PA_RAMP_TBL1 = com_config->pa_ramp_tbl_1_init;

    if ((mode_datarate_config->radio_mode == MSK) && ((mode_datarate_config->data_rate == DR_500KBPS) || (mode_datarate_config->data_rate == DR_250KBPS))) {
        /* Apply a specific value of TX_DIG_EN which assumes no DATA PADDING */
        XCVR_TSM->TIMING35 = com_config->tsm_timing_35_init | B0(TX_DIG_EN_ASSERT_MSK500); /* LSbyte is mode specific */
    }
    else {
        XCVR_TSM->TIMING35 = com_config->tsm_timing_35_init | mode_config->tsm_timing_35_init; /* LSbyte is mode specific, other bytes are common */
    }

    /* XCVR_TX_DIG configs */
    if (CLOCK_RADIOXTAL == 26000000ul) {
        XCVR_TX_DIG->FSK_SCALE = mode_datarate_config->tx_fsk_scale_26mhz; /* Applies only to 802.15.4 & MSK but won't harm other protocols */
        XCVR_TX_DIG->GFSK_COEFF1 = mode_config->tx_gfsk_coeff1_26mhz;
        XCVR_TX_DIG->GFSK_COEFF2 = mode_config->tx_gfsk_coeff2_26mhz;
    }
    else {
        XCVR_TX_DIG->FSK_SCALE = mode_datarate_config->tx_fsk_scale_32mhz; /* Applies only to 802.15.4 & MSK but won't harm other protocols */
        XCVR_TX_DIG->GFSK_COEFF1 = mode_config->tx_gfsk_coeff1_32mhz;
        XCVR_TX_DIG->GFSK_COEFF2 = mode_config->tx_gfsk_coeff2_32mhz;
    }

    XCVR_TX_DIG->CTRL = com_config->tx_ctrl;
    XCVR_TX_DIG->DATA_PADDING = com_config->tx_data_padding;
    XCVR_TX_DIG->DFT_PATTERN = com_config->tx_dft_pattern;

    XCVR_TX_DIG->RF_DFT_BIST_1 = com_config->rf_dft_bist_1;
    XCVR_TX_DIG->RF_DFT_BIST_2 = com_config->rf_dft_bist_2;

    XCVR_TX_DIG->GFSK_CTRL = mode_config->tx_gfsk_ctrl;

    /* Force receiver warmup */
    bit_set32(&XCVR_TSM->CTRL, XCVR_TSM_CTRL_FORCE_RX_EN_SHIFT);
    /* Wait for TSM to reach the end of warmup (unless you want to capture some samples during DCOC cal phase) */
    uint32_t end_of_rx_wu = XCVR_CTRL_XCVR_STATUS_TSM_COUNT(
        (XCVR_TSM->END_OF_SEQ & XCVR_TSM_END_OF_SEQ_END_OF_RX_WU_MASK) >>
            XCVR_TSM_END_OF_SEQ_END_OF_RX_WU_SHIFT);
    while ((XCVR_MISC->XCVR_STATUS & XCVR_CTRL_XCVR_STATUS_TSM_COUNT_MASK) != end_of_rx_wu) {};

    int res = kw41zrf_rx_bba_dcoc_dac_trim_DCest();
    if (res < 0) {
        config_status = res;
    }
    //~ DCOC_DAC_INIT_Cal(0);
    kw41zrf_dcoc_dac_init_cal();
    /* Force receiver warmdown */
    bit_clear32(&XCVR_TSM->CTRL, XCVR_TSM_CTRL_FORCE_RX_EN_SHIFT);

    return config_status;
}

int kw41zrf_xcvr_init(kw41zrf_t *dev)
{
    (void) dev;
    uint8_t radio_id = ((RSIM->MISC & RSIM_MISC_RADIO_VERSION_MASK) >> RSIM_MISC_RADIO_VERSION_SHIFT);
    switch (radio_id) {
        case 0x3: /* KW41/31/21 v1 */
        case 0xb: /* KW41/31/21 v1.1 */
            break;
        default:
            return -ENODEV;
    }

    RSIM->RF_OSC_CTRL = (RSIM->RF_OSC_CTRL &
        ~(RSIM_RF_OSC_CTRL_RADIO_EXT_OSC_OVRD_MASK)) | /* Set EXT_OSC_OVRD value to zero */
        RSIM_RF_OSC_CTRL_RADIO_EXT_OSC_OVRD_EN_MASK; /* Enable over-ride with zero value */
    bit_set32(&SIM->SCGC5, SIM_SCGC5_PHYDIG_SHIFT); /* Enable PHY clock gate */

    /* Load IFR trim values */
    IFR_SW_TRIM_TBL_ENTRY_T sw_trim_tbl[] =
    {
        {TRIM_STATUS, 0, 0}, /*< Fetch the trim status word if available.*/
        {TRIM_VERSION, 0, 0} /*< Fetch the trim version number if available.*/
    };
    handle_ifr(&sw_trim_tbl[0], ARRAY_SIZE(sw_trim_tbl));
    DEBUG("[kw41zrf] sw_trim_tbl:\n");

    for (unsigned k = 0; k < ARRAY_SIZE(sw_trim_tbl); ++k) {
        DEBUG("[kw41zrf] [%u] id=0x%04x ", k, (unsigned)sw_trim_tbl[k].trim_id);
        if (sw_trim_tbl[k].trim_id == TRIM_STATUS) {
            DEBUG("(TRIM_STATUS)  ");
        }
        else if (sw_trim_tbl[k].trim_id == TRIM_VERSION) {
            DEBUG("(TRIM_VERSION) ");
        }
        DEBUG("value=%" PRIu32 ", valid=%u\n", sw_trim_tbl[k].trim_value,
            (unsigned)sw_trim_tbl[k].valid);
    }

    /* We only use 802.15.4 mode in this driver */
    xcvrStatus_t status = kw41zrf_xcvr_configure(dev, &xcvr_common_config,
        &zgbe_mode_config, &xcvr_ZIGBEE_500kbps_config, &xcvr_802_15_4_500kbps_config);

    if (status != gXcvrSuccess_c) {
        return -EIO;
    }
    return 0;
}
