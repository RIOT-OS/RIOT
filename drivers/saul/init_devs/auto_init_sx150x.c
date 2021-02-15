/*
 * Copyright (C) 2021 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of SX150x devices
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 * Unlike other auto-init'ed SAUL devices, this does not allocate on its
 * own, for other parts of the board may use it as well (eg. in power
 * management). It does, however, do the basic SX150x initialization, for
 * the board can't do that (because board_init can not do the asynchronous
 * communication needed for initialization.
 *
 * If a board ever actually implements non-SAUL access and needs that to be
 * present even when SAUL is not enabled, then the initialization will need to
 * be moved into a dedicated non-SAUL-gated auto-init routine that's pulled in
 * both by this and the other accessor.
 *
 * @}
 */

#include "sx150x_params.h"

struct rgbs {
    sx150x_t *dev;
    /* Pins for R, G and B */
    unsigned pins[3];
};

static const struct rgbs rgbs[] = SX150X_RGBS;

void auto_init_sx150x(void) {
    /* Initialize I/O expander */

    const sx150x_params_t params[] = SX150X_PARAMS;
    sx150x_t *const devs[] = SX150X_DEVS;

    const int nparams = sizeof(params) / sizeof(params[0]);
    const int ndevs = sizeof(devs) / sizeof(devs[0]);
    const int nrgbs = sizeof(rgbs) / sizeof(rgbs[0]);

    assert(nparams == ndevs);
    for (int i = 0; i < ndevs; ++i) {
        sx150x_init(devs[i], &params[i]);
    }

    for (int d = 0; d < nrgbs; ++d) {
        for (int p = 0; p < 3; ++p) {
            sx150x_gpio_init(rgbs[d].dev, rgbs[d].pins[p], GPIO_OUT);
            for (int j = 0; j < 2; ++j) {
                sx150x_gpio_set(rgbs[d].dev, rgbs[d].pins[p]);
                for(volatile int i = 0; i < 1000000; ++i) { }
                sx150x_gpio_clear(rgbs[d].dev, rgbs[d].pins[p]);
                for(volatile int i = 0; i < 1000000; ++i) { }
            }

            sx150x_pwm_init(rgbs[d].dev, rgbs[d].pins[p]);
            for (int i = 0; i < 255; i += 10) {
                for(volatile int t = 0; t < 100000; ++t) { }
                sx150x_pwm_set(rgbs[d].dev, rgbs[d].pins[p], i);
            }
            for (int i = 255; i > 0; i -= 10) {
                for(volatile int t = 0; t < 100000; ++t) { }
                sx150x_pwm_set(rgbs[d].dev, rgbs[d].pins[p], i);
            }
        }
    }
}
