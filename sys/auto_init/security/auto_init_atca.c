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

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
#include "psa/crypto.h"
#include "psa_crypto_se_management.h"

extern psa_drv_se_t atca_methods;

psa_se_config_t atca_config_list[] = { ATCA_CONFIG_LIST };
#endif

ATCADevice atca_devs_ptr[ATCA_NUMOF];

static struct atca_device atca_devs[ATCA_NUMOF];

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
void auto_init_atca(void)
{
    DEBUG("[auto_init_atca] Number of secure elements: %zu\n", ATCA_NUMOF);
    for (unsigned i = 0; i < ATCA_NUMOF; i++) {
        int status = initATCADevice((ATCAIfaceCfg *)&atca_params[i].cfg, (ATCADevice)&atca_devs[i]);
        if (status != ATCA_SUCCESS) {
            LOG_ERROR("[auto_init_atca] error initializing cryptoauth device #%u, status: %d\n",
                      i, status);
            continue;
        }
        atca_devs_ptr[i] = &atca_devs[i];

        DEBUG("[auto_init_atca] Registering Driver with address: %x and location: %lx\n", atca_params[i].cfg.atcai2c.address, (unsigned long) atca_params[i].atca_loc);
        status = psa_register_secure_element(atca_params[i].atca_loc,
                                            &atca_methods,
                                            &atca_config_list[i],
                                            &atca_devs[i]);
        if (status != PSA_SUCCESS) {
            LOG_ERROR(
                "[auto_init_atca] PSA Crypto – error registering cryptoauth PSA driver\
                for device #%u, status: %s\n", i, psa_status_to_humanly_readable(status));
            continue;
        }
    }
}
#else
void auto_init_atca(void)
{
    DEBUG("[auto_init_atca] Number of secure elements: %zu\n", ATCA_NUMOF);
    for (unsigned i = 0; i < ATCA_NUMOF; i++) {
        int status = initATCADevice((ATCAIfaceCfg *)&atca_params[i], (ATCADevice)&atca_devs[i]);
        if (status != ATCA_SUCCESS) {
            LOG_ERROR("[auto_init_atca] error initializing cryptoauth device #%u, status: %d\n",
                      i, status);
            continue;
        }
        atca_devs_ptr[i] = &atca_devs[i];
    }
}
#endif
