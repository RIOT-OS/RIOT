/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core_twr
 * @{
 *
 * @file
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "uwb/uwb.h"
#include "uwb_rng/uwb_rng.h"
#include "uwb_core/twr.h"

#include "mutex.h"
#include "timex.h"
#include "event/callback.h"
#include "event/periodic.h"

/* setup timeout to the maximum value */
#define UWB_CORE_TWR_RX_TIMEOUT             0xfffff

/* forward declaration */
static bool _complete_cb(struct uwb_dev *inst, struct uwb_mac_interface *cbs);
static bool _rx_timeout_cb(struct uwb_dev *inst, struct uwb_mac_interface *cbs);

/* currently there can only be one so lets just keep a pointer to it */
struct uwb_rng_instance *_rng;
struct uwb_dev *_udev;

/* forward declaration */
static void _rng_listen(void *arg);
static void _rng_request(void *arg);
static event_callback_t _rng_listen_event = EVENT_CALLBACK_INIT(_rng_listen, NULL);

#define TWR_STATUS_INITIATOR      (1 << 0)
#define TWR_STATUS_RESPONDER      (1 << 1)
static uint8_t _status;
static mutex_t _lock = MUTEX_INIT;

/**
 * @brief   Event to handle periodic scheduling of UWB TWR requests
 */
typedef struct uwb_core_twr_event {
    event_callback_t event;     /**< the event callback */
    event_periodic_t periodic;  /**< the periodic event struct */
    twr_protocol_t proto;       /**< protocol to use */
    uint16_t addr;              /**< dest short address */
} uwb_core_twr_event_t;

static uwb_core_twr_event_t _rng_request_event = {
    .event = EVENT_CALLBACK_INIT(_rng_request, &_rng_request_event),
    .addr = 0xffff,
    .proto = TWR_PROTOCOL_SS,
};

/* Structure of extension callbacks common for mac layer */
static struct uwb_mac_interface _uwb_mac_cbs = (struct uwb_mac_interface){
    .id = UWBEXT_APP0,
    .complete_cb = _complete_cb,
    .rx_timeout_cb = _rx_timeout_cb,
};

static uwb_core_twr_data_t _rng_data;
static uwb_core_twr_cb_t _user_rng_data_cb;

/* callback to offload printing */
static void _rng_data_cb(void *arg)
{
    _user_rng_data_cb(arg);
}
static event_callback_t _print_rng_data_event = EVENT_CALLBACK_INIT(
    _rng_data_cb, &_rng_data);

/**
 * @brief Range request complete callback.
 *
 * This callback is part of the  struct uwb_mac_interface extension
 * interface and invoked of the completion of a range request in the
 * context of this example. The struct uwb_mac_interface is in the
 * interrupt context and is used to schedule events an event queue.
 *
 * Processing should be kept to a minimum given the interrupt context.
 * All algorithms activities should be deferred to a thread on an event
 * queue. The callback should return true if and only if it can determine
 * if it is the sole recipient of this event.
 *
 * @note The MAC extension interface is a linked-list of callbacks,
 * subsequentcallbacks on the list will be not be called if the callback
 * returns true.
 *
 * @param[in] inst  Pointer to struct uwb_dev.
 * @param[in] cbs   Pointer to struct uwb_mac_interface.
 * *
 * @return true if valid recipient for the event, false otherwise
 */
static bool _complete_cb(struct uwb_dev *inst, struct uwb_mac_interface *cbs)
{
    (void)cbs;
    if (inst->fctrl != FCNTL_IEEE_RANGE_16 &&
        inst->fctrl != (FCNTL_IEEE_RANGE_16 | UWB_FCTRL_ACK_REQUESTED)) {
        /* on completion of a range request setup an event to keep listening */
        event_post(uwb_core_get_eventq(), &_rng_listen_event.super);
        return false;
    }

    /* get received frame */
    struct uwb_rng_instance *rng = (struct uwb_rng_instance *)cbs->inst_ptr;

    rng->idx_current = (rng->idx) % rng->nframes;
    twr_frame_t *frame = rng->frames[rng->idx_current];

    /* with ss twr, the last frame will hold the initiator as the src address,
       with ds twr, it will hold the responder address */
    if (IS_ACTIVE(CONFIG_TWR_NOTIFY_INITIATOR_ONLY) &&
        ((frame->code < UWB_DATA_CODE_DS_TWR && frame->src_address != _udev->uid) ||
         (frame->code >= UWB_DATA_CODE_DS_TWR && frame->dst_address != _udev->uid))) {
        event_post(uwb_core_get_eventq(), &_rng_listen_event.super);
        return true;
    }
    /* parse and rng data only if the callback is set */
    if (_user_rng_data_cb) {
        /* parse data*/
        uwb_core_twr_data_t data;
        data.src = frame->src_address;
        data.dest = frame->dst_address;
        data.time = ztimer_now(ZTIMER_MSEC);
        float range_f =
            uwb_rng_tof_to_meters(uwb_rng_twr_to_tof(rng, rng->idx_current));

        /* convert from float meters to cm */
        data.d_cm = ((int32_t)(range_f * 100));
    #if IS_USED(MODULE_UWB_CORE_RNG_TRX_INFO)
        data.rssi = (int16_t)uwb_calc_rssi(inst, inst->rxdiag);
        data.fppl = uwb_calc_fppl(inst, inst->rxdiag);
        data.tof = uwb_rng_twr_to_tof(rng, rng->idx_current);
        data.los = uwb_estimate_los(rng->dev_inst, data.rssi, data.fppl);
    #endif
        memcpy(&_rng_data, &data, sizeof(uwb_core_twr_data_t));
        /* offload to event loop */
        event_post(uwb_core_get_eventq(), &_print_rng_data_event.super);
    }
    /* on completion of a range request setup an event to keep listening */
    event_post(uwb_core_get_eventq(), &_rng_listen_event.super);
    return true;
}

/**
 * @brief   API for receive timeout callback.
 *
 * @param[in] inst  Pointer to struct uwb_dev.
 * @param[in] cbs   Pointer to struct uwb_mac_interface.
 *
 * @return true on success
 */
static bool _rx_timeout_cb(struct uwb_dev *inst, struct uwb_mac_interface *cbs)
{
    (void)inst;
    (void)cbs;
    event_post(uwb_core_get_eventq(), &_rng_listen_event.super);
    return true;
}

/**
 * @brief   An event callback to update the remaining listening time
 */
static void _rng_listen(void *arg)
{
    (void)arg;

    if (!ztimer_is_set(ZTIMER_MSEC, &_rng_request_event.periodic.timer.timer)) {
        _status &= ~TWR_STATUS_INITIATOR;
    }
    if (_status & TWR_STATUS_RESPONDER) {
        /* wake up if needed */
        if (IS_USED(MODULE_UWB_CORE_TWR_SLEEP) && _udev->status.sleeping) {
            uwb_wakeup(_udev);
        }
        /* start listening */
        if (dpl_sem_get_count(&_rng->sem) == 1) {
            uwb_rng_listen(_rng, UWB_CORE_TWR_RX_TIMEOUT, UWB_NONBLOCKING);
        }
    }
    else {
        /* go to sleep if possible */
        if (IS_USED(MODULE_UWB_CORE_TWR_SLEEP) &&
            (_rng_request_event.periodic.timer.interval > CONFIG_TWR_MIN_IDLE_SLEEP_MS)) {
            uwb_sleep_config(_udev);
            uwb_enter_sleep(_udev);
        }
    }
}

void uwb_core_twr_listen_enable(void)
{
    _status |= TWR_STATUS_RESPONDER;
    /* post event to start listening */
    event_post(uwb_core_get_eventq(), &_rng_listen_event.super);
}

void uwb_core_twr_listen_disable(void)
{
    _status &= ~TWR_STATUS_RESPONDER;
}

bool uwb_core_twr_listen_is_enabled(void)
{
    return _status && TWR_STATUS_RESPONDER;
}

/**
 * @brief   An event callback to send a range request
 */
static void _rng_request(void *arg)
{
    uwb_core_twr_event_t *event = (uwb_core_twr_event_t *)arg;

    mutex_lock(&_lock);

    /* wake up if needed */
    if (IS_USED(MODULE_UWB_CORE_TWR_SLEEP) && _udev->status.sleeping) {
        uwb_wakeup(_udev);
    }
    /* force transition to trxoff instead of waiting for rng_listen operations
       to finish */
    if (dpl_sem_get_count(&_rng->sem) == 0) {
        uwb_phy_forcetrxoff(_udev);
    }
    uwb_rng_request(_rng, event->addr, (uwb_dataframe_code_t)event->proto);
    mutex_unlock(&_lock);
}

void uwb_core_twr_start(uint16_t addr, twr_protocol_t proto, uint32_t interval, uint32_t count)
{
    event_periodic_stop(&_rng_request_event.periodic);

    _rng_request_event.proto = proto;
    _rng_request_event.addr = addr;

    _status |= TWR_STATUS_INITIATOR;

    event_periodic_set_count(&_rng_request_event.periodic, count);
    event_periodic_start(&_rng_request_event.periodic, interval);
}

void uwb_core_twr_init(void)
{
    _udev = uwb_dev_idx_lookup(0);
    _rng = (struct uwb_rng_instance *)uwb_mac_find_cb_inst_ptr(_udev, UWBEXT_RNG);
    assert(_rng);
    /* set up local mac callbacks */
    _uwb_mac_cbs.inst_ptr = _rng;
    uwb_mac_append_interface(_udev, &_uwb_mac_cbs);
    /* init request periodic event */
    event_periodic_init(&_rng_request_event.periodic, ZTIMER_MSEC,
                        uwb_core_get_eventq(), &_rng_request_event.event.super);
    /* set initial status */
    _status = 0;

    if (IS_USED(MODULE_UWB_CORE_TWR_SLEEP)) {
        /* got to sleep on boot */
        struct uwb_dev *_udev = uwb_dev_idx_lookup(0);
        uwb_sleep_config(_udev);
        uwb_enter_sleep(_udev);
    }
}

uint32_t uwb_core_twr_req_remaining(void)
{
    /* doesn't matter if its not atomic */
    return _rng_request_event.periodic.count;
}

void uwb_core_twr_set_cb(uwb_core_twr_cb_t cb)
{
    _user_rng_data_cb = cb;
}
