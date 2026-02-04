/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the ATA8520E Sigfox module
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "shell.h"

#include "ata8520e_params.h"
#include "ata8520e.h"

static ata8520e_t dev;

static void _print_sigfox_usage(void)
{
    puts("Usage: sigfox <info|keys|tx|tx_rx>\n\n"
         " <info> prints information on module versions\n"
         " <keys> prints device internal keys, use them to register the device"
         " at https://backend.sigfox.com/activate\n"
         " <tx> sends a frame or a bit to the Sigfox backend\n"
         " <tx_rx> sends a frame to the Sigfox backend and waits for received "
         "data");
}

int ata8520e_sigfox_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_sigfox_usage();
        return 1;
    }

    if (strcmp(argv[1], "info") == 0) {
        uint8_t atmel_version[2];
        char sigfox_version[11];
        ata8520e_read_atmel_version(&dev, atmel_version);
        ata8520e_read_sigfox_version(&dev, sigfox_version);
        printf("Atmel version : %d.%d\n", atmel_version[0], atmel_version[1]);
        printf("Sigfox version: %s\n", sigfox_version);
    }
    else if (strcmp(argv[1], "keys") == 0) {
        char sigfox_id[SIGFOX_ID_LENGTH + 1];
        ata8520e_read_id(&dev, sigfox_id);
        char sigfox_pac[SIGFOX_PAC_LENGTH + 1];
        ata8520e_read_pac(&dev, sigfox_pac);
        printf("ID: %s\nPAC: %s\n", sigfox_id, sigfox_pac);
    }
    else if (strcmp(argv[1], "tx") == 0) {
        if (argc != 3) {
            puts("Usage: sigfox tx <payload>");
            return 1;
        }
        if ((strlen(argv[2]) == 1) &&
            ((atoi(argv[2]) == 0) || (atoi(argv[2]) == 1))) {
            bool bit = (atoi(argv[2]) == 1);
            if (ata8520e_send_bit(&dev, bit) != ATA8520E_OK) {
                puts("Bit not sent");
                return 1;
            }
            puts("Bit sent with success");
        }
        else {
            if (strlen(argv[2]) > SIGFOX_MAX_TX_LENGTH) {
                printf("Message length cannot exceeds %d characters length, your "
                       "message length is %" PRIuSIZE, SIGFOX_MAX_TX_LENGTH, strlen(argv[2]));
                return 1;
            }
            if (ata8520e_send_frame(&dev,
                                    (uint8_t *)argv[2],
                                    strlen(argv[2])) != ATA8520E_OK) {
                puts("Message not sent");
                return 1;
            }
            puts("Message sent with success");
        }
    }
    else if (strcmp(argv[1], "tx_rx") == 0) {
        if (argc != 3) {
            puts("Usage: sigfox tx_rx <payload>");
            return 1;
        }
        if (strlen(argv[2]) > SIGFOX_MAX_TX_LENGTH) {
            printf("Message length cannot exceeds %d characters length, your "
                   "message length is %" PRIuSIZE, SIGFOX_MAX_TX_LENGTH, strlen(argv[2]));
            return 1;
        }
        uint8_t rx_buf[SIGFOX_RX_LENGTH];
        if (ata8520e_send_receive_frame(&dev,
                                        (uint8_t *)argv[2],
                                        strlen(argv[2]),
                                        rx_buf) != ATA8520E_OK) {
            puts("Message not sent");
            return 1;
        }
        puts("Message sent with success");
        printf("Data received: %s\n", (char *)rx_buf);
    }
    else {
        _print_sigfox_usage();
        return 1;
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "sigfox", "interact with ATA8520E Sigfox module", ata8520e_sigfox_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("ATA8520E test application\n");
    puts("+------------Initializing------------+\n");
    switch (ata8520e_init(&dev, &ata8520e_params[0])) {
        case -ATA8520E_ERR_SPI:
            puts("[Error] An error occurred when initializing SPI bus.");
            return -1;
        case -ATA8520E_ERR_GPIO_INT:
            puts("[Error] An error occurred when initializing interrupt pin.");
            return -1;
        case -ATA8520E_ERR_GPIO_POWER:
            puts("[Error] An error occurred when initializing poweron pin.");
            return -1;
        case -ATA8520E_ERR_GPIO_RESET:
            puts("[Error] An error occurred when initializing reset- pin.");
            return -1;
    }

    /* start the shell */
    puts("Initialization OK, starting shell now");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
