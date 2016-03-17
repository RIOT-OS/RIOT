/*
 * Copyright (C) 2016 Hochschule für Angewandte Wissenschaften Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 * @{
 *
 * @file
 * @brief       Netdev adaption for the MRF24J40 drivers
 *
 * @author      Tobias Fredersdorf <Tobias.Fredersdorf@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "net/eui64.h"
#include "net/ieee802154.h"
#include "net/netdev2.h"
#include "net/netdev2_ieee802154.h"

#include "mrf24j40.h"
#include "mrf24j40_netdev.h"
#include "mrf24j40_internal.h"
#include "mrf24j40_registers.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_MHR_OVERHEAD   (25)

static int _send(netdev2_t *netdev, const struct iovec *vector, int count);
static int _recv(netdev2_t *netdev, char *buf, int len);
static int _init(netdev2_t *netdev);
static void _isr(netdev2_t *netdev);
static int _get(netdev2_t *netdev, netopt_t opt, void *val, size_t max_len);
static int _set(netdev2_t *netdev, netopt_t opt, void *val, size_t len);

const netdev2_driver_t mrf24j40_driver = {
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
        dev->event_callback(dev, NETDEV2_EVENT_ISR, NULL);
    }
}

static int _init(netdev2_t *netdev)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;

    /* initialise GPIOs */
    gpio_init(dev->cs_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->cs_pin);
    gpio_init(dev->sleep_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_clear(dev->sleep_pin);
    gpio_init(dev->reset_pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_set(dev->reset_pin);
    gpio_init_int(dev->int_pin, GPIO_NOPULL, GPIO_RISING, _irq_handler, dev);

    /* make sure device is not sleeping, so we can query part number */
    mrf24j40_assert_awake(dev);
    /* reset device to default values and put it into RX state */
    mrf24j40_reset(dev);

    return 0;
}

static int _send(netdev2_t *netdev, const struct iovec *vector, int count)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;
    const struct iovec *ptr = vector;
    size_t len = 0;

    mrf24j40_tx_prepare(dev);

    /* load packet data into FIFO */
    for (int i = 0; i < count; i++, ptr++) {
        /* current packet data + FCS too long */
        if ((len + ptr->iov_len + 2) > MRF24J40_MAX_PKT_LENGTH) {
            printf("[mrf24j40] error: packet too large (%u byte) to be send\n",
                   (unsigned)len + 2);
            return -EOVERFLOW;
        }
        len = mrf24j40_tx_load(dev, ptr->iov_base, ptr->iov_len, len);
    }
/
    return (int)len;
}

static int _recv(netdev2_t *netdev, char *buf, int len)
{
    mrf24j40_t *dev = (mrf24j40_t *)netdev;
    uint8_t phr;
    size_t pkt_len;

    /* frame buffer protection will be unlocked as soon as mrf24j40_fb_stop()
     * is called*/
    mrf24j40_fb_start(dev); 
    /* get the size of the received packet */
    mrf24j40_fb_read(dev, &phr, 1);

    /* Include FCS, LQI and RSSI in packet length */
    pkt_len = phr + 2;
    /* just return length when buf == NULL */
    if (buf == NULL) {
        mrf24j40_fb_stop(dev);
        return pkt_len;
    }
    /* not enough space in buf */
    if (pkt_len > len) {
        mrf24j40_fb_stop(dev);
        return -ENOBUFS;
    }
}