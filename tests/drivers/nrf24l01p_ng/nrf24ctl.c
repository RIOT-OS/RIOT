/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tool to configure NRF24L01+ (NG) transceiver
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "nrf24l01p_ng.h"
#include "nrf24l01p_ng_params.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS    (0)
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE    (1)
#endif

#define MIN_ARGC    (2)

extern nrf24l01p_ng_t _nrf24l01p_ng_devs[NRF24L01P_NG_NUM];

static void print_help(void)
{
    puts(
    "Usage:\n"

    "nrf24ctl <device index> "
    "--set "
    "<address | pipe> <value> <pipe index>\n"

    "nrf24ctl <device index> "
    "--set "
    "<channel | crc | data_rate | power | retr_delay | state | max_retr> "
    "<value>\n"

    "nrf24ctl <device index> "
    "--get "
    "<address | pipe> <pipe index>\n"

    "nrf24ctl <device index> "
    "--get "
    "<pipe | channel | crc | data_rate | power | retr_delay | stata | "
    "max_retr>\n"

    "nrf24ctl <device index>" " --regs\n"
    "nrf24ctl <device index>" " --info\n"
    );
}

int sc_nrf24ctl(int argc, char *argv[])
{
    if (argc < MIN_ARGC) {
        printf("[nrf24ctl] Expect at least %d arguments\n", MIN_ARGC);
        goto PRINT_HELP_EXIT;
    }
    int dev_index = atoi(argv[1]);
    if (dev_index < 0 || dev_index >= (int)NRF24L01P_NG_NUM) {
        printf("[nrf24ctl] Invalid device index: %d\n", dev_index);
        goto PRINT_HELP_EXIT;
    }
    nrf24l01p_ng_t *dev = &_nrf24l01p_ng_devs[dev_index];
    int ret = 0;
    switch (argc) {
        case 6: {
            /* nrf24ctl <dev_index> --set <attribute> <value> <pipe> */
            int pipe = atoi(argv[5]);
            if (pipe < 0 || pipe >= NRF24L01P_NG_PX_NUM_OF) {
                printf("[nrf24ctl] bad pipe index\n");
                goto PRINT_HELP_EXIT;
            }
            if ((!strcmp(argv[2], "-s")) || (!strcmp(argv[2], "--set"))) {
                if (!(strcmp(argv[3], "address"))) {
                    uint8_t address[NRF24L01P_NG_MAX_ADDR_WIDTH];
                    char *tok = NULL;
                    unsigned a = 0;
                    while (a < sizeof(address) &&
                           (tok = strtok(a ? NULL : argv[4], ":"))) {
                        if (strlen(tok) > 2) {
                            printf("[nrf24ctl] Invalid address token: %s\n",
                                   tok);
                            goto PRINT_HELP_EXIT;
                        }
                        address[a++] = (int)strtol(tok, NULL, 16);
                    }
                    ret = nrf24l01p_ng_set_rx_address(dev, address, pipe);
                    if (ret < 0) {
                        printf("[nrf24ctl] Could not set address (%d)\n", ret);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else if (!(strcmp(argv[3], "pipe"))) {
                    bool enable = !!(atoi(argv[4]));
                    ret = nrf24l01p_ng_set_enable_pipe(dev, pipe, enable);
                    if (ret < 0) {
                        printf("[nrf24ctl] "
                               "Could not enable/disable pipe %d\n", pipe);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else {
                    printf("[nrf24ctl] Invalid attribute: --set %s\n",
                           argv[3]);
                    goto PRINT_HELP_EXIT;
                }
            }
            else {
                printf("[nrf24ctl] Invalid command: %s\n", argv[2]);
                goto PRINT_HELP_EXIT;
            }
        } break;
        case 5: {
            /* nrf24ctl <dev_index> --set <attribute> <value> */
            if ((!strcmp(argv[2], "-s")) || (!strcmp(argv[2], "--set"))) {
                if (!(strcmp(argv[3], "channel"))) {
                    int ch = atoi(argv[4]);
                    ret = nrf24l01p_ng_set_channel(dev, ch);
                    if (ret < 0) {
                        printf("[nrf24ctl] Could not switch to channel (%d)\n",
                               ret);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else if (!(strcmp(argv[3], "crc"))) {
                    int crc_len = atoi(argv[4]);
                    nrf24l01p_ng_crc_t crco = nrf24l01p_ng_valtoe_crc(crc_len);
                    ret = nrf24l01p_ng_set_crc(dev, crco);
                    if (ret < 0) {
                        printf("[nrf24ctl] Could not set CRC length (%d)\n",
                               ret);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else if (!(strcmp(argv[3], "data_rate"))) {
                    int dr = atoi(argv[4]);
                    nrf24l01p_ng_rfdr_t adr = nrf24l01p_ng_valtoe_rfdr(dr);
                    ret = nrf24l01p_ng_set_air_data_rate(dev, adr);
                    if (ret < 0) {
                        printf("[nrf24ctl] Could not set air data rate (%d)\n",
                               ret);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else if (!(strcmp(argv[3], "power"))) {
                    int pwr = atoi(argv[4]);
                    nrf24l01p_ng_tx_power_t txp =
                        nrf24l01p_ng_valtoe_tx_power(pwr);
                    ret = nrf24l01p_ng_set_tx_power(dev, txp);
                    if (ret < 0) {
                        printf("[nrf24ctl] Could not set Tx power (%d)\n",
                               ret);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else if (!(strcmp(argv[3], "retr_delay"))) {
                    int del = atoi(argv[4]);
                    nrf24l01p_ng_ard_t ard = nrf24l01p_ng_valtoe_ard(del);
                    ret = nrf24l01p_ng_set_retransm_delay(dev, ard);
                    if (ret < 0) {
                        printf("[nrf24ctl] "
                               "Could not set retransmission delay (%d)\n",
                               ret);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else if (!(strcmp(argv[3], "state"))) {
                    nrf24l01p_ng_state_t s =
                        nrf24l01p_ng_diagnostics_string_to_state(argv[4]);
                    ret = nrf24l01p_ng_set_state(dev, s);
                    if (ret < 0) {
                        printf("[nrf24l01p] "
                               "Could not change device state (%d)",
                               ret);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else if (!(strcmp(argv[3], "max_retr"))) {
                    int retr = atoi(argv[4]);
                    ret = nrf24l01p_ng_set_max_retransm(dev, retr);
                    if (ret < 0) {
                        printf("[nrf24ctl] "
                               "Could not set max. retransmissions (%d)\n",
                               ret);
                        goto PRINT_HELP_EXIT;
                    }
                }
                else {
                    printf("[nrf24ctl] Invalid attribute: --set %s\n",
                           argv[3]);
                    goto PRINT_HELP_EXIT;
                }
            }
            /* nrf24ctl <dev_index> --get <attribute> <pipe> */
            else if ((!strcmp(argv[2], "-g")) ||
                     (!strcmp(argv[2], "--get"))) {
                int pipe = atoi(argv[4]);
                if (pipe < 0 || pipe >= NRF24L01P_NG_PX_NUM_OF) {
                    printf("[nrf24ctl] bad pipe index\n");
                    goto PRINT_HELP_EXIT;
                }
                if (!(strcmp(argv[3], "address"))) {
                    uint8_t address[2 * NRF24L01P_NG_MAX_ADDR_WIDTH + 5];
                    address[sizeof(address) - 1] = '\0';
                    ret = nrf24l01p_ng_get_rx_address(dev, address, pipe);
                    uint8_t i;
                    for (i = sizeof(address) - 2; ret > 0;) {
                        char hx[3] = { '0', '0', 0 };
                        sprintf(hx, "%2x", address[--ret]);
                        address[i--] = hx[1];
                        address[i--] = hx[0];
                        if (ret) {
                            address[i--] = ':';
                        }
                    }
                    printf("%s\n", address + (i += 1));
                }
                else if (!(strcmp(argv[3], "pipe"))) {
                    bool enable = false;
                    nrf24l01p_ng_get_enable_pipe(dev, pipe, &enable);
                    printf("nrf24l01p device %d - pipe %d: %s\n",
                           dev_index, pipe,
                           (enable ? "enabled" : "disabled"));
                }
                else {
                    printf("[nrf24ctl] Invalid attribute: --get %s\n",
                           argv[3]);
                    goto PRINT_HELP_EXIT;
                }
            }
            else {
                printf("[nrf24ctl] Invalid command: %s\n", argv[2]);
                goto PRINT_HELP_EXIT;
            }
        } break;
        case 4: {
            /* nrf24ctl <dev_index> --get <attribute> */
            if ((!strcmp(argv[2], "-g")) || (!strcmp(argv[2], "--get"))) {
                if (!(strcmp(argv[3], "channel"))) {
                    uint8_t ch = nrf24l01p_ng_get_channel(dev);
                    printf("nrf24l01p device %d - Channel: %u (%d MHz)\n",
                           dev_index, ch, NRF24L01P_NG_BASE_FRQ_MHZ + ch);
                }
                else if (!(strcmp(argv[3], "crc"))) {
                    uint8_t crc_len = nrf24l01p_ng_get_crc(dev, NULL);
                    printf("nrf24l01p device %d - "
                           "CRC length: %u [bytes]\n",
                           dev_index,  crc_len);
                }
                else if (!(strcmp(argv[3], "data_rate"))) {
                    uint16_t data_rate =
                        nrf24l01p_ng_get_air_data_rate(dev, NULL);
                    printf("nrf24l01p device %d - "
                           "Air data rate: %u [kbit/s]\n",
                           dev_index, data_rate);
                }
                else if (!(strcmp(argv[3], "power"))) {
                    int8_t power = nrf24l01p_ng_get_tx_power(dev, NULL);
                    printf("nrf24l01p device %d - "
                           "Tx Power: %d [dbm]\n",
                           dev_index, power);
                }
                else if (!(strcmp(argv[3], "retr_delay"))) {
                    uint16_t delay =
                        nrf24l01p_ng_get_retransm_delay(dev, NULL);
                    printf(
                        "nrf24l01p device %d - "
                        "Retransmission delay: %u [us]\n",
                        dev_index, delay);
                }
                else if (!(strcmp(argv[3], "state"))) {
                    const char *sstate =
                        nrf24l01p_ng_diagnostics_state_to_string(dev->state);
                    printf("nrf24l01p device %d - State: %s\n", dev_index,
                           sstate);
                }
                else if (!(strcmp(argv[3], "max_retr"))) {
                    uint8_t retransm = nrf24l01p_ng_get_max_retransm(dev);
                    printf("nrf24l01p device %d - "
                           "Maximum retransmission: %u\n",
                           dev_index, retransm);
                }
                else {
                    printf("[nrf24ctl] Invalid attribute: --get %s\n",
                           argv[3]);
                    goto PRINT_HELP_EXIT;
                }
            }
            else {
                printf("[nrf24ctl] Invalid command: %s\n", argv[2]);
                goto PRINT_HELP_EXIT;
            }
        } break;
        case 3: {
            /* nrf24ctl <dev_index> --regs */
            if ((!strcmp(argv[2], "-r")) || (!strcmp(argv[2], "--regs"))) {
                nrf24l01p_ng_print_all_regs(dev);
            }
            /* nrf24ctl <dev_index> --info */
            else if ((!strcmp(argv[2], "-i")) ||
                     (!strcmp(argv[2], "--info"))) {
                nrf24l01p_ng_print_dev_info(dev);
            }
            else {
                printf("[nrf24ctl] Invalid command: %s\n", argv[2]);
                goto PRINT_HELP_EXIT;
            }
        } break;
        default:
PRINT_HELP_EXIT:
            print_help();
            return EXIT_FAILURE;
    }
    puts("[nrf24ctl] Success");
    return EXIT_SUCCESS;
}
