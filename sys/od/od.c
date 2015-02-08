/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    od.c
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "od.h"

#define _OCTAL_BYTE_LENGTH  (3)
#define _INT_BYTE_LENGTH    (3)
#define _HEX_BYTE_LENGTH    (2)

static inline void _address_format(char *format, uint16_t flags)
{
    switch (flags & OD_FLAGS_ADDRESS_MASK) {
        case OD_FLAGS_ADDRESS_OCTAL:
            strncpy(format, "%09o", sizeof("%09o"));
            break;

        case OD_FLAGS_ADDRESS_HEX:
            strncpy(format, "%06x", sizeof("%06x"));
            break;

        case OD_FLAGS_ADDRESS_DECIMAL:
            strncpy(format, "%07d", sizeof("%07d"));
            break;

        default:
            break;
    }
}

static inline uint8_t _length(uint16_t flags)
{
    if (flags & OD_FLAGS_BYTES_CHAR) {
        return 1;
    }

    switch (flags & OD_FLAGS_LENGTH_MASK) {
        case OD_FLAGS_LENGTH_1:
            return 1;

        case OD_FLAGS_LENGTH_SHORT:
            return sizeof(short);

        case OD_FLAGS_LENGTH_2:
            return 2;

        case OD_FLAGS_LENGTH_LONG:
            return sizeof(long);

        case OD_FLAGS_LENGTH_8:
            return 8;

        case OD_FLAGS_LENGTH_4:
        default:
            return 4;
    }
}

static inline void _bytes_format(char *format, uint16_t flags)
{
    if (flags & OD_FLAGS_BYTES_CHAR) {
        strncpy(format, "    %c", sizeof("    %c"));
        return;
    }

    switch (flags & (OD_FLAGS_BYTES_MASK | OD_FLAGS_LENGTH_MASK)) {
        case OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_1:
            strncpy(format, " %03" PRIo8, sizeof(" %03" PRIo8));
            break;

        case OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_2:
            strncpy(format, " %06" PRIo16, sizeof(" %06" PRIo16));
            break;

        case OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_4:
            strncpy(format, " %012" PRIo32, sizeof(" %012" PRIo32));
            break;

        case OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_8:
            strncpy(format, " %024" PRIo64, sizeof(" %024" PRIo64));
            break;

        case OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_SHORT:
            sprintf(format, " %%0%dho", sizeof(short) * _OCTAL_BYTE_LENGTH);
            break;

        case OD_FLAGS_BYTES_OCTAL | OD_FLAGS_LENGTH_LONG:
            sprintf(format, " %%0%dlo", sizeof(long) * _OCTAL_BYTE_LENGTH);
            break;

        case OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_1:
            strncpy(format, " %4" PRId8, sizeof(" %4" PRId8));
            break;

        case OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_2:
            strncpy(format, " %6" PRId16, sizeof(" %6" PRId16));
            break;

        case OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_4:
            strncpy(format, " %12" PRId32, sizeof(" %12" PRId32));
            break;

        case OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_8:
            strncpy(format, " %24" PRId64, sizeof(" %24" PRId64));
            break;

        case OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_SHORT:
            sprintf(format, " %%%dhd", sizeof(short) * _INT_BYTE_LENGTH);
            break;

        case OD_FLAGS_BYTES_INT | OD_FLAGS_LENGTH_LONG:
            sprintf(format, " %%%dld", sizeof(long) * _INT_BYTE_LENGTH);
            break;

        case OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_1:
            strncpy(format, " %3" PRIu8, sizeof(" %3" PRIu8));
            break;

        case OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_2:
            strncpy(format, " %6" PRIu16, sizeof(" %6" PRIu16));
            break;

        case OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_4:
            strncpy(format, " %12" PRIu32, sizeof(" %12" PRIu32));
            break;

        case OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_8:
            strncpy(format, " %24" PRIu64, sizeof(" %24" PRIu64));
            break;

        case OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_SHORT:
            sprintf(format, " %%%dhu", sizeof(short) * _INT_BYTE_LENGTH);
            break;

        case OD_FLAGS_BYTES_UINT | OD_FLAGS_LENGTH_LONG:
            sprintf(format, " %%%dlu", sizeof(long) * _INT_BYTE_LENGTH);
            break;

        case OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_1:
            strncpy(format, " %02" PRIx8, sizeof(" %02" PRIx8));
            break;

        case OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_2:
            strncpy(format, " %04" PRIx16, sizeof(" %04" PRIx16));
            break;

        case OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_4:
            strncpy(format, " %08" PRIx32, sizeof(" %08" PRIx32));
            break;

        case OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_8:
            strncpy(format, " %016" PRIx64, sizeof(" %016" PRIx64));
            break;

        case OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_SHORT:
            sprintf(format, " %%0%dhx", sizeof(short) * _HEX_BYTE_LENGTH);
            break;

        case OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_LONG:
            sprintf(format, " %%0%dlx", sizeof(long) * _HEX_BYTE_LENGTH);
            break;

        default:
            break;
    }
}

static void _print_date(void *data, size_t offset, char *format, uint8_t length,
                        uint16_t flags)
{
    switch (length) {
        case 1:
            if (flags & OD_FLAGS_BYTES_CHAR) {
                switch (((char *)data)[offset]) {
                    case '\0':
                        printf("   \\0");
                        return;

                    case '\a':
                        printf("   \\a");
                        return;

                    case '\b':
                        printf("   \\b");
                        return;

                    case '\f':
                        printf("   \\f");
                        return;

                    case '\n':
                        printf("   \\n");
                        return;

                    case '\r':
                        printf("   \\r");
                        return;

                    case '\t':
                        printf("   \\t");
                        return;

                    case '\v':
                        printf("   \\v");
                        return;

                    default:
                        if (((char *)data)[offset] < 0) {
                            printf("  %03o", ((unsigned char *)data)[offset]);
                            return;
                        }
                        else if (((char *)data)[offset] < 32) {
                            printf("  %03o", ((char *)data)[offset]);
                            return;
                        }

                        break;
                }

            }

            if (flags & OD_FLAGS_BYTES_INT) {
                printf(format, ((int8_t *)data)[offset]);
            }
            else {
                printf(format, ((uint8_t *)data)[offset]);
            }

            break;

        case 2:
            if (flags & OD_FLAGS_BYTES_INT) {
                printf(format, ((int16_t *)data)[offset]);
            }
            else {
                printf(format, ((uint16_t *)data)[offset]);
            }

            break;

        case 4:
        default:
            if (flags & OD_FLAGS_BYTES_INT) {
                printf(format, ((int32_t *)data)[offset]);
            }
            else {
                printf(format, ((uint32_t *)data)[offset]);
            }

            break;

        case 8:
            if (flags & OD_FLAGS_BYTES_INT) {
                printf(format, ((int64_t *)data)[offset]);
            }
            else {
                printf(format, ((uint64_t *)data)[offset]);
            }

            break;

    }
}

static int _log10(uint8_t a)
{
    int res = 0;

    while (a > 0) {
        a /= 10;
        ++res;
    }

    return ++res;
}

void od(void *data, size_t data_len, uint8_t width, uint16_t flags)
{
    char address_format[5];
    uint8_t date_length = _length(flags);
    char bytes_format[_log10(date_length) + 7];

    _address_format(address_format, flags);
    _bytes_format(bytes_format, flags);

    if (width == 0) {
        width = OD_WIDTH_DEFAULT;
    }

    if (width < date_length) {
        width = 1;
    }
    else {
        width = (width / date_length);
    }

    if (data_len % date_length) {
        data_len = (data_len / date_length) + 1;
    }
    else {
        data_len = data_len / date_length;
    }

    if ((flags & OD_FLAGS_ADDRESS_MASK) != OD_FLAGS_ADDRESS_NONE) {
        printf(address_format, 0);
    }

    for (size_t i = 0; i < data_len; i++) {
        _print_date(data, i, bytes_format, date_length, flags);

        if ((((i + 1) % width) == 0) || i == (data_len - 1)) {
            printf("\n");

            if (i != (data_len - 1)) {
                if ((flags & OD_FLAGS_ADDRESS_MASK) != OD_FLAGS_ADDRESS_NONE) {
                    printf(address_format, date_length * (i + 1));
                }
            }
        }
    }
}

/** @} */
