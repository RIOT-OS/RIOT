/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
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

#include "uwb/uwb.h"
#include "uwb_rng/uwb_rng.h"
#include "dpl/dpl.h"
#include "control.h"

#include "shell_commands.h"
#include "shell.h"
#include "xtimer.h"

static struct dpl_callout _rng_req_callout;
static uint8_t _ranging_enabled_flag;
static struct dpl_event _slot_event;

/* forward declaration */
static void _slot_complete_cb(struct dpl_event *ev);
static bool _complete_cb(struct uwb_dev *inst, struct uwb_mac_interface *cbs);
static bool _rx_timeout_cb(struct uwb_dev *inst, struct uwb_mac_interface *cbs);

static int _range_cli_cmd(int argc, char **argv)
{
    (void)argc;

    if (!strcmp(argv[1], "start")) {
        printf("Start ranging\n");
        dpl_callout_reset(&_rng_req_callout, RANGE_REQUEST_T_US);
        _ranging_enabled_flag = 1;
    }
    else if (!strcmp(argv[1], "stop")) {
        printf("Stop ranging\n");
        dpl_callout_stop(&_rng_req_callout);
        _ranging_enabled_flag = 0;
    }
    else {
        puts("Usage:");
        puts("\trange start: to start ranging");
        puts("\trange stop:  to stop  ranging");
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "range", "Control command", _range_cli_cmd },
    { NULL, NULL, NULL }
};

/* Structure of extension callbacks common for mac layer */
static struct uwb_mac_interface _uwb_mac_cbs = (struct uwb_mac_interface){
    .id = UWBEXT_APP0,
    .complete_cb = _complete_cb,
    .rx_timeout_cb = _rx_timeout_cb,
};

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
        return false;
    }
    /* on completion of a range request setup an event to keep listening,
       if is anchor */
    dpl_eventq_put(dpl_eventq_dflt_get(), &_slot_event);
    return true;
}

/**
 * @brief API for receive timeout callback.
 *
 * @param[in] inst  Pointer to struct uwb_dev.
 * @param[in] cbs   Pointer to struct uwb_mac_interface.
 *
 * @return true on success
 */
static bool _rx_timeout_cb(struct uwb_dev *inst, struct uwb_mac_interface *cbs)
{
    struct uwb_rng_instance *rng = (struct uwb_rng_instance *)cbs->inst_ptr;

    if (inst->role & UWB_ROLE_ANCHOR) {
        /* Restart receiver */
        uwb_phy_forcetrxoff(inst);
        uwb_rng_listen(rng, 0xfffff, UWB_NONBLOCKING);
    }
    return true;
}

/**
 * @brief In the example this function represents the event context
 * processing of the received range request which has been offloaded from
 * ISR context to an event queue.
 */
static void _slot_complete_cb(struct dpl_event *ev)
{
    assert(ev != NULL);

    struct uwb_rng_instance *rng = (struct uwb_rng_instance *)
        dpl_event_get_arg(ev);
    struct uwb_dev *inst = rng->dev_inst;

    if (inst->role & UWB_ROLE_ANCHOR) {
        uwb_rng_listen(rng, 0xfffff, UWB_NONBLOCKING);
    }
}

/**
 * @brief An event callback to send range request every RANGE_REQUEST_T_US.
 * On every request it will switch the used ranging algorithm.
 */
static void uwb_ev_cb(struct dpl_event *ev)
{
    struct uwb_rng_instance *rng = (struct uwb_rng_instance *)ev->arg;
    struct uwb_dev *inst = rng->dev_inst;

    if (inst->role & UWB_ROLE_ANCHOR) {
        if (dpl_sem_get_count(&rng->sem) == 1) {
            uwb_rng_listen(rng, 0xfffff, UWB_NONBLOCKING);
        }
    }
    else {
        int mode_v[8] = { 0 }, mode_i = 0, mode = -1;
        static int last_used_mode = 0;
        if (IS_USED(MODULE_UWB_CORE_TWR_SS)) {
            mode_v[mode_i++] = UWB_DATA_CODE_SS_TWR;
        }
        if (IS_USED(MODULE_UWB_CORE_TWR_SS_ACK)) {
            mode_v[mode_i++] = UWB_DATA_CODE_SS_TWR_ACK;
        }
        if (IS_USED(MODULE_UWB_CORE_TWR_SS_EXT)) {
            mode_v[mode_i++] = UWB_DATA_CODE_SS_TWR_EXT;
        }
        if (IS_USED(MODULE_UWB_CORE_TWR_DS)) {
            mode_v[mode_i++] = UWB_DATA_CODE_DS_TWR;
        }
        if (IS_USED(MODULE_UWB_CORE_TWR_DS_EXT)) {
            mode_v[mode_i++] = UWB_DATA_CODE_DS_TWR_EXT;
        }
        if (++last_used_mode >= mode_i) {
            last_used_mode = 0;
        }
        mode = mode_v[last_used_mode];
#ifdef UWB_TWR_ALGORITHM_ONLY_ONE
        mode = UWB_TWR_ALGORITHM_ONLY_ONE;
#endif
        if (mode > 0) {
            uwb_rng_request(rng, ANCHOR_ADDRESS, mode);
        }
    }

    /* reset the callback if ranging is enabled */
    if (_ranging_enabled_flag) {
        dpl_callout_reset(&_rng_req_callout, RANGE_REQUEST_T_US);
    }
}

void init_ranging(void)
{
    struct uwb_dev *udev = uwb_dev_idx_lookup(0);
    struct uwb_rng_instance *rng =
        (struct uwb_rng_instance *)uwb_mac_find_cb_inst_ptr(udev, UWBEXT_RNG);

    assert(rng);

    /* set up local mac callbacks */
    _uwb_mac_cbs.inst_ptr = rng;
    uwb_mac_append_interface(udev, &_uwb_mac_cbs);

    uint32_t utime = xtimer_now_usec();

    printf("{\"utime\": %" PRIu32 ",\"exec\": \"%s\"}\n", utime, __FILE__);
    printf("{\"device_id\"=\"%" PRIx32 "\"", udev->device_id);
    printf(",\"panid=\"%X\"", udev->pan_id);
    printf(",\"addr\"=\"%X\"", udev->uid);
    printf(",\"part_id\"=\"%" PRIx32 "\"",
              (uint32_t)(udev->euid & 0xffffffff));
    printf(",\"lot_id\"=\"%" PRIx32 "\"}\n", (uint32_t)(udev->euid >> 32));
    printf("{\"utime\": %"PRIu32",\"msg\": \"frame_duration = %d usec\"}\n",
              utime, uwb_phy_frame_duration(udev, sizeof(twr_frame_final_t)));
    printf("{\"utime\": %"PRIu32",\"msg\": \"SHR_duration = %d usec\"}\n",
              utime, uwb_phy_SHR_duration(udev));
    printf("{\"utime\": %"PRIu32",\"msg\": \"holdoff = %d usec\"}\n", utime,
              (uint16_t)ceilf(uwb_dwt_usecs_to_usecs(rng->config.
                                                     tx_holdoff_delay)));

    if (IS_USED(MODULE_UWB_CORE_TWR_SS_ACK)) {
        uwb_set_autoack(udev, true);
        uwb_set_autoack_delay(udev, 12);
    }

    dpl_callout_init(&_rng_req_callout, dpl_eventq_dflt_get(),
                     uwb_ev_cb, rng);
    dpl_callout_reset(&_rng_req_callout, RANGE_REQUEST_T_US);
    dpl_event_init(&_slot_event, _slot_complete_cb, rng);

    /* Apply config */
    uwb_mac_config(udev, NULL);
    uwb_txrf_config(udev, &udev->config.txrf);

    if ((udev->role & UWB_ROLE_ANCHOR)) {
        printf("Node role: ANCHOR \n");
        udev->my_short_address = ANCHOR_ADDRESS;
        uwb_set_uid(udev, udev->my_short_address);
        /* anchor starts listening by default */
        _ranging_enabled_flag = 1;
    }
    else {
        _ranging_enabled_flag = 0;
        printf("Node role: TAG \n");
    }

    /* define buffer to be used by the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
}
