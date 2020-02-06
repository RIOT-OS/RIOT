/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application for ADC NG drivers
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdlib.h>

#include "adc_ng.h"
#include "adc_ng_util.h"
#include "fmt.h"

int main(void)
{
    print_str(
        "RIOT ADC NG NTC entropy driver test\n"
        "===================================\n"
        "\n"
    );

    while (1) {
        int8_t entropy[16];
        int retval = adc_ng_entropy(0, entropy, sizeof(entropy));
        if (retval) {
            print_str("adc_ng_entropy() failed with ");
            print_s32_dec(retval);
            print_str("\n");
            return -1;
        }
        char buf[2];
        fmt_byte_hex(buf, entropy[0]);
        print(buf, 2);
        for (unsigned i = 1; i < sizeof(entropy); i++) {
            print(" ", 1);
            fmt_byte_hex(buf, entropy[i]);
            print(buf, 2);
        }
        print("\n", 1);
    }

    return 0;
}
