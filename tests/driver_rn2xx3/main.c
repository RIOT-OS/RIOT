/*
 * Copyright (C) 2017 Inria
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
 * @brief       Test application for RN2483/RN2903 LoRa module driver
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

#include "rn2xx3.h"
#include "rn2xx3_params.h"
#include "rn2xx3_internal.h"

static rn2xx3_t rn2xx3_dev;
static uint8_t payload[RN2XX3_MAX_BUF];

static void _print_sys_usage(void)
{
    puts("Usage: sys <sleep|reset|factoryRESET>");
}

static void _print_mac_usage(void)
{
    puts("Usage: mac <join|tx|set|get|save>");
}

static void _print_mac_get_usage(void)
{
    puts("Usage: mac get "
         "<deveui|appeui|devaddr|txport|txmode|poweridx|dr|adr|"
         "band|retx|rx1|rx2|ar|rx2dr|rx2freq>");
}

static void _print_mac_set_usage(void)
{
    puts("Usage: mac set "
         "<deveui|appeui|appkey|devaddr|appskey|nwkskey|txport|txmode|pwridx|dr"
         "|adr|bat|retx|linkchk|rx1|ar|rx2dr|rx2freq|sleep_duration> <value>");
}

int rn2xx3_sys_cmd(int argc, char **argv) {
    if (argc < 2) {
        _print_sys_usage();
        return -1;
    }

    if (strcmp(argv[1], "reset") == 0) {
        if (rn2xx3_sys_reset(&rn2xx3_dev) != RN2XX3_OK) {
            puts("System reset failed");
            return -1;
        }

        printf("System version: %s\n", rn2xx3_dev.resp_buf);
    }
    else if (strcmp(argv[1], "sleep") == 0) {
        if (rn2xx3_sys_sleep(&rn2xx3_dev) != RN2XX3_OK) {
            puts("Cannot put device in sleep mode");
            return -1;
        }

        printf("Success: device is in sleep mode during %lus\n",
               (unsigned long)rn2xx3_dev.sleep / MS_PER_SEC);
    }
    else if (strcmp(argv[1], "factoryRESET") == 0) {
        if (rn2xx3_sys_factory_reset(&rn2xx3_dev) != RN2XX3_OK) {
            puts("Factory reset failed");
            return -1;
        }

        printf("System version: %s\n", rn2xx3_dev.resp_buf);
    }
    else {
        _print_sys_usage();
        return -1;
    }

    return 0;
}

int rn2xx3_mac_cmd(int argc, char **argv) {
    if (argc < 2) {
        _print_mac_usage();
        return -1;
    }

    if (strcmp(argv[1], "join") == 0) {
        if (argc != 3) {
            puts("Usage: mac join <otaa|abp>");
            return -1;
        }

        uint8_t mode;
        if (strcmp(argv[2], "otaa") == 0) {
            mode = LORAMAC_JOIN_OTAA;
        }
        else {
            mode = LORAMAC_JOIN_ABP;
        }

        switch (rn2xx3_mac_join_network(&rn2xx3_dev, mode)) {
            case RN2XX3_ERR_NO_FREE_CH:
                printf("%s: all channels are busy\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_SILENT:
                printf("%s: device is in silent state\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_FR_CNT_REJOIN_NEEDED:
                printf("%s: frame counter rolled over\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_BUSY:
                printf("%s: MAC layer not in Idle state\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_KEYS_NOT_INIT:
                printf("%s: device keys not configured\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_REPLY_TIMEOUT:
                puts("[ERROR]: No reply received from server");
                return -1;

            case RN2XX3_REPLY_JOIN_ACCEPTED:
                puts("Join procedure succeeded");
                break;

            case RN2XX3_REPLY_JOIN_DENIED:
                puts("Join procedure failed");
                return -1;

            default:
                /* Should not happen */
                break;
        }
    }
    else if (strcmp(argv[1], "tx") == 0) {
        if (argc != 3) {
            puts("Usage: mac tx <payload>");
            return -1;
        }

        switch (rn2xx3_mac_tx(&rn2xx3_dev, (uint8_t *)argv[2], strlen(argv[2]))) {
            case RN2XX3_ERR_INVALID_PARAM:
                printf("%s: invalid param given\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_NOT_JOINED:
                printf("%s: network is not joined\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_NO_FREE_CH:
                printf("%s: all channels are busy\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_SILENT:
                printf("%s: device is in silent state\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_FR_CNT_REJOIN_NEEDED:
                printf("%s: frame counter rolled over\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_BUSY:
                printf("%s: MAC layer not in Idle state\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_ERR_KEYS_NOT_INIT:
                printf("%s: device keys not configured\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_REPLY_TIMEOUT:
                puts("[ERROR]: No reply received from server");
                return -1;

            case RN2XX3_ERR_INVALID_DATA_LENGTH:
                printf("%s: payload is too large\n", rn2xx3_dev.cmd_buf);
                return -1;

            case RN2XX3_REPLY_TX_MAC_OK:
                puts("MAC transmission succeeded");
                break;

            case RN2XX3_REPLY_TX_MAC_ERR:
                puts("MAC transmission failed");
                return -1;

            case RN2XX3_REPLY_TX_INVALID_DATA_LEN:
                puts("Application payload too large");
                return -1;

            case RN2XX3_REPLY_TX_MAC_RX:
            {
                puts("MAC transmission succeeded");
                puts("Data received:");
                printf(" -port: %d\n", rn2xx3_mac_get_rx_port(&rn2xx3_dev));
                printf(" -payload len: %d\n", (uint8_t)strlen((char *)rn2xx3_dev.rx_buf));
                printf(" -payload: '%s'\n", rn2xx3_dev.rx_buf);
            }
                break;

            default:
                /* Should not happen */
                break;
        }
    }
    else if (strcmp(argv[1], "save") == 0) {
        if (rn2xx3_mac_save(&rn2xx3_dev) != RN2XX3_OK) {
            puts("Cannot save MAC state");
        }
        else {
            puts("MAC state saved");
        }
    }
    else if (strcmp(argv[1], "get") == 0) {
        if (argc < 3) {
            _print_mac_get_usage();
            return -1;
        }
        if (strcmp(argv[2], "deveui") == 0) {
            rn2xx3_mac_get_deveui(&rn2xx3_dev, payload);
            memset(rn2xx3_dev.resp_buf, 0, strlen(rn2xx3_dev.resp_buf));
            fmt_bytes_hex(rn2xx3_dev.resp_buf, payload, LORAMAC_DEVEUI_LEN);
            payload[LORAMAC_DEVEUI_LEN*2] = '\0';
            printf("device eui: %s\n", rn2xx3_dev.resp_buf);
        }
        else if (strcmp(argv[2], "appeui") == 0) {
            rn2xx3_mac_get_appeui(&rn2xx3_dev, payload);
            memset(rn2xx3_dev.resp_buf, 0, strlen(rn2xx3_dev.resp_buf));
            fmt_bytes_hex(rn2xx3_dev.resp_buf, payload, LORAMAC_APPEUI_LEN);
            payload[LORAMAC_APPEUI_LEN*2] = '\0';
            printf("application eui: %s\n", rn2xx3_dev.resp_buf);
        }
        else if (strcmp(argv[2], "devaddr") == 0) {
            rn2xx3_mac_get_devaddr(&rn2xx3_dev, payload);
            memset(rn2xx3_dev.resp_buf, 0, strlen(rn2xx3_dev.resp_buf));
            fmt_bytes_hex(rn2xx3_dev.resp_buf, payload, LORAMAC_DEVADDR_LEN);
            payload[LORAMAC_DEVADDR_LEN*2] = '\0';
            printf("device addr: %s\n", rn2xx3_dev.resp_buf);
        }
        else if (strcmp(argv[2], "txport") == 0) {
            uint8_t port = rn2xx3_mac_get_tx_port(&rn2xx3_dev);
            printf("TX port: %d\n", port);
        }
        else if (strcmp(argv[2], "txmode") == 0) {
            loramac_tx_mode_t mode = rn2xx3_mac_get_tx_mode(&rn2xx3_dev);
            printf("TX mode: ");
            switch (mode) {
                case LORAMAC_TX_CNF:
                    printf("confirmable");
                    break;
                case LORAMAC_TX_UNCNF:
                    printf("unconfirmable");
                    break;
                default:
                    printf("not supported");
                    break;
            }
            puts("");
        }
        else if (strcmp(argv[2], "pwridx") == 0) {
            loramac_tx_pwr_idx_t idx = rn2xx3_mac_get_tx_power(&rn2xx3_dev);
            printf("pwridx: %d\n", idx);
        }
        else if (strcmp(argv[2], "dr") == 0) {
            loramac_dr_idx_t dr = rn2xx3_mac_get_dr(&rn2xx3_dev);
            printf("dr: %d\n", dr);
        }
        else if (strcmp(argv[2], "band") == 0) {
            uint16_t band = rn2xx3_mac_get_band(&rn2xx3_dev);
            printf("band: %dMHz\n", band);
        }
        else if (strcmp(argv[2], "adr") == 0) {
            bool adr = rn2xx3_mac_get_adr(&rn2xx3_dev);
            printf("adr: %s\n", adr ? "on" : "off");
        }
        else if (strcmp(argv[2], "retx") == 0) {
            uint8_t retx = rn2xx3_mac_get_adr(&rn2xx3_dev);
            printf("retx: %d\n", retx);
        }
        else if (strcmp(argv[2], "rx1") == 0) {
            uint16_t rx1 = rn2xx3_mac_get_rx1_delay(&rn2xx3_dev);
            printf("rx1: %d\n", rx1);
        }
        else if (strcmp(argv[2], "rx2") == 0) {
            uint16_t rx2 = rn2xx3_mac_get_rx2_delay(&rn2xx3_dev);
            printf("rx2: %d\n", rx2);
        }
        else if (strcmp(argv[2], "ar") == 0) {
            bool ar = rn2xx3_mac_get_ar(&rn2xx3_dev);
            printf("ar: %s\n", ar ? "on" : "off");
        }
        else if (strcmp(argv[2], "rx2dr") == 0) {
            loramac_dr_idx_t rx2_dr = rn2xx3_mac_get_rx2_dr(&rn2xx3_dev);
            printf("rx2 dr: %d\n", rx2_dr);
        }
        else if (strcmp(argv[2], "rx2freq") == 0) {
            uint32_t rx2_freq = rn2xx3_mac_get_rx2_freq(&rn2xx3_dev);
            printf("rx2 freq: %lu\n", (unsigned long)rx2_freq);
        }
        else {
            _print_mac_get_usage();
            return -1;
        }
    }
    else if (strcmp(argv[1], "set") == 0) {
        if (argc < 4) {
            _print_mac_set_usage();
            return -1;
        }
        if (strcmp(argv[2], "deveui") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            rn2xx3_mac_set_deveui(&rn2xx3_dev, payload);
        }
        else if (strcmp(argv[2], "appeui") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            rn2xx3_mac_set_appeui(&rn2xx3_dev, payload);
        }
        else if (strcmp(argv[2], "appkey") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            rn2xx3_mac_set_appkey(&rn2xx3_dev, payload);
        }
        else if (strcmp(argv[2], "devaddr") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            rn2xx3_mac_set_devaddr(&rn2xx3_dev, payload);
        }
        else if (strcmp(argv[2], "appskey") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            rn2xx3_mac_set_appskey(&rn2xx3_dev, payload);
        }
        else if (strcmp(argv[2], "nwkskey") == 0) {
            fmt_hex_bytes(payload, argv[3]);
            rn2xx3_mac_set_nwkskey(&rn2xx3_dev, payload);
        }
        else if (strcmp(argv[2], "txport") == 0) {
            rn2xx3_mac_set_tx_port(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "txmode") == 0) {
            loramac_tx_mode_t mode;
            if (strcmp(argv[3], "cnf") == 0) {
                mode = LORAMAC_TX_CNF;
            }
            else if (strcmp(argv[3], "uncnf") == 0) {
                mode = LORAMAC_TX_UNCNF;
            }
            else {
                puts("Usage: mac set txmode <cnf|uncnf>");
                return -1;
            }
            rn2xx3_mac_set_tx_mode(&rn2xx3_dev, mode);
        }
        else if (strcmp(argv[2], "pwridx") == 0) {
            rn2xx3_mac_set_tx_power(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "dr") == 0) {
            rn2xx3_mac_set_dr(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "adr") == 0) {
            bool adr = false;
            if (strcmp(argv[3], "on") == 0) {
                adr = true;
            }
            rn2xx3_mac_set_adr(&rn2xx3_dev, adr);
        }
        else if (strcmp(argv[2], "bat") == 0) {
            rn2xx3_mac_set_battery(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "retx") == 0) {
            rn2xx3_mac_set_retx(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "linkchk") == 0) {
            rn2xx3_mac_set_linkchk_interval(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "rx1") == 0) {
            rn2xx3_mac_set_rx1_delay(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "ar") == 0) {
            bool ar = false;
            if (strcmp(argv[3], "on") == 0) {
                ar = true;
            }
            rn2xx3_mac_set_ar(&rn2xx3_dev, ar);
        }
        else if (strcmp(argv[2], "rx2dr") == 0) {
            rn2xx3_mac_set_rx2_dr(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "rx2freq") == 0) {
            rn2xx3_mac_set_rx2_freq(&rn2xx3_dev, atoi(argv[3]));
        }
        else if (strcmp(argv[2], "sleep_duration") == 0) {
            rn2xx3_sys_set_sleep_duration(&rn2xx3_dev, atoi(argv[3]));
        }
        else {
            _print_mac_set_usage();
            return -1;
        }
    }
    else {
        _print_mac_usage();
        return -1;
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "sys",   "Run RN2XX3 system commands",         rn2xx3_sys_cmd },
    { "mac",   "Run RN2XX3 LoRa mac layer commands", rn2xx3_mac_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RN2XX3 device driver test");

    rn2xx3_setup(&rn2xx3_dev, &rn2xx3_params[0]);
    if (rn2xx3_init(&rn2xx3_dev) != RN2XX3_OK) {
        puts("RN2XX3 initialization failed");
        return -1;
    }

    /* start the shell */
    puts("Initialization OK, starting shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
