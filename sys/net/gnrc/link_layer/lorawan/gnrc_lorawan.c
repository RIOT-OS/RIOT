/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "errno.h"
#include "kernel_defines.h"

#include "net/lora.h"
#include "net/gnrc/lorawan.h"
#include "net/gnrc/pktbuf.h"
#include "net/lorawan/hdr.h"
#include "net/loramac.h"
#include "net/gnrc/lorawan/region.h"
#include "random.h"
#include "timex.h"

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
#include "periph/flashpage.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define GNRC_LORAWAN_DL_RX2_DR_MASK       (0x0F)    /**< DL Settings DR Offset mask */
#define GNRC_LORAWAN_DL_RX2_DR_POS        (0)       /**< DL Settings DR Offset pos */
#define GNRC_LORAWAN_DL_DR_OFFSET_MASK    (0x70)    /**< DL Settings RX2 DR mask */
#define GNRC_LORAWAN_DL_DR_OFFSET_POS     (4)       /**< DL Settings RX2 DR pos */

static gnrc_lorawan_fsm_status_t _state_idle(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);
static gnrc_lorawan_fsm_status_t _state_wait_rx_window(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);
static gnrc_lorawan_fsm_status_t _state_rx_window(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);
static gnrc_lorawan_fsm_status_t _state_tx(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);

static gnrc_lorawan_fsm_status_t _state_mac_state_link_down(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);
static gnrc_lorawan_fsm_status_t _state_mac_state_wait_join_req(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);
static gnrc_lorawan_fsm_status_t _state_mac_state_idle(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);
//static gnrc_lorawan_fsm_status_t _state_mac_state_wait_ack_req(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);
static gnrc_lorawan_fsm_status_t _state_mac_state_tx(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev);

static void _ev_timer(event_t *ev);
static void _ev_mac_timer(event_t *ev);
static void _ev_toa(event_t *ev);

static inline void gnrc_lorawan_mlme_reset(gnrc_lorawan_t *mac)
{
    mac->mlme.activation = MLME_ACTIVATION_NONE;
    mac->mlme.pending_mlme_opts = 0;
    mac->rx_delay = (CONFIG_LORAMAC_DEFAULT_RX1_DELAY / MS_PER_SEC);
    mac->mlme.nid = CONFIG_LORAMAC_DEFAULT_NETID;
    memset(mac->mlme.dev_nonce, 0x00, sizeof(mac->mlme.dev_nonce));
}

static inline void gnrc_lorawan_mlme_backoff_init(gnrc_lorawan_t *mac)
{
    mac->mlme.backoff_state = 0;

    gnrc_lorawan_mlme_backoff_expire_cb(mac);
}

static inline void gnrc_lorawan_mcps_reset(gnrc_lorawan_t *mac)
{
    mac->mcps.ack_requested = false;
    mac->mcps.waiting_for_ack = false;
    mac->mcps.fcnt = 0;
    mac->mcps.fcnt_down = 0;
    gnrc_lorawan_set_uncnf_redundancy(mac, CONFIG_LORAMAC_DEFAULT_REDUNDANCY);
}

void gnrc_lorawan_set_rx2_dr(gnrc_lorawan_t *mac, uint8_t rx2_dr)
{
    mac->dl_settings &= ~GNRC_LORAWAN_DL_RX2_DR_MASK;
    mac->dl_settings |= (rx2_dr << GNRC_LORAWAN_DL_RX2_DR_POS) &
                        GNRC_LORAWAN_DL_RX2_DR_MASK;
}

static void _sleep_radio(gnrc_lorawan_t *mac)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    netopt_state_t state = NETOPT_STATE_SLEEP;

    dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
}

static void _load_persistent_state(gnrc_lorawan_t *mac)
{
    (void) mac;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    void *addr = flashpage_addr(GNRC_LORAWAN_STATE_FLASHPAGE_NUM);
    gnrc_lorawan_persistent_state_t state = { 0 };

    memcpy(&state, addr, sizeof(state));

    if (state.initialized_marker != GNRC_LORAWAN_INITIALIZED_MARKER) {
        memset(&state, 0x0, sizeof(state));
        state.initialized_marker = GNRC_LORAWAN_INITIALIZED_MARKER;

        flashpage_erase(GNRC_LORAWAN_STATE_FLASHPAGE_NUM);
        /* ensure written length is multiple of FLASHPAGE_WRITE_BLOCK_SIZE */
        flashpage_write(addr, &state, (sizeof(state) /
                                           FLASHPAGE_WRITE_BLOCK_SIZE + 0x1) *
                            FLASHPAGE_WRITE_BLOCK_SIZE);
    }
    memcpy(mac->mlme.dev_nonce, state.dev_nonce, sizeof(mac->mlme.dev_nonce));
#endif
}

void gnrc_lorawan_init(gnrc_lorawan_t *mac, uint8_t *joineui, const gnrc_lorawan_key_ctx_t *ctx, event_queue_t *evq)
{
    DEBUG("Lorawan init !\n");
    mac->joineui = joineui;

    memcpy(&mac->ctx, ctx, sizeof(gnrc_lorawan_key_ctx_t));

    mac->ev_timer.handler = _ev_timer;
    mac->ev_mac.handler = _ev_mac_timer;
    mac->ev_toa.handler = _ev_toa;

    event_timeout_ztimer_init(&mac->evt, ZTIMER_MSEC, evq, &mac->ev_timer);
    event_timeout_ztimer_init(&mac->evt_mac, ZTIMER_MSEC, evq, &mac->ev_mac);
    event_timeout_ztimer_init(&mac->evt_toa, ZTIMER_MSEC, evq, &mac->ev_toa);

    gnrc_lorawan_mlme_backoff_init(mac);
    gnrc_lorawan_reset(mac);

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
        _load_persistent_state(mac);
    }

    event_timeout_set(&mac->evt_toa, GNRC_LORAWAN_BACKOFF_WINDOW_TICK);
}

void gnrc_lorawan_reset(gnrc_lorawan_t *mac)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    uint8_t cr = LORA_CR_4_5;

    dev->driver->set(dev, NETOPT_CODING_RATE, &cr, sizeof(cr));

    uint8_t syncword = IS_ACTIVE(CONFIG_LORAMAC_DEFAULT_PRIVATE_NETWORK) ? LORA_SYNCWORD_PRIVATE
                                                                         : LORA_SYNCWORD_PUBLIC;

    dev->driver->set(dev, NETOPT_SYNCWORD, &syncword, sizeof(syncword));

    /* Continuous reception */
    uint32_t rx_timeout = 0;

    dev->driver->set(dev, NETOPT_RX_TIMEOUT, &rx_timeout, sizeof(rx_timeout));

    gnrc_lorawan_set_rx2_dr(mac, CONFIG_LORAMAC_DEFAULT_RX2_DR);

    mac->toa = 0;
    gnrc_lorawan_mcps_reset(mac);
    gnrc_lorawan_mlme_reset(mac);
    gnrc_lorawan_channels_init(mac);

    /* By default the MAC is disconnected */
    mac->phy_fsm = _state_idle;
    mac->mac_fsm = _state_mac_state_link_down;

    /* Trigger the entry state of each state machine */
    _state_idle(mac, GNRC_LORAWAN_EV_ENTRY);
    _state_mac_state_link_down(mac, GNRC_LORAWAN_EV_ENTRY);
}

void gnrc_lorawan_store_dev_nonce(uint8_t *dev_nonce)
{
    (void) dev_nonce;
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    void *addr = flashpage_addr(GNRC_LORAWAN_STATE_FLASHPAGE_NUM);
    gnrc_lorawan_persistent_state_t state = { 0 };
    memcpy(&state, addr, sizeof(state));

    memcpy(state.dev_nonce, dev_nonce, sizeof(state.dev_nonce));

    flashpage_erase(GNRC_LORAWAN_STATE_FLASHPAGE_NUM);
    /* ensure written length is multiple of FLASHPAGE_WRITE_BLOCK_SIZE */
    flashpage_write(addr, &state, (sizeof(state) /
                    FLASHPAGE_WRITE_BLOCK_SIZE + 0x1) *
                    FLASHPAGE_WRITE_BLOCK_SIZE);
#endif
}

static void _config_radio(gnrc_lorawan_t *mac, uint32_t channel_freq,
                          uint8_t dr, int rx, bool rx_single)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);

    netopt_state_t state = NETOPT_STATE_STANDBY;
    dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
    if (channel_freq != 0) {
        dev->driver->set(dev, NETOPT_CHANNEL_FREQUENCY, &channel_freq,
                         sizeof(channel_freq));
    }

    netopt_enable_t iq_invert = rx;

    dev->driver->set(dev, NETOPT_IQ_INVERT, &iq_invert, sizeof(iq_invert));

    gnrc_lorawan_set_dr(mac, dr);

    if (rx) {
        /* Switch to single listen mode */
        const netopt_enable_t single = rx_single;
        uint16_t timeout = CONFIG_GNRC_LORAWAN_MIN_SYMBOLS_TIMEOUT;
        dev->driver->set(dev, NETOPT_SINGLE_RECEIVE, &single, sizeof(single));
        dev->driver->set(dev, NETOPT_RX_SYMBOL_TIMEOUT, &timeout,
                         sizeof(timeout));
    }
}

static void _configure_rx_window(gnrc_lorawan_t *mac, uint32_t channel_freq,
                                 uint8_t dr, bool rx_single)
{
    _config_radio(mac, channel_freq, dr, true, rx_single);
}

void gnrc_lorawan_dispatch_event(gnrc_lorawan_t *mac, gnrc_lorawan_state_t *fsm, gnrc_lorawan_event_t event)
{
    gnrc_lorawan_state_t last_state = *fsm;
    int last_event = event;
    int res;

    /* This line may update the state if there's a state transition */
    while ((res = (*fsm)(mac, last_event)) == GNRC_LORAWAN_FSM_TRANSITION) {
        res = last_state(mac, GNRC_LORAWAN_EV_EXIT);
        assert(res != GNRC_LORAWAN_FSM_TRANSITION);
        last_event = GNRC_LORAWAN_EV_ENTRY;
    }
}

static void _ev_toa(event_t *ev)
{
    gnrc_lorawan_t *mac = container_of(ev, gnrc_lorawan_t, ev_toa);
    gnrc_lorawan_mlme_backoff_expire_cb(mac);
    event_timeout_set(&mac->evt_toa, GNRC_LORAWAN_BACKOFF_WINDOW_TICK);
}

static void _ev_mac_timer(event_t *ev)
{
    gnrc_lorawan_t *mac = container_of(ev, gnrc_lorawan_t, ev_mac);
    gnrc_lorawan_dispatch_event(mac, &mac->mac_fsm, GNRC_LORAWAN_EV_TO);
}

static void _ev_timer(event_t *ev)
{
    gnrc_lorawan_t *mac = container_of(ev, gnrc_lorawan_t, ev_timer);
    gnrc_lorawan_dispatch_event(mac, &mac->phy_fsm, GNRC_LORAWAN_EV_TO);
}

void gnrc_lorawan_send_pkt(gnrc_lorawan_t *mac, iolist_t *psdu, uint8_t dr,
                           uint32_t chan_idx)
{
    assert(psdu);
    mac->last_dr = dr;
    mac->last_chan_idx = chan_idx;
    mac->psdu = psdu;
    gnrc_lorawan_dispatch_event(mac, &mac->phy_fsm, GNRC_LORAWAN_EV_REQUEST_TX);
}

static gnrc_lorawan_fsm_status_t _state_transition(gnrc_lorawan_state_t *fsm, gnrc_lorawan_state_t next_state)
{
    *fsm = next_state;
    return GNRC_LORAWAN_FSM_TRANSITION;
}

static void _transmit_pkt(gnrc_lorawan_t *mac)
{
    size_t mhdr_size = sizeof(lorawan_hdr_t) + 1 +
                       lorawan_hdr_get_frame_opts_len((void *)mac->mcps.mhdr_mic);

    iolist_t header =
    { .iol_base = mac->mcps.mhdr_mic, .iol_len = mhdr_size,
      .iol_next = mac->mcps.msdu };
    iolist_t footer =
    { .iol_base = mac->mcps.mhdr_mic + header.iol_len, .iol_len = MIC_SIZE,
      .iol_next = NULL };
    iolist_t *last_snip = mac->mcps.msdu;

    while (last_snip->iol_next != NULL) {
        last_snip = last_snip->iol_next;
    }

    uint16_t conf_fcnt = 0;

    if (IS_USED(MODULE_GNRC_LORAWAN_1_1)) {
        /**
         * If the ACK bit of the uplink frame is set, meaning this frame is
         * acknowledging a downlink “confirmed” frame, then ConfFCnt is the frame
         * counter value modulo 2^16 of the “confirmed” downlink frame that is being
         * acknowledged. In all other cases ConfFCnt = 0x0000
         */
        lorawan_hdr_t *lw_hdr = (lorawan_hdr_t *)header.iol_base;
        if (lorawan_hdr_get_ack(lw_hdr)) {
            conf_fcnt = gnrc_lorawan_get_last_fcnt_down(mac);
        }
    }

    gnrc_lorawan_calculate_mic_uplink(&header, conf_fcnt, mac, footer.iol_base);

    last_snip->iol_next = &footer;
    gnrc_lorawan_send_pkt(mac, &header, mac->last_dr,
                          gnrc_lorawan_pick_channel(mac));

    /* cppcheck-suppress redundantAssignment
     * (reason: cppcheck bug. The pointer is temporally modified to add a footer.
     *          The `gnrc_lorawan_send_pkt` function uses this hack to append
     *          the MIC independently of `gnrc_pktsnip_t` structures) */
    last_snip->iol_next = NULL;
}

void gnrc_lorawan_event_retrans_timeout(gnrc_lorawan_t *mac)
{
    _transmit_pkt(mac);
}

/************************************* MAC FSM ********************************/

static gnrc_lorawan_fsm_status_t _state_mac_state_tx_join_req(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    switch (ev) {
    case GNRC_LORAWAN_EV_ENTRY:
        event_timeout_set(&mac->evt_mac, (random_uint32() & GNRC_LORAWAN_JOIN_DELAY_U32_MASK) / 1000);
        return GNRC_LORAWAN_FSM_HANDLED;
    case GNRC_LORAWAN_EV_TO:
        gnrc_lorawan_trigger_join(mac);
        return _state_transition(&mac->mac_fsm, _state_mac_state_wait_join_req);
    case GNRC_LORAWAN_EV_EXIT:
        return GNRC_LORAWAN_FSM_IGNORED;
    default:
        assert(false);
    }
    return GNRC_LORAWAN_FSM_IGNORED;
}

static gnrc_lorawan_fsm_status_t _state_mac_state_link_down(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    switch (ev) {
    case GNRC_LORAWAN_EV_ENTRY:
    case GNRC_LORAWAN_EV_EXIT:
        return GNRC_LORAWAN_FSM_IGNORED;
    case GNRC_LORAWAN_EV_LINK_UP:
        if (mac->mlme.activation == MLME_ACTIVATION_ABP) {
            return _state_transition(&mac->mac_fsm, _state_mac_state_idle);
        }
        else {
            return _state_transition(&mac->mac_fsm, _state_mac_state_tx_join_req);
        }
    case GNRC_LORAWAN_EV_PHY_READY:
        return GNRC_LORAWAN_FSM_IGNORED;
    default:
        assert(false);
        return GNRC_LORAWAN_FSM_IGNORED;
    }
}

static gnrc_lorawan_fsm_status_t _state_mac_state_wait_join_req(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    mlme_confirm_t mlme_confirm;
    mlme_confirm.type = MLME_JOIN;
    switch (ev) {
    case GNRC_LORAWAN_EV_ENTRY:
        return GNRC_LORAWAN_FSM_IGNORED;

    case GNRC_LORAWAN_EV_EXIT:
        return GNRC_LORAWAN_FSM_IGNORED;

    case GNRC_LORAWAN_EV_RX_ERROR:
    case GNRC_LORAWAN_EV_PHY_READY:
        mlme_confirm.status = -ETIMEDOUT;
        gnrc_lorawan_mlme_confirm(mac, &mlme_confirm);
        return _state_transition(&mac->mac_fsm, _state_mac_state_link_down);
    case GNRC_LORAWAN_EV_RX_DONE:
        mlme_confirm.status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
        gnrc_lorawan_mlme_confirm(mac, &mlme_confirm);
        return _state_transition(&mac->mac_fsm, _state_mac_state_idle);
    default:
        assert(false);
        return GNRC_LORAWAN_FSM_IGNORED;
    }
}

bool gnrc_lorawan_is_busy(gnrc_lorawan_t *mac)
{
    return ((mac->mac_fsm != _state_mac_state_idle
             && mac->mac_fsm != _state_mac_state_link_down)
            || mac->phy_fsm != _state_idle);
}

bool gnrc_lorawan_is_joined(gnrc_lorawan_t *mac)
{
    return (mac->mac_fsm != _state_mac_state_link_down
            && mac->mac_fsm != _state_mac_state_wait_join_req
            && mac->mac_fsm != _state_mac_state_tx_join_req);
}

static gnrc_lorawan_fsm_status_t _state_mac_state_idle(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    (void) mac;
    switch (ev) {
    case GNRC_LORAWAN_EV_ENTRY:
        mac->mcps.msdu = NULL;
        mac->mcps.waiting_for_ack = false;
        return GNRC_LORAWAN_FSM_HANDLED;
    case GNRC_LORAWAN_EV_EXIT:
        return GNRC_LORAWAN_FSM_IGNORED;
    case GNRC_LORAWAN_EV_REQUEST_TX:
        return _state_transition(&mac->mac_fsm, _state_mac_state_tx);
    case GNRC_LORAWAN_EV_PHY_READY:
        return GNRC_LORAWAN_FSM_IGNORED;
    default:
        assert(false);
    }
    return GNRC_LORAWAN_FSM_IGNORED;
}

static gnrc_lorawan_fsm_status_t _state_mac_state_ifs(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    switch (ev) {
    case GNRC_LORAWAN_EV_ENTRY:
        event_timeout_set(&mac->evt_mac, 1000 + random_uint32_range(0, 2000));
        return GNRC_LORAWAN_FSM_HANDLED;
    case GNRC_LORAWAN_EV_TO:
        return _state_transition(&mac->mac_fsm, _state_mac_state_tx);
    case GNRC_LORAWAN_EV_EXIT:
        return GNRC_LORAWAN_FSM_IGNORED;
    default:
        assert(false);
    }

    return GNRC_LORAWAN_FSM_IGNORED;
}

static gnrc_lorawan_fsm_status_t _state_mac_state_wait_ack_req(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{

    switch (ev) {
    case GNRC_LORAWAN_EV_ENTRY:
        /* Set ACK Timeout... */
        /* TODO: De-hardcode to ACK_Timeout */
        event_timeout_set(&mac->evt_mac, LORAMAC_DEFAULT_ACK_TIMEOUT * MS_PER_SEC);
        return GNRC_LORAWAN_FSM_HANDLED;
    case GNRC_LORAWAN_EV_TO:
        if (mac->mcps.waiting_for_ack && mac->mcps.nb_trials > 0) {
            mac->mcps.nb_trials--;
            return _state_transition(&mac->mac_fsm, _state_mac_state_ifs);
        }
        else {
            /* Finish transmission */
            mac->mcps.fcnt++;
            mcps_confirm_t mcps_confirm;
            mcps_confirm.type = MCPS_CONFIRMED;
            mcps_confirm.status = mac->mcps.nb_trials > 0 ? GNRC_LORAWAN_REQ_STATUS_SUCCESS : -ETIMEDOUT;
            mcps_confirm.msdu = mac->mcps.msdu;
            gnrc_lorawan_mcps_confirm(mac, &mcps_confirm);
            return _state_transition(&mac->mac_fsm, _state_mac_state_idle);
        }
    case GNRC_LORAWAN_EV_PHY_READY:
        return GNRC_LORAWAN_FSM_IGNORED;
    case GNRC_LORAWAN_EV_EXIT:
        return GNRC_LORAWAN_FSM_IGNORED;
    default:
        assert(false);
    }

    return GNRC_LORAWAN_FSM_IGNORED;
}

static gnrc_lorawan_fsm_status_t _state_mac_state_tx(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    switch (ev) {
    case GNRC_LORAWAN_EV_TO:
    case GNRC_LORAWAN_EV_ENTRY:
        /* Transmit packet on entry */
        _transmit_pkt(mac);
        break;
    case GNRC_LORAWAN_EV_PHY_READY:
        /* Check whether this is a confirmed transmission or not */
        if (mac->mcps.waiting_for_ack) {
            return _state_transition(&mac->mac_fsm, _state_mac_state_wait_ack_req);
        }
        else if (mac->mcps.nb_trials--) {
            /* If we got here, this means the redundancy mode is activated */
            /* Schedule another transmission */
            event_timeout_set(&mac->evt_mac, 1000 + random_uint32_range(0, 2000));
            return GNRC_LORAWAN_FSM_HANDLED;
        }
        else {
            /* Finish transmission */
            mac->mcps.fcnt++;
            mcps_confirm_t mcps_confirm;
            mcps_confirm.type = MCPS_UNCONFIRMED;
            mcps_confirm.status = GNRC_LORAWAN_REQ_STATUS_SUCCESS;
            mcps_confirm.msdu = mac->mcps.msdu;
            gnrc_lorawan_mcps_confirm(mac, &mcps_confirm);
            return _state_transition(&mac->mac_fsm, _state_mac_state_idle);
            /* Otherwise, finish the MAC state immediately */
        }

    case GNRC_LORAWAN_EV_EXIT:
        return GNRC_LORAWAN_FSM_IGNORED;
    default:
        assert(false);
    }

    return GNRC_LORAWAN_FSM_IGNORED;
}

/************************************* PHY FSM ********************************/

static void _process_rx_done(gnrc_lorawan_t *mac)
{
    if (!IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C)) {
        _sleep_radio(mac);
    }
    uint8_t *psdu = mac->psdu->iol_base;
    uint8_t size = mac->psdu->iol_len;
    uint8_t mtype = (*(psdu) & MTYPE_MASK) >> 5;

    switch (mtype) {
    case MTYPE_JOIN_ACCEPT:
        gnrc_lorawan_mlme_process_join(mac, psdu, size);
        break;
    case MTYPE_CNF_DOWNLINK:
    case MTYPE_UNCNF_DOWNLINK:
        gnrc_lorawan_mcps_process_downlink(mac, psdu, size);
        break;
    default:
        break;
    }
}

static gnrc_lorawan_fsm_status_t _state_rx_window(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    netopt_state_t state;
    switch(ev) {
        case GNRC_LORAWAN_EV_ENTRY:
            if (mac->rx_state == GNRC_LORAWAN_RXW_1) {
                uint8_t dr_offset = (mac->dl_settings & GNRC_LORAWAN_DL_DR_OFFSET_MASK) >>
                                    GNRC_LORAWAN_DL_DR_OFFSET_POS;

                _configure_rx_window(mac, mac->channel[mac->last_chan_idx],
                                     gnrc_lorawan_rx1_get_dr_offset(mac->last_dr, dr_offset),
                                     true);
            }
            else {
                assert(!IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C));
                _configure_rx_window(mac, CONFIG_LORAMAC_DEFAULT_RX2_FREQ,
                                 mac->dl_settings &
                                 GNRC_LORAWAN_DL_RX2_DR_MASK,
                                 true);
            }
            /* Open RX Window */
            state = NETOPT_STATE_RX;
            dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
            return GNRC_LORAWAN_FSM_HANDLED;
        case GNRC_LORAWAN_EV_EXIT:
            mac->rx_state = GNRC_LORAWAN_RXW_2;
            return GNRC_LORAWAN_FSM_HANDLED;
        case GNRC_LORAWAN_EV_RX_TO:
            if (IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C)
                || mac->rx_state == GNRC_LORAWAN_RXW_2) {
                /* In class A the device will go to Idle at the end of RX2 */
                /* In class C the device goes as soon as possible to Idle */
                return _state_transition(&mac->phy_fsm, _state_idle);
            }
            else if (mac->rx_state == GNRC_LORAWAN_RXW_1) {
                /* Set timeout for RX2 */
                uint16_t now = ztimer_now(ZTIMER_MSEC);
                uint16_t delta = (uint16_t) (now - mac->rxw_ts);
                assert(delta <= MS_PER_SEC);
                if (!IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C)) {
                    event_timeout_set(&mac->evt, MS_PER_SEC - delta);
                }
                return _state_transition(&mac->phy_fsm, _state_wait_rx_window);
            }
            break;
        case GNRC_LORAWAN_EV_TO:
            assert(!IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C));
            /* This occurs when the packet is longer than the RX window duration.
             * We set a maximum timeout in case the device does not trigger RX_DONE
             */
            if (mac->rx_state != GNRC_LORAWAN_RX_PENDING) {
                mac->rx_state = GNRC_LORAWAN_RX_PENDING;
                event_timeout_set(&mac->evt, 2000);
            }
            else {
                /* If we get here again, go back to IDLE */
                return _state_transition(&mac->phy_fsm, _state_idle);
            }
            break;
        case GNRC_LORAWAN_EV_RX_ERROR:
            return _state_transition(&mac->phy_fsm, _state_idle);
        case GNRC_LORAWAN_EV_RX_DONE:
            _process_rx_done(mac);
            return _state_transition(&mac->phy_fsm, _state_idle);
        default:
            assert(false);
            break;
    }

    return GNRC_LORAWAN_FSM_IGNORED;
}

static gnrc_lorawan_fsm_status_t _state_wait_rx_window(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    switch(ev) {
        case GNRC_LORAWAN_EV_ENTRY:
            if (mac->rx_state == GNRC_LORAWAN_RXW_1) {
                /* Configure timeout */
                /* if the MAC is not activated, then this is a Join Request */
                int rx_1 = mac->mlme.activation == MLME_ACTIVATION_NONE ?
                       CONFIG_LORAMAC_DEFAULT_JOIN_DELAY1 : mac->rx_delay;

                event_timeout_set(&mac->evt, rx_1 * MS_PER_SEC);
            }

            if (IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C) && mac->mlme.activation != MLME_ACTIVATION_NONE) {
                /* Open RX Window */
                _configure_rx_window(mac, CONFIG_LORAMAC_DEFAULT_RX2_FREQ,
                                 mac->dl_settings &
                                 GNRC_LORAWAN_DL_RX2_DR_MASK, false);
                netopt_state_t state = NETOPT_STATE_RX;
                dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
            }
            else {
                _sleep_radio(mac);
            }
            return GNRC_LORAWAN_FSM_HANDLED;

        case GNRC_LORAWAN_EV_RX_ERROR:
            assert(IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C));
            return GNRC_LORAWAN_FSM_IGNORED;
        case GNRC_LORAWAN_EV_RX_DONE:
            assert(IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C));
            _process_rx_done(mac);
            return GNRC_LORAWAN_FSM_HANDLED;
            break;
        case GNRC_LORAWAN_EV_EXIT:
            return GNRC_LORAWAN_FSM_IGNORED;
        case GNRC_LORAWAN_EV_TO:
            /* Tag last 16-bit of timestamp */
            mac->rxw_ts = (uint16_t) ztimer_now(ZTIMER_MSEC);
            return _state_transition(&mac->phy_fsm, _state_rx_window);
        default:
            assert(false);
            break;
    }
    return GNRC_LORAWAN_FSM_IGNORED;
}

static gnrc_lorawan_fsm_status_t _state_tx(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    switch(ev) {
        case GNRC_LORAWAN_EV_ENTRY:
            /* Send packet */
            _config_radio(mac, mac->channel[mac->last_chan_idx], mac->last_dr, false, false);
            uint8_t cr;

            dev->driver->get(dev, NETOPT_CODING_RATE, &cr, sizeof(cr));
            mac->toa = lora_time_on_air(iolist_size(mac->psdu), mac->last_dr, cr);
            if (dev->driver->send(dev, mac->psdu) == -ENOTSUP) {
                DEBUG("gnrc_lorawan: Cannot send: radio is still transmitting");
            }
            return GNRC_LORAWAN_FSM_HANDLED;
        case GNRC_LORAWAN_EV_TX_DONE: {
            /* Indicate upper layer */
            return _state_transition(&mac->phy_fsm, _state_wait_rx_window);
        }
        case GNRC_LORAWAN_EV_EXIT:
            mac->rx_state = GNRC_LORAWAN_RXW_1;
            return GNRC_LORAWAN_FSM_HANDLED;
        default:
            assert(false);
            break;
    }
    return GNRC_LORAWAN_FSM_IGNORED;
}

static void _set_idle(gnrc_lorawan_t *mac)
{
    netdev_t *dev = gnrc_lorawan_get_netdev(mac);
    if (IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C) && mac->mlme.activation != MLME_ACTIVATION_NONE) {
        _configure_rx_window(mac, CONFIG_LORAMAC_DEFAULT_RX2_FREQ,
                         mac->dl_settings &
                         GNRC_LORAWAN_DL_RX2_DR_MASK, false);
        netopt_state_t state = NETOPT_STATE_RX;
        dev->driver->set(dev, NETOPT_STATE, &state, sizeof(state));
    }
    else {
        _sleep_radio(mac);
    }
}

static gnrc_lorawan_fsm_status_t _state_idle(gnrc_lorawan_t *mac, gnrc_lorawan_event_t ev)
{
    switch(ev) {
        case GNRC_LORAWAN_EV_ENTRY:
            _set_idle(mac);
            gnrc_lorawan_dispatch_event(mac, &mac->mac_fsm, GNRC_LORAWAN_EV_PHY_READY);
            return GNRC_LORAWAN_FSM_HANDLED;
        case GNRC_LORAWAN_EV_EXIT:
            return GNRC_LORAWAN_FSM_IGNORED;
        case GNRC_LORAWAN_EV_REQUEST_TX: {
            return _state_transition(&mac->phy_fsm, _state_tx);
        }
        break;
        case GNRC_LORAWAN_EV_RX_ERROR:
            assert(IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C));
            _set_idle(mac);
            return GNRC_LORAWAN_FSM_HANDLED;
        case GNRC_LORAWAN_EV_RX_DONE:
            assert(IS_ACTIVE(CONFIG_GNRC_LORAWAN_CLASS_C));
            _process_rx_done(mac);
            _set_idle(mac);
            return GNRC_LORAWAN_FSM_HANDLED;
        default:
            assert(false);
            break;
    }
    return GNRC_LORAWAN_FSM_IGNORED;
}
