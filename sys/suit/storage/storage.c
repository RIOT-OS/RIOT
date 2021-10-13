/*
 * Copyright (C) 2020 Koen Zandberg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_suit_storage
 * @{
 *
 * @file
 * @brief       SUIT storage backend helpers
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>
#include "kernel_defines.h"

#include "suit.h"
#include "suit/storage.h"

#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
#include "suit/storage/flashwrite.h"
extern suit_storage_flashwrite_t suit_storage_flashwrite;
#endif

#ifdef MODULE_SUIT_STORAGE_RAM
#include "suit/storage/ram.h"
extern suit_storage_ram_t suit_storage_ram;
#endif

static suit_storage_t *reg[] = {
#ifdef MODULE_SUIT_STORAGE_FLASHWRITE
    &suit_storage_flashwrite.storage,
#endif
#ifdef MODULE_SUIT_STORAGE_RAM
    &suit_storage_ram.storage,
#endif
};

static const size_t reg_size = ARRAY_SIZE(reg);

suit_storage_t *suit_storage_find_by_id(const char *id)
{
    for (size_t i = 0; i < reg_size; i++) {
        if (suit_storage_has_location(reg[i], id)) {
            return reg[i];
        }
    }
    return NULL;
}

void suit_storage_init_all(void)
{
    for (size_t i = 0; i < reg_size; i++) {
        suit_storage_init(reg[i]);
    }
}

suit_storage_t *suit_storage_find_by_component(const suit_manifest_t *manifest,
        const suit_component_t *component)
{
    for (size_t i = 0; i < reg_size; i++) {
        char name[CONFIG_SUIT_COMPONENT_MAX_NAME_LEN];
        if (suit_component_name_to_string(manifest, component,
                                          reg[i]->driver->separator,
                                          name, sizeof(name)) == SUIT_OK) {

            if (suit_storage_has_location(reg[i], name)) {
                return reg[i];
            }
        }
    }
    return NULL;
}

int suit_storage_get_highest_seq_no(uint32_t *seq_no)
{
    uint32_t max_seq = 0;
    int res = SUIT_ERR_STORAGE;

    for (size_t i = 0; i < reg_size; i++) {
        uint32_t seq_no = 0;
        if (suit_storage_get_seq_no(reg[i], &seq_no) == SUIT_OK) {
            res = SUIT_OK;
            if (seq_no > max_seq) {
                max_seq = seq_no;
            }
        }
    }
    *seq_no = max_seq;
    return res;
}

int suit_storage_set_seq_no_all(uint32_t seq_no)
{
    for (size_t i = 0; i < reg_size; i++) {
        suit_storage_set_seq_no(reg[i], seq_no);
    }
    return 0;
}
