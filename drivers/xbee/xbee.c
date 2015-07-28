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
#include "hwtimer.h"
#include "msg.h"
#include "net/eui64.h"
#include "net/ng_ieee802154.h"
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
    for (int i = 3; i < size; i++) {
        res -= buf[i];
    }
    return res;
}

static void _at_cmd(xbee_t *dev, const char *cmd)
{
    DEBUG("xbee: AT_CMD: %s", cmd);

    for (int i = 0; cmd[i] != '\0'; i++) {
        uart_write_blocking(dev->uart, cmd[i]);
    }
}

static void _api_at_cmd(xbee_t *dev, uint8_t *cmd, uint8_t size, resp_t *resp)
{
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

    /* prepare UART for sending out the data and receiving the response */
    dev->tx_limit = size + 6;
    dev->tx_count = 0;
    dev->resp_count = 0;
    /* start send data */
    uart_tx_begin(dev->uart);

    /* wait for results */
    while (dev->resp_limit != dev->resp_count) {
        mutex_lock(&(dev->resp_lock));
    }

    /* populate response data structure */
    resp->status = dev->resp_buf[3];
    resp->data_len = dev->resp_limit - 5;
    if (resp->data_len > 0) {
        memcpy(resp->data, &(dev->resp_buf[4]), resp->data_len);
    }
}

/*
 * Interrupt callbacks
 */
static int _tx_cb(void *arg)
{
    xbee_t *dev = (xbee_t *)arg;
    if (dev->tx_count < dev->tx_limit) {
        /* more data to send */
        char c = (char)(dev->tx_buf[dev->tx_count++]);
        uart_write(dev->uart, c);
        return 1;
    }
    /* release TX lock */
    mutex_unlock(&(dev->tx_lock));
    return 0;
}

static void _rx_cb(void *arg, char c)
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
            dev->int_size += (uint8_t)c;
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
                dev->rx_buf[dev->rx_count++] = (uint8_t)c;
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
            dev->resp_buf[dev->resp_count++] = (uint8_t)c;
            if (dev->resp_count == dev->resp_limit) {
                /* here we ignore the checksum to prevent deadlocks */
                mutex_unlock(&(dev->resp_lock));
                dev->int_state = XBEE_INT_STATE_IDLE;
            }
            break;
        case XBEE_INT_STATE_RX:
            dev->rx_buf[dev->rx_count++] = (uint8_t)c;
            if (dev->rx_count == dev->rx_limit) {
                /* packet is complete */
                msg.type = NG_NETDEV_MSG_TYPE_EVENT;
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
        memcpy(val, resp.data, 2);
        return 2;
    }
    return -ECANCELED;
}

static int _get_addr_long(xbee_t *dev, uint8_t *val, size_t len)
{
    uint8_t cmd[2];
    resp_t resp;

    if (len < 8) {
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
        return 8;
    }
    return -ECANCELED;
}

static int _set_addr(xbee_t *dev, uint8_t *val, size_t len)
{
    uint8_t cmd[4];
    resp_t resp;

    /* device only supports setting the short address */
    if (len != 2) {
        return -ENOTSUP;
    }
    cmd[0] = 'M';
    cmd[1] = 'Y';
    cmd[2] = val[0];
    cmd[3] = val[1];
    _api_at_cmd(dev, cmd, 4, &resp);
    if (resp.status == 0) {
        memcpy(dev->addr_short, val, 2);
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
        case 8:
            dev->addr_flags |= XBEE_ADDR_FLAGS_LONG;
            break;
        case 2:
            dev->addr_flags &= ~XBEE_ADDR_FLAGS_LONG;
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
    if (max < sizeof(ng_nettype_t)) {
        return -EOVERFLOW;
    }
    memcpy(val, &(dev->proto), sizeof(ng_nettype_t));
    return sizeof(ng_nettype_t);
}

static int _set_proto(xbee_t *dev, uint8_t *val, size_t len)
{
    if (len != sizeof(ng_nettype_t)) {
        return -EINVAL;
    }
    memcpy(&(dev->proto), val, sizeof(ng_nettype_t));
    return sizeof(ng_nettype_t);
}

/*
 * Driver's "public" functions
 */
int xbee_init(xbee_t *dev, uart_t uart, uint32_t baudrate,
              gpio_t reset_pin, gpio_t sleep_pin)
{
    uint8_t tmp[2];

    /* check device and bus parameters */
    if (dev == NULL) {
        return -ENODEV;
    }
    if (uart >= UART_NUMOF) {
        return -ENXIO;
    }
    /* set device driver */
    dev->driver = &xbee_driver;
    /* set peripherals to use */
    dev->uart = uart;
    dev->reset_pin = reset_pin;
    dev->sleep_pin = sleep_pin;
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
    if (uart_init(uart, baudrate, _rx_cb, _tx_cb, dev) < 0) {
        DEBUG("xbee: Error initializing UART\n");
        return -ENXIO;
    }
    if (reset_pin != GPIO_UNDEF) {
        if (gpio_init(reset_pin, GPIO_DIR_OUT, GPIO_NOPULL) < 0) {
            DEBUG("xbee: Error initializing RESET pin\n");
            return -ENXIO;
        }
        gpio_set(reset_pin);
    }
    if (sleep_pin != GPIO_UNDEF) {
        if (gpio_init(sleep_pin, GPIO_DIR_OUT, GPIO_NOPULL) < 0) {
            DEBUG("xbee: Error initializing SLEEP pin\n");
            return -ENXIO;
        }
        gpio_clear(sleep_pin);
    }
    /* if reset pin is connected, do a hardware reset */
    if (reset_pin != GPIO_UNDEF) {
        gpio_clear(reset_pin);
        hwtimer_wait(HWTIMER_TICKS(RESET_DELAY));
        gpio_set(reset_pin);
    }
     /* put the XBee device into command mode */
    hwtimer_wait(HWTIMER_TICKS(ENTER_CMD_MODE_DELAY));
    _at_cmd(dev, "+++");
    hwtimer_wait(HWTIMER_TICKS(ENTER_CMD_MODE_DELAY));
    /* disable non IEEE802.15.4 extensions */
    _at_cmd(dev, "ATMM2\r");
    /* put XBee module in "API mode without escaped characters" */
    _at_cmd(dev, "ATAP1\r");
    /* apply AT commands */
    _at_cmd(dev, "ATAC\r");
    /* exit command mode */
    _at_cmd(dev, "ATCN\r");

    /* load long address (we can not set it, its read only for Xbee devices) */
    _get_addr_long(dev, dev->addr_long.uint8, 8);
    /* set default channel */
    _set_addr(dev, &((dev->addr_long).uint8[6]), 2);
    tmp[1] = 0;
    tmp[0] = XBEE_DEFAULT_CHANNEL;
    _set_channel(dev, tmp, 2);
    /* set default PAN ID */
    tmp[1] = (uint8_t)(XBEE_DEFAULT_PANID >> 8);
    tmp[0] = (uint8_t)(XBEE_DEFAULT_PANID & 0xff);
    _set_panid(dev, tmp, 2);

    DEBUG("xbee: Initialization successful\n");
    return 0;
}

static inline bool _is_broadcast(ng_netif_hdr_t *hdr) {
    /* IEEE 802.15.4 does not support multicast so we need to check both flags */
    return (bool)(hdr->flags & (NG_NETIF_HDR_FLAGS_BROADCAST |
                                NG_NETIF_HDR_FLAGS_MULTICAST));
}

static int _send(ng_netdev_t *netdev, ng_pktsnip_t *pkt)
{
    xbee_t *dev = (xbee_t *)netdev;
    size_t size;
    size_t pos;
    ng_netif_hdr_t *hdr;
    ng_pktsnip_t *payload;

    /* check device descriptor and packet */
    if (pkt == NULL) {
        return -ENOMSG;
    }
    if (dev == NULL) {
        ng_pktbuf_release(pkt);
        return -ENODEV;
    }

    /* figure out the size of the payload to send */
    size = ng_pkt_len(pkt->next);
    if (size > XBEE_MAX_PAYLOAD_LENGTH) {
        DEBUG("xbee: Error sending data, payload length exceeds limit\n");
        ng_pktbuf_release(pkt);
        return -EOVERFLOW;
    }
    /* get netif header check address length and flags */
    hdr = (ng_netif_hdr_t *)pkt->data;
    if (!((hdr->dst_l2addr_len == 2) || (hdr->dst_l2addr_len == 8) ||
          _is_broadcast(hdr))) {
        ng_pktbuf_release(pkt);
        return -ENOMSG;
    }

    /* acquire TX lock */
    mutex_lock(&(dev->tx_lock));
    /* put together the API frame */
    dev->tx_buf[0] = API_START_DELIMITER;
    dev->tx_buf[4] = 0;         /* set to zero to disable response frame */
    /* set size, API id and address field depending on dst address length  */
    if (_is_broadcast(hdr)) {
        dev->tx_buf[1] = (uint8_t)((size + 5) >> 8);
        dev->tx_buf[2] = (uint8_t)(size + 5);
        dev->tx_buf[3] = API_ID_TX_SHORT_ADDR;
        dev->tx_buf[4] = 0xff;
        dev->tx_buf[5] = 0xff;
    }
    if (hdr->dst_l2addr_len == 2) {
        dev->tx_buf[1] = (uint8_t)((size + 5) >> 8);
        dev->tx_buf[2] = (uint8_t)(size + 5);
        dev->tx_buf[3] = API_ID_TX_SHORT_ADDR;
        memcpy(dev->tx_buf + 5, ng_netif_hdr_get_dst_addr(hdr), 2);
        pos = 7;
    } else {
        dev->tx_buf[1] = (uint8_t)((size + 11) >> 8);
        dev->tx_buf[2] = (uint8_t)(size + 11);
        dev->tx_buf[3] = API_ID_TX_LONG_ADDR;
        memcpy(dev->tx_buf + 5, ng_netif_hdr_get_dst_addr(hdr), 8);
        pos = 13;
    }
    /* set options */
    dev->tx_buf[pos++] = dev->options;
    /* copy payload */
    payload = pkt->next;
    while (payload) {
        memcpy(&(dev->tx_buf[pos]), payload->data, payload->size);
        pos += payload->size;
        payload = payload->next;
    }
    /* set checksum */
    dev->tx_buf[pos] = _cksum(dev->tx_buf, pos);
    /* prepare transmission */
    dev->tx_limit = (uint16_t)pos + 1;
    dev->tx_count = 0;
    /* start transmission */
    uart_tx_begin(dev->uart);
    /* release data */
    ng_pktbuf_release(pkt);
    /* return number of payload byte */
    return (int)size;
}

static int _add_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
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

static int _rem_cb(ng_netdev_t *dev, ng_netdev_event_cb_t cb)
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

static int _get(ng_netdev_t *netdev, ng_netopt_t opt, void *value, size_t max_len)
{
    xbee_t *dev = (xbee_t *)netdev;
    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NG_NETOPT_ADDRESS:
            return _get_addr_short(dev, (uint8_t *)value, max_len);
        case NG_NETOPT_ADDRESS_LONG:
            return _get_addr_long(dev, (uint8_t *)value, max_len);
        case NG_NETOPT_ADDR_LEN:
        case NG_NETOPT_SRC_LEN:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            if (dev->addr_flags & XBEE_ADDR_FLAGS_LONG) {
                *((uint16_t *)value) = 8;
            }
            else {
                *((uint16_t *)value) = 2;
            }
            return sizeof(uint16_t);
        case NG_NETOPT_IPV6_IID:
            if (max_len < sizeof(eui64_t)) {
                return -EOVERFLOW;
            }
            if (dev->addr_flags & XBEE_ADDR_FLAGS_LONG) {
                ng_ieee802154_get_iid(value, (uint8_t *)&dev->addr_long, 8);
            }
            else {
                ng_ieee802154_get_iid(value, (uint8_t *)&dev->addr_short, 2);
            }

            return sizeof(eui64_t);
        case NG_NETOPT_CHANNEL:
            return _get_channel(dev, (uint8_t *)value, max_len);
        case NG_NETOPT_MAX_PACKET_SIZE:
            if (max_len < sizeof(uint16_t)) {
                return -EOVERFLOW;
            }
            *((uint16_t *)value) = XBEE_MAX_PAYLOAD_LENGTH;
            return sizeof(uint16_t);
        case NG_NETOPT_NID:
            return _get_panid(dev, (uint8_t *)value, max_len);
        case NG_NETOPT_PROTO:
            return _get_proto(dev, (uint8_t *)value, max_len);
        default:
            return -ENOTSUP;
    }
}

static int _set(ng_netdev_t *netdev, ng_netopt_t opt, void *value, size_t value_len)
{
    xbee_t *dev = (xbee_t *)netdev;
    if (dev == NULL) {
        return -ENODEV;
    }

    switch (opt) {
        case NG_NETOPT_ADDRESS:
            return _set_addr(dev, (uint8_t *)value, value_len);
        case NG_NETOPT_ADDR_LEN:
        case NG_NETOPT_SRC_LEN:
            return _set_addr_len(dev, value, value_len);
        case NG_NETOPT_CHANNEL:
            return _set_channel(dev, (uint8_t *)value, value_len);
        case NG_NETOPT_NID:
            return _set_panid(dev, (uint8_t *)value, value_len);
        case NG_NETOPT_PROTO:
            return _set_proto(dev, (uint8_t *)value, value_len);
        default:
            return -ENOTSUP;
    }
}

static void _isr_event(ng_netdev_t *netdev, uint32_t event_type)
{
    xbee_t *dev = (xbee_t *)netdev;
    ng_pktsnip_t *pkt_head;
    ng_pktsnip_t *pkt;
    ng_netif_hdr_t *hdr;
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
        addr_len = 2;
    }
    else {
        addr_len = 8;
    }

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
    pkt_head = ng_pktbuf_add(NULL, NULL,
                             sizeof(ng_netif_hdr_t) + (2 * addr_len),
                             NG_NETTYPE_NETIF);
    if (pkt_head == NULL) {
        DEBUG("xbee: Error allocating netif header in packet buffer on RX\n");
        dev->rx_count = 0;
        return;
    }
    hdr = (ng_netif_hdr_t *)pkt_head->data;
    hdr->src_l2addr_len = (uint8_t)addr_len;
    hdr->dst_l2addr_len = (uint8_t)addr_len;
    hdr->if_pid = dev->mac_pid;
    hdr->rssi = dev->rx_buf[2 + addr_len];
    hdr->lqi = 0;
    ng_netif_hdr_set_src_addr(hdr, &(dev->rx_buf[1]), addr_len);
    if (addr_len == 2) {
        ng_netif_hdr_set_dst_addr(hdr, dev->addr_short, 2);
    }
    else {
        ng_netif_hdr_set_dst_addr(hdr, dev->addr_long.uint8, 8);
    }
    pos = 3 + addr_len;
    /* allocate and copy payload */
    pkt = ng_pktbuf_add(pkt_head, &(dev->rx_buf[pos]), dev->rx_limit - pos - 1,
                        dev->proto);
    if (pkt == NULL) {
        DEBUG("xbee: Error allocating payload in packet buffer on RX\n");
        ng_pktbuf_release(pkt_head);
        dev->rx_count = 0;
        return;
    }

    /* pass on the received packet */
    dev->event_cb(NETDEV_EVENT_RX_COMPLETE, pkt);
    /* reset RX byte counter to enable receiving of the next packet */
    dev->rx_count = 0;
}

/*
 * The drivers netdev interface
 */
const ng_netdev_driver_t xbee_driver = {
    .send_data = _send,
    .add_event_callback = _add_cb,
    .rem_event_callback = _rem_cb,
    .get = _get,
    .set = _set,
    .isr_event = _isr_event,
};
