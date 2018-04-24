/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_firmware
 * @{
 *
 * @file
 * @brief       Firmware module
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>

#ifdef RIOT_VERSION
#include "cpu.h"
#include "log.h"
#else
#include <stdio.h>
#define LOG_INFO(...) printf(__VA_ARGS__)
#endif

#include "firmware.h"
#include "checksum/fletcher32.h"

int firmware_validate_metadata_checksum(firmware_metadata_t *metadata)
{
    if (memcmp(metadata, "RIOT", 4)) {
        LOG_INFO("%s: metadata magic number invalid\n", __func__);
        return -1;
    }

    int res = firmware_metadata_checksum(metadata) == metadata->chksum ? 0 : -1;
    if (res) {
        LOG_INFO("%s: metadata checksum invalid\n", __func__);
    }

    return res;
}

uint32_t firmware_metadata_checksum(firmware_metadata_t *metadata)
{
    return fletcher32((uint16_t *)metadata, FIRMWARE_CHECKSUM_LEN / 2);
}

#ifdef RIOT_VERSION
#ifndef BOARD_NATIVE
static const unsigned _firmware_slot_start[] = {
    CPU_FLASH_BASE,
    CPU_FLASH_BASE + SLOT0_SIZE,
    CPU_FLASH_BASE + SLOT0_SIZE + SLOT1_SIZE
};

const unsigned firmware_num_slots = sizeof(_firmware_slot_start) / sizeof(unsigned);

void firmware_jump_to_image(firmware_metadata_t *metadata)
{
    cpu_jump_to_image(metadata->start_addr);
}

int firmware_current_slot(void)
{
    unsigned base_addr = cpu_get_image_baseaddr() - FIRMWARE_METADATA_SIZE;

    for (unsigned i = 1; i < firmware_num_slots; i++) {
        if (base_addr == _firmware_slot_start[i]) {
            return i;
        }
    }

    return -1;
}

int firmware_target_slot(void)
{
    /* TODO: find logic for more than one slot */
    return firmware_current_slot() == 1 ? 2 : 1;
}

firmware_metadata_t *firmware_get_metadata(unsigned slot)
{
    assert(slot < FIRMWARE_NUM_SLOTS);
    return (firmware_metadata_t *)_firmware_slot_start[slot];
}

unsigned firmware_get_image_startaddr(unsigned slot)
{
    assert(slot < FIRMWARE_NUM_SLOTS);
    return _firmware_slot_start[slot] + FIRMWARE_METADATA_SIZE;
}

void firmware_jump_to_slot(unsigned slot)
{
    firmware_jump_to_image(firmware_get_metadata(slot));
}

void firmware_dump_slot_addrs(void)
{
    for (unsigned i = 1; i < FIRMWARE_NUM_SLOTS; i++) {
        printf("slot %u: metadata: 0x%08x image: 0x%08x\n", i,
               (unsigned)_firmware_slot_start[i], firmware_get_image_startaddr(i));
    }
}
#else /* BOARD_NATIVE */
const unsigned firmware_num_slots = 1;

void firmware_jump_to_image(firmware_metadata_t *metadata)
{
    (void)metadata;
    printf("%s native stub\n", __func__);
}

int firmware_current_slot(void)
{
    printf("%s native stub\n", __func__);

    return 1;
}

firmware_metadata_t *firmware_get_metadata(unsigned slot)
{
    (void)slot;
    printf("%s native stub\n", __func__);
    return NULL;
}

unsigned firmware_get_image_startaddr(unsigned slot)
{
    (void)slot;
    printf("%s native stub\n", __func__);
    return 0;
}

void firmware_jump_to_slot(unsigned slot)
{
    (void)slot;
    printf("%s native stub\n", __func__);
}

void firmware_dump_slot_addrs(void)
{
    printf("%s native stub\n", __func__);
}
#endif
#endif /* RIOT_VERSION */
