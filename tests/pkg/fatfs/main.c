/*
 * Copyright (C) 2016 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
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
 * @brief       Test application for the fatfs package.
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#if FATFS_FFCONF_OPT_FS_NORTC == 0
#include "periph/rtc.h"
#endif
#include "mtd.h"
#include "fatfs_diskio_mtd.h"
#include "shell.h"
#include "container.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#define TEST_FATFS_READ_BUFFER_SIZE 64
#define TEST_FATFS_MAX_LBL_SIZE 64
#define TEST_FATFS_MAX_VOL_STR_LEN 14 /* "-2147483648:/\0" */
#define TEST_FATFS_FIXED_SECTOR_SIZE 512
#define TEST_FATFS_FATENT_OFFSET 2
#define TEST_FATFS_SHIFT_B_TO_GIB 30
#define TEST_FATFS_SHIFT_B_TO_MIB 20
#define TEST_FATFS_RTC_MON_OFFSET 1
#define TEST_FATFS_RTC_YEAR 2000
#define TEST_FATFS_RTC_MON  1
#define TEST_FATFS_RTC_DAY  1
#define TEST_FATFS_RTC_H    0
#define TEST_FATFS_RTC_M    0
#define TEST_FATFS_RTC_S    0
#define IEC_KIBI 1024
#define SI_KILO 1000

FATFS fat_fs; /* FatFs work area needed for each volume */

#ifdef MODULE_MTD_NATIVE

/* mtd device for native is provided in boards/native/board_init.c */
mtd_dev_t *fatfs_mtd_devs[1];

#elif MODULE_MTD_SDMMC

#include "mtd_sdmmc.h"

mtd_dev_t *fatfs_mtd_devs[1];

#elif MODULE_MTD_SDCARD

#include "mtd_sdcard.h"
#include "sdcard_spi_params.h"

#define SDCARD_SPI_NUM ARRAY_SIZE(sdcard_spi_params)

/* sdcard devs are provided by drivers/sdcard_spi/sdcard_spi.c */
extern sdcard_spi_t sdcard_spi_devs[SDCARD_SPI_NUM];
mtd_sdcard_t mtd_sdcard_devs[SDCARD_SPI_NUM];
mtd_dev_t *fatfs_mtd_devs[SDCARD_SPI_NUM];

#endif

#define MTD_NUM ARRAY_SIZE(fatfs_mtd_devs)

static int _mount(int argc, char **argv)
{
    int vol_idx;

    if (argc != 2) {
        printf("usage: %s <volume_idx>\n", argv[0]);
        return -1;
    }

    vol_idx = atoi(argv[1]);

    if (vol_idx > (int)(MTD_NUM-1)) {
        printf("max allowed <volume_idx> is %d\n", (int)(MTD_NUM - 1));
        return -1;
    }

    char volume_str[TEST_FATFS_MAX_VOL_STR_LEN];
    sprintf(volume_str, "%d:/", vol_idx);

    puts("mounting file system image...");

    /* "0:/" points to the root dir of drive 0 */
    FRESULT mountresu = f_mount(&fat_fs, volume_str, 1);
    TCHAR label[TEST_FATFS_MAX_LBL_SIZE];

    if (mountresu == FR_OK) {
        puts("[OK]");
        if (f_getlabel("", label, NULL) == FR_OK) {
            printf("Volume name: %s\n", label);
        }

        FATFS *fs;
        DWORD fre_clust;

        /* Get volume information and free clusters of selected drive */
        if (f_getfree(volume_str, &fre_clust, &fs) != FR_OK) {
            puts("wasn't able to get volume size info!");
        }
        else {

#if FF_MAX_SS == FF_MIN_SS
            uint16_t sector_size = TEST_FATFS_FIXED_SECTOR_SIZE;
#else
            uint16_t sector_size = fs->ssize;
#endif

            uint64_t total_bytes = (fs->n_fatent - TEST_FATFS_FATENT_OFFSET) * fs->csize;
            total_bytes *= sector_size;
            uint64_t free_bytes = fre_clust * fs->csize;
            free_bytes *= sector_size;

            uint32_t to_gib_i = total_bytes >> TEST_FATFS_SHIFT_B_TO_GIB;
            uint32_t to_gib_f = ((((total_bytes >> TEST_FATFS_SHIFT_B_TO_MIB) - to_gib_i * IEC_KIBI)
                                  * SI_KILO) / IEC_KIBI);

            uint32_t fr_gib_i = free_bytes >> TEST_FATFS_SHIFT_B_TO_GIB;
            uint32_t fr_gib_f = ((((free_bytes >> TEST_FATFS_SHIFT_B_TO_MIB) - fr_gib_i * IEC_KIBI)
                                  * SI_KILO) / IEC_KIBI);

            printf("%" PRIu32 ",%03" PRIu32 " GiB of %" PRIu32 ",%03" PRIu32
                   " GiB available\n", fr_gib_i, fr_gib_f, to_gib_i, to_gib_f);
        }
    }
    else {
        puts("[FAILED]");
        switch (mountresu) {
            case FR_NO_FILESYSTEM:
                puts("no filesystem -> you need to format the card to FAT");
                break;
            case FR_DISK_ERR:
                puts("error in the low-level disk driver!");
                break;
            default:
                printf("error %d -> see ff.h of fatfs package for "
                       "further details\n", mountresu);
        }
        return -1;
    }
    return 0;
}

static int _touch(int argc, char **argv)
{
    FIL fd;

    if (argc != 2) {
        printf("usage: %s <filename>\n", argv[0]);
        return -1;
    }

    FRESULT open_resu = f_open(&fd, argv[1], FA_WRITE | FA_CREATE_ALWAYS);
    if (open_resu == FR_OK) {
        FRESULT close_resu = f_close(&fd);
        if (close_resu == FR_OK) {
            puts("[OK]");
            return 0;
        }

        printf("[FAILED] (f_close error %d)\n", close_resu);
        return -2;
    }

    printf("[FAILED] (f_open error %d)\n", open_resu);
    return -3;
}

static int _read(int argc, char **argv)
{
    FIL fd;
    int resu = 0;

    if ((argc < 2) || (argc > 3)) {
        printf("usage: %s <filename> [<len>]\n", argv[0]);
        return -1;
    }

    FRESULT open_resu = f_open(&fd, argv[1], FA_READ | FA_OPEN_EXISTING);
    if (open_resu == FR_OK) {
        UINT read_chunk;
        uint32_t len = ((argc == 3) ? (uint32_t)atoi(argv[2]) : f_size(&fd));

        char buffer[TEST_FATFS_READ_BUFFER_SIZE];

        for (uint32_t read = 0; read < len; read += read_chunk) {
            uint32_t to_read = len - read;

            if (to_read > sizeof(buffer)) {
                to_read = sizeof(buffer);
            }

            FRESULT lseek_resu = f_lseek(&fd, read);

            if (lseek_resu != FR_OK) {
                printf("[FAILED] f_lseek error %d\n", lseek_resu);
                resu = -3;
                break;
            }

            FRESULT read_resu = f_read(&fd, buffer, to_read, &read_chunk);

            if (read_resu != FR_OK) {
                printf("[FAILED] (f_read error %d)\n", read_resu);
                resu = -4;
                break;
            }

            for (uint32_t i = 0; i < read_chunk; i++) {
                printf("%c", buffer[i]);
            }
        }
        puts("");

        FRESULT close_resu = f_close(&fd);

        if (close_resu == FR_OK) {
            puts("[OK]");
            resu = 0;
        }
        else {
            printf("[FAILED] (f_close error %d)\n", open_resu);
            resu = -5;
        }

    }
    else {
        printf("[FAILED] (f_open error %d)\n", open_resu);
        resu = -2;
    }

    return resu;
}

static int _write(int argc, char **argv)
{
    FIL fd;
    UINT bw;

    if (argc != 3) {
        printf("usage: %s <filename> <string>\n", argv[0]);
        return -1;
    }

    uint32_t len = strlen(argv[2]);
    FRESULT open_resu = f_open(&fd, argv[1], FA_WRITE | FA_OPEN_APPEND);

    if (open_resu == FR_OK) {
        printf("writing %" PRId32 " bytes to %s ...", len, argv[1]);
        FRESULT write_resu = f_write(&fd, argv[2], len, &bw);

        if ((write_resu != FR_OK) || (bw < len)) {
            printf("[FAILED] (f_write error %d)\n", write_resu);
            return -2;
        }
        else {
            FRESULT close_resu = f_close(&fd);

            if (close_resu == FR_OK) {
                puts("[OK]");
                return 0;
            }

            printf("[FAILED] (f_close error %d)\n", open_resu);
            return -3;
        }
    }

    printf("[FAILED] (f_open error %d)\n", open_resu);
    return -1;
}

static int _ls(int argc, char **argv)
{
    char *path;
    FRESULT res;
    DIR dir;
    static FILINFO fno;

    if (argc == 2) {
        path = argv[1];
    }
    else {
        path = "/";
    }

    res = f_opendir(&dir, path);/* Open the directory */

    if (res == FR_OK) {
        while (true) {
            res = f_readdir(&dir, &fno);    /* Read a directory item */

            if ((res != FR_OK) || fno.fname[0] == 0) {
                break;                      /* Break on error or end of dir */
            }

            if (fno.fattrib & AM_DIR) {     /* if this element is a directory */
                printf("%s%s/\n", path, fno.fname);
            }
            else {
                printf("%s/%s\n", path, fno.fname);
            }
        }

        f_closedir(&dir);
        return 0;
    }

    printf("[FAILED] error %d\n", res);
    return -1;
}

static int _mkfs(int argc, char **argv)
{
    int vol_idx;
    MKFS_PARM opt = {0};

    if (argc == 3) {
        vol_idx = atoi(argv[1]);

        if (strcmp(argv[2], "fat") == 0) {
            opt.fmt = FM_FAT;
        }
        else if (strcmp(argv[2], "fat32") == 0) {
            opt.fmt = FM_FAT32;
        }
        else if (strcmp(argv[2], "exfat") == 0) {
            opt.fmt = FM_EXFAT;
        }
        else {
            opt.fmt = FM_ANY;
        }
    }
    else {
        printf("usage: %s <volume_idx> <fat|fat32|exfat|any>\n", argv[0]);
        return -1;
    }

    if (vol_idx > (int)(MTD_NUM - 1)) {
        printf("max allowed <volume_idx> is %d\n", (int)(MTD_NUM - 1));
        return -1;
    }

    char volume_str[TEST_FATFS_MAX_VOL_STR_LEN];
    sprintf(volume_str, "%d:/", vol_idx);
    BYTE work[FF_MAX_SS];

    puts("formatting media...");

    FRESULT mkfs_resu = f_mkfs(volume_str, &opt, work, sizeof(work));

    if (mkfs_resu == FR_OK) {
        puts("[OK]");
        return 0;
    }

    printf("[FAILED] error %d\n", mkfs_resu);
    return -1;
}

static const shell_command_t shell_commands[] = {
    { "mount", "mount file system", _mount },
    { "mkfs", "format volume", _mkfs },
    { "touch", "create file", _touch },
    { "read", "print file content to console", _read },
    { "write", "append string to file", _write },
    { "ls", "list files", _ls },
    { NULL, NULL, NULL }
};

int main(void)
{
#if FATFS_FFCONF_OPT_FS_NORTC == 0
    /* the rtc is used in diskio.c for timestamps of files */
    puts("Initializing the RTC driver");
    rtc_poweron();

    struct tm time;
    time.tm_year = TEST_FATFS_RTC_YEAR - RTC_YEAR_OFFSET;  /* years are counted from 1900 */
    time.tm_mon  = TEST_FATFS_RTC_MON;                     /* 0 = January, 11 = December */
    time.tm_mday = TEST_FATFS_RTC_DAY;
    time.tm_hour = TEST_FATFS_RTC_H;
    time.tm_min  = TEST_FATFS_RTC_M;
    time.tm_sec  = TEST_FATFS_RTC_S;

    printf("Setting RTC to %04d-%02d-%02d %02d:%02d:%02d\n",
           time.tm_year + RTC_YEAR_OFFSET,
           time.tm_mon + TEST_FATFS_RTC_MON_OFFSET,
           time.tm_mday,
           time.tm_hour,
           time.tm_min,
           time.tm_sec);
    rtc_set_time(&time);
#endif

#if MODULE_MTD_NATIVE
    fatfs_mtd_devs[0] = mtd_dev_get(0);
#elif MODULE_MTD_SDMMC
    extern mtd_sdmmc_t mtd_sdmmc_dev0;
    fatfs_mtd_devs[0] = &mtd_sdmmc_dev0.base;
#elif MODULE_MTD_SDCARD
    for (unsigned int i = 0; i < SDCARD_SPI_NUM; i++){
        mtd_sdcard_devs[i].base.driver = &mtd_sdcard_driver;
        mtd_sdcard_devs[i].sd_card = &sdcard_spi_devs[i];
        mtd_sdcard_devs[i].params = &sdcard_spi_params[i];
        fatfs_mtd_devs[i] = &mtd_sdcard_devs[i].base;

        if(mtd_init(&mtd_sdcard_devs[i].base) == 0) {
            printf("init sdcard_mtd %u [OK]\n", i);
        }else{
            printf("init sdcard_mtd %u [FAILED]\n", i);
        }
    }
#endif

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
