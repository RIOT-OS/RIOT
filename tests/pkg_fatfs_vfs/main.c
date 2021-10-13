/*
 * Copyright (C) 2017 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       test application for using fatfs with vfs
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @}
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "fs/fatfs.h"
#include "vfs.h"
#include "mtd.h"
#include "board.h"

#include "kernel_defines.h"

#ifdef MODULE_MTD_SDCARD
#include "mtd_sdcard.h"
#include "sdcard_spi.h"
#include "sdcard_spi_params.h"
#endif

#if FATFS_FFCONF_OPT_FS_NORTC == 0
#include "periph/rtc.h"
#endif

#define MNT_PATH  "/test"
#define FNAME1 "TEST.TXT"
#define FNAME2 "NEWFILE.TXT"
#define FNAME_RNMD  "RENAMED.TXT"
#define FNAME_NXIST "NOFILE.TXT"
#define FULL_FNAME1 (MNT_PATH "/" FNAME1)
#define FULL_FNAME2 (MNT_PATH "/" FNAME2)
#define FULL_FNAME_RNMD  (MNT_PATH "/" FNAME_RNMD)
#define FULL_FNAME_NXIST (MNT_PATH "/" FNAME_NXIST)
#define DIR_NAME "SOMEDIR"

static const char test_txt[]  = "the test file content 123 abc";
static const char test_txt2[] = "another text";
static const char test_txt3[] = "hello world for vfs";

static fatfs_desc_t fatfs = {
    .vol_idx = 0
};

static vfs_mount_t _test_vfs_mount = {
    .mount_point = MNT_PATH,
    .fs = &fatfs_file_system,
    .private_data = (void *)&fatfs,
};

/* provide mtd devices for use within diskio layer of fatfs */
mtd_dev_t *fatfs_mtd_devs[FF_VOLUMES];

#ifdef MODULE_MTD_NATIVE
/* mtd device for native is provided in boards/native/board_init.c */
extern mtd_dev_t *mtd0;
#elif MODULE_MTD_SDCARD
#define SDCARD_SPI_NUM ARRAY_SIZE(sdcard_spi_params)
extern sdcard_spi_t sdcard_spi_devs[SDCARD_SPI_NUM];
mtd_sdcard_t mtd_sdcard_devs[SDCARD_SPI_NUM];
/* always default to first sdcard*/
static mtd_dev_t *mtd1 = (mtd_dev_t*)&mtd_sdcard_devs[0];
#endif

static void print_test_result(const char *test_name, int ok)
{
    printf("%s:[%s]\n", test_name, ok ? "OK" : "FAILED");
}

static void test_mount(void)
{
    print_test_result("test_mount__mount", vfs_mount(&_test_vfs_mount) == 0);
    print_test_result("test_mount__umount", vfs_umount(&_test_vfs_mount) == 0);
}

static void test_open(void)
{
    int fd;
    print_test_result("test_open__mount", vfs_mount(&_test_vfs_mount) == 0);

    /* try to open file that doesn't exist */
    fd = vfs_open(FULL_FNAME_NXIST, O_RDONLY, 0);
    print_test_result("test_open__open", fd == -ENOENT);

    /* open file with RO, WO and RW access */
    fd = vfs_open(FULL_FNAME1, O_RDONLY, 0);
    print_test_result("test_open__open_ro", fd >= 0);
    print_test_result("test_open__close_ro", vfs_close(fd) == 0);
    fd = vfs_open(FULL_FNAME1, O_WRONLY, 0);
    print_test_result("test_open__open_wo", fd >= 0);
    print_test_result("test_open__close_wo", vfs_close(fd) == 0);
    fd = vfs_open(FULL_FNAME1, O_RDWR, 0);
    print_test_result("test_open__open_rw", fd >= 0);
    print_test_result("test_open__close_rw", vfs_close(fd) == 0);

    print_test_result("test_open__umount", vfs_umount(&_test_vfs_mount) == 0);
}

static void test_rw(void)
{
    char buf[sizeof(test_txt) + sizeof(test_txt2)];
    int fd;
    ssize_t nr, nw;
    off_t new_pos;

    print_test_result("test_rw__mount", vfs_mount(&_test_vfs_mount) == 0);

    fd = vfs_open(FULL_FNAME1, O_RDONLY, 0);
    print_test_result("test_rw__open_ro", fd >= 0);

    /* compare file content with expected value */
    memset(buf, 0, sizeof(buf));
    nr = vfs_read(fd, buf, sizeof(test_txt));
    print_test_result("test_rw__read_ro", (nr == sizeof(test_txt)) &&
                      (strncmp(buf, test_txt, sizeof(test_txt)) == 0));

    /* try to write to RO file (success if no bytes are actually written) */
    nw = vfs_write(fd, test_txt2, sizeof(test_txt2));
    print_test_result("test_rw__write_ro", nw <= 0);
    print_test_result("test_rw__close_ro", vfs_close(fd) == 0);

    fd = vfs_open("/test/test.txt", O_WRONLY, 0);
    print_test_result("test_rw__open_wo", fd >= 0);

    /* try to read from WO file (success if no bytes are actually read) */
    nr = vfs_read(fd, buf, sizeof(test_txt));
    print_test_result("test_rw__read_wo", nr <= 0);

    print_test_result("test_rw__close_wo", vfs_close(fd) == 0);

    memset(buf, 0, sizeof(buf));
    fd = vfs_open(FULL_FNAME1, O_RDWR, 0);
    print_test_result("test_rw__open_rw", fd >= 0);

    /* read file content and compare it to the expected value */
    nr = vfs_read(fd, buf, sizeof(test_txt));
    print_test_result("test_rw__read_rw", (nr == sizeof(test_txt)) &&
                      (strncmp(buf, test_txt, sizeof(test_txt)) == 0));

    /* write to the file (text should be appended to the end of file) */
    nw = vfs_write(fd, test_txt2, sizeof(test_txt2));
    print_test_result("test_rw__write_rw", nw == sizeof(test_txt2));

    /* seek to start of file */
    new_pos = vfs_lseek(fd, 0, SEEK_SET);
    print_test_result("test_rw__lseek", new_pos == 0);

    /* read file content and compare to expected value */
    memset(buf, 0, sizeof(buf));
    nr = vfs_read(fd, buf, sizeof(buf));
    print_test_result("test_rw__read_rw", (nr == sizeof(buf)) &&
                      (strncmp(buf, test_txt, sizeof(test_txt)) == 0) &&
                      (strncmp(&buf[sizeof(test_txt)],
                               test_txt2,
                               sizeof(test_txt2)) == 0));

    print_test_result("test_rw__close_rw", vfs_close(fd) == 0);

    /* create new file */
    fd = vfs_open(FULL_FNAME2, O_RDWR | O_CREAT, 0);
    print_test_result("test_rw__open_rwc", fd >= 0);

    /* write to the new file, read it's content and compare to expected value */
    nw = vfs_write(fd, test_txt3, sizeof(test_txt3));
    print_test_result("test_rw__write_rwc", nw == sizeof(test_txt3));

    new_pos = vfs_lseek(fd, 0, SEEK_SET);
    print_test_result("test_rw__lseek_rwc", new_pos == 0);

    memset(buf, 0, sizeof(buf));
    nr = vfs_read(fd, buf, sizeof(test_txt3));
    print_test_result("test_rw__read_rwc", (nr == sizeof(test_txt3)) &&
                      (strncmp(buf, test_txt3, sizeof(test_txt3)) == 0));

    print_test_result("test_rw__close_rwc", vfs_close(fd) == 0);
    print_test_result("test_rw__umount", vfs_umount(&_test_vfs_mount) == 0);
}

static void test_dir(void)
{
    vfs_DIR dir;
    vfs_dirent_t entry;
    vfs_dirent_t entry2;

    print_test_result("test_dir__mount", vfs_mount(&_test_vfs_mount) == 0);
    print_test_result("test_dir__opendir", vfs_opendir(&dir, MNT_PATH) == 0);
    print_test_result("test_dir__readdir1", vfs_readdir(&dir, &entry) == 1);
    print_test_result("test_dir__readdir2", vfs_readdir(&dir, &entry2) == 1);

    print_test_result("test_dir__readdir_name",
                      ((strncmp(FNAME1, entry.d_name, sizeof(FNAME1)) == 0) &&
                       (strncmp(FNAME2, entry2.d_name, sizeof(FNAME2)) == 0))
                      ||
                      ((strncmp(FNAME1, entry2.d_name, sizeof(FNAME1)) == 0) &&
                       (strncmp(FNAME2, entry.d_name, sizeof(FNAME2)) == 0)));

    print_test_result("test_dir__readdir3", vfs_readdir(&dir, &entry2) == 0);
    print_test_result("test_dir__closedir", vfs_closedir(&dir) == 0);
    print_test_result("test_dir__umount", vfs_umount(&_test_vfs_mount) == 0);
}

static void test_rename(void)
{
    vfs_DIR dir;
    vfs_dirent_t entry;
    vfs_dirent_t entry2;

    print_test_result("test_rename__mount", vfs_mount(&_test_vfs_mount) == 0);

    print_test_result("test_rename__rename",
                      vfs_rename(FULL_FNAME1, FULL_FNAME_RNMD) == 0);

    print_test_result("test_rename__opendir", vfs_opendir(&dir, MNT_PATH) == 0);
    print_test_result("test_rename__readdir1", vfs_readdir(&dir, &entry) == 1);
    print_test_result("test_rename__readdir2", vfs_readdir(&dir, &entry2) == 1);

    print_test_result("test_rename__check_name",
              ((strncmp(FNAME_RNMD, entry.d_name, sizeof(FNAME_RNMD)) == 0) &&
               (strncmp(FNAME2, entry2.d_name, sizeof(FNAME2)) == 0))
              ||
              ((strncmp(FNAME_RNMD, entry2.d_name, sizeof(FNAME_RNMD)) == 0) &&
               (strncmp(FNAME2, entry.d_name, sizeof(FNAME2)) == 0)));

    print_test_result("test_rename__readdir3", vfs_readdir(&dir, &entry2) == 0);
    print_test_result("test_rename__closedir", vfs_closedir(&dir) == 0);
    print_test_result("test_rename__umount", vfs_umount(&_test_vfs_mount) == 0);
}

static void test_unlink(void)
{
    vfs_DIR dir;
    vfs_dirent_t entry;

    print_test_result("test_unlink__mount", vfs_mount(&_test_vfs_mount) == 0);
    print_test_result("test_unlink__unlink1", vfs_unlink(FULL_FNAME2) == 0);
    print_test_result("test_unlink__unlink2", vfs_unlink(FULL_FNAME_RNMD) == 0);
    print_test_result("test_unlink__opendir", vfs_opendir(&dir, MNT_PATH) == 0);
    print_test_result("test_unlink__readdir", vfs_readdir(&dir, &entry) == 0);
    print_test_result("test_unlink__closedir", vfs_closedir(&dir) == 0);
    print_test_result("test_unlink__umount", vfs_umount(&_test_vfs_mount) == 0);
}

static void test_mkrmdir(void)
{
    vfs_DIR dir;

    print_test_result("test_mkrmdir__mount", vfs_mount(&_test_vfs_mount) == 0);

    print_test_result("test_mkrmdir__mkdir",
                      vfs_mkdir(MNT_PATH"/"DIR_NAME, 0) == 0);

    print_test_result("test_mkrmdir__opendir1",
                      vfs_opendir(&dir, MNT_PATH"/"DIR_NAME) == 0);

    print_test_result("test_mkrmdir__closedir", vfs_closedir(&dir) == 0);

    print_test_result("test_mkrmdir__rmdir",
                      vfs_rmdir(MNT_PATH"/"DIR_NAME) == 0);

    print_test_result("test_mkrmdir__opendir2",
                      vfs_opendir(&dir, MNT_PATH"/"DIR_NAME) < 0);

    print_test_result("test_mkrmdir__umount",
                      vfs_umount(&_test_vfs_mount) == 0);
}

static void test_create(void)
{
    int fd;
    ssize_t nw;
    print_test_result("test_create__mount", vfs_mount(&_test_vfs_mount) == 0);

    fd = vfs_open(FULL_FNAME1, O_WRONLY | O_CREAT, 0);
    print_test_result("test_create__open_wo", fd >= 0);

    nw = vfs_write(fd, test_txt, sizeof(test_txt));
    print_test_result("test_create__write_wo", nw == sizeof(test_txt));
    print_test_result("test_create__close_wo", vfs_close(fd) == 0);

    /* test create if file exists */
    fd = vfs_open(FULL_FNAME1, O_WRONLY | O_CREAT, 0);
    print_test_result("test_create__open_wo2", fd >= 0);

    nw = vfs_write(fd, test_txt, sizeof(test_txt));
    print_test_result("test_create__write_wo2", nw == sizeof(test_txt));
    print_test_result("test_create__close_wo2", vfs_close(fd) == 0);

    print_test_result("test_create__umount", vfs_umount(&_test_vfs_mount) == 0);
}

static void test_fstat(void)
{
    int fd;
    struct stat stat_buf;

    print_test_result("test_stat__mount", vfs_mount(&_test_vfs_mount) == 0);

    fd = vfs_open(FULL_FNAME1, O_WRONLY | O_TRUNC, 0);
    print_test_result("test_stat__open", fd >= 0);
    print_test_result("test_stat__write",
            vfs_write(fd, test_txt, sizeof(test_txt)) == sizeof(test_txt));
    print_test_result("test_stat__close", vfs_close(fd) == 0);

    print_test_result("test_stat__direct", vfs_stat(FULL_FNAME1, &stat_buf) == 0);

    fd = vfs_open(FULL_FNAME1, O_RDONLY, 0);
    print_test_result("test_stat__open", fd >= 0);
    print_test_result("test_stat__stat", vfs_fstat(fd, &stat_buf) == 0);
    print_test_result("test_stat__close", vfs_close(fd) == 0);
    print_test_result("test_stat__size", stat_buf.st_size == sizeof(test_txt));
    print_test_result("test_stat__umount", vfs_umount(&_test_vfs_mount) == 0);
}

#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
static void test_libc(void)
{
    FILE* fl;
    char buf[sizeof(test_txt) + sizeof(test_txt2)];
    print_test_result("test_libc__mount", vfs_mount(&_test_vfs_mount) == 0);

    /* try to open file that doesn't exist */
    fl = fopen(FULL_FNAME_NXIST, "r");
    print_test_result("test_libc__fopen", fl == NULL);
    if (fl) {
        fclose(fl);
    }

    /* create new file write and check content */
    remove(FULL_FNAME2);
    fl = fopen(FULL_FNAME2, "w+");
    print_test_result("test_libc__fopen_w", fl != NULL);
    if (fl) {
        print_test_result("test_libc__fputs_w", fputs(test_txt, fl) >= 0);
        rewind(fl);
        print_test_result("test_libc__fread_w",
                fread(buf, sizeof(*buf), sizeof(buf), fl) > 0);
        print_test_result("test_libc__strcmp_w", strcmp(test_txt, buf) == 0);
        print_test_result("test_libc__fclose_w", fclose(fl) == 0);
    }

    /* cppcheck-suppress resourceLeak
     * (reason: cppcheck <2.0 reports a false positive here) */
    fl = fopen(FULL_FNAME2, "r"); /* open file RO */
    print_test_result("test_libc__fopen_r", fl != NULL);
    if (fl) {
        print_test_result("test_libc__fclose_r", fclose(fl) == 0);
    }

    /* remove file */
    print_test_result("test_libc__remove", remove(FULL_FNAME2) == 0);

    /* append to non existing file */
    fl = fopen(FULL_FNAME2, "a");
    print_test_result("test_libc__fopen_a", fl != NULL);
    if (fl) {
        print_test_result("test_libc__fputs_a", fputs(test_txt, fl) >= 0);
        print_test_result("test_libc__fclose_a", fclose(fl) == 0);
    }

    /* append to existing file and check content */
    fl = fopen(FULL_FNAME2, "a+");
    print_test_result("test_libc__fopen_a2", fl != NULL);
    if (fl) {
        print_test_result("test_libc__fputs_a2", fputs(test_txt2, fl) >= 0);
        rewind(fl);
        print_test_result("test_libc__fread_a2",
                fread(buf, sizeof(*buf), sizeof(buf), fl) > 0);
        print_test_result("test_libc__strcmp_a2",
                strncmp(test_txt, buf, strlen(test_txt)) == 0);
        print_test_result("test_libc__strcmp_a2", strncmp(test_txt2,
                    &buf[strlen(test_txt)], strlen(test_txt2)) == 0);
        print_test_result("test_libc__fclose_a2", fclose(fl) == 0);
    }
    print_test_result("test_libc__remove", remove(FULL_FNAME2) == 0);

    print_test_result("test_libc__umount", vfs_umount(&_test_vfs_mount) == 0);
}
#endif

int main(void)
{
#if MODULE_MTD_SDCARD
    for(unsigned int i = 0; i < SDCARD_SPI_NUM; i++){
        mtd_sdcard_devs[i].base.driver = &mtd_sdcard_driver;
        mtd_sdcard_devs[i].sd_card = &sdcard_spi_devs[i];
        mtd_sdcard_devs[i].params = &sdcard_spi_params[i];
        fatfs_mtd_devs[i] = &mtd_sdcard_devs[i].base;
        mtd_init(&mtd_sdcard_devs[i].base);
    }
#endif

#if defined(MODULE_MTD_NATIVE) || defined(MODULE_MTD_MCI)
    fatfs_mtd_devs[fatfs.vol_idx] = mtd0;
#else
    fatfs_mtd_devs[fatfs.vol_idx] = mtd1;
#endif

    printf("Tests for FatFs over VFS - test results will be printed "
           "in the format test_name:result\n");

    test_mount();
    test_open();
    test_rw();
    test_dir();
    test_rename();
    test_unlink();
    test_mkrmdir();
    test_create();
    test_fstat();
#if defined(MODULE_NEWLIB) || defined(MODULE_PICOLIBC)
    test_libc();
#endif

    printf("Test end.\n");
    return 0;
}
