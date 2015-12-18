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
 * @brief           CC2538 General Purpose Timer (GPTIMER) driver
 *
 * @author          Ian Martin <ian@locicontrols.com>
 */

#ifndef GPTIMER_H
#define GPTIMER_H

#include <stdint.h>

#include "cc2538.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPTIMER_NUMOF            4       /**< The CC2538 has four general-purpose timer units. */
#define NUM_CHANNELS_PER_GPTIMER 2       /**< Each G.P. timer unit has two channels: A and B. */

enum {
    GPTIMER_ONE_SHOT_MODE = 1,           /**< GPTIMER one-shot mode */
    GPTIMER_PERIODIC_MODE = 2,           /**< GPTIMER periodic mode */
    GPTIMER_CAPTURE_MODE  = 3,           /**< GPTIMER capture mode */
};

enum {
    GPTMCFG_32_BIT_TIMER           = 0,  /**< 32-bit timer configuration */
    GPTMCFG_32_BIT_REAL_TIME_CLOCK = 1,  /**< 32-bit real-time clock */
    GPTMCFG_16_BIT_TIMER           = 4,  /**< 16-bit timer configuration */
};

/**
 * @brief GPTIMER component registers
 */
typedef struct {
    cc2538_reg_t CFG;                    /**< GPTIMER Configuration */

    union {
        cc2538_reg_t TAMR;               /**< GPTIMER Timer A mode */
        struct {
            cc2538_reg_t TAMR2     :  2; /**< GPTM Timer A mode */
            cc2538_reg_t TACRM     :  1; /**< GPTM Timer A capture mode */
            cc2538_reg_t TAAMS     :  1; /**< GPTM Timer A alternate mode */
            cc2538_reg_t TACDIR    :  1; /**< GPTM Timer A count direction */
            cc2538_reg_t TAMIE     :  1; /**< GPTM Timer A match interrupt enable */
            cc2538_reg_t TAWOT     :  1; /**< GPTM Timer A wait-on-trigger */
            cc2538_reg_t TASNAPS   :  1; /**< GPTM Timer A snap shot mode */
            cc2538_reg_t TAILD     :  1; /**< GPTM Timer A interval load write */
            cc2538_reg_t TAPWMIE   :  1; /**< GPTM Timer A PWM interrupt enable */
            cc2538_reg_t TAMRSU    :  1; /**< Timer A match register update mode */
            cc2538_reg_t TAPLO     :  1; /**< Legacy PWM operation */
            cc2538_reg_t RESERVED5 : 20; /**< Reserved bits */
        } TAMRbits;
    } cc2538_gptimer_tamr;

    union {
        cc2538_reg_t TBMR;               /**< GPTIMER Timer B mode */
        struct {
            cc2538_reg_t TBMR2     :  2; /**< GPTM Timer B mode */
            cc2538_reg_t TBCRM     :  1; /**< GPTM Timer B capture mode */
            cc2538_reg_t TBAMS     :  1; /**< GPTM Timer B alternate mode */
            cc2538_reg_t TBCDIR    :  1; /**< GPTM Timer B count direction */
            cc2538_reg_t TBMIE     :  1; /**< GPTM Timer B match interrupt enable */
            cc2538_reg_t TBWOT     :  1; /**< GPTM Timer B wait-on-trigger */
            cc2538_reg_t TBSNAPS   :  1; /**< GPTM Timer B snap shot mode */
            cc2538_reg_t TBILD     :  1; /**< GPTM Timer B interval load write */
            cc2538_reg_t TBPWMIE   :  1; /**< GPTM Timer B PWM interrupt enable */
            cc2538_reg_t TBMRSU    :  1; /**< Timer B match register update mode */
            cc2538_reg_t TBPLO     :  1; /**< Legacy PWM operation */
            cc2538_reg_t RESERVED6 : 20; /**< Reserved bits */
        } TBMRbits;
    } cc2538_gptimer_tbmr;

    union {
        cc2538_reg_t CTL;                /**< GPTIMER Control */
        struct {
            cc2538_reg_t TAEN      :  1; /**< GPTM Timer A enable */
            cc2538_reg_t TASTALL   :  1; /**< GPTM Timer A stall enable */
            cc2538_reg_t TAEVENT   :  1; /**< GPTM Timer A event mode */
            cc2538_reg_t RESERVED1 :  1; /**< Reserved bits */
            cc2538_reg_t TAOTE     :  1; /**< GPTM Timer A PWM output trigger enable */
            cc2538_reg_t TAPWML    :  1; /**< GPTM Timer A PWM output level */
            cc2538_reg_t RESERVED2 :  1; /**< Reserved bits */
            cc2538_reg_t TBEN      :  1; /**< GPTM Timer B enable */
            cc2538_reg_t TBSTALL   :  1; /**< GPTM Timer B stall enable */
            cc2538_reg_t TBEVENT   :  1; /**< GPTM Timer B event mode */
            cc2538_reg_t RESERVED3 :  1; /**< Reserved bits */
            cc2538_reg_t TBOTE     :  1; /**< GPTM Timer B PWM output trigger enable */
            cc2538_reg_t TBPWML    :  1; /**< GPTM Timer B PWM output level */
            cc2538_reg_t RESERVED4 : 17; /**< Reserved bits */
        } CTLbits;
    } cc2538_gptimer_ctl;

    cc2538_reg_t SYNC;                   /**< GPTIMER Synchronize */
    cc2538_reg_t RESERVED2;              /**< Reserved word */
    cc2538_reg_t IMR;                    /**< GPTIMER Interrupt Mask */
    cc2538_reg_t RIS;                    /**< GPTIMER Raw Interrupt Status */
    cc2538_reg_t MIS;                    /**< GPTIMER Masked Interrupt Status */
    cc2538_reg_t ICR;                    /**< GPTIMER Interrupt Clear */
    cc2538_reg_t TAILR;                  /**< GPTIMER Timer A Interval Load */
    cc2538_reg_t TBILR;                  /**< GPTIMER Timer B Interval Load */
    cc2538_reg_t TAMATCHR;               /**< GPTIMER Timer A Match */
    cc2538_reg_t TBMATCHR;               /**< GPTIMER Timer B Match */
    cc2538_reg_t TAPR;                   /**< GPTIMER Timer A Prescale Register */
    cc2538_reg_t TBPR;                   /**< GPTIMER Timer B Prescale Register */
    cc2538_reg_t TAPMR;                  /**< GPTIMER Timer A Prescale Match Register */
    cc2538_reg_t TBPMR;                  /**< GPTIMER Timer B Prescale Match Register */
    cc2538_reg_t TAR;                    /**< GPTIMER Timer A */
    cc2538_reg_t TBR;                    /**< GPTIMER Timer B */
    cc2538_reg_t TAV;                    /**< GPTIMER Timer A Value */
    cc2538_reg_t TBV;                    /**< GPTIMER Timer B Value */
    cc2538_reg_t RESERVED3;              /**< Reserved word */
    cc2538_reg_t TAPS;                   /**< GPTIMER Timer A Prescale Snapshot */
    cc2538_reg_t TBPS;                   /**< GPTIMER Timer B Prescale Snapshot */
    cc2538_reg_t TAPV;                   /**< GPTIMER Timer A Prescale Value */
    cc2538_reg_t TBPV;                   /**< GPTIMER Timer B Prescale Value */
    cc2538_reg_t RESERVED[981];
    cc2538_reg_t PP;                     /**< GPTIMER Peripheral Properties */
} cc2538_gptimer_t;

#define GPTIMER0 ( (cc2538_gptimer_t*)0x40030000 )       /**< GPTIMER0 Instance */
#define GPTIMER1 ( (cc2538_gptimer_t*)0x40031000 )       /**< GPTIMER1 Instance */
#define GPTIMER2 ( (cc2538_gptimer_t*)0x40032000 )       /**< GPTIMER2 Instance */
#define GPTIMER3 ( (cc2538_gptimer_t*)0x40033000 )       /**< GPTIMER3 Instance */

void isr_timer0_chan0(void); /**< RIOT Timer 0 Channel 0 Interrupt Service Routine */
void isr_timer0_chan1(void); /**< RIOT Timer 0 Channel 1 Interrupt Service Routine */
void isr_timer1_chan0(void); /**< RIOT Timer 1 Channel 0 Interrupt Service Routine */
void isr_timer1_chan1(void); /**< RIOT Timer 1 Channel 1 Interrupt Service Routine */
void isr_timer2_chan0(void); /**< RIOT Timer 2 Channel 0 Interrupt Service Routine */
void isr_timer2_chan1(void); /**< RIOT Timer 2 Channel 1 Interrupt Service Routine */
void isr_timer3_chan0(void); /**< RIOT Timer 3 Channel 0 Interrupt Service Routine */
void isr_timer3_chan1(void); /**< RIOT Timer 3 Channel 1 Interrupt Service Routine */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* GPTIMER_H */

/* @} */
