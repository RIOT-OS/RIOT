/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "net/ieee802154.h"
#include "net/ieee802154/submac.h"
#include "net/ieee802154/radio.h"
#include "errno.h"
#include "luid.h"
#include "ztimer.h"
#include "random.h"
#include "thread.h"
#include "container.h"
#include "atomic_utils.h"
#include "compiler_hints.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define CSMA_SENDER_BACKOFF_PERIOD_UNIT_US  (320U)
#define ACK_TIMEOUT_US                      (864U)
/* 2.4 GHz, 250 kb/s, O-QPSK 62.5 ksymbols/s, 1 / 62 500 s = 16 µs */
/* 12 symbols -> 12 * 16us = 192us */
#define SIFS_PERIOD_US                      (192U)

/* internal type for IEEE 802.15.4 frame control field */
typedef enum {
    _FCF_BEACON     = IEEE802154_FCF_TYPE_BEACON,
    _FCF_DATA       = IEEE802154_FCF_TYPE_DATA,
    _FCF_ACK        = IEEE802154_FCF_TYPE_ACK,
    _FCF_MACCMD     = IEEE802154_FCF_TYPE_MACCMD
} ieee802154_fcf_t;

/**
 * @brief Handle invalid FSM state - should never be reached during normal operation
 * @param[in] submac Pointer to SubMAC instance
 * @param[in] ev FSM event to process
 * @return IEEE802154_SUBMAC_FSM_RETURN_IGNORED for all events
 */
static ieee802154_submac_fsm_return_t _fsm_state_invalid(ieee802154_submac_t *submac,
                                                         ieee802154_fsm_ev_t ev,
                                                         void* ctx);

/**
 * @brief Handle idle state - transceiver is powered off and not transmitting/receiving
 * @param[in] submac Pointer to SubMAC instance
 * @param[in] ev FSM event to process
 * @return FSM return status indicating transition to new state or event handling result
 */
static ieee802154_submac_fsm_return_t _fsm_state_idle(ieee802154_submac_t *submac,
                                                      ieee802154_fsm_ev_t ev,
                                                      void* ctx);

/**
 * @brief Handle RX state - SubMAC is ready to receive frames from the radio
 * @param[in] submac Pointer to SubMAC instance
 * @param[in] ev FSM event to process
 * @return FSM return status indicating transition to new state or event handling result
 */
static ieee802154_submac_fsm_return_t _fsm_state_rx(ieee802154_submac_t *submac,
                                                    ieee802154_fsm_ev_t ev,
                                                    void* ctx);

/**
 * @brief Handle TX state - SubMAC is currently transmitting a frame via radio
 * @param[in] submac Pointer to SubMAC instance
 * @param[in] ev FSM event to process
 * @return FSM return status indicating transition to new state or event handling result
 */
static ieee802154_submac_fsm_return_t _fsm_state_tx(ieee802154_submac_t *submac,
                                                    ieee802154_fsm_ev_t ev,
                                                    void* ctx);

/**
 * @brief Handle prepare state - SubMAC is preparing the next transmission (CSMA backoff)
 * @param[in] submac Pointer to SubMAC instance
 * @param[in] ev FSM event to process
 * @return FSM return status indicating transition to new state or event handling result
 * @note This state handles CSMA-CA backoff timing if radio doesn't support auto-CSMA
 */
static ieee802154_submac_fsm_return_t _fsm_state_prepare(ieee802154_submac_t *submac,
                                                         ieee802154_fsm_ev_t ev,
                                                         void* ctx);

/**
 * @brief Handle wait-for-ACK state - SubMAC is waiting for an acknowledgment frame
 * @param[in] submac Pointer to SubMAC instance
 * @param[in] ev FSM event to process
 * @return FSM return status indicating transition to new state or event handling result
 * @note ACK timeout timer is active in this state
 */
static ieee802154_submac_fsm_return_t _fsm_state_wait_for_ack(ieee802154_submac_t *submac,
                                                              ieee802154_fsm_ev_t ev,
                                                              void* ctx);

typedef struct {
    ieee802154_fsm_state_cb_t state;
    const char *name;
} ieee802154_fsm_state_mapping_t;

static const ieee802154_fsm_state_mapping_t states_mapping[] = {
    { _fsm_state_invalid, "INVALID" },
    { _fsm_state_rx, "RX" },
    { _fsm_state_idle, "IDLE" },
    { _fsm_state_prepare, "PREPARE" },
    { _fsm_state_tx, "TX" },
    { _fsm_state_wait_for_ack, "WAIT_FOR_ACK" },
};

static char *str_ev[] = {
    "TX_DONE",
    "RX_DONE",
    "CRC_ERROR",
    "ACK_TIMEOUT",
    "BH",
    "REQUEST_TX",
    "REQUEST_SET_RX_ON",
    "REQUEST_SET_IDLE",
};

MAYBE_UNUSED
static const char *get_state_name(ieee802154_fsm_state_cb_t state)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(states_mapping); i++) {
        if (states_mapping[i].state == state) {
            return states_mapping[i].name;
        }
    }
    return "UNKNOWN";
}

static int _is_ack(const iolist_t *psdu) {
    return (psdu->iol_len == IEEE802154_ACK_FRAME_LEN - IEEE802154_FCS_LEN &&
            (((uint8_t *)psdu->iol_base)[0] & IEEE802154_FCF_TYPE_ACK) &&
            psdu->iol_next == NULL);
}

static ieee802154_submac_fsm_return_t _state_transition(ieee802154_fsm_t *fsm,
                                                        ieee802154_fsm_state_cb_t new_state)
{
    fsm->fsm_state_cb = new_state;
    return IEEE802154_SUBMAC_FSM_RETURN_TRANSITION;
}

static inline bool _does_handle_ack(ieee802154_dev_t *dev)
{
    return ieee802154_radio_has_frame_retrans(dev) ||
           ieee802154_radio_has_irq_ack_timeout(dev);
}

static inline bool _does_handle_csma(ieee802154_dev_t *dev)
{
    return ieee802154_radio_has_frame_retrans(dev) ||
           ieee802154_radio_has_auto_csma(dev);
}

static bool _has_retrans_left(ieee802154_submac_t *submac)
{
    return submac->retrans < CONFIG_IEEE802154_DEFAULT_MAX_FRAME_RETRANS;
}

static ieee802154_submac_fsm_return_t _tx_end(ieee802154_submac_t *submac, int status,
                                              ieee802154_tx_info_t *info)
{
    int res;

    /* This is required to prevent unused variable warnings */
    (void)res;

    submac->wait_for_ack = false;

    res = ieee802154_radio_set_idle(&submac->dev, true);

    assert(res >= 0);
    submac->cb->tx_done(submac, status, info);
    return _state_transition(&submac->fsm, _fsm_state_idle);
}

static void _print_state_transition(ieee802154_fsm_state_cb_t old, ieee802154_fsm_state_cb_t new,
                         ieee802154_fsm_ev_t ev)
{
    DEBUG("%s--(%s)->%s\n", get_state_name(old), str_ev[ev], get_state_name(new));
}

static ieee802154_submac_fsm_return_t _handle_tx_no_ack(ieee802154_submac_t *submac)
{
    int res;

    /* This is required to prevent unused variable warnings */
    (void) res;

    /* In case of ACK Timeout, either trigger retransmissions or end
     * the TX procedure */
    if (_has_retrans_left(submac)) {
        submac->retrans++;
        res = ieee802154_radio_set_idle(&submac->dev, true);
        assert(res >= 0);
        return _state_transition(&submac->fsm, _fsm_state_prepare);
    }
    ieee802154_radio_set_frame_filter_mode(&submac->dev, IEEE802154_FILTER_ACCEPT);
    return _tx_end(submac, TX_STATUS_NO_ACK, NULL);
}

static int _handle_fsm_ev_request_tx(ieee802154_submac_t *submac)
{
    ieee802154_dev_t *dev = &submac->dev;

    /* Set state to TX_ON */
    int res = ieee802154_radio_set_idle(dev, false);

    if (res < 0) {
        submac->fsm_context_res = res;
        return res;
    }
    if ((res = ieee802154_radio_write(dev, submac->psdu)) < 0) {
        submac->fsm_context_res = res;
        return res;
    }
    _state_transition(&submac->fsm, _fsm_state_prepare);
    return 0;
}

ieee802154_submac_fsm_return_t _fsm_state_invalid(ieee802154_submac_t *submac,
                                                  ieee802154_fsm_ev_t ev,
                                                  void* ctx)
{
    (void)submac;
    (void) ctx;
    assert(false);
    switch (ev) {
    case IEEE802154_FSM_EV_ENTRY:
    case IEEE802154_FSM_EV_EXIT:
    default:
        return IEEE802154_SUBMAC_FSM_RETURN_IGNORED;
    }
}

static ieee802154_submac_fsm_return_t _fsm_state_rx(ieee802154_submac_t *submac,
                                                    ieee802154_fsm_ev_t ev,
                                                    void* ctx)
{
    DEBUG("IEEE802154 submac: _fsm_state_rx + %s\n", str_ev[ev]);
    (void) ctx;
    ieee802154_dev_t *dev = &submac->dev;
    int res;

    /* This is required to prevent unused variable warnings */
    (void)res;

    switch (ev) {
    case IEEE802154_FSM_EV_REQUEST_SET_RX_ON:
        submac->fsm_context_res = -EALREADY;
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_REQUEST_TX:
        if (_handle_fsm_ev_request_tx(submac) < 0) {
            return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
        }
        return _state_transition(&submac->fsm, _fsm_state_prepare);
    case IEEE802154_FSM_EV_RX_DONE:
        /* Make sure it's not an ACK frame */
        while (ieee802154_radio_set_idle(&submac->dev, false) < 0) {}
        if (ieee802154_radio_len(&submac->dev) > (int)IEEE802154_MIN_FRAME_LEN) {
            submac->cb->rx_done(submac);
            return _state_transition(&submac->fsm, _fsm_state_idle);
        }
        else {
            ieee802154_radio_read(&submac->dev, NULL, 0, NULL);

            /* If the radio doesn't support RX Continuous, go to RX */
            res = ieee802154_radio_set_rx(&submac->dev);
            assert(res >= 0);

            /* Keep on current state */
            return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
        }
    case IEEE802154_FSM_EV_CRC_ERROR:
        while (ieee802154_radio_set_idle(&submac->dev, false) < 0) {}
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        /* If the radio doesn't support RX Continuous, go to RX */
        res = ieee802154_radio_set_rx(&submac->dev);
        assert(res >= 0);
        /* Keep on current state */
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;

    case IEEE802154_FSM_EV_REQUEST_SET_IDLE:
        /* Try to turn off the transceiver */
        if ((ieee802154_radio_request_set_idle(dev, false)) < 0) {
            /* Keep on current state */
            submac->fsm_context_res = -EBUSY;
            return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
        }
        while (ieee802154_radio_confirm_set_idle(dev) == -EAGAIN) {}
        return _state_transition(&submac->fsm, _fsm_state_idle);

    case IEEE802154_FSM_EV_ENTRY:
    case IEEE802154_FSM_EV_EXIT:
    default:
        return IEEE802154_SUBMAC_FSM_RETURN_IGNORED;

    }
    return _state_transition(&submac->fsm, _fsm_state_invalid);
}

static ieee802154_submac_fsm_return_t _fsm_state_idle(ieee802154_submac_t *submac,
                                                      ieee802154_fsm_ev_t ev,
                                                      void* ctx)
{
    DEBUG("IEEE802154 submac: _fsm_state_idle + %s\n", str_ev[ev]);
    (void) ctx;
    ieee802154_dev_t *dev = &submac->dev;

    switch (ev) {

    case IEEE802154_FSM_EV_REQUEST_SET_IDLE:
        submac->fsm_context_res = -EALREADY;
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_REQUEST_TX:
        if (_handle_fsm_ev_request_tx(submac) < 0) {
            return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
        }
        return _state_transition(&submac->fsm, _fsm_state_prepare);
    case IEEE802154_FSM_EV_REQUEST_SET_RX_ON:
        /* Try to go turn on the transceiver */
        if ((ieee802154_radio_set_rx(dev) < 0)) {
            /* Keep on current state */
            submac->fsm_context_res = -EBUSY;
            return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
        }
        return _state_transition(&submac->fsm, _fsm_state_rx);
    case IEEE802154_FSM_EV_RX_DONE:
    case IEEE802154_FSM_EV_CRC_ERROR:
        /* This might happen in case there's a race condition between ACK_TIMEOUT
         * and TX_DONE. We simply discard the frame and keep the state as
         * it is
         */
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_ENTRY:
    case IEEE802154_FSM_EV_EXIT:
    default:
        return IEEE802154_SUBMAC_FSM_RETURN_IGNORED;

    }
    return _state_transition(&submac->fsm, _fsm_state_invalid);
}


static ieee802154_submac_fsm_return_t _fsm_state_prepare(ieee802154_submac_t *submac,
                                                         ieee802154_fsm_ev_t ev,
                                                         void* ctx)
{
    DEBUG("IEEE802154 submac: _fsm_state_prepare + %s\n", str_ev[ev]);
    ieee802154_dev_t *dev = &submac->dev;
    ieee802154_fcf_t ftype;

    switch (ev) {
    case IEEE802154_FSM_EV_ENTRY:
        ftype = *(ieee802154_fcf_t *)ctx;
        if (ftype == IEEE802154_FCF_TYPE_DATA
            && !_does_handle_csma(dev)) {
            /* delay for an adequate random backoff period */
            uint32_t bp = (random_uint32() & submac->backoff_mask) *
                          submac->csma_backoff_us;

            ztimer_sleep(ZTIMER_USEC, bp);
            /* Prepare for next iteration */
            uint8_t curr_be = (submac->backoff_mask + 1) >> 1;
            if (curr_be < submac->be.max) {
                submac->backoff_mask = (submac->backoff_mask << 1) | 1;
            }
        }
        else if (ftype == IEEE802154_FCF_TYPE_ACK) {
            /* no backoff for ACK frames but wait for SIFSPeriod */
            ztimer_sleep(ZTIMER_USEC, SIFS_PERIOD_US);
        }
        while (ieee802154_radio_request_transmit(dev) == -EBUSY) {}
        return _state_transition(&submac->fsm, _fsm_state_tx);
    case IEEE802154_FSM_EV_REQUEST_TX:
    case IEEE802154_FSM_EV_REQUEST_SET_IDLE:
    case IEEE802154_FSM_EV_REQUEST_SET_RX_ON:
        submac->fsm_context_res = -EBUSY;
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_RX_DONE:
    case IEEE802154_FSM_EV_CRC_ERROR:
        /* This might happen in case there's a race condition between ACK_TIMEOUT
         * and TX_DONE. We simply discard the frame and keep the state as
         * it is
         */
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_EXIT:
    default:
        return IEEE802154_SUBMAC_FSM_RETURN_IGNORED;

    }
    return _state_transition(&submac->fsm, _fsm_state_invalid);
}

static ieee802154_submac_fsm_return_t _fsm_state_tx_process_tx_done(
    ieee802154_submac_t *submac,
    ieee802154_tx_info_t *info)
{
    ieee802154_dev_t *dev = &submac->dev;
    int res;

    /* This is required to prevent unused variable warnings */
    (void)res;
    switch (info->status) {
    case TX_STATUS_FRAME_PENDING:
        assert(_does_handle_ack(&submac->dev));
    /* FALL-THRU */
    case TX_STATUS_SUCCESS:
        submac->csma_retries_nb = 0;
        /* If the radio handles ACK, the TX_DONE event marks completion of
        * the transmission procedure. Report TX done to the upper layer */
        if (_does_handle_ack(&submac->dev) || !submac->wait_for_ack) {
            return _tx_end(submac, info->status, info);
        }
        /* If the radio doesn't handle ACK, set the transceiver state to RX_ON
         * and enable the ACK filter */
        else {
            ieee802154_radio_set_frame_filter_mode(dev, IEEE802154_FILTER_ACK_ONLY);
            res = ieee802154_radio_set_rx(dev);
            assert(res >= 0);

            /* Handle ACK reception */
            ieee802154_submac_ack_timer_set(submac);
            return _state_transition(&submac->fsm, _fsm_state_wait_for_ack);
        }
        break;
    case TX_STATUS_NO_ACK:
        assert(_does_handle_ack(&submac->dev));
        submac->csma_retries_nb = 0;
        return _handle_tx_no_ack(submac);
    case TX_STATUS_MEDIUM_BUSY:
        /* If radio has retransmissions or CSMA-CA, this means the CSMA-CA
         * procedure failed. We finish the SubMAC operation and report
         * medium busy
         */
        if (_does_handle_csma(&submac->dev)
            || submac->csma_retries_nb++ >= submac->csma_retries) {
            return _tx_end(submac, info->status, info);
        }
        /* Otherwise, this is a failed CCA attempt. Proceed with CSMA-CA */
        else {
            /* The HAL should guarantee that's still possible to transmit
             * in the current state, since the radio is still in TX_ON.
             * Therefore, this is valid */
            return _state_transition(&submac->fsm, _fsm_state_prepare);
        }
    }
    return _state_transition(&submac->fsm, _fsm_state_invalid);
}

static ieee802154_submac_fsm_return_t _fsm_state_tx(ieee802154_submac_t *submac,
                                                    ieee802154_fsm_ev_t ev,
                                                    void* ctx)
{
    DEBUG("IEEE802154 submac: _fsm_state_tx + %s\n", str_ev[ev]);
    ieee802154_fcf_t ftype;
    ieee802154_tx_info_t info;
    int res;

    /* This is required to prevent unused variable warnings */
    (void)res;

    switch (ev) {
    case IEEE802154_FSM_EV_TX_DONE:
        if ((res = ieee802154_radio_confirm_transmit(&submac->dev, &info)) >= 0) {
            return _fsm_state_tx_process_tx_done(submac, &info);
        }
        break;
    case IEEE802154_FSM_EV_RX_DONE:
    case IEEE802154_FSM_EV_CRC_ERROR:
        /* This might happen in case there's a race condition between ACK_TIMEOUT
         * and TX_DONE. We simply discard the frame and keep the state as
         * it is
         */
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_REQUEST_TX:
    case IEEE802154_FSM_EV_REQUEST_SET_RX_ON:
    case IEEE802154_FSM_EV_REQUEST_SET_IDLE:
        submac->fsm_context_res = -EBUSY;
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_ENTRY:
        /* An ACK is sent synchronous to prevent that the upper layer (IPv6)
           can initiate the next transmission which would fail because the transceiver
           is still busy. */
        ftype = *(ieee802154_fcf_t *)ctx;
        if (ftype == IEEE802154_FCF_TYPE_ACK) {
            while ((res = ieee802154_radio_confirm_transmit(&submac->dev, &info)) < 0) {
                DEBUG("IEEE802154 submac: wait until ACK sent\n");
            }
            return _fsm_state_tx_process_tx_done(submac, &info);
        }
        return IEEE802154_SUBMAC_FSM_RETURN_IGNORED;
    case IEEE802154_FSM_EV_EXIT:
    default:
        return IEEE802154_SUBMAC_FSM_RETURN_IGNORED;

    }
    return _state_transition(&submac->fsm, _fsm_state_invalid);
}

static ieee802154_submac_fsm_return_t _fsm_state_wait_for_ack(ieee802154_submac_t *submac,
                                                              ieee802154_fsm_ev_t ev,
                                                              void *ctx)
{
    DEBUG("IEEE802154 submac: _fsm_state_wait_for_ack + %s\n", str_ev[ev]);
    (void) ctx;
    uint8_t ack[3];

    switch (ev) {
    case IEEE802154_FSM_EV_RX_DONE:
        assert(!ieee802154_radio_has_irq_ack_timeout(&submac->dev));
        if (ieee802154_radio_read(&submac->dev, ack, 3, NULL) &&
            ack[0] & IEEE802154_FCF_TYPE_ACK) {
            ieee802154_submac_ack_timer_cancel(submac);
            ieee802154_tx_info_t tx_info;
            tx_info.retrans = submac->retrans;
            bool fp = (ack[0] & IEEE802154_FCF_FRAME_PEND);
            ieee802154_radio_set_frame_filter_mode(&submac->dev, IEEE802154_FILTER_ACCEPT);
            return _tx_end(submac, fp ? TX_STATUS_FRAME_PENDING : TX_STATUS_SUCCESS,
                           &tx_info);
        }
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_CRC_ERROR:
        /* Received invalid ACK. Drop frame */
        ieee802154_radio_read(&submac->dev, NULL, 0, NULL);
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_ACK_TIMEOUT:
        return _handle_tx_no_ack(submac);
    case IEEE802154_FSM_EV_REQUEST_TX:
    case IEEE802154_FSM_EV_REQUEST_SET_RX_ON:
    case IEEE802154_FSM_EV_REQUEST_SET_IDLE:
        submac->fsm_context_res = -EBUSY;
        return IEEE802154_SUBMAC_FSM_RETURN_HANDLED;
    case IEEE802154_FSM_EV_ENTRY:
    case IEEE802154_FSM_EV_EXIT:
    default:
        return IEEE802154_SUBMAC_FSM_RETURN_IGNORED;

    }
    return _state_transition(&submac->fsm, _fsm_state_invalid);
}

int ieee802154_submac_process_ev(ieee802154_submac_t *submac,
                                 ieee802154_fsm_ev_t ev,
                                 void *ctx)
{
    ieee802154_fsm_state_cb_t last_state = submac->fsm.fsm_state_cb;
    ieee802154_fsm_ev_t last_event = ev;
    ieee802154_submac_fsm_return_t res;

    uint8_t was_busy =  atomic_fetch_or_u8(&submac->fsm.busy_status, true);

    if (was_busy) {
        return -EBUSY;
    }
    while ((res = (*submac->fsm.fsm_state_cb)(submac,
                                              last_event, ctx)) ==
           IEEE802154_SUBMAC_FSM_RETURN_TRANSITION) {
        if (_fsm_state_invalid == submac->fsm.fsm_state_cb) {
            _print_state_transition(last_state, submac->fsm.fsm_state_cb, ev);
            assert(false);
        }
        res = (*last_state)(submac, IEEE802154_FSM_EV_EXIT, ctx);
        /* state shouldn`t make a transition on exit event */
        assert(res != IEEE802154_SUBMAC_FSM_RETURN_TRANSITION);
        last_state = submac->fsm.fsm_state_cb;
        last_event = IEEE802154_FSM_EV_ENTRY;
    }
    atomic_store_u8(&submac->fsm.busy_status, false);
    return res;
}

int ieee802154_send(ieee802154_submac_t *submac, const iolist_t *iolist)
{
    if (iolist == NULL) {
        return 0;
    }

    uint8_t fsm_busy =  atomic_load_u8(&submac->fsm.busy_status);
    if (fsm_busy) {
        return -EBUSY;
    }

    uint8_t *buf = iolist->iol_base;
    bool cnf = buf[0] & IEEE802154_FCF_ACK_REQ;
    bool is_ack = _is_ack(iolist);

    submac->wait_for_ack = cnf;
    submac->psdu = iolist;
    submac->retrans = 0;
    submac->csma_retries_nb = 0;
    submac->backoff_mask = (1 << submac->be.min) - 1;
    submac->fsm_context_res = 0;

    int res = 0;
    ieee802154_fcf_t fcf_type = is_ack ? IEEE802154_FCF_TYPE_ACK : IEEE802154_FCF_TYPE_DATA;
    if (is_ack) {

        res = ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_REQUEST_TX, &fcf_type);
        if (submac->fsm.fsm_state_cb != _fsm_state_idle) {
            DEBUG("IEEE802154 submac: ieee802154_send(): Tx ACK failed \n");
            return -EBUSY;
        }
    }
    else {
        res = ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_REQUEST_TX, &fcf_type);
    }
    if (res < 0) {
        return res;
    }
    return submac->fsm_context_res;
}

/*
 * MR-OQPSK timing calculations
 *
 * The standard unfortunately does not list the formula, instead it has to be pieced together
 * from scattered information and tables in the IEEE 802.15.4 document - may contain errors.
 */

static uint8_t _mr_oqpsk_spreading(uint8_t chips, uint8_t mode)
{
    if (mode == 4) {
        return 1;
    }

    uint8_t spread = 1 << (3 - mode);

    if (chips == IEEE802154_MR_OQPSK_CHIPS_1000) {
        return 2 * spread;
    }

    if (chips == IEEE802154_MR_OQPSK_CHIPS_2000) {
        return 4 * spread;
    }

    return spread;
}

static inline uint16_t _mr_oqpsk_symbol_duration_us(uint8_t chips)
{
    /* 802.15.4g, Table 183 / Table 165 */
    switch (chips) {
    case IEEE802154_MR_OQPSK_CHIPS_100:
        return 320;
    case IEEE802154_MR_OQPSK_CHIPS_200:
        return 160;
    case IEEE802154_MR_OQPSK_CHIPS_1000:
    case IEEE802154_MR_OQPSK_CHIPS_2000:
    default:
        return 64;
    }
}

static inline uint8_t _mr_oqpsk_cca_duration_syms(uint8_t chips)
{
    /* 802.15.4g, Table 188 */
    return (chips < IEEE802154_MR_OQPSK_CHIPS_1000) ? 4 : 8;
}

static inline uint8_t _mr_oqpsk_shr_duration_syms(uint8_t chips)
{
    /* 802.15.4g, Table 184 / Table 165 */
    return (chips < IEEE802154_MR_OQPSK_CHIPS_1000) ? 48 : 72;
}

static inline uint8_t _mr_oqpsk_ack_psdu_duration_syms(uint8_t chips, uint8_t mode)
{
    /* pg. 119, section 18.3.2.14 */
    static const uint8_t sym_len[] = { 32, 32, 64, 128 };
    const uint8_t Ns = sym_len[chips];
    const uint8_t Rspread = _mr_oqpsk_spreading(chips, mode);
    /* Nd == 63, since ACK length is 5 or 7 octets only */
    const uint16_t Npsdu = Rspread * 2 * 63;

    /* phyPSDUDuration = ceiling(Npsdu / Ns) + ceiling(Npsdu / Mp) */
    /* with Mp = Np * 16, see Table 182 */
    return (Npsdu + Ns / 2) / Ns + (Npsdu + 8 * Ns) / (16 * Ns);
}

MAYBE_UNUSED
static inline uint16_t _mr_oqpsk_ack_timeout_us(const ieee802154_mr_oqpsk_conf_t *conf)
{
    /* see 802.15.4g-2012, p. 30 */
    uint16_t symbols = _mr_oqpsk_cca_duration_syms(conf->chips)
                       + _mr_oqpsk_shr_duration_syms(conf->chips)
                       + 15 /* PHR duration */
                       + _mr_oqpsk_ack_psdu_duration_syms(conf->chips, conf->rate_mode);

    return _mr_oqpsk_symbol_duration_us(conf->chips) * symbols
           + IEEE802154G_ATURNAROUNDTIME_US;
}

MAYBE_UNUSED
static inline uint16_t _mr_oqpsk_csma_backoff_period_us(const ieee802154_mr_oqpsk_conf_t *conf)
{
    return _mr_oqpsk_cca_duration_syms(conf->chips) * _mr_oqpsk_symbol_duration_us(conf->chips)
           + IEEE802154G_ATURNAROUNDTIME_US;
}

/*
 * MR-OFDM timing calculations
 *
 * The standard unfortunately does not list the formula, instead it has to be pieced together
 * from scattered information and tables in the IEEE 802.15.4 document - may contain errors.
 */

static unsigned _mr_ofdm_frame_duration(uint8_t option, uint8_t scheme, uint8_t bytes)
{
    /* Table 150 - phySymbolsPerOctet values for MR-OFDM PHY, IEEE 802.15.4g-2012 */
    static const uint8_t quot[] = { 3, 3, 6, 12, 18, 24, 36 };

    --option;
    /* phyMaxFrameDuration = phySHRDuration + phyPHRDuration
     *                     + ceiling [(aMaxPHYPacketSize + 1) x phySymbolsPerOctet] */
    const unsigned phySHRDuration = 6;
    const unsigned phyPHRDuration = option ? 6 : 3;
    const unsigned phyPDUDuration = ((bytes + 1) * (1 << option) + quot[scheme] - 1)
                                    / quot[scheme];

    return (phySHRDuration + phyPHRDuration + phyPDUDuration) * IEEE802154_MR_OFDM_SYMBOL_TIME_US;
}

static inline uint16_t _mr_ofdm_csma_backoff_period_us(const ieee802154_mr_ofdm_conf_t *conf)
{
    (void)conf;

    return IEEE802154_CCA_DURATION_IN_SYMBOLS * IEEE802154_MR_OFDM_SYMBOL_TIME_US
           + IEEE802154G_ATURNAROUNDTIME_US;
}

MAYBE_UNUSED
static inline uint16_t _mr_ofdm_ack_timeout_us(const ieee802154_mr_ofdm_conf_t *conf)
{
    return _mr_ofdm_csma_backoff_period_us(conf)
           + IEEE802154G_ATURNAROUNDTIME_US
           + _mr_ofdm_frame_duration(conf->option, conf->scheme, IEEE802154_ACK_FRAME_LEN);
}

/*
 * MR-FSK timing calculations
 *
 * The standard unfortunately does not list the formula, instead it has to be pieced together
 * from scattered information and tables in the IEEE 802.15.4 document - may contain errors.
 */

MAYBE_UNUSED
static inline uint16_t _mr_fsk_csma_backoff_period_us(const ieee802154_mr_fsk_conf_t *conf)
{
    (void)conf;

    return IEEE802154_CCA_DURATION_IN_SYMBOLS * IEEE802154_MR_FSK_SYMBOL_TIME_US
           + IEEE802154G_ATURNAROUNDTIME_US;
}

MAYBE_UNUSED
static inline uint16_t _mr_fsk_ack_timeout_us(const ieee802154_mr_fsk_conf_t *conf)
{
    uint8_t ack_len = IEEE802154_ACK_FRAME_LEN;
    uint8_t fsk_pl = ieee802154_mr_fsk_plen(conf->srate);

    /* PHR uses same data rate as PSDU */
    ack_len += 2;

    /* 4-FSK doubles data rate */
    if (conf->mod_ord == 4) {
        ack_len /= 2;
    }

    /* forward error correction halves data rate */
    if (conf->fec) {
        ack_len *= 2;
    }

    return _mr_fsk_csma_backoff_period_us(conf)
           + IEEE802154G_ATURNAROUNDTIME_US
           /* long Preamble + SFD; SFD=2 */
           + ((fsk_pl * 8 + 2) + ack_len) * 8 * IEEE802154_MR_FSK_SYMBOL_TIME_US;
}

static int ieee802154_submac_config_phy(ieee802154_submac_t *submac,
                                        const ieee802154_phy_conf_t *conf)
{
    switch (conf->phy_mode) {
    case IEEE802154_PHY_OQPSK:
        submac->ack_timeout_us = ACK_TIMEOUT_US;
        submac->csma_backoff_us = CSMA_SENDER_BACKOFF_PERIOD_UNIT_US;
        break;
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
    case IEEE802154_PHY_MR_OQPSK:
        submac->ack_timeout_us = _mr_oqpsk_ack_timeout_us((void *)conf);
        submac->csma_backoff_us = _mr_oqpsk_csma_backoff_period_us((void *)conf);
        break;
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
    case IEEE802154_PHY_MR_OFDM:
        submac->ack_timeout_us = _mr_ofdm_ack_timeout_us((void *)conf);
        submac->csma_backoff_us = _mr_ofdm_csma_backoff_period_us((void *)conf);
        break;
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
    case IEEE802154_PHY_MR_FSK:
        submac->ack_timeout_us = _mr_fsk_ack_timeout_us((void *)conf);
        submac->csma_backoff_us = _mr_fsk_csma_backoff_period_us((void *)conf);
        break;
#endif
    case IEEE802154_PHY_NO_OP:
    case IEEE802154_PHY_DISABLED:
        break;
    default:
        return -EINVAL;
    }

    return ieee802154_radio_config_phy(&submac->dev, conf);
}

int ieee802154_submac_init(ieee802154_submac_t *submac, const network_uint16_t *short_addr,
                           const eui64_t *ext_addr)
{
    ieee802154_dev_t *dev = &submac->dev;

    submac->fsm.fsm_state_cb = _fsm_state_rx;

    int res;

    if ((res = ieee802154_radio_request_on(dev)) < 0) {
        return res;
    }

    /* generate EUI-64 and short address */
    memcpy(&submac->ext_addr, ext_addr, sizeof(eui64_t));
    memcpy(&submac->short_addr, short_addr, sizeof(network_uint16_t));
    submac->panid = CONFIG_IEEE802154_DEFAULT_PANID;

    submac->be.min = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MIN_BE;
    submac->csma_retries = CONFIG_IEEE802154_DEFAULT_CSMA_CA_RETRIES;
    submac->be.max = CONFIG_IEEE802154_DEFAULT_CSMA_CA_MAX_BE;

    submac->tx_pow = CONFIG_IEEE802154_DEFAULT_TXPOWER;

    if (ieee802154_radio_has_24_ghz(dev)) {
        submac->channel_num = CONFIG_IEEE802154_DEFAULT_CHANNEL;

        /* 2.4 GHz only use page 0 */
        submac->channel_page = 0;
    }
    else {
        submac->channel_num = CONFIG_IEEE802154_DEFAULT_SUBGHZ_CHANNEL;
        submac->channel_page = CONFIG_IEEE802154_DEFAULT_SUBGHZ_PAGE;
    }

    /* Get supported PHY modes */
    int supported_phy_modes = ieee802154_radio_get_phy_modes(dev);

    assert(supported_phy_modes != 0);

    uint32_t default_phy_cap = ieee802154_phy_mode_to_cap(CONFIG_IEEE802154_DEFAULT_PHY_MODE);

    /* Check if configuration provides valid PHY */
    if (CONFIG_IEEE802154_DEFAULT_PHY_MODE != IEEE802154_PHY_DISABLED &&
        (supported_phy_modes & default_phy_cap)) {
        /* Check if default PHY is supported */
        submac->phy_mode = CONFIG_IEEE802154_DEFAULT_PHY_MODE;
    }
    else {
        /* Get first set bit, and use it as the default,
         *
         * by this order, the priority is defined on the ieee802154_rf_caps_t
         * definition, first IEEE 802.15.4-2006 PHY modes, then
         * IEEE 802.15.4g-2012 PHY modes. */
        unsigned bit = bitarithm_lsb(supported_phy_modes);

        submac->phy_mode = ieee802154_cap_to_phy_mode(1 << bit);
    }

    /* If the radio is still not in TRX_OFF state, spin */
    while (ieee802154_radio_confirm_on(dev) == -EAGAIN) {}

    /* Configure address filter */
    ieee802154_radio_config_addr_filter(dev, IEEE802154_AF_SHORT_ADDR, &submac->short_addr);
    ieee802154_radio_config_addr_filter(dev, IEEE802154_AF_EXT_ADDR, &submac->ext_addr);
    ieee802154_radio_config_addr_filter(dev, IEEE802154_AF_PANID, &submac->panid);

    /* Configure PHY settings (mode, channel, TX power) */
    union {
        ieee802154_phy_conf_t super;
#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
        ieee802154_mr_oqpsk_conf_t mr_oqpsk;
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
        ieee802154_mr_ofdm_conf_t mr_ofdm;
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
        ieee802154_mr_fsk_conf_t mr_fsk;
#endif
    } conf;

#ifdef MODULE_NETDEV_IEEE802154_MR_OQPSK
    if (submac->phy_mode == IEEE802154_PHY_MR_OQPSK) {
        conf.mr_oqpsk.chips = CONFIG_IEEE802154_MR_OQPSK_DEFAULT_CHIPS;
        conf.mr_oqpsk.rate_mode = CONFIG_IEEE802154_MR_OQPSK_DEFAULT_RATE;
    }
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_OFDM
    if (submac->phy_mode == IEEE802154_PHY_MR_OFDM) {
        conf.mr_ofdm.option = CONFIG_IEEE802154_MR_OFDM_DEFAULT_OPTION;
        conf.mr_ofdm.scheme = CONFIG_IEEE802154_MR_OFDM_DEFAULT_SCHEME;
    }
#endif
#ifdef MODULE_NETDEV_IEEE802154_MR_FSK
    if (submac->phy_mode == IEEE802154_PHY_MR_FSK) {
        conf.mr_fsk.srate = CONFIG_IEEE802154_MR_FSK_DEFAULT_SRATE;
        conf.mr_fsk.mod_ord = CONFIG_IEEE802154_MR_FSK_DEFAULT_MOD_ORD;
        conf.mr_fsk.mod_idx = CONFIG_IEEE802154_MR_FSK_DEFAULT_MOD_IDX;
        conf.mr_fsk.fec = CONFIG_IEEE802154_MR_FSK_DEFAULT_FEC;
    }
#endif

    conf.super.phy_mode = submac->phy_mode;
    conf.super.channel = submac->channel_num;
    conf.super.page = submac->channel_page;
    conf.super.pow = submac->tx_pow;

    ieee802154_submac_config_phy(submac, &conf.super);
    ieee802154_radio_set_cca_threshold(dev,
                                       CONFIG_IEEE802154_CCA_THRESH_DEFAULT);
    assert(res >= 0);

    while (ieee802154_radio_set_rx(dev) < 0) {}

    return res;
}

int ieee802154_set_phy_conf(ieee802154_submac_t *submac, const ieee802154_phy_conf_t *conf)
{
    ieee802154_dev_t *dev = &submac->dev;
    int res;
    ieee802154_fsm_state_cb_t current_state = submac->fsm.fsm_state_cb;

    /* Changing state can be only performed on IDLE or RX state */
    if (current_state != _fsm_state_rx && current_state != _fsm_state_idle) {
        return -EBUSY;
    }

    /* If the radio is listening, turn it off first */
    if (current_state == _fsm_state_rx) {
        if ((res = ieee802154_radio_request_set_idle(dev, false)) < 0) {
            return res;
        }
    }

    res = ieee802154_submac_config_phy(submac, conf);

    if (res >= 0) {
        submac->channel_num = conf->channel;
        submac->channel_page = conf->page;
        submac->tx_pow = conf->pow;
        if (conf->phy_mode != IEEE802154_PHY_NO_OP) {
            submac->phy_mode = conf->phy_mode;
        }
    }
    while (ieee802154_radio_confirm_set_idle(dev) == -EAGAIN) {}

    /* Go back to RX if needed */
    if (current_state == _fsm_state_rx) {
        int rx = ieee802154_radio_set_rx(dev);
        assert(rx >= 0);
        (void)rx;
    }

    return res;
}

int ieee802154_set_rx(ieee802154_submac_t *submac)
{
    int res = ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_REQUEST_SET_RX_ON, 0);

    if (res < 0) {
        return res;
    }
    return submac->fsm_context_res;
}

int ieee802154_set_idle(ieee802154_submac_t *submac)
{
    int res = ieee802154_submac_process_ev(submac, IEEE802154_FSM_EV_REQUEST_SET_IDLE, 0);

    if (res < 0) {
        return res;
    }
    return submac->fsm_context_res;
}

bool ieee802154_submac_state_is_idle(ieee802154_submac_t *submac)
{
    return submac->fsm.fsm_state_cb == _fsm_state_idle;
}

bool ieee802154_submac_state_is_rx(ieee802154_submac_t *submac)
{
    return submac->fsm.fsm_state_cb == _fsm_state_rx;
}

/** @} */
