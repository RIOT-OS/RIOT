/*
 * Copyright (C) 2016 Michel Rottleuthner
 *               2023 Gunar Schorcht
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
 * @brief       Test application for the SDIO/SD/MMC driver
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "byteorder.h"
#include "container.h"
#include "fmt.h"
#include "macros/units.h"
#include "shell.h"

#include "sdmmc/sdmmc.h"

/* independent of what you specify in a r/w cmd this is the maximum number of blocks read at once.
   If you call read with a bigger blockcount the read is performed in chunks*/
#define MAX_BLOCKS_IN_BUFFER 4
#define BLOCK_PRINT_BYTES_PER_LINE 16
#define FIRST_PRINTABLE_ASCII_CHAR 0x20
#define ASCII_UNPRINTABLE_REPLACEMENT "."

sdmmc_dev_t *dev = NULL;

uint8_t buffer[SDMMC_SDHC_BLOCK_SIZE * MAX_BLOCKS_IN_BUFFER];

static int _card_assert(void)
{
    if (dev == NULL) {
        printf("[Error] SD/MMC device not initialized, use init command\n");
        return -1;
    }

    if (!dev->present) {
        printf("[Error] Card not present\n");
        return -1;
    }

    if (!dev->init_done) {
        printf("[Error] Card not initialized, use init command\n");
        return -1;
    }

    return 0;
}

static int _init(int argc, char **argv)
{
    int device = 0;

    if ((argc == 2)) {
        device = atoi(argv[1]);
    }

    dev = sdmmc_get_dev(device);

    if (dev == NULL) {
        printf("[Error] No device with index %i\n", device);
        /* use the first SDMMC device by default */
        dev = sdmmc_get_dev(0);
        return -1;
    }

    if (!dev->present) {
        printf("[Error] Card not present\n");
        return -1;
    }

    printf("Initializing SD Card/MMC at SD/MMC device %i\n", device);
    if (sdmmc_card_init(dev)) {
        puts("[FAILED]");
        puts("enable debugging in sdmmc.c for more information!");
        return -1;
    }
    printf("card found [OK]\n");

    return 0;
}

static int _cid(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (_card_assert()) {
        return -1;
    }

    sdmmc_cid_t *cid = &dev->cid;

    puts("----------------------------------------");
    if (IS_USED(MODULE_SDMMC_MMC) && (dev->type == SDMMC_CARD_TYPE_MMC)) {
        printf("MID: %d\n", cid->mmc.MID);
        printf("OID: 0x%04x\n", byteorder_ntohs(cid->mmc.OID));
        printf("PNM: %c%c%c%c%c%c\n",
               cid->mmc.PNM[0], cid->mmc.PNM[1], cid->mmc.PNM[2],
               cid->mmc.PNM[3], cid->mmc.PNM[4], cid->mmc.PNM[5]);
        printf("PRV: %d.%d\n", cid->mmc.PRV >> 4, cid->mmc.PRV & 0x0f);
        printf("PSN: %" PRIu32 "\n", byteorder_ntohl(cid->mmc.PSN));
        printf("MDT: %u/%u\n", 1997 + (cid->mmc.MDT >> 4), cid->mmc.MDT & 0x0f);
        printf("CRC: 0x%02x\n", cid->mmc.CID_CRC >> 1);
    }
    else {
        printf("MID: %d\n", cid->sd.MID);
        printf("OID: %c%c (0x%04x)\n",
               cid->sd.OID[0], cid->sd.OID[1], (cid->sd.OID[0] << 8) | cid->sd.OID[1]);
        printf("PNM: %c%c%c%c%c\n",
               cid->sd.PNM[0], cid->sd.PNM[1], cid->sd.PNM[2],
               cid->sd.PNM[3], cid->sd.PNM[4]);
        printf("PRV: %d.%d\n", cid->sd.PRV >> 4, cid->sd.PRV & 0x0f);
        printf("PSN: %" PRIu32 "\n", byteorder_ntohl(cid->sd.PSN));
        printf("MDT: %u/%u\n", 2000 + (byteorder_ntohs(cid->sd.MDT) >> 4),
               byteorder_ntohs(cid->sd.MDT) & 0x000f);
        printf("CRC: 0x%02x\n", cid->sd.CID_CRC >> 1);
    }
    puts("----------------------------------------");
    return 0;
}

static int _scr(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (_card_assert()) {
        return -1;
    }

    sdmmc_scr_t *scr = &dev->scr;
    uint8_t ver = SDMMC_SCR_SD_SPEC(dev->scr);

    printf("Physical Layer Specification Version ");
    switch (ver) {
    case 0:
        printf("1.0 or 1.01\n");
        break;
    case 1:
        printf("1.1\n");
        break;
    case 2:
        printf("2.00\n");
        break;
    case 3:
        printf("3.0X\n");
        break;
    default:
        printf("%u.XX\n", ver);
    }

    printf("Reserved for manufacurer: %08"PRIx32"\n", scr->reserved0);
    printf("SCR_STRUCTURE: 0x%01x\n", scr->SCR_STRUCTURE);
    printf("SD_SPEC: 0x%01x\n", scr->SD_SPEC);
    printf("DATA_STAT_AFTER_ERASE: %d\n", scr->DATA_STAT_AFTER_ERASE);
    printf("SD_SECURITY: 0x%01x\n", scr->SD_SECURITY);
    printf("SD_BUS_WIDTHS: 0x%01x\n", scr->SD_BUS_WIDTHS);
    printf("SD_SPEC3: %d\n", scr->SD_SPEC3);
    printf("EX_SECURITY: 0x%02x\n", scr->EX_SECURITY);
    printf("SD_SPEC4: %d\n", scr->SD_SPEC4);
    printf("SD_SPECX: 0x%01x\n", scr->SD_SPECX);
    printf("CMD_SUPPORT 0x%02x\n", scr->CMD_SUPPORT);

    return 0;
}

static int _csd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (_card_assert()) {
        return -1;
    }

    sdmmc_csd_t *csd = &dev->csd;

    if (IS_USED(MODULE_SDMMC_MMC) && (dev->type == SDMMC_CARD_TYPE_MMC)) {
        puts("CSD MMC\n---------------------------------------");
        printf("CSD_STRUCTURE: 0x%x\n", csd->mmc.CSD_STRUCTURE);
        printf("SPEC_VERS: 0x%x\n", csd->mmc.SPEC_VERS);
        printf("TAAC: 0x%x\n", csd->mmc.TAAC);
        printf("NSAC: 0x%x\n", csd->mmc.NSAC);
        printf("TRAN_SPEED: 0x%x\n", csd->mmc.TRAN_SPEED);
        printf("CCC: 0x%x\n", csd->mmc.CCC);
        printf("READ_BL_LEN: 0x%x\n", csd->mmc.READ_BL_LEN);
        printf("READ_BL_PARTIAL: 0x%x\n", csd->mmc.READ_BL_PARTIAL);
        printf("WRITE_BLK_MISALIGN: 0x%x\n", csd->mmc.WRITE_BLK_MISALIGN);
        printf("READ_BLK_MISALIGN: 0x%x\n", csd->mmc.READ_BLK_MISALIGN);
        printf("DSR_IMP: 0x%x\n", csd->mmc.DSR_IMP);
        printf("C_SIZE: 0x%x\n", csd->mmc.C_SIZE);
        printf("VDD_R_CURR_MIN: 0x%x\n", csd->mmc.VDD_R_CURR_MIN);
        printf("VDD_R_CURR_MAX: 0x%x\n", csd->mmc.VDD_R_CURR_MAX);
        printf("VDD_W_CURR_MIN: 0x%x\n", csd->mmc.VDD_W_CURR_MIN);
        printf("VDD_W_CURR_MAX: 0x%x\n", csd->mmc.VDD_W_CURR_MAX);
        printf("C_SIZE_MULT: 0x%x\n", csd->mmc.C_SIZE_MULT);
        printf("ERASE_GRP_SIZE: 0x%x\n", csd->mmc.ERASE_GRP_SIZE);
        printf("ERASE_GRP_MULT: 0x%x\n", csd->mmc.ERASE_GRP_MULT);
        printf("WP_GRP_SIZE: 0x%x\n", csd->mmc.WP_GRP_SIZE);
        printf("WP_GRP_ENABLE: 0x%x\n", csd->mmc.WP_GRP_ENABLE);
        printf("R2W_FACTOR: 0x%x\n", csd->mmc.R2W_FACTOR);
        printf("WRITE_BL_LEN: 0x%x\n", csd->mmc.WRITE_BL_LEN);
        printf("WRITE_BL_PARTIAL: 0x%x\n", csd->mmc.WRITE_BL_PARTIAL);
        printf("CONTENT_PROT_APP: 0x%x\n", csd->mmc.CONTENT_PROT_APP);
        printf("FILE_FORMAT_GRP: 0x%x\n", csd->mmc.FILE_FORMAT_GRP);
        printf("COPY: 0x%x\n", csd->mmc.COPY);
        printf("PERM_WRITE_PROTECT: 0x%x\n", csd->mmc.PERM_WRITE_PROTECT);
        printf("TMP_WRITE_PROTECT: 0x%x\n", csd->mmc.TMP_WRITE_PROTECT);
        printf("FILE_FORMAT: 0x%x\n", csd->mmc.FILE_FORMAT);
        printf("ECC: 0x%x\n", csd->mmc.ECC);
        printf("CRC: 0x%x\n", csd->mmc.CSD_CRC);

#if IS_USED(MODULE_SDMMC_MMC)
        sdmmc_ext_csd_t *ext_csd = &dev->ext_csd;

        puts("\nEXT_CSD MMC\n---------------------------------------");
        printf("CSD_STRUCTURE: 0x%x\n", ext_csd->CSD_STRUCTURE);
        printf("CARD_TYPE: 0x%02x\n", ext_csd->CARD_TYPE);
        printf("BUS_WIDTH: %u\n", ext_csd->BUS_WIDTH);
        printf("SEC_COUNT: %"PRIu32"\n", ext_csd->SEC_COUNT);
#endif
    }
    else if (dev->csd.v1.CSD_STRUCTURE == SDMMC_CSD_V1) {
        puts("CSD V1\n----------------------------------------");
        printf("CSD_STRUCTURE: 0x%x\n", csd->v1.CSD_STRUCTURE);
        printf("TAAC: 0x%x\n", csd->v1.TAAC);
        printf("NSAC: 0x%x\n", csd->v1.NSAC);
        printf("TRAN_SPEED: 0x%x\n", csd->v1.TRAN_SPEED);
        printf("CCC: 0x%x\n", csd->v1.CCC);
        printf("READ_BL_LEN: 0x%x\n", csd->v1.READ_BL_LEN);
        printf("READ_BL_PARTIAL: 0x%x\n", csd->v1.READ_BL_PARTIAL);
        printf("WRITE_BLK_MISALIGN: 0x%x\n", csd->v1.WRITE_BLK_MISALIGN);
        printf("READ_BLK_MISALIGN: 0x%x\n", csd->v1.READ_BLK_MISALIGN);
        printf("DSR_IMP: 0x%x\n", csd->v1.DSR_IMP);
        printf("C_SIZE: 0x%x\n", csd->v1.C_SIZE);
        printf("VDD_R_CURR_MIN: 0x%x\n", csd->v1.VDD_R_CURR_MIN);
        printf("VDD_R_CURR_MAX: 0x%x\n", csd->v1.VDD_R_CURR_MAX);
        printf("VDD_W_CURR_MIN: 0x%x\n", csd->v1.VDD_W_CURR_MIN);
        printf("VDD_W_CURR_MAX: 0x%x\n", csd->v1.VDD_W_CURR_MAX);
        printf("C_SIZE_MULT: 0x%x\n", csd->v1.C_SIZE_MULT);
        printf("ERASE_BLK_EN: 0x%x\n", csd->v1.ERASE_BLK_EN);
        printf("SECTOR_SIZE: 0x%x\n", csd->v1.SECTOR_SIZE);
        printf("WP_GRP_SIZE: 0x%x\n", csd->v1.WP_GRP_SIZE);
        printf("WP_GRP_ENABLE: 0x%x\n", csd->v1.WP_GRP_ENABLE);
        printf("R2W_FACTOR: 0x%x\n", csd->v1.R2W_FACTOR);
        printf("WRITE_BL_LEN: 0x%x\n", csd->v1.WRITE_BL_LEN);
        printf("WRITE_BL_PARTIAL: 0x%x\n", csd->v1.WRITE_BL_PARTIAL);
        printf("FILE_FORMAT_GRP: 0x%x\n", csd->v1.FILE_FORMAT_GRP);
        printf("COPY: 0x%x\n", csd->v1.COPY);
        printf("PERM_WRITE_PROTECT: 0x%x\n", csd->v1.PERM_WRITE_PROTECT);
        printf("TMP_WRITE_PROTECT: 0x%x\n", csd->v1.TMP_WRITE_PROTECT);
        printf("FILE_FORMAT: 0x%x\n", csd->v1.FILE_FORMAT);
        printf("CRC: 0x%x\n", csd->v1.CSD_CRC);
    }
    else if (dev->csd.v2.CSD_STRUCTURE == SDMMC_CSD_V2) {
        puts("CSD V2:\n----------------------------------------");
        printf("CSD_STRUCTURE: 0x%x\n", csd->v2.CSD_STRUCTURE);
        printf("TAAC: 0x%x\n", csd->v2.TAAC);
        printf("NSAC: 0x%x\n", csd->v2.NSAC);
        printf("TRAN_SPEED: 0x%x\n", csd->v2.TRAN_SPEED);
        printf("CCC: 0x%x\n", csd->v2.CCC);
        printf("READ_BL_LEN: 0x%x\n", csd->v2.READ_BL_LEN);
        printf("READ_BL_PARTIAL: 0x%x\n", csd->v2.READ_BL_PARTIAL);
        printf("WRITE_BLK_MISALIGN: 0x%x\n", csd->v2.WRITE_BLK_MISALIGN);
        printf("READ_BLK_MISALIGN: 0x%x\n", csd->v2.READ_BLK_MISALIGN);
        printf("DSR_IMP: 0x%x\n", csd->v2.DSR_IMP);
        printf("C_SIZE: 0x%"PRIu32"\n", (uint32_t)csd->v2.C_SIZE);
        printf("ERASE_BLK_EN: 0x%x\n", csd->v2.ERASE_BLK_EN);
        printf("SECTOR_SIZE: 0x%x\n", csd->v2.SECTOR_SIZE);
        printf("WP_GRP_SIZE: 0x%x\n", csd->v2.WP_GRP_SIZE);
        printf("WP_GRP_ENABLE: 0x%x\n", csd->v2.WP_GRP_ENABLE);
        printf("R2W_FACTOR: 0x%x\n", csd->v2.R2W_FACTOR);
        printf("WRITE_BL_LEN: 0x%x\n", csd->v2.WRITE_BL_LEN);
        printf("WRITE_BL_PARTIAL: 0x%x\n", csd->v2.WRITE_BL_PARTIAL);
        printf("FILE_FORMAT_GRP: 0x%x\n", csd->v2.FILE_FORMAT_GRP);
        printf("COPY: 0x%x\n", csd->v2.COPY);
        printf("PERM_WRITE_PROTECT: 0x%x\n", csd->v2.PERM_WRITE_PROTECT);
        printf("TMP_WRITE_PROTECT: 0x%x\n", csd->v2.TMP_WRITE_PROTECT);
        printf("FILE_FORMAT: 0x%x\n", csd->v2.FILE_FORMAT);
        printf("CRC: 0x%x\n", csd->v2.CSD_CRC);
    }
    else {
        printf("[FAILED] wrong CSD structure version %u\n",
               dev->csd.v1.CSD_STRUCTURE);
        return -1;
    }

    puts("----------------------------------------");
    return 0;
}

static int _sds(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    sdmmc_sd_status_t sds;

    if (sdmmc_read_sds(dev, &sds) == 0) {
        puts("SD Status:\n----------------------------------------");
        printf("SIZE_OF_PROTECTED_AREA: 0x%0lx\n", (unsigned long)sds.SIZE_OF_PROTECTED_AREA);
        printf("SUS_ADDR: 0x%0lx\n", (unsigned long)sds.SUS_ADDR);
        printf("VSC_AU_SIZE: 0x%0lx\n", (unsigned long)sds.VSC_AU_SIZE);
        printf("SD_CARD_TYPE: 0x%0lx\n", (unsigned long)sds.SD_CARD_TYPE);
        printf("ERASE_SIZE: 0x%0lx\n", (unsigned long)sds.ERASE_SIZE);
        printf("SPEED_CLASS: 0x%0lx\n", (unsigned long)sds.SPEED_CLASS);
        printf("PERFORMANCE_MOVE: 0x%0lx\n", (unsigned long)sds.PERFORMANCE_MOVE);
        printf("VIDEO_SPEED_CLASS: 0x%0lx\n", (unsigned long)sds.VIDEO_SPEED_CLASS);
        printf("ERASE_TIMEOUT: 0x%0lx\n", (unsigned long)sds.ERASE_TIMEOUT);
        printf("ERASE_OFFSET: 0x%0lx\n", (unsigned long)sds.ERASE_OFFSET);
        printf("UHS_SPEED_GRADE: 0x%0lx\n", (unsigned long)sds.UHS_SPEED_GRADE);
        printf("UHS_AU_SIZE: 0x%0lx\n", (unsigned long)sds.UHS_AU_SIZE);
        printf("AU_SIZE: 0x%0lx\n", (unsigned long)sds.AU_SIZE);
        printf("DAT_BUS_WIDTH: 0x%0lx\n", (unsigned long)sds.DAT_BUS_WIDTH);
        printf("SECURED_MODE: 0x%0lx\n", (unsigned long)sds.SECURED_MODE);
        puts("----------------------------------------");
        return 0;
    }
    return -1;
}

#define KILO    (1000UL)
#define MEGA    (1000000UL)
#define GIGA    (1000000000UL)

static void _print_size(uint64_t bytes)
{
    /* gib_frac = (bytes - gib_int * GiB) / MiB * KILO / KiB; */
    uint32_t gib_int = bytes / GiB(1);
    uint32_t gib_frac = (((bytes / MiB(1)) - (gib_int * KiB(1))) * KILO) / KiB(1);

    /* gb_frac = (bytes - gb_int * GIGA) / MEGA */
    uint32_t gb_int = bytes / GIGA;
    uint32_t gb_frac = (bytes / MEGA) - (gb_int * KILO);

    print_u64_dec( bytes );
    printf(" bytes (%" PRIu32 ",%03" PRIu32 " GiB | %" PRIu32 ",%03" PRIu32 " GB)\n", gib_int,
           gib_frac, gb_int, gb_frac);
}

static int _size(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    puts("\nCard size: ");
    _print_size(sdmmc_get_capacity(dev));

    return 0;
}

static int _read(int argc, char **argv)
{
    int blockaddr;
    int cnt;
    bool print_as_char = false;

    if (_card_assert()) {
        return -1;
    }

    if ((argc == 3) || (argc == 4)) {
        blockaddr = atoi(argv[1]);
        cnt = atoi(argv[2]);
        if (argc == 4 && (strcmp("-c", argv[3]) == 0)) {
            print_as_char = true;
        }
    }
    else {
        printf("usage: %s blockaddr cnt [-c]\n", argv[0]);
        return -1;
    }

    int total_read = 0;

    while (total_read < cnt) {
        int chunk_blocks = cnt - total_read;
        if (chunk_blocks > MAX_BLOCKS_IN_BUFFER) {
            chunk_blocks = MAX_BLOCKS_IN_BUFFER;
        }

        uint16_t chunks_read;
        int res = sdmmc_read_blocks(dev, blockaddr + total_read,
                                    SDMMC_SDHC_BLOCK_SIZE,
                                    chunk_blocks, buffer, &chunks_read);
        if (res) {
            printf("read error %d (block %d/%d)\n",
                   res, total_read + chunks_read, cnt);
            return -1;
        }

        if (IS_USED(OUTPUT)) {
            for (int i = 0; i < chunk_blocks * SDMMC_SDHC_BLOCK_SIZE; i++) {

                if ((i % SDMMC_SDHC_BLOCK_SIZE) == 0) {
                    printf("BLOCK %d:\n",
                           blockaddr + total_read + i / SDMMC_SDHC_BLOCK_SIZE);
                }

                if (print_as_char) {
                    if (buffer[i] >= FIRST_PRINTABLE_ASCII_CHAR) {
                        printf("%c", buffer[i]);
                    }
                    else {
                        printf(ASCII_UNPRINTABLE_REPLACEMENT);
                    }
                }
                else {
                    printf("%02x ", buffer[i]);
                }

                if ((i % BLOCK_PRINT_BYTES_PER_LINE) == (BLOCK_PRINT_BYTES_PER_LINE - 1)) {
                    puts(""); /* line break after BLOCK_PRINT_BYTES_PER_LINE bytes */
                }

                if ((i % SDMMC_SDHC_BLOCK_SIZE) == (SDMMC_SDHC_BLOCK_SIZE - 1)) {
                    puts(""); /* empty line after each printed block */
                }
            }
        }
        total_read += chunks_read;
    }
    printf("read %d block(s) from %d [OK]\n", cnt, blockaddr);

    return 0;
}

static int _write(int argc, char **argv)
{
    int bladdr;
    char *data;
    int size;
    bool repeat_data = false;

    if (_card_assert()) {
        return -1;
    }

    if (argc == 3 || argc == 4) {
        bladdr = atoi(argv[1]);
        data = argv[2];
        size = strlen(argv[2]);
        printf("will write '%s' (%d chars) at start of block %d\n",
               data, size, bladdr);
        if (argc == 4 && (strcmp("-r", argv[3]) == 0)) {
            repeat_data = true;
            puts("the rest of the block will be filled with copies of that string");
        }
        else {
            puts("the rest of the block will be filled with zeros");
        }
    }
    else {
        printf("usage: %s blockaddr string [-r]\n", argv[0]);
        return -1;
    }

    if (size > SDMMC_SDHC_BLOCK_SIZE) {
        printf("maximum stringsize to write at once is %d ...aborting\n",
               SDMMC_SDHC_BLOCK_SIZE);
        return -1;
    }

    /* copy data to a full-block-sized buffer an fill remaining block space
     * according to -r param*/
    uint8_t write_buffer[SDMMC_SDHC_BLOCK_SIZE];
    for (unsigned i = 0; i < sizeof(write_buffer); i++) {
        if (repeat_data || ((int)i < size)) {
            write_buffer[i] = data[i % size];
        }
        else {
            write_buffer[i] = 0;
        }
    }

    int res = sdmmc_write_blocks(dev, bladdr, SDMMC_SDHC_BLOCK_SIZE, 1,
                                 write_buffer, NULL);
    if (res) {
        printf("write error %d (wrote 1/1 blocks)\n", res);
        return -1;
    }

    printf("write block %d [OK]\n", bladdr);
    return 0;
}

static int _writem(int argc, char **argv)
{
    int bladdr;
    int cnt;
    uint16_t done;

    if (_card_assert()) {
        return -1;
    }

    if (argc == 3) {
        bladdr = atoi(argv[1]);
        cnt = atoi(argv[2]);
    }
    else {
        printf("usage: %s blockaddr num\n", argv[0]);
        return -1;
    }

    /* writing cnt blocks with data from stack */
    int res = sdmmc_write_blocks(dev, bladdr, SDMMC_SDHC_BLOCK_SIZE, cnt,
                                 (void *)&bladdr, &done);
    if (res) {
        printf("write error %d (wrote %u/%d blocks)\n", res, done, cnt);
        return -1;
    }

    printf("write %d block(s) to %d [OK]\n", cnt, bladdr);
    return 0;
}

static int _erase(int argc, char **argv)
{
    int blockaddr;
    int cnt;

    if (_card_assert()) {
        return -1;
    }

    if (argc == 3) {
        blockaddr = atoi(argv[1]);
        cnt = atoi(argv[2]);
    }
    else {
        printf("usage: %s blockaddr cnt\n", argv[0]);
        return -1;
    }

    int res = sdmmc_erase_blocks(dev, blockaddr, cnt);
    if (res) {
        printf("erase error %d\n", res);
        return -1;
    }

    printf("erase %d block(s) from %d [OK]\n", cnt, blockaddr);
    return 0;
}

static int _copy(int argc, char **argv)
{
    int src_block;
    int dst_block;
    int num_block = 1;
    uint8_t tmp_copy[SDMMC_SDHC_BLOCK_SIZE];

    if (_card_assert()) {
        return -1;
    }

    if (argc < 3) {
        printf("usage: %s src dst [num]\n", argv[0]);
        return -1;
    }

    src_block = atoi(argv[1]);
    dst_block = atoi(argv[2]);

    if (argc == 4) {
        num_block = atoi(argv[3]);
    }

    for (int i = 0; i < num_block; i++) {
        int res = sdmmc_read_blocks(dev, src_block + i, SDMMC_SDHC_BLOCK_SIZE, 1,
                                    tmp_copy, NULL);

        if (res) {
            printf("read error %d (block %d)\n", res, src_block + i);
            return -1;
        }

        res = sdmmc_write_blocks(dev, dst_block + i, SDMMC_SDHC_BLOCK_SIZE, 1,
                                 tmp_copy, NULL);

        if (res) {
            printf("write error %d (block %d)\n", res, dst_block + i);
            return -1;
        }

        if (IS_USED(OUTPUT) && (num_block > 1)) {
            extern ssize_t stdio_write(const void *buffer, size_t len);
            /* A single-byte write can not be incomplete */
            stdio_write(".", 1); //printf(".");
            if ((num_block % 79) == 79) {
                printf("\n");
            }
        }
    }

    if (IS_USED(OUTPUT)) {
        printf("\n");
    }

    printf("copy %d block(s) from %d to %d [OK]\n",
           num_block, src_block, dst_block);
    return 0;
}

static int _sector_count(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("available sectors on card: %" PRIu32 "\n",
           (uint32_t)(sdmmc_get_capacity(dev) / SDMMC_SDHC_BLOCK_SIZE));
    return 0;
}

void _card_event_cb(sdmmc_dev_t *dev, sdmmc_event_t event)
{
    (void)dev;
    switch (event) {
    case SDMMC_EVENT_CARD_INSERTED:
        puts("Event: Card inserted");
        break;
    case SDMMC_EVENT_CARD_REMOVED:
        puts("Event: Card removed");
        break;
    default:
        puts("Event: unknown");
    }
}

static const shell_command_t shell_commands[] = {
    { "init", "initializes default card", _init },
    { "cid",  "print content of CID (Card IDentification) register", _cid },
    { "csd", "print content of CSD (Card-Specific Data) register", _csd },
    { "sds", "print SD Status", _sds },
    { "scr", "print content of SCR (SD Card Configuration Register)", _scr },
    { "size", "print card size", _size },
    { "sectors", "print sector count of card", _sector_count },
    { "read", "'read n m' reads m blocks beginning at block address n and prints the result. "
              "Append -c option to print data readable chars", _read },
    { "write", "'write n data' writes data to block n. Append -r option to "
               "repeatedly write data to complete block", _write },
    { "copy", "'copy src dst' copies block src to block dst", _copy },
    { "erase", "'erase n m' erases m blocks beginning at block address n", _erase },
    { "writem", "'write n m' writes m data blocks beginning at block address n.",
                _writem },
    { NULL, NULL, NULL }
};

int main(void)
{
    /* use the first SDMMC device by default */
    dev = sdmmc_get_dev(0);
    dev->event_cb = _card_event_cb;

    puts("SDMMC driver test application");

    puts("insert a SD Card/MMC and use 'init' command to set card to spi mode");
    puts("WARNING: using 'write' or 'copy' commands WILL overwrite data on your card and");
    puts("almost for sure corrupt existing filesystems, partitions and contained data!");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
