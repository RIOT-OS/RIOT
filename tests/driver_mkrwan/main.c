/*
 * Copyright (C) 2018 Inria
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
 * @brief       Test application for MKRWAN LoRa module driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "timex.h"
#include "shell.h"
#include "shell_commands.h"
#include "fmt.h"

#include "mkrwan_params.h"
#include "mkrwan.h"

static mkrwan_t mkrwan_dev;

static uint8_t payload[MKRWAN_INT_BUF_SIZE];

static void _print_mkrwan_usage(void)
{
    puts("Usage: mkrwan <get|set|join|tx|reset|version>");
}

static void _print_join_usage(void)
{
    puts("Usage: mkrwan join <otaa|abp>");
}

static void _print_tx_usage(void)
{
    puts("Usage: mkrwan tx <data> <uncnf|cnf> <port>");
}

static void _print_set_usage(void)
{
    puts("Usage: mkrwan set "
         "<appeui|devaddr|appskey|nwkskey|class|public|duty_cycle|"
         "dr|adr|port> <value>");
}

static void _print_get_usage(void)
{
    puts("Usage: mkrwan get "
         "<deveui|appeui|devaddr|appskey|nwkskey|class|public|duty_cycle|"
         "dr|adr|port>");
}

int mkrwan_cmd(int argc, char **argv)
{
    if (strcmp(argv[1], "get") == 0) {
        if (argc != 3) {
            _print_get_usage();
            return 1;
        }

        if (strcmp(argv[2], "deveui") == 0) {
            char buf[LORAMAC_DEVEUI_LEN * 2 + 1];
            mkrwan_get_deveui(&mkrwan_dev, payload);
            fmt_bytes_hex(buf, payload, LORAMAC_DEVEUI_LEN);
            buf[LORAMAC_DEVEUI_LEN * 2] = '\0';
            printf("device eui: %s\n", buf);
        }
        else if (strcmp(argv[2], "appeui") == 0) {
            char buf[LORAMAC_APPEUI_LEN * 2 + 1];
            mkrwan_get_appeui(&mkrwan_dev, payload);
            fmt_bytes_hex(buf, payload, LORAMAC_APPEUI_LEN);
            buf[LORAMAC_APPEUI_LEN * 2] = '\0';
            printf("application eui: %s\n", buf);
        }
        else if (strcmp(argv[2], "appkey") == 0) {
            char buf[LORAMAC_APPKEY_LEN * 2 + 1];
            mkrwan_get_appkey(&mkrwan_dev, payload);
            fmt_bytes_hex(buf, payload, LORAMAC_APPKEY_LEN);
            buf[LORAMAC_APPKEY_LEN * 2] = '\0';
            printf("application key: %s\n", buf);
        }
        else if (strcmp(argv[2], "devaddr") == 0) {
            char buf[LORAMAC_DEVADDR_LEN * 2 + 1];
            mkrwan_get_devaddr(&mkrwan_dev, payload);
            fmt_bytes_hex(buf, payload, LORAMAC_DEVADDR_LEN);
            buf[LORAMAC_DEVADDR_LEN * 2] = '\0';
            printf("device address: %s\n", buf);
        }
        else if (strcmp(argv[2], "appskey") == 0) {
            char buf[LORAMAC_APPSKEY_LEN * 2 + 1];
            mkrwan_get_appskey(&mkrwan_dev, payload);
            fmt_bytes_hex(buf, payload, LORAMAC_APPSKEY_LEN);
            buf[LORAMAC_APPSKEY_LEN * 2] = '\0';
            printf("application session key: %s\n", buf);
        }
        else if (strcmp(argv[2], "nwkskey") == 0) {
            char buf[LORAMAC_NWKSKEY_LEN * 2 + 1];
            mkrwan_get_nwkskey(&mkrwan_dev, payload);
            fmt_bytes_hex(buf, payload, LORAMAC_NWKSKEY_LEN);
            buf[LORAMAC_NWKSKEY_LEN * 2] = '\0';
            printf("network session key: %s\n", buf);
        }
        else if (strcmp(argv[2], "public") == 0) {
            bool enable = mkrwan_get_public_network(&mkrwan_dev);
            printf("Public network: %s\n", enable ? "on": "off");
        }
        else if (strcmp(argv[2], "duty_cycle") == 0) {
            bool enable = mkrwan_get_duty_cycle(&mkrwan_dev);
            printf("Duty cycle: %s\n", enable ? "on": "off");
        }
        else if (strcmp(argv[2], "dr") == 0) {
            uint8_t dr = mkrwan_get_datarate(&mkrwan_dev);
            printf("Datarate: %d\n", dr);
        }
        else if (strcmp(argv[2], "adr") == 0) {
            bool enable = mkrwan_get_adr(&mkrwan_dev);
            printf("Adaptive datarate: %s\n", enable ? "on": "off");
        }
        else if (strcmp(argv[2], "port") == 0) {
            uint8_t port = mkrwan_get_tx_port(&mkrwan_dev);
            printf("Port: %d\n", port);
        }
        else {
            _print_get_usage();
            return 1;
        }
    }
    else if (strcmp(argv[1], "set") == 0) {
        if (argc != 4) {
            _print_set_usage();
            return 1;
        }

        if (strcmp(argv[2], "deveui") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            mkrwan_set_deveui(&mkrwan_dev, payload);
        }
        else if (strcmp(argv[2], "appeui") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            mkrwan_set_appeui(&mkrwan_dev, payload);
        }
        else if (strcmp(argv[2], "appkey") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            mkrwan_set_appkey(&mkrwan_dev, payload);
        }
        else if (strcmp(argv[2], "devaddr") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            mkrwan_set_devaddr(&mkrwan_dev, payload);
        }
        else if (strcmp(argv[2], "appskey") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            mkrwan_set_appskey(&mkrwan_dev, payload);
        }
        else if (strcmp(argv[2], "nwkskey") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            mkrwan_set_nwkskey(&mkrwan_dev, payload);
        }
        else if (strcmp(argv[2], "class") == 0) {
            uint8_t class;
            if (strcmp(argv[3], "A") == 0) {
                class = LORAMAC_CLASS_A;
            }
            else if (strcmp(argv[3], "B") == 0) {
                class = LORAMAC_CLASS_B;
            }
            else if (strcmp(argv[3], "C") == 0) {
                class = LORAMAC_CLASS_C;
            }
            else {
                printf("Invalid class '%s'\n", argv[3]);
                return 1;
            }

            mkrwan_set_class(&mkrwan_dev, class);
        }
        else if (strcmp(argv[2], "public") == 0) {
            mkrwan_set_public_network(&mkrwan_dev, strcmp(argv[3], "on") == 0);
        }
        else if (strcmp(argv[2], "duty_cycle") == 0) {
            mkrwan_set_duty_cycle(&mkrwan_dev, strcmp(argv[3], "on") == 0);
        }
        else if (strcmp(argv[2], "dr") == 0) {
            mkrwan_set_datarate(&mkrwan_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "adr") == 0) {
            mkrwan_set_adr(&mkrwan_dev, (bool)atoi(argv[3]));
        }
        else if (strcmp(argv[2], "port") == 0) {
            mkrwan_set_tx_port(&mkrwan_dev, atoi(argv[3]));
        }
        else {
            _print_set_usage();
            return 1;
        }
    }
    else if (strcmp(argv[1], "join") == 0) {
        if (argc != 3) {
            _print_join_usage();
            return 1;
        }

        uint8_t mode;
        if (strcmp(argv[2], "otaa") == 0) {
            mode = LORAMAC_JOIN_OTAA;
        }
        else {
            mode = LORAMAC_JOIN_ABP;
        }
        if (mkrwan_join(&mkrwan_dev, mode) != MKRWAN_OK) {
            puts("Join procedure failed!");
        }
        else {
            puts("Join procedure succeeded!");
        }
    }
    else if (strcmp(argv[1], "tx") == 0) {
        if ((argc < 3) || (argc > 5)) {
            _print_tx_usage();
            return 1;
        }

        uint8_t mode = LORAMAC_DEFAULT_TX_MODE;

        if (argc > 3) {
            if (strcmp(argv[3], "cnf") == 0) {
                mode = LORAMAC_TX_CNF;
            }
            else if (strcmp(argv[3], "uncnf") == 0) {
                mode = LORAMAC_TX_UNCNF;
            }
            else {
                _print_tx_usage();
                return 1;
            }
        }

        if (argc > 4) {
            uint8_t port = atoi(argv[4]);
            mkrwan_set_tx_port(&mkrwan_dev, port);
        }

        if (mkrwan_send(&mkrwan_dev, (uint8_t*)argv[2], strlen(argv[2]),
                        mode) != MKRWAN_OK) {
            puts("Send failed!");
        }
        else {
            puts("Data sent with success");
        }
    }
    else if (strcmp(argv[1], "reset") == 0) {
        if (argc != 2) {
            _print_mkrwan_usage();
            return 1;
        }

        char buf[MKRWAN_INT_BUF_SIZE];
        mkrwan_version(&mkrwan_dev, buf);
        printf("Version: %s\n", buf);
    }
    else if (strcmp(argv[1], "version") == 0) {
        if (argc != 2) {
            _print_mkrwan_usage();
            return 1;
        }

        char buf[MKRWAN_INT_BUF_SIZE];
        mkrwan_version(&mkrwan_dev, buf);
        printf("Version: %s\n", buf);
    }
    else {
        _print_mkrwan_usage();
        return 1;
    }

    return 0;
}

int mkrwan_at_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <at command>\n", argv[0]);
        return 1;
    }

    char buf[MKRWAN_INT_BUF_SIZE];
    at_send_cmd_get_resp(&mkrwan_dev.at_dev, argv[1],
                         buf, sizeof(buf), MKRWAN_TIMEOUT);

    printf("Response: %s\n", buf);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "mkrwan",     "Control the mkrwan driver",    mkrwan_cmd },
    { "at",         "Send an AT command",        mkrwan_at_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("MKRWAN device driver test");

    mkrwan_setup(&mkrwan_dev, &mkrwan_params[0]);
    if (mkrwan_init(&mkrwan_dev) != MKRWAN_OK) {
        puts("MKRWAN initialization failed");
        return -1;
    }

    /* start the shell */
    puts("Initialization OK, starting shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
