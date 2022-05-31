/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     bootloaders
 * @{
 *
 * @file
 * @brief       Bootloader to load firmware from external storage via VFS
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <fcntl.h>
#include "cpu.h"
#include "panic.h"
#include "riotboot/slot.h"
#include "riotboot/vfs.h"
#include "vfs.h"
#include "vfs_util.h"
#include "periph/flashpage.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef FLASHPAGE_SIZE
#define FLASHPAGE_SIZE  1024
#endif

static void _flash_image(const char *file)
{
    static uint8_t buffer[FLASHPAGE_SIZE];
    uint8_t *dst = (void *)(uintptr_t)SLOT0_OFFSET;
    int res, fd = vfs_open(file, O_RDONLY, 0);

    DEBUG("flashing %s…\n", file);

    if (fd < 0) {
        DEBUG("can't open: %d\n", fd);
        return;
    }

    /* get image size */
    off_t size = vfs_lseek(fd, 0, SEEK_END);
    unsigned last_page = flashpage_page((void *)(uintptr_t)(SLOT0_OFFSET + size));

    /* backup old image, close new image to conserve fds */
    vfs_close(fd);
    if (riotboot_vfs_dump_rom(RIOTBOOT_VFS_BACKUP_FILE) <= 0) {
        DEBUG("can't backup current ROM");
        return;
    }

    /* new firmware has to remove the update file */
    vfs_file_from_buffer(RIOTBOOT_VFS_UPDATE_FILE,
                         RIOTBOOT_VFS_BACKUP_FILE, sizeof(RIOTBOOT_VFS_BACKUP_FILE));

    /* clear pages */
    for (unsigned i = flashpage_page((void *)(uintptr_t)SLOT0_OFFSET); i <= last_page; ++i) {
        DEBUG("erase page %u\n", i);
        flashpage_erase(i);
    }

    DEBUG("write %lu bytes\n", size);
    fd = vfs_open(file, O_RDONLY, 0);

    /* flash image */
    do {
        res = vfs_read(fd, buffer, sizeof(buffer));
        flashpage_write(dst, buffer, res);
        dst += res;
    } while (res > 0);

    DEBUG_PUTS("done.");

    vfs_close(fd);
}

void kernel_init(void)
{
    DEBUG_PUTS("riotboot VFS");

    extern void auto_init(void);
    auto_init();

    /* get file to flash */
    char update_file[64];
    if (vfs_file_to_buffer(RIOTBOOT_VFS_UPDATE_FILE, update_file, sizeof(update_file)) > 0) {
        _flash_image(update_file);
    }
    DEBUG("jump to 0x%x\n", SLOT0_OFFSET);
    cpu_jump_to_image(SLOT0_OFFSET);

    /* serious trouble! nothing to boot */
    while (1) {}
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
