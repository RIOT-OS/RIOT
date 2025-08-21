/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       IEEE 802.15.4 Radio HAL implementation for the KW2x RF driver
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>

#include "net/gnrc.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"
#include "kw2xrf.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_getset.h"
#include "kw2xrf_intern.h"
#include "net/ieee802154/radio.h"
#include "kw2xrf_params.h"
#include "event/thread.h"
#include "xtimer.h"

static void _clr_irq_flags(void)
{
    uint32_t irqsts = ZLL->IRQSTS;
    ZLL->IRQSTS = irqsts;
}

static const ieee802154_radio_ops_t kw2xrf_ops;

static void _start_tx(kw2xrf_t *kw_dev)
{
    /* Check whether it should do TX/RX or just TX */
    if (kw_dev->ack_requested) {
        kw41zrf_set_sequence(dev, XCVSEQ_TX_RX | ZLL_PHY_CTRL_TC3TMOUT_MASK);
    }
    else {

        kw41zrf_set_sequence(dev, XCVSEQ_TRANSMIT | ZLL_PHY_CTRL_TC3TMOUT_MASK);
    }
}

/* Common CCA check handler code for sequences Transmit and Transmit/Receive */
static uint32_t _isr_event_seq_t_ccairq(kw41zrf_t *dev, uint32_t irqsts)
{
    uint32_t handled_irqs = 0;
    if (irqsts & ZLL_IRQSTS_TMR2IRQ_MASK) {
        assert(!(irqsts & ZLL_IRQSTS_CCAIRQ_MASK));
        handled_irqs |= ZLL_IRQSTS_TMR2IRQ_MASK;

        dev->tx_timeout = 0;
        KW41ZRF_LED_TX_ON;

        if (dev->tx_timeout > 0) {
            /* Set real timeout for RX ACK */
            kw41zrf_timer_set(dev, &ZLL->T3CMP, dev->tx_timeout);
        }

        /* disable TMR2 match */
        bit_clear32(&ZLL->PHY_CTRL, ZLL_PHY_CTRL_TMR2CMP_EN_SHIFT);
        /* Start CCA */
    }
    if (irqsts & ZLL_IRQSTS_CCAIRQ_MASK) {
        /* CCA before TX has completed */
        handled_irqs |= ZLL_IRQSTS_CCAIRQ_MASK;
        if (irqsts & ZLL_IRQSTS_CCA_MASK) {
            /* Channel was determined busy */
            if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
                handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
                kw41zrf_abort_sequence(dev);
                KW41ZRF_LED_TX_OFF;
            }

            LOG_DEBUG("[kw41zrf] CCA ch busy (RSSI: %d retry: %u)\n",
                  (int8_t)((ZLL->LQI_AND_RSSI & ZLL_LQI_AND_RSSI_CCA1_ED_FNL_MASK) >>
                ZLL_LQI_AND_RSSI_CCA1_ED_FNL_SHIFT),
                dev->csma_num_backoffs
            );

            if (dev->csma_num_backoffs < dev->csma_max_backoffs) {
                /* Perform CSMA/CA backoff algorithm */
                ++dev->csma_num_backoffs;
                if (dev->csma_be < dev->csma_max_be) {
                    /* Increase delay exponent */
                    ++dev->csma_be;
                }
                /* Resubmit the frame for transmission */
                kw41zrf_tx_exec(dev);
                return handled_irqs;
            }

            /* If we get here we've used up the csma retries and we're done */
            kw41zrf_abort_sequence(dev);
            kw41zrf_set_sequence(dev, dev->idle_seq);

            if (dev->netdev.netdev.event_callback) {
                dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);
                LOG_INFO("[kw41zrf] dropping frame after %u backoffs\n",
                          dev->csma_num_backoffs);
            }

        }
        else {
            /* Channel is idle */
            DEBUG("[kw41zrf] CCA ch idle (RSSI: %d retries: %u)\n",
                  (int8_t)((ZLL->LQI_AND_RSSI & ZLL_LQI_AND_RSSI_CCA1_ED_FNL_MASK) >>
                  ZLL_LQI_AND_RSSI_CCA1_ED_FNL_SHIFT),
                  dev->csma_num_backoffs
            );
            if (dev->netdev.netdev.event_callback) {
                /* TX will start automatically after CCA check succeeded */
                dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_TX_STARTED);
            }
        }
    }
    return handled_irqs;
}

/* Handler for Receive sequence */
static uint32_t _isr_event_seq_r(kw41zrf_t *dev, uint32_t irqsts)
{
    uint32_t handled_irqs = 0;

    if (irqsts & ZLL_IRQSTS_RXWTRMRKIRQ_MASK) {
        DEBUG("[kw41zrf] RXWTRMRKIRQ (R)\n");
        handled_irqs |= ZLL_IRQSTS_RXWTRMRKIRQ_MASK;
        if (dev->netdev.netdev.event_callback) {
            dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_RX_STARTED);
        }
    }

    if (irqsts & ZLL_IRQSTS_FILTERFAIL_IRQ_MASK) {
        KW41ZRF_LED_RX_OFF;
        DEBUG("[kw41zrf] FILTERFAILIRQ: %04"PRIx32"\n", ZLL->FILTERFAIL_CODE);
        handled_irqs |= ZLL_IRQSTS_FILTERFAIL_IRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_RXIRQ_MASK) {
        KW41ZRF_LED_RX_ON;
        DEBUG("[kw41zrf] finished RX\n");
        handled_irqs |= ZLL_IRQSTS_RXIRQ_MASK;
        DEBUG("[kw41zrf] RX len: %3u\n",
            (unsigned int)((ZLL->IRQSTS & ZLL_IRQSTS_RX_FRAME_LENGTH_MASK) >>
            ZLL_IRQSTS_RX_FRAME_LENGTH_SHIFT));
        if (ZLL->PHY_CTRL & ZLL_PHY_CTRL_AUTOACK_MASK) {
            KW41ZRF_LED_TX_ON;
            DEBUG("[kw41zrf] perform TXACK\n");
        }
    }

    if (irqsts & ZLL_IRQSTS_TXIRQ_MASK) {
        KW41ZRF_LED_TX_OFF;
        DEBUG("[kw41zrf] finished TXACK\n");
        handled_irqs |= ZLL_IRQSTS_TXIRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        uint32_t seq_ctrl_sts = ZLL->SEQ_CTRL_STS;
        kw41zrf_abort_sequence(dev);

        DEBUG("[kw41zrf] SEQIRQ (R)\n");
        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        KW41ZRF_LED_TX_OFF;
        if ((irqsts & ZLL_IRQSTS_CRCVALID_MASK) == 0) {
            DEBUG("[kw41zrf] CRC failure (R)\n");
        }
        else if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_TC3_ABORTED_MASK) {
            DEBUG("[kw41zrf] RX timeout (R)\n");
        }
        else if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_PLL_ABORTED_MASK) {
            LOG_ERROR("[kw41zrf] PLL unlock (R)\n");
        }
        else if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_SW_ABORTED_MASK) {
            LOG_ERROR("[kw41zrf] SW abort (R)\n");
        }
        else {
            /* No error reported */
            DEBUG("[kw41zrf] success (R)\n");
            /* Block XCVSEQ_RECEIVE until netdev->recv has been called */
            dev->recv_blocked = 1;
            kw41zrf_set_sequence(dev, dev->idle_seq);
            if (dev->netdev.netdev.event_callback) {
                dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_RX_COMPLETE);
            }
            return handled_irqs;
        }

        kw41zrf_set_sequence(dev, dev->idle_seq);
    }

    return handled_irqs;
}

/* Handler for Transmit sequence */
static uint32_t _isr_event_seq_t(kw41zrf_t *dev, uint32_t irqsts)
{
    uint32_t handled_irqs = 0;
    if (irqsts & ZLL_IRQSTS_TXIRQ_MASK) {
        DEBUG("[kw41zrf] finished TX (T)\n");
        handled_irqs |= ZLL_IRQSTS_TXIRQ_MASK;
    }
    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        /* Finished T sequence */
        kw41zrf_abort_sequence(dev);
        /* Go back to being idle */
        kw41zrf_set_sequence(dev, dev->idle_seq);

        DEBUG("[kw41zrf] SEQIRQ (T)\n");
        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        if (dev->netdev.netdev.event_callback) {
            dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_TX_COMPLETE);
        }
        KW41ZRF_LED_TX_OFF;
    }

    return handled_irqs;
}

/* Handler for standalone CCA */
static uint32_t _isr_event_seq_cca(kw41zrf_t *dev, uint32_t irqsts)
{
    uint32_t handled_irqs = 0;

    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        /* Finished CCA sequence */
        kw41zrf_abort_sequence(dev);
        kw41zrf_set_sequence(dev, dev->idle_seq);

        DEBUG("[kw41zrf] SEQIRQ (C)\n");
        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        if (irqsts & ZLL_IRQSTS_CCA_MASK) {
            DEBUG("[kw41zrf] CCA ch busy\n");
            dev->cca_result = 1;
        }
        else {
            DEBUG("[kw41zrf] CCA ch idle\n");
            dev->cca_result = 0;
        }
    }

    return handled_irqs;
}

/* Handler for Transmit/Receive sequence */
static uint32_t _isr_event_seq_tr(kw41zrf_t *dev, uint32_t irqsts)
{
    uint32_t handled_irqs = 0;
    if (irqsts & ZLL_IRQSTS_TXIRQ_MASK) {
        KW41ZRF_LED_RX_ON;
        DEBUG("[kw41zrf] finished TX (TR)\n");
        handled_irqs |= ZLL_IRQSTS_TXIRQ_MASK;
        DEBUG("[kw41zrf] wait for RX ACK\n");
    }

    if (irqsts & ZLL_IRQSTS_RXIRQ_MASK) {
        KW41ZRF_LED_RX_OFF;
        DEBUG("[kw41zrf] got RX ACK\n");
        handled_irqs |= ZLL_IRQSTS_RXIRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_FILTERFAIL_IRQ_MASK) {
        DEBUG("[kw41zrf] FILTERFAILIRQ (TR): %04"PRIx32"\n", ZLL->FILTERFAIL_CODE);
        handled_irqs |= ZLL_IRQSTS_FILTERFAIL_IRQ_MASK;
    }

    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        uint32_t seq_ctrl_sts = ZLL->SEQ_CTRL_STS;
        kw41zrf_abort_sequence(dev);
        DEBUG("[kw41zrf] SEQIRQ (TR)\n");

        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        KW41ZRF_LED_TX_OFF;
        KW41ZRF_LED_RX_OFF;
        if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_TC3_ABORTED_MASK) {
            if (dev->num_retrans < dev->max_retrans) {
                /* Perform frame retransmission */
                ++dev->num_retrans;
                DEBUG("[kw41zrf] TX retry %u\n", (unsigned)dev->num_retrans);
                /* Reset CSMA counters for backoff handling */
                dev->csma_be = dev->csma_min_be;
                dev->csma_num_backoffs = 0;
                /* Resubmit the frame for transmission */
                kw41zrf_tx_exec(dev);
                return handled_irqs;
            }
        }

        assert(!kw41zrf_is_dsm());
        kw41zrf_set_sequence(dev, dev->idle_seq);

        if (dev->netdev.netdev.event_callback) {
            if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_TC3_ABORTED_MASK) {
                LOG_DEBUG("[kw41zrf] RXACK timeout (TR)\n");
                dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_TX_NOACK);
            }
            else if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_PLL_ABORTED_MASK) {
                LOG_ERROR("[kw41zrf] PLL unlock (TR)\n");
                /* TODO: there is no other error event for TX failures */
                dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);
                /* if this does happen in development, it's worth checking why */
                assert(0);
            }
            else if (seq_ctrl_sts & ZLL_SEQ_CTRL_STS_SW_ABORTED_MASK) {
                DEBUG("[kw41zrf] SW abort (TR)\n");
                /* TODO: there is no other error event for TX failures */
                dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);
            }
            else {
                /* No error reported */
                DEBUG("[kw41zrf] TX success (TR)\n");
                dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_TX_COMPLETE);
            }
        }
    }

    return handled_irqs;
}

/* Handler for Continuous CCA */
static uint32_t _isr_event_seq_ccca(kw41zrf_t *dev, uint32_t irqsts)
{
    uint32_t handled_irqs = 0;
    if (irqsts & ZLL_IRQSTS_SEQIRQ_MASK) {
        DEBUG("[kw41zrf] SEQIRQ (CCCA)\n");
        handled_irqs |= ZLL_IRQSTS_SEQIRQ_MASK;
        if (irqsts & ZLL_IRQSTS_CCA_MASK) {
            DEBUG("[kw41zrf] CCCA ch busy\n");
        }
        else {
            DEBUG("[kw41zrf] CCCA ch idle\n");
        }
        kw41zrf_abort_sequence(dev);
        kw41zrf_set_sequence(dev, dev->idle_seq);
    }

    return handled_irqs;
}

void kw41z_radio_hal_irq_handler(void *arg)
{
    ieee802154_dev_t *dev = arg;
    kw41zrf_t *dev = dev->priv;

    irq_is_queued = false;

    /* ZLL register access requires that the transceiver is not in deep sleep mode */
    if (kw41zrf_is_dsm()) {
        /* Transceiver is sleeping, the IRQ must have occurred before entering
         * sleep, discard the call */
        DEBUG("kw41zrf: unexpected IRQ while sleeping\n");
        kw41zrf_unmask_irqs();
        return;
    }

    uint32_t irqsts = ZLL->IRQSTS;
    /* Clear all IRQ flags now */
    ZLL->IRQSTS = irqsts;

    uint32_t handled_irqs = 0;
    DEBUG("[kw41zrf] CTRL %08" PRIx32 ", IRQSTS %08" PRIx32 ", FILTERFAIL %08" PRIx32 "\n",
          ZLL->PHY_CTRL, irqsts, ZLL->FILTERFAIL_CODE);

    uint8_t seq = (ZLL->PHY_CTRL & ZLL_PHY_CTRL_XCVSEQ_MASK)
                  >> ZLL_PHY_CTRL_XCVSEQ_SHIFT;

    switch (seq) {
        case XCVSEQ_RECEIVE:
            handled_irqs |= _isr_event_seq_r(dev, irqsts);
            break;

        case XCVSEQ_IDLE: /* XCVSEQ is idle during csma backoff */
        case XCVSEQ_TRANSMIT:
            /* First check CCA flags */
            handled_irqs |= _isr_event_seq_t_ccairq(dev, irqsts);
            /* Then TX flags */
            handled_irqs |= _isr_event_seq_t(dev, irqsts & ~handled_irqs);
            break;

        case XCVSEQ_CCA:
            handled_irqs |= _isr_event_seq_cca(dev, irqsts);
            break;

        case XCVSEQ_TX_RX:
            /* First check CCA flags */
            handled_irqs |= _isr_event_seq_t_ccairq(dev, irqsts);
            /* Then TX/RX flags */
            handled_irqs |= _isr_event_seq_tr(dev, irqsts & ~handled_irqs);
            break;

        case XCVSEQ_CONTINUOUS_CCA:
            handled_irqs |= _isr_event_seq_ccca(dev, irqsts);
            break;

        default:
            assert(0);
            break;
    }

    irqsts &= ~handled_irqs;

    /* doesn't need handling; just prevent outputting an error below */
    irqsts &= ~ZLL_IRQSTS_RXWTRMRKIRQ_MASK;

    if (irqsts & 0x000f017ful) {
        LOG_ERROR("[kw41zrf] Unhandled IRQs: 0x%08lx\n", (irqsts & 0x000f017ful));
    }

    kw41zrf_unmask_irqs();
}
}

static int _write(ieee802154_dev_t *dev, const iolist_t *iolist)
{
    kw2xrf_t *kw_dev = dev->priv;

    /* get length */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        /* current packet data + FCS too long */
        len = kw41zrf_tx_load(iol->iol_base, iol->iol_len, len);
    }

    *((volatile uint8_t *)&ZLL->PKT_BUFFER_TX[0]) = len + IEEE802154_FCS_LEN;

    DEBUG("[kw41zrf] TX %u bytes\n", len);
    return 0;
}

static int _request_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    kw2xrf_t *kw_dev = dev->priv;
    int res = -EINVAL;
    (void) ctx;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        kw_dev->tx_done = false;
        if (kw_dev->cca_before_tx) {
            kw_dev->tx_cca_pending = true;
            kw41zrf_set_sequence(kw_dev, XCVSEQ_CCA);
        } else {
            _start_tx(kw_dev);
        }
        kw2xrf_enable_irq_b(kw_dev);
        break;
    case IEEE802154_HAL_OP_SET_RX:
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        ZLL->PHY_CTRL = (ZLL->PHY_CTRL &
            ~(ZLL_PHY_CTRL_XCVSEQ_MASK |
            ZLL_PHY_CTRL_TC3TMOUT_MASK | ZLL_PHY_CTRL_TMRTRIGEN_MASK)) |
            ZLL_PHY_CTRL_XCVSEQ(XCVSEQ_IDLE) | ZLL_PHY_CTRL_SEQMSK_MASK;
        res = 0;
        break;
    case IEEE802154_HAL_OP_CCA:
        break;
    default:
        assert(false);
    }

    return res;
}

static int _confirm_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    kw2xrf_t *kw_dev = dev->priv;
    int res = -EAGAIN;
    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        break;
    case IEEE802154_HAL_OP_SET_RX:
        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        if (ZLL->SEQ_CTRL_STS & ZLL_SEQ_CTRL_STS_SEQ_IDLE_MASK) {
            res = 0;
            _clr_irq_flags();
        }
        break;
    case IEEE802154_HAL_OP_CCA:
        break;
    default:
        assert(false);
    }

    return res;
}

static int _len(ieee802154_dev_t *dev)
{
    kw2xrf_t *kw_dev = dev->priv;
    size_t pkt_len = kw2xrf_read_dreg(kw_dev, MKW2XDM_RX_FRM_LEN) -
                     IEEE802154_FCS_LEN;
    return pkt_len;
}

static int _read(ieee802154_dev_t *dev, void *buf, size_t size, ieee802154_rx_info_t *info)
{
    kw2xrf_t *kw_dev = dev->priv;

    size_t rxlen =  _len(dev);
    if (!buf) {
        return 0;
    }
    rxlen = size < rxlen ? size : rxlen;
    kw2xrf_read_fifo(kw_dev, (uint8_t *)buf, rxlen);

    if (info != NULL) {
        info->lqi = kw2xrf_read_dreg(kw_dev, MKW2XDM_LQI_VALUE);
        info->rssi = kw2xrf_get_rssi(info->lqi);
    }

    return rxlen;
}

static int _set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    kw2xrf_t *kw_dev = dev->priv;
    /* normalize to absolute value */
    if (threshold < 0) {
        threshold = -threshold;
    }

    kw2xrf_write_iregs(kw_dev, MKW2XDMI_CCA1_THRESH, (uint8_t*)&threshold, 1);
    kw2xrf_write_iregs(kw_dev, MKW2XDMI_CCA2_THRESH, (uint8_t*)&threshold, 1);

    return 0;
}

/* PLL integer and fractional lookup tables
 *
 * Fc = 2405 + 5(k - 11) , k = 11,12,...,26
 *
 * Equation for PLL frequency, MKW2xD Reference Manual, p.255 :
 * F = ((PLL_INT0 + 64) + (PLL_FRAC0/65536))32MHz
 *
 */
static const uint8_t pll_int_lt[16] = {
    11, 11, 11, 11,
    11, 11, 12, 12,
    12, 12, 12, 12,
    13, 13, 13, 13
};

static const uint16_t pll_frac_lt[16] = {
    10240, 20480, 30720, 40960,
    51200, 61440, 6144, 16384,
    26624, 36864, 47104, 57344,
    2048, 12288, 22528, 32768
};

static int _config_phy(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf)
{
    kw2xrf_t *kw_dev = dev->priv;
    kw2xrf_set_tx_power(kw_dev, conf->pow);
    uint8_t tmp = conf->channel - 11;
    kw2xrf_write_dreg(kw_dev, MKW2XDM_PLL_INT0, MKW2XDM_PLL_INT0_VAL(pll_int_lt[tmp]));
    kw2xrf_write_dreg(kw_dev, MKW2XDM_PLL_FRAC0_LSB, (uint8_t)pll_frac_lt[tmp]);
    kw2xrf_write_dreg(kw_dev, MKW2XDM_PLL_FRAC0_MSB, (uint8_t)(pll_frac_lt[tmp] >> 8));
    return 0;
}

static int _off(ieee802154_dev_t *dev)
{
    kw2xrf_t *kw_dev = dev->priv;
    /* TODO: check if power down via RST_B is possible  */
    kw2xrf_set_power_mode(kw_dev, KW2XRF_HIBERNATE);
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value)
{
    const uint16_t *pan_id = value;
    const network_uint16_t *short_addr = value;
    const eui64_t *ext_addr = value;
    kw2xrf_t *kw_dev = dev->priv;
    switch(cmd) {
        case IEEE802154_AF_SHORT_ADDR:
            kw2xrf_set_addr_short(kw_dev, byteorder_ntohs(*short_addr));
            break;
        case IEEE802154_AF_EXT_ADDR:
            kw2xrf_set_addr_long(kw_dev, ext_addr->uint64.u64);
            break;
        case IEEE802154_AF_PANID:
            kw2xrf_set_pan(kw_dev, *pan_id);
            break;
        case IEEE802154_AF_PAN_COORD:
            return -ENOTSUP;
    }

    return 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    kw2xrf_t *kw_dev = dev->priv;
    /* enable xtal and put power management controller to high power mode */
    kw2xrf_set_power_mode(kw_dev, KW2XRF_IDLE);
    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    kw2xrf_t *kw_dev = dev->priv;
    size_t pwr_modes = kw2xrf_read_dreg(kw_dev, MKW2XDM_PWR_MODES);
    return (pwr_modes & MKW2XDM_PWR_MODES_XTAL_READY) ? 0 : -EAGAIN;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    kw2xrf_t *kw_dev = dev->priv;

    uint8_t cca_ctl_val = 0;
    uint8_t dev_mode = 0;
    switch (mode) {
        case IEEE802154_CCA_MODE_ED_THRESHOLD:
            dev_mode = 1;
            break;
        case IEEE802154_CCA_MODE_CARRIER_SENSING:
            dev_mode = 2;
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_AND_CS:
            dev_mode = 3;
            kw2xrf_read_iregs(kw_dev, MKW2XDMI_CCA_CTRL, &cca_ctl_val, 1);
            cca_ctl_val |= MKW2XDMI_CCA_CTRL_CCA3_AND_NOT_OR;
            kw2xrf_write_iregs(kw_dev, MKW2XDMI_CCA_CTRL, &cca_ctl_val, 1);
            break;
        case IEEE802154_CCA_MODE_ED_THRESH_OR_CS:
            dev_mode = 3;
            kw2xrf_read_iregs(kw_dev, MKW2XDMI_CCA_CTRL, &cca_ctl_val, 1);
            cca_ctl_val &= ~(MKW2XDMI_CCA_CTRL_CCA3_AND_NOT_OR);
            kw2xrf_write_iregs(kw_dev, MKW2XDMI_CCA_CTRL, &cca_ctl_val, 1);
            break;
    }

    kw2xrf_set_cca_mode(kw_dev, dev_mode);
    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *dev, ieee802154_src_match_t cmd, const void *value)
{
    kw2xrf_t *kw_dev = dev->priv;
    switch(cmd) {
        case IEEE802154_SRC_MATCH_EN: {

            const bool en = *((const bool*) value);
            if (en) {
                kw2xrf_set_dreg_bit(kw_dev, MKW2XDM_SRC_CTRL,
                                        MKW2XDM_SRC_CTRL_ACK_FRM_PND);
            }
            else {
                kw2xrf_clear_dreg_bit(kw_dev, MKW2XDM_SRC_CTRL,
                                          MKW2XDM_SRC_CTRL_ACK_FRM_PND);
            }
            break;
        }
        default:
            return -ENOTSUP;
    }
    return 0;
}

static int _set_frame_filter_mode(ieee802154_dev_t *dev, ieee802154_filter_mode_t mode)
{
    kw2xrf_t *kw_dev = dev->priv;

    bool _promisc = false;

    switch (mode) {
        case IEEE802154_FILTER_ACCEPT:
            break;
        case IEEE802154_FILTER_PROMISC:
            _promisc = true;
            break;
        case IEEE802154_FILTER_ACK_ONLY:
            /* Not implemented */
            break;
        default:
            return -ENOTSUP;
    }

    if (_promisc) {
        kw2xrf_set_dreg_bit(kw_dev, MKW2XDM_PHY_CTRL4,
                            MKW2XDM_PHY_CTRL4_PROMISCUOUS);
    }
    else {
        kw2xrf_clear_dreg_bit(kw_dev, MKW2XDM_PHY_CTRL4,
                            MKW2XDM_PHY_CTRL4_PROMISCUOUS);
    }

    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd, int8_t retries)
{
    kw2xrf_t *kw_dev = dev->priv;
    (void) bd;

    if (retries < 0) {
        kw_dev->cca_before_tx = false;
        return 0;
    } else if (retries == 0) {
        kw_dev->cca_before_tx = true;
        return 0;
    }

    return -EINVAL;
}

static const ieee802154_radio_ops_t kw41z_ops = {
    .caps =  IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_IRQ_CRC_ERROR
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_CCA_DONE
          | IEEE802154_CAP_IRQ_ACK_TIMEOUT
          | IEEE802154_CAP_PHY_OQPSK,
    .write = _write,
    .read = _read,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .len = _len,
    .off = _off,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .set_csma_params = _set_csma_params,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_frame_filter_mode = _set_frame_filter_mode,
};
