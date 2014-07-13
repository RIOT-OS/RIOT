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

#ifndef DTLS_RECORD_H_
#define DTLS_RECORD_H_

#include "dtls.h"

#define DTLS_RECORD_MAX_SIZE 16384  /* 2^14 */
#define TLS_PLAINTEXT_MAX_SIZE 16384
#define DTLS_RECORD_HEADER_SIZE 13 /* in bytes */
#define DTLS_RECORD_LISTEN_BUFFER_SIZE 32
#define DTLS_RECORD_BUFFER_SIZE 128

#define DTLS_RECORD_ERR_INIT -2
#define DTLS_RECORD_ERR_LENGTH -3
#define DTLS_RECORD_ERR_SEQUENCE -4
#define DTLS_RECORD_ERR_SEND -5 
#define DTLS_RECORD_ERR_ENCRYPT -6
#define DTLS_RECORD_ERR_DECRYPT -7
#define DTLS_RECORD_ERR_COMPRESSION -8
#define DTLS_RECORD_ERR_DECOMPRESSION -9


typedef struct __attribute__((packed)) {
  tls_content_type_t type;
  tls_protocol_version_t version;
  uint16_t epoch;
  uint48_t sequence_number;
  uint16_t length;
} dtls_record_header_t;

#define DTLS_RECORD_HEADER_INIT {0,DTLS_VERSION,0,{0},0}

typedef struct __attribute__((packed)) {
  dtls_record_header_t *header;
  uint8_t *fragment;
} dtls_record_t;

typedef int (*dtls_record_cb_t)(dtls_connection_t *conn, dtls_record_t*);

int dtls_send_raw(dtls_connection_t* conn, uint8_t* data, size_t size);

int dtls_record_send(dtls_connection_t *conn, tls_content_type_t type, 
    uint8_t* input, size_t size);

int dtls_record_stream_send(dtls_connection_t *conn, tls_content_type_t type, 
    uint8_t* input, size_t size);

#endif
/**
 * @}
 */
