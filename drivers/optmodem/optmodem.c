#include <string.h>
#include "optmodem.h"

#include "net/netdev.h"
#include "net/netopt.h"
#include "iolist.h"

#if OPTMODEM_PRESENTS_AS_ETHERNET
#include "net/ethernet.h"
#endif

/* --- Forward decls --- */
static int  _send(netdev_t *netdev, const iolist_t *iolist);
static int  _confirm_send(netdev_t *netdev, void *info);
static int  _recv(netdev_t *netdev, void *buf, size_t len, void *info);
static int  _init(netdev_t *netdev);
static void _isr(netdev_t *netdev);
static int  _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len);
static int  _set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len);

/* Driver vtable */
const netdev_driver_t optmodem_driver = {
    .send         = _send,
    .confirm_send = _confirm_send,
    .recv         = _recv,
    .init         = _init,
    .isr          = _isr,
    .get          = _get,
    .set          = _set,
};

/* UART RX interrupt: called per byte in IRQ context */
static void _uart_rx_cb(void *arg, uint8_t data)
{
    optmodem_t *dev = (optmodem_t *)arg;

    /* Simple length-prefixed framing: first two bytes = length (host order) */
    if (dev->rx_expected == 0) {
        /* gather 2-byte length header */
        if (dev->rx_have == 0) {
            dev->rx_buf[0] = data;
            dev->rx_have = 1;
            return;
        }
        else {
            uint16_t len = ((uint16_t)dev->rx_buf[0]) | (((uint16_t)data) << 8);
            dev->rx_expected = len;
            dev->rx_have = 0;
            /* sanity clamp */
            if (dev->rx_expected > OPTMODEM_MAX_FRAME) {
                dev->rx_expected = 0;
                dev->rx_have = 0;
                return;
            }
            return;
        }
    }

    if (dev->rx_have < dev->rx_expected) {
        dev->rx_buf[dev->rx_have++] = data;
        if (dev->rx_have == dev->rx_expected) {
            dev->rx_ready = true;
            /* Notify upper layer in IRQ-safe way */
            netdev_trigger_event_isr(&dev->netdev);
        }
    }
}

/* --- netdev ops --- */

static int _init(netdev_t *netdev)
{
    optmodem_t *dev = (optmodem_t *)netdev;

    dev->mode       = OPTMODEM_MODE_UNKNOWN;
    dev->state      = NETOPT_STATE_IDLE;
    dev->rx_expected = 0;
    dev->rx_have     = 0;
    dev->rx_ready    = false;

    int res = uart_init(dev->params.uart, dev->params.baud, _uart_rx_cb, dev);
    if (res < 0) {
        return res;
    }

    /* If you need RTS/CTS, parity, or collision detect: call uart_mode() /
     * uart_* helpers here. */

    return 0;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    optmodem_t *dev = (optmodem_t *)netdev;

    /* Flatten outgoing iolist (L2 header + payload) into a bounded stack buffer */
    size_t total = iolist_size(iolist);
    if (total > OPTMODEM_MAX_FRAME) {
        return -EMSGSIZE;
    }

    /* Write 2-byte length, then the bytes */
    uint8_t len_hdr[2];
    len_hdr[0] = (uint8_t)(total & 0xff);
    len_hdr[1] = (uint8_t)((total >> 8) & 0xff);

    uart_write(dev->params.uart, len_hdr, sizeof(len_hdr));

    /* Stream out each iolist chunk without copying if possible */
    for (const iolist_t *p = iolist; p; p = p->iol_next) {
        if (p->iol_len) {
            uart_write(dev->params.uart, (const uint8_t *)p->iol_base, p->iol_len);
        }
    }

    /* We implement a blocking send (UART is synchronous here).
     * Per netdev “new API”, returning the number of bytes means no TX_COMPLETE
     * event will be emitted and confirm_send() won’t be called. */
    return (int)total;
}

static int _confirm_send(netdev_t *netdev, void *info)
{
    (void)netdev; (void)info;
    /* Not used for blocking UART; see send() note. */
    return -EAGAIN;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    (void)info;
    optmodem_t *dev = (optmodem_t *)netdev;

    if (!dev->rx_ready) {
        /* Nothing buffered; upper layer should only call after RX_COMPLETE */
        return -EAGAIN;
    }

    /* Query length without consuming */
    if (buf == NULL && len == 0) {
        return (int)dev->rx_expected;
    }

    /* Drop frame request */
    if (buf == NULL && len > 0) {
        dev->rx_ready    = false;
        dev->rx_expected = 0;
        dev->rx_have     = 0;
        return 0;
    }

    /* Copy out the frame */
    if (len < dev->rx_expected) {
        /* drop and report ENOBUFS per netdev semantics */
        dev->rx_ready    = false;
        dev->rx_expected = 0;
        dev->rx_have     = 0;
        return -ENOBUFS;
    }

    memcpy(buf, dev->rx_buf, dev->rx_expected);
    int out = (int)dev->rx_expected;

    dev->rx_ready    = false;
    dev->rx_expected = 0;
    dev->rx_have     = 0;

    return out;
}

static void _isr(netdev_t *netdev)
{
    /* Called from the GNRC/lwIP hosting thread after netdev_trigger_event_isr().
     * We simply escalate to RX_COMPLETE so the stack can fetch the frame. */
    if (netdev->event_callback) {
        netdev->event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
    }
}

static int _get(netdev_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    optmodem_t *dev = (optmodem_t *)netdev;

    switch (opt) {
    case NETOPT_DEVICE_TYPE:
        if (max_len < sizeof(uint16_t)) return -EINVAL;
#if OPTMODEM_PRESENTS_AS_ETHERNET
        *(uint16_t *)value = NETDEV_TYPE_ETHERNET;
#else
        *(uint16_t *)value = NETDEV_TYPE_RAW;
#endif
        return sizeof(uint16_t);

    case NETOPT_IS_WIRED: {
        if (max_len < sizeof(netopt_enable_t)) return -EINVAL;
        *(netopt_enable_t *)value = NETOPT_ENABLE; /* optical ≈ wired for stacks */
        return sizeof(netopt_enable_t);
    }

    case NETOPT_ADDR_LEN:
        if (max_len < sizeof(uint16_t)) return -EINVAL;
        *(uint16_t *)value = OPTMODEM_ADDR_LEN;
        return sizeof(uint16_t);

#if OPTMODEM_PRESENTS_AS_ETHERNET
    case NETOPT_ADDRESS:
        if (max_len < ETHERNET_ADDR_LEN) return -EINVAL;
        memcpy(value, dev->params.mac, ETHERNET_ADDR_LEN);
        return ETHERNET_ADDR_LEN;
#endif

    case NETOPT_MAX_PDU_SIZE: {
        if (max_len < sizeof(uint16_t)) return -EINVAL;
        uint16_t sz = (uint16_t)OPTMODEM_MAX_FRAME;
        *(uint16_t *)value = sz;
        return sizeof(uint16_t);
    }

    case NETOPT_STATE:
        if (max_len < sizeof(netopt_state_t)) return -EINVAL;
        *(netopt_state_t *)value = dev->state;
        return sizeof(netopt_state_t);

    /* We don’t add software CRC/FEC; modem/PHY should handle integrity.
       Expose as disabled if queried. */
    case NETOPT_INTEGRITY_CHECK: {
        if (max_len < sizeof(netopt_enable_t)) return -EINVAL;
        *(netopt_enable_t *)value = NETOPT_DISABLE;
        return sizeof(netopt_enable_t);
    }

    default:
        return -ENOTSUP;
    }
}

static int _set(netdev_t *netdev, netopt_t opt, const void *value, size_t value_len)
{
    optmodem_t *dev = (optmodem_t *)netdev;

    switch (opt) {
#if OPTMODEM_PRESENTS_AS_ETHERNET
    case NETOPT_ADDRESS:
        if (value_len != ETHERNET_ADDR_LEN) return -EINVAL;
        memcpy(dev->params.mac, value, ETHERNET_ADDR_LEN);
        return (int)value_len;
#endif

    case NETOPT_STATE: {
        if (value_len != sizeof(netopt_state_t)) return -EINVAL;
        netopt_state_t st = *(const netopt_state_t *)value;
        if (st == NETOPT_STATE_RESET) {
            dev->rx_expected = dev->rx_have = 0;
            dev->rx_ready    = false;
            dev->state       = NETOPT_STATE_IDLE;
            return sizeof(netopt_state_t);
        }
        /* We ignore other state transitions in this minimal driver */
        dev->state = st;
        return sizeof(netopt_state_t);
    }

    /* If you later map modem bandwidth, channel (wavelength), or TX power,
       you can implement NETOPT_BANDWIDTH, NETOPT_CHANNEL_FREQUENCY, NETOPT_TX_POWER here. */

    default:
        return -ENOTSUP;
    }
}

/* Public convenience init */
int optmodem_init(optmodem_t *dev)
{
    memset(dev, 0, sizeof(*dev));
    dev->netdev.driver = &optmodem_driver;
    dev->state = NETOPT_STATE_IDLE;

    /* Ensure UART index/baud are set by caller prior to init() */
    if (dev->params.uart == UART_UNDEF || dev->params.baud == 0) {
        dev->params.uart = OPTMODEM_UART_DEV;
        dev->params.baud = OPTMODEM_UART_BAUD;
    }
#if OPTMODEM_PRESENTS_AS_ETHERNET
    /* Default to a locally administered MAC if none was provided */
    if (dev->params.mac[0] == 0 &&
        dev->params.mac[1] == 0 &&
        dev->params.mac[2] == 0 &&
        dev->params.mac[3] == 0 &&
        dev->params.mac[4] == 0 &&
        dev->params.mac[5] == 0) {
        /* 02:00:00:xx:yy:zz */
        dev->params.mac[0] = 0x02;
        dev->params.mac[1] = 0x00;
        dev->params.mac[2] = 0x00;
        dev->params.mac[3] = 0x00;
        dev->params.mac[4] = 0x00;
        dev->params.mac[5] = 0x01;
    }
#endif

    return _init(&dev->netdev);
}
