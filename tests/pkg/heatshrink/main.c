/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 * @file
 * @brief       Tests for the heatshrink compression library package
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */
#include <string.h>
#include <stdio.h>

#include "embUnit.h"

#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"

static const char *_data = "This is a test string TEST TEST TEST TEST!";

#define BUFSIZE (256U)

static uint8_t _buf[BUFSIZE];
static uint8_t _buf_res[BUFSIZE];

static heatshrink_encoder _encoder;
static heatshrink_decoder _decoder;

static int _comp_uncomp(const uint8_t *buf, size_t len)
{
    memset(_buf, 0, sizeof(_buf));
    memset(_buf_res, 0, sizeof(_buf_res));

    heatshrink_encoder_reset(&_encoder);
    heatshrink_decoder_reset(&_decoder);

    size_t n = len;
    uint8_t *inpos = (uint8_t*)buf;
    uint8_t *outpos = _buf;

    /* compress */
    while(1) {
        size_t n_sunk = 0;
        if (n) {
            heatshrink_encoder_sink(&_encoder, inpos, n, &n_sunk);
            if (n_sunk) {
                inpos += n_sunk;
                n -= n_sunk;
            }
            size_t written = 0;
            heatshrink_encoder_poll(&_encoder, outpos, (_buf + sizeof(_buf) - outpos), &written);
            outpos += written;
        }
        else {
            while (heatshrink_encoder_finish(&_encoder) == HSER_FINISH_MORE) {
                size_t written = 0;
                heatshrink_encoder_poll(&_encoder, outpos, (_buf + sizeof(_buf) - outpos), &written);
                outpos += written;
            }
            break;
        }
    }

    /* decompress */
    n = (outpos - _buf);
    inpos = (uint8_t*) _buf;
    outpos = _buf_res;

    while(1) {
        size_t n_sunk = 0;
        if (n) {
            heatshrink_decoder_sink(&_decoder, inpos, n, &n_sunk);
            if (n_sunk) {
                inpos += n_sunk;
                n -= n_sunk;
            }
            size_t written = 0;
            heatshrink_decoder_poll(&_decoder, outpos, (_buf + sizeof(_buf) - outpos), &written);
            outpos += written;
        }
        else {
            while (heatshrink_decoder_finish(&_decoder) == HSDR_FINISH_MORE) {
                size_t written = 0;
                heatshrink_decoder_poll(&_decoder, outpos, (_buf_res + sizeof(_buf_res) - outpos), &written);
                outpos += written;
            }
            break;
        }
    }

    if ((outpos - _buf_res) - len) {
        return -1;
    }

    return memcmp(buf, _buf_res, len);
}

static void test_heatshrink(void)
{
    TEST_ASSERT_EQUAL_INT(_comp_uncomp((const uint8_t*)_data, strlen(_data)), 0);
}

TestRef tests_heatshrink(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_heatshrink),
    };

    EMB_UNIT_TESTCALLER(HeatshrinkTest, 0, 0, fixtures);
    return (TestRef) & HeatshrinkTest;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_heatshrink());
    TESTS_END();

    return 0;
}
