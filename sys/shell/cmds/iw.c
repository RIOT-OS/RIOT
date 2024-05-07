/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell commands for interacting with Wi-Fi interfaces
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "kernel_defines.h"
#include "mutex.h"
#include "net/wifi_scan_list.h"
#include "net/netdev/wifi.h"
#include "net/netopt.h"
#include "net/wifi.h"
#include "sched.h"
#include "shell.h"
#include "net/netif.h"
#include "net/netdev.h"
#include "fmt.h"
#include "string_utils.h"
#include "ztimer.h"

#define SC_IW_AP_NUMOF                      10
#define SC_IW_AP_SCAN_TIMEOUT_SEC_MAX       30
#define SC_IW_AP_CONNECT_TIMEOUT_SEC_MAX    60

static mutex_t _sync = MUTEX_INIT;

static struct {
    unsigned numof;
    wifi_scan_result_t ap[SC_IW_AP_NUMOF];
} _aps;

static union {
    wifi_security_wep_psk_t wep;
    wifi_security_wpa_psk_t wpa;
    wifi_security_wpa_enterprise_t eap;
} _cred;

static const wifi_scan_result_t *_get_ap(const char *ssid)
{
    for (unsigned i = 0; i < _aps.numof; i++) {
        if (!strcmp(_aps.ap[i].ssid, ssid)) {
            return &_aps.ap[i];
        }
    }
    return NULL;
}

static const char *_ssec(wifi_security_mode_t mode)
{
    switch (mode) {
        case WIFI_SECURITY_MODE_OPEN:
            return "open";
        case WIFI_SECURITY_MODE_WEP_PSK:
            return "WEP";
        case WIFI_SECURITY_MODE_WPA2_PERSONAL:
            return "WPA";
        case WIFI_SECURITY_MODE_WPA2_ENTERPRISE:
            return "WPA2 Enterprise";
        default:
            return "unknown";
    }
}

static void _list_ap(void)
{
    puts(" SSID                            | SEC             | RSSI  | CHANNEL");
    puts("---------------------------------+-----------------+-------+--------");
    for (unsigned i = 0; i < _aps.numof; i++) {
        printf(" %-31s | %-15s | %-5"PRId16" | %-6d \n",
               _aps.ap[i].ssid,
               _ssec(_aps.ap[i].sec_mode),
               _aps.ap[i].base.strength,
               _aps.ap[i].base.channel);
    }
}

static void _wifi_scan_cb(void *netif, const wifi_scan_list_t *result)
{
    (void)netif;
    _aps.numof = wifi_scan_list_to_array(result, _aps.ap, ARRAY_SIZE(_aps.ap));
    _list_ap();
    mutex_unlock(&_sync);
}

static void _wifi_connect_cb(void *netif, const wifi_connect_result_t *result)
{
    (void)netif;
    if (result) {
        printf("connected to %s\n", result->ssid);
    }
    mutex_unlock(&_sync);
}

static void _wifi_disconnect_cb(void *netif, const wifi_disconnect_result_t *result)
{
    (void)netif;
    if (result) {
        printf("could not connect to %s\n", result->ssid);
    }
    mutex_unlock(&_sync);
}

static int _iw_probe(netif_t *iface)
{
    long ret;
    uint16_t val16;
    if ((ret = netif_get_opt(iface, NETOPT_IS_WIRED, 0, &val16, sizeof(&val16))) < 0) {
        if (ret != -ENOTSUP) {  /* -ENOTSUP means wireless */
            return -EIO;
        }
    }
    else {
        return -ENOTSUP;        /* wired */
    }
    if ((ret = netif_get_opt(iface, NETOPT_DEVICE_TYPE, 0, &val16, sizeof(val16))) < 0) {
        return -EIO;
    }
    if (val16 != NETDEV_TYPE_ETHERNET) {
        return -ENOTSUP;
    }
    return 0;
}

static int _iw_disconnect(netif_t *iface)
{
    long ret;
    wifi_disconnect_request_t request = WIFI_DISCONNECT_REQUEST_INITIALIZER(NULL);
    if ((ret = netif_set_opt(iface, NETOPT_DISCONNECT, 0, &request, sizeof(request))) < 0) {
        return ret;
    }
    return 0;
}

static int _iw_cmd_disconnect(netif_t *iface, int argc, char **argv)
{
    (void)argc; (void)argv;
    return _iw_disconnect(iface);
}

static int _iw_connect(netif_t *iface, wifi_connect_request_t *request)
{
    long ret;
    /* this should not block! */
    mutex_lock(&_sync);
    if ((ret = netif_set_opt(iface, NETOPT_CONNECT, 0, request, sizeof(*request))) < 0) {
        mutex_unlock(&_sync);
        return ret;
    }
    /* callback unlocks mutex */
    ztimer_mutex_lock_timeout(ZTIMER_SEC, &_sync, SC_IW_AP_CONNECT_TIMEOUT_SEC_MAX);
    mutex_unlock(&_sync);
    return 0;
}

static int _iw_cmd_connect(netif_t *iface, int argc, char **argv)
{
    (void)iface; (void)argc; (void)argv;
    if (argc < 1) {
        return -EINVAL;
    }
    wifi_connect_request_t request = WIFI_CONNECT_REQUEST_INITIALIZER(0, _wifi_connect_cb,
                                                                      _wifi_disconnect_cb, NULL);
    if (strlen(argv[0]) > sizeof(request.ssid) - 1) {
        printf("SSID too long\n");
        return -EINVAL;
    }
    strcpy(request.ssid, argv[0]);
    argc--;
    argv++;
    const char *user = NULL;
    const char *pwd = NULL;
    const char *psk = NULL;
    bool wep = false;
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] != '-') {
            return -EINVAL;
        }
        if (!strcmp("-u", argv[i])) {
            if (++i >= argc) {
                printf("-u requires an argument\n");
                return -EINVAL;
            }
            user = argv[i];
        }
        else if (!strcmp("-p", argv[i])) {
            if (++i >= argc) {
                printf("-p requires an argument\n");
                return -EINVAL;
            }
            pwd = argv[i];
        }
        else if (!strcmp("-k", argv[i])) {
            if (++i >= argc) {
                printf("-k requires an argument\n");
                return -EINVAL;
            }
            if (!strcmp("WEP", argv[i])) {
                if (++i >= argc) {
                    printf("-k requires an argument\n");
                    return -EINVAL;
                }
                wep = true;
            }
            psk = argv[i];
        }
        else {
            printf("unknown option %s\n", argv[i]);
            return -EINVAL;
        }
    }
    const wifi_scan_result_t *ap = _get_ap(request.ssid);
    if (ap) {
        if (ap->sec_mode == WIFI_SECURITY_MODE_WEP_PSK) {
            if (!psk) {
                printf("%s requires -k WEP\n", request.ssid);
                return -EPERM;
            }
        }
        else if (ap->sec_mode == WIFI_SECURITY_MODE_WPA2_PERSONAL) {
            if (!psk) {
                printf("%s requires -k\n", request.ssid);
                return -EPERM;
            }
        }
        else if (ap->sec_mode == WIFI_SECURITY_MODE_WPA2_ENTERPRISE) {
            if (!user || !pwd) {
                printf("%s requires -u and -p\n", request.ssid);
                return -EPERM;
            }
        }
        request.base.channel = ap->base.channel;
    }
    if (psk) {
        if (strlen(psk) > sizeof(_cred.wpa.psk) - 1) {
            printf("Key too long\n");
            return -EINVAL;
        }
        /* also copies to WEP key */
        strcpy(_cred.wpa.psk, psk);
        if (wep) {
            _cred.wep.sec = WIFI_SECURITY_MODE_WEP_PSK;
            request.cred = &_cred.wep.sec;
        }
        else {
            _cred.wpa.sec = WIFI_SECURITY_MODE_WPA2_PERSONAL;
            request.cred = &_cred.wpa.sec;
        }
    }
    if (user) {
        if (strlen(user) > sizeof(_cred.eap.user) - 1) {
            printf("username too long\n");
            return -EINVAL;
        }
        strcpy(_cred.eap.user, user);
        _cred.eap.sec = WIFI_SECURITY_MODE_WPA2_ENTERPRISE;
        request.cred = &_cred.eap.sec;
        explicit_bzero(_cred.eap.pwd, sizeof(_cred.eap.pwd));
    }
    if (pwd) {
        if (strlen(pwd) > sizeof(_cred.eap.pwd) - 1) {
            printf("password too long\n");
            return -EINVAL;
        }
        strcpy(_cred.eap.pwd, pwd);
    }
    return _iw_connect(iface, &request);
}

static int _iw_scan(netif_t *iface, wifi_scan_request_t *request)
{
    int ret;
    /* this should not block! */
    mutex_lock(&_sync);
    if ((ret = netif_set_opt(iface, NETOPT_SCAN, 0, request, sizeof(*request))) < 0) {
        mutex_unlock(&_sync);
        return ret;
    }
    /* callback unlocks mutex */
    ztimer_mutex_lock_timeout(ZTIMER_SEC, &_sync, SC_IW_AP_SCAN_TIMEOUT_SEC_MAX);
    mutex_unlock(&_sync);
    return 0;
}

static int _iw_cmd_scan(netif_t *iface, int argc, char **argv)
{
    long ret;
    netopt_channel_t ch = NETOPT_SCAN_REQ_ALL_CH;
    uint32_t timeout_ms = 0;
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] != '-') {
            return -EINVAL;
        }
        if (!strcmp("-c", argv[i])) {
            if (++i >= argc) {
                printf("-c requires an argument\n");
                return -EINVAL;
            }
            if (!fmt_is_number(argv[i]) ||
                (ret = strtol(argv[i], NULL, 10)) < 0) {
                printf("-c argument %s is not a number\n", argv[i]);
                return -EINVAL;
            }
            ch = (netopt_channel_t)ret;
        }
        else if (!strcmp("-t", argv[i])) {
            if (++i >= argc) {
                printf("-t requires an argument\n");
                return -EINVAL;
            }
            if (!fmt_is_number(argv[i]) ||
                (ret = strtol(argv[i], NULL, 10)) < 0) {
                printf("-t argument %s is not a number\n", argv[i]);
                return -EINVAL;
            }
            timeout_ms = (uint32_t)ret;
        }
        else {
            printf("unknown option %s\n", argv[i]);
            return -EINVAL;
        }
    }
    wifi_scan_request_t request = WIFI_SCAN_REQUEST_INITIALIZER(ch, _wifi_scan_cb, timeout_ms);
    return _iw_scan(iface, &request);
}

static void _iw_usage(const char *cmd)
{
    printf("usage: %s <if_id> <command>\n", cmd);
    printf("commands:\n"
           "    scan [-c <channel>] [-t <channel timeout ms>]\n"
           "    connect <SSID> [-u <username> -p <password>] | -k [WEP] <psk>]\n"
           "    disconnect\n"
          );
}

static void _iw_error(const char *cmd, int error)
{
    printf("%s: error (%d) %s\n", cmd, error, strerror(error));
}

int _iw_cmd(int argc, char **argv)
{
    if (argc < 3) {
        goto exit_help;
    }
    int ret = -EINVAL;
    netif_t *iface = netif_get_by_name(argv[1]);
    if (!iface) {
        printf("%s: invalid interface given\n", argv[0]);
        goto exit_failure;
    }
    if ((ret = _iw_probe(iface)) != 0) {
        if (ret == -ENOTSUP) {
            printf("%s: interface is not Wi-Fi\n", argv[0]);
            goto exit_failure;
        }
        else {
            printf("%s: interface communication error\n", argv[0]);
            goto exit_failure;
        }
    }

    if (!strcmp(argv[2], "scan")) {
        if ((ret = _iw_cmd_scan(iface, argc - 3, argv + 3)) != 0) {
            goto exit_failure;
        }
    }
    else if (!strcmp(argv[2], "connect")) {
        if ((ret = _iw_cmd_connect(iface, argc - 3, argv + 3)) != 0) {
            goto exit_failure;
        }
    }
    else if (!strcmp(argv[2], "disconnect")) {
        if ((ret = _iw_cmd_disconnect(iface, argc - 3, argv + 3)) != 0) {
            goto exit_failure;
        }
    }
    else {
        goto exit_help;
    }

    printf("%s: ok\n", argv[0]);
    return EXIT_SUCCESS;

exit_help:
    _iw_usage(argv[0]);
    return EXIT_FAILURE;

exit_failure:
    _iw_error(argv[0], ret);
    return EXIT_FAILURE;

}

SHELL_COMMAND(iw, "Control Wi-Fi interfaces", _iw_cmd);
