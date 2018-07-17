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
    size_t cmdlen = strlen(command);

    uart_write(dev->uart, (const uint8_t *)command, cmdlen);
    uart_write(dev->uart, (const uint8_t *)AT_SEND_EOL, AT_SEND_EOL_LEN);

    if (AT_SEND_ECHO) {
        if (at_expect_bytes(dev, command, timeout)) {
            return -1;
        }

        if (at_expect_bytes(dev, AT_SEND_EOL AT_RECV_EOL_1 AT_RECV_EOL_2, timeout)) {
            return -2;
        }
    }

    return 0;
}

void at_drain(at_dev_t *dev)
{
    char _tmp[16];
    int res;

    do {
        /* consider no character within 10ms "drained" */
        res = isrpipe_read_timeout(&dev->isrpipe, _tmp, sizeof(_tmp), 10000U);
    } while (res > 0);
}

ssize_t at_send_cmd_get_resp(at_dev_t *dev, const char *command,
                             char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res;

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

ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command,
                              char *resp_buf, size_t len, bool keep_eol, uint32_t timeout)
{
    const char eol[] = AT_RECV_EOL_1 AT_RECV_EOL_2;
    assert(sizeof(eol) > 1);

    ssize_t res;
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
                *pos++ = eol[sizeof(eol) - 2];
                bytes_left--;
            }
            continue;
        }
        else if (res > 0) {
            bytes_left -= res;
            size_t len_ok = sizeof(AT_RECV_OK) - 1;
            size_t len_error = sizeof(AT_RECV_ERROR) - 1;
            if (((size_t )res == (len_ok + keep_eol)) &&
                (len_ok != 0) &&
                (strncmp(pos, AT_RECV_OK, len_ok) == 0)) {
                res = len - bytes_left;
                break;
            }
            else if (((size_t )res == (len_error + keep_eol)) &&
                     (len_error != 0) &&
                     (strncmp(pos, AT_RECV_ERROR, len_error) == 0)) {
                return -1;
            }
            else if (strncmp(pos, "+CME ERROR:", 11) == 0) {
                return -1;
            }
            else if (strncmp(pos, "+CMS ERROR:", 11) == 0) {
                return -1;
            }
            else {
                pos += res;
                if (bytes_left) {
                    *pos++ = eol[sizeof(eol) - 2];
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
    uart_write(dev->uart, (const uint8_t *)AT_SEND_EOL, AT_SEND_EOL_LEN);

    if (at_expect_bytes(dev, command, timeout)) {
        return -1;
    }

    if (at_expect_bytes(dev, AT_SEND_EOL AT_RECV_EOL_2, timeout)) {
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
        ssize_t len_ok = sizeof(AT_RECV_OK) - 1;
        if ((len_ok != 0) && (strcmp(resp_buf, AT_RECV_OK) == 0)) {
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
    const char eol[] = AT_RECV_EOL_1 AT_RECV_EOL_2;
    assert(sizeof(eol) > 1);

    ssize_t res = -1;
    char *resp_pos = resp_buf;

    memset(resp_buf, 0, len);

    while (len) {
        int read_res;
        if ((read_res = isrpipe_read_timeout(&dev->isrpipe, resp_pos, 1, timeout)) == 1) {
            if (AT_PRINT_INCOMING) {
                print(resp_pos, read_res);
            }
            if (sizeof(eol) > 2 && *resp_pos == eol[0]) {
                if (!keep_eol) {
                    continue;
                }
            }
            if (*resp_pos == eol[sizeof(eol) - 2]) {
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

#ifdef MODULE_AT_URC
void at_add_urc(at_dev_t *dev, at_urc_t *urc)
{
    assert(urc);
    assert(urc->code);
    assert(strlen(urc->code) != 0);
    assert(urc->cb);

    clist_rpush(&dev->urc_list, &urc->list_node);
}

void at_remove_urc(at_dev_t *dev, at_urc_t *urc)
{
    clist_remove(&dev->urc_list, &urc->list_node);
}

static int _check_urc(clist_node_t *node, void *arg)
{
    const char *buf = arg;
    at_urc_t *urc = container_of(node, at_urc_t, list_node);

    DEBUG("Trying to match with %s\n", urc->code);

    if (strncmp(buf, urc->code, strlen(urc->code)) == 0) {
        urc->cb(urc->arg, buf);
        return 1;
    }

    return 0;
}

void at_process_urc(at_dev_t *dev, uint32_t timeout)
{
    char buf[AT_BUF_SIZE];

    DEBUG("Processing URC (timeout=%" PRIu32 "us)\n", timeout);

    ssize_t res;
    /* keep reading while received data are shorter than EOL */
    while ((res = at_readline(dev, buf, sizeof(buf), true, timeout)) <
           (ssize_t)sizeof(AT_RECV_EOL_1 AT_RECV_EOL_2) - 1) {
        if (res < 0) {
            return;
        }
    }
    clist_foreach(&dev->urc_list, _check_urc, buf);
}
#endif

void at_dev_poweron(at_dev_t *dev)
{
    uart_poweron(dev->uart);
}

void at_dev_poweroff(at_dev_t *dev)
{
    uart_poweroff(dev->uart);
}
