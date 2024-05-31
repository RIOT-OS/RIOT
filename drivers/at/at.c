/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "at.h"
#include "fmt.h"
#include "isrpipe.h"
#include "isrpipe/read_timeout.h"
#include "periph/uart.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef AT_PRINT_INCOMING
#define AT_PRINT_INCOMING (0)
#endif


#if defined(MODULE_AT_URC)
static int _check_urc(clist_node_t *node, void *arg);
#endif

static ssize_t at_readline_skip_empty_stop_at_str(at_dev_t *dev, char *resp_buf,
                                                  size_t len, bool keep_eol,
                                                  char const *substr, uint32_t timeout);
static ssize_t at_readline_stop_at_str(at_dev_t *dev, char *resp_buf, size_t len,
                                      bool keep_eol, char const *substr,
                                      uint32_t timeout);
static ssize_t read_line_or_echo(at_dev_t *dev, char const *cmd, char *resp_buf,
                        size_t len, uint32_t timeout);

static inline bool starts_with(char const *str, char const *prefix)
{
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

static void _isrpipe_write_one_wrapper(void *_dev, uint8_t data)
{
    at_dev_t *dev = (at_dev_t *) _dev;
    isrpipe_write_one(&dev->isrpipe, data);
}

int at_dev_init(at_dev_t *dev, at_dev_init_t const *init)
{
    assert(strlen(AT_RECV_EOL) >= 1);
    assert(init->rp_buf_size >= 16);

    dev->uart = init->uart;
    dev->rp_buf = init->rp_buf;
    dev->rp_buf_size = init->rp_buf_size;

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
    ssize_t res = 0;
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
    return (int)res;
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
    while (len) {
        ssize_t res;
        if ((res = isrpipe_read_timeout(&dev->isrpipe, (uint8_t *)resp_pos,
                                        1, timeout)) == 1) {
            if (AT_PRINT_INCOMING) {
                print(resp_pos, 1);
            }
            resp_pos += res;
            len -= res;
        }
        else if (res == -ETIMEDOUT) {
            break;
        }
    }
    return (resp_pos - bytes);
}

int at_recv_bytes_until_string(at_dev_t *dev, const char *string,
                               char *bytes, size_t *bytes_len, uint32_t timeout)
{
    size_t len = 0;
    char *_string = (char *)string;
    ssize_t res = 0;

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
    return (int)res;
}

static int wait_echo(at_dev_t *dev, char const *command, uint32_t timeout)
{
    ssize_t res;
    while ((res = read_line_or_echo(dev, command, dev->rp_buf, dev->rp_buf_size, timeout)) > 0) {
        /* keep reading until echo or some error happens. */
#ifdef MODULE_AT_URC
        clist_foreach(&dev->urc_list, _check_urc, dev->rp_buf);
#endif
    }
    return (int)res;
}

int at_send_cmd(at_dev_t *dev, const char *command, uint32_t timeout)
{
    size_t cmdlen = strlen(command);

    uart_write(dev->uart, (const uint8_t *)command, cmdlen);
    uart_write(dev->uart, (const uint8_t *)CONFIG_AT_SEND_EOL, AT_SEND_EOL_LEN);

    if (IS_ACTIVE(CONFIG_AT_SEND_SKIP_ECHO)) {
        return 0;
    }
    return wait_echo(dev, command, timeout);
}

void at_drain(at_dev_t *dev)
{
    uint8_t _tmp[16];
    ssize_t res;

    do {
        /* consider no character within 10ms "drained" */
        res = isrpipe_read_timeout(&dev->isrpipe, _tmp, sizeof(_tmp), 10000U);
    } while (res > 0);
}

static bool is_eol(char p)
{
    return p == '\r' || p == '\n';
}

static char *skip_leading_eol(char *line)
{
    while (is_eol(*line)) {
        line++;
    }
    return line;
}

static size_t trim_leading_eol(char *buf, size_t str_len)
{
    char *p = skip_leading_eol(buf);
    if (p == buf) {
        /* not sure if memmove is a no-op in this case */
        return str_len;
    }
    size_t size_left = str_len - (size_t)(p - buf);
    /* +1 for the terminating \0 */
    memmove(buf, p, size_left + 1);
    return size_left;
}

static size_t trim_trailing_eol(char *line, size_t str_len)
{
    while (str_len && is_eol(line[str_len - 1])) {
        line[--str_len] = '\0';
    }
    return str_len;
}

static size_t at_drain_n(at_dev_t *dev, size_t n)
{
    unsigned char drain_buf[16];
    while (n > 0) {
        size_t const to_read = n > sizeof(drain_buf) ? sizeof(drain_buf) : n;
        ssize_t res = isrpipe_read_timeout(&dev->isrpipe, drain_buf, to_read, 10000U);
        if (res < 1) {
            break;
        }
        n -= res;
    }
    return n;
}

/**
 * @retval  0 if an echo was received and flushed
 * @retval  length of the line if an URC was intercepted
 * @retval <0 on error */
static ssize_t read_line_or_echo(at_dev_t *dev, char const *cmd, char *resp_buf,
                        size_t len, uint32_t timeout)
{
    size_t const cmd_len = strlen(cmd);
    if (cmd_len == 0 || len == 0) {
        return -EINVAL;
    }

    if (len == 1) {
        return -ENOBUFS;
    }
    /* We keep EOL in case the echoed command contains binary data and, by
     * chance, a EOL sequence. */
    ssize_t res = at_readline_skip_empty_stop_at_str(dev, resp_buf, len, true,
                                                     cmd, timeout);
    bool overflow = false;
    if (res < 0) {
        if (res != -ENOBUFS) {
            return res;
        }
        /* fine to overflow when intercepting an echo */
        overflow = true;
        res =  len - 1;
    }
    if ((unsigned)res > cmd_len) {
        /* definitely a URC */
        if (overflow) {
            /* URC didn't fit into the buffer so it's garbage */
            return -ENOBUFS;
        }
        return res;
    }
    /* maybe a URC, but might also be:
     * 1. the command contained binary data and, by chance, a newline sequence
     * 2. the command overflowed the resp_buf */
    if (strncmp(cmd, resp_buf, res)) {
        /* no match, indeed a URC. */
        return trim_trailing_eol(resp_buf, res);
    }
    /* very good chance this is a echo, flush the rest */
    size_t const left_in_echo = cmd_len - res + AT_SEND_EOL_LEN;
    res = at_drain_n(dev, left_in_echo);
    if (res > 0) {
        res = -ETIMEDOUT;
    }

    resp_buf[0] = '\0';
    return res;
}

ssize_t at_send_cmd_get_resp(at_dev_t *dev, const char *command,
                             char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res = at_send_cmd(dev, command, timeout);
    if (res) {
        return res;
    }
    return at_readline_skip_empty(dev, resp_buf, len, false, timeout);
}

ssize_t at_get_resp_with_prefix(at_dev_t *dev, const char *resp_prefix,
                                    char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res;
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
    return res;
}

ssize_t at_send_cmd_get_resp_wait_ok(at_dev_t *dev, const char *command, const char *resp_prefix,
                                     char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res = at_send_cmd(dev, command, timeout);
    if (res) {
        return res;
    }
    res = at_get_resp_with_prefix(dev, resp_prefix, resp_buf, len, timeout);
    if (res < 1) {
        /* error or OK (empty response) */
        return res;
    }
    /* got response, wait for OK */
    return at_wait_ok(dev, timeout);
}

#if IS_USED(MODULE_AT_URC)
static char *next_line(char *p)
{
    while (*p && *p != '\r' && *p != '\n') {
        p++;
    }
    return skip_leading_eol(p);
}

static void handle_urc_lines(at_dev_t *dev, char *resp_buf)
{
    char *p = resp_buf;
    do {
        char *next = next_line(p);
        clist_foreach(&dev->urc_list, _check_urc, p);
        p = next;
    } while (*p);
}
#endif

static ssize_t get_lines(at_dev_t *dev, char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res;
    char *pos = resp_buf;
    while ((res = at_readline_skip_empty(dev, pos, len, true, timeout)) > 0) {
        size_t const line_len = res;
        res = at_parse_resp(dev, pos);

        len -= line_len;
        pos += line_len;

        switch (res) {
        case 0: /* OK */
            return (size_t)(pos - resp_buf);
        case 1: /* response or URC */
            if (len == 0) {
                return -ENOBUFS;
            }
            continue;
        default: /* <0 */
#if IS_USED(MODULE_AT_URC)
            /* DCE responded with an error. If we got some lines before that,
             * they must be URCs. */
            handle_urc_lines(dev, resp_buf);
#endif
            return res;
        }
    }
    return res;
}

ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf,
                              size_t len, uint32_t timeout)
{
    ssize_t res = at_send_cmd(dev, command, timeout);
    if (res) {
        return res;
    }
    return get_lines(dev, resp_buf, len, timeout);
}

int at_wait_prompt(at_dev_t *dev, uint32_t timeout)
{
    ssize_t res;
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

    return (int)res;
}

int at_send_cmd_wait_prompt(at_dev_t *dev, const char *command, uint32_t timeout)
{
    ssize_t res = at_send_cmd(dev, command, timeout);
    if (res) {
        return (int)res;
    }
    return at_wait_prompt(dev, timeout);
}

int at_send_cmd_wait_ok(at_dev_t *dev, const char *command, uint32_t timeout)
{
    int res = at_send_cmd(dev, command, timeout);
    if (res < 0) {
        return res;
    }
    return at_wait_ok(dev, timeout);
}

/* Used to detect a substring that may happen before the EOL. For example,
 * Ublox LTE modules don't add EOL after the prompt character `>`. */
static ssize_t at_readline_stop_at_str(at_dev_t *dev, char *resp_buf, size_t len,
                                      bool keep_eol, char const *substr,
                                      uint32_t timeout)
{
    ssize_t res = 0;
    size_t substr_len = 0;
    if (len < 1) {
        return -EINVAL;
    }

    if (substr) {
        substr_len = strlen(substr);
        if (substr_len == 0) {
            return -EINVAL;
        }
    }

    char *resp_pos = resp_buf;
    char const *substr_p = resp_buf;
    memset(resp_buf, 0, len);

    while (len > 1) {
        res = isrpipe_read_timeout(&dev->isrpipe, (uint8_t *)resp_pos, 1, timeout);
        if (res < 1) {
            res = -ETIMEDOUT;
            break;
        }
        if (AT_PRINT_INCOMING) {
            print(resp_pos, 1);
        }

        resp_pos++;
        len--;

        if ((size_t)(resp_pos - resp_buf) >= strlen(AT_RECV_EOL)) {
            char *const eol_begin = resp_pos - strlen(AT_RECV_EOL);
            if (strcmp(eol_begin, AT_RECV_EOL) == 0) {
                if (keep_eol) {
                    break;
                }
                *eol_begin = '\0';
                resp_pos -= strlen(AT_RECV_EOL);
                break;
            }
        }

        if (substr && (size_t)(resp_pos - substr_p) >= substr_len) {
            if (strncmp(substr_p, substr, substr_len) == 0) {
                break;
            } else {
                substr_p++;
            }
        }
    }
    if (len <= 1) {
        return -ENOBUFS;
    }
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
    ssize_t res;
    if (len == 1) {
        /* Reading in a buffer of length 1 will forever return an empty line */
        return -ENOBUFS;
    }
    do {
        res = at_readline_stop_at_str(dev, resp_buf, len, keep_eol, substr, timeout);
        /* Trim any rogue EOL characters */
        if (res > 0) {
            res = trim_leading_eol(resp_buf, (size_t)res);
        } else if (res == -ENOBUFS) {
            res = trim_leading_eol(resp_buf, len - 1);
        }
    } while (res == 0);
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
            return (int)res;
        }
        res = at_parse_resp(dev, dev->rp_buf);
        if (res < 1) {
            return (int)res;
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

    DEBUG("Trying to match %s with %s\n", buf, urc->code);

    if (starts_with(buf, urc->code)) {
        DEBUG("Matched %s\n", urc->code);
        urc->cb(urc->arg, buf);
        return 1;
    }

    return 0;
}

void at_process_urc(at_dev_t *dev, uint32_t timeout)
{
    DEBUG("Processing URC (timeout=%" PRIu32 "us)\n", timeout);

    while (at_readline_skip_empty(dev, dev->rp_buf, dev->rp_buf_size, false, timeout) > 0) {
        clist_foreach(&dev->urc_list, _check_urc, dev->rp_buf);
    }
}

void at_postprocess_urc(at_dev_t *dev, char *buf)
{
    clist_foreach(&dev->urc_list, _check_urc, buf);
}

void at_postprocess_urc_all(at_dev_t *dev, char *buf)
{
    handle_urc_lines(dev, buf);
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

#ifdef MODULE_EMBUNIT
/* Exports for unit tests */
__attribute__((alias("read_line_or_echo")))
ssize_t _emb_read_line_or_echo(at_dev_t *dev, char const *cmd, char *resp_buf,
                        size_t len, uint32_t timeout);
__attribute__((alias("get_lines")))
ssize_t _emb_get_lines(at_dev_t *dev, char *resp_buf, size_t len, uint32_t timeout);
__attribute__((alias("wait_echo")))
int _emb_wait_echo(at_dev_t *dev, char const *command, uint32_t timeout);
#endif
