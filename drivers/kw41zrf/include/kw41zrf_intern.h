/*
 * Copyright (C) 2017 SKF AB
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_kw41zrf
 * @{
 *
 * @file
 * @brief       Internal function interfaces for kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */


#include <stdint.h>
#include "kw41zrf.h"
/* For XCVSEQ_IDLE */
#include "kw41zrf_getset.h"
/* For ZLL CPU registers */
#include "cpu.h"

/* When the transceiver is not in DSM, this power mode will be blocked. */
#define KW41ZRF_PM_BLOCKER KINETIS_PM_VLPS

/* Set to 1 to use on board LEDs to show RX/TX activity */
#ifndef KW41ZRF_ENABLE_LEDS
#define KW41ZRF_ENABLE_LEDS     (0)
#endif

#if KW41ZRF_ENABLE_LEDS
/* For LED macros */
#include "board.h"
#if !defined(KW41ZRF_LED_RX_ON)
#if defined(LED0_ON)
#define KW41ZRF_LED_RX_ON       LED0_ON
#define KW41ZRF_LED_RX_OFF      LED0_OFF
#else /* defined(LED0_ON) */
#define KW41ZRF_LED_RX_ON
#define KW41ZRF_LED_RX_OFF
#endif /* defined(LED0_ON) */
#endif /* !defined(KW41ZRF_LED_RX_ON) */
#if !defined(KW41ZRF_LED_TX_ON)
#if defined(LED1_ON)
/* Separate TX LED */
#define KW41ZRF_LED_TX_ON       LED1_ON
#define KW41ZRF_LED_TX_OFF      LED1_OFF
#elif defined(LED0_ON)
/* Combined RX+TX in one LED */
#define KW41ZRF_LED_TX_ON       LED0_ON
#define KW41ZRF_LED_TX_OFF      LED0_OFF
#else /* defined(LEDx_ON) */
#define KW41ZRF_LED_TX_ON
#define KW41ZRF_LED_TX_OFF
#endif /* defined(LEDx_ON) */
#endif /* !defined(KW41ZRF_LED_TX_ON) */
#if !defined(KW41ZRF_LED_NDSM_ON)
#if defined(LED2_ON)
#define KW41ZRF_LED_NDSM_ON     LED2_ON
#define KW41ZRF_LED_NDSM_OFF    LED2_OFF
#else /* defined(LEDx_ON) */
#define KW41ZRF_LED_NDSM_ON
#define KW41ZRF_LED_NDSM_OFF
#endif /* defined(LEDx_ON) */
#endif /* !defined(KW41ZRF_LED_NDSM_ON) */
#if !defined(KW41ZRF_LED_IRQ_ON)
#if defined(LED3_ON)
#define KW41ZRF_LED_IRQ_ON      LED3_ON
#define KW41ZRF_LED_IRQ_OFF     LED3_OFF
#else /* defined(LEDx_ON) */
#define KW41ZRF_LED_IRQ_ON
#define KW41ZRF_LED_IRQ_OFF
#endif /* defined(LEDx_ON) */
#endif /* !defined(KW41ZRF_LED_IRQ_ON) */
#else /* KW41ZRF_ENABLE_LEDS */
#define KW41ZRF_LED_NDSM_ON
#define KW41ZRF_LED_NDSM_OFF
#define KW41ZRF_LED_TX_ON
#define KW41ZRF_LED_TX_OFF
#define KW41ZRF_LED_RX_ON
#define KW41ZRF_LED_RX_OFF
#define KW41ZRF_LED_IRQ_ON
#define KW41ZRF_LED_IRQ_OFF
#endif /* KW41ZRF_ENABLE_LEDS */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   KW41Z transceiver power modes
 */
typedef enum {
    KW41ZRF_POWER_IDLE = 0, /**< All parts powered */
    KW41ZRF_POWER_DSM, /**< Deep sleep mode */
} kw41zrf_powermode_t;

/**
 * @brief   Timebase settings
 */
typedef enum kw41zrf_timer_timebase {
    KW41ZRF_TIMEBASE_500000HZ = 0b010,
    KW41ZRF_TIMEBASE_250000HZ = 0b011,
    KW41ZRF_TIMEBASE_125000HZ = 0b100,
    KW41ZRF_TIMEBASE_62500HZ  = 0b101,
    KW41ZRF_TIMEBASE_31250HZ  = 0b110,
    KW41ZRF_TIMEBASE_15625HZ  = 0b111,
} kw41zrf_timer_timebase_t;

/**
 * @brief   Mask all transceiver interrupts
 */
static inline void kw41zrf_mask_irqs(void)
{
    NVIC_DisableIRQ(Radio_1_IRQn);
    NVIC_ClearPendingIRQ(Radio_1_IRQn);
}

/**
 * @brief   Unmask all transceiver interrupts
 */
static inline void kw41zrf_unmask_irqs(void)
{
    KW41ZRF_LED_IRQ_OFF;
    NVIC_EnableIRQ(Radio_1_IRQn);
}

/**
 * @brief   Set the callback function for the radio ISR
 *
 * This callback will be called from ISR context when a radio_1 interrupt occurs
 *
 * @param[in]  cb   Pointer to callback function
 * @param[in]  arg  Argument that will be passed to the callback
 */
void kw41zrf_set_irq_callback(void (*cb)(void *arg), void *arg);

/**
 * @brief   Set power mode for device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] pm        power mode value
 */
void kw41zrf_set_power_mode(kw41zrf_t *dev, kw41zrf_powermode_t pm);

/**
 * @brief Determine if the transceiver is busy doing TX or RX
 *
 * @param[in] dev       kw41zrf device descriptor
 *
 * @return 0 if transceiver is in progress transmitting a packet
 * @return 1 otherwise
 */
int kw41zrf_can_switch_to_idle(kw41zrf_t *dev);

/**
 * @brief   Set sequence state of device
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] seq       sequence
 */
void kw41zrf_set_sequence(kw41zrf_t *dev, uint32_t seq);

/**
 * @brief Abort the current autosequence
 *
 * @param[in] dev       kw41zrf device descriptor
 */
static inline void kw41zrf_abort_sequence(kw41zrf_t *dev)
{
    (void) dev;
    /* Writing IDLE to XCVSEQ aborts any ongoing sequence */
    ZLL->PHY_CTRL = (ZLL->PHY_CTRL &
        ~(ZLL_PHY_CTRL_XCVSEQ_MASK |
        ZLL_PHY_CTRL_TC3TMOUT_MASK | ZLL_PHY_CTRL_TMRTRIGEN_MASK)) |
        ZLL_PHY_CTRL_XCVSEQ(XCVSEQ_IDLE) | ZLL_PHY_CTRL_SEQMSK_MASK;
    /* Spin until the sequence manager has acknowledged the sequence abort, this
     * should not take many cycles */
    while (!(ZLL->SEQ_CTRL_STS & ZLL_SEQ_CTRL_STS_SEQ_IDLE_MASK)) {}

    /* Clear interrupt flags */
    uint32_t irqsts = ZLL->IRQSTS;
    ZLL->IRQSTS = irqsts;
}

/**
 * @brief   Check if the radio is in deep sleep mode
 *
 * @return non-zero if radio is in deep sleep
 * @return 0 if radio is not in deep sleep mode
 */
static inline uint32_t kw41zrf_is_dsm(void)
{
    return (RSIM->DSM_CONTROL & RSIM_DSM_CONTROL_ZIG_DEEP_SLEEP_STATUS_MASK);
}

/**
 * @brief Set event timer counter value
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] value     new time
 */
static inline void kw41zrf_timer_load(kw41zrf_t *dev, uint32_t value)
{
    (void) dev;
    ZLL->EVENT_TMR = ZLL_EVENT_TMR_EVENT_TMR(value) | ZLL_EVENT_TMR_EVENT_TMR_LD_MASK;
}

/**
 * @brief Get current event timer counter value
 *
 * @param[in] dev       kw41zrf device descriptor
 *
 * @return  Current timer value
 */
static inline uint32_t kw41zrf_timer_get(kw41zrf_t *dev)
{
    (void) dev;
    return (ZLL->EVENT_TMR & ZLL_EVENT_TMR_EVENT_TMR_MASK) >> ZLL_EVENT_TMR_EVENT_TMR_SHIFT;
}

/**
 * @brief Set a timeout value for the given compare register of the Event Timer
 *
 * @param[in]  dev       kw41zrf device descriptor
 * @param[out] cmp_reg   pointer to timer compare register, &ZLL->TxCMP
 * @param[in]  timeout   timer offset from current time
 */
static inline void kw41zrf_timer_set(kw41zrf_t *dev, volatile uint32_t *cmp_reg, uint32_t timeout)
{
    uint32_t now = kw41zrf_timer_get(dev);

    *cmp_reg = now + timeout;
}

/**
 * @brief   Initialize the Event Timer Block (up counter)
 *
 * The Event Timer Block provides:
 *   - Abort an RX and CCA sequence at pre-determined time
 *   - Latches "timestamp" value during packet reception
 *   - Initiates timer-triggered sequences
 *
 * @param[in] dev       kw41zrf device descriptor
 * @param[in] tb        timer base value
 */
static inline void kw41zrf_timer_init(kw41zrf_t *dev, kw41zrf_timer_timebase_t tb)
{
    ZLL->TMR_PRESCALE = (ZLL->TMR_PRESCALE & ~ZLL_TMR_PRESCALE_TMR_PRESCALE_MASK) |
    ZLL_TMR_PRESCALE_TMR_PRESCALE(tb);
    kw41zrf_timer_load(dev, 0);
}

/**
 * @brief   Returns timestamp of the beginning of the most recently received packet
 *
 * The latched timestamp corresponds to the point where the SFD detection was
 * triggered for the most recent packet, i.e. right before the first byte of the
 * packet.
 *
 * @param[in] dev       kw41zrf device descriptor
 *
 * @return              timestamp value
 */
static inline uint32_t kw41zrf_get_timestamp(kw41zrf_t *dev)
{
    (void) dev;
    return ZLL->TIMESTAMP;
}

#ifdef __cplusplus
}
#endif

/** @} */
