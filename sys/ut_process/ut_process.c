/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>

#include "fmt.h"

#include "ut_process.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef enum {
    INVAL = -1, /* reserved operators '=', ',', '!', '@', '|', '$', '(', ')' */
    SIMPLE = 0, /* no operator */
    RESERVED,   /* '+' */
    FRAGMENT,   /* '#' */
    LABEL,      /* '.' */
    PATH,       /* '/' */
    PATH_PARAM, /* ';' */
    QUERY,      /* '?' */
    QUERY_CONT, /* '&' */
} _op_t;

static int _set_var_list(const char *var_list, size_t var_list_len,
                         const ut_process_var_t *vars, size_t vars_len,
                         char *uri, size_t uri_len, unsigned uri_idx);
static int _set_var(const char *var, size_t var_len,
                    const ut_process_var_t *vars, size_t vars_len,
                    char *uri, size_t uri_len, unsigned uri_idx, _op_t op,
                    bool first);
static int _fill_var(const ut_process_var_t *var, bool has_reserved,
                      bool has_name, bool empty_equal,
                      char *uri, size_t uri_len, unsigned uri_idx);
static int _copy_char(char c, char *out, size_t out_len);
static int _copy_str(const char *in, size_t in_len, char *out, size_t out_len);

int ut_process_expand(const char *ut, size_t ut_len,
                      const ut_process_var_t *vars, size_t vars_len,
                      char *uri, size_t *uri_len_ptr)
{
    const char *exp_start = NULL;
    int res;
    unsigned i, uri_idx = 0;
    size_t uri_len = *uri_len_ptr;

    assert(ut != NULL);
    assert((vars_len == 0) || (vars != NULL));
    assert((uri != NULL) && (uri_len > 0));
    for (i = 0; i < ut_len; i++) {
        switch (ut[i]) {
            case '{':
                if (exp_start) {
                    /* nested variable expressions are not allowed so we are
                     * not in a variable expression. Write-back all collected
                     * chars so far */
                    res = _copy_str(exp_start, (&ut[i] - exp_start),
                                    &uri[uri_idx], uri_len - uri_idx);
                    if (res < 0) {
                        DEBUG("ut_process: %p(%u) does not fit write-back %.*s "
                              "of template %s\n",
                              (void *)uri, (unsigned)uri_len,
                              (int)(&ut[i] - exp_start), exp_start, ut);
                        return res;
                    }
                    uri_idx += res;
                }
                exp_start = &ut[i];
                break;
            case '}':
                if (exp_start) {
                    res = _set_var_list(
                        exp_start + 1,
                        i - (exp_start - ut) - 1,
                        vars, vars_len,
                        uri, uri_len, uri_idx
                    );

                    if (res < 0) {
                        return res;
                    }
                    uri_idx = res;
                    exp_start = NULL;
                    break;
                }
                /* else treat as literal */
                /* Intentionally falls through */
            default:
                if (!exp_start) {
                    res = _copy_char(ut[i], &uri[uri_idx], uri_len - uri_idx);
                    if (res < 0) {
                        DEBUG("ut_process: %p(%u) does not fit literal %c of "
                              "template %s\n", (void *)uri, (unsigned)uri_len,
                              ut[i], ut);
                        return res;
                    }
                    uri_idx += res;
                }
                break;
        }
    }
    if (exp_start) {
        /* a { was opened but not closed until the end of template, copy it
         * into the URI */
        res = _copy_str(exp_start, (&ut[i] - exp_start),
                        &uri[uri_idx], uri_len - uri_idx);
        if (res < 0) {
            DEBUG("ut_process: %p(%u) does not fit terminal write-back %.*s of "
                  "template %s\n",
                  (void *)uri, (unsigned)uri_len,
                  (int)(&ut[i] - exp_start), exp_start, ut);
            return res;
        }
        uri_idx += res;
    }
    res = _copy_char('\0', &uri[uri_idx], uri_len - uri_idx);
    if (res < 0) {
        DEBUG("ut_process: %p(%u) does not fit terminating '\\0' char\n",
              (void *)uri, (unsigned)uri_len);
        return res;
    }
    /* do not increment uri_idx. We want the string length so \0 does not count
     * ;-) */
    *uri_len_ptr = uri_idx;
    return uri_idx;
}

static inline bool _is_lower(char c)
{
    return (c >= 'a') && (c <= 'z');
}

static inline bool _is_alpha(char c)
{
    return (fmt_is_upper(c)) || (_is_lower(c));
}

static inline bool _is_valid_name_char(char c)
{
    return fmt_is_digit(c) || _is_alpha(c) || (c == '_') ||
           (c == '%');  /* pct-encoded, hex is within fmt_is_digit and _is_alpha */
}

static bool _is_unreserved(char c)
{
    return fmt_is_digit(c) || _is_alpha(c) || (c == '-') || (c == '.') ||
           (c == '_') || (c == '~');
}

static bool _is_reserved(char c)
{
    return _is_lower(c) || (c == '!') || (c == '#') || (c == '$') ||
            /* ascii range & to ; includes digits and a few unreserved but
             * safes us a few checks */
            ((c >= '&') && (c <= ';')) ||
            /* ascii range ? to [ includes upper alphas and a few unreserved but
             * safes us a few checks */
            (c == '=') || ((c >= '?') && (c <= '[')) ||
            (c == ']') || _is_unreserved(c);
}

static _op_t _get_op(char ch)
{
    switch (ch) {
        case '#':
            return FRAGMENT;
        case '&':
            return QUERY_CONT;
        case '+':
            return RESERVED;
        case '.':
            return LABEL;
        case '/':
            return PATH;
        case ';':
            return PATH_PARAM;
        case '?':
            return QUERY;
            break;
        case '!': case '$': case '(': case ')':
        case ',': case '=': case '|': case '@':
            /* reserved operators;
             * see https://datatracker.ietf.org/doc/html/rfc6570#section-2.2 */
            return INVAL;
        default:
            return SIMPLE;
    }
}

static const ut_process_var_t *_find_var(const char *var, size_t var_len,
                                         const ut_process_var_t *vars,
                                         size_t vars_len)

{
    for (unsigned i = 0; i < vars_len; i++) {
        const char *name = vars[i].name;

        if ((strlen(name) == var_len) && strncmp(name, var, var_len) == 0) {
            return &vars[i];
        }
    }
    return NULL;
}

static size_t _enc_reserved(char c, char *enc)
{
    if (_is_reserved(c)) {
        *enc = c;
        return sizeof(c);
    }
    else {
        enc[0] = '%';
        return 1 + fmt_byte_hex(&enc[1], (uint8_t)c);
    }
}

static size_t _enc_unreserved(char c, char *enc)
{
    if (_is_unreserved(c)) {
        *enc = c;
        return sizeof(c);
    }
    else {
        enc[0] = '%';
        return 1 + fmt_byte_hex(&enc[1], (uint8_t)c);
    }
}

static int _set_var_list(const char *var_list, size_t var_list_len,
                         const ut_process_var_t *vars, size_t vars_len,
                         char *uri, size_t uri_len, unsigned uri_idx)
{
    int res;
    bool first = true;
    const char *cur_var;
    _op_t op = _get_op(*var_list);

    if (op == INVAL) {
        DEBUG("ut_process: reserved operator %c used\n", *var_list);
        return -EINVAL;
    }
    else if (op != SIMPLE) {
        var_list++;
        var_list_len--;
    }

    cur_var = var_list;
    for (unsigned i = 0; i < var_list_len; i++) {
        switch (var_list[i]) {
            case ',':
                res = _set_var(
                    cur_var, &var_list[i] - cur_var,
                    vars, vars_len, uri, uri_len, uri_idx, op, first
                );

                if (res < 0) {
                    return res;
                }
                if (res > 0) {
                    uri_idx = res;
                    first = false;
                }
                cur_var = &var_list[i + 1];
                break;
            default:
                if (!_is_valid_name_char(var_list[i])) {
                    DEBUG("ut_process: invalid variable name character %c\n",
                          var_list[i]);
                    return -EINVAL;
                }
                break;
        }
    }
    res =  _set_var(cur_var, var_list_len - (cur_var - var_list),
                    vars, vars_len, uri, uri_len, uri_idx, op, first);
    if (res == 0) {     /* the variable was not expanded */
        return uri_idx;
    }
    return res;
}

static int _set_var(const char *var, size_t var_len,
                    const ut_process_var_t *vars, size_t vars_len,
                    char *uri, size_t uri_len, unsigned uri_idx, _op_t op,
                    bool first)
{
    int res;
    const ut_process_var_t *value;
    bool has_name = false;
    bool has_reserved = false;
    bool empty_equal = true;
    char prefix = '\0', sep = '\0';

    if ((var == NULL) || (var_len == 0)) {
        DEBUG("ut_process: zero-length variable found\n");
        return -EINVAL;
    }
    value = _find_var(var, var_len, vars, vars_len);
    if ((value == NULL) || (value->value == NULL)) {
        return 0;
    }
    switch (op) {
        case SIMPLE:
            sep = ',';
            break;
        case RESERVED:
            sep = ',';
            has_reserved = true;    /* reserved chars are allowed in expansion */
            break;
        case FRAGMENT:
            prefix = '#';
            sep = ',';
            has_reserved = true;    /* reserved chars are allowed in expansion */
            break;
        case LABEL:
            prefix = sep = '.';
            break;
        case PATH:
            prefix = sep = '/';
            break;
        case PATH_PARAM:
            prefix = sep = ';';
            has_name = true;        /* name-value pair is used completely */
            empty_equal = false;    /* append equal only if value is non-empty */
            break;
        case QUERY:
            prefix = '?';
            sep = '&';
            has_name = true;        /* name-value pair is used completely */
            break;
        case QUERY_CONT:
            prefix = sep = '&';
            has_name = true;        /* name-value pair is used completely */
            break;
        default:
            break;
    }
    if (first) {
        if (prefix) {
            res = _copy_char(prefix, &uri[uri_idx], uri_len - uri_idx);
            if (res < 0) {
                DEBUG("ut_process: %p(%u) does not fit prefix %c\n",
                      (void *)uri, (unsigned)uri_len, prefix);
                return res;
            }
            uri_idx += res;
        }
    }
    else {
        assert(sep);    /* all operators have a separator defined */
        res = _copy_char(sep, &uri[uri_idx], uri_len - uri_idx);
        if (res < 0) {
            DEBUG("ut_process: %p(%u) does not fit separator '%c'\n",
                  (void *)uri, (unsigned)uri_len, sep);
            return -ENOBUFS;
        }
        uri_idx += res;
    }
    return _fill_var(value, has_reserved, has_name, empty_equal, uri, uri_len,
                     uri_idx);
}

static int _fill_var(const ut_process_var_t *var, bool has_reserved,
                     bool has_name, bool empty_equal,
                     char *uri, size_t uri_len, unsigned uri_idx)
{
    int res;

    if (has_name) {
        /* copy one by one so we do not iterate twice for strlen(var->name)
         * and strcpy() (also code size becomes smaller due to the omitted
         * strlen()) */
        for (const char *c = var->name; *c != '\0'; c++) {
            res = _copy_char(*c, &uri[uri_idx], uri_len - uri_idx);
            if (res < 0) {
                DEBUG("ut_process: %p(%u) does not var name %s\n", (void *)uri,
                      (unsigned)uri_len, var->name);
                return res;
            }
            uri_idx += res;
        }
        /* value is not an empty string */
        if ((var->value[0] != '\0') || empty_equal) {
            res = _copy_char('=', &uri[uri_idx], uri_len - uri_idx);
            if (res < 0) {
                DEBUG("ut_process: %p(%u) does not fit =\n", (void *)uri,
                      (unsigned)uri_len);
                return res;
            }
            uri_idx += res;
        }
    }
    for (const char *c = var->value; *c != '\0'; c++) {
        char enc[sizeof("%00")];
        size_t enc_len;

        if (has_reserved) {
            enc_len = _enc_reserved(*c, enc);
        }
        else {
            enc_len = _enc_unreserved(*c, enc);
        }
        res = _copy_str(enc, enc_len, &uri[uri_idx], uri_len - uri_idx);
        if (res < 0) {
            DEBUG("ut_process: %p(%u) does not fit value encoding %.*s\n",
                  (void *)uri, (unsigned)uri_len, (unsigned)enc_len, enc);
            return res;
        }
        uri_idx += res;
    }
    return uri_idx;
}

static int _copy_char(char c, char *out, size_t out_len)
{
    if (out_len == 0) {
        return -ENOBUFS;
    }
    out[0] = c;
    return sizeof(c);
}

static int _copy_str(const char *in, size_t in_len, char *out, size_t out_len)
{
    if (in_len >= out_len) {
        return -ENOBUFS;
    }
    strncpy(out, in, in_len);
    return in_len;
}

/** @} */
