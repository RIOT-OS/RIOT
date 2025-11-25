/*
 * SPDX-FileCopyrightText: 2014 INRIA
 * SPDX-FileCopyrightText: 2015-2016 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_xbee
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
#include <errno.h>

#include "assert.h"
#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev.h"
#include "timex.h"
#include "xbee.h"
#include "ztimer.h"
#ifdef MODULE_GNRC
#include "net/gnrc.h"
#endif

#define ENABLE_DEBUG            0
#include "debug.h"

/**
 * @brief   Internal driver event type when RX is finished
 */
#define ISR_EVENT_RX_DONE       (0x0001)

/**
 * @brief   Delay when entering command mode, must be > 1s
 */
#define ENTER_CMD_MODE_DELAY_MS (1100UL)
/**
 * @brief   Delay when resetting the device, 10ms
 */
#define RESET_DELAY_MS          (10UL)

/**
 * @brief   Timeout for receiving AT command response
 */
#define RESP_TIMEOUT_MS         (1UL * MS_PER_SEC)

/**
 * @brief   Start delimiter in API frame mode
 */
#define API_START_DELIMITER     (0x7e)

/**
 * @brief   Command IDs when communicating in API frame mode
 * @{
 */
#define API_ID_MODEM_STATUS     (0x8a)  /**< modem status frame */
#define API_ID_AT               (0x08)  /**< AT command request frame */
#define API_ID_AT_QUEUE         (0x09)  /**< queued AT command frame */
#define API_ID_AT_RESP          (0x88)  /**< AT command response frame */
#define API_ID_TX_LONG_ADDR     (0x00)  /**< TX frame (long address) */
#define API_ID_TX_SHORT_ADDR    (0x01)  /**< TX frame (short address) */
#define API_ID_TX_RESP          (0x89)  /**< TX response frame */
#define API_ID_RX_LONG_ADDR     (0x80)  /**< RX frame (long address) */
#define API_ID_RX_SHORT_ADDR    (0x81)  /**< RX frame (short address) */
/** @} */

/**
 * @brief   Internal option flags (to be expanded if needed)
 * @{
 */
#define OPT_DIS_AUTO_ACK        (0x01)  /**< disable sending of auto ACKs */
#define OPT_BCAST_ADDR          (0x02)  /**< address broadcast */
#define OPT_BCAST_PAN           (0x04)  /**< PAN broadcast */
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
static uint8_t _cksum(size_t offset, uint8_t *buf, size_t size)
{
    uint8_t res = 0xff;

    for (size_t i = offset; i < size; i++) {
        res -= buf[i];
    }
    return res;
}

static void _at_cmd(xbee_t *dev, const char *cmd)
{
    DEBUG("[xbee] AT_CMD: %s\n", cmd);

    uart_write(dev->p.uart, (uint8_t *)cmd, strlen(cmd));
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
    DEBUG("[xbee] AT_CMD: %s\n", cmd);

    /* acquire TX lock */
    mutex_lock(&(dev->tx_lock));
    /* construct API frame */
    dev->cmd_buf[0] = API_START_DELIMITER;
    dev->cmd_buf[1] = (size + 2) >> 8;
    dev->cmd_buf[2] = (size + 2) & 0xff;
    dev->cmd_buf[3] = API_ID_AT;
    dev->cmd_buf[4] = 1;             /* use fixed frame id */
    memcpy(dev->cmd_buf + 5, cmd, size);
    dev->cmd_buf[size + 5] = _cksum(3, dev->cmd_buf, size + 5);

    /* reset the response data counter */
    dev->resp_count = 0;
    /* start send data */
    uart_write(dev->p.uart, dev->cmd_buf, size + 6);

    ztimer_now_t sent_time = ztimer_now(ZTIMER_MSEC);

    ztimer_t resp_timer;

    resp_timer.callback = isr_resp_timeout;
    resp_timer.arg = dev;

    ztimer_set(ZTIMER_MSEC, &resp_timer, RESP_TIMEOUT_MS);

    /* wait for results */
    while ((dev->resp_limit != dev->resp_count) &&
           ((int32_t)(sent_time + RESP_TIMEOUT_MS - ztimer_now(ZTIMER_MSEC)) > 0)) {
        mutex_lock(&(dev->resp_lock));
    }

    ztimer_remove(ZTIMER_MSEC, &resp_timer);

    if (dev->resp_limit != dev->resp_count) {
        DEBUG("[xbee] api_at_cmd: response timeout\n");
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
            netdev_trigger_event_isr((netdev_t *)dev);
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

static int _set_short_addr(xbee_t *dev, const uint8_t *address)
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

static int _set_addr(xbee_t *dev, const uint8_t *val, size_t len)
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

static int _set_addr_len(xbee_t *dev, const uint16_t *val, size_t len)
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

static int _set_channel(xbee_t *dev, const uint8_t *val, size_t len)
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

static int _set_panid(xbee_t *dev, const uint8_t *val, size_t len)
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

#ifdef MODULE_XBEE_ENCRYPTION
static int _set_encryption(xbee_t *dev, const uint8_t *val)
{
    uint8_t cmd[3];
    resp_t resp;

    /* get the current state of Encryption */
    cmd[0] = 'E';
    cmd[1] = 'E';
    _api_at_cmd(dev, cmd, 2, &resp);

    /* Prevent writing the same value in EE. */
    if (val[0] != resp.data[0]) {
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

static int _set_encryption_key(xbee_t *dev, const uint8_t *val, size_t len)
{
    uint8_t cmd[18];
    resp_t resp;

    if (len != 16) {     /* the AES key is 128bit, 16 byte */
        return -EINVAL;
    }
    cmd[0] = 'K';
    cmd[1] = 'Y';

    for (int i = 0; i < 16; i++) {    /* Append the key to the KY API AT command */
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

void xbee_setup(xbee_t *dev, const xbee_params_t *params)
{
    assert(dev && (params->uart < UART_NUMOF));

    /* set device driver */
    dev->driver = &xbee_driver;
    dev->event_callback = NULL;
    dev->context = dev;

    /* set peripherals to use */
    dev->p = *params;

    /* initialize pins */
    if (gpio_is_valid(dev->p.pin_reset)) {
        gpio_init(dev->p.pin_reset, GPIO_OUT);
        gpio_set(dev->p.pin_reset);
    }
    if (gpio_is_valid(dev->p.pin_sleep)) {
        gpio_init(dev->p.pin_sleep, GPIO_OUT);
        gpio_clear(dev->p.pin_sleep);
    }
    /* we initialize the UART later, since we can not handle interrupts, yet */
}

int xbee_build_hdr(xbee_t *dev, uint8_t *xhdr, size_t payload_len,
                   void *dst_addr, size_t addr_len)
{
    /* make sure payload fits into a packet */
    if (payload_len > XBEE_MAX_PAYLOAD_LENGTH) {
        return -EOVERFLOW;
    }

    /* set start delimiter, configure address and set options. Also make sure,
     * that the link layer address is of known length */
    xhdr[0] = API_START_DELIMITER;
    xhdr[4] = dev->tx_fid++;
    if (addr_len == IEEE802154_SHORT_ADDRESS_LEN) {
        xhdr[3] = API_ID_TX_SHORT_ADDR;
        xhdr[7] = dev->options;
    }
    else if (addr_len == IEEE802154_LONG_ADDRESS_LEN) {
        xhdr[3] = API_ID_TX_LONG_ADDR;
        xhdr[13] = dev->options;
    }
    else {
        return -ENOMSG;
    }

    /* finally configure the packet size and copy the actual dst address */
    uint16_t size = (uint16_t)(payload_len + addr_len + 3);

    xhdr[1] = (uint8_t)(size >> 8);
    xhdr[2] = (uint8_t)(size & 0xff);
    memcpy(&xhdr[5], dst_addr, addr_len);

    return (int)(addr_len + 6);
}

int xbee_parse_hdr(xbee_t *dev, const uint8_t *xhdr, xbee_l2hdr_t *l2hdr)
{
    uint8_t alen;

    assert(xhdr && l2hdr);

    /* get the address length */
    if (xhdr[0] == API_ID_RX_SHORT_ADDR) {
        alen = IEEE802154_SHORT_ADDRESS_LEN;
    }
    else if (xhdr[0] == API_ID_RX_LONG_ADDR) {
        alen = IEEE802154_LONG_ADDRESS_LEN;
    }
    else {
        return -ENOMSG;
    }

    /* copy the actual SRC address and the RSSI value */
    memcpy(l2hdr->src_addr, &xhdr[1], alen);
    l2hdr->rssi = xhdr[1 + alen];

    /* copy the destination address */
    l2hdr->bcast = (xhdr[2 + alen] & OPT_BCAST_ADDR) ? 1 : 0;
    if (l2hdr->bcast) {
        memset(l2hdr->dst_addr, 0xff, alen);
    }
    else {
        if (alen == IEEE802154_SHORT_ADDRESS_LEN) {
            memcpy(l2hdr->dst_addr, dev->addr_short, alen);
        }
        else {
            memcpy(l2hdr->dst_addr, dev->addr_long.uint8, alen);
        }
    }
    l2hdr->addr_len = alen;

    return (int)(alen + 3);
}

int xbee_init(netdev_t *dev)
{
    uint8_t tmp[2];
    xbee_t *xbee = (xbee_t *)dev;

    /* set default options */
    xbee->addr_flags = XBEE_ADDR_FLAGS_LONG;
    xbee->options = 0;
    /* initialize buffers and locks*/
    mutex_init(&(xbee->tx_lock));
    mutex_init(&(xbee->resp_lock));
    xbee->resp_limit = 1;    /* needs to be greater then 0 initially */
    xbee->rx_count = 0;
    /* initialize UART and GPIO pins */
    if (uart_init(xbee->p.uart, xbee->p.br, _rx_cb, xbee) != UART_OK) {
        DEBUG("[xbee] init: error initializing UART\n");
        return -ENXIO;
    }
    /* if reset pin is connected, do a hardware reset */
    if (gpio_is_valid(xbee->p.pin_reset)) {
        gpio_clear(xbee->p.pin_reset);
        ztimer_sleep(ZTIMER_MSEC, RESET_DELAY_MS);
        gpio_set(xbee->p.pin_reset);
    }
    /* put the XBee device into command mode */
    ztimer_sleep(ZTIMER_MSEC, ENTER_CMD_MODE_DELAY_MS);
    _at_cmd(xbee, "+++");
    ztimer_sleep(ZTIMER_MSEC, ENTER_CMD_MODE_DELAY_MS);
    /* disable non IEEE802.15.4 extensions */
    _at_cmd(xbee, "ATMM2\r");
    /* put XBee module in "API mode without escaped characters" */
    _at_cmd(xbee, "ATAP1\r");
    /* disable xbee CTS and RTS, unless hardware flow control is used */
    if (!IS_USED(MODULE_PERIPH_UART_HW_FC)) {
        DEBUG("[xbee] init: WARNING if using an arduino BOARD + arduino xbee " \
              "shield with ICSP connector, hardware flow control can't be " \
              "used since CTS pin is connected to ICSP RESET pin\n");
        _at_cmd(xbee, "ATD6 0\r");
        _at_cmd(xbee, "ATD7 0\r");
    }
    /* apply AT commands */
    _at_cmd(xbee, "ATAC\r");
    /* exit command mode */
    _at_cmd(xbee, "ATCN\r");

    /* load long address (we can not set it, its read only for Xbee devices) */
    if (_get_addr_long(xbee, xbee->addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN) < 0) {
        DEBUG("[xbee] init: error getting address\n");
        return -EIO;
    }
    if (_set_addr(xbee, &((xbee->addr_long).uint8[6]), IEEE802154_SHORT_ADDRESS_LEN) < 0) {
        DEBUG("[xbee] init: error setting short address\n");
        return -EIO;
    }
    /* set default channel */
    tmp[1] = 0;
    tmp[0] = XBEE_DEFAULT_CHANNEL;
    if (_set_channel(xbee, tmp, 2) < 0) {
        DEBUG("[xbee] init: error setting channel\n");
        return -EIO;
    }
    /* set default PAN ID */
    tmp[1] = (uint8_t)(XBEE_DEFAULT_PANID >> 8);
    tmp[0] = (uint8_t)(XBEE_DEFAULT_PANID & 0xff);
    if (_set_panid(xbee, tmp, 2) < 0) {
        DEBUG("[xbee] init: error setting PAN ID\n");
        return -EIO;
    }

    DEBUG("[xbee] init: Initialization successful\n");

    /* signal link UP */
    dev->event_callback(dev, NETDEV_EVENT_LINK_UP);

    return 0;
}

static int xbee_send(netdev_t *dev, const iolist_t *iolist)
{
    xbee_t *xbee = (xbee_t *)dev;
    size_t size;
    uint8_t csum;

    assert(xbee && iolist);

    /* calculate the checksum and the packet size */
    size = iolist->iol_len;
    csum = _cksum(3, (uint8_t *)iolist->iol_base, size);
    for (const iolist_t *iol = iolist->iol_next; iol; iol = iol->iol_next) {
        size_t len = iol->iol_len;

        size += len;
        for (size_t p = 0; p < len; p++) {
            csum -= ((uint8_t *)iol->iol_base)[p];
        }
    }

    /* make sure the data fits into a packet */
    if (size >= XBEE_MAX_PKT_LENGTH) {
        DEBUG("[xbee] send: data to send is too large for TX buffer\n");
        return -1;
    }

    /* send the actual data packet */
    DEBUG("[xbee] send: now sending out %" PRIuSIZE " byte\n", size);
    mutex_lock(&(xbee->tx_lock));
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (iol->iol_len > 0) {
            uart_write(xbee->p.uart, iol->iol_base, iol->iol_len);
        }
    }
    uart_write(xbee->p.uart, &csum, 1);
    mutex_unlock(&(xbee->tx_lock));

    /* return number of payload bytes */
    return (int)size;
}

static int xbee_recv(netdev_t *dev, void *buf, size_t len, void *info)
{
    (void)info;
    size_t size;
    xbee_t *xbee = (xbee_t *)dev;

    assert(xbee);

    /* make sure we have new data waiting */
    if (xbee->rx_count != xbee->rx_limit) {
        DEBUG("[xbee] recv: no data available for reading\n");
        return 0;
    }

    /* data available, so we read it (or it's size) */
    size = (size_t)(xbee->rx_limit - 1);
    if (buf == NULL) {
        if (len > 0) {
            DEBUG("[xbee] recv: reading size and dropping: %" PRIuSIZE "\n", size);
            xbee->rx_count = 0;
        }
        else {
            DEBUG("[xbee] recv: reading size without dropping: %" PRIuSIZE "\n", size);
        }
    }
    else {
        size = (size > len) ? len : size;
        DEBUG("[xbee] recv: consuming packet: reading %" PRIuSIZE " byte\n", size);
        memcpy(buf, xbee->rx_buf, size);
        xbee->rx_count = 0;
    }

    return (int)size;
}

static void xbee_isr(netdev_t *netdev)
{
    xbee_t *dev = (xbee_t *)netdev;

    if (dev->rx_count == dev->rx_limit) {
        /* make sure the checksum checks out */
        if (_cksum(0, dev->rx_buf, dev->rx_limit) != 0) {
            DEBUG("[xbee] isr: invalid RX checksum\n");
            dev->rx_count = 0;
        }
        else {
            DEBUG("[xbee] isr: data available, waiting for read\n");
            dev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
        }
    }
}

static int xbee_get(netdev_t *ndev, netopt_t opt, void *value, size_t max_len)
{
    xbee_t *dev = (xbee_t *)ndev;

    assert(dev);

    switch (opt) {
    case NETOPT_ADDRESS:
        assert(max_len >= IEEE802154_SHORT_ADDRESS_LEN);
        memcpy(value, dev->addr_short, IEEE802154_SHORT_ADDRESS_LEN);
        return IEEE802154_SHORT_ADDRESS_LEN;
    case NETOPT_ADDRESS_LONG:
        assert(max_len >= IEEE802154_LONG_ADDRESS_LEN);
        memcpy(value, dev->addr_long.uint8, IEEE802154_LONG_ADDRESS_LEN);
        return IEEE802154_LONG_ADDRESS_LEN;
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
    case NETOPT_DEVICE_TYPE:
        assert(max_len == sizeof(uint16_t));
        *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
        return sizeof(uint16_t);
    case NETOPT_CHANNEL:
        return _get_channel(dev, (uint8_t *)value, max_len);
    case NETOPT_MAX_PDU_SIZE:
        if (max_len < sizeof(uint16_t)) {
            return -EOVERFLOW;
        }
        *((uint16_t *)value) = XBEE_MAX_PAYLOAD_LENGTH;
        return sizeof(uint16_t);
    case NETOPT_NID:
        return _get_panid(dev, (uint8_t *)value, max_len);
#ifdef MODULE_GNRC
    case NETOPT_PROTO:
        if (max_len != sizeof(gnrc_nettype_t)) {
            return -EOVERFLOW;
        }
        *((gnrc_nettype_t *)value) = XBEE_DEFAULT_PROTOCOL;
        return sizeof(gnrc_nettype_t);
#endif
    default:
        return -ENOTSUP;
    }
}

static int xbee_set(netdev_t *ndev, netopt_t opt, const void *value, size_t len)
{
    xbee_t *dev = (xbee_t *)ndev;

    assert(dev);

    switch (opt) {
    case NETOPT_ADDRESS:
        return _set_addr(dev, value, len);
    case NETOPT_ADDR_LEN:
    case NETOPT_SRC_LEN:
        return _set_addr_len(dev, value, len);
    case NETOPT_CHANNEL:
        return _set_channel(dev, value, len);
    case NETOPT_NID:
        return _set_panid(dev, value, len);
#ifdef MODULE_XBEE_ENCRYPTION
    case NETOPT_ENCRYPTION:
        return _set_encryption(dev, value);
    case NETOPT_ENCRYPTION_KEY:
        return _set_encryption_key(dev, value, len);
#endif
    default:
        return -ENOTSUP;
    }
}

/*
 * The drivers netdev interface
 */
const netdev_driver_t xbee_driver = {
    .send = xbee_send,
    .recv = xbee_recv,
    .init = xbee_init,
    .isr = xbee_isr,
    .get = xbee_get,
    .set = xbee_set,
};
