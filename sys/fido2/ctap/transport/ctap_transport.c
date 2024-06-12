/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap_transport
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#include "event/timeout.h"

#include "fido2/ctap/transport/ctap_transport.h"
#include "fido2/ctap/ctap.h"

#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
#include "usb/usbus.h"
#include "usb/usbus/hid_io.h"
#include "fido2/ctap/transport/hid/ctap_hid.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
/**
 * @brief CTAPHID timeout handler
 */
static void _ctap_hid_timeout_cb(event_t *arg);
/**
 * @brief CTAPHID timeout event
 */
static event_t _ctap_hid_timeout_event = { .handler = _ctap_hid_timeout_cb };

/**
 * @brief CTAPHID event_timeout object
 */
static event_timeout_t _ctap_hid_event_timeout;
#endif

/**
 * @brief CTAP stack
 */
static char _ctap_stack[CTAP_STACKSIZE];

/**
 * @brief CTAP transport event queue
 */
static event_queue_t _queue;

static void *_event_loop(void *arg)
{
    (void)arg;
    int ret;

    ret = fido2_ctap_init();

    if (ret < 0) {
        return NULL;
    }

    event_queue_init(&_queue);

#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
    event_timeout_ztimer_init(&_ctap_hid_event_timeout, ZTIMER_MSEC, &_queue,
                              &_ctap_hid_timeout_event);
    event_timeout_set(&_ctap_hid_event_timeout, CTAP_HID_TRANSACTION_TIMEOUT_MS);
#endif

    event_loop(&_queue);

    return NULL;
}

#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
static void _ctap_hid_timeout_cb(event_t *arg)
{
    (void)arg;
    fido2_ctap_transport_hid_check_timeouts();
    event_timeout_set(&_ctap_hid_event_timeout, CTAP_HID_TRANSACTION_TIMEOUT_MS);
}
#endif

void fido2_ctap_transport_init(void)
{
#if IS_USED(MODULE_FIDO2_CTAP_TRANSPORT_HID)
    fido2_ctap_transport_hid_init(&_queue);
#endif

    int ret = thread_create(_ctap_stack, sizeof(_ctap_stack), CTAP_TRANSPORT_PRIO,
                            0, _event_loop, NULL,
                            "fido2_ctap_transport_loop");

    (void)ret;
    assert(ret > 0);
}
