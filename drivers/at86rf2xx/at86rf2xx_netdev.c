/*
 * SPDX-FileCopyrightText: 2018 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2023 Gerson Fernando Budke
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "architecture.h"
#include "iolist.h"

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"

#include "at86rf2xx.h"
#include "at86rf2xx_netdev.h"
#include "at86rf2xx_internal.h"
#include "at86rf2xx_registers.h"
#if IS_USED(IEEE802154_SECURITY)
#include "net/ieee802154_security.h"
#endif
#if IS_USED(MODULE_AT86RF2XX_AES_SPI)
#include "at86rf2xx_aes.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

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

/* Default AT86RF2XX channel */
static const uint16_t at86rf2xx_chan_default = AT86RF2XX_DEFAULT_CHANNEL;

#if IS_USED(MODULE_AT86RF2XX_AES_SPI) && \
    IS_USED(MODULE_IEEE802154_SECURITY)
/**
 * @brief   Pass the 802.15.4 encryption key to the transceiver hardware
 *
 * @param[in] dev               Abstract security device descriptor
 * @param[in] key               Encryption key to be used
 * @param[in] key_size          Size of the encryption key in bytes
 */
static void _at86rf2xx_set_key(ieee802154_sec_dev_t *dev,
                               const uint8_t *key, uint8_t key_size)
{
    (void)key_size;
    at86rf2xx_aes_key_write_encrypt((at86rf2xx_t *)dev->ctx, key);
}

/**
 * @brief   Compute CBC-MAC from IEEE 802.15.4 security context
 *
 * @param[in]       dev         Abstract security device descriptor
 * @param[out]      cipher      Buffer to store cipher blocks
 * @param[in]       iv          Initial vector
 * @param[in]       plain       Input data blocks
 * @param[in]       nblocks     Number of blocks
 */
static void _at86rf2xx_cbc(const ieee802154_sec_dev_t *dev,
                           uint8_t *cipher,
                           uint8_t *iv,
                           const uint8_t *plain,
                           uint8_t nblocks)
{
    at86rf2xx_aes_cbc_encrypt((at86rf2xx_t *)dev->ctx,
                              (aes_block_t *)cipher,
                              NULL,
                              iv,
                              (aes_block_t *)plain,
                              nblocks);
}

/**
 * @brief   Perform ECB encryption
 *
 * @param[in]       dev         Abstract security device descriptor
 * @param[out]      cipher      Output cipher blocks
 * @param[in]       plain       Plain blocks
 * @param[in]       nblocks     Number of blocks
 */
static void _at86rf2xx_ecb(const ieee802154_sec_dev_t *dev,
                           uint8_t *cipher,
                           const uint8_t *plain,
                           uint8_t nblocks)
{
    at86rf2xx_aes_ecb_encrypt((at86rf2xx_t *)dev->ctx,
                              (aes_block_t *)cipher,
                              NULL,
                              (aes_block_t *)plain,
                              nblocks);

}
/**
 * @brief   Struct that contains IEEE 802.15.4 security operations
 *          which are implemented, using the transceiver´s hardware
 *          crypto capabilities
 */
static const ieee802154_radio_cipher_ops_t _at86rf2xx_cipher_ops = {
    .set_key = _at86rf2xx_set_key,
    .ecb = _at86rf2xx_ecb,
    .cbc = _at86rf2xx_cbc
};
#endif /* IS_USED(MODULE_AT86RF2XX_AES_SPI) && \
          IS_USED(MODULE_IEEE802154_SECURITY) */

/* SOC has radio interrupts, store reference to netdev */
static netdev_t *at86rfmega_dev;
static void _irq_handler(void *arg)
{
    netdev_trigger_event_isr(arg);
}

static int _init(netdev_t *netdev)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);

    if (AT86RF2XX_IS_PERIPH) {
        at86rfmega_dev = netdev;
    }
    else {
        at86rf2xx_spi_init(dev, _irq_handler);
    }

    /* reset hardware into a defined state */
    at86rf2xx_hardware_reset(dev);

    /* test if the device is responding */
    if (at86rf2xx_reg_read(dev, AT86RF2XX_REG__PART_NUM) != AT86RF2XX_PARTNUM) {
        DEBUG("[at86rf2xx] error: unable to read correct part number\n");
        return -ENOTSUP;
    }

    /* reset device to default values and put it into RX state */
    netdev_ieee802154_reset(&dev->netdev);
    at86rf2xx_set_addr_long(dev, (eui64_t *)dev->netdev.long_addr);
    at86rf2xx_set_addr_short(dev, (network_uint16_t *)dev->netdev.short_addr);

    /* `netdev_ieee802154_reset` does not set the default channel. */
    netdev_ieee802154_set(&dev->netdev, NETOPT_CHANNEL,
                          &at86rf2xx_chan_default, sizeof(at86rf2xx_chan_default));

    if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
        static const netopt_enable_t ack_req =
            IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_ACK_REQ) ? NETOPT_ENABLE : NETOPT_DISABLE;
        netdev_ieee802154_set(&dev->netdev, NETOPT_ACK_REQ,
                              &ack_req, sizeof(ack_req));
    }
#if IS_USED(MODULE_IEEE802154_SECURITY) && \
    IS_USED(MODULE_AT86RF2XX_AES_SPI)
    dev->netdev.sec_ctx.dev.cipher_ops = &_at86rf2xx_cipher_ops;
    dev->netdev.sec_ctx.dev.ctx = dev;
#endif

    at86rf2xx_reset(dev);

    /* Initialize CSMA seed with hardware address */
    at86rf2xx_set_csma_seed(dev, dev->netdev.long_addr);

    /* signal link UP */
    netdev->event_callback(netdev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    size_t len = 0;

    at86rf2xx_tx_prepare(dev);

    /* load packet data into FIFO */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        /* current packet data + FCS too long */
        if ((len + iol->iol_len + 2) > AT86RF2XX_MAX_PKT_LENGTH) {
            DEBUG("[at86rf2xx] error: packet too large (%" PRIuSIZE
                  " byte) to be send\n", len + 2);
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            len = at86rf2xx_tx_load(dev, iol->iol_base, iol->iol_len, len);
        }
    }

    /* send data out directly if pre-loading id disabled */
    if (!(dev->flags & AT86RF2XX_OPT_PRELOADING)) {
        at86rf2xx_tx_exec(dev);
        if (netdev->event_callback) {
            netdev->event_callback(netdev, NETDEV_EVENT_TX_STARTED);
        }
    }
    /* return the number of bytes that were actually loaded into the frame
     * buffer/send out */
    return (int)len;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    uint8_t phr;
    size_t pkt_len;

    /* frame buffer protection will be unlocked as soon as at86rf2xx_fb_stop() is called,
     * Set receiver to PLL_ON state to be able to free the SPI bus and avoid losing data. */
    at86rf2xx_set_state(dev, AT86RF2XX_STATE_PLL_ON);

    /* start frame buffer access */
    at86rf2xx_fb_start(dev);

    /* get the size of the received packet */
    phr = at86rf2xx_get_rx_len(dev);

    /* ignore MSB (refer p.80) and subtract length of FCS field */
    pkt_len = (phr & 0x7f) - 2;

    /* return length when buf == NULL */
    if (buf == NULL) {
        /* release SPI bus */
        at86rf2xx_fb_stop(dev);

        /* drop packet, continue receiving */
        if (len > 0) {
            /* set device back in operation state which was used before last transmission.
             * This state is saved in at86rf2xx.c/at86rf2xx_tx_prepare() e.g RX_AACK_ON */
            at86rf2xx_set_state(dev, dev->idle_state);
        }

        return pkt_len;
    }

    /* not enough space in buf */
    if (pkt_len > len) {
        at86rf2xx_fb_stop(dev);
        /* set device back in operation state which was used before last transmission.
         * This state is saved in at86rf2xx.c/at86rf2xx_tx_prepare() e.g RX_AACK_ON */
        at86rf2xx_set_state(dev, dev->idle_state);
        return -ENOBUFS;
    }
    /* copy payload */
    at86rf2xx_fb_read(dev, (uint8_t *)buf, pkt_len);

    /* Ignore FCS but advance fb read - we must give a temporary buffer here,
     * as we are not allowed to issue SPI transfers without any buffer */
    uint8_t tmp[2];
    at86rf2xx_fb_read(dev, tmp, 2);
    (void)tmp;

    /* AT86RF212B RSSI_BASE_VAL + 1.03 * ED, base varies for diff. modulation and datarates
     * AT86RF232  RSSI_BASE_VAL + ED, base -91dBm
     * AT86RF233  RSSI_BASE_VAL + ED, base -94dBm
     * AT86RF231  RSSI_BASE_VAL + ED, base -91dBm
     * AT86RFA1   RSSI_BASE_VAL + ED, base -90dBm
     * AT86RFR2   RSSI_BASE_VAL + ED, base -90dBm
     *
     * AT86RF231 MAN. p.92, 8.4.3 Data Interpretation
     * AT86RF232 MAN. p.91, 8.4.3 Data Interpretation
     * AT86RF233 MAN. p.102, 8.5.3 Data Interpretation
     *
     * for performance reasons we ignore the 1.03 scale factor on the 212B,
     * which causes a slight error in the values, but the accuracy of the ED
     * value is specified as +/- 5 dB, so it should not matter very much in real
     * life.
     */
    if (info != NULL) {
        uint8_t ed = 0;
        netdev_ieee802154_rx_info_t *radio_info = info;
        at86rf2xx_fb_read(dev, &(radio_info->lqi), 1);

#if AT86RF2XX_HAVE_ED_REGISTER
        /* AT86RF231 does not provide ED at the end of the frame buffer, read
         * from separate register instead */
        at86rf2xx_fb_stop(dev);
        ed = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_ED_LEVEL);
#else
        at86rf2xx_fb_read(dev, &ed, 1);
        at86rf2xx_fb_stop(dev);
#endif
        radio_info->rssi = RSSI_BASE_VAL + ed;
        DEBUG("[at86rf2xx] LQI:%d high is good, RSSI:%d high is either good or "
              "too much interference.\n", radio_info->lqi, radio_info->rssi);
#if AT86RF2XX_IS_PERIPH && IS_USED(MODULE_NETDEV_IEEE802154_RX_TIMESTAMP)
        /* AT86RF2XX_IS_PERIPH means the MCU is ATmegaRFR2 that has symbol counter */
        {
            uint32_t rx_sc;
            rx_sc = at86rf2xx_get_sc(dev);

            /* convert counter value to ns */
            uint64_t res = SC_TO_NS * (uint64_t)rx_sc;
            netdev_ieee802154_rx_info_set_timestamp(radio_info, res);
            DEBUG("[at86rf2xx] CS: %" PRIu32 " timestamp: %" PRIu32 ".%09" PRIu32 " ",
                    rx_sc, (uint32_t)(radio_info->timestamp / NS_PER_SEC),
                           (uint32_t)(radio_info->timestamp % NS_PER_SEC));
        }
#endif

    }
    else {
        at86rf2xx_fb_stop(dev);
    }

    /* set device back in operation state which was used before last transmission.
     * This state is saved in at86rf2xx.c/at86rf2xx_tx_prepare() e.g RX_AACK_ON */
    at86rf2xx_set_state(dev, dev->idle_state);

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
            at86rf2xx_set_state(dev, AT86RF2XX_PHY_STATE_RX);
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
                at86rf2xx_set_state(dev, AT86RF2XX_PHY_STATE_TX);
                at86rf2xx_tx_exec(dev);
                if (dev->netdev.netdev.event_callback) {
                    dev->netdev.netdev.event_callback(&dev->netdev.netdev, NETDEV_EVENT_TX_STARTED);
                }
            }
            break;
        case NETOPT_STATE_RESET:
            at86rf2xx_hardware_reset(dev);
            netdev_ieee802154_reset(&dev->netdev);
            /* set short and long address */
            at86rf2xx_set_addr_long(dev, (eui64_t *)dev->netdev.long_addr);
            at86rf2xx_set_addr_short(dev, (network_uint16_t *)dev->netdev.short_addr);

            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                static const netopt_enable_t ack_req =
                    IS_ACTIVE(CONFIG_IEEE802154_DEFAULT_ACK_REQ) ? NETOPT_ENABLE : NETOPT_DISABLE;
                netdev_ieee802154_set(&dev->netdev, NETOPT_ACK_REQ,
                                      &ack_req, sizeof(ack_req));
            }
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
        case AT86RF2XX_PHY_STATE_RX_BUSY:
            return NETOPT_STATE_RX;
        case AT86RF2XX_PHY_STATE_TX:
        case AT86RF2XX_PHY_STATE_TX_BUSY:
            return NETOPT_STATE_TX;
        case AT86RF2XX_PHY_STATE_RX:
        default:
            return NETOPT_STATE_IDLE;
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);

    if (netdev == NULL) {
        return -ENODEV;
    }

    /* getting these options doesn't require the transceiver to be responsive */
    switch (opt) {
#if AT86RF2XX_HAVE_SUBGHZ
        case NETOPT_CHANNEL_PAGE:
            assert(max_len >= sizeof(uint16_t));
            ((uint8_t *)val)[1] = 0;
            ((uint8_t *)val)[0] = dev->page;
            return sizeof(uint16_t);
#endif

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
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                if (dev->flags & AT86RF2XX_OPT_PROMISCUOUS) {
                    *((netopt_enable_t *)val) = NETOPT_ENABLE;
                }
                else {
                    *((netopt_enable_t *)val) = NETOPT_DISABLE;
                }
                return sizeof(netopt_enable_t);
            }
            break;

        case NETOPT_RX_START_IRQ:
        case NETOPT_TX_START_IRQ:
        case NETOPT_TX_END_IRQ:
            *((netopt_enable_t *)val) = NETOPT_ENABLE;
            return sizeof(netopt_enable_t);

        case NETOPT_CSMA:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                *((netopt_enable_t *)val) =
                    !!(dev->flags & AT86RF2XX_OPT_CSMA);
                return sizeof(netopt_enable_t);
            }
            break;

/* Only radios with the XAH_CTRL_2 register support frame retry reporting */
#if AT86RF2XX_HAVE_RETRIES
        case NETOPT_TX_RETRIES_NEEDED:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                assert(max_len >= sizeof(uint8_t));
                *((uint8_t *)val) = dev->tx_retries;
                return sizeof(uint8_t);
            }
            break;
#endif

        default:
            /* Can still be handled in second switch */
            break;
    }

    int res;

    if (((res = netdev_ieee802154_get(container_of(netdev, netdev_ieee802154_t, netdev),
                                      opt, val, max_len)) >= 0)
        || (res != -ENOTSUP)) {
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
            *((uint16_t *)val) = netdev_ieee802154->txpower;
            res = sizeof(uint16_t);
            break;

        case NETOPT_RETRANS:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                assert(max_len >= sizeof(uint8_t));
                *((uint8_t *)val) = at86rf2xx_get_max_retries(dev);
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_CSMA_RETRIES:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                assert(max_len >= sizeof(uint8_t));
                *((uint8_t *)val) = at86rf2xx_get_csma_max_retries(dev);
                res = sizeof(uint8_t);
            }
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
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                assert(max_len >= sizeof(netopt_enable_t));
                uint8_t tmp = at86rf2xx_reg_read(dev, AT86RF2XX_REG__CSMA_SEED_1);
                *((netopt_enable_t *)val) = (tmp & AT86RF2XX_CSMA_SEED_1__AACK_DIS_ACK)
                                          ? false : true;
                res = sizeof(netopt_enable_t);
            }
            break;

#ifdef MODULE_NETDEV_IEEE802154_OQPSK

        case NETOPT_IEEE802154_PHY:
            assert(max_len >= sizeof(int8_t));
            *(uint8_t *)val = at86rf2xx_get_phy_mode(dev);
            return sizeof(uint8_t);

        case NETOPT_OQPSK_RATE:
            assert(max_len >= sizeof(int8_t));
            *(uint8_t *)val = at86rf2xx_get_rate(dev);
            return sizeof(uint8_t);

#endif /* MODULE_NETDEV_IEEE802154_OQPSK */
#if AT86RF2XX_RANDOM_NUMBER_GENERATOR
        case NETOPT_RANDOM:
            at86rf2xx_get_random(dev, (uint8_t*)val, max_len);
            return max_len;
#endif
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
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    if (dev == NULL) {
        return -ENODEV;
    }
    uint8_t old_state = at86rf2xx_get_status(dev);
    int res = -ENOTSUP;

    /* temporarily wake up if sleeping and opt != NETOPT_STATE.
     * opt != NETOPT_STATE check prevents redundant wake-up.
     * when opt == NETOPT_STATE, at86rf2xx_set_state() will wake up the
     * radio if needed. */
    if ((old_state == AT86RF2XX_STATE_SLEEP) && (opt != NETOPT_STATE)) {
        at86rf2xx_assert_awake(dev);
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(len == sizeof(network_uint16_t));
            memcpy(dev->netdev.short_addr, val, len);
#ifdef MODULE_SIXLOWPAN
            /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
             * 0 for unicast addresses */
            dev->netdev.short_addr[0] &= 0x7F;
#endif
            at86rf2xx_set_addr_short(dev, val);
            /* don't set res to set netdev_ieee802154_t::short_addr */
            break;
        case NETOPT_ADDRESS_LONG:
            assert(len == sizeof(eui64_t));
            memcpy(dev->netdev.long_addr, val, len);
            at86rf2xx_set_addr_long(dev, val);
            /* don't set res to set netdev_ieee802154_t::long_addr */
            break;
        case NETOPT_NID:
            assert(len == sizeof(uint16_t));
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
            dev->netdev.chan = chan;
#if AT86RF2XX_HAVE_SUBGHZ
            at86rf2xx_configure_phy(dev, chan, dev->page, dev->netdev.txpower);
#else
            at86rf2xx_configure_phy(dev, chan, 0, dev->netdev.txpower);
#endif
            /* don't set res to set netdev_ieee802154_t::chan */
            break;

        case NETOPT_CHANNEL_PAGE:
            assert(len == sizeof(uint16_t));
            uint8_t page = (((const uint16_t *)val)[0]) & UINT8_MAX;
#if AT86RF2XX_HAVE_SUBGHZ
            if ((page != 0) && (page != 2)) {
                res = -EINVAL;
            }
            else {
                dev->page = page;
                at86rf2xx_configure_phy(dev, dev->netdev.chan, page, dev->netdev.txpower);
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
            netdev_ieee802154->txpower = *((const int16_t *)val);
#if AT86RF2XX_HAVE_SUBGHZ
            at86rf2xx_configure_phy(dev, dev->netdev.chan, dev->page, *((const int16_t *)val));
#else
            at86rf2xx_configure_phy(dev, dev->netdev.chan, 0, *((const int16_t *)val));
#endif
            res = sizeof(uint16_t);
            break;

        case NETOPT_STATE:
            assert(len <= sizeof(netopt_state_t));
            res = _set_state(dev, *((const netopt_state_t *)val));
            break;

        case NETOPT_AUTOACK:

            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                at86rf2xx_set_option(dev, AT86RF2XX_OPT_AUTOACK,
                                     ((const bool *)val)[0]);
                res = sizeof(netopt_enable_t);
            }
            break;

        case NETOPT_ACK_PENDING:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                at86rf2xx_set_option(dev, AT86RF2XX_OPT_ACK_PENDING,
                                     ((const bool *)val)[0]);
                res = sizeof(netopt_enable_t);
            }
            break;

        case NETOPT_RETRANS:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                assert(len <= sizeof(uint8_t));
                at86rf2xx_set_max_retries(dev, *((const uint8_t *)val));
                res = sizeof(uint8_t);
            }
            break;

        case NETOPT_PRELOADING:
            at86rf2xx_set_option(dev, AT86RF2XX_OPT_PRELOADING,
                                 ((const bool *)val)[0]);
            res = sizeof(netopt_enable_t);
            break;

        case NETOPT_PROMISCUOUSMODE:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                at86rf2xx_set_option(dev, AT86RF2XX_OPT_PROMISCUOUS,
                                     ((const bool *)val)[0]);
                res = sizeof(netopt_enable_t);
            }
            break;

        case NETOPT_CSMA:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
                at86rf2xx_set_option(dev, AT86RF2XX_OPT_CSMA,
                                     ((const bool *)val)[0]);
                res = sizeof(netopt_enable_t);
            }
            break;

        case NETOPT_CSMA_RETRIES:
            if (!IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
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
            }
            break;

        case NETOPT_CCA_THRESHOLD:
            assert(len <= sizeof(int8_t));
            at86rf2xx_set_cca_threshold(dev, *((const int8_t *)val));
            res = sizeof(int8_t);
            break;

#ifdef MODULE_NETDEV_IEEE802154_OQPSK

        case NETOPT_OQPSK_RATE:
            assert(len <= sizeof(int8_t));
            if (at86rf2xx_set_rate(dev, *((const uint8_t *)val)) < 0) {
                res = -EINVAL;
            } else {
                res = sizeof(uint8_t);
            }
            break;

#endif /* MODULE_NETDEV_IEEE802154_OQPSK */
        default:
            break;
    }

    /* go back to sleep if were sleeping and state hasn't been changed */
    if ((old_state == AT86RF2XX_STATE_SLEEP)
        && (opt != NETOPT_STATE)) {
        at86rf2xx_set_state(dev, AT86RF2XX_STATE_SLEEP);
    }

    if (res == -ENOTSUP) {
        res = netdev_ieee802154_set(container_of(netdev, netdev_ieee802154_t, netdev),
                                    opt, val, len);
    }

    return res;
}

static void _isr_send_complete(at86rf2xx_t *dev, uint8_t trac_status)
{
    netdev_t *netdev = &dev->netdev.netdev;
    if (IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
        return;
    }
/* Only radios with the XAH_CTRL_2 register support frame retry reporting */
#if AT86RF2XX_HAVE_RETRIES_REG
    dev->tx_retries = (at86rf2xx_reg_read(dev, AT86RF2XX_REG__XAH_CTRL_2)
                       & AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_MASK) >>
                      AT86RF2XX_XAH_CTRL_2__ARET_FRAME_RETRIES_OFFSET;
#endif

    DEBUG("[at86rf2xx] EVT - TX_END\n");

    if (netdev->event_callback) {
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

static inline void _isr_recv_complete(netdev_t *netdev)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    if (!netdev->event_callback) {
        return;
    }
    if (IS_ACTIVE(AT86RF2XX_BASIC_MODE)) {
        uint8_t phy_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__PHY_RSSI);
        bool crc_ok = phy_status & AT86RF2XX_PHY_RSSI_MASK__RX_CRC_VALID;

        if (crc_ok) {
            netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
        else {
            netdev->event_callback(netdev, NETDEV_EVENT_CRC_ERROR);
        }
    }
    else {
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

static void _isr(netdev_t *netdev)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
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
    irq_mask = at86rf2xx_get_irq_flags(dev);

    trac_status = at86rf2xx_reg_read(dev, AT86RF2XX_REG__TRX_STATE)
                  & AT86RF2XX_TRX_STATE_MASK__TRAC;

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__RX_START) {
        netdev->event_callback(netdev, NETDEV_EVENT_RX_STARTED);
        DEBUG("[at86rf2xx] EVT - RX_START\n");
    }

    if (irq_mask & AT86RF2XX_IRQ_STATUS_MASK__TRX_END) {
        if ((state == AT86RF2XX_PHY_STATE_RX)
            || (state == AT86RF2XX_PHY_STATE_RX_BUSY)) {
            DEBUG("[at86rf2xx] EVT - RX_END\n");

            _isr_recv_complete(netdev);

        }
        else if (state == AT86RF2XX_PHY_STATE_TX) {
            /* check for more pending TX calls and return to idle state if
             * there are none */
            assert(dev->pending_tx != 0);
            /* Radio is idle, any TX transaction is done */
            dev->pending_tx = 0;
            at86rf2xx_set_state(dev, dev->idle_state);
            DEBUG("[at86rf2xx] return to idle state 0x%x\n", dev->idle_state);
            _isr_send_complete(dev, trac_status);
        }
        /* Only the case when an interrupt was received and the radio is busy
         * with a next PDU transmission when _isr is called.
         * dev->pending == 1 means a receive and immediately a send happened.
         * The receive is discarded as the send already overwrote the internal
         * buffer.
         * dev->pending == 2 means two transmits occurred and this is the isr for
         * the first.
         */
        else if (state == AT86RF2XX_PHY_STATE_TX_BUSY) {
            if (dev->pending_tx > 1) {
                dev->pending_tx--;
                _isr_send_complete(dev, trac_status);
            }
        }
    }
}

void at86rf2xx_setup(at86rf2xx_t *dev, const at86rf2xx_params_t *params, uint8_t index)
{
    netdev_t *netdev = &dev->netdev.netdev;

    netdev->driver = &at86rf2xx_driver;
    /* State to return after receiving or transmitting */
    dev->idle_state = AT86RF2XX_STATE_TRX_OFF;
    /* radio state is P_ON when first powered-on */
    dev->state = AT86RF2XX_STATE_P_ON;
    dev->pending_tx = 0;

#if AT86RF2XX_IS_PERIPH
    (void) params;
    /* set all interrupts off */
    at86rf2xx_reg_write(dev, AT86RF2XX_REG__IRQ_MASK, 0x00);
#else
    /* initialize device descriptor */
    dev->params = *params;
#endif

    netdev_register(netdev, NETDEV_AT86RF2XX, index);
    /* set device address */
    netdev_ieee802154_setup(&dev->netdev);
}

#if AT86RF2XX_IS_PERIPH

/**
 * @brief ISR for transceiver's TX_START interrupt
 *
 * In procedure TX_ARET the TRX24_TX_START interrupt is issued separately for every
 * frame transmission and frame retransmission.
 * Indicates the frame start of a transmitted acknowledge frame in procedure RX_AACK.
 *
 * Flow Diagram Manual p. 52 / 63
 */
#if AT86RF2XX_HAVE_RETRIES
ISR(TRX24_TX_START_vect){
    /* __enter_isr(); is not necessary as there is nothing which causes a
     * thread_yield and the interrupt can not be interrupted by an other ISR */

    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);

    dev->tx_retries++;
}
#endif

/**
 * @brief  Transceiver PLL Lock
 *
 *  Is triggered when PLL locked successfully.
 *
 * Manual p. 40
 */
static inline void txr24_pll_lock_handler(void)
{
    DEBUG("TRX24_PLL_LOCK\n");
    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__PLL_LOCK;
}
AVR8_ISR(TRX24_PLL_LOCK_vect, txr24_pll_lock_handler);

/**
 * @brief  Transceiver PLL Unlock
 *
 *  Is triggered when PLL unlocked unexpectedly.
 *
 * Manual p. 89
 */
static inline void txr24_pll_unlock_handler(void)
{
    DEBUG("TRX24_PLL_UNLOCK\n");
    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__PLL_UNLOCK;
}
AVR8_ISR(TRX24_PLL_UNLOCK_vect, txr24_pll_unlock_handler);

/**
 * @brief ISR for transceiver's receive start interrupt
 *
 *  Is triggered when valid PHR is detected.
 *  Save IRQ status and inform upper layer of data reception.
 *
 * Flow Diagram Manual p. 52 / 63
 */
static inline void txr24_rx_start_handler(void)
{
    uint8_t status = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_RX_START 0x%x\n", status);

    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__RX_START;
    /* Call upper layer to process valid PHR */
    netdev_trigger_event_isr(at86rfmega_dev);
}
AVR8_ISR(TRX24_RX_START_vect, txr24_rx_start_handler);

/**
 * @brief ISR for transceiver's receive end interrupt
 *
 *  Is triggered when valid data is received. FCS check passed.
 *  Save IRQ status and inform upper layer of data reception.
 *
 * Flow Diagram Manual p. 52 / 63
 */
static inline void txr24_rx_end_handler(void)
{
    uint8_t status = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_RX_END 0x%x\n", status);

    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__RX_END;
    /* Call upper layer to process received data */
    netdev_trigger_event_isr(at86rfmega_dev);
}
AVR8_ISR(TRX24_RX_END_vect, txr24_rx_end_handler);

/**
 * @brief  Transceiver CCAED Measurement finished
 *
 *  Is triggered when CCA or ED measurement completed.
 *
 * Manual p. 74 and p. 76
 */
static inline void txr24_cca_ed_done_handler(void)
{
    DEBUG("TRX24_CCA_ED_DONE\n");
    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__CCA_ED_DONE;
}
AVR8_ISR(TRX24_CCA_ED_DONE_vect, txr24_cca_ed_done_handler);

/**
 * @brief  Transceiver Frame Address Match, indicates incoming frame
 *
 *  Is triggered when Frame with valid Address is received.
 *  Can be used to wake up MCU from sleep, etc.
 *
 * Flow Diagram Manual p. 52 / 63
 */
static inline void txr24_xah_ami_handler(void)
{
    DEBUG("TRX24_XAH_AMI\n");
    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__AMI;
}
AVR8_ISR(TRX24_XAH_AMI_vect, txr24_xah_ami_handler);

/**
 * @brief ISR for transceiver's transmit end interrupt
 *
 *  Is triggered when data or when acknowledge frames where send.
 *
 * Flow Diagram Manual p. 52 / 63
 */
static inline void txr24_tx_end_handler(void)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    uint8_t status = *AT86RF2XX_REG__TRX_STATE & AT86RF2XX_TRX_STATUS_MASK__TRX_STATUS;
    DEBUG("TRX24_TX_END 0x%x\n", status);

    /* only inform upper layer when a transmission was done,
     * not for sending acknowledge frames if data was received. */
    if (status != AT86RF2XX_PHY_STATE_RX) {
        dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__TX_END;

        /* Call upper layer to process if data was send successful */
        netdev_trigger_event_isr(at86rfmega_dev);
    }
}
AVR8_ISR(TRX24_TX_END_vect, txr24_tx_end_handler);

/**
 * @brief ISR for transceiver's wakeup finished interrupt
 *
 *  Is triggered when transceiver module reset is finished.
 *  Save IRQ status and inform upper layer the transceiver is operational.
 *
 * Manual p. 40
 */
static inline void txr24_awake_handler(void)
{
    DEBUG("TRX24_AWAKE\n");

    netdev_ieee802154_t *netdev_ieee802154 = container_of(at86rfmega_dev,
                          netdev_ieee802154_t, netdev);
    at86rf2xx_t *dev = container_of(netdev_ieee802154, at86rf2xx_t, netdev);
    dev->irq_status |= AT86RF2XX_IRQ_STATUS_MASK__AWAKE;
    /* Call upper layer to process transceiver wakeup finished */
    netdev_trigger_event_isr(at86rfmega_dev);
}
AVR8_ISR(TRX24_AWAKE_vect, txr24_awake_handler);

#endif /* AT86RF2XX_IS_PERIPH */
