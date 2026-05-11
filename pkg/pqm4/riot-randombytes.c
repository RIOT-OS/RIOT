/*
 * SPDX-FileCopyrightText: 2026 Mert Cakir <mert-cakir@outlook.com>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: CC0-1.0
 */

/**
 * @ingroup pkg_pqm4
 * @{
 *
 * @file
 * @brief   Linker-side randombytes() for pqm4 backed by RIOT's RNG.
 *
 * pqm4 (via pqclean) calls randombytes(), which is `#define`d to
 * PQCLEAN_randombytes() in pqclean/common/randombytes.h. We provide
 * that symbol here, backed by RIOT's @ref random_bytes(). This
 * replaces pqm4's default randombytes implementation (which falls
 * back to an insecure stub on most platforms).
 *
 * @author  Mert Cakir <mert-cakir@outlook.com>
 */

#include <stddef.h>
#include <stdint.h>

#include "random.h"

/**
 * @brief   pqclean-style RNG entry point.
 *
 * Called by pqm4 internals (post-`#define` rewrite of randombytes() ->
 * PQCLEAN_randombytes()). Routes to RIOT's @ref random_bytes().
 *
 * Marked **weak** so applications that need deterministic randomness
 * (for example NIST FIPS 204 KAT vector replay) can provide their own
 * non-weak override at link time.
 *
 * @param[out] output   Bytes to fill.
 * @param[in]  n        Number of bytes.
 *
 * @return Always 0 (success); RIOT @ref random_bytes() does not fail.
 */
__attribute__((weak)) int PQCLEAN_randombytes(uint8_t *output, size_t n)
{
    random_bytes(output, n);
    return 0;
}

/** @} */
