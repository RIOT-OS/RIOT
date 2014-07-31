/**
 * Shell commands for accessing storage
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup shell_commands
 * @{
 * @file    sc_disk.c
 * @brief   provides shell commands to access storage (like mmc)
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "shell_commands.h"
#include "diskio.h"

static inline uint8_t sector_read(unsigned char *read_buf, unsigned long sector, unsigned long length, unsigned long offset)
{
    unsigned long i;

    if (MCI_read(read_buf, sector, 1) == RES_OK) {
        printf("[disk] Read sector %lu (%lu):\n", sector, offset);

        for (i = offset + 1; i <= offset + length; i++) {
            printf(" %u", read_buf[i - 1]);

            if (!(i % 16)) {
                puts("");
            }
        }

        puts("");
        return 1;
    }

    return 0;
}

void _get_sectorsize(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    unsigned short ssize;
    if (MCI_ioctl(GET_SECTOR_SIZE, &ssize) == RES_OK) {
        printf("[disk] sector size is %u\n", ssize);
    }
    else {
        puts("[disk] Failed to fetch sector size. Card inserted?");
    }
}

void _get_blocksize(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    unsigned long bsize;
    if (MCI_ioctl(GET_BLOCK_SIZE, &bsize) == RES_OK) {
        printf("[disk] block size is %lu\n", bsize);
    }
    else {
        puts("[disk] Failed to fetch block size. Card inserted?");
    }
}

void _get_sectorcount(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    unsigned long scount;
    if (MCI_ioctl(GET_SECTOR_COUNT, &scount) == RES_OK) {
        printf("[disk] sector count is %lu\n", scount);
    }
    else {
        puts("[disk] Failed to fetch sector count. Card inserted?");
    }
}

void _read_sector(int argc, char **argv)
{
    unsigned long sectornr, scount;
    unsigned short ssize;

    if (argc == 2) {
        sectornr = atol(argv[1]);

        if ((MCI_ioctl(GET_SECTOR_COUNT, &scount) == RES_OK) && (MCI_ioctl(GET_SECTOR_SIZE, &ssize) == RES_OK)) {
            unsigned char read_buf[ssize];

            if (sector_read(read_buf, sectornr, ssize, 0)) {
                return;
            }
        }

        printf("[disk] Error while reading sector %lu\n", sectornr);
    }
    else {
        printf("[disk] Usage:\n%s <SECTOR>\n", argv[0]);
        return;
    }
}

void _read_bytes(int argc, char **argv)
{
    unsigned long sector = 1, scount, offset;
    unsigned short ssize, length;

    if (argc != 3) {
        printf("[disk] Usage:\n%s <OFFSET> <LENGTH>\n", argv[0]);
        return;
    }

    offset = atol(argv[1]);
    length = atoi(argv[2]);

    /* get card info */
    if ((MCI_ioctl(GET_SECTOR_COUNT, &scount) == RES_OK) && (MCI_ioctl(GET_SECTOR_SIZE, &ssize) == RES_OK)) {
        /* calculate sector and offset position */
        sector = (offset / ssize) + 1;
        offset = (offset % ssize);
        /* preapre buffer (size must be a multiple of sector size) */
        unsigned char read_buf[((length / ssize) + 1) * 512];

        /* read from several sectors */
        if (length > (ssize - offset)) {
            /* buffer offset */
            unsigned long j = 0;
            /* chunk from current sector */
            unsigned short tmp = ssize - offset;

            while (length) {
                sector_read(read_buf + j, sector++, tmp, offset);
                /* decrease length  and recalculate chunk */
                length -= tmp;
                tmp = (length >= ssize) ? ssize : length;
            }

            return;
        } /* length > (ssize - offset) */
        /* read only one sector */
        else {
            if (sector_read(read_buf, sector, length, offset)) {
                return;
            }
        } /* length < (ssize - offset) */
    } /* ioctl */

    printf("[disk] Error while reading sector %lu\n", sector);
    return;

}
