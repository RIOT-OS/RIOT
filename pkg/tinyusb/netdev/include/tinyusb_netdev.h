/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_tinyusb_netdev TinyUSB netdev driver
 * @ingroup     pkg_tinyusb
 * @ingroup     drivers_netdev
 * @brief       TinyUSB netdev driver for CDC ECM, CDC NCM and RNDIS protocol
 *
 * The tinyUSB netdev driver is part of the \ref pkg_tinyusb "tinyUSB package"
 * and is enabled by module `tinyusb_netdev`. It is available for boards that
 * provide the `tinyusb_device` feature.
 *
 * @note Since the tinyUSB package is distinct from (and incompatible with)
 * the USB stack provided around USBUS in RIOT (see USB), the tinyUSB netdev
 * driver cannot be used together with with any USBUS device class.
 *
 * The tinyUSB netdev driver uses Ethernet over USB and supports the following
 * protocols:
 *
 * - CDC ECM (Ethernet Control Model)
 * - CDC NCM (Network Control Model)
 * - RNDIS (Microsoft Remote NDIS)
 *
 * While Linux and macOS support all these protocols, Microsoft Windows
 * only supports the RNDIS protocol and since Windows version 11 also the
 * CDC NCM protocol. macOS supports the RNDIS protocol since version 10.15
 * (Catalina).
 *
 * Which protocol is used is selected by the corresponding
 * pseudomodules `tinyusb_class_net_cdc_ecm`, `tinyusb_class_net_cdc_ncm`
 * and `tinyusb_class_net_rndis`.
 *
 * The CDC ECM protocol (`tinyusb_class_net_cdc_ecm`) and the RNDIS protocol
 * (`tinyusb_class_net_rndis`) can be used simultaneously to support all
 * operating systems, for example :
 * ```
 * USEMODULE='tinyusb_netdev tinyusb_class_net_rndis tinyusb_class_net_cdc_ecm' \
 * BOARD=... make -C ... flash
 * ```
 * In this case, the CDC ECM protocol is the default protocol and the RNDIS
 * protocol the alternative protocol defined as second device configuration.
 *
 * The CDC NCM protocol cannot be used together with the CDC ECM or the RNDIS
 * protocol.
 *
 * @{
 * @file
 * @brief       TinyUSB netdev driver for CDC ECM, CDC NCM and RNDIS protocol
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "net/ethernet.h"
#include "net/netdev.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device descriptor for tinyUSB CDC ECM network devices
 */
typedef struct {
    netdev_t netdev;                        /**< netdev parent struct */
    uint8_t mac_addr[ETHERNET_ADDR_LEN];    /**< device MAC address */
    uint8_t mac_host[ETHERNET_ADDR_LEN];    /**< host MAC address */

    uint16_t rx_len;                        /**< number of bytes received */
    uint16_t tx_len;                        /**< number of bytes in transmit buffer */

    const uint8_t *rx_buf;                  /**< receive buffer in tinyUSB stack */
    uint8_t tx_buf[ETHERNET_MAX_LEN];       /**< transmit buffer */

    mutex_t lock;                           /**< device is already in use */
} tinyusb_netdev_t;

#ifdef __cplusplus
}
#endif

/** @} */
