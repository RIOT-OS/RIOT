/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test logging with colors gives the expected output
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#include <inttypes.h>

#define LOG_UNIT "this.test"
#include "log.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define format "Logging value '%d' and string '%s'\n"

int main(void)
{
    const uint8_t value = 42;
    const char *string = "test";

    LOG_ERROR(format, value, string);
    LOG_WARNING(format, value, string);
    LOG_INFO(format, value, string);
    LOG_DEBUG(format, value, string);
    DEBUG(format, value, string);

    return 0;
}
