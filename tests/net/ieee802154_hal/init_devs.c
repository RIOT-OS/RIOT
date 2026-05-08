/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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

#include <stdio.h>
#include "assert.h"
#include "kernel_defines.h"
#include "net/ieee802154/radio.h"
#include "net/ieee802154/init_radio.h"
#include "test_common.h"
#include "bhp/event.h"

#include "event/thread.h"
extern void auto_init_event_thread(void);

void ieee802154_hal_test_init_devs(ieee802154_dev_cb_t cb, void *opaque)
{
    if (IS_USED(MODULE_EVENT_THREAD)) {
        auto_init_event_thread();
    }

    /* Call the init function of the device (this should be handled by
     * `auto_init`) */
    ieee802154_dev_t *radio = NULL;
    (void) radio;
    (void) cb;
    (void) opaque;

    if (RADIOS_NUMOF == 0) {
        puts("Radio is either not supported or not present");
        assert(false);
    }
    ieee802154_dev_type_t radio_type;

#ifdef MODULE_CC2538_RF
    radio_type = IEEE802154_DEV_TYPE_CC2538_RF;
#endif

#ifdef MODULE_ESP_IEEE802154
    radio_type = IEEE802154_DEV_TYPE_ESP_IEEE802154;
#endif

#ifdef MODULE_NRF802154
    radio_type = IEEE802154_DEV_TYPE_NRF802154;
#endif

#ifdef MODULE_KW2XRF
    radio_type = IEEE802154_DEV_TYPE_KW2XRF;
#endif

#ifdef MODULE_SOCKET_ZEP
    radio_type = IEEE802154_DEV_TYPE_SOCKET_ZEP;
#endif

#ifdef MODULE_MRF24J40
    radio_type = IEEE802154_DEV_TYPE_MRF24J40;
#endif
    if ((radio = cb(radio_type, opaque)) ){
        ieee802154_radio_init(radio, 0, NULL);
    }
}
