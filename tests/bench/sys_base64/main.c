/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Benchmark for the base64 lib
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "base64.h"
#include "compiler_hints.h"
#include "fmt.h"
#include "macros/utils.h"
#include "xtimer.h"

static char buf[128];

/* no need for the zero-termination here, base64_encode() gets the size of the
 * string as explicit argument */
NONSTRING
static const char input[96] = "This is an extremely, enormously, greatly, "
                              "immensely, tremendously, remarkably lengthy "
                              "sentence!";
NONSTRING
static const char base64[128] =
"VGhpcyBpcyBhbiBleHRyZW1lbHksIGVub3Jtb3VzbHksIGdyZWF0bHksIGltbWVuc2VseSwgdHJl"
"bWVuZG91c2x5LCByZW1hcmthYmx5IGxlbmd0aHkgc2VudGVuY2Uh";

int main(void) {
    uint32_t start, stop;
    size_t size;

    /* We don't want check return value in the benchmark loop, so we just do
     * a simple self test now. */
    print_str("Verifying that base64 encoding works for benchmark input: ");
    size = sizeof(buf);
    if ((BASE64_SUCCESS != base64_encode(input, sizeof(input), buf, &size)) ||
        (size != sizeof(buf)) ||
        (0 != memcmp(base64, buf, sizeof(base64))))
        {
        print_str("FAIL\nGot:      \"");
        print(buf, MIN(size, sizeof(base64)));
        print_str("\"\nExpected: \"");
        print(base64, sizeof(base64));
        print_str("\"\n");
    }
    else {
        print_str("OK\n");
    }

    print_str("Verifying that base64 decoding works for benchmark input: ");
    size = sizeof(buf);
    if ((BASE64_SUCCESS != base64_decode(base64, sizeof(base64), buf, &size)) ||
        (size != sizeof(input)) ||
        (0 != memcmp(input, buf, sizeof(input))))
        {
        print_str("FAIL\nGot:      \"");
        print(buf, MIN(size, sizeof(input)));
        print_str("\"\nExpected: \"");
        print(input, sizeof(input));
        print_str("\"\n");
    }
    else {
        print_str("OK\n");
    }

    start = xtimer_now_usec();
    for (unsigned i = 0; i < 1000; i++) {
        size = sizeof(buf);
        base64_encode(input, sizeof(input), buf, &size);
    }
    stop = xtimer_now_usec();

    print_str("Encoding 1.000 x 96 bytes (128 bytes in base64): ");
    print_u32_dec(stop - start);
    print_str(" µs\n");

    start = xtimer_now_usec();
    for (unsigned i = 0; i < 1000; i++) {
        size = sizeof(buf);
        base64_decode(base64, sizeof(base64), buf, &size);
    }
    stop = xtimer_now_usec();

    print_str("Decoding 1.000 x 96 bytes (128 bytes in base64): ");
    print_u32_dec(stop - start);
    print_str(" µs\n");
    return 0;
}
