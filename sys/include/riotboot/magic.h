/*
 * Copyright (C) 2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef RIOTBOOT_MAGIC_H
#define RIOTBOOT_MAGIC_H

/**
 * @defgroup    sys_riotboot_magic     Magic values for riotboot
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       USB DFU/serial initialization constants for riotboot
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "riotboot/hdr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name USB RAM information for riotboot
 * @{
 */
#ifndef RIOTBOOT_MAGIC_ADDR
#define RIOTBOOT_MAGIC_ADDR   (CPU_RAM_BASE + CPU_RAM_SIZE - 4)   /**< default magic address */
#endif
#ifndef RIOTBOOT_MAGIC_NUMBER
#define RIOTBOOT_MAGIC_NUMBER  RIOTBOOT_MAGIC                     /**< default magic value */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* RIOTBOOT_MAGIC_H */
