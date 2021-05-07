/*
 * Copyright (C) 2021 Inria
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
 * @brief       lz4 package test application
 *
 * This application is adapted to RIOT from
 * https://github.com/lz4/lz4/blob/dev/examples/simple_buffer.c
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "lz4.h"

#define BUFFER_SIZE 1024

static char compressed_data[BUFFER_SIZE];
static char regen_buffer[BUFFER_SIZE];

static const char* const src =  "Lorem ipsum dolor sit amet, consectetur "
                                "adipiscing elit, sed do eiusmod tempor "
                                "incididunt ut labore et dolore magna aliqua. "
                                "Nisl vel pretium lectus quam id leo. Volutpat "
                                "sed cras ornare arcu dui vivamus arcu felis "
                                "bibendum. Velit scelerisque in dictum non "
                                "consectetur a erat nam. Pretium viverra "
                                "suspendisse potenti nullam ac tortor vitae "
                                "purus faucibus. Tristique et egestas quis "
                                "ipsum suspendisse. At imperdiet dui accumsan "
                                "sit amet nulla facilisi. Pulvinar neque "
                                "laoreet suspendisse interdum consectetur "
                                "libero. Vulputate sapien nec sagittis aliquam "
                                "malesuada bibendum arcu vitae. Sed adipiscing "
                                "diam donec adipiscing tristique risus nec. "
                                "Venenatis tellus in metus vulputate eu "
                                "scelerisque. Id faucibus nisl tincidunt eget "
                                "nullam non nisi est. Integer feugiat "
                                "scelerisque varus morbi enim. Est sit amet "
                                "facilisis magna etiam. Venenatis cras sed "
                                "felis eget velit aliquet sagittis.";

int main(void)
{
    /* Compression */
    const unsigned src_size = strlen(src) + 1;

    const int compressed_data_size = LZ4_compress_default(src, compressed_data,
                                                          src_size, BUFFER_SIZE);
    compressed_data[compressed_data_size] = 0;
    /* Check return_value to determine what happened. */
    if (compressed_data_size <= 0) {
        puts("Compression failed.");
        return -1;
    }

    unsigned compression_ratio = ((unsigned)compressed_data_size * 10000) / src_size;
    printf("Data compressed with success (ratio: %u.%u)\n",
           compression_ratio / 10000, (compression_ratio / 100) % 100);

    /* Decompression */
    const int decompressed_size = LZ4_decompress_safe(compressed_data, regen_buffer,
                                                      compressed_data_size, src_size);
    if (decompressed_size < 0) {
        puts("Decompression failed.");
        return -1;
    }
    puts("Data decompressed with success!");

    if ((unsigned)decompressed_size != src_size) {
        puts("Decompressed data is different from original");
        return -1;
    }

    /* Validation */
    if (memcmp(src, regen_buffer, src_size) != 0) {
        puts("Validation failed");
        return -1;
    }

    printf("Validation done, decompressed string:\n%s\n", regen_buffer);
    return 0;
}
