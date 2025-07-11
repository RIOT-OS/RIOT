/*
 * Copyright (C) 2024 Université de Lille
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       An application demonstrating xipfs
 *
 * @author      Damien Amara <damien.amara@univ-lille.fr>
 * @author      Gregory Guche <gregory.guche@univ-lille.fr>
 *
 * @}
 */

#include <fcntl.h>
#include <stdlib.h>

#include "fs/xipfs_fs.h"
#include "periph/flashpage.h"
#include "shell.h"
#include "vfs.h"

/**
 * @def PANIC
 *
 * @brief This macro handles fatal errors
 */
#define PANIC() for (;;) {}

/**
 * @def NVME0P0_PAGE_NUM
 *
 * @brief The number of flash page for the nvme0p0 file system
 */
#define NVME0P0_PAGE_NUM 10

/**
 * @def NVME0P1_PAGE_NUM
 *
 * @brief The number of flash page for the nvme0p1 file system
 */
#define NVME0P1_PAGE_NUM 15

/*
 * Allocate a new contiguous space for the nvme0p0 file system
 */
XIPFS_NEW_PARTITION(nvme0p0, "/dev/nvme0p0", NVME0P0_PAGE_NUM);

/*
 * Allocate a new contiguous space for the nvme0p1 file system
 */
XIPFS_NEW_PARTITION(nvme0p1, "/dev/nvme0p1", NVME0P1_PAGE_NUM);

#ifdef BOARD_DWM1001

/**
 * @brief hello-world.fae data blob.
 *
 * To create a *.fae file, you will need to clone the master branch of
 * xipfs_format, that can be found at https://github.com/2xs/XiPFS_Format .
 *
 * Then modify the Makefile to suit your needs/sources and call make.
 * You should end up with a *.fae file ready to be uploaded.
 */
#include "blob/hello-world.fae.h"

#define FILENAME_OF_HELLO_WORLD_FAE  "/dev/nvme0p0/hello-world.fae"
#define SIZEOF_HELLO_WORLD_FAE       (sizeof(hello_world_fae) / sizeof(hello_world_fae[0]))

/**
 * @brief Execution in-place demonstrator.
 *
 * This shell command handler will create a file hello-world.fae on /dev/nvme0p0,
 * if none exists yet from the data blob above.
 * Then, it will execute this file.
 *
 * Once the file has been created, execute command can be used to rerun
 * the executable file as many times as wanted.
 *
 * ```bash
 * 2025-01-14 09:48:36,303 # main(): This is RIOT! (Version: 2024.10)
 * 2025-01-14 09:48:36,307 # vfs_mount: "/dev/nvme0p0": OK
 * 2025-01-14 09:48:36,313 # vfs_mount: "/dev/nvme0p1": OK
 * > help
 * 2025-01-14 09:48:42,300 # help
 * 2025-01-14 09:48:42,302 # Command              Description
 * 2025-01-14 09:48:42,305 # ---------------------------------------
 * 2025-01-14 09:48:42,309 # exec                 Execute Hello World
 * 2025-01-14 09:48:42,314 # create_executable    Create an XIPFS executable file
 * 2025-01-14 09:48:42,317 # execute              Execute an XIPFS file
 * 2025-01-14 09:48:42,320 # ls                   list files
 * 2025-01-14 09:48:42,325 # pm                   interact with layered PM subsystem
 * 2025-01-14 09:48:42,331 # ps                   Prints information about running threads.
 * 2025-01-14 09:48:42,334 # reboot               Reboot the node
 * 2025-01-14 09:48:42,338 # version              Prints current RIOT_VERSION
 * 2025-01-14 09:48:42,343 # vfs                  virtual file system operations
 * > exec
 * 2025-01-14 09:48:49,572 # exec
 * 2025-01-14 09:48:49,573 # Hello World!
 * > ls /dev/nvme0p0
 * 2025-01-14 09:48:59,997 # ls /dev/nvme0p0
 * 2025-01-14 09:48:59,999 # hello-world.fae       896 B
 * 2025-01-14 09:49:00,000 # total 1 files
 * > vfs df
 * 2025-01-14 09:49:04,957 # vfs df
 * 2025-01-14 09:49:04,962 # Mountpoint              Total         Used    Available     Use%
 * 2025-01-14 09:49:04,968 # /dev/nvme0p0           40 KiB        4 KiB       36 KiB      10%
 * 2025-01-14 09:49:04,974 # /dev/nvme0p1           60 KiB          0 B       60 KiB       0%
 * execute /dev/nvme0p0/hello-world.fae ipsum dolores it
 * 2025-01-14 09:49:14,223 # execute /dev/nvme0p0/hello-world.fae Lorem ipsum dolor sit amet
 * 2025-01-14 09:49:14,225 # Hello World!
 * 2025-01-14 09:49:14,225 # Lorem
 * 2025-01-14 09:49:14,226 # ipsum
 * 2025-01-14 09:49:14,226 # dolor
 * 2025-01-14 09:49:14,227 # sit
 * 2025-01-14 09:49:14,227 # amet
 * ```
 */
int execution_handler(int argc, char **argv) {
    (void)argc;
    (void)argv;

    int file_handle = vfs_open(FILENAME_OF_HELLO_WORLD_FAE, O_RDONLY, 0);
    if (file_handle < 0) {

        /** There's no executable file yet, let's drop one */
        int ret = xipfs_extended_driver_new_file(
            FILENAME_OF_HELLO_WORLD_FAE, SIZEOF_HELLO_WORLD_FAE, 1
        );
        if (ret < 0) {
            printf("xipfs_extended_driver_new_file : failed to create '%s' : error=%d\n",
                   FILENAME_OF_HELLO_WORLD_FAE, ret);
            return EXIT_FAILURE;
        }

        /**
         * Fill it with blob data
         * Take care : vfs does not support O_APPEND with vfs_write, only O_WRONLY or O_RDWR
         */
        file_handle = vfs_open(FILENAME_OF_HELLO_WORLD_FAE, O_WRONLY, 0);
        if (file_handle < 0) {
            printf("vfs_open : failed to open '%s' : error =%d\n",
                   FILENAME_OF_HELLO_WORLD_FAE, file_handle);
            return EXIT_FAILURE;
        }

        ssize_t write_ret = vfs_write(file_handle, hello_world_fae, SIZEOF_HELLO_WORLD_FAE);
        if (write_ret < 0) {
            printf("vfs_write : failed to fill '%s' : error=%d\n",
                   FILENAME_OF_HELLO_WORLD_FAE, write_ret);
            vfs_close(file_handle);
            return EXIT_FAILURE;
        }
    }

    vfs_close(file_handle);

    char *exec_argv[] = {
        FILENAME_OF_HELLO_WORLD_FAE,
        NULL
    };

    int ret = xipfs_extended_driver_execv(FILENAME_OF_HELLO_WORLD_FAE, exec_argv);
    if (ret < 0) {
        printf("Failed to execute '%s' : error=%d\n", FILENAME_OF_HELLO_WORLD_FAE, ret);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static shell_command_t shell_commands[] = {
    {"exec", "Execute Hello World", execution_handler},
    {NULL, NULL, NULL},
};

#else /* BOARD_DWM1001 */

static shell_command_t shell_commands[] = { {NULL, NULL, NULL} };

#endif /* BOARD_DWM1001 */

/**
 * @internal
 *
 * @brief Mount a partition, or if it is corrupted, format and
 * remount it
 *
 * @param xipfs_mp A pointer to a memory region containing an
 * xipfs mount point structure
 */
static void mount_or_format(vfs_xipfs_mount_t *xipfs_mp)
{
    if (vfs_mount(&xipfs_mp->vfs_mp) < 0) {
        printf("vfs_mount: \"%s\": file system has not been "
            "initialized or is corrupted\n", xipfs_mp->vfs_mp.mount_point);
        printf("vfs_format: \"%s\": try initializing it\n",
            xipfs_mp->vfs_mp.mount_point);
        vfs_format(&xipfs_mp->vfs_mp);
        printf("vfs_format: \"%s\": OK\n", xipfs_mp->vfs_mp.mount_point);
        if (vfs_mount(&xipfs_mp->vfs_mp) < 0) {
            printf("vfs_mount: \"%s\": file system is corrupted!\n",
                xipfs_mp->vfs_mp.mount_point);
            PANIC();
        }
    }
    printf("vfs_mount: \"%s\": OK\n", xipfs_mp->vfs_mp.mount_point);
}

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    mount_or_format(&nvme0p0);
    mount_or_format(&nvme0p1);

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
