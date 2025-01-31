/*
 * Copyright (C) 2024 Technische Universität Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef NDEF_TEXT_PAYLOAD_H
#define NDEF_TEXT_PAYLOAD_H

/**
 * @ingroup     sys/net/nfc
 * @{
 *
 * @file
 * @brief       Typedefs and function definitions for the NDEF text payload
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 */

#include <stdint.h>
#include "net/nfc/ndef/ndef.h"

typedef enum {
    UTF8 = 0,
    UTF16 = 1
} ndef_text_encoding_t;

int ndef_add_text_record(ndef_t *message, const char *text, uint32_t text_length,
                         const char *lang_code, uint8_t lang_code_length,
                         ndef_text_encoding_t encoding);

size_t ndef_calculate_text_record_size(uint32_t text_length, uint8_t lang_code_length);

/** @} */
#endif
