/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       coreJSON package test application
 *
 *  Adapted to RIOT style from the [reference example](https://github.com/FreeRTOS/coreJSON#reference-example)
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "core_json.h"

int main(void)
{
    /* Variables used in this example. */
    JSONStatus_t result;
    char buffer[] = "{\"foo\":\"abc\",\"bar\":{\"foo\":\"xyz\"}}";
    size_t bufferLength = sizeof(buffer) - 1;
    char queryKey[] = "bar.foo";
    size_t queryKeyLength = sizeof(queryKey) - 1;
    char *value;
    size_t valueLength;

    /* Calling JSON_Validate() is not necessary if the document is guaranteed to be valid. */
    result = JSON_Validate(buffer, bufferLength);

    if (result == JSONSuccess) {
        result = JSON_Search(
            buffer, bufferLength, queryKey, queryKeyLength,
            &value, &valueLength
        );
    }

    if (result == JSONSuccess) {
        /* The pointer "value" will point to a location in the "buffer". */
        char save = value[valueLength];
        /* After saving the character, set it to a null byte for printing. */
        value[valueLength] = '\0';
        /* "Found: bar.foo -> xyz" will be printed. */
        printf("Found: %s -> %s\n", queryKey, value);
        /* Restore the original character. */
        value[valueLength] = save;
    }

    return 0;
}
