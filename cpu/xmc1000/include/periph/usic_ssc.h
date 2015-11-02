/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_xmc1000
 * @{
 *
 * @file
 * @brief           Register values to use a USIC channel as SPI master
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#ifndef USIC_SSC_H_
#define USIC_SSC_H_

#include "periph/usic_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup cpu_xmc1000_usic USIC configuration
 * @ingroup cpu_xmc1000
 * @{
 */

/**
 * @brief Set of configuration values suitable for driving a USIC
 *        channel as SPI master.
 * @{
 */
static const usic_controls_t _xmc_usic_ssc_master_controls = {

    .ccr =
        /* mode: SSC (SPI) */
        (1 << USIC_CH_CCR_MODE_Pos),

    .sctr =
        /* shift direction: MSB first */
        ((1 << USIC_CH_SCTR_SDIR_Pos) |
        /* data shift mode: shift one bit at a time */
         (0 << USIC_CH_SCTR_DSM_Pos) |
        /* transmission mode: active at 1-level */
         (1 << USIC_CH_SCTR_TRM_Pos) |
        /* word length: 8 bits */
         (7 << USIC_CH_SCTR_WLE_Pos) |
        /* frame length: explicit end-of-frame control */
         (63 << USIC_CH_SCTR_FLE_Pos) |
        /* output level at the MOSI when no data avalaible: high */
         (1 << USIC_CH_SCTR_PDL_Pos)),

    .tcsr =
        /* WLE mode: enabled */
        (1 <<  USIC_CH_TCSR_WLEMD_Pos) |
        /* TBUF data enable: transmit if TDV = 1 */
        (1 <<  USIC_CH_TCSR_TDEN_Pos) |
        /* TBUF data single shot mode: invalidate data after load */
        (1 <<  USIC_CH_TCSR_TDSSM_Pos),

    .pcr =
        /* length of a time quantum for the generation of the
         * next-frame and inter-word delay: 1 (0 + 1) */
        (0 << USIC_CH_PCR_SSCMode_PCTQ1_Pos) |
        /* number of time quanta for delays: 0 */
        (0 << USIC_CH_PCR_SSCMode_DCTQ1_Pos) |
        /* inter-word delay: disabled */
        (0 << USIC_CH_PCR_SSCMode_TIWEN_Pos) |
        /* frame end mode: current frame considered empty when TBUF unfed */
        (1 << USIC_CH_PCR_SSCMode_FEM_Pos) |
        /* master-slave-select signal generation: enabled */
        (1 << USIC_CH_PCR_SSCMode_MSLSEN_Pos) |
        /* select control: direct select mode */
        (1 << USIC_CH_PCR_SSCMode_SELCTR_Pos) |
        /* select active high: inverted */
        (1 << USIC_CH_PCR_SSCMode_SELINV_Pos) |
        /* select output: the corresponding SELOx line can be activated */
        (1 <<  USIC_CH_PCR_SSCMode_SELO_Pos),

    .dx2_dsel = 7,

    .inpr =
        /* no interrupts for SPI */
        0

};
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* USIC_SSC_H_ */
