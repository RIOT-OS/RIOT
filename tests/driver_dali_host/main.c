/*
 * Copyright (C) 2021 Mesotic SAS
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
 * @brief       Test application for DALI device drivers
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "shell_commands.h"
#include "shell.h"
#include "dali.h"
#include "dali_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef SHELL_BUFSIZE
#define SHELL_BUFSIZE       (128U)
#endif

#define CONVERT_ERROR   (-32768)
#define DALI_CMD    0x90

static dali_host_t dev;

static inline int _get_num(const char *str)
{
    errno = 0;
    char *temp;
    long val = strtol(str, &temp, 0);

    if (temp == str || *temp != '\0' ||
        ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE)) {
        val = CONVERT_ERROR;
    }
    return (int)val;
}

static int cmd_send(int argc, char **argv)
{
    uint8_t addr, cmd;
    if (argc < 3) {
        puts("send: missing argument");
        return -1;
    }
    addr = _get_num(argv[1]);
    cmd = _get_num(argv[2]);

    printf("sending cmd:0x%x to 0x%x\n", cmd, addr);
    dali_host_xfer(&dev, addr, cmd, NULL);

    return 0;
}

static int cmd_conf(int argc, char **argv)
{
    uint8_t addr, cmd;
    if (argc < 3) {
        puts("send: missing argument");
        return -1;
    }
    addr = _get_num(argv[1]);
    cmd = _get_num(argv[2]);

    printf("sending cmd:0x%x to 0x%x\n", cmd, addr);
    dali_host_xfer(&dev, addr, cmd, NULL);
    ztimer_sleep(ZTIMER_MSEC, 30);
    dali_host_xfer(&dev, addr, cmd, NULL);

    return 0;
}

static int cmd_send_bcast(int argc, char **argv)
{
    uint8_t cmd;
    if (argc < 2) {
        puts("sendall: missing argument");
        return -1;
    }
    cmd = _get_num(argv[1]);
    for (unsigned i=0; i<64; i++) {
        printf("sending cmd:0x%x to 0x%x\n", cmd, i);
        dali_host_xfer(&dev, (i << 1) | 1, cmd, NULL);
        ztimer_sleep(ZTIMER_MSEC, 25);
    }
    return 0;
}

static int cmd_xfer(int argc, char **argv)
{
    uint8_t addr, cmd;
    uint8_t data;
    int ret;
    if (argc < 3) {
        puts("xfer: missing argument");
        return -1;
    }
    addr = _get_num(argv[1]);
    cmd = _get_num(argv[2]);

    printf("sending cmd:0x%x to 0x%x\n", cmd, addr);
    ret = dali_host_xfer(&dev, addr, cmd, &data);
    if (ret != 0) {
        printf("error on reception:%d\n", ret);
        return -1;
    }

    printf("device answered: 0x%x\n", data);

    return 0;
}

static int cmd_detect(int argc, char **argv)
{
    uint8_t data;
    (void)data;
    (void)argc;
    (void)argv;
    puts("Scanning for DALI devices...");

    puts(
        "addr not ack'ed = \"-\", addr ack'ed = \"X\",  error = \"E\"\n"
        "     0 1 2 3 4 5 6 7 8 9 a b c d e f"
    );

    for (char i = 0; i < 4; i++) {
        char row[] = { '0', 'x', '0' + i, '0', '\0' };
        fputs(row, stdout);
        uint8_t addr = i;
        addr <<= 4;
        for (unsigned j = 0; j < 16; j++) {
            char str[] = { ' ', '-', '\0' };
            uint8_t data;
            int ret;
            ret = dali_host_xfer(&dev, (addr << 1) | 1, DALI_CMD, &data);

            switch (ret) {
                case 0:
                    /* success: Device did respond */
                    str[1] = 'X';
                    break;
                case -ENXIO:
                    /* No ACK --> no device */
                    break;
                default:
                    /* Some unexpected error */
                    str[1] = 'E';
                    break;
            }

            fputs(str, stdout);
            addr++;
        }
        puts("");
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "send", "send a command to a device", cmd_send },
    { "conf", "send a configuration command (twice a command"\
      "under 100 ms)  to a device", cmd_conf },
    { "send_bcast", "send a command to 64 devices", cmd_send_bcast},
    { "xfer", "send a command and wait for response", cmd_xfer },
    { "detect", "query available devices on the bus", cmd_detect},
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nDALI host driver test application");

    memset(&dev, 0, sizeof(dali_host_t));
    dali_host_init(&dev, &dali_config[0]);

    /* run the shell */
    char line_buf[SHELL_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_BUFSIZE);
    return 0;
}
