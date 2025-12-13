/*
 * server_coap.c -- RIOT example
 *
 * Copyright (C) 2023-2025 Jon Shallow <supjps-libcoap@jpshallow.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

#include "coap_config.h"
#include <coap3/coap.h>
#include "server_coap.h"
#include <stdio.h>
#include "macros/utils.h"

#ifdef CONFIG_LIBCOAP_USE_PSK
#define COAP_USE_PSK CONFIG_LIBCOAP_USE_PSK
#else /* CONFIG_LIBCOAP_USE_PSK */
#define COAP_USE_PSK NULL
#endif /* CONFIG_LIBCOAP_USE_PSK */

static volatile int running = 0;
static int quit;

coap_context_t *main_coap_context;

static coap_time_t clock_offset;
/* changeable clock base (see handle_put_time()) */
static coap_time_t my_clock_base = 0;
static coap_resource_t *time_resource = NULL; /* just for testing */

static void
hnd_get_time(coap_resource_t *resource, coap_session_t  *session,
             const coap_pdu_t *request, const coap_string_t *query,
             coap_pdu_t *response)
{
    unsigned char buf[40];
    size_t len;
    coap_tick_t now;
    coap_tick_t t;

    (void)resource;
    (void)session;
    (void)request;
    /* FIXME: return time, e.g. in human-readable by default and ticks
     * when query ?ticks is given. */

    /* if my_clock_base was deleted, we pretend to have no such resource */
    coap_pdu_set_code(response, my_clock_base ? COAP_RESPONSE_CODE_CONTENT :
                      COAP_RESPONSE_CODE_NOT_FOUND);
    if (my_clock_base) {
        coap_add_option(response, COAP_OPTION_CONTENT_FORMAT,
                        coap_encode_var_safe(buf, sizeof(buf),
                                             COAP_MEDIATYPE_TEXT_PLAIN),
                        buf);
    }

    coap_add_option(response, COAP_OPTION_MAXAGE,
                    coap_encode_var_safe(buf, sizeof(buf), 0x01), buf);

    if (my_clock_base) {

        /* calculate current time */
        coap_ticks(&t);
        now = my_clock_base + (t / COAP_TICKS_PER_SECOND);

        if (query != NULL
            && coap_string_equal(query, coap_make_str_const("ticks"))) {
            /* output ticks */
            len = snprintf((char *)buf, sizeof(buf), "%u", (unsigned int)now);
            coap_add_data(response, len, buf);
        }
    }
}

static void
init_coap_resources(coap_context_t *ctx)
{
    coap_resource_t *r;

#if 0
    r = coap_resource_init(NULL, 0, 0);
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_index);

    coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
    coap_add_attr(r, coap_make_str_const("title"), coap_make_str_const("\"General Info\""), 0);
    coap_add_resource(ctx, r);
#endif
    /* store clock base to use in /time */
    my_clock_base = clock_offset;

    r = coap_resource_init(coap_make_str_const("time"), 0);
    if (!r) {
        goto error;
    }

    coap_resource_set_get_observable(r, 1);
    time_resource = r;
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_time);
#if 0
    coap_register_handler(r, COAP_REQUEST_PUT, hnd_put_time);
    coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete_time);
#endif
    coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
    /* coap_add_attr(r, coap_make_str_const("title"),
                     coap_make_str_const("\"Internal Clock\""), 0); */
    coap_add_attr(r, coap_make_str_const("rt"), coap_make_str_const("\"ticks\""), 0);
    coap_add_attr(r, coap_make_str_const("if"), coap_make_str_const("\"clock\""), 0);

    coap_add_resource(ctx, r);
#if 0
    if (coap_async_is_supported()) {
        r = coap_resource_init(coap_make_str_const("async"), 0);
        coap_register_handler(r, COAP_REQUEST_GET, hnd_get_async);

        coap_add_attr(r, coap_make_str_const("ct"), coap_make_str_const("0"), 0);
        coap_add_resource(ctx, r);
    }
#endif

    return;
error:
    coap_log_crit("cannot create resource\n");
}

static int
init_coap_context_endpoints(const char *use_psk)
{
    coap_address_t listenaddress;
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    ipv6_addr_t addr;
    char addr_str[INET6_ADDRSTRLEN + 8];
    int scheme_hint_bits = 1 << COAP_URI_SCHEME_COAP;
    coap_addr_info_t *info = NULL;
    coap_addr_info_t *info_list = NULL;
    coap_str_const_t local;
    int have_ep = 0;

    /* Get the first address on the interface */
    if (gnrc_netif_ipv6_addrs_get(netif, &addr, sizeof(addr)) < 0) {
        puts("Unable to get first address of the interface");
        return 0;
    }

    coap_address_init(&listenaddress);
    listenaddress.riot.family = AF_INET6;
    memcpy(&listenaddress.riot.addr.ipv6, &addr,
           sizeof(listenaddress.riot.addr.ipv6));
    coap_print_ip_addr(&listenaddress, addr_str, sizeof(addr_str));
    coap_log_info("Server IP [%s]\n", addr_str);

    main_coap_context = coap_new_context(NULL);
    if (!main_coap_context) {
        return 0;
    }

    if (use_psk && coap_dtls_is_supported()) {
        coap_dtls_spsk_t setup_data;

        /* Need PSK set up before setting up endpoints */
        memset(&setup_data, 0, sizeof(setup_data));
        setup_data.version = COAP_DTLS_SPSK_SETUP_VERSION;
        setup_data.psk_info.key.s = (const uint8_t *)use_psk;
        setup_data.psk_info.key.length = strlen(use_psk);
        coap_context_set_psk2(main_coap_context, &setup_data);
        scheme_hint_bits |= 1 << COAP_URI_SCHEME_COAPS;
    }

    local.s = (uint8_t *)addr_str;
    local.length = strlen(addr_str);
    info_list = coap_resolve_address_info(&local, COAP_DEFAULT_PORT,
                                          COAPS_DEFAULT_PORT,
                                          0, 0,
                                          0,
                                          scheme_hint_bits,
                                          COAP_RESOLVE_TYPE_REMOTE);
    for (info = info_list; info != NULL; info = info->next) {
        coap_endpoint_t *ep;

        ep = coap_new_endpoint(main_coap_context, &info->addr, info->proto);
        if (!ep) {
            coap_log_warn("cannot create endpoint for proto %u\n",
                          info->proto);
        }
        else {
            have_ep = 1;
        }
    }
    coap_free_address_info(info_list);
    if (!have_ep) {
        return 0;
    }

    return 1;
}

void *
server_coap_run(void *arg)
{
    (void)arg;

    /* Initialize libcoap library */
    coap_startup();

    coap_set_log_level(COAP_MAX_LOGGING_LEVEL);

    if (!init_coap_context_endpoints(COAP_USE_PSK)) {
        goto fail;
    }

    /* Limit the number of idle sessions to save RAM */
    coap_context_set_max_idle_sessions(main_coap_context, 2);
    clock_offset = 1; /* Need a non-zero value */
    init_coap_resources(main_coap_context);

    coap_log_info("libcoap server ready\n");
    /* Keep on processing ... */
    while (quit == 0) {
        coap_io_process(main_coap_context, 1000);
    }
fail:
    /* Clean up library usage so client can be run again */
    coap_free_context(main_coap_context);
    main_coap_context = NULL;
    coap_cleanup();
    running = 0;
    quit = 0;
    coap_log_info("libcoap server stopped\n");
    return NULL;
}

static char server_stack[THREAD_STACKSIZE_MAIN +
                         THREAD_EXTRA_STACKSIZE_PRINTF];

static
void
start_server(void)
{
    kernel_pid_t server_pid;

    /* Only one instance of the server */
    if (running) {
        puts("Error: server already running");
        return;
    }

    /* The server is initialized */
    server_pid = thread_create(server_stack,
                               sizeof(server_stack),
                               THREAD_PRIORITY_MAIN - 1,
                               THREAD_CREATE_STACKTEST,
                               server_coap_run, NULL, "libcoap_server");

    /* Uncommon but better be sure */
    if (server_pid == EINVAL) {
        puts("ERROR: Thread invalid");
        return;
    }

    if (server_pid == EOVERFLOW) {
        puts("ERROR: Thread overflow!");
        return;
    }

    running = 1;
    return;
}

static
void
stop_server(void)
{
    /* check if server is running at all */
    if (running == 0) {
        puts("Error: libcoap server is not running");
        return;
    }

    quit = 1;

    puts("Stopping server...");
}

void
server_coap_init(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s start|stop\n", argv[0]);
        return;
    }
    if (strcmp(argv[1], "start") == 0) {
        start_server();
    }
    else if (strcmp(argv[1], "stop") == 0) {
        stop_server();
    }
    else {
        printf("Error: invalid command. Usage: %s start|stop\n", argv[0]);
    }
    return;
}
