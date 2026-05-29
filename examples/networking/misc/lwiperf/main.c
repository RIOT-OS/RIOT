/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Network throughput benchmarks using lwIP iPerf.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "shell.h"
#include "thread.h"
#include "ztimer.h"

#include "arch/sys_arch.h"
#include "lwip.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"

/**
 * @brief Default DHCP timeout in seconds.
 */
#ifndef TEST_DHCP_TIMEOUT_SEC
#  define TEST_DHCP_TIMEOUT_SEC   10
#endif

/**
 * @brief Default local IPv4 address, when DHCP is not used.
 */
#ifndef TEST_ADDR4_LOCAL
#  define TEST_ADDR4_LOCAL        IP4_ADDR_INIT(192, 168, 100, 11)
#endif

/**
 * @brief Default local IPv4 subnet mask, when DHCP is not used.
 */
#ifndef TEST_ADDR4_MASK
#  define TEST_ADDR4_MASK         IP4_ADDR_INIT(255, 255, 255, 0)
#endif

static void *_session;

static void _iperf_report_cb(void *arg,
                             enum lwiperf_report_type report_type,
                             const ip_addr_t *local_addr,  u16_t local_port,
                             const ip_addr_t *remote_addr, u16_t remote_port,
                             u32_t bytes_transferred,
                             u32_t ms_duration,
                             u32_t bandwidth_kbitpsec)
{
    (void)arg;
    (void)local_addr;
    (void)local_port;

    const char *type_str;

    switch (report_type) {
    case LWIPERF_TCP_DONE_SERVER:
        type_str = "TCP server done";
        break;
    case LWIPERF_TCP_DONE_CLIENT:
        type_str = "TCP client done";
        break;
    case LWIPERF_TCP_ABORTED_LOCAL:
        type_str = "aborted (local)";
        break;
    case LWIPERF_TCP_ABORTED_REMOTE:
        type_str = "aborted (remote)";
        break;
    default:
        type_str = "unknown";
        break;
    }

    puts("--- Results ---");
    printf("Status      : %s\n", type_str);
    printf("Remote      : %s:%" PRIu16 "\n", ipaddr_ntoa(remote_addr), remote_port);
    printf("Transferred : %" PRIu32 " bytes\n", bytes_transferred);
    printf("Duration    : %" PRIu32 " ms\n",    ms_duration);
    printf("Bandwidth   : %" PRIu32 " kbit/s\n", bandwidth_kbitpsec);
    puts("--------------");
    puts("");

    /* runs from the lwIP thread, so locking not necessary */
    _session = NULL;
}

static int _cmd_server(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    sys_lock_tcpip_core();

    if (_session != NULL) {
        sys_unlock_tcpip_core();
        puts("Error: another session is already active. Run 'abort' first.");
        return 1;
    }

    _session = lwiperf_start_tcp_server_default(_iperf_report_cb, NULL);

    if (_session == NULL) {
        sys_unlock_tcpip_core();
        puts("Error: failed to start server");
        return 1;
    }

    sys_unlock_tcpip_core();

    printf("TCP server listening on port %d\n", LWIPERF_TCP_PORT_DEFAULT);

#if IS_USED(MODULE_LWIP_IPV4) && !IS_USED(MODULE_LWIP_IPV6)
    puts("Run on host: iperf -c <board_ip>");
#elif !IS_USED(MODULE_LWIP_IPV4) && IS_USED(MODULE_LWIP_IPV6)
    puts("Run on host: iperf -V -c <board_ip>");
#else
    puts("Run on host: iperf -c <board_ip>    (for IPv4)");
    puts("             iperf -V -c <board_ip> (for IPv6)");
#endif

    return 0;
}

static int _cmd_client(int argc, char **argv)
{
    ip_addr_t remote_addr;

    if (argc < 2) {
        printf("Usage: %s <ip>\n", argv[0]);
        return 1;
    }

    sys_lock_tcpip_core();

    if (_session != NULL) {
        sys_unlock_tcpip_core();
        puts("Error: another session is already active. Run 'abort' first.");
        return 1;
    }

    if (!ipaddr_aton(argv[1], &remote_addr)) {
        sys_unlock_tcpip_core();
        printf("Error: invalid IP address: %s\n", argv[1]);
        return 1;
    }

    _session = lwiperf_start_tcp_client_default(&remote_addr, _iperf_report_cb, NULL);

    if (_session == NULL) {
        sys_unlock_tcpip_core();
        puts("Error: failed to start client.");
        return 1;
    }

    sys_unlock_tcpip_core();

    printf("TCP client connecting to %s port %d\n", argv[1], LWIPERF_TCP_PORT_DEFAULT);

    return 0;
}

static int _cmd_abort(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    sys_lock_tcpip_core();

    if (_session == NULL) {
        sys_unlock_tcpip_core();
        puts("Error: no active session.");
        return 1;
    }

    lwiperf_abort(_session);
    _session = NULL;

    sys_unlock_tcpip_core();

    puts("Session aborted.");

    return 0;
}

SHELL_COMMAND(abort,  "abort active iPerf session", _cmd_abort);
SHELL_COMMAND(client, "start iPerf client <ip>", _cmd_client);
SHELL_COMMAND(server, "start iPerf server", _cmd_server);

int main(void)
{
#if IS_USED(MODULE_LWIP_IPV4) && IS_USED(MODULE_LWIP_DHCP_AUTO)
    /* auto-configure using DHCP */
    puts("Waiting for DHCP address autoconfiguration ...");
    ztimer_sleep(ZTIMER_MSEC, TEST_DHCP_TIMEOUT_SEC * MS_PER_SEC);
#elif IS_USED(MODULE_LWIP_IPV4)
    /* configure static IP address */
    ip4_addr_t ip = TEST_ADDR4_LOCAL;
    ip4_addr_t subnet = TEST_ADDR4_MASK;

    sys_lock_tcpip_core();
    struct netif *iface = netif_find("ET0");
    netif_set_addr(iface, &ip, &subnet, NULL);
    sys_unlock_tcpip_core();
#endif

    /* print usage instruction */
    puts("lwIP iPerf application running. Use `help` to get started.");

    /* spawn a shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
