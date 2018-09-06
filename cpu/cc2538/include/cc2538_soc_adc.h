/*
 * Copyright (C) 2014 Loci Controls Inc.
 *               2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc2538_adc CC2538 ADC
 * @ingroup         cpu_cc2538_regs
 * @{
 *
 * @file
 * @brief           CC2538 SOC ADC interface
 *
 * @author          Ian Martin <ian@locicontrols.com>
 * @author          Sebastian Meiling <s@mlng.net>
 */

#ifndef CC2538_SOC_ADC_H
#define CC2538_SOC_ADC_H

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SOC ADC component registers
 */
typedef struct {
    cc2538_reg_t ADCCON1;       /**< ADC Control Register 1 */
    cc2538_reg_t ADCCON2;       /**< ADC Control Register 2 */
    cc2538_reg_t ADCCON3;       /**< ADC Control Register 3 */
    cc2538_reg_t ADCL;          /**< Least-significant part of ADC conversion result. */
    cc2538_reg_t ADCH;          /**< Most-significant part of ADC conversion result. */
    cc2538_reg_t RNDL;          /**< Random-number-generator data; low byte. */
    cc2538_reg_t RNDH;          /**< Random-number-generator data; high byte. */
    cc2538_reg_t RESERVED[2];   /**< Reserved bytes */
    cc2538_reg_t CMPCTL;        /**< Analog comparator control and status register. */
} cc2538_soc_adc_t;

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC2538_SOC_ADC_H */

/** @} */
