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
 * @brief           Register values to use a USIC channel as I2C master
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#ifndef USIC_I2C_H_
#define USIC_I2C_H_

#include "periph/usic.h"

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
 *        channel as an I2C master.
 * @{
 */
static const usic_mode_t _xmc_usic_i2c_master_mode = {

    .ccr =
        /* mode: IIC (I2C) */
        (4 << USIC_CH_CCR_MODE_Pos),

    .sctr =
        /* shift direction: MSB first */
        ((1 << USIC_CH_SCTR_SDIR_Pos) |
        /* data shift mode: shift one bit at a time */
         (1 << USIC_CH_SCTR_DSM_Pos) |
        /* transmission mode: considered active
           without referring to the actual signal level. */
         (3 << USIC_CH_SCTR_TRM_Pos) |
        /* word length: 8 bits */
         (7 << USIC_CH_SCTR_WLE_Pos) |
        /* frame length: explicit end-of-frame control */
         (63 << USIC_CH_SCTR_FLE_Pos) |
        /* output level at the MOSI when no data avalaible: high */
         (0 << USIC_CH_SCTR_PDL_Pos)),

    .tcsr =
        /* TBUF data enable: transmit if TDV = 1 */
        (1 <<  USIC_CH_TCSR_TDEN_Pos) |
        /* TBUF data single shot mode: invalidate data after load */
        (1 <<  USIC_CH_TCSR_TDSSM_Pos),

    .pcr =
        /* slave address: none */
        (0 << USIC_CH_PCR_IICMode_SLAD_Pos) |
        /* acknowledge 0x00: no */
        (0 << USIC_CH_PCR_IICMode_ACK00_Pos) |
        /* symbol timing: 10 time quanta (100 kBaud) */
        (0 << USIC_CH_PCR_IICMode_STIM_Pos) |
        /* start condition received interrupt: disabled */
        (0 << USIC_CH_PCR_IICMode_SCRIEN_Pos) |
        /* repeated start condition received interrupt: disabled */
        (0 << USIC_CH_PCR_IICMode_RSCRIEN_Pos) |
        /* stop condition receive interrupt: disabled */
        (0 << USIC_CH_PCR_IICMode_PCRIEN_Pos) |
        /* nack interrupt: disabled */
        (0 << USIC_CH_PCR_IICMode_NACKIEN_Pos) |
        /* arbitration lost interrupt: disabled  */
        (0 << USIC_CH_PCR_IICMode_ARLIEN_Pos)  |
        /* slave read request interrupt: disabled */
        (0 << USIC_CH_PCR_IICMode_SRRIEN_Pos) |
        /* error interrupt: disabled  */
        (0 << USIC_CH_PCR_IICMode_ERRIEN_Pos) |
        /* slave acknowledge disable: .. */
        (0 << USIC_CH_PCR_IICMode_SACKDIS_Pos) |
        /* hardware delay: none */
        (0 << USIC_CH_PCR_IICMode_HDEL_Pos) |
        /* acknowledge interrupt: disabled */
        (0 << USIC_CH_PCR_IICMode_ACKIEN_Pos) |
        /* master clock output: disabled */
        (0 << USIC_CH_PCR_IICMode_MCLK_Pos),

    .inpr = 0
};
/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* USIC_I2C_H_ */
