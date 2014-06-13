/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup dtls
 * @{
 * @file
 * @brief   add foo here
 * @author  Jonatan Zint <j.zint@fu-berlin.com>
 * @author  Nico von Geyso <nico.geyso@fu-berlin.com>
 */

#ifndef DTLS_PROTOCOLS_RECORD_H_
#define DTLS_PROTOCOLS_RECORD_H_

#include "dtls.h"

#define DTLS_RECORD_MAX_SIZE 16384  /* 2^14 */
#define TLS_PLAINTEXT_MAX_SIZE 16384
#define DTLS_RECORD_HEADER_SIZE 12 /* in bytes */

typedef struct __attribute__((packed)) {
  tls_content_type_t type;
  tls_protocol_version_t version;
  uint16_t epoch;
  uint48_t sequence_number;
  uint16_t length;
  uint8_t *fragment;
} dtls_record_t;


int dtls_record_fragment_init(dtls_record_t *record, tls_content_type_t type,
   uint16_t epoch, uint64_t sequence_number, uint8_t *payload, size_t size);

int dtls_record_compress(dtls_record_t *fragment, tls_compression_method_t meth);

int dtls_record_stream(dtls_connection_t *conn, tls_content_type_t type, 
    uint8_t* input, size_t size);

#endif
/**
 * @}
 */
