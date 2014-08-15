/*
 * Copyright (C) 2014 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef COAP_SERVER_H
#define COAP_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "coap.h"

#define COAP_SERVER_STACK_SIZE (KERNEL_CONF_STACKSIZE_MAIN)
#define COAP_SERVER_HOST "fe80::ff:fe00:1234"
#define COAP_SERVER_PORT 5683

/* CoAP server thread */
void *coap_server(void *params);

/* The main server loop */
void coap_loop(coap_context_t *ctx);

void register_handlers(coap_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* COAP_SERVER_H */
