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

#define ENABLE_DEBUG 0
#include "debug.h"

#define ATCA_NUMOF (ARRAY_SIZE(atca_params))

void auto_init_atca(void)
{
    for (unsigned i = 0; i < ATCA_NUMOF; i++) {
        if (atcab_init((ATCAIfaceCfg *)&atca_params[i]) != ATCA_SUCCESS) {
            LOG_ERROR("[auto_init_atca] error initializing cryptoauth device #%u\n", i);
            continue;
        }
    }
}
