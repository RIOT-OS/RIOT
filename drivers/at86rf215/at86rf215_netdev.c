/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Netdev adaption for the AT86RF215 driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Georg von Zengen <vonzengen@ibr.cs.tu-bs.de>
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
#include "net/gnrc/netif/internal.h"

#include "sys/bus.h"

#include "at86rf215.h"
#include "at86rf215_netdev.h"
#include "at86rf215_internal.h"

#include "debug.h"

static int _send(netdev_t *netdev, const iolist_t *iolist);
static int _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int _init(netdev_t *netdev);
static void _isr(netdev_t *netdev);
static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len);

const netdev_driver_t at86rf215_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

static bool _is_busy(at86rf215_t *dev)
{
    if (dev->flags & AT86RF215_OPT_TX_PENDING) {
        return true;
    }

    if (dev->state == AT86RF215_STATE_TX ||
        dev->state == AT86RF215_STATE_TX_WAIT_ACK ||
        dev->state == AT86RF215_STATE_RX_SEND_ACK) {
        return true;
    }

    return false;
}

static uint8_t _get_best_match(const uint8_t *array, uint8_t len, uint8_t val)
{
    uint8_t res = 0;
    uint8_t best = 0xFF;
    for (uint8_t i = 0; i < len; ++i) {
        if (abs((int)array[i] - val) < best) {
            best = abs((int)array[i] - val);
            res = i;
        }
    }

    return res;
}

/* executed in the GPIO ISR context */
static void _irq_handler(void *arg)
{
    netdev_t *netdev = (netdev_t *) arg;

    netdev->event_callback(netdev, NETDEV_EVENT_ISR);
}

/* if only one interface is active, but the other one to sleep */
static inline void _put_sibling_to_sleep(at86rf215_t *dev) {
    if (is_subGHz(dev)) {
        at86rf215_reg_write(dev, RG_RF24_CMD, CMD_RF_SLEEP);
    } else {
        at86rf215_reg_write(dev, RG_RF09_CMD, CMD_RF_SLEEP);
    }
}

static int _init(netdev_t *netdev)
{
    int res;
    at86rf215_t *dev = (at86rf215_t *)netdev;

    /* don't call HW init for both radios */
    if (is_subGHz(dev) || dev->sibling == NULL) {
        /* initialize GPIOs */
        spi_init_cs(dev->params.spi, dev->params.cs_pin);
        gpio_init(dev->params.reset_pin, GPIO_OUT);
        gpio_set(dev->params.reset_pin);

        /* reset the entire chip */
        if ((res = at86rf215_hardware_reset(dev))) {
            return res;
        }

        /* turn off unused interface */
        if (dev->sibling == NULL) {
            _put_sibling_to_sleep(dev);
        }

        gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_RISING, _irq_handler, dev);
    }

    res = at86rf215_reg_read(dev, RG_RF_PN);
    if ((res != AT86RF215_PN) && (res != AT86RF215M_PN)) {
        DEBUG("[at86rf215] error: unable to read correct part number: %x\n", res);
        return -ENOTSUP;;
    }

    /* reset device to default values and put it into RX state */
    at86rf215_reset_and_cfg(dev);

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    at86rf215_t *dev = (at86rf215_t *)netdev;
    size_t len = 0;

    if (at86rf215_tx_prepare(dev)) {
        return -EBUSY;
    }

    /* load packet data into FIFO */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {

        /* current packet data + FCS too long */
        if ((len + iol->iol_len + IEEE802154_FCS_LEN) > AT86RF215_MAX_PKT_LENGTH) {
            DEBUG("[at86rf215] error: packet too large (%u byte) to be send\n",
                  (unsigned)len + IEEE802154_FCS_LEN);
            at86rf215_tx_abort(dev);
            return -EOVERFLOW;
        }

        if (iol->iol_len) {
            len = at86rf215_tx_load(dev, iol->iol_base, iol->iol_len, len);
        }
    }

    /* send data out directly if pre-loading id disabled */
    if (!(dev->flags & AT86RF215_OPT_PRELOADING)) {
        at86rf215_tx_exec(dev);
    }

    /* return the number of bytes that were actually loaded into the frame
     * buffer/send out */
    return (int)len;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    at86rf215_t *dev = (at86rf215_t *)netdev;
    int16_t pkt_len;

    /* get the size of the received packet */
    at86rf215_reg_read_bytes(dev, dev->BBC->RG_RXFLL, &pkt_len, sizeof(pkt_len));

    /* subtract length of FCS field */
    pkt_len = (pkt_len & 0x7ff) - IEEE802154_FCS_LEN;

    /* just return length when buf == NULL */
    if (buf == NULL) {
        return pkt_len;
    }

    /* not enough space in buf */
    if (pkt_len > (int) len) {
        return -ENOBUFS;
    }

    /* copy payload */
    at86rf215_reg_read_bytes(dev, dev->BBC->RG_FBRXS, buf, pkt_len);

    if (info != NULL) {
        netdev_ieee802154_rx_info_t *radio_info = info;
        radio_info->rssi = (int8_t) at86rf215_reg_read(dev, dev->RF->RG_EDV);

        if (IS_USED(MODULE_AT86RF215_TIMESTAMP)) {
            uint32_t rx_timestamp;
            at86rf215_reg_read_bytes(dev, dev->BBC->RG_CNT0, &rx_timestamp,
                                    sizeof(rx_timestamp));

            /* convert counter value to ns */
            radio_info->timestamp = rx_timestamp * 1000ULL / 32;
            radio_info->flags |= NETDEV_RX_IEEE802154_INFO_FLAG_TIMESTAMP;
        }
    }

    return pkt_len;
}

static int _set_state(at86rf215_t *dev, netopt_state_t state)
{
    if (_is_busy(dev)) {
        return -EBUSY;
    }

    switch (state) {
        case NETOPT_STATE_STANDBY:
            at86rf215_set_idle_from_rx(dev, CMD_RF_TRXOFF);
            break;
        case NETOPT_STATE_SLEEP:
            at86rf215_set_idle_from_rx(dev, CMD_RF_SLEEP);
            break;
        case NETOPT_STATE_RX:
        case NETOPT_STATE_IDLE:
            at86rf215_set_rx_from_idle(dev, NULL);
            break;
        case NETOPT_STATE_TX:
            if (dev->flags & AT86RF215_OPT_PRELOADING) {
                return at86rf215_tx_exec(dev);
            }
            break;
        case NETOPT_STATE_RESET:
            at86rf215_reset(dev);
            break;
        default:
            return -ENOTSUP;
    }
    return sizeof(netopt_state_t);
}

static netopt_state_t _get_state(at86rf215_t *dev)
{
    switch (dev->state) {
        case AT86RF215_STATE_SLEEP:
            return NETOPT_STATE_SLEEP;
        case AT86RF215_STATE_RX_SEND_ACK:
            return NETOPT_STATE_RX;
        case AT86RF215_STATE_TX:
        case AT86RF215_STATE_TX_WAIT_ACK:
            return NETOPT_STATE_TX;
        case AT86RF215_STATE_OFF:
            return NETOPT_STATE_OFF;
        case AT86RF215_STATE_IDLE:
        default:
            return NETOPT_STATE_IDLE;
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    at86rf215_t *dev = (at86rf215_t *) netdev;

    if (netdev == NULL) {
        return -ENODEV;
    }

    /* getting these options doesn't require the transceiver to be responsive */
    switch (opt) {
        case NETOPT_STATE:
            assert(max_len >= sizeof(netopt_state_t));
            *((netopt_state_t *)val) = _get_state(dev);
            return sizeof(netopt_state_t);

        case NETOPT_PRELOADING:
            if (dev->flags & AT86RF215_OPT_PRELOADING) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_PROMISCUOUSMODE:
            if (dev->flags & AT86RF215_OPT_PROMISCUOUS) {
                *((netopt_enable_t *)val) = NETOPT_ENABLE;
            }
            else {
                *((netopt_enable_t *)val) = NETOPT_DISABLE;
            }
            return sizeof(netopt_enable_t);

        case NETOPT_RX_START_IRQ:
        case NETOPT_RX_END_IRQ:
        case NETOPT_TX_START_IRQ:
        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) = NETOPT_ENABLE;
            return sizeof(netopt_enable_t);

        case NETOPT_CSMA:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF215_OPT_CSMA);
            return sizeof(netopt_enable_t);

        case NETOPT_CSMA_RETRIES:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = dev->csma_retries_max;
            return sizeof(uint8_t);

        case NETOPT_CSMA_MAXBE:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = dev->csma_maxbe;
            return sizeof(uint8_t);

        case NETOPT_CSMA_MINBE:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = dev->csma_minbe;
            return sizeof(uint8_t);

        case NETOPT_RETRANS:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = dev->retries_max;
            return sizeof(uint8_t);

        case NETOPT_TX_RETRIES_NEEDED:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = dev->retries_max - dev->retries;
            return sizeof(uint8_t);

        case NETOPT_AUTOACK:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF215_OPT_AUTOACK);
            return sizeof(netopt_enable_t);

        case NETOPT_CHANNEL_PAGE:
            assert(max_len >= sizeof(uint16_t));
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_OQPSK) {
                return -ENOTSUP;
            }

            ((uint8_t *)val)[1] = 0;
            ((uint8_t *)val)[0] = is_subGHz(dev) ? 2 : 0;
            return sizeof(uint16_t);

        case NETOPT_AUTOCCA:
            *((netopt_enable_t *)val) =
                !!(dev->flags & AT86RF215_OPT_CCATX);
            return sizeof(netopt_enable_t);

        default:
            /* Can still be handled in second switch */
            break;
    }

    int res;

    if (((res = netdev_ieee802154_get((netdev_ieee802154_t *)netdev, opt, val,
                                      max_len)) >= 0) || (res != -ENOTSUP)) {
        return res;
    }

    /* properties are not available if the device is sleeping */
    if (dev->state == AT86RF215_STATE_SLEEP) {
        return -ENOTSUP;
    }

    /* these options require the transceiver to be not sleeping*/
    switch (opt) {
        case NETOPT_TX_POWER:
            assert(max_len >= sizeof(int16_t));
            *((uint16_t *)val) = at86rf215_get_txpower(dev);
            res = sizeof(uint16_t);
            break;

        case NETOPT_CCA_THRESHOLD:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf215_get_cca_threshold(dev);
            res = sizeof(int8_t);
            break;

        case NETOPT_IS_CHANNEL_CLR:
            assert(max_len >= sizeof(netopt_enable_t));
            *((netopt_enable_t *)val) = at86rf215_cca(dev);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_LAST_ED_LEVEL:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf215_get_ed_level(dev);
            res = sizeof(int8_t);
            break;

        case NETOPT_RANDOM:
            at86rf215_get_random(dev, val, max_len);
            res = max_len;
            break;

        case NETOPT_IEEE802154_PHY:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf215_get_phy_mode(dev);
            res = max_len;
            break;
#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
        case NETOPT_MR_FSK_MODULATION_INDEX:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf215_FSK_get_mod_idx(dev);
            res = max_len;
            break;

        case NETOPT_MR_FSK_MODULATION_ORDER:
            assert(max_len >= sizeof(int8_t));
            /* 0 -> 2-FSK, 1 -> 4-FSK */
            *((int8_t *)val) = 2 + 2 * at86rf215_FSK_get_mod_order(dev);
            res = max_len;
            break;

        case NETOPT_MR_FSK_SRATE:
            assert(max_len >= sizeof(uint16_t));
            /* netopt expects symbol rate in kHz, internally it's stored in 10kHz steps */
            *((uint16_t *)val) = _at86rf215_fsk_srate_10kHz[at86rf215_FSK_get_srate(dev)]
                               * 10;
            res = max_len;
            break;

        case NETOPT_MR_FSK_FEC:
            assert(max_len >= sizeof(uint8_t));
            *((uint8_t *)val) = at86rf215_FSK_get_fec(dev);
            res = max_len;
            break;

        case NETOPT_CHANNEL_SPACING:
            assert(max_len >= sizeof(uint16_t));
            *((uint16_t *)val) = at86rf215_get_channel_spacing(dev);
            res = max_len;
            break;
#endif /* MODULE_NETDEV_IEEE802154_MR_FSK */
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
        case NETOPT_MR_OFDM_OPTION:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf215_OFDM_get_option(dev);
            res = max_len;
            break;

        case NETOPT_MR_OFDM_MCS:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf215_OFDM_get_scheme(dev);
            res = max_len;
            break;
#endif /* MODULE_NETDEV_IEEE802154_MR_OFDM */
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
        case NETOPT_MR_OQPSK_CHIPS:
            assert(max_len >= sizeof(int16_t));
            switch (at86rf215_OQPSK_get_chips(dev)) {
            case 0: *((int16_t *)val) =  100; break;
            case 1: *((int16_t *)val) =  200; break;
            case 2: *((int16_t *)val) = 1000; break;
            case 3: *((int16_t *)val) = 2000; break;
            }
            res = max_len;
            break;

        case NETOPT_MR_OQPSK_RATE:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf215_OQPSK_get_mode(dev);
            res = max_len;
            break;
#endif /* MODULE_NETDEV_IEEE802154_MR_OQPSK */
#ifdef MODULE_NETDEV_IEEE802154_OQPSK
        case NETOPT_OQPSK_RATE:
            assert(max_len >= sizeof(int8_t));
            *((int8_t *)val) = at86rf215_OQPSK_get_mode_legacy(dev);
            res = max_len;
            break;
#endif /* MODULE_NETDEV_IEEE802154_OQPSK */
        default:
            res = -ENOTSUP;
            break;
    }

    return res;
}

static int _set(netdev_t *netdev, netopt_t opt, const void *val, size_t len)
{
    at86rf215_t *dev = (at86rf215_t *) netdev;
    int res = -ENOTSUP;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* no need to wake up the device when it's sleeping - all registers
       are reset on wakeup. */

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(len <= sizeof(uint16_t));
            at86rf215_set_addr_short(dev, 0, *((const uint16_t *)val));
            /* don't set res to set netdev_ieee802154_t::short_addr */
            break;
        case NETOPT_ADDRESS_LONG:
            assert(len <= sizeof(uint64_t));
            at86rf215_set_addr_long(dev, *((const uint64_t *)val));
            /* don't set res to set netdev_ieee802154_t::long_addr */
            break;
        case NETOPT_NID:
            assert(len <= sizeof(uint16_t));
            at86rf215_set_pan(dev, 0, *((const uint16_t *)val));
            /* don't set res to set netdev_ieee802154_t::pan */
            break;
        case NETOPT_CHANNEL:
            assert(len == sizeof(uint16_t));
            uint16_t chan = *((const uint16_t *)val);

            if (at86rf215_chan_valid(dev, chan) != chan) {
                res = -EINVAL;
                break;
            }

            at86rf215_set_chan(dev, chan);
            /* don't set res to set netdev_ieee802154_t::chan */
            break;

        case NETOPT_TX_POWER:
            assert(len <= sizeof(int16_t));
            at86rf215_set_txpower(dev, *((const int16_t *)val));
            res = sizeof(uint16_t);
            break;

        case NETOPT_STATE:
            assert(len <= sizeof(netopt_state_t));
            res = _set_state(dev, *((const netopt_state_t *)val));
            break;

        case NETOPT_AUTOACK:
            at86rf215_set_option(dev, AT86RF215_OPT_AUTOACK,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_AUTOCCA:
            at86rf215_set_option(dev, AT86RF215_OPT_CCATX,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

#ifdef MODULE_AT86RF215_BATMON
        case NETOPT_BATMON:
            assert(len <= sizeof(uint16_t));
            {
                uint16_t mV = *(const uint16_t *)val;
                if (mV) {
                    res = at86rf215_enable_batmon(dev, mV);
                    res = (res == 0) ? (int)sizeof(uint16_t) : res;
                } else {
                    at86rf215_disable_batmon(dev);
                    res = sizeof(uint16_t);
                }
            }
            break;
#endif

        case NETOPT_RETRANS:
            assert(len <= sizeof(uint8_t));
            dev->retries_max =  *((const uint8_t *)val);
            res = sizeof(uint8_t);
            break;

        case NETOPT_PRELOADING:
            at86rf215_set_option(dev, AT86RF215_OPT_PRELOADING,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_PROMISCUOUSMODE:
            at86rf215_set_option(dev, AT86RF215_OPT_PROMISCUOUS,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA:
            at86rf215_set_option(dev, AT86RF215_OPT_CSMA,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_CSMA_RETRIES:
            assert(len <= sizeof(uint8_t));
            dev->csma_retries_max = *((const uint8_t *)val);
            res = sizeof(uint8_t);
            break;

        case NETOPT_CSMA_MAXBE:
            assert(len <= sizeof(uint8_t));
            dev->csma_maxbe = *((const uint8_t *)val);
            res = sizeof(uint8_t);
            break;

        case NETOPT_CSMA_MINBE:
            assert(len <= sizeof(uint8_t));
            dev->csma_minbe = *((const uint8_t *)val);
            res = sizeof(uint8_t);
            break;

        case NETOPT_CCA_THRESHOLD:
            assert(len <= sizeof(int8_t));
            at86rf215_set_cca_threshold(dev, *((const int8_t *)val));
            res = sizeof(int8_t);
            break;

        case NETOPT_IEEE802154_PHY:
            assert(len <= sizeof(uint8_t));
            switch (*(uint8_t *)val) {
#ifdef MODULE_NETDEV_IEEE802154_OQPSK
            case IEEE802154_PHY_OQPSK:
                at86rf215_configure_legacy_OQPSK(dev, at86rf215_OQPSK_get_mode_legacy(dev));
                res = sizeof(uint8_t);
                break;
#endif /* MODULE_NETDEV_IEEE802154_OQPSK */
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
            case IEEE802154_PHY_MR_OQPSK:
                at86rf215_configure_OQPSK(dev,
                                          at86rf215_OQPSK_get_chips(dev),
                                          at86rf215_OQPSK_get_mode(dev));
                res = sizeof(uint8_t);
                break;
#endif /* MODULE_NETDEV_IEEE802154_MR_OQPSK */
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
            case IEEE802154_PHY_MR_OFDM:
                at86rf215_configure_OFDM(dev,
                                         at86rf215_OFDM_get_option(dev),
                                         at86rf215_OFDM_get_scheme(dev));
                res = sizeof(uint8_t);
                break;
#endif /* MODULE_NETDEV_IEEE802154_MR_OFDM */
#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
            case IEEE802154_PHY_MR_FSK:
                at86rf215_configure_FSK(dev,
                                        at86rf215_FSK_get_srate(dev),
                                        at86rf215_FSK_get_mod_idx(dev),
                                        at86rf215_FSK_get_mod_order(dev),
                                        at86rf215_FSK_get_fec(dev));
                res = sizeof(uint8_t);
                break;
#endif /* MODULE_NETDEV_IEEE802154_MR_FSK */
            default:
                return -ENOTSUP;
            }
            break;

#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
        case NETOPT_MR_FSK_MODULATION_INDEX:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_FSK) {
                return -ENOTSUP;
            }

            if (at86rf215_FSK_set_mod_idx(dev, *(uint8_t *)val) == 0) {
                res = at86rf215_FSK_get_mod_idx(dev);
            } else {
                res = -ERANGE;
            }
            break;

        case NETOPT_MR_FSK_MODULATION_ORDER:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_FSK) {
                return -ENOTSUP;
            }

            if (*(uint8_t *)val != 2 && *(uint8_t *)val != 4) {
                res = -ERANGE;
            } else {
                /* 4-FSK -> 1, 2-FSK -> 0 */
                at86rf215_FSK_set_mod_order(dev, *(uint8_t *)val >> 2);
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_MR_FSK_SRATE:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_FSK) {
                return -ENOTSUP;
            }

            /* find the closest symbol rate value (in 10 kHz) that matches
               the requested input (in kHz) */
            res = _get_best_match(_at86rf215_fsk_srate_10kHz,
                                  FSK_SRATE_400K + 1, *(uint16_t *)val / 10);
            if (at86rf215_FSK_set_srate(dev, res) == 0) {
                res = 10 * _at86rf215_fsk_srate_10kHz[res];
            } else {
                res = -ERANGE;
            }
            break;

        case NETOPT_MR_FSK_FEC:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_FSK) {
                return -ENOTSUP;
            }

            if (at86rf215_FSK_set_fec(dev, *(uint8_t *)val) == 0) {
                res = sizeof(uint8_t);
            } else {
                res = -ERANGE;
            }

            break;

        case NETOPT_CHANNEL_SPACING:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_FSK) {
                return -ENOTSUP;
            }

            /* find the closest channel spacing value (in 25 kHz) that matches
               the requested input (in kHz) */
            res = _get_best_match(_at86rf215_fsk_channel_spacing_25kHz,
                                  FSK_CHANNEL_SPACING_400K + 1, *(uint16_t *)val / 25);
            if (at86rf215_FSK_set_channel_spacing(dev, res) == 0) {
                res = 25 * _at86rf215_fsk_channel_spacing_25kHz[res];
            } else {
                res = -ERANGE;
            }
            break;
#endif /* MODULE_NETDEV_IEEE802154_MR_FSK */
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
        case NETOPT_MR_OFDM_OPTION:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_OFDM) {
                return -ENOTSUP;
            }

            assert(len <= sizeof(uint8_t));
            if (at86rf215_OFDM_set_option(dev, *((const uint8_t *)val)) == 0) {
                res = sizeof(uint8_t);
            } else {
                res = -ERANGE;
            }
            break;

        case NETOPT_MR_OFDM_MCS:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_OFDM) {
                return -ENOTSUP;
            }

            assert(len <= sizeof(uint8_t));
            if (at86rf215_OFDM_set_scheme(dev, *((const uint8_t *)val)) == 0) {
                res = sizeof(uint8_t);
            } else {
                res = -ERANGE;
            }
            break;
#endif /* MODULE_NETDEV_IEEE802154_MR_OFDM */
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
        case NETOPT_MR_OQPSK_CHIPS:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_OQPSK) {
                return -ENOTSUP;
            }

            uint8_t chips;
            assert(len <= sizeof(uint16_t));
            if (*((const uint16_t *)val) == 100) {
                chips = 0;
            } else if (*((const uint16_t *)val) == 200) {
                chips = 1;
            } else if (*((const uint16_t *)val) == 1000) {
                chips = 2;
            } else if (*((const uint16_t *)val) == 2000) {
                chips = 3;
            } else {
                res = -EINVAL;
                break;
            }

            if (at86rf215_OQPSK_set_chips(dev, chips) == 0) {
                res = sizeof(uint8_t);
            } else {
                res = -ERANGE;
            }
            break;

        case NETOPT_MR_OQPSK_RATE:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_MR_OQPSK) {
                return -ENOTSUP;
            }

            assert(len <= sizeof(uint8_t));
            if (at86rf215_OQPSK_set_mode(dev, *(uint8_t *)val) == 0) {
                res = sizeof(uint8_t);
            } else {
                res = -ERANGE;
            }
            break;
#endif /* MODULE_NETDEV_IEEE802154_MR_OQPSK */
#ifdef MODULE_NETDEV_IEEE802154_OQPSK
        case NETOPT_OQPSK_RATE:
            if (at86rf215_get_phy_mode(dev) != IEEE802154_PHY_OQPSK) {
                return -ENOTSUP;
            }

            assert(len <= sizeof(uint8_t));
            if (at86rf215_OQPSK_set_mode_legacy(dev, *(uint8_t *)val) == 0) {
                res = sizeof(uint8_t);
            } else {
                res = -ERANGE;
            }
            break;
#endif /* MODULE_NETDEV_IEEE802154_OQPSK */
        default:
            break;
    }

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set((netdev_ieee802154_t *)netdev, opt, val, len);
    }

    return res;
}

static void _enable_tx2rx(at86rf215_t *dev)
{
    uint8_t amcs = at86rf215_reg_read(dev, dev->BBC->RG_AMCS);

    /* disable AACK, enable TX2RX */
    amcs |=  AMCS_TX2RX_MASK;
    amcs &= ~AMCS_AACK_MASK;

    at86rf215_reg_write(dev, dev->BBC->RG_AMCS, amcs);
}

static void _tx_end(at86rf215_t *dev, netdev_event_t event)
{
    netdev_t *netdev = (netdev_t *)dev;

    /* listen to non-ACK packets again */
    if (dev->flags & AT86RF215_OPT_ACK_REQUESTED) {
        dev->flags &= ~AT86RF215_OPT_ACK_REQUESTED;
        at86rf215_filter_ack(dev, false);
    }

    at86rf215_tx_done(dev);

    if (netdev->event_callback) {
        netdev->event_callback(netdev, event);
    }

    dev->timeout = 0;
    dev->state = AT86RF215_STATE_IDLE;
}

static void _ack_timeout_cb(void* arg) {
    at86rf215_t *dev = arg;
    dev->timeout = AT86RF215_TIMEOUT_ACK;
    msg_send_int(&dev->timer_msg, dev->timer_msg.sender_pid);
}

static void _backoff_timeout_cb(void* arg) {
    at86rf215_t *dev = arg;
    dev->timeout = AT86RF215_TIMEOUT_CSMA;
    msg_send_int(&dev->timer_msg, dev->timer_msg.sender_pid);
}

static void _set_idle(at86rf215_t *dev)
{
    dev->state = AT86RF215_STATE_IDLE;

    uint8_t next_state;

    if (dev->flags & AT86RF215_OPT_TX_PENDING) {
        next_state = CMD_RF_TXPREP;
    } else {
        next_state = CMD_RF_RX;
    }

    at86rf215_rf_cmd(dev, next_state);
}

/* wake up the radio thread after ACK timeout */
static void _start_ack_timer(at86rf215_t *dev)
{
    dev->timer_msg.type = NETDEV_MSG_TYPE_EVENT;
    dev->timer_msg.sender_pid = thread_getpid();

    dev->timer.arg = dev;
    dev->timer.callback = _ack_timeout_cb;

    xtimer_set(&dev->timer, dev->ack_timeout_usec);
}

/* wake up the radio thread after CSMA backoff period */
static void _start_backoff_timer(at86rf215_t *dev)
{
    uint8_t be; /* backoff exponent */
    uint32_t base;

    /* energy detect interrupt happened -> hardware is still in RX mode */
    at86rf215_get_random(dev, &base, sizeof(base));

    be = ((dev->csma_retries_max - dev->csma_retries) - 1) + dev->csma_minbe;

    if (be > dev->csma_maxbe) {
        be = dev->csma_maxbe;
    }

    uint32_t csma_backoff_usec = ((1LU << be) - 1) * dev->csma_backoff_period;
    /* limit the 32bit random value to the current backoff */
    csma_backoff_usec = base % csma_backoff_usec;

    DEBUG("Set CSMA backoff to %"PRIu32" (be %u min %u max %u base: %"PRIu32")\n",
          csma_backoff_usec, be, dev->csma_minbe, dev->csma_maxbe, base);

    dev->timer_msg.type = NETDEV_MSG_TYPE_EVENT;
    dev->timer_msg.sender_pid = thread_getpid();

    dev->timer.arg = dev;
    dev->timer.callback = _backoff_timeout_cb;

    xtimer_set(&dev->timer, csma_backoff_usec);
}

static inline bool _ack_frame_received(at86rf215_t *dev)
{
    /* check if the sequence numbers (3rd byte) match */
    return at86rf215_reg_read(dev, dev->BBC->RG_FBRXS + 2)
        == at86rf215_reg_read(dev, dev->BBC->RG_FBTXS + 2);
}

static void _handle_ack_timeout(at86rf215_t *dev)
{
    if (dev->retries) {
        --dev->retries;

        if (dev->flags & AT86RF215_OPT_CSMA) {
            dev->csma_retries = dev->csma_retries_max;
            if (!(dev->flags & AT86RF215_OPT_CCATX)){
                dev->flags |= AT86RF215_OPT_CCA_PENDING;
            }
        }

        dev->flags |= AT86RF215_OPT_TX_PENDING;
        at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
    } else {
        /* no retransmissions left */
        _tx_end(dev, NETDEV_EVENT_TX_NOACK);
    }
}

/* clear the other IRQ if the sibling is not ready yet */
static inline void _clear_sibling_irq(at86rf215_t *dev) {
    if (is_subGHz(dev)) {
        at86rf215_reg_read(dev, RG_RF24_IRQS);
        at86rf215_reg_read(dev, RG_BBC1_IRQS);
    } else {
        at86rf215_reg_read(dev, RG_RF09_IRQS);
        at86rf215_reg_read(dev, RG_BBC0_IRQS);
    }
}

static void _handle_edc(at86rf215_t *dev)
{
    netdev_t *netdev = (netdev_t *) dev;

    /* In CCATX mode this function is only triggered if busy */
    if (!(dev->flags & AT86RF215_OPT_CCATX)) {
        /* channel clear -> TX */
        if ((int8_t)at86rf215_reg_read(dev, dev->RF->RG_EDV) <= at86rf215_get_cca_threshold(dev)) {
            dev->flags &= ~AT86RF215_OPT_CCA_PENDING;
            at86rf215_enable_baseband(dev);
            at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
            return;
        }
    }

    DEBUG("CSMA busy\n");
    if (dev->csma_retries) {
        --dev->csma_retries;
        _start_backoff_timer(dev);
    } else {
        /* channel busy and no retries left */
        dev->flags &= ~(AT86RF215_OPT_CCA_PENDING | AT86RF215_OPT_TX_PENDING);
        dev->state = AT86RF215_STATE_IDLE;

        at86rf215_enable_baseband(dev);
        at86rf215_enable_rpc(dev);
        at86rf215_tx_done(dev);

        netdev->event_callback(netdev, NETDEV_EVENT_TX_MEDIUM_BUSY);

        DEBUG("CSMA give up");
        /* radio is still in RX mode, tx_done sets IDLE state */
    }
}

/* executed in the radio thread */
static void _isr(netdev_t *netdev)
{
    at86rf215_t *dev = (at86rf215_t *) netdev;
    uint8_t bb_irq_mask, rf_irq_mask;
    uint8_t bb_irqs_enabled = BB_IRQ_RXFE | BB_IRQ_TXFE;

    /* not using IRQMM because we want to know about AGCH */
    bb_irqs_enabled |= BB_IRQ_RXAM;

    rf_irq_mask = at86rf215_reg_read(dev, dev->RF->RG_IRQS);
    bb_irq_mask = at86rf215_reg_read(dev, dev->BBC->RG_IRQS);

    uint8_t timeout = dev->timeout;
    if (timeout) {
        dev->timeout = 0;
    }

    /* mark AGC Hold bit */
    if (bb_irq_mask & BB_IRQ_AGCH) {
        dev->flags |= AT86RF215_OPT_AGCH;
    }

    /* clear AGC Hold bit */
    if (bb_irq_mask & BB_IRQ_AGCR) {
        dev->flags &= ~AT86RF215_OPT_AGCH;
    }

    /* we got here because of CMSA timeout */
    if (timeout & AT86RF215_TIMEOUT_CSMA) {
        timeout = 0;

        if (!(dev->flags & AT86RF215_OPT_CCATX)) {
            at86rf215_reg_write(dev, dev->RF->RG_EDC, 1);
        } else {
            at86rf215_rf_cmd(dev, CMD_RF_TXPREP);
        }
    }

    /* If the interrupt pin is still high, there was an IRQ on the other radio */
    if (gpio_read(dev->params.int_pin)) {
        if (dev->sibling && dev->sibling->state != AT86RF215_STATE_OFF) {
            netdev->event_callback((netdev_t *) dev->sibling, NETDEV_EVENT_ISR);
        } else {
            _clear_sibling_irq(dev);
        }
    }

    /* Handle Low Battery IRQ */
#if MODULE_AT86RF215_BATMON
    if ((rf_irq_mask & RF_IRQ_BATLOW)) {
        msg_bus_t *bus = sys_bus_get(SYS_BUS_POWER);
        msg_bus_post(bus, SYS_BUS_POWER_EVENT_LOW_VOLTAGE, NULL);
    }
#endif

    /* exit early if the interrupt was not for this interface */
    if (!((bb_irq_mask & bb_irqs_enabled) ||
          (rf_irq_mask & (RF_IRQ_EDC | RF_IRQ_TRXRDY)) || timeout)) {
        return;
    }

    /* check if the received packet has the ACK request bit set */
    bool rx_ack_req;
    if (bb_irq_mask & BB_IRQ_RXFE) {
        rx_ack_req = at86rf215_reg_read(dev, dev->BBC->RG_FBRXS) & IEEE802154_FCF_ACK_REQ;
    } else {
        rx_ack_req = 0;
    }

#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
    /* listen for short preamble in RX */
    if (bb_irq_mask & BB_IRQ_TXFE && dev->fsk_pl) {
        at86rf215_FSK_prepare_rx(dev);
    }
#endif /* MODULE_NETDEV_IEEE802154_MR_FSK */

    if (dev->flags & AT86RF215_OPT_CCA_PENDING) {

        /* Start ED or handle result */
        if (rf_irq_mask & RF_IRQ_EDC) {
            _handle_edc(dev);
        } else if (rf_irq_mask & RF_IRQ_TRXRDY) {
            /* disable baseband for energy detection */
            at86rf215_disable_baseband(dev);
            at86rf215_disable_rpc(dev);
            /* switch to state RX for energy detection */
            at86rf215_rf_cmd(dev, CMD_RF_RX);
            /* start energy measurement */
            at86rf215_reg_write(dev, dev->RF->RG_EDC, 1);
        }

    } else if (dev->flags & AT86RF215_OPT_TX_PENDING) {

        /* start transmitting the frame */
        if (rf_irq_mask & RF_IRQ_TRXRDY) {

#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
            /* send long preamble in TX */
            if (dev->fsk_pl) {
                at86rf215_FSK_prepare_tx(dev);
            }
#endif /* MODULE_NETDEV_IEEE802154_MR_FSK */

            /* automatically switch to RX when TX is done */
            _enable_tx2rx(dev);

            /* only listen for ACK frames */
            if (dev->flags & AT86RF215_OPT_ACK_REQUESTED) {
                at86rf215_filter_ack(dev, true);
            }

            /* switch to state TX */
            dev->state = AT86RF215_STATE_TX;
            dev->flags &= ~AT86RF215_OPT_TX_PENDING;
            at86rf215_rf_cmd(dev, CMD_RF_TX);

            /* This also tells the upper layer about retransmissions - should it be like that? */
            if (netdev->event_callback) {
                netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
            }
        }
    }

    /* CCATX signals medium busy */
    if ((dev->flags & AT86RF215_OPT_CCATX) && (rf_irq_mask & RF_IRQ_EDC) && (bb_irq_mask & BB_IRQ_TXFE)) {
        bb_irq_mask &= ~BB_IRQ_TXFE;
        rf_irq_mask &= ~RF_IRQ_EDC;
        _handle_edc(dev);
    }

    int iter = 0;
    while (timeout || (bb_irq_mask & (BB_IRQ_RXFE | BB_IRQ_TXFE))) {

    /* This should never happen */
    if (++iter > 3) {
        puts("AT86RF215: stuck in ISR");
        printf("\tnum_channels: %d\n", dev->num_chans);
        printf("\tHW: %s\n", at86rf215_hw_state2a(at86rf215_get_rf_state(dev)));
        printf("\tSW: %s\n", at86rf215_sw_state2a(dev->state));
        printf("\trf_irq_mask: %x\n", rf_irq_mask);
        printf("\tbb_irq_mask: %x\n", bb_irq_mask);
        printf("\ttimeout: %x\n", timeout);
        break;
    }

    switch (dev->state) {
    case AT86RF215_STATE_IDLE:

        if (!(bb_irq_mask & (BB_IRQ_RXFE | BB_IRQ_RXAM))) {
            DEBUG("IDLE: only RXFE/RXAM expected (%x)\n", bb_irq_mask);
            break;
        }

        if ((bb_irq_mask & BB_IRQ_RXAM) && netdev->event_callback) {
            /* will be executed in the same thread */
            netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
        }

        bb_irq_mask &= ~BB_IRQ_RXAM;

        if (!(bb_irq_mask & BB_IRQ_RXFE)) {
            break;
        }

        bb_irq_mask &= ~BB_IRQ_RXFE;

        if (netdev->event_callback) {
            /* will be executed in the same thread */
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }

        if (rx_ack_req) {
            dev->state = AT86RF215_STATE_RX_SEND_ACK;
            break;
        }

        _set_idle(dev);

        break;

    case AT86RF215_STATE_RX_SEND_ACK:

        if (!(bb_irq_mask & BB_IRQ_TXFE)) {
            DEBUG("RX_SEND_ACK: only TXFE expected (%x)\n", bb_irq_mask);
            break;
        }

        bb_irq_mask &= ~BB_IRQ_TXFE;

        _set_idle(dev);
        break;

    case AT86RF215_STATE_TX:

        if (!(bb_irq_mask & BB_IRQ_TXFE)) {
            DEBUG("TX: only TXFE expected (%x)\n", bb_irq_mask);
            break;
        }

        bb_irq_mask &= ~BB_IRQ_TXFE;

        if (dev->flags & AT86RF215_OPT_ACK_REQUESTED) {
            dev->state = AT86RF215_STATE_TX_WAIT_ACK;
            _start_ack_timer(dev);
        } else {
            _tx_end(dev, NETDEV_EVENT_TX_COMPLETE);
        }
        break;

    case AT86RF215_STATE_TX_WAIT_ACK:

        if (!((bb_irq_mask & BB_IRQ_RXFE) || timeout)) {
            DEBUG("TX_WAIT_ACK: only RXFE or timeout expected (%x)\n", bb_irq_mask);
            break;
        }

        /* handle timeout case */
        if (!(bb_irq_mask & BB_IRQ_RXFE)) {
            goto timeout;
        }

        bb_irq_mask &= ~BB_IRQ_RXFE;

        if (_ack_frame_received(dev)) {
            timeout = 0;
            xtimer_remove(&dev->timer);
            _tx_end(dev, NETDEV_EVENT_TX_COMPLETE);
            at86rf215_rf_cmd(dev, CMD_RF_RX);
            break;
        }

        /* we got a spurious ACK */
        if (!timeout) {
            at86rf215_rf_cmd(dev, CMD_RF_RX);
            break;
        }

timeout:
       /* For a yet unknown reason, the device spends an excessive amount of time
        * transmitting the preamble in non-legacy modes.
        * This means the calculated ACK timeouts are often too short.
        * To mitigate this, postpone the ACK timeout if the device is still RXign
        * the ACK frame when the timeout expires.
        */
        if (dev->flags & AT86RF215_OPT_AGCH) {
            DEBUG("[at86rf215] Ack timeout postponed\n");
            _start_ack_timer(dev);
        } else {
            DEBUG("[at86rf215] Ack timeout\n");
            _handle_ack_timeout(dev);
        }

        timeout = 0;
        break;
    }
    }
}
