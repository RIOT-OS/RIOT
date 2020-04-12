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

#include "adc_ng.h"
#include "fmt.h"
#include "kernel_defines.h"
#include "periph/adc.h"
#include "xtimer.h"

#ifndef ADC_NUM
#define ADC_NUM             (0)
#endif

#ifndef ADC_CHANNEL
#define ADC_CHANNEL         (0)
#endif

#ifndef PERIPH_ADC_LINE
#define PERIPH_ADC_LINE     (0)
#endif

#ifndef PERIPH_ADC_RES
#define PERIPH_ADC_RES      (ADC_RES_10BIT)
#endif

static int32_t buf[1024];

static void print_stats(uint32_t total)
{
    uint32_t single = (total + ARRAY_SIZE(buf) / 2) / ARRAY_SIZE(buf);
    uint32_t rate = (ARRAY_SIZE(buf) * US_PER_SEC + total / 2) / total;

    print_str("Performed ");
    print_u32_dec(ARRAY_SIZE(buf));
    print_str(" conversions in ");
    print_u32_dec(total);
    print_str(" µs (");
    print_u32_dec(single);
    print_str(" µs per conversion)\nConversion rate: ");
    char numbuf[16];
    print(numbuf, fmt_s32_dfp(numbuf, rate, -3));
    print_str(" kHz\n");
}

static void bench_adc_ng(uint8_t res)
{
    int16_t ref = ADC_NG_MAX_REF;

    int retval = adc_ng_init(ADC_NUM, ADC_CHANNEL, res, &ref);
    if (retval) {
        print_str("Error: adc_ng_init() failed with ");
        print_s32_dec(retval);
        print_str("\n");
        return;
    }
    print_str("Resolution: ");
    print_u32_dec(res);
    print_str(" bit, reference voltage: ");
    print_s32_dec(ref);
    print_str(" mV\n");

    uint32_t start = xtimer_now_usec();
    retval = adc_ng_burst(ADC_NUM, buf, ARRAY_SIZE(buf));
    uint32_t end = xtimer_now_usec();
    adc_ng_off(ADC_NUM);

    if (retval) {
        print_str("Error: adc_ng_burst() failed with ");
        print_s32_dec(retval);
        print_str("\n");
        return;
    }

    print_str("print_stats\n");
    print_stats(end - start);
}

static void bench_periph_adc(void)
{
    adc_init(PERIPH_ADC_LINE);
    uint32_t start = xtimer_now_usec();
    for (size_t i = 0; i < ARRAY_SIZE(buf); i++) {
        if ((buf[i] = adc_sample(PERIPH_ADC_LINE, PERIPH_ADC_RES)) < 0) {
            print_str("adc_sample() failed\n");
            return;
        }
    }
    uint32_t end = xtimer_now_usec();
    print_stats(end - start);
}

int main(void)
{
    uint8_t res;

    print_str(
        "RIOT ADC NG driver test for burst mode sampling\n"
        "===============================================\n"
    );

    if (!adc_ng_backends[ADC_NUM].driver->burst) {
        print_str("Warning: Driver does not support burst mode\n");
    }

    print_str(
        "\n"
        "periph_adc\n"
        "----------\n"
    );
    bench_periph_adc();

    print_str(
        "\n"
        "ADC NG: Minimum Resolution\n"
        "--------------------------\n"
    );
    res = adc_ng_min_res(ADC_NUM);
    bench_adc_ng(res);

    print_str(
        "\n"
        "ADC NG: Maximum Resolution\n"
        "--------------------------\n"
    );
    res = adc_ng_max_res(ADC_NUM);
    bench_adc_ng(res);

    return 0;
}
