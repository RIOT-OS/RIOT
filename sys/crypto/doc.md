<!-- SPDX-FileCopyrightText: 2013 Freie Universität Berlin -->
<!-- SPDX-License-Identifier: LGPL-2.1-only -->

@defgroup   sys_crypto Crypto
@ingroup    sys

@brief      RIOT provides a collection of block cipher ciphers, different
            operation modes and cryptographic hash algorithms.

@warning    This module has **NO GUARANTEES** regarding to robustness against
            side channel attacks (such as differential power analysis or timing
            attacks).

@note       This module is implemented in portable C code. The C standard is
            defined in terms of an abstract machine, which does not model
            execution time or power consumption. Typical side-channel attacks
            cannot even be expressed in terms of the abstract machine.
            It is therefore strictly impossible to implement cryptography using
            only portable C code that is guaranteed to be side-channel free
            across platforms and compilers.

@section ciphers Ciphers

RIOT supports the following block ciphers:
-   AES-128, AES-192, AES-256
-   NULL

You can use them directly by adding `crypto_aes_128`, `crypto_aes_192` or
`crypto_aes_256` to your `USEMODULE`-List.
While you can use the ciphers functions directly, you should resort to
the generic API for block ciphers whenever possible.

Depending on the selected block ciphers, a sufficient large buffer size of
the cipher_context_t is used for en-/de-cryption operations.

Example:
```c
#include "crypto/ciphers.h"

cipher_t cipher;
uint8_t key[AES_KEY_SIZE] = {0},
    plain_text[AES_BLOCK_SIZE] = {0},
    cipher_text[AES_BLOCK_SIZE] = {0};

if (cipher_init(&cipher, CIPHER_AES, key, AES_KEY_SIZE) < 0) {
    printf("Cipher init failed!\n");
}

if (cipher_encrypt(&cipher, plain_text, cipher_text) < 0) {
    printf("Cipher encryption failed!\n");
}
else {
    od_hex_dump(cipher_text, AES_BLOCK_SIZE, 0);
}
```

Some aspects of the AES implementation can be fine tuned by pseudo-modules:
-   crypto_aes_precalculated: Use pre-calculated T-tables. This improved
    speed at the expense of increased program size. The default is to
    calculate most tables on the fly.
-   crypto_aes_unroll: enable manually-unrolled loops. The default is to not
    have them unrolled.

If you need to encrypt data of arbitrary size take a look at the different
operation modes like: CBC, CTR or CCM.

Additional examples can be found in the test suite.
