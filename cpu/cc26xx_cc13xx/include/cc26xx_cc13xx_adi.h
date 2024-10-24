/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx MCU I/O register definitions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26XX_CC13XX_ADI_H
#define CC26XX_CC13XX_ADI_H

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ADI_3_REFSYS registers
 */
typedef struct {
    reg8_t __reserved1; /**< Reserved */
    reg8_t ATESTCTL1; /**< Internal */
    reg8_t REFSYSCTL0; /**< Internal */
    reg8_t REFSYSCTL1; /**< Internal */
    reg8_t REFSYSCTL2; /**< Internal */
    reg8_t REFSYSCTL3; /**< Internal */
    reg8_t DCDCCTL0; /**< DCDC Control 0 */
    reg8_t DCDCCTL1; /**< DCDC Control 1 */
    reg8_t DCDCCTL2; /**< DCDC Control 2 */
    reg8_t DCDCCTL3; /**< Internal */
    reg8_t DCDCCTL4; /**< Internal */
    reg8_t DCDCCTL5; /**< Internal */
#ifdef CPU_VARIANT_X2
    reg8_t AUX_DEBUG; /**< RECHARGE_CONTROL_1 */
    reg8_t CTL_RECHARGE_CMP0; /**< Recharge Comparator Control Byte 0 */
    reg8_t CTL_RECHARGE_CMP1; /**< Recharge Comparator Control Byte 1 */
#endif
} adi_3_refsys_regs_t;

/**
 * @brief   ADI_3_REFSYS registers
 */
typedef struct {
    reg8_m4_t __reserved1; /**< Reserved */
    reg8_m4_t ATESTCTL1; /**< Internal */
    reg8_m4_t REFSYSCTL0; /**< Internal */
    reg8_m4_t REFSYSCTL1; /**< Internal */
    reg8_m4_t REFSYSCTL2; /**< Internal */
    reg8_m4_t REFSYSCTL3; /**< Internal */
    reg8_m4_t DCDCCTL0; /**< DCDC Control 0 */
    reg8_m4_t DCDCCTL1; /**< DCDC Control 1 */
    reg8_m4_t DCDCCTL2; /**< DCDC Control 2 */
    reg8_m4_t DCDCCTL3; /**< Internal */
    reg8_m4_t DCDCCTL4; /**< Internal */
    reg8_m4_t DCDCCTL5; /**< Internal */
#ifdef CPU_VARIANT_X2
    reg8_m4_t AUX_DEBUG; /**< RECHARGE_CONTROL_1 */
    reg8_m4_t CTL_RECHARGE_CMP0; /**< Recharge Comparator Control Byte 0 */
    reg8_m4_t CTL_RECHARGE_CMP1; /**< Recharge Comparator Control Byte 1 */
#endif
} adi_3_refsys_regs_m4_t;

/**
 * @brief   ADI3 register values
 * @{
 */
#define ADI_3_REFSYS_DCDCCTL3_VDDR_BOOST_COMP_BOOST 0x00000002
#define ADI_3_REFSYS_AUX_DEBUG_LPM_BIAS_BACKUP_EN   0x00000040
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   ADI3 base address
 */
#define ADI_3_REFSYS_BASE    (PERIPH_BASE + 0x86200)
/**
 * @brief   ADI3 base address for SET instruction
 */
#define ADI_3_REFSYS_BASE_SET (ADI_3_REFSYS_BASE + ADI_SET)
/**
 * @brief   ADI3 base address for CLR instruction
 */
#define ADI_3_REFSYS_BASE_CLR (ADI_3_REFSYS_BASE + ADI_CLR)
/**
 * @brief   ADI3 base address for 4-bit masked access
 */
#define ADI_3_REFSYS_BASE_M4 (ADI_3_REFSYS_BASE + ADI_MASK4B)
/** @} */

/**
 * @brief   ADI3 register bank
 */
#define ADI3                 ((adi_3_refsys_regs_t *) (ADI_3_REFSYS_BASE))
/**
 * @brief   ADI3 register bank for SET instruction
 */
#define ADI3_SET             ((adi_3_refsys_regs_t *) (ADI_3_REFSYS_BASE_SET))
/**
 * @brief   ADI3 register bank for CLR instruction
 */
#define ADI3_CLR             ((adi_3_refsys_regs_t *) (ADI_3_REFSYS_BASE_CLR))
/**
 * @brief   ADI3 register bank for 4-bit masked access
 */
#define ADI3_M4              ((adi_3_refsys_regs_m4_t *) (ADI_3_REFSYS_BASE_M4))

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CC26XX_CC13XX_ADI_H */
