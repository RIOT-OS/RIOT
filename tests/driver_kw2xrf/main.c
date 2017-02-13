/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     tests
 * @file
 * @brief       Test application for KW2x network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "kw2xrf.h"
#include "shell_commands.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netapi.h"
#include "net/netopt.h"

#define KW2XRF_TESTMODE     (1)

#ifdef KW2XRF_TESTMODE
#include "kw2xrf_tm.h"

/* utility functions */
static bool _is_number(char *str)
{
    for (; *str; str++) {
        if (*str < '0' || *str > '9') {
            return false;
        }
    }

    return true;
}

static bool _is_iface(kernel_pid_t dev)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    size_t numof = gnrc_netif_get(ifs);

    for (size_t i = 0; i < numof && i < GNRC_NETIF_NUMOF; i++) {
        if (ifs[i] == dev) {
            return true;
        }
    }

    return false;
}

static void _set_test_mode(int argc, char **argv, uint8_t mode)
{
    (void) argc;
    if (_is_number(argv[1])) {
        kernel_pid_t dev = (kernel_pid_t)atoi(argv[1]);

        if (_is_iface(dev)) {
            gnrc_netapi_set(dev, NETOPT_RF_TESTMODE, 0, (void *)&mode, sizeof(mode));
            return;
        }
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

#endif

static const shell_command_t shell_commands[] = {
#ifdef KW2XRF_TESTMODE
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
#endif
    { NULL, NULL, NULL }
};

int main(void)
{

    puts("KW2XRF device driver test");

    /* start the shell */
    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
