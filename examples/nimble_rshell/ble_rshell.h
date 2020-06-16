/*
 * Copyright (C) 2020 Thomas Stilwell <stilwellt@openlabs.co>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BLE_RSHELL_H_INCLUDED
#define BLE_RSHELL_H_INCLUDED

#include "rshell.h"

#include "host/ble_gatt.h"

/**
 * @brief BLE shell struct
 */
typedef struct {
    rshell_t rshell;
    const char *device_name;
    const shell_command_t *shell_commands;
    uint16_t conn_handle;
    uint16_t read_handle;
    uint16_t write_handle;
    struct ble_gatt_svc_def gatt_svr_svcs[2];
    struct ble_gatt_chr_def characteristics[3];

} ble_rshell_t;

int ble_rshell_init(ble_rshell_t *ctx);

#endif /* BLE_RSHELL_H_INCLUDED */
