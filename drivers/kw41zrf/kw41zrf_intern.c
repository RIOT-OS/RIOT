/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw41zrf
 * @{
 * @file
 * @brief       Internal function of kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */

#include "irq.h"
#include "panic.h"
#include "kw41zrf.h"
#include "kw41zrf_getset.h"
#include "kw41zrf_intern.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Delay before entering deep sleep mode, in DSM_TIMER ticks (32.768 kHz)
 *
 * @attention must be >= 4 according to SoC ref. manual
 */
#define KW41ZRF_DSM_ENTER_DELAY 5

/**
 * @brief Delay before leaving deep sleep mode, in DSM_TIMER ticks (32.768 kHz)
 *
 * @attention must be >= 4 according to SoC ref. manual
 */
#define KW41ZRF_DSM_EXIT_DELAY 5

struct {
    void (*cb)(void *arg); /**< Callback function called from radio ISR */
    void *arg;             /**< Argument to callback */
} isr_config;

void kw41zrf_set_irq_callback(void (*cb)(void *arg), void *arg)
{
    unsigned int mask = irq_disable();
    isr_config.cb = cb;
    isr_config.arg = arg;
    irq_restore(mask);
}

void kw41zrf_disable_interrupts(kw41zrf_t *dev)
{
    (void) dev;
    DEBUG("[kw41zrf] disable interrupts\n");
    /* Clear and disable all interrupts */
    ZLL->PHY_CTRL |=
        ZLL_PHY_CTRL_TSM_MSK_MASK |
        ZLL_PHY_CTRL_WAKE_MSK_MASK |
        ZLL_PHY_CTRL_CRC_MSK_MASK |
        ZLL_PHY_CTRL_PLL_UNLOCK_MSK_MASK |
        ZLL_PHY_CTRL_FILTERFAIL_MSK_MASK |
        ZLL_PHY_CTRL_RX_WMRK_MSK_MASK |
        ZLL_PHY_CTRL_CCAMSK_MASK |
        ZLL_PHY_CTRL_RXMSK_MASK |
        ZLL_PHY_CTRL_TXMSK_MASK |
        ZLL_PHY_CTRL_SEQMSK_MASK;

    /* Mask all timer interrupts and clear all interrupt flags */
    ZLL->IRQSTS =
        ZLL_IRQSTS_TMR1MSK_MASK |
        ZLL_IRQSTS_TMR2MSK_MASK |
        ZLL_IRQSTS_TMR3MSK_MASK |
        ZLL_IRQSTS_TMR4MSK_MASK |
        ZLL_IRQSTS_TMR1IRQ_MASK |
        ZLL_IRQSTS_TMR2IRQ_MASK |
        ZLL_IRQSTS_TMR3IRQ_MASK |
        ZLL_IRQSTS_TMR4IRQ_MASK |
        ZLL_IRQSTS_WAKE_IRQ_MASK |
        ZLL_IRQSTS_PLL_UNLOCK_IRQ_MASK |
        ZLL_IRQSTS_FILTERFAIL_IRQ_MASK |
        ZLL_IRQSTS_RXWTRMRKIRQ_MASK |
        ZLL_IRQSTS_CCAIRQ_MASK |
        ZLL_IRQSTS_RXIRQ_MASK |
        ZLL_IRQSTS_TXIRQ_MASK |
        ZLL_IRQSTS_SEQIRQ_MASK;
}

void kw41zrf_set_power_mode(kw41zrf_t *dev, kw41zrf_powermode_t pm)
{
    DEBUG("[kw41zrf] set power mode to %u\n", pm);
    unsigned state = irq_disable();
    switch (pm) {
        case KW41ZRF_POWER_IDLE:
        {
            if (!(RSIM->DSM_CONTROL & RSIM_DSM_CONTROL_ZIG_DEEP_SLEEP_STATUS_MASK)) {
                /* Already awake */
                break;
            }
            /* Assume DSM timer has been running since we entered sleep mode */
            /* In case it was not already running, however, we still set the
             * enable flag here. */
            RSIM->DSM_CONTROL = (RSIM_DSM_CONTROL_DSM_TIMER_EN_MASK |
                                RSIM_DSM_CONTROL_ZIG_SYSCLK_REQUEST_EN_MASK);
            /* The wake target must be at least (4 + RSIM_DSM_OSC_OFFSET) ticks
             * into the future, to let the oscillator stabilize before switching
             * on the clocks */
            RSIM->ZIG_WAKE = KW41ZRF_DSM_EXIT_DELAY + RSIM->DSM_TIMER + RSIM->DSM_OSC_OFFSET;
            while (RSIM->DSM_CONTROL & RSIM_DSM_CONTROL_ZIG_DEEP_SLEEP_STATUS_MASK) {}
            /* Convert DSM ticks (32.768 kHz) to event timer ticks (1 MHz) */
            uint64_t tmp = (uint64_t)(RSIM->ZIG_WAKE - RSIM->ZIG_SLEEP) * 15625ul;
            uint32_t usec = (tmp >> 9); /* equivalent to (usec / 512) */
            ZLL->EVENT_TMR = ZLL_EVENT_TMR_EVENT_TMR_ADD_MASK |
                ZLL_EVENT_TMR_EVENT_TMR(usec);
            break;
        }
        case KW41ZRF_POWER_DSM:
        {
            if (RSIM->DSM_CONTROL & RSIM_DSM_CONTROL_ZIG_DEEP_SLEEP_STATUS_MASK) {
                /* Already asleep */
                break;
            }
            /* Clear IRQ flags */
            RSIM->DSM_CONTROL = RSIM->DSM_CONTROL;
            /* Enable timer triggered sleep */
            ZLL->DSM_CTRL = ZLL_DSM_CTRL_ZIGBEE_SLEEP_EN_MASK;
            /* Set sleep start time */
            /* The target time must be at least 4 DSM_TIMER ticks into the future */
            RSIM->ZIG_SLEEP = RSIM->DSM_TIMER + KW41ZRF_DSM_ENTER_DELAY;
            /* The device will automatically wake up 8.5 minutes from now if not
             * awoken sooner by software */
            /* TODO handle automatic wake in the ISR if it becomes an issue */
            RSIM->ZIG_WAKE = RSIM->DSM_TIMER - 1;
            /* Start the 32.768 kHz DSM timer in case it was not already running */
            /* If ZIG_SYSCLK_REQUEST_EN is not set then the hardware will not
             * enter DSM and we get stuck in the while() below */
            RSIM->DSM_CONTROL = (RSIM_DSM_CONTROL_DSM_TIMER_EN_MASK |
                                RSIM_DSM_CONTROL_ZIG_SYSCLK_REQUEST_EN_MASK);
            while (!(RSIM->DSM_CONTROL & RSIM_DSM_CONTROL_ZIG_DEEP_SLEEP_STATUS_MASK)) {}
            /* Let the DSM timer run until we exit deep sleep mode */
            break;
        }
        default:
            DEBUG("[kw41zrf] Unknown power mode %u\n", pm);
            break;
    }
    irq_restore(state);
}

void kw41zrf_set_sequence(kw41zrf_t *dev, uint32_t seq)
{
    (void) dev;
    DEBUG("[kw41zrf] set sequence to %u\n", (unsigned int)seq);
    assert((ZLL->PHY_CTRL & ZLL_PHY_CTRL_XCVSEQ_MASK) == XCVSEQ_IDLE);
    while ((ZLL->SEQ_CTRL_STS & ZLL_SEQ_CTRL_STS_SEQ_IDLE_MASK) == 0) {
        kw41zrf_abort_sequence(dev);
    }
    /* Clear interrupt flags, sometimes the sequence complete flag is immediately set */
    ZLL->IRQSTS = ZLL->IRQSTS;
    ZLL->PHY_CTRL = (ZLL->PHY_CTRL & ~(ZLL_PHY_CTRL_XCVSEQ_MASK | ZLL_PHY_CTRL_SEQMSK_MASK)) | seq;
    while (((ZLL->SEQ_CTRL_STS & ZLL_SEQ_CTRL_STS_XCVSEQ_ACTUAL_MASK) >>
        ZLL_SEQ_CTRL_STS_XCVSEQ_ACTUAL_SHIFT) != (ZLL_PHY_CTRL_XCVSEQ_MASK & seq)) {}
}

int kw41zrf_can_switch_to_idle(kw41zrf_t *dev)
{
    (void) dev;
    uint8_t seq = (ZLL->PHY_CTRL & ZLL_PHY_CTRL_XCVSEQ_MASK) >> ZLL_PHY_CTRL_XCVSEQ_SHIFT;

    DEBUG("[kw41zrf] XCVSEQ=0x%x, SEQ_STATE=0x%" PRIx32 ", SEQ_CTRL_STS=0x%" PRIx32 "\n", seq,
        ZLL->SEQ_STATE, ZLL->SEQ_CTRL_STS);

    switch (seq)
    {
        case XCVSEQ_TRANSMIT:
        case XCVSEQ_TX_RX:
        case XCVSEQ_CCA:
            /* We should wait until TX or CCA has finished before moving to
             * another mode */
            return 0;
        default:
            break;
    }

    return 1;
}

void isr_radio_1(void)
{
    DEBUG("[kw41zrf] INT1\n");
    if (isr_config.cb != NULL) {
        isr_config.cb(isr_config.arg);
    }
    cortexm_isr_end();
}
