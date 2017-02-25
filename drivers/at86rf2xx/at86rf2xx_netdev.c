/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Netdev adaption for the AT86RF2xx drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev2.h"
#include "net/netdev2/ieee802154.h"

#include "at86rf2xx.h"
#include "at86rf2xx_netdev.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_MHR_OVERHEAD   (25)

static int _send(netdev2_t *netdev, const struct iovec *vector, unsigned count);
static int _recv(netdev2_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev2_t *netdev);
static void _isr(netdev2_t *netdev);
static int _get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev2_t *netdev, netopt_t opt, void *val, size_t len);

const netdev2_driver_t at86rf2xx_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void _irq_handler(void *arg)
{
    netdev2_t *dev = (netdev2_t *) arg;

    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV2_EVENT_ISR);
    }
}

static int _init(netdev2_t *netdev)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;

    /* initialise GPIOs */
    spi_init_cs(dev->params.spi, dev->params.cs_pin);
    gpio_init(dev->params.sleep_pin, GPIO_OUT);
    gpio_clear(dev->params.sleep_pin);
    gpio_init(dev->params.reset_pin, GPIO_OUT);
    gpio_set(dev->params.reset_pin);
    gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, _irq_handler, dev);

    /* make sure device is not sleeping, so we can query part number */
    at86rf2xx_assert_awake(dev);

    /* test if the SPI is set up correctly and the device is responding */
    if (at86rf2xx_reg_read(dev, AT86RF2XX_REG__PART_NUM) !=
        AT86RF2XX_PARTNUM) {
        DEBUG("[at86rf2xx] error: unable to read correct part number\n");
        return -1;
    }

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif
    /* reset device to default values and put it into RX state */
    at86rf2xx_reset(dev);

    return 0;
}

static int _send(netdev2_t *netdev, const struct iovec *vector, unsigned count)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;
    const struct iovec *ptr = vector;
    size_t len = 0;

    at86rf2xx_tx_prepare(dev);

    /* load packet data into FIFO */
    for (unsigned i = 0; i < count; i++, ptr++) {
        /* current packet data + FCS too long */
        if ((len + ptr->iov_len + 2) > AT86RF2XX_MAX_PKT_LENGTH) {
            DEBUG("[at86rf2xx] error: packet too large (%u byte) to be send\n",
                  (unsigned)len + 2);
            return -EOVERFLOW;
        }
#ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_bytes += len;
#endif
        len = at86rf2xx_tx_load(dev, ptr->iov_base, ptr->iov_len, len);
    }

    /* send data out directly if pre-loading id disabled */
    if (!(dev->netdev.flags & AT86RF2XX_OPT_PRELOADING)) {
        at86rf2xx_tx_exec(dev);
    }
    /* return the number of bytes that were actually send out */
    return (int)len;
}

static int _recv(netdev2_t *netdev, void *buf, size_t len, void *info)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;
    uint8_t phr;
    size_t pkt_len;

    /* frame buffer protection will be unlocked as soon as at86rf2xx_fb_stop()
     * is called*/
    at86rf2xx_fb_start(dev);

    /* get the size of the received packet */
    at86rf2xx_fb_read(dev, &phr, 1);

    /* ignore MSB (refer p.80) and substract length of FCS field */
    pkt_len = (phr & 0x7f) - 2;

    /* just return length when buf == NULL */
    if (buf == NULL) {
        at86rf2xx_fb_stop(dev);
        return pkt_len;
    }
    /* not enough space in buf */
    if (pkt_len > len) {
        at86rf2xx_fb_stop(dev);
        return -ENOBUFS;
    }
    #ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += pkt_len;
    #endif
    /* copy payload */
    at86rf2xx_fb_read(dev, (uint8_t *)buf, pkt_len);

    /* Ignore FCS but advance fb read - we must give a temporary buffer here,
     * as we are not allowed to issue SPI transfers without any buffer */
    uint8_t tmp[2];
    at86rf2xx_fb_read(dev, tmp, 2);
    (void)tmp;

    if (info != NULL) {
        netdev2_ieee802154_rx_info_t *radio_info = info;
        at86rf2xx_fb_read(dev, &(radio_info->lqi), 1);
#ifndef MODULE_AT86RF231
        at86rf2xx_fb_read(dev, &(radio_info->rssi), 1);
        at86rf2xx_fb_stop(dev);
#else
        at86rf2xx_fb_stop(dev);
        radio_info->rssi = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_ED_LEVEL);
#endif
    }
    else {
        at86rf2xx_fb_stop(dev);
    }

    return pkt_len;
}

static int _set_state(at86rf2xx_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_SLEEP:
            at86rf2xx_set_state(dev, AT86RF2XX_STATE_SLEEP);
            break;
        case NETOPT_STATE_IDLE:
            at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);
            break;
        case NETOPT_STATE_TX:
            if (dev->netdev.flags & AT86RF2XX_OPT_PRELOADING) {
                at86rf2xx_tx_exec(dev);
            }
            break;
        case NETOPT_STATE_RESET:
            at86rf2xx_reset(dev);
            break;
        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

netopt_state_t _get_state(at86rf2xx_t *dev)
{
    switch (at86rf2xx_get_status(dev)) {
        case AT86RF2XX_STATE_SLEEP:
            return NETOPT_STATE_SLEEP;
        case AT86RF2XX_STATE_BUSY_RX_AACK:
            return NETOPT_STATE_RX;
        case AT86RF2XX_STATE_BUSY_TX_ARET:
        case AT86RF2XX_STATE_TX_ARET_ON:
            return NETOPT_STATE_TX;
        case AT86RF2XX_STATE_RX_AACK_ON:
        default:
            return NETOPT_STATE_IDLE;
    }
}

static int _get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    at86rf2xx_t *dev = (at86rf2xx_t *) netdev;

    if (netdev == NULL) {
        return -ENODEV;
    }

    /* getting these options doesn't require the transceiver to be responsive */
    switch (opt) {
        case NETOPT_CHANNEL_PAGE:
            assert(max_len >= sizeof(uint16_t));
            ((uint8_t *)val)[1] = 0;
            ((uint8_t *)val)[0] = at86rf2xx_get_page(dev);
            return sizeof(uint16_t);

        case NETOPT_MAX_PACKET_SIZE:
            assert(max_len >= sizeof(int16_t));
            *((uint16_t *)val) = AT86RF2XX_MAX_PKT_LENGTH - _MAX_MHR_OVERHEAD;
            return sizeof(uint16_t);

        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = _get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_PRELOADING:
            if (dev->netdev.flags & AT86RF2XX_OPT_PRELOADING) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_PROMISCUOUSMODE:
            if (dev->netdev.flags & AT86RF2XX_OPT_PROMISCUOUS) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_RX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & AT86RF2XX_OPT_TELL_RX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_RX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & AT86RF2XX_OPT_TELL_RX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & AT86RF2XX_OPT_TELL_TX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & AT86RF2XX_OPT_TELL_TX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_CSMA:
            *((netopt_enable_t *)val) =
                !!(dev->netdev.flags & AT86RF2XX_OPT_CSMA);
            return sizeof(netopt_enable_t);

        default:
            /* Can still be handled in second switch */
            break;
    }

    int res;

    if (((res = netdev2_ieee802154_get((netdev2_ieee802154_t *)netdev, opt, val,
                                       max_len)) >= 0) || (res != -ENOTSUP)) {
        return res;
    }

    uint8_t old_state = at86rf2xx_get_status(dev);
    res = 0;

    /* temporarily wake up if sleeping */
    if (old_state == AT86RF2XX_STATE_SLEEP) {
        at86rf2xx_assert_awake(dev);
    }

    /* these options require the transceiver to be not sleeping*/
    switch (opt) {
        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *((uint16_t *)val) = at86rf2xx_get_txpower(dev);
            res = sizeof(uint16_t);
            break;

        case NETOPT_RETRANS:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = at86rf2xx_get_max_retries(dev);
            res = sizeof(uint8_t);
            break;

        case NETOPT_CSMA_RETRIES:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = at86rf2xx_get_csma_max_retries(dev);
            res = sizeof(uint8_t);
            break;

        case NETOPT_CCA_THRESHOLD:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf2xx_get_cca_threshold(dev);
            res = sizeof(int8_t);
            break;

        default:
            res = -ENOTSUP;
    }

    /* go back to sleep if were sleeping */
    if (old_state == AT86RF2XX_STATE_SLEEP) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_SLEEP);
    }

    return res;
}

static int _set(netdev2_t *netdev, netopt_t opt, void *val, size_t len)
{
    at86rf2xx_t *dev = (at86rf2xx_t *) netdev;
    uint8_t old_state = at86rf2xx_get_status(dev);
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* temporarily wake up if sleeping */
    if (old_state == AT86RF2XX_STATE_SLEEP) {
        at86rf2xx_assert_awake(dev);
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(len <= sizeof(uint16_t));
            at86rf2xx_set_addr_short(dev, *((uint16_t *)val));
            /* don't set res to set netdev2_ieee802154_t::short_addr */
            break;
        case NETOPT_ADDRESS_LONG:
            assert(len <= sizeof(uint64_t));
            at86rf2xx_set_addr_long(dev, *((uint64_t *)val));
            /* don't set res to set netdev2_ieee802154_t::long_addr */
            break;
        case NETOPT_NID:
            assert(len <= sizeof(uint16_t));
            at86rf2xx_set_pan(dev, *((uint16_t *)val));
            /* don't set res to set netdev2_ieee802154_t::pan */
            break;
        case NETOPT_CHANNEL:
            assert(len != sizeof(uint8_t));
            uint8_t chan = ((uint8_t *)val)[0];
            if (chan < AT86RF2XX_MIN_CHANNEL ||
                chan > AT86RF2XX_MAX_CHANNEL) {
                res = -EINVAL;
                break;
            }
            at86rf2xx_set_chan(dev, chan);
            /* don't set res to set netdev2_ieee802154_t::chan */
            break;

        case NETOPT_CHANNEL_PAGE:
            assert(len != sizeof(uint8_t));
            uint8_t page = ((uint8_t *)val)[0];
#ifdef MODULE_AT86RF212B
            if ((page != 0) && (page != 2)) {
                res = -EINVAL;
            }
            else {
                at86rf2xx_set_page(dev, page);
                res = sizeof(uint8_t);
            }
#else
            /* rf23x only supports page 0, no need to configure anything in the driver. */
            if (page != 0) {
                res = -EINVAL;
            }
            else {
                res = sizeof(uint8_t);
            }
#endif
            break;

        case NETOPT_TX_POWER:
            assert(len <= sizeof(int16_t));
            at86rf2xx_set_txpower(dev, *((int16_t *)val));
            res = sizeof(uint16_t);
            break;

        case NETOPT_STATE:
            assert(len <= sizeof(netopt_state_t));
            res = _set_state(dev, *((netopt_state_t *)val));
            break;

        case NETOPT_AUTOACK:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK,
                                 ((bool *)val)[0]);
            /* don't set res to set netdev2_ieee802154_t::flags */
            break;

        case NETOPT_RETRANS:
            assert(len <= sizeof(uint8_t));
            at86rf2xx_set_max_retries(dev, *((uint8_t *)val));
            res = sizeof(uint8_t);
            break;

        case NETOPT_PRELOADING:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_PRELOADING,
                                 ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_PROMISCUOUSMODE:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_PROMISCUOUS,
                                 ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_START_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_START,
                                 ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_END_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_END,
                                 ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_START_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_START,
                                 ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_END_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_END,
                                 ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA,
                                 ((bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA_RETRIES:
            assert(len <= sizeof(uint8_t));
            if( !(dev->netdev.flags & AT86RF2XX_OPT_CSMA ||
                (*((uint8_t *)val) > 5)) ) {
                /* If CSMA is disabled, don't allow setting retries */
                res = -EINVAL;
            }
            else {
                at86rf2xx_set_csma_max_retries(dev, *((uint8_t *)val));
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_CCA_THRESHOLD:
            assert(len <= sizeof(int8_t));
            at86rf2xx_set_cca_threshold(dev, *((int8_t *)val));
            res = sizeof(int8_t);
            break;

        default:
            break;
    }

    /* go back to sleep if were sleeping and state hasn't been changed */
    if ((old_state == AT86RF2XX_STATE_SLEEP) &&
        (opt != NETOPT_STATE)) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_SLEEP);
    }

    if (res == -ENOTSUP) {
        res = netdev2_ieee802154_set((netdev2_ieee802154_t *)netdev, opt,
                                     val, len);
    }

    return res;
}

static void _isr(netdev2_t *netdev)
{
    at86rf2xx_t *dev = (at86rf2xx_t *) netdev;
    uint8_t irq_mask;
    uint8_t state;
    uint8_t trac_status;

    /* If transceiver is sleeping register access is impossible and frames are
     * lost anyway, so return immediately.
     */
    state = at86rf2xx_get_status(dev);
    if (state == AT86RF2XX_STATE_SLEEP) {
        return;
    }

    /* read (consume) device status */
    irq_mask = at86rf2xx_reg_read(dev, AT86RF2XX_REG__IRQ_STATUS);

    trac_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATE) &
                  AT86RF2XX_TRX_STATE_MASK__TRAC;

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__RX_START) {
        netdev->event_callback(netdev, NETDEV2_EVENT_RX_STARTED);
        DEBUG("[at86rf2xx] EVT - RX_START\n");
    }

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__TRX_END) {
        if (state == AT86RF2XX_STATE_RX_AACK_ON ||
            state == AT86RF2XX_STATE_BUSY_RX_AACK) {
            DEBUG("[at86rf2xx] EVT - RX_END\n");
            if (!(dev->netdev.flags & AT86RF2XX_OPT_TELL_RX_END)) {
                return;
            }
            netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE);
        }
        else if (state == AT86RF2XX_STATE_TX_ARET_ON ||
                 state == AT86RF2XX_STATE_BUSY_TX_ARET) {
            /* check for more pending TX calls and return to idle state if
             * there are none */
            assert(dev->pending_tx != 0);
            if ((--dev->pending_tx) == 0) {
                at86rf2xx_set_state(dev, dev->idle_state);
                DEBUG("[at86rf2xx] return to state 0x%x\n", dev->idle_state);
            }

            DEBUG("[at86rf2xx] EVT - TX_END\n");

            if (netdev->event_callback && (dev->netdev.flags & AT86RF2XX_OPT_TELL_TX_END)) {
                switch (trac_status) {
                    case AT86RF2XX_TRX_STATE__TRAC_SUCCESS:
                    case AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING:
                        netdev->event_callback(netdev, NETDEV2_EVENT_TX_COMPLETE);
                        DEBUG("[at86rf2xx] TX SUCCESS\n");
                        break;
                    case AT86RF2XX_TRX_STATE__TRAC_NO_ACK:
                        netdev->event_callback(netdev, NETDEV2_EVENT_TX_NOACK);
                        DEBUG("[at86rf2xx] TX NO_ACK\n");
                        break;
                    case AT86RF2XX_TRX_STATE__TRAC_CHANNEL_ACCESS_FAILURE:
                        netdev->event_callback(netdev, NETDEV2_EVENT_TX_MEDIUM_BUSY);
                        DEBUG("[at86rf2xx] TX_CHANNEL_ACCESS_FAILURE\n");
                        break;
                    default:
                        DEBUG("[at86rf2xx] Unhandled TRAC_STATUS: %d\n",
                              trac_status >> 5);
                }
            }
        }
    }
}
