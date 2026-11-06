/*
 * SPDX-FileCopyrightText: 2022 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_libcose
 * @{
 *
 * @file
 * @brief       RIOT as a crypto backend common functions
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdint.h>
#include "random.h"
#include "kernel_defines.h"
#include "xfa.h"

#include "cose/crypto.h"
#include "cose/crypto/riot.h"

#if IS_USED(MODULE_AUTO_INIT)
#include "auto_init_utils.h"
#endif

static int _riot_random_bytes(void* arg, unsigned char * buf, size_t len)
{
    (void)arg;
    random_bytes((uint8_t*) buf, len);
    return 1;
}

void libcose_crypt_init(void)
{
    cose_crypt_set_rng(_riot_random_bytes, NULL);
}

#if IS_USED(MODULE_AUTO_INIT_LIBCOSE_CRYPT)
/* initialize just after random module */
AUTO_INIT(libcose_crypt_init, AUTO_INIT_PRIO_MOD_LIBCOSE);
#endif
