@defgroup pkg_pqm4 pqm4 - Post-Quantum Cryptography for ARM Cortex-M
@ingroup  pkg
@brief    pqm4 ML-DSA-44 (FIPS 204) reference for Cortex-M targets
@author   Mert Cakir <mert-cakir@outlook.com>

pqm4 is a benchmarking framework and library of post-quantum cryptographic
primitives optimised for ARM Cortex-M microcontrollers. This RIOT package
exposes the ML-DSA-44 (FIPS 204) signing scheme via pqm4's m4f speed
variant.

The same Cortex-M4F-targeted assembly runs unmodified on Cortex-M33 and
later cores (ARMv8-M Mainline + DSP + FPv5 is a strict superset of
ARMv7E-M + DSP + FPv4). The package thus brings ML-DSA-44 to any
RIOT-supported Cortex-M >= M4F target.

## Usage

Add to your application Makefile:

```makefile
USEPKG += pqm4
```

Then in your code:

```c
#include "api.h"

uint8_t pk[CRYPTO_PUBLICKEYBYTES];
uint8_t sk[CRYPTO_SECRETKEYBYTES];
uint8_t sig[CRYPTO_BYTES];

crypto_sign_keypair(pk, sk);
crypto_sign(sm, &smlen, msg, msglen, sk);
crypto_sign_open(m, &mlen, sm, smlen, pk);
```

## References

@see https://github.com/mupq/pqm4
@see https://csrc.nist.gov/pubs/fips/204/final
@see https://tches.iacr.org/index.php/TCHES/article/view/8989
