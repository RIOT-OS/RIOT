/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_util RIOT Registry utilities
 * @ingroup     sys
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

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "base64.h"
#include "kernel_defines.h"

#include "registry.h"
#include "registry/util.h"
#include "fmt.h"

int registry_util_convert_str_to_value(
    const char *src,
    void *dest,
    const size_t dest_len,
    const registry_type_t dest_type)
{
    assert(src != NULL);
    assert(dest != NULL);

    char *eptr = NULL;

    if (!src) {
        return -EINVAL;
    }

    switch (dest_type) {
    case REGISTRY_TYPE_NONE: {
        return -EINVAL;
    }

    case REGISTRY_TYPE_OPAQUE: {
        size_t base64_decode_len;
        if (base64_decode(src, strlen(src), dest, &base64_decode_len) !=
                BASE64_SUCCESS &&
            base64_decode_len <= dest_len) {
            return -EINVAL;
        }
        break;
    }

    case REGISTRY_TYPE_STRING: {
        if (strlen(src) + 1 > dest_len) {
            return -EINVAL;
        }
        strcpy((char *)dest, src);
        break;
    }

    case REGISTRY_TYPE_BOOL: {
        *(bool *)dest = strtol(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_UINT8: {
        *(uint8_t *)dest = strtoul(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_UINT16: {
        *(uint16_t *)dest = strtoul(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_UINT32: {
        *(uint32_t *)dest = strtoul(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_UINT64: {
        *(uint64_t *)dest = strtoull(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_INT8: {
        *(int8_t *)dest = strtol(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_INT16: {
        *(int16_t *)dest = strtol(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_INT32: {
        *(int32_t *)dest = strtol(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_INT64: {
        *(int64_t *)dest = strtoll(src, &eptr, 0);
        break;
    }

    case REGISTRY_TYPE_FLOAT32: {
        *(float *)dest = strtof(src, &eptr);
        break;
    }

    case REGISTRY_TYPE_FLOAT64: {
        *(double *)dest = strtod(src, &eptr);
        break;
    }
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
    case REGISTRY_TYPE_NONE: {
        return -EINVAL;
    }

    case REGISTRY_TYPE_OPAQUE: {
        if (base64_encode(src->buf, src->buf_len, dest, &str_len) != BASE64_SUCCESS || str_len > dest_len - 1) {
            /* If dest is NULL, the length is returned */
            if (dest != NULL) {
                return -EINVAL;
            }
        }
        else {
            dest[str_len] = '\0';
        }
        break;
    }

    case REGISTRY_TYPE_STRING: {
        char *str_val = (char *)src->buf;

        str_len = strlen(str_val);

        if (str_len > dest_len - 1) {
            /* If dest is NULL, the length is returned */
            if (dest != NULL) {
                return -EINVAL;
            }
        }
        else {
            strcpy(dest, str_val);
        }
        break;
    }

    case REGISTRY_TYPE_BOOL: {
        str_len = snprintf(dest, dest_len, " %" PRId8, *(bool *)src->buf);
        break;
    }

    case REGISTRY_TYPE_UINT8: {
        str_len = snprintf(dest, dest_len, " %" PRIu8, *(uint8_t *)src->buf);
        break;
    }

    case REGISTRY_TYPE_UINT16: {
        str_len = snprintf(dest, dest_len, " %" PRIu16, *(uint16_t *)src->buf);
        break;
    }

    case REGISTRY_TYPE_UINT32: {
        str_len = snprintf(dest, dest_len, " %" PRIu32, *(uint32_t *)src->buf);
        break;
    }

    case REGISTRY_TYPE_UINT64: {
        str_len = snprintf(dest, dest_len, " %" PRIu64, *(uint64_t *)src->buf);
        break;
    }

    case REGISTRY_TYPE_INT8: {
        str_len = snprintf(dest, dest_len, " %" PRId8, *(int8_t *)src->buf);
        break;
    }

    case REGISTRY_TYPE_INT16: {
        str_len = snprintf(dest, dest_len, " %" PRId16, *(int16_t *)src->buf);
        break;
    }

    case REGISTRY_TYPE_INT32: {
        str_len = snprintf(dest, dest_len, " %" PRId32, *(int32_t *)src->buf);
        break;
    }

    case REGISTRY_TYPE_INT64: {
        str_len = snprintf(dest, dest_len, " %" PRId64, *(int64_t *)src->buf);
        break;
    }

    case REGISTRY_TYPE_FLOAT32: {
        str_len = sprintf(dest, " %f", *(float *)src->buf);
        if (str_len > dest_len - 1) {
            /* If dest is NULL, the length is returned */
            if (dest != NULL) {
                return -EINVAL;
            }
        }
        break;
    }

    case REGISTRY_TYPE_FLOAT64: {
        str_len = sprintf(dest, " %f", *(double *)src->buf);
        if (str_len > dest_len - 1) {
            /* If dest is NULL, the length is returned */
            if (dest != NULL) {
                return -EINVAL;
            }
        }
        break;
    }
    }

    return str_len;
}
