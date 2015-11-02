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
 * @brief           Register values to use a USIC channel as UART
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#ifndef USIC_ASC_H_
#define USIC_ASC_H_

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
 *        channel as UART.
 * @{
 */
static const usic_controls_t _xmc_usic_asc_controls = {

    .ccr =
        /* mode: ASC (UART) */
        (2 << USIC_CH_CCR_MODE_Pos) |
        /* receive interrupt: enabled */
        (1 << USIC_CH_CCR_RIEN_Pos),

    .sctr =
        /* transmission mode: active at 1-level */
        (1 << USIC_CH_SCTR_TRM_Pos) |
        /* frame length: 8 bits */
        (7 << USIC_CH_SCTR_FLE_Pos) |
        /* word length: 8 bits */
        (7 << USIC_CH_SCTR_WLE_Pos) |
        /* output level at the TX when no data available: high */
        (1 << USIC_CH_SCTR_PDL_Pos),

    .tcsr =
        /* TBUF data single shot mode: invalidate data after load */
        (1 << USIC_CH_TCSR_TDSSM_Pos) |
        /* TBUF data enable: transmit if TDV = 1 */
        (1 << USIC_CH_TCSR_TDEN_Pos),

    .pcr =
        /* sample mode: three samples per bit time & majority decision */
        (1 << USIC_CH_PCR_ASCMode_SMD_Pos) |
        /* sample point: 4 <= brg.dctq */
        (4 << USIC_CH_PCR_ASCMode_SP_Pos) |
        /* stop bits: 1 */
        (0 << USIC_CH_PCR_ASCMode_STPB_Pos),

    .dx2_dsel = 0,

    .inpr =
        /* set receive interrupt pointer to SR5 */
        5 << USIC_CH_INPR_RINP_Pos

};
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* USIC_ASC_H_ */
