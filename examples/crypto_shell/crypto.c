/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       UDP Shell application
 *
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hashes.h"
#include "crypto/ciphers.h"
#include "crypto/modes/ecb.h"
#include "crypto/modes/ctr.h"
#include "crypto/modes/cbc.h"
#include "crypto/modes/ccm.h"
#include "crypto.h"

cipher_id_t get_cipher_id(char* str)
{
    switch (kr_hash((const uint8_t*)str, strlen(str)))
    {
        case CIPHER_STR_ID_AES:
            return CIPHER_AES_128;
        case CIPHER_STR_ID_TWOFISH:
            return CIPHER_TWOFISH;
        case CIPHER_STR_ID_3DES:
            return CIPHER_3DES;
        case CIPHER_STR_ID_SKIPJACK:
            return CIPHER_SKIPJACK;
        case CIPHER_STR_ID_RC5:
            return CIPHER_RC5;
        default:
            return CIPHER_UNKNOWN;
    }
}

uint8_t get_len_of_hexstring(char* str)
{
    uint8_t size = 1;
    do {
        if (*str == ' ')
            size++;
    } while (*(str++));

    return size;
}


void shell_cipher_encrypt(int argc, char **argv)
{
    cipher_t cipher;
    cipher_id_t cipher_id;
    int len = 0;
    size_t key_size = 0, p_size, c_size = 0;
    uint8_t *key, iv[16] = {0}, nonce[16] = {0}, *plaintext, *ciphertext,
            block_size;

    if (argc < 5) {
        printf("\t usage: encrypt <cipher> <mode> <key> <msg_to_encrypt>\n");
        printf("\t\t Ciphers: aes, 3des, rc5, skipjack and twofish\n");
        printf("\t\t Modes: ecb, cbc, ctr or ccm\n");
        return;
    }

    key = (uint8_t*) argv[3];
    key_size = strlen(argv[3]);

    cipher_id = get_cipher_id(argv[1]);
    cipher_init(&cipher, cipher_id, key, key_size);

    block_size = cipher_get_block_size(&cipher);
    plaintext = (uint8_t*) argv[4];
    p_size = strlen(argv[4]);
    c_size = (p_size + (block_size - p_size % block_size) + 8);

    ciphertext = calloc(c_size, sizeof(uint8_t));
    if (ciphertext == NULL) {
        printf("ERROR: not enough memory\n");
        goto cleanup;
    }

    switch (kr_hash((const uint8_t*)argv[2], strlen(argv[2])))
    {
        case BLOCK_MODE_ECB:
            len = cipher_encrypt_ecb(&cipher, plaintext, p_size, ciphertext); 
        break;

        case BLOCK_MODE_CBC:
            len = cipher_encrypt_cbc(&cipher, iv, plaintext, p_size, ciphertext); 
        break;

        case BLOCK_MODE_CTR:
            len = cipher_encrypt_ctr(&cipher, nonce, 0, plaintext, p_size,
                        ciphertext); 
        break;

        case BLOCK_MODE_CCM:
            len = cipher_encrypt_ccm(&cipher, NULL, 0, 8, 2, nonce, 0,
                        plaintext, p_size, ciphertext); 
        break;

       default:
            printf("ERROR: invalid mode. Supported Modes are: ecb, cbc, ctr or ccm.\n");
            goto cleanup;
    }

    if (len < 0) {
        if (len == CIPHER_ERR_INVALID_LENGTH) {
            printf("ERROR: Your selected mode expects plaintext of n*block_size.\n");
            printf("       Your input was %d but block size is %d\n", p_size, block_size);
        } else {
            printf("ERROR: could not encrypt plaintext (err = %d)\n", len);
        }
        goto cleanup;
    }

    // Output ciphertext in Hex
    printf("\n");
    for (uint16_t i=0; i < len; ++i)
        printf("%x ", ciphertext[i]);
    printf("\n");

cleanup:
    if (ciphertext != NULL)
        free(ciphertext);
}


void shell_cipher_decrypt(int argc, char **argv)
{
    cipher_t cipher;
    cipher_id_t cipher_id;
    size_t key_size = 0, size;
    int len;
    char *str;
    uint8_t iv[16] = {0}, nonce[16] = {0}, *key, *ciphertext, *plaintext;

    if (argc < 4) {
        printf("\t usage: decrypt <cipher> <mode> <key> <ciphertext>\n");
        printf("\t\t Ciphers: aes, 3des, rc5, skipjack and twofish\n");
        printf("\t\t Modes: ecb, cbc, ctr and ccm\n");
        return;
    }

    key = (uint8_t*) argv[3];
    key_size = strlen(argv[3]);

    cipher_id = get_cipher_id(argv[1]);
    cipher_init(&cipher, cipher_id, key, key_size);

    size = get_len_of_hexstring(argv[4]);
    ciphertext = calloc(size, sizeof(uint8_t));
    plaintext = calloc(size + 1, sizeof(uint8_t));

    if (ciphertext == NULL || plaintext == NULL) {
        printf("ERROR: not enough memory\n");
        goto cleanup;
    }

    // convert hex string input to binary
    str = argv[4];
    for (uint8_t i=0; i < size; ++i)
        ciphertext[i] = strtol(str, &str, 16);


    switch (kr_hash((const uint8_t*)argv[2], strlen(argv[2])))
    {
        case BLOCK_MODE_ECB:
            len = cipher_decrypt_ecb(&cipher, ciphertext, size, plaintext);
        break;

        case BLOCK_MODE_CBC:
            len = cipher_decrypt_cbc(&cipher, iv, ciphertext, size, plaintext);
        break;

        case BLOCK_MODE_CTR:
            len = cipher_decrypt_ctr(&cipher, nonce, 0, ciphertext, size,
                        plaintext);
        break;

        case BLOCK_MODE_CCM:
            len = cipher_decrypt_ccm(&cipher, NULL, 0, 8, 2, nonce, 0,
                        ciphertext, size, plaintext);
        break;

       default:
            printf("ERROR: invalid mode. Supported Modes are: ecb, cbc, ctr, ccm.\n");
            goto cleanup;
    }

    if (len < 0) {
        printf("ERROR: could not decrypt ciphertext\n");
        goto cleanup;
    }

    printf("%s\n", plaintext);

cleanup:
    if (plaintext != NULL)
        free(plaintext);

    if (ciphertext != NULL)
        free(ciphertext);
}
