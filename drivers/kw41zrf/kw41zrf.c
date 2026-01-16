/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw41zrf
 * @{
 * @file
 * @brief       Basic functionality of kw41zrf driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */
#include "net/netdev/ieee802154_submac.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "bit.h"
#include "log.h"
#include "msg.h"
#include "net/gnrc.h"
#include "net/ieee802154.h"

#include "kw41zrf.h"
#include "kw41zrf_netdev.h"
#include "kw41zrf_getset.h"
#include "kw41zrf_intern.h"
#include "vendor/XCVR/MKW41Z4/fsl_xcvr.h"
#include "vendor/XCVR/MKW41Z4/ifr_radio.h"
#include "vendor/MKW41Z4.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* vendor routine to initialize the radio core */
int kw41zrf_xcvr_init(kw41zrf_t *dev);

int kw41zrf_reset_hardware(kw41zrf_t *dev)
{
    /* Enable RSIM oscillator in RUN and WAIT modes, in order to be able to
     * access the XCVR and ZLL registers when using the internal reference clock
     * for the CPU core */
    RSIM->CONTROL |= RSIM_CONTROL_RF_OSC_EN(1);

    /* Wait for oscillator ready signal */
    while ((RSIM->CONTROL & RSIM_CONTROL_RF_OSC_READY_MASK) == 0) {}

    /* Assert radio software reset */
    RSIM->CONTROL |= RSIM_CONTROL_RADIO_RESET_BIT_MASK;
    /* De-assert radio software reset twice to follow recommendations in the
     * reference manual */
    RSIM->CONTROL &= ~RSIM_CONTROL_RADIO_RESET_BIT_MASK;
    RSIM->CONTROL &= ~RSIM_CONTROL_RADIO_RESET_BIT_MASK;

    DEBUG("[kw41zrf] start xcvr init\n");
    int res = kw41zrf_xcvr_init(dev);
    if (res < 0) {
        return res;
    }

    /* Configure DSM exit oscillator stabilization delay */
    uint32_t tmp = (RSIM->RF_OSC_CTRL & RSIM_RF_OSC_CTRL_BB_XTAL_READY_COUNT_SEL_MASK) >>
        RSIM_RF_OSC_CTRL_BB_XTAL_READY_COUNT_SEL_SHIFT;
    /* Stabilization time is 1024 * 2^x radio crystal clocks, 0 <= x <= 3 */
    RSIM->DSM_OSC_OFFSET = (1024ul << tmp) / (CLOCK_RADIOXTAL / 32768u) + 1u; /* round up */

    /* Clear and disable all interrupts */
    /* Reset PHY_CTRL to the default values, mask all interrupts,
     * enable RXACKRQD, we only use TR mode for receiving acknowledgements */
    ZLL->PHY_CTRL =
        ZLL_PHY_CTRL_CCATYPE(1) |
        ZLL_PHY_CTRL_TSM_MSK_MASK |
        ZLL_PHY_CTRL_WAKE_MSK_MASK |
        ZLL_PHY_CTRL_CRC_MSK_MASK |
        ZLL_PHY_CTRL_PLL_UNLOCK_MSK_MASK |
        ZLL_PHY_CTRL_FILTERFAIL_MSK_MASK |
        ZLL_PHY_CTRL_RX_WMRK_MSK_MASK |
        ZLL_PHY_CTRL_CCAMSK_MASK |
        ZLL_PHY_CTRL_RXMSK_MASK |
        ZLL_PHY_CTRL_TXMSK_MASK |
        ZLL_PHY_CTRL_SEQMSK_MASK |
        ZLL_PHY_CTRL_RXACKRQD_MASK |
        ZLL_PHY_CTRL_XCVSEQ(XCVSEQ_IDLE);

    /* Mask all unused timer interrupts and clear all interrupt flags */
    ZLL->IRQSTS =
        ZLL_IRQSTS_TMR1MSK_MASK |
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

    /* Clear source address cache */
    ZLL->SAM_TABLE |= ZLL_SAM_TABLE_INVALIDATE_ALL_MASK;

    /* Accept FrameVersion 0 and 1, data, command, and beacon frames */
    ZLL->RX_FRAME_FILTER = ZLL_RX_FRAME_FILTER_FRM_VER_FILTER(3) |
                           ZLL_RX_FRAME_FILTER_BEACON_FT_MASK |
                           ZLL_RX_FRAME_FILTER_CMD_FT_MASK |
                           ZLL_RX_FRAME_FILTER_DATA_FT_MASK;

    /* Set prescaler to obtain 1 symbol (16us) timebase */
    kw41zrf_timer_init(dev, KW41ZRF_TIMEBASE_62500HZ);

    /* Set CCA threshold to KW41ZRF_DEFAULT_CCA_THRESHOLD dBm */
    /* The hardware default for this register is +75 dBm (0x4b), which is nonsense */
    ZLL->CCA_LQI_CTRL = (ZLL->CCA_LQI_CTRL & ~ZLL_CCA_LQI_CTRL_CCA1_THRESH_MASK) |
        ZLL_CCA_LQI_CTRL_CCA1_THRESH(KW41ZRF_DEFAULT_CCA_THRESHOLD);

    /* Set default LQI compensation */
    /* Hardware reset default is 102 */
    ZLL->CCA_LQI_CTRL = (ZLL->CCA_LQI_CTRL & ~ZLL_CCA_LQI_CTRL_LQI_OFFSET_COMP_MASK) |
        ZLL_CCA_LQI_CTRL_LQI_OFFSET_COMP(KW41ZRF_DEFAULT_LQI_COMPENSATION);

    /* set defaults */
    ZLL->SEQ_CTRL_STS = ZLL_SEQ_CTRL_STS_EVENT_TMR_DO_NOT_LATCH_MASK;

    return 0;
}

void kw41zrf_print_irq_state(void)
{
    uint32_t irqsts = ZLL->IRQSTS;

    printf("\n=== ZLL IRQSTS (0x%08lX) ===\n", irqsts);
    printf("SEQIRQ:         %s\n", (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) ? "ON" : "OFF");
    printf("TXIRQ:          %s\n", (irqsts & ZLL_IRQSTS_TXIRQ_MASK) ? "ON" : "OFF");
    printf("RXIRQ:          %s\n", (irqsts & ZLL_IRQSTS_RXIRQ_MASK) ? "ON" : "OFF");
    printf("CCAIRQ:         %s\n", (irqsts & ZLL_IRQSTS_CCAIRQ_MASK) ? "ON" : "OFF");
    printf("RXWTRMRK:       %s\n", (irqsts & ZLL_IRQSTS_RXWTRMRKIRQ_MASK) ? "ON" : "OFF");
    printf("FILTERFAIL_IRQ: %s\n", (irqsts & ZLL_IRQSTS_FILTERFAIL_IRQ_MASK) ? "ON" : "OFF");
    printf("PLL_UNLOCK_IRQ: %s\n", (irqsts & ZLL_IRQSTS_PLL_UNLOCK_IRQ_MASK) ? "ON" : "OFF");
    printf("RX_FRM_PEND:    %s\n", (irqsts & ZLL_IRQSTS_RX_FRM_PEND_MASK) ? "ON" : "OFF");
    printf("WAKE_IRQ:       %s\n", (irqsts & ZLL_IRQSTS_WAKE_IRQ_MASK) ? "ON" : "OFF");
    printf("TMR1IRQ:        %s\n", (irqsts & ZLL_IRQSTS_TMR1IRQ_MASK) ? "ON" : "OFF");
    printf("TMR2IRQ:        %s\n", (irqsts & ZLL_IRQSTS_TMR2IRQ_MASK) ? "ON" : "OFF");
    printf("TMR3IRQ:        %s\n", (irqsts & ZLL_IRQSTS_TMR3IRQ_MASK) ? "ON" : "OFF");
    printf("TMR4IRQ:        %s\n", (irqsts & ZLL_IRQSTS_TMR4IRQ_MASK) ? "ON" : "OFF");
    printf("CRC_VALID:      %s\n", (irqsts & ZLL_IRQSTS_CRCVALID_MASK) ? "ON" : "OFF");
    printf("PI:             %s\n", (irqsts & ZLL_IRQSTS_PI_MASK) ? "ON" : "OFF");
    printf("SRCADDR:        0x%02lX\n", irqsts & ZLL_IRQSTS_SRCADDR_MASK);
    printf("=============================\n");
}

void kw41zrf_print_phy_ctrl(void)
{
    uint32_t phy_ctrl = ZLL->PHY_CTRL;

    printf("\n=== ZLL PHY_CTRL (0x%08lX) ===\n", phy_ctrl);
    printf("XCVSEQ:          0x%02lX\n", phy_ctrl & ZLL_PHY_CTRL_XCVSEQ_MASK);
    printf("AUTOACK:         %s\n", (phy_ctrl & ZLL_PHY_CTRL_AUTOACK_MASK) ? "ON" : "OFF");
    printf("RXACKRQD:        %s\n", (phy_ctrl & ZLL_PHY_CTRL_RXACKRQD_MASK) ? "ON" : "OFF");
    printf("CCABFRTX:        %s\n", (phy_ctrl & ZLL_PHY_CTRL_CCABFRTX_MASK) ? "ON" : "OFF");
    printf("SLOTTED:         %s\n", (phy_ctrl & ZLL_PHY_CTRL_SLOTTED_MASK) ? "ON" : "OFF");
    printf("TMRTRIGEN:       %s\n", (phy_ctrl & ZLL_PHY_CTRL_TMRTRIGEN_MASK) ? "ON" : "OFF");
    printf("SEQMSK:          %s\n", (phy_ctrl & ZLL_PHY_CTRL_SEQMSK_MASK) ? "ON" : "OFF");
    printf("TXMSK:           %s\n", (phy_ctrl & ZLL_PHY_CTRL_TXMSK_MASK) ? "ON" : "OFF");
    printf("RXMSK:           %s\n", (phy_ctrl & ZLL_PHY_CTRL_RXMSK_MASK) ? "ON" : "OFF");
    printf("CCAMSK:          %s\n", (phy_ctrl & ZLL_PHY_CTRL_CCAMSK_MASK) ? "ON" : "OFF");
    printf("RX_WMRK_MSK:     %s\n", (phy_ctrl & ZLL_PHY_CTRL_RX_WMRK_MSK_MASK) ? "ON" : "OFF");
    printf("FILTERFAIL_MSK:  %s\n", (phy_ctrl & ZLL_PHY_CTRL_FILTERFAIL_MSK_MASK) ? "ON" : "OFF");
    printf("PLL_UNLOCK_MSK:  %s\n", (phy_ctrl & ZLL_PHY_CTRL_PLL_UNLOCK_MSK_MASK) ? "ON" : "OFF");
    printf("CRC_MSK:         %s\n", (phy_ctrl & ZLL_PHY_CTRL_CRC_MSK_MASK) ? "ON" : "OFF");
    printf("WAKE_MSK:        %s\n", (phy_ctrl & ZLL_PHY_CTRL_WAKE_MSK_MASK) ? "ON" : "OFF");
    printf("TSM_MSK:         %s\n", (phy_ctrl & ZLL_PHY_CTRL_TSM_MSK_MASK) ? "ON" : "OFF");
    printf("TMR1CMP_EN:      %s\n", (phy_ctrl & ZLL_PHY_CTRL_TMR1CMP_EN_MASK) ? "ON" : "OFF");
    printf("TMR2CMP_EN:      %s\n", (phy_ctrl & ZLL_PHY_CTRL_TMR2CMP_EN_MASK) ? "ON" : "OFF");
    printf("TMR3CMP_EN:      %s\n", (phy_ctrl & ZLL_PHY_CTRL_TMR3CMP_EN_MASK) ? "ON" : "OFF");
    printf("TMR4CMP_EN:      %s\n", (phy_ctrl & ZLL_PHY_CTRL_TMR4CMP_EN_MASK) ? "ON" : "OFF");
    printf("TC2PRIME_EN:     %s\n", (phy_ctrl & ZLL_PHY_CTRL_TC2PRIME_EN_MASK) ? "ON" : "OFF");
    printf("PROMISCUOUS:     %s\n", (phy_ctrl & ZLL_PHY_CTRL_PROMISCUOUS_MASK) ? "ON" : "OFF");
    printf("CCATYPE:         0x%02lX\n", phy_ctrl & ZLL_PHY_CTRL_CCATYPE_MASK);
    printf("PANCORDNTR0:     %s\n", (phy_ctrl & ZLL_PHY_CTRL_PANCORDNTR0_MASK) ? "ON" : "OFF");
    printf("TC3TMOUT:        %s\n", (phy_ctrl & ZLL_PHY_CTRL_TC3TMOUT_MASK) ? "ON" : "OFF");
    printf("TRCV_MSK:        %s\n", (phy_ctrl & ZLL_PHY_CTRL_TRCV_MSK_MASK) ? "ON" : "OFF");
    printf("==================================\n");
}

void kw41zrf_print_seq_ctrl_sts(void)
{
    uint32_t seq_ctrl_sts = ZLL->SEQ_CTRL_STS;

    printf("\n=== ZLL SEQ_CTRL_STS (0x%08lX) ===\n", seq_ctrl_sts);
    printf("CLR_NEW_SEQ_INHIBIT:    %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_CLR_NEW_SEQ_INHIBIT_MASK) ? "ON" : "OFF");
    printf("EVENT_TMR_DO_NOT_LATCH: %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_EVENT_TMR_DO_NOT_LATCH_MASK) ? "ON" : "OFF");
    printf("LATCH_PREAMBLE:         %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_LATCH_PREAMBLE_MASK) ? "ON" : "OFF");
    printf("NO_RX_RECYCLE:          %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_NO_RX_RECYCLE_MASK) ? "ON" : "OFF");
    printf("FORCE_CRC_ERROR:        %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_FORCE_CRC_ERROR_MASK) ? "ON" : "OFF");
    printf("CONTINUOUS_EN:          %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_CONTINUOUS_EN_MASK) ? "ON" : "OFF");
    printf("XCVSEQ_ACTUAL:          0x%03lX\n", seq_ctrl_sts & ZLL_SEQ_CTRL_STS_XCVSEQ_ACTUAL_MASK);
    printf("SEQ_IDLE:               %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_SEQ_IDLE_MASK) ? "ON" : "OFF");
    printf("NEW_SEQ_INHIBIT:        %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_NEW_SEQ_INHIBIT_MASK) ? "ON" : "OFF");
    printf("RX_TIMEOUT_PENDING:     %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_RX_TIMEOUT_PENDING_MASK) ? "ON" : "OFF");
    printf("RX_MODE:                %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_RX_MODE_MASK) ? "ON" : "OFF");
    printf("TMR2_SEQ_TRIG_ARMED:    %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_TMR2_SEQ_TRIG_ARMED_MASK) ? "ON" : "OFF");
    printf("SEQ_T_STATUS:           0x%05lX\n", seq_ctrl_sts & ZLL_SEQ_CTRL_STS_SEQ_T_STATUS_MASK);
    printf("SW_ABORTED:             %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_SW_ABORTED_MASK) ? "ON" : "OFF");
    printf("TC3_ABORTED:            %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_TC3_ABORTED_MASK) ? "ON" : "OFF");
    printf("PLL_ABORTED:            %s\n", (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_PLL_ABORTED_MASK) ? "ON" : "OFF");
    printf("======================================\n");
}
