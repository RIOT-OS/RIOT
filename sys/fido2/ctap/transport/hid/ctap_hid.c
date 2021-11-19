/*
 * Copyright (C) 2021 Freie Universität Berlin
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
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 * @}
 */

#include <string.h>

#include "xtimer.h"
#include "usb/usbus.h"
#include "usb/usbus/hid.h"
#include "usb/usbus/hid_io.h"

#include "fido2/ctap.h"
#include "fido2/ctap/transport/hid/ctap_hid.h"
#include "fido2/ctap/ctap_utils.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief CTAP HID report descriptor
 *
 * CTAP specification (version 20190130) section 8.1.8.2
 */
const uint8_t _hid_report_desc[] = {
    USB_HID_USAGE_PAGE16(USB_HID_USAGE_FIDO),
    USB_HID_USAGE(USB_HID_USAGE_FIDO_U2F_AUTHENTICATOR_DEVICE),
    USB_HID_COLLECTION(USB_HID_COLLECTION_APPLICATION),
        USB_HID_USAGE(USB_HID_USAGE_FIDO_INPUT_REPORT_DATA),
        USB_HID_LOGICAL_MIN8(0),
        USB_HID_LOGICAL_MAX16(255),
        USB_HID_REPORT_SIZE(8),
        USB_HID_REPORT_COUNT(CONFIG_USBUS_HID_INTERRUPT_EP_SIZE),
        USB_HID_INPUT(0x02),
        USB_HID_USAGE(USB_HID_USAGE_FIDO_OUTPUT_REPORT_DATA),
        USB_HID_LOGICAL_MIN8(0),
        USB_HID_LOGICAL_MAX16(255),
        USB_HID_REPORT_SIZE(8),
        USB_HID_REPORT_COUNT(CONFIG_USBUS_HID_INTERRUPT_EP_SIZE),
        USB_HID_OUTPUT(0x02),
    USB_HID_END_COLLECTION,
};

/**
 * @brief CTAP_HID buffer struct
 *
 */
typedef struct {
    uint32_t cid;                           /**< channel identifier */
    uint8_t cmd;                            /**< CTAP_HID command */
    uint8_t buffer[CTAP_HID_BUFFER_SIZE];   /**< data buffer */
    uint16_t offset;                        /**< current offset into data buffer */
    int16_t seq;                            /**< current sequence number */
    uint16_t bcnt;                          /**< expected amount of bytes to be received */
    uint8_t err;                            /**< error type if error  */
    bool is_locked;                         /**< buffer is locked by transaction */
    bool should_cancel;                     /**< flag if current transaction should be cancelled */
} ctap_hid_state_t;

/**
 * @brief Serialize data and transmit it via USB HID layer
 */
static void _ctap_hid_write(uint8_t cmd, uint32_t cid, const void *_data, size_t size);

/**
 * @brief CTAPHID_CBOR command
 *
 * CTAP specification (version 20190130) section 8.1.9.1.2
 */
static void _handle_cbor_packet(uint8_t cmd, uint32_t cid, uint8_t *buf, uint16_t bcnt);

/**
 * @brief CTAPHID_INIT command
 *
 * CTAP specification (version 20190130) section 8.1.9.1.3
 */
static uint32_t _handle_init_packet(uint32_t cid, uint16_t bcnt,
                                    const uint8_t *nonce);

/**
 * @brief CTAPHID_WINK command
 *
 * CTAP specification (version 20190130) section 8.1.9.2.1
 */
static void _wink(uint32_t cid, uint8_t cmd);

/**
 * @brief Encode response to CTAPHID_INIT command
 */
static void _send_init_response(uint32_t cid_old, uint32_t cid_new,
                                const uint8_t *nonce);

/**
 * @brief Clear the CTAP packet buffer
 */
static void _clear_ctap_buffer(void);

/**
 * @brief Buffer packet belonging to currently processed transaction
 */
static uint8_t _buffer_pkt(const ctap_hid_pkt_t *pkt);

/**
 * @brief Send error code to cid
 */
static void _send_error_response(uint32_t cid, uint8_t err);

/**
 * @brief Refresh the last_used timestamp for this cid
 */
static int8_t _refresh_cid(uint32_t cid);

/**
 * @brief Allocate a new logical channel
 */
static int8_t _add_cid(uint32_t cid);

/**
 * @brief Delete logical channel
 */
static int8_t _delete_cid(uint32_t cid);

/**
 * @brief Check if a logical channel with cid exists
 */
static bool _cid_exists(uint32_t cid);

/**
 * @brief Parse packet length from pkt
 */
static inline uint16_t _get_packet_len(const ctap_hid_pkt_t *pkt);

/**
 * @brief Process CTAPHID transaction
 */
static void _process_transaction(event_t *arg);

/**
 * @brief Check if packet is an initialization packet
 */
static inline bool _is_init_type_pkt(const ctap_hid_pkt_t *pkt);

/* usbus functionality */

/**
 * @brief USB stack
 */
static char _usb_stack[USBUS_STACKSIZE];

/**
 * @brief USBUS context
 */
static usbus_t _usbus;

/**
 * @brief Indicate if authenticator is busy processing a transactions
 *
 * Transactions are atomic, therefore only 1 transaction can be processed at
 * once
 */
static bool _is_busy = false;

/**
 * @brief State for handling transactions
 */
static ctap_hid_state_t _state;

/**
 * @brief Logical CTAPHID channels
 */
static ctap_hid_cid_t g_cids[CTAP_HID_CIDS_MAX];

/**
 * @brief Incremental channel ids
 *
 * channel id 0 is reserved
 */
static uint32_t _cid = 1;

/**
 * @brief CTAP transport layer event queue
 */
static event_queue_t *_queue;

/**
 * @brief CTAPHID event
 */
static event_t _ctap_hid_event = { .handler = _process_transaction };

/**
 * @brief USBUS context
 */
static usbus_t _usbus;

static void _usb_cb(void *arg)
{
    (void)arg;

    uint8_t buffer[CONFIG_USBUS_HID_INTERRUPT_EP_SIZE];
    int read;

    read = usb_hid_io_read(buffer, CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);

    if (read == CONFIG_USBUS_HID_INTERRUPT_EP_SIZE) {
        fido2_ctap_transport_hid_handle_packet(buffer);
    }
}

void fido2_ctap_transport_hid_init(event_queue_t *queue)
{
    _queue = queue;
    usbdev_t *usbdev = usbdev_get_ctx(0);

    assert(usbdev);
    usbus_init(&_usbus, usbdev);
    usb_hid_io_init(&_usbus, _hid_report_desc, sizeof(_hid_report_desc));
    usb_hid_io_set_rx_cb(_usb_cb, NULL);
    usbus_create(_usb_stack, sizeof(_usb_stack), USBUS_PRIO, USBUS_TNAME, &_usbus);
}

void fido2_ctap_transport_hid_handle_packet(void *pkt_raw)
{
    ctap_hid_pkt_t *pkt = (ctap_hid_pkt_t *)pkt_raw;
    uint32_t cid = pkt->cid;
    uint8_t status = CTAP_HID_BUFFER_STATUS_BUFFERING;

    if (cid == 0x00) {
        /* cid = 0x00 always invalid */
        _send_error_response(cid, CTAP_HID_ERR_INVALID_CHANNEL);
        return;
    }
    else if (_is_busy) {
        if (_state.cid == cid) {
            /* CTAP specification (version 20190130) section 8.1.5.3 */
            if (_is_init_type_pkt(pkt)) {
                if (pkt->init.cmd == CTAP_HID_COMMAND_INIT) {
                    /* abort */
                    _clear_ctap_buffer();
                    status = _buffer_pkt(pkt);
                }
                else if (_state.is_locked && pkt->init.cmd ==
                         CTAP_HID_COMMAND_CANCEL) {
                    _state.should_cancel = true;
                }
                /* random init type pkt. invalid sequence of pkts */
                else {
                    _send_error_response(cid, CTAP_HID_ERR_INVALID_SEQ);
                    return;
                }
            }
            /* packet for this cid is currently being worked */
            else if (_state.is_locked) {
                _send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
                return;
            }
            else {
                /* buffer cont packets */
                status = _buffer_pkt(pkt);
            }
        }
        /* transactions are atomic. Deny all other cids if busy with one cid */
        else {
            _send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
            return;
        }
    }
    else {
        /* first init packet received starts a transaction */
        if (_is_init_type_pkt(pkt)) {
            _is_busy = true;
            status = _buffer_pkt(pkt);
        }
        /* ignore rest */
    }

    if (status == CTAP_HID_BUFFER_STATUS_ERROR) {
        _send_error_response(cid, _state.err);
        _delete_cid(cid);
        _clear_ctap_buffer();
        _is_busy = false;
        return;
    }

    /* pkt->init.bcnt bytes have been received. Transaction can now be processed */
    if (status == CTAP_HID_BUFFER_STATUS_DONE) {
        _state.is_locked = 1;
        event_post(_queue, &_ctap_hid_event);
        _is_busy = false;
    }
    else {
        /* refresh timestamp of cid that is being buffered */
        _refresh_cid(_state.cid);
    }
}

static uint8_t _buffer_pkt(const ctap_hid_pkt_t *pkt)
{
    if (_is_init_type_pkt(pkt)) {
        /**
         * broadcast cid only allowed for CTAP_HID_COMMAND_INIT
         */
        if (pkt->cid == CTAP_HID_BROADCAST_CID &&
            pkt->init.cmd != CTAP_HID_COMMAND_INIT) {
            _send_error_response(pkt->cid, CTAP_HID_ERR_INVALID_CHANNEL);
        }

        /**
         * received CTAP_HID_COMMAND_CANCEL while buffering packet.
         * Cancel request.
         */
        if (pkt->init.cmd == CTAP_HID_COMMAND_CANCEL && !_state.is_locked &&
            pkt->cid == _state.cid) {

            _state.err = CTAP2_ERR_KEEPALIVE_CANCEL;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        _state.bcnt = _get_packet_len(pkt);

        /* check for init transaction size described in CTAP specification
           (version 20190130) section 8.1.9.1.3 */
        if (pkt->init.cmd == CTAP_HID_COMMAND_INIT && _state.bcnt != 8) {
            _state.err = CTAP_HID_ERR_INVALID_LEN;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        /* don't allow transactions bigger than max buffer size */
        if (_state.bcnt > CTAP_HID_BUFFER_SIZE) {
            _state.err = CTAP_HID_ERR_INVALID_LEN;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        uint16_t size = (_state.bcnt < CTAP_HID_INIT_PAYLOAD_SIZE) ?
                        _state.bcnt : CTAP_HID_INIT_PAYLOAD_SIZE;
        _state.cmd = pkt->init.cmd;
        _state.cid = pkt->cid;
        _state.seq = -1;
        memcpy(_state.buffer, pkt->init.payload, size);
        _state.offset = size;
    }
    else {
        int left = _state.bcnt - _state.offset;
        int diff = left - CTAP_HID_CONT_PAYLOAD_SIZE;
        _state.seq++;

        /* seqs have to increase sequentially */
        if (pkt->cont.seq != _state.seq) {
            _state.err = CTAP_HID_ERR_INVALID_SEQ;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        /* check for potential buffer overflow */
        if (_state.offset + CTAP_HID_CONT_PAYLOAD_SIZE > CTAP_HID_BUFFER_SIZE) {
            _state.err = CTAP_HID_ERR_INVALID_LEN;
            return CTAP_HID_BUFFER_STATUS_ERROR;
        }

        if (diff <= 0) {
            memcpy(_state.buffer + _state.offset, pkt->cont.payload, left);
            _state.offset += left;
        }
        else {
            memcpy(_state.buffer + _state.offset, pkt->cont.payload,
                   CTAP_HID_CONT_PAYLOAD_SIZE);
            _state.offset += CTAP_HID_CONT_PAYLOAD_SIZE;
        }
    }

    return _state.offset == _state.bcnt ?
           CTAP_HID_BUFFER_STATUS_DONE : CTAP_HID_BUFFER_STATUS_BUFFERING;
}

static void _process_transaction(event_t *arg)
{
    (void)arg;
    uint8_t *buf = (uint8_t *)&_state.buffer;
    uint32_t cid = _state.cid;
    uint16_t bcnt = _state.bcnt;
    uint8_t cmd = _state.cmd;

    if (cmd == CTAP_HID_COMMAND_INIT) {
        _handle_init_packet(cid, bcnt, buf);
    }
    else {
        /* readding deleted cid */
        if (!_cid_exists(cid) && _add_cid(cid) == -1) {
            _send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
        }
        else {
            switch (cmd) {
            case CTAP_HID_COMMAND_MSG:
                /* not implemented as of now */
                DEBUG("CTAP_HID: MSG COMMAND \n");
                _send_error_response(cid, CTAP_HID_ERR_INVALID_CMD);
                break;
            case CTAP_HID_COMMAND_CBOR:
                DEBUG("CTAP_HID: CBOR COMMAND \n");
                _handle_cbor_packet(cmd, cid, buf, bcnt);
                break;
            case CTAP_HID_COMMAND_WINK:
                DEBUG("CTAP_HID: wink \n");
                _wink(cid, cmd);
                break;
            case CTAP_HID_COMMAND_PING:
                DEBUG("CTAP_HID: PING \n");
                _ctap_hid_write(cmd, cid, buf, bcnt);
                break;
            case CTAP_HID_COMMAND_CANCEL:
                /*
                 * no transaction is currently being processed,
                 * no reason to send cancel
                 */
                break;
            default:
                _send_error_response(cid, CTAP_HID_ERR_INVALID_CMD);
                DEBUG("Ctaphid: unknown command %u \n", cmd);
            }
        }
    }

    /* transaction done, cleanup */
    _clear_ctap_buffer();
}

static uint32_t _handle_init_packet(uint32_t cid, uint16_t bcnt,
                                    const uint8_t *nonce)
{
    uint32_t cid_new = 0;

    /* cid 0 is reserved */
    if (cid == 0) {
        _send_error_response(cid, CTAP_HID_ERR_INVALID_CHANNEL);
        return 0;
    }
    /* check for len described in standard */
    if (bcnt != 8) {
        _send_error_response(cid, CTAP_HID_ERR_INVALID_LEN);
        return 0;
    }
    /* create new channel */
    if (cid == CTAP_HID_BROADCAST_CID) {
        cid_new = _cid++;

        if (_add_cid(cid_new) == -1) {
            _send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
            return 0;
        }
        _send_init_response(cid, cid_new, nonce);
    }
    /* synchronize channel */
    else {
        cid_new = cid;
        if (!_cid_exists(cid)) {
            if (_add_cid(cid) == -1) {
                /* reached cid limit */
                _send_error_response(cid, CTAP_HID_ERR_CHANNEL_BUSY);
                return 0;
            }
        }
        _send_init_response(cid, cid, nonce);
    }

    return cid_new;
}

static void _handle_cbor_packet(uint8_t cmd, uint32_t cid, uint8_t *buf, uint16_t bcnt)
{
    ctap_resp_t resp;
    uint8_t err;
    size_t size;

    if (bcnt == 0) {
        err = CTAP_HID_ERR_INVALID_LEN;
        cmd = CTAP_HID_COMMAND_ERROR;
        _ctap_hid_write(cmd, cid, &err, sizeof(err));
        return;
    }

    memset(&resp, 0, sizeof(ctap_resp_t));

    ctap_req_t req;

    req.method = *buf;
    req.buf = buf + 1;
    req.len = bcnt - 1;

    size = fido2_ctap_handle_request(&req, &resp);

    /* transaction done, clear should_cancel flag */
    _state.should_cancel = false;

    if (resp.status == CTAP2_OK && size > 0) {
        /* status + data */
        _ctap_hid_write(cmd, cid, &resp, size + sizeof(resp.status));
    }
    else {
        /* status only */
        _ctap_hid_write(cmd, cid, &resp.status, sizeof(resp.status));
    }
}

static inline bool _is_init_type_pkt(const ctap_hid_pkt_t *pkt)
{
    return ((pkt->init.cmd & CTAP_HID_INIT_PACKET) == CTAP_HID_INIT_PACKET);
}

static void _clear_ctap_buffer(void)
{
    memset(&_state, 0, sizeof(_state));
}

bool fido2_ctap_transport_hid_should_cancel(void)
{
    return _state.should_cancel;
}

void fido2_ctap_transport_hid_check_timeouts(void)
{
    uint64_t now = xtimer_now_usec64();

    for (uint8_t i = 0; i < CTAP_HID_CIDS_MAX; i++) {
        /* transaction timed out because cont packets didn't arrive in time */
        if (_is_busy && g_cids[i].taken &&
            (now - g_cids[i].last_used) >= CTAP_HID_TRANSACTION_TIMEOUT &&
            _state.cid == g_cids[i].cid && !_state.is_locked) {

            _send_error_response(g_cids[i].cid, CTAP_HID_ERR_MSG_TIMEOUT);
            _delete_cid(g_cids[i].cid);
            _clear_ctap_buffer();

            _is_busy = false;
        }
    }
}

static int8_t _add_cid(uint32_t cid)
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

static int8_t _refresh_cid(uint32_t cid)
{
    for (int i = 0; i < CTAP_HID_CIDS_MAX; i++) {
        if (g_cids[i].cid == cid) {
            g_cids[i].last_used = xtimer_now_usec64();
            return CTAP_HID_OK;
        }
    }
    return CTAP_HID_ERR_OTHER;
}

static int8_t _delete_cid(uint32_t cid)
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

static bool _cid_exists(uint32_t cid)
{
    for (int i = 0; i < CTAP_HID_CIDS_MAX; i++) {
        if (g_cids[i].cid == cid) {
            return true;
        }
    }
    return false;
}

static inline uint16_t _get_packet_len(const ctap_hid_pkt_t *pkt)
{
    return (uint16_t)((pkt->init.bcnth << 8) | pkt->init.bcntl);
}

static void _wink(uint32_t cid, uint8_t cmd)
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

    _ctap_hid_write(cmd, cid, NULL, 0);
}

static void _send_error_response(uint32_t cid, uint8_t err)
{
    DEBUG("ctap_trans_hid err resp: %02x \n", err);
    _ctap_hid_write(CTAP_HID_COMMAND_ERROR, cid, &err, sizeof(err));
}

static void _send_init_response(uint32_t cid_old, uint32_t cid_new,
                                const uint8_t *nonce)
{
    ctap_hid_init_resp_t resp;

    memset(&resp, 0, sizeof(ctap_hid_init_resp_t));

    resp.cid = cid_new;
    resp.protocol_version = CTAP_HID_PROTOCOL_VERSION;
    resp.version_major = 0;
    resp.version_minor = 0;
    resp.build_version = 0;
    memcpy(resp.nonce, nonce, sizeof(resp.nonce));

    uint8_t cmd = (CTAP_HID_INIT_PACKET | CTAP_HID_COMMAND_INIT);

    resp.capabilities = CTAP_HID_CAPABILITY_CBOR | CTAP_HID_CAPABILITY_WINK
                        | CTAP_HID_CAPABILITY_NMSG;

    _ctap_hid_write(cmd, cid_old, &resp, sizeof(ctap_hid_init_resp_t));
}

void _ctap_hid_write(uint8_t cmd, uint32_t cid, const void *_data, size_t len)
{
    const uint8_t *data = (uint8_t *)_data;
    uint8_t buf[CONFIG_USBUS_HID_INTERRUPT_EP_SIZE] = { 0 };
    uint16_t bytes_written = 0;
    uint8_t seq = 0;
    uint8_t offset = 0;

    memcpy(buf, &cid, sizeof(cid));
    offset += sizeof(cid);
    buf[offset++] = cmd;
    /* high part of payload length first */
    buf[offset++] = (len & 0xff00) >> 8;
    buf[offset++] = (len & 0xff) >> 0;

    if (data == NULL) {
        usb_hid_io_write(buf, CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
        return;
    }

    for (size_t i = 0; i < len; i++) {
        if (offset == 0) {
            memcpy(buf, &cid, sizeof(cid));
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
            usb_hid_io_write(buf, CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
            offset = 0;
        }
    }

    if (offset > 0) {
        memset(buf + offset, 0, CONFIG_USBUS_HID_INTERRUPT_EP_SIZE - offset);
        usb_hid_io_write(buf, CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);
    }
}
