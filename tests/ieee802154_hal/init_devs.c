/*
 * Copyright (C) 2020 HAW Hamburg
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
 * @brief       Test application for ieee802154_hal
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include "assert.h"
#include "kernel_defines.h"
#include "net/ieee802154/radio.h"
#include "common.h"
#include "bhp/event.h"

#ifdef MODULE_CC2538_RF
#include "cc2538_rf.h"
#endif

#ifdef MODULE_NRF802154
#include "nrf802154.h"
#endif

#ifdef MODULE_SOCKET_ZEP
#include "socket_zep.h"
#include "socket_zep_params.h"
#endif

#ifdef MODULE_KW2XRF
#include "kw2xrf.h"
#include "kw2xrf_params.h"
#include "event/thread.h"
#define KW2XRF_NUM   ARRAY_SIZE(kw2xrf_params)
extern void auto_init_event_thread(void);
static kw2xrf_t kw2xrf_dev[KW2XRF_NUM];
static bhp_event_t kw2xrf_bhp[KW2XRF_NUM];

#endif

void ieee802154_hal_test_init_devs(ieee802154_dev_cb_t cb, void *opaque)
{
    /* Call the init function of the device (this should be handled by
     * `auto_init`) */
    ieee802154_dev_t *radio = NULL;
    (void) radio;
#ifdef MODULE_CC2538_RF
    if ((radio = cb(IEEE802154_DEV_TYPE_CC2538_RF, opaque)) ){
        cc2538_rf_hal_setup(radio);
        cc2538_init();
    }
#endif

#ifdef MODULE_NRF802154
    if ((radio = cb(IEEE802154_DEV_TYPE_NRF802154, opaque)) ){
        nrf802154_hal_setup(radio);
        nrf802154_init();
    }
#endif

#ifdef MODULE_KW2XRF
    auto_init_event_thread();
    if ((radio = cb(IEEE802154_DEV_TYPE_KW2XRF, opaque)) ){
        for (unsigned i = 0; i < KW2XRF_NUM; i++) {
            const kw2xrf_params_t *p = &kw2xrf_params[i];
            bhp_event_init(&kw2xrf_bhp[i], EVENT_PRIO_HIGHEST, &kw2xrf_radio_hal_irq_handler, radio);
            kw2xrf_init(&kw2xrf_dev[i], p, radio, bhp_event_isr_cb, &kw2xrf_bhp[i]);
            break;
        }
    }
#endif

#ifdef MODULE_SOCKET_ZEP
    static socket_zep_t _socket_zeps[SOCKET_ZEP_MAX];
    if ((radio = cb(IEEE802154_DEV_TYPE_SOCKET_ZEP, opaque)) ){
        socket_zep_hal_setup(&_socket_zeps[0], radio);
        socket_zep_setup(&_socket_zeps[0], &socket_zep_params[0]);
    }
#endif
}
