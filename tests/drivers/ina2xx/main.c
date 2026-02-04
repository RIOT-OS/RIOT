/*
 * SPDX-FileCopyrightText: 2015 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the INA2XX sensor driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "fmt.h"
#include "fmt_table.h"
#include "ina2xx.h"
#include "ina2xx_params.h"

int main(void)
{
    ina2xx_t dev;

    print_str("INA2XX sensor driver test application\n\n");

    print_str("Initializing INA2XX sensor at I2C_");
    print_s32_dec(ina2xx_params[0].i2c);
    print_str(", address 0x");
    print_u32_hex(ina2xx_params[0].addr);
    print_str("\n");
    if (ina2xx_init(&dev, &ina2xx_params[0]) == 0) {
        print_str("[OK]\n");
    } else {
        print_str("[Failed]\n");
        return EXIT_FAILURE;
    }

    const char *line = "+------------+--------------+----------+--------+\n";
    print_str(line);
    print_str("| U_Bus [mV] | U_Shunt [µV] |  I [µA]  | P [µW] |\n");
    print_str(line);

    while (1) {
        uint16_t u_bus;
        int16_t u_shunt;
        int32_t i_shunt;
        uint32_t p;

        /* Read bus voltage until flag indicates new value is present */
        switch (ina2xx_read_bus(&dev, &u_bus)){
            case 0:
                /* No measurement available yet */
                continue;
            case 1:
                /* New measurement available, continue */
                break;
            case -EDOM:
                print_str("[WARNING]: INA2xx detected math overflow ==> data "
                          "will be incorrect\n");
                break;
            default:
                /* Error */
                print_str("Error while reading bus voltage\n");
                return EXIT_FAILURE;
        }

        if (ina2xx_read_shunt(&dev, &u_shunt) < 0) {
            print_str("Error while reading shunt voltage\n");
            return EXIT_FAILURE;
        }

        if (ina2xx_read_current(&dev, &i_shunt) < 0) {
            print_str("Error while reading current\n");
            return EXIT_FAILURE;
        }

        if (ina2xx_read_power(&dev, &p) < 0) {
            print_str("Error while reading power\n");
            return EXIT_FAILURE;
        }

        print_str("| ");
        print_col_u32_dec(u_bus, 10);
        print_str(" | ");
        print_col_s32_dec(10 * (int32_t)u_shunt, 12);
        print_str(" | ");
        print_col_s32_dec(10 * i_shunt, 8);
        print_str(" | ");
        print_col_u32_dec(100 * p, 6);
        print_str(" |\n");
    }

    return 0;
}
