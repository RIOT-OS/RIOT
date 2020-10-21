/*
 * Copyright (C) 2019 Freie Universität Berlin
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
 * @brief       Shell commands to control NimBLEs netif wrapper
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "fmt.h"
#include "xtimer.h"
#include "nimble_riot.h"
#include "nimble_netif.h"
#include "nimble_netif_conn.h"
#include "net/bluetil/ad.h"
#include "net/bluetil/addr.h"

#if !IS_USED(MODULE_NIMBLE_AUTOCONN) && !IS_USED(MODULE_NIMBLE_STATCONN)
#include "nimble_scanlist.h"
#include "nimble_scanner.h"
#endif

#define DEFAULT_NODE_NAME           "bleRIOT"
#define DEFAULT_SCAN_DURATION       (500U)      /* 500ms */
#define DEFAULT_CONN_TIMEOUT        (500U)      /* 500ms */

#if !IS_USED(MODULE_NIMBLE_AUTOCONN) && !IS_USED(MODULE_NIMBLE_STATCONN)
static const char *_name_to_connect = NULL;

static void _scan_for_name(uint8_t type, const ble_addr_t *addr, int8_t rssi,
                           const uint8_t *ad, size_t ad_len)
{
    (void)type;
    (void)rssi;
    int res;
    bluetil_ad_t adstruct;

    bluetil_ad_init(&adstruct, (uint8_t *)ad, ad_len, ad_len);
    res = bluetil_ad_find_and_cmp(&adstruct, BLE_GAP_AD_NAME,
                                  _name_to_connect, strlen(_name_to_connect));
    if (res) {
        nimble_scanner_stop();
        nimble_netif_connect(addr, NULL, DEFAULT_CONN_TIMEOUT);
    }
}

static void _print_evt(const char *msg, int handle, const uint8_t *addr)
{
    printf("event: handle %i -> %s (", handle, msg);
    bluetil_addr_print(addr);
    puts(")");
}

static void _on_ble_evt(int handle, nimble_netif_event_t event,
                        const uint8_t *addr)
{
    switch (event) {
        case NIMBLE_NETIF_CONNECTED_MASTER: {
            _print_evt("CONNECTED as MASTER", handle, addr);
            if (_name_to_connect != NULL) {
                printf("connection to '%s' established\n", _name_to_connect);
                _name_to_connect = NULL;
            }
            break;
        }
        case NIMBLE_NETIF_CONNECTED_SLAVE:
            _print_evt("CONNECTED as SLAVE", handle, addr);
            break;
        case NIMBLE_NETIF_CLOSED_MASTER:
        case NIMBLE_NETIF_CLOSED_SLAVE:
            _print_evt("CONNECTION CLOSED", handle, addr);
            break;
        case NIMBLE_NETIF_ABORT_MASTER:
        case NIMBLE_NETIF_ABORT_SLAVE:
            _print_evt("CONNECTION ABORT", handle, addr);
            break;
        case NIMBLE_NETIF_CONN_UPDATED:
        default:
            /* do nothing */
            break;
    }
}
#endif

static int _conn_dump(nimble_netif_conn_t *conn, int handle, void *arg)
{
    (void)arg;
    struct ble_gap_conn_desc desc;

    int res = ble_gap_conn_find(conn->gaphandle, &desc);
    assert(res == 0);   /* the handle should always be valid here */
    (void)res;

    char role = (conn->state & NIMBLE_NETIF_GAP_MASTER) ? 'M' : 'S';
    unsigned itvl = ((unsigned)desc.conn_itvl * BLE_HCI_CONN_ITVL) / 1000;
    unsigned sto = (unsigned)desc.supervision_timeout * 10;

    printf("[%2i] ", handle);
    bluetil_addr_print(conn->addr);
#ifdef MODULE_GNRC_IPV6
    printf(" ");
    bluetil_addr_ipv6_l2ll_print(conn->addr);
#endif
    printf(" (%c,%ums,%ums,%i)", role, itvl, sto, (int)desc.conn_latency);
    puts("");

    return 0;
}

static int _conn_state_dump(nimble_netif_conn_t *conn, int handle, void *arg)
{
    (void)arg;
    printf("[%2i] state: 0x%04x -", handle, conn->state);
    if (conn->state & NIMBLE_NETIF_UNUSED) {
        printf(" unused");
    }
    if (conn->state & NIMBLE_NETIF_CONNECTING) {
        printf(" connecting");
    }
    if (conn->state & NIMBLE_NETIF_ADV) {
        printf(" advertising");
    }
    if (conn->state & NIMBLE_NETIF_GAP_SLAVE) {
        printf(" GAP-slave");
    }
    if (conn->state & NIMBLE_NETIF_GAP_MASTER) {
        printf(" GAP-master");
    }
    if (conn->state & NIMBLE_NETIF_L2CAP_SERVER) {
        printf(" L2CAP-server");
    }
    if (conn->state & NIMBLE_NETIF_L2CAP_CLIENT) {
        printf(" L2CAP-client");
    }
    puts("");
    return 0;
}

static void _conn_list(void)
{
    unsigned active = nimble_netif_conn_count(NIMBLE_NETIF_L2CAP_CONNECTED);
    printf("Connections: %u\n", active);
    if (active > 0) {
        nimble_netif_conn_foreach(NIMBLE_NETIF_L2CAP_CONNECTED,
                                  _conn_dump, NULL);
        puts("     (role, conn itvl, superv. timeout, slave latency)");
    }
}

static void _cmd_info(void)
{
    unsigned free = nimble_netif_conn_count(NIMBLE_NETIF_UNUSED);

    uint8_t own_addr[BLE_ADDR_LEN];
    uint8_t tmp_addr[BLE_ADDR_LEN];
    ble_hs_id_copy_addr(nimble_riot_own_addr_type, tmp_addr, NULL);
    bluetil_addr_swapped_cp(tmp_addr, own_addr);
    printf("Own Address: ");
    bluetil_addr_print(own_addr);
#ifdef MODULE_GNRC_IPV6
    printf(" -> ");
    bluetil_addr_ipv6_l2ll_print(own_addr);
#endif
    puts("");

    printf(" Free slots: %u/%u\n", free, NIMBLE_NETIF_MAX_CONN);
    printf("Advertising: ");
    if (nimble_netif_conn_get_adv() != NIMBLE_NETIF_CONN_INVALID) {
        puts("yes");
    }
    else {
        puts("no");
    }

    _conn_list();

    puts("Slots:");
    nimble_netif_conn_foreach(NIMBLE_NETIF_ANY, _conn_state_dump, NULL);

    puts("");
}

#if !IS_USED(MODULE_NIMBLE_AUTOCONN) && !IS_USED(MODULE_NIMBLE_STATCONN)
static void _cmd_adv(const char *name)
{
    int res;
    (void)res;
    uint8_t buf[BLE_HS_ADV_MAX_SZ];
    bluetil_ad_t ad;
    const struct ble_gap_adv_params _adv_params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,
        .disc_mode = BLE_GAP_DISC_MODE_LTD,
        .itvl_min = BLE_GAP_ADV_FAST_INTERVAL2_MIN,
        .itvl_max = BLE_GAP_ADV_FAST_INTERVAL2_MAX,
    };

    /* make sure no advertising is in progress */
    if (nimble_netif_conn_is_adv()) {
        puts("err: advertising already in progress");
        return;
    }

    /* build advertising data */
    res = bluetil_ad_init_with_flags(&ad, buf, BLE_HS_ADV_MAX_SZ,
                                     BLUETIL_AD_FLAGS_DEFAULT);
    assert(res == BLUETIL_AD_OK);
    uint16_t ipss = BLE_GATT_SVC_IPSS;
    res = bluetil_ad_add(&ad, BLE_GAP_AD_UUID16_INCOMP, &ipss, sizeof(ipss));
    assert(res == BLUETIL_AD_OK);
    if (name == NULL) {
        name = DEFAULT_NODE_NAME;
    }
    res = bluetil_ad_add(&ad, BLE_GAP_AD_NAME, name, strlen(name));
    if (res != BLUETIL_AD_OK) {
        puts("err: the given name is too long");
        return;
    }

    /* start listening for incoming connections */
    res = nimble_netif_accept(ad.buf, ad.pos, &_adv_params);
    if (res != NIMBLE_NETIF_OK) {
        printf("err: unable to start advertising (%i)\n", res);
    }
    else {
        printf("success: advertising this node as '%s'\n", name);
    }
}

static void _cmd_adv_stop(void)
{
    int res = nimble_netif_accept_stop();
    if (res == NIMBLE_NETIF_OK) {
        puts("canceled advertising");
    }
    else if (res == NIMBLE_NETIF_NOTADV) {
        puts("no advertising in progress");
    }
}

static void _do_scan(nimble_scanner_cb cb, unsigned duration)
{
    if (duration == 0) {
        printf("err: duration must be > 0\n");
        return;
    }
    if (nimble_scanner_status() == NIMBLE_SCANNER_SCANNING) {
        printf("err: scanner already active\n");
        return;
    }
    nimble_scanner_init(NULL, cb);
    nimble_scanlist_clear();
    nimble_scanner_start();
    xtimer_usleep(duration);
    nimble_scanner_stop();
}

static void _cmd_scan(unsigned duration)
{
    printf("scanning (for %ums) ...\n", (duration / 1000));
    _do_scan(nimble_scanlist_update, duration);
    puts("done");
    nimble_scanlist_print();
}

static void _cmd_connect_addr(ble_addr_t *addr)
{
    /* simply use NimBLEs default connection parameters */
    int res = nimble_netif_connect(addr, NULL, DEFAULT_CONN_TIMEOUT);
    if (res < 0) {
        printf("err: unable to trigger connection sequence (%i)\n", res);
        return;
    }

    printf("initiated connection procedure with ");
    uint8_t addrn[BLE_ADDR_LEN];
    bluetil_addr_swapped_cp(addr->val, addrn);
    bluetil_addr_print(addrn);
    puts("");

}

static void _cmd_connect_addr_raw(const uint8_t *addr_in)
{
    /* RANDOM is the most common type, has no noticeable effect when connecting
       anyhow... */
    ble_addr_t addr = { .type = BLE_ADDR_RANDOM };
    /* NimBLE expects address in little endian, so swap */
    bluetil_addr_swapped_cp(addr_in, addr.val);
    _cmd_connect_addr(&addr);
}

static void _cmd_connect_name(const char *name, unsigned duration)
{
    if (_name_to_connect != NULL) {
        printf("err: already trying to connect to '%s'\n", _name_to_connect);
        return;
    }
    _name_to_connect = name;
    printf("trying to find and connect to a node with name '%s'\n", name);
    _do_scan(_scan_for_name, duration);
    if (_name_to_connect != NULL) {
        printf("fail: unable to connect to '%s'\n", _name_to_connect);
        _name_to_connect = NULL;
    }
}

static void _cmd_connect_scanlist(unsigned pos)
{
    nimble_scanlist_entry_t *sle = nimble_scanlist_get_by_pos(pos);
    if (sle == NULL) {
        puts("err: unable to find given entry in scanlist");
        return;
    }
    _cmd_connect_addr(&sle->addr);
}
#endif /* MODULE_NIMBLE_AUTOCONN */

static void _cmd_close(int handle)
{
    int res = nimble_netif_close(handle);
    if (res != NIMBLE_NETIF_OK) {
        puts("err: unable to close connection with given handle");
    }
    else {
        puts("success: connection tear down initiated");
    }
}

static void _cmd_update(int handle, int itvl, int timeout)
{
    struct ble_gap_upd_params params;
    params.itvl_min = (uint16_t)BLE_GAP_CONN_ITVL_MS(itvl);
    params.itvl_max = (uint16_t)BLE_GAP_CONN_ITVL_MS(itvl);
    params.latency = 0;
    params.supervision_timeout = (uint16_t)(timeout / 10);
    params.min_ce_len = BLE_GAP_INITIAL_CONN_MIN_CE_LEN;
    params.max_ce_len = BLE_GAP_INITIAL_CONN_MAX_CE_LEN;

    int res = nimble_netif_update(handle, &params);
    if (res != NIMBLE_NETIF_OK) {
        puts("err: unable to update connection parameters for given handle");
    }
    else {
        puts("success: connection parameters updated");
    }
}

static int _ishelp(char *argv)
{
    return memcmp(argv, "help", 4) == 0;
}

void sc_nimble_netif_init(void)
{
#if !IS_USED(MODULE_NIMBLE_AUTOCONN) && !IS_USED(MODULE_NIMBLE_STATCONN)
    /* setup the scanning environment */
    nimble_scanlist_init();

    /* register event callback with the netif wrapper */
    nimble_netif_eventcb(_on_ble_evt);
#endif
}

int _nimble_netif_handler(int argc, char **argv)
{
    if ((argc == 1) || _ishelp(argv[1])) {
#if !IS_USED(MODULE_NIMBLE_AUTOCONN) && !IS_USED(MODULE_NIMBLE_STATCONN)
        printf("usage: %s [help|info|adv|scan|connect|close|update]\n", argv[0]);
#else
        printf("usage: %s [help|info|close|update]\n", argv[0]);
#endif
        return 0;
    }
    if (memcmp(argv[1], "info", 4) == 0) {
        _cmd_info();
    }

#if !IS_USED(MODULE_NIMBLE_AUTOCONN) && !IS_USED(MODULE_NIMBLE_STATCONN)
    else if (memcmp(argv[1], "adv", 3) == 0) {
        char *name = NULL;
        if (argc > 2) {
            if (_ishelp(argv[2])) {
                printf("usage: %s adv [help|stop|<name>]\n", argv[0]);
                return 0;
            }
            if (memcmp(argv[2], "stop", 4) == 0) {
                _cmd_adv_stop();
                return 0;
            }
            name = argv[2];
        }
        _cmd_adv(name);
    }
    else if (memcmp(argv[1], "scan", 4) == 0) {
        uint32_t duration = DEFAULT_SCAN_DURATION;
        if (argc > 2) {
            if (_ishelp(argv[2])) {
                printf("usage: %s scan [help|list|[duration in ms]]\n", argv[0]);
                return 0;
            }
            if (memcmp(argv[2], "list", 4) == 0) {
                nimble_scanlist_print();
                return 0;
            }
            duration = atoi(argv[2]);
        }
        _cmd_scan(duration * 1000);
    }
    else if (memcmp(argv[1], "connect", 7) == 0) {
        if ((argc < 3) || _ishelp(argv[2])) {
            printf("usage: %s connect [help|list|<scanlist entry #>|<BLE addr>|<name>]\n",
                   argv[0]);
            return 0;
        }
        if (memcmp(argv[2], "list", 4) == 0) {
            _conn_list();
            return 0;
        }
        /* try if param is an BLE address */
        uint8_t addr[BLE_ADDR_LEN];
        if (bluetil_addr_from_str(addr, argv[2]) != NULL) {
            _cmd_connect_addr_raw(addr);
            return 0;
        }
        /* try if param is a name (contains non-number chars) */
        if (!fmt_is_number(argv[2])) {
            unsigned duration = DEFAULT_SCAN_DURATION;
            if (argc > 3) {
                duration = (unsigned)atoi(argv[3]);
            }
            _cmd_connect_name(argv[2], duration * 1000);
            return 0;
        }

        unsigned pos = atoi(argv[2]);
        _cmd_connect_scanlist(pos);
    }
#endif
    else if (memcmp(argv[1], "close", 5) == 0) {
        if ((argc < 3) || _ishelp(argv[2])) {
            printf("usage: %s close [help|list|<conn #>]\n", argv[0]);
            return 0;
        }
        if (memcmp(argv[2], "list", 4) == 0) {
            _conn_list();
            return 0;
        }
        int handle = atoi(argv[2]);
        _cmd_close(handle);
    }
    else if ((memcmp(argv[1], "update", 6) == 0)) {
        if ((argc < 5) || _ishelp(argv[2])) {
            printf("usage: %s update [help|<handle> <itvl> <timeout>]\n",
                   argv[0]);
            return 0;
        }
        int handle = atoi(argv[2]);
        int itvl = atoi(argv[3]);
        int timeout = atoi(argv[4]);
        _cmd_update(handle, itvl, timeout);
    }
    else {
        printf("unable to parse the command. Use '%s help' for more help\n",
               argv[0]);
    }

    return 0;
}
