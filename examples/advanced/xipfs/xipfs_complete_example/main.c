/*
 * SPDX-FileCopyrightText: 2024-2026 Université de Lille
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       An application demonstrating xipfs usage from code.
 *
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

#define PANIC() for (;;) {} /**< This macro handles fatal errors */

/*
 * Include a mount point image that has been built on a PC workstation.
 *
 * Please mind the deliberate missing semicolon character after
 * XIPFS_START_PARTITION_INCLUSION(nvme0p0).
 */
static
XIPFS_START_PARTITION_INCLUSION(nvme0p0)
#include "blob/nvme0p0.flash.h"
XIPFS_END_PARTITION_INCLUSION(nvme0p0, "/nvme0p0", nvme0p0_flash, nvme0p0_flash_len);

#define NVME0P1_PAGE_NUM 10 /**< The number of flash page for the nvme0p1 file system. */

/* Allocate a new contiguous space for the nvme0p1 file system. */
XIPFS_NEW_PARTITION(nvme0p1, "/nvme0p1", NVME0P1_PAGE_NUM);

static shell_command_t shell_commands[] = { {NULL, NULL, NULL} };

/**
 * @internal
 *
 * @brief Initialize a VFS XiPFS mountpoint.
 *
 * @param[in]   mp The mountpoint to initialize.
 *
 * @retval <0 on errors
 * @retval >=0 otherwise
 */
static int init_mount_point(vfs_xipfs_mount_t *mp)
{
    if (vfs_mount(&mp->vfs_mp) < 0) {
        printf("Error: vfs_mount: \"%s\": file system has not been "
            "initialized or is corrupted\n", mp->vfs_mp.mount_point);
        return -1;
    }

    printf("vfs_mount: \"%s\": OK\n", mp->vfs_mp.mount_point);
    return 0;
}

/**
 * @internal
 *
 * @brief Initialize all example mountpoints.
 */
static void init_mount_points(void)
{
    vfs_xipfs_mount_t *mps[2] = {
        [0] = &nvme0p0,
        [1] = &nvme0p1,
    };
    const size_t mps_count = ARRAY_SIZE(mps);
    for (size_t i = 0; i < mps_count; i++) {
        if (init_mount_point(mps[i]) < 0) {
            PANIC();
        }
    }
}

/**
 * @internal
 *
 * @brief XiPFS file copier.
 *
 * XiPFS requires to know both filesize and execution permission when creating
 * a file, which are not reported when using vfs_copy.
 *
 * @param[in] src_filename          The source filename to be copied.
 * @param[in] dst_filename          The destination filename.
 * @param[in] is_executable_file    File execution permission.
 *
 * @remark This function can not be inlined, as it would otherwise allocate a
 *         calculated maximum amount of bytes on stack, and would never
 *         release it when calling shell_run.
 *         This could lead to stack overflow/exhaustion in later shell
 *         commands.
 */
static __attribute__((noinline))
void copy_file(const char *src_filename, const char *dst_filename,
               uint32_t is_executable_file)
{
    struct stat src_stat;
    if (vfs_stat(src_filename, &src_stat) <  0) {
        printf("Error: failed to retrieve stats for file '%s'.\n", src_filename);
        PANIC();
    }

    int src_fd = vfs_open(src_filename, O_RDONLY, 0);
    if (src_fd < 0) {
        printf("Error: failed to open file '%s'.\n", src_filename);
        PANIC();
    }

    if (xipfs_extended_driver_new_file(dst_filename, src_stat.st_size,
                                       is_executable_file) < 0) {
        printf("Error: failed to create new file '%s'.\n", dst_filename);
        PANIC();
    }

    int dst_fd = vfs_open(dst_filename, O_WRONLY, 0);
    if (dst_fd < 0) {
        printf("Error: failed to open file '%s'.\n", dst_filename);
        PANIC();
    }

    int read_bytes_count, write_bytes_count, pos;
    char buffer[256];
    while (true) {
        read_bytes_count = vfs_read(src_fd, buffer, sizeof(buffer));
        if (read_bytes_count < 0) {
            printf("Error: failed to read bytes from file '%s'.\n", src_filename);
            PANIC();
        }
        if (read_bytes_count == 0) {
            /* EOF */
            break;
        }

        pos = 0;
        while (read_bytes_count > 0) {
            write_bytes_count = vfs_write(dst_fd, buffer + pos,
                                          read_bytes_count);
            if (write_bytes_count <= 0) {
                printf("Error: failed to write bytes to file '%s'.\n", dst_filename);
                PANIC();
            }
            read_bytes_count -= write_bytes_count;
            pos += write_bytes_count;
        }
    }

    (void)vfs_close(src_fd);
    (void)vfs_close(dst_fd);

    printf("%s '%s' file has been copied to '%s'\n",
           (is_executable_file) ? "Executable" : "",
           src_filename, dst_filename);
}

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    /* Mount both included and allocated filesystems. */
    init_mount_points();

    printf("\n");

    /* Create "/nvme0p1/bin" directory */
    const char *nvme0p1_bin_directory_name = "/nvme0p1/bin";
    if (vfs_mkdir(nvme0p1_bin_directory_name, 0) < 0) {
        printf("Error: failed to create '%s' directory.\n", nvme0p1_bin_directory_name);
        PANIC();
    }

    const char *src_filename = "/nvme0p0/21.fae";
    const char *dst_filename = "/nvme0p1/bin/twenty-one.fae";

    /* Copy "/nvme0p0/21.fae" to "/nvme0p1/bin/twenty-one.fae". */
    copy_file(src_filename, dst_filename, 1);

    /* Execute both files, sum up their result and check if it's equal to expected one. */
    /* Execution WITHOUT memory protection. */
    char *args[] = { (char *)src_filename, NULL };
    int global_result = xipfs_extended_driver_execv(src_filename, args);
    if (global_result < 0) {
        printf("Error: failed to execute file '%s'.\n", src_filename);
        PANIC();
    }
    printf("'%s' has been run and returned '%d'.\n", src_filename, global_result);

    /* Execution WITH memory protection enabled. */
    args[0] = (char *)dst_filename;
    int local_result = xipfs_extended_driver_safe_execv(dst_filename, args);
    if (local_result < 0) {
        printf("Error: failed to execute file '%s'.\n", dst_filename);
        PANIC();
    }
    printf("'%s' has been run and returned '%d'.\n", dst_filename, local_result);

    /* Accumulate results and check */
    global_result += local_result;
    if (global_result != 42) {
        printf("Error: the cumulated result of '%s' and '%s'"
                " executions differs from expected one (%d VS %d).\n",
                src_filename, dst_filename, global_result, 42);
        PANIC();
    }
    printf("The cumulated returned value of '%s' and '%s' is equal to %d.\n",
            src_filename, dst_filename, 42);

    /* Remove "/nvme0p1/bin/twenty-one.fae" */
    if (vfs_unlink(dst_filename) < 0) {
        printf("Error: Failed to delete '%s' file.\n", dst_filename);
        PANIC();
    }
    printf("'%s' file has been erased.\n", dst_filename);

    /* Remove "/nvme0p1/bin" */
    if (vfs_rmdir(nvme0p1_bin_directory_name) < 0) {
        printf("Error: Failed to delete '%s' directory.\n", nvme0p1_bin_directory_name);
        PANIC();
    }
    printf("'%s' directory has been removed.\n", nvme0p1_bin_directory_name);

    const char *memory_dumper_filename = "/nvme0p0/dumper.fae";

    printf("\nXiPFS is now going to run '%s' with different arguments, "
            "guarded by the hardware MPU.\n\n", memory_dumper_filename);

    const char *casename = "Illegal attempt to read values from RAM";
    char *dumper_args[] = { (char *)memory_dumper_filename, "non-legit-ram", NULL };
    printf("%s in progress...\n", casename);
    if (xipfs_extended_driver_safe_execv(memory_dumper_filename, dumper_args) >= 0) {
        printf("Error: '%s' has succeeded, but it should have failed.\n", casename);
        PANIC();
    }
    printf("%s has failed as expected.\n\n", casename);

    casename = "Legal attempt to read values from RAM";
    dumper_args[1] = "legit-ram";
    printf("%s in progress...\n", casename);
    if (xipfs_extended_driver_safe_execv(memory_dumper_filename, dumper_args) < 0) {
        printf("Error: '%s' has failed, but it should have succeeded.\n", casename);
        PANIC();
    }
    printf("%s has succeeded as expected.\n\n", casename);

    casename = "Illegal attempt to read values from ROM";
    dumper_args[1] = "non-legit-rom";
    printf("%s in progress...\n", casename);
    if (xipfs_extended_driver_safe_execv(memory_dumper_filename, dumper_args) >= 0) {
        printf("Error: '%s' has succeeded, but it should have failed.\n", casename);
        PANIC();
    }
    printf("%s has failed as expected.\n\n", casename);

    casename = "Legal attempt to read values from ROM";
    dumper_args[1] = "legit-rom";
    printf("%s in progress...\n", casename);
    if (xipfs_extended_driver_safe_execv(memory_dumper_filename, dumper_args) < 0) {
        printf("Error: '%s' has failed, but it should have succeeded.\n", casename);
        PANIC();
    }
    printf("%s has succeeded as expected.\n\n", casename);

    printf("Now, you can enter VFS shell commands to manage '/nvme0p0' and "
           "'/nvme0p1' mount points.\n");
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
