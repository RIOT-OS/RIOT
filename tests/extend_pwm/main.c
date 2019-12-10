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
 * @brief       PWM extension test routine
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include "extend/pwm.h"
#include "periph/pwm.h"

#define PWM_TMODE        PWM_LEFT
#define PWM_TFREQ        (1000U)
#define PWM_TRES         (1000U)
#define PWM_TVALUE       (PWM_TRES >> 1)

/* PWM extension test functions */
uint32_t test_pwm_init(void *dev, pwm_mode_t mode, uint32_t freq, uint16_t res);
uint8_t test_pwm_channels(void *dev);
void test_pwm_set(void *dev, uint8_t chn, uint16_t val);
void test_pwm_poweron(void *dev);
void test_pwm_poweroff(void *dev);

/* PWM extension test driver */
const pwm_ext_driver_t tests_extend_pwm_driver = {
    .init = test_pwm_init,
    .set = test_pwm_set,
    .poweron = test_pwm_poweron,
    .poweroff = test_pwm_poweroff,
    .channels = test_pwm_channels,
};

uint32_t test_pwm_init(void *dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    printf("\t%s dev 0x%04x, mode %u, freq %"PRIu32", res %u\n",
           __func__, (uint16_t)(uintptr_t)dev, mode, freq, res);
    return freq;
}

uint8_t test_pwm_channels(void *dev)
{
    printf("\t%s dev 0x%04x\n", __func__, (uint16_t)(uintptr_t)dev);
    return 4;
}

void test_pwm_set(void *dev, uint8_t chn, uint16_t val)
{
    printf("\t%s dev 0x%04x, chn %u, val %u\n",
           __func__, (uint16_t)(uintptr_t)dev, chn, val);
}

void test_pwm_poweron(void *dev)
{
    printf("\t%s dev 0x%04x\n", __func__, (uint16_t)(uintptr_t)dev);
}

void test_pwm_poweroff(void *dev)
{
    printf("\t%s dev 0x%04x\n", __func__, (uint16_t)(uintptr_t)dev);
}

int main(void)
{
    puts("PWM extension test routine");

#if MODULE_PERIPH_PWM
    printf("\nRunning PWM functions for onboard devices\n");
    printf("Number of onboard devices: %d\n", PWM_NUMOF);
    for (pwm_t pwm = 0; pwm < PWM_NUMOF; pwm++) {
        printf("- Init PWM device %u\n", pwm);
        if (pwm_init(PWM_DEV(pwm), PWM_TMODE, PWM_TFREQ, PWM_TRES) == 0) {
            puts("[FAILED]");
            return 1;
        }

        uint8_t num = pwm_channels(PWM_DEV(pwm));
        printf("- Number of PWM channels: %u\n", num);

        for (uint8_t chn = 0; chn < num; chn++) {
            printf("- Set PWM channel %u: %u\n", chn, (unsigned)PWM_TVALUE);
            pwm_set(PWM_DEV(pwm), chn, PWM_TVALUE);
        }
        printf("- Poweroff PWM device\n");
        pwm_poweroff(PWM_DEV(pwm));
        printf("- Poweron PWM device\n");
        pwm_poweron(PWM_DEV(pwm));
    }
#else
    puts("\nNo PWM onboard devices");
#endif

    puts("\nRunning PWM functions for extension test device");

    printf("- Init PWM device %u\n", PWM_EXT_DEV(0));
    if (pwm_init(PWM_EXT_DEV(0), PWM_TMODE, PWM_TFREQ, PWM_TRES) == 0) {
        puts("[FAILED]");
        return 1;
    }

    uint8_t num = pwm_channels(PWM_EXT_DEV(0));
    printf("- Number of PWM channels: %u\n", num);

    for (uint8_t chn = 0; chn < num; chn++) {
        printf("- Set PWM channel %u: %u\n", chn, (unsigned)PWM_TVALUE);
        pwm_set(PWM_EXT_DEV(0), chn, PWM_TVALUE);
    }
    printf("- Poweroff PWM device\n");
    pwm_poweroff(PWM_EXT_DEV(0));
    printf("- Poweron PWM device\n");
    pwm_poweron(PWM_EXT_DEV(0));

    puts("\nRunning PWM functions for extension notsup device");
    puts("(they should not print output)");

    if (pwm_init(PWM_EXT_DEV(1), PWM_TMODE, PWM_TFREQ, PWM_TRES) != 0) {
        puts("[FAILED]");
        return 1;
    }
    pwm_set(PWM_EXT_DEV(1), 0, PWM_TVALUE);
    pwm_poweroff(PWM_EXT_DEV(1));
    pwm_poweron(PWM_EXT_DEV(1));

    puts("\n[SUCCESS]");

    return 0;
}
