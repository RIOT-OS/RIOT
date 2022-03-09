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
#include <stdbool.h>

#include "fmt.h"
#include "ztimer.h"
#include "nimble_riot.h"
#include "nimble_netif.h"
#include "nimble_netif_conn.h"
#include "net/bluetil/ad.h"
#include "net/bluetil/addr.h"

#define FULL_CONTROL            !IS_USED(MODULE_NIMBLE_AUTOCONN) && \
                                !IS_USED(MODULE_NIMBLE_STATCONN) && \
                                !IS_USED(MODULE_NIMBLE_RPBLE)

#if FULL_CONTROL
#include "nimble_scanlist.h"
#include "nimble_scanner.h"
#endif

#define DEFAULT_SCAN_DURATION_MS    500U
#define DEFAULT_CONN_TIMEOUT_MS     500U
#define DEFAULT_SCAN_ITVL_MS        100U
#define DEFAULT_CONN_ITVL_MS        75U
#define DEFAULT_TX_POWER            0           /* 0dBm */
#define DEFAULT_ADV_ITVL_MS         75U

static const char *_phystr[] = { "N/A", "1M", "2M", "CODED" };

#if FULL_CONTROL
static const char *_name_to_connect = NULL;
static nimble_netif_connect_cfg_t _connect_params = {
    .scan_itvl_ms = DEFAULT_SCAN_ITVL_MS,
    .scan_window_ms = DEFAULT_SCAN_ITVL_MS,
    .conn_itvl_min_ms = DEFAULT_CONN_ITVL_MS,
    .conn_itvl_max_ms = DEFAULT_CONN_ITVL_MS,
    .conn_supervision_timeout_ms = DEFAULT_CONN_ITVL_MS * 20,
    .conn_slave_latency = 0,
    .timeout_ms = 0,       /* will be filled later */
    .phy_mode = 0,      /* will be filled later */
    .own_addr_type = 0  /* will be filled later */,
};

static void _scan_for_name(uint8_t type, const ble_addr_t *addr,
                           const nimble_scanner_info_t *info,
                           const uint8_t *ad, size_t ad_len)
{
    (void)type;
    (void)info;
    int res;
    bluetil_ad_t adstruct;

    bluetil_ad_init(&adstruct, (uint8_t *)ad, ad_len, ad_len);
    res = bluetil_ad_find_and_cmp(&adstruct, BLE_GAP_AD_NAME,
                                  _name_to_connect, strlen(_name_to_connect));
    if (res) {
        nimble_scanner_stop();
        nimble_netif_connect(addr, &_connect_params);
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
        case NIMBLE_NETIF_ACCEPT_STOP:
            _print_evt("ACCEPT STOP", handle, addr);
        case NIMBLE_NETIF_CONN_UPDATED:
        default:
            /* do nothing */
            break;
    }
}

static uint8_t _parsephy(const char *phy_str)
{
    if (memcmp(phy_str, "1M", 2) == 0) {
        return NIMBLE_PHY_1M;
    }
#if IS_ACTIVE(MODULE_NIMBLE_PHY_2MBIT)
    else if (memcmp(phy_str, "2M", 2) == 0) {
        return NIMBLE_PHY_2M;
    }
#endif
#if IS_ACTIVE(MODULE_NIMBLE_PHY_CODED)
    else if (memcmp(phy_str, "CODED", 5) == 0) {
        return NIMBLE_PHY_CODED;
    }
#endif
    else {
        return NIMBLE_PHY_INVALID;
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

#if IS_USED(MODULE_NIMBLE_NETIF_EXT)
    uint8_t phy_rx, phy_tx;
    (void)phy_tx;
    res = ble_gap_read_le_phy(conn->gaphandle, &phy_tx, &phy_rx);
    if (res != 0) {
        phy_rx = 1;
    }
#else
    /* when not using extended advertisements we always use the 1M phy mode */
    uint8_t phy_rx = 1;
#endif
    printf(" (%c,%ums,%ums,%i,%s)\n",
            role, itvl, sto, (int)desc.conn_latency, _phystr[phy_rx]);

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
        puts("     (role, conn itvl, superv. timeout, slave latency, PHY)");
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

    printf("Supported PHY modes: 1M");
#if IS_USED(MODULE_NIMBLE_PHY_2MBIT)
    printf(" 2M");
#endif
#if IS_USED(MODULE_NIMBLE_PHY_CODED)
    printf(" CODED");
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

#if FULL_CONTROL
static int _cmd_adv(int argc, char **argv, bool legacy)
{
    int res;
    (void)res;
    const char *name = NULL;
    uint8_t addrn[BLE_ADDR_LEN];
    ble_addr_t addr = { .type = nimble_riot_own_addr_type };

    /* stop sub-command: stop advertising */
    if (memcmp(argv[2], "stop", 4) == 0) {
        res = nimble_netif_accept_stop();
        if (res == 0) {
            puts("advertising stopped");
        }
        else if (res == -EALREADY) {
            puts("no advertising in progress");
        }
        return 0;
    }

    /* make sure no advertising is in progress */
    if (nimble_netif_conn_is_adv()) {
        puts("err: advertising already in progress");
        return 1;
    }

    /* try if first parameter is a BLE address, if so, use directed
     * advertisement */
    if (bluetil_addr_from_str(addrn, argv[2]) != NULL) {
        /* NimBLE expects address in little endian, so swap */
        bluetil_addr_swapped_cp(addrn, addr.val);
        puts("Found BLE address: sending directed advertisements");
    }
    else {
        name = argv[2];
    }

    uint32_t timeout = 0;
    if (argc >= 4) {
        timeout = (uint32_t)atoi(argv[3]);
    }

    uint8_t phy_sec = BLE_GAP_LE_PHY_1M;
    if (argc >= 5) {
        phy_sec = _parsephy(argv[4]);
        if (phy_sec == 0) {
            puts("err: PHY mode not supported\n");
            return 1;
        }
    }
    uint8_t phy_pri = (phy_sec == BLE_HCI_LE_PHY_2M) ? BLE_HCI_LE_PHY_1M
                                                     : phy_sec;

    nimble_netif_accept_cfg_t p = {
        .flags = (legacy) ? NIMBLE_NETIF_FLAG_LEGACY : 0,
        .adv_itvl_ms = DEFAULT_ADV_ITVL_MS,
        .primary_phy = phy_pri,
        .secondary_phy = phy_sec,
        .tx_power = DEFAULT_TX_POWER,
        .channel_map = 0,
        .timeout_ms = timeout,
        .own_addr_type = nimble_riot_own_addr_type,
    };

    if (name != NULL) {
        uint8_t buf[BLE_HS_ADV_MAX_SZ];
        bluetil_ad_t ad;
        /* build advertising data */
        res = bluetil_ad_init_with_flags(&ad, buf, BLE_HS_ADV_MAX_SZ,
                                         BLUETIL_AD_FLAGS_DEFAULT);
        assert(res == BLUETIL_AD_OK);
        uint16_t ipss = BLE_GATT_SVC_IPSS;
        res = bluetil_ad_add(&ad, BLE_GAP_AD_UUID16_INCOMP, &ipss, sizeof(ipss));
        assert(res == BLUETIL_AD_OK);
        res = bluetil_ad_add(&ad, BLE_GAP_AD_NAME, name, strlen(name));
        if (res != BLUETIL_AD_OK) {
            puts("err: the given name is too long");
            return 1;
        }

        res = nimble_netif_accept(ad.buf, ad.pos, &p);
    }
    else {
        res = nimble_netif_accept_direct(&addr, &p);
    }

    if (res != 0) {
        printf("err: unable to start advertising (%i)\n", res);
    }
    else {
        if (name != NULL) {
            printf("success: advertising this node as '%s'\n", name);
        }
        else {
            printf("success: sending direct advertisements to ");
            bluetil_addr_print(addrn);
            puts("");
        }
    }
    return 0;
}

static void _do_scan(nimble_scanner_cb cb, unsigned duration)
{
    if (duration == 0) {
        printf("err: duration must be > 0\n");
        return;
    }
    if (nimble_scanner_is_active()) {
        printf("err: scanner already active\n");
        return;
    }

    nimble_scanner_cfg_t p = {
        .itvl_ms = DEFAULT_SCAN_ITVL_MS,
        .win_ms = DEFAULT_SCAN_ITVL_MS,
#if IS_USED(MODULE_NIMBLE_PHY_CODED)
        .flags = (NIMBLE_SCANNER_PASSIVE | NIMBLE_SCANNER_PHY_1M |
                  NIMBLE_SCANNER_PHY_CODED),
#else
        .flags = (NIMBLE_SCANNER_PASSIVE | NIMBLE_SCANNER_PHY_1M),
#endif
    };

    nimble_scanner_init(&p, cb);
    nimble_scanlist_clear();
    nimble_scanner_start();
    ztimer_sleep(ZTIMER_MSEC, duration);
    nimble_scanner_stop();
}

static void _cmd_scan(unsigned duration)
{
    printf("scanning (for %ums) ...\n", duration);
    _do_scan(nimble_scanlist_update, duration);
    puts("done");
    nimble_scanlist_print();
}

static void _cmd_connect(int argc, char **argv)
{
    ble_addr_t addr;
    int proceed = 0;

    /* populate connection parameters */
    _connect_params.timeout_ms = DEFAULT_CONN_TIMEOUT_MS;
    if (argc >= 4) {
        _connect_params.timeout_ms = (uint32_t)atoi(argv[3]);
    }
    _connect_params.phy_mode = NIMBLE_PHY_1M;
    if (argc >= 5) {
        _connect_params.phy_mode = _parsephy(argv[4]);
        if (_connect_params.phy_mode == 0) {
            puts("err: PHY mode not supported\n");
            return;
        }
    }
    _connect_params.own_addr_type = nimble_riot_own_addr_type;

    /* try to parse address directly */
    uint8_t addrn[BLE_ADDR_LEN];
    if (bluetil_addr_from_str(addrn, argv[2]) != NULL) {
        addr.type = nimble_riot_own_addr_type;
        /* NimBLE expects address in little endian, so swap */
        bluetil_addr_swapped_cp(addrn, addr.val);
        proceed = 1;
    }
    /* try if param is a number, if so use it as scanlist entry number */
    else if (fmt_is_number(argv[2])) {
        unsigned pos = atoi(argv[2]);
        nimble_scanlist_entry_t *sle = nimble_scanlist_get_by_pos(pos);
        if (sle == NULL) {
            puts("err: unable to find given entry in scanlist");
            return;
        }
        _connect_params.phy_mode = sle->phy_sec;
        memcpy(&addr, &sle->addr, sizeof(addr));
        proceed = 1;
    }
    /* else interpret value as name and search for that peer */
    else {
        unsigned duration = DEFAULT_SCAN_DURATION_MS;
        if (argc > 3) {
            duration = atoi(argv[3]);
        }
        _name_to_connect = argv[2];
        printf("trying to find and connect to a node with name '%s'\n", argv[2]);
        _do_scan(_scan_for_name, duration);
        if (_name_to_connect != NULL) {
            printf("fail: unable to connect to '%s'\n", _name_to_connect);
            _name_to_connect = NULL;
        }
        return;
    }

    if (proceed == 1) {
        int res = nimble_netif_connect(&addr, &_connect_params);
        if (res == 0) {
            puts("Successfully connected 123");
        }
        else {
            puts("err: unable to connect");
        }
    }
}
#endif

static void _cmd_close(int handle)
{
    int res = nimble_netif_close(handle);
    if (res != 0) {
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
    if (res != 0) {
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
#if FULL_CONTROL
    /* setup the scanning environment */
    nimble_scanlist_init();

    /* register event callback with the netif wrapper */
    nimble_netif_eventcb(_on_ble_evt);
#endif
}

int _nimble_netif_handler(int argc, char **argv)
{
    if ((argc == 1) || _ishelp(argv[1])) {
#if FULL_CONTROL
        printf("usage: %s [help|info|adv|adv_ext|adv_dir|"
               "scan|connect|close|update|chanmap]\n", argv[0]);
#else
        printf("usage: %s [help|info|close|update|chanmap]\n", argv[0]);
#endif
        return 0;
    }
    if (memcmp(argv[1], "info", 4) == 0) {
        _cmd_info();
    }

#if FULL_CONTROL
    else if (memcmp(argv[1], "adv_ext", 7) == 0) {
        if (argc <= 2 || _ishelp(argv[2])) {
            printf("usage: %s adv_ext <help|stop|addr|name> [timeout] [phy mode]\n"
                   "        timeout in ms, 0 for no timeout\n"
                   "        phy mode: [1M|2M|CODED]\n", argv[0]);
            return 0;
        }
        return _cmd_adv(argc, argv, false);
    }
    else if (memcmp(argv[1], "adv", 3) == 0) {
        if (argc <= 2 || _ishelp(argv[2])) {
            printf("usage: %s adv <help|stop|addr|name> [timeout]\n"
                   "        timeout in ms, 0 for no timeout\n", argv[0]);
            return 0;
        }
        return _cmd_adv(argc, argv, true);
    }
    else if (memcmp(argv[1], "scan", 4) == 0) {
        uint32_t duration = DEFAULT_SCAN_DURATION_MS;
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
        _cmd_scan(duration);
    }
    else if (memcmp(argv[1], "connect", 7) == 0) {
        if ((argc < 3) || _ishelp(argv[2])) {
            printf("usage: %s %s [help|list|<scanlist #>|<BLE addr>|<name>] "
                   "[timeout ms] [phy mode]\n"
                   "        phy mode: [1M|2M|CODED]\n", argv[0], argv[1]);
            return 0;
        }
        if (memcmp(argv[2], "list", 4) == 0) {
            _conn_list();
            return 0;
        }
        _cmd_connect(argc, argv);
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
    else if ((memcmp(argv[1], "chanmap", 7) == 0)) {
        int handle = nimble_netif_conn_get_next(NIMBLE_NETIF_CONN_INVALID,
                                                NIMBLE_NETIF_GAP_CONNECTED);
        if (handle == NIMBLE_NETIF_CONN_INVALID) {
            printf("err: no BLE connection(s) found\n");
            return 0;
        }
        puts("Used channels per Slot\n"
             "Bitmap denotes channels in ascending order: CH36, ..., CH0\n"
             "e.g. '1f ff ff ff ff' shows that all 37 data channels in use");
        while (handle != NIMBLE_NETIF_CONN_INVALID) {
            uint8_t map[5];
            nimble_netif_used_chanmap(handle, map);
            printf("[%02i] %02x %02x %02x %02x %02x\n", handle,
                    (int)map[4], (int)map[3], (int)map[2],
                    (int)map[1], (int)map[0]);
            handle = nimble_netif_conn_get_next(handle,
                                                NIMBLE_NETIF_GAP_CONNECTED);
        }
        return 0;
    }
    else {
        printf("unable to parse the command. Use '%s help' for more help\n",
               argv[0]);
    }

    return 0;
}
