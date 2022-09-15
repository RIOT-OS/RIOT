/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit
 * @defgroup    sys_suit_transport_worker SUIT firmware worker thread
 * @brief       SUIT secure firmware updates worker thread
 *
 * @{
 *
 * @brief       SUIT CoAP helper API
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef SUIT_TRANSPORT_WORKER_H
#define SUIT_TRANSPORT_WORKER_H

#include "net/nanocoap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Start SUIT worker thread
 */
void suit_worker_run(void);

/**
 * @brief   Trigger a SUIT udate
 *
 * @param[in] url       url pointer containing the full coap url to the manifest
 * @param[in] len       length of the url
 */
void suit_worker_trigger(const char *url, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* SUIT_TRANSPORT_WORKER_H */
/** @} */
