/*
 * SPDX-FileCopyrightText: 2025-2026 Nico Behrens <nico@nico-behrens.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#include <stdint.h>

#define NFC_B_NFCID0_LEN  (4u)
#define NFC_B_APP_DATA_LEN (4u)
#define NFC_B_PROT_INFO_LEN (4u)

typedef struct {
    uint8_t afi;
    uint8_t crc[2];
    uint8_t number_of_applications;
} nfc_b_sens_res_application_data_t;

typedef struct {
    uint8_t nfcid0[NFC_B_NFCID0_LEN];
    nfc_b_sens_res_application_data_t application_data;
    uint8_t protocol_info[NFC_B_PROT_INFO_LEN];
} nfc_b_sensb_res_t;

typedef struct {
    nfc_b_sensb_res_t sensb_res;
} nfc_b_listener_config_t;
