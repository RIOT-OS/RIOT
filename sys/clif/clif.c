/*
 * Copyright (c) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_clif
 * @{
 *
 * @file
 * @brief       CoRE Link format encoding and decoding library implementation
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 * @}
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "clif.h"
#include "clif_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* returns the correspondent attribute string */
static const char *_attr_to_str[] = {
    [CLIF_ATTR_ANCHOR]    = LF_ATTR_ANCHOR,
    [CLIF_ATTR_REL]       = LF_ATTR_REL_TYPE,
    [CLIF_ATTR_LANG]      = LF_ATTR_LANG,
    [CLIF_ATTR_MEDIA]     = LF_ATTR_MEDIA,
    [CLIF_ATTR_TITLE]     = LF_ATTR_TITLE,
    [CLIF_ATTR_TITLE_EXT] = LF_ATTR_TITLE_EXT,
    [CLIF_ATTR_TYPE]      = LF_ATTR_TYPE,
    [CLIF_ATTR_RT]        = LF_ATTR_RES_TYPE,
    [CLIF_ATTR_IF]        = LF_ATTR_IF_DESC,
    [CLIF_ATTR_SZ]        = LF_ATTR_SIZE,
    [CLIF_ATTR_CT]        = LF_ATTR_CT,
    [CLIF_ATTR_OBS]       = LF_ATTR_OBS
};

/* returns the correspondent attribute string size */
static const unsigned _attr_to_size[] = {
    [CLIF_ATTR_ANCHOR]    = LF_ATTR_ANCHOR_S,
    [CLIF_ATTR_REL]       = LF_ATTR_REL_TYPE_S,
    [CLIF_ATTR_LANG]      = LF_ATTR_LANG_S,
    [CLIF_ATTR_MEDIA]     = LF_ATTR_MEDIA_S,
    [CLIF_ATTR_TITLE]     = LF_ATTR_TITLE_S,
    [CLIF_ATTR_TITLE_EXT] = LF_ATTR_TITLE_EXT_S,
    [CLIF_ATTR_TYPE]      = LF_ATTR_TYPE_S,
    [CLIF_ATTR_RT]        = LF_ATTR_RES_TYPE_S,
    [CLIF_ATTR_IF]        = LF_ATTR_IF_DESC_S,
    [CLIF_ATTR_SZ]        = LF_ATTR_SIZE_S,
    [CLIF_ATTR_CT]        = LF_ATTR_CT_S,
    [CLIF_ATTR_OBS]       = LF_ATTR_OBS_S
};

/* do not count extension attr type */
#define ATTRS_NUMOF ARRAY_SIZE(_attr_to_str)

ssize_t clif_decode_link(clif_t *link, clif_attr_t *attrs, unsigned attrs_len,
                         const char *buf, size_t maxlen)
{

    assert(buf);
    assert(link);

    char *pos;
    const char *end = buf + maxlen;
    clif_attr_t _dummy_attr;

    ssize_t size = clif_get_target(buf, maxlen, &pos);
    if (size < 0) {
        return CLIF_NOT_FOUND;
    }
    link->target = pos;
    link->target_len = size;
    link->attrs_len = 0;
    link->attrs = attrs;
    pos += size + 1; /* escape the '>' */

    DEBUG("Found target (%u): %.*s\n", (unsigned)size, (unsigned)size,
          link->target);

    /* if there is no attr array iterate over the buffer, if not until all
     * the array is used */
    while ((!attrs && pos < end) || (attrs && link->attrs_len < attrs_len)) {
        clif_attr_t *attr = attrs ? &attrs[link->attrs_len] : &_dummy_attr;
        size = clif_get_attr(pos, end - pos, attr);
        if (size < 0) {
            break;
        }
        pos += size;
        link->attrs_len++;
    }

    return pos - buf;
}

ssize_t clif_encode_link(const clif_t *link, char *buf, size_t maxlen)
{
    assert(link);
    size_t pos = 0;
    ssize_t res = 0;

    res = clif_add_target(link->target, buf, maxlen);
    if (res < 0) {
        return res;
    }
    pos += res;

    for (unsigned i = 0; i < link->attrs_len; i++) {
        res = clif_add_attr(&link->attrs[i], buf ? &buf[pos] : NULL,
                             maxlen - pos);
        if (res <= 0) {
            return res;
        }
        pos += res;
    }
    return pos;
}

ssize_t clif_add_target(const char *target, char *buf, size_t maxlen)
{
    assert(target);

    size_t pos = 0;
    size_t target_len = strlen(target);

    if (!buf) {
        return target_len + 2; /* size after adding '<' and '>' */
    }

    if ((target_len + 2) > maxlen) {
        return CLIF_NO_SPACE;
    }

    buf[pos++] = LF_PATH_BEGIN_C;

    memcpy(&buf[pos], target, target_len);
    pos += target_len;

    buf[pos++] = LF_PATH_END_C;

    return pos;
}

ssize_t clif_add_link_separator(char *buf, size_t maxlen)
{
    if (!buf) {
        return 1;
    }

    if (maxlen < 1) {
        return CLIF_NO_SPACE;
    }

    *buf = LF_LINK_SEPARATOR_C;
    return 1;
}

ssize_t clif_add_attr(clif_attr_t *attr, char *buf, size_t maxlen)
{
    assert(attr);
    assert(attr->key);

    /* if no length given, calculate it */
    if (!attr->key_len) {
        attr->key_len = strlen(attr->key);
    }

    /* count attr name size and separator ';' */
    size_t req_space = attr->key_len + 1;
    size_t pos = 0;
    int quoted = strcmp(attr->key, LF_ATTR_SIZE) ? 1 : 0;

    if (attr->value) {
        if (!attr->value_len) {
            attr->value_len = strlen(attr->value);
        }
        /* count also '=' */
        req_space += attr->value_len +  1;
    }

    if (quoted) {
        req_space += 2;
    }

    if (!buf) {
        return req_space;
    }

    if (req_space > maxlen) {
        return CLIF_NO_SPACE;
    }

    /* add attribute separator ';' */
    buf[pos++] = LF_ATTR_SEPARATOR_C;

    /* add attribute name */
    memcpy(&buf[pos], attr->key, attr->key_len);
    pos += attr->key_len;

    /* add attribute value if defined */
    if (attr->value) {
        buf[pos++] = LF_ATTR_VAL_SEPARATOR_C;

        if (quoted) {
            buf[pos++] = '"';
        }

        memcpy(&buf[pos], attr->value, attr->value_len);
        pos += attr->value_len;

        if (quoted) {
            buf[pos++] = '"';
        }
    }

    return pos;
}

ssize_t clif_get_target(const char *input, size_t input_len, char **output)
{
    assert(input);
    char *target_end;

    *output = memchr(input, LF_PATH_BEGIN_C, input_len);
    if (!*output) {
        DEBUG("Path start not found\n");
        return CLIF_NOT_FOUND;
    }
    *output += 1;

    target_end = memchr(*output, LF_PATH_END_C, (input + input_len) - *output);
    if (!target_end) {
        DEBUG("Path end not found\n");
        return CLIF_NOT_FOUND;
    }
    ssize_t res = target_end - *output;
    return res;
}

ssize_t clif_get_attr(const char *input, size_t input_len, clif_attr_t *attr)
{
    assert(input);
    assert(attr);
    const char *pos = input;
    const char *end = input + input_len;
    bool quoted = false;
    bool scan_value = false;

    /* initialize attr */
    attr->value = NULL;
    attr->key = NULL;

    /* an attribute should start with the separator */
    if (*pos != LF_ATTR_SEPARATOR_C) {
        DEBUG("Attribute should start with separator, found %c\n", *pos);
        return CLIF_NOT_FOUND;
    }
    pos++;
    attr->key = pos;

    /* iterate over key */
    while (pos < end) {
        if (*pos == LF_ATTR_SEPARATOR_C || *pos == LF_LINK_SEPARATOR_C) {
            /* key ends, no value */
            attr->key_len = pos - attr->key;
            break;
        }
        if (*pos == LF_ATTR_VAL_SEPARATOR_C) {
            /* key ends, has value */
            attr->key_len = pos - attr->key;
            /* check if the value is quoted and prepare pointer for value scan */
            pos++;
            if (*pos == '"') {
                quoted = true;
                pos++;
            }
            attr->value = (char *)pos;
            scan_value = true;
            break;
        }
        pos++;
    }

    if (scan_value) {
        /* iterate over value */
        while (pos < end) {
            if (quoted) {
                if (*pos == '"' && *(pos - 1) != '\\') {
                    /* found unescaped quote */
                    attr->value_len = pos - attr->value;
                    pos++;
                    break;
                }
            }
            else {
                if (*pos == LF_ATTR_SEPARATOR_C || *pos == LF_LINK_SEPARATOR_C) {
                    /* value ends */
                    attr->value_len = pos - attr->value;
                    break;
                }
            }
            pos++;
        }
    }
    else {
        /* buffer exhausted and no special character found, calculate length of
        * attribute and exit */
        attr->key_len = pos - attr->key;
    }

    return pos - input;
}

ssize_t clif_attr_type_to_str(clif_attr_type_t type, const char **str)
{
    if (type < ATTRS_NUMOF) {
        *str = _attr_to_str[type];
        return _attr_to_size[type];
    }
    return CLIF_NOT_FOUND;
}

clif_attr_type_t clif_get_attr_type(const char *input, size_t input_len)
{
    assert(input);
    assert(input_len > 0);
    clif_attr_type_t ret = CLIF_ATTR_EXT;
    for (unsigned i = 0; i < ATTRS_NUMOF; i++) {
        if (input_len == _attr_to_size[i] &&
            !strncmp(input, _attr_to_str[i], input_len)) {
            ret = i;
            break;
        }
    }
    return ret;
}

int clif_init_attr(clif_attr_t *attr, clif_attr_type_t type)
{
    assert(attr);
    attr->key_len = clif_attr_type_to_str(type, &attr->key);
    return attr->key_len > 0 ? 0 : CLIF_NOT_FOUND;
}
