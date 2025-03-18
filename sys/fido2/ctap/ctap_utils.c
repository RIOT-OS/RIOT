/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap_utils
 * @{
 * @file
 *
 * @author     Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#include "ztimer.h"

#include "fido2/ctap.h"
#include "fido2/ctap/ctap_utils.h"

#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
#include "periph/gpio.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief Flag holding information if user is present or not
 */
static bool _user_present = false;

/**
 * @brief GPIO pin to use for user presence test
 */
static gpio_t _pin;

/**
 * @brief Button callback function
 */
static void _gpio_cb(void *arg);

ctap_status_code_t fido2_ctap_utils_init_gpio_pin(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank)
{
    if (gpio_init_int(pin, mode, flank, _gpio_cb, NULL) < 0) {
        return CTAP1_ERR_OTHER;
    }

    _pin = pin;

    return CTAP2_OK;
}

ctap_status_code_t fido2_ctap_utils_user_presence_test(void)
{
    int ret;

    gpio_irq_enable(_pin);

    fido2_ctap_utils_wait_for_user_presence();

    ret = _user_present ? CTAP2_OK : CTAP2_ERR_ACTION_TIMEOUT;

    gpio_irq_disable(_pin);

    _user_present = false;
    return ret;
}

static void _gpio_cb(void *arg)
{
    (void)arg;
    _user_present = true;
}

void fido2_ctap_utils_wait_for_user_presence(void)
{
    uint32_t start = ztimer_now(ZTIMER_MSEC);
    uint32_t diff = 0;
    uint32_t delay = 500;

    while (!_user_present && diff < CTAP_UP_TIMEOUT) {
        if (!IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_LED)) {
    #ifdef LED0_TOGGLE
            LED0_TOGGLE;
    #endif
    #ifdef LED1_TOGGLE
            LED1_TOGGLE;
    #endif
    #ifdef LED2_TOGGLE
            LED2_TOGGLE;
    #endif
    #ifdef LED3_TOGGLE
            LED3_TOGGLE;
    #endif
        }
        ztimer_sleep(ZTIMER_MSEC, delay);
        diff = ztimer_now(ZTIMER_MSEC) - start;
    }

    if (!IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_LED)) {
    #ifdef LED0_TOGGLE
        LED0_OFF;
    #endif
    #ifdef LED1_TOGGLE
        LED1_OFF;
    #endif
    #ifdef LED2_TOGGLE
        LED2_OFF;
    #endif
    #ifdef LED3_TOGGLE
        LED3_OFF;
    #endif
    }
}
