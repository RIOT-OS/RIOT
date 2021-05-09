/*
 * Copyright (C) 2021 Inria
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
 * @brief       Test application for the qr-code-generator package
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>
#include "qrcodegen.h"

#ifndef MESSAGE_TO_ENCODE
#define MESSAGE_TO_ENCODE   "unknown"
#endif

static uint8_t qr0[qrcodegen_BUFFER_LEN_FOR_VERSION(2)];
static uint8_t buffer[qrcodegen_BUFFER_LEN_FOR_VERSION(2)];

int main(void)
{
    if (!qrcodegen_encodeText(MESSAGE_TO_ENCODE,
                              buffer, qr0, qrcodegen_Ecc_MEDIUM,
                              qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
                              qrcodegen_Mask_AUTO, true)) {
        puts("Encoding error");
        return -1;
    }

    int size = qrcodegen_getSize(qr0);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            printf("%s", qrcodegen_getModule(qr0, x, y) ? "██" : "  ");
        }
        puts("");
    }

    return 0;
}
