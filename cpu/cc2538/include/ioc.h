/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief           CC2538 I/O Control driver
 *
 * @author          Ian Martin <ian@locicontrols.com>
 *
 * @addtogroup cpu_cc2538
 * @{
 *
 * @defgroup cc2538-ioc CC2538 I/O Control
 *
 * Header file with declarations for the I/O Control module
 * @{
 */
#ifndef IOC_H_
#define IOC_H_

#include <stdint.h>

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IOC_PXX_SEL  ( (cc2538_reg_t*)0x400d4000 )
#define IOC_PXX_OVER ( (cc2538_reg_t*)0x400d4080 )

/** @name Peripheral Signal Select Values (for IOC_Pxx_SEL registers)
 * @{
 */
enum {
    UART0_TXD      =  0,                /**< UART0 TXD */
    UART1_RTS      =  1,                /**< UART1 RTS */
    UART1_TXD      =  2,                /**< UART1 TXD */
    SSI0_TXD       =  3,                /**< SSI0 TXD */
    SSI0_CLKOUT    =  4,                /**< SSI0 CLKOUT */
    SSI0_FSSOUT    =  5,                /**< SSI0 FSSOUT */
    SSI0_STXSER_EN =  6,                /**< SSI0 STXSER EN */
    SSI1_TXD       =  7,                /**< SSI1 TXD */
    SSI1_CLKOUT    =  8,                /**< SSI1 CLKOUT */
    SSI1_FSSOUT    =  9,                /**< SSI1 FSSOUT */
    SSI1_STXSER_EN = 10,                /**< SSI1 STXSER EN */
    I2C_CMSSDA     = 11,                /**< I2C CMSSDA */
    I2C_CMSSCL     = 12,                /**< I2C CMSSCL */
    GPT0_ICP1      = 13,                /**< GPT0 ICP1 */
    GPT0_ICP2      = 14,                /**< GPT0 ICP2 */
    GPT1_ICP1      = 15,                /**< GPT1 ICP1 */
    GPT1_ICP2      = 16,                /**< GPT1 ICP2 */
    GPT2_ICP1      = 17,                /**< GPT2 ICP1 */
    GPT2_ICP2      = 18,                /**< GPT2 ICP2 */
    GPT3_ICP1      = 19,                /**< GPT3 ICP1 */
    GPT3_ICP2      = 20,                /**< GPT3 ICP2 */
};
/** @} */

/** @name Values for IOC_PXX_OVER
 * @{
 */
#define IOC_OVERRIDE_OE   0x00000008    /**< Output Enable */
#define IOC_OVERRIDE_PUE  0x00000004    /**< Pull Up Enable */
#define IOC_OVERRIDE_PDE  0x00000002    /**< Pull Down Enable */
#define IOC_OVERRIDE_ANA  0x00000001    /**< Analog Enable */
#define IOC_OVERRIDE_DIS  0x00000000    /**< Override Disabled */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* IOC_H_ */

/** @} */
/** @} */
/** @} */
