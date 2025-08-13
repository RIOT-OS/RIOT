/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup      cpu_cc2538
 * @{
 *
 * @file
 * @brief           CC2538 SSI interface
 *
 * @author          Ian Martin <ian@locicontrols.com>
 * @author          Sebastian Meiling <s@mlng.net>
 */

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SSI component registers
 */
typedef struct {
    cc2538_reg_t CR0;       /**< SSI Control Register 0 */
    cc2538_reg_t CR1;       /**< SSI Control Register 1 */
    cc2538_reg_t DR;        /**< SSI Data register */
    cc2538_reg_t SR;        /**< SSI FIFO/busy Status Register */
    cc2538_reg_t CPSR;      /**< SSI Clock Register */
    cc2538_reg_t IM;        /**< SSI Interrupt Mask register */
    cc2538_reg_t RIS;       /**< SSI Raw Interrupt Status register */
    cc2538_reg_t MIS;       /**< SSI Masked Interrupt Status register */
    cc2538_reg_t ICR;       /**< SSI Interrupt Clear Register */
    cc2538_reg_t DMACTL;    /**< SSI uDMA Control Register. */
    cc2538_reg_t CC;        /**< SSI clock configuration */
} cc2538_ssi_t;

/**
 * @brief   Set CR0 data size (bits)
 */
#define SSI_CR0_DSS(x)      (x - 1)

/**
 * @brief   Define CC register bitfields
 * @{
 */
#define SSI_CC_CS_SYSDIV    (0x0)
#define SSI_CC_CS_IODIV     (0x1)
#define SSI_CC_CS_DSEN      (0x4)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
