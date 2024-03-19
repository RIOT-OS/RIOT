/*
 * Copyright (C) 2022 HAW Hamburg
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
 * @brief       Test application for KW2XRF IEEE 802.15.4 device driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "common.h"
#include "fmt.h"
#include "init_dev.h"
#include "kw2xrf.h"
#include "kw2xrf_tm.h"
#include "shell.h"
#include "test_utils/netdev_ieee802154_minimal.h"
#include "net/netdev/ieee802154_submac.h"

static netdev_ieee802154_submac_t kw2xrf_netdev[KW2XRF_NUM];

/* utility functions */
static void _set_test_mode(int argc, char **argv, uint8_t mode)
{
    (void) argc;
    if (fmt_is_number(argv[1])) {
        int idx = atoi(argv[1]);
        if (idx >= (int)KW2XRF_NUM) {
            puts("invalid interface ID");
            return;
        }

        kw2xrf_t *dev = kw2xrf_netdev[idx].submac.dev.priv;
        kw2xrf_set_test_mode(dev, mode);
        return;
    }
    printf("usage: %s <if_id>\n", argv[0]);
    return;
}

static int _tm_idle(int argc, char **argv)
{
    _set_test_mode(argc, argv, NETOPT_RF_TESTMODE_IDLE);
    return 0;
}

static int _tm_crx(int argc, char **argv)
{
    _set_test_mode(argc, argv, NETOPT_RF_TESTMODE_CRX);
    return 0;
}

static int _tm_ctx_cw(int argc, char **argv)
{
    _set_test_mode(argc, argv, NETOPT_RF_TESTMODE_CTX_CW);
    return 0;
}

static int _tm_ctx_prbs9(int argc, char **argv)
{
    _set_test_mode(argc, argv, NETOPT_RF_TESTMODE_CTX_PRBS9);
    return 0;
}

static int _tm_ctx_preamble(int argc, char **argv)
{
    _set_test_mode(argc, argv, KW2XRF_TM_CTX_PREAMBLE);
    return 0;
}

static int _tm_ctx_2mhz(int argc, char **argv)
{
    _set_test_mode(argc, argv, KW2XRF_TM_CTX_2MHZ);
    return 0;
}

static int _tm_ctx_200khz(int argc, char **argv)
{
    _set_test_mode(argc, argv, KW2XRF_TM_CTX_200KHZ);
    return 0;
}

static int _tm_ctx_1mbps_prbs9(int argc, char **argv)
{
    _set_test_mode(argc, argv, KW2XRF_TM_CTX_1MBPS_PRBS9);
    return 0;
}

static int _tm_ctx_ext(int argc, char **argv)
{
    _set_test_mode(argc, argv, KW2XRF_TM_CTX_EXT);
    return 0;
}

static int _tm_ctx_nm0(int argc, char **argv)
{
    _set_test_mode(argc, argv, KW2XRF_TM_CTX_NM0);
    return 0;
}

static int _tm_ctx_nm1(int argc, char **argv)
{
    _set_test_mode(argc, argv, KW2XRF_TM_CTX_NM1);
    return 0;
}

static ieee802154_dev_t *_reg_callback(ieee802154_dev_type_t type, void *opaque)
{
    if (type != IEEE802154_DEV_TYPE_KW2XRF) {
        assert(false);
    }
    int *c = opaque;
    return &kw2xrf_netdev[(*(c))++].submac.dev;
}

int netdev_ieee802154_minimal_init_devs(netdev_event_cb_t cb) {
    puts("Initializing KW2XRF devices");

    int c = 0;
    /* This function will iterate through all kw2xrf radios */
    ieee802154_hal_test_init_devs(_reg_callback, &c);

    for (unsigned i = 0; i < KW2XRF_NUM; i++) {
        printf("%d out of %d\n", i + 1, KW2XRF_NUM);
        netdev_register(&kw2xrf_netdev[i].dev.netdev, NETDEV_KW2XRF, 0);
        netdev_ieee802154_submac_init(&kw2xrf_netdev[i]);

        /* set the application-provided callback */
        kw2xrf_netdev[i].dev.netdev.event_callback = cb;

        /* initialize the device driver */
        int res = kw2xrf_netdev[i].dev.netdev.driver->init(&kw2xrf_netdev[i].dev.netdev);
        if (res != 0) {
            return -1;
        }
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "idle", "xcvr idle mode", _tm_idle },
    { "ctx_prbs9", "continues transmit the prbs9 pattern", _tm_ctx_prbs9 },
    { "crx", "continues receive mode, useful for current measuring", _tm_crx },
    { "ctx_preamble", "continues transmit the 10101010 pattern", _tm_ctx_preamble },
    { "ctx_cw", "continues transmit carrier wave", _tm_ctx_cw },
    { "ctx_2mhz", "continues transmit modulated carrier", _tm_ctx_2mhz },
    { "ctx_200khz", "continues transmit modulated carrier", _tm_ctx_200khz },
    { "ctx_1mbps_prbs9", "continues transmit the prbs9 pattern", _tm_ctx_1mbps_prbs9 },
    { "ctx_ext", "continues transmit modulated carrier", _tm_ctx_ext },
    { "ctx_nm0", "continues transmit modulated carrier", _tm_ctx_nm0 },
    { "ctx_nm1", "continues transmit modulated carrier", _tm_ctx_nm1 },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Test application for KW2XRF IEEE 802.15.4 device driver");

    int res = netdev_ieee802154_minimal_init();
    if (res) {
        puts("Error initializing devices");
        return 1;
    }

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
