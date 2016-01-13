/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc2538
 * @{
 *
 * @file
 * @brief           CC2538 SOC ADC interface
 *
 * @author          Ian Martin <ian@locicontrols.com>
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
    union {
        cc2538_reg_t ADCCON1;            /**< ADC Control Register 1 */
        struct {
            cc2538_reg_t RESERVED2 :  2; /**< Reserved bits */
            cc2538_reg_t RCTRL     :  2; /**< Random number generator control */
            cc2538_reg_t STSEL     :  2; /**< Start select */
            cc2538_reg_t ST        :  1; /**< Start conversion */
            cc2538_reg_t EOC       :  1; /**< End of conversion */
            cc2538_reg_t RESERVED1 : 24; /**< Reserved bits */
        } ADCCON1bits;
    } cc2538_adc_adccon1;

    cc2538_reg_t ADCCON2;                /**< ADC Control Register 2 */
    cc2538_reg_t ADCCON3;                /**< ADC Control Register 3 */
    cc2538_reg_t ADCL;                   /**< Least-significant part of ADC conversion result. */
    cc2538_reg_t ADCH;                   /**< Most-significant part of ADC conversion result. */
    cc2538_reg_t RNDL;                   /**< Random-number-generator data; low byte. */
    cc2538_reg_t RNDH;                   /**< Random-number-generator data; high byte. */
    cc2538_reg_t RESERVED[2];            /**< Reserved bytes */
    cc2538_reg_t CMPCTL;                 /**< Analog comparator control and status register. */
} cc2538_soc_adc_t;

#define SOC_ADC ( (cc2538_soc_adc_t*)0x400d7000 ) /**< One and only instance of the SOC ADC component */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC2538_SOC_ADC_H */

/** @} */
