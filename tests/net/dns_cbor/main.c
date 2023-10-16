/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <martine.lenders@tu-dresden.de>
 */

#include <errno.h>
#include <string.h>

#include "net/dns/types.h"
#include "fmt.h"
#include "od.h"
#include "shell.h"

#include "net/dns/cbor.h"

static uint8_t _buf[CONFIG_DNS_CBOR_LEN];
static size_t _buf_len = 0U;
static dns_cbor_status_t _status;

static void _print_errno(int err)
{
    print_u32_dec(err);
    print_str(" \"");
    print_str(strerror(err));
    print_str("\"\n");
}

static int _error_codes(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    print_str("EBADMSG ");
    _print_errno(EBADMSG);
    print_str("EINVAL ");
    _print_errno(EINVAL);
    print_str("ENOBUFS ");
    _print_errno(ENOBUFS);
    print_str("ENOMEM ");
    _print_errno(ENOMEM);
    print_str("ENOSPC ");
    _print_errno(ENOSPC);
    print_str("END_ERROR\n");
    return 0;
}

static void _dump_buf(void)
{
    od_hex_dump(_buf, _buf_len, OD_WIDTH_DEFAULT);
}

static int _load(int argc, char **argv)
{
    if (argc < 2) {
        print_str("usage: ");
        print_str(argv[0]);
        print_str(" <hex bytes>\n");
        return 1;
    }
    char *in_ptr = argv[1];
    size_t orig_buf_len = _buf_len;
    uint8_t shift = 4U;
    while (*in_ptr != '\0') {
        char c = *in_ptr++;
        uint8_t d;

        if (_buf_len >= sizeof(_buf)) {
            print_str("Buffer would overflow\n");
            _buf_len = orig_buf_len;
            return 1;
        }
        if (('0' <= c) && (c <= '9')){
            d = c - (uint8_t)'0';
        }
        else if (('A' <= c) && (c <= 'F')) {
            d = c - (uint8_t)'A' + 0xaU;
        }
        else if (('a' <= c) && (c <= 'f')) {
            d = c - (uint8_t)'a' + 0xaU;
        }
        else {
            print_str("Invalid character '");
            print(&c, 1);
            print_str("' in \"");
            print_str(argv[1]);
            print_str("\"\n");
            _buf_len = orig_buf_len;
            return 1;
        }
        if (shift == 0) {
            _buf[_buf_len++] |= d;
            shift = 4U;
        }
        else {
            _buf[_buf_len] = (d << shift);
            shift = 0U;
        }
    }
    if (shift == 0) {
        /* add overhang; this does not cause a buffer overflow, since _buf_len is only incremented
         * above before `shift` is set to 4 */
        _buf_len++;
    }
    print_str("Successfully loaded \"");
    print_str(argv[1]);
    print_str("\" into buffer:\n");
    _dump_buf();
    return 0;
}

static void _dump_status(void)
{
    print_str("  question:\n");
    print_str("    domain_name: \"");
    print_str(_status.question.domain_name);
    print_str("\"\n");
    print_str("    type: ");
    print_u32_dec(_status.question.type);
    print_str("\n");
    print_str("    class: ");
    print_u32_dec(_status.question.class);
    print_str("\n");
}

static int _set_status(int argc, char **argv)
{
    if (argc < 2) {
        print_str("usage: ");
        print_str(argv[0]);
        print_str(" <domain name> [<type> [<class>]]\n");
        return 1;
    }
    strncpy(_status.question.domain_name, argv[1], CONFIG_DNS_CBOR_NAME_LEN);
    if (argc > 2) {
        _status.question.type = scn_u32_dec(argv[2], strlen(argv[2]));
    }
    if (argc > 3) {
        _status.question.class = scn_u32_dec(argv[3], strlen(argv[3]));
    }
    print_str("Successfully set status:\n");
    _dump_status();
    return 0;
}

static int _dump(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    print_str("Buffer:");
    if (_buf_len == 0) {
        print_str(" empty\n");
    }
    else {
        print_str("\n");
        _dump_buf();
    }
    print_str("Status:\n");
    _dump_status();
    print_str("END\n");
    return 0;
}

static void _reset(void)
{
    _buf_len = 0U;
    memset(&_status, 0, sizeof(_status));
}

static int _reset_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    _reset();
    return 0;
}

static int _compose(int argc, char **argv)
{
    ssize_t res;
    uint32_t type;
    uint32_t class = DNS_CLASS_IN;

    if (argc < 3) {
        print_str("usage: ");
        print_str(argv[0]);
        print_str(" <domain name> <type> [<class>]\n");
        return 1;
    }

    if (strlen(argv[1]) >= CONFIG_DNS_CBOR_NAME_LEN) {
        print_str("Domain name \"");
        print_str(argv[1]);
        print_str("\" is longer than ");
        print_u32_dec(CONFIG_DNS_CBOR_NAME_LEN - 1);
        print_str(" characters\n");
        return 1;
    }
    type = scn_u32_dec(argv[2], strlen(argv[2]));
    if (type > UINT16_MAX) {
        print_str("Type ");
        print_u32_dec(type);
        print_str(" too large (must be 16-bit number)\n");
        return 1;
    }
    if (argc > 3) {
        class = scn_u32_dec(argv[3], strlen(argv[3]));
        if (class > UINT16_MAX) {
            print_str("Class ");
            print_u32_dec(class);
            print_str(" too large (must be 16-bit number)\n");
            return 1;
        }
    }
    strncpy(_status.question.domain_name, argv[1], sizeof(_status.question.domain_name));
    _status.question.type = type;
    _status.question.class = class;
    res = dns_cbor_compose_query(&_status, _buf);
    if (res < 0) {
        /* not reachable with default config */
        print_str("Error ");
        _print_errno(-res);
        return 1;
    }
    else {
        print_str("Success:\n");
        _buf_len = res;
        _dump_buf();
    }
    return 0;
}

static int _parse(int argc, char **argv)
{
    static uint8_t out[CONFIG_DNS_CBOR_LEN] = { 0 };
    uint32_t ttl = 0;
    ssize_t res;

    (void)argc;
    (void)argv;
    res = dns_cbor_parse_response(_buf, _buf_len, &_status, out, &ttl);
    if (res < 0) {
        print_str("Error ");
        _print_errno(-res);
        return 1;
    }
    else {
        print_str("Success:");
        if (res > 0) {
            print_str("\n");
            od_hex_dump(out, res, OD_WIDTH_DEFAULT);
        }
        else {
            print_str(" no data\n");
        }
        print_str("TTL: ");
        print_u32_dec(ttl);
        print_str("\n");
    }
    return 0;
}

SHELL_COMMAND(error_codes, "Output error codes for this test\n", _error_codes);
SHELL_COMMAND(load, "Load hex string consecutive into buffer\n", _load);
SHELL_COMMAND(set_status, "Set parsing status\n", _set_status);
SHELL_COMMAND(reset, "Reset buffer and parsing status\n", _reset_cmd);
SHELL_COMMAND(dump, "Dump buffer and parsing status\n", _dump);
SHELL_COMMAND(compose, "Compose query (calls reset before)\n", _compose);
SHELL_COMMAND(parse, "Parse response in buffer with parsing status\n", _parse);

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(NULL, line_buf, sizeof(line_buf));
    return 0;
}

/** @} */
