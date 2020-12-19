/**
 * @defgroup pkg_relic  Relic toolkit for RIOT
 * @ingroup  pkg
 * @ingroup  sys_crypto
 * @brief    Provides the Relic cryptographic toolkit to RIOT
 *
 *
 * # Configuration Options
 * You can pass along configuration flags for RELIC from your project makefile via:
 * ```
 * export RELIC_CONFIG_FLAGS=-DARCH=NONE -DQUIET=off -DWORD=32 -DFP_PRIME=255 -DWITH="BN;MD;DV;FP;EP;CP;BC;EC" -DSEED=ZERO
 * ```
 *
 * This should happen before the ```USEPKG``` line.
 *
 * # Usage
 * Just put
 * ```
 * USEPKG += relic
 * ```
 * in your Makefile and
 * ```c
 * #include <relic.h>
 * ```
 * in your `main.c`.
 *
 * @see      https://github.com/relic-toolkit/relic
 */
