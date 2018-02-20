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

#define ENABLE_DEBUG (1)
#include "debug.h"

#ifndef AT_PRINT_INCOMING
#define AT_PRINT_INCOMING (1)
#endif

int at_dev_init(at_dev_t *dev, uart_t uart, uint32_t baudrate, char *buf, size_t bufsize)
{
    dev->uart = uart;
    isrpipe_init(&dev->isrpipe, buf, bufsize);
    uart_init(uart, baudrate, (uart_rx_cb_t) isrpipe_write_one,
              &dev->isrpipe);

    return 0;
}

int at_expect_bytes(at_dev_t *dev, const char *bytes, uint32_t timeout)
{
    while (*bytes) {
        char c;
        int res;
        if ((res = isrpipe_read_timeout(&dev->isrpipe, &c, 1, timeout)) == 1) {
            if (AT_PRINT_INCOMING) {
                print(&c, 1);
            }
            if (c != *bytes++) {
                return -1;
            }
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
    uart_write(dev->uart, (const uint8_t *)AT_EOL, AT_EOL_LEN);

    if (at_expect_bytes(dev, command, timeout)) {
        return -1;
    }

    if (at_expect_bytes(dev, AT_EOL "\r\n", timeout)) {
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

ssize_t at_read_bytes(at_dev_t *dev, char *bytes, size_t len, uint32_t timeout)
{
    return isrpipe_read_timeout(&dev->isrpipe, bytes, len, timeout);
}

ssize_t at_send_cmd_get_resp(at_dev_t *dev, const char *command,
                             char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res = -1;

    at_drain(dev);

    res = at_send_cmd(dev, command, timeout);
    if (res) {
        goto out;
    }

    res = at_readline(dev, resp_buf, len, false, timeout);
    if (res == 0) {
        /* skip possible empty line */
        res = at_readline(dev, resp_buf, len, false, timeout);
    }

out:
    return res;
}

ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf,
                              size_t len, bool keep_eol, uint32_t timeout)
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

    while (1) {
        res = at_readline(dev, pos, bytes_left, keep_eol, timeout);
        if (res == 0) {
            if (bytes_left) {
                *pos++ = '\n';
                bytes_left--;
            }
            continue;
        }
        else if (res > 0) {
            bytes_left -= res;
            if ((res == (2 + (keep_eol ? 1 : 0))) && (strncmp(pos, "OK", 2) == 0)) {
                res = len - bytes_left;
                break;
            }
            else if ((res == (5 + (keep_eol ? 1 : 0))) && (strncmp(pos, "ERROR", 5) == 0)) {
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
    uart_write(dev->uart, (const uint8_t *)AT_EOL, AT_EOL_LEN);

    if (at_expect_bytes(dev, command, timeout)) {
        return -1;
    }

    if (at_expect_bytes(dev, AT_EOL "\n", timeout)) {
        return -2;
    }

    if (at_expect_bytes(dev, ">", timeout)) {
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

ssize_t at_readline(at_dev_t *dev, char *resp_buf, size_t len, bool keep_eol, uint32_t timeout)
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
                if (!keep_eol) {
                    continue;
                }
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

void at_add_oob(at_dev_t *dev, at_oob_t *oob)
{
    assert(oob);
    assert(oob->urc);
    assert(oob->cb);

    clist_rpush(&dev->oob_list, &oob->list_node);
}

void at_remove_oob(at_dev_t *dev, at_oob_t *oob)
{
    clist_remove(&dev->oob_list, &oob->list_node);
}

static int _check_oob(clist_node_t *node, void *arg)
{
    const char *buf = arg;
    at_oob_t *oob = container_of(node, at_oob_t, list_node);

    DEBUG("Trying to match with %s\n", oob->urc);

    if (strncmp(buf, oob->urc, strlen(oob->urc)) == 0) {
        oob->cb(oob->arg, buf);
        return 1;
    }

    return 0;
}

void at_process_oob(at_dev_t *dev)
{
    char buf[AT_BUF_SIZE];
    char *p;
    size_t read = 0;

    while (1) {
        int res = isrpipe_try_read(&dev->isrpipe, buf + read, sizeof(buf) - read);
        if (!res) {
            return;
        }
        if (AT_PRINT_INCOMING) {
            print(buf, res);
        }
        read += res;
        if (buf[read - 1] != '\n') {
            DEBUG("Not a full line\n");
            continue;
        }
        p = buf;
        while ((*p == '\r' || *p == '\n' || *p == ' ') && ((size_t)(p - buf) < read)) {
            p++;
        }
        clist_foreach(&dev->oob_list, _check_oob, p);
        return;
    }
}

void at_dev_poweron(at_dev_t *dev)
{
    uart_poweron(dev->uart);
}

void at_dev_poweroff(at_dev_t *dev)
{
    uart_poweroff(dev->uart);
}
