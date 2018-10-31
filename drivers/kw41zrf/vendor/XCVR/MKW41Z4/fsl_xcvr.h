/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
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
#ifndef _FSL_XCVR_H_
/* clang-format off */
#define _FSL_XCVR_H_
/* clang-format on */

#include "cpu.h"

/*!
 * @addtogroup xcvr
 * @{
 */

/*! @file*/

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* KW4xZ/KW3xZ/KW2xZ Radio type */
#define RADIO_IS_GEN_2P0  (1)

#if (CLOCK_RADIOXTAL == 26000000ul)
#define RF_OSC_26MHZ 1
#endif

#define TBD_ZERO                (0)
#define FSL_XCVR_DRIVER_VERSION (MAKE_VERSION(0, 1, 0))

#define B0(x)   (((uint32_t)(((uint32_t)(x)) << 0)) & 0xFFU)
#define B1(x)   (((uint32_t)(((uint32_t)(x)) << 8)) & 0xFF00U)
#define B2(x)   (((uint32_t)(((uint32_t)(x)) << 16)) & 0xFF0000U)
#define B3(x)   (((uint32_t)(((uint32_t)(x)) << 24)) & 0xFF000000U)

#define USE_DEFAULT_PRE_REF             (0)
#define TRIM_BBA_DCOC_DAC_AT_INIT       (1)
#define PRESLOW_ENA                     (1)

/* GEN3 TSM defines */
#if RADIO_IS_GEN_3P0

/* TSM timings initializations for Gen3 radio */
/* NOTE: These timings are stored in 32MHz or 26MHz "baseline" settings, selected by conditional compile below */
/* The init structures for 32Mhz and 26MHz are made identical to allow the same code in fsl_xcvr.c to apply the */
/* settings for all radio generations. The Gen2 radio init value storage had a different structure so this preserves compatibility */
#if RF_OSC_26MHZ == 1
#define TSM_TIMING00init  (0x6d006f00U) /* (bb_ldo_hf_en) */
#define TSM_TIMING01init  (0x6d006f00U) /* (bb_ldo_adcdac_en) */
#define TSM_TIMING02init  (0x6d00ffffU) /* (bb_ldo_bba_en) */
#define TSM_TIMING03init  (0x6d006f00U) /* (bb_ldo_pd_en) */
#define TSM_TIMING04init  (0x6d006f00U) /* (bb_ldo_fdbk_en) */
#define TSM_TIMING05init  (0x6d006f00U) /* (bb_ldo_vcolo_en) */
#define TSM_TIMING06init  (0x6d006f00U) /* (bb_ldo_vtref_en) */
#define TSM_TIMING07init  (0x05000500U) /* (bb_ldo_fdbk_bleed_en) */
#define TSM_TIMING08init  (0x03000300U) /* (bb_ldo_vcolo_bleed_en) */
#define TSM_TIMING09init  (0x03000300U) /* (bb_ldo_vcolo_fastcharge_en) */
#define TSM_TIMING10init  (0x6d036f03U) /* (bb_xtal_pll_ref_clk_en) */
#define TSM_TIMING11init  (0xffff6f03U) /* (bb_xtal_dac_ref_clk_en) */
#define TSM_TIMING12init  (0x6d03ffffU) /* (rxtx_auxpll_vco_ref_clk_en) */
#define TSM_TIMING13init  (0x18004c00U) /* (sy_vco_autotune_en) */
#define TSM_TIMING14init  (0x6d356863U) /* (sy_pd_cycle_slip_ld_ft_en) */
#define TSM_TIMING15init  (0x6d036f03U) /* (sy_vco_en) */
#define TSM_TIMING16init  (0x6d20ffffU) /* (sy_lo_rx_buf_en) */
#define TSM_TIMING17init  (0xffff6f58U) /* (sy_lo_tx_buf_en) */
#define TSM_TIMING18init  (0x6d056f05U) /* (sy_divn_en) */
#define TSM_TIMING19init  (0x18034c03U) /* (sy_pd_filter_charge_en) */
#define TSM_TIMING20init  (0x6d036f03U) /* (sy_pd_en) */
#define TSM_TIMING21init  (0x6d046f04U) /* (sy_lo_divn_en) */
#define TSM_TIMING22init  (0x6d04ffffU) /* (sy_lo_rx_en) */
#define TSM_TIMING23init  (0xffff6f04U) /* (sy_lo_tx_en) */
#define TSM_TIMING24init  (0x18004c00U) /* (sy_divn_cal_en) */
#define TSM_TIMING25init  (0x6d21ffffU) /* (rx_lna_mixer_en) */
#define TSM_TIMING26init  (0xffff6e58U) /* (tx_pa_en) */
#define TSM_TIMING27init  (0x6d24ffffU) /* (rx_adc_i_q_en) */
#define TSM_TIMING28init  (0x2524ffffU) /* (rx_adc_reset_en) */
#define TSM_TIMING29init  (0x6d22ffffU) /* (rx_bba_i_q_en) */
#define TSM_TIMING30init  (0x6d24ffffU) /* (rx_bba_pdet_en) */
#define TSM_TIMING31init  (0x6d23ffffU) /* (rx_bba_tza_dcoc_en) */
#define TSM_TIMING32init  (0x6d21ffffU) /* (rx_tza_i_q_en) */
#define TSM_TIMING33init  (0x6d24ffffU) /* (rx_tza_pdet_en) */
#define TSM_TIMING34init  (0x6d076f07U) /* (pll_dig_en) */
#define TSM_TIMING35init  (0xffff6f5fU) /* (tx_dig_en) */
#define TSM_TIMING36init  (0x6d6affffU) /* (rx_dig_en) */
#define TSM_TIMING37init  (0x6b6affffU) /* (rx_init) */
#define TSM_TIMING38init  (0x6d0e6f42U) /* (sigma_delta_en) */
#define TSM_TIMING39init  (0x6d6affffU) /* (rx_phy_en) */
#define TSM_TIMING40init  (0x6d2affffU) /* (dcoc_en) */
#define TSM_TIMING41init  (0x2b2affffU) /* (dcoc_init) */
#define TSM_TIMING42init  (0xffffffffU) /* (sar_adc_trig_en) */
#define TSM_TIMING43init  (0xffffffffU) /* (tsm_spare0_en) */
#define TSM_TIMING44init  (0xffffffffU) /* (tsm_spare1_en) */
#define TSM_TIMING45init  (0xffffffffU) /* (tsm_spare2_en) */
#define TSM_TIMING46init  (0xffffffffU) /* (tsm_spare3_en) */
#define TSM_TIMING47init  (0xffffffffU) /* (gpio0_trig_en) */
#define TSM_TIMING48init  (0xffffffffU) /* (gpio1_trig_en) */
#define TSM_TIMING49init  (0xffffffffU) /* (gpio2_trig_en) */
#define TSM_TIMING50init  (0xffffffffU) /* (gpio3_trig_en) */
#define TSM_TIMING51init  (0x6d03ffffU) /* (rxtx_auxpll_bias_en) */
#define TSM_TIMING52init  (0x1b06ffffU) /* (rxtx_auxpll_fcal_en) */
#define TSM_TIMING53init  (0x6d03ffffU) /* (rxtx_auxpll_lf_pd_en) */
#define TSM_TIMING54init  (0x1b03ffffU) /* (rxtx_auxpll_pd_lf_filter_charge_en) */
#define TSM_TIMING55init  (0x6d24ffffU) /* (rxtx_auxpll_adc_buf_en) */
#define TSM_TIMING56init  (0x6d24ffffU) /* (rxtx_auxpll_dig_buf_en) */
#define TSM_TIMING57init  (0x1a03ffffU) /* (rxtx_rccal_en) */
#define TSM_TIMING58init  (0xffff6f03U) /* (tx_hpm_dac_en) */
#define END_OF_SEQinit    (0x6d6c6f67U) /*  */
#define TX_RX_ON_DELinit  (0x00008a86U) /*  */
#define TX_RX_SYNTH_init  (0x00002318U) /*  */
#else
#define TSM_TIMING00init  (0x69006f00U) /* (bb_ldo_hf_en) */
#define TSM_TIMING01init  (0x69006f00U) /* (bb_ldo_adcdac_en) */
#define TSM_TIMING02init  (0x6900ffffU) /* (bb_ldo_bba_en) */
#define TSM_TIMING03init  (0x69006f00U) /* (bb_ldo_pd_en) */
#define TSM_TIMING04init  (0x69006f00U) /* (bb_ldo_fdbk_en) */
#define TSM_TIMING05init  (0x69006f00U) /* (bb_ldo_vcolo_en) */
#define TSM_TIMING06init  (0x69006f00U) /* (bb_ldo_vtref_en) */
#define TSM_TIMING07init  (0x05000500U) /* (bb_ldo_fdbk_bleed_en) */
#define TSM_TIMING08init  (0x03000300U) /* (bb_ldo_vcolo_bleed_en) */
#define TSM_TIMING09init  (0x03000300U) /* (bb_ldo_vcolo_fastcharge_en) */
#define TSM_TIMING10init  (0x69036f03U) /* (bb_xtal_pll_ref_clk_en) */
#define TSM_TIMING11init  (0xffff6f03U) /* (bb_xtal_dac_ref_clk_en) */
#define TSM_TIMING12init  (0x6903ffffU) /* (rxtx_auxpll_vco_ref_clk_en) */
#define TSM_TIMING13init  (0x18004c00U) /* (sy_vco_autotune_en) */
#define TSM_TIMING14init  (0x69316863U) /* (sy_pd_cycle_slip_ld_ft_en) */
#define TSM_TIMING15init  (0x69036f03U) /* (sy_vco_en) */
#define TSM_TIMING16init  (0x691cffffU) /* (sy_lo_rx_buf_en) */
#define TSM_TIMING17init  (0xffff6f58U) /* (sy_lo_tx_buf_en) */
#define TSM_TIMING18init  (0x69056f05U) /* (sy_divn_en) */
#define TSM_TIMING19init  (0x18034c03U) /* (sy_pd_filter_charge_en) */
#define TSM_TIMING20init  (0x69036f03U) /* (sy_pd_en) */
#define TSM_TIMING21init  (0x69046f04U) /* (sy_lo_divn_en) */
#define TSM_TIMING22init  (0x6904ffffU) /* (sy_lo_rx_en) */
#define TSM_TIMING23init  (0xffff6f04U) /* (sy_lo_tx_en) */
#define TSM_TIMING24init  (0x18004c00U) /* (sy_divn_cal_en) */
#define TSM_TIMING25init  (0x691dffffU) /* (rx_lna_mixer_en) */
#define TSM_TIMING26init  (0xffff6e58U) /* (tx_pa_en) */
#define TSM_TIMING27init  (0x6920ffffU) /* (rx_adc_i_q_en) */
#define TSM_TIMING28init  (0x2120ffffU) /* (rx_adc_reset_en) */
#define TSM_TIMING29init  (0x691effffU) /* (rx_bba_i_q_en) */
#define TSM_TIMING30init  (0x6920ffffU) /* (rx_bba_pdet_en) */
#define TSM_TIMING31init  (0x691fffffU) /* (rx_bba_tza_dcoc_en) */
#define TSM_TIMING32init  (0x691dffffU) /* (rx_tza_i_q_en) */
#define TSM_TIMING33init  (0x6920ffffU) /* (rx_tza_pdet_en) */
#define TSM_TIMING34init  (0x69076f07U) /* (pll_dig_en) */
#define TSM_TIMING35init  (0xffff6f5fU) /* (tx_dig_en) */
#define TSM_TIMING36init  (0x6966ffffU) /* (rx_dig_en) */
#define TSM_TIMING37init  (0x6766ffffU) /* (rx_init) */
#define TSM_TIMING38init  (0x690e6f42U) /* (sigma_delta_en) */
#define TSM_TIMING39init  (0x6966ffffU) /* (rx_phy_en) */
#define TSM_TIMING40init  (0x6926ffffU) /* (dcoc_en) */
#define TSM_TIMING41init  (0x2726ffffU) /* (dcoc_init) */
#define TSM_TIMING42init  (0xffffffffU) /* (sar_adc_trig_en) */
#define TSM_TIMING43init  (0xffffffffU) /* (tsm_spare0_en) */
#define TSM_TIMING44init  (0xffffffffU) /* (tsm_spare1_en) */
#define TSM_TIMING45init  (0xffffffffU) /* (tsm_spare2_en) */
#define TSM_TIMING46init  (0xffffffffU) /* (tsm_spare3_en) */
#define TSM_TIMING47init  (0xffffffffU) /* (gpio0_trig_en) */
#define TSM_TIMING48init  (0xffffffffU) /* (gpio1_trig_en) */
#define TSM_TIMING49init  (0xffffffffU) /* (gpio2_trig_en) */
#define TSM_TIMING50init  (0xffffffffU) /* (gpio3_trig_en) */
#define TSM_TIMING51init  (0x6903ffffU) /* (rxtx_auxpll_bias_en) */
#define TSM_TIMING52init  (0x1706ffffU) /* (rxtx_auxpll_fcal_en) */
#define TSM_TIMING53init  (0x6903ffffU) /* (rxtx_auxpll_lf_pd_en) */
#define TSM_TIMING54init  (0x1703ffffU) /* (rxtx_auxpll_pd_lf_filter_charge_en) */
#define TSM_TIMING55init  (0x6920ffffU) /* (rxtx_auxpll_adc_buf_en) */
#define TSM_TIMING56init  (0x6920ffffU) /* (rxtx_auxpll_dig_buf_en) */
#define TSM_TIMING57init  (0x1a03ffffU) /* (rxtx_rccal_en) */
#define TSM_TIMING58init  (0xffff6f03U) /* (tx_hpm_dac_en) */
#define END_OF_SEQinit    (0x69686f67U) /*  */
#define TX_RX_ON_DELinit  (0x00008a86U) /*  */
#define TX_RX_SYNTH_init  (0x00002318U) /*  */
#endif /* RF_OSC_26MHZ == 1 */

#define AUX_PLL_DELAY       (0)
/* TSM bitfield shift and value definitions */
#define TX_DIG_EN_ASSERT    (95) /* Assertion time for TX_DIG_EN, used in mode specific settings */
#define ZGBE_TX_DIG_EN_ASSERT (TX_DIG_EN_ASSERT - 1) /* Zigbee TX_DIG_EN must assert 1 tick sooner, see adjustment below based on data padding */
/* EDIT THIS LINE TO CONTROL PA_RAMP! */
#define PA_RAMP_TIME        (2) /* Only allowable values are [0, 1, 2, or 4] in Gen3 */
#define PA_RAMP_SEL_0US     (0)
#define PA_RAMP_SEL_1US     (1)
#define PA_RAMP_SEL_2US     (2)
#define PA_RAMP_SEL_4US     (3)
#if !((PA_RAMP_TIME == 0) || (PA_RAMP_TIME == 1) || (PA_RAMP_TIME == 2) || (PA_RAMP_TIME == 4))
#error "Invalid value for PA_RAMP_TIME macro"
#endif /* Error check of PA RAMP TIME */

#define ADD_FOR_26MHZ           (4)
#define END_OF_TX_WU_NORAMP     (103) /* NOTE: NORAMP and 2us ramp time behaviors are identical for TX WU and WD */
#define END_OF_TX_WD_NORAMP     (111) /* NOTE: NORAMP and 2us ramp time behaviors are identical for TX WU and WD */
/* Redefine the values of END_OF_TX_WU and END_OF_TX_WD based on whether DATA PADDING is enabled and the selection of ramp time */
/* These two constants are then used on both common configuration and mode specific configuration files to define the TSM timing values */
#if ((PA_RAMP_TIME == 0) || (PA_RAMP_TIME == 1) || (PA_RAMP_TIME == 2))
    #define END_OF_TX_WU   (END_OF_TX_WU_NORAMP)
    #define END_OF_TX_WD   (END_OF_TX_WD_NORAMP)
    #if (PA_RAMP_TIME == 0)
        #define PA_RAMP_SEL PA_RAMP_SEL_0US
        #define DATA_PADDING_EN (0)
    #else
        #define DATA_PADDING_EN (1)
        #if (PA_RAMP_TIME == 1)
            #define PA_RAMP_SEL PA_RAMP_SEL_1US
        #else
            #define PA_RAMP_SEL PA_RAMP_SEL_2US
        #endif /* (PA_RAMP_TIME == 1) */
    #endif /* (PA_RAMP_TIME == 0) */
#else /* ((PA_RAMP_TIME == 0) || (PA_RAMP_TIME == 1) || (PA_RAMP_TIME == 2)) */
    #if (PA_RAMP_TIME == 4)
        #define END_OF_TX_WU    (END_OF_TX_WU_NORAMP + 2)
        #define END_OF_TX_WD    (END_OF_TX_WD_NORAMP + 4)
        #define PA_RAMP_SEL PA_RAMP_SEL_4US
        #define DATA_PADDING_EN (1)
    #else /* (PA_RAMP_TIME == 4) */
        #error "Invalid value for PA_RAMP_TIME macro"
    #endif /* (PA_RAMP_TIME == 4) */
#endif/* (PA_RAMP_TIME == 4) */

#define END_OF_RX_WU    (104 + AUX_PLL_DELAY)

#if RF_OSC_26MHZ == 1
#define END_OF_RX_WD    (END_OF_RX_WU + 1 + ADD_FOR_26MHZ) /* Need to handle normal signals extending when 26MHZ warmdown is extended */
#else
#define END_OF_RX_WD    (END_OF_RX_WU + 1)
#endif /* RF_OSC_26MHZ == 1 */

#define END_OF_RX_WU_26MHZ  (END_OF_RX_WU + ADD_FOR_26MHZ)
#define END_OF_RX_WD_26MHZ  (END_OF_RX_WU + 1 + ADD_FOR_26MHZ)

/* PA Bias Table - Gen3 version */
#define PA_RAMP_0 0x1
#define PA_RAMP_1 0x2
#define PA_RAMP_2 0x4
#define PA_RAMP_3 0x6
#define PA_RAMP_4 0x8
#define PA_RAMP_5 0xc
#define PA_RAMP_6 0x10
#define PA_RAMP_7 0x14
#define PA_RAMP_8 0x18
#define PA_RAMP_9 0x1c
#define PA_RAMP_10 0x22
#define PA_RAMP_11 0x28
#define PA_RAMP_12 0x2c
#define PA_RAMP_13 0x30
#define PA_RAMP_14 0x36
#define PA_RAMP_15 0x3c

#else /* Gen2 TSM definitions */
/* GEN2 TSM defines */
#define AUX_PLL_DELAY       (0)
/* TSM bitfield shift and value definitions */
#define TX_DIG_EN_ASSERT    (95) /* Assertion time for TX_DIG_EN, used in mode specific settings */
#define ZGBE_TX_DIG_EN_ASSERT (TX_DIG_EN_ASSERT - 1) /* Zigbee TX_DIG_EN must assert 1 tick sooner, see adjustment below based on data padding */
/* EDIT THIS LINE TO CONTROL PA_RAMP! */
#define PA_RAMP_TIME        (2) /* Only allowable values are [0, 2, 4, or 8] for PA RAMP times in Gen2.0 */
#define PA_RAMP_SEL_0US     (0)
#define PA_RAMP_SEL_2US     (1)
#define PA_RAMP_SEL_4US     (2)
#define PA_RAMP_SEL_8US     (3)

#if !((PA_RAMP_TIME == 0) || (PA_RAMP_TIME == 2) || (PA_RAMP_TIME == 4) || (PA_RAMP_TIME == 8))
#error "Invalid value for PA_RAMP_TIME macro"
#endif /* Error check of PA RAMP TIME */
#define ADD_FOR_26MHZ           (4)
#define END_OF_TX_WU_NORAMP     (103) /* NOTE: NORAMP and 2us ramp time behaviors are identical for TX WU and WD */
#define END_OF_TX_WD_NORAMP     (111) /* NOTE: NORAMP and 2us ramp time behaviors are identical for TX WU and WD */
/* Redefine the values of END_OF_TX_WU and END_OF_TX_WD based on whether DATA PADDING is enabled and the selection of ramp time */
/* These two constants are then used on both common configuration and mode specific configuration files to define the TSM timing values */
#if ((PA_RAMP_TIME == 0) || (PA_RAMP_TIME == 2))
    #define END_OF_TX_WU   (END_OF_TX_WU_NORAMP)
    #define END_OF_TX_WD   (END_OF_TX_WD_NORAMP)
    #define TX_SYNTH_DELAY_ADJ       (0)
    #define PD_CYCLE_SLIP_TX_HI_ADJ  (0)
    #define PD_CYCLE_SLIP_TX_LO_ADJ  (1)
    #define ZGBE_TX_DIG_EN_TX_HI_ADJ (-5) /* Only applies to Zigbee mode */
    #if (PA_RAMP_TIME == 0)
        #define PA_RAMP_SEL PA_RAMP_SEL_0US
        #define DATA_PADDING_EN     (0)
        #define TX_DIG_EN_TX_HI_ADJ (-2)
    #else
        #define DATA_PADDING_EN     (1)
        #define TX_DIG_EN_TX_HI_ADJ (0)
        #define PA_RAMP_SEL PA_RAMP_SEL_2US
    #endif /* (PA_RAMP_TIME == 0) */
#else /* ((PA_RAMP_TIME == 0) || (PA_RAMP_TIME == 2)) */
    #if (PA_RAMP_TIME == 4)
        #define END_OF_TX_WU    (END_OF_TX_WU_NORAMP + 2)
        #define END_OF_TX_WD    (END_OF_TX_WD_NORAMP + 4)
        #define TX_SYNTH_DELAY_ADJ       (2)
        #define PD_CYCLE_SLIP_TX_HI_ADJ  (2)
        #define PD_CYCLE_SLIP_TX_LO_ADJ  (1)
        #define TX_DIG_EN_TX_HI_ADJ      (0)
        #define ZGBE_TX_DIG_EN_TX_HI_ADJ (-3) /* Only applies to Zigbee mode */
        #define PA_RAMP_SEL PA_RAMP_SEL_4US
        #define DATA_PADDING_EN (1)
    #else /* (PA_RAMP_TIME==4) */
        #if ((PA_RAMP_TIME == 8) && (!RADIO_IS_GEN_3P0))
            #define END_OF_TX_WU    (END_OF_TX_WU_NORAMP + 6)
            #define END_OF_TX_WD    (END_OF_TX_WD_NORAMP + 12)
            #define TX_SYNTH_DELAY_ADJ       (6)
            #define PD_CYCLE_SLIP_TX_HI_ADJ  (6)
            #define PD_CYCLE_SLIP_TX_LO_ADJ  (1)
            #define TX_DIG_EN_TX_HI_ADJ      (4)
            #define ZGBE_TX_DIG_EN_TX_HI_ADJ (1) /* Only applies to Zigbee mode */
            #define PA_RAMP_SEL PA_RAMP_SEL_8US
            #define DATA_PADDING_EN (1)
        #else /* (PA_RAMP_TIME == 8) */
            #error "Invalid value for PA_RAMP_TIME macro"
        #endif /* (PA_RAMP_TIME == 8) */
    #endif/* (PA_RAMP_TIME == 4) */
#endif /* ((PA_RAMP_TIME == 0) || (PA_RAMP_TIME == 2)) */

#define TX_DIG_EN_ASSERT_MSK500    (END_OF_TX_WU - 3)

#define END_OF_RX_WU    (104 + AUX_PLL_DELAY)
#if RF_OSC_26MHZ == 1
#define END_OF_RX_WD    (END_OF_RX_WU + 1 + ADD_FOR_26MHZ)  /* Need to handle normal signals extending when 26MHZ warmdown is extended */
#else
#define END_OF_RX_WD    (END_OF_RX_WU + 1)
#endif /* RF_OSC_26MHZ == 1 */
#define END_OF_RX_WU_26MHZ  (END_OF_RX_WU + ADD_FOR_26MHZ)
#define END_OF_RX_WD_26MHZ  (END_OF_RX_WU + 1 + ADD_FOR_26MHZ)

/* PA Bias Table */
#define PA_RAMP_0 0x1
#define PA_RAMP_1 0x2
#define PA_RAMP_2 0x4
#define PA_RAMP_3 0x8
#define PA_RAMP_4 0xe
#define PA_RAMP_5 0x16
#define PA_RAMP_6 0x22
#define PA_RAMP_7 0x2e

/* BLE LL timing definitions */
#define TX_ON_DELAY     (0x85) /* Adjusted TX_ON_DELAY to make turnaround time 150usec */
#define RX_ON_DELAY     (29 + END_OF_RX_WU+4)
#define RX_ON_DELAY_26MHZ     (29 + END_OF_RX_WU_26MHZ+4)
#define TX_RX_ON_DELAY_VAL  (TX_ON_DELAY << 8 | RX_ON_DELAY)
#define TX_RX_ON_DELAY_VAL_26MHZ  (TX_ON_DELAY << 8 | RX_ON_DELAY_26MHZ)
#define TX_SYNTH_DELAY  (TX_ON_DELAY - END_OF_TX_WU - TX_SYNTH_DELAY_ADJ) /* Adjustment to TX_SYNTH_DELAY due to DATA_PADDING */
#define RX_SYNTH_DELAY  (0x18)
#define TX_RX_SYNTH_DELAY_VAL (TX_SYNTH_DELAY << 8 | RX_SYNTH_DELAY)

/* PHY reference waveform assembly */
#define RW0PS(loc, val)      (((val) & 0x1F) << ((loc) * 5)) /* Ref Word 0 - loc is the phase info symbol number, val is the value of the phase info */
#define RW1PS(loc, val)      (((val) & 0x1F) << (((loc) * 5) - 32)) /* Ref Word 1 - loc is the phase info symbol number, val is the value of the phase info */
#define RW2PS(loc, val)      (((val) & 0x1F) << (((loc) * 5) - 64)) /* Ref Word 2 - loc is the phase info symbol number, val is the value of the phase info */
#endif /* RADIO_IS_GEN_3P0 */

/*! @brief  Error codes for the XCVR driver. */
typedef enum _xcvrStatus
{
    gXcvrSuccess_c = 0,
    gXcvrInvalidParameters_c,
    gXcvrUnsupportedOperation_c,
    gXcvrTrimFailure_c
} xcvrStatus_t;

/*! @brief  Health status returned from PHY upon status check function return. */
typedef enum _healthStatus
{
    NO_ERRORS = 0,
    PLL_CTUNE_FAIL = 1,
    PLL_CYCLE_SLIP_FAIL = 2,
    PLL_FREQ_TARG_FAIL = 4,
    PLL_TSM_ABORT_FAIL = 8,
} healthStatus_t;

/*! @brief  Health status returned from PHY upon status check function return. */
typedef enum _ext_clock_config
{
    EXT_CLK_32_MHZ = 0,
    EXT_CLK_26_MHZ = 1,
} ext_clock_config_t;

/*! @brief  Radio operating mode setting types. */
typedef enum
{
    BLE_MODE = 0,
    ZIGBEE_MODE = 1,
    ANT_MODE = 2,

    /* BT=0.5, h=** */
    GFSK_BT_0p5_h_0p5  = 3, /* < BT=0.5, h=0.5 [BLE at 1MBPS data rate; CS4 at 250KBPS data rate] */
    GFSK_BT_0p5_h_0p32 = 4, /* < BT=0.5, h=0.32*/
    GFSK_BT_0p5_h_0p7  = 5, /* < BT=0.5, h=0.7 [ CS1 at 500KBPS data rate] */
    GFSK_BT_0p5_h_1p0  = 6, /* < BT=0.5, h=1.0 [ CS4 at 250KBPS data rate] */

    /* BT=** h=0.5 */
    GFSK_BT_0p3_h_0p5 = 7, /* < BT=0.3, h=0.5 [ CS2 at 1MBPS data rate] */
    GFSK_BT_0p7_h_0p5 = 8, /* < BT=0.7, h=0.5 */

    MSK = 9,
    NUM_RADIO_MODES = 10,
} radio_mode_t;

/*! @brief  Link layer types. */
typedef enum
{
    BLE_LL = 0, /* Must match bit assignment in RADIO1_IRQ_SEL */
    ZIGBEE_LL = 1, /* Must match bit assignment in RADIO1_IRQ_SEL */
    ANT_LL = 2, /* Must match bit assignment in RADIO1_IRQ_SEL */
    GENFSK_LL = 3, /* Must match bit assignment in RADIO1_IRQ_SEL */
    UNASSIGNED_LL = 4, /* Must match bit assignment in RADIO1_IRQ_SEL */
} link_layer_t;

/*! @brief  Data rate selections. */
typedef enum
{
    DR_1MBPS = 0, /* Must match bit assignment in BITRATE field */
    DR_500KBPS = 1, /* Must match bit assignment in BITRATE field */
    DR_250KBPS = 2, /* Must match bit assignment in BITRATE field */
#if RADIO_IS_GEN_3P0
    DR_2MBPS = 3, /* Must match bit assignment in BITRATE field */
#endif /* RADIO_IS_GEN_3P0 */
    DR_UNASSIGNED = 4, /* Must match bit assignment in BITRATE field */
} data_rate_t;

/*!
 * @brief XCVR RX_DIG channel filter coefficient storage
 * Storage of the coefficients varies from 6 bits to 10 bits so all use int16_t for storage.
 */
typedef struct
{
    uint16_t rx_chf_coef_0; /* < 6 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_1; /* < 6 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_2; /* < 7 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_3; /* < 7 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_4; /* < 7 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_5; /* < 7 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_6; /* < 8 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_7; /* < 8 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_8; /* < 9 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_9; /* < 9 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_10; /* < 10 bit two's complement stored in a uint16_t */
    uint16_t rx_chf_coef_11; /* < 10 bit two's complement stored in a uint16_t */
} xcvr_rx_chf_coeffs_t;

/*!
 * @brief XCVR masked init type for 32 bit registers
 * Initialization uses the mask to clear selected fields of the register and then OR's in the init value. All init values must be in their proper field position.
 */
typedef struct
{
    uint32_t mask;
    uint32_t init;
} xcvr_masked_init_32_t;

/*!
 * @brief XCVR common configure structure
 */
typedef struct
{
    /* XCVR_ANA configs */
    xcvr_masked_init_32_t ana_sy_ctrl1;

    /* XCVR_PLL_DIG configs */
    uint32_t pll_hpm_bump;
    uint32_t pll_mod_ctrl;
    uint32_t pll_chan_map;
    uint32_t pll_lock_detect;
    uint32_t pll_hpm_ctrl;
#if !RADIO_IS_GEN_2P1
    uint32_t pll_hpmcal_ctrl;
#endif /* !RADIO_IS_GEN_2P1 */
    uint32_t pll_hpm_sdm_res;
    uint32_t pll_lpm_ctrl;
    uint32_t pll_lpm_sdm_ctrl1;
    uint32_t pll_delay_match;
    uint32_t pll_ctune_ctrl;

    /* XCVR_RX_DIG configs */
    uint32_t rx_dig_ctrl_init; /* NOTE: Common init, mode init, and datarate init will be OR'd together for RX_DIG_CTRL to form complete register initialization */
    uint32_t dcoc_ctrl_0_init_26mhz; /* NOTE: This will be OR'd with mode specific init for DCOC_CTRL_0 to form complete register initialization */
    uint32_t dcoc_ctrl_0_init_32mhz; /* NOTE: This will be OR'd with mode specific init for DCOC_CTRL_0 to form complete register initialization */
    uint32_t dcoc_ctrl_1_init;
    uint32_t dcoc_cal_gain_init;
    uint32_t dc_resid_ctrl_init; /* NOTE: This will be OR'd with datarate specific init for DCOC_RESID_CTRL to form complete register initialization */
    uint32_t dcoc_cal_rcp_init;
    uint32_t lna_gain_val_3_0;
    uint32_t lna_gain_val_7_4;
    uint32_t lna_gain_val_8;
    uint32_t bba_res_tune_val_7_0;
    uint32_t bba_res_tune_val_10_8;
    uint32_t lna_gain_lin_val_2_0_init;
    uint32_t lna_gain_lin_val_5_3_init;
    uint32_t lna_gain_lin_val_8_6_init;
    uint32_t lna_gain_lin_val_9_init;
    uint32_t bba_res_tune_lin_val_3_0_init;
    uint32_t bba_res_tune_lin_val_7_4_init;
    uint32_t bba_res_tune_lin_val_10_8_init;
    uint32_t dcoc_bba_step_init;
    uint32_t dcoc_tza_step_00_init;
    uint32_t dcoc_tza_step_01_init;
    uint32_t dcoc_tza_step_02_init;
    uint32_t dcoc_tza_step_03_init;
    uint32_t dcoc_tza_step_04_init;
    uint32_t dcoc_tza_step_05_init;
    uint32_t dcoc_tza_step_06_init;
    uint32_t dcoc_tza_step_07_init;
    uint32_t dcoc_tza_step_08_init;
    uint32_t dcoc_tza_step_09_init;
    uint32_t dcoc_tza_step_10_init;
#if (RADIO_IS_GEN_3P0 || RADIO_IS_GEN_2P1)
    uint32_t dcoc_cal_fail_th_init;
    uint32_t dcoc_cal_pass_th_init;
#endif /* (RADIO_IS_GEN_3P0 || RADIO_IS_GEN_2P1) */
    uint32_t agc_ctrl_0_init; /* NOTE: Common init and mode init will be OR'd together for AGC_CTRL_0 to form complete register initialization */
    uint32_t agc_ctrl_1_init_26mhz; /* NOTE: This will be OR'd with datarate specific init to form complete register initialization */
    uint32_t agc_ctrl_1_init_32mhz; /* NOTE: This will be OR'd with datarate specific init to form complete register initialization */
    uint32_t agc_ctrl_3_init;
    /* Other agc config inits are in the modeXdatarate config table */
    uint32_t agc_gain_tbl_03_00_init;
    uint32_t agc_gain_tbl_07_04_init;
    uint32_t agc_gain_tbl_11_08_init;
    uint32_t agc_gain_tbl_15_12_init;
    uint32_t agc_gain_tbl_19_16_init;
    uint32_t agc_gain_tbl_23_20_init;
    uint32_t agc_gain_tbl_26_24_init;
    uint32_t rssi_ctrl_0_init;
#if RADIO_IS_GEN_3P0
    uint32_t rssi_ctrl_1_init;
#endif /* RADIO_IS_GEN_3P0 */
    uint32_t cca_ed_lqi_ctrl_0_init;
    uint32_t cca_ed_lqi_ctrl_1_init;

    /* XCVR_TSM configs */
    uint32_t tsm_ctrl;
    uint32_t tsm_ovrd2_init;
    uint32_t end_of_seq_init_26mhz;
    uint32_t end_of_seq_init_32mhz;
#if !RADIO_IS_GEN_2P1
    uint32_t lpps_ctrl_init;
#endif /* !RADIO_IS_GEN_2P1 */
    uint32_t tsm_fast_ctrl2_init_26mhz;
    uint32_t tsm_fast_ctrl2_init_32mhz;
    uint32_t recycle_count_init_26mhz;
    uint32_t recycle_count_init_32mhz;
    uint32_t pa_ramp_tbl_0_init;
    uint32_t pa_ramp_tbl_1_init;
#if RADIO_IS_GEN_3P0
    uint32_t pa_ramp_tbl_2_init;
    uint32_t pa_ramp_tbl_3_init;
#endif /* RADIO_IS_GEN_3P0 */
    uint32_t tsm_timing_00_init;
    uint32_t tsm_timing_01_init;
    uint32_t tsm_timing_02_init;
    uint32_t tsm_timing_03_init;
    uint32_t tsm_timing_04_init;
    uint32_t tsm_timing_05_init;
    uint32_t tsm_timing_06_init;
    uint32_t tsm_timing_07_init;
    uint32_t tsm_timing_08_init;
    uint32_t tsm_timing_09_init;
    uint32_t tsm_timing_10_init;
    uint32_t tsm_timing_11_init;
    uint32_t tsm_timing_12_init;
    uint32_t tsm_timing_13_init;
    uint32_t tsm_timing_14_init_26mhz; /* tsm_timing_14 has mode specific LSbyte (both LS bytes) */
    uint32_t tsm_timing_14_init_32mhz; /* tsm_timing_14 has mode specific LSbyte (both LS bytes) */
    uint32_t tsm_timing_15_init;
    uint32_t tsm_timing_16_init_26mhz;
    uint32_t tsm_timing_16_init_32mhz;
    uint32_t tsm_timing_17_init;
    uint32_t tsm_timing_18_init;
    uint32_t tsm_timing_19_init;
    uint32_t tsm_timing_20_init;
    uint32_t tsm_timing_21_init;
    uint32_t tsm_timing_22_init;
    uint32_t tsm_timing_23_init;
    uint32_t tsm_timing_24_init;
    uint32_t tsm_timing_25_init_26mhz;
    uint32_t tsm_timing_25_init_32mhz;
    uint32_t tsm_timing_26_init;
    uint32_t tsm_timing_27_init_26mhz;
    uint32_t tsm_timing_27_init_32mhz;
    uint32_t tsm_timing_28_init_26mhz;
    uint32_t tsm_timing_28_init_32mhz;
    uint32_t tsm_timing_29_init_26mhz;
    uint32_t tsm_timing_29_init_32mhz;
    uint32_t tsm_timing_30_init_26mhz;
    uint32_t tsm_timing_30_init_32mhz;
    uint32_t tsm_timing_31_init_26mhz;
    uint32_t tsm_timing_31_init_32mhz;
    uint32_t tsm_timing_32_init_26mhz;
    uint32_t tsm_timing_32_init_32mhz;
    uint32_t tsm_timing_33_init_26mhz;
    uint32_t tsm_timing_33_init_32mhz;
    uint32_t tsm_timing_34_init;
    uint32_t tsm_timing_35_init; /* tsm_timing_35 has a mode specific LSbyte*/
    uint32_t tsm_timing_36_init_26mhz;
    uint32_t tsm_timing_36_init_32mhz;
    uint32_t tsm_timing_37_init_26mhz;
    uint32_t tsm_timing_37_init_32mhz;
    uint32_t tsm_timing_38_init;
    uint32_t tsm_timing_39_init_26mhz;
    uint32_t tsm_timing_39_init_32mhz;
    uint32_t tsm_timing_40_init_26mhz;
    uint32_t tsm_timing_40_init_32mhz;
    uint32_t tsm_timing_41_init_26mhz;
    uint32_t tsm_timing_41_init_32mhz;
    uint32_t tsm_timing_51_init;
    uint32_t tsm_timing_52_init_26mhz;
    uint32_t tsm_timing_52_init_32mhz;
    uint32_t tsm_timing_53_init;
    uint32_t tsm_timing_54_init_26mhz;
    uint32_t tsm_timing_54_init_32mhz;
    uint32_t tsm_timing_55_init_26mhz;
    uint32_t tsm_timing_55_init_32mhz;
    uint32_t tsm_timing_56_init_26mhz;
    uint32_t tsm_timing_56_init_32mhz;
    uint32_t tsm_timing_57_init;
    uint32_t tsm_timing_58_init;

    /* XCVR_TX_DIG configs */
    uint32_t tx_ctrl;
    uint32_t tx_data_padding;
     uint32_t tx_dft_pattern;
#if !RADIO_IS_GEN_2P1
    uint32_t rf_dft_bist_1;
    uint32_t rf_dft_bist_2;
#endif /* !RADIO_IS_GEN_2P1 */
} xcvr_common_config_t;

/*! @brief XCVR mode specific configure structure (varies by radio mode) */
typedef struct
{
    radio_mode_t radio_mode;
    uint32_t scgc5_clock_ena_bits;

    /* XCVR_MISC configs */
    xcvr_masked_init_32_t xcvr_ctrl;

    /* XCVR_PHY configs */
#if RADIO_IS_GEN_3P0
    uint32_t phy_fsk_pd_cfg0;
    uint32_t phy_fsk_pd_cfg1;
    uint32_t phy_fsk_cfg;
    uint32_t phy_fsk_misc;
    uint32_t phy_fad_ctrl;
#else
    uint32_t phy_pre_ref0_init;
    uint32_t phy_pre_ref1_init;
    uint32_t phy_pre_ref2_init;
    uint32_t phy_cfg1_init;
    uint32_t phy_el_cfg_init; /* EL_WIN_SIZE and EL_INTERVAL are in the data_rate specific configuration */
#endif /* RADIO_IS_GEN_3P0 */

    /* XCVR_RX_DIG configs */
    uint32_t rx_dig_ctrl_init_26mhz; /* NOTE: Common init, mode init, and datarate init will be OR'd together for RX_DIG_CTRL to form complete register initialization */
    uint32_t rx_dig_ctrl_init_32mhz; /* NOTE: Common init, mode init, and datarate init will be OR'd together for RX_DIG_CTRL to form complete register initialization */
    uint32_t agc_ctrl_0_init; /* NOTE:  Common init and mode init will be OR'd together for AGC_CTRL_0 to form complete register initialization */

    /* XCVR_TSM configs */
#if (RADIO_IS_GEN_2P0 || RADIO_IS_GEN_2P1)
    uint32_t tsm_timing_35_init; /* Only the LSbyte is mode specific */
#endif /* (RADIO_IS_GEN_2P0 || RADIO_IS_GEN_2P1) */

    /* XCVR_TX_DIG configs */
    uint32_t tx_gfsk_ctrl;
    uint32_t tx_gfsk_coeff1_26mhz;
    uint32_t tx_gfsk_coeff2_26mhz;
    uint32_t tx_gfsk_coeff1_32mhz;
    uint32_t tx_gfsk_coeff2_32mhz;
} xcvr_mode_config_t;

/*!
 * @brief XCVR modeXdatarate specific configure structure (varies by radio mode AND data rate)
 * This structure is used to store all of the XCVR settings which are dependent upon both radio mode and data rate. It is used as an overlay
 * on top of the xcvr_mode_config_t structure to supply definitions which are either not in that table or which must be overridden for data rate.
 */
typedef struct
{
    radio_mode_t radio_mode;
    data_rate_t data_rate;

    /* XCVR_ANA configs */
    xcvr_masked_init_32_t ana_sy_ctrl2;
    xcvr_masked_init_32_t ana_rx_bba;
    xcvr_masked_init_32_t ana_rx_tza;

    /* XCVR_PHY configs */
#if RADIO_IS_GEN_3P0
    uint32_t phy_fsk_misc_mode_datarate;
#else
    uint32_t phy_cfg2_init;
#endif /* RADIO_IS_GEN_3P0 */

    uint32_t agc_ctrl_2_init_26mhz;
    uint32_t agc_ctrl_2_init_32mhz;
    xcvr_rx_chf_coeffs_t rx_chf_coeffs_26mhz; /* 26MHz ext clk */
    xcvr_rx_chf_coeffs_t rx_chf_coeffs_32mhz; /* 32MHz ext clk */
    uint32_t rx_rccal_ctrl_0;
    uint32_t rx_rccal_ctrl_1;

    /* XCVR_TX_DIG configs */
    uint32_t tx_fsk_scale_26mhz; /* Only used by MSK mode, but dependent on datarate */
    uint32_t tx_fsk_scale_32mhz; /* Only used by MSK mode, but dependent on datarate */
} xcvr_mode_datarate_config_t;

/*!
 * @brief XCVR datarate specific configure structure (varies by data rate)
 * This structure is used to store all of the XCVR settings which are dependent upon data rate. It is used as an overlay
 * on top of the xcvr_mode_config_t structure to supply definitions which are either not in that table or which must be overridden for data rate.
 */
typedef struct
{
    data_rate_t data_rate;

    /* XCVR_PHY configs */
    uint32_t phy_el_cfg_init; /* Note: EL_ENABLE  is set in xcvr_mode_config_t settings */

    /* XCVR_RX_DIG configs */
    uint32_t rx_dig_ctrl_init_26mhz; /* NOTE: Common init, mode init, and datarate init will be OR'd together for RX_DIG_CTRL to form complete register initialization */
    uint32_t rx_dig_ctrl_init_32mhz; /* NOTE: Common init, mode init, and datarate init will be OR'd together for RX_DIG_CTRL to form complete register initialization */
    uint32_t agc_ctrl_1_init_26mhz;
    uint32_t agc_ctrl_1_init_32mhz;
    uint32_t dcoc_ctrl_0_init_26mhz; /* NOTE: This will be OR'd with common init for DCOC_CTRL_0 to form complete register initialization */
    uint32_t dcoc_ctrl_0_init_32mhz; /* NOTE: This will be OR'd with common init for DCOC_CTRL_0 to form complete register initialization */
    uint32_t dcoc_ctrl_1_init_26mhz; /* NOTE: This will be OR'd with common init for DCOC_CTRL_1 to form complete register initialization */
    uint32_t dcoc_ctrl_1_init_32mhz; /* NOTE: This will be OR'd with common init for DCOC_CTRL_1 to form complete register initialization */
    uint32_t dcoc_ctrl_2_init_26mhz;
    uint32_t dcoc_ctrl_2_init_32mhz;
    uint32_t dcoc_cal_iir_init_26mhz;
    uint32_t dcoc_cal_iir_init_32mhz;
    uint32_t dc_resid_ctrl_26mhz;/* NOTE: This will be OR'd with common init for DCOC_RESID_CTRL to form complete register initialization */
    uint32_t dc_resid_ctrl_32mhz;/* NOTE: This will be OR'd with common init for DCOC_RESID_CTRL to form complete register initialization */
} xcvr_datarate_config_t;

/*!
 * @brief LPUART callback function type
 *
 * The panic callback function is defined by system if system need to be informed of XCVR fatal errors.
 * refer to #XCVR_RegisterPanicCb
 */
typedef void (*panic_fptr)(uint32_t panic_id, uint32_t location, uint32_t extra1, uint32_t extra2);

/* Make available const structures from config files */
extern const xcvr_common_config_t xcvr_common_config;
extern const xcvr_mode_config_t zgbe_mode_config;
extern const xcvr_mode_config_t ble_mode_config;
extern const xcvr_mode_config_t ant_mode_config;
extern const xcvr_mode_config_t gfsk_bt_0p5_h_0p5_mode_config;
extern const xcvr_mode_config_t gfsk_bt_0p5_h_0p7_mode_config;
extern const xcvr_mode_config_t gfsk_bt_0p5_h_0p32_mode_config;
extern const xcvr_mode_config_t gfsk_bt_0p5_h_1p0_mode_config;
extern const xcvr_mode_config_t gfsk_bt_0p3_h_0p5_mode_config;
extern const xcvr_mode_config_t gfsk_bt_0p7_h_0p5_mode_config;
extern const xcvr_mode_config_t msk_mode_config;

#if RADIO_IS_GEN_3P0
extern const xcvr_datarate_config_t xcvr_2mbps_config;
#endif /* RADIO_IS_GEN_3P0 */
extern const xcvr_datarate_config_t xcvr_1mbps_config;
extern const xcvr_datarate_config_t xcvr_500kbps_config;
extern const xcvr_datarate_config_t xcvr_250kbps_config;
extern const xcvr_datarate_config_t xcvr_802_15_4_500kbps_config; /* Custom datarate settings for 802.15.4 since it is 2MChips/sec */

#if RADIO_IS_GEN_3P0
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p5_2mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p32_2mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p3_h_0p5_2mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p7_h_0p5_2mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_MSK_2mbps_config;
#endif /* RADIO_IS_GEN_3P0 */
extern const xcvr_mode_datarate_config_t xcvr_BLE_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_ZIGBEE_500kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_ANT_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p5_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p5_500kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p5_250kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p32_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p32_500kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p32_250kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p7_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p7_500kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_0p7_250kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_1p0_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_1p0_500kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p5_h_1p0_250kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p3_h_0p5_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p3_h_0p5_500kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p3_h_0p5_250kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p7_h_0p5_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p7_h_0p5_500kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_GFSK_BT_0p7_h_0p5_250kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_MSK_1mbps_config;
extern const xcvr_mode_datarate_config_t xcvr_MSK_500kbps_config;
extern const xcvr_mode_datarate_config_t xcvr_MSK_250kbps_config;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/*! @}*/

#endif /* _FSL_XCVR_H_ */
