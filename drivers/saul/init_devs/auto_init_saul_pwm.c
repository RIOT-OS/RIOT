/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of PWM pins directly mapped to SAUL reg
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * When this module is used, any PWM device assigned inside the configuration
 * structs inside `pwm_params.h` in the @ref saul_pwm_dimmer_params_t and @ref
 * saul_pwm_rgb_params_t is initialized at startup for 8-bit dimming at about
 * 1kHz, and the indicated channels are exposed via SAUL.
 *
 * @}
 */

#include <inttypes.h>
#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "pwm_params.h"
#include "periph/pwm.h"

#if !defined(SAUL_PWM_FREQ)
#define SAUL_PWM_FREQ SAUL_PWM_FREQ_DEFAULT
#endif

/**
 * @brief   Define the number of configured dimmers
 */
#ifndef SAUL_PWM_NO_DIMMER
#define SAUL_PWM_DIMMER_NUMOF ARRAY_SIZE(saul_pwm_dimmer_params)
#else
#define SAUL_PWM_DIMMER_NUMOF 0
static const saul_pwm_dimmer_params_t saul_pwm_dimmer_params[0];
#endif

/**
 * @brief   Define the number of configured RGB LEDs
 */
#ifndef SAUL_PWM_NO_RGB
#define SAUL_PWM_RGB_NUMOF ARRAY_SIZE(saul_pwm_rgb_params)
#else
#define SAUL_PWM_RGB_NUMOF 0
static const saul_pwm_rgb_params_t saul_pwm_rgb_params[0];
#endif

/**
 * @brief   Memory for the registry RGB LED entries
 */
/* The static variable will be unused in the 0 case and thus not emitted. */
static saul_reg_t saul_reg_entries_rgb[SAUL_PWM_RGB_NUMOF];

/**
 * @brief   Memory for the registry dimmer entries
 */
/* The static variable will be unused in the 0 case and thus not emitted. */
static saul_reg_t saul_reg_entries_dimmer[SAUL_PWM_DIMMER_NUMOF];

/**
 * @brief   Reference to the driver for single-channel dimmers
 */
extern saul_driver_t dimmer_saul_driver;

/**
 * @brief   Reference to the driver for RGB LEDs
 */
extern saul_driver_t rgb_saul_driver;

/**
 * Configure a PWM driver for LED output (1kHz, 8bit)
 */
static int configure(pwm_t dev)
{
    LOG_DEBUG("[auto_init_saul] initializing PWM %u for LED operation,", dev);
    uint32_t freq = pwm_init(dev, PWM_LEFT, SAUL_PWM_FREQ, saul_pwm_resolution);
    LOG_DEBUG(" actual frequency %" PRIu32 ",\n", freq);
    return freq != 0 ? 0 : -ENOTSUP;
}

/**
 * Configure the PWM driver at the given index (inside saul_pwm_dimmer_params,
 * overflowing into saul_pwm_rgb_params) unless that device came up previously,
 * in which case the function returns without any action.
 * */
static int configure_on_first_use(pwm_t dev, unsigned index)
{
    /* Work around -Werror=type-limits that would otherwise trigger */
    unsigned dimmer_numof = SAUL_PWM_DIMMER_NUMOF;
    for (unsigned i = 0; i < dimmer_numof; ++i) {
        pwm_t currentdev = saul_pwm_dimmer_params[i].channel.dev;
        if (currentdev == dev) {
            if (i == index) {
                return configure(dev);
            }
            return 0;
        }
    }

    /* Work around -Werror=type-limits that would otherwise trigger */
    unsigned rgb_numof = SAUL_PWM_RGB_NUMOF;
    for (unsigned i = 0; i < rgb_numof; ++i) {
        for (int j = 0; j < 3; ++j) {
            unsigned index = SAUL_PWM_DIMMER_NUMOF + i * 3 + j;
            pwm_t currentdev = saul_pwm_rgb_params[i].channels[j].dev;
            if (currentdev == dev) {
                if (i == index) {
                    return configure(dev);
                }
                return 0;
            }
        }
    }
    return -ENOENT;
}

void auto_init_saul_pwm(void)
{
    /* Work around -Werror=type-limits that would otherwise trigger */
    unsigned dimmer_numof = SAUL_PWM_DIMMER_NUMOF;
    for (unsigned i = 0; i < dimmer_numof; i++) {
        const saul_pwm_dimmer_params_t *p = &saul_pwm_dimmer_params[i];

        LOG_DEBUG("[auto_init_saul] initializing dimmer #%u\n", i);

        saul_reg_entries_dimmer[i].dev = (void*)p;
        saul_reg_entries_dimmer[i].name = p->name;
        saul_reg_entries_dimmer[i].driver = &dimmer_saul_driver;

        int err = configure_on_first_use(p->channel.dev, i);
        if (err != 0) {
            LOG_ERROR(
                "[auto_init_saul] Error initializing device for dimmer #%u\n",
                i);
            /* not `continue`ing: we could run into this on a non-first use and
             * then we couldn't break either */
        }
        /* set initial dark state */
        phydat_t s;
        s.val[0] = 0;
        saul_reg_entries_dimmer[i].driver->write(p, &s);
        /* add to registry */
        saul_reg_add(&(saul_reg_entries_dimmer[i]));
    }

    /* Work around -Werror=type-limits that would otherwise trigger */
    unsigned rgb_numof = SAUL_PWM_RGB_NUMOF;
    for (unsigned i = 0; i < rgb_numof; i++) {
        const saul_pwm_rgb_params_t *p = &saul_pwm_rgb_params[i];

        LOG_DEBUG("[auto_init_saul] initializing RGB #%u\n", i);

        saul_reg_entries_rgb[i].dev = (void*)p;
        saul_reg_entries_rgb[i].name = p->name;
        saul_reg_entries_rgb[i].driver = &rgb_saul_driver;

        for (int j = 0; j < 3; ++j) {
            unsigned index = SAUL_PWM_DIMMER_NUMOF + i * 3 + j;
            int err = configure_on_first_use(p->channels[j].dev, index);
            if (err != 0) {
                LOG_ERROR(
                    "[auto_init_saul] Error initializing device for RGB #%u/%u\n",
                    i, j);
                /* not `continue`ing: we could run into this on a non-first use and
                 * then we couldn't break either */
            }
        }
        /* set initial dark state */
        phydat_t s;
        s.val[0] = 0;
        s.val[1] = 0;
        s.val[2] = 0;
        saul_reg_entries_rgb[i].driver->write(p, &s);
        /* add to registry */
        saul_reg_add(&(saul_reg_entries_rgb[i]));
    }
}
