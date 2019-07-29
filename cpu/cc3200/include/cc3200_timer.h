/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc3200_timer CC3200 General Purpose Timer
 * @ingroup         cpu_cc3200_regs
 * @{
 *
 * @file
 * @brief           Driver for the cc3200 timer controller
 *
 * Header file with register and macro declarations for the cc3200 Timer module
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 * @{
 */

#ifndef CC3200_TIMER_H
#define CC3200_TIMER_H

#include "cc3200.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Timer interrupts
 * @{
 */
#define TIMER_TIMB_DMA 0x00002000     /**< TimerB DMA Done interrupt */
#define TIMER_TIMB_MATCH 0x00000800   /**< TimerB match interrupt */
#define TIMER_CAPB_EVENT 0x00000400   /**< CaptureB event interrupt */
#define TIMER_CAPB_MATCH 0x00000200   /**< CaptureB match interrupt */
#define TIMER_TIMB_TIMEOUT 0x00000100 /**< TimerB time out interrupt */
#define TIMER_TIMA_DMA 0x00000020     /**< TimerA DMA Done interrupt */
#define TIMER_TIMA_MATCH 0x00000010   /**< TimerA match interrupt */
#define TIMER_CAPA_EVENT 0x00000004   /**< CaptureA event interrupt */
#define TIMER_CAPA_MATCH 0x00000002   /**< CaptureA match interrupt */
#define TIMER_TIMA_TIMEOUT 0x00000001 /**< TimerA time out interrupt */
/** @} */

/**
 * @brief Timer/Subtimer selection
 * @{
 */
#define TIMER_A 0x000000ff    /**< Timer A */
#define TIMER_B 0x0000ff00    /**< Timer B */
#define TIMER_BOTH 0x0000ffff /**< Timer Both */
/** @} */

/**
 * @brief Timer configurations
 * @{
 */
#define TIMER_CFG_A_ONE_SHOT     0x00000021 /**< Timer A one-shot timer */
#define TIMER_CFG_A_ONE_SHOT_UP  0x00000031 /**< Timer A one-shot up-count timer */
#define TIMER_CFG_A_PERIODIC     0x00000022 /**< Timer A periodic timer */
#define TIMER_CFG_A_PERIODIC_UP  0x00000032 /**< Timer A periodic up-count timer */
#define TIMER_CFG_A_CAP_COUNT    0x00000003 /**< Timer A event counter */
#define TIMER_CFG_A_CAP_COUNT_UP 0x00000013 /**< Timer A event up-counter */
#define TIMER_CFG_A_CAP_TIME     0x00000007 /**< Timer A event timer */
#define TIMER_CFG_A_CAP_TIME_UP  0x00000017 /**< Timer A event up-count timer */
#define TIMER_CFG_A_PWM          0x0000000A /**< Timer A PWM output */
#define TIMER_CFG_B_ONE_SHOT     0x00002100 /**< Timer B one-shot timer */
#define TIMER_CFG_B_ONE_SHOT_UP  0x00003100 /**< Timer B one-shot up-count timer */
#define TIMER_CFG_B_PERIODIC     0x00002200 /**< Timer B periodic timer */
#define TIMER_CFG_B_PERIODIC_UP  0x00003200 /**< Timer B periodic up-count timer */
#define TIMER_CFG_B_CAP_COUNT    0x00000300 /**< Timer B event counter */
#define TIMER_CFG_B_CAP_COUNT_UP 0x00001300 /**< Timer B event up-counter */
#define TIMER_CFG_B_CAP_TIME     0x00000700 /**< Timer B event timer */
#define TIMER_CFG_B_CAP_TIME_UP  0x00001700 /**< Timer B event up-count timer */
#define TIMER_CFG_B_PWM          0x00000A00 /**< Timer B PWM output */
/** @} */

/**
 * @brief Timer component register
 * @{
 */
typedef struct {
    uint32_t conf;                /**< configuration */
    uint32_t timer_a_mode;        /**< timer A Mode */
    uint32_t timer_b_mode;        /**< timer B Mode */
    uint32_t ctrl;                /**< timer control register */
    uint8_t sync[8];              /**< sync */
    uint32_t intr_mask;           /**< interrupt mask */
    uint32_t intr_raw_stat;       /**< raw interrupt status */
    uint32_t masked_intr;         /**< masked interrupt */
    uint32_t intr_clear;          /**< interrupt clear */
    uint32_t interval_load_a;     /**< interval load a */
    uint32_t interval_load_b;     /**< interval load b */
    uint32_t match_a;             /**< timer match a */
    uint32_t match_b;             /**< timer match b */
    uint32_t prescale_a;          /**< timer prescale a */
    uint32_t prescale_b;          /**< timer prescale b */
    uint32_t prescale_match_a;    /**< timer prescale match a */
    uint32_t prescale_match_b;    /**< timer prescale match b */
    uint32_t timer_a;             /**< timer a */
    uint32_t timer_b;             /**< timer b */
    uint32_t val_a;               /**< timer value a */
    uint32_t val_b;               /**< timer value b */
    uint32_t rtc_predivide;       /**< RTC Predivide */
    uint32_t prescale_snaphot_a;  /**< timer prescale snapshot a */
    uint32_t prescale_snapshot_b; /**< timer prescale snapshot b */
    uint32_t val_snapshot_a;      /**< timer value snapshot a */
    uint32_t val_snapshot_b;      /**< timer value snapshot b */
    uint32_t dma_event;           /**< DMA event */
} cc3200_timer_t;
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC3200_TIMER_H */

/** @} */