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

#ifdef MODULE_AT86RF2XX
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#endif  /* MODULE_AT86RF2XX */

#include "net/netdev.h"
#include "xtimer.h"

#include "netdev_flood.h"

#if     defined(MODULE_AT86RF2XX)
#define NETDEV_ADDR_LEN             (2U)
#define NETDEV_FLOOD_HDR_SEQ_OFFSET (2U)
/* IEEE 802.15.4 header */
#define NETDEV_FLOOD_HDR            { 0x71, 0x98, /* FCF */                 \
                                      0xa1,       /* Sequence number 161 */ \
                                      0x23, 0x00, /* PAN ID 0x23 */         \
                                      0x0e, 0x50, /* 0x500e (start of NETDEV_FLOOD_TARGET) */ \
                                      0x0e, 0x12, /* 0x120e (start of NETDEV_FLOOD_SOURCE) */ }
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
#define NETDEV_FLOOD_HDR            { 0 }
#error "Board not supported by this test; please provide setup for the " \
       "board's network device"
#endif

/**
 * @brief   The address of the flooding source (in-memory version)
 */
static const uint8_t _flood_source[] = NETDEV_FLOOD_SOURCE;

/**
 * @brief   The header for the packets that flood the target (in-memory version)
 */
static uint8_t _flood_hdr[] = NETDEV_FLOOD_HDR;

/**
 * @brief   The payload for the packets that flood the target (in-memory version)
 */
static uint8_t _flood_payload[] = NETDEV_FLOOD_PAYLOAD;

/**
 * @brief   Payload iolist entry for _flood_payload
 */
static iolist_t _flood_payload_ioe = {
    .iol_next = NULL,
    .iol_base = _flood_payload,
    .iol_len = sizeof(_flood_payload),
};

/**
 * @brief   The packet to flood the target with
 */
static iolist_t _flood_pkt = {
    .iol_next = &_flood_payload_ioe,
    .iol_base = _flood_hdr,
    .iol_len = sizeof(_flood_hdr),
};

/**
 * @brief   Setups and initializes the flooding device
 *
 * @note    Please amend for more device support
 */
static void _setup(void);

/**
 * @brief   Get's the flooding device as a netdev_t pointer
 *
 * @note    Please amend for more device support
 *
 * @return  The flooding device as a netdev_t pointer
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

int main(void)
{
    netdev_t *dev;

    _setup();
    xtimer_init();
    dev = _get_netdev();
    if (dev == NULL) {
        puts("Board not supported by this test; please provide setup for the "
             "board's network device");
        return 1;
    }
    dev->driver->init(dev);
    _config_netdev(dev);
    if (_config_address(dev) < 0) {
        puts("Unable to configure address");
        return 1;
    }
    puts("Starting to flood now; start an instance of tests/netdev_flood_replier\n"
         "(on a device with matching link-layer ;-)) to see results.");
    while (1) {
        /* wait for NETDEV_FLOOD_INTERVAL microseconds to send next packet */
        xtimer_usleep(NETDEV_FLOOD_INTERVAL);
        if (dev->driver->send(dev, &_flood_pkt) < 0) {
            puts("Error on send");
        }
        _update_packet(dev);
    }
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
    _flood_hdr[NETDEV_FLOOD_HDR_SEQ_OFFSET]++;
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
                         _flood_source, NETDEV_ADDR_LEN) < 0) {
        return -1;
    }
    return 0;
}

/** @} */
