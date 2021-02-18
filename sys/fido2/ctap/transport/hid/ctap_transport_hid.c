/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap_transport_hid
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */

#include <string.h>

#ifdef CONFIG_CTAP_NATIVE
#include <sys/time.h>
#endif

#include "xtimer.h"
#include "cbor.h"

#include "fido2/ctap.h"
#include "fido2/ctap/transport/ctap_transport.h"
#include "fido2/ctap/transport/hid/ctap_transport_hid.h"
#include "fido2/ctap/ctap_utils.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief HID report descriptor
 *
 * CTAP specification (version 20190130) section 8.1.8.2
 */
static const uint8_t report_desc_ctap[] = {
    0x06, 0xD0, 0xF1,   // HID_UsagePage ( FIDO_USAGE_PAGE ),
    0x09, 0x01,         // HID_Usage ( FIDO_USAGE_CTAPHID ),
    0xA1, 0x01,         // HID_Collection ( HID_Application ),
    0x09, 0x20,         // HID_Usage ( FIDO_USAGE_DATA_IN ),
    0x15, 0x00,         // HID_LogicalMin ( 0 ),
    0x26, 0xFF, 0x00,   // HID_LogicalMaxS ( 0xff ),
    0x75, 0x08,         // HID_ReportSize ( 8 ),
    0x95, 0x40,         // HID_ReportCount ( HID_INPUT_REPORT_BYTES ),
    0x81, 0x02,         // HID_Input ( HID_Data | HID_Absolute | HID_Variable ),
    0x09, 0x21,         // HID_Usage ( FIDO_USAGE_DATA_OUT ),
    0x15, 0x00,         // HID_LogicalMin ( 0 ),
    0x26, 0xFF, 0x00,   // HID_LogicalMaxS ( 0xff ),
    0x75, 0x08,         // HID_ReportSize ( 8 ),
    0x95, 0x40,         // HID_ReportCount ( HID_OUTPUT_REPORT_BYTES ),
    0x91, 0x02,         // HID_Output ( HID_Data | HID_Absolute | HID_Variable ),
    0xC0,               // HID_EndCollection
};

/**
 * @brief Serialize data and transmit it via USB HID layer
 */
static void ctap_hid_write(uint8_t cmd, uint32_t cid, const void *_data, size_t size);

/**
 * @brief CTAPHID_CBOR command
 *
 * CTAP specification (version 20190130) section 8.1.9.1.2
 */
static void handle_cbor_packet(uint32_t cid, uint16_t bcnt, uint8_t cmd, uint8_t *payload);

/**
 * @brief CTAPHID_INIT command
 *
 * CTAP specification (version 20190130) section 8.1.9.1.3
 */
static uint32_t handle_init_packet(uint32_t cid, uint16_t bcnt,
                                   const uint8_t *nonce);

/**
 * @brief CTAPHID_WINK command
 *
 * CTAP specification (version 20190130) section 8.1.9.2.1
 */
static void wink(uint32_t cid, uint8_t cmd);

/**
 * @brief Encode response to CTAPHID_INIT command
 */
static void send_init_response(uint32_t cid_old, uint32_t cid_new,
                               const uint8_t *nonce);

static void reset_ctap_buffer(void);
static uint8_t buffer_pkt(const ctap_hid_pkt_t *pkt);
static void send_error_response(uint32_t cid, uint8_t err);
static int8_t refresh_cid(uint32_t cid);
static int8_t add_cid(uint32_t cid);
static int8_t delete_cid(uint32_t cid);
static bool cid_exists(uint32_t cid);
static inline uint32_t get_new_cid(void);
static inline uint16_t get_packet_len(const ctap_hid_pkt_t *pkt);
static void pkt_worker(void);
static inline bool is_init_type_pkt(const ctap_hid_pkt_t *pkt);
static inline bool should_cancel(void);

static bool g_is_busy = false;
static ctap_hid_buffer_t g_ctap_buffer;
/* logical channels */
static ctap_hid_cid_t g_cids[CTAP_HID_CIDS_MAX];
/* channel id 0 is reserved */
static uint32_t g_cid = 1;

void fido2_ctap_transport_hid_create(void)
{
    fido2_ctap_transport_create(CTAP_TRANSPORT_USB, (void *)report_desc_ctap,
                                sizeof(report_desc_ctap));
}

void fido2_ctap_transport_hid_handle_packet(void *pkt_raw, int size)
{
    (void)size;
    ctap_hid_pkt_t *pkt = (ctap_hid_pkt_t *)pkt_raw;
    uint32_t cid = pkt->cid;
    uint8_t status = CTAP_HID_BUFFER_STATUS_BUFFERING;

    if (cid == 0x00) {
        /* cid = 0x00 always invalid */
        send_error_response(cid, CTAP_HID_ERR_INVALID_CHANNEL);
    }
    else if (g_is_busy) {
        if (g_ctap_buffer.cid == cid) {
            /* CTAP specification (version 20190130) section 8.1.5.3 */
            if (is_init_type_pkt(pkt)) {
                if (pkt->init.cmd == CTAP_HID_COMMAND_INIT) {
                    /* abort */
                    reset_ctap_buffer();
                    status = buffer_pkt(pkt);
                }
                else if (g_ctap_buffer.is_locked && pkt->init.cmd ==
                         CTAP_HID_COMMAND_CANCEL) {
                    g_ctap_buffer.should_cancel = true;
                }
                /* random init type pkt. invalid sequence of pkts */
                else {
                    send_error_response(cid, CTAP_HID_ERR_INVALID_SEQ);
                }
            }
            /* packet for this cid is currently being worked */
            else if (g_ctap_buffer.is_locked) {
                send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
            }
            else {
                /* buffer cont packets */
                status = buffer_pkt(pkt);
            }
        }
        /* transactions are atomic, deny all other cids if busy with one cid */
        else {
            send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
        }
    }
    else {
        /* first init packet received starts a transaction */
        if (is_init_type_pkt(pkt)) {
            g_is_busy = true;
            status = buffer_pkt(pkt);
        }
        /* ignore rest */
    }

    if (status == CTAP_HID_BUFFER_STATUS_ERROR) {
        send_error_response(cid, g_ctap_buffer.err);
        delete_cid(cid);
        reset_ctap_buffer();
        g_is_busy = false;
    }
    /* pkt->init.bcnt bytes have been received. Wakeup worker */
    else if (status == CTAP_HID_BUFFER_STATUS_DONE) {
        g_ctap_buffer.is_locked = 1;
        pkt_worker();
        g_is_busy = false;
    }
    else {
        /* refresh timestamp of cid that is being buffered */
        refresh_cid(g_ctap_buffer.cid);
    }
}

static uint8_t buffer_pkt(const ctap_hid_pkt_t *pkt)
{
    if (is_init_type_pkt(pkt)) {
        /* received should_cancel for cid being buffered atm, should_cancel as
           long as worker not awoken */
        if (pkt->init.cmd == CTAP_HID_COMMAND_CANCEL && !g_ctap_buffer.is_locked &&
            pkt->cid == g_ctap_buffer.cid) {

            g_ctap_buffer.err = CTAP2_ERR_KEEPALIVE_CANCEL;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        g_ctap_buffer.bcnt = get_packet_len(pkt);

        /* check for init transaction size described in CTAP specification
           (version 20190130) section 8.1.9.1.3 */
        if (pkt->init.cmd == CTAP_HID_COMMAND_INIT && g_ctap_buffer.bcnt != 8) {
            g_ctap_buffer.err = CTAP_HID_ERR_INVALID_LEN;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        /* don't allow transactions bigger than max buffer size */
        if (g_ctap_buffer.bcnt > CTAP_HID_BUFFER_SIZE) {
            g_ctap_buffer.err = CTAP_HID_ERR_INVALID_LEN;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        uint16_t size = (g_ctap_buffer.bcnt < CTAP_HID_INIT_PAYLOAD_SIZE) ?
                        g_ctap_buffer.bcnt : CTAP_HID_INIT_PAYLOAD_SIZE;
        g_ctap_buffer.cmd = pkt->init.cmd;
        g_ctap_buffer.cid = pkt->cid;
        g_ctap_buffer.seq = -1;
        memmove(g_ctap_buffer.buffer, pkt->init.payload, size);
        g_ctap_buffer.offset = size;
    }
    else {
        int left = g_ctap_buffer.bcnt - g_ctap_buffer.offset;
        int diff = left - CTAP_HID_CONT_PAYLOAD_SIZE;
        g_ctap_buffer.seq++;

        /* seqs have to increase sequentially */
        if (pkt->cont.seq != g_ctap_buffer.seq) {
            g_ctap_buffer.err = CTAP_HID_ERR_INVALID_SEQ;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        /* check for potential buffer overflow */
        if (g_ctap_buffer.offset + CTAP_HID_CONT_PAYLOAD_SIZE > CTAP_HID_BUFFER_SIZE) {
            g_ctap_buffer.err = CTAP_HID_ERR_INVALID_LEN;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        if (diff <= 0) {
            memmove(g_ctap_buffer.buffer + g_ctap_buffer.offset, pkt->cont.payload, left);
            g_ctap_buffer.offset += left;
        }
        else {
            memmove(g_ctap_buffer.buffer + g_ctap_buffer.offset, pkt->cont.payload,
                    CTAP_HID_CONT_PAYLOAD_SIZE);
            g_ctap_buffer.offset += CTAP_HID_CONT_PAYLOAD_SIZE;
        }
    }

    return g_ctap_buffer.offset == g_ctap_buffer.bcnt ?
           CTAP_HID_BUFFER_STATUS_DONE : CTAP_HID_BUFFER_STATUS_BUFFERING;
}

static void pkt_worker(void)
{
    uint8_t *buf = (uint8_t *)&g_ctap_buffer.buffer;
    uint32_t cid = g_ctap_buffer.cid;
    uint16_t bcnt = g_ctap_buffer.bcnt;
    uint8_t cmd = g_ctap_buffer.cmd;

    if (cmd == CTAP_HID_COMMAND_INIT) {
        cid = handle_init_packet(cid, bcnt, buf);
    }
    else {
        /* broadcast cid only allowed for CTAP_HID_COMMAND_INIT */
        if (cid == CTAP_HID_BROADCAST_CID || cid == 0) {
            send_error_response(cid, CTAP_HID_ERR_INVALID_CHANNEL);
        }
        /* readding deleted cid */
        else if (!cid_exists(cid) && add_cid(cid) == -1) {
            send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
        }
        else {
            switch (cmd) {
            case CTAP_HID_COMMAND_MSG:
                /* not implemented as of now */
                DEBUG("CTAP_HID: MSG COMMAND \n");
                send_error_response(cid, CTAP_HID_ERR_INVALID_CMD);
                break;
            case CTAP_HID_COMMAND_CBOR:
                DEBUG("CTAP_HID: CBOR COMMAND \n");
                handle_cbor_packet(cid, bcnt, cmd, buf);
                break;
            case CTAP_HID_COMMAND_WINK:
                DEBUG("CTAP_HID: wink \n");
                wink(cid, cmd);
                break;
            case CTAP_HID_COMMAND_PING:
                DEBUG("CTAP_HID: PING \n");
                ctap_hid_write(cmd, cid, buf, bcnt);
                break;
            case CTAP_HID_COMMAND_CANCEL:
                /*
                 * no transaction is currently being processed,
                 * no reason to send cancel
                 */
                break;
            default:
                send_error_response(cid, CTAP_HID_ERR_INVALID_CMD);
                DEBUG("Ctaphid: unknown command %u \n", cmd);
            }
        }
    }

    /* transaction done, cleanup */
    reset_ctap_buffer();
}

static uint32_t handle_init_packet(uint32_t cid, uint16_t bcnt,
                                   const uint8_t *nonce)
{
    uint32_t cid_new = 0;

    /* cid 0 is reserved */
    if (cid == 0) {
        send_error_response(cid, CTAP_HID_ERR_INVALID_CHANNEL);
        return 0;
    }
    /* check for len described in standard */
    if (bcnt != 8) {
        send_error_response(cid, CTAP_HID_ERR_INVALID_LEN);
        return 0;
    }
    /* create new channel */
    if (cid == CTAP_HID_BROADCAST_CID) {
        cid_new = get_new_cid();
        if (add_cid(cid_new) == -1) {
            send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
            return 0;
        }
        send_init_response(cid, cid_new, nonce);
    }
    /* synchronize channel */
    else {
        cid_new = cid;
        if (!cid_exists(cid)) {
            if (add_cid(cid) == -1) {
                /* reached cid limit */
                send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
                return 0;
            }
        }
        send_init_response(cid, cid, nonce);
    }

    return cid_new;
}

static void handle_cbor_packet(uint32_t cid, uint16_t bcnt, uint8_t cmd,
                               uint8_t *payload)
{
    ctap_resp_t resp;
    uint8_t err;
    uint8_t type = *payload;
    size_t size;

    if (bcnt == 0) {
        err = CTAP_HID_ERR_INVALID_LEN;
        cmd = CTAP_HID_COMMAND_ERROR;
        ctap_hid_write(cmd, cid, &err, sizeof(err));
        return;
    }

    memset(&resp, 0, sizeof(ctap_resp_t));

    size = fido2_ctap_handle_request(payload, bcnt, &resp, &should_cancel);

    if (resp.status == CTAP2_OK && size > 0) {
        /* status + data */
        ctap_hid_write(cmd, cid, &resp, size + sizeof(resp.status));
    }
    else {
        /* status only */
        ctap_hid_write(cmd, cid, &resp.status, sizeof(resp.status));
    }
}

static inline bool is_init_type_pkt(const ctap_hid_pkt_t *pkt)
{
    return ((pkt->init.cmd & CTAP_HID_INIT_PACKET) == CTAP_HID_INIT_PACKET);
}

static void reset_ctap_buffer(void)
{
    memset(&g_ctap_buffer, 0, sizeof(g_ctap_buffer));
}

static inline bool should_cancel(void)
{
    return g_ctap_buffer.should_cancel;
}

static inline uint32_t get_new_cid(void)
{
    return g_cid++;
}

void fido2_ctap_transport_hid_check_timeouts(void)
{
    uint64_t now = xtimer_now_usec64();

    for (uint8_t i = 0; i < CTAP_HID_CIDS_MAX; i++) {
        /* transaction timed out because cont packets didnt arrive in time */
        if (g_is_busy && g_cids[i].taken &&
            (now - g_cids[i].last_used) >= CTAP_HID_TRANSACTION_TIMEOUT &&
            g_ctap_buffer.cid == g_cids[i].cid && !g_ctap_buffer.is_locked) {

            send_error_response(g_cids[i].cid, CTAP_HID_ERR_MSG_TIMEOUT);
            delete_cid(g_cids[i].cid);
            reset_ctap_buffer();

            g_is_busy = false;
        }
    }
}

static int8_t add_cid(uint32_t cid)
{
    uint64_t oldest = xtimer_now_usec64();
    int8_t index_oldest = -1;

    for (int i = 0; i < CTAP_HID_CIDS_MAX; i++) {
        if (!g_cids[i].taken) {
            g_cids[i].taken = true;
            g_cids[i].cid = cid;
            g_cids[i].last_used = xtimer_now_usec64();

            return CTAP_HID_OK;
        }

        if (g_cids[i].last_used < oldest) {
            oldest = g_cids[i].last_used;
            index_oldest = i;
        }
    }

    /* remove oldest cid to make place for a new one */
    if (index_oldest > -1) {
        g_cids[index_oldest].taken = true;
        g_cids[index_oldest].cid = cid;
        g_cids[index_oldest].last_used = xtimer_now_usec64();
        return CTAP_HID_OK;
    }

    return CTAP_HID_ERR_OTHER;
}

static int8_t refresh_cid(uint32_t cid)
{
    for (int i = 0; i < CTAP_HID_CIDS_MAX; i++) {
        if (g_cids[i].cid == cid) {
            g_cids[i].last_used = xtimer_now_usec64();
            return CTAP_HID_OK;
        }
    }
    return CTAP_HID_ERR_OTHER;
}

static int8_t delete_cid(uint32_t cid)
{
    for (int i = 0; i < CTAP_HID_CIDS_MAX; i++) {
        if (g_cids[i].cid == cid) {
            g_cids[i].taken = false;
            g_cids[i].cid = 0;

            return CTAP_HID_OK;
        }
    }
    return CTAP_HID_ERR_OTHER;
}

static bool cid_exists(uint32_t cid)
{
    for (int i = 0; i < CTAP_HID_CIDS_MAX; i++) {
        if (g_cids[i].cid == cid) {
            return true;
        }
    }
    return false;
}

static inline uint16_t get_packet_len(const ctap_hid_pkt_t *pkt)
{
    return (uint16_t)((pkt->init.bcnth << 8) | pkt->init.bcntl);
}

static void wink(uint32_t cid, uint8_t cmd)
{
#if !IS_ACTIVE(CONFIG_FIDO2_CTAP_DISABLE_LED)
    uint32_t delay = CTAP_HID_WINK_DELAY;
    for (int i = 1; i <= 8; i++) {
#ifdef LED0_TOGGLE
        LED0_TOGGLE;
        xtimer_msleep(delay);
#endif
#ifdef LED1_TOGGLE
        LED1_TOGGLE;
        xtimer_msleep(delay);
#endif
#ifdef LED2_TOGGLE
        LED2_TOGGLE;
        xtimer_msleep(delay);
#endif
#ifdef LED3_TOGGLE
        LED3_TOGGLE;
        xtimer_msleep(delay);
#endif
        delay /= 2;
    }
#endif /* CONFIG_FIDO2_CTAP_DISABLE_LED  */

    ctap_hid_write(cmd, cid, NULL, 0);
}

void fido2_ctap_transport_hid_send_keepalive(uint8_t status)
{
    ctap_hid_write(CTAP_HID_COMMAND_KEEPALIVE, g_ctap_buffer.cid, &status,
                   sizeof(status));
}

static void send_error_response(uint32_t cid, uint8_t err)
{
    DEBUG("ctap_trans_hid err resp: %02x \n", err);
    ctap_hid_write(CTAP_HID_COMMAND_ERROR, cid, &err, sizeof(err));
}

static void send_init_response(uint32_t cid_old, uint32_t cid_new,
                               const uint8_t *nonce)
{
    ctap_hid_init_resp_t resp;

    memset(&resp, 0, sizeof(ctap_hid_init_resp_t));

    resp.cid = cid_new;
    memmove(resp.nonce, nonce, 8);
    resp.protocol_version = CTAP_HID_PROTOCOL_VERSION;
    resp.version_major = 0;
    resp.version_minor = 0;
    resp.build_version = 0;

    uint8_t cmd = (CTAP_HID_INIT_PACKET | CTAP_HID_COMMAND_INIT);

    resp.capabilities = CTAP_HID_CAPABILITY_CBOR | CTAP_HID_CAPABILITY_WINK
                        | CTAP_HID_CAPABILITY_NMSG;

    ctap_hid_write(cmd, cid_old, &resp, sizeof(ctap_hid_init_resp_t));
}

static void ctap_hid_write(uint8_t cmd, uint32_t cid, const void *_data, size_t len)
{
    const uint8_t *data = (uint8_t *)_data;
    uint8_t buf[CONFIG_USBUS_HID_INTERRUPT_EP_SIZE] = { 0 };
    uint16_t bytes_written = 0;
    uint8_t seq = 0;
    uint8_t offset = 0;

    memmove(buf, &cid, sizeof(cid));
    offset += sizeof(cid);
    buf[offset++] = cmd;
    /* high part of payload length first */
    buf[offset++] = (len & 0xff00) >> 8;
    buf[offset++] = (len & 0xff) >> 0;

    if (data == NULL) {
        fido2_ctap_transport_write(CTAP_TRANSPORT_USB, buf,
                                   CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
        return;
    }

    for (size_t i = 0; i < len; i++) {
        if (offset == 0) {
            memmove(buf, &cid, sizeof(cid));
            offset += sizeof(cid);

            /* initialization packet */
            if (bytes_written == 0) {
                buf[offset++] = cmd;
                buf[offset++] = (len & 0xff00) >> 8;
                buf[offset++] = (len & 0xff) >> 0;
            }
            /* continuation packet */
            else {
                buf[offset++] = seq++;
            }
        }

        buf[offset++] = data[i];
        bytes_written++;

        if (offset == CONFIG_USBUS_HID_INTERRUPT_EP_SIZE) {
            fido2_ctap_transport_write(CTAP_TRANSPORT_USB, buf,
                                       CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
            offset = 0;
        }
    }

    if (offset > 0) {
        memset(buf + offset, 0, CONFIG_USBUS_HID_INTERRUPT_EP_SIZE - offset);
        fido2_ctap_transport_write(CTAP_TRANSPORT_USB, buf,
                                   CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
    }
}
