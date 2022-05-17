/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
