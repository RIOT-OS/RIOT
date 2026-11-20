/*
 * SPDX-FileCopyrightText: 2026 Jeewoong Kim
 * SPDX-License-Identifier: LGPL-2.1-only
 */

 /**
  * @ingroup     tests
  * @{
  *
  * @file
  * @brief       Regression test application for ETHOS RX frame handling
  *
  * @}
  */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ethos.h"
#include "net/netdev.h"
#include "periph/uart.h"
#include "shell.h"
#include "test_utils/netdev_eth_minimal.h"

#include "init_dev.h"

#define TEST_INBUF_SIZE (64U)

static ethos_t _ethos;
static uint8_t _inbuf[TEST_INBUF_SIZE];

static netdev_event_cb_t _base_event_cb;
static volatile bool _hold_rx;
static volatile unsigned _held_rx;

static const ethos_params_t _params = {
    .uart = UART_DEV(0),
    .baudrate = 115200,
};

static void _test_event_cb(netdev_t *dev, netdev_event_t event)
{
    if ((event == NETDEV_EVENT_RX_COMPLETE) && _hold_rx) {
        _held_rx++;
        printf("ETHOS_TEST RX_HELD %u\n", _held_rx);
        return;
    }

    _base_event_cb(dev, event);
}

int netdev_eth_minimal_init_devs(netdev_event_cb_t cb)
{
    _base_event_cb = cb;

    ethos_setup(&_ethos, &_params, 0, _inbuf, sizeof(_inbuf));
    _ethos.netdev.event_callback = _test_event_cb;

    return _ethos.netdev.driver->init(&_ethos.netdev);
}

static int _cmd_hold(int argc, char **argv)
{
    if (argc != 2) {
        puts("usage: ethos_hold 0|1");
        return 1;
    }

    _hold_rx = (strtoul(argv[1], NULL, 0) != 0);
    if (_hold_rx) {
        _held_rx = 0;
    }

    printf("ETHOS_TEST hold=%u\n", (unsigned)_hold_rx);
    return 0;
}

static int _cmd_status(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int pending = _ethos.netdev.driver->recv(&_ethos.netdev, NULL, 0, NULL);

    printf("ETHOS_TEST pending=%d held=%u state=%u type=%u\n",
           pending,
           _held_rx,
           (unsigned)_ethos.state,
           _ethos.frametype);
    return 0;
}

static int _cmd_recv(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    uint8_t buf[64];

    int res = _ethos.netdev.driver->recv(
        &_ethos.netdev, buf, sizeof(buf), NULL
    );

    printf("ETHOS_TEST recv=%d data=", res);

    if (res > 0) {
        for (int i = 0; i < res; i++) {
            printf("%02x", buf[i]);
        }
    }

    puts("");
    return 0;
}

SHELL_COMMAND(ethos_hold, "hold/release ETHOS RX_COMPLETE", _cmd_hold);
SHELL_COMMAND(ethos_status, "show queued ETHOS RX state", _cmd_status);
SHELL_COMMAND(ethos_recv, "receive one ETHOS frame", _cmd_recv);

int main(void)
{
    puts("Test application for ETHOS RX frame handling");

    int res = netdev_eth_minimal_init();
    if (res) {
        printf("ETHOS_TEST init_failed=%d\n", res);
        return 1;
    }

    puts("Initialization successful - starting the shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, sizeof(line_buf));

    return 0;
}
