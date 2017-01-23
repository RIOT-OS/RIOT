/*
 * Copyright (c) 2016, Mark Solters <msolters@gmail.com>.
 *               2016, Francisco Acosta <francisco.acosta@inria.fr>
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
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/resource.h>

#include "fw_slots.h"
#include "hashes/sha256.h"

/* Input firmware .bin file */
FILE *firmware_bin;

/* Output metadata .bin file */
FILE *metadata_bin;

uint32_t firmware_size = 0;

int main(int argc, char *argv[])
{
    FW_metadata_t metadata;
    sha256_context_t firmware_sha256;
    uint8_t output_buffer[sizeof(FW_metadata_t)];
    int bytes_read = 0;
    uint8_t firmware_buffer[1024];

    (void)argc;

    if (!argv[1]) {
        printf("Please provide a .bin file to perform SHA256 on as the first argument.\n");
        return -1;
    }

    if (!argv[2]) {
        printf("Please provide a 16-bit hex firmware version integer as the second argument.\n");
        return -1;
    }

    if (!argv[3]) {
        printf("Please provide a 32-bit hex UUID integer as the third argument.\n");
        return -1;
    }

    /* (1) Open the firmware .bin file */
    firmware_bin = fopen(argv[1], "r");

    sha256_init(&firmware_sha256);

    while((bytes_read = fread(firmware_buffer, 1, sizeof(firmware_buffer), firmware_bin))) {
        sha256_update(&firmware_sha256, firmware_buffer, bytes_read);
        firmware_size += bytes_read;
    }
    sha256_final(&firmware_sha256, metadata.hash);

    printf("Firmware bytes read: %u\n", firmware_size);

    /* Close the .bin file. */
    fclose(firmware_bin);

    /*
     * TODO Sign hash
     */
    for (unsigned long i = 0; i < sizeof(metadata.shash); i++) {
        metadata.shash[i] = 0;
    }

    /* Generate FW image metadata */

    metadata.size = firmware_size;
    sscanf(argv[2], "%xu", (unsigned int *)&(metadata.version));
    sscanf(argv[3], "%xu", &(metadata.uuid));
    memcpy(output_buffer, (uint8_t*)&metadata, sizeof(FW_metadata_t));

    printf("Firmware Size: %d\n", metadata.size);
    printf("Firmware Version: %#x\n", metadata.version);
    printf("Firmware UUID: %#x\n", metadata.uuid);
    printf("Firmware HASH: ");
    for (unsigned long i = 0; i < sizeof(metadata.hash); i++) {
        printf("%02x ", metadata.hash[i]);
    }
    printf("\n");
    printf("Firmware signed HASH: ");
    for (unsigned long i = 0; i < sizeof(metadata.shash); i++) {
        printf("%02x ", metadata.shash[i]);
    }
    printf("\n");

    /* Open the output firmware .bin file */
    metadata_bin = fopen("firmware-metadata.bin", "w");

    /* Write the metadata */
    printf("Metadata size: %lu\n", sizeof(FW_metadata_t));
    fwrite(output_buffer, sizeof(output_buffer), 1, metadata_bin);

    /* 0xff spacing until firmware binary starts */
    uint8_t blank_buffer[256 - sizeof(FW_metadata_t)];

    for (unsigned long b = 0; b < sizeof(blank_buffer); b++) {
        blank_buffer[b] = 0xff;
    }

    fwrite(blank_buffer, sizeof(blank_buffer), 1, metadata_bin);

    /* Close the metadata file */
    fclose(metadata_bin);

    return 0;
}
