/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "at.h"
#include "fmt.h"
#include "isrpipe.h"
#include "isrpipe/read_timeout.h"
#include "periph/uart.h"
#include "event/thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef AT_PRINT_INCOMING
#define AT_PRINT_INCOMING (0)
#endif

#if defined(MODULE_AT_URC_ISR_LOW)
#define AT_EVENT_PRIO EVENT_PRIO_LOW
#elif defined(MODULE_AT_URC_ISR_MEDIUM)
#define AT_EVENT_PRIO EVENT_PRIO_MEDIUM
#elif defined(MODULE_AT_URC_ISR_HIGH)
#define AT_EVENT_PRIO EVENT_PRIO_HIGH
#endif

#if defined(MODULE_AT_URC_ISR)
static void _event_process_urc(event_t *_event)
{
    at_dev_t *dev = (at_dev_t *)container_of(_event, at_dev_t, event);
    at_process_urc(dev, 1000);
}
#endif

static void _isrpipe_write_one_wrapper(void *_dev, uint8_t data)
{
    at_dev_t *dev = (at_dev_t *) _dev;
    isrpipe_write_one(&dev->isrpipe, data);
#if defined(MODULE_AT_URC_ISR)
    if (data == AT_RECV_EOL_2[0] && !dev->awaiting_response) {
        event_post(AT_EVENT_PRIO, &dev->event);
    }
#endif
}

int at_dev_init(at_dev_t *dev, uart_t uart, uint32_t baudrate, char *buf, size_t bufsize)
{
    dev->uart = uart;

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = false;
    dev->event.handler = _event_process_urc;
#endif

    isrpipe_init(&dev->isrpipe, (uint8_t *)buf, bufsize);

    return uart_init(uart, baudrate, _isrpipe_write_one_wrapper,
                     dev);
}

int at_expect_bytes(at_dev_t *dev, const char *bytes, uint32_t timeout)
{
    int res = 0;

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = true;
#endif

    while (*bytes) {
        char c;
        if ((res = isrpipe_read_timeout(&dev->isrpipe, (uint8_t *)&c, 1, timeout)) == 1) {
            if (AT_PRINT_INCOMING) {
                print(&c, 1);
            }
            if (c != *bytes++) {
                res = -1;
                goto out;
            }
        }
        else {
            goto out;
        }
    }
    res = 0;

out:
#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = false;
#endif

    return res;
}

void at_send_bytes(at_dev_t *dev, const char *bytes, size_t len)
{
    uart_write(dev->uart, (const uint8_t *)bytes, len);
}

ssize_t at_recv_bytes(at_dev_t *dev, char *bytes, size_t len, uint32_t timeout)
{
    char *resp_pos = bytes;

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = true;
#endif

    while (len) {
        int read_res;
        if ((read_res = isrpipe_read_timeout(&dev->isrpipe, (uint8_t *)resp_pos,
                                             1, timeout)) == 1) {
            resp_pos += read_res;
            len -= read_res;
        }
        else if (read_res == -ETIMEDOUT) {
            break;
        }
    }

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = false;
#endif

    return (resp_pos - bytes);
}

int at_recv_bytes_until_string(at_dev_t *dev, const char *string,
                               char *bytes, size_t *bytes_len, uint32_t timeout)
{
    size_t len = 0;
    char *_string = (char *)string;
    int res = 0;

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = true;
#endif

    while (*_string && len < *bytes_len) {
        char c;
        if ((res = isrpipe_read_timeout(&dev->isrpipe, (uint8_t *)&c, 1, timeout)) == 1) {
            if (AT_PRINT_INCOMING) {
                print(&c, 1);
            }
            if (c == *_string) {
                _string++;
            }
            bytes[len] = c;
            len++;
        }
        else {
            break;
        }
    }
    *bytes_len = len;

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = false;
#endif

    return res;
}

int at_send_cmd(at_dev_t *dev, const char *command, uint32_t timeout)
{
    size_t cmdlen = strlen(command);

    uart_write(dev->uart, (const uint8_t *)command, cmdlen);
    uart_write(dev->uart, (const uint8_t *)CONFIG_AT_SEND_EOL, AT_SEND_EOL_LEN);

    if (AT_SEND_ECHO) {
        if (at_expect_bytes(dev, command, timeout)) {
            return -1;
        }

        if (at_expect_bytes(dev, CONFIG_AT_SEND_EOL AT_RECV_EOL_1 AT_RECV_EOL_2, timeout)) {
            return -2;
        }
    }

    return 0;
}

void at_drain(at_dev_t *dev)
{
    uint8_t _tmp[16];
    int res;

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = true;
#endif

    do {
        /* consider no character within 10ms "drained" */
        res = isrpipe_read_timeout(&dev->isrpipe, _tmp, sizeof(_tmp), 10000U);
    } while (res > 0);

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = false;
#endif
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

    at_drain(dev);

    res = at_send_cmd(dev, command, timeout);
    if (res) {
        goto out;
    }

    /* only clear the response buffer after sending the command,
     * so the same buffer can be used for command and response */
    memset(resp_buf, '\0', len);

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
            size_t len_ok = sizeof(CONFIG_AT_RECV_OK) - 1;
            size_t len_error = sizeof(CONFIG_AT_RECV_ERROR) - 1;
            if (((size_t )res == (len_ok + keep_eol)) &&
                (len_ok != 0) &&
                (strncmp(pos, CONFIG_AT_RECV_OK, len_ok) == 0)) {
                res = len - bytes_left;
                break;
            }
            else if (((size_t )res == (len_error + keep_eol)) &&
                     (len_error != 0) &&
                     (strncmp(pos, CONFIG_AT_RECV_ERROR, len_error) == 0)) {
                return -1;
            }
            else if (strncmp(pos, "+CME ERROR:", 11) == 0) {
                return -2;
            }
            else if (strncmp(pos, "+CMS ERROR:", 11) == 0) {
                return -2;
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
    uart_write(dev->uart, (const uint8_t *)CONFIG_AT_SEND_EOL, AT_SEND_EOL_LEN);

    if (at_expect_bytes(dev, command, timeout)) {
        return -1;
    }

    if (at_expect_bytes(dev, CONFIG_AT_SEND_EOL AT_RECV_EOL_2, timeout)) {
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
        ssize_t len_ok = sizeof(CONFIG_AT_RECV_OK) - 1;
        if ((len_ok != 0) && (strcmp(resp_buf, CONFIG_AT_RECV_OK) == 0)) {
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

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = true;
#endif

    memset(resp_buf, 0, len);

    while (len) {
        int read_res;
        if ((read_res = isrpipe_read_timeout(&dev->isrpipe, (uint8_t *)resp_pos,
                                             1, timeout)) == 1) {
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
#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = false;
#endif

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
