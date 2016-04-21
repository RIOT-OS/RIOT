/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_xbee
 * @{
 *
 * @file
 * @brief       High-level driver implementation for the XBee S1 802.15.4 modem
 *
 * @author      Kévin Roussel <kevin.roussel@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "xbee.h"
#include "xtimer.h"
#include "msg.h"
#include "net/eui64.h"
#include "net/ieee802154.h"
#include "periph/cpuid.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Internal driver event type when RX is finished
 */
#define ISR_EVENT_RX_DONE           (0x0001)

/**
 * @brief   Delay when entering command mode, must be > 1s
 */
#define ENTER_CMD_MODE_DELAY        (1100U * 1000U)
/**
 * @brief   Delay when resetting the device, 10ms
 */
#define RESET_DELAY                 (10U * 1000U)

/**
 * @brief   Timeout for receiving AT command response
 */
#define RESP_TIMEOUT_USEC           (SEC_IN_USEC)

/**
 * @brief   Start delimiter in API frame mode
 */
#define API_START_DELIMITER         (0x7e)

/**
 * @brief   Command IDs when communicating in API frame mode
 * @{
 */
#define API_ID_MODEM_STATUS         (0x8a)  /**< modem status frame */
#define API_ID_AT                   (0x08)  /**< AT command request frame */
#define API_ID_AT_QUEUE             (0x09)  /**< queued AT command frame */
#define API_ID_AT_RESP              (0x88)  /**< AT command response frame */
#define API_ID_TX_LONG_ADDR         (0x00)  /**< TX frame (long address) */
#define API_ID_TX_SHORT_ADDR        (0x01)  /**< TX frame (short address) */
#define API_ID_TX_RESP              (0x89)  /**< TX response frame */
#define API_ID_RX_LONG_ADDR         (0x80)  /**< RX frame (long address) */
#define API_ID_RX_SHORT_ADDR        (0x81)  /**< RX frame (short address) */
/** @} */

/**
 * @brief   Internal option flags (to be expanded if needed)
 * @{
 */
#define OPT_DIS_AUTO_ACK            (0x01)  /**< disable sending of auto ACKs */
/** @} */

/**
 * @brief   Data-structure describing AT command response frames
 */
typedef struct {
    uint8_t status;         /**< AT command response status, 0 for success */
    uint8_t data[8];        /**< returned data from the AT command */
    uint8_t data_len;       /**< number ob bytes written to @p data */
} resp_t;


/*
 * Driver's internal utility functions
 */
static uint8_t _cksum(uint8_t *buf, size_t size)
{
    uint8_t res = 0xff;
    for (size_t i = 3; i < size; i++) {
        res -= buf[i];
    }
    return res;
}

static void _at_cmd(xbee_t *dev, const char *cmd)
{
    DEBUG("xbee: AT_CMD: %s\n", cmd);

    uart_write(dev->uart, (uint8_t *)cmd, strlen(cmd));
}

static void isr_resp_timeout(void *arg)
{
    xbee_t *dev = (xbee_t *)arg;

    if (mutex_trylock(&(dev->resp_lock)) == 0) {
        dev->int_state = XBEE_INT_STATE_IDLE;
    }

    mutex_unlock(&(dev->resp_lock));
}

static void _api_at_cmd(xbee_t *dev, uint8_t *cmd, uint8_t size, resp_t *resp)
{
    DEBUG("xbee: AT_CMD: %s\n", cmd);

    /* acquire TX lock */
    mutex_lock(&(dev->tx_lock));
    /* construct API frame */
    dev->tx_buf[0] = API_START_DELIMITER;
    dev->tx_buf[1] = (size + 2) >> 8;
    dev->tx_buf[2] = (size + 2) & 0xff;
    dev->tx_buf[3] = API_ID_AT;
    dev->tx_buf[4] = 1;             /* use fixed frame id */
    memcpy(dev->tx_buf + 5, cmd, size);
    dev->tx_buf[size + 5] = _cksum(dev->tx_buf, size + 5);

    /* reset the response data counter */
    dev->resp_count = 0;
    /* start send data */
    uart_write(dev->uart, dev->tx_buf, size + 6);

    uint64_t sent_time = xtimer_now64();

    xtimer_t resp_timer;

    resp_timer.callback = isr_resp_timeout;
    resp_timer.arg = dev;

    xtimer_set(&resp_timer, RESP_TIMEOUT_USEC);

    /* wait for results */
    while ((dev->resp_limit != dev->resp_count) &&
           (xtimer_now64() - sent_time < RESP_TIMEOUT_USEC)) {
        mutex_lock(&(dev->resp_lock));
    }

    xtimer_remove(&resp_timer);

    if (dev->resp_limit != dev->resp_count) {
        DEBUG("xbee: response timeout\n");
        resp->status = 255;
        mutex_unlock(&(dev->tx_lock));

        return;
    }

    /* populate response data structure */
    resp->status = dev->resp_buf[3];
    resp->data_len = dev->resp_limit - 5;
    if (resp->data_len > 0) {
        memcpy(resp->data, &(dev->resp_buf[4]), resp->data_len);
    }
    mutex_unlock(&(dev->tx_lock));
}

/*
 * Interrupt callbacks
 */
static void _rx_cb(void *arg, uint8_t c)
{
    xbee_t *dev = (xbee_t *)arg;
    msg_t msg;

    switch (dev->int_state) {
        case XBEE_INT_STATE_IDLE:
            /* check for beginning of new data frame */
            if (c == API_START_DELIMITER) {
                dev->int_state = XBEE_INT_STATE_SIZE1;
            }
            break;
        case XBEE_INT_STATE_SIZE1:
            dev->int_size = ((uint16_t)c) << 8;
            dev->int_state = XBEE_INT_STATE_SIZE2;
            break;
        case XBEE_INT_STATE_SIZE2:
            dev->int_size += c;
            dev->int_state = XBEE_INT_STATE_TYPE;
            break;
        case XBEE_INT_STATE_TYPE:
            if (c == API_ID_RX_SHORT_ADDR || c == API_ID_RX_LONG_ADDR) {
                /* in case old data was not processed, ignore incoming data */
                if (dev->rx_count != 0) {
                    dev->int_state = XBEE_INT_STATE_IDLE;
                    return;
                }
                dev->rx_limit = dev->int_size + 1;
                dev->rx_buf[dev->rx_count++] = c;
                dev->int_state = XBEE_INT_STATE_RX;
            }
            else if (c == API_ID_AT_RESP) {
                dev->resp_limit = dev->int_size;
                dev->int_state = XBEE_INT_STATE_RESP;
            }
            else {
                dev->int_state = XBEE_INT_STATE_IDLE;
            }
            break;
        case XBEE_INT_STATE_RESP:
            dev->resp_buf[dev->resp_count++] = c;
            if (dev->resp_count == dev->resp_limit) {
                /* here we ignore the checksum to prevent deadlocks */
                mutex_unlock(&(dev->resp_lock));
                dev->int_state = XBEE_INT_STATE_IDLE;
            }
            break;
        case XBEE_INT_STATE_RX:
            dev->rx_buf[dev->rx_count++] = c;
            if (dev->rx_count == dev->rx_limit) {
                /* packet is complete */
                msg.type = GNRC_NETDEV_MSG_TYPE_EVENT;
                msg.content.value = ISR_EVENT_RX_DONE;
                msg_send_int(&msg, dev->mac_pid);
                dev->int_state = XBEE_INT_STATE_IDLE;
            }
            break;
        default:
            /* this should never be the case */
            break;
    }
}

/*
 * Getter and setter functions
 */

static int _get_addr_short(xbee_t *dev, uint8_t *val, size_t len)
{
    uint8_t cmd[2];
    resp_t resp;

    if (len < 2) {
        return -EOVERFLOW;
    }

    cmd[0] = 'M';
    cmd[1] = 'Y';
    _api_at_cmd(dev, cmd, 2, &resp);
    if (resp.status == 0) {
        memcpy(val, resp.data, IEEE802154_SHORT_ADDRESS_LEN);
        return IEEE802154_SHORT_ADDRESS_LEN;
    }
    return -ECANCELED;
}

static int _get_addr_long(xbee_t *dev, uint8_t *val, size_t len)
{
    uint8_t cmd[2];
    resp_t resp;

    if (len < IEEE802154_LONG_ADDRESS_LEN) {
        return -EOVERFLOW;
    }

    /* read 4 high byte - AT command: SH*/
    cmd[0] = 'S';
    cmd[1] = 'H';
    _api_at_cmd(dev, cmd, 2, &resp);
    if (resp.status == 0) {
        memcpy(val, resp.data, 4);
    }
    else {
        return -ECANCELED;
    }
    /* read next 4 byte - AT command: SL */
    cmd[1] = 'L';
    _api_at_cmd(dev, cmd, 2, &resp);
    if (resp.status == 0) {
        memcpy(val + 4, resp.data, 4);
        return IEEE802154_LONG_ADDRESS_LEN;
    }
    return -ECANCELED;
}

static int _set_short_addr(xbee_t *dev, uint8_t *address)
{
    uint8_t cmd[4];
    resp_t resp;

    cmd[0] = 'M';
    cmd[1] = 'Y';
    cmd[2] = address[0];
    cmd[3] = address[1];
    _api_at_cmd(dev, cmd, 4, &resp);

    return resp.status;
}

static int _set_addr(xbee_t *dev, uint8_t *val, size_t len)
{
    uint8_t addr[2];

    /* device only supports setting the short address */
    if (len != 2) {
        return -ENOTSUP;
    }

    addr[0] = val[0];
    addr[1] = val[1];

#ifdef MODULE_SIXLOWPAN
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit
     * to 0 for unicast addresses */
    addr[1] &= 0x7F;
#endif

    if (dev->addr_flags & XBEE_ADDR_FLAGS_LONG ||
          _set_short_addr(dev, addr) == 0) {

        memcpy(dev->addr_short, addr, 2);

        return 2;
    }

    return -ECANCELED;
}

static int _set_addr_len(xbee_t *dev, uint16_t *val, size_t len)
{
    if (len != sizeof(uint16_t)) {
        return -EOVERFLOW;
    }

    switch (*val) {
        case IEEE802154_LONG_ADDRESS_LEN:
            dev->addr_flags |= XBEE_ADDR_FLAGS_LONG;

            /* disable short address */
            uint8_t disabled_addr[] = { 0xFF, 0xFF };

            _set_short_addr(dev, disabled_addr);

            break;
        case IEEE802154_SHORT_ADDRESS_LEN:
            dev->addr_flags &= ~XBEE_ADDR_FLAGS_LONG;

            /* restore short address */
            _set_short_addr(dev, dev->addr_short);

            break;
        default:
            return -EINVAL;
    }

    return sizeof(uint16_t);
}

static int _get_channel(xbee_t *dev, uint8_t *val, size_t max)
{
    uint8_t cmd[2];
    resp_t resp;

    if (max < 2) {
        return -EOVERFLOW;
    }
    cmd[0] = 'C';
    cmd[1] = 'H';
    _api_at_cmd(dev, cmd, 2, &resp);
    if (resp.status == 0) {
        val[0] = resp.data[0];
        val[1] = 0;
        return 2;
    }
    return -ECANCELED;
}

static int _set_channel(xbee_t *dev, uint8_t *val, size_t len)
{
    uint8_t cmd[3];
    resp_t resp;

    if (len != 2 || val[1] != 0) {
        return -EINVAL;
    }
    cmd[0] = 'C';
    cmd[1] = 'H';
    cmd[2] = val[0];
    _api_at_cmd(dev, cmd, 3, &resp);
    if (resp.status == 0) {
        return 2;
    }
    return -EINVAL;
}

static int _get_panid(xbee_t *dev, uint8_t *val, size_t max)
{
    uint8_t cmd[2];
    resp_t resp;

    if (max < 2) {
        return -EOVERFLOW;
    }
    cmd[0] = 'I';
    cmd[1] = 'D';
    _api_at_cmd(dev, cmd, 2, &resp);
    if (resp.status == 0) {
        val[0] = resp.data[1];
        val[1] = resp.data[0];
        return 2;
    }
    return -ECANCELED;
}

static int _set_panid(xbee_t *dev, uint8_t *val, size_t len)
{
    uint8_t cmd[4];
    resp_t resp;

    if (len != 2) {
        return -EINVAL;
    }
    cmd[0] = 'I';
    cmd[1] = 'D';
    cmd[2] = val[1];
    cmd[3] = val[0];
    _api_at_cmd(dev, cmd, 4, &resp);
    if (resp.status == 0) {
        return 2;
    }
    return -EINVAL;
}

static int _get_proto(xbee_t *dev, uint8_t *val, size_t max)
{
    if (max < sizeof(gnrc_nettype_t)) {
        return -EOVERFLOW;
    }
    memcpy(val, &(dev->proto), sizeof(gnrc_nettype_t));
    return sizeof(gnrc_nettype_t);
}

static int _set_proto(xbee_t *dev, uint8_t *val, size_t len)
{
    if (len != sizeof(gnrc_nettype_t)) {
        return -EINVAL;
    }
    memcpy(&(dev->proto), val, sizeof(gnrc_nettype_t));
    return sizeof(gnrc_nettype_t);
}

#ifdef MODULE_XBEE_ENCRYPTION
static int _set_encryption(xbee_t *dev, uint8_t *val)
{
    uint8_t cmd[3];
    resp_t resp;
    /* get the current state of Encryption */
    cmd[0] = 'E';
    cmd[1] = 'E';
    _api_at_cmd(dev, cmd, 2, &resp);

    /* Prevent writing the same value in EE. */
    if (val[0] != resp.data[0] ){
        cmd[0] = 'E';
        cmd[1] = 'E';
        cmd[2] = val[0];
        _api_at_cmd(dev, cmd, 3, &resp);
    }
    if (resp.status == 0) {
        return 2;
    }
    return -ECANCELED;
}

static int _set_encryption_key(xbee_t *dev, uint8_t *val, size_t len)
{
        uint8_t cmd[18];
        resp_t resp;
        if (len != 16) { /* the AES key is 128bit, 16 byte */
            return  -EINVAL;
        }
        cmd[0] = 'K';
        cmd[1] = 'Y';

       for (int i = 0; i < 16; i++) { /* Append the key to the KY API AT command */
           cmd[i + 2] = val[i];
       }
        _api_at_cmd(dev, cmd, 18, &resp);
        if (resp.status == 0) {
            return 2;
        }
        return -ECANCELED;
}
#endif

/*
 * Driver's "public" functions
 */
int xbee_init(xbee_t *dev, const xbee_params_t *params)
{
    uint8_t tmp[2];

    /* check device and bus parameters */
    if (dev == NULL) {
        return -ENODEV;
    }
    if (params->uart >= UART_NUMOF) {
        return -ENXIO;
    }
    /* set device driver */
    dev->driver = &xbee_driver;
    /* set peripherals to use */
    dev->uart = params->uart;
    dev->reset_pin = params->reset_pin;
    dev->sleep_pin = params->sleep_pin;
    /* set default options */
    dev->addr_flags = 0;
    dev->proto = XBEE_DEFAULT_PROTOCOL;
    dev->options = 0;
    /* initialize buffers and locks*/
    mutex_init(&(dev->tx_lock));
    mutex_init(&(dev->resp_lock));
    dev->resp_limit = 1;    /* needs to be greater then 0 initially */
    dev->rx_count = 0;
    /* initialize UART and GPIO pins */
    if (uart_init(params->uart, params->baudrate, _rx_cb, dev) < 0) {
        DEBUG("xbee: Error initializing UART\n");
        return -ENXIO;
    }
    if (params->reset_pin != GPIO_UNDEF) {
        if (gpio_init(params->reset_pin, GPIO_OUT) < 0) {
            DEBUG("xbee: Error initializing RESET pin\n");
            return -ENXIO;
        }
        gpio_set(params->reset_pin);
    }
    if (params->sleep_pin != GPIO_UNDEF) {
        if (gpio_init(params->sleep_pin, GPIO_OUT) < 0) {
            DEBUG("xbee: Error initializing SLEEP pin\n");
            return -ENXIO;
        }
        gpio_clear(params->sleep_pin);
    }
    /* if reset pin is connected, do a hardware reset */
    if (params->reset_pin != GPIO_UNDEF) {
        gpio_clear(params->reset_pin);
        xtimer_usleep(RESET_DELAY);
        gpio_set(params->reset_pin);
    }
    /* put the XBee device into command mode */
    xtimer_usleep(ENTER_CMD_MODE_DELAY);
    _at_cmd(dev, "+++");
    xtimer_usleep(ENTER_CMD_MODE_DELAY);
    /* disable non IEEE802.15.4 extensions */
    _at_cmd(dev, "ATMM2\r");
    /* put XBee module in "API mode without escaped characters" */
    _at_cmd(dev, "ATAP1\r");
    /* apply AT commands */
    _at_cmd(dev, "ATAC\r");
    /* exit command mode */
    _at_cmd(dev, "ATCN\r");

    /* load long address (we can not set it, its read only for Xbee devices) */
    if (_get_addr_long(dev, dev->addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN) < 0) {
        DEBUG("xbee: Error getting address\n");
        return -EIO;
    }
    if (_set_addr(dev, &((dev->addr_long).uint8[6]), IEEE802154_SHORT_ADDRESS_LEN) < 0) {
        DEBUG("xbee: Error setting short address\n");
        return -EIO;
    }
    /* set default channel */
    tmp[1] = 0;
    tmp[0] = XBEE_DEFAULT_CHANNEL;
    if (_set_channel(dev, tmp, 2) < 0) {
        DEBUG("xbee: Error setting channel\n");
        return -EIO;
    }
    /* set default PAN ID */
    tmp[1] = (uint8_t)(XBEE_DEFAULT_PANID >> 8);
    tmp[0] = (uint8_t)(XBEE_DEFAULT_PANID & 0xff);
    if (_set_panid(dev, tmp, 2) < 0) {
        DEBUG("xbee: Error setting PAN ID\n");
        return -EIO;
    }

    DEBUG("xbee: Initialization successful\n");
    return 0;
}

static inline bool _is_broadcast(gnrc_netif_hdr_t *hdr)
{
    /* IEEE 802.15.4 does not support multicast so we need to check both flags */
    return (bool)(hdr->flags & (GNRC_NETIF_HDR_FLAGS_BROADCAST |
                                GNRC_NETIF_HDR_FLAGS_MULTICAST));
}

static int _send(gnrc_netdev_t *netdev, gnrc_pktsnip_t *pkt)
{
    xbee_t *dev = (xbee_t *)netdev;
    size_t size;
    size_t pos;
    gnrc_netif_hdr_t *hdr;
    gnrc_pktsnip_t *payload;

    /* check device descriptor and packet */
    if (pkt == NULL) {
        return -ENOMSG;
    }
    if (dev == NULL) {
        gnrc_pktbuf_release(pkt);
        return -ENODEV;
    }

    /* figure out the size of the payload to send */
    size = gnrc_pkt_len(pkt->next);
    if (size > XBEE_MAX_PAYLOAD_LENGTH) {
        DEBUG("xbee: Error sending data, payload length exceeds limit\n");
        gnrc_pktbuf_release(pkt);
        return -EOVERFLOW;
    }
    /* get netif header check address length and flags */
    hdr = (gnrc_netif_hdr_t *)pkt->data;
    if (!((hdr->dst_l2addr_len == IEEE802154_SHORT_ADDRESS_LEN) ||
          (hdr->dst_l2addr_len == IEEE802154_LONG_ADDRESS_LEN) ||
          _is_broadcast(hdr))) {
        gnrc_pktbuf_release(pkt);
        return -ENOMSG;
    }

    /* acquire TX lock */
    mutex_lock(&(dev->tx_lock));
    /* put together the API frame */
    dev->tx_buf[0] = API_START_DELIMITER;
    dev->tx_buf[4] = 0;         /* set to zero to disable response frame */
    /* set size, API id and address field depending on dst address length  */
    if (_is_broadcast(hdr)) {
        DEBUG("xbee: sending broadcast");
        dev->tx_buf[1] = (uint8_t)((size + 5) >> 8);
        dev->tx_buf[2] = (uint8_t)(size + 5);
        dev->tx_buf[3] = API_ID_TX_SHORT_ADDR;
        dev->tx_buf[5] = 0xff;
        dev->tx_buf[6] = 0xff;
        pos = 7;
    }
    else if (hdr->dst_l2addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
        uint8_t *destination = gnrc_netif_hdr_get_dst_addr(hdr);

        DEBUG("xbee: sending unicast to %02x:%02x",
              (uint8_t) destination[0], (uint8_t) destination[1]);

        dev->tx_buf[1] = (uint8_t)((size + 5) >> 8);
        dev->tx_buf[2] = (uint8_t)(size + 5);
        dev->tx_buf[3] = API_ID_TX_SHORT_ADDR;
        memcpy(dev->tx_buf + 5, destination, IEEE802154_SHORT_ADDRESS_LEN);
        pos = 7;
    }
    else {
        uint8_t *destination = gnrc_netif_hdr_get_dst_addr(hdr);

        DEBUG("xbee: sending unicast to %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
              (uint8_t) destination[0], (uint8_t) destination[1],
              (uint8_t) destination[2], (uint8_t) destination[3],
              (uint8_t) destination[4], (uint8_t) destination[5],
              (uint8_t) destination[6], (uint8_t) destination[7]);

        dev->tx_buf[1] = (uint8_t)((size + 11) >> 8);
        dev->tx_buf[2] = (uint8_t)(size + 11);
        dev->tx_buf[3] = API_ID_TX_LONG_ADDR;
        memcpy(dev->tx_buf + 5, destination, IEEE802154_LONG_ADDRESS_LEN);
        pos = 13;
    }
    /* set options */
    DEBUG(", option: %02x", dev->options);
    dev->tx_buf[pos++] = dev->options;
    /* copy payload */
    DEBUG(", payload:");
    payload = pkt->next;
    while (payload) {
#if ENABLE_DEBUG
        for (size_t i = 0; i < payload->size; i++) {
            DEBUG(" %02x", ((uint8_t *) payload->data)[i]);
        }
#endif
        memcpy(&(dev->tx_buf[pos]), payload->data, payload->size);
        pos += payload->size;
        payload = payload->next;
    }
    /* set checksum */
    dev->tx_buf[pos] = _cksum(dev->tx_buf, pos);
    /* start transmission */
    uart_write(dev->uart, dev->tx_buf, pos + 1);
    /* release data */
    gnrc_pktbuf_release(pkt);
    /* release TX lock */
    mutex_unlock(&(dev->tx_lock));
    /* return number of payload byte */
    DEBUG("\n");
    return (int)size;
}

static int _add_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb != NULL) {
        return -ENOBUFS;
    }
    dev->event_cb = cb;
    return 0;
}

static int _rem_cb(gnrc_netdev_t *dev, gnrc_netdev_event_cb_t cb)
{
    if (dev == NULL) {
        return -ENODEV;
    }
    if (dev->event_cb != cb) {
        return -ENOENT;
    }
    dev->event_cb = NULL;
    return 0;
}

static int _get(gnrc_netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    xbee_t *dev = (xbee_t *)netdev;
    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            return _get_addr_short(dev, (uint8_t *)value, max_len);
        case NETOPT_ADDRESS_LONG:
            return _get_addr_long(dev, (uint8_t *)value, max_len);
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            if (dev->addr_flags & XBEE_ADDR_FLAGS_LONG) {
                *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
            }
            else {
                *((uint16_t *)value) = IEEE802154_SHORT_ADDRESS_LEN;
            }
            return sizeof(uint16_t);
        case NETOPT_IPV6_IID:
            if (max_len < sizeof(eui64_t)) {
                return -EOVERFLOW;
            }
            if (dev->addr_flags & XBEE_ADDR_FLAGS_LONG) {
                ieee802154_get_iid(value, (uint8_t *)&dev->addr_long, IEEE802154_LONG_ADDRESS_LEN);
            }
            else {
                ieee802154_get_iid(value, (uint8_t *)&dev->addr_short, IEEE802154_SHORT_ADDRESS_LEN);
            }

            return sizeof(eui64_t);
        case NETOPT_CHANNEL:
            return _get_channel(dev, (uint8_t *)value, max_len);
        case NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)value) = XBEE_MAX_PAYLOAD_LENGTH;
            return sizeof(uint16_t);
        case NETOPT_NID:
            return _get_panid(dev, (uint8_t *)value, max_len);
        case NETOPT_PROTO:
            return _get_proto(dev, (uint8_t *)value, max_len);
        default:
            return -ENOTSUP;
    }
}

static int _set(gnrc_netdev_t *netdev, netopt_t opt, void *value, size_t value_len)
{
    xbee_t *dev = (xbee_t *)netdev;
    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NETOPT_ADDRESS:
            return _set_addr(dev, (uint8_t *)value, value_len);
        case NETOPT_ADDR_LEN:
        case NETOPT_SRC_LEN:
            return _set_addr_len(dev, value, value_len);
        case NETOPT_CHANNEL:
            return _set_channel(dev, (uint8_t *)value, value_len);
        case NETOPT_NID:
            return _set_panid(dev, (uint8_t *)value, value_len);
        case NETOPT_PROTO:
            return _set_proto(dev, (uint8_t *)value, value_len);
#ifdef MODULE_XBEE_ENCRYPTION
        case NETOPT_ENCRYPTION:
            return _set_encryption(dev, (uint8_t *)value);
        case NETOPT_ENCRYPTION_KEY:
            return _set_encryption_key(dev, (uint8_t *)value, value_len);
#endif
        default:
            return -ENOTSUP;
    }
}

static void _isr_event(gnrc_netdev_t *netdev, uint32_t event_type)
{
    xbee_t *dev = (xbee_t *)netdev;
    gnrc_pktsnip_t *pkt_head;
    gnrc_pktsnip_t *pkt;
    gnrc_netif_hdr_t *hdr;
    size_t pos;
    size_t addr_len;
    uint8_t cksum = 0;

    /* check device */
    if (dev == NULL) {
        return;
    }
    /* check rx callback and event type */
    if (event_type != ISR_EVENT_RX_DONE || dev->event_cb == NULL) {
        dev->rx_count = 0;
        return;
    }

    /* read address length */
    if (dev->rx_buf[0] == API_ID_RX_SHORT_ADDR) {
        addr_len = IEEE802154_SHORT_ADDRESS_LEN;
    }
    else {
        addr_len = IEEE802154_LONG_ADDRESS_LEN;
    }

#ifdef XBEE_DENIED_ADDRESSES
    if (addr_len == 8) {
        uint8_t denied_addresses[] = XBEE_DENIED_ADDRESSES;
        for (size_t i = 0; i < sizeof(denied_addresses) / 8; i++) {
            if (memcmp(&(dev->rx_buf[1]),
                       &denied_addresses[i * 8],
                       addr_len) == 0) {
                dev->rx_count = 0;

                DEBUG("xbee: dropping denied packet\n");

                return;
            }
        }
    }
#endif

    /* check checksum for correctness */
    for (int i = 0; i < dev->rx_limit; i++) {
        cksum += dev->rx_buf[i];
    }
    if (cksum != 0xff) {
        DEBUG("xbee: Received packet with incorrect checksum, dropping it\n");
        dev->rx_count = 0;
        return;
    }

    /* allocate and fill interface header */
    pkt_head = gnrc_pktbuf_add(NULL, NULL,
                               sizeof(gnrc_netif_hdr_t) + (2 * addr_len),
                               GNRC_NETTYPE_NETIF);
    if (pkt_head == NULL) {
        DEBUG("xbee: Error allocating netif header in packet buffer on RX\n");
        dev->rx_count = 0;
        return;
    }
    hdr = (gnrc_netif_hdr_t *)pkt_head->data;
    hdr->src_l2addr_len = (uint8_t)addr_len;
    hdr->dst_l2addr_len = (uint8_t)addr_len;
    hdr->if_pid = dev->mac_pid;
    hdr->rssi = dev->rx_buf[1 + addr_len]; /* API ID + source address */
    hdr->lqi = 0;
    gnrc_netif_hdr_set_src_addr(hdr, &(dev->rx_buf[1]), addr_len);
    if (addr_len == 2) {
        gnrc_netif_hdr_set_dst_addr(hdr, dev->addr_short, IEEE802154_SHORT_ADDRESS_LEN);
    }
    else {
        gnrc_netif_hdr_set_dst_addr(hdr, dev->addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN);
    }
    pos = 3 + addr_len;
    /* allocate and copy payload */
    pkt = gnrc_pktbuf_add(pkt_head, &(dev->rx_buf[pos]), dev->rx_limit - pos - 1,
                          dev->proto);
    if (pkt == NULL) {
        DEBUG("xbee: Error allocating payload in packet buffer on RX\n");
        gnrc_pktbuf_release(pkt_head);
        dev->rx_count = 0;
        return;
    }

#if ENABLE_DEBUG
    DEBUG("xbee: received packet from");
    for (size_t i = 0; i < addr_len; i++) {
        DEBUG(" %02x", (uint8_t) gnrc_netif_hdr_get_src_addr(hdr)[i]);
    }

    DEBUG(", RSSI: -%d dBm", hdr->rssi);
    DEBUG(", options: %02x", (uint8_t) dev->rx_buf[1 + addr_len + 1]); /* API ID + source address + RSSI */
    DEBUG(", payload:");

    for (size_t i = 0; i < pkt->size; i++) {
        DEBUG(" %02x", ((uint8_t *) pkt->data)[i]);
    }
    DEBUG("\n");
#endif

    /* pass on the received packet */
    dev->event_cb(NETDEV_EVENT_RX_COMPLETE, pkt);
    /* reset RX byte counter to enable receiving of the next packet */
    dev->rx_count = 0;
}

/*
 * The drivers netdev interface
 */
const gnrc_netdev_driver_t xbee_driver = {
    .send_data = _send,
    .add_event_callback = _add_cb,
    .rem_event_callback = _rem_cb,
    .get = _get,
    .set = _set,
    .isr_event = _isr_event,
};
