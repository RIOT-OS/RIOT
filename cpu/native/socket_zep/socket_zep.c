/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 * @author Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "async_read.h"
#include "byteorder.h"
#include "checksum/crc16_ccitt.h"
#include "native_internal.h"

#include "net/ieee802154/radio.h"
#include "socket_zep.h"
#include "random.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define _UNIX_NTP_ERA_OFFSET    (2208988800U)
/* can't use timex.h's US_PER_SEC as timeval's tv_usec is signed long
 * (https://pubs.opengroup.org/onlinepubs/9699919799.2016edition/basedefs/time.h.html) */
#define TV_USEC_PER_SEC         (1000000L)

/* IEEE 802.15.4 ACK delay */
#define ACK_DELAY_US            (IEEE802154_SYMBOL_TIME_US * IEEE802154_ATURNAROUNDTIME_IN_SYMBOLS)

/* dummy packet to register with ZEP dispatcher */
#define SOCKET_ZEP_V2_TYPE_HELLO   (255)

/* simulate RSSI by calculating error function of LQI */
static const uint8_t lqi_to_rssi[256] = {
     74,  74,  74,  74,  74,  74,  74,  74,  74,  75,  75,  75,  75,  75,  75,  75,
     75,  75,  75,  75,  75,  75,  75,  75,  75,  75,  75,  75,  75,  75,  75,  75,
     75,  76,  76,  76,  76,  76,  76,  76,  76,  76,  76,  76,  76,  76,  76,  76,
     77,  77,  77,  77,  77,  77,  77,  77,  77,  77,  77,  77,  78,  78,  78,  78,
     78,  78,  78,  78,  78,  79,  79,  79,  79,  79,  79,  79,  79,  80,  80,  80,
     80,  80,  80,  80,  81,  81,  81,  81,  81,  81,  82,  82,  82,  82,  82,  83,
     83,  83,  83,  83,  83,  84,  84,  84,  84,  85,  85,  85,  85,  85,  86,  86,
     86,  86,  87,  87,  87,  87,  88,  88,  88,  88,  89,  89,  89,  90,  90,  90,
     90,  91,  91,  91,  92,  92,  92,  93,  93,  93,  94,  94,  94,  95,  95,  95,
     96,  96,  96,  97,  97,  97,  98,  98,  98,  99,  99, 100, 100, 100, 101, 101,
    102, 102, 102, 103, 103, 104, 104, 104, 105, 105, 106, 106, 107, 107, 107, 108,
    108, 109, 109, 110, 110, 111, 111, 112, 112, 112, 113, 113, 114, 114, 115, 115,
    116, 116, 117, 117, 118, 118, 119, 119, 120, 120, 121, 121, 122, 122, 123, 123,
    124, 124, 125, 125, 126, 126, 127, 127, 128, 128, 129, 129, 130, 130, 131, 131,
    132, 132, 133, 133, 134, 135, 135, 136, 136, 137, 137, 138, 138, 139, 139, 140,
    140, 141, 141, 142, 142, 143, 143, 144, 144, 145, 145, 146, 146, 147, 147, 148,
};

static size_t _zep_hdr_fill_v2_data(socket_zep_t *dev, zep_v2_data_hdr_t *hdr,
                                    size_t payload_len)
{
    struct timeval tv;

    real_gettimeofday(&tv, NULL);
    hdr->hdr.version = 2;
    hdr->type = ZEP_V2_TYPE_DATA;
    hdr->chan = dev->chan;
    hdr->dev = byteorder_htons((uint16_t)((((intptr_t)dev)) & 0xffff));
    hdr->lqi_mode = 1;
    hdr->lqi_val = 0xff;                /* set by ZEP dispatcher */
    hdr->time.seconds = byteorder_htonl(tv.tv_sec + _UNIX_NTP_ERA_OFFSET);
    assert(tv.tv_usec < TV_USEC_PER_SEC);
    hdr->time.fraction = byteorder_htonl(
            (uint32_t)((uint64_t)tv.tv_usec * TV_USEC_PER_SEC) / 232U
        );
    hdr->seq = byteorder_htonl(dev->seq);
    memset(hdr->resv, 0, sizeof(hdr->resv));
    hdr->length = payload_len;

    return sizeof(zep_v2_data_hdr_t);
}

static inline size_t _zep_hdr_fill(socket_zep_t *dev, zep_hdr_t *hdr,
                                   size_t payload_len)
{
    hdr->preamble[0] = 'E';
    hdr->preamble[1] = 'X';

    /* keep possibility for ZEPv1 open */
    return _zep_hdr_fill_v2_data(dev, (zep_v2_data_hdr_t *)hdr,
                                 payload_len);
}

static void _continue_reading(socket_zep_t *dev)
{
    /* work around lost signals */
    fd_set rfds;
    struct timeval t;
    memset(&t, 0, sizeof(t));
    FD_ZERO(&rfds);
    FD_SET(dev->sock_fd, &rfds);

    _native_pending_syscalls_up(); /* no switching here */

    dev->state = ZEPDEV_STATE_RX_ON;

    if (real_select(dev->sock_fd + 1, &rfds, NULL, NULL, &t) == 1) {
        int sig = SIGIO;
        extern int _signal_pipe_fd[2];
        real_write(_signal_pipe_fd[1], &sig, sizeof(sig));
        _native_pending_signals++;
    }
    else {
        native_async_read_continue(dev->sock_fd);
    }

    _native_pending_syscalls_down();
}

static inline bool _dst_not_me(socket_zep_t *dev, const void *buf)
{
    uint8_t dst_addr[IEEE802154_LONG_ADDRESS_LEN] = { 0 };
    int dst_len;
    le_uint16_t dst_pan = { .u16 = 0 };
    bool is_ack = *(uint8_t *)buf & IEEE802154_FCF_TYPE_ACK;

    /* no need to check address if we are in promiscuous mode */
    if (dev->filter_mode == IEEE802154_FILTER_PROMISC ||
        dev->filter_mode == IEEE802154_FILTER_SNIFFER) {
        return false;
    }

    /* ignore everything but ACK frames */
    if ((dev->filter_mode == IEEE802154_FILTER_ACK_ONLY) && !is_ack) {
        DEBUG("socket_zep::dst_not_me: ignoring non-ACK frame\n");
        return true;
    }

    /* ACKs carry no address */
    if (is_ack) {
        DEBUG("socket_zep::dst_not_me: got ACK\n");
        return false;
    }

    dst_len = ieee802154_get_dst(buf, dst_addr, &dst_pan);

    if (dst_pan.u16 != dev->pan_id) {
        DEBUG("socket_zep::dst_not_me: PAN ID %x != %x\n", dst_pan.u16, dev->pan_id);
        return true;
    }

    switch (dst_len) {
    case IEEE802154_LONG_ADDRESS_LEN:
       return memcmp(dst_addr, dev->addr_long, dst_len);
    case IEEE802154_SHORT_ADDRESS_LEN:
        return memcmp(dst_addr, ieee802154_addr_bcast, dst_len) &&
               memcmp(dst_addr, dev->addr_short, dst_len);
    default:
        return false;    /* better safe than sorry ;-) */
    }
}

static int _bind_local(const socket_zep_params_t *params)
{
    int res;
    static const struct addrinfo hints = { .ai_family = AF_UNSPEC,
                                           .ai_socktype = SOCK_DGRAM };
    struct addrinfo *ai = NULL;

    if (params->local_addr == NULL) {
        return -1;
    }

    /* bind and connect socket */
    if ((res = real_getaddrinfo(params->local_addr, params->local_port, &hints,
                                &ai)) < 0) {
        errx(EXIT_FAILURE, "ZEP: unable to get local address: %s\n",
             gai_strerror(res));
    }

    for (struct addrinfo *local = ai; local != NULL; local = local->ai_next) {
        if ((res = real_socket(local->ai_family, local->ai_socktype,
                               local->ai_protocol)) < 0) {
            continue;
        }
        if (real_bind(res, local->ai_addr, local->ai_addrlen) == 0) {
            break;  /* successfully bound */
        }
    }
    real_freeaddrinfo(ai);

    if (res < 0) {
        err(EXIT_FAILURE, "ZEP: Unable to bind socket");
    }

    return res;
}

static int _connect_remote(socket_zep_t *dev, const socket_zep_params_t *params)
{
    int res;
    static const struct addrinfo hints = { .ai_family = AF_UNSPEC,
                                           .ai_socktype = SOCK_DGRAM };
    struct addrinfo *ai = NULL, *remote;

    if (params->remote_addr == NULL) {
        return -1;
    }

    if ((res = real_getaddrinfo(params->remote_addr, params->remote_port, &hints,
                                &ai)) < 0) {
        errx(EXIT_FAILURE, "ZEP: unable to get remote address: %s\n",
             gai_strerror(res));
    }

    for (remote = ai; remote != NULL; remote = remote->ai_next) {
        if (real_connect(dev->sock_fd, remote->ai_addr, remote->ai_addrlen) == 0) {
            break;  /* successfully connected */
        }
    }

    if (remote == NULL) {
        err(EXIT_FAILURE, "ZEP: Unable to connect socket");
    }

    real_freeaddrinfo(ai);

    return res;
}

static void _send_zep_hello(socket_zep_t *dev)
{
    if (IS_USED(MODULE_SOCKET_ZEP_HELLO) && dev->send_hello) {
        /* dummy packet */
        zep_v2_data_hdr_t hdr = {
            .hdr.preamble = "EX",
            .hdr.version  = 2,
            .type = SOCKET_ZEP_V2_TYPE_HELLO,
            .resv = "HELLO",
            .length = sizeof(dev->addr_long),
        };

        /* append HW addr */
        real_send(dev->sock_fd, &hdr, sizeof(hdr), MSG_MORE);
        real_send(dev->sock_fd, dev->addr_long, sizeof(dev->addr_long), 0);
    }
}

static void _send_ack(void *arg)
{
    ieee802154_dev_t *dev = arg;
    socket_zep_t *zepdev = dev->priv;
    const uint8_t *rxbuf = &zepdev->rcv_buf[sizeof(zep_v2_data_hdr_t)];
    uint8_t ack[3];
    zep_v2_data_hdr_t hdr;

    /* sending ACK should only happen if we received a frame */
    assert(zepdev->state == ZEPDEV_STATE_RX_RECV);
    /* ACK request bit should be set if we get here */
    assert((rxbuf[0] & IEEE802154_FCF_ACK_REQ) != 0);

    DEBUG("socket_zep::send_ack: seq_no: %u\n", rxbuf[2]);

    _zep_hdr_fill(zepdev, &hdr.hdr, sizeof(ack) + IEEE802154_FCF_LEN);

    ack[0] = IEEE802154_FCF_TYPE_ACK; /* FCF */
    ack[1] = 0; /* FCF */
    ack[2] = rxbuf[2];  /* SeqNum */

    /* calculate checksum */
    uint16_t chksum = crc16_ccitt_false_update(0, ack, 3);

    real_send(zepdev->sock_fd, &hdr, sizeof(hdr), MSG_MORE);
    real_send(zepdev->sock_fd, ack, sizeof(ack), MSG_MORE);
    real_send(zepdev->sock_fd, &chksum, sizeof(chksum), 0);

    dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
}

static void _send_frame(void *arg)
{
    ieee802154_dev_t *dev = arg;
    socket_zep_t *zepdev = dev->priv;

    assert(zepdev->state == ZEPDEV_STATE_TX);

    int res = real_write(zepdev->sock_fd, zepdev->snd_buf, zepdev->snd_len);
    DEBUG("socket_zep::send_frame: wrote %d bytes\n", res);

    zepdev->state = ZEPDEV_STATE_IDLE;
    dev->cb(dev, IEEE802154_RADIO_CONFIRM_TX_DONE);
}

static void _socket_isr(int fd, void *arg)
{
    ieee802154_dev_t *dev = arg;
    socket_zep_t *zepdev = dev->priv;
    int res;

    if (zepdev->state != ZEPDEV_STATE_RX_ON) {
        res = real_recv(zepdev->sock_fd, &res, sizeof(res), MSG_TRUNC);
        DEBUG("socket_zep::_socket_isr: discard frame (%d bytes, state %u)\n", res, zepdev->state);
        return;
    }

    zepdev->rcv_len = 0;
    zepdev->state = ZEPDEV_STATE_RX_RECV;

    res = real_recv(zepdev->sock_fd, zepdev->rcv_buf, sizeof(zepdev->rcv_buf), 0);
    DEBUG("socket_zep::_socket_isr: %d bytes on %d\n", res, fd);

    if (res < (int)sizeof(zep_v2_data_hdr_t)) {
        DEBUG("socket_zep::_socket_isr: frame is shorter than the header, %d < %zu\n",
              res, sizeof(zep_v2_data_hdr_t));
        goto out;
    }

    zep_hdr_t *tmp = (zep_hdr_t *)zepdev->rcv_buf;

    if ((tmp->preamble[0] != 'E') || (tmp->preamble[1] != 'X')) {
        DEBUG("socket_zep::read: invalid ZEP header\n");
        goto out;
    }

    if (tmp->version != 2) {
        DEBUG("socket_zep::read: unsupported ZEP version %u\n", tmp->version);
        goto out;
    }

    if (((zep_v2_ack_hdr_t *)tmp)->type != ZEP_V2_TYPE_DATA) {
        DEBUG("socket_zep::read: unknown type %u\n", ((zep_v2_ack_hdr_t *)tmp)->type);
        goto out;
    }

    /* we received a valid ZEP frame */
    zep_v2_data_hdr_t *zep = (zep_v2_data_hdr_t *)tmp;

    if (zep->chan != zepdev->chan) {
        DEBUG("socket_zep::read: wrong channel %d but expected %d\n", zep->chan, zepdev->chan);
        goto out;
    }

    if (_dst_not_me(zepdev, zep + 1)) {
        DEBUG("socket_zep::read: dst not me\n");
        goto out;
    }

    zepdev->rcv_len = res;
    dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_START);

    /* send ACK after 192 µs */
    if ((((uint8_t *)(zep + 1))[0] & IEEE802154_FCF_ACK_REQ) != 0) {
        zepdev->ack_timer.callback = _send_ack;
        ztimer_set(ZTIMER_USEC, &zepdev->ack_timer, ACK_DELAY_US);
    } else {
        dev->cb(dev, IEEE802154_RADIO_INDICATION_RX_DONE);
    }

    return;
out:
    _continue_reading(zepdev);
}

void socket_zep_setup(socket_zep_t *dev, const socket_zep_params_t *params)
{
    DEBUG("socket_zep_setup(%p, %p)\n", (void *)dev, (void *)params);
    assert((params->remote_addr != NULL) && (params->remote_port != NULL));

    dev->params = params;

    native_async_read_setup();
}

void socket_zep_cleanup(socket_zep_t *dev)
{
    assert(dev != NULL);
    /* cleanup signal handling */
    native_async_read_cleanup();
    /* close the socket */
    close(dev->sock_fd);
    dev->sock_fd = 0;
}

static int _request_on(ieee802154_dev_t *dev)
{
    socket_zep_t *zepdev = dev->priv;

    DEBUG("socket_zep::request_on()\n");

    int res = _bind_local(zepdev->params);

    if (res < 0) {
        zepdev->sock_fd = socket(AF_INET6, SOCK_DGRAM, 0);
        res = zepdev->sock_fd;
    } else {
        zepdev->sock_fd = res;
    }

    if (res < 0) {
        return res;
    }

    native_async_read_add_handler(zepdev->sock_fd, dev, _socket_isr);

    /* only send hello if we are connected to a remote */
    zepdev->send_hello = !_connect_remote(zepdev, zepdev->params);

    return 0;
}

static int _confirm_on(ieee802154_dev_t *dev)
{
    (void) dev;
    return 0;
}

static int _off(ieee802154_dev_t *dev)
{
    socket_zep_t *zepdev = dev->priv;

    DEBUG("socket_zep::off()\n");

    close(zepdev->sock_fd);
    zepdev->sock_fd = -1;
    return 0;
}

static int _set_cca_mode(ieee802154_dev_t *dev, ieee802154_cca_mode_t mode)
{
    (void) dev;
    (void) mode;
    return 0;
}

static int _set_cca_threshold(ieee802154_dev_t *dev, int8_t threshold)
{
    (void) dev;
    (void) threshold;
    return 0;
}

static int _set_csma_params(ieee802154_dev_t *dev, const ieee802154_csma_be_t *bd,
                            int8_t retries)
{
    (void) dev;
    (void) bd;
    (void) retries;
    return 0;
}

static int _config_phy(ieee802154_dev_t *dev, const ieee802154_phy_conf_t *conf)
{
    socket_zep_t *zepdev = dev->priv;

    zepdev->chan = conf->channel;
    return 0;
}

static int _config_addr_filter(ieee802154_dev_t *dev, ieee802154_af_cmd_t cmd, const void *value)
{
    socket_zep_t *zepdev = dev->priv;

    switch (cmd) {
    case IEEE802154_AF_SHORT_ADDR:
        memcpy(zepdev->addr_short, value, IEEE802154_SHORT_ADDRESS_LEN);
        break;
    case IEEE802154_AF_EXT_ADDR:
        memcpy(zepdev->addr_long, value, IEEE802154_LONG_ADDRESS_LEN);
        _send_zep_hello(zepdev);
        break;
    case IEEE802154_AF_PANID:
        memcpy(&zepdev->pan_id, value, sizeof(zepdev->pan_id));
        break;
    case IEEE802154_AF_PAN_COORD:
        return -ENOTSUP;
    }

    return 0;
}

static int _config_src_addr_match(ieee802154_dev_t *dev, ieee802154_src_match_t cmd,
                                  const void *value)
{
    (void) dev;
    (void) cmd;
    (void) value;
    return -ENOTSUP;
}

static int _set_frame_filter_mode(ieee802154_dev_t *dev, ieee802154_filter_mode_t mode)
{
    socket_zep_t *zepdev = dev->priv;
    zepdev->filter_mode = mode;
    return 0;
}

static int _write(ieee802154_dev_t *dev, const iolist_t *iolist)
{
    socket_zep_t *zepdev = dev->priv;
    unsigned n = iolist_count(iolist);
    size_t bytes = iolist_size(iolist) + sizeof(uint16_t); /* FCS field */
    uint8_t *out = zepdev->snd_buf;
    uint16_t chksum = 0;

    DEBUG("socket_zep::write(%zu bytes)\n", bytes);

    out += _zep_hdr_fill(zepdev, (void *)out, bytes);

    /* make sure we are not overflowing the TX buffer */
    if (out + bytes > zepdev->snd_buf + sizeof(zepdev->snd_buf)) {
        return -ENOBUFS;
    }

    for (unsigned i = 0; i < n; i++) {
        memcpy(out, iolist->iol_base, iolist->iol_len);
        chksum = crc16_ccitt_false_update(chksum, iolist->iol_base, iolist->iol_len);
        out += iolist->iol_len;
        iolist = iolist->iol_next;
    }
    chksum = byteorder_htols(chksum).u16;
    memcpy(out, &chksum, sizeof(chksum));
    out += sizeof(chksum);

    zepdev->snd_len = out - zepdev->snd_buf;

    return 0;
}

static int _request_transmit(ieee802154_dev_t *dev)
{
    socket_zep_t *zepdev = dev->priv;

    zepdev->state = ZEPDEV_STATE_TX;

    /* 8 bit are mapped to 2 symbols */
    unsigned time_tx = 2 * zepdev->snd_len * IEEE802154_SYMBOL_TIME_US;
    DEBUG("socket_zep::request_transmit(%u bytes, %u µs)\n", zepdev->snd_len, time_tx);

    dev->cb(dev, IEEE802154_RADIO_INDICATION_TX_START);

    /* delay transmission to simulate airtime */
    zepdev->ack_timer.callback = _send_frame;
    ztimer_set(ZTIMER_USEC, &zepdev->ack_timer, time_tx);

    return 0;
}

static int _confirm_transmit(ieee802154_dev_t *dev, ieee802154_tx_info_t *info)
{
    (void) dev;

    if (info) {
        info->status = TX_STATUS_SUCCESS;
    }

    return 0;
}

int _len(ieee802154_dev_t *dev)
{
    socket_zep_t *zepdev = dev->priv;

    zep_v2_data_hdr_t *hdr = (zep_v2_data_hdr_t *)zepdev->rcv_buf;

    DEBUG("socket_zep::len: %u\n", hdr->length - IEEE802154_FCS_LEN);

    /* report size without ZEP header and checksum */
    return hdr->length - IEEE802154_FCS_LEN;
}

static int _read(ieee802154_dev_t *dev, void *buf, size_t max_size,
                 ieee802154_rx_info_t *info)
{
    socket_zep_t *zepdev = dev->priv;
    size_t res;

    DEBUG("socket_zep::read: reading up to %zu bytes into %p\n", max_size, buf);

    if (buf == NULL || zepdev->rcv_len == 0) {
        return 0;
    }

    DEBUG("socket_zep::read: %zu/%zu bytes into %p\n",
           max_size, zepdev->rcv_len - sizeof(zep_v2_data_hdr_t) - IEEE802154_FCS_LEN, buf);

    if (max_size != zepdev->rcv_len - sizeof(zep_v2_data_hdr_t) - IEEE802154_FCS_LEN) {
        DEBUG("socket_zep::read: size mismatch!\n");
        return -EINVAL;
    }

    zep_v2_data_hdr_t *zep = (zep_v2_data_hdr_t *)zepdev->rcv_buf;

    if (info) {
        info->lqi = zep->lqi_val;
        info->rssi = lqi_to_rssi[zep->lqi_val]
                   /* slightly randomize simulated RSSI */
                   + ((random_uint32() & 0x3) - 2);
    }

    /* return payload size without frame checksum */
    res = zep->length - IEEE802154_FCS_LEN;
    if (res > max_size) {
        return -ENOBUFS;
    }

    /* skip the ZEP header, just copy payload without FCS */
    memcpy(buf, zep + 1, res);
    return res;
}

static int _request_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    socket_zep_t *zepdev = dev->priv;
    int res = -ENOTSUP;
    (void) ctx;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        if (zepdev->state != ZEPDEV_STATE_IDLE) {
            return -EBUSY;
        }
        res = _request_transmit(dev);
        break;
    case IEEE802154_HAL_OP_SET_RX:
        switch (zepdev->state) {
        case ZEPDEV_STATE_IDLE:
            DEBUG("socket_zep::request_op: switch to state RX_ON\n");
            _continue_reading(zepdev);
            return 0;
        case ZEPDEV_STATE_TX:
            DEBUG("socket_zep::request_op: request RX in state TX\n");
            return -EBUSY;
        case ZEPDEV_STATE_RX_RECV:
            DEBUG("socket_zep::request_op: already have RX frame\n");
            return -EBUSY;
        case ZEPDEV_STATE_RX_ON:
            DEBUG("socket_zep::request_op: already in state RX_ON\n");
            return 0;
        }

        break;
    case IEEE802154_HAL_OP_SET_IDLE:
        assert(ctx);
        bool force = *((bool*) ctx);

        DEBUG("socket_zep::request_op: switch to IDLE from %u\n", zepdev->state);
        if (force || zepdev->state != ZEPDEV_STATE_TX) {
            ztimer_remove(ZTIMER_USEC, &zepdev->ack_timer);
            zepdev->state = ZEPDEV_STATE_IDLE;
        } else {
            return -EBUSY;
        }

        res = 0;
        break;
    case IEEE802154_HAL_OP_CCA:
        res = -ENOTSUP;
        break;
    }

    return res;
}

static int _confirm_op(ieee802154_dev_t *dev, ieee802154_hal_op_t op, void *ctx)
{
    socket_zep_t *zepdev = dev->priv;
    int res = -EAGAIN;

    switch (op) {
    case IEEE802154_HAL_OP_TRANSMIT:
        res = _confirm_transmit(dev, ctx);
        break;
    case IEEE802154_HAL_OP_SET_RX:
        /* we are still in RX state while ACK is being sent */
        if (zepdev->state != ZEPDEV_STATE_RX_ON &&
            zepdev->state != ZEPDEV_STATE_RX_RECV) {
            break;
        }
        /* fall-through */
    case IEEE802154_HAL_OP_SET_IDLE:
        res = 0;
        break;
    case IEEE802154_HAL_OP_CCA:
        /* This shouldn't happen! */
        assert(false);
        break;
    }

    return res;
}

static const ieee802154_radio_ops_t socket_zep_rf_ops = {
    .caps = IEEE802154_CAP_24_GHZ
          | IEEE802154_CAP_AUTO_CSMA
          | IEEE802154_CAP_IRQ_TX_DONE
          | IEEE802154_CAP_IRQ_TX_START
          | IEEE802154_CAP_IRQ_RX_START
          | IEEE802154_CAP_PHY_OQPSK,

    .write = _write,
    .read = _read,
    .request_op = _request_op,
    .confirm_op = _confirm_op,
    .len = _len,
    .off = _off,
    .request_on = _request_on,
    .confirm_on = _confirm_on,
    .set_cca_threshold = _set_cca_threshold,
    .set_cca_mode = _set_cca_mode,
    .config_phy = _config_phy,
    .config_addr_filter = _config_addr_filter,
    .config_src_addr_match = _config_src_addr_match,
    .set_csma_params = _set_csma_params,
    .set_frame_filter_mode = _set_frame_filter_mode,
};

void socket_zep_hal_setup(socket_zep_t *dev, ieee802154_dev_t *hal)
{
    hal->driver = &socket_zep_rf_ops;
    hal->priv = dev;

    dev->ack_timer.arg = hal;
}
