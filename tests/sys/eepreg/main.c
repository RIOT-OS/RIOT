/*
 * SPDX-FileCopyrightText: 2018 Acutam Automation, LLC
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       eepreg test application
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "eepreg.h"
#include "mtd.h"

#ifndef TEST_EEPREG_MTD
#  define TEST_EEPREG_MTD       MTD_0
#endif

/* works anyway only up to 4GiB because of 32bit position in API */
#define TEST_EEPROM_SIZE      ((TEST_EEPREG_MTD)->sector_count * \
                               (TEST_EEPREG_MTD)->pages_per_sector * \
                               (TEST_EEPREG_MTD)->page_size)

#define DAT_START    (TEST_EEPROM_SIZE - EEPROM_RESERV_CPU_HI \
                      - EEPROM_RESERV_BOARD_HI - 1)

#define ENT1_NAME    "foo"
#define ENT1_SIZE    (12U)
#define ENT2_NAME    "bar"
#define ENT2_SIZE    (34U)
#define DATA         "spam and eggs"

int eepreg_iter_cb(mtd_dev_t *dev, char *name, void *arg)
{
    (void)dev;
    (void)arg;

    printf("%s ", name);

    return 0;
}

int main(void)
{
    int ret;
    uint32_t tmp1, tmp2, tmp3;
    char data[sizeof(DATA)];
    mtd_dev_t *dev = TEST_EEPREG_MTD;
    assert(dev);
    mtd_init(dev);

    puts("EEPROM registry (eepreg) test routine");

    printf("Testing new registry creation: ");

    printf("reset ");
    ret = eepreg_reset(dev);
    if (ret < 0) {
        puts("[FAILED]");
        return 1;
    }

    printf("check ");
    ret = eepreg_check(dev);
    if (ret < 0) {
        puts("[FAILED]");
        return 1;
    }

    puts("[SUCCESS]");

    printf("Testing writing and reading entries: ");

    printf("add ");
    ret = eepreg_add(dev, &tmp1, ENT1_NAME, ENT1_SIZE);
    if (ret < 0 || tmp1 != DAT_START - ENT1_SIZE) {
        puts("[FAILED]");
        return 1;
    }

    printf("write ");
    ret = eepreg_write(dev, &tmp2, ENT2_NAME, ENT2_SIZE);
    if (ret < 0 || tmp2 != DAT_START - ENT1_SIZE - ENT2_SIZE) {
        puts("[FAILED]");
        return 1;
    }

    /* read via add */
    printf("add ");
    ret = eepreg_add(dev, &tmp3, ENT1_NAME, ENT1_SIZE);
    if (ret < 0 || tmp1 != tmp3) {
        puts("[FAILED]");
        return 1;
    }

    printf("read ");
    ret = eepreg_read(dev, &tmp1, ENT2_NAME);
    if (ret < 0 || tmp1 != tmp2) {
        puts("[FAILED]");
        return 1;
    }

    puts("[SUCCESS]");

    printf("Testing detection of conflicting size: ");

    printf("add ");
    ret = eepreg_add(dev, &tmp1, ENT1_NAME, ENT1_SIZE + 1);
    if (ret != -EADDRINUSE) {
        puts("[FAILED]");
        return 1;
    }

    puts("[SUCCESS]");

    printf("Testing calculation of lengths: ");

    printf("len ");
    ret = eepreg_len(dev, &tmp1, ENT1_NAME);
    if (ret < 0 || tmp1 != ENT1_SIZE) {
        puts("[FAILED]");
        return 1;
    }

    printf("len ");
    ret = eepreg_len(dev, &tmp2, ENT2_NAME);
    if (ret < 0 || tmp2 != ENT2_SIZE) {
        puts("[FAILED]");
        return 1;
    }

    puts("[SUCCESS]");

    printf("Testing of successful data move after rm: ");

    printf("rm ");
    eepreg_read(dev, &tmp1, ENT1_NAME);
    eepreg_read(dev, &tmp2, ENT2_NAME);
    mtd_write(dev, (uint8_t *)DATA, tmp2, sizeof(DATA));
    ret = eepreg_rm(dev, ENT1_NAME);
    if (ret < 0) {
        puts("[FAILED]");
        return 1;
    }

    printf("read ");
    ret = eepreg_read(dev, &tmp3, ENT2_NAME);
    if (ret < 0 || tmp3 != (DAT_START - ENT2_SIZE)) {
        puts("[FAILED]");
        return 1;
    }

    printf("data ");
    mtd_read(dev, data, tmp3, sizeof(DATA));
    if (strcmp(data, DATA) != 0) {
        puts("[FAILED]");
        return 1;
    }

    puts("[SUCCESS]");

    printf("Testing of free space change after write: ");

    printf("free ");
    ret = eepreg_free(dev, &tmp1);
    if (ret < 0) {
        puts("[FAILED]");
        return 1;
    }

    printf("add ");
    ret = eepreg_add(dev, &tmp3, ENT1_NAME, ENT1_SIZE);
    if (ret < 0) {
        puts("[FAILED]");
        return 1;
    }

    printf("free ");
    ret = eepreg_free(dev, &tmp2);
    if (ret < 0
        || tmp1 != (tmp2 + ENT1_SIZE + sizeof(ENT1_NAME) + eepreg_ptr_len(dev))) {

        puts("[FAILED]");
        return 1;
    }

    puts("[SUCCESS]");

    printf("Testing of iteration over registry: ");

    printf("iter ");
    ret = eepreg_iter(dev, eepreg_iter_cb, NULL);
    if (ret < 0) {
        puts("[FAILED]");
        return 1;
    }

    puts("[SUCCESS]");

    puts("Tests complete!");

    return 0;
}
