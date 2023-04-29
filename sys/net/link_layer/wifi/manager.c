/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_wifi_manager
 * @{
 *
 * @file
 * @brief   WiFi manager implementation
 *
 * The manager dispatches events from WiFi interfaces to registered listeners
 * and instructs a managed interface to automatically connect to stored
 * access points.
 *
 * @author  Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <string.h>

#include "cib.h"
#include "compiler_hints.h"
#include "configuration/wifi.h"
#include "container.h"
#include "list.h"
#include "msg.h"
#include "mutex.h"
#include "net/netdev.h"
#include "net/netdev/wifi.h"
#include "net/netopt.h"
#include "net/wifi_manager.h"
#include "net/wifi_scan_list.h"
#include "string_utils.h"
#include "thread.h"
#include "ztimer/periodic.h"

#define ENABLE_DEBUG    0
#include "debug.h"

kernel_pid_t wifi_manager_pid = KERNEL_PID_UNDEF;

static char _stack[WIFI_MANAGER_STACK_SIZE];
static msg_t _msg_queue[WIFI_MANAGER_MSG_QUEUE_SIZE];
static cib_t _ctx_cib;
static wifi_manager_event_ctx_t _event_ctx[WIFI_MANAGER_MSG_QUEUE_SIZE];
static wifi_manager_netif_ctx_t _netif_ctx[WIFI_MANAGER_NETIF_CTX_SIZE];
static mutex_t _event_ctx_lock = MUTEX_INIT;
static mutex_t _netif_ctx_lock = MUTEX_INIT;
static list_node_t _listeners;

void wifi_manager_register_listener(wifi_manager_listener_t *listener)
{
    assert(listener);
    assert(!listener->node.next);
    mutex_lock(&_event_ctx_lock);
    list_add(&_listeners, &listener->node);
    mutex_unlock(&_event_ctx_lock);
}

void wifi_manager_deregister_listener(wifi_manager_listener_t *listener)
{
    assert(listener);
    mutex_lock(&_event_ctx_lock);
    list_remove(&_listeners, &listener->node);
    listener->node.next = NULL;
    mutex_unlock(&_event_ctx_lock);
}

static void _wifi_manager_call_on_scan_listeners(void *netif, const wifi_scan_result_t *scan)
{
    static struct {
        wifi_scan_list_t head;
        wifi_scan_list_node_t array[WIFI_MANAGER_SCAN_LIST_SIZE];
    } _manager_scan_list;
    unsigned i;
    for (i = 0; i < WIFI_MANAGER_SCAN_LIST_SIZE && *scan[i].ssid; i++) {
        _manager_scan_list.array[i].node.next = &_manager_scan_list.array[i + 1].node;
        _manager_scan_list.array[i].result = scan[i];
    }
    if (i) {
        _manager_scan_list.head.head.next = &_manager_scan_list.array[0].node;
        _manager_scan_list.array[i - 1].node.next = NULL;
    }
    mutex_lock(&_event_ctx_lock);
    for (const list_node_t *n = _listeners.next; n; n = n->next) {
        const wifi_manager_listener_t *listener = container_of(n, wifi_manager_listener_t, node);
        if (listener->on_scan) {
            listener->on_scan(netif, &_manager_scan_list.head);
        }
    }
    mutex_unlock(&_event_ctx_lock);
}

static void _wifi_manager_call_on_connect_listeners(void *netif, const wifi_connect_result_t *connect)
{
    mutex_lock(&_event_ctx_lock);
    for (const list_node_t *n = _listeners.next; n; n = n->next) {
        const wifi_manager_listener_t *listener = container_of(n, wifi_manager_listener_t, node);
        if (listener->on_connect) {
            listener->on_connect(netif, connect);
        }
    }
    mutex_unlock(&_event_ctx_lock);
}

static void _wifi_manager_call_on_disconnect_listeners(void *netif, const wifi_disconnect_result_t *disconnect)
{
    mutex_lock(&_event_ctx_lock);
    for (const list_node_t *n = _listeners.next; n; n = n->next) {
        const wifi_manager_listener_t *listener = container_of(n, wifi_manager_listener_t, node);
        if (listener->on_disconnect) {
            listener->on_disconnect(netif, disconnect);
        }
    }
    mutex_unlock(&_event_ctx_lock);
}

static bool _periodic_scan_cb(void *arg)
{
    wifi_manager_netif_ctx_t *netif = arg;
    msg_t m = {
        .type = WIFI_MANAGER_EVENT_DO_SCAN,
        .content.ptr = netif_get_by_id(netif->netif_pid),
    };
    msg_send_int(&m, wifi_manager_pid);
    return true; /* keep going */
}

static wifi_manager_netif_ctx_t *_wifi_manager_netif_get_by_pid(kernel_pid_t netif_pid)
{
    wifi_manager_netif_ctx_t *ctx = NULL;
    mutex_lock(&_netif_ctx_lock);
    for (unsigned i = 0; !ctx && i < ARRAY_SIZE(_netif_ctx); i++) {
        if (netif_pid == _netif_ctx[i].netif_pid) {
            ctx = &_netif_ctx[i];
        }
    }
    mutex_unlock(&_netif_ctx_lock);
    return ctx;
}

const wifi_manager_netif_ctx_t *wifi_manager_netif_get_by_pid(kernel_pid_t netif_pid)
{
    /* public API only returns const pointer */
    return _wifi_manager_netif_get_by_pid(netif_pid);
}

static wifi_manager_netif_ctx_t *_wifi_manager_netif_add_by_pid(kernel_pid_t netif_pid,
                                                                const wifi_manager_connection_hint_t *hint,
                                                                bool periodic_scan)
{
    wifi_manager_netif_ctx_t *ctx = NULL;
    const netif_t *netif = netif_get_by_id(netif_pid);
    if (!netif) {
        return NULL;
    }
    mutex_lock(&_netif_ctx_lock);
    for (unsigned i = 0; i < ARRAY_SIZE(_netif_ctx); i++) {
        if (!_netif_ctx[i].netif_pid) {
            ctx = &_netif_ctx[i];
        }
        else if (_netif_ctx[i].netif_pid == netif_pid) {
            /* already added but add hint */
            if (hint) {
                _netif_ctx[i].hint = hint;
            }
            break;
        }
    }
    if (ctx) {
        ctx->netif_pid = netif_pid;
        if (hint) {
            ctx->hint = hint;
        }
        ctx->no_periodic_scan = !periodic_scan;
        ztimer_periodic_init(ZTIMER_SEC, &ctx->timer,
                             _periodic_scan_cb, ctx,
                             CONFIG_WIFI_MANAGER_AUTOSCAN_TIMEOUT_SEC);
        if (wifi_manager_pid != KERNEL_PID_UNDEF && !ctx->no_periodic_scan) {
            /* start if manager thread is already running */
            ztimer_periodic_start(&ctx->timer);
        }
    }
    mutex_unlock(&_netif_ctx_lock);
    return ctx;
}

const wifi_manager_netif_ctx_t *wifi_manager_netif_add_by_pid(kernel_pid_t netif_pid,
                                                              const wifi_manager_connection_hint_t *hint,
                                                              bool periodic_scan)
{
    /* public API only returns const pointer */
    return _wifi_manager_netif_add_by_pid(netif_pid, hint, periodic_scan);
}

static void _wifi_manager_scan_cb(void *netif, const wifi_scan_list_t *res)
{
    int pos = cib_put(&_ctx_cib);
    if (pos < 0) {
        DEBUG("WiFi manager callback: scan done - no context available");
        return;
    }
    kernel_pid_t netif_pid = thread_get_active()->pid;
    msg_t m = {
        .sender_pid = netif_pid,
        .type = WIFI_MANAGER_EVENT_SCAN_DONE,
        .content = {
            .ptr = netif,
        },
    };
    _event_ctx[pos].event = WIFI_MANAGER_EVENT_SCAN_DONE;
    memset(_event_ctx[pos].result.scan, 0, sizeof(_event_ctx[pos].result.scan));
    wifi_scan_list_to_array(res, _event_ctx[pos].result.scan,
                            ARRAY_SIZE(_event_ctx[pos].result.scan));
    msg_send(&m, wifi_manager_pid);
}

static void _wifi_manager_connect_cb(void *netif, const wifi_connect_result_t *res)
{
    int pos = cib_put(&_ctx_cib);
    if (pos < 0) {
        DEBUG("WiFi manager callback: connect - no context available");
        return;
    }
    kernel_pid_t netif_pid = thread_get_active()->pid;
    msg_t m = {
        .sender_pid = netif_pid,
        .type = WIFI_MANAGER_EVENT_CONNECTED,
        .content = {
            .ptr = netif,
        },
    };
    _event_ctx[pos].event = WIFI_MANAGER_EVENT_CONNECTED;
    _event_ctx[pos].result.connect = *res;
    msg_send(&m, wifi_manager_pid);
}

static void _wifi_manager_disconnect_cb(void *netif, const wifi_disconnect_result_t *res)
{
    int pos = cib_put(&_ctx_cib);
    if (pos < 0) {
        DEBUG("WiFi manager callback: disconenct - no context available");
        return;
    }
    kernel_pid_t netif_pid = thread_get_active()->pid;
    msg_t m = {
        .sender_pid = netif_pid,
        .type = WIFI_MANAGER_EVENT_DISCONNECTED,
        .content = {
            .ptr = netif,
        },
    };
    _event_ctx[pos].event = WIFI_MANAGER_EVENT_DISCONNECTED;
    _event_ctx[pos].result.disconnect = *res;
    msg_send(&m, wifi_manager_pid);
}

static void _wifi_manager_auto_connect(const wifi_manager_netif_ctx_t *netif_ctx,
                                       const wifi_scan_result_t *scan)
{
    assert(netif_ctx);
    const netif_t *netif = netif_get_by_id(netif_ctx->netif_pid);
    assert(netif); (void)netif;
#if IS_USED(MODULE_WIFI_CONFIGURATION)
    wifi_conf_ap_t conf;
#endif
    const wifi_security_mode_t *sec = NULL;
    if (netif_ctx->hint && netif_ctx->hint->ssid) {
        for (unsigned i = 0; !sec && i < WIFI_MANAGER_SCAN_LIST_SIZE && *(scan[i].ssid); i++) {
            if (strcmp(netif_ctx->hint->ssid, scan[i].ssid)) {
                continue;
            }
            if (netif_ctx->hint->sec) {
                if ((*netif_ctx->hint->sec) != scan[i].sec_mode) {
                    continue;
                }
                sec = netif_ctx->hint->sec;
                scan = &scan[i];
            }
#if IS_USED(MODULE_WIFI_CONFIGURATION)
           if (!netif_ctx->hint->sec) {
                if (wifi_configuration_ap_get(&conf, scan[i].ssid, NULL)) {
                    continue;
                }
                sec = &conf.sec.sec;
                scan = &scan[i];
           }
#endif
        }
    }
    if (!sec) {
#if IS_USED(MODULE_WIFI_CONFIGURATION)
        for (unsigned i = 0; !sec && i < WIFI_MANAGER_SCAN_LIST_SIZE && *(scan[i].ssid); i++) {
            if (!wifi_configuration_ap_get(&conf, scan[i].ssid, NULL)) {
                sec = &conf.sec.sec;
                scan = &scan[i];
            }
        }
#endif
    }
    if (sec) {
        wifi_manager_connect(netif_ctx->netif_pid,
                             scan->ssid, sec,
                             scan->base.channel);
    }
}

static void _wifi_manager_save_ap(const wifi_connect_result_t *connect)
{
    (void)connect;
#if IS_USED(MODULE_WIFI_CONFIGURATION)
    wifi_conf_ap_t ap;
    memset(&ap, 0, sizeof(ap));
    strcpy(ap.ssid.ssid, connect->ssid);
    if ((ap.sec.sec = connect->credentials.sec) == WIFI_SECURITY_MODE_WEP_PSK) {
        ap.sec.wep = connect->credentials.wep;
    }
    else if (connect->credentials.sec == WIFI_SECURITY_MODE_WPA2_PERSONAL) {
        ap.sec.wpa_psk = connect->credentials.wpa_psk;
    }
    else if (connect->credentials.sec == WIFI_SECURITY_MODE_WPA2_ENTERPRISE) {
        ap.sec.wpa_enterprise = connect->credentials.wpa_enterprise;
    }
    int ret; (void)ret;
    if ((ret = wifi_configuration_ap_add(&ap, true))) {
        DEBUG("WiFi manager: could not save access point due to error %d\n", ret);
    }
#endif
}

NORETURN
static void *_wifi_manager_thread(void *arg)
{
    (void)arg;
    msg_init_queue(_msg_queue, ARRAY_SIZE(_msg_queue));

    while (1) {
        msg_t m;
        msg_receive(&m);

        if (m.type == WIFI_MANAGER_EVENT_DO_SCAN) {
            netopt_state_t state;
            netif_t *netif = m.content.ptr;
            if (netif_get_opt(netif, NETOPT_STATE, 0,
                              &state, sizeof(state)) > 0 &&
                              state != NETOPT_STATE_SLEEP) {
                wifi_manager_scan(netif_get_id(netif));
            }
        }
        else if (m.type == WIFI_MANAGER_EVENT_ENABLE) {
            wifi_manager_enable();
        }
        else if (m.type == WIFI_MANAGER_EVENT_DISABLE) {
            wifi_manager_disable();
        }
        else {
            netif_t *netif = m.content.ptr;
            wifi_manager_event_ctx_t event = _event_ctx[cib_get_unsafe(&_ctx_cib)];
            wifi_manager_netif_ctx_t *netif_ctx = _wifi_manager_netif_get_by_pid(netif_get_id(netif));
            if (m.type == WIFI_MANAGER_EVENT_SCAN_DONE) {
                _wifi_manager_call_on_scan_listeners(netif, event.result.scan);
                if (netif_ctx) {
                    bool is_connected;
                    if (netif_get_opt(netif, NETOPT_LINK, 0,
                                      &is_connected, sizeof(is_connected)) > 0 && !is_connected) {
                        _wifi_manager_auto_connect(netif_ctx, event.result.scan);
                    }
                }
            }
            else if (m.type == WIFI_MANAGER_EVENT_CONNECTED) {
                if (netif_ctx && !netif_ctx->no_periodic_scan) {
                    ztimer_periodic_stop(&netif_ctx->timer);
                }
                _wifi_manager_save_ap(&event.result.connect);
                _wifi_manager_call_on_connect_listeners(netif, &event.result.connect);
            }
            else if (m.type == WIFI_MANAGER_EVENT_DISCONNECTED) {
                if (netif_ctx && !netif_ctx->no_periodic_scan) {
                    ztimer_periodic_start(&netif_ctx->timer);
                }
                _wifi_manager_call_on_disconnect_listeners(netif, &event.result.disconnect);
            }
        }
    }
}

void wifi_manager_start(void)
{
    if (wifi_manager_pid == KERNEL_PID_UNDEF) {
        wifi_manager_pid = thread_create(_stack, sizeof(_stack),
                                         THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST,
                                         _wifi_manager_thread, NULL,
                                         "wifi_manager");

        wifi_manager_enable();
    }
}

int wifi_manager_scan(kernel_pid_t netif_pid)
{
    netif_t *netif = netif_get_by_id(netif_pid);
    if (!netif) {
        return -ENOENT;
    }
    wifi_scan_request_t request
        = WIFI_SCAN_REQUEST_INITIALIZER(NETOPT_SCAN_REQ_ALL_CH,
                                        _wifi_manager_scan_cb, 0);
    int ret;
    if ((ret = netif_set_opt(netif, NETOPT_SCAN, 0, &request, sizeof(request))) < 0) {
        return ret;
    }
    return 0;
}

int wifi_manager_connect(kernel_pid_t netif_pid,
                         const char *ssid,
                         const wifi_security_mode_t *credentials,
                         unsigned channel)
{
    netif_t *netif = netif_get_by_id(netif_pid);
    if (!netif) {
        return -ENOENT;
    }
    wifi_connect_request_t request
        = WIFI_CONNECT_REQUEST_INITIALIZER(channel,
                                           _wifi_manager_connect_cb,
                                           _wifi_manager_disconnect_cb,
                                           credentials);
    strscpy(request.ssid, ssid, sizeof(request.ssid));
    int ret;
    if ((ret = netif_set_opt(netif, NETOPT_CONNECT, 0, &request, sizeof(request))) < 0) {
        return ret;
    }
    return 0;
}

int wifi_manager_disconnect(kernel_pid_t netif_pid)
{
    netif_t *netif = netif_get_by_id(netif_pid);
    if (!netif) {
        return -ENOENT;
    }
    wifi_disconnect_request_t request
        = WIFI_DISCONNECT_REQUEST_INITIALIZER(_wifi_manager_disconnect_cb);
    int ret;
    if ((ret = netif_set_opt(netif, NETOPT_DISCONNECT, 0, &request, sizeof(request))) < 0) {
        return ret;
    }
    return 0;
}

void wifi_manager_enable(void)
{
    mutex_lock(&_netif_ctx_lock);
    for (unsigned i = 0; i < ARRAY_SIZE(_netif_ctx); i++) {
        if (_netif_ctx[i].netif_pid && !_netif_ctx[i].no_periodic_scan) {
            ztimer_periodic_start(&_netif_ctx[i].timer);
        }
    }
    mutex_unlock(&_netif_ctx_lock);
}

void wifi_manager_disable(void)
{
    mutex_lock(&_netif_ctx_lock);
    for (unsigned i = 0; i < ARRAY_SIZE(_netif_ctx); i++) {
        if (_netif_ctx[i].netif_pid && !_netif_ctx[i].no_periodic_scan) {
            ztimer_periodic_stop(&_netif_ctx[i].timer);
        }
    }
    mutex_unlock(&_netif_ctx_lock);
}

void auto_init_wifi_manager(void)
{
    wifi_manager_start();
}
