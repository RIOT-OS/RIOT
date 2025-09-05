/*
 * SPDX-FileCopyrightText: 2017 SKF AB
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <assert.h>
#include "log.h"
#include "irq.h"
#include "panic.h"
#include "kw41zrf.h"
#include "kw41zrf_getset.h"
#include "kw41zrf_intern.h"

#define ENABLE_DEBUG 0
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

void kw41zrf_set_power_mode(kw41zrf_t *dev, kw41zrf_powermode_t pm)
{
    DEBUG("[kw41zrf] set power mode to %u\n", pm);
    unsigned state = irq_disable();
    switch (pm) {
        case KW41ZRF_POWER_IDLE:
        {
            /* Disable some CPU power management if we need to be active, otherwise the
             * radio will be stuck in state retention mode. */
            if (!dev->pm_blocked) {
                PM_BLOCK(KW41ZRF_PM_BLOCKER);
                dev->pm_blocked = 1;
            }
            /* Restore saved RF oscillator settings, enable oscillator in RUN mode
             * to allow register access */
            /* This is also where the oscillator is enabled during kw41zrf_init:
             * kw41zrf_init -> kw41zrf_reset_phy -> kw41zrf_set_power_mode
             * => Do not return before this line during init */
            RSIM->CONTROL |= RSIM_CONTROL_RF_OSC_EN(1);
            /* Assume DSM timer has been running since we entered sleep mode */
            /* In case it was not already running, however, we still set the
             * enable flag here. */
            /* RSIM_DSM_CONTROL_ZIG_SYSCLK_REQUEST_EN lets the link layer
             * request the RF oscillator to remain on during STOP and VLPS, to
             * allow stopping the CPU core without affecting TX or RX operations */
            RSIM->DSM_CONTROL = (RSIM_DSM_CONTROL_DSM_TIMER_EN_MASK |
                                RSIM_DSM_CONTROL_ZIG_SYSCLK_REQUEST_EN_MASK);
            /* Wait for oscillator ready signal before attempting to recover from DSM */
            while ((RSIM->CONTROL & RSIM_CONTROL_RF_OSC_READY_MASK) == 0) {}
            KW41ZRF_LED_NDSM_ON;
            /* If we are already awake we can just return now. */
            if (!(kw41zrf_is_dsm())) {
                /* Already awake */
                break;
            }
            /* The wake target must be at least (4 + RSIM_DSM_OSC_OFFSET) ticks
             * into the future, to let the oscillator stabilize before switching
             * on the clocks */
            RSIM->ZIG_WAKE = KW41ZRF_DSM_EXIT_DELAY + RSIM->DSM_TIMER + RSIM->DSM_OSC_OFFSET;
            /* Wait to come out of DSM */
            while (kw41zrf_is_dsm()) {}

            /* Convert DSM ticks (32.768 kHz) to event timer ticks (1 MHz) */
            uint64_t tmp = (uint64_t)(RSIM->ZIG_WAKE - RSIM->ZIG_SLEEP) * 15625ul;
            uint32_t usec = (tmp >> 9); /* equivalent to (usec / 512) */
            /* Add the offset */
            ZLL->EVENT_TMR = ZLL_EVENT_TMR_EVENT_TMR_ADD_MASK |
                ZLL_EVENT_TMR_EVENT_TMR(usec);

            /* Clear IRQ flags */
            uint32_t irqsts = ZLL->IRQSTS;
            DEBUG("[kw41zrf] wake IRQSTS=%" PRIx32 "\n", irqsts);
            ZLL->IRQSTS = irqsts;

            /* Disable DSM timer triggered sleep */
            ZLL->DSM_CTRL = 0;

            break;
        }
        case KW41ZRF_POWER_DSM:
        {
            if (kw41zrf_is_dsm()) {
                /* Already asleep */
                break;
            }
            if (dev->pm_blocked) {
                PM_UNBLOCK(KW41ZRF_PM_BLOCKER);
                dev->pm_blocked = 0;
            }
            /* Race condition: if sleep is re-triggered after wake before the
             * DSM_ZIG_FINISHED flag has been switched off, then the RSIM
             * becomes stuck and never enters DSM.
             * The time from ZIG_WAKE until DSM_ZIG_FINISHED is turned off seem
             * to be constant at 2 DSM ticks */
            while (RSIM->DSM_CONTROL & RSIM_DSM_CONTROL_DSM_ZIG_FINISHED_MASK) {}
            /* Clear IRQ flags */
            uint32_t irqsts = RSIM->DSM_CONTROL;
            RSIM->DSM_CONTROL = irqsts;
            irqsts = ZLL->IRQSTS;
            DEBUG("[kw41zrf] sleep IRQSTS=%" PRIx32 "\n", irqsts);
            ZLL->IRQSTS = irqsts;
            NVIC_ClearPendingIRQ(Radio_1_IRQn);

            /* Enable timer triggered sleep */
            ZLL->DSM_CTRL = ZLL_DSM_CTRL_ZIGBEE_SLEEP_EN_MASK;
            /* The device will automatically wake up 8.5 minutes from now if not
             * awoken sooner by software */
            /* TODO handle automatic wake in the ISR if it becomes an issue */
            RSIM->ZIG_WAKE = RSIM->DSM_TIMER - KW41ZRF_DSM_EXIT_DELAY - RSIM->DSM_OSC_OFFSET;
            /* Set sleep start time */
            /* The target time must be at least 4 DSM_TIMER ticks into the future */
            RSIM->ZIG_SLEEP = RSIM->DSM_TIMER + KW41ZRF_DSM_ENTER_DELAY;
            /* Start the 32.768 kHz DSM timer in case it was not already running */
            /* If ZIG_SYSCLK_REQUEST_EN is not set then the hardware will not
             * enter DSM and we get stuck in the while() below */
            RSIM->DSM_CONTROL = (RSIM_DSM_CONTROL_DSM_TIMER_EN_MASK |
                                RSIM_DSM_CONTROL_ZIG_SYSCLK_REQUEST_EN_MASK);
            while (!(kw41zrf_is_dsm())) {}
            KW41ZRF_LED_NDSM_OFF;
            /* Restore saved RF_OSC_EN bits (from kw41zrf_init)
             * This will disable the RF oscillator unless the system was
             * configured to use the RF oscillator before kw41zrf_init() was
             * called, for example when using the RF oscillator for the CPU core
             * clock. */
            RSIM->CONTROL = (RSIM->CONTROL & ~RSIM_CONTROL_RF_OSC_EN_MASK) |
                dev->rf_osc_en_idle;
            /* Let the DSM timer run until we exit deep sleep mode */
            break;
        }
        default:
            LOG_ERROR("[kw41zrf] Unknown power mode %u\n", pm);
            break;
    }
    irq_restore(state);
}

void kw41zrf_set_sequence(kw41zrf_t *dev, uint32_t seq)
{
    (void) dev;
    DEBUG("[kw41zrf] set sequence to %x\n", (unsigned)seq);
    assert(!kw41zrf_is_dsm());
    unsigned back_to_sleep = 0;
    if (seq == XCVSEQ_DSM_IDLE) {
        back_to_sleep = 1;
        seq = XCVSEQ_IDLE;
    }
    else if ((seq == XCVSEQ_RECEIVE) && dev->recv_blocked) {
        /* Wait in standby until recv has been called to avoid corrupting the RX
         * buffer before the frame has been received by the higher layers */
        seq = XCVSEQ_IDLE;
    }
    uint32_t seq_old = ZLL->PHY_CTRL & ZLL_PHY_CTRL_XCVSEQ_MASK;
    if (seq_old != XCVSEQ_IDLE && seq_old != XCVSEQ_RECEIVE) {
        LOG_ERROR("[kw41zrf] seq not idle: 0x%" PRIu32 "\n", seq_old);
        assert(0);
    }

    kw41zrf_abort_sequence(dev);

    ZLL->PHY_CTRL = (ZLL->PHY_CTRL & ~(ZLL_PHY_CTRL_XCVSEQ_MASK | ZLL_PHY_CTRL_SEQMSK_MASK)) | seq;
    while (((ZLL->SEQ_CTRL_STS & ZLL_SEQ_CTRL_STS_XCVSEQ_ACTUAL_MASK) >>
        ZLL_SEQ_CTRL_STS_XCVSEQ_ACTUAL_SHIFT) != (ZLL_PHY_CTRL_XCVSEQ_MASK & seq)) {}
    if (back_to_sleep) {
        kw41zrf_set_power_mode(dev, KW41ZRF_POWER_DSM);
    }
}

int kw41zrf_can_switch_to_idle(kw41zrf_t *dev)
{
    (void) dev;
    if (!kw41zrf_is_dsm()) {
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
