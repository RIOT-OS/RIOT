/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef RIOTBOOT_FLASHWRITE_H
#define RIOTBOOT_FLASHWRITE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief mockup flashwrite context
 */
typedef struct {
    unsigned target_slot;   /**< Mockup slot */
} riotboot_flashwrite_t;

/**
 * @brief Mockup flashwrite initialization function
 *
 * @param   state       flashwrite state
 * @param   target_slot Target slot
 *
 * @return              Always returns 0
 */
static inline int riotboot_flashwrite_init(riotboot_flashwrite_t *state,
                                           int target_slot)
{
    (void)state;
    (void)target_slot;
    return 0;
}

static inline riotboot_flashwrite_verify_sha256(digest,
                                                img_size,
                                                target_slot) {
    (void)digest;
    (void)img_size;
    (void)target_slot;
    return 0;
}
#ifdef __cplusplus
}
#endif

#endif /* RIOTBOOT_FLASHWRITE_H */
