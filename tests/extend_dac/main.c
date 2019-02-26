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
 * @brief       DAC extension test routine
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <stdio.h>

#include "extend/dac.h"
#include "periph/dac.h"

/* DAC extension test functions */
int8_t test_dac_init(void *dev, dac_t chn);
void test_dac_set(void *dev, dac_t chn, uint16_t val);
void test_dac_poweron(void *dev, dac_t chn);
void test_dac_poweroff(void *dev, dac_t chn);
unsigned int test_dac_channels(void *dev);

/* DAC extension test driver */
const dac_ext_driver_t tests_extend_dac_driver = {
    .init = test_dac_init,
    .set = test_dac_set,
    .poweron = test_dac_poweron,
    .poweroff = test_dac_poweroff,
    .channels = test_dac_channels,
};

int8_t test_dac_init(void *dev, dac_t chn)
{
    printf("\t%s dev 0x%04x, chn %u\n", __func__, (uint16_t)(uintptr_t)dev, chn);
    return 0;
}

void test_dac_set(void *dev, dac_t chn, uint16_t val)
{
    printf("\t%s dev 0x%04x, chn %u, val %u\n", __func__, (uint16_t)(uintptr_t)dev, chn, val);
}

void test_dac_poweron(void *dev, dac_t chn)
{
    printf("\t%s dev 0x%04x, chn %u\n", __func__, (uint16_t)(uintptr_t)dev, chn);
}

void test_dac_poweroff(void *dev, dac_t chn)
{
    printf("\t%s dev 0x%04x, chn %u\n", __func__, (uint16_t)(uintptr_t)dev, chn);
}

unsigned int test_dac_channels(void *dev)
{
    printf("\t%s dev 0x%04x\n", __func__, (uint16_t)(uintptr_t)dev);
    return 4;
}

int main(void)
{
    puts("DAC extension test routine");
    puts("\nRunning DAC functions for internal device");

    uint16_t value = 1024;

    unsigned int num = dac_channels(DAC_LINE(0));
    printf("- Number of DAC channels: %u\n", num);

    for (unsigned int chn = 0; chn < num; chn++) {
        printf("- Init DAC channel %u\n", chn);
        if (dac_init(DAC_LINE(chn))) {
            puts("[FAILED]");
            return 1;
        }
        printf("- Set DAC channel %u: %u\n", chn, chn * value);
        dac_set(DAC_LINE(chn), chn * value);
        printf("- Poweroff DAC channel %u\n", chn);
        dac_poweroff(DAC_LINE(chn));
        printf("- Poweron DAC channel %u\n", chn);
        dac_poweron(DAC_LINE(chn));
    }

    puts("\nRunning DAC functions for extension test device");

    num = dac_channels(DAC_EXT_LINE(0, 0));
    printf("- Number of DAC channels: %u\n", num);

    for (unsigned int chn = 0; chn < num; chn++) {
        printf("- Init DAC channel %u\n", chn);
        if (dac_init(DAC_EXT_LINE(0, chn))) {
            puts("[FAILED]");
            return 1;
        }
        printf("- Set DAC channel %u: %u\n", chn, chn * value);
        dac_set(DAC_EXT_LINE(0, chn), chn * value);
        printf("- Poweroff DAC channel %u\n", chn);
        dac_poweroff(DAC_EXT_LINE(0, chn));
        printf("- Poweron DAC channel %u\n", chn);
        dac_poweron(DAC_EXT_LINE(0, chn));
    }

    puts("\nRunning DAC functions for extension notsup device");
    puts("(they should not print output)");

    for (unsigned int chn = 0; chn < num; chn++) {
        printf("- notsup dac.h functions on channel %u\n", chn);
        if (!dac_init(DAC_EXT_LINE(1, chn))) {
            puts("[FAILED]");
            return 1;
        }
        dac_set(DAC_EXT_LINE(1, chn), chn * value);
        dac_poweroff(DAC_EXT_LINE(1, chn));
        dac_poweron(DAC_EXT_LINE(1, chn));
    }

    puts("\nChecking that all channels in range have init error using notsup");
    puts("(lack of init error implies improper redirection)");

    for (unsigned int chn = 0; chn <= DAC_EXT_LINE_MASK; chn +=16) {
        if (dac_init(DAC_EXT_LINE(1, chn)) >= 0) {
            printf("init succeeded on channel %u\n", chn);
            puts("[FAILED]");
            return 1;
        }
    }

    puts("[SUCCESS]");

    return 0;
}
