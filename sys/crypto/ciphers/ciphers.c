#include <string.h>
#include <stdio.h>
#include "crypto/ciphers.h"

const cipher_entry_t cipher_list[]= {
    {"NULL", CIPHER_NULL, &null_interface, 0},
#ifdef MODULE_CRYPTO_CIPHERS_RC5
    {"RC5-32/12", CIPHER_RC5, &rs5_interface, 32},
#endif
#ifdef MODULE_CRYPTO_CIPHERS_3DES
    {"3DES", CIPHER_3DES, &tipledes_interface, 8},
#endif
#ifdef MODULE_CRYPTO_CIPHERS_AES
    {"AES-128", CIPHER_AES_128, &aes_interface, 16},
#endif
#ifdef MODULE_CRYPTO_CIPHERS_TWOFISH
    {"TWOFISH", CIPHER_AES_128, &twofish_interface, 16},
#endif
#ifdef MODULE_CRYPTO_CIPHERS_SKIPJACK
    {"SKIPJACK", CIPHER_AES_128, &skipjack_interface, 8},
#endif
    {NULL, CIPHER_UNKNOWN, NULL, 0}
};


int cipher_init(cipher_t* cipher, cipher_id_t cipher_id, uint8_t *key,
                uint8_t key_size)
{
    const cipher_entry_t *entry = NULL;

    for (uint8_t i = 0; cipher_list[i].id != CIPHER_UNKNOWN; ++i) {
        if (cipher_list[i].id == cipher_id) {
            entry = &cipher_list[i];
            break;
        }
    }

    if (entry == NULL)
        return CIPHER_ERR_UNSUPPORTED_CIHPER;


    if (key_size > entry->interface->max_key_size)
        return CIPHER_ERR_INVALID_KEY_SIZE;

    cipher->interface = entry->interface;
    return cipher->interface->init(&cipher->context, entry->block_size, key,
                key_size);

}


int cipher_set_key(cipher_t* cipher, uint8_t *key, uint8_t key_size)
{
    if (key_size > cipher->interface->max_key_size)
        return CIPHER_ERR_INVALID_KEY_SIZE;

    return cipher->interface->set_key(&cipher->context, key, key_size);
}


int cipher_encrypt(cipher_t* cipher, uint8_t* input, uint8_t* output)
{
    return cipher->interface->encrypt(&cipher->context, input, output);
}


int cipher_decrypt(cipher_t* cipher, uint8_t* input, uint8_t* output)
{
    return cipher->interface->decrypt(&cipher->context, input, output);
}


int cipher_get_block_size(cipher_t* cipher)
{
    return cipher->interface->block_size;
}


void cipher_print_supported_ciphers()
{
    printf("Not yet implemented!\n");
}
