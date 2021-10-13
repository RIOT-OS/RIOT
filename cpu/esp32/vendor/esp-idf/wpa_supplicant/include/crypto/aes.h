/*
 * AES functions
 * Copyright (c) 2003-2006, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef AES_H
#define AES_H

#ifdef __cplusplus
extern "C" {
#endif

#define AES_BLOCK_SIZE 16

void *wpa_aes_encrypt_init(const u8 *key, size_t len);
void wpa_aes_encrypt(void *ctx, const u8 *plain, u8 *crypt);
void wpa_aes_encrypt_deinit(void *ctx);
void *wpa_aes_decrypt_init(const u8 *key, size_t len);
void wpa_aes_decrypt(void *ctx, const u8 *crypt, u8 *plain);
void wpa_aes_decrypt_deinit(void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* AES_H */
