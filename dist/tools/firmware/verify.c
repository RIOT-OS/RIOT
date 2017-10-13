/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "firmware.h"
#include "common.h"
#include "tweetnacl.h"

const char verify_usage[] = "firmware verify <imagefile> <pubkeyfile>";

int verify(int argc, char *argv[])
{
    unsigned char pubkey[FIRMWARE_PUBKEY_LEN];
    firmware_metadata_t metadata;

    if (argc < 4) {
        goto usage;
    }

    if (!from_file(argv[2], &metadata, sizeof(metadata))) {
        fprintf(stderr, "error reading image file\n");
        return 1;
    }

    if (!from_file(argv[3], pubkey, sizeof(pubkey))) {
        fprintf(stderr, "error reading keyfile\n");
        return 1;
    }

    int res = firmware_validate_metadata_signature(&metadata, pubkey) ? 1 : 0;
    if (res) {
        printf("signature check failed\n");
        return -1;
    } else {
        printf("signature check passed\n");
    }

    off_t size = fsize(argv[2]);
    if (size == sizeof(firmware_metadata_t)) {
        printf("hash check skipped\n");
        goto out;
    }

    char hash[SHA256_DIGEST_LENGTH];
    do_sha256(argv[2], hash, sizeof(firmware_metadata_t));
    res = memcmp(hash, metadata.hash, SHA256_DIGEST_LENGTH) ? 1 : 0;
    if (res) {
        printf("hash check failed\n");
    } else {
        printf("hash check passed\n");
    }

out:
    return res;

usage:
    fprintf(stderr, "usage: %s\n", verify_usage);
    return 1;
}

