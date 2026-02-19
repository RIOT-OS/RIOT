/*
 * Copyright (C) 2026 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @defgroup     drivers_ieee802154_hal IEEE802.15.4 Radio Hardware Abstraction Layer
 * @ingroup      drivers_netdev
 * @experimental This API is experimental and in an early state - expect
 *               changes!

 * @brief        This is a Hardware Abstraction Layer for IEEE802.15.4 compatible
 *               radios.
 * @{
 *
 * @author       Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/* include available radios */
#ifdef MODULE_CC2538_RF
#  include "cc2538_rf.h"
#  define IEEE802154_RADIO_COUNT 1
#endif

#ifdef MODULE_ESP_IEEE802154
#  include "esp_ieee802154_hal.h"
#  define IEEE802154_RADIO_COUNT 1
#endif

#ifdef MODULE_NRF802154
#  include "nrf802154.h"
#  define IEEE802154_RADIO_COUNT 1
#endif

#ifdef MODULE_SOCKET_ZEP
#  include "socket_zep.h"
#  include "socket_zep_params.h"
static socket_zep_t _socket_zeps[SOCKET_ZEP_MAX];
#  define IEEE802154_RADIO_COUNT SOCKET_ZEP_MAX
#endif

#ifdef MODULE_KW2XRF
#  include "kw2xrf.h"
#  include "kw2xrf_params.h"
#  define KW2XRF_NUM   ARRAY_SIZE(kw2xrf_params)
static kw2xrf_t kw2xrf_dev[KW2XRF_NUM];
static bhp_event_t kw2xrf_bhp[KW2XRF_NUM];
#  define IEEE802154_RADIO_COUNT KW2XRF_NUM
#endif

#ifdef MODULE_MRF24J40
#  include "mrf24j40.h"
#  include "mrf24j40_params.h"
#  define MRF24J40_NUM    ARRAY_SIZE(mrf24j40_params)
static mrf24j40_t mrf24j40_dev[MRF24J40_NUM];
static bhp_event_t mrf24j40_bhp[MRF24J40_NUM];
#  define IEEE802154_RADIO_COUNT MRF24J40_NUM
#endif

#include "event.h"
#include "event/thread.h"

/**
 * @brief Initialize a radio at given index
 *
 * @param[in] dev IEEE802.15.4 device descriptor
 * @param[in] index index of radio (0 <= index < IEEE802154_RADIO_COUNT)
 * @param[in] evq optional event queue, default: EVENT_PRIO_HIGHEST
 *
 * @retval number of initialized interfaces (should always be one)
 */
static inline int ieee802154_radio_init(ieee802154_dev_t *dev, int index, event_queue_t *evq)
{
    (void)dev;
    int res = 0;
    if (!evq) {
        evq = EVENT_PRIO_HIGHEST;
    }
#ifdef IEEE802154_RADIO_COUNT
    assert(index < IEEE802154_RADIO_COUNT);
#  ifdef MODULE_CC2538_RF
    cc2538_rf_hal_setup(dev);
    cc2538_init();
    res++;
#  endif

#  ifdef MODULE_ESP_IEEE802154
    esp_ieee802154_setup(dev);
    esp_ieee802154_init();
    res++;
#  endif

#  ifdef MODULE_NRF802154
    nrf802154_hal_setup(dev);
    nrf802154_init();
    res++;
#  endif

#  ifdef MODULE_KW2XRF
    const kw2xrf_params_t *p = &kw2xrf_params[index];
    bhp_event_init(&kw2xrf_bhp[index], evq, &kw2xrf_radio_hal_irq_handler, dev);
    kw2xrf_init(&kw2xrf_dev[index], p, dev, bhp_event_isr_cb, &kw2xrf_bhp[index]);
    res++;
#  endif

#  ifdef MODULE_SOCKET_ZEP
    socket_zep_hal_setup(&_socket_zeps[index], dev);
    socket_zep_setup(&_socket_zeps[index], &socket_zep_params[index]);
    res++;
#  endif

#  ifdef MODULE_MRF24J40
    const mrf24j40_params_t *p = &mrf24j40_params[index];
    bhp_event_init(&mrf24j40_bhp[index], evq, &mrf24j40_radio_irq_handler, dev);
    mrf24j40_init(&mrf24j40_dev[index], p, dev, bhp_event_isr_cb, &mrf24j40_bhp[index]);
    res++;
#  endif
#endif
    return res;
}

#ifdef __cplusplus
}
#endif

/** @} */
