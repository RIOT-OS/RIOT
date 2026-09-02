/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_bplib_cla_uwb
 * @{
 *
 * @file
 * @brief       UWB CLA implementation
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */

#include "bplib_cla_uwb.h"
#include "bplib_init.h"

#include "assert.h"
#include "fmt.h"
#include "macros/utils.h"
#include "msg.h"
#include "net/ieee802154.h"

#include <inttypes.h>
#include <errno.h>
#include <stdbool.h>

#include "deca_device_api.h"

#define ENABLE_DEBUG 0
#define DEBUG_PREFIX "[deca CLA]"
#include "debug.h"

/* Shortcuts */
#define BUFFER_SIZE             CONFIG_BPLIB_CLA_UWB_BUFLEN
#define FRAME_LEN_MAX           CONFIG_BPLIB_CLA_UWB_MAX_FRAMELEN
#define ACK_RX_TIMEOUT          CONFIG_BPLIB_CLA_UWB_ACK_TO
#define NUM_RETRANS_ATTEMPTS    CONFIG_BPLIB_CLA_UWB_MAX_RETRANS

/* Static defines for the CLA */
#define CLA_HEADER_LEN          2
/* Not a LoWPAN (first 2 bits) shall be 0 */
#define CLA_HEADER_NALP_MASK    0x03
#define CLA_HEADER_BIT_VRS      0x04
#define CLA_HEADER_BIT_FRG      0x08
#define CLA_HEADER_BIT_FED      0x10

/* TX Thread flags */
#define THREAD_FLAG_ACK_RX      0x01
#define THREAD_FLAG_ACK_TO      0x02
#define THREAD_FLAG_RX_ERR      0x04

#define THREAD_FLAGS_SEND       (THREAD_FLAG_ACK_RX | \
                                 THREAD_FLAG_ACK_TO | \
                                 THREAD_FLAG_RX_ERR)

/* RX Thread flags */
#define THREAD_FLAG_RX_OK       0x01
#define THREAD_FLAG_TERM        0x02

#define THREAD_FLAGS_RECEIVE    (THREAD_FLAG_RX_OK | \
                                 THREAD_FLAG_TERM)

typedef enum {
    STATE_IDLE,
    STATE_RX,
    STATE_TX,
    STATE_TX_AWAITING_ACK
} dw3000_state_t;

typedef enum {
    TX_OK,
    TX_TO,
    TX_STOP
} tx_result_t;

typedef struct {
    /** Whether the CLA is running */
    volatile int running;
    /** Stack of the egress thread (bplib -> uwb send) */
    char stack_tx[BPLIB_CLA_UWB_TX_STACK_SIZE];
    /** Stack of the ingress thread (uwb recv -> bplib) */
    char stack_rx[BPLIB_CLA_UWB_RX_STACK_SIZE];

    thread_t* tx_thread; /**< Pointer to TX thread */
    thread_t* rx_thread; /**< Pointer to RX thread */

    network_uint16_t dst;
    le_uint16_t dst_pan;
    network_uint16_t src;
    le_uint16_t src_pan;

    /** Buffer for transmission */
    uint8_t tx_buffer[BUFFER_SIZE];
    /** Next to use sequence number on transmission layer */
    uint8_t tx_seq_no_trans;
    /** Next to use sequence number on frame layer */
    uint8_t tx_seq_no_frame;

    /** Buffer for reassembly and reception */
    uint8_t rx_buffer[BUFFER_SIZE];
    /** Last received sequence number on transmission layer */
    uint16_t rx_seq_no_trans;
    /** Last received sequence number on frame layer */
    uint16_t rx_seq_no_frame;
    /** Received bytes in the RX buffer */
    uint16_t rx_offset;

    /** Current state of the radio */
    dw3000_state_t radio_state;

    /** @brief Index of the contact in the contact table */
    uint32_t contact_id;
} bplib_cla_uwb_t;

static bplib_cla_uwb_t _cla;

static inline bool _rx_enable(void)
{
    if (_cla.radio_state == STATE_TX || _cla.radio_state == STATE_TX_AWAITING_ACK) {
        return false;
    }

    dwt_setrxtimeout(0);
    _cla.radio_state = STATE_RX;
    dwt_rxenable(DWT_START_RX_IMMEDIATE);

    return true;
}

/**
 * @brief Send a bundle with DW3000.
 *
 * This function handles fragmentation and blocks until all fragments have been
 * transferred (or the single frame) and have been ACKd.
 *
 * It also handles retransmissions, up to NUM_RETRANS_ATTEMPTS with some
 * back-off. After all transmissions the radio will be in RX again.
 *
 * @param[in] buf CBOR encoded bundle
 * @param[in] len Length of the bundle
 *
 * @return The status of the transmission
 */
static tx_result_t _send_tx(uint8_t* buf, uint16_t len)
{
    uint8_t mhr[IEEE802154_MAX_HDR_LEN];
    uint8_t cla_hdr[CLA_HEADER_LEN];
    thread_flags_t wait_flags;
    size_t hdr_len;
    uint16_t fragment_offset = 0;
    uint16_t fragment_data_size;
    uint16_t fragment_size_remaining;
    uint16_t fragment_size;
    uint8_t backoff_sleep = 1;
    tx_result_t res = TX_OK;

    DEBUG_PUTS(DEBUG_PREFIX" TX sending");

    cla_hdr[0] = 0;
    cla_hdr[1] = _cla.tx_seq_no_trans++;

    /* Here TX always has priority. This means also that a running fragmented RX
     * transmission will get blocked by TX. */

    dwt_forcetrxoff();
    dwt_setrxtimeout(ACK_RX_TIMEOUT);

    do {
        /* Generate MAC header */
        hdr_len = ieee802154_set_frame_hdr(mhr,
                                           _cla.src.u8, 2,
                                           _cla.dst.u8, 2,
                                           _cla.src_pan, _cla.dst_pan,
                                           IEEE802154_FCF_TYPE_DATA |
                                           IEEE802154_FCF_ACK_REQ,
                                           _cla.tx_seq_no_frame++);

        fragment_size_remaining = len - fragment_offset;
        fragment_data_size = FRAME_LEN_MAX - hdr_len - CLA_HEADER_LEN - IEEE802154_FCS_LEN;
        if (hdr_len + CLA_HEADER_LEN + len + IEEE802154_FCS_LEN > FRAME_LEN_MAX) {
            /* Need to fragment */
            cla_hdr[0] |= CLA_HEADER_BIT_FRG;
            if (fragment_size_remaining <= fragment_data_size) {
                cla_hdr[0] |= CLA_HEADER_BIT_FED;
            }
        }
        fragment_size = MIN(fragment_data_size, fragment_size_remaining);

        /* Fill frame buffer once. Retransmissions DONT need this again. */
        dwt_writetxdata(hdr_len, mhr, 0);
        dwt_writetxdata(CLA_HEADER_LEN, cla_hdr, hdr_len);
        dwt_writetxdata(fragment_size, buf + fragment_offset, hdr_len + CLA_HEADER_LEN);
        /* This function needs to include the two CRC bytes */
        dwt_writetxfctrl(hdr_len + CLA_HEADER_LEN + fragment_size + IEEE802154_FCS_LEN, 0, 0);

        /* Retransmission logic */
        for (uint8_t i = 0; i < NUM_RETRANS_ATTEMPTS; i++) {
            _cla.radio_state = STATE_TX;
            thread_flags_clear(THREAD_FLAGS_SEND);
            dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);

            wait_flags = thread_flags_wait_any(THREAD_FLAGS_SEND);

            if (wait_flags & THREAD_FLAG_ACK_RX) {
                /* Continue with next fragment (or return) */
                DEBUG_PUTS(DEBUG_PREFIX"  ACK Received");
                break;
            }
            else {
                /* Retransmit after wait. For now just use [ms] and linear
                 * backoff */
                DEBUG_PUTS(DEBUG_PREFIX"  ACK Timeout, retransmitting");
                if (i < NUM_RETRANS_ATTEMPTS - 1) {
                    ztimer_sleep(ZTIMER_MSEC, backoff_sleep++);
                    continue;
                }
                else {
                    res = TX_TO;
                    goto end;
                }
            }
        }
        fragment_offset += fragment_data_size;
    } while (fragment_size_remaining > fragment_data_size);

end:
    DEBUG_PUTS(DEBUG_PREFIX" TX complete");
    _cla.radio_state = STATE_IDLE;
    _rx_enable();
    return res;
}

static void _irq_tx_done_cb(const dwt_cb_data_t* dat)
{
    (void) dat;
    if (_cla.radio_state == STATE_TX) {
        _cla.radio_state = STATE_TX_AWAITING_ACK;
    }
    else if (_cla.radio_state == STATE_RX) {
        /* In RX a TX has to be an auto ACK. After that enable RX again. */
        _rx_enable();
    }
}

static void _irq_rx_ok_cb(const dwt_cb_data_t* dat)
{
    (void) dat;
    uint8_t mhr[IEEE802154_MAX_HDR_LEN + CLA_HEADER_LEN];
    uint8_t flags;
    uint8_t seq;
    uint8_t seq_frame;
    size_t hdr_len;
    uint16_t frame_len = dwt_getframelength(NULL);

    bool rx_done = false;

    dwt_readrxdata(mhr, IEEE802154_MAX_HDR_LEN, 0);

    if ((mhr[0] & IEEE802154_FCF_TYPE_ACK) && (_cla.radio_state == STATE_TX_AWAITING_ACK)) {
        thread_flags_set(_cla.tx_thread, THREAD_FLAG_ACK_RX);
    }
    else if ((_cla.radio_state == STATE_RX) && (mhr[0] & IEEE802154_FCF_TYPE_DATA)) {
        hdr_len = ieee802154_get_frame_hdr_len(mhr);
        seq_frame = ieee802154_get_seq(mhr);
        flags = mhr[hdr_len];
        seq = mhr[hdr_len + 1];

        if ((flags & CLA_HEADER_NALP_MASK) || (flags & CLA_HEADER_BIT_VRS)) {
            /* Not using version 0 */
            return;
        }

        if (seq_frame == _cla.rx_seq_no_frame) {
            /* Retransmission of already received frame (lost ACK?) */
            return;
        }

        if ((flags & CLA_HEADER_BIT_FRG) && (seq != _cla.rx_seq_no_trans)) {
            /* New fragmented transmission */
            _cla.rx_seq_no_trans = seq;
            _cla.rx_offset = 0;
            DEBUG_PUTS(DEBUG_PREFIX" RX Begin (fragmented)");
        }
        else if ((flags & CLA_HEADER_BIT_FRG) && (flags & CLA_HEADER_BIT_FED)) {
            /* End of fragmented transmission */
            rx_done = true;
        }
        else if ((flags & CLA_HEADER_BIT_FRG) && !(flags & CLA_HEADER_BIT_FED)) {
            /* Middle of some fragmented transmission */
            DEBUG_PUTS(DEBUG_PREFIX"  +1 fragment");
        }
        else if (!(flags & CLA_HEADER_BIT_FRG)) {
            /* Atomic transmission */
            _cla.rx_seq_no_trans = seq;
            _cla.rx_offset = 0;
            rx_done = true;
            DEBUG_PUTS(DEBUG_PREFIX" RX Begin (non fragmented)");
        }
        else {
            /* Something invalid / not supported, ignore it */
            return;
        }

        /* Frame length is now the length of the useful data */
        frame_len = frame_len - (hdr_len + CLA_HEADER_LEN + IEEE802154_FCS_LEN);

        if (_cla.rx_offset + frame_len > BUFFER_SIZE) {
            /* Whatever we get is too large for the buffer */
            return;
        }

        dwt_readrxdata(_cla.rx_buffer + _cla.rx_offset,
                       frame_len,
                       hdr_len + CLA_HEADER_LEN);
        _cla.rx_offset += frame_len;
        _cla.rx_seq_no_frame = seq_frame;

        if (rx_done) {
            /* The RX is re-enabled after ingress, preventing the buffer from
             * being overwritten */
            DEBUG_PUTS(DEBUG_PREFIX" RX End");
            _cla.radio_state = STATE_IDLE;
            thread_flags_set(_cla.rx_thread, THREAD_FLAG_RX_OK);
        }

        /* RX will be re-enabled when the auto ACK has been sent, OR, when a
         * bundle has arrived completely, it will be re-enabled after the ingress.
         * This prevents buffer overwrites. */
    }
}

static void _irq_rx_err_cb(const dwt_cb_data_t* dat)
{
    (void) dat;
    DEBUG(DEBUG_PREFIX" RX error: 0x%"PRIx32"\n", dat->status);
    if (_cla.radio_state == STATE_RX) {
        _rx_enable();
    }

    thread_flags_set(_cla.tx_thread, THREAD_FLAG_RX_ERR);
}

static void _irq_rx_to_cb(const dwt_cb_data_t *dat)
{
    (void)dat;
    /* Normal RX does not time out, so it is currently waiting for an ACK */
    thread_flags_set(_cla.tx_thread, THREAD_FLAG_ACK_TO);
}

dwt_callbacks_s _dwt_callbacks = {
    .cbTxDone = _irq_tx_done_cb,
    .cbRxOk = _irq_rx_ok_cb,
    .cbRxErr = _irq_rx_err_cb,
    .cbRxTo = _irq_rx_to_cb,
};

static void* _cla_uwb_out(void* arg)
{
    (void) arg;
    BPLib_Status_t egress_status;
    size_t out_size;

    while (_cla.running && bplib_instance_data.running) {
        egress_status = BPLib_CLA_Egress(&bplib_instance_data.BPLibInst, _cla.contact_id,
            _cla.tx_buffer, &out_size, BUFFER_SIZE, CONFIG_BPLIB_CLA_UWB_TIMEOUT);
        if (egress_status == BPLIB_SUCCESS) {
            _send_tx(_cla.tx_buffer, out_size);
        }
        else if (egress_status != BPLIB_CLA_TIMEOUT) {
            printf("Error egressing, RC=%"PRId32"\n", egress_status);
        }
    }
    return NULL;
}

static void* _cla_uwb_in(void* arg)
{
    (void) arg;
    BPLib_Status_t bp_status;
    thread_flags_t wait_flags;

    while (_cla.running && bplib_instance_data.running) {
        /* Wait for receive here */
        wait_flags = thread_flags_wait_any(THREAD_FLAGS_RECEIVE);
        if (wait_flags & THREAD_FLAG_TERM) {
            return NULL;
        }

        bp_status = BPLib_CLA_Ingress(&bplib_instance_data.BPLibInst, _cla.contact_id,
                                      _cla.rx_buffer, _cla.rx_offset, 0);
        if (bp_status != BPLIB_SUCCESS) {
            printf("BPLib_CLA_Ingress Fail RC=%"PRId32"\n", bp_status);
        }

        /* Usually we wait for the ACK to be sent before RX enable. Since bplib
         * takes quite some time this should not be needed here and would need
         * even more weird synchronization paths */
        _rx_enable();
    }

    return NULL;
}

int bplib_cla_uwb_start(uint32_t contact_id)
{
    const char* addr_str;
    int rc;
    uint16_t pan;
    uint16_t addr;

    if (_cla.running) {
        return -EINVAL;
    }

    /* Parse the out-address. Here an address string is to be understood as
     * SHORT_ADDR, in hexadecimal, without leading 0x, resulting in a
     * total length of 4. Long addresses are currently not supported and the
     * addresses and PAN should not be changed outside of the CLA */
    addr_str = bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaOutAddr;
    if (strlen(addr_str) != 4) {
        return -EINVAL;
    }
    addr = scn_u32_hex(addr_str, 4);
    _cla.dst = byteorder_htons(addr);

    /* The remote PAN here is given as the port of the in configuration */
    pan = bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaOutPort;
    _cla.dst_pan = byteorder_htols(pan);
    DEBUG(DEBUG_PREFIX" Sending to [ADDR @ PAN]: 0x%04"PRIx16" @ 0x%04"PRIx16"\n", addr, pan);

    /* The in-address is read in the same way as the out-addr and is set to the
     * configured netdev. */
    addr_str = bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaInAddr;
    if (strlen(addr_str) != 4) {
        return -EINVAL;
    }
    addr = scn_u32_hex(addr_str, 4);
    _cla.src = byteorder_htons(addr);

    /* The local PAN here is given as the port of the in configuration */
    pan = bplib_instance_data.ConfigPtrs.ContactsConfigPtr->ContactSet[contact_id].ClaInPort;
    _cla.src_pan = byteorder_htols(pan);
    DEBUG(DEBUG_PREFIX" Listening at [ADDR @ PAN]: 0x%04"PRIx16" @ 0x%04"PRIx16"\n", addr, pan);

    /* Apply options if nothing failed */
    dwt_setpanid(pan);
    dwt_setaddress16(addr);

    dwt_configureframefilter(DWT_FF_ENABLE_802_15_4, DWT_FF_DATA_EN |
                                                     DWT_FF_ACK_EN);
    dwt_enableautoack(10, 1);

    dwt_setcallbacks(&_dwt_callbacks);
    dwt_setinterrupt(DWT_INT_RXFCG_BIT_MASK | /* <-- RX success */
                     /* RX errors: */
                     DWT_INT_RXPHE_BIT_MASK |
                     DWT_INT_RXFCE_BIT_MASK |
                     DWT_INT_RXFSL_BIT_MASK |
                     DWT_INT_RXSTO_BIT_MASK |
                     DWT_INT_CIAERR_BIT_MASK |
                     DWT_INT_ARFE_BIT_MASK |
                     /* RX timeouts: */
                     DWT_INT_RXFTO_BIT_MASK |
                     DWT_INT_RXPTO_BIT_MASK |
                     /* TX done: */
                     DWT_INT_TXFRS_BIT_MASK,
                     0, DWT_ENABLE_INT_ONLY);

    _cla.radio_state = STATE_IDLE;
    _cla.rx_seq_no_trans = 0xff00;
    _cla.rx_seq_no_frame = 0xff00;

    /* This should be true before because it is the threads exit condition */
    _cla.running = 1;
    _cla.contact_id = contact_id;

    rc = thread_create(_cla.stack_rx, BPLIB_CLA_UWB_RX_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 2, 0, _cla_uwb_in,
        NULL, "bplib-cla-uwb-rx");
    if (rc < 0) {
        _cla.running = 0;
        return rc;
    }
    _cla.rx_thread = thread_get(rc);

    rc = thread_create(_cla.stack_tx, BPLIB_CLA_UWB_TX_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 2, 0, _cla_uwb_out,
        NULL, "bplib-cla-uwb-tx");
    if (rc < 0) {
        _cla.running = 0;
        return rc;
    }
    _cla.tx_thread = thread_get(rc);

    _rx_enable();

    return 0;
}

void bplib_cla_uwb_stop(void)
{
    _cla.running = 0;
    thread_flags_set(_cla.rx_thread, THREAD_FLAG_TERM);
}
