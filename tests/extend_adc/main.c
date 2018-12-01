/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
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
 * @brief       ADC extension test routine
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <stdio.h>

#include "extend/adc.h"
#include "periph/adc.h"

/* ADC extension test functions */
int test_adc_init(void *dev, adc_t chn);
int test_adc_sample(void *dev, adc_t chn, adc_res_t res);
unsigned int test_adc_channels(void *dev);

/* ADC extension test driver */
const adc_ext_driver_t tests_extend_adc_driver = {
    .init = test_adc_init,
    .sample = test_adc_sample,
    .channels = test_adc_channels,
};

int test_adc_init(void *dev, adc_t chn)
{
    printf("\t%s dev=0x%04x chn=%u\n", __func__, (uint16_t)(uintptr_t)dev, chn);
    return 0;
}

int test_adc_sample(void *dev, adc_t chn, adc_res_t res)
{
    printf("\t%s dev=0x%04x chn=%u res=%u\n",
           __func__, (uint16_t)(uintptr_t)dev, chn, res);
    return 128 * chn;
}

unsigned int test_adc_channels(void *dev)
{
    printf("\t%s dev=0x%04x\n", __func__, (uint16_t)(uintptr_t)dev);
    return 4;
}

int main(void)
{
    adc_t chn;

    puts("ADC extension test routine");
    puts("\nRunning ADC functions for internal device");

    adc_t num = adc_channels(ADC_LINE(0));
    printf("- Number of ADC channels: %d\n", num);

    for (chn = 0; chn < num; chn++) {
        printf("- Init ADC channel %u\n", chn);
        if (adc_init(ADC_LINE(chn))) {
            puts("[FAILED]");
            return 1;
        }
        int sample = adc_sample(ADC_LINE(chn), ADC_RES_10BIT);
        printf("- Sample of ADC channel %u: %d\n", chn, sample);
    }

    puts("\nRunning ADC functions for extension test device");

    num = adc_channels(ADC_EXT_LINE(0, 0));
    printf("- Number of ADC channels: %d\n", num);

    for (chn = 0; chn < num; chn++) {
        printf("- Init ADC channel %u\n", chn);
        if (adc_init(ADC_EXT_LINE(0, chn))) {
            puts("[FAILED]");
            return 1;
        }
        int sample = adc_sample(ADC_EXT_LINE(0, chn), ADC_RES_10BIT);
        printf("- Sample of ADC channel %u: %d\n", chn, sample);
    }

    puts("\nRunning ADC functions for extension notsup device");
    puts("(they should not print output)");

    for (chn = 0; chn < num; chn++) {
        printf("- notsup adc.h functions on channel %u\n",
            (unsigned int)chn);

        if (!adc_init(ADC_EXT_LINE(1, chn))) {
            puts("[FAILED]");
            return 1;
        }

        if (adc_sample(ADC_EXT_LINE(1, chn), ADC_RES_10BIT) >= 0) {
            puts("[FAILED]");
            return 1;
        }
    }

    puts("\nChecking that all channels in range have init error using notsup");
    puts("(lack of init error implies improper redirection)");

    for (chn = 0; chn <= ADC_EXT_LINE_MASK; chn +=16) {
        if (adc_init(ADC_EXT_LINE(1, chn)) >= 0) {
            printf("init succeeded on channel %u\n", chn);
            puts("[FAILED]");
            return 1;
        }
    }

    puts("[SUCCESS]");

    return 0;
}
