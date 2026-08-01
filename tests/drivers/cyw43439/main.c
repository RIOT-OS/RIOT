#if defined(CPU_CORE_CORTEX_M0PLUS)
int main(void) { return 0; }
#else

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "thread.h"

#include "host/ble_gap.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "gatt_svr.h"

static volatile bool _connected = false;
static volatile bool _notify_enabled = false;
static uint8_t _own_addr_type;

static int _gap_event(struct ble_gap_event *event, void *arg);

/* Start undirected connectable advertising. */
static void _start_adv(void)
{
    struct ble_gap_adv_params advp;
    struct ble_hs_adv_fields adv_fields;
    struct ble_hs_adv_fields rsp_fields;
    int rc;

    if (ble_gap_adv_active()) {
        rc = ble_gap_adv_stop();
        if ((rc != 0) && (rc != BLE_HS_EALREADY)) {
            return;
        }
    }

    memset(&adv_fields, 0, sizeof(adv_fields));
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    rc = ble_gap_adv_set_fields(&adv_fields);
    if (rc != 0) {
        return;
    }

    memset(&rsp_fields, 0, sizeof(rsp_fields));
    static const char name[] = "PSOC6_IMU";
    rsp_fields.name = (uint8_t *)name;
    rsp_fields.name_len = sizeof(name) - 1U;
    rsp_fields.name_is_complete = 1;
    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if (rc != 0) {
        return;
    }

    memset(&advp, 0, sizeof(advp));
    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(_own_addr_type, NULL, BLE_HS_FOREVER,
                           &advp, _gap_event, NULL);
    if (rc == 0) {
        puts("[BLE] advertisement started");
    }
}

/* GAP callback keeps connection and CCCD state in sync. */
static int _gap_event(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            _connected = true;
            _notify_enabled = false;
            gatt_svr_set_conn_handle(event->connect.conn_handle);
        }
        else {
            _connected = false;
            _notify_enabled = false;
            gatt_svr_clear_conn_handle();
            _start_adv();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        _connected = false;
        _notify_enabled = false;
        gatt_svr_clear_conn_handle();
        _start_adv();
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        if (event->subscribe.attr_handle == gatt_svr_get_notify_handle()) {
            _notify_enabled = (event->subscribe.cur_notify != 0);
            puts(_notify_enabled ? "[BLE] notify enabled"
                                 : "[BLE] notify disabled");
        }
        return 0;

    default:
        return 0;
    }
}

int main(void)
{
    /* nimble_riot_init already synced before main runs. */
    puts("[BLE] synced");

    ble_svc_gap_init();
    ble_svc_gatt_init();

    int rc = gatt_svr_init();
    assert(rc == 0);

    rc = ble_gatts_start();
    assert(rc == 0);

    rc = ble_hs_id_infer_auto(0, &_own_addr_type);
    assert(rc == 0);

    _start_adv();

    while (1) {
        thread_sleep();
    }
}

#endif
