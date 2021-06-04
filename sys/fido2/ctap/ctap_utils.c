/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */

#include <string.h>

#include "xtimer.h"

#include "fido2/ctap.h"
#include "fido2/ctap/ctap_utils.h"

#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
#if CONFIG_CTAP_HID
#include "fido2/ctap/transport/hid/ctap_transport_hid.h"
#include "periph/gpio.h"
#endif
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

static bool g_user_present = false;

#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_UP)
static void gpio_cb(void *arg);

int fido2_ctap_utils_user_presence_test(void)
{
#ifdef BTN0_PIN
    int ret;
#if CONFIG_CTAP_HID
    fido2_ctap_transport_hid_send_keepalive(CTAP_HID_STATUS_UPNEEDED);
#endif

    if (gpio_init_int(BTN0_PIN, BTN0_MODE, GPIO_FALLING, gpio_cb, NULL) < 0) {
        return CTAP1_ERR_OTHER;
    }

#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_LED)
    fido2_ctap_utils_led_animation();
#endif

    ret = g_user_present ? CTAP2_OK : CTAP2_ERR_ACTION_TIMEOUT;
    gpio_irq_disable(BTN0_PIN);
    g_user_present = false;
    return ret;
#else
    return CTAP1_ERR_OTHER;
#endif /* BTN0_PIN */
}

static void gpio_cb(void *arg)
{
    (void)arg;
    g_user_present = true;
}

void fido2_ctap_utils_led_animation(void)
{
    uint32_t start = xtimer_now_usec();
    uint32_t diff = 0;
    uint32_t delay = (500 * US_PER_MS);

    while (!g_user_present && diff < CTAP_UP_TIMEOUT) {
#ifdef LED0_TOGGLE
        LED0_TOGGLE;
#endif
#ifdef LED1_TOGGLE
        LED1_TOGGLE;
#endif
#ifdef LED3_TOGGLE
        LED3_TOGGLE;
#endif
#ifdef LED2_TOGGLE
        LED2_TOGGLE;
#endif
        xtimer_usleep(delay);
        diff = xtimer_now_usec() - start;
    }

#ifdef LED0_TOGGLE
    LED0_OFF;
#endif
#ifdef LED1_TOGGLE
    LED1_OFF;
#endif
#ifdef LED3_TOGGLE
    LED3_OFF;
#endif
#ifdef LED2_TOGGLE
    LED2_OFF;
#endif
}
#endif /* CONFIG_FIDO2_CTAP_DISABLE_UP */
