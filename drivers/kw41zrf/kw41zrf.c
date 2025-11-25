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

#include "kw41zrf.h"
#include "kw41zrf_netdev.h"
#include "kw41zrf_getset.h"
#include "kw41zrf_intern.h"
#include "vendor/XCVR/MKW41Z4/fsl_xcvr.h"
#include "vendor/XCVR/MKW41Z4/ifr_radio.h"
#include "vendor/MKW41Z4.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void kw41zrf_set_address(kw41zrf_t *dev)
{
    DEBUG("[kw41zrf] Set MAC address\n");

    /* set short and long address */
    kw41zrf_set_addr_long(dev, (eui64_t *)&dev->netdev.long_addr);
    kw41zrf_set_addr_short(dev, (network_uint16_t *)&dev->netdev.short_addr);
}

void kw41zrf_setup(kw41zrf_t *dev, uint8_t index)
{
    netdev_t *netdev = &dev->netdev.netdev;

    netdev->driver = &kw41zrf_driver;

    /* register with netdev */
    netdev_register(netdev, NETDEV_KW41ZRF, index);

    /* get unique IDs to use as hardware addresses */
    netdev_ieee802154_setup(&dev->netdev);

    /* initialize device descriptor */
    dev->idle_seq = XCVSEQ_RECEIVE;
    dev->pm_blocked = 0;
    dev->recv_blocked = 0;
    /* Set default parameters according to STD IEEE802.15.4-2015 */
    dev->csma_max_be = 5;
    dev->csma_min_be = 3;
    dev->max_retrans = 3;
    dev->csma_max_backoffs = 4;

    DEBUG("[kw41zrf] setup finished\n");
}

/* vendor routine to initialize the radio core */
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

    DEBUG("[kw41zrf] enabling RX start IRQs\n");
    bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_RX_WMRK_MSK_SHIFT);

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

int kw41zrf_reset(kw41zrf_t *dev)
{
    kw41zrf_mask_irqs();

    /* Sometimes (maybe 1 in 30 reboots) there is a failure in the vendor
     * routines in kw41zrf_rx_bba_dcoc_dac_trim_DCest() that can be worked
     * around by retrying. Clearly this is not ideal.
     */
    for (int retries = 0; ; retries++) {
        int res = kw41zrf_reset_hardware(dev);
        if (!res) {
            if (retries) {
                LOG_WARNING("kw41zrf_reset_hardware() needed %i retries\n",
                            retries);
            }
            break;
        }
        if (retries == 9) {
            LOG_ERROR("kw41zrf_reset_hardware() returned %i\n", res);
            kw41zrf_unmask_irqs();
            return res;
        }
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
    kw41zrf_set_power_mode(dev, KW41ZRF_POWER_IDLE);
    kw41zrf_abort_sequence(dev);
    kw41zrf_set_tx_power(dev, dev->tx_power);

    kw41zrf_set_channel(dev, KW41ZRF_DEFAULT_CHANNEL);

    kw41zrf_set_address(dev);

    kw41zrf_set_cca_mode(dev, 1);

    kw41zrf_set_rx_watermark(dev, 1);

    kw41zrf_set_option(dev, KW41ZRF_OPT_AUTOACK, 1);
    kw41zrf_set_option(dev, KW41ZRF_OPT_CSMA, 1);

    static const netopt_enable_t ack_req =
        IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_ACK_REQ) ? NETOPT_ENABLE : NETOPT_DISABLE;
    netdev_ieee802154_set(&dev->netdev, NETOPT_ACK_REQ,
                          &ack_req, sizeof(ack_req));

    kw41zrf_abort_sequence(dev);
    kw41zrf_set_sequence(dev, dev->idle_seq);
    kw41zrf_unmask_irqs();

    DEBUG("[kw41zrf] reset radio and set to channel %d.\n",
          KW41ZRF_DEFAULT_CHANNEL);
    return 0;
}
