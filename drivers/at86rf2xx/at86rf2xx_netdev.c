/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
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
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "iolist.h"

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "at86rf2xx.h"
#include "at86rf2xx_netdev.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_MHR_OVERHEAD   (25)

static int _send(netdev_t *netdev, const iolist_t *iolist);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev_t *netdev);
static void _isr(netdev_t *netdev);
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len);

const netdev_driver_t at86rf2xx_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static void _irq_handler(void *arg)
{
    netdev_t *dev = (netdev_t *) arg;

    if (dev->event_callback) {
        dev->event_callback(dev, NETDEV_EVENT_ISR);
    }
}

static int _init(netdev_t *netdev)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;

    /* initialize GPIOs */
    spi_init_cs(dev->params.spi, dev->params.cs_pin);
    gpio_init(dev->params.sleep_pin, GPIO_OUT);
    gpio_clear(dev->params.sleep_pin);
    gpio_init(dev->params.reset_pin, GPIO_OUT);
    gpio_set(dev->params.reset_pin);
    gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, _irq_handler, dev);

    /* reset device to default values and put it into RX state */
    at86rf2xx_reset(dev);

    /* test if the SPI is set up correctly and the device is responding */
    if (at86rf2xx_reg_read(dev, AT86RF2XX_REG__PART_NUM) != AT86RF2XX_PARTNUM) {
        DEBUG("[at86rf2xx] error: unable to read correct part number\n");
        return -1;
    }

#ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
#endif

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    at86rf2xx_t *dev = (at86rf2xx_t *)netdev;
    size_t len = 0;

    at86rf2xx_tx_prepare(dev);

    /* load packet data into FIFO */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        /* current packet data + FCS too long */
        if ((len + iol->iol_len + 2) > AT86RF2XX_MAX_PKT_LENGTH) {
            DEBUG("[at86rf2xx] error: packet too large (%u byte) to be send\n",
                  (unsigned)len + 2);
            return -EOVERFLOW;
        }
#ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_bytes += len;
#endif
        len = at86rf2xx_tx_load(dev, iol->iol_base, iol->iol_len, len);
    }

    /* send data out directly if pre-loading id disabled */
    if (!(dev->flags & AT86RF2XX_OPT_PRELOADING)) {
        at86rf2xx_tx_exec(dev);
    }
    /* return the number of bytes that were actually loaded into the frame
     * buffer/send out */
    return (int)len;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
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

    /* at86rf212  RSSI_BASE_VAL +3.1*RSSI, base varies for diff. modulation and datarates
     * at86RF232  RSSI_BASE_VAL +3.0*RSSI, base -91dBm
     * at86RF233  RSSI_BASE_VAL +3.0*RSSI, base -94dBm
     * at86RF231  RSSI_BASE_VAL +3.0*(RSSI-1), base -91dBm
     * at***RFR2  RSSI_BASE_VAL +3.0*(RSSI-1), base -90dBm
     *
     * AT86RF231 MAN. p.89 8.3.2 Reading RSSI
     * AT86RF232 MAN. p.88 8.3.2 Reading RSSI
     * AT86RF233 MAN. p.99 8.4.2 Reading RSSI
     * "It is not recommended reading the RSSI value when using the Extended
     * Operating Modes, use ED instead"
     * at86RF231  RSSI_BASE_VAL +ED, base -90dBm
     * at86RF232  RSSI_BASE_VAL +ED, base -91dBm
     * at86RF233  RSSI_BASE_VAL +ED, base -94dBm
     * at***RFR2  RSSI_BASE_VAL +ED, base -90dBm
     */
    if (info != NULL) {
        uint8_t rssi = 0;
        netdev_ieee802154_rx_info_t *radio_info = info;

#if defined(MODULE_AT86RF231) || defined(MODULE_AT86RF232) || defined(MODULE_AT86RF233)
        at86rf2xx_fb_read(dev, &(radio_info->lqi), 1);
        at86rf2xx_fb_stop(dev);
        rssi = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_ED_LEVEL);
#else
        at86rf2xx_fb_read(dev, &(radio_info->lqi), 1);
        at86rf2xx_fb_read(dev, &(rssi), 1);
        at86rf2xx_fb_stop(dev);
        rssi = 3 * rssi;
#endif
        radio_info->rssi = RSSI_BASE_VAL + rssi;
        DEBUG("[at86rf2xx] LQI:%d high is good, RSSI:%d high is either good or"
              "too much interference.\n", radio_info->lqi, radio_info->rssi);
    }
    else {
        at86rf2xx_fb_stop(dev);
    }

    return pkt_len;
}

static int _set_state(at86rf2xx_t *dev, netopt_state_t state)
{
    switch (state) {
        case NETOPT_STATE_STANDBY:
            at86rf2xx_set_state(dev, AT86RF2XX_STATE_TRX_OFF);
            break;
        case NETOPT_STATE_SLEEP:
            at86rf2xx_set_state(dev, AT86RF2XX_STATE_SLEEP);
            break;
        case NETOPT_STATE_IDLE:
            at86rf2xx_set_state(dev, AT86RF2XX_STATE_RX_AACK_ON);
            break;
        case NETOPT_STATE_TX:
            if (dev->flags & AT86RF2XX_OPT_PRELOADING) {
                /* The netdev driver ISR switches the transceiver back to the
                 * previous idle state after a completed TX. If the user tries
                 * to initiate another transmission (retransmitting the same data)
                 * without first going to TX_ARET_ON, the command to start TX
                 * would be ignored, leading to a deadlock in this netdev driver
                 * thread.
                 * Additionally, avoids driver thread deadlock when PRELOADING
                 * is set and the user tries to initiate TX without first calling
                 * send() to write some frame data.
                 */
                if (dev->pending_tx == 0) {
                    /* retransmission of old data, at86rf2xx_tx_prepare normally
                     * increments this and the ISR for TX_END decrements it, to
                     * know when to switch back to the idle state. */
                    ++dev->pending_tx;
                }
                at86rf2xx_set_state(dev, AT86RF2XX_STATE_TX_ARET_ON);
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
        case AT86RF2XX_STATE_TRX_OFF:
            return NETOPT_STATE_STANDBY;
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

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
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
            if (dev->flags & AT86RF2XX_OPT_PRELOADING) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_PROMISCUOUSMODE:
            if (dev->flags & AT86RF2XX_OPT_PROMISCUOUS) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_RX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF2XX_OPT_TELL_RX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_RX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF2XX_OPT_TELL_RX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_START_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF2XX_OPT_TELL_TX_START);
            return sizeof(netopt_enable_t);

        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF2XX_OPT_TELL_TX_END);
            return sizeof(netopt_enable_t);

        case NETOPT_CSMA:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF2XX_OPT_CSMA);
            return sizeof(netopt_enable_t);

/* Only radios with the XAH_CTRL_2 register support frame retry reporting */
#if AT86RF2XX_HAVE_RETRIES
        case NETOPT_TX_RETRIES_NEEDED:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = dev->tx_retries;
            return sizeof(uint8_t);
#endif

        default:
            /* Can still be handled in second switch */
            break;
    }

    int res;

    if (((res = netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, val,
                                      max_len)) >= 0) || (res != -ENOTSUP)) {
        return res;
    }

    uint8_t old_state = at86rf2xx_get_status(dev);

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

        case NETOPT_IS_CHANNEL_CLR:
            assert(max_len >= sizeof(netopt_enable_t));
            *((netopt_enable_t *)val) = at86rf2xx_cca(dev);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_LAST_ED_LEVEL:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf2xx_get_ed_level(dev);
            res = sizeof(int8_t);
            break;

        case NETOPT_AUTOACK:
            assert(max_len >= sizeof(netopt_enable_t));
            uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
            *((netopt_enable_t *)val) = (tmp & AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK) ? false : true;
            res = sizeof(netopt_enable_t);
            break;

        default:
            res = -ENOTSUP;
            break;
    }

    /* go back to sleep if were sleeping */
    if (old_state == AT86RF2XX_STATE_SLEEP) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_SLEEP);
    }

    return res;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    at86rf2xx_t *dev = (at86rf2xx_t *) netdev;
    uint8_t old_state = at86rf2xx_get_status(dev);
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* temporarily wake up if sleeping and opt != NETOPT_STATE.
     * opt != NETOPT_STATE check prevents redundant wake-up.
     * when opt == NETOPT_STATE, at86rf2xx_set_state() will wake up the
     * radio if needed. */
    if ((old_state == AT86RF2XX_STATE_SLEEP) && (opt != NETOPT_STATE)) {
        at86rf2xx_assert_awake(dev);
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(len <= sizeof(uint16_t));
            at86rf2xx_set_addr_short(dev, *((const uint16_t *)val));
            /* don't set res to set netdev_ieee802154_t::short_addr */
            break;
        case NETOPT_ADDRESS_LONG:
            assert(len <= sizeof(uint64_t));
            at86rf2xx_set_addr_long(dev, *((const uint64_t *)val));
            /* don't set res to set netdev_ieee802154_t::long_addr */
            break;
        case NETOPT_NID:
            assert(len <= sizeof(uint16_t));
            at86rf2xx_set_pan(dev, *((const uint16_t *)val));
            /* don't set res to set netdev_ieee802154_t::pan */
            break;
        case NETOPT_CHANNEL:
            assert(len == sizeof(uint16_t));
            uint8_t chan = (((const uint16_t *)val)[0]) & UINT8_MAX;
#if AT86RF2XX_MIN_CHANNEL
            if (chan < AT86RF2XX_MIN_CHANNEL || chan > AT86RF2XX_MAX_CHANNEL) {
#else
            if (chan > AT86RF2XX_MAX_CHANNEL) {
#endif /* AT86RF2XX_MIN_CHANNEL */
                res = -EINVAL;
                break;
            }
            at86rf2xx_set_chan(dev, chan);
            /* don't set res to set netdev_ieee802154_t::chan */
            break;

        case NETOPT_CHANNEL_PAGE:
            assert(len == sizeof(uint16_t));
            uint8_t page = (((const uint16_t *)val)[0]) & UINT8_MAX;
#ifdef MODULE_AT86RF212B
            if ((page != 0) && (page != 2)) {
                res = -EINVAL;
            }
            else {
                at86rf2xx_set_page(dev, page);
                res = sizeof(uint16_t);
            }
#else
            /* rf23x only supports page 0, no need to configure anything in the driver. */
            if (page != 0) {
                res = -EINVAL;
            }
            else {
                res = sizeof(uint16_t);
            }
#endif
            break;

        case NETOPT_TX_POWER:
            assert(len <= sizeof(int16_t));
            at86rf2xx_set_txpower(dev, *((const int16_t *)val));
            res = sizeof(uint16_t);
            break;

        case NETOPT_STATE:
            assert(len <= sizeof(netopt_state_t));
            res = _set_state(dev, *((const netopt_state_t *)val));
            break;

        case NETOPT_AUTOACK:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_ACK_PENDING:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_ACK_PENDING,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RETRANS:
            assert(len <= sizeof(uint8_t));
            at86rf2xx_set_max_retries(dev, *((const uint8_t *)val));
            res = sizeof(uint8_t);
            break;

        case NETOPT_PRELOADING:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_PRELOADING,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_PROMISCUOUSMODE:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_PROMISCUOUS,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_START_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_START,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_RX_END_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_RX_END,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_START_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_START,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_TX_END_IRQ:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_TELL_TX_END,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA_RETRIES:
            assert(len <= sizeof(uint8_t));
            if (!(dev->flags & AT86RF2XX_OPT_CSMA) ||
                (*((uint8_t *)val) > 5)) {
                /* If CSMA is disabled, don't allow setting retries */
                res = -EINVAL;
            }
            else {
                at86rf2xx_set_csma_max_retries(dev, *((const uint8_t *)val));
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_CCA_THRESHOLD:
            assert(len <= sizeof(int8_t));
            at86rf2xx_set_cca_threshold(dev, *((const int8_t *)val));
            res = sizeof(int8_t);
            break;

        default:
            break;
    }

    /* go back to sleep if were sleeping and state hasn't been changed */
    if ((old_state == AT86RF2XX_STATE_SLEEP)
        && (opt != NETOPT_STATE)) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_SLEEP);
    }

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt, val, len);
    }

    return res;
}

static void _isr(netdev_t *netdev)
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

    trac_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATE)
                  & AT86RF2XX_TRX_STATE_MASK__TRAC;

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__RX_START) {
        netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
        DEBUG("[at86rf2xx] EVT - RX_START\n");
    }

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__TRX_END) {
        if ((state == AT86RF2XX_STATE_RX_AACK_ON)
            || (state == AT86RF2XX_STATE_BUSY_RX_AACK)) {
            DEBUG("[at86rf2xx] EVT - RX_END\n");
            if (!(dev->flags & AT86RF2XX_OPT_TELL_RX_END)) {
                return;
            }
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
        else if ((state == AT86RF2XX_STATE_TX_ARET_ON)
                 || (state == AT86RF2XX_STATE_BUSY_TX_ARET)) {
            /* check for more pending TX calls and return to idle state if
             * there are none */
            assert(dev->pending_tx != 0);
            if ((--dev->pending_tx) == 0) {
                at86rf2xx_set_state(dev, dev->idle_state);
                DEBUG("[at86rf2xx] return to idle state 0x%x\n", dev->idle_state);
            }
/* Only radios with the XAH_CTRL_2 register support frame retry reporting */
#if AT86RF2XX_HAVE_RETRIES
            dev->tx_retries = (at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_2)
                               & AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_MASK) >>
                              AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_OFFSET;
#endif

            DEBUG("[at86rf2xx] EVT - TX_END\n");

            if (netdev->event_callback && (dev->flags & AT86RF2XX_OPT_TELL_TX_END)) {
                switch (trac_status) {
#ifdef MODULE_OPENTHREAD
                    case AT86RF2XX_TRX_STATE__TRAC_SUCCESS:
                        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
                        DEBUG("[at86rf2xx] TX SUCCESS\n");
                        break;
                    case AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING:
                        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE_DATA_PENDING);
                        DEBUG("[at86rf2xx] TX SUCCESS DATA PENDING\n");
                        break;
#else
                    case AT86RF2XX_TRX_STATE__TRAC_SUCCESS:
                    case AT86RF2XX_TRX_STATE__TRAC_SUCCESS_DATA_PENDING:
                        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
                        DEBUG("[at86rf2xx] TX SUCCESS\n");
                        break;
#endif
                    case AT86RF2XX_TRX_STATE__TRAC_NO_ACK:
                        netdev->event_callback(netdev, NETDEV_EVENT_TX_NOACK);
                        DEBUG("[at86rf2xx] TX NO_ACK\n");
                        break;
                    case AT86RF2XX_TRX_STATE__TRAC_CHANNEL_ACCESS_FAILURE:
                        netdev->event_callback(netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);
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
