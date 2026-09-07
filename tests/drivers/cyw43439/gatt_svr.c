#include "gatt_svr.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "host/ble_gatt.h"
#include "host/ble_hs.h"
#include "os/os_mbuf.h"
#include <stdatomic.h>

/* connection + notify handle */
static uint16_t _conn_handle = BLE_HS_CONN_HANDLE_NONE;
static uint16_t _notify_handle = 0;


/* 128-bit Service UUID and Characteristic UUIDs */
static const ble_uuid128_t _svc_uuid =
    BLE_UUID128_INIT(0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0,
                     0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0);

/* READ */
static const ble_uuid128_t _chr_hello_uuid =
    BLE_UUID128_INIT(0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00,0x11,
                     0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x01);

/* NOTIFY (u32) */
static const ble_uuid128_t _chr_notify_uuid =
    BLE_UUID128_INIT(0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00,0x11,
                     0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x02);

const uint8_t *gatt_svr_get_svc_uuid128(void)
{
    return _svc_uuid.value;
}

uint16_t gatt_svr_get_notify_handle(void)
{
    return _notify_handle;
}

void gatt_svr_set_conn_handle(uint16_t conn_handle)
{
    _conn_handle = conn_handle;
}

void gatt_svr_clear_conn_handle(void)
{
    _conn_handle = BLE_HS_CONN_HANDLE_NONE;
}

static int _hello_access(uint16_t conn_handle, uint16_t attr_handle,
                         struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        const char *msg = "hello from PSoC6 (RIOT)";
        return os_mbuf_append(ctxt->om, msg, strlen(msg));
    }

    return BLE_ATT_ERR_UNLIKELY;
}

static int _notify_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    /* optional: allow reads of last value (we just return a string here) */
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        const char *msg = "enable notify to receive counter";
        return os_mbuf_append(ctxt->om, msg, strlen(msg));
    }

    return BLE_ATT_ERR_UNLIKELY;
}

/* Service + characteristic definitions */
static const struct ble_gatt_svc_def _gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = (ble_uuid_t *)&_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = (ble_uuid_t *)&_chr_hello_uuid.u,
                .access_cb = _hello_access,
                .flags = BLE_GATT_CHR_F_READ,
            },
            {
                .uuid = (ble_uuid_t *)&_chr_notify_uuid.u,
                .access_cb = _notify_access,
                .val_handle = &_notify_handle,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
            },
            { 0 }
        }
    },
    { 0 }
};

int gatt_svr_init(void)
{
    int rc;

    rc = ble_gatts_count_cfg(_gatt_svcs);
    assert(rc == 0);

    rc = ble_gatts_add_svcs(_gatt_svcs);
    assert(rc == 0);

    return 0;
}

int gatt_svr_notify_axis(char axis)
{
    if (_conn_handle == BLE_HS_CONN_HANDLE_NONE || _notify_handle == 0) {
        return BLE_HS_EINVAL;
    }

    struct os_mbuf *om = ble_hs_mbuf_from_flat(&axis, 1);
    if (!om) {
        return BLE_HS_ENOMEM;
    }


    int rc = ble_gatts_notify_custom(_conn_handle, _notify_handle, om);

    if (rc != 0) {
        os_mbuf_free_chain(om);
    }

    return rc;
}

int gatt_svr_notify_u32(uint32_t v)
{
    if (_conn_handle == BLE_HS_CONN_HANDLE_NONE || _notify_handle == 0) {
        return BLE_HS_EINVAL;
    }

    uint8_t payload[4];
    payload[0] = (uint8_t)(v >> 0);
    payload[1] = (uint8_t)(v >> 8);
    payload[2] = (uint8_t)(v >> 16);
    payload[3] = (uint8_t)(v >> 24);

    struct os_mbuf *om = ble_hs_mbuf_from_flat(payload, sizeof(payload));
    if (!om) {
        return BLE_HS_ENOMEM;
    }

    int rc = ble_gatts_notify_custom(_conn_handle, _notify_handle, om);

    /* IMPORTANT: free on failure (otherwise you leak msys buffers) */
    if (rc != 0) {
        os_mbuf_free_chain(om);
    }

    return rc;
}


