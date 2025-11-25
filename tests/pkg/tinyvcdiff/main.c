/*
 * Copyright (C) 2022 Juergen Fitschen
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
 * @brief       tinyvcdiff library backend tests
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "embUnit.h"
#include "vcdiff.h"
#include "vcdiff_mtd.h"
#include "vcdiff_vfs.h"
#include "fakemtd.h"
#include "fs/littlefs2_fs.h"

/* Generated using open-vcdiff:
 * $ echo "Hello world! I hope you are doing well ..." >source.bin
 * $ echo "Hello universe! I hope you are doing well ..." >target.bin
 * $ vcdiff delta -interleaved -dictionary source.bin <target.bin >delta.bin */
#include "blob/source.bin.h"
#include "blob/target.bin.h"
#include "blob/delta.bin.h"

static vcdiff_t vcdiff;
static fake_mtd_t storage_a = FAKE_MTD_INIT;
static fake_mtd_t storage_b = FAKE_MTD_INIT;
static littlefs2_desc_t fs = { .dev = &storage_a.mtd };
static vfs_mount_t mnt = {
    .mount_point = "/mnt",
    .fs = &littlefs2_file_system,
    .private_data = &fs,
};
static uint8_t target_buf[sizeof(target_bin)];

static void test_tinyvcdiff_mtd(void)
{
    int rc;

    mtd_dev_t * source_mtd = &storage_a.mtd;
    vcdiff_mtd_t source_vcdiff = VCDIFF_MTD_INIT(source_mtd);

    mtd_dev_t * target_mtd = &storage_b.mtd;
    vcdiff_mtd_t target_vcdiff = VCDIFF_MTD_INIT(target_mtd);

    /* write source data to fake-mtd */
    mtd_write(source_mtd, source_bin, 0, source_bin_len);

    /* setup vcdiff */
    vcdiff_init(&vcdiff);
    vcdiff_set_source_driver(&vcdiff, &vcdiff_mtd_driver, &source_vcdiff);
    vcdiff_set_target_driver(&vcdiff, &vcdiff_mtd_driver, &target_vcdiff);

    /* apply diff */
    rc = vcdiff_apply_delta(&vcdiff, delta_bin, delta_bin_len);
    TEST_ASSERT_EQUAL_INT(0, rc);
    rc = vcdiff_finish(&vcdiff);
    TEST_ASSERT_EQUAL_INT(0, rc);

    /* check reconstructed target */
    mtd_read(target_mtd, target_buf, 0, sizeof(target_buf));
    TEST_ASSERT_EQUAL_INT(0, memcmp(target_bin, target_buf, sizeof(target_buf)));
}

static void test_tinyvcdiff_vfs(void)
{
    int rc;
    int source_fd, target_fd;

    vfs_format(&mnt);
    vfs_mount(&mnt);

    /* write source data to fs */
    source_fd = vfs_open("/mnt/source", O_WRONLY | O_CREAT, 0);
    vfs_write(source_fd, source_bin, source_bin_len);
    vfs_close(source_fd);

    /* open source and target file */
    source_fd = vfs_open("/mnt/source", O_RDONLY, 0);
    vcdiff_vfs_t source_vcdiff = VCDIFF_VFS_INIT(source_fd);
    target_fd = vfs_open("/mnt/target", O_WRONLY | O_CREAT, 0);
    vcdiff_vfs_t target_vcdiff = VCDIFF_VFS_INIT(target_fd);

    /* setup vcdiff */
    vcdiff_init(&vcdiff);
    vcdiff_set_source_driver(&vcdiff, &vcdiff_vfs_driver, &source_vcdiff);
    vcdiff_set_target_driver(&vcdiff, &vcdiff_vfs_driver, &target_vcdiff);

    /* apply diff */
    rc = vcdiff_apply_delta(&vcdiff, delta_bin, delta_bin_len);
    TEST_ASSERT_EQUAL_INT(0, rc);
    rc = vcdiff_finish(&vcdiff);
    TEST_ASSERT_EQUAL_INT(0, rc);

    vfs_close(source_fd);
    vfs_close(target_fd);

    /* check reconstructed target */
    memset(target_buf, 0, sizeof(target_buf));
    target_fd = vfs_open("/mnt/target", O_RDONLY, 0);
    vfs_read(target_fd, target_buf, sizeof(target_buf));
    TEST_ASSERT_EQUAL_INT(0, memcmp(target_bin, target_buf, sizeof(target_buf)));
    vfs_close(target_fd);

    vfs_umount(&mnt, false);
}

Test *tests_tinyvcdiff(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_tinyvcdiff_mtd),
        new_TestFixture(test_tinyvcdiff_vfs),
    };

    EMB_UNIT_TESTCALLER(tinycvdiff_tests, NULL, NULL, fixtures);
    return (Test*)&tinycvdiff_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_tinyvcdiff());
    TESTS_END();
    return 0;
}
