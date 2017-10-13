/*
 * Copyright (c) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Mark Solters <msolters@gmail.com>
 *               2016 Inria
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * @ingroup     FW
 * @file
 * @brief       Meta-data generation for FW images
 *
 * @author      Mark Solters <msolters@gmail.com>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "firmware.h"
#include "common.h"

const char genmeta_usage[] = "firmware genmeta <BINFILE> <VERSION> <APPID> <START-ADDR> <seckey> <outfile>";

int genmeta(int argc, char *argv[])
{
    firmware_metadata_t metadata;
    unsigned char sk[FIRMWARE_SECKEY_LEN];
    ssize_t firmware_size;

    memset(&metadata, '\0', sizeof(firmware_metadata_t));

    if (argc < 7) {
        fprintf(stderr, "usage: %s\n", genmeta_usage);
        return -1;
    }

    /* Read secret key to sign metadata */
    if (!from_file(argv[5], sk, sizeof(sk))) {
        fprintf(stderr, "Error: cannot read keyfile\n");
        return -1;
    }

    firmware_size = do_sha256(argv[1], metadata.hash, 0);
    if (firmware_size == -1) {
        fprintf(stderr, "Error: cannot read firmware file\n");
        return -1;
    }

    /* Generate FW image metadata */
    memcpy(&metadata.magic_number, "RIOT", 4);
    metadata.size = firmware_size;
    sscanf(argv[2], "%xu", (unsigned int *)&(metadata.version));
    sscanf(argv[3], "%xu", &(metadata.appid));
    sscanf(argv[4], "%xu", &(metadata.start_addr));

    /* calculate metadata checksum */
    metadata.chksum = firmware_metadata_checksum(&metadata);

    /* sign */
    firmware_sign_metadata(&metadata, sk);

    /* talk to the user */
    firmware_metadata_print(&metadata);

    /* Write the metadata */
    printf("Metadata size: %lu\n", sizeof(firmware_metadata_t));
    if (!to_file(argv[6], &metadata, sizeof(firmware_metadata_t))) {
        fprintf(stderr, "Error: cannot write output\n");
        return -1;
    }

    return 0;
}
