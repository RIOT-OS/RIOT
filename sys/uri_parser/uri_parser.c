/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_uri_parser
 * @{
 *
 * @file
 * @brief       A minimal, non-destructive URI parser.
 *              @see https://tools.ietf.org/html/rfc3986
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>

#include "uri_parser.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* strchr for non-Null-terminated strings (buffers) */
static const char *_strchrb(const char *start, const char *stop, char c)
{
    for (; start < stop; start++) {
        if (*start == c) {
            return start;
        }
    }
    return NULL;
}

static const char *_consume_scheme(uri_parser_result_t *result, const char *uri,
                                   const char *uri_end, bool *has_authority)
{
    assert(uri);

    /* assume no authority section first */
    *has_authority = false;

    /* cannot have empty scheme */
    if (uri[0] == ':') {
        return NULL;
    }

    const char *p = _strchrb(uri, uri_end, ':');

    result->scheme = uri;
    result->scheme_len = p - uri;

    /* check if authority part exists '://' */
    if (((uri_end - p) > 2) && (p[1] == '/') && (p[2] == '/')) {
        *has_authority = true;
        /* skip '://' */
        return p + 3;
    }

    /* skip ':' */
    return p + 1;
}

void _consume_userinfo(uri_parser_result_t *result, const char *uri,
                       const char *authority_end)
{
    /* check for userinfo within authority */
    const char *userinfo_end = _strchrb(uri, authority_end, '@');

    /* check if match */
    if (userinfo_end) {
        result->userinfo = uri;
        result->userinfo_len = userinfo_end - uri;

        /* shift host part beyond userinfo and '@', but only if possible */
        unsigned offset = result->userinfo_len + 1;
        if ((result->host + offset) > authority_end) {
            result->host_len = 0;
            return;
        }

        result->host_len -= offset;
        result->host += offset;
    }
}

bool _consume_port(uri_parser_result_t *result, const char *ipv6_end,
                   const char *authority_end)
{
    /* check for port after host part */
    const char *port_begin = NULL;
    /* repeat until last ':' in authority section */
    /* if ipv6 address, check after ipv6 end marker */
    const char *p = (ipv6_end ? ipv6_end : result->host);
    while (p != NULL && (p < authority_end)) {
        port_begin = p;
        p = _strchrb(p + 1, authority_end, ':');
    }

    /* check if match */
    if (port_begin && (port_begin[0] == ':')) {
        /* port should be at least one character, => + 1 */
        if (port_begin + 1 == authority_end) {
            return false;
        }
        result->port = port_begin + 1;
        result->port_len = authority_end - result->port;
        /* cut host part before port and ':' */
        result->host_len -= result->port_len + 1;
    }

    return true;
}

static const char *_consume_authority(uri_parser_result_t *result, const char *uri,
                                      const char *uri_end)
{
    assert(uri);

    /* search until first '/' */
    const char *authority_end = _strchrb(uri, uri_end, '/');
    if (!authority_end) {
        authority_end = uri_end;
    }
    result->host = uri;
    result->host_len = authority_end - uri;

    /* consume userinfo, if available */
    _consume_userinfo(result, uri, authority_end);

    /* host is empty */
    if (result->host_len == 0) {
        return authority_end;
    }

    const char *ipv6_end = NULL;
    /* validate IPv6 form */
    if (result->host[0] == '[') {
        ipv6_end = _strchrb(result->host, uri_end, ']');
        /* found end marker of IPv6 form beyond authority part */
        if (ipv6_end >= authority_end) {
            return NULL;
        }

        const char *zoneid_start = _strchrb(result->host, ipv6_end, '%');
        if (zoneid_start) {
            /* skip % */
            result->zoneid = zoneid_start + 1;
            result->zoneid_len = ipv6_end - result->zoneid;

            /* zoneid cannot be empty */
            if (result->zoneid_len == 0) {
                return NULL;
            }
        }

        /* remove '[', ']', and '%' zoneid from ipv6addr */
        result->ipv6addr = result->host + 1;
        result->ipv6addr_len = ipv6_end - result->ipv6addr;
        if (result->zoneid) {
            result->ipv6addr_len -= result->zoneid_len + 1;
        }
    }

    /* consume port, if available */
    if (!_consume_port(result, ipv6_end, authority_end)) {
        return NULL;
    }

    /* this includes the '/' */
    return authority_end;
}

static const char *_consume_path(uri_parser_result_t *result, const char *uri,
                                 const char *uri_end)
{
    assert(uri);

    result->path = uri;
    result->path_len = (uri_end - uri);

    /* check for query start '?' */
    const char *path_end = _strchrb(uri, uri_end, '?');

    /* no query string found, return! */
    if (!path_end) {
        return (result->path + result->path_len);
    }

    /* there is a query string */
    result->query = path_end + 1;
    /* do not count '?' */
    result->query_len = result->path_len - (path_end - uri) - 1;
    /* cut path part before query and '?' */
    result->path_len -= result->query_len + 1;

    return (result->query + result->query_len);
}

static int _parse_relative(uri_parser_result_t *result, const char *uri,
                           const char *uri_end)
{
    uri = _consume_path(result, uri, uri_end);
    /* uri should point to uri_end, otherwise there's something left
     * to consume ... */
    if (uri != uri_end) {
        return -1;
    }

    return 0;
}

static int _parse_absolute(uri_parser_result_t *result, const char *uri,
                           const char *uri_end)
{
    bool has_authority;

    uri = _consume_scheme(result, uri, uri_end, &has_authority);
    if (uri == NULL) {
        return -1;
    }

    if (uri >= uri_end) {
        /* nothing more to consume */
        return 0;
    }

    if (has_authority) {
        uri = _consume_authority(result, uri, uri_end);
        if (uri == NULL) {
            return -1;
        }
    }

    /* is there more to parse after authority? */
    if (uri < uri_end) {
        /* parsing the path, starting with '/' */
        return _parse_relative(result, uri, uri_end);
    }
    return 0;
}

bool uri_parser_is_absolute(const char *uri, size_t uri_len)
{
    const char *colon = _strchrb(uri, uri + uri_len, ':');

    /* potentially absolute, if ':' exists */
    if (colon) {
        /* first character should be ALPHA */
        if (!(((uri[0] >= 'A') && (uri[0] <= 'Z')) ||
              ((uri[0] >= 'a') && (uri[0] <= 'z')))) {
            /* relative */
            return false;
        }
        for (int i = 0; &uri[i] < colon; ++i) {
            if (!(((uri[i] >= 'A') && (uri[i] <= 'Z')) ||
                  ((uri[i] >= 'a') && (uri[i] <= 'z')) ||
                  ((uri[i] >= '0') && (uri[i] <= '9')) ||
                  (uri[i] == '+') ||
                  (uri[i] == '-') ||
                  (uri[i] == '.'))) {
                /* relative */
                return false;
            }
        }

        /* absolute */
        return true;
    }

    /* relative */
    return false;
}

bool uri_parser_is_absolute_string(const char *uri)
{
    return uri_parser_is_absolute(uri, strlen(uri));
}

int uri_parser_process(uri_parser_result_t *result, const char *uri,
                       size_t uri_len)
{
    /* uri cannot be empty */
    if ((NULL == uri) || (uri[0] == '\0')) {
        return -1;
    }

    memset(result, 0, sizeof(*result));

    if (uri_parser_is_absolute(uri, uri_len)) {
        return _parse_absolute(result, uri, uri + uri_len);
    }
    else {
        return _parse_relative(result, uri, uri + uri_len);
    }

    return 0;
}

int uri_parser_process_string(uri_parser_result_t *result, const char *uri)
{
    return uri_parser_process(result, uri, strlen(uri));
}

int uri_parser_split_query(const uri_parser_result_t *uri,
                           uri_parser_query_param_t *params,
                           size_t params_len)
{
    const char *query_end;
    unsigned idx = 0;

    assert(uri);
    assert(params);

    if ((uri->query == NULL) || (uri->query_len == 0) || (params_len == 0)) {
        return 0;
    }
    assert(params[0].name == 0);
    assert(params[0].name_len == 0);
    assert(params[0].value == 0);
    assert(params[0].value_len == 0);
    query_end = uri->query + uri->query_len;
    params[0].name = uri->query;
    for (const char *c = uri->query; c < query_end; c++) {
        switch (*c) {
            case '#':
            case '&':
                if (params[idx].value == NULL) {
                    /* we should have picked up a parameter value by now */
                    return -1;
                }
                params[idx].value_len = c - params[idx].value;
                if (*c == '#') {
                    /* we've reached the end of the query string, next comes an
                     * anchor, enforce end of loop
                     * XXX: can be removed when uri_parser has anchor support */
                    c = query_end;
                }
                else if ((idx + 1) < params_len) {
                    /* c is an ampersand (&), so mark the next char as the next
                     * parameter's name name */
                    params[++idx].name = c + 1U;
                    assert(params[idx].name_len == 0);
                }
                else {
                    /* c is an ampersand (&), but we exceeded param_len.
                     * Return -2 as per doc */
                    return -2;
                }
                break;
            case '=':
                /* params[idx].value != NULL picks up duplicate = in query
                 * parameter */
                if ((params[idx].name == NULL) || params[idx].value != NULL) {
                    /* we should have picked up a parameter name by now */
                    return -1;
                }
                params[idx].name_len = c - params[idx].name;
                /* pick next char as start of value */
                params[idx].value = c + 1U;
                /* make sure the precondition on params is met */
                assert(params[idx].value_len == 0);
                break;
            default:
                break;
        }
    }
    if ((uri->query != query_end) && (params[idx].value == NULL)) {
        /* we should have picked up a parameter value by now */
        return -1;
    }
    /* set final value_len */
    params[idx].value_len = query_end - params[idx].value;
    return idx + 1;
}
