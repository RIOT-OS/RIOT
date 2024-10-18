/*
 * Copyright (C) 2014 Loci Controls Inc.
 *               2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc2538_gptimer CC2538 General Purpose Timer
 * @ingroup         cpu_cc2538_regs
 * @{
 *
 * @file
 * @brief           CC2538 General Purpose Timer (GPTIMER) driver
 *
 * @author          Ian Martin <ian@locicontrols.com>
 * @author          Sebastian Meiling <s@mlng.net>
 */

#ifndef CC2538_GPTIMER_H
#define CC2538_GPTIMER_H

#include <stdint.h>

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timer modes
 */
enum {
    GPTIMER_ONE_SHOT_MODE = 1,              /**< GPTIMER one-shot mode */
    GPTIMER_PERIODIC_MODE = 2,              /**< GPTIMER periodic mode */
    GPTIMER_CAPTURE_MODE  = 3,              /**< GPTIMER capture mode */
};

/**
 * @brief Timer width configuration
 */
enum {
    GPTMCFG_32_BIT_TIMER           = 0,     /**< 32-bit timer configuration */
    GPTMCFG_32_BIT_REAL_TIME_CLOCK = 1,     /**< 32-bit real-time clock */
    GPTMCFG_16_BIT_TIMER           = 4,     /**< 16-bit timer configuration */
};

/**
 * @brief GPTIMER component registers
 */
typedef struct {
    cc2538_reg_t CFG;                       /**< GPTIMER Configuration */
    cc2538_reg_t TAMR;                      /**< GPTIMER Timer A mode */
    cc2538_reg_t TBMR;                      /**< GPTIMER Timer B mode */
    cc2538_reg_t CTL;                       /**< GPTIMER Control */
    cc2538_reg_t SYNC;                      /**< GPTIMER Synchronize */
    cc2538_reg_t RESERVED2;                 /**< Reserved word */
    cc2538_reg_t IMR;                       /**< GPTIMER Interrupt Mask */
    cc2538_reg_t RIS;                       /**< GPTIMER Raw Interrupt Status */
    cc2538_reg_t MIS;                       /**< GPTIMER Masked Interrupt Status */
    cc2538_reg_t ICR;                       /**< GPTIMER Interrupt Clear */
    cc2538_reg_t TAILR;                     /**< GPTIMER Timer A Interval Load */
    cc2538_reg_t TBILR;                     /**< GPTIMER Timer B Interval Load */
    cc2538_reg_t TAMATCHR;                  /**< GPTIMER Timer A Match */
    cc2538_reg_t TBMATCHR;                  /**< GPTIMER Timer B Match */
    cc2538_reg_t TAPR;                      /**< GPTIMER Timer A Prescale Register */
    cc2538_reg_t TBPR;                      /**< GPTIMER Timer B Prescale Register */
    cc2538_reg_t TAPMR;                     /**< GPTIMER Timer A Prescale Match Register */
    cc2538_reg_t TBPMR;                     /**< GPTIMER Timer B Prescale Match Register */
    cc2538_reg_t TAR;                       /**< GPTIMER Timer A */
    cc2538_reg_t TBR;                       /**< GPTIMER Timer B */
    cc2538_reg_t TAV;                       /**< GPTIMER Timer A Value */
    cc2538_reg_t TBV;                       /**< GPTIMER Timer B Value */
    cc2538_reg_t RESERVED3;                 /**< Reserved word */
    cc2538_reg_t TAPS;                      /**< GPTIMER Timer A Prescale Snapshot */
    cc2538_reg_t TBPS;                      /**< GPTIMER Timer B Prescale Snapshot */
    cc2538_reg_t TAPV;                      /**< GPTIMER Timer A Prescale Value */
    cc2538_reg_t TBPV;                      /**< GPTIMER Timer B Prescale Value */
    cc2538_reg_t RESERVED[981];             /**< Reserved */
    cc2538_reg_t PP;                        /**< GPTIMER Peripheral Properties */
    cc2538_reg_t RESERVED4[15];             /**< Reserved */
} cc2538_gptimer_t;

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
#endif /* CC2538_GPTIMER_H */
