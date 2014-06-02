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

/**
 * basic stdint types
 */
typedef uint8_t uint24_t[3];
typedef uint8_t uint48_t[6];


/**
 * TLS structs
 */

typedef struct tls_protocol_version_st {
    uint8_t major;
    uint8_t minor;
} tls_protocol_version_t;

typedef enum {
  change_cipher_spec = 20,
  alert = 21,
  handshake = 22,
  application_data = 23
} tls_content_type_t;

typedef enum {
  TLS_COMPRESSION_NULL = 0
} tls_compression_method_t;

typedef enum {
  TLS_CONNECTION_SERVER,
  TLS_CONNECTION_CLIENT
} tls_connection_end_t;

typedef enum {
  TLS_PRF_SHA_256
} tls_prf_algorithm_t;

typedef enum {
  TLS_BULK_CIPHER_NULL,
  TLS_BULK_CIPHER_RC4,
  TLS_BULK_CIPHER_3DES,
  TLS_BULK_CIPHER_AES
} tls_bulk_cipher_alghorithm_t;

typedef enum {
//  TLS_CIPHER_STREAM,
  TLS_CIPHER_BLOCK,
//  TLS_CIPHER_AEAD
} tls_cipher_type;

typedef enum {
  TLS_MAC_ALG_NULL, 
  TLS_MAC_ALG_HMAC_MD5,
  TLS_MAC_ALG_SHA1,
  TLS_MAC_ALG_HMAC_SHA256,
  TLS_MAC_ALG_HMAC_SHA384,
  TLS_MAC_ALG_HMAC_SHA512
} tls_mac_algorithm;

typedef struct tls_security_parameters_st {
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

typedef struct tls_block_ciphered_st {
  uint8_t *content; /* size: TLSCompressed.length */
  uint8_t *mac; /* size: SecurityParameters.mac_length */
  uint8_t *padding; /* size : GenericBlockCipher.padding_length */
  uint8_t padding_length;
} tls_block_ciphered_t;

typedef struct tls_generic_block_cipher_st {
  uint8_t *iv; /* size: SecurityParameters.record_iv_length */
  tls_block_ciphered_t block_ciphered;
} tls_generic_block_cipher_t;


/**
 * Cipher suites
 */

typedef uint8_t tls_cipher_suite_t[2];

#define TLS_RSA_EXPORT_WITH_RC4_40_MD5        {0x00,0x03} /* [RFC4346] */
#define TLS_RSA_WITH_RC4_128_MD5              {0x00,0x04} /* [RFC5246] */
#define TLS_RSA_WITH_RC4_128_SHA              {0x00,0x05} /* [RFC5246] */
#define TLS_DH_anon_EXPORT_WITH_RC4_40_MD5    {0x00,0x17} /* [RFC4346] */
#define TLS_DH_anon_WITH_RC4_128_MD5          {0x00,0x18} /* [RFC5246] */
#define TLS_KRB5_WITH_RC4_128_SHA             {0x00,0x20} /* [RFC2712] */
#define TLS_KRB5_WITH_RC4_128_MD5             {0x00,0x24} /* [RFC2712] */
#define TLS_KRB5_EXPORT_WITH_RC4_40_SHA       {0x00,0x28} /* [RFC2712] */
#define TLS_KRB5_EXPORT_WITH_RC4_40_MD5       {0x00,0x2B} /* [RFC2712] */
#define TLS_PSK_WITH_RC4_128_SHA              {0x00,0x8A} /* [RFC4279] */
#define TLS_DHE_PSK_WITH_RC4_128_SHA          {0x00,0x8E} /* [RFC4279] */
#define TLS_RSA_PSK_WITH_RC4_128_SHA          {0x00,0x92} /* [RFC4279] */
#define TLS_ECDH_ECDSA_WITH_RC4_128_SHA       {0xC0,0x02} /* [RFC4492] */
#define TLS_ECDHE_ECDSA_WITH_RC4_128_SHA      {0xC0,0x07} /* [RFC4492] */
#define TLS_ECDH_RSA_WITH_RC4_128_SHA         {0xC0,0x0C} /* [RFC4492] */
#define TLS_ECDHE_RSA_WITH_RC4_128_SHA        {0xC0,0x11} /* [RFC4492] */
#define TLS_ECDH_anon_WITH_RC4_128_SHA        {0xC0,0x16} /* [RFC4492] */
#define TLS_ECDHE_PSK_WITH_RC4_128_SHA        {0xC0,0x33} /* [RFC5489] */


/**
 * RECORD Layer 
 */

typedef struct dtls_record_st {
  tls_content_type_t type;
  tls_protocol_version_t version;
  uint16_t epoch;
  uint48_t sequence_number;
  uint16_t length;
  void *body;
}

/**
 * Handshake Protocol
 */

typedef enum {
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

typedef struct dtls_handshake_st {
  dtls_handshake_type_t msg_type;
  uint24_t length;
  uint16_t message_seq;
  uint24_t fragment_offset;
  uint24_t fragment_length;
  void *body;
} dtls_handshake_t;

#endif
/**
 * @}
 */
