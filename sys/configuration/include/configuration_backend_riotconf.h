/*
 * Copyright (C) 2024 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    sys_configuration
 *
 * @{
 *
 * @file
 * @brief   Interface for riotconf as a configuration backend
 *
 * @author  Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef CONFIGURATION_BACKEND_RIOTCONF_H
#define CONFIGURATION_BACKEND_RIOTCONF_H

#include "board.h"
#include "modules.h"
#include "mtd.h"
#include "configuration.h"
#include "riotconf/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

#if DOXYGEN
/**
 * @brief   The application may define this to set a preferred configuration version
 */
#define CONFIGURATION_BACKENT_RIOTCONF_VERSION
#endif

/**
 * @brief   __attribute__((weak)) function to be overwritten by the application
 *          to set a more specific criteria for a preferred configuration slot
 *
 * @param[in]   hdr     The header of the configuration slot to check
 *
 * @return 0 if the slot is not accepted, 1 if the slot is accepted
 */
int configuration_backend_riotconf_accept(const riotconf_hdr_t *hdr);

/**
 * @brief   Retrieve the current riotconf slot in use
 *
 * Pass the return value to @ref riotconf_slot_other to get the other slot
 * to perform a safe configuration update.
 *
 * @return riotconf_slot_t
 */
riotconf_slot_t configuration_backend_riotconf_slot_current(void);

/**
 * @brief   Retrieve the header of the current configuration slot
 *
 * @return  The header of the current configuration slot
 */
riotconf_hdr_t configuration_backend_riotconf_header_current(void);

/**
 * @brief   __attribute__((weak)) function to be overwritten by the application
 *          to call @ref riotconf_storage_init with board supplied MTDs
 */
void configuration_backend_riotconf_storage_init(void);

/**
 * @brief   Initialize the riotconf configuration backend
 *
 * @return  0 on success, <0 on error
 */
int configuration_backend_riotconf_init(void);

/**
 * @brief   Reset the riotconf configuration backend
 *
 * @return  0 on success, <0 on error
 */
int configuration_backend_riotconf_reset(void);

/**
 * @brief   Auto-initialize the riotconf configuration backend
 */
void auto_init_configuration_backend_riotconf(void);

/**
 * @brief   riotconf backed operations
 */
extern const conf_backend_ops_t conf_backend_riotconf_ops;

#ifdef __cplusplus
}
#endif

#endif /* CONFIGURATION_BACKEND_RIOTCONF_H */
/** @} */
