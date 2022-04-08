/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_libcsp
 * @{
 *
 * @file
 * @brief       LibCSP RIOT
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef CSP_RIOT_H
#define CSP_RIOT_H

#include <stdint.h>
#include "csp/csp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default CSP rout table
 */
#ifndef CONFIG_CSP_ROUTE_TABLE
#define CONFIG_CSP_ROUTE_TABLE                  NULL
#endif

/**
 * @brief   Set de-duplication configuration
 *
 * @param type  CSP_DEDUP_OFF De-duplication off
 *              CSP_DEDUP_FWD De-duplication on forwarding only
 *              CSP_DEDUP_INCOMING De-duplication on incoming only
 *              CSP_DEDUP_ALL De-duplication on incoming and forwarding
 */
void libcsp_set_dedup(enum csp_dedup_types type);

/**
 * @brief   Return de-duplication configuration
 *
 * @return  De-duplication configuration
 */
enum csp_dedup_types libcsp_get_dedup(void);

/**
 * @brief   Return configured CSP version
 *
 * @return  The version
 */
uint8_t libcsp_get_csp_version(void);

/**
 * @brief   Set CSP version
 *
 * @param version   CSP version, 1 or 2
 *
 * @return  0 on success, -EINVAL if invalid version
 */
int libcsp_set_csp_version(uint8_t version);

/**
 * @brief   Initialize LibCSP
 *
 */
void libcsp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* CSP_RIOT_H */
