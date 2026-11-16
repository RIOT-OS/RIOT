/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>
#include <errno.h>

#include "kernel_defines.h"
#include "net/ieee802154/mac.h"
#include "net/ieee802154/radio.h"

#include "init_devs.h"

#ifdef MODULE_EVENT_THREAD
#include "event/thread.h"
extern void auto_init_event_thread(void);
#endif

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_ESP_IEEE802154
#include "esp_ieee802154_hal.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#ifdef MODULE_SOCKET_ZEP
#include "socket_zep.h"
#include "socket_zep_params.h"
#endif

#if defined(MODULE_KW2XRF) || defined(MODULE_MRF24J40)
#include "bhp/event.h"
#endif

#ifdef MODULE_KW2XRF
#include "kw2xrf.h"
#include "kw2xrf_params.h"
#define KW2XRF_NUM   ARRAY_SIZE(kw2xrf_params)
static kw2xrf_t kw2xrf_dev[KW2XRF_NUM];
static bhp_event_t kw2xrf_bhp[KW2XRF_NUM];
#endif

#ifdef MODULE_MRF24J40
#include "mrf24j40.h"
#include "mrf24j40_params.h"
#define MRF24J40_NUM    ARRAY_SIZE(mrf24j40_params)
static mrf24j40_t mrf24j40_dev[MRF24J40_NUM];
static bhp_event_t mrf24j40_bhp[MRF24J40_NUM];
#endif

int ieee802154_mac_test_init_devs(ieee802154_dev_t *radio,
                                  ieee802154_dev_type_t *dev_type)
{
    if (!radio || !dev_type) {
        return -EINVAL;
    }

#ifdef MODULE_EVENT_THREAD
    auto_init_event_thread();
#endif

    *dev_type = IEEE802154_DEV_TYPE_INVALID;

#ifdef MODULE_CC2538_RF
    cc2538_rf_hal_setup(radio);
    cc2538_init();
    *dev_type = IEEE802154_DEV_TYPE_CC2538_RF;
    return 0;
#endif

#ifdef MODULE_ESP_IEEE802154
    esp_ieee802154_setup(radio);
    esp_ieee802154_init();
    *dev_type = IEEE802154_DEV_TYPE_ESP_IEEE802154;
    return 0;
#endif

#ifdef MODULE_NRF802154
    nrf802154_hal_setup(radio);
    nrf802154_init();
    *dev_type = IEEE802154_DEV_TYPE_NRF802154;
    return 0;
#endif

#ifdef MODULE_KW2XRF
    if (KW2XRF_NUM > 0) {
        const kw2xrf_params_t *p = &kw2xrf_params[0];
        bhp_event_init(&kw2xrf_bhp[0], EVENT_PRIO_HIGHEST,
                       &kw2xrf_radio_hal_irq_handler, radio);
        kw2xrf_init(&kw2xrf_dev[0], p, radio, bhp_event_isr_cb, &kw2xrf_bhp[0]);
        *dev_type = IEEE802154_DEV_TYPE_KW2XRF;
        return 0;
    }
#endif

#ifdef MODULE_SOCKET_ZEP
    if (SOCKET_ZEP_MAX > 0) {
        static socket_zep_t _socket_zeps[SOCKET_ZEP_MAX];
        socket_zep_hal_setup(&_socket_zeps[0], radio);
        socket_zep_setup(&_socket_zeps[0], &socket_zep_params[0]);
        *dev_type = IEEE802154_DEV_TYPE_SOCKET_ZEP;
        return 0;
    }
#endif

#ifdef MODULE_MRF24J40
    if (MRF24J40_NUM > 0) {
        const mrf24j40_params_t *p = &mrf24j40_params[0];
        bhp_event_init(&mrf24j40_bhp[0], EVENT_PRIO_HIGHEST,
                       &mrf24j40_radio_irq_handler, radio);
        mrf24j40_init(&mrf24j40_dev[0], p, radio, bhp_event_isr_cb,
                      &mrf24j40_bhp[0]);
        *dev_type = IEEE802154_DEV_TYPE_MRF24J40;
        return 0;
    }
#endif

    puts("Radio is either not supported or not present");
    return -ENODEV;
}
