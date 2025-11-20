/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "net/ieee802154.h"
#include "net/l2util.h"
#include "shell.h"
#include "ztimer.h"

#include "control.h"

#define IEEE802154_LONG_ADDRESS_LEN_STR_MAX \
    (sizeof("00:00:00:00:00:00:00:00"))
#define IEEE802154_SHORT_ADDRESS_LEN_STR_MAX \
    (sizeof("00:00"))

/* See 7.2.31.1 Units of TX Power Control */
#define DW1000_TX_POWER_COARSE_SHIFT        (5)
#define DW1000_TX_POWER_COARSE_MASK         (0xE0)
#define DW1000_TX_POWER_FINE_MASK           (0x1F)
#define DW1000_TX_POWER_MULTI               (10)
#define DW1000_TX_POWER_COARSE_STEP         (30)        /* 3dbM * 10 */
#define DW1000_TX_POWER_FINE_STEP           (5)         /* 0.5dbM * 10 */

static int _twr_ifconfig(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    char addr_str[IEEE802154_LONG_ADDRESS_LEN_STR_MAX];
    struct uwb_dev *udev = uwb_dev_idx_lookup(0);
    uint8_t buffer[IEEE802154_LONG_ADDRESS_LEN];

    printf("Iface  %d", udev->task_str.t.pid);
    byteorder_htobebufs(buffer, udev->uid);
    printf("\tHWaddr: %s  ",
           l2util_addr_to_str(buffer, IEEE802154_SHORT_ADDRESS_LEN, addr_str));
    byteorder_htobebufs(buffer, udev->pan_id);
    printf("Channel: %d  ", udev->config.channel);
    printf("NID: %s\n\n",
           l2util_addr_to_str(buffer, IEEE802154_SHORT_ADDRESS_LEN, addr_str));
    byteorder_htobebufll(buffer, udev->euid);
    printf("\t\tLong HWaddr: %s\n",
           l2util_addr_to_str(buffer, IEEE802154_LONG_ADDRESS_LEN, addr_str));
    /* 000 -> 18dBM gain, 110 -> 0dBm gain */
    int tx_power =
        DW1000_TX_POWER_COARSE_STEP  *
        (6 -
         ((udev->config.txrf.BOOSTNORM & DW1000_TX_POWER_COARSE_MASK) >>
          DW1000_TX_POWER_COARSE_SHIFT)) +
        (udev->config.txrf.BOOSTNORM & DW1000_TX_POWER_FINE_MASK) *
        DW1000_TX_POWER_FINE_STEP;

    printf("\t\tTX-Power: %d.%ddBm  ", tx_power / DW1000_TX_POWER_MULTI,
           tx_power > (tx_power / DW1000_TX_POWER_MULTI) * DW1000_TX_POWER_MULTI ? 5 : 0);
    printf("TC-PGdelay: 0x%02x\n", udev->config.txrf.PGdly);

    return 0;
}

SHELL_COMMAND(ifconfig, "Network interface information", _twr_ifconfig);

static void _print_usage(void)
{
    puts("Usage:");
    puts("\ttwr req <short_addr> [-p <proto>] [-c <count>] [-h]"
         "[-i <ms interval>] ");
    puts("\t - short_addr: short address of request destination");
    puts("\t - count: number of requests (default: 1)");
    puts("\t - ms interval: wait interval milliseconds between requests"
         "(default: 1000)");
    puts("\t - proto: twr protocol (ss|ss-ext|ss-ack|ds|ds-ext, default: ss)");
    puts("\ttwr lst on: start listening for ranging requests");
    puts("\ttwr lst off: stop listening for ranging requests");
}

static int _twr_handler(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage();
        return -1;
    }

    if (!strcmp(argv[1], "lst")) {
        if (argc == 3) {
            if (!strcmp(argv[2], "on")) {
                puts("[twr]: start listening");
                uwb_core_rng_listen_enable();
                return 0;
            }
            else if (!strcmp(argv[2], "off")) {
                puts("[twr]: stop listening");
                uwb_core_rng_listen_disable();
                return 0;
            }
        }
        _print_usage();
        return -1;
    }

    if (!strcmp(argv[1], "req")) {
        uint32_t count = 1;
        uint32_t interval_ms = 1000;
        int proto = UWB_DATA_CODE_SS_TWR;
        uint8_t addr[IEEE802154_SHORT_ADDRESS_LEN];
        uint16_t short_addr = 0x0000;
        int res = 0;
        if (argc < 3) {
            _print_usage();
            return -1;
        }
        /* parse command line arguments */
        for (int i = 2; i < argc; i++) {
            char *arg = argv[i];
            if (arg[0] != '-') {
                size_t addr_len = l2util_addr_from_str(arg, addr);
                if (addr_len != 2) {
                    puts("[Error]: unable to parse address.\n"
                         "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*\n"
                         "(hex pairs delimited by colons)");
                    res = 1;
                }
                short_addr = addr[1] + (addr[0] << 8);
                if (short_addr == 0x0000) {
                    printf("[ERROR]: invalid addr %s\n", arg);
                    res = 1;
                }
            }
            else {
                switch (arg[1]) {
                case 'c':
                    if (((i++) + 1) < argc) {
                        count = atoi(argv[i]);
                        if (count > 0) {
                            continue;
                        }
                        res = 1;
                    }
                /* intentionally falls through */
                case 'h':
                    res = 1;
                    continue;
                /* intentionally falls through */
                case 'p':
                    if ((++i) < argc) {
                        if (!strcmp(argv[i], "ss")) {
                            proto = UWB_DATA_CODE_SS_TWR;
                            continue;
                        }
                        else if (!strcmp(argv[i], "ss-ack")) {
                            proto = UWB_DATA_CODE_SS_TWR_ACK;
                            continue;
                        }
                        else if (!strcmp(argv[i], "ss-ext")) {
                            proto = UWB_DATA_CODE_SS_TWR_EXT;
                            continue;
                        }
                        else if (!strcmp(argv[i], "ds")) {
                            proto = UWB_DATA_CODE_DS_TWR;
                            continue;
                        }
                        else if (!strcmp(argv[i], "ds-ext")) {
                            proto = UWB_DATA_CODE_DS_TWR_EXT;
                            continue;
                        }
                        else {
                            printf("[ERROR]: invalid protocol %s\n", argv[i]);
                            res = 1;
                        }
                    }
                /* intentionally falls through */
                case 'i':
                    if ((++i) < argc) {
                        interval_ms = atoi(argv[i]);
                        continue;
                    }
                /* intentionally falls through */
                default:
                    res = 1;
                    break;
                }
            }
        }
        if (res != 0 || (short_addr == 0x0000)) {
            _print_usage();
            return 1;
        }
        puts("[twr]: start ranging");
        uwb_core_rng_start(short_addr, proto, interval_ms, count);
        if (IS_ACTIVE(CONFIG_TWR_SHELL_BLOCKING)) {
            while (uwb_core_rng_req_remaining()) {
                ztimer_sleep(ZTIMER_MSEC, interval_ms);
            }
            /* some time to finish up */
            ztimer_sleep(ZTIMER_MSEC, 100 + interval_ms);
        }
        return 0;
    }

    _print_usage();
    return -1;
}

SHELL_COMMAND(twr, "Two-way-ranging (TWR) cli", _twr_handler);
