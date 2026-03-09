/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_registry_util
 * @brief       RIOT Registry Util module providing utility functions
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#include "base64.h"
#include "kernel_defines.h"
#include "registry.h"
#include "registry/util.h"
#include "fmt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int registry_util_convert_str_to_value(
    const char *src,
    void *dest,
    const size_t dest_len,
    const registry_type_t dest_type)
{
    assert(src != NULL);
    assert(dest != NULL);

    char *eptr = NULL;

    switch (dest_type) {
    case REGISTRY_TYPE_NONE:
        return -EINVAL;

    case REGISTRY_TYPE_OPAQUE:
        fmt_hex_bytes(dest, src);
        break;

    case REGISTRY_TYPE_STRING:
        if (strlen(src) + 1 > dest_len) {
            return -EINVAL;
        }
        fmt_str((char *)dest, src);
        break;

    case REGISTRY_TYPE_BOOL:
        *(bool *)dest = strtol(src, &eptr, 0);
        break;

    case REGISTRY_TYPE_UINT8:
        *(uint8_t *)dest = strtoul(src, &eptr, 0);
        break;

    case REGISTRY_TYPE_UINT16:
        *(uint16_t *)dest = strtoul(src, &eptr, 0);
        break;

    case REGISTRY_TYPE_UINT32:
        *(uint32_t *)dest = scn_u32_dec(src, strlen(src));
        break;

    case REGISTRY_TYPE_UINT64:
        *(uint64_t *)dest = strtoull(src, &eptr, 0);
        break;

    case REGISTRY_TYPE_INT8:
        *(int8_t *)dest = strtol(src, &eptr, 0);
        break;

    case REGISTRY_TYPE_INT16:
        *(int16_t *)dest = strtol(src, &eptr, 0);
        break;

    case REGISTRY_TYPE_INT32:
        *(int32_t *)dest = strtol(src, &eptr, 0);
        break;

    case REGISTRY_TYPE_INT64:
        *(int64_t *)dest = strtoll(src, &eptr, 0);
        break;

    case REGISTRY_TYPE_FLOAT32:
        *(float *)dest = strtof(src, &eptr);
        break;

    case REGISTRY_TYPE_FLOAT64:
        *(double *)dest = strtod(src, &eptr);
        break;
    }

    if (eptr != NULL && *eptr != '\0') {
        return -EINVAL;
    }

    return 0;
}

int registry_util_convert_value_to_str(
    const registry_value_t *src,
    char *dest,
    const size_t dest_len)
{
    assert(src != NULL);

    size_t str_len;

    switch (src->type) {
    case REGISTRY_TYPE_NONE:
        return -EINVAL;

    case REGISTRY_TYPE_OPAQUE:
        str_len = fmt_bytes_hex(dest, src->buf, src->buf_len);
        break;

    case REGISTRY_TYPE_STRING:
        str_len = fmt_str(dest, (char *)src->buf);
        break;

    case REGISTRY_TYPE_BOOL:
        /* There is no fmt_bool_dec, so we use this instead */
        str_len = fmt_s16_dec(dest, *(bool *)src->buf);
        break;

    case REGISTRY_TYPE_UINT8:
        /* There is no fmt_u8_dec, so we use this instead */
        str_len = fmt_u16_dec(dest, *(uint8_t *)src->buf);
        break;

    case REGISTRY_TYPE_UINT16:
        str_len = fmt_u16_dec(dest, *(uint16_t *)src->buf);
        break;

    case REGISTRY_TYPE_UINT32:
        str_len = fmt_u32_dec(dest, *(uint32_t *)src->buf);
        break;

    case REGISTRY_TYPE_UINT64:
        str_len = fmt_u64_dec(dest, *(uint64_t *)src->buf);
        break;

    case REGISTRY_TYPE_INT8:
        /* There is no fmt_s8_dec, so we use this instead */
        str_len = fmt_s16_dec(dest, *(int8_t *)src->buf);
        break;

    case REGISTRY_TYPE_INT16:
        str_len = fmt_s16_dec(dest, *(int16_t *)src->buf);
        break;

    case REGISTRY_TYPE_INT32:
        str_len = fmt_s32_dec(dest, *(int32_t *)src->buf);
        break;

    case REGISTRY_TYPE_INT64:
        str_len = fmt_s64_dec(dest, *(int64_t *)src->buf);
        break;

    case REGISTRY_TYPE_FLOAT32:
        str_len = fmt_float(
            dest,
            *(float *)src->buf,
            REGISTRY_UTIL_FORMAT_FLOAT_PRECISION);
        break;

    case REGISTRY_TYPE_FLOAT64:
        /* There is no fmt_double, so we use this instead */
        str_len = sprintf(dest, "%f", *(double *)src->buf);
        break;
    }

    if (dest != NULL) {
        /* return error, if the dest buffer is too small */
        if (str_len > dest_len) {
            return -EINVAL;
        }

        /* Add \n terminator only if the output buffer has space for it */
        if (str_len < dest_len) {
            dest[str_len] = '\0';
        }
    }

    return str_len;
}
