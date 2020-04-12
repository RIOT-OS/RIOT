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
        "RIOT ADC NG NTC temperature driver test\n"
        "=======================================\n"
        "\n"
    );

    while (1) {
        int16_t value;
        int retval = adc_ng_internal_ntc(0, &value);
        if (retval) {
            print_str("adc_ng_internal_ntc() failed with ");
            print_s32_dec(retval);
            print_str("\n");
            return -1;
        }
        char buf[16];
        print(buf, fmt_s32_dfp(buf, value, -1));
        print_str("°C\n");
    }

    return 0;
}
