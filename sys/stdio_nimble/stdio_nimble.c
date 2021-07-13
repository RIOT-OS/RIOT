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
 *
 * @}
 */

#include <errno.h>
#include <stdlib.h>

#include "nimble_riot.h"
#include "net/bluetil/ad.h"

#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "ringbuffer.h"
#include "stdio_nimble.h"

#define MTU_MAX_NIMBLE BLE_ATT_MTU_MAX
#define ATT_PKT_HEADER_SIZE 13
#define MAX_MTU_PAYLOAD (MTU_MAX_NIMBLE - ATT_PKT_HEADER_SIZE)

/* ringbuffer for stdin */
static ringbuffer_t _rb_stdin;
static char _rb_stdin_mem[STDIN_BUFSIZE];

/* ringbuffer for stdout */
static ringbuffer_t _rb_stdout;
static char _rb_stdout_mem[STDOUT_BUFSIZE];

static mutex_t _rb_stdout_mutex = MUTEX_INIT;
static int _current_max_mtu_payload = MAX_MTU_PAYLOAD;

/* intermediate buffer to transfer data between ringbuffer and nimble functions,
 * which are all based on os_mbuf implementation */
static char _stdin_read_buf[MAX_MTU_PAYLOAD];
static char _stdout_write_buf[MAX_MTU_PAYLOAD];

/* information about bluetooth connection */
static uint16_t _conn_handle;
static uint16_t _val_handle_stdout;
static bool _send_stdio = false;
static bool _sending_in_progress = false;
static mutex_t _send_mutex = MUTEX_INIT;

static struct ble_gap_event_listener _gap_event_listener;

/**
 * @brief UUID for stdio service (value: e6d54866-0292-4779-b8f8-c52bbec91e71)
 */
static const ble_uuid128_t gatt_svr_svc_stdio_uuid
        = BLE_UUID128_INIT(0x71, 0x1e, 0xc9, 0xbe, 0x2b, 0xc5, 0xf8, 0xb8, 0x79,
                           0x47, 0x92, 0x02, 0x66, 0x48, 0xd5, 0xe6);

/**
 * @brief UUID for stdout characteristic (value: 35f28386-3070-4f3b-ba38-27507e991762)
 */
static const ble_uuid128_t gatt_svr_chr_stdout_uuid
        = BLE_UUID128_INIT(0x62, 0x17, 0x99, 0x7e, 0x50, 0x27, 0x38, 0xba, 0x3b,
                           0x4f, 0x70, 0x30, 0x86, 0x83, 0xf2, 0x35);

/**
 * @brief UUID for stdin characteristic (value: ccdd113f-40d5-4d68-86ac-a728dd82f4aa)
 */
static const ble_uuid128_t gatt_svr_chr_stdin_uuid
        = BLE_UUID128_INIT(0xaa, 0xf4, 0x82, 0xdd, 0x28, 0xa7, 0xac, 0x86, 0x68,
                           0x4d, 0xd5, 0x40, 0x3f, 0x11, 0xdd, 0xcc);

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
        struct ble_gatt_access_ctxt *ctxt, void *arg) {

    (void) conn_handle;
    (void) attr_handle;
    (void) ctxt;
    (void) arg;

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
        .uuid = (ble_uuid_t*) &gatt_svr_svc_stdio_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: stdout */
            .uuid = (ble_uuid_t*) &gatt_svr_chr_stdout_uuid.u,
            .access_cb = gatt_svr_chr_access_noop,
            .val_handle = &_val_handle_stdout,
            .flags = BLE_GATT_CHR_F_INDICATE,
        }, {
            /* Characteristic: stdin */
            .uuid = (ble_uuid_t*) &gatt_svr_chr_stdin_uuid.u,
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

static int _gap_event_cb(struct ble_gap_event *event, void *arg);

static void _send_from_ringbuffer(bool mutex_locked);

void stdio_init(void)
{
    ringbuffer_init(&_rb_stdin, _rb_stdin_mem, STDIN_BUFSIZE);
    ringbuffer_init(&_rb_stdout, _rb_stdout_mem, STDOUT_BUFSIZE);
}

ssize_t stdio_read(void *buffer, size_t count)
{
    ssize_t res;

    /* block until at least on character was read from the buffer */
    while (!(res = ringbuffer_get(&_rb_stdin, buffer, count))) {};

    return res;
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    mutex_lock(&_rb_stdout_mutex);

    unsigned int free = ringbuffer_get_free(&_rb_stdout);
    unsigned int consumed = 0;

    if (free > 0) {
        unsigned int to_send = (free <= len) ? free : len;
        consumed = ringbuffer_add(&_rb_stdout, buffer, to_send);
    }

    mutex_unlock(&_rb_stdout_mutex);

    /* initially trigger chained async calls (chained via "_gap_event_cb") */
    if (mutex_trylock(&_send_mutex)) {
        if (!_sending_in_progress) {
            _send_from_ringbuffer(true);
        }
        else {
            /* there is an active call chain, we don't must not initiate a new one */
            mutex_unlock(&_send_mutex);
        }
    }

    return consumed;
}

/* is gonna be called by auto_init */
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
    (void) rc;
}

/* there should be always only one active call chain */
/* prevent multiple chains by "_send_mutex" and "_sending_in_progress" */
static void _send_from_ringbuffer(bool mutex_locked)
{
    if (!mutex_locked) {
        mutex_lock(&_send_mutex);
    }

    mutex_lock(&_rb_stdout_mutex);

    if (ringbuffer_empty(&_rb_stdout)) {
        _sending_in_progress = false;

        mutex_unlock(&_rb_stdout_mutex);
        mutex_unlock(&_send_mutex);

        return;
    }

    int len_read = ringbuffer_get(&_rb_stdout, _stdout_write_buf, _current_max_mtu_payload);

    mutex_unlock(&_rb_stdout_mutex);

    int rc = 1;
    if (_send_stdio) {
        if (_conn_handle != 0 && _val_handle_stdout != 0) {

            struct os_mbuf *om = ble_hs_mbuf_from_flat(_stdout_write_buf, len_read);

            if (om != NULL) {
                _sending_in_progress = true;
                rc = ble_gattc_indicate_custom(_conn_handle, _val_handle_stdout, om);
            }
        }
    }

    if (rc != 0) {
        _sending_in_progress = false;
    }

    mutex_unlock(&_send_mutex);
}

static void _clear_ringbuffer(void)
{
    ringbuffer_init(&_rb_stdin, _rb_stdin_mem, STDIN_BUFSIZE);

    mutex_lock(&_rb_stdout_mutex);
    ringbuffer_init(&_rb_stdout, _rb_stdout_mem, STDOUT_BUFSIZE);
    mutex_unlock(&_rb_stdout_mutex);
}

static int _gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void) arg;

    switch (event->type) {

        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                _clear_ringbuffer();
                _conn_handle = event->connect.conn_handle;
                ble_att_set_preferred_mtu(MTU_MAX_NIMBLE);
                ble_gattc_exchange_mtu(_conn_handle, NULL, NULL);
            }
            else {
                _send_stdio = false;
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            _send_stdio = false;
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
            if (event->subscribe.attr_handle == _val_handle_stdout) {
                if (event->subscribe.cur_indicate == 1) {
                    _send_stdio = true;
                }
                else {
                    _send_stdio = false;
                }
            }
            break;

        case BLE_GAP_EVENT_NOTIFY_TX:
            if (event->notify_tx.indication == 1) {
                if (event->notify_tx.status == BLE_HS_EDONE) {
                    _send_from_ringbuffer(false);
                }
                else if (event->notify_tx.status != 0) {
                    _send_stdio = false;
                }
            }
            break;

        case BLE_GAP_EVENT_MTU:
            _current_max_mtu_payload = event->mtu.value - ATT_PKT_HEADER_SIZE;
            break;
    }

    return 0;
}

static int gatt_svr_chr_access_stdin(
        uint16_t conn_handle, uint16_t attr_handle,
        struct ble_gatt_access_ctxt *ctxt, void *arg)
{

    (void) conn_handle;
    (void) attr_handle;
    (void) arg;

    uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);

    /* read sent data */
    int rc = ble_hs_mbuf_to_flat(ctxt->om, _stdin_read_buf,
            sizeof(_stdin_read_buf), &om_len);

    ringbuffer_add(&_rb_stdin, _stdin_read_buf, om_len);

    return rc;
}
