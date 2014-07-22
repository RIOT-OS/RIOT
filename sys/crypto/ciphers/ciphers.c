/**
 *
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sys_crypto
 * @{
 * @file   ciphers.c
 * @author Nico von Geyso <nico.geyso@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <stdio.h>
#include "crypto/ciphers.h"

extern cipher_interface_t null_interface;

#ifdef MODULE_RC5
extern cipher_interface_t rc5_interface;
#endif

#ifdef MODULE_3DES
extern cipher_interface_t tripledes_interface;
#endif

#ifdef MODULE_AES
extern cipher_interface_t aes_interface;
#endif

#ifdef MODULE_TWOFISH
extern cipher_interface_t twofish_interface;
#endif

#ifdef MODULE_SKIPJACK
extern cipher_interface_t skipjack_interface;
#endif

const cipher_entry_t cipher_list[] = {
	{"NULL", CIPHER_NULL, &null_interface, 16},
#ifdef MODULE_RC5
	{"RC5-32/12", CIPHER_RC5, &rc5_interface, 32},
#endif
#ifdef MODULE_3DES
	{"3DES", CIPHER_3DES, &tripledes_interface, 8},
#endif
#ifdef MODULE_AES
	{"AES-128", CIPHER_AES_128, &aes_interface, 16},
#endif
#ifdef MODULE_TWOFISH
	{"TWOFISH", CIPHER_TWOFISH, &twofish_interface, 16},
#endif
#ifdef MODULE_SKIPJACK
	{"SKIPJACK", CIPHER_SKIPJACK, &skipjack_interface, 8},
#endif
	{NULL, CIPHER_UNKNOWN, NULL, 0}
};


int cipher_init(cipher_t* cipher, cipher_id_t cipher_id, uint8_t* key,
                uint8_t key_size)
{
	const cipher_entry_t* entry = NULL;

	for (uint8_t i = 0; cipher_list[i].id != CIPHER_UNKNOWN; ++i) {
		if (cipher_list[i].id == cipher_id) {
			entry = &cipher_list[i];
			break;
		}
	}

	if (entry == NULL) {
		return CIPHER_ERR_UNSUPPORTED_CIHPER;
	}


	if (key_size > entry->interface->max_key_size) {
		return CIPHER_ERR_INVALID_KEY_SIZE;
	}

	cipher->interface = entry->interface;
	return cipher->interface->init(&cipher->context, entry->block_size, key,
	                               key_size);

}


int cipher_set_key(cipher_t* cipher, uint8_t* key, uint8_t key_size)
{
	if (key_size > cipher->interface->max_key_size) {
		return CIPHER_ERR_INVALID_KEY_SIZE;
	}

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
