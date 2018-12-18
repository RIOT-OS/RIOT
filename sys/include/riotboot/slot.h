/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_riotboot_slot   Helpers to manipulate partitions (slots)
 *                              on internal flash
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       Slot management tools
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#ifndef RIOTBOOT_SLOT_H
#define RIOTBOOT_SLOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "riotboot/hdr.h"

/**
 * @brief  Get currently running image slot
 *
 * @returns nr of currently active slot
 */
int riotboot_slot_current(void);

/**
 * @brief  Get jump-to address of image slot
 *
 * @param[in]   slot    slot nr to work on
 *
 * @returns address of first byte of @p slot
 */
uint32_t riotboot_slot_get_image_startaddr(unsigned slot);

/**
 * @brief  Boot into image in slot @p slot
 *
 * @param[in]   slot    slot nr to jump to
 */
void riotboot_slot_jump(unsigned slot);

/**
 * @brief  Get header from a given flash slot
 *
 * @param[in]   slot    slot nr to work on
 *
 * @returns header of image slot nr @p slot
 */
const riotboot_hdr_t *riotboot_slot_get_hdr(unsigned slot);

/**
 * @brief  Validate slot
 *
 * @param[in] slot    slot nr to work on
 *
 * @returns 0 if ok.
 */
static inline int riotboot_slot_validate(unsigned slot)
{
    return riotboot_hdr_validate(riotboot_slot_get_hdr(slot));
}

/**
 * @brief  Print formatted slot header to STDIO
 *
 * @param[in]   slot    slot nr to work on
 *
 */
static inline void riotboot_slot_print_hdr(unsigned slot)
{
    riotboot_hdr_print(riotboot_slot_get_hdr(slot));
}

/**
 * @brief  Dump the addresses of all configured slots
 *
 */
void riotboot_slot_dump_addrs(void);

/**
 * @brief   Number of configured firmware slots (incl. bootloader slot)
 */
extern const unsigned riotboot_slot_numof;

/**
 * @brief   Storage for header pointers of the configured slots
 */
extern const riotboot_hdr_t * const riotboot_slots[];

#ifdef __cplusplus
}
#endif

#endif /* RIOTBOOT_SLOT_H */
