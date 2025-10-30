/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       STDIO over NimBLE implementation
 *
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "nimble_riot.h"
#include "nimble/nimble_port.h"
#include "net/bluetil/ad.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#if IS_USED(MODULE_STDIO_NIMBLE_DEBUG)
#include <stdarg.h>
#include "stdio_uart.h"
#include "periph/uart.h"
#endif /* IS_USED(MODULE_STDIO_NIMBLE_DEBUG) */

#include "stdio_base.h"
#include "stdio_nimble.h"

#define NIMBLE_MAX_PAYLOAD  MYNEWT_VAL(BLE_LL_MAX_PKT_SIZE)

/* Nimble uses ZTIMER_MSEC => 1 tick equals 1 ms */
#define CALLOUT_TICKS_MS    1

enum {
    STDIO_NIMBLE_DISCONNECTED,
    STDIO_NIMBLE_CONNECTED,
    STDIO_NIMBLE_SUBSCRIBED,
    STDIO_NIMBLE_SENDING,
};

/* tsrb for stdout */
static uint8_t _tsrb_stdout_mem[CONFIG_STDIO_NIMBLE_STDOUT_BUFSIZE];
static tsrb_t _tsrb_stdout = TSRB_INIT(_tsrb_stdout_mem);

/* intermediate buffer to transfer data between tsrb and nimble functions,
 * which are all based on os_mbuf implementation */
static uint8_t _stdin_read_buf[NIMBLE_MAX_PAYLOAD];
static uint8_t _stdout_write_buf[NIMBLE_MAX_PAYLOAD];

/* information about bluetooth connection */
static uint16_t _conn_handle;
static uint16_t _val_handle_stdout;
static volatile uint8_t _status = STDIO_NIMBLE_DISCONNECTED;

/* nimble related structs */
static struct ble_npl_callout _send_stdout_callout;
static struct ble_gap_event_listener _gap_event_listener;

#if IS_USED(MODULE_STDIO_NIMBLE_DEBUG)
#define DEBUG_PRINTF_BUFSIZE  512
#define PREFIX_STDIN    "\nSTDIN: "
#define PREFIX_STDOUT   "STDOUT: "

static char _debug_printf_buf[DEBUG_PRINTF_BUFSIZE];
#endif /* IS_USED(MODULE_STDIO_NIMBLE_DEBUG) */

#if IS_USED(MODULE_STDIO_NIMBLE_DEBUG)
#define _debug_printf(...) \
    do { \
        unsigned state = irq_disable(); \
        int rc = snprintf(_debug_printf_buf, DEBUG_PRINTF_BUFSIZE, __VA_ARGS__); \
        uart_write(STDIO_UART_DEV, (const uint8_t *)_debug_printf_buf, rc); \
        irq_restore(state); \
    } while(0)
#else
#define _debug_printf(...) (void)0
#endif

/**
 * @brief UUID for stdio service (value: e6d54866-0292-4779-b8f8-c52bbec91e71)
 */
static const ble_uuid128_t gatt_svr_svc_stdio_uuid
    = BLE_UUID128_INIT(0x71, 0x1e, 0xc9, 0xbe, 0x2b, 0xc5, 0xf8, 0xb8,
                       0x79, 0x47, 0x92, 0x02, 0x66, 0x48, 0xd5, 0xe6);

/**
 * @brief UUID for stdout characteristic (value: 35f28386-3070-4f3b-ba38-27507e991762)
 */
static const ble_uuid128_t gatt_svr_chr_stdout_uuid
    = BLE_UUID128_INIT(0x62, 0x17, 0x99, 0x7e, 0x50, 0x27, 0x38, 0xba,
                       0x3b, 0x4f, 0x70, 0x30, 0x86, 0x83, 0xf2, 0x35);

/**
 * @brief UUID for stdin characteristic (value: ccdd113f-40d5-4d68-86ac-a728dd82f4aa)
 */
static const ble_uuid128_t gatt_svr_chr_stdin_uuid
    = BLE_UUID128_INIT(0xaa, 0xf4, 0x82, 0xdd, 0x28, 0xa7, 0xac, 0x86,
                       0x68, 0x4d, 0xd5, 0x40, 0x3f, 0x11, 0xdd, 0xcc);

/**
 * @brief Nimble access callback for stdin characteristic
 */
static int gatt_svr_chr_access_stdin(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg);

/**
 * @brief Dummy access callback, because nimble requires one
 */
static int gatt_svr_chr_access_noop(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg)
{

    (void)conn_handle;
    (void)attr_handle;
    (void)ctxt;
    (void)arg;

    return 0;
}

/**
 * @brief Struct to define the stdio bluetooth service with its characteristics
 */
static const struct ble_gatt_svc_def _gatt_svr_svcs[] =
{
    /*
     * access_cb defines a callback for read and write access events on
     * given characteristics
     */
    {
        /* Service: stdio */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t *)&gatt_svr_svc_stdio_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: stdout */
            .uuid = (ble_uuid_t *)&gatt_svr_chr_stdout_uuid.u,
            .access_cb = gatt_svr_chr_access_noop,
            .val_handle = &_val_handle_stdout,
            .flags = BLE_GATT_CHR_F_INDICATE,
        }, {
            /* Characteristic: stdin */
            .uuid = (ble_uuid_t *)&gatt_svr_chr_stdin_uuid.u,
            .access_cb = gatt_svr_chr_access_stdin,
            .flags = BLE_GATT_CHR_F_WRITE,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },
    {
        0, /* No more services */
    },
};

static void _purge_buffer(void)
{
    tsrb_clear(&stdin_isrpipe.tsrb);

#if IS_USED(MODULE_SHELL)
    /* send Ctrl-C to the shell to reset the input */
    isrpipe_write_one(&stdin_isrpipe, '\x03');
#endif

    tsrb_clear(&_tsrb_stdout);
}

static void _send_stdout(struct ble_npl_event *ev)
{
    (void)ev;

    /* rearm callout */
    ble_npl_callout_reset(&_send_stdout_callout, CALLOUT_TICKS_MS);

    if (_status == STDIO_NIMBLE_SUBSCRIBED) {
        _status = STDIO_NIMBLE_SENDING;
        int to_send = tsrb_peek(&_tsrb_stdout, _stdout_write_buf, NIMBLE_MAX_PAYLOAD);

        if (to_send > 0) {
            struct os_mbuf *om = ble_hs_mbuf_from_flat(_stdout_write_buf, to_send);
            if (om != NULL) {
                int rc = ble_gatts_indicate_custom(_conn_handle, _val_handle_stdout, om);
                if (rc == 0) {
                    /* bytes were successfully sent, so drop them from the buffer */
                    tsrb_drop(&_tsrb_stdout, to_send);
                    _debug_printf("%d bytes sent successfully\n", to_send);
                }
                else {
                    _status = STDIO_NIMBLE_SUBSCRIBED;
                }
            }
            else {
                _status = STDIO_NIMBLE_SUBSCRIBED;
            }
        }
        else {
            _status = STDIO_NIMBLE_SUBSCRIBED;
        }
    }
}

static int _gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {

    case BLE_GAP_EVENT_CONNECT:
        _debug_printf("BLE_GAP_EVENT_CONNECT handle: %d\n", event->connect.conn_handle);
        if (event->connect.status == 0 && _conn_handle == 0) {
            _status = STDIO_NIMBLE_CONNECTED;
            if (CONFIG_STDIO_NIMBLE_CLEAR_BUFFER_ON_CONNECT) {
                _purge_buffer();
            }
        }
        else if (event->connect.conn_handle == _conn_handle) {
            _conn_handle = 0;
            _status = STDIO_NIMBLE_DISCONNECTED;
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        _debug_printf("BLE_GAP_EVENT_DISCONNECT %d\n", event->disconnect.conn.conn_handle);
        if (event->disconnect.conn.conn_handle == _conn_handle) {
            _status = STDIO_NIMBLE_DISCONNECTED;
            _conn_handle = 0;
        }
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        _debug_printf("BLE_GAP_EVENT_SUBSCRIBE %d\n", event->subscribe.conn_handle);
        if (event->subscribe.attr_handle == _val_handle_stdout) {
            if (event->subscribe.cur_indicate == 1) {
                _status = STDIO_NIMBLE_SUBSCRIBED;
                _conn_handle = event->subscribe.conn_handle;
            }
            else {
                _status = STDIO_NIMBLE_CONNECTED;
                _conn_handle = 0;
            }
        }
        break;

    case BLE_GAP_EVENT_NOTIFY_TX:
        _debug_printf("BLE_GAP_EVENT_NOTIFY_TX %d\n", event->notify_tx.conn_handle);
        if (event->notify_tx.indication == 1 && (event->notify_tx.conn_handle == _conn_handle)) {
            if (event->notify_tx.status == BLE_HS_EDONE) {
                _status = STDIO_NIMBLE_SUBSCRIBED;
            }
            else if (event->notify_tx.status != 0) {
                _status = STDIO_NIMBLE_SUBSCRIBED;
            }
        }
        break;

    case BLE_GAP_EVENT_MTU:
        _debug_printf("BLE_GAP_EVENT_MTU: mtu = %d\n", event->mtu.value);
        break;
    }

    return 0;
}

static int gatt_svr_chr_access_stdin(
    uint16_t conn_handle, uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);

    /* read sent data */
    int rc = ble_hs_mbuf_to_flat(ctxt->om, _stdin_read_buf, sizeof(_stdin_read_buf), &om_len);

    isrpipe_write(&stdin_isrpipe, _stdin_read_buf, om_len);

    return rc;
}

static void _init(void)
{
#if IS_USED(MODULE_STDIO_NIMBLE_DEBUG)
    uart_init(STDIO_UART_DEV, STDIO_UART_BAUDRATE, NULL, NULL);
#endif

    ble_npl_callout_init(&_send_stdout_callout, nimble_port_get_dflt_eventq(),
                         _send_stdout, NULL);
}

static ssize_t _write(const void *buffer, size_t len)
{
    unsigned state = irq_disable();

#if IS_USED(MODULE_STDIO_NIMBLE_DEBUG)
    uart_write(STDIO_UART_DEV, (const uint8_t *)PREFIX_STDOUT, strlen(PREFIX_STDOUT));
    uart_write(STDIO_UART_DEV, (const uint8_t *)buffer, len);
    uart_write(STDIO_UART_DEV, (const uint8_t *)"\n", 1);
#endif

    irq_restore(state);

    unsigned int consumed = tsrb_add(&_tsrb_stdout, buffer, len);

    if (_status == STDIO_NIMBLE_SUBSCRIBED || _status == STDIO_NIMBLE_SENDING) {
        if (!ble_npl_callout_is_active(&_send_stdout_callout)) {
            /* bootstrap callout */
            ble_npl_callout_reset(&_send_stdout_callout, CALLOUT_TICKS_MS);
        }
    }

    return consumed;
}

/* is going to be called by auto_init */
void stdio_nimble_init(void)
{
    int rc = 0;

    /* verify and add our custom services */
    rc = ble_gatts_count_cfg(_gatt_svr_svcs);
    assert(rc == 0);
    rc = ble_gatts_add_svcs(_gatt_svr_svcs);
    assert(rc == 0);

    /* reload the GATT server to link our added services */
    ble_gatts_start();

    /* register gap event listener */
    rc = ble_gap_event_listener_register(&_gap_event_listener, _gap_event_cb, NULL);
    assert(rc == 0);

    /* fix compilation error when using DEVELHELP=0 */
    (void)rc;
}

STDIO_PROVIDER(STDIO_NIMBLE, _init, NULL, _write)
