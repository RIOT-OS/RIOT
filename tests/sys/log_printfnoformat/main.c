/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
