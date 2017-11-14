/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc2538
 * @{
 *
 * @file
 * @brief           CC2538 SSI interface
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */

#ifndef CC2538_SSI_H
#define CC2538_SSI_H

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SSI component registers
 */
typedef struct {
    union {
        cc2538_reg_t CR0;                  /**< SSI Control Register 0 */
        struct {
            cc2538_reg_t DSS        :  4;  /**< SSI data size select */
            cc2538_reg_t FRF        :  2;  /**< SSI frame format select */
            cc2538_reg_t SPO        :  1;  /**< SSI serial clock polarity */
            cc2538_reg_t SPH        :  1;  /**< SSI serial clock phase */
            cc2538_reg_t SCR        :  8;  /**< SSI serial clock rate */
            cc2538_reg_t RESERVED   : 16;  /**< Reserved bits */
        } CR0bits;
    };

    union {
        cc2538_reg_t CR1;                  /**< SSI Control Register 1 */
        struct {
            cc2538_reg_t LBM        :  1;  /**< SSI loop-back mode */
            cc2538_reg_t SSE        :  1;  /**< SSI synchronous serial port enable */
            cc2538_reg_t MS         :  1;  /**< SSI master and slave select */
            cc2538_reg_t SOD        :  1;  /**< SSI slave mode output disable */
            cc2538_reg_t RESERVED   : 28;  /**< Reserved bits */
        } CR1bits;
    };

    cc2538_reg_t DR;                       /**< SSI Data register */

    union {
        cc2538_reg_t SR;                   /**< SSI FIFO/busy Status Register */
        struct {
            cc2538_reg_t TFE        :  1;  /**< SSI transmit FIFO empty */
            cc2538_reg_t TNF        :  1;  /**< SSI transmit FIFO not full */
            cc2538_reg_t RNE        :  1;  /**< SSI receive FIFO not empty */
            cc2538_reg_t RFF        :  1;  /**< SSI receive FIFO full */
            cc2538_reg_t BSY        :  1;  /**< SSI busy bit */
            cc2538_reg_t RESERVED   : 27;  /**< Reserved bits */
        } SRbits;
    };
    cc2538_reg_t CPSR;                     /**< SSI Clock Register */
    cc2538_reg_t IM;                       /**< SSI Interrupt Mask register */
    cc2538_reg_t RIS;                      /**< SSI Raw Interrupt Status register */
    cc2538_reg_t MIS;                      /**< SSI Masked Interrupt Status register */
    cc2538_reg_t ICR;                      /**< SSI Interrupt Clear Register */
    cc2538_reg_t DMACTL;                   /**< SSI uDMA Control Register. */
    cc2538_reg_t CC;                       /**< SSI clock configuration */
} cc2538_ssi_t;

#define SSI0 ( (cc2538_ssi_t*)0x40008000 ) /**< SSI0 Instance */
#define SSI1 ( (cc2538_ssi_t*)0x40009000 ) /**< SSI1 Instance */

/**
 * @brief   Define CR0 register bitfields
 * @{
 */
#define SSI_CR0_DSS(x)          ((x - 1) << 0)
#define SSI_CR0_SPO             (1 << 6)
#define SSI_CR0_SPH             (1 << 7)
/** @} */

/**
 * @brief   Define CR1 register bitfields
 * @{
 */
#define SSI_CR1_LBM             (1 << 0)
#define SSI_CR1_SSE             (1 << 1)
#define SSI_CR1_MS              (1 << 2)
#define SSI_CR1_SOD             (1 << 3)
/** @} */

/**
 * @brief   Define SR register bitfields
 * @{
 */
#define SSI_SR_TFE              (1 << 0)
#define SSI_SR_TNF              (1 << 1)
#define SSI_SR_RNE              (1 << 2)
#define SSI_SR_RFF              (1 << 3)
#define SSI_SR_BSY              (1 << 4)
/** @} */

/**
 * @brief   Define CC register bitfields
 * @{
 */
#define SSI_SS_PIOSC            (1 << 0)
#define SSI_SS_DSEN             (1 << 2)
#define SSI_SS_SYSDIV           (0)
#define SSI_SS_IODIV            (SSI_SS_PIOSC)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC2538_SSI_H */
/** @} */
