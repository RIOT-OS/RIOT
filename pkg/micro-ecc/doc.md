/**
 * @defgroup pkg_micro_ecc   Micro-ECC for RIOT
 * @ingroup  pkg
 * @ingroup  sys_crypto
 * @brief    Micro-ECC for RIOT
 *
 * # Micro-ECC for RIOT
 *
 * This port of Micro-ECC to RIOT is based on the Micro-ECC
 * [upstream](https://github.com/kmackay/micro-ecc) and adds `hwrng_read`
 * (provided by RIOT) as the default RNG function if it is available on the target
 * platform. This port also fixes a minor issue with unused variables in the
 * upstream code.
 *
 * # Usage
 *
 * ## Build
 *
 * Add
 * ```Makefile
 * USEPKG += micro-ecc
 * ```
 * to your Makefile.
 *
 * ## Choosing the right API
 *
 * Before using the Micro-ECC library, you need to check the `Makefile.features`
 * of your target board to see if `periph_hwrng` is provided.
 *
 * If it is provided, you may safely use `uECC_make_key` to generate ECDSA key
 * pairs and call `uECC_sign`/`uECC_verify` to sign/verify the ECDSA signatures.
 *
 * If not, you cannot use `uECC_make_key` or `uECC_sign` APIs anymore. The ECDSA
 * keys have to be generated on a platform with HWRNG support (e.g., `native`) and
 * transferred to your target device. You need to use `uECC_sign_deterministic` to
 * perform ECDSA deterministic signing (standardized by RFC 6979). You can still
 * use `uECC_verify` to verify the signatures from both signing APIs.
 *
 * **WARNING** Calling `uECC_make_key` and `uECC_sign` APIs on platforms without
 * HWRNG support will lead to compile failure.
 *
 * Examples of using these uECC APIs can be found in the `test` folder of the
 * Micro-ECC upstream.
 *
 * @see     https://github.com/kmackay/micro-ecc
 */
