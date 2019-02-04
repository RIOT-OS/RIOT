# Micro-ECC for RIOT

This port of Micro-ECC to RIOT is based on the Micro-ECC
[upstream](https://github.com/kmackay/micro-ecc) and adds `hwrng_read`
(provided by RIOT) as the default RNG function if it is available on the target
platform. This port also fixes a minor issue with unused variables in the
upstream code.

# Usage

## Build

Add
```Makefile
USEPKG += micro-ecc
```
to your Makefile.

## Examples

Examples of using the uECC API can be found in the `test` folder of the
Micro-ECC upstream.

**WARNING** For `uECC_make_key` and `uECC_sign` to be secure, the
random module is expected to be seeded with a high entropy random seed.
