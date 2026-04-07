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
#include "periph/i2c.h"
#include "bmi270_r.h"
#include "bmi2.h"
#include "bmi2_defs.h"

static volatile bool _connected = false;
static volatile bool _notify_enabled = false;

static bmi270_t _bmi;
static uint8_t _own_addr_type;

static char _imu_stack[THREAD_STACKSIZE_DEFAULT];

static int _gap_event(struct ble_gap_event *event, void *arg);

/* Small cooperative wait helper for this port. */
static void _busy_wait_us(uint32_t usec)
{
    for (volatile uint32_t i = 0; i < usec * 4U; i++) {
        __asm__ volatile ("nop");
    }
}

/* Keep yielding so NimBLE RX can run while we delay. */
static void _delay_ms_cooperative(uint32_t ms)
{
    while (ms--) {
        _busy_wait_us(1000U);
        thread_yield();
    }
}

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

/* IMU task: sample at 20 Hz and notify current dominant axis. */
static void *_imu_thread(void *arg)
{
    (void)arg;

    int rc;
    i2c_init(0);

    i2c_acquire(0);
    rc = bmi270_init_i2c(&_bmi, 0, BMI2_I2C_PRIM_ADDR);
    i2c_release(0);
    if (rc != 0) {
        return NULL;
    }

    i2c_acquire(0);
    rc = bmi270_config_default(&_bmi);
    i2c_release(0);
    if (rc != 0) {
        return NULL;
    }

    struct bmi2_sens_config cfg = { 0 };
    cfg.type = BMI2_ACCEL;
    cfg.cfg.acc.odr = BMI2_ACC_ODR_50HZ;
    cfg.cfg.acc.range = BMI2_ACC_RANGE_8G;

    i2c_acquire(0);
    rc = bmi2_set_sensor_config(&cfg, 1, &(_bmi.sensor));
    i2c_release(0);
    if (rc != 0) {
        return NULL;
    }

    puts("[IMU] ready");

    while (1) {
        bmi270_data_t data;

        i2c_acquire(0);
        rc = bmi270_read(&_bmi, &data);
        i2c_release(0);

        if ((rc == 0) && _connected && _notify_enabled) {
            int16_t x = data.sensor_data.acc.x;
            int16_t y = data.sensor_data.acc.y;
            int16_t z = data.sensor_data.acc.z;

            int16_t ax = (x < 0) ? -x : x;
            int16_t ay = (y < 0) ? -y : y;
            int16_t az = (z < 0) ? -z : z;

            char axis = 'x';
            if (ay > ax && ay > az) {
                axis = 'y';
            }
            else if (az > ax && az > ay) {
                axis = 'z';
            }

            (void)gatt_svr_notify_axis(axis);
        }

        _delay_ms_cooperative(50);
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

    kernel_pid_t imu_pid = thread_create(_imu_stack, sizeof(_imu_stack),
                                         THREAD_PRIORITY_MAIN + 2,
                                         THREAD_CREATE_STACKTEST,
                                         _imu_thread, NULL, "imu");
    assert(imu_pid > KERNEL_PID_UNDEF);

    while (1) {
        thread_sleep();
    }
}

#endif
