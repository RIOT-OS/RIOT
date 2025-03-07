/*
 * Copyright (C) 2024 Technische Universität Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef NDEF_MIME_PAYLOAD_H
#define NDEF_MIME_PAYLOAD_H

/**
 * @ingroup     sys/net/nfc
 * @{
 *
 * @file
 * @brief       Typedefs and function definitions for the NDEF MIME payload
 *
 * @author      Nico Behrens <nifrabe@outlook.de>
 */

#include "net/nfc/ndef/ndef.h"

int ndef_add_mime_record(ndef_t *message, const char *mime_type, uint32_t mime_type_length,
                         const uint8_t *mime_payload, uint32_t mime_payload_length);

size_t ndef_calculate_mime_record_size(uint32_t mime_type_length, uint32_t mime_payload_length);

/** @} */
#endif