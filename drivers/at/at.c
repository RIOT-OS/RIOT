/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include <string.h>

#include "at.h"
#include "fmt.h"
#include "isrpipe.h"
#include "periph/uart.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef AT_PRINT_INCOMING
#define AT_PRINT_INCOMING (0)
#endif

int at_dev_init(at_dev_t *dev, uart_t uart, uint32_t baudrate, char *buf, size_t bufsize)
{
    dev->uart = uart;
    isrpipe_init(&dev->isrpipe, buf, bufsize);
    uart_init(uart, baudrate, (uart_rx_cb_t) isrpipe_write_one,
              &dev->isrpipe);

    return 0;
}

int at_expect_bytes(at_dev_t *dev, const char *bytes, size_t len, uint32_t timeout)
{
    while (len) {
        char c;
        int res;
        if ((res = isrpipe_read_timeout(&dev->isrpipe, &c, 1, timeout)) == 1) {
            if (AT_PRINT_INCOMING) {
                print(&c, 1);
            }
            if (!(c == *bytes++)) {
                return -1;
            }
            len--;
        }
        else {
            return res;
        }
    }

    return 0;
}

void at_send_bytes(at_dev_t *dev, const char *bytes, size_t len)
{
    uart_write(dev->uart, (const uint8_t *)bytes, len);
}

int at_send_cmd(at_dev_t *dev, const char *command, uint32_t timeout)
{
    unsigned cmdlen = strlen(command);

    uart_write(dev->uart, (const uint8_t *)command, cmdlen);
    uart_write(dev->uart, (const uint8_t *)AT_END_OF_LINE, sizeof(AT_END_OF_LINE) - 1);

    if (at_expect_bytes(dev, command, cmdlen, timeout)) {
        return -1;
    }

    if (at_expect_bytes(dev, AT_END_OF_LINE "\r\n", sizeof(AT_END_OF_LINE) + 1, timeout)) {
        return -2;
    }

    return 0;
}

void at_drain(at_dev_t *dev)
{
    char _tmp[16];
    int res;

    do {
        res = isrpipe_read_timeout(&dev->isrpipe, _tmp, sizeof(_tmp), 10000U);
    } while (res > 0);
}

ssize_t at_send_cmd_get_resp(at_dev_t *dev, const char *command, char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res = -1;

    at_drain(dev);

    res = at_send_cmd(dev, command, timeout);
    if (res) {
        goto out;
    }

    res = at_readline(dev, resp_buf, len, timeout);
    if (res == 0) {
        /* skip possible empty line */
        res = at_readline(dev, resp_buf, len, timeout);
    }

out:
    return res;
}

ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res = -1;
    size_t bytes_left = len - 1;
    char *pos = resp_buf;

    memset(resp_buf, '\0', len);

    at_drain(dev);

    res = at_send_cmd(dev, command, timeout);
    if (res) {
        goto out;
    }

    while(1) {
        res = at_readline(dev, pos, bytes_left, timeout);
        if (res == 0) {
            continue;
        }
        else if (res > 0) {
            bytes_left -= res;
            if ((res == 2) && (strncmp(pos, "OK", 2) == 0)) {
                res = len - bytes_left;
                break;
            }
            else if ((res == 5) && (strncmp(pos, "ERROR", 5) == 0)) {
                return -1;
            }
            else if (strncmp(pos, "+CME ERROR:", 11) == 0) {
                return -1;
            }
            else {
                pos += res;
                if (bytes_left) {
                    *pos++ = '\n';
                    bytes_left--;
                }
                else {
                    return -1;
                }
            }
        }
        else {
            break;
        }
    }

out:
    return res;
}

int at_send_cmd_wait_prompt(at_dev_t *dev, const char *command, uint32_t timeout)
{
    unsigned cmdlen = strlen(command);

    at_drain(dev);

    uart_write(dev->uart, (const uint8_t *)command, cmdlen);
    uart_write(dev->uart, (const uint8_t *)AT_END_OF_LINE, sizeof(AT_END_OF_LINE) - 1);

    if (at_expect_bytes(dev, command, cmdlen, timeout)) {
        return -1;
    }

    if (at_expect_bytes(dev, AT_END_OF_LINE "\n", sizeof(AT_END_OF_LINE), timeout)) {
        return -2;
    }

    if (at_expect_bytes(dev, ">", 1, timeout)) {
        return -3;
    }

    return 0;
}

int at_send_cmd_wait_ok(at_dev_t *dev, const char *command, uint32_t timeout)
{
    int res;
    char resp_buf[64];

    res = at_send_cmd_get_resp(dev, command, resp_buf, sizeof(resp_buf), timeout);
    if (res > 0) {
        if (strcmp(resp_buf, "OK") == 0) {
            res = 0;
        }
        else {
            res = -1;
        }
    }

    return res;
}

ssize_t at_readline(at_dev_t *dev, char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res = -1;
    char *resp_pos = resp_buf;

    memset(resp_buf, 0, len);

    while (len) {
        int read_res;
        if ((read_res = isrpipe_read_timeout(&dev->isrpipe, resp_pos, 1, timeout)) == 1) {
            if (AT_PRINT_INCOMING) {
                print(resp_pos, read_res);
            }
            if (*resp_pos == '\r') {
                continue;
            }
            if (*resp_pos == '\n') {
                *resp_pos = '\0';
                res = resp_pos - resp_buf;
                goto out;
            }

            resp_pos += read_res;
            len -= read_res;
        }
        else if (read_res == -ETIMEDOUT) {
            res = -ETIMEDOUT;
            break;
        }
    }

out:
    if (res < 0) {
        *resp_buf = '\0';
    }
    return res;
}
