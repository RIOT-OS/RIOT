/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Wi-Fi manager test application
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "compiler_hints.h"
#include "net/gnrc/netif.h"
#include "net/netdev.h"
#include "net/wifi.h"
#include "shell.h"
#include "net/wifi_manager.h"

/**
 * @brief   An application can provide connection hints respected for automatic
 *          connection per Wi-Fi device
 */
typedef struct {
    netdev_type_t type;
    uint8_t index;
    wifi_manager_connection_hint_t hint;
} app_connection_hint_t;

MAYBE_UNUSED
static const wifi_security_wpa_psk_t _hints_wpa[] = {
    {
        .sec = WIFI_SECURITY_MODE_WPA2_PERSONAL,
        .psk = "password",
    }
};

static const app_connection_hint_t _hints[] = {
    {
        .type = NETDEV_ATWINC15X0,
        .index = 0,
        .hint = {
            .ssid = "AndroidAP",
#if 0
        /* PSK hint takes precedence over saved configuration, so only
           specify a password hint if you are really sure about it */
            .sec = &_hints_wpa[0].sec,
#endif
        }
    }
};

static inline const wifi_manager_connection_hint_t *_check_hints(netdev_type_t type,
                                                                 uint8_t index)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_hints); i++) {
        if (_hints[i].type == type && _hints[i].index == index) {
            return &_hints[i].hint;
        }
    }
    return NULL;
}

static inline unsigned _wifi_manager_netif_add_all_by_type(netdev_type_t type)
{
    const gnrc_netif_t *netif;
    uint8_t i;
    for (i = 0; (netif = gnrc_netif_get_by_type(type, i)); i++) {
        const wifi_manager_connection_hint_t *netif_hints = _check_hints(type, i);
        if (!wifi_manager_netif_add_by_pid(netif->pid, netif_hints, true)) {
            break;
        }
    }
    return i;
}

static void _on_scan(void *netif, const wifi_scan_list_t *scan)
{
    (void)netif; (void)scan;
    printf("Wi-Fi manager test %p _on_scan\n", netif);
}

static void _on_connect(void *netif, const wifi_connect_result_t *connect)
{
    (void)netif; (void)connect;
    printf("Wi-Fi manager test %p _on_connect\n", netif);
}

static void _on_disconnect(void *netif, const wifi_disconnect_result_t *disconnect)
{
    (void)netif; (void)disconnect;
    printf("Wi-Fi manager test %p _on_disconnect\n", netif);
}

static wifi_manager_listener_t _listener = {
    .on_scan = _on_scan,
    .on_connect = _on_connect,
    .on_disconnect = _on_disconnect,
};

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static const shell_command_t shell_commands[] = {
    { NULL, NULL, NULL }
};

int main(void)
{
    _wifi_manager_netif_add_all_by_type(NETDEV_ATWINC15X0);
    wifi_manager_register_listener(&_listener);
    wifi_manager_start();

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
