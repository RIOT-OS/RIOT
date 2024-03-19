/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* A note on URCs (Unsolicited Result Codes), regardless of whether URC handling
 * is enabled or not.
 *
 * Some DCEs (Data Circuit-terminating Equipment, aka modem), like the LTE
 * modules from uBlox define a grace period where URCs are guaranteed NOT to be
 * sent as the time span between:
 *  - the command EOL character reception AND command being internally accepted
 *  - the EOL character of the last response line
 *
 * As follows, there is an indeterminate amount of time between:
 *  - the command EOL character being sent
 *  - the command EOL character reception AND command being internally accepted,
 *    i.e. the begin of the grace period
 *
 * In other words, we can get a URC (or more?) just after issuing the command
 * and before the first line of response. The net effect is that such URCs will
 * appear to be the first line of response to the last issued command.
 *
 * The current solution is to skip characters that don't match the expected
 * response, at the expense of losing these URCs. Note, we may already lose URCs
 * when calling at_drain() just before any at_send_cmd(). Success partially
 * depends on whether command echoing is enabled or not:
 *  1. echo enabled: by observation, it seems that the grace period begins
 *   BEFORE the echoed command. This has the advantage that we ALWAYS know what
 *   the first line of response must look like and so if it doesn't, then it's a
 *   URC. Thus, any procedure that calls at_send_cmd() will catch and discard
 *   these URCs.
 *  2. echo disabled: commands that expect a response (e.g. at_send_cmd_get_resp_wait_ok())
 *   will catch and discard any URC (or, if MODULE_AT_URC enabled, hand it over
 *   to the URC callbacks). For the rest, it is the application's responsibility
 *   to handle it.
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
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

#if defined(MODULE_AT_URC_ISR_LOWEST)
#define AT_EVENT_PRIO EVENT_PRIO_LOWEST
#elif defined(MODULE_AT_URC_ISR_MEDIUM)
#define AT_EVENT_PRIO EVENT_PRIO_MEDIUM
#elif defined(MODULE_AT_URC_ISR_HIGHEST)
#define AT_EVENT_PRIO EVENT_PRIO_HIGHEST
#endif

#if defined(MODULE_AT_URC)
static int _check_urc(clist_node_t *node, void *arg);
#endif

#if defined(MODULE_AT_URC_ISR)
static void _event_process_urc(event_t *_event)
{
    at_dev_t *dev = (at_dev_t *)container_of(_event, at_dev_t, event);
    at_process_urc(dev, 1000);
}
#endif

static ssize_t at_readline_skip_empty_stop_at_str(at_dev_t *dev, char *resp_buf,
                                                  size_t len, bool keep_eol,
                                                  char const *substr, uint32_t timeout);
static size_t at_readline_stop_at_str(at_dev_t *dev, char *resp_buf, size_t len,
                                      bool keep_eol, char const *substr,
                                      uint32_t timeout);

static inline bool starts_with(char const *str, char const *prefix)
{
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

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

int at_dev_init(at_dev_t *dev, at_dev_init_t const *init)
{
    dev->uart = init->uart;
    assert(init->rp_buf_size >= 16);
    dev->rp_buf = init->rp_buf;
    dev->rp_buf_size = init->rp_buf_size;

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = false;
    dev->event.handler = _event_process_urc;
#endif

    isrpipe_init(&dev->isrpipe, (uint8_t *)init->rx_buf, init->rx_buf_size);

    return uart_init(init->uart, init->baudrate, _isrpipe_write_one_wrapper, dev);
}

int at_parse_resp(at_dev_t *dev, char const *resp)
{
    if (*resp == '\0') {
        return 1;
    }
    if (starts_with(resp, CONFIG_AT_RECV_OK)) {
        dev->rp_buf[0] = '\0';
        return 0;
    }
    if (starts_with(resp, CONFIG_AT_RECV_ERROR)) {
        return -1;
    }
    /* A specific command may return either CME or CMS, we need not differentiate */
    if (!starts_with(resp, "+CME ERROR: ") &&
        !starts_with(resp, "+CMS ERROR: ")) {
        /* neither `OK` nor error, must be a response or URC */
        return 1;
    }

    resp += strlen("+CMx ERROR: ");
    size_t resp_len = strlen(resp);
    if (resp_len + 1 > dev->rp_buf_size) {
        return -ENOBUFS;
    }
    /* dev->rp_buf and resp may overlap */
    memmove(dev->rp_buf, resp, resp_len + 1);
    return -AT_ERR_EXTENDED;
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

int at_wait_bytes(at_dev_t *dev, const char *bytes, uint32_t timeout)
{
    int res;
    do {
        res = at_expect_bytes(dev, bytes, timeout);
    } while (res != 0 && res != -ETIMEDOUT);
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

    if (!IS_ACTIVE(CONFIG_AT_SEND_SKIP_ECHO)) {
        if (at_wait_bytes(dev, command, timeout)) {
            return -1;
        }

        if (at_expect_bytes(dev, CONFIG_AT_SEND_EOL, timeout)) {
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

    res = at_readline_skip_empty(dev, resp_buf, len, false, timeout);

out:
    return res;
}

ssize_t at_send_cmd_get_resp_wait_ok(at_dev_t *dev, const char *command, const char *resp_prefix,
                                     char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res;
    ssize_t res_ok;

    at_drain(dev);

    res = at_send_cmd(dev, command, timeout);
    if (res) {
        return res;
    }

    /* URCs may occur right after the command has been sent and before the
     * expected response */
    while ((res = at_readline_skip_empty(dev, resp_buf, len, false, timeout)) >= 0) {
        if (!resp_prefix || *resp_prefix == '\0') {
            break;
        }
        /* Strip the expected prefix */
        size_t prefix_len = strlen(resp_prefix);
        if (starts_with(resp_buf, resp_prefix)) {
            size_t remaining_len = strlen(resp_buf) - prefix_len;
            /* The one extra byte in the copy is the terminating nul byte */
            memmove(resp_buf, resp_buf + prefix_len, remaining_len + 1);
            res -= prefix_len;
            break;
        }
        res = at_parse_resp(dev, resp_buf);
        if (res == 0) {
            /* empty response */
            return 0;
        }
        if (res < 0) {
            return res;
        }
#if IS_USED(MODULE_AT_URC)
        else {
            clist_foreach(&dev->urc_list, _check_urc, resp_buf);
        }
#endif
    }
    if (res < 0) {
        return res;
    }
    /* wait for OK */
    res_ok = at_readline_skip_empty(dev, dev->rp_buf, dev->rp_buf_size, false, timeout);
    if (res_ok < 0) {
        return -1;
    }
    res_ok = at_parse_resp(dev, dev->rp_buf);
    if (res_ok == 0) {
        return res;
    }
    if (res_ok < 0) {
        return res_ok;
    }
    /* Neither OK nor error, go figure... */
    return -1;
}

ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf,
                              size_t len, bool keep_eol, uint32_t timeout)
{
    const char eol[] = AT_RECV_EOL_1 AT_RECV_EOL_2;
    assert(sizeof(eol) > 1);

    ssize_t res;
    size_t bytes_left = len - 1;
    char *pos = resp_buf;

    at_drain(dev);

    res = at_send_cmd(dev, command, timeout);
    if (res) {
        return res;
    }

    memset(resp_buf, '\0', len);

    bool first_line = true;
    while (bytes_left) {
        if (first_line) {
            res = at_readline_skip_empty(dev, pos, bytes_left, keep_eol, timeout);
            first_line = false;
        } else {
            /* keep subsequent empty lines, for whatever reason */
            res = at_readline(dev, pos, bytes_left, keep_eol, timeout);
        }
        if (res == 0) {
            *pos++ = eol[sizeof(eol) - 2];
            bytes_left--;
            continue;
        }
        else if (res > 0) {
            size_t const res_len = res;
            bytes_left -= res_len;
            res = at_parse_resp(dev, pos);

            switch (res) {
            case 0: /* OK */
                res = len - bytes_left;
                return res;
            case 1: /* response or URC */
                pos += res_len;
                if (bytes_left == 0) {
                    return -ENOBUFS;
                }
                *pos++ = eol[sizeof(eol) - 2];
                bytes_left--;
                continue;
            default: /* <0 */
                return res;
            }
        }
        else {
            return res;
        }
    }

    return -ENOBUFS;
}

int at_send_cmd_wait_prompt(at_dev_t *dev, const char *command, uint32_t timeout)
{
    at_drain(dev);

    int res = at_send_cmd(dev, command, timeout);
    if (res) {
        return res;
    }

    do {
        res = at_readline_skip_empty_stop_at_str(dev, dev->rp_buf, dev->rp_buf_size,
                                                 false, ">", timeout);
        if (res < 0) {
            break;
        }
        if (strstr(dev->rp_buf, ">")) {
            return 0;
        }
        res = at_parse_resp(dev, dev->rp_buf);
#ifdef MODULE_AT_URC
        if (res == 1) {
            clist_foreach(&dev->urc_list, _check_urc, dev->rp_buf);
        }
#endif
    } while (res >= 0);

    return res;
}

int at_send_cmd_wait_ok(at_dev_t *dev, const char *command, uint32_t timeout)
{
    int res;

    res = at_send_cmd_get_resp(dev, command, dev->rp_buf, dev->rp_buf_size, timeout);

    while (res >= 0) {
        res = at_parse_resp(dev, dev->rp_buf);
        if (res < 1) {
            return res;
        }
#ifdef MODULE_AT_URC
        clist_foreach(&dev->urc_list, _check_urc, dev->rp_buf);
#endif
        res = at_readline_skip_empty(dev, dev->rp_buf, dev->rp_buf_size, false, timeout);
    }

    return res;
}

/* Used to detect a substring that may happen before the EOL. For example,
 * Ublox LTE modules don't add EOL after the prompt character `>`. */
static size_t at_readline_stop_at_str(at_dev_t *dev, char *resp_buf, size_t len,
                                      bool keep_eol, char const *substr,
                                      uint32_t timeout)
{
    const char eol[] = AT_RECV_EOL_1 AT_RECV_EOL_2;
    assert(sizeof(eol) > 1);

    ssize_t res = 0;
    char *resp_pos = resp_buf;

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = true;
#endif

    memset(resp_buf, 0, len);
    size_t substr_len = 0;
    if (substr) {
        substr_len = strlen(substr);
        if (substr_len == 0) {
            return -EINVAL;
        }
    }
    char const *substr_p = resp_buf;
    while (len > 1) {
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
                break;
            }

            resp_pos += read_res;
            len -= read_res;

            if (substr && (size_t)(resp_pos - resp_buf) >= substr_len) {
                if (strncmp(substr_p, substr, substr_len) == 0) {
                    break;
                } else {
                    substr_p++;
                }
            }
        }
        else if (read_res == -ETIMEDOUT) {
            res = -ETIMEDOUT;
            break;
        }
    }

#if IS_USED(MODULE_AT_URC_ISR)
    dev->awaiting_response = false;
#endif

    if (res < 0) {
        *resp_buf = '\0';
    } else {
        res = resp_pos - resp_buf;
    }
    return res;
}

ssize_t at_readline(at_dev_t *dev, char *resp_buf, size_t len, bool keep_eol,
                    uint32_t timeout)
{
    return at_readline_stop_at_str(dev, resp_buf, len, keep_eol, NULL, timeout);
}

static ssize_t at_readline_skip_empty_stop_at_str(at_dev_t *dev, char *resp_buf,
                                                  size_t len, bool keep_eol,
                                                  char const *substr, uint32_t timeout)
{
    ssize_t res = at_readline_stop_at_str(dev, resp_buf, len, keep_eol, substr, timeout);
    if (res == 0) {
        /* skip possible empty line */
        res = at_readline_stop_at_str(dev, resp_buf, len, keep_eol, substr, timeout);
    }
    return res;

}
ssize_t at_readline_skip_empty(at_dev_t *dev, char *resp_buf, size_t len,
                               bool keep_eol, uint32_t timeout)
{
    return at_readline_skip_empty_stop_at_str(dev, resp_buf, len, keep_eol, NULL, timeout);
}

int at_wait_ok(at_dev_t *dev, uint32_t timeout)
{
    while (1) {
        ssize_t res = at_readline_skip_empty(dev, dev->rp_buf, dev->rp_buf_size,
                                            false, timeout);
        if (res < 0) {
            return res;
        }
        res = at_parse_resp(dev, dev->rp_buf);
        if (res < 1) {
            return res;
        }
#ifdef MODULE_AT_URC
        clist_foreach(&dev->urc_list, _check_urc, dev->rp_buf);
#endif
    }
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

    if (starts_with(buf, urc->code)) {
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
