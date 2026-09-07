#include "psoc6_ble_nimble.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "periph/gpio.h"
#include "periph/uart.h"
#include "mutex.h"
#include "ringbuffer.h"
#include "thread.h"

// NimBLE includes
#include "nimble/transport/hci_h4.h"
#include "os/os_mbuf.h"
#include "thread_flags.h"

#define RX_FLAG   (1U << 0)

// H4 packet types
#define H4_CMD 0x01
#define H4_ACL 0x02
#define H4_EVT 0x04

// Transport entry points provided by NimBLE
extern int  ble_transport_to_hs_evt_impl(void *buf);
extern int  ble_transport_to_hs_acl_impl(struct os_mbuf *om);
extern void ble_transport_free(void *buf);

static thread_t *_rx_thread_ptr;

// Serialize UART writes towards the controller
static mutex_t _tx_lock = MUTEX_INIT;

// H4 state machine that hands complete frames to NimBLE
static struct hci_h4_sm _h4sm;

static int _h4_frame_cb(uint8_t pkt_type, void *data)
{
    switch (pkt_type) {
    case HCI_H4_EVT:
        return ble_transport_to_hs_evt_impl(data);
    case HCI_H4_ACL:
        return ble_transport_to_hs_acl_impl((struct os_mbuf *)data);
    default:
        // The controller should never send CMD packets back to the host.
        assert(0);
        return -1;
    }
}

// RX ring buffer and thread wakeup state
#define RX_RING_SIZE 4096
static char _rx_ring_buf[RX_RING_SIZE];
static ringbuffer_t _rx_ring = RINGBUFFER_INIT(_rx_ring_buf);

static kernel_pid_t _rx_pid = KERNEL_PID_UNDEF;
static char _rx_stack[THREAD_STACKSIZE_DEFAULT];

// Local H4 pre-parser state to reject garbage before passing frames to NimBLE
typedef enum {
    ST_WAIT_TYPE = 0,
    ST_WAIT_HDR,
    ST_WAIT_PAYLOAD,
} _h4_rx_state_t;

static _h4_rx_state_t _st = ST_WAIT_TYPE;
static uint8_t _ptype = 0;
static uint8_t _hdr[4];
static unsigned _hdr_have = 0;
static unsigned _hdr_need = 0;
static unsigned _payload_need = 0;
static unsigned _payload_have = 0;

// Buffer that holds one complete H4 frame before handing it to NimBLE
static uint8_t _frame[1 + 4 + 1024];
static unsigned _frame_len = 0;

// reset local H4 pre-parser so next byte starts a fresh frame
static void _h4_parser_reset(void)
{
    _st = ST_WAIT_TYPE;
    _ptype = 0;
    _hdr_have = 0;
    _hdr_need = 0;
    _payload_need = 0;
    _payload_have = 0;
    _frame_len = 0;
}

// feed one byte from UART ring into local H4 parser
static inline void _h4_feed_byte(uint8_t b)
{
    switch (_st) {
    case ST_WAIT_TYPE:
        if (b != H4_CMD && b != H4_ACL && b != H4_EVT) {
            // ignore garbage until a plausible H4 packet type appears
            return;
        }
        _ptype = b;
        _frame[0] = b;
        _frame_len = 1;

        _hdr_have = 0;
        if (b == H4_EVT)      { _hdr_need = 2; }
        else if (b == H4_ACL) { _hdr_need = 4; }
        else                  { _hdr_need = 3; } // CMD

        _st = ST_WAIT_HDR;
        return;

    case ST_WAIT_HDR:
        _hdr[_hdr_have++] = b;

        if (_frame_len < sizeof(_frame)) {
            _frame[_frame_len++] = b;
        }
        else {
            _h4_parser_reset();
            return;
        }

        if (_hdr_have == _hdr_need) {
            if (_ptype == H4_EVT) {
                _payload_need = _hdr[1];
            }
            else if (_ptype == H4_ACL) {
                _payload_need = (unsigned)_hdr[2] | ((unsigned)_hdr[3] << 8);
            }
            else { // CMD
                _payload_need = _hdr[2];
            }

            // never allow payload to overflow the local frame buffer
            if (_payload_need > 1024) {
                _h4_parser_reset();
                return;
            }

            _payload_have = 0;
            _st = (_payload_need == 0) ? ST_WAIT_TYPE : ST_WAIT_PAYLOAD;

            if (_payload_need == 0) {
                hci_h4_sm_rx(&_h4sm, _frame, _frame_len);
                _h4_parser_reset();
            }
        }
        return;

    case ST_WAIT_PAYLOAD:
        if (_frame_len < sizeof(_frame)) {
            _frame[_frame_len++] = b;
        }
        else {
            _h4_parser_reset();
            return;
        }

        if (++_payload_have == _payload_need) {
            hci_h4_sm_rx(&_h4sm, _frame, _frame_len);
            _h4_parser_reset();
        }
        return;

    default:
        _h4_parser_reset();
        return;
    }
}

static void _uart_rx_cb(void *arg, uint8_t byte)
{
    (void)arg;

    if (ringbuffer_add_one(&_rx_ring, (char)byte) == 0) {
        return;
    }

    if (_rx_thread_ptr) {
        thread_flags_set(_rx_thread_ptr, RX_FLAG);
    }
}

// drain UART ring from thread ctx, pass bytes to H4 parser
static void *_rx_thread(void *arg)
{
    (void)arg;

    while (1) {
        thread_flags_wait_any(RX_FLAG);

        while (1) {
            int v = ringbuffer_get_one(&_rx_ring);
            if (v < 0) {
                break;
            }
            _h4_feed_byte((uint8_t)v);
        }
    }

    return NULL;
}

static bool _inited;
static mutex_t _init_lock = MUTEX_INIT;

// power up external ctrl, start UART/H4 transport once
static void _ensure_inited(void)
{
    if (_inited) {
        return;
    }

    mutex_lock(&_init_lock);
    if (!_inited) {
        // power up external controller
        gpio_init(PIN_BT_REG_ON, GPIO_OUT);
        gpio_set(PIN_BT_REG_ON);

        // init NimBLE H4 state machine
        hci_h4_sm_init(&_h4sm, &hci_h4_allocs_from_ll, _h4_frame_cb);

        // start RX thread before enabling HCI UART
        if (_rx_pid == KERNEL_PID_UNDEF) {
            _rx_pid = thread_create(_rx_stack, sizeof(_rx_stack),
                                    THREAD_PRIORITY_MAIN - 1,
                                    THREAD_CREATE_STACKTEST,
                                    _rx_thread, NULL, "bt_hci_rx");
            _rx_thread_ptr = thread_get(_rx_pid);
        }

        // init UART after RX thread exists
        uart_init(CONFIG_PSOC6_BT_UART, 115200, _uart_rx_cb, NULL);

        _inited = true;
    }
    mutex_unlock(&_init_lock);
}

static inline void _to_ll_write(const uint8_t *buf, size_t len)
{
    mutex_lock(&_tx_lock);
    uart_write(CONFIG_PSOC6_BT_UART, buf, len);
    mutex_unlock(&_tx_lock);
}

int ble_transport_to_ll_cmd_impl(void *buf)
{
    _ensure_inited();

    uint8_t *cmd = (uint8_t *)buf;
    assert(cmd);

    size_t len = 3U + cmd[2]; // HCI header plus payload length
    uint8_t out[1 + 3 + 255];

    out[0] = H4_CMD;
    memcpy(&out[1], cmd, len);

    _to_ll_write(out, 1 + len);
    ble_transport_free(buf);
    return 0;
}

int ble_transport_to_ll_acl_impl(struct os_mbuf *om)
{
    _ensure_inited();
    assert(om);

    size_t plen = OS_MBUF_PKTLEN(om);
    if (plen == 0) {
        os_mbuf_free_chain(om);
        return 0;
    }

    // ACL can be > 255
    if (plen > 1024) {
        os_mbuf_free_chain(om);
        return 0;
    }

    uint8_t out[1 + 1024];
    out[0] = H4_ACL;
    os_mbuf_copydata(om, 0, plen, &out[1]);

    _to_ll_write(out, 1 + plen);
    os_mbuf_free_chain(om);
    return 0;
}

void psoc6_ble_nimble_init(void)
{
    _ensure_inited();
}
