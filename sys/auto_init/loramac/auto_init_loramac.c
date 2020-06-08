/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for Semtech Loramac-node package
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "log.h"
#include "semtech_loramac.h"

semtech_loramac_t loramac;

void auto_init_loramac(void)
{
    semtech_loramac_init(&loramac);
}
/** @} */
