/*
 * SPDX-FileCopyrightText: 2025-2026 Nico Behrens <nico@nico-behrens.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

typedef enum {
    NFC_APPLICATION_TYPE_UNKNOWN = 0,
    NFC_APPLICATION_TYPE_T1T,
    NFC_APPLICATION_TYPE_T2T,
    NFC_APPLICATION_TYPE_T3T,
    NFC_APPLICATION_TYPE_T4T,
    NFC_APPLICATION_TYPE_T5T,
    NFC_APPLICATION_NFC_DEP,
    NFC_APPLICATION_MIFARE_ULTRALIGHT,  /* fully T2T compliant */
    NFC_APPLICATION_MIFARE_CLASSIC,     /* partially T2T compliant */
    NFC_APPLICATION_MIFARE_DESFIRE,     /* based on T4T */
    NFC_APPLICATION_MIFARE_PLUS,        /* based on T4T */
} nfc_application_type_t;

typedef enum {
    NFC_TECHNOLOGY_A = 0,
    NFC_TECHNOLOGY_B,
    NFC_TECHNOLOGY_F,
    NFC_TECHNOLOGY_V,
} nfc_technology_t;

typedef enum {
    NFC_BAUDRATE_106K = 0,
    NFC_BAUDRATE_212K,
    NFC_BAUDRATE_424K,
} nfc_baudrate_t;

#include "nfc_a.h"
#include "nfc_b.h"
#include "nfc_f.h"

#include "nfc_error.h"
