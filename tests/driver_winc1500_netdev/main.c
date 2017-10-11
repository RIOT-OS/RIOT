/*
 * Copyright (C) 2017 Bumsik Kim <k.bumsik@gmail.com>
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
 * @brief       Test application for the WINC1500 driver
 *
 * @author      Bumsik Kim <k.bumsik@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "shell.h"
#include <string.h>

#include "winc1500.h"
#include "winc1500_params.h"

/* Shell functions */
static int _init(int argc, char **argv);
static int _scan(int argc, char **argv);
static int _connect(int argc, char **argv);
static int _disconnect(int argc, char **argv);
static int _info(int argc, char **argv);

#define MAIN_QUEUE_SIZE     (8U)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static const shell_command_t shell_commands[] = {
    { "init", "initializes WINC1500 module", _init },
    { "scan", "Scan for Access Points available", _scan },
    { "connect", "Connect to an Access Point", _connect },
    { "disconnect", "Connect from an Access Point", _disconnect },
    { "info", "Connected AP information", _info },
    { NULL, NULL, NULL }
};

static int _init(int argc, char **argv)
{
#ifdef MODULE_NETDEV_ETH
    puts("winc1500_init() won't have any effect if GNRC(NETDEV_ETH) is enabled.");
#endif
    if (WINC1500_OK == winc1500_init(&winc1500_params[0])) {
        uint8_t mac[6];
        winc1500_get_mac_addr(mac);
        printf("MAC Address : ");
        printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
            mac[0], mac[1],
            mac[2], mac[3], 
            mac[4], mac[5]);

        puts("[OK]");
        return 0;
    }
    puts("[Error]");
    return 1;
}

static int _scan(int argc, char **argv)
{
    int result = winc1500_scan();
    if (result < 0) {
        puts("[Scanning error]");
        return 1;
    }
    printf("%d access points found\n", result);

    char ssid[WINC1500_MAX_SSID_LEN];
    winc1500_ap_t ap = {.ssid = ssid};
    for (int i = 0; i < result; i++) {
        winc1500_read_ap(&ap, i);
        printf("[%d] %s %d dBm ", i, ssid, ap.rssi);
        if (ap.sec & WINC1500_SEC_FLAGS_ENTERPRISE) {
            puts("WPA_Enterprise");
        }
        else if (ap.sec & WINC1500_SEC_FLAGS_WPA) {
            puts("WPA_PSK");
        }
        else if (ap.sec & WINC1500_SEC_FLAGS_OPEN) {
            puts("OPEN");
        }
        else if (ap.sec & WINC1500_SEC_FLAGS_WEP) {
            puts("WEP");
        }
        else {
            puts("Unknown");
        }
    }
    puts("[OK]");
    return result;
}

static int _connect(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s SSID [passphrase]\n", argv[0]);
        return 1;
    }

    winc1500_ap_t ap = {.ssid = NULL, .password = NULL};

    /* Get SSID */
    if (argc > 1) {
        ap.ssid = (char *)argv[1];
        ap.sec = WINC1500_SEC_FLAGS_OPEN;
    } else {
        puts("Please provide SSID to connect");
        return 1;
    }
    /* Get password if provided */
    if (argc > 2) {
        ap.password = (char *)argv[2];
        ap.sec = WINC1500_SEC_FLAGS_WPA2;
    }

    int result = winc1500_connect(&ap);
    if (result == WINC1500_OK) {
        puts("[OK]");
        return 0;
    } else {
        puts("Try using another SSID and passphrase, or just try connecting again.");
        puts("[Connecting failed]");
        return 1;
    }
}

static int _disconnect(int argc, char **argv)
{
    int result = winc1500_disconnect();
    if (result == WINC1500_OK) {
        puts("[OK]");
        return 0;
    } else {
        puts("[Failed]");
        return 1;
    }
}

static int _info(int argc, char **argv)
{
    char ssid[WINC1500_MAX_SSID_LEN];
    uint8_t mac[6];
    winc1500_ap_t ap = {.ssid = ssid};
    
    int result = winc1500_connection_info(&ap, mac);
    if (result < 0) {
        puts("Wi-Fi is not connected yet.");
        puts("[Failed]");
        return 1;
    }

    printf("AP Info: %s %d dBm ", ssid, ap.rssi);
    if (ap.sec & WINC1500_SEC_FLAGS_ENTERPRISE) {
        puts("WPA_Enterprise");
    }
    else if (ap.sec & WINC1500_SEC_FLAGS_WPA) {
        puts("WPA_PSK");
    }
    else if (ap.sec & WINC1500_SEC_FLAGS_OPEN) {
        puts("OPEN");
    }
    else if (ap.sec & WINC1500_SEC_FLAGS_WEP) {
        puts("WEP");
    }
    else {
        puts("Unknown");
    }
    printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
            mac[0], mac[1], mac[2],
            mac[3], mac[4], mac[5]);
    puts("[OK]");
    return result;
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("WINC1500 WiFi module test application");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
