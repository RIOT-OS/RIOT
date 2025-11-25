/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    app_spectrum_scanner
 * @brief       Scanner application to find free channels
 * @{
 *
 * @file
 * @brief       Spectrum scanner application for RIOT
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "fmt.h"
#include "thread.h"
#include "xtimer.h"
#include "net/ieee802154.h"
#include "net/gnrc.h"

/* Scanning interval */
#define INTERVAL (500U * US_PER_MS)

/**
 * @brief Measure the radio energy spectrum and print on stdout
 *
 * Algorithm description:
 *
 * The process will repeat as many measurements as possible during the
 * measurement interval, before the average power is computed. This reduces the
 * noise in the measurement and will yield a better image of what the radio
 * environment contains.
 *
 * Still, 122 measurements per second (frdm-kw41z) and 128 us per measurement
 * will only give a time coverage of about 1.5%, but because the measurements are
 * spread out over time they should still give a good representation of which
 * channels are free.
 *
 * Note that because the ED values are given in decibels, the average radio
 * power is not the same as the arithmetic mean of the ED measurements. To
 * compute the average of the dB measurements this algorithm requires both
 * logarithm and exponentiation, quite heavy operations on the kinds of CPUs
 * that RIOT targets. Increasing the CPU clock frequency may therefore reduce
 * the noise in the output, because of the more frequent energy measurements
 * possible.
 */
void spectrum_scanner(unsigned long interval_us)
{
    size_t netif_numof = gnrc_netif_numof();

    /* Using expf(x) (natural exponent) gives quicker computations on Cortex-M0+,
     * compared to using powf(10, x). */
    /*
     * This was optimized by testing different combinations of expf, powf, logf, log10f:
     *
     * functions used | measurement iterations per 0.5 s on reference system (frdm-kw41z)
     * ------------------------------------------------------------------
     * expf, logf     | 64
     * powf, log10f   | 46
     * expf, log10f   | 61
     * no-op (baseline) | 83 (but the measurements are useless)
     */

    while (1) {
        /* Stack optimization, statically allocate this buffer */
        float ed_average[netif_numof][IEEE802154_CHANNEL_MAX + 1];

        memset(ed_average, 0, sizeof(ed_average));

        uint64_t last_wakeup = xtimer_now_usec64();
        uint64_t target = last_wakeup + interval_us;
        /* We spin and try to do as many measurements as possible in the
         * interval time */
        unsigned int count = 0;
        do {
            gnrc_netif_t *netif = NULL;
            for (unsigned int k = 0; (netif = gnrc_netif_iter(netif)); k++) {
                for (unsigned int ch = IEEE802154_CHANNEL_MIN; ch <= IEEE802154_CHANNEL_MAX; ++ch) {
                    uint16_t tmp_ch = ch;
                    int res;
                    res = gnrc_netapi_set(netif->pid, NETOPT_CHANNEL, 0, &tmp_ch, sizeof(uint16_t));
                    if (res < 0) {
                        continue;
                    }
                    netopt_enable_t tmp;
                    /* Perform CCA to update ED level */
                    res = gnrc_netapi_get(netif->pid, NETOPT_IS_CHANNEL_CLR, 0, &tmp, sizeof(netopt_enable_t));
                    if (res < 0) {
                        continue;
                    }
                    int8_t level = 0;
                    res = gnrc_netapi_get(netif->pid, NETOPT_LAST_ED_LEVEL, 0, &level, sizeof(int8_t));
                    if (res < 0) {
                        continue;
                    }
                    /* Convert dB to pseudo-energy before summing together the
                     * measurements. "Pseudo" because we use the natural
                     * exponential function e^x instead of computing 10^x which
                     * would be required if we needed the real measured energy.
                     * There is no need to know the real energy level because we
                     * will be converting back to dB again before printing. */
                    ed_average[k][ch] += expf((float)level / 128.f);
                }
            }
            ++count;
            thread_yield();
        } while (xtimer_now_usec64() < target);
        for (unsigned int k = 0; k < netif_numof; ++k) {
            print("[", 1);
            print_u32_dec(k);
            print(", ", 2);
            print_u64_dec(target);
            print(", ", 2);
            print_u32_dec(count);
            print("] ", 2);
            for (unsigned int ch = IEEE802154_CHANNEL_MIN; ch <= IEEE802154_CHANNEL_MAX; ++ch) {
                /* Compute the average pseudo-energy and convert back to dB */
                ed_average[k][ch] = logf(ed_average[k][ch] / count) * 128.f;
                print_u32_dec(ch);
                print(": ", 2);
                print_float(ed_average[k][ch], 4);
                print(", ", 2);
            }
            print("\n", 1);
        }
    }
}

int main(void)
{
    puts("RIOT scanner application");

    spectrum_scanner(INTERVAL);
    return 0;
}
