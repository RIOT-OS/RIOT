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


