/*
 * Copyright (c) 2016 Inria
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 *
 */

/**
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "riotboot/hdr.h"
#include "common.h"

/**
 *  @brief Alignment required to set VTOR in Cortex-M0+/3/4/7
 */
#define HDR_ALIGN              (256)

static size_t populate_hdr_v1(riotboot_hdr_t *hdr, uint32_t ver, uint32_t addr)
{
    /* ensure the buffer and header have 0's */
    memset(hdr, '\0', sizeof(*hdr));
    /* Generate image header v1 */
    hdr->v1.magic_number = RIOTBOOT_MAGIC_V1;
    hdr->v1.version = ver;
    hdr->v1.start_addr = addr;
    /* calculate header checksum */
    hdr->v1.chksum = riotboot_hdr_checksum(hdr);
    return sizeof(hdr->v1);
}

static size_t populate_hdr_v2(riotboot_hdr_t *hdr, uint32_t ver, uint32_t addr)
{
    /* ensure the buffer and header have 0's */
    memset(hdr, '\0', sizeof(*hdr));
    /* Generate image header v2 */
    hdr->v2.magic_number = RIOTBOOT_MAGIC_V2;
    hdr->v2.version = ver;
    hdr->v2.start_addr = addr;
    hdr->v2.flags = 0xffffffff;
    /* calculate header checksum */
    hdr->v2.chksum = riotboot_hdr_checksum(hdr);
    return sizeof(hdr->v2);
}

int genhdr(int argc, char *argv[])
{
    const char generate_usage[] = "<IMG_BIN> <APP_VER> <START_ADDR> <HDR_LEN> <outfile|-> [-v [v1,v2]]";

    /* riotboot_hdr buffer */
    uint8_t *hdr_buf;

    /* arguments storage variables */
    unsigned long app_ver_arg = 0;
    unsigned long start_addr_arg = 0;
    unsigned long hdr_len_arg = 0;

    /* header variables */
    size_t hdr_len = 0;
    uint32_t app_ver = 0;
    uint32_t start_addr = 0;

    /* helpers */
    errno = 0;
    char *p;

    if (argc < 6) {
        fprintf(stderr, "usage: genhdr generate %s\n", generate_usage);
        return -1;
    }

    app_ver_arg = strtol(argv[2], &p, 0);
    if (errno != 0 || *p != '\0' || app_ver_arg > UINT32_MAX) {
        fprintf(stderr, "Error: APP_VER not valid!\n");
    }
    else {
        app_ver = app_ver_arg;
    }

    start_addr_arg = strtol(argv[3], &p, 0);
    if (errno != 0 || *p != '\0' || start_addr_arg > UINT32_MAX) {
        fprintf(stderr, "Error: START_ADDR not valid!\n");
    }
    else {
        start_addr = start_addr_arg;
    }

    hdr_len_arg = strtol(argv[4], &p, 0);
    if (errno != 0 || *p != '\0' || hdr_len_arg % HDR_ALIGN || hdr_len_arg > UINT32_MAX) {
        fprintf(stderr, "Error: HDR_LEN not valid!\n");
        return -1;
    }
    else {
        hdr_len = hdr_len_arg;
    }

    /* generate only v1 by default */
    bool gen_v1 = true;
    bool gen_v2 = false;
    if (argc >= 8) {
        gen_v1 = false;
        if (!strcmp(argv[6], "-v")) {
            for (int a = 7; a < argc; a++) {
                if (!strcmp(argv[a], "v1")) {
                    gen_v1 = true;
                }
                else if (!strcmp(argv[a], "v2")) {
                    gen_v2 = true;
                }
                else {
                    fprintf(stderr, "Error: unknown version '%s'!\n", argv[a]);
                    return -1;
                }
            }
        }
    }
    /* prepare a 0 initialised buffer for riotboot_hdr_t */
    hdr_buf = calloc(1, hdr_len);
    if (hdr_buf == NULL) {
        fprintf(stderr, "Error: not enough memory!\n");
        return -1;
    }

    size_t gen_hdr_size = 0;
    uint8_t *p_hdr = hdr_buf;
    if (gen_v1) {
        gen_hdr_size += populate_hdr_v1((riotboot_hdr_t *)(p_hdr + gen_hdr_size), app_ver, start_addr);
    }
    if (gen_v2) {
        gen_hdr_size += populate_hdr_v2((riotboot_hdr_t *)(p_hdr + gen_hdr_size), app_ver, start_addr);
    }

    /* Write the header */
    if (!to_file(argv[5], hdr_buf, hdr_len)) {
        fprintf(stderr, "Error: cannot write output\n");
        free(hdr_buf);
        return 1;
    }

    free(hdr_buf);

    return 0;
}

int updatehdr(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "usage: genhdr update <file> <new_version>\n");
        return -1;
    }
    const char *file = argv[1];

    riotboot_hdr_t hdr = { 0 };
    int res = from_file(file, &hdr, sizeof(hdr));
    if (res < (int)sizeof(hdr.v1)) {
        fprintf(stderr, "Can't read header from %s\n", file);
        return -EIO;
    }
    uint32_t magic = riotboot_hdr_get_magic_number(&hdr);
    if (magic == RIOTBOOT_MAGIC_V2) {
        hdr.v2.magic_number = RIOTBOOT_MAGIC_V2;
        hdr.v2.version = atoi(argv[2]);
        hdr.v2.chksum = riotboot_hdr_checksum(&hdr);
        to_file(file, &hdr, sizeof(hdr.v2));
    }
    else if (magic == RIOTBOOT_MAGIC_V1) {
        hdr.v1.magic_number = RIOTBOOT_MAGIC_V1;
        hdr.v1.version = atoi(argv[2]);
        hdr.v1.chksum = riotboot_hdr_checksum(&hdr);
        to_file(file, &hdr, sizeof(hdr.v1));
    }
    else {
        fprintf(stderr, "Invalid magic: %x\n", magic);
        return -EIO;
    }

    return 0;
}

static void print_hdr(const riotboot_hdr_t *hdr)
{
    printf("version: %u\n", riotboot_hdr_get_version(hdr));
    printf("address: 0x%x\n", riotboot_hdr_get_start_addr(hdr));
    printf("checksum: %svalid\n", riotboot_hdr_validate(hdr) ? "in" : "");
}

static void print_hdr_json(const riotboot_hdr_t *hdr)
{
    printf("{\n");
    printf("\t\"version\": %u,\n", riotboot_hdr_get_version(hdr));
    printf("\t\"address\": %u,\n", riotboot_hdr_get_start_addr(hdr));
    printf("\t\"valid\": %s\n", riotboot_hdr_validate(hdr) ? "false" : "true");
    printf("}\n");
}

int readhdr(const char *file, bool json)
{
    riotboot_hdr_t hdr = { 0 };
    int res = from_file(file, &hdr, sizeof(hdr));
    if (res < (int)sizeof(hdr)) {
        fprintf(stderr, "Can't read header from %s\n", file);
        return -EIO;
    }

    if (riotboot_hdr_get_magic_number(&hdr) != RIOTBOOT_MAGIC) {
        fprintf(stderr, "Invalid magic: %x\n", riotboot_hdr_get_magic_number(&hdr));
        return -EIO;
    }

    if (json) {
        print_hdr_json(&hdr);
    } else {
        print_hdr(&hdr);
    }

    return 0;
}
