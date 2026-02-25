/*
 * client-coap.h -- RIOT client example
 *
 * Copyright (C) 2023-2025 Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#ifndef CLIENT_COAP_H
#define CLIENT_COAP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Start up the CoAP Client */
void client_coap_init(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* CLIENT_COAP_H */
