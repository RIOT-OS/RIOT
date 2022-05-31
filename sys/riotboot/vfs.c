/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotboot_vfs
 * @{
 *
 * @file
 * @brief       VFS Bootloader helpers
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <string.h>

#include "architecture.h"
#include "vfs_util.h"
#include "periph/flashpage.h"
#include "riotboot/vfs.h"

/* try to guess the end of the firmware */
static const void *_first_free_chunk(const void *start)
{
    const uword_t *cur = start;
    const uword_t *end = flashpage_addr(FLASHPAGE_NUMOF);
    uword_t empty;
    unsigned const chunk_size = 256;
    memset(&empty, FLASHPAGE_ERASE_STATE, sizeof(empty));

    /* try to find the first unused page */
    while (cur < end) {
        if (cur[0] == empty && cur[1] == empty &&
            cur[2] == empty && cur[3] == empty) {
            break;
        }
        cur += chunk_size;
    }

    return cur;
}

int riotboot_vfs_dump_rom(const char *file)
{
    const void *start, *end;

    if (IS_ACTIVE(RIOTBOOT)) {
        start = (void *)(uintptr_t)SLOT0_OFFSET;
        end   = _first_free_chunk(start);
    } else {
        start = (void *)cpu_get_image_baseaddr();
        end   = flashpage_addr(flashpage_first_free());
    }

    return vfs_file_from_buffer(file, start, end - start);
}

void riotboot_vfs_cancel(void)
{
#ifdef MODULE_VFS
    vfs_unlink(RIOTBOOT_VFS_UPDATE_FILE);
#endif
}
