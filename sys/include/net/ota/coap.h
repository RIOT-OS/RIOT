/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_firmware
 * @{
 *
 * @file
 * @brief       Firmware update via CoAP API
 *
 * See examples/ota for example usage.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#ifndef OTA_COAP_H
#define OTA_COAP_H

#include <stdint.h>

#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CoAP OTA handler function
 *
 * @param[in]       pkt     CoAP pkt to handle
 * @param[out]      buf     reply buffer
 * @param[in]       len     reply buffer length
 * @param[in]       context unused
 *
 * @returns length of reply packet
 */
ssize_t ota_coap_handler(coap_pkt_t* pkt, uint8_t *buf, size_t len, void *context);

/**
 * @brief   Macro for integrating handler into main CoAP resource list
 */
#define COAP_OTA_HANDLER \
    { "/firmware", COAP_PUT | COAP_POST, ota_coap_handler, NULL }

#ifdef __cplusplus
}
#endif

#endif /* OTA_COAP_H */

/** @} */
