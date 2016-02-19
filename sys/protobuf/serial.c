/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 *
 * @addtogroup      <module_name>
 * @{
 *
 * @file
 * @brief           
 *
 * @author          Attilio Dona'
 */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "protoreg.h"

#define MAX_BIN_SIZE 512

#define IO_ERROR       -1
#define IO_BUFF_OVFLOW -2

static uint8_t _buff[MAX_BIN_SIZE];
#if 0
#ifdef MODULE_NEWLIB
/* use local copy of putchar, as it seems to be inlined,
 * enlarging code by 50% */
static void _putchar(int c) {
    putchar(c);
}
#else
#define _putchar putchar
#endif

#endif

#if 1
static proto_msg_t readbinary(uint8_t *buf, size_t *size)
{
    uint8_t *line_buf_ptr = buf;
    uint8_t start = 1;
    int8_t binlen = 0;
    proto_msg_t message = M_PROTO_START;
    int c;

    while (1) {
        if ((line_buf_ptr - buf) >= MAX_BIN_SIZE) {
            return IO_BUFF_OVFLOW;
        }

        c = getchar();
        if (c < 0) {
            return IO_ERROR;
        }
        //_putchar(c);

        if (start && c == 0x1E) {
            // RS found, start of block
            start = 0;

            message = getchar();
            if (message < 0)
                return IO_ERROR;

            binlen = getchar();
            if (binlen < 0)
                return IO_ERROR;

            continue;
        }

        if (!start) {
            *line_buf_ptr++ = c;
            if ((line_buf_ptr - buf) == binlen) {
                *size = binlen;
                return message;
            }
        }
    }
    return M_PROTO_START;
}
#endif

#if 0
static proto_msg_t readbinary(uint8_t *buf, size_t *size)
{
    uint8_t *line_buf_ptr = buf;
    uint binlen = 4;
    proto_msg_t message = M_PROTO_START;

    while (1) {
        if ((line_buf_ptr - buf) >= MAX_BIN_SIZE) {
            return IO_BUFF_OVFLOW;
        }

        int c = getchar();
        if (c < 0) {
            return IO_ERROR;
        }
        //_putchar(c);

        *line_buf_ptr++ = c;
        if ((line_buf_ptr - buf) == binlen) {
            *size = binlen;
            return message;
        }
    }
    return M_PROTO_START;
}
#endif


void proto_shell_run(void)
{
    size_t len = 0;
    proto_msg_t ptype = M_PROTO_START;

    while (1) {
        ptype = readbinary(_buff, &len);

        if (ptype != M_PROTO_START) {
            submit(ptype, _buff, len);
        }
    }
}
