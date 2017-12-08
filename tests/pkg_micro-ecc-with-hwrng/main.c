/*-
 * Copyright 2014 Kenneth MacKay
 * Copyright 2014 Frank Holtz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * @file
 * @brief       Check if the micro-ecc builds and working
 *
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 *
 */

#include <stdio.h>
#include <string.h>
#include "uECC.h"
#include "periph/hwrng.h"

#define TESTROUNDS  16

int main(void)
{
    printf("micro-ecc compiled!\n");

    const struct uECC_Curve_t *curve = uECC_secp256r1();
    int i, errorc = 0;

    int curve_size = uECC_curve_private_key_size(curve);
    int public_key_size = uECC_curve_public_key_size(curve);

    uint8_t l_secret1[curve_size];
    uint8_t l_secret2[curve_size];

    /* reserve space for a SHA-256 hash */
    uint8_t l_hash[32] = { 0 };

    uint8_t l_sig[public_key_size];

    printf("Testing %d random private key pairs and signature using HWRNG\n", TESTROUNDS);

    uint8_t l_private1[curve_size];
    uint8_t l_private2[curve_size];

    uint8_t l_public1[public_key_size];
    uint8_t l_public2[public_key_size];

    for (i = 0; i < TESTROUNDS; ++i) {
        printf(".");

        if (!uECC_make_key(l_public1, l_private1, curve) || !uECC_make_key(l_public2, l_private2, curve)) {
            printf("\nRound %d: uECC_make_key() failed", i);
            errorc++;
        }
        else {
            if (!uECC_shared_secret(l_public2, l_private1, l_secret1, curve)) {
                printf("\nRound %d: shared_secret() failed (1)", i);
                errorc++;
            }
            else {
                if (!uECC_shared_secret(l_public1, l_private2, l_secret2, curve)) {
                    printf("\nRound: %d: shared_secret() failed (2)", i);
                    errorc++;
                }
                else {
                    if (memcmp(l_secret1, l_secret2, sizeof(l_secret1)) != 0) {
                        printf("\nShared secrets are not identical!\n");
                        errorc++;
                    }

                    /* copy some bogus data into the hash */
                    memcpy(l_hash, l_public1, 32);

                    if ((uECC_sign(l_private1, l_hash, sizeof(l_hash), l_sig, curve)) != 1) {
                        printf("\nRound %d: uECC_sign() failed", i);
                        errorc++;
                    }
                    else {
                        if ((uECC_verify(l_public1, l_hash, sizeof(l_hash), l_sig, curve)) != 1) {
                            printf("\nRound %d: uECC_verify() failed", i);
                            errorc++;
                        }
                    }
                }
            }
        }
    }

    printf(" done with %d error(s)\n", errorc);

    if (errorc) {
        puts("FAILURE");
        return 1;
    }
    else {
        puts("SUCCESS");
        return 0;
    }
}
