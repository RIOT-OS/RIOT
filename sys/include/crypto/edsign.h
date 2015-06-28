/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Headers for the implementation of the Ed25519 signature system
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author      D. J. Bernstein
 *
 */

#ifndef __EDSIGN_H_
#define __EDSIGN_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Implementation constants
 * @{
 */
/* Any string of 32 random bytes is a valid secret key */
#define EDSIGN_SECRET_KEY_SIZE	32

/* Given a secret key, produce the public key (a packed Edwards-curve * point) */
#define EDSIGN_PUBLIC_KEY_SIZE	32

/* Expand a secret key */
#define EXPANDED_SIZE		64

/* Produce a signature for a message */
#define EDSIGN_SIGNATURE_SIZE	64
/** @} */

/**
 * @brief  Compute public key from secret key
 *
 * @param[out]  pub		Public key output
 * @param[in]   secret		Secret key input
 */
void edsign_sec_to_pub(uint8_t *pub, const uint8_t *secret);

/**
 * @brief  Produce an Ed25519 signature for a message
 *
 * @param[out]	signature	Signature output
 * @param[in]	pub		Public key input
 * @param[in]	secret		Secret key input
 * @param[in]	message		Message to sign
 * @param[in]	len		Length of input message
 */
void edsign_sign(uint8_t *signature, const uint8_t *pub, const uint8_t *secret,
                 const uint8_t *message, size_t len);
/**
 * @brief  Verify a message signature
 *
 * @param[in]	signature	Signature to verify
 * @param[in]	pub		Public key input
 * @param[in]	message		Message to verify
 * @param[in]	len		Length of input message to verify
 *
 * @return Returns non-zero value if verification is ok
 */
uint8_t edsign_verify(const uint8_t *signature, const uint8_t *pub, const uint8_t *message,
                      size_t len);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __EDSIGN_H_ */
