/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       Compute SPI clock scaler values for STM32x CPUs
 *
 * This helper tool calculates the needed SPI scaler values for the APBx buses.
 * It outputs the values for the possible SPI clock speeds based on the clock
 * speeds of the APB1 and the APB2 bus.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


static int targets[] = { 100000, 400000, 1000000, 5000000, 10000000 };

/**
 * @brief   Find the best fitting divisor value base on target and APB clock
 *
 * The SPI bus clock speed is calculated as follows:
 *
 * clk_bus = clk_apb / 2 ^ (x + 1)
 *
 * In this function we will try to find the x-value, that brings clk_bus as
 * close as possible to the value given in @p target.
 *
 * @param[in] bus       clock speed of the given APB bus
 * @param[in] target    targeted SPI clock speed
 *
 * @return  the closes divisor value to be used in the clock config register
 */
static int find_best(int bus, int target)
{
    int br = -1;
    int tmp = bus;

    do {
        // printf("b: x: %i - tmp: %i\n", br, tmp);
        ++br;
        tmp /= 2;
        if (tmp == target) {
            return br;
        }
        // printf("a: x: %i - tmp: %i\n", br, tmp);
        // printf("  (tmp - target):%i\n", (tmp - target));
    } while ((tmp - target > 0) && (br < 7));

    int old = tmp * 2;
    // printf("(target - tmp):%i, (old - target): %i\n", (target - tmp), (old - target));

    if ((target - tmp) > (old - target)) {
        return (br - 1);
    }
    return br;
}

static int real_clk(int bus, int br)
{
    return bus / (2 << br);
}

int main(int argc, char **argv)
{
    int tnum = sizeof(targets) / sizeof(targets[0]);
    int apb[2];

    if (argc != 3) {
        printf("usage: %s <APB1 clk> <APB2 clk> (in Hz)\n", argv[0]);
        return 1;
    }

    apb[0] = atoi(argv[1]); /* APB1 */
    apb[1] = atoi(argv[2]); /* APB2 */

    if ((apb[0] <= 0) || (apb[1] <= 0)) {
        puts("error: invalid APB clock speed values");
        return 1;
    }

    printf("static const uint8_t spi_divtable[2][%i] = {\n", tnum);

    for (int bus = 0; bus < (sizeof(apb) / sizeof(apb[0])); bus ++) {
        printf("    {       /* for APB%i @ %iHz */\n", (bus + 1), apb[bus]);
        for (int t = 0; t < tnum; t++) {
            int br = find_best(apb[bus], targets[t]);
            printf("        %i%c  /* -> %iHz */\n",
                   br, ((t < (tnum - 1)) ? ',' : ' '), real_clk(apb[bus], br));
        }
        printf("    }%s\n", ((bus == 0) ? "," : ""));
    }
    puts("};");

    return 0;
}
