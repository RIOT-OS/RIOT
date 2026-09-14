/*
 * SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
 */

#pragma once

/**
 * @defgroup    sys_riotboot_bootloader RIOT bootloader common functions
 * @ingroup     sys
 * @{
 *
 * @brief       RIOT bootloader common functions
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.loc>
 * @}
 */

#include "riotboot/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the slot number of the bootable image
 *
 * @param[out]  riot_hdr    Copy of riotboot image header
 *
 * @retval  Slot number of the bootable image
 * @retval  -1 if none found
 */
int riotboot_bootloader_get_slot(riotboot_hdr_t *riot_hdr);

#ifdef __cplusplus
}
#endif
