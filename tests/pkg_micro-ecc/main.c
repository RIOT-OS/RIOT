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
#include "periph/random.h"

#define TESTROUNDS  16

int main(void)
{
    printf("micro-ecc compiled!\n");

    int i, errorc = 0;

    uint8_t l_private1[uECC_BYTES];
    uint8_t l_private2[uECC_BYTES];

    uint8_t l_public1[uECC_BYTES * 2];
    uint8_t l_public2[uECC_BYTES * 2];

    uint8_t l_secret1[uECC_BYTES];
    uint8_t l_secret2[uECC_BYTES];

    uint8_t l_hash[uECC_BYTES];

    uint8_t l_sig[uECC_BYTES * 2];

    /* initialize hardware random number generator */
    random_init();
    /* power off RNG to save energy */
    random_poweroff();

    printf("Testing %d random private key pairs and signature\n", TESTROUNDS);

    for (i = 0; i < TESTROUNDS; ++i) {
        printf(".");

        if (!uECC_make_key(l_public1, l_private1) || !uECC_make_key(l_public2, l_private2)) {
            printf("\nRound %d: uECC_make_key() failed", i);
            errorc++;
        }
        else {
            if (!uECC_shared_secret(l_public2, l_private1, l_secret1)) {
                printf("\nRound %d: shared_secret() failed (1)", i);
                errorc++;
            }
            else {
                if (!uECC_shared_secret(l_public1, l_private2, l_secret2)) {
                    printf("\nRound: %d: shared_secret() failed (2)", i);
                    errorc++;
                }
                else {
                    if (memcmp(l_secret1, l_secret2, sizeof(l_secret1)) != 0) {
                        printf("\nShared secrets are not identical!\n");
                        errorc++;
                    }

                    memcpy(l_hash, l_public1, uECC_BYTES);

                    if ((uECC_sign(l_private1, l_hash, l_sig)) != 1) {
                        printf("\nRound %d: uECC_sign() failed", i);
                        errorc++;
                    }
                    else {
                        if ((uECC_verify(l_public1, l_hash, l_sig)) != 1) {
                            printf("\nRound %d: uECC_verify() failed", i);
                            errorc++;
                        }
                    }
                }
            }
        }
    }

    printf(" done with %d error(s)\n", errorc);

    if (errorc == 0) {
        return 0;
    }
    else {
        return 1;
    }
}
