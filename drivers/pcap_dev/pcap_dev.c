/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <string.h>

#include "pcap_dev.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _send(netdev2_t *netdev, const struct iovec *vector, int count);
static int _recv(netdev2_t *netdev, char *buf, int len, void *info);
static int _init(netdev2_t *netdev);
static void _isr(netdev2_t *netdev);
static int _get(netdev2_t *netdev, netopt_t opt, void *value, size_t max_len);
static int _set(netdev2_t *netdev, netopt_t opt, void *value, size_t value_len);

static const netdev2_driver_t _pcap_dev_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
};

void pcap_dev_setup(pcap_dev_t *dev, const pcap_dev_sd_t *sd, void *sd_state,
                    uint32_t drain_delay, uint8_t *drain_buf,
                    uint16_t drain_buf_size, uint32_t net,
                    const pcap_dev_opt_fn_t getters[],
                    const pcap_dev_opt_fn_t setters[])
{
    netdev2_t *netdev = (netdev2_t *)dev;

    netdev->driver = &_pcap_dev_driver;
    dev->sd = sd;
    dev->sd_state = sd_state;
    dev->drain_buf = drain_buf;
    dev->drain_buf_size = drain_buf_size;
    dev->net = net;
    dev->getters = getters;
    dev->setters = setters;
    /* XXX: use xtimer struct to store drain delay temporarily */
    dev->drain_timer.target = drain_delay;
}

void _trigger_drain(void *dev)
{
    netdev2_t *netdev = dev;

    netdev->event_callback(dev, NETDEV2_EVENT_ISR, netdev->isr_arg);
}

static int _init(netdev2_t *netdev)
{
    pcap_dev_t *dev = (pcap_dev_t *)netdev;
    uint32_t drain_delay;
    int res;
    const pcap_hdr_t pcap_hdr = {
        .magic_number = PCAP_MAGIC_NUMBER,
        .version_major = PCAP_VERSION_MAJOR,
        .version_minor = PCAP_VERSION_MINOR,
        .this_zone = 0,
        .sigfigs = 0,
        .snaplen = 0xffffU,
        .net = dev->net,
    };

    /* get temporarily stored drain delay */
    drain_delay = dev->drain_timer.target;
    /* initialize sink/drain */
    res = dev->sd->init(dev, &pcap_hdr);
    if (res < 0) {
        return res;
    }
    /* initialize drain timer */
    dev->drain_timer.target = 0U;
    dev->drain_timer.long_target = 0U;
    dev->drain_timer.callback = _trigger_drain;
    dev->drain_timer.arg = dev;
    xtimer_set(&dev->drain_timer, drain_delay);
    return 0;
}

static int _send(netdev2_t *netdev, const struct iovec *vector, int count)
{
    pcap_dev_t *dev = (pcap_dev_t *)netdev;
    pcaprec_hdr_t hdr;
    uint64_t now = xtimer_now64();
    uint32_t len = 0;
    int res = 0;

    for (int i = 0; i < count; i++) {
        len += vector[i].iov_len;
    }
    hdr.timestamp = timex_from_uint64(now);
    hdr.incl_len = (uint32_t)len;
    hdr.orig_len = (uint32_t)len;
    if ((res = dev->sd->output(dev, &hdr, sizeof(hdr))) < 0) {
        DEBUG("pcap_dev:_sink: Error writing to sink\n");
        return res;
    }
    len = 0;
    for (int i = 0; i < count; i++) {
        if ((res = dev->sd->output(dev, vector[i].iov_base, vector[i].iov_len)) < 0) {
            DEBUG("pcap_dev:_sink: Error writing to sink\n");
            return res;
        }
    }

    return (int)len;
}

static int _recv(netdev2_t *netdev, char *buf, int len, void *info)
{
    pcap_dev_t *dev = (pcap_dev_t *)netdev;
    pcaprec_hdr_t *hdr = (pcaprec_hdr_t *)dev->drain_buf;

    (void)info;
    if (((unsigned)len) < hdr->orig_len) {
        DEBUG("pcap_dev:_recv: not enough space in buf\n");
        return -ENOBUFS;
    }
    memset(buf, 0, hdr->orig_len);
    /* was nulled in case orig_len > incl_len so this is not redundant */
    /* cppcheck-suppress redundantCopy */
    memcpy(buf, dev->drain_buf + sizeof(pcaprec_hdr_t), hdr->incl_len);

    return hdr->orig_len;
}

static void _isr(netdev2_t *netdev)
{
    if (netdev->event_callback != NULL) {
        pcap_dev_t *dev = (pcap_dev_t *)netdev;
        timex_t this, next;

        netdev->event_callback(netdev, NETDEV2_EVENT_RX_STARTED, NULL);
        if (dev->sd->input(dev, (pcaprec_hdr_t *)dev->drain_buf,
                           dev->drain_buf_size, &next) < 0) {
            DEBUG("pcap_dev:_isr: Error reading drain\n");
            return;
        }
        /* "current" timestamp is stored in the first 8 bytes of the drain
         * buffer */
        memcpy(&this, dev->drain_buf, sizeof(timex_t));
        netdev->event_callback(netdev, NETDEV2_EVENT_RX_COMPLETE, NULL);
        if ((next.seconds != 0) && (next.microseconds != 0)) {
            uint64_t usec = timex_uint64(timex_sub(next, this));
            /* remove drain_timer just to be on the save site (though init
             * and this function are the only ones supposed to set it). */
            xtimer_remove(&dev->drain_timer);
            dev->drain_timer.target = 0U;
            dev->drain_timer.long_target = 0U;
            DEBUG("pcap_dev:_isr: Get next frame in %" PRIu64 " usec\n",
                  usec);
            _xtimer_set64(&dev->drain_timer, (uint32_t)usec, usec >> 32);
        }
    }
}

static int _get(netdev2_t *netdev, netopt_t opt, void *value, size_t max_len)
{
    pcap_dev_t *dev = (pcap_dev_t *)netdev;

    if (dev->getters[opt] != NULL) {
        return dev->getters[opt](netdev, opt, value, max_len);
    }
    else {
        return -ENOTSUP;
    }
}

static int _set(netdev2_t *netdev, netopt_t opt, void *value, size_t value_len)
{
    pcap_dev_t *dev = (pcap_dev_t *)netdev;

    if (dev->setters[opt] != NULL) {
        return dev->setters[opt](netdev, opt, value, value_len);
    }
    else {
        return -ENOTSUP;
    }
}

/** @} */
