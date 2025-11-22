/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test logging with no format gives the expected output
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#include <inttypes.h>

#include "log.h"

int main(void)
{
    uint8_t value = 42;
    const char *string = "test";
    const char *format = "Logging value %d and string %s";

    LOG_ERROR(format, value, string);
    LOG_WARNING(format, value, string);
    LOG_INFO(format, value, string);
    LOG_DEBUG(format, value, string);

    return 0;
}
