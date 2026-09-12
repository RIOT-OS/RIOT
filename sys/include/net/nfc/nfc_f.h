/*
 * SPDX-FileCopyrightText: 2025-2026 Nico Behrens <nico@nico-behrens.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#define NFC_F_NFCID2_LEN  (8u)

typedef struct {
    uint8_t nfcid2[NFC_F_NFCID2_LEN];
    uint8_t pad0[2];
    uint8_t pad1[3];
    uint8_t mrti_check;
    uint8_t mrti_update;
    uint8_t pad2;
    uint8_t rd[2];
} nfc_f_sensf_res_t;

typedef struct {
    nfc_f_sensf_res_t sensf_res;
} nfc_f_listener_config_t;
