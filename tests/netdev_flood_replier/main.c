/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif  /* MODULE_AT86RF2XX */

#include "event.h"
#include "event/timeout.h"
#include "net/netdev.h"
#include "od.h"
#include "xtimer.h"

#include "netdev_flood.h"

#if     defined(MODULE_AT86RF2XX)
#define NETDEV_ADDR_LEN             (2U)
#define NETDEV_REPLY_HDR_SEQ_OFFSET (2U)
/* IEEE 802.15.4 header */
#define NETDEV_REPLY_HDR            { 0x71, 0x98, /* FCF */                 \
                                      0x87,       /* Sequence number 135 */ \
                                      0x23, 0x00, /* PAN ID 0x23 */         \
                                      0x0e, 0x12, /* 0x500e (start of NETDEV_FLOOD_SOURCE) */ \
                                      0x0e, 0x50, /* 0x120e (start of NETDEV_FLOOD_TARGET) */ }
#else
/**
 * @brief   The address length for the devices
 *
 * @note    Please define for your device (try to find a common one)
 */
#define NETDEV_ADDR_LEN             (8U)

/**
 * @brief   The header for the packets that flood the target
 *
 * @note    Please define for your device
 */
#define NETDEV_REPLY_HDR            { 0 }
#error "Board not supported by this test; please provide setup for the " \
       "board's network device"
#endif

#define NETDEV_REPLY_PAYLOAD        { 0x41, 0x73, 0x20, 0x49, 0x20, 0x73, 0x61, 0x69, \
                                      0x64, 0x2c, 0x20, 0x73, 0x6f, 0x20, 0x73, 0x6f, \
                                      0x72, 0x72, 0x79, 0x21, 0x20, 0x3e, 0x2e, 0x3c, \
                                      0x2e, 0x2e, 0x2e }

/**
 * @brief   Event type to pass events of netdevs
 */
typedef struct {
    event_t super;      /**< the base class */
    netdev_t *dev;      /**< the network device */
} _event_netdev_t;

/**
 * @brief   The address of the flooding target (in-memory version)
 */
static uint8_t _flood_target[] = NETDEV_FLOOD_TARGET;

/**
 * @brief   The header for the packets that flood the target (in-memory version)
 */
static uint8_t _reply_hdr[] = NETDEV_REPLY_HDR;

/**
 * @brief   The payload for the packets that flood the target (in-memory version)
 */
static uint8_t _reply_payload[] = NETDEV_REPLY_PAYLOAD;

/**
 * @brief   The payload for the packets that flood the target (in-memory version)
 */
static const uint8_t _flood_payload[] = NETDEV_FLOOD_PAYLOAD;

/**
 * @brief   Buffer for reception
 */
static uint8_t _flood_pkt[NETDEV_FLOOD_PKT_SIZE];

/**
 * @brief   Payload iolist entry for _flood_payload
 */
static iolist_t _reply_payload_ioe = {
    .iol_next = NULL,
    .iol_base = _reply_payload,
    .iol_len = sizeof(_reply_payload),
};

/**
 * @brief   The packet to flood the target with
 */
static iolist_t _reply_pkt = {
    .iol_next = &_reply_payload_ioe,
    .iol_base = _reply_hdr,
    .iol_len = sizeof(_reply_hdr),
};

/**
 * @brief   Setups and initializes the replying device
 *
 * @note    Please amend for more device support
 */
static void _setup(void);

/**
 * @brief   Get's the replying device as a netdev_t pointer
 *
 * @note    Please amend for more device support
 *
 * @return  The replying device as a netdev_t pointer
 */
static inline netdev_t *_get_netdev(void);

/**
 * @brief   Updates _flood_pkt according to the needs of the link-layer of the
 *          device (e.g. increment sequence number)
 *
 * @param[in] dev   The device
 *
 * @note    Please amend for more device support
 */
static void _update_packet(netdev_t *dev);

/**
 * @brief   Activates RX complete for the device
 *
 * @param[in] dev   The device
 */
static void _config_netdev(netdev_t *dev);

/**
 * @brief   Configures the address to NETDEV_FLOOD_SOURCE of length
 *          NETDEV_ADDR_LEN for @p dev
 *
 * @param[in] dev   The device
 *
 * @return  0 on success
 * @return  -1 on error
 */
static inline int _config_address(netdev_t *dev);

/**
 * @brief   Fetch flooding packet from device and send reply
 *
 * @param[in] dev The device
 *
 * @note    Please amend for more device support
 */
static void _recv(netdev_t *dev);

static void _event_handler_isr(event_t *event)
{
    _event_netdev_t *ev = (_event_netdev_t *)event;
    netdev_t *dev = ev->dev;
    dev->driver->isr(dev);
}

static void _event_handler_send(event_t *event)
{
    _event_netdev_t *ev = (_event_netdev_t *)event;
    netdev_t *dev = ev->dev;
    if (dev->driver->send(dev, &_reply_pkt) < 0) {
        puts("Error on send");
    }
    _update_packet(dev);
}

static _event_netdev_t _isr_event = {
    .super = { .handler = _event_handler_isr, }
};
static _event_netdev_t _send_event = {
    .super = { .handler = _event_handler_send, }
};
static event_queue_t _event_queue;
static event_timeout_t _event_timeout;

static void _event_handler(netdev_t *dev, netdev_event_t event)
{
    switch (event) {
        case NETDEV_EVENT_ISR:
            event_post(&_event_queue, &_isr_event.super);
            break;
        case NETDEV_EVENT_RX_COMPLETE:
            _recv(dev);
            break;
        default:
            break;
    }
}

int main(void)
{
    netdev_t *dev;

    _setup();
    xtimer_init();
    event_queue_init(&_event_queue);
    dev = _get_netdev();
    if (dev == NULL) {
        puts("Board not supported by this test; please provide setup for the "
             "board's network device");
        return 1;
    }
    _isr_event.dev = dev;
    _send_event.dev = dev;
    event_timeout_init(&_event_timeout, &_event_queue, &_send_event.super);
    dev->event_callback = _event_handler;
    dev->driver->init(dev);
    _config_netdev(dev);
    if (_config_address(dev) < 0) {
        puts("Unable to configure address");
        return 1;
    }
    puts("Starting to flooding packets now; start an instance of\n"
         "tests/netdev_flood_flooder (on a device with matching link-layer ;-))\n"
         "to see results.");
    event_loop(&_event_queue);
    return 0;
}

#ifdef MODULE_AT86RF2XX
static at86rf2xx_t at86rf2xx_dev;
#endif  /* MODULE_AT86RF2XX */

static void _setup(void)
{
#ifdef MODULE_AT86RF2XX
    at86rf2xx_setup(&at86rf2xx_dev, &at86rf2xx_params[0]);
#endif  /* MODULE_AT86RF2XX */
}

static inline netdev_t *_get_netdev(void)
{
#ifdef MODULE_AT86RF2XX
    return (netdev_t *)&at86rf2xx_dev;
#else   /* MODULE_AT86RF2XX */
    return NULL;
#endif  /* MODULE_AT86RF2XX */
}

static void _update_packet(netdev_t *dev)
{
#if     defined(MODULE_AT86RF2XX)
    (void)dev;
    _reply_hdr[NETDEV_REPLY_HDR_SEQ_OFFSET]++;
#endif
}

static void _config_netdev(netdev_t *dev)
{
    static const netopt_enable_t enable = NETOPT_ENABLE;
    int res = dev->driver->set(dev, NETOPT_RX_END_IRQ, &enable, sizeof(enable));

    if (res < 0) {
        puts("enable NETOPT_RX_END_IRQ failed");
    }
}

static inline int _config_address(netdev_t *dev)
{
    if (dev->driver->set(dev, NETOPT_ADDRESS,
                         _flood_target, NETDEV_ADDR_LEN) < 0) {
        return -1;
    }
    return 0;
}

static void _recv(netdev_t *dev)
{
    int nread = dev->driver->recv(dev, _flood_pkt, sizeof(_flood_pkt), NULL);
    size_t mhr_len;

    if (nread <= 0) {
        puts("Error receiving flooding packet");
        return;
    }
    mhr_len = ieee802154_get_frame_hdr_len(_flood_pkt);
    if (memcmp(&_flood_pkt[mhr_len],
               _flood_payload, sizeof(_flood_payload)) != 0) {
        puts("Unexpected payload. This test failed!");
        od_hex_dump(&_flood_pkt[mhr_len], nread - mhr_len, OD_WIDTH_DEFAULT);
        return;
    }
    event_timeout_set(&_event_timeout, 2 * US_PER_MS);
}

/** @} */
