/*
 * Copyright (C) 2017 Inria
 *               2017 Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       Semtech LoRaMAC test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Jose Alamos <jose.alamos@inria.cl>
 */

#include <string.h>

#include "msg.h"
#include "shell.h"
#include "fmt.h"

#include "net/loramac.h"
#include "semtech_loramac.h"

#include "sx127x.h"
#include "sx127x_params.h"

sx127x_t sx127x;
static char print_buf[48];

static void _loramac_usage(void)
{
    puts("Usage: loramac <get|set|join|tx>");
}

static void _loramac_join_usage(void)
{
    puts("Usage: loramac join <otaa|abp>");
}

static void _loramac_tx_usage(void)
{
    puts("Usage: loramac tx <payload> [<cnf|uncnf>] [port]");
}

static void _loramac_set_usage(void)
{
    puts("Usage: loramac set <deveui|appeui|appkey|appskey|nwkskey|devaddr|"
         "class|dr|adr|public|netid|tx_power|rx2_freq|rx2_dr> <value>");
}

static void _loramac_get_usage(void)
{
    puts("Usage: loramac get <deveui|appeui|appkey|appskey|nwkskey|devaddr|"
         "class|dr|adr|public|netid|tx_power|rx2_freq|rx2_dr>");
}

static int _cmd_loramac(int argc, char **argv)
{
    if (argc < 2) {
        _loramac_usage();
        return 1;
    }

    if (strcmp(argv[1], "get") == 0) {
        if (argc < 3) {
            _loramac_get_usage();
            return 1;
        }

        if (strcmp("deveui", argv[2]) == 0) {
            uint8_t deveui[LORAMAC_DEVEUI_LEN];
            semtech_loramac_get_deveui(deveui);
            fmt_bytes_hex(print_buf, deveui, LORAMAC_DEVEUI_LEN);
            printf("DEVEUI: %s\n", print_buf);
        }
        else if (strcmp("appeui", argv[2]) == 0) {
            uint8_t appeui[LORAMAC_APPEUI_LEN];
            semtech_loramac_get_appeui(appeui);
            fmt_bytes_hex(print_buf, appeui, LORAMAC_APPEUI_LEN);
            printf("APPEUI: %s\n", print_buf);
        }
        else if (strcmp("appkey", argv[2]) == 0) {
            uint8_t appkey[LORAMAC_APPKEY_LEN];
            semtech_loramac_get_appkey(appkey);
            fmt_bytes_hex(print_buf, appkey, LORAMAC_APPKEY_LEN);
            printf("APPKEY: %s\n", print_buf);
        }
        else if (strcmp("appskey", argv[2]) == 0) {
            uint8_t appskey[LORAMAC_APPSKEY_LEN];
            semtech_loramac_get_appskey(appskey);
            fmt_bytes_hex(print_buf, appskey, LORAMAC_APPSKEY_LEN);
            printf("APPSKEY: %s\n", print_buf);
        }
        else if (strcmp("nwkskey", argv[2]) == 0) {
            uint8_t nwkskey[LORAMAC_NWKSKEY_LEN];
            semtech_loramac_get_nwkskey(nwkskey);
            fmt_bytes_hex(print_buf, nwkskey, LORAMAC_NWKSKEY_LEN);
            printf("NWKSKEY: %s\n", print_buf);
        }
        else if (strcmp("devaddr", argv[2]) == 0) {
            uint8_t devaddr[LORAMAC_DEVADDR_LEN];
            semtech_loramac_get_devaddr(devaddr);
            fmt_bytes_hex(print_buf, devaddr, LORAMAC_DEVADDR_LEN);
            printf("DEVADDR: %s\n", print_buf);
        }
        else if (strcmp("class", argv[2]) == 0) {
            printf("Device class: ");
            switch(semtech_loramac_get_class()) {
                case LORAMAC_CLASS_A:
                    puts("A");
                    break;
                case LORAMAC_CLASS_B:
                    puts("B");
                    break;
                case LORAMAC_CLASS_C:
                    puts("C");
                    break;
                default:
                    puts("Invalid");
                    break;
            }
        }
        else if (strcmp("dr", argv[2]) == 0) {
            printf("DATARATE: %d\n",
                   semtech_loramac_get_dr());
        }
        else if (strcmp("adr", argv[2]) == 0) {
            printf("ADR: %s\n",
                   semtech_loramac_get_adr() ? "on" : "off");
        }
        else if (strcmp("public", argv[2]) == 0) {
            printf("Public network: %s\n",
                   semtech_loramac_get_public_network() ? "on" : "off");
        }
        else if (strcmp("netid", argv[2]) == 0) {
            printf("NetID: %lu\n", semtech_loramac_get_netid());
        }
        else if (strcmp("tx_power", argv[2]) == 0) {
            printf("TX power index: %d\n", semtech_loramac_get_tx_power());
        }
        else if (strcmp("rx2_freq", argv[2]) == 0) {
            printf("RX2 freq: %lu\n", semtech_loramac_get_rx2_freq());
        }
        else if (strcmp("rx2_dr", argv[2]) == 0) {
            printf("RX2 dr: %d\n", semtech_loramac_get_rx2_dr());
        }
        else {
            _loramac_get_usage();
            return 1;
        }
    }
    else if (strcmp(argv[1], "set") == 0) {
        if (argc < 3) {
            _loramac_set_usage();
            return 1;
        }

        if (strcmp("deveui", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_DEVEUI_LEN * 2)) {
                puts("Usage: loramac set deveui <16 hex chars>");
                return 1;
            }
            uint8_t deveui[LORAMAC_DEVEUI_LEN];
            fmt_hex_bytes(deveui, argv[3]);
            semtech_loramac_set_deveui(deveui);
        }
        else if (strcmp("appeui", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPEUI_LEN * 2)) {
                puts("Usage: loramac set appeui <16 hex chars>");
                return 1;
            }
            uint8_t appeui[LORAMAC_APPEUI_LEN];
            fmt_hex_bytes(appeui, argv[3]);
            semtech_loramac_set_appeui(appeui);
        }
        else if (strcmp("appkey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPKEY_LEN * 2)) {
                puts("Usage: loramac set appkey <32 hex chars>");
                return 1;
            }
            uint8_t appkey[LORAMAC_APPKEY_LEN];
            fmt_hex_bytes(appkey, argv[3]);
            semtech_loramac_set_appkey(appkey);
        }
        else if (strcmp("appskey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_APPSKEY_LEN * 2)) {
                puts("Usage: loramac set appskey <32 hex chars>");
                return 1;
            }
            uint8_t appskey[LORAMAC_APPSKEY_LEN];
            fmt_hex_bytes(appskey, argv[3]);
            semtech_loramac_set_appskey(appskey);
        }
        else if (strcmp("nwkskey", argv[2]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_NWKSKEY_LEN * 2)) {
                puts("Usage: loramac set nwkskey <32 hex chars>");
                return 1;
            }
            uint8_t nwkskey[LORAMAC_NWKSKEY_LEN];
            fmt_hex_bytes(nwkskey, argv[3]);
            semtech_loramac_set_nwkskey(nwkskey);
        }
        else if (strcmp("devaddr", argv[3]) == 0) {
            if ((argc < 4) || (strlen(argv[3]) != LORAMAC_DEVADDR_LEN * 2)) {
                puts("Usage: loramac set devaddr <8 hex chars>");
                return 1;
            }
            uint8_t devaddr[LORAMAC_DEVADDR_LEN];
            fmt_hex_bytes(devaddr, argv[3]);
            semtech_loramac_set_devaddr(devaddr);
        }
        else if (strcmp("class", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set class <A,B,C>");
                return 1;
            }
            loramac_class_t cls;
            if (strcmp(argv[3], "A") == 0) {
                cls = LORAMAC_CLASS_A;
            }
            else if (strcmp(argv[3], "B") == 0) {
                cls = LORAMAC_CLASS_B;
            }
            else if (strcmp(argv[3], "C") == 0) {
                cls = LORAMAC_CLASS_C;
            }
            else {
                puts("Usage: loramac set class <A,B,C>");
                return 1;
            }
            semtech_loramac_set_class(cls);
        }
        else if (strcmp("dr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set dr <0..16>");
                return 1;
            }
            uint8_t dr = atoi(argv[3]);
            if (dr > LORAMAC_DR_15) {
                puts("Usage: loramac set dr <0..16>");
                return 1;
            }
            semtech_loramac_set_dr(dr);
        }
        else if (strcmp("adr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set adr <on|off>");
                return 1;
            }
            bool adr;
            if (strcmp("on", argv[3]) == 0) {
                adr = true;
            }
            else if (strcmp("off", argv[3]) == 0) {
                adr = false;
            }
            else {
                puts("Usage: loramac set adr <on|off>");
                return 1;
            }
            semtech_loramac_set_adr(adr);
        }
        else if (strcmp("public", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set public <on|off>");
                return 1;
            }
            bool public;
            if (strcmp("on", argv[3]) == 0) {
                public = true;
            }
            else if (strcmp("off", argv[3]) == 0) {
                public = false;
            }
            else {
                puts("Usage: loramac set public <on|off>");
                return 1;
            }
            semtech_loramac_set_public_network(public);
        }
        else if (strcmp("netid", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set netid <integer value>");
                return 1;
            }
            semtech_loramac_set_netid(strtoul(argv[2], NULL, 0));
        }
        else if (strcmp("tx_power", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set tx_power <0..16>");
                return 1;
            }
            uint8_t power = atoi(argv[3]);
            if (power > LORAMAC_TX_PWR_15) {
                puts("Usage: loramac set tx_power <0..16>");
                return 1;
            }
            semtech_loramac_set_tx_power(power);
        }
        else if (strcmp("rx2_freq", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set rx2_freq <frequency>");
                return 1;
            }
            uint32_t freq = atoi(argv[3]);
            semtech_loramac_set_rx2_freq(freq);
        }
        else if (strcmp("rx2_dr", argv[2]) == 0) {
            if (argc < 4) {
                puts("Usage: loramac set rx2_dr <0..16>");
                return 1;
            }
            uint8_t dr = atoi(argv[3]);
            if (dr > LORAMAC_DR_15){
                puts("Usage: loramac set rx2_dr <0..16>");
                return 1;
            }
            semtech_loramac_set_rx2_dr(dr);
        }
        else {
            _loramac_set_usage();
            return 1;
        }
    }
    else if (strcmp(argv[1], "join") == 0) {
        if (argc < 3) {
            _loramac_join_usage();
            return 1;
        }

        uint8_t join_type;
        if (strcmp(argv[2], "otaa") == 0) {
            join_type = LORAMAC_JOIN_OTAA;
        }
        else if (strcmp(argv[2], "abp") == 0) {
            join_type = LORAMAC_JOIN_ABP;
        }
        else {
            _loramac_join_usage();
            return 1;
        }

        if (semtech_loramac_join(join_type) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
            puts("Join procedure failed!");
            return 1;
        }

        puts("Join procedure succeeded!");
    }
    else if (strcmp(argv[1], "tx") == 0) {
        if (argc < 3) {
            _loramac_tx_usage();
            return 1;
        }

        uint8_t cnf = LORAMAC_DEFAULT_TX_MODE;  /* Default: confirmable */
        uint8_t port = LORAMAC_DEFAULT_TX_PORT; /* Default: 2 */
        /* handle optional parameters */
        if (argc > 3) {
            if (strcmp(argv[3], "cnf") == 0) {
                cnf = LORAMAC_TX_CNF;
            }
            else if (strcmp(argv[3], "uncnf") == 0) {
                cnf = LORAMAC_TX_UNCNF;
            }
            else {
                _loramac_tx_usage();
                return 1;
            }

            if (argc > 4) {
                port = atoi(argv[4]);
                if (port == 0 || port >= 224) {
                    printf("error: invalid port given '%d', "
                           "port can only be between 1 and 223\n", port);
                    return 1;
                }
            }
        }

        semtech_loramac_rx_data_t rx_data;
        switch (semtech_loramac_send(cnf, port,
                                     (uint8_t *)argv[2], strlen(argv[2]),
                                     &rx_data)) {
            case SEMTECH_LORAMAC_RX_DATA:
                printf("Data received: %s, port: %d\n",
                       (char *)rx_data.payload, rx_data.port);
                return 0;

            case SEMTECH_LORAMAC_TX_DONE:
                puts("TX done");
                return 0;

            case SEMTECH_LORAMAC_NOT_JOINED:
                puts("Failed: not joined");
                return 1;
        }
    }
    else {
        _loramac_usage();
        return 1;
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "loramac", "control the loramac stack", _cmd_loramac },
    { NULL, NULL, NULL }
};

int main(void)
{
    sx127x_setup(&sx127x, &sx127x_params[0]);
    semtech_loramac_init(&sx127x);

    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
