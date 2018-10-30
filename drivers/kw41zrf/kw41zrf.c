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
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "msg.h"
#include "net/gnrc.h"
#include "net/ieee802154.h"
#include "luid.h"

#include "kw41zrf.h"
#include "kw41zrf_netdev.h"
#include "kw41zrf_getset.h"
#include "kw41zrf_intern.h"
#include "vendor/XCVR/MKW41Z4/fsl_xcvr.h"
#include "vendor/XCVR/MKW41Z4/ifr_radio.h"
#include "periph/timer.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

static void kw41zrf_set_address(kw41zrf_t *dev)
{
    DEBUG("[kw41zrf] Set MAC address\n");
    eui64_t addr_long;
    /* get an 8-byte unique ID to use as hardware address */
    luid_get(addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN);
    /* make sure we mark the address as non-multicast and not globally unique */
    addr_long.uint8[0] &= ~(0x01);
    addr_long.uint8[0] |=  (0x02);
    /* set short and long address */
    kw41zrf_set_addr_long(dev, ntohll(addr_long.uint64.u64));
    kw41zrf_set_addr_short(dev, ntohs(addr_long.uint16[0].u16));
}

void kw41zrf_setup(kw41zrf_t *dev)
{
    netdev_t *netdev = (netdev_t *)dev;

    netdev->driver = &kw41zrf_driver;
    /* initialize device descriptor */
    dev->idle_seq = XCVSEQ_RECEIVE;
    dev->pm_blocked = false;
    dev->recv_blocked = false;
    /* Set default parameters according to STD IEEE802.15.4-2015 */
    dev->csma_max_be = 5;
    dev->csma_min_be = 3;
    dev->max_retrans = 3;
    dev->csma_max_backoffs = 4;

    DEBUG("[kw41zrf] setup finished\n");
}

int kw41zrf_xcvr_init(kw41zrf_t *dev);

int kw41zrf_init(kw41zrf_t *dev, kw41zrf_cb_t cb)
{
    if (dev == NULL) {
        return -EINVAL;
    }

    /* Save a copy of the RF_OSC_EN setting to use when the radio is in deep sleep */
    dev->rf_osc_en_idle = RSIM->CONTROL & RSIM_CONTROL_RF_OSC_EN_MASK;
    kw41zrf_mask_irqs();
    kw41zrf_set_irq_callback(cb, dev);

    /* Perform clean reset of the radio modules. */
    int res = kw41zrf_reset(dev);
    if (res < 0) {
        /* initialization error signaled from vendor driver */
        /* Restore saved RF_OSC_EN setting */
        RSIM->CONTROL = (RSIM->CONTROL & ~RSIM_CONTROL_RF_OSC_EN_MASK) | dev->rf_osc_en_idle;
        return res;
    }
    /* Radio is now on and idle */

    /* Allow radio interrupts */
    kw41zrf_unmask_irqs();

    DEBUG("[kw41zrf] init finished\n");

    return 0;
}

int kw41zrf_reset_hardware(kw41zrf_t *dev)
{
    /* Enable RSIM oscillator in RUN and WAIT modes, in order to be able to
     * access the XCVR and ZLL registers when using the internal reference clock
     * for the CPU core */
    RSIM->CONTROL |= RSIM_CONTROL_RF_OSC_EN(1);

    /* Wait for oscillator ready signal */
    while((RSIM->CONTROL & RSIM_CONTROL_RF_OSC_READY_MASK) == 0) {}

    /* Assert radio software reset */
    RSIM->CONTROL |= RSIM_CONTROL_RADIO_RESET_BIT_MASK;
    /* De-assert radio software reset twice to follow recommendations in the
     * reference manual */
    RSIM->CONTROL &= ~RSIM_CONTROL_RADIO_RESET_BIT_MASK;
    RSIM->CONTROL &= ~RSIM_CONTROL_RADIO_RESET_BIT_MASK;

    timer_init(TIMER_PIT_DEV(0), 1000000ul, NULL, NULL);
    DEBUG("[kw41zrf] start xcvr init\n");
    uint32_t before = timer_read(TIMER_PIT_DEV(0));
    int res = kw41zrf_xcvr_init(dev);
    uint32_t after = timer_read(TIMER_PIT_DEV(0));
    DEBUG("[kw41zrf] took %" PRIu32 " us\n", (after - before));
    if (res < 0) {
        /* Most likely a calibration failure in XCVR driver */
        return res;
    }

    /* Configure DSM exit oscillator stabilization delay */
    uint32_t tmp = (RSIM->RF_OSC_CTRL & RSIM_RF_OSC_CTRL_BB_XTAL_READY_COUNT_SEL_MASK) >>
        RSIM_RF_OSC_CTRL_BB_XTAL_READY_COUNT_SEL_SHIFT;
    /* Stabilization time is 1024 * 2^x radio crystal clocks, 0 <= x <= 3 */
    RSIM->DSM_OSC_OFFSET = (1024ul << tmp) / (CLOCK_RADIOXTAL / 32768u) + 1u; /* round up */

    /* Clear and disable all interrupts */
    /* Reset PHY_CTRL to the default value of mask all interrupts and all other
     * settings disabled */
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
        ZLL_PHY_CTRL_XCVSEQ(XCVSEQ_IDLE);

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

    /* Clear source address cache */
    ZLL->SAM_TABLE |= ZLL_SAM_TABLE_INVALIDATE_ALL_MASK;

    /* Accept FrameVersion 0 and 1 packets, reject all others */
    ZLL->RX_FRAME_FILTER = ZLL_RX_FRAME_FILTER_FRM_VER_FILTER(3) |
        ZLL_RX_FRAME_FILTER_ACK_FT_MASK |
        ZLL_RX_FRAME_FILTER_DATA_FT_MASK;

    /* Set prescaler to obtain 1 symbol (16us) timebase */
    kw41zrf_timer_init(dev, KW41ZRF_TIMEBASE_62500HZ);

    /* Set CCA threshold to -70 dBm */
    /* The hardware default for this register is +75 dBm (0x4b), which is nonsense */
    ZLL->CCA_LQI_CTRL = (ZLL->CCA_LQI_CTRL & ~ZLL_CCA_LQI_CTRL_CCA1_THRESH_MASK) |
        ZLL_CCA_LQI_CTRL_CCA1_THRESH(-70);

    /* IEEE 802.15.4 requires that ACK transmission commences 12 symbol periods
     * (192 us) after the reception of the last octet of the frame being acknowledged. */
    //~ ZLL->ACKDELAY = (ZLL->ACKDELAY & ~ZLL_ACKDELAY_ACKDELAY_MASK) | ZLL_ACKDELAY_ACKDELAY(-8);

    /* Set default LQI compensation */
    /* Hardware reset default is 102 */
    ZLL->CCA_LQI_CTRL = (ZLL->CCA_LQI_CTRL & ~ZLL_CCA_LQI_CTRL_LQI_OFFSET_COMP_MASK) |
        ZLL_CCA_LQI_CTRL_LQI_OFFSET_COMP(102);

    /* set defaults */
    ZLL->SEQ_CTRL_STS = ZLL_SEQ_CTRL_STS_EVENT_TMR_DO_NOT_LATCH_MASK;

    return 0;
}

int kw41zrf_reset(kw41zrf_t *dev)
{
    kw41zrf_mask_irqs();

    int res = kw41zrf_reset_hardware(dev);
    if (res < 0) {
        /* Most likely a calibration failure in XCVR driver */
        kw41zrf_unmask_irqs();
        return res;
    }

    /* Compute warmup times (scaled to 16us) */
    dev->rx_warmup_time =
        (XCVR_TSM->END_OF_SEQ & XCVR_TSM_END_OF_SEQ_END_OF_RX_WU_MASK) >>
        XCVR_TSM_END_OF_SEQ_END_OF_RX_WU_SHIFT;
    dev->tx_warmup_time =
        (XCVR_TSM->END_OF_SEQ & XCVR_TSM_END_OF_SEQ_END_OF_TX_WU_MASK) >>
        XCVR_TSM_END_OF_SEQ_END_OF_TX_WU_SHIFT;

    /* divide by 16 and round up */
    dev->rx_warmup_time = (dev->rx_warmup_time + 15) / 16;
    dev->tx_warmup_time = (dev->tx_warmup_time + 15) / 16;

    /* Reset software link layer driver state */
    netdev_ieee802154_reset(&dev->netdev);

    dev->tx_power = KW41ZRF_DEFAULT_TX_POWER;
    dev->idle_seq = XCVSEQ_RECEIVE;
    kw41zrf_set_tx_power(dev, dev->tx_power);

    kw41zrf_set_channel(dev, KW41ZRF_DEFAULT_CHANNEL);

    kw41zrf_set_pan(dev, KW41ZRF_DEFAULT_PANID);
    kw41zrf_set_address(dev);

    kw41zrf_set_cca_mode(dev, 1);

    kw41zrf_set_rx_watermark(dev, 1);

    kw41zrf_set_option(dev, KW41ZRF_OPT_AUTOACK, true);
    kw41zrf_set_option(dev, KW41ZRF_OPT_ACK_REQ, true);
    kw41zrf_set_option(dev, KW41ZRF_OPT_CSMA, true);

    kw41zrf_set_power_mode(dev, KW41ZRF_POWER_IDLE);
    kw41zrf_set_sequence(dev, dev->idle_seq);

    bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_SEQMSK_SHIFT);

    kw41zrf_abort_sequence(dev);
    kw41zrf_unmask_irqs();

    DEBUG("[kw41zrf] reset radio and set to channel %d and pan %d.\n",
          KW41ZRF_DEFAULT_CHANNEL, KW41ZRF_DEFAULT_PANID);
    return 0;
}
