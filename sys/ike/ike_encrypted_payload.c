#include "ike/ike_payloads.h"
#include "ike/ike.h"

#include "random.h"

#include <errno.h>
#include <string.h>
#include <byteorder.h>
#include <stdio.h>

#include "hashes/sha1.h"
#include "crypto/aes.h"
#include "crypto/modes/cbc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _encrypt(chunk_t in, chunk_t *out, chunk_t key, chunk_t iv);
static int _decrypt(chunk_t in, chunk_t *out, chunk_t key, chunk_t iv);
static int _sign(chunk_t in, chunk_t *out, chunk_t key);

int build_encrypted_payload(char *start, size_t max_len, size_t *new_len,
                            ike_payload_type_t first_payload, chunk_t enc_data, chunk_t ekey,
                            chunk_t akey)
{
    char iv[AES_BLOCK_SIZE];
    chunk_t ivc = (chunk_t){ .len = AES_BLOCK_SIZE, .ptr = iv };
    uint8_t padding_len = (AES_BLOCK_SIZE - ((enc_data.len + 1) % AES_BLOCK_SIZE)) % AES_BLOCK_SIZE;
    uint16_t len = sizeof(ike_generic_payload_header_t) + AES_BLOCK_SIZE + enc_data.len +
                   padding_len + 1 + HMAC_SIZE_SHA1_96;

    random_bytes((uint8_t *)iv, AES_BLOCK_SIZE);
    //puts("IV:");
    //printf_chunk(ivc, 8);
    if (max_len < len) {
        return -ENOMEM;
    }
    memset(enc_data.ptr + enc_data.len, 0x00, padding_len);
    memset(enc_data.ptr + enc_data.len + padding_len, padding_len, 1);
    chunk_t enc_in = malloc_chunk(enc_data.len + padding_len + 1);

    memcpy(enc_in.ptr, enc_data.ptr, enc_in.len);
    // chunk_t enc_in = {
    //     .ptr = enc_data.ptr,
    //     .len = enc_data.len + padding_len + 1};
    chunk_t enc_out = {
        .ptr = start + sizeof(ike_generic_payload_header_t) + AES_BLOCK_SIZE,
        .len = enc_in.len
    };

    if (_encrypt(enc_in, &enc_out, ekey, ivc) != 0) {
        free_chunk(&enc_in);
        return -1;
    }
    free_chunk(&enc_in);
    ike_generic_payload_header_t h = {
        .next_payload = first_payload,
        .payload_length = htons(len),
    };

    memcpy(start, &h, sizeof(h));
    memcpy(start + sizeof(h), ivc.ptr, ivc.len);
    ike_header_t *ike_h = (ike_header_t *)(start - sizeof(ike_header_t));

    ike_h->length = htonl(sizeof(ike_header_t) + len);

    chunk_t auth_in = {
        .ptr = (char *)ike_h,
        .len = sizeof(ike_header_t) + sizeof(ike_generic_payload_header_t) + AES_BLOCK_SIZE +
               enc_out.len,
    };
    chunk_t auth_out = {
        .ptr = start + sizeof(ike_generic_payload_header_t) + AES_BLOCK_SIZE + enc_out.len,
    };

    // TODO Truncate better
    if (_sign(auth_in, &auth_out, akey) != 0) {
        return -1;
    }

    *new_len = len;
    return 0;
}

int process_encrypted_payload(char *start, size_t max_len, size_t *cur_len,
                              ike_payload_type_t *next_payload, chunk_t *dec_data, chunk_t ekey,
                              chunk_t akey)
{
    int ret = process_generic_payload_header(start, max_len, cur_len, next_payload);

    if (ret < 0) {
        return ret;
    }
    char icd[20];
    chunk_t auth_in = {
        .ptr = start - sizeof(ike_header_t),
        .len = sizeof(ike_header_t) + *cur_len - HMAC_SIZE_SHA1_96,
    };
    chunk_t auth_out = {
        .ptr = icd,
        .len = sizeof(icd),
    };

    if (_sign(auth_in, &auth_out,
              akey) ||
        memcmp(auth_out.ptr, start + *cur_len - HMAC_SIZE_SHA1_96, HMAC_SIZE_SHA1_96)) {
        puts("Failed to verify checksum");
        return -1;
    }
    chunk_t ivc =
        (chunk_t){ .len = AES_BLOCK_SIZE, .ptr = start + sizeof(ike_generic_payload_header_t) };
    chunk_t dec_in = {
        .ptr = start + sizeof(ike_generic_payload_header_t) + AES_BLOCK_SIZE,
        .len = *cur_len - sizeof(ike_generic_payload_header_t) - AES_BLOCK_SIZE - HMAC_SIZE_SHA1_96,
    };

    *dec_data = malloc_chunk(dec_in.len);
    int error = _decrypt(dec_in, dec_data, ekey, ivc);

    if (error) {
        free_chunk(dec_data);
        return -1;
    }

    uint8_t padding_len = *(dec_data->ptr + dec_data->len - 1);

    if (padding_len > dec_data->len) {
        free_chunk(dec_data);
        return -1;
    }
    dec_data->ptr = realloc(dec_data->ptr, dec_data->len - padding_len - 1);
    dec_data->len -= padding_len + 1;

    return 0;
}

static int _encrypt(chunk_t in, chunk_t *out, chunk_t key, chunk_t iv)
{
    cipher_t cipher;

    if (cipher_init(&cipher, CIPHER_AES, (unsigned char *)key.ptr,
                    key.len) != CIPHER_INIT_SUCCESS) {
        DEBUG("IKE: Failed to initialize cipher\n");
        return -1;
    }

    if (cipher_encrypt_cbc(&cipher, (unsigned char *)iv.ptr,
                           (unsigned char *)in.ptr, in.len,
                           (unsigned char *)out->ptr) < 0) {
        DEBUG("IKE: Failed to encrypt\n");
        return -1;
    }

    return 0;
}

static int _decrypt(chunk_t in, chunk_t *out, chunk_t key, chunk_t iv)
{
    cipher_t cipher;

    if (cipher_init(&cipher, CIPHER_AES, (unsigned char *)key.ptr,
                    key.len) != CIPHER_INIT_SUCCESS) {
        DEBUG("IKE: Failed to initialize cipher\n");
        return -1;
    }

    if (cipher_decrypt_cbc(&cipher, (unsigned char *)iv.ptr,
                           (unsigned char *)in.ptr, in.len,
                           (unsigned char *)out->ptr) < 0) {
        DEBUG("IKE: Failed to decrypt\n");
        return -1;
    }

    return 0;
}

static int _sign(chunk_t in, chunk_t *out, chunk_t key)
{
    sha1_context hash;

    sha1_init_hmac(&hash, (unsigned char *)key.ptr, key.len);
    sha1_update(&hash, (unsigned char *)in.ptr, in.len);
    sha1_final_hmac(&hash, (unsigned char *)out->ptr);
    return 0;
}
