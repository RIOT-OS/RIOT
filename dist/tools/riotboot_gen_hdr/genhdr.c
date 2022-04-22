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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "riotboot/hdr.h"
#include "common.h"

/**
 *  @brief Alignment required to set VTOR in Cortex-M0+/3/4/7
 */
#define HDR_ALIGN              (256)

static void populate_hdr(riotboot_hdr_t *hdr, uint32_t ver, uint32_t addr,
                         uint32_t img_size, uint8_t*digest)
{
    /* ensure the buffer and header have 0's */
    memset(hdr, '\0', sizeof(riotboot_hdr_t));

    /* Generate image header */
    hdr->magic_number = RIOTBOOT_MAGIC;
    hdr->version = ver;
    hdr->start_addr = addr;
    hdr->img_size = img_size;
    hdr->chksum_legacy = riotboot_hdr_checksum_legacy(hdr);
    memcpy(hdr->digest, digest, SHA256_DIGEST_LENGTH);
    /* calculate header checksum */
    hdr->chksum = riotboot_hdr_checksum(hdr);
}

int genhdr(int argc, char *argv[])
{
    const char generate_usage[] =
        "<IMG_BIN> <APP_VER> <START_ADDR> <HDR_LEN> <IMG_DIGEST> <outfile|->";

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

    /* digest buffer */
    uint32_t img_size = 0;
    uint8_t digest[SHA256_DIGEST_LENGTH];

    /* helpers */
    errno = 0;
    char *p;

    if (argc < 7) {
        fprintf(stderr, "usage: genhdr generate %s\n", generate_usage);
        return -1;
    }

    struct stat sb;

    if (stat(argv[1], &sb) == -1) {
        fprintf(stderr, "Error: invalid IMG_BIN path!\n");
        return -1;
    }
    img_size = sb.st_size;

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

    /* convert string digest to byte array */
    if (strlen(argv[5]) != 2 * SHA256_DIGEST_LENGTH) {
        fprintf(stderr, "Error: wrong digest size\n");
        return -1;
    }
    const char *pos = argv[5];
    for (size_t count = 0; count < SHA256_DIGEST_LENGTH; count++) {
        sscanf(pos, "%2hhx", &digest[count]);
        pos += 2;
    }

    /* prepare a 0 initialised buffer for riotboot_hdr_t */
    hdr_buf = calloc(1, hdr_len);
    if (hdr_buf == NULL) {
        fprintf(stderr, "Error: not enough memory!\n");
        return -1;
    }

    populate_hdr((riotboot_hdr_t *)hdr_buf, app_ver, start_addr, img_size, digest);

    /* Write the header */
    if (!to_file(argv[6], hdr_buf, hdr_len)) {
        fprintf(stderr, "Error: cannot write output\n");
        free(hdr_buf);
        return 1;
    }

    free(hdr_buf);

    return 0;
}
