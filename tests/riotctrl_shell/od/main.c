/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Tests the output of the `od` module
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @note        IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
 *              YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
 *              See https://github.com/RIOT-OS/Release-Specs/
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "fmt.h"
#include "od.h"
#include "shell.h"

#define OD_BUF_SIZE     64U

static uint8_t _od_buf[OD_BUF_SIZE];
static uint8_t _od_buf_len = 0;

static int _fill_buf(int argc, char **argv)
{
    ssize_t data_len;

    if (argc < 2) {
        printf("usage: %s data_hex\n", argv[0]);
        return 1;
    }

    if ((_od_buf_len + (strlen(argv[1]) / 2)) > sizeof(_od_buf)) {
        printf("error: \"%s\" does not fit into remaining buffer " "(buffer size: %lu).\n", argv[1],
               (long unsigned)sizeof(_od_buf));
        return 1;
    }

    if ((data_len = scn_buf_hex(&_od_buf[_od_buf_len], sizeof(_od_buf) - _od_buf_len, argv[1],
                                strlen(argv[1]))) < 0) {
        printf("error: Cannot parse \"%s\" as hex string.\n", argv[1]);
        return 1;
    }
    _od_buf_len += data_len;

    return 0;
}

static int _reset_buf(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    _od_buf_len = 0U;

    return 0;
}

static int _od(int argc, char **argv)
{
    uint32_t offset = 0;
    uint8_t width = OD_WIDTH_DEFAULT;

    if (argc > 1) {
        char *endptr;

        errno = 0;
        offset = strtol(argv[1], &endptr, 10);
        if ((argv[1] == endptr) || (errno == ERANGE)) {
            printf("usage: %s [offset [width]]\n", argv[0]);
            return 1;
        }
    }

    if (argc > 2) {
        char *endptr;
        uint32_t tmp;

        errno = 0;
        tmp = strtol(argv[2], &endptr, 10);
        if ((argv[2] == endptr) || (errno == ERANGE) || (tmp == 0) || (tmp > UINT8_MAX)) {
            printf("usage: %s [offset [width]]\n", argv[0]);
            return 1;
        }
        width = (uint8_t)tmp;
    }

    if (offset == 0) {
        /* Yes, od_hex_dump() aliases to od_hex_dump_ext(), but let's still
         * test this call in case this changes later on. */
        od_hex_dump(_od_buf, _od_buf_len, width);
    }
    else {
        od_hex_dump_ext(_od_buf, _od_buf_len, width, offset);
    }

    return 0;
}

SHELL_COMMAND(fillbuf, "Fill the buffer to object dump", _fill_buf);
SHELL_COMMAND(resetbuf, "Reset the buffer to object dump", _reset_buf);
SHELL_COMMAND(od, "Object dump buffer", _od);

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    puts("Starting riotctrl_shell.od test");

    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
