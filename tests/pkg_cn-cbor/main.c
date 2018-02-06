/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       cn-cbor test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "cn-cbor/cn-cbor.h"

#define CBOR_TEST   { 0x17 }    /* 23 */

int main(void)
{
    const char cbor[] = CBOR_TEST;

    cn_cbor *c = cn_cbor_decode((const uint8_t *)cbor, strlen(cbor), NULL);

    if (c && (c->v.sint == 23)) {
        puts("[SUCCESS]");
    }
    else {
        puts("[FAIL]");
    }

    return 0;
}
