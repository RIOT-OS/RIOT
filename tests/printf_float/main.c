/*
 * Copyright (C) 2016 Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       print floating point values test application
 *
 * This test is supposed to check that floating point values can be
 * displayed with printf function.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

static const char * expected_result = "2016.0";
static const double floating_point_value = 2016.0317;

int main(void)
{
    const uint8_t str_len = strlen(expected_result);
    char result[str_len];
    snprintf(result, str_len + 1,
             "%.f", floating_point_value);

    printf("Value displayed: %s\n", result);

    if (strcmp(result, expected_result) == 0) {
        printf("[OK]\n");
    }
    else {
        printf("[FAILED] Values are not equal:\n"
               "actual: %s\n"
               "expected: %s\n", result, expected_result);
        return 1;
    }

    return 0;
}
