/*
 * Copyright (C) 2021 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Short SenML SAUL example
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "senml/saul.h"
#include "fmt.h"

static uint8_t cbor_buf[1024];

void print_hex(uint8_t *a, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        print_byte_hex(a[i]);
    }
}

int main(void)
{
    size_t len = senml_saul_encode_cbor(cbor_buf, sizeof cbor_buf, saul_reg);

    if (len == 0) {
        print_str("SenML/SAUL error\n");
        return 1;
    }

    print_str("CBOR (");
    print_u32_dec(len);
    print_str(" B): ");
    print_hex(cbor_buf, len);
    print_str("\n");

    return 0;
}
