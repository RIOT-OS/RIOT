/*
 * Copyright (C) 2016 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 *
 * @file
 * @brief       Netdev interface for kw2xrf drivers
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "log.h"
#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev2.h"
#include "net/netdev2/ieee802154.h"

#include "kw2xrf.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_netdev.h"
#include "kw2xrf_getset.h"
#include "kw2xrf_tm.h"
#include "kw2xrf_intern.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_MHR_OVERHEAD           (25)

#define _MACACKWAITDURATION         (864 / 16) /* 864us * 62500Hz */

static uint8_t _send_last_fcf;

static void _irq_handler(void *arg)
{
    netdev2_t *dev = (netdev2_t *) arg;

    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV2_EVENT_ISR);
    }
}

static int _init(netdev2_t *netdev)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;

    /* initialise SPI and GPIOs */
    if (kw2xrf_init(dev, &_irq_handler)) {
        LOG_ERROR("[kw2xrf] unable to initialize device\n");
        return -1;
    }

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif

    /* reset device to default values and put it into RX state */
    kw2xrf_reset_phy(dev);

    return 0;
}

static size_t kw2xrf_tx_load(uint8_t *pkt_buf, uint8_t *buf, size_t len, size_t offset)
{
    for (unsigned i = 0; i < len; i++) {
        pkt_buf[i + offset] = buf[i];
    }
    return offset + len;
}

static void kw2xrf_tx_exec(kw2xrf_t *dev)
{
    if ((dev->netdev.flags & KW2XRF_OPT_AUTOACK) &&
        (_send_last_fcf & IEEE802154_FCF_ACK_REQ)) {
        kw2xrf_set_sequence(dev, XCVSEQ_TX_RX);
    }
    else {
        kw2xrf_set_sequence(dev, XCVSEQ_TRANSMIT);
    }
}

static int _send(netdev2_t *netdev, const struct iovec *vector, unsigned count)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    const struct iovec *ptr = vector;
    uint8_t *pkt_buf = &(dev->buf[1]);
    size_t len = 0;

    /* load packet data into buffer */
    for (unsigned i = 0; i < count; i++, ptr++) {
        /* current packet data + FCS too long */
        if ((len + ptr->iov_len + IEEE802154_FCS_LEN) > KW2XRF_MAX_PKT_LENGTH) {
            LOG_ERROR("[kw2xrf] packet too large (%u byte) to be send\n",
                  (unsigned)len + IEEE802154_FCS_LEN);
            return -EOVERFLOW;
        }
        len = kw2xrf_tx_load(pkt_buf, ptr->iov_base, ptr->iov_len, len);
    }

    /* make sure ongoing t or tr sequenz are finished */
    if (kw2xrf_can_switch_to_idle(dev)) {
        kw2xrf_set_sequence(dev, XCVSEQ_IDLE);
        dev->pending_tx++;
    }
    else {
        /* do not wait, this can lead to a dead lock */
        return 0;
    }

    /*
     * Nbytes = FRAME_LEN - 2 -> FRAME_LEN = Nbytes + 2
     * MKW2xD Reference Manual, P.192
     */
    dev->buf[0] = len + IEEE802154_FCS_LEN;

    /* Help for decision to use T or TR sequenz */
    _send_last_fcf = dev->buf[1];

    kw2xrf_write_fifo(dev, dev->buf, dev->buf[0]);
#ifdef MODULE_NETSTATS_L2
    netdev->stats.tx_bytes += len;
#endif

    /* send data out directly if pre-loading id disabled */
    if (!(dev->netdev.flags & KW2XRF_OPT_PRELOADING)) {
        kw2xrf_tx_exec(dev);
    }

    return (int)len;
}

static int _recv(netdev2_t *netdev, void *buf, size_t len, void *info)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    size_t pkt_len = 0;

    /* get size of the received packet */
    pkt_len = kw2xrf_read_dreg(dev, MKW2XDM_RX_FRM_LEN);

    /* just return length when buf == NULL */
    if (buf == NULL) {
        return pkt_len + 1;
    }

#ifdef MODULE_NETSTATS_L2
    netdev->stats.rx_count++;
    netdev->stats.rx_bytes += pkt_len;
#endif

    if (pkt_len > len) {
        /* not enough space in buf */
        return -ENOBUFS;
    }

    kw2xrf_read_fifo(dev, (uint8_t *)buf, pkt_len + 1);

    if (info != NULL) {
        netdev2_ieee802154_rx_info_t *radio_info = info;
        radio_info->lqi = ((uint8_t*)buf)[pkt_len];
        radio_info->rssi = (uint8_t)kw2xrf_get_rssi(radio_info->lqi);
    }

    /* skip FCS and LQI */
    return pkt_len - 2;
}

static int _set_state(kw2xrf_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_SLEEP:
            kw2xrf_set_power_mode(dev, KW2XRF_DOZE);
            break;
        case NETOPT_STATE_IDLE:
            kw2xrf_set_power_mode(dev, KW2XRF_AUTODOZE);
            kw2xrf_set_sequence(dev, dev->idle_state);
            break;
        case NETOPT_STATE_TX:
            if (dev->netdev.flags & KW2XRF_OPT_PRELOADING) {
                kw2xrf_tx_exec(dev);
            }
            break;
        case NETOPT_STATE_RESET:
            kw2xrf_reset_phy(dev);
            break;
        case NETOPT_STATE_OFF:
            /* TODO: Replace with powerdown (set reset input low) */
            kw2xrf_set_power_mode(dev, KW2XRF_HIBERNATE);
        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static netopt_state_t _get_state(kw2xrf_t *dev)
{
    return dev->state;
}

int _get(netdev2_t *netdev, netopt_t opt, void *value, size_t len)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_MAX_PACKET_SIZE:
            if (len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }

            *((uint16_t *)value) = KW2XRF_MAX_PKT_LENGTH - _MAX_MHR_OVERHEAD;
            return sizeof(uint16_t);

        case NETOPT_STATE:
            if (len < sizeof(netopt_state_t)) {
                return -EOVERFLOW;
            }
            *((netopt_state_t *)value) = _get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_PRELOADING:
            if (dev->netdev.flags & KW2XRF_OPT_PRELOADING) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_PROMISCUOUSMODE:
            if (dev->netdev.flags & KW2XRF_OPT_PROMISCUOUS) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_RX_START_IRQ:
            *((netopt_enable_t *)value) =
                !!(dev->netdev.flags & KW2XRF_OPT_TELL_RX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_RX_END_IRQ:
            *((netopt_enable_t *)value) =
                !!(dev->netdev.flags & KW2XRF_OPT_TELL_RX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_START_IRQ:
            *((netopt_enable_t *)value) =
                !!(dev->netdev.flags & KW2XRF_OPT_TELL_TX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)value) =
                !!(dev->netdev.flags & KW2XRF_OPT_TELL_TX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_AUTOCCA:
            *((netopt_enable_t *)value) =
                !!(dev->netdev.flags & KW2XRF_OPT_AUTOCCA);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_POWER:
            if (len < sizeof(int16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)value) = kw2xrf_get_txpower(dev);
            return sizeof(uint16_t);

        case NETOPT_IS_CHANNEL_CLR:
            if (kw2xrf_cca(dev)) {
                *((netopt_enable_t *)value) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)value) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_CCA_THRESHOLD:
            if (len < sizeof(uint8_t)) {
                return -EOVERFLOW;
            }
            else {
                *(int8_t *)value = kw2xrf_get_cca_threshold(dev);
            }
            return sizeof(int8_t);

        case NETOPT_CCA_MODE:
            if (len < sizeof(uint8_t)) {
                return -EOVERFLOW;
            }
            else {
                *(uint8_t *)value = kw2xrf_get_cca_mode(dev);
                switch (*((int8_t *)value)) {
                    case NETDEV2_IEEE802154_CCA_MODE_1:
                    case NETDEV2_IEEE802154_CCA_MODE_2:
                    case NETDEV2_IEEE802154_CCA_MODE_3:
                        return sizeof(uint8_t);
                    default:
                        break;
                }
                return -EOVERFLOW;
            }
            break;

        case NETOPT_CHANNEL_PAGE:
        default:
            break;
    }

    return netdev2_ieee802154_get((netdev2_ieee802154_t *)netdev, opt, value, len);
}

static int _set(netdev2_t *netdev, netopt_t opt, void *value, size_t len)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            if (len > sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                kw2xrf_set_addr_short(dev, *((uint16_t *)value));
                /* don't set res to set netdev2_ieee802154_t::short_addr */
            }
            break;

        case NETOPT_ADDRESS_LONG:
            if (len > sizeof(uint64_t)) {
                return -EOVERFLOW;
            }
            else {
                kw2xrf_set_addr_long(dev, *((uint64_t *)value));
                /* don't set res to set netdev2_ieee802154_t::short_addr */
            }
            break;

        case NETOPT_NID:
            if (len > sizeof(uint16_t)) {
                return -EOVERFLOW;
            }

            else {
                kw2xrf_set_pan(dev, *((uint16_t *)value));
                /* don't set res to set netdev2_ieee802154_t::pan */
            }
            break;

        case NETOPT_CHANNEL:
            if (len != sizeof(uint16_t)) {
                res = -EINVAL;
            }
            else {
                uint8_t chan = ((uint8_t *)value)[0];
                if (kw2xrf_set_channel(dev, chan)) {
                    res = -EINVAL;
                    break;
                }
                dev->netdev.chan = chan;
                /* don't set res to set netdev2_ieee802154_t::chan */
            }
            break;

        case NETOPT_CHANNEL_PAGE:
            res = -EINVAL;
            break;

        case NETOPT_TX_POWER:
            if (len < sizeof(uint16_t)) {
                res = -EOVERFLOW;
            }
            else {
                kw2xrf_set_tx_power(dev, *(int16_t *)value);
                res = sizeof(uint16_t);
            }
            break;

        case NETOPT_STATE:
            if (len > sizeof(netopt_state_t)) {
                res = -EOVERFLOW;
            }
            else {
                res = _set_state(dev, *((netopt_state_t *)value));
            }
            break;

        case NETOPT_AUTOACK:
            /* Set up HW generated automatic ACK after Receive */
            kw2xrf_set_option(dev, KW2XRF_OPT_AUTOACK,
                              ((bool *)value)[0]);
            break;

        case NETOPT_ACK_REQ:
            kw2xrf_set_option(dev, KW2XRF_OPT_ACK_REQ,
                              ((bool *)value)[0]);
            break;

        case NETOPT_PRELOADING:
            kw2xrf_set_option(dev, KW2XRF_OPT_PRELOADING,
                              ((bool *)value)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_PROMISCUOUSMODE:
            kw2xrf_set_option(dev, KW2XRF_OPT_PROMISCUOUS,
                              ((bool *)value)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_START_IRQ:
            kw2xrf_set_option(dev, KW2XRF_OPT_TELL_RX_START,
                                 ((bool *)value)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_END_IRQ:
            kw2xrf_set_option(dev, KW2XRF_OPT_TELL_RX_END,
                                 ((bool *)value)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_START_IRQ:
            kw2xrf_set_option(dev, KW2XRF_OPT_TELL_TX_START,
                                 ((bool *)value)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_END_IRQ:
            kw2xrf_set_option(dev, KW2XRF_OPT_TELL_TX_END,
                                 ((bool *)value)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_AUTOCCA:
            kw2xrf_set_option(dev, KW2XRF_OPT_AUTOCCA,
                                 ((bool *)value)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CCA_THRESHOLD:
            if (len < sizeof(uint8_t)) {
                res = -EOVERFLOW;
            }
            else {
                kw2xrf_set_cca_threshold(dev, *((int8_t*)value));
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_CCA_MODE:
            if (len < sizeof(uint8_t)) {
                res = -EOVERFLOW;
            }
            else {
                switch (*((int8_t*)value)) {
                    case NETDEV2_IEEE802154_CCA_MODE_1:
                    case NETDEV2_IEEE802154_CCA_MODE_2:
                    case NETDEV2_IEEE802154_CCA_MODE_3:
                        kw2xrf_set_cca_mode(dev, *((int8_t*)value));
                        res = sizeof(uint8_t);
                        break;
                    case NETDEV2_IEEE802154_CCA_MODE_4:
                    case NETDEV2_IEEE802154_CCA_MODE_5:
                    case NETDEV2_IEEE802154_CCA_MODE_6:
                    default:
                        break;
                }
            }
            break;

        case NETOPT_RF_TESTMODE:
#ifdef KW2XRF_TESTMODE
            if (len < sizeof(uint8_t)) {
                res = -EOVERFLOW;
            }
            else {
                kw2xrf_set_test_mode(dev, *((uint8_t *)value));
                res = sizeof(uint8_t);
            }
#endif
            break;

        default:
            break;
    }

    if (res == -ENOTSUP) {
        res = netdev2_ieee802154_set((netdev2_ieee802154_t *)netdev, opt,
                                     value, len);
    }

    return res;
}

static void _isr_event_seq_r(netdev2_t *netdev, uint8_t *dregs)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    uint8_t irqsts1 = 0;

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_RXWTRMRKIRQ) {
        DEBUG("[kw2xrf] got RXWTRMRKIRQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_RXWTRMRKIRQ;
        netdev->event_callback(netdev, NETDEV2_EVENT_RX_STARTED);
    }

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_RXIRQ) {
        DEBUG("[kw2xrf] finished RXSEQ\n");
        dev->state = NETOPT_STATE_RX;
        irqsts1 |= MKW2XDM_IRQSTS1_RXIRQ;
        netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE);
        if (dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_AUTOACK) {
            DEBUG("[kw2xrf]: perform TX ACK\n");
        }
    }

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_TXIRQ) {
        DEBUG("[kw2xrf] finished (ACK) TXSEQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_TXIRQ;
    }

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ) {
        DEBUG("[kw2xrf] SEQIRQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_SEQIRQ;
        kw2xrf_set_idle_sequence(dev);
    }

    kw2xrf_write_dreg(dev, MKW2XDM_IRQSTS1, irqsts1);
    dregs[MKW2XDM_IRQSTS1] &= ~irqsts1;
}

static void _isr_event_seq_t(netdev2_t *netdev, uint8_t *dregs)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    uint8_t irqsts1 = 0;

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_TXIRQ) {
        DEBUG("[kw2xrf] finished TXSEQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_TXIRQ;
    }

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ) {
        DEBUG("[kw2xrf] SEQIRQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_SEQIRQ;

        if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_CCAIRQ) {
            irqsts1 |= MKW2XDM_IRQSTS1_CCAIRQ;
            if (dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_CCA) {
                DEBUG("[kw2xrf] CCA CH busy\n");
                netdev->event_callback(netdev, NETDEV2_EVENT_TX_MEDIUM_BUSY);
            }
            else {
                netdev->event_callback(netdev, NETDEV2_EVENT_TX_COMPLETE);
            }
        }

        assert(dev->pending_tx != 0);
        dev->pending_tx--;
        kw2xrf_set_idle_sequence(dev);
    }

    kw2xrf_write_dreg(dev, MKW2XDM_IRQSTS1, irqsts1);
    dregs[MKW2XDM_IRQSTS1] &= ~irqsts1;
}

/* Standalone CCA */
static void _isr_event_seq_cca(netdev2_t *netdev, uint8_t *dregs)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    uint8_t irqsts1 = 0;

    if ((dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_CCAIRQ) &&
        (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ)) {
        irqsts1 |= MKW2XDM_IRQSTS1_CCAIRQ | MKW2XDM_IRQSTS1_SEQIRQ;
        if (dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_CCA) {
            DEBUG("[kw2xrf] SEQIRQ, CCA CH busy\n");
        }
        else {
            DEBUG("[kw2xrf] SEQIRQ, CCA CH idle\n");
        }
        kw2xrf_set_idle_sequence(dev);
    }
    kw2xrf_write_dreg(dev, MKW2XDM_IRQSTS1, irqsts1);
    dregs[MKW2XDM_IRQSTS1] &= ~irqsts1;
}

static void _isr_event_seq_tr(netdev2_t *netdev, uint8_t *dregs)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    uint8_t irqsts1 = 0;

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_TXIRQ) {
        DEBUG("[kw2xrf] finished TXSEQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_TXIRQ;
        if (dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_RXACKRQD) {
            DEBUG("[kw2xrf] wait for RX ACK\n");
            kw2xrf_seq_timeout_on(dev, _MACACKWAITDURATION);
        }
    }

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_RXWTRMRKIRQ) {
        DEBUG("[kw2xrf] got RXWTRMRKIRQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_RXWTRMRKIRQ;
    }

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_FILTERFAIL_IRQ) {
        DEBUG("[kw2xrf] got FILTERFAILIRQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_FILTERFAIL_IRQ;
    }

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_RXIRQ) {
        DEBUG("[kw2xrf] got RX ACK\n");
        irqsts1 |= MKW2XDM_IRQSTS1_RXIRQ;
    }

    if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ) {
        if (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_CCAIRQ) {
            irqsts1 |= MKW2XDM_IRQSTS1_CCAIRQ;
            if (dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_CCA) {
                DEBUG("[kw2xrf] CCA CH busy\n");
                netdev->event_callback(netdev, NETDEV2_EVENT_TX_MEDIUM_BUSY);
            }
        }

        DEBUG("[kw2xrf] SEQIRQ\n");
        irqsts1 |= MKW2XDM_IRQSTS1_SEQIRQ;
        assert(dev->pending_tx != 0);
        dev->pending_tx--;
        netdev->event_callback(netdev, NETDEV2_EVENT_TX_COMPLETE);
        kw2xrf_seq_timeout_off(dev);
        kw2xrf_set_idle_sequence(dev);
    }
    else if (dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR4IRQ) {
        DEBUG("[kw2xrf] TC4TMOUT, no SEQIRQ, TX failed\n");
        assert(dev->pending_tx != 0);
        dev->pending_tx--;
        netdev->event_callback(netdev, NETDEV2_EVENT_TX_NOACK);
        kw2xrf_seq_timeout_off(dev);
        kw2xrf_set_sequence(dev, dev->idle_state);
    }

    kw2xrf_write_dreg(dev, MKW2XDM_IRQSTS1, irqsts1);
    dregs[MKW2XDM_IRQSTS1] &= ~irqsts1;
}

static void _isr_event_seq_ccca(netdev2_t *netdev, uint8_t *dregs)
{
    kw2xrf_t *dev = (kw2xrf_t *)netdev;
    uint8_t irqsts1 = 0;

    if ((dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_CCAIRQ) &&
        (dregs[MKW2XDM_IRQSTS1] & MKW2XDM_IRQSTS1_SEQIRQ)) {
        irqsts1 |= MKW2XDM_IRQSTS1_CCAIRQ | MKW2XDM_IRQSTS1_SEQIRQ;
        DEBUG("[kw2xrf] CCCA CH idle\n");
        kw2xrf_seq_timeout_off(dev);
        kw2xrf_set_sequence(dev, dev->idle_state);
    }
    else if (dregs[MKW2XDM_IRQSTS3] & MKW2XDM_IRQSTS3_TMR4IRQ) {
        irqsts1 |= MKW2XDM_IRQSTS1_CCAIRQ | MKW2XDM_IRQSTS1_SEQIRQ;
        DEBUG("[kw2xrf] CCCA timeout\n");
        kw2xrf_seq_timeout_off(dev);
        kw2xrf_set_sequence(dev, dev->idle_state);
    }
    kw2xrf_write_dreg(dev, MKW2XDM_IRQSTS1, irqsts1);
    dregs[MKW2XDM_IRQSTS1] &= ~irqsts1;
}

static void _isr(netdev2_t *netdev)
{
    uint8_t dregs[MKW2XDM_PHY_CTRL4 + 1];
    kw2xrf_t *dev = (kw2xrf_t *)netdev;

    kw2xrf_read_dregs(dev, MKW2XDM_IRQSTS1, dregs, MKW2XDM_PHY_CTRL4 + 1);
    kw2xrf_mask_irq_b(dev);

    DEBUG("[kw2xrf] CTRL1 %0x, IRQSTS1 %0x, IRQSTS2 %0x\n",
          dregs[MKW2XDM_PHY_CTRL1], dregs[MKW2XDM_IRQSTS1], dregs[MKW2XDM_IRQSTS2]);

    switch (dregs[MKW2XDM_PHY_CTRL1] & MKW2XDM_PHY_CTRL1_XCVSEQ_MASK) {
        case XCVSEQ_RECEIVE:
            _isr_event_seq_r(netdev, dregs);
            break;

        case XCVSEQ_TRANSMIT:
            _isr_event_seq_t(netdev, dregs);
            break;

        case XCVSEQ_CCA:
            _isr_event_seq_cca(netdev, dregs);
            break;

        case XCVSEQ_TX_RX:
            _isr_event_seq_tr(netdev, dregs);
            break;

        case XCVSEQ_CONTINUOUS_CCA:
            _isr_event_seq_ccca(netdev, dregs);
            break;

        case XCVSEQ_IDLE:
        default:
            DEBUG("[kw2xrf] undefined seq state in isr\n");
            break;
    }

    uint8_t irqsts2 = 0;
    if (dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_PB_ERR_IRQ) {
        DEBUG("[kw2xrf] untreated PB_ERR_IRQ\n");
        irqsts2 |= MKW2XDM_IRQSTS2_PB_ERR_IRQ;
    }
    if (dregs[MKW2XDM_IRQSTS2] & MKW2XDM_IRQSTS2_WAKE_IRQ) {
        DEBUG("[kw2xrf] untreated WAKE_IRQ\n");
        irqsts2 |= MKW2XDM_IRQSTS2_WAKE_IRQ;
    }
    kw2xrf_write_dreg(dev, MKW2XDM_IRQSTS2, irqsts2);

    if (ENABLE_DEBUG) {
        /* for debugging only */
        kw2xrf_read_dregs(dev, MKW2XDM_IRQSTS1, dregs, MKW2XDM_IRQSTS1 + 3);
        if (dregs[MKW2XDM_IRQSTS1] & 0x7f) {
            DEBUG("[kw2xrf] IRQSTS1 contains untreated IRQs: 0x%02x\n",
                dregs[MKW2XDM_IRQSTS1]);
        }
        if (dregs[MKW2XDM_IRQSTS2] & 0x02) {
            DEBUG("[kw2xrf] IRQSTS2 contains untreated IRQs: 0x%02x\n",
                dregs[MKW2XDM_IRQSTS2]);
        }
        if (dregs[MKW2XDM_IRQSTS3] & 0x0f) {
            DEBUG("[kw2xrf] IRQSTS3 contains untreated IRQs: 0x%02x\n",
                dregs[MKW2XDM_IRQSTS3]);
        }
    }

    kw2xrf_enable_irq_b(dev);
}

const netdev2_driver_t kw2xrf_driver = {
    .init = _init,
    .send = _send,
    .recv = _recv,
    .get = _get,
    .set = _set,
    .isr = _isr,
};

/** @} */
