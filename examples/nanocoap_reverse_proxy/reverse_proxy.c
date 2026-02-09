/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "event/thread.h"
#include "net/nanocoap.h"
#include "net/nanocoap_proxy.h"

/* set up reverse proxies per supported transport */
#if MODULE_NANOCOAP_WS_UDP_YOLO
static nanocoap_rproxy_ctx_t _yolo_proxy = {
    .evq = EVENT_PRIO_MEDIUM,
    .scheme = "coap+yolo://"
};

NANOCOAP_RESOURCE(yolo_proxy) {
    .path = "/yolo",
    .methods = COAP_GET | COAP_PUT | COAP_POST | COAP_DELETE | COAP_MATCH_SUBTREE,
    .handler= nanocoap_rproxy_handler,
    .context = &_yolo_proxy,
};
#endif

#if MODULE_NANOCOAP_UDP
static nanocoap_rproxy_ctx_t _udp_proxy = {
    .evq = EVENT_PRIO_MEDIUM,
    .scheme = "coap://"
};

NANOCOAP_RESOURCE(udp_proxy) {
    .path = "/udp",
    .methods = COAP_GET | COAP_PUT | COAP_POST | COAP_DELETE | COAP_MATCH_SUBTREE,
    .handler= nanocoap_rproxy_handler,
    .context = &_udp_proxy,
};

static nanocoap_rproxy_ctx_t _fixed_proxy = {
    .evq = EVENT_PRIO_MEDIUM,
    .scheme = "coap://",
    .target_ep = "[::1]:3333",
};

NANOCOAP_RESOURCE(fixed_proxy) {
    .path = "/fixed",
    .methods = COAP_GET | COAP_PUT | COAP_POST | COAP_DELETE | COAP_MATCH_SUBTREE,
    .handler= nanocoap_rproxy_handler,
    .context = &_fixed_proxy,
};
#endif

#if MODULE_NANOCOAP_TCP
static nanocoap_rproxy_ctx_t _tcp_proxy = {
    .evq = EVENT_PRIO_MEDIUM,
    .scheme = "coap+tcp://"
};

NANOCOAP_RESOURCE(tcp_proxy) {
    .path = "/tcp",
    .methods = COAP_GET | COAP_PUT | COAP_POST | COAP_DELETE | COAP_MATCH_SUBTREE,
    .handler= nanocoap_rproxy_handler,
    .context = &_tcp_proxy,
};
#endif
