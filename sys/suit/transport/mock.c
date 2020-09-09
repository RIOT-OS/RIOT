/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "kernel_defines.h"
#include "cpu_conf.h"

#include "riotboot/flashwrite.h"
#include "riotboot/hdr.h"

#define SLOT0_OFFSET 0x1000
#define SLOT1_OFFSET 0x2000

static riotboot_hdr_t _riotboot_slots[] = {
    { .magic_number = RIOTBOOT_MAGIC,
      .version = 1,
      .start_addr=0x100000,
    },
    { .magic_number = RIOTBOOT_MAGIC,
      .version = 2,
      .start_addr=0x200000,
    },
};

const riotboot_hdr_t * const riotboot_slots[] = {
    &_riotboot_slots[0],
    &_riotboot_slots[1],
};

const unsigned riotboot_slot_numof = ARRAY_SIZE(riotboot_slots);

static int _current_slot;

int riotboot_slot_current(void)
{
    return _current_slot;
}

int riotboot_slot_other(void)
{
    return (_current_slot == 0) ? 1 : 0;
}

const riotboot_hdr_t *riotboot_slot_get_hdr(unsigned slot)
{
    assert(slot < riotboot_slot_numof);

    return riotboot_slots[slot];
}

size_t riotboot_slot_offset(unsigned slot)
{
    return (slot == 0) ? SLOT0_OFFSET : SLOT1_OFFSET;
}

int riotboot_flashwrite_init_raw(riotboot_flashwrite_t *state, int target_slot,
                                 size_t offset)
{
    (void)state;
    (void)target_slot;
    (void)offset;
    puts("riotboot_flashwrite_init_raw() empty mock");
    return 0;
}

int riotboot_flashwrite_verify_sha256(const uint8_t *sha256_digest,
                                      size_t img_size, int target_slot)
{
    (void)sha256_digest;
    (void)img_size;
    (void)target_slot;
    return 0;
}
