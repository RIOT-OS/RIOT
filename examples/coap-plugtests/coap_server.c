/*
 * Copyright (C) 2014 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "strings.h"
#include "sys/socket.h"
#include "inet_pton.h"
//#include "netinet/in.h"
#include "arpa/inet.h"
#include "coap.h"
#include "vtimer.h"
#include "coap_server.h"
#include "coap_handlers.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__ ((unused))
#else /* not a GCC */
#define UNUSED_PARAM
#endif /* GCC */

void *coap_server(void *params UNUSED_PARAM)
{
    coap_address_t coap_address;
    coap_context_t *ctx = NULL;

    puts("CoAP server starting...");

    coap_set_log_level(LOG_DEBUG);

    coap_address_init(&coap_address);
    coap_address.sin6_family = AF_INET6;
    coap_address.sin6_port = HTONS(COAP_SERVER_PORT);

    ctx = coap_new_context(&coap_address);

    if (ctx) {
        puts("Server context initialized.");
        register_handlers(ctx);
        puts("Handlers registered.");
        puts("CoAP server listening...");
        coap_loop(ctx);
    }
    else {
        puts("Server context could not be initialized.");
    }

    return NULL;
}

void register_handlers(coap_context_t *ctx)
{
    coap_resource_t *r;

    r = coap_resource_init(NULL, 0, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)index_handler);
    coap_add_resource(ctx, r);

    /* TD_COAP_CORE_{01..08} */
    r = coap_resource_init((unsigned char *)"test", 4, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_01);
    coap_register_handler(r, COAP_REQUEST_DELETE, (coap_method_handler_t)td_coap_core_02);
    coap_register_handler(r, COAP_REQUEST_PUT, (coap_method_handler_t)td_coap_core_03);
    coap_register_handler(r, COAP_REQUEST_POST, (coap_method_handler_t)td_coap_core_04);
    coap_add_attr(r, (unsigned char *)"rt", 2, (unsigned char *)"\"Type1 Type2\"", 13, 0);
    coap_add_attr(r, (unsigned char *)"if", 2, (unsigned char *)"\"If1\"", 5, 0);
    coap_add_resource(ctx, r);

    r = coap_resource_init((unsigned char *)"link1", 5, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_01);
    coap_add_resource(ctx, r);

    r = coap_resource_init((unsigned char *)"link2", 5, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_01);
    coap_add_resource(ctx, r);

    r = coap_resource_init((unsigned char *)"link3", 5, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_01);
    coap_add_resource(ctx, r);

    r = coap_resource_init((unsigned char *)"path", 4, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_link_09);
    coap_add_attr(r, (unsigned char *)"ct", 2, (unsigned char *)"40", 2, 0);
    coap_add_resource(ctx, r);

    r = coap_resource_init((unsigned char *)"path/sub1", 9, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_01);
    coap_add_resource(ctx, r);

    /* TD_COAP_CORE_09 */
    r = coap_resource_init((unsigned char *)"separate", 8, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_09);
    coap_add_attr(r, (unsigned char *)"rt", 2, (unsigned char *)"\"Type2 Type3\"", 13, 0);
    coap_add_attr(r, (unsigned char *)"if", 2, (unsigned char *)"\"If2\"", 5, 0);
    coap_add_resource(ctx, r);

    /* TD_COAP_CORE_13 */
    r = coap_resource_init((unsigned char *)"seg1/seg2/seg3", 14, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_01);
    coap_add_attr(r, (unsigned char *)"rt", 2, (unsigned char *)"\"Type1 Type3\"", 13, 0);
    coap_add_attr(r, (unsigned char *)"if", 2, (unsigned char *)"\"foo\"", 5, 0);
    coap_add_resource(ctx, r);

    /* TD_COAP_CORE_14 */
    r = coap_resource_init((unsigned char *)"query", 5, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_01);
    coap_add_resource(ctx, r);

    /* TD_COAP_CORE_19 */
    r = coap_resource_init((unsigned char *)"location-query", 14, 0);
    coap_register_handler(r, COAP_REQUEST_POST, (coap_method_handler_t)td_coap_core_19);
    coap_add_resource(ctx, r);

    /* TD_COAP_CORE_20 */
    r = coap_resource_init((unsigned char *)"multi-format", 12, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_20);
    coap_add_resource(ctx, r);

    /* TD_COAP_CORE_21 */
    r = coap_resource_init((unsigned char *)"validate", 8, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_core_21);
    coap_register_handler(r, COAP_REQUEST_PUT, (coap_method_handler_t)td_coap_core_03);
    coap_add_resource(ctx, r);

    /* TD_COAP_CORE_23 */
    r = coap_resource_init((unsigned char *)"create1", 7, 0);
    coap_register_handler(r, COAP_REQUEST_PUT, (coap_method_handler_t)td_coap_core_23);
    coap_add_resource(ctx, r);

    /* TD_COAP_BLOCK_01 */
    r = coap_resource_init((unsigned char *)"large", 5, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)td_coap_block_01);
    coap_add_resource(ctx, r);

    r = coap_resource_init((unsigned char *)"threads", 7, 0);
    coap_register_handler(r, COAP_REQUEST_GET, (coap_method_handler_t)threads_handler);
    coap_add_resource(ctx, r);

    init_local_data();
}

void coap_loop(coap_context_t *ctx)
{
    while (1) {
        coap_tick_t now;

        // Returns the next pdu to send without removing from sendqeue.
        coap_queue_t *nextpdu = coap_peek_next(ctx);

        // Saves current ticks in &now
        coap_ticks(&now);

        // Loops over all pdus scheduled to send
        while (nextpdu && nextpdu->t <= now - ctx->sendqueue_basetime) {
            // Handles retransmissions of confirmable messages.
            coap_retransmit(ctx, coap_pop_next(ctx));
            nextpdu = coap_peek_next(ctx);
        }

#ifdef WITH_DESTINY_TIMEOUT
        timex_t timeout;

        /* example with timeout, timeout is fixed in this case but could be set
           dependant on observable resources */
        if (nextpdu && nextpdu->t <= COAP_RESOURCE_CHECK_TIME) {
            /* set timeout if there is a pdu to send before our automatic timeout occurs */
            timeout.microseconds = ((nextpdu->t) % COAP_TICKS_PER_SECOND) * 1000000 / COAP_TICKS_PER_SECOND;
            timeout.seconds = (nextpdu->t) / COAP_TICKS_PER_SECOND;
        }
        else {
            timeout.microseconds = 0;
            timeout.seconds = COAP_RESOURCE_CHECK_TIME;
        }

        coap_try_read(ctx, &timeout);
        printf("Set timeout to %i seconds and %i microseconds.\n", timeout.seconds, timeout.microseconds);
#endif


        coap_read(ctx);
    }

    coap_free_context(ctx);
}
