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

/**
 * @brief NDEF text encoding
 */
typedef enum {
    UTF8 = 0,
    UTF16 = 1
} ndef_text_encoding_t;

/**
 * @brief Adds an NDEF text record to the NDEF message
 * 
 * @note The text record is added to the end of the NDEF message.
 * 
 * @param[in,out]   message             NDEF message
 * @param[in]       text                Text to add
 * @param[in]       text_length         Length of the text
 * @param[in]       lang_code           Language code
 * @param[in]       lang_code_length    Length of the language code
 * @param[in]       encoding            Encoding of the text
 * @return int 
 */
int ndef_add_text_record(ndef_t *message, const char *text, uint32_t text_length,
                         const char *lang_code, uint8_t lang_code_length,
                         ndef_text_encoding_t encoding);

/**
 * @brief Calculates the size of an NDEF text record
 * 
 * @param[in] text_length       Length of the text
 * @param[in] lang_code_length  Length of the language code
 * @return size_t Size of the text record
 */
size_t ndef_calculate_text_record_size(uint32_t text_length, uint8_t lang_code_length);

/** @} */
#endif
