/*
 * SPDX-FileCopyrightText: 2024 Université de Lille
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       An application for testing xipfs
 *
 * @author      Damien Amara <damien.amara@univ-lille.fr>
 * @author      Gregory Guche <gregory.guche@univ-lille.fr>
 * @}
 */

#include <errno.h>

#include "fs/xipfs_fs.h"
#include "periph/flashpage.h"
#include "shell.h"
#include "vfs.h"
#include  "mtd_flashpage.h"
#include "include/xipfs.h"
#include "include/flash.h"
#include "cpu.h"

#include "blob/minimal.fae.h"

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
#define NVME0P0_PAGE_NUM 20

#define XIPFS_ASSERT(condition)       \
do {                                  \
    if (!(condition)) {               \
        printf("Line %d ", __LINE__); \
    }                                 \
    assert((condition));              \
} while (0)

/*
 * Allocate a new contiguous space for the nvme0p0 file system
 */
XIPFS_NEW_PARTITION(nvme0p0, "/dev/nvme0p0", NVME0P0_PAGE_NUM);

/*
 * Get a pointer to an xipfs_mount_t from a vfs_xipfs_mount_t
 */
xipfs_mount_t *xipfs_nvme0p0 = NULL;

/*
 * Test function prototypes
 */
static void test_xipfs_flash_memory(void);

static void test_xipfs_format_invalid_cases(void);
static void test_xipfs_mount_invalid_cases(void);

static void test_xipfs_close_ebadf_descp(void);
static void test_xipfs_close_ok(void);
static void test_xipfs_fstat_ebadbf_descp(void);
static void test_xipfs_fstat_efault_buf(void);
static void test_xipfs_fstat_ebadbf_xipfs_infos(void);
static void test_xipfs_fstat_ok(void);
static void test_xipfs_lseek_einval_descp(void);
static void test_xipfs_lseek_ebadf_descp(void);
static void test_xipfs_lseek_einval_whence(void);
static void test_xipfs_lseek_einval_seek_set_negative_pos(void);
static void test_xipfs_lseek_einval_seek_set_pos_past_max_pos(void);
static void test_xipfs_lseek_einval_seek_cur_negative_pos_out_of_file(void);
static void test_xipfs_lseek_einval_seek_cur_positive_pos_out_of_file(void);
static void test_xipfs_lseek_einval_seek_end_positive_pos(void);
static void test_xipfs_lseek_einval_seek_end_negative_pos_out_of_file(void);
static void test_xipfs_lseek_ok_seek_set_empty_file_pos_0(void);
static void test_xipfs_lseek_ok_seek_cur_empty_file_pos_0(void);
static void test_xipfs_lseek_ok_seek_end_empty_file_pos_0(void);
static void test_xipfs_lseek_ok_seek_set_pos_0(void);
static void test_xipfs_lseek_ok_seek_cur_pos_0(void);
static void test_xipfs_lseek_ok_seek_end_pos_0(void);
static void test_xipfs_lseek_ok_seek_set_pos_1(void);
static void test_xipfs_lseek_ok_seek_cur_pos_1(void);
static void test_xipfs_lseek_ok_seek_cur_pos_minus_1(void);
static void test_xipfs_lseek_ok_seek_end_pos_minus_3(void);
static void test_xipfs_open_efault_name(void);
static void test_xipfs_open_einval_flags(void);
static void test_xipfs_open_enametoolong_name(void);
static void test_xipfs_open_eexist_xipfs_infos_flags(void);
static void test_xipfs_open_eacces_xipfs_infos_flags(void);
static void test_xipfs_open_eexist_name(void);
static void test_xipfs_open_eisdir_name(void);
static void test_xipfs_open_enotdir_name(void);
static void test_xipfs_open_enoent_flags(void);
static void test_xipfs_open_eisdir_name_trailing_slash(void);
static void test_xipfs_open_edquot(void);
static void test_xipfs_open_ok(void);
static void test_xipfs_read_ebadf_descp(void);
static void test_xipfs_read_efault_dest(void);
static void test_xipfs_read_eacces_flags(void);
static void test_xipfs_read_eio_empty_file(void);
static void test_xipfs_write_ebadf_descp(void);
static void test_xipfs_write_efault_src(void);
static void test_xipfs_write_eacces_flags(void);
static void test_xipfs_write_ok(void);
static void test_xipfs_write_consistency(void);
static void test_xipfs_write_page_change(void);
static void test_xipfs_write_io_buffer(void);
static void test_xipfs_write_last_page_of_filesystem(void);
static void test_xipfs_fsync_ebadf_descp(void);
static void test_xipfs_fsync_eacces_flags(void);
static void test_xipfs_fsync_ok(void);
static void test_xipfs_opendir_efault_dirname(void);
static void test_xipfs_opendir_enoent_dirname_null_char(void);
static void test_xipfs_opendir_enametoolong_dirname(void);
static void test_xipfs_opendir_enotdir_dirname(void);
static void test_xipfs_opendir_enoent_dirname(void);
static void test_xipfs_opendir_ok(void);
static void test_xipfs_readdir_efault_direntp(void);
static void test_xipfs_readdir_ebadf_descp(void);
static void test_xipfs_readdir_ok(void);
static void test_xipfs_closedir_ebadf_descp(void);
static void test_xipfs_closedir_ok(void);
static void test_xipfs_unlink_efault_name(void);
static void test_xipfs_unlink_enoent_name_null_char(void);
static void test_xipfs_unlink_eisdir_name_root(void);
static void test_xipfs_unlink_enametoolong_name(void);
static void test_xipfs_unlink_eisdir_name(void);
static void test_xipfs_unlink_enotdir_name(void);
static void test_xipfs_unlink_enoent_name(void);
static void test_xipfs_unlink_ok(void);
static void test_xipfs_mkdir_efault_name(void);
static void test_xipfs_mkdir_enoent_name_null_char(void);
static void test_xipfs_mkdir_eexist_name_root(void);
static void test_xipfs_mkdir_enametoolong_name(void);
static void test_xipfs_mkdir_eexist_name(void);
static void test_xipfs_mkdir_enotdir_name(void);
static void test_xipfs_mkdir_enoent_name(void);
static void test_xipfs_mkdir_enametoolong_name_trailing_slash(void);
static void test_xipfs_mkdir_ok(void);
static void test_xipfs_rmdir_efault_name(void);
static void test_xipfs_rmdir_enoent_name_null_char(void);
static void test_xipfs_rmdir_ebusy_name_root(void);
static void test_xipfs_rmdir_enametoolong_name(void);
static void test_xipfs_rmdir_einval_name(void);
static void test_xipfs_rmdir_enotdir_name(void);
static void test_xipfs_rmdir_enotempty_name(void);
static void test_xipfs_rmdir_enoent_name(void);
static void test_xipfs_rmdir_ok(void);
static void test_xipfs_rename_efault_from_path(void);
static void test_xipfs_rename_efault_to_path(void);
static void test_xipfs_rename_enoent_from_path_null_char(void);
static void test_xipfs_rename_enoent_to_path_null_char(void);
static void test_xipfs_rename_enametoolong_from_path(void);
static void test_xipfs_rename_enametoolong_to_path(void);
static void test_xipfs_rename_eisdir_from_path_file_to_path_dirs(void);
static void test_xipfs_rename_enotdir_from_path_file_to_path_not_dirs(void);
static void test_xipfs_rename_enoent_from_path_file_to_path_not_found(void);
static void test_xipfs_rename_enotdir_from_path_file_to_path_creatable_trailing_slash(void);
static void test_xipfs_rename_enotdir_from_path_empty_dir_to_path_file(void);
static void test_xipfs_rename_enotempty_from_path_empty_dir_to_path_nonempty_dir(void);
static void test_xipfs_rename_enotdir_from_path_empty_dir_to_path_not_dirs(void);
static void test_xipfs_rename_enoent_from_path_empty_dir_to_path_not_found(void);
static void test_xipfs_rename_enametoolong_from_path_empty_dir_to_path_trailing_slash(void);
static void test_xipfs_rename_einval_from_path_empty_dir_to_path_creatable_subdir_itself(void);
static void test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_file(void);
static void test_xipfs_rename_einval_from_path_nonempty_dir_to_path_empty_dir_subdir_itself(void);
static void test_xipfs_rename_enotempty_from_path_nonempty_dir_to_path_nonempty_dir(void);
static void test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_not_dirs(void);
static void test_xipfs_rename_enoent_from_path_nonempty_dir_to_path_not_found(void);
static void test_xipfs_rename_enametoolong_from_path_nonempty_dir_to_path_trailing_slash(void);
static void test_xipfs_rename_enotdir_from_path_not_dirs(void);
static void test_xipfs_rename_enoent_from_path_not_found(void);
static void test_xipfs_rename_ok(void);
static void test_xipfs_stat_efault_path(void);
static void test_xipfs_stat_efault_buf(void);
static void test_xipfs_stat_enoent_path_null_char(void);
static void test_xipfs_stat_enametoolong_path(void);
static void test_xipfs_stat_enotdir_path(void);
static void test_xipfs_stat_enoent_path(void);
static void test_xipfs_stat_ok(void);
static void test_xipfs_statvfs_efault_buf(void);
static void test_xipfs_statvfs_ok(void);
static void test_xipfs_new_file_efault_path(void);
static void test_xipfs_new_file_enoent_path_null_char(void);
static void test_xipfs_new_file_eisdir_path_root(void);
static void test_xipfs_new_file_enametoolong_path(void);
static void test_xipfs_new_file_einval_exec(void);
static void test_xipfs_new_file_einval_negative_size(void);
static void test_xipfs_new_file_eexist_path(void);
static void test_xipfs_new_file_eisdir_path(void);
static void test_xipfs_new_file_enotdir_path(void);
static void test_xipfs_new_file_enoent_path(void);
static void test_xipfs_new_file_eisdir_trailing_slash(void);
static void test_xipfs_new_file_edquot(void);
static void test_xipfs_new_file_ok(void);
static void test_xipfs_execv_efault_path(void);
static void test_xipfs_execv_enoent_path_null_char(void);
static void test_xipfs_execv_eisdir_path_root(void);
static void test_xipfs_execv_enametoolong_path(void);
static void test_xipfs_execv_efault_no_syscalls_table(void);
static void test_xipfs_execv_efault_syscalls_table_with_null(void);
static void test_xipfs_execv_eisdir_path(void);
static void test_xipfs_execv_enotdir_path(void);
static void test_xipfs_execv_enoent_path(void);
static void test_xipfs_execv_efault_null_args(void);
static void test_xipfs_execv_efault_args_0_null(void);
static void test_xipfs_extended_driver_execv(void);
#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT

#  ifndef __MPU_PRESENT
#    error "No MPU present"
#  endif

static void test_xipfs_safe_execv_efault_path(void);
static void test_xipfs_safe_execv_enoent_path_null_char(void);
static void test_xipfs_safe_execv_eisdir_path_root(void);
static void test_xipfs_safe_execv_enametoolong_path(void);
static void test_xipfs_safe_execv_efault_no_syscalls_table(void);
static void test_xipfs_safe_execv_efault_syscalls_table_with_null(void);
static void test_xipfs_safe_execv_eisdir_path(void);
static void test_xipfs_safe_execv_enotdir_path(void);
static void test_xipfs_safe_execv_enoent_path(void);
static void test_xipfs_safe_execv_efault_null_args(void);
static void test_xipfs_safe_execv_efault_args_0_null(void);
static void test_xipfs_extended_driver_safe_execv(void);

#endif /* XIPFS_ENABLE_SAFE_EXEC_SUPPORT */

void test_xipfs_suite(vfs_xipfs_mount_t *vfs_xipfs_mount) {
    int ret;

    printf( "Tests started with vfs_xipfs_mount %p, "
            "awaiting for normal termination or assertion...\n",
           vfs_xipfs_mount);

    xipfs_nvme0p0 = (xipfs_mount_t *)&(vfs_xipfs_mount->magic);

    /* Should not fail unless there is a corrupted data
     * structure or a flash memory failure. */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    /* Should not fail unless there is a corrupted data
     * structure or a flash memory failure. */
    ret = xipfs_mount(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    /* xipfs_close */
    test_xipfs_close_ebadf_descp();
    test_xipfs_close_ok();

    /* xipfs_fstat */
    test_xipfs_fstat_ebadbf_descp();
    test_xipfs_fstat_efault_buf();
    test_xipfs_fstat_ebadbf_xipfs_infos();
    test_xipfs_fstat_ok();

    /* xipfs_lseek */
    test_xipfs_lseek_einval_descp();
    test_xipfs_lseek_ebadf_descp();
    test_xipfs_lseek_einval_whence();
    test_xipfs_lseek_einval_seek_set_negative_pos();
    test_xipfs_lseek_einval_seek_set_pos_past_max_pos();
    test_xipfs_lseek_einval_seek_cur_negative_pos_out_of_file();
    test_xipfs_lseek_einval_seek_cur_positive_pos_out_of_file();
    test_xipfs_lseek_einval_seek_end_positive_pos();
    test_xipfs_lseek_einval_seek_end_negative_pos_out_of_file();
    test_xipfs_lseek_ok_seek_set_empty_file_pos_0();
    test_xipfs_lseek_ok_seek_cur_empty_file_pos_0();
    test_xipfs_lseek_ok_seek_end_empty_file_pos_0();
    test_xipfs_lseek_ok_seek_set_pos_0();
    test_xipfs_lseek_ok_seek_cur_pos_0();
    test_xipfs_lseek_ok_seek_end_pos_0();
    test_xipfs_lseek_ok_seek_set_pos_1();
    test_xipfs_lseek_ok_seek_cur_pos_1();
    test_xipfs_lseek_ok_seek_cur_pos_minus_1();
    test_xipfs_lseek_ok_seek_end_pos_minus_3();

    /* xipfs_open */
    test_xipfs_open_efault_name();
    test_xipfs_open_einval_flags();
    test_xipfs_open_enametoolong_name();
    test_xipfs_open_eexist_xipfs_infos_flags();
    test_xipfs_open_eacces_xipfs_infos_flags();
    test_xipfs_open_eexist_name();
    test_xipfs_open_eisdir_name();
    test_xipfs_open_enotdir_name();
    test_xipfs_open_enoent_flags();
    test_xipfs_open_eisdir_name_trailing_slash();
    test_xipfs_open_edquot();
    test_xipfs_open_ok();

    /* xipfs_read */
    test_xipfs_read_ebadf_descp();
    test_xipfs_read_efault_dest();
    test_xipfs_read_eacces_flags();
    test_xipfs_read_eio_empty_file();

    /* xipfs_write */
    test_xipfs_write_ebadf_descp();
    test_xipfs_write_efault_src();
    test_xipfs_write_eacces_flags();
    test_xipfs_write_ok();
    test_xipfs_write_consistency();
    test_xipfs_write_page_change();
    test_xipfs_write_io_buffer();
    test_xipfs_write_last_page_of_filesystem();

    /* xipfs_fsync */
    test_xipfs_fsync_ebadf_descp();
    test_xipfs_fsync_eacces_flags();;
    test_xipfs_fsync_ok();

    /* xipfs_opendir */
    test_xipfs_opendir_efault_dirname();
    test_xipfs_opendir_enoent_dirname_null_char();
    test_xipfs_opendir_enametoolong_dirname();
    test_xipfs_opendir_enotdir_dirname();
    test_xipfs_opendir_enoent_dirname();
    test_xipfs_opendir_ok();

    /* xipfs_readdir */
    test_xipfs_readdir_efault_direntp();
    test_xipfs_readdir_ebadf_descp();
    test_xipfs_readdir_ok();

    /* xipfs_closedir */
    test_xipfs_closedir_ebadf_descp();
    test_xipfs_closedir_ok();

    /* xipfs_unlink */
    test_xipfs_unlink_efault_name();
    test_xipfs_unlink_enoent_name_null_char();
    test_xipfs_unlink_eisdir_name_root();
    test_xipfs_unlink_enametoolong_name();
    test_xipfs_unlink_eisdir_name();
    test_xipfs_unlink_enotdir_name();
    test_xipfs_unlink_enoent_name();
    test_xipfs_unlink_ok();

    /* xipfs_mkdir */
    test_xipfs_mkdir_efault_name();
    test_xipfs_mkdir_enoent_name_null_char();
    test_xipfs_mkdir_eexist_name_root();
    test_xipfs_mkdir_enametoolong_name();
    test_xipfs_mkdir_eexist_name();
    test_xipfs_mkdir_enotdir_name();
    test_xipfs_mkdir_enoent_name();
    test_xipfs_mkdir_enametoolong_name_trailing_slash();
    test_xipfs_mkdir_ok();

    /* xipfs_rmdir */
    test_xipfs_rmdir_efault_name();
    test_xipfs_rmdir_enoent_name_null_char();
    test_xipfs_rmdir_ebusy_name_root();
    test_xipfs_rmdir_enametoolong_name();
    test_xipfs_rmdir_einval_name();
    test_xipfs_rmdir_enotdir_name();
    test_xipfs_rmdir_enotempty_name();
    test_xipfs_rmdir_enoent_name();
    test_xipfs_rmdir_ok();

    /* xipfs_rename */
    test_xipfs_rename_efault_from_path();
    test_xipfs_rename_efault_to_path();
    test_xipfs_rename_enoent_from_path_null_char();
    test_xipfs_rename_enoent_to_path_null_char();
    test_xipfs_rename_enametoolong_from_path();
    test_xipfs_rename_enametoolong_to_path();
    test_xipfs_rename_eisdir_from_path_file_to_path_dirs();
    test_xipfs_rename_enotdir_from_path_file_to_path_not_dirs();
    test_xipfs_rename_enoent_from_path_file_to_path_not_found();
    test_xipfs_rename_enotdir_from_path_file_to_path_creatable_trailing_slash();
    test_xipfs_rename_enotdir_from_path_empty_dir_to_path_file();
    test_xipfs_rename_enotempty_from_path_empty_dir_to_path_nonempty_dir();
    test_xipfs_rename_enotdir_from_path_empty_dir_to_path_not_dirs();
    test_xipfs_rename_enoent_from_path_empty_dir_to_path_not_found();
    test_xipfs_rename_enametoolong_from_path_empty_dir_to_path_trailing_slash();
    test_xipfs_rename_einval_from_path_empty_dir_to_path_creatable_subdir_itself();
    test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_file();
    test_xipfs_rename_einval_from_path_nonempty_dir_to_path_empty_dir_subdir_itself();
    test_xipfs_rename_enotempty_from_path_nonempty_dir_to_path_nonempty_dir();
    test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_not_dirs();
    test_xipfs_rename_enoent_from_path_nonempty_dir_to_path_not_found();
    test_xipfs_rename_enametoolong_from_path_nonempty_dir_to_path_trailing_slash();
    test_xipfs_rename_enotdir_from_path_not_dirs();
    test_xipfs_rename_enoent_from_path_not_found();
    test_xipfs_rename_ok();

    /* xipfs_stat */
    test_xipfs_stat_efault_path();
    test_xipfs_stat_efault_buf();
    test_xipfs_stat_enoent_path_null_char();
    test_xipfs_stat_enametoolong_path();
    test_xipfs_stat_enotdir_path();
    test_xipfs_stat_enoent_path();
    test_xipfs_stat_ok();

    /* xipfs_statvfs */
    test_xipfs_statvfs_efault_buf();
    test_xipfs_statvfs_ok();

    /* xipfs_new_file */
    test_xipfs_new_file_efault_path();
    test_xipfs_new_file_enoent_path_null_char();
    test_xipfs_new_file_eisdir_path_root();
    test_xipfs_new_file_enametoolong_path();
    test_xipfs_new_file_einval_exec();
    test_xipfs_new_file_einval_negative_size();
    test_xipfs_new_file_eexist_path();
    test_xipfs_new_file_eisdir_path();
    test_xipfs_new_file_enotdir_path();
    test_xipfs_new_file_enoent_path();
    test_xipfs_new_file_eisdir_trailing_slash();
    test_xipfs_new_file_edquot();
    test_xipfs_new_file_ok();

    /* xipfs_execv */
    test_xipfs_execv_efault_path();
    test_xipfs_execv_enoent_path_null_char();
    test_xipfs_execv_eisdir_path_root();
    test_xipfs_execv_enametoolong_path();
    test_xipfs_execv_efault_no_syscalls_table();
    test_xipfs_execv_efault_syscalls_table_with_null();
    test_xipfs_execv_eisdir_path();
    test_xipfs_execv_enotdir_path();
    test_xipfs_execv_enoent_path();
    test_xipfs_execv_efault_null_args();
    test_xipfs_execv_efault_args_0_null();

    /* xipfs_safe_execv */
#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT
    test_xipfs_safe_execv_efault_path();
    test_xipfs_safe_execv_enoent_path_null_char();
    test_xipfs_safe_execv_eisdir_path_root();
    test_xipfs_safe_execv_enametoolong_path();
    test_xipfs_safe_execv_efault_no_syscalls_table();
    test_xipfs_safe_execv_efault_syscalls_table_with_null();
    test_xipfs_safe_execv_eisdir_path();
    test_xipfs_safe_execv_enotdir_path();
    test_xipfs_safe_execv_enoent_path();
    test_xipfs_safe_execv_efault_null_args();
    test_xipfs_safe_execv_efault_args_0_null();
#endif /* XIPFS_ENABLE_SAFE_EXEC_SUPPORT */

    /* Should not fail unless there is a corrupted data
     * structure or a flash memory failure. */
    ret = xipfs_umount(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    /* From here, we need vfs to be initialized */
    ret = vfs_format(&vfs_xipfs_mount->vfs_mp);
    XIPFS_ASSERT(ret == 0);
    ret = vfs_mount(&vfs_xipfs_mount->vfs_mp);
    XIPFS_ASSERT(ret == 0);

    /* xipfs_extended_driver_execv */
    test_xipfs_extended_driver_execv();

#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT
    /* xipfs_extended_driver_safe_execv */
    test_xipfs_extended_driver_safe_execv();
#endif

    ret = vfs_umount(&vfs_xipfs_mount->vfs_mp, true);
    XIPFS_ASSERT(ret == 0);

    printf("Tests finished.\n");
}

/*
 * Entry point
 */

int main(void)
{
    test_xipfs_flash_memory();

    test_xipfs_format_invalid_cases();
    test_xipfs_mount_invalid_cases();

    test_xipfs_suite(&nvme0p0);

    {
        printf("Constructing vfs_xipfs_mount from mtd_flash_aux_slot...");

        mutex_t            mutex;
        mutex_t            execution_mutex;
        vfs_xipfs_mount_t  vfs_xipfs_mount;

        int ret = xipfs_construct_from_flashpage(
            &mtd_flash_aux_slot, "/dev/nvme0p0",
            &execution_mutex, &mutex,
            &vfs_xipfs_mount);
        XIPFS_ASSERT(ret == 0);
        printf("Done.\n");

        printf("mtd_flash_aux_slot.offset : %lu, mtd_flash_aux_slot.base.sector_count : %lu\n",
               mtd_flash_aux_slot.offset, mtd_flash_aux_slot.base.sector_count);

        printf("vfs_xipfs_mount.page_num : %u, vfs_xipfs_mount.page_addr : %p\n",
               vfs_xipfs_mount.page_num, vfs_xipfs_mount.page_addr);

        test_xipfs_suite(&vfs_xipfs_mount);
    }

    for (;;) {}
}

/*
 * Test function implementations
 */
static void test_xipfs_flash_memory(void)
{
    printf("Flash memory tests started...\n");
    XIPFS_ASSERT(xipfs_nvm_addr(0) == (const void *)xipfs_flash_base_addr());
    XIPFS_ASSERT(xipfs_nvm_addr(XIPFS_NVM_NUMOF) == (void *)xipfs_flash_end_addr());
    XIPFS_ASSERT(xipfs_nvm_page((const void *)xipfs_flash_base_addr()) == 0);
    XIPFS_ASSERT(xipfs_nvm_page((const void *)xipfs_flash_end_addr()) == XIPFS_NVM_NUMOF);
    printf("Flash memory tests finished.\n");
}

typedef int (*test_xipfs_mountpoint_callback_t)(xipfs_mount_t *mp);

static void test_xipfs_mountpoint_invalid_cases(test_xipfs_mountpoint_callback_t callback)
{
    xipfs_mount_t mp;
    const char *mount_path = "test";
    mutex_t mutex, execution_mutex;

    XIPFS_ASSERT(callback != NULL);

    /* NULL mountpoint. */
    {
        int ret = callback(NULL);
        XIPFS_ASSERT(ret == -EFAULT);
    }

    mp.mount_path = mount_path;
    mp.page_num = 1;
    mp.page_addr = xipfs_nvm_addr(0);
    mp.execution_mutex = &execution_mutex;
    mp.mutex = &mutex;

    /* Invalid magic. */
    {
        XIPFS_ASSERT(mp.mount_path == mount_path);
        XIPFS_ASSERT(mp.page_num == 1);
        XIPFS_ASSERT(mp.page_addr == xipfs_nvm_addr(0));
        XIPFS_ASSERT(mp.execution_mutex == &execution_mutex);
        XIPFS_ASSERT(mp.mutex == &mutex);

        mp.magic = (unsigned)0xFF42CAFE;
        int ret = callback(&mp);
        XIPFS_ASSERT(ret = -EINVAL);
    }

    mp.magic = XIPFS_MAGIC;

    /* Mount path .*/
    {
        XIPFS_ASSERT(mp.magic == XIPFS_MAGIC);
        XIPFS_ASSERT(mp.page_num == 1);
        XIPFS_ASSERT(mp.page_addr == xipfs_nvm_addr(0));
        XIPFS_ASSERT(mp.execution_mutex == &execution_mutex);
        XIPFS_ASSERT(mp.mutex == &mutex);

        /* NULL. */
        mp.mount_path = NULL;
        {
            int ret = callback(&mp);
            XIPFS_ASSERT(ret == -EINVAL);
        }
        /* Empty string */
        mp.mount_path = "";
        {
            int ret = callback(&mp);
            XIPFS_ASSERT(ret == -EINVAL);
        }
        /* Root */
        mp.mount_path = "/";
        {
            int ret = callback(&mp);
            XIPFS_ASSERT(ret == -EINVAL);
        }
    }

    mp.mount_path = mount_path;

    /* page_addr out of flash range. */
    {
        XIPFS_ASSERT(mp.magic == XIPFS_MAGIC);
        XIPFS_ASSERT(mp.mount_path == mount_path);
        XIPFS_ASSERT(mp.page_num == 1);
        XIPFS_ASSERT(mp.execution_mutex == &execution_mutex);
        XIPFS_ASSERT(mp.mutex == &mutex);

        /* Pointer less than base if any. */
        {
            const unsigned page_addr = xipfs_flash_base_addr() - 1;
            if (page_addr != xipfs_flash_end_addr()) {
                mp.page_addr = (void *)page_addr;
                int ret = callback(&mp);
                XIPFS_ASSERT(ret == -EINVAL);
            }
        }

        /* Pointer greater than xipfs_flash_end_addr if any */
        {
            const unsigned page_addr = xipfs_flash_end_addr() + 1;
            if (page_addr != xipfs_flash_base_addr()) {
                mp.page_addr = (void *)page_addr;
                int ret = callback(&mp);
                XIPFS_ASSERT(ret == -EINVAL);
            }
        }
    }

    mp.page_addr = xipfs_nvm_addr(0);

    /* num_page out of range */
    {
        XIPFS_ASSERT(mp.magic == XIPFS_MAGIC);
        XIPFS_ASSERT(mp.mount_path == mount_path);
        XIPFS_ASSERT(mp.page_addr == xipfs_nvm_addr(0));
        XIPFS_ASSERT(mp.execution_mutex == &execution_mutex);
        XIPFS_ASSERT(mp.mutex == &mutex);

        /* Empty filesystem. */
        mp.page_num = 0;
        {
            int ret = callback(&mp);
            XIPFS_ASSERT(ret == -EINVAL);
        }

        /* Filesystem bigger than NVM capacity */
        if ((size_t)XIPFS_NVM_NUMOF != SIZE_MAX) {
            mp.page_num = ((size_t)XIPFS_NVM_NUMOF) + 1;
            {
                int ret = callback(&mp);
                XIPFS_ASSERT(ret == -EINVAL);
            }
        }

        /* Filesystem pages count greater then max supported pages count. */
        mp.page_num = ((size_t)INT_MAX) + 1;
        {
            int ret = callback(&mp);
            XIPFS_ASSERT(ret == -EINVAL);
        }
    }

    mp.page_num = 1;

    /* page_addr leads to a page index greater than max supported pages count. */
    if (sizeof(int) < sizeof(void *))
    {
        /* This case cannot happen on 32 bits architectures, only for 64 bits architectures. */
        XIPFS_ASSERT(mp.magic == XIPFS_MAGIC);
        XIPFS_ASSERT(mp.mount_path == mount_path);
        XIPFS_ASSERT(mp.page_num == 1);
        XIPFS_ASSERT(mp.execution_mutex == &execution_mutex);
        XIPFS_ASSERT(mp.mutex == &mutex);

        mp.page_addr = xipfs_nvm_addr(((unsigned)INT_MAX) + 1);
        {
            int ret = callback(&mp);
            XIPFS_ASSERT(ret == -EINVAL);
        }
    }

    /* page_addr + page_num out of NVM capacity. */
    {
        XIPFS_ASSERT(mp.magic == XIPFS_MAGIC);
        XIPFS_ASSERT(mp.mount_path == mount_path);
        XIPFS_ASSERT(mp.execution_mutex == &execution_mutex);
        XIPFS_ASSERT(mp.mutex == &mutex);

        /* page_addr in NVM range, page_num greater than remaining pages in NVM */
        mp.page_addr = xipfs_nvm_addr(XIPFS_NVM_NUMOF / 2);
        mp.page_num = (XIPFS_NVM_NUMOF / 2) + 1;
        {
            int ret = callback(&mp);
            XIPFS_ASSERT(ret == -EINVAL);
        }

        /* corner case : page_addr in NVM range, page_num greater than remaining pages in NVM */
        mp.page_addr = xipfs_nvm_addr(XIPFS_NVM_NUMOF - 1);
        mp.page_num = 2;
        {
            int ret = callback(&mp);
            XIPFS_ASSERT(ret == -EINVAL);
        }

        /* Corner case : page_addr = xipfs_flash_base_addr(), page_num = XIPFS_NVM_NUMOF + 1
           has already been tested above */
    }

    mp.page_addr = xipfs_nvm_addr(0);
    mp.page_num = 1;

    /* Mutex. */
    {
        XIPFS_ASSERT(mp.magic == XIPFS_MAGIC);
        XIPFS_ASSERT(mp.mount_path == mount_path);
        XIPFS_ASSERT(mp.page_num == 1);
        XIPFS_ASSERT(mp.page_addr == xipfs_nvm_addr(0));
        XIPFS_ASSERT(mp.execution_mutex == &execution_mutex);

        /* NULL */
        mp.mutex = NULL;
        int ret = callback(&mp);
        XIPFS_ASSERT(ret == -EINVAL);
    }

    mp.mutex = &mutex;

    /* Execution mutex. */
    {
        XIPFS_ASSERT(mp.magic == XIPFS_MAGIC);
        XIPFS_ASSERT(mp.mount_path == mount_path);
        XIPFS_ASSERT(mp.page_num == 1);
        XIPFS_ASSERT(mp.page_addr == xipfs_nvm_addr(0));
        XIPFS_ASSERT(mp.mutex == &mutex);

        /* NULL */
        mp.execution_mutex = NULL;
        int ret = callback(&mp);
        XIPFS_ASSERT(ret == -EINVAL);
    }
}

static void test_xipfs_format_invalid_cases(void) {
    printf("Format invalid cases tests started...\n");
    test_xipfs_mountpoint_invalid_cases(xipfs_format);
    printf("Format invalid cases tests finished.\n");
}
static void test_xipfs_mount_invalid_cases(void) {
    printf("Mount invalid cases tests started...\n");
    test_xipfs_mountpoint_invalid_cases(xipfs_mount);
    printf("Mount invalid cases tests finished.\n");
}

static void test_xipfs_close_ebadf_descp(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == -EINVAL);
}

static void test_xipfs_close_ok(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_fstat_ebadbf_descp(void)
{
    xipfs_file_desc_t desc;
    struct stat buf;
    int ret;

    /* test */
    ret = xipfs_fstat(xipfs_nvme0p0, &desc, &buf);
    XIPFS_ASSERT(ret == -EINVAL);
}

static void test_xipfs_fstat_efault_buf(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_fstat(xipfs_nvme0p0, &desc, NULL);
    XIPFS_ASSERT(ret == -EFAULT);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_fstat_ebadbf_xipfs_infos(void)
{
    xipfs_file_desc_t desc;
    struct stat buf;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/.xipfs_infos",
            O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_fstat(xipfs_nvme0p0, &desc, &buf);
    XIPFS_ASSERT(ret == -EBADF);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_fstat_ok(void)
{
    xipfs_file_desc_t desc;
    struct stat buf;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_fstat(xipfs_nvme0p0, &desc, &buf);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_einval_descp(void)
{
    xipfs_file_desc_t desc;
    off_t ret_seek;

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_SET);
    XIPFS_ASSERT(ret_seek == (off_t)-EINVAL);
}

static void test_xipfs_lseek_ebadf_descp(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_SET);
    XIPFS_ASSERT(ret_seek == (off_t)-EBADF);
}

static void test_xipfs_lseek_einval_whence(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, 0xffffffff);
    XIPFS_ASSERT(ret_seek == (off_t)-EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_einval_seek_set_negative_pos(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, -1, SEEK_SET);
    XIPFS_ASSERT(ret_seek == (off_t)-EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_einval_seek_set_pos_past_max_pos(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc,
                           XIPFS_NVM_PAGE_SIZE - sizeof(xipfs_file_t) + 1,
                           SEEK_SET);
    XIPFS_ASSERT(ret_seek == (off_t)-EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_einval_seek_cur_negative_pos_out_of_file(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, -1, SEEK_CUR);
    XIPFS_ASSERT(ret_seek == (off_t)-EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_einval_seek_cur_positive_pos_out_of_file(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc,
                           XIPFS_NVM_PAGE_SIZE - sizeof(xipfs_file_t) + 1,
                           SEEK_CUR);
    XIPFS_ASSERT(ret_seek == (off_t)-EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_einval_seek_end_positive_pos(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 1, SEEK_END);
    XIPFS_ASSERT(ret_seek == (off_t)-EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_einval_seek_end_negative_pos_out_of_file(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, -1, SEEK_END);
    XIPFS_ASSERT(ret_seek == (off_t)-EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_set_empty_file_pos_0(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_SET);
    XIPFS_ASSERT(ret_seek == (off_t)0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_cur_empty_file_pos_0(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_CUR);
    XIPFS_ASSERT(ret_seek == (off_t)0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_end_empty_file_pos_0(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_END);
    XIPFS_ASSERT(ret_seek == (off_t)0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_set_pos_0(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;
    ssize_t ret_write;
    const char buffer[3] = { '1', '2', '3' };

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_write = xipfs_write(xipfs_nvme0p0, &desc, buffer, sizeof(buffer));
    XIPFS_ASSERT(ret_write == (ssize_t)sizeof(buffer));

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_SET);
    XIPFS_ASSERT(ret_seek == (off_t)0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_cur_pos_0(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;
    ssize_t ret_write, ret_read;
    const char buffer[3] = { '1', '2', '3' };
    char read_buffer;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_write = xipfs_write(xipfs_nvme0p0, &desc, buffer, sizeof(buffer));
    XIPFS_ASSERT(ret_write == (ssize_t)sizeof(buffer));

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_read = xipfs_read(xipfs_nvme0p0, &desc, &read_buffer, sizeof(read_buffer));
    XIPFS_ASSERT(ret_read == (ssize_t)sizeof(read_buffer));

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_CUR);
    XIPFS_ASSERT(ret_seek == (off_t)1);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_end_pos_0(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;
    ssize_t ret_write;
    const char buffer[3] = { '1', '2', '3' };

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_write = xipfs_write(xipfs_nvme0p0, &desc, buffer, sizeof(buffer));
    XIPFS_ASSERT(ret_write == (ssize_t)sizeof(buffer));

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_END);
    XIPFS_ASSERT(ret_seek == (off_t)3);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_set_pos_1(void)
{
    xipfs_file_desc_t desc;
    int ret;
    ssize_t ret_write;
    off_t ret_seek;
    const char buffer[3] = { '1', '2', '3'};

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_write = xipfs_write(xipfs_nvme0p0, &desc, buffer, sizeof(buffer));
    XIPFS_ASSERT(ret_write == (ssize_t)sizeof(buffer));

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 1, SEEK_SET);
    XIPFS_ASSERT(ret_seek == (off_t)1);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_cur_pos_1(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;
    ssize_t ret_write, ret_read;
    const char buffer[3] = { '1', '2', '3' };
    char read_buffer;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_write = xipfs_write(xipfs_nvme0p0, &desc, buffer, sizeof(buffer));
    XIPFS_ASSERT(ret_write == (ssize_t)sizeof(buffer));

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_read = xipfs_read(xipfs_nvme0p0, &desc, &read_buffer, sizeof(read_buffer));
    XIPFS_ASSERT(ret_read == (ssize_t)sizeof(read_buffer));

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, 1, SEEK_CUR);
    XIPFS_ASSERT(ret_seek == (off_t)2);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_cur_pos_minus_1(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;
    ssize_t ret_write, ret_read;
    const char buffer[3] = { '1', '2', '3' };
    char read_buffer;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_write = xipfs_write(xipfs_nvme0p0, &desc, buffer, sizeof(buffer));
    XIPFS_ASSERT(ret_write == (ssize_t)sizeof(buffer));

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_read = xipfs_read(xipfs_nvme0p0, &desc, &read_buffer, sizeof(read_buffer));
    XIPFS_ASSERT(ret_read == (ssize_t)sizeof(read_buffer));

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, -1, SEEK_CUR);
    XIPFS_ASSERT(ret_seek == (off_t)0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok_seek_end_pos_minus_3(void)
{
    xipfs_file_desc_t desc;
    int ret;
    off_t ret_seek;
    ssize_t ret_write;
    const char buffer[3] = { '1', '2', '3' };

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    ret_write = xipfs_write(xipfs_nvme0p0, &desc, buffer, sizeof(buffer));
    XIPFS_ASSERT(ret_write == (ssize_t)sizeof(buffer));

    /* test */
    ret_seek = xipfs_lseek(xipfs_nvme0p0, &desc, -3, SEEK_END);
    XIPFS_ASSERT(ret_seek == (off_t)0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_open_efault_name(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, NULL, O_CREAT, 0);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_open_einval_flags(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            0xffffffff, 0);
    XIPFS_ASSERT(ret == -EINVAL);
}

static void test_xipfs_open_enametoolong_name(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/totoooooooooooooooo"
            "ooooooooooooooooooooooooooooooooooooooooooooo",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_open_eexist_xipfs_infos_flags(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/.xipfs_infos",
            O_CREAT | O_EXCL, 0);
    XIPFS_ASSERT(ret == -EEXIST);
}

static void test_xipfs_open_eacces_xipfs_infos_flags(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/.xipfs_infos",
            O_WRONLY, 0);
    XIPFS_ASSERT(ret == -EACCES);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/.xipfs_infos",
            O_APPEND, 0);
    XIPFS_ASSERT(ret == -EACCES);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/.xipfs_infos",
            O_RDWR, 0);
    XIPFS_ASSERT(ret == -EACCES);
}

static void test_xipfs_open_eexist_name(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_EXCL, 0);
    XIPFS_ASSERT(ret == -EEXIST);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_open_eisdir_name(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_RDONLY, 0);
    XIPFS_ASSERT(ret == -EISDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_open_enotdir_name(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto/toto",
                     O_CREAT, 0);
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_open_enoent_flags(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_RDONLY, 0);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_open_eisdir_name_trailing_slash(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto/",
            O_CREAT | O_RDONLY, 0);
    XIPFS_ASSERT(ret == -EISDIR);
}

static void test_xipfs_open_edquot(void)
{
    char path[XIPFS_PATH_MAX];
    xipfs_file_desc_t desc;
    size_t i;
    int ret;

    /* initialization */
    for (i = 0; i < xipfs_nvme0p0->page_num; i++) {
        (void)sprintf(path, "/%zu", i);

        ret = xipfs_open(xipfs_nvme0p0, &desc, path,
                O_CREAT, 0);
        XIPFS_ASSERT(ret == 0);

        ret = xipfs_close(xipfs_nvme0p0, &desc);
        XIPFS_ASSERT(ret == 0);
    }

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == -EDQUOT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_open_ok(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_read_ebadf_descp(void)
{
    xipfs_file_desc_t desc;
    char buf;
    int ret;

    /* test */
    ret = xipfs_read(xipfs_nvme0p0, &desc, &buf, sizeof(buf));
    XIPFS_ASSERT(ret == -EBADF);
}

static void test_xipfs_read_efault_dest(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_read(xipfs_nvme0p0, &desc, NULL, 0);
    XIPFS_ASSERT(ret == -EFAULT);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_read_eacces_flags(void)
{
    xipfs_file_desc_t desc;
    char buf;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_read(xipfs_nvme0p0, &desc, &buf, sizeof(buf));
    XIPFS_ASSERT(ret == -EACCES);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_read_eio_empty_file(void)
{
    xipfs_file_desc_t desc;
    char buf;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_read(xipfs_nvme0p0, &desc, &buf, sizeof(buf));
    XIPFS_ASSERT(ret == -EIO);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_write_ebadf_descp(void)
{
    xipfs_file_desc_t desc;
    char buf = 0xff;
    int ret;

    /* test */
    ret = xipfs_write(xipfs_nvme0p0, &desc, &buf, sizeof(buf));
    XIPFS_ASSERT(ret == -EBADF);
}

static void test_xipfs_write_efault_src(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_write(xipfs_nvme0p0, &desc, NULL, 0);
    XIPFS_ASSERT(ret == -EFAULT);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_write_eacces_flags(void)
{
    xipfs_file_desc_t desc;
    char buf = 0xff;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_write(xipfs_nvme0p0, &desc, &buf, sizeof(buf));
    XIPFS_ASSERT(ret == -EACCES);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_write_ok(void)
{
    xipfs_file_desc_t desc;
    char buf = 0xff;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_write(xipfs_nvme0p0, &desc, &buf, sizeof(buf));
    XIPFS_ASSERT(ret == 1);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_write_read_consistency(size_t pages_count)
{
#define PATTERN_BYTESIZE (16)
    const char write_pattern[PATTERN_BYTESIZE] =
    {
        0x0, 0x1, 0x2, 0x3,
        0x4, 0x5, 0x6, 0x7,
        0x8, 0x9, 0xA, 0xB,
        0xC, 0xD, 0xE, 0xF
    };
    const size_t filesize = (((size_t)XIPFS_NVM_PAGE_SIZE) * pages_count) - sizeof(xipfs_file_t);
    const size_t patterns_count = filesize / (size_t)PATTERN_BYTESIZE;
    const size_t remaining_bytes_count = filesize % (size_t)PATTERN_BYTESIZE;
    char read_buffer[PATTERN_BYTESIZE];
    int ret;
    ssize_t bytes_count;
    xipfs_file_desc_t desc;
    const char *filename = "/toto";
    size_t i;

    XIPFS_ASSERT(pages_count > 0);

    ret = xipfs_new_file(xipfs_nvme0p0, filename, filesize, 0);
    XIPFS_ASSERT(ret == 0);

    /* Fill nvm pages with write_pattern. */
    {
        ret = xipfs_open(xipfs_nvme0p0, &desc, filename, O_WRONLY, 0);
        XIPFS_ASSERT(ret == 0);

        for (i = 0; i < patterns_count; i++)
        {
            bytes_count = xipfs_write(xipfs_nvme0p0, &desc, write_pattern, PATTERN_BYTESIZE);
            XIPFS_ASSERT(bytes_count == (ssize_t)PATTERN_BYTESIZE);
        }
        bytes_count = xipfs_write(xipfs_nvme0p0, &desc, write_pattern, remaining_bytes_count);
        XIPFS_ASSERT(bytes_count == (ssize_t)remaining_bytes_count);

        ret = xipfs_close(xipfs_nvme0p0, &desc);
        XIPFS_ASSERT(ret == 0);
    }

    /* Read pages back and check that the pattern is as expected. */
    {
        ret = xipfs_open(xipfs_nvme0p0, &desc, filename, O_RDONLY, 0);
        XIPFS_ASSERT(ret == 0);

        for (i = 0; i < patterns_count; i++)
        {
            bytes_count = xipfs_read(xipfs_nvme0p0, &desc, read_buffer, PATTERN_BYTESIZE);
            XIPFS_ASSERT(bytes_count == (ssize_t)PATTERN_BYTESIZE);

            ret = memcmp(read_buffer, write_pattern, PATTERN_BYTESIZE);
            XIPFS_ASSERT(ret == 0);
        }
        bytes_count = xipfs_read(xipfs_nvme0p0, &desc, read_buffer, remaining_bytes_count);
        XIPFS_ASSERT(bytes_count == (ssize_t)remaining_bytes_count);

        ret = memcmp(read_buffer, write_pattern, remaining_bytes_count);
        XIPFS_ASSERT(ret == 0);

        ret = xipfs_close(xipfs_nvme0p0, &desc);
        XIPFS_ASSERT(ret == 0);
    }

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
#undef PATTERN_BYTESIZE
}

static void test_xipfs_write_consistency(void)
{
    test_xipfs_write_read_consistency(1);
}

static void test_xipfs_write_page_change(void)
{
    test_xipfs_write_read_consistency(2);
}

static void test_xipfs_write_io_buffer(void)
{
    const char *filename_0 = "/toto";
    const char *filename_1 = "/titi";
    const char *pattern_0 = "012345";
    const char *pattern_1 = "ABCDEF";
    xipfs_file_desc_t desc_0, desc_1;
    ssize_t bytes_count;
    int ret;

    /* Write to 2 distinct files in an interleaved way.
     * so that we check io buffer integrity on different pages writes.
     */
    {
        ret = xipfs_open(xipfs_nvme0p0, &desc_0, filename_0, O_CREAT | O_WRONLY, 0);
        XIPFS_ASSERT(ret == 0);

        ret = xipfs_open(xipfs_nvme0p0, &desc_1, filename_1, O_CREAT | O_WRONLY, 0);
        XIPFS_ASSERT(ret == 0);

        bytes_count = xipfs_write(xipfs_nvme0p0, &desc_0, pattern_0, 3);
        XIPFS_ASSERT(bytes_count == (ssize_t)3);

        bytes_count = xipfs_write(xipfs_nvme0p0, &desc_1, pattern_1, 2);
        XIPFS_ASSERT(bytes_count == (ssize_t)2);

        bytes_count = xipfs_write(xipfs_nvme0p0, &desc_0, pattern_0 + 3, 3);
        XIPFS_ASSERT(bytes_count == (ssize_t)3);

        bytes_count = xipfs_write(xipfs_nvme0p0, &desc_1, pattern_1 + 2, 4);
        XIPFS_ASSERT(bytes_count == (ssize_t)4);

        ret = xipfs_close(xipfs_nvme0p0, &desc_0);
        XIPFS_ASSERT(ret == 0);
        ret = xipfs_close(xipfs_nvme0p0, &desc_1);
        XIPFS_ASSERT(ret == 0);
    }

    /* Check that files contents are as expected in a sequential way. */
    {
        char read_buffer[6];

        ret = xipfs_open(xipfs_nvme0p0, &desc_0, filename_0, O_RDONLY, 0);
        XIPFS_ASSERT(ret == 0);
        ret = xipfs_open(xipfs_nvme0p0, &desc_1, filename_1, O_RDONLY, 0);
        XIPFS_ASSERT(ret == 0);

        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_0, read_buffer, 6);
        XIPFS_ASSERT(bytes_count == (ssize_t)6);
        ret = memcmp(read_buffer, pattern_0, 6);
        XIPFS_ASSERT(ret == 0);

        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_1, read_buffer, 6);
        XIPFS_ASSERT(bytes_count == (ssize_t)6);
        ret = memcmp(read_buffer, pattern_1, 6);
        XIPFS_ASSERT(ret == 0);

        ret = xipfs_close(xipfs_nvme0p0, &desc_0);
        XIPFS_ASSERT(ret == 0);
        ret = xipfs_close(xipfs_nvme0p0, &desc_1);
        XIPFS_ASSERT(ret == 0);
    }

    /* Check that files contents are as expected in an interleaved way;
     * so that we check io buffer integrity on different pages reads.
     */
    {
        char read_buffer_0[6];
        char read_buffer_1[6];

        ret = xipfs_open(xipfs_nvme0p0, &desc_0, filename_0, O_RDONLY, 0);
        XIPFS_ASSERT(ret == 0);
        ret = xipfs_open(xipfs_nvme0p0, &desc_1, filename_1, O_RDONLY, 0);
        XIPFS_ASSERT(ret == 0);

        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_0, read_buffer_0, 2);
        XIPFS_ASSERT(bytes_count == (ssize_t)2);

        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_1, read_buffer_1, 3);
        XIPFS_ASSERT(bytes_count == (ssize_t)3);

        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_0, read_buffer_0 + 2, 4);
        XIPFS_ASSERT(bytes_count == (ssize_t)4);
        ret = memcmp(read_buffer_0, pattern_0, 6);
        XIPFS_ASSERT(ret == 0);

        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_1, read_buffer_1 + 3, 3);
        XIPFS_ASSERT(bytes_count == (ssize_t)3);
        ret = memcmp(read_buffer_1, pattern_1, 6);
        XIPFS_ASSERT(ret == 0);

        ret = xipfs_close(xipfs_nvme0p0, &desc_0);
        XIPFS_ASSERT(ret == 0);
        ret = xipfs_close(xipfs_nvme0p0, &desc_1);
        XIPFS_ASSERT(ret == 0);
    }

    /* Try to read from an existing file, while writing to another one. */
    {
        char read_buffer[12];

        ret = xipfs_open(xipfs_nvme0p0, &desc_0, filename_0, O_RDONLY, 0);
        XIPFS_ASSERT(ret == 0);

        ret = xipfs_open(xipfs_nvme0p0, &desc_1, filename_1, O_RDWR | O_APPEND, 0);
        XIPFS_ASSERT(ret == 0);

        /* Read and write in an interleaved way. */
        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_0, read_buffer, 3);
        XIPFS_ASSERT(bytes_count == (ssize_t)3);
        ret = memcmp(read_buffer, pattern_0, 3);
        XIPFS_ASSERT(ret == 0);

        bytes_count = xipfs_write(xipfs_nvme0p0, &desc_1, pattern_1, 3);
        XIPFS_ASSERT(bytes_count == (ssize_t)3);

        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_0, read_buffer, 3);
        XIPFS_ASSERT(bytes_count == (ssize_t)3);
        ret = memcmp(read_buffer, pattern_0 + 3, 3);
        XIPFS_ASSERT(ret == 0);

        bytes_count = xipfs_write(xipfs_nvme0p0, &desc_1, pattern_1 + 3, 3);
        XIPFS_ASSERT(bytes_count == (ssize_t)3);

        ret = xipfs_close(xipfs_nvme0p0, &desc_0);
        XIPFS_ASSERT(ret == 0);

        /* Now reset the file position at the beginning of the file,
         * and check that contents are consistent.
         */
        off_t position_in_file = xipfs_lseek(xipfs_nvme0p0, &desc_1, 0, SEEK_SET);
        XIPFS_ASSERT(position_in_file == (off_t)0);

        bytes_count = xipfs_read(xipfs_nvme0p0, &desc_1, read_buffer, 12);
        XIPFS_ASSERT(bytes_count == (ssize_t)12);

        ret = memcmp(pattern_1, read_buffer, 6);
        XIPFS_ASSERT(ret == 0);

        ret = memcmp(pattern_1, read_buffer + 6, 6);
        XIPFS_ASSERT(ret == 0);

        ret = xipfs_close(xipfs_nvme0p0, &desc_1);
        XIPFS_ASSERT(ret == 0);
    }

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_write_last_page_of_filesystem(void)
{
    char path[XIPFS_PATH_MAX];
    int ret;
    xipfs_file_desc_t desc;
    ssize_t bytes_count;
    size_t i;

    for (i = 0; i < xipfs_nvme0p0->page_num - 1; i++)
    {
        (void)sprintf(path, "/%zu", i);

        ret = xipfs_open(xipfs_nvme0p0, &desc, path, O_CREAT, 0);
        XIPFS_ASSERT(ret == 0);

        ret = xipfs_close(xipfs_nvme0p0, &desc);
        XIPFS_ASSERT(ret == 0);
    }

#define PATTERN_BYTESIZE (16)
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/last_file", O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    const char write_pattern[PATTERN_BYTESIZE] =
    {
        0x0, 0x1, 0x2, 0x3,
        0x4, 0x5, 0x6, 0x7,
        0x8, 0x9, 0xA, 0xB,
        0xC, 0xD, 0xE, 0xF
    };
    const size_t filesize = (size_t)XIPFS_NVM_PAGE_SIZE - sizeof(xipfs_file_t);
    const size_t patterns_count = filesize / (size_t)PATTERN_BYTESIZE;
    const size_t remaining_bytes_count = filesize % (size_t)PATTERN_BYTESIZE;
    for (size_t i = 0; i < patterns_count; i++)
    {
        bytes_count = xipfs_write(xipfs_nvme0p0, &desc, write_pattern, PATTERN_BYTESIZE);
        XIPFS_ASSERT(bytes_count == (ssize_t)PATTERN_BYTESIZE);
    }
    bytes_count = xipfs_write(xipfs_nvme0p0, &desc, write_pattern, remaining_bytes_count);
    XIPFS_ASSERT(bytes_count == remaining_bytes_count);
#undef PATTERN_BYTESIZE

    /* Now the filesystem is full.
     * Try to write one more byte into the file.
     */
    bytes_count = xipfs_write(xipfs_nvme0p0, &desc, write_pattern, 1);
    XIPFS_ASSERT(bytes_count == -EDQUOT);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* Try to create one more file. */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/should_not_happen", O_CREAT, 0);
    XIPFS_ASSERT(ret == -EDQUOT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_fsync_ebadf_descp(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_fsync(xipfs_nvme0p0, &desc, 0);
    XIPFS_ASSERT(ret == -EBADF);
}

static void test_xipfs_fsync_eacces_flags(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_RDONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_fsync(xipfs_nvme0p0, &desc, 0);
    XIPFS_ASSERT(ret == -EACCES);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_fsync_ok(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT | O_WRONLY, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_fsync(xipfs_nvme0p0, &desc, 0);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_opendir_efault_dirname(void)
{
    xipfs_dir_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_opendir(xipfs_nvme0p0, &desc, NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_opendir_enoent_dirname_null_char(void)
{
    xipfs_dir_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_opendir(xipfs_nvme0p0, &desc, "");
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_opendir_enametoolong_dirname(void)
{
    xipfs_dir_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_opendir(xipfs_nvme0p0, &desc, "/totooooo"
            "oooooooooooooooooooooooooooooooooooooooooooooooooo"
            "ooooo/");
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_opendir_enotdir_dirname(void)
{
    xipfs_file_desc_t desc_file;
    xipfs_dir_desc_t desc_dir;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc_file, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc_file);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_opendir(xipfs_nvme0p0, &desc_dir, "/toto");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_opendir_enoent_dirname(void)
{
    xipfs_dir_desc_t desc;
    int ret;

    /* test */
    ret = xipfs_opendir(xipfs_nvme0p0, &desc, "/toto/");
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_opendir_ok(void)
{
    xipfs_dir_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_opendir(xipfs_nvme0p0, &desc, "/toto/");
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_closedir(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_readdir_efault_direntp(void)
{
    xipfs_dir_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_opendir(xipfs_nvme0p0, &desc, "/toto/");
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_readdir(xipfs_nvme0p0, &desc, NULL);
    XIPFS_ASSERT(ret == -EFAULT);

    /* clean up */
    ret = xipfs_closedir(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_readdir_ebadf_descp(void)
{
    xipfs_dir_desc_t desc;
    xipfs_dirent_t dirent;
    int ret;

    /* test */
    ret = xipfs_readdir(xipfs_nvme0p0, &desc, &dirent);
    XIPFS_ASSERT(ret == -EBADF);
}

static void test_xipfs_readdir_ok(void)
{
    xipfs_dir_desc_t desc;
    xipfs_dirent_t dirent;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_opendir(xipfs_nvme0p0, &desc, "/toto/");
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_readdir(xipfs_nvme0p0, &desc, &dirent);
    XIPFS_ASSERT(ret >= 0);

    /* clean up */
    ret = xipfs_closedir(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_closedir_ebadf_descp(void)
{
    xipfs_dir_desc_t desc;
    xipfs_dirent_t dirent;
    int ret;

    /* test */
    ret = xipfs_readdir(xipfs_nvme0p0, &desc, &dirent);
    XIPFS_ASSERT(ret == -EBADF);
}

static void test_xipfs_closedir_ok(void)
{
    xipfs_dir_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_opendir(xipfs_nvme0p0, &desc, "/toto/");
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_closedir(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_unlink_efault_name(void)
{
    int ret;

    /* test */
    ret = xipfs_unlink(xipfs_nvme0p0, NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_unlink_enoent_name_null_char(void)
{
    int ret;

    /* test */
    ret = xipfs_unlink(xipfs_nvme0p0, "");
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_unlink_eisdir_name_root(void)
{
    int ret;

    /* test */
    ret = xipfs_unlink(xipfs_nvme0p0, "/");
    XIPFS_ASSERT(ret == -EISDIR);
}

static void test_xipfs_unlink_enametoolong_name(void)
{
    int ret;

    /* test */
    ret = xipfs_unlink(xipfs_nvme0p0, "/totooooooooooooo"
            "oooooooooooooooooooooooooooooooooooooooooooooooo");
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_unlink_eisdir_name(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_unlink(xipfs_nvme0p0, "/toto");
    XIPFS_ASSERT(ret == -EISDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_unlink_enotdir_name(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_unlink(xipfs_nvme0p0, "/toto/toto");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_unlink_enoent_name(void)
{
    int ret;

    /* test */
    ret = xipfs_unlink(xipfs_nvme0p0, "/toto");
    XIPFS_ASSERT(ret == -ENOENT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_unlink_ok(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_unlink(xipfs_nvme0p0, "/toto");
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_mkdir_efault_name(void)
{
    int ret;

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, NULL, 0);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_mkdir_enoent_name_null_char(void)
{
    int ret;

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, "", 0);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_mkdir_eexist_name_root(void)
{
    int ret;

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/", 0);
    XIPFS_ASSERT(ret == -EEXIST);
}

static void test_xipfs_mkdir_enametoolong_name(void)
{
    int ret;

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/totoooooooooooooo"
            "oooooooooooooooooooooooooooooooooooooooooooooo/", 0);
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_mkdir_eexist_name(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == -EEXIST);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_mkdir_enotdir_name(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/toto/", 0);
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_mkdir_enoent_name(void)
{
    int ret;

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/toto/", 0);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_mkdir_enametoolong_name_trailing_slash(void)
{
    int ret;

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/totoooooooooooooo"
            "ooooooooooooooooooooooooooooooooooooooooooooooo", 0);
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_mkdir_ok(void)
{
    int ret;

    /* test */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rmdir_efault_name(void)
{
    int ret;

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_rmdir_enoent_name_null_char(void)
{
    int ret;

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, "");
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_rmdir_ebusy_name_root(void)
{
    int ret;

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, "/");
    XIPFS_ASSERT(ret == -EBUSY);
}

static void test_xipfs_rmdir_enametoolong_name(void)
{
    int ret;

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, "/totoooooooooooooooooooooo"
        "oooooooooooooooooooooooooooooooooooooo/");
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_rmdir_einval_name(void)
{
    int ret;

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, ".");
    XIPFS_ASSERT(ret == -EINVAL);
}

static void test_xipfs_rmdir_enotdir_name(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, "/toto");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rmdir_enotempty_name(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, "/toto/");
    XIPFS_ASSERT(ret == -ENOTEMPTY);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rmdir_enoent_name(void)
{
    int ret;

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, "/toto/toto/");
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_rmdir_ok(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rmdir(xipfs_nvme0p0, "/toto/");
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_efault_from_path(void)
{
    int ret;

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, NULL, "/to/");
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_rename_efault_to_path(void)
{
    int ret;

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_rename_enoent_from_path_null_char(void)
{
    int ret;

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "", "/to/");
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_rename_enoent_to_path_null_char(void)
{
    int ret;

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "");
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_rename_enametoolong_from_path(void)
{
    int ret;

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/frommmmmmmmmmmmmmmmmmmmm"
            "mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm/", "/to/");
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_rename_enametoolong_to_path(void)
{
    int ret;

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/tooooooooooooo"
            "oooooooooooooooooooooooooooooooooooooooooooooooo/");
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_rename_eisdir_from_path_file_to_path_dirs(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/from", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/to/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from", "/to/");
    XIPFS_ASSERT(ret == -EISDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotdir_from_path_file_to_path_not_dirs(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/from", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/to", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from", "/to/from");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enoent_from_path_file_to_path_not_found(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/from", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from", "/to/from");
    XIPFS_ASSERT(ret == -ENOENT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotdir_from_path_file_to_path_creatable_trailing_slash(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/from", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from", "/to/");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotdir_from_path_empty_dir_to_path_file(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/to", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotempty_from_path_empty_dir_to_path_nonempty_dir(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/to/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/to/to/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/");
    XIPFS_ASSERT(ret == -ENOTEMPTY);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotdir_from_path_empty_dir_to_path_not_dirs(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/to", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/from/");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enoent_from_path_empty_dir_to_path_not_found(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/from/");
    XIPFS_ASSERT(ret == -ENOENT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enametoolong_from_path_empty_dir_to_path_trailing_slash(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/toooooooooooooooooo"
        "oooooooooooooooooooooooooooooooooooooooooooo");
    XIPFS_ASSERT(ret == -ENAMETOOLONG);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_einval_from_path_empty_dir_to_path_creatable_subdir_itself(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/from/from/");
    XIPFS_ASSERT(ret == -EINVAL);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_file(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/to", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_einval_from_path_nonempty_dir_to_path_empty_dir_subdir_itself(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/from/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/from/from/from/");
    XIPFS_ASSERT(ret == -EINVAL);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotempty_from_path_nonempty_dir_to_path_nonempty_dir(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/to/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/to/to/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/");
    XIPFS_ASSERT(ret == -ENOTEMPTY);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_not_dirs(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/to", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/from/");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enoent_from_path_nonempty_dir_to_path_not_found(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/from/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/from/");
    XIPFS_ASSERT(ret == -ENOENT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enametoolong_from_path_nonempty_dir_to_path_trailing_slash(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/from/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/toooooooooooooooooo"
        "oooooooooooooooooooooooooooooooooooooooooooo");
    XIPFS_ASSERT(ret == -ENAMETOOLONG);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enotdir_from_path_not_dirs(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/from", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/from", "/to");
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_enoent_from_path_not_found(void)
{
    int ret;

    /* test */
    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/");
    XIPFS_ASSERT(ret == -ENOENT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_rename_ok(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* file to file */

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/from", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/to", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_rename(xipfs_nvme0p0, "/from", "/to");
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    /* file to creatable */

    ret = xipfs_open(xipfs_nvme0p0, &desc, "/from", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_rename(xipfs_nvme0p0, "/from", "/to");
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    /* empty directory to empty directory */

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/to/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/");
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    /* empty directory to creatable */

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/");
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    /* non empty directory to empty directory */

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/to/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/");
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    /* non empty directory to creatable */

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_mkdir(xipfs_nvme0p0, "/from/from/", 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_rename(xipfs_nvme0p0, "/from/", "/to/");
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_stat_efault_path(void)
{
    struct stat buf;
    int ret;

    /* test */
    ret = xipfs_stat(xipfs_nvme0p0, NULL, &buf);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_stat_efault_buf(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_stat(xipfs_nvme0p0, "/toto", NULL);
    XIPFS_ASSERT(ret == -EFAULT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_stat_enoent_path_null_char(void)
{
    struct stat buf;
    int ret;

    /* test */
    ret = xipfs_stat(xipfs_nvme0p0, "", &buf);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_stat_enametoolong_path(void)
{
    struct stat buf;
    int ret;

    /* test */
    ret = xipfs_stat(xipfs_nvme0p0, "/totooooooooooooooooooooooo"
            "ooooooooooooooooooooooooooooooooooooo", &buf);
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_stat_enotdir_path(void)
{
    xipfs_file_desc_t desc;
    struct stat buf;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_stat(xipfs_nvme0p0, "/toto/toto", &buf);
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_stat_enoent_path(void)
{
    struct stat buf;
    int ret;

    /* test */
    ret = xipfs_stat(xipfs_nvme0p0, "/toto", &buf);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_stat_ok(void)
{
    xipfs_file_desc_t desc;
    struct stat buf;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_stat(xipfs_nvme0p0, "/toto", &buf);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_statvfs_efault_buf(void)
{
    int ret;

    /* test */
    ret = xipfs_statvfs(xipfs_nvme0p0, NULL, NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_statvfs_ok(void)
{
    struct xipfs_statvfs buf;
    int ret;

    /* test */
    ret = xipfs_statvfs(xipfs_nvme0p0, NULL, &buf);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_new_file_efault_path(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, NULL,
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_new_file_enoent_path_null_char(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_new_file_eisdir_path_root(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -EISDIR);
}

static void test_xipfs_new_file_enametoolong_path(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/totooooooooooooooooooo"
            "ooooooooooooooooooooooooooooooooooooooooo",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_new_file_einval_exec(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto",
            XIPFS_NVM_PAGE_SIZE, 0xffffffff);
    XIPFS_ASSERT(ret == -EINVAL);
}

static void test_xipfs_new_file_einval_negative_size(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto", -1, 0);
    XIPFS_ASSERT(ret == -EINVAL);
}

static void test_xipfs_new_file_eexist_path(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -EEXIST);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_new_file_eisdir_path(void)
{
    int ret;

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -EISDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_new_file_enotdir_path(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto/toto",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_new_file_enoent_path(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto/toto",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_new_file_eisdir_trailing_slash(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto/",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == -EISDIR);
}

static void test_xipfs_new_file_edquot(void)
{
    char path[XIPFS_PATH_MAX];
    size_t i;
    int ret;

    /* initialization */
    for (i = 0; i < xipfs_nvme0p0->page_num; i++)
    {
        (void)sprintf(path, "/%d", i);
        ret = xipfs_new_file(xipfs_nvme0p0, path,
                0, 0);
        XIPFS_ASSERT(ret == 0);
    }

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto",
            0, 0);
    XIPFS_ASSERT(ret == -EDQUOT);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_new_file_ok(void)
{
    int ret;

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto",
            XIPFS_NVM_PAGE_SIZE, 0);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

typedef int (*xipfs_exec_function_t)(xipfs_mount_t *mp, const char *full_path,
                                     char *const argv[],
                                     const void *syscalls[XIPFS_SYSCALL_MAX]);

static void
test_xipfs_exec_efault_path_base(xipfs_exec_function_t exec_function)
{
    int ret;

    /* test */
    ret = exec_function(xipfs_nvme0p0, NULL, NULL, NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void
test_xipfs_exec_enoent_path_null_char_base(xipfs_exec_function_t exec_function)
{
    int ret;

    /* test */
    ret = exec_function(xipfs_nvme0p0, "", NULL, NULL);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void
test_xipfs_exec_eisdir_path_root_base(xipfs_exec_function_t exec_function)
{
    int ret;

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/", NULL, NULL);
    XIPFS_ASSERT(ret == -EISDIR);
}

static void
test_xipfs_exec_enametoolong_path_base(xipfs_exec_function_t exec_function)
{
    int ret;

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/totoooooooooooooooooooooo"
            "oooooooooooooooooooooooooooooooooooooo", NULL, NULL);
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void
test_xipfs_exec_efault_no_syscalls_table_base(xipfs_exec_function_t exec_function)
{
    int ret;

    char *argv[2] = {
        "/toto/", NULL
    };

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/toto/", argv, NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void
test_xipfs_exec_efault_syscalls_table_with_null_base(xipfs_exec_function_t exec_function)
{
    int ret;

    char *argv[2] = {
        "/toto/", NULL
    };
    const void *user_syscalls[XIPFS_SYSCALL_MAX] = {
        NULL
    };

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/toto/", argv, user_syscalls);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void fake_syscall(void) {}
static const void *fake_syscalls[XIPFS_SYSCALL_MAX] = {
    [         XIPFS_SYSCALL_EXIT] = fake_syscall,
    [      XIPFS_SYSCALL_VPRINTF] = fake_syscall,
    [     XIPFS_SYSCALL_GET_TEMP] = fake_syscall,
    [      XIPFS_SYSCALL_ISPRINT] = fake_syscall,
    [       XIPFS_SYSCALL_STRTOL] = fake_syscall,
    [      XIPFS_SYSCALL_GET_LED] = fake_syscall,
    [      XIPFS_SYSCALL_SET_LED] = fake_syscall,
    [    XIPFS_SYSCALL_COPY_FILE] = fake_syscall,
    [XIPFS_SYSCALL_GET_FILE_SIZE] = fake_syscall,
    [       XIPFS_SYSCALL_MEMSET] = fake_syscall
};

static void
test_xipfs_exec_eisdir_path_base(xipfs_exec_function_t exec_function)
{
    int ret;

    char *argv[2] = {
        "/toto/", NULL
    };

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/toto/", argv, fake_syscalls);
    XIPFS_ASSERT(ret == -EISDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void
test_xipfs_exec_enotdir_path_base(xipfs_exec_function_t exec_function)
{
    xipfs_file_desc_t desc;
    int ret;
    char *argv[2] = {
        "/toto/toto", NULL
    };

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto", O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/toto/toto", argv, fake_syscalls);
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_exec_enoent_path_base(xipfs_exec_function_t exec_function)
{
    int ret;
    char *argv[2] = {
        "/toto", NULL
    };

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/toto", argv, fake_syscalls);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_exec_efault_null_args_base(xipfs_exec_function_t exec_function)
{
    int ret;

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/toto", NULL, fake_syscalls);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_exec_efault_args_0_null_base(xipfs_exec_function_t exec_function)
{
    int ret;
    char *argv[1] = { NULL };

    /* test */
    ret = exec_function(xipfs_nvme0p0, "/toto", argv, fake_syscalls);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_execv_efault_path(void)
{
    test_xipfs_exec_efault_path_base(xipfs_execv);
}

static void test_xipfs_execv_enoent_path_null_char(void)
{
    test_xipfs_exec_enoent_path_null_char_base(xipfs_execv);
}

static void test_xipfs_execv_eisdir_path_root(void)
{
    test_xipfs_exec_eisdir_path_root_base(xipfs_execv);
}

static void test_xipfs_execv_enametoolong_path(void)
{
    test_xipfs_exec_enametoolong_path_base(xipfs_execv);
}

static void test_xipfs_execv_efault_no_syscalls_table(void)
{
    test_xipfs_exec_efault_no_syscalls_table_base(xipfs_execv);
}

static void test_xipfs_execv_efault_syscalls_table_with_null(void)
{
    test_xipfs_exec_efault_syscalls_table_with_null_base(xipfs_execv);
}

static void test_xipfs_execv_eisdir_path(void)
{
    test_xipfs_exec_eisdir_path_base(xipfs_execv);
}

static void test_xipfs_execv_enotdir_path(void)
{
    test_xipfs_exec_enotdir_path_base(xipfs_execv);
}

static void test_xipfs_execv_enoent_path(void)
{
    test_xipfs_exec_enoent_path_base(xipfs_execv);
}

static void test_xipfs_execv_efault_null_args(void)
{
    test_xipfs_exec_efault_null_args_base(xipfs_execv);
}

static void test_xipfs_execv_efault_args_0_null(void)
{
    test_xipfs_exec_efault_args_0_null_base(xipfs_execv);
}

#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT

static void test_xipfs_safe_execv_efault_path(void)
{
    test_xipfs_exec_efault_path_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_enoent_path_null_char(void)
{
    test_xipfs_exec_enoent_path_null_char_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_eisdir_path_root(void)
{
    test_xipfs_exec_eisdir_path_root_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_enametoolong_path(void)
{
    test_xipfs_exec_enametoolong_path_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_efault_no_syscalls_table(void)
{
    test_xipfs_exec_efault_no_syscalls_table_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_efault_syscalls_table_with_null(void)
{
    test_xipfs_exec_efault_syscalls_table_with_null_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_eisdir_path(void)
{
    test_xipfs_exec_eisdir_path_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_enotdir_path(void)
{
    test_xipfs_exec_enotdir_path_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_enoent_path(void)
{
    test_xipfs_exec_enoent_path_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_efault_null_args(void)
{
    test_xipfs_exec_efault_null_args_base(xipfs_safe_execv);
}

static void test_xipfs_safe_execv_efault_args_0_null(void)
{
    test_xipfs_exec_efault_args_0_null_base(xipfs_safe_execv);
}

#endif /* XIPFS_ENABLE_SAFE_EXEC_SUPPORT */

#define MINIMAL_FAE_FILENAME "/dev/nvme0p0/minimal.fae"

static void drop_minimal_fae_file(void)
{
    const uint32_t bytesize = sizeof(minimal_fae)/sizeof(minimal_fae[0]);

    /* Drop file into filesystem. */
    int ret = xipfs_extended_driver_new_file(MINIMAL_FAE_FILENAME, bytesize, 1);
    XIPFS_ASSERT(ret >= 0);

    ret = vfs_open(MINIMAL_FAE_FILENAME, O_WRONLY, 0);
    XIPFS_ASSERT(ret >= 0);

    ssize_t write_ret = vfs_write(ret, minimal_fae, bytesize);
    XIPFS_ASSERT(write_ret >= 0);

    vfs_close(ret);
}

static void test_xipfs_extended_driver_execv(void)
{
    drop_minimal_fae_file();

    char *argv[2] = {
        MINIMAL_FAE_FILENAME, NULL
    };

    /* Actual test */
    int ret = xipfs_extended_driver_execv(MINIMAL_FAE_FILENAME, argv);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

#ifdef XIPFS_ENABLE_SAFE_EXEC_SUPPORT

static void test_xipfs_extended_driver_safe_execv(void)
{
    drop_minimal_fae_file();

    char *argv[2] = {
        MINIMAL_FAE_FILENAME, NULL
    };

    /* Actual test */
    int ret = xipfs_extended_driver_safe_execv(MINIMAL_FAE_FILENAME, argv);
    /* ARMv8M is not supported for now, then we expect ret == -1. */
#  if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
    XIPFS_ASSERT(ret == -1);
#  else
    XIPFS_ASSERT(ret == 0);
#  endif

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

#endif /* XIPFS_ENABLE_SAFE_EXEC_SUPPORT */
