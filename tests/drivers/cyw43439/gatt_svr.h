#pragma once
#include <stdint.h>

int gatt_svr_init(void);

/* For advertising: returns pointer to 16 bytes (UUID128 in little-endian as NimBLE expects) */
const uint8_t *gatt_svr_get_svc_uuid128(void);

/* For notifications */
uint16_t gatt_svr_get_notify_handle(void);
void gatt_svr_set_conn_handle(uint16_t conn_handle);
void gatt_svr_clear_conn_handle(void);
int gatt_svr_notify_u32(uint32_t v);
int gatt_svr_notify_axis(char axis);
