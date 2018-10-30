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

#include "cpu.h"
#include "fsl_xcvr.h"
#include "fsl_xcvr_trim.h"
#include <inttypes.h>
#include <stdio.h>

#define ENABLE_DEBUG (1)
#include "debug.h"

/*******************************************************************************
* Definitions
******************************************************************************/
#define DCOC_DAC_BBF_STEP (16)

/*******************************************************************************
* Prototypes
******************************************************************************/

/*******************************************************************************
* Variables
******************************************************************************/
const int8_t TsettleCal = 10;

/*******************************************************************************
 * Macros
 ******************************************************************************/
#define ISIGN(x) !((uint16_t)x & 0x8000)
#define ABS(x) ((x) > 0 ? (x) : -(x))
#ifndef MIN
#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif


/*******************************************************************************
 * Code
 ******************************************************************************/

static uint32_t calc_dcoc_dac_slope(int16_t lower, int16_t upper)
{
    /* Normalize internal measurement */
    int16_t norm_dc_code = upper - lower;
    if (norm_dc_code < 0) {
        norm_dc_code = -norm_dc_code;
    }
    uint32_t dc_step = ((uint32_t)norm_dc_code << 16) / DCOC_DAC_BBF_STEP;
    printf("slope: %d, %d -> %f\n", (int) lower, (int)upper, (float)dc_step / (1 << 16));
    return dc_step;
}

void calc_tza_step_rcp_reg(TZAdcocstep_t *out, uint32_t base_step, uint16_t gain, uint16_t gain0)
{
    uint32_t step = (base_step * gain + (gain0 >> 1)) / gain0;
    /* Calculate TZA DCOC STEPSIZE & its RECIPROCAL */
    out->dcoc_step = (step + (1u << 12)) >> 13;
    out->dcoc_step_rcp = (0x80000000ul + (step >> 1)) / step;
}

/*! *********************************************************************************
 * \brief  Temporary delay function
 *
 * \param[in] none.
 *
 * \return none.
 *
 * \details
 *
 ***********************************************************************************/
void XcvrCalDelay(uint32_t time)
{
    time *= 32; /* Time delay is roughly in uSec. */
    while(time > 0)
    {
        time--;
    }
}

/*! *********************************************************************************
 * rx_dc_est_average : Get DC EST values and return the Average
 ***********************************************************************************/
void rx_dc_est_average(int16_t * i_avg, int16_t * q_avg)
{
    static const unsigned nsamples = 64;
    int32_t avg_i = 0;
    int32_t avg_q = 0;

     /* Wait for TSM to reach the end of warmup (unless you want to capture some samples during DCOC cal phase). */
    uint32_t end_of_rx_wu = (XCVR_TSM->END_OF_SEQ & XCVR_TSM_END_OF_SEQ_END_OF_RX_WU_MASK) >> XCVR_TSM_END_OF_SEQ_END_OF_RX_WU_SHIFT;
    while ((( XCVR_MISC->XCVR_STATUS & XCVR_CTRL_XCVR_STATUS_TSM_COUNT_MASK) >> XCVR_CTRL_XCVR_STATUS_TSM_COUNT_SHIFT ) != end_of_rx_wu) {};

    /* Read DCOC DC EST register. */
    for (unsigned k = 0; k < nsamples; k++)
    {
        uint32_t dc_temp = XCVR_RX_DIG->DCOC_DC_EST;
        int16_t dc_meas_i = (dc_temp & XCVR_RX_DIG_DCOC_DC_EST_DC_EST_I_MASK) >> XCVR_RX_DIG_DCOC_DC_EST_DC_EST_I_SHIFT;
        dc_meas_i = (int16_t)(dc_meas_i << 4) / 16; /* Sign extend from 12 to 16 bits. */
        avg_i += dc_meas_i;

        int16_t dc_meas_q = (dc_temp & XCVR_RX_DIG_DCOC_DC_EST_DC_EST_Q_MASK) >> XCVR_RX_DIG_DCOC_DC_EST_DC_EST_Q_SHIFT;
        dc_meas_q = (int16_t)(dc_meas_q << 4) / 16; /* Sign extend from 12 to 16 bits. */
        avg_q += dc_meas_q;
    }

    avg_i /= nsamples;
    avg_q /= nsamples;

    *i_avg = (int16_t)avg_i;
    *q_avg = (int16_t)avg_q;
}

/*! *********************************************************************************
 * \brief  This function performs a trim of the BBA DCOC DAC on the DUT
 *
 * \return status - 1 if passed, 0 if failed.
 *
 * \ingroup PublicAPIs
 *
 * \details
 *   Requires the RX to be warmed up before this function is called.
 *
 ***********************************************************************************/
uint8_t rx_bba_dcoc_dac_trim_DCest(void)
{
    /* Estimate the actual gain by measuring three points and approximating a line */
    TZAdcocstep_t tza_dcoc_step[11];
    uint8_t status = 0;

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

    XcvrCalDelay(TsettleCal * 2);

    int16_t dc_meas_i_nominal = 0;
    int16_t dc_meas_q_nominal = 0;
    /* TODO: remove this since it cancels out in the calculations below */
    rx_dc_est_average(&dc_meas_i_nominal, &dc_meas_q_nominal);

    /* SWEEP I/Q CHANNEL */
    int16_t dc_meas_i = 0;
    int16_t dc_meas_q = 0;
    /* BBF NEG STEP */
    XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(bbf_dacinit_i - DCOC_DAC_BBF_STEP) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(bbf_dacinit_q - DCOC_DAC_BBF_STEP) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(tza_dacinit_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(tza_dacinit_q);
    XcvrCalDelay(TsettleCal * 4);

    rx_dc_est_average(&dc_meas_i, &dc_meas_q);
    uint32_t temp_mi = calc_dcoc_dac_slope(dc_meas_i, dc_meas_i_nominal);
    uint32_t temp_mq = calc_dcoc_dac_slope(dc_meas_q, dc_meas_q_nominal);

    /* BBF POS STEP */
    XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(bbf_dacinit_i + DCOC_DAC_BBF_STEP) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(bbf_dacinit_q + DCOC_DAC_BBF_STEP) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(tza_dacinit_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(tza_dacinit_q);
    XcvrCalDelay(TsettleCal * 2);
    rx_dc_est_average(&dc_meas_i, &dc_meas_q);
    uint32_t temp_pi = calc_dcoc_dac_slope(dc_meas_i_nominal, dc_meas_i);
    uint32_t temp_pq = calc_dcoc_dac_slope(dc_meas_q_nominal, dc_meas_q);

    XCVR_RX_DIG->DCOC_DAC_INIT = dcoc_init_reg_value_dcgain; /* Return DAC setting to initial */

    /* Calculate BBF DCOC STEPS, RECIPROCALS */
    /* calc_dcoc_dac_step2 divides by 16 internally, no risk of overflow, temp_xy < (1 << 28) */
    uint32_t meas_sum = (temp_mi + temp_pi + temp_mq + temp_pq);
    float temp_step = ((float)meas_sum / 4) / (1 << 16);
    /* rounded result, (temp_step * 8) == (meas_sum * 2) */
    uint32_t bbf_dcoc_step = (meas_sum + (1u << 14)) >> 15;

    DEBUG("temp_mi = %f\n", (float)temp_mi / (1 << 16));
    DEBUG("temp_mq = %f\n", (float)temp_mq / (1 << 16));
    DEBUG("temp_pi = %f\n", (float)temp_pi / (1 << 16));
    DEBUG("temp_pq = %f\n", (float)temp_pq / (1 << 16));
    DEBUG("temp_step = %f\n", (float)temp_step);
    DEBUG("bbf_dcoc_step = %"PRIu32"\n", bbf_dcoc_step);


    if ((bbf_dcoc_step > 265) & (bbf_dcoc_step < 305))
    {
        /* rounded result, (0x8000 / temp_step) */
        uint32_t bbf_dcoc_step_rcp = (0x80000000ul + (meas_sum >> 3)) / (meas_sum >> 2);
        DEBUG("bbf_dcoc_step_rcp = %"PRIu32"\n", bbf_dcoc_step_rcp);

        /* Calculate TZA DCOC STEPS & RECIPROCALS and IQ_DC_GAIN_MISMATCH */
        /* (bbf_dcoc_step >> 3U) / 3.6F */
        uint32_t base_step = (((bbf_dcoc_step * 5u) << 13) / 18u);
        temp_step = (float)base_step / (1u << 16);
        DEBUG("temp_step0 = %f\n", temp_step);
        /* rounded result, (temp_step * 8) */
        tza_dcoc_step[0].dcoc_step = (base_step + (1u << 12)) >> 13;
        DEBUG("tza_dcoc_step[0].dcoc_step = %u\n", (unsigned)tza_dcoc_step[0].dcoc_step);
        /* rounded result, (0x8000 / temp_step) */
        tza_dcoc_step[0].dcoc_step_rcp = (0x80000000ul + (base_step >> 1)) / base_step;
        DEBUG("tza_dcoc_step[0].dcoc_step_rcp = %u\n", (unsigned)tza_dcoc_step[0].dcoc_step_rcp);
        const uint32_t *dcoc_tza_step_ptr = &xcvr_common_config.dcoc_tza_step_00_init;
        uint16_t gain0 = (dcoc_tza_step_ptr[0] >> XCVR_RX_DIG_DCOC_TZA_STEP_0_DCOC_TZA_STEP_GAIN_0_SHIFT);
        temp_step /= gain0;
        DEBUG("base step = %" PRIx32 " %f\n", base_step, temp_step);
        for (unsigned k = 1; k < 11; ++k)
        {
            uint16_t gain = (dcoc_tza_step_ptr[k] >> XCVR_RX_DIG_DCOC_TZA_STEP_0_DCOC_TZA_STEP_GAIN_0_SHIFT);
            calc_tza_step_rcp_reg(&tza_dcoc_step[k], base_step, gain, gain0);
            DEBUG("tza_dcoc_step[%u].dcoc_step = %u\n", k, (unsigned)tza_dcoc_step[k].dcoc_step);
            DEBUG("tza_dcoc_step[%u].dcoc_step_rcp = %u\n", k, (unsigned)tza_dcoc_step[k].dcoc_step_rcp);
        }

        /* Make the trims active */
        XCVR_RX_DIG->DCOC_BBA_STEP =
            XCVR_RX_DIG_DCOC_BBA_STEP_BBA_DCOC_STEP(bbf_dcoc_step) |
            XCVR_RX_DIG_DCOC_BBA_STEP_BBA_DCOC_STEP_RECIP(bbf_dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_0 =
            XCVR_RX_DIG_DCOC_TZA_STEP_0_DCOC_TZA_STEP_GAIN_0(tza_dcoc_step[0].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_0_DCOC_TZA_STEP_RCP_0(tza_dcoc_step[0].dcoc_step_rcp) ;
        XCVR_RX_DIG->DCOC_TZA_STEP_1 =
            XCVR_RX_DIG_DCOC_TZA_STEP_1_DCOC_TZA_STEP_GAIN_1(tza_dcoc_step[1].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_1_DCOC_TZA_STEP_RCP_1(tza_dcoc_step[1].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_2 =
            XCVR_RX_DIG_DCOC_TZA_STEP_2_DCOC_TZA_STEP_GAIN_2(tza_dcoc_step[2].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_2_DCOC_TZA_STEP_RCP_2(tza_dcoc_step[2].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_3 =
            XCVR_RX_DIG_DCOC_TZA_STEP_3_DCOC_TZA_STEP_GAIN_3(tza_dcoc_step[3].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_3_DCOC_TZA_STEP_RCP_3(tza_dcoc_step[3].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_4 =
            XCVR_RX_DIG_DCOC_TZA_STEP_4_DCOC_TZA_STEP_GAIN_4(tza_dcoc_step[4].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_4_DCOC_TZA_STEP_RCP_4(tza_dcoc_step[4].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_5 =
            XCVR_RX_DIG_DCOC_TZA_STEP_5_DCOC_TZA_STEP_GAIN_5(tza_dcoc_step[5].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_5_DCOC_TZA_STEP_RCP_5(tza_dcoc_step[5].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_6 =
            XCVR_RX_DIG_DCOC_TZA_STEP_6_DCOC_TZA_STEP_GAIN_6(tza_dcoc_step[6].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_6_DCOC_TZA_STEP_RCP_6(tza_dcoc_step[6].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_7 =
            XCVR_RX_DIG_DCOC_TZA_STEP_7_DCOC_TZA_STEP_GAIN_7(tza_dcoc_step[7].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_7_DCOC_TZA_STEP_RCP_7(tza_dcoc_step[7].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_8 =
            XCVR_RX_DIG_DCOC_TZA_STEP_8_DCOC_TZA_STEP_GAIN_8(tza_dcoc_step[8].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_8_DCOC_TZA_STEP_RCP_8(tza_dcoc_step[8].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_9 =
            XCVR_RX_DIG_DCOC_TZA_STEP_9_DCOC_TZA_STEP_GAIN_9(tza_dcoc_step[9].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_9_DCOC_TZA_STEP_RCP_9(tza_dcoc_step[9].dcoc_step_rcp);
        XCVR_RX_DIG->DCOC_TZA_STEP_10 =
            XCVR_RX_DIG_DCOC_TZA_STEP_10_DCOC_TZA_STEP_GAIN_10(tza_dcoc_step[10].dcoc_step) |
            XCVR_RX_DIG_DCOC_TZA_STEP_10_DCOC_TZA_STEP_RCP_10(tza_dcoc_step[10].dcoc_step_rcp);

        status = 1; /* Success */
    }
    else
    {
        DEBUG("!!! XCVR trim failed: bbf_dcoc_step = %"PRIu32"!\n", bbf_dcoc_step);
        status = 0; /* Failure */
    }

    /* Restore Registers */
    XCVR_RX_DIG->DCOC_CTRL_0 = dcoc_ctrl_0_stack; /* Restore DCOC_CTRL_0 state to prior settings */
    XCVR_RX_DIG->DCOC_CTRL_1 = dcoc_ctrl_1_stack; /* Restore DCOC_CTRL_1 state to prior settings */
    XCVR_RX_DIG->RX_DIG_CTRL = rx_dig_ctrl_stack; /* Restore RX_DIG_CTRL state to prior settings */
    XCVR_RX_DIG->DCOC_CAL_GAIN = dcoc_cal_gain_state; /* Restore DCOC_CAL_GAIN state to prior setting */
    XCVR_RX_DIG->AGC_CTRL_1 = agc_ctrl_1_stack; /* Save state of RX_DIG_CTRL for later restore */

    return status;
}

/*! *********************************************************************************
 * DCOC_DAC_INIT_Cal : slope sign seek depending on measure's sign
 ***********************************************************************************/
void DCOC_DAC_INIT_Cal(uint8_t standalone_operation)
{
    int16_t dc_meas_i = 2000, dc_meas_i_p = 2000;
    int16_t dc_meas_q = 2000, dc_meas_q_p = 2000;
    uint8_t curr_tza_dac_i, curr_tza_dac_q;
    uint8_t curr_bba_dac_i, curr_bba_dac_q;
    uint8_t p_tza_dac_i = 0, p_tza_dac_q = 0;
    uint8_t p_bba_dac_i = 0, p_bba_dac_q = 0;
    uint8_t i = 0;
    uint8_t bba_gain = 11;
    bool TZA_I_OK = 0, TZA_Q_OK = 0, BBA_I_OK = 0, BBA_Q_OK = 0;

    uint32_t dcoc_ctrl_0_stack;
    uint32_t dcoc_ctrl_1_stack;
    uint32_t agc_ctrl_1_stack;
    uint32_t rx_dig_ctrl_stack;
    uint32_t dcoc_cal_gain_state;
    uint32_t xcvr_ctrl_stack = 0;

    uint32_t temp;

    /* Save registers */
    dcoc_ctrl_0_stack = XCVR_RX_DIG->DCOC_CTRL_0; /* Save state of DCOC_CTRL_0 for later restore */
    dcoc_ctrl_1_stack = XCVR_RX_DIG->DCOC_CTRL_1; /* Save state of DCOC_CTRL_1 for later restore */
    rx_dig_ctrl_stack =  XCVR_RX_DIG->RX_DIG_CTRL; /* Save state of RX_DIG_CTRL for later restore */
    agc_ctrl_1_stack = XCVR_RX_DIG->AGC_CTRL_1; /* Save state of RX_DIG_CTRL for later restore */
    dcoc_cal_gain_state = XCVR_RX_DIG->DCOC_CAL_GAIN; /* Save state of DCOC_CAL_GAIN for later restore */

    /* WarmUp */
    if (standalone_operation)
    {
        temp = XCVR_MISC->XCVR_CTRL;
        xcvr_ctrl_stack = temp;
        temp &= ~(XCVR_CTRL_XCVR_CTRL_PROTOCOL_MASK);
        temp |= XCVR_CTRL_XCVR_CTRL_PROTOCOL(0);
        XCVR_MISC->XCVR_CTRL = temp;
        XCVR_OverrideChannel(12, 1); /* Calibrate on channel #12, 2.426 GHz in BLE map */
        XCVR_ForceRxWu();
        XcvrCalDelay(2000);
    }

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

    XcvrCalDelay(TsettleCal);

    /* Set default DCOC DAC INIT Value */
    /* LNA and BBA DAC Sweep */
    curr_bba_dac_i = 0x20;
    curr_bba_dac_q = 0x20;
    curr_tza_dac_i = 0x80;
    curr_tza_dac_q = 0x80;

    /* Perform a first DC measurement to ensure that measurement is not clipping */
    XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(curr_bba_dac_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(curr_bba_dac_q) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(curr_tza_dac_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(curr_tza_dac_q);

    do
    {
        bba_gain--;
        /* Set DAC user gain */
        XCVR_RX_DIG->AGC_CTRL_1 = XCVR_RX_DIG_AGC_CTRL_1_USER_LNA_GAIN_EN(1) |
                                  XCVR_RX_DIG_AGC_CTRL_1_LNA_USER_GAIN(0) | /* 2 */
                                  XCVR_RX_DIG_AGC_CTRL_1_USER_BBA_GAIN_EN(1) |
                                  XCVR_RX_DIG_AGC_CTRL_1_BBA_USER_GAIN(bba_gain) ; /* 10 */
        XcvrCalDelay(TsettleCal * 2);
        rx_dc_est_average(&dc_meas_i, &dc_meas_q);
    } while ((ABS(dc_meas_i) > 1900) | (ABS(dc_meas_q) > 1900));

    for (i = 0; i < 0x0F; i++)
    {
        /* I channel :  */
        if (!TZA_I_OK)
        {
            if ((ISIGN(dc_meas_i) != ISIGN(dc_meas_i_p)) && (i > 0))
            {
                if (ABS(dc_meas_i) != MIN(ABS(dc_meas_i), ABS(dc_meas_i_p)))
                {
                    curr_tza_dac_i = p_tza_dac_i;
                }

                TZA_I_OK = 1;
            }
            else
            {
                p_tza_dac_i = curr_tza_dac_i;

                if (ISIGN(dc_meas_i)) /* If positif */
                {
                    curr_tza_dac_i--;
                }
                else
                {
                    curr_tza_dac_i++;
                }
            }
        }
        else /* Sweep BBA I */
        {
            if (!BBA_I_OK)
            {
                if ((ISIGN(dc_meas_i) != ISIGN(dc_meas_i_p)) && (curr_bba_dac_i != 0x20))
                {
                    if (ABS(dc_meas_i) != MIN(ABS(dc_meas_i), ABS(dc_meas_i_p)))
                    {
                        curr_bba_dac_i = p_bba_dac_i;
                    }

                    BBA_I_OK = 1;
                }
                else
                {
                    p_bba_dac_i = curr_bba_dac_i;
                    if (ISIGN(dc_meas_i)) /* If positif */
                    {
                        curr_bba_dac_i--;
                    }
                    else
                    {
                        curr_bba_dac_i++;
                    }
                }
            }
        }

        /* Q channel : */
        if (!TZA_Q_OK)
        {
            if ((ISIGN(dc_meas_q) != ISIGN(dc_meas_q_p)) && (i > 0))
            {
                if (ABS(dc_meas_q) != MIN(ABS(dc_meas_q), ABS(dc_meas_q_p)))
                {
                    curr_tza_dac_q = p_tza_dac_q;
                }
                TZA_Q_OK = 1;
            }
            else
            {
                p_tza_dac_q = curr_tza_dac_q;
                if (ISIGN(dc_meas_q)) /* If positif */
                {
                    curr_tza_dac_q--;
                }
                else
                {
                    curr_tza_dac_q++;
                }
            }
        }
        else  /* Sweep BBA Q */
        {
            if (!BBA_Q_OK)
            {
                if ((ISIGN(dc_meas_q) != ISIGN(dc_meas_q_p)) && (curr_bba_dac_q != 0x20))
                {
                    if (ABS(dc_meas_q) != MIN(ABS(dc_meas_q), ABS(dc_meas_q_p)))
                    {
                        curr_bba_dac_q = p_bba_dac_q;
                    }
                    BBA_Q_OK = 1;
                 }
                 else
                 {
                     p_bba_dac_q = curr_bba_dac_q;
                     if (ISIGN(dc_meas_q)) /* If positif */
                     {
                       curr_bba_dac_q--;
                     }
                     else
                     {
                         curr_bba_dac_q++;
                     }
                 }
            }
        }

        /* DC OK break : */
        if (TZA_I_OK && TZA_Q_OK && BBA_I_OK && BBA_Q_OK)
        {
            break;
        }

        dc_meas_i_p = dc_meas_i; /* Store as previous value */
        dc_meas_q_p = dc_meas_q; /* Store as previous value */
        XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(curr_bba_dac_i) |
                                     XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(curr_bba_dac_q) |
                                     XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(curr_tza_dac_i) |
                                     XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(curr_tza_dac_q);
        XcvrCalDelay(TsettleCal * 2);
        rx_dc_est_average(&dc_meas_i, &dc_meas_q);
    }

    /* Apply optimized DCOC DAC INIT : */
    XCVR_RX_DIG->DCOC_DAC_INIT = XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_I(curr_bba_dac_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_BBA_DCOC_INIT_Q(curr_bba_dac_q) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_I(curr_tza_dac_i) |
                                 XCVR_RX_DIG_DCOC_DAC_INIT_TZA_DCOC_INIT_Q(curr_tza_dac_q);

    /* WarmDown */
    if (standalone_operation)
    {
        XCVR_ForceRxWd(); /* Don't leave the receiver running. */
        XcvrCalDelay(200);
        XCVR_OverrideChannel(0xFF,1); /* Release channel overrides */
        XCVR_MISC->XCVR_CTRL = xcvr_ctrl_stack;
    }

    /* Restore register */
    XCVR_RX_DIG->DCOC_CTRL_0 = dcoc_ctrl_0_stack; /* Restore DCOC_CTRL_0 state to prior settings */
    XCVR_RX_DIG->DCOC_CTRL_1 = dcoc_ctrl_1_stack; /* Restore DCOC_CTRL_1 state to prior settings */
    XCVR_RX_DIG->RX_DIG_CTRL = rx_dig_ctrl_stack; /* Restore RX_DIG_CTRL state to prior settings */
    XCVR_RX_DIG->DCOC_CAL_GAIN = dcoc_cal_gain_state; /* Restore DCOC_CAL_GAIN state to prior setting */
    XCVR_RX_DIG->AGC_CTRL_1 = agc_ctrl_1_stack; /* Save state of RX_DIG_CTRL for later restore */
}
