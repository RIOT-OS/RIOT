/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       Initializes cryptoauth devices
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 * @}
 */

#include "log.h"
#include "atca.h"
#include "atca_params.h"
#include "kernel_defines.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if (IS_ACTIVE(CONFIG_MODULE_PSA_CRYPTO))
#include "psa_crypto_se_management.h"

extern psa_drv_se_t atca_methods;
#endif

#define ATCA_NUMOF (ARRAY_SIZE(atca_params))

static ATCADevice atca_devs[ATCA_NUMOF];

void auto_init_atca(void)
{
    for (unsigned i = 0; i < ATCA_NUMOF; i++) {
        atca_devs[i] = NULL;
        int status = atcab_init_ext(&atca_devs[i], (ATCAIfaceCfg *)&atca_params[i].cfg);
        if (status != ATCA_SUCCESS) {
            LOG_ERROR("[auto_init_atca] error initializing cryptoauth device #%u, status: 0x%02x\n",
                      i, status);
            continue;
        }
#if (IS_ACTIVE(CONFIG_MODULE_PSA_CRYPTO))
        if (i >= PSA_MAX_SE_COUNT) {
            LOG_ERROR("[auto_init_atca] PSA Crypto – too many secure elements #%u\n", i + 1);
            continue;
        }

        DEBUG("Registering Driver with location: %lx\n", atca_params[i].atca_loc);
        status = psa_register_secure_element(atca_params[i].atca_loc, &atca_methods, atca_devs[i]);
        if (status != PSA_SUCCESS) {
            LOG_ERROR(
                "[auto_init_atca] PSA Crypto – error registering cryptoauth PSA driver \
                for device #%u, status: %d\n", i,
                status);
            continue;
        }
#endif
    }
}
