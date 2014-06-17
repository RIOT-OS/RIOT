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
 * @brief   DTLS 1.2 data structs and prototypes
 * @author  Jonatan Zint <j.zint@fu-berlin.com>
 * @author  Nico von Geyso <nico.geyso@fu-berlin.com>
 */

#ifndef DTLS_H_
#define DTLS_H_

#include <stdint.h>
#include <sys/socket.h>
#include "common.h"


#define DTLS_OK 0


/**
 * TLS structs
 */

typedef struct __attribute__((packed)) {
    uint8_t major;
    uint8_t minor;
} tls_protocol_version_t;


#define DTLS_VERSION_MAJOR 254
#define DTLS_VERSION_MINOR 253
#define DTLS_VERSION {254,253}

typedef enum __attribute__((packed)) {
  TLS_CONTENT_TYPE_CHANGE_CIPHER_SPEC = 20,
  TLS_CONTENT_TYPE_ALERT = 21,
  TLS_CONTENT_TYPE_HANDSHAKE = 22,
  TLS_CONTENT_TYPE_APPLICATION_DATA = 23
} tls_content_type_t;

typedef enum __attribute__((packed)) {
  TLS_COMPRESSION_NULL = 0
} tls_compression_method_t;

typedef enum __attribute__((packed)) {
  TLS_CONNECTION_SERVER,
  TLS_CONNECTION_CLIENT
} tls_connection_end_t;

typedef enum __attribute__((packed)) {
  TLS_PRF_SHA_256
} tls_prf_algorithm_t;

typedef enum __attribute__((packed)) {
  TLS_BULK_CIPHER_NULL,
  TLS_BULK_CIPHER_RC4,
  TLS_BULK_CIPHER_3DES,
  TLS_BULK_CIPHER_AES
} tls_bulk_cipher_alghorithm_t;

typedef enum __attribute__((packed)) {
  TLS_CIPHER_STREAM,
  TLS_CIPHER_BLOCK,
  TLS_CIPHER_AEAD
} tls_cipher_type;

typedef enum __attribute__((packed)) {
  TLS_MAC_ALG_NULL, 
  TLS_MAC_ALG_HMAC_MD5,
  TLS_MAC_ALG_SHA1,
  TLS_MAC_ALG_HMAC_SHA256,
  TLS_MAC_ALG_HMAC_SHA384,
  TLS_MAC_ALG_HMAC_SHA512
} tls_mac_algorithm;

typedef struct __attribute__((packed)) tls_security_parameters_st {
  tls_connection_end_t entity;
  tls_prf_algorithm_t  prf_algorithm;
  tls_bulk_cipher_alghorithm_t bulk_cipher_algorithm;
  tls_cipher_type cipher_type;
  uint8_t enc_key_length;
  uint8_t block_length;
  uint8_t fixed_iv_length;
  uint8_t record_iv_length;
  tls_mac_algorithm mac_algorithm;
  uint8_t mac_length;
  uint8_t mac_key_length;
  tls_compression_method_t compression_algorithm;
  uint8_t master_secret[48];
  uint8_t client_random[32];
  uint8_t server_random[32];
} tls_security_parameters_t;

typedef struct __attribute__((packed)) tls_block_ciphered_st {
  uint8_t *content; /* size: TLSCompressed.length */
  uint8_t *mac; /* size: SecurityParameters.mac_length */
  uint8_t *padding; /* size : GenericBlockCipher.padding_length */
  uint8_t padding_length;
} tls_block_ciphered_t;

typedef struct __attribute__((packed)) tls_generic_block_cipher_st {
  uint8_t *iv; /* size: SecurityParameters.record_iv_length */
  tls_block_ciphered_t block_ciphered;
} tls_generic_block_cipher_t;


/**
 * Cipher suites
 */

typedef uint8_t tls_cipher_suite_t[2];

// PSK - RFC4279
#define TLS_PSK_WITH_RC4_128_SHA              {0x00,0x8A}
#define TLS_PSK_WITH_3DES_EDE_CBC_SHA         {0x00,0x8B}
#define TLS_PSK_WITH_AES_128_CBC_SHA          {0x00,0x8C}
#define TLS_PSK_WITH_AES_256_CBC_SHA          {0x00,0x8D}

// PSK based AES-CCM - RFC 6655
#define TLS_PSK_WITH_AES_128_CCM              {0xC0,0xA4}
#define TLS_PSK_WITH_AES_256_CCM              {0xC0,0xA5}
#define TLS_PSK_WITH_AES_128_CCM_8            {0xC0,0xA8}
#define TLS_PSK_WITH_AES_256_CCM_8            {0xC0,0xA9}

/**
 * Handshake Protocol
 */

typedef enum __attribute__((packed)) {
  DTLS_HANDSHAKE_HELLO_REQUEST = 0,
  DTLS_HANDSHAKE_CLIENT_HELLO = 1,
  DTLS_HANDSHAKE_SERVER_HELLO = 2,
  DTLS_HANDSHAKE_HELLO_VERIFY_REQUEST = 3,
  DTLS_HANDSHAKE_CERTIFICATE = 11,
  DTLS_HANDSHAKE_SERVER_KEY_EXCHANGE = 12,
  DTLS_HANDSHAKE_CERTIFICATE_REQUEST = 13,
  DTLS_HANDSHAKE_SERVER_HELLO_DONE = 14,
  DTLS_HANDSHAKE_CERTIFICATE_VERIFY = 15,
  DTLS_HANDSHAKE_CLIENT_KEY_EXCHANGE = 16,
  DTLS_HANDSHAKE_FINISHED = 20,
} dtls_handshake_type_t;

typedef struct __attribute__((packed)) dtls_handshake_st {
  dtls_handshake_type_t msg_type;
  uint24_t length;
  uint16_t message_seq;
  uint24_t fragment_offset;
  uint24_t fragment_length;
  void *body;
} dtls_handshake_t;


/**
 * RIOT internal DTLS structures
 */
typedef struct __attribute__((packed)) dtls_connection_st {
  dtls_handshake_type_t state;
  tls_connection_end_t type;
  int socket;
  sockaddr6_t socket_addr;
  uint16_t epoch;
  uint64_t sequence_number;  // is 48bit but we use 64bit for math operations
} dtls_connection_t;

#define DTLS_CONNECTION_INIT {0,0,-1,{0},0,0}

#endif
/**
 * @}
 */
