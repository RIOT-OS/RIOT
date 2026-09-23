/*
 * SPDX-FileCopyrightText: 2025-2026 Nico Behrens <nico@nico-behrens.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#define NFC_V_UID_LEN  (8u)

typedef struct {
    uint8_t res_flag;
    uint8_t dsfid;
    uint8_t uid[NFC_V_UID_LEN];
} nfc_v_inventory_res_t;

typedef struct {
    nfc_v_inventory_res_t inventory_res;
} nfc_v_listener_config_t;
