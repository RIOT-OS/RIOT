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

/**
 * @def PANIC
 *
 * @brief This macro handles fatal errors
 */
#define PANIC() for (;;);

/**
 * @def NVME0P0_PAGE_NUM
 *
 * @brief The number of flash page for the nvme0p0 file system
 */
#define NVME0P0_PAGE_NUM 20

#define XIPFS_ASSERT(condition)  \
do {                             \
    if (!(condition)) {          \
        printf("Line %d ", __LINE__); \
    }                            \
    assert((condition));           \
}while(0)

/*
 * Allocate a new contiguous space for the nvme0p0 file system
 */
XIPFS_NEW_PARTITION(nvme0p0, "/dev/nvme0p0", NVME0P0_PAGE_NUM);

/*
 * Get a pointer to an xipfs_mount_t from a vfs_xipfs_mount_t
 */
xipfs_mount_t *xipfs_nvme0p0 = NULL; //(xipfs_mount_t *)&nvme0p0.magic;


/*
 * Test function prototypes
 */

static void test_xipfs_close_ebadf_descp(void);
static void test_xipfs_close_ok(void);
static void test_xipfs_fstat_ebadbf_descp(void);
static void test_xipfs_fstat_efault_buf(void);
static void test_xipfs_fstat_ebadbf_xipfs_infos(void);
static void test_xipfs_fstat_ok(void);
static void test_xipfs_lseek_einval_descp(void);
static void test_xipfs_lseek_ebadf_descp(void);
static void test_xipfs_lseek_einval_whence(void);
static void test_xipfs_lseek_einval_new_pos(void);
static void test_xipfs_lseek_ok(void);
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
static void test_xipfs_read_ok(void);
static void test_xipfs_write_ebadf_descp(void);
static void test_xipfs_write_efault_src(void);
static void test_xipfs_write_eacces_flags(void);
static void test_xipfs_write_ok(void);
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
static void test_xipfs_rename_enametoolong_from_path_empty_dir_to_path_creatable_trailing_slash(void);
static void test_xipfs_rename_einval_from_path_empty_dir_to_path_creatable_subdir_itself(void);
static void test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_file(void);
static void test_xipfs_rename_einval_from_path_nonempty_dir_to_path_empty_dir_subdir_itself(void);
static void test_xipfs_rename_enotempty_from_path_nonempty_dir_to_path_nonempty_dir(void);
static void test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_not_dirs(void);
static void test_xipfs_rename_enoent_from_path_nonempty_dir_to_path_not_found(void);
static void test_xipfs_rename_enametoolong_from_path_nonempty_dir_to_path_creatable_trailing_slash(void);
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
    test_xipfs_lseek_einval_new_pos();
    test_xipfs_lseek_ok();

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
    test_xipfs_read_ok();

    /* xipfs_write */
    test_xipfs_write_ebadf_descp();
    test_xipfs_write_efault_src();
    test_xipfs_write_eacces_flags();
    test_xipfs_write_ok();

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
    test_xipfs_rename_enametoolong_from_path_empty_dir_to_path_creatable_trailing_slash();
    test_xipfs_rename_einval_from_path_empty_dir_to_path_creatable_subdir_itself();
    test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_file();
    test_xipfs_rename_einval_from_path_nonempty_dir_to_path_empty_dir_subdir_itself();
    test_xipfs_rename_enotempty_from_path_nonempty_dir_to_path_nonempty_dir();
    test_xipfs_rename_enotdir_from_path_nonempty_dir_to_path_not_dirs();
    test_xipfs_rename_enoent_from_path_nonempty_dir_to_path_not_found();
    test_xipfs_rename_enametoolong_from_path_nonempty_dir_to_path_creatable_trailing_slash();
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

    /* Should not fail unless there is a corrupted data
     * structure or a flash memory failure. */
    ret = xipfs_umount(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);

    printf("Tests finished.\n");
}



/*
 * Entry point
 */

int main(void)
{
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


    for (;;);
}

/*
 * Test function implementations
 */

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
    int ret;

    /* test */
    ret = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_SET);
    XIPFS_ASSERT(ret == -EINVAL);
}

static void test_xipfs_lseek_ebadf_descp(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_SET);
    XIPFS_ASSERT(ret == -EBADF);
}

static void test_xipfs_lseek_einval_whence(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_lseek(xipfs_nvme0p0, &desc, 0, 0xffffffff);
    XIPFS_ASSERT(ret == -EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_einval_new_pos(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_lseek(xipfs_nvme0p0, &desc, 0xffffffff,
            SEEK_SET);
    XIPFS_ASSERT(ret == -EINVAL);

    /* clean up */
    ret = xipfs_close(xipfs_nvme0p0, &desc);
    XIPFS_ASSERT(ret == 0);

    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_lseek_ok(void)
{
    xipfs_file_desc_t desc;
    int ret;

    /* initialization */
    ret = xipfs_open(xipfs_nvme0p0, &desc, "/toto",
            O_CREAT, 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_lseek(xipfs_nvme0p0, &desc, 0, SEEK_SET);
    XIPFS_ASSERT(ret == 0);

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
        (void)sprintf(path, "/%d", i);

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

static void test_xipfs_read_ok(void)
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
    XIPFS_ASSERT(ret == 0);

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

static void test_xipfs_rename_enametoolong_from_path_empty_dir_to_path_creatable_trailing_slash(void)
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

static void test_xipfs_rename_enametoolong_from_path_nonempty_dir_to_path_creatable_trailing_slash(void)
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
    for (i = 0; i < xipfs_nvme0p0->page_num; i++) {
        (void)sprintf(path, "/%d", i);
        ret = xipfs_new_file(xipfs_nvme0p0, path,
                XIPFS_NVM_PAGE_SIZE, 0);
        XIPFS_ASSERT(ret == 0);
    }

    /* test */
    ret = xipfs_new_file(xipfs_nvme0p0, "/toto",
            XIPFS_NVM_PAGE_SIZE, 0);
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

static void test_xipfs_execv_efault_path(void)
{
    int ret;

    /* test */
    ret = xipfs_execv(xipfs_nvme0p0, NULL, NULL, NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_execv_enoent_path_null_char(void)
{
    int ret;

    /* test */
    ret = xipfs_execv(xipfs_nvme0p0, "", NULL, NULL);
    XIPFS_ASSERT(ret == -ENOENT);
}

static void test_xipfs_execv_eisdir_path_root(void)
{
    int ret;

    /* test */
    ret = xipfs_execv(xipfs_nvme0p0, "/", NULL, NULL);
    XIPFS_ASSERT(ret == -EISDIR);
}

static void test_xipfs_execv_enametoolong_path(void)
{
    int ret;

    /* test */
    ret = xipfs_execv(xipfs_nvme0p0, "/totoooooooooooooooooooooo"
            "oooooooooooooooooooooooooooooooooooooo", NULL, NULL);
    XIPFS_ASSERT(ret == -ENAMETOOLONG);
}

static void test_xipfs_execv_efault_no_syscalls_table(void) {
    int ret;

    char *argv[2] = {
        "/toto/", NULL
    };

    /* test */
    ret = xipfs_execv(xipfs_nvme0p0, "/toto/", argv, NULL);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void test_xipfs_execv_efault_syscalls_table_with_null(void) {
    int ret;

    char *argv[2] = {
        "/toto/", NULL
    };
    const void *user_syscalls[XIPFS_USER_SYSCALL_MAX] = {
        NULL
    };

    /* test */
    ret = xipfs_execv(xipfs_nvme0p0, "/toto/", argv, user_syscalls);
    XIPFS_ASSERT(ret == -EFAULT);
}

static void fake_syscall(void) {}
static const void *fake_user_syscalls[XIPFS_USER_SYSCALL_MAX] = {
    [       XIPFS_USER_SYSCALL_PRINTF] = fake_syscall,
    [     XIPFS_USER_SYSCALL_GET_TEMP] = fake_syscall,
    [      XIPFS_USER_SYSCALL_ISPRINT] = fake_syscall,
    [       XIPFS_USER_SYSCALL_STRTOL] = fake_syscall,
    [      XIPFS_USER_SYSCALL_GET_LED] = fake_syscall,
    [      XIPFS_USER_SYSCALL_SET_LED] = fake_syscall,
    [    XIPFS_USER_SYSCALL_COPY_FILE] = fake_syscall,
    [XIPFS_USER_SYSCALL_GET_FILE_SIZE] = fake_syscall,
    [       XIPFS_USER_SYSCALL_MEMSET] = fake_syscall
};

static void test_xipfs_execv_eisdir_path(void)
{
    int ret;

    char *argv[2] = {
        "/toto/", NULL
    };

    /* initialization */
    ret = xipfs_mkdir(xipfs_nvme0p0, "/toto/", 0);
    XIPFS_ASSERT(ret == 0);

    /* test */
    ret = xipfs_execv(xipfs_nvme0p0, "/toto/", argv, fake_user_syscalls);
    XIPFS_ASSERT(ret == -EISDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_execv_enotdir_path(void)
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
    ret = xipfs_execv(xipfs_nvme0p0, "/toto/toto", argv, fake_user_syscalls);
    XIPFS_ASSERT(ret == -ENOTDIR);

    /* clean up */
    ret = xipfs_format(xipfs_nvme0p0);
    XIPFS_ASSERT(ret == 0);
}

static void test_xipfs_execv_enoent_path(void)
{
    int ret;
    char *argv[2] = {
        "/toto", NULL
    };

    /* test */
    ret = xipfs_execv(xipfs_nvme0p0, "/toto", argv, fake_user_syscalls);
    XIPFS_ASSERT(ret == -ENOENT);
}
