/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2014 PHYTEC Messtechnik GmbH
 *               2015 Eistec AB
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @brief       SPI bus scaler computation
 *
 * This helper tool calculates the needed SPI scaler values for a given APB bus
 * clock speed. The result of the computation must be placed in a board's
 * periph_conf.h for quick reference by the SPI drivers.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Targeted SPI bus speed values (pre-defined by RIOT)
 */
static uint32_t targets[] = { 100000, 400000, 1000000, 5000000, 10000000 };

/**
 * @brief Helper function for finding optimal baud rate scalers.
 *
 * Find the prescaler and scaler settings that will yield a clock frequency
 * as close as possible (but not above) the target frequency, given the module
 * runs at module_clock Hz.
 *
 * Hardware properties (Baud rate configuration):
 * Possible prescalers: 2, 3, 5, 7
 * Possible scalers: 2, 4, 6 (sic!), 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768
 *
 * SCK baud rate = (f_BUS/PBR) x [(1+DBR)/BR]
 *
 * where PBR is the prescaler, BR is the scaler, DBR is the Double BaudRate bit.
 *
 * @note We are not using the DBR bit because it may affect the SCK duty cycle.
 *
 * @param module_clock Module clock frequency (e.g. F_BUS)
 * @param target_clock Desired baud rate
 * @param closest_prescaler pointer where to write the optimal prescaler index.
 * @param closest_scaler pointer where to write the optimal scaler index.
 *
 * @return The actual achieved frequency on success
 * @return Less than 0 on error.
 */

static long find_closest_baudrate_scalers(const uint32_t module_clock, const long target_clock,
                                          uint8_t *closest_prescaler, uint8_t *closest_scaler)
{
    uint8_t i;
    uint8_t k;
    long freq;
    static const uint8_t num_scalers = 16;
    static const uint8_t num_prescalers = 4;
    static const int br_scalers[16] = {
        2,     4,     6,     8,    16,    32,    64,   128,
        256,   512,  1024,  2048,  4096,  8192, 16384, 32768
    };
    static const int br_prescalers[4] = {2, 3, 5, 7};

    long closest_frequency = -1;

    /* Test all combinations until we arrive close to the target clock */
    for (i = 0; i < num_prescalers; ++i) {
        for (k = 0; k < num_scalers; ++k) {
            freq = module_clock / (br_scalers[k] * br_prescalers[i]);

            if (freq <= target_clock) {
                /* Found closest lower frequency at this prescaler setting,
                 * compare to the best result */
                if (closest_frequency < freq) {
                    closest_frequency = freq;
                    *closest_scaler = k;
                    *closest_prescaler = i;
                }

                break;
            }
        }
    }

    if (closest_frequency < 0) {
        /* Error, no solution found, this line is never reachable with current
         * hardware settings unless a _very_ low target clock is requested.
         * (scaler_max * prescaler_max) = 229376 => target_min@100MHz = 435 Hz*/
        return -1;
    }

    return closest_frequency;
}

/**
 * @brief Helper function for finding optimal delay scalers.
 *
 * Find the prescaler and scaler settings that will yield a delay timing
 * as close as possible (but not shorter than) the target delay, given the
 * module runs at module_clock Hz.
 *
 * Hardware properties (delay configuration):
 * Possible prescalers: 1, 3, 5, 7
 * Possible scalers: 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536
 *
 * delay = (1/f_BUS) x prescaler x scaler
 *
 * Because we want to do this using only integers, the target_freq parameter is
 * the reciprocal of the delay time.
 *
 * @param module_clock Module clock frequency (e.g. F_BUS)
 * @param target_freq Reciprocal (i.e. 1/t [Hz], frequency) of the desired delay time.
 * @param closest_prescaler pointer where to write the optimal prescaler index.
 * @param closest_scaler pointer where to write the optimal scaler index.
 *
 * @return The actual achieved frequency on success
 * @return Less than 0 on error.
 */
static long find_closest_delay_scalers(const uint32_t module_clock, const long target_freq,
                                      uint8_t *closest_prescaler, uint8_t *closest_scaler)
{
    uint8_t i;
    uint8_t k;
    long freq;
    int prescaler;
    int scaler;
    static const uint8_t num_scalers = 16;
    static const uint8_t num_prescalers = 4;

    long closest_frequency = -1;

    /* Test all combinations until we arrive close to the target clock */
    for (i = 0; i < num_prescalers; ++i) {
        for (k = 0; k < num_scalers; ++k) {
            prescaler = (i * 2) + 1;
            scaler = (1 << (k + 1)); /* 2^(k+1) */
            freq = module_clock / (prescaler * scaler);

            if (freq <= target_freq) {
                /* Found closest lower frequency at this prescaler setting,
                 * compare to the best result */
                if (closest_frequency < freq) {
                    closest_frequency = freq;
                    *closest_scaler = k;
                    *closest_prescaler = i;
                }

                break;
            }
        }
    }

    if (closest_frequency < 0) {
        /* Error, no solution found, this line is never reachable with current
         * hardware settings unless a _very_ low target clock is requested.
         * (scaler_max * prescaler_max) = 458752 */
        return -1;
    }

    return closest_frequency;
}


int main(int argc, char **argv)
{
    uint32_t modclk;
    int i;

    if (argc != 2) {
        printf("usage: %s <module clock>\n", argv[0]);
        return 1;
    }

    modclk = (uint32_t)atoi(argv[1]);
    if (modclk == 0) {
        printf("error: invalid input value\n");
        return 1;
    }

    printf("\nCalculating SPI clock scalers for a module clock of: %iHz\n\n",
            (int)modclk);


    puts("static const uint32_t spi_clk_config[] = {");

    for (i = 0; i < (sizeof(targets) / sizeof(targets[0])); i++) {
        uint8_t tmp, ptmp;
        long res;
        /* bus clock */
        res = find_closest_baudrate_scalers(modclk, targets[i], &ptmp, &tmp);
        if (res < 0) {
            puts("error: no applicable bus clock scalers could be found!");
            return 1;
        }
        puts("    (");
        printf("        SPI_CTAR_PBR(%i) | SPI_CTAR_BR(%i) |          /* -> %iHz */\n",
               (int)ptmp, (int)tmp, (int)res);

        /* t_csc: chip select to fist clock signal delay */
        if (find_closest_delay_scalers(modclk, targets[i], &ptmp, &tmp) < 0) {
            puts("error: no applicable delay values for t_csc found\n");
            return 1;
        }
        printf("        SPI_CTAR_PCSSCK(%i) | SPI_CTAR_CSSCK(%i) |\n", (int)ptmp, (int)tmp);

        /* t_asc: delay after last clock signal to release of chip select */
        if (find_closest_delay_scalers(modclk, targets[i], &ptmp, &tmp) < 0) {
            puts("error: no applicable delay values for t_asc found\n");
            return 1;
        }
        printf("        SPI_CTAR_PASC(%i) | SPI_CTAR_ASC(%i) |\n", (int)ptmp, (int)tmp);

        /* t_psc: delay between release and next assertion of chip select */
        if (find_closest_delay_scalers(modclk, targets[i], &ptmp, &tmp) < 0) {
            puts("error: no applicable delay values for t_csc found\n");
            return 1;
        }
        printf("        SPI_CTAR_PDT(%i) | SPI_CTAR_DT(%i)\n", (int)ptmp, (int)tmp);

        if (i == (sizeof(targets) / sizeof(targets[0])) - 1) {
            puts("    )");
        }
        else {
            puts("    ),");
        }
    }
    puts("};");

    return 0;
}
