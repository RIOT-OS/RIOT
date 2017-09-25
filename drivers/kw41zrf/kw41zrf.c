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

#define ENABLE_DEBUG    (0)
#include "debug.h"

enum {
    KW41Z_CCA_ED,
    KW41Z_CCA_MODE1,
    KW41Z_CCA_MODE2,
    KW41Z_CCA_MODE3
};

enum {
    KW41Z_STATE_IDLE,
    KW41Z_STATE_RX,
    KW41Z_STATE_TX,
    KW41Z_STATE_CCA,
    KW41Z_STATE_TXRX,
    KW41Z_STATE_CCCA
};

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
    /* Set default parameters according to STD IEEE802.15.4-2015 */
    dev->csma_max_be = 5;
    dev->csma_min_be = 3;
    dev->max_retrans = 3;
    dev->csma_max_backoffs = 4;

//     kw41zrf_set_power_mode(dev, KW41ZRF_POWER_IDLE);
    DEBUG("[kw41zrf] setup finished\n");
}

int kw41zrf_init(kw41zrf_t *dev, kw41zrf_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }

    /* Enable RSIM oscillator in Run mode, in order to be able to access the XCVR
     * registers if using the internal reference clock for the CPU core */
    bit_set32(&RSIM->CONTROL, RSIM_CONTROL_RF_OSC_EN_SHIFT);

    /* Wait for oscillator ready signal */
    while((RSIM->CONTROL & RSIM_CONTROL_RF_OSC_READY_MASK) == 0) {}

    xcvrStatus_t xcvrStatus = XCVR_Init(ZIGBEE_MODE, DR_500KBPS);
    if (xcvrStatus != gXcvrSuccess_c) {
        return -EIO;
    }

    /* Software reset of most settings */
    kw41zrf_reset_phy(dev);

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

    /* Configre Radio IRQ */
    kw41zrf_set_irq_callback(cb, dev);
    NVIC_ClearPendingIRQ(Radio_1_IRQn);
    NVIC_EnableIRQ(Radio_1_IRQn);

    kw41zrf_abort_sequence(dev);
    kw41zrf_unmask_irqs();

    DEBUG("[kw41zrf] init finished\n");

    return 0;
}

void kw41zrf_reset_phy(kw41zrf_t *dev)
{
    /* reset options and sequence number */
    dev->netdev.seq = 0;
    dev->netdev.flags = 0;

    /* set default protocol */
#ifdef MODULE_GNRC_SIXLOWPAN
    dev->netdev.proto = GNRC_NETTYPE_SIXLOWPAN;
#elif MODULE_GNRC
    dev->netdev.proto = GNRC_NETTYPE_UNDEF;
#endif

    /* Configure DSM exit oscillator stabilization delay */
    uint32_t tmp = (RSIM->RF_OSC_CTRL & RSIM_RF_OSC_CTRL_BB_XTAL_READY_COUNT_SEL_MASK) >>
        RSIM_RF_OSC_CTRL_BB_XTAL_READY_COUNT_SEL_SHIFT;
    /* Stabilization time is 1024 * 2^x radio crystal clocks, 0 <= x <= 3 */
    RSIM->DSM_OSC_OFFSET = (1024ul << tmp) / (CLOCK_RADIOXTAL / 32768) + 1; /* round up */

    /* Bring the device out of low power mode */
    kw41zrf_set_power_mode(dev, KW41ZRF_POWER_IDLE);

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

    /* Clear and disable all interrupts */
    kw41zrf_disable_interrupts(dev);

    /* Clear source address cache */
    ZLL->SAM_TABLE |= ZLL_SAM_TABLE_INVALIDATE_ALL_MASK;

    /* Accept FrameVersion 0 and 1 packets, reject all others */
    ZLL->RX_FRAME_FILTER = ZLL_RX_FRAME_FILTER_FRM_VER_FILTER(3) |
        ZLL_RX_FRAME_FILTER_ACK_FT_MASK |
        ZLL_RX_FRAME_FILTER_DATA_FT_MASK;

    /* Set prescaler to obtain 1 symbol (16us) timebase */
    kw41zrf_timer_init(dev, KW41ZRF_TIMEBASE_62500HZ);

    /* Set CCA threshold to -75 dBm */
    ZLL->CCA_LQI_CTRL = (ZLL->CCA_LQI_CTRL & ~ZLL_CCA_LQI_CTRL_CCA1_THRESH_MASK) |
        ZLL_CCA_LQI_CTRL_CCA1_THRESH(0xB5);

    /* Adjust ACK delay to fulfill the 802.15.4 turnaround requirements */
    ZLL->ACKDELAY = (ZLL->ACKDELAY & ~ZLL_ACKDELAY_ACKDELAY_MASK) | ZLL_ACKDELAY_ACKDELAY(-8);

    /* Adjust LQI compensation */
    /* Hardware reset default is 102 */
    ZLL->CCA_LQI_CTRL = (ZLL->CCA_LQI_CTRL & ~ZLL_CCA_LQI_CTRL_LQI_OFFSET_COMP_MASK) |
        ZLL_CCA_LQI_CTRL_LQI_OFFSET_COMP(96);

    /* set defaults */
    ZLL->SEQ_CTRL_STS = ZLL_SEQ_CTRL_STS_EVENT_TMR_DO_NOT_LATCH_MASK;

    dev->tx_power = KW41ZRF_DEFAULT_TX_POWER;
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

    kw41zrf_set_option(dev, KW41ZRF_OPT_TELL_RX_START, true);
    kw41zrf_set_option(dev, KW41ZRF_OPT_TELL_RX_END, true);
    kw41zrf_set_option(dev, KW41ZRF_OPT_TELL_TX_END, true);
    bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_SEQMSK_SHIFT);

    DEBUG("[kw41zrf] reset PHY and (re)set to channel %d and pan %d.\n",
          KW41ZRF_DEFAULT_CHANNEL, KW41ZRF_DEFAULT_PANID);
}
