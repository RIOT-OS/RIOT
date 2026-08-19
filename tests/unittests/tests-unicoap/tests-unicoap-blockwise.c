/*
 * SPDX-FileCopyrightText: 2024-2026 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup unittests
 * @brief   Unit tests for blockwise implementation kit
 * @author  Carl Seifert <carl.seifert@tu-dresden.de>
 */


#include <stdio.h>

#include "tests-unicoap.h"

#include "net/unicoap/message.h"
#include "net/unicoap/blockwise.h"

#define _TEST_ASSERT_EQUAL_BLOCK(block, number, size, m) \
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_number(block), number); \
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_size(block), size); \
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_more(block), m)

#define BLOCK32 "FIRST..:12345678-------_-------$"
#define BLOCK16 "SECOND.:12345678"
#define BLOCK_REMAINDER "THIRD123"

#define _sizeof_string(s) (sizeof(s) - 1)

static_assert(_sizeof_string(BLOCK32) == 32);
static_assert(_sizeof_string(BLOCK16) == 16);

#define BLOCK1_REPRESENTATION (BLOCK32 BLOCK16 BLOCK_REMAINDER)

static void test_block_option(void) {
    printf("TEST: %s\n", __func__);
    
    unicoap_block_option_t block = 0;
    
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_more(block), false);
    unicoap_block_set_more(&block, true);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_more(block), true);
    unicoap_block_set_more(&block, false);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_more(block), false);
    
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_number(block), 0);
    unicoap_block_set_number(&block, 42);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_number(block), 42);
    unicoap_block_set_number(&block, 0);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_number(block), 0);
    
    TEST_ASSERT_EQUAL_INT(unicoap_block_szx_from_size(16), UNICOAP_BLOCK_SZX_16);
    TEST_ASSERT_EQUAL_INT(unicoap_block_szx_from_size(32), UNICOAP_BLOCK_SZX_32);
    TEST_ASSERT_EQUAL_INT(unicoap_block_szx_from_size(512), UNICOAP_BLOCK_SZX_512);
    TEST_ASSERT_EQUAL_INT(unicoap_block_szx_from_size(1024), UNICOAP_BLOCK_SZX_1024);
    
    TEST_ASSERT_EQUAL_INT(unicoap_block_size_from_szx(UNICOAP_BLOCK_SZX_16), 16);
    TEST_ASSERT_EQUAL_INT(unicoap_block_size_from_szx(UNICOAP_BLOCK_SZX_32), 32);
    TEST_ASSERT_EQUAL_INT(unicoap_block_size_from_szx(UNICOAP_BLOCK_SZX_512), 512);
    TEST_ASSERT_EQUAL_INT(unicoap_block_size_from_szx(UNICOAP_BLOCK_SZX_1024), 1024);
    TEST_ASSERT_EQUAL_INT(unicoap_block_size_from_szx(UNICOAP_BLOCK_SZX_1024_BERT), UNICOAP_BERT_BLOCK_SIZE);
    
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), UNICOAP_BLOCK_SZX_16);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_size(block), 16);
    unicoap_block_set_szx(&block, UNICOAP_BLOCK_SZX_128);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), UNICOAP_BLOCK_SZX_128);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_size(block), 128);
    unicoap_block_set_szx(&block, UNICOAP_BLOCK_SZX_1024_BERT);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), UNICOAP_BLOCK_SZX_1024_BERT);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_size(block), 1024);
    unicoap_block_set_szx(&block, 0);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), 0);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_size(block), 16);

    unicoap_block_set_size(&block, 256);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), UNICOAP_BLOCK_SZX_256);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_size(block), 256);
    unicoap_block_set_szx(&block, 0);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), 0);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_size(block), 16);
    
    unicoap_block_set_number(&block, 42);
    unicoap_block_set_size(&block, 128);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_offset(block), 42 * 128);
    
    unicoap_block_set_number(&block, 42);
    unicoap_block_set_szx(&block, UNICOAP_BLOCK_SZX_1024_BERT);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_offset(block), 42 * 1024);
    
    unicoap_block_set_szx(&block, UNICOAP_BLOCK_SZX_1024_BERT);
    unicoap_block_set_number_from_offset(&block, 5 * 1024);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_number(block), 5);
    
    block = unicoap_block_from_szx(UNICOAP_BLOCK_SZX_1024_BERT, 9, true);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_number(block), 9);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), UNICOAP_BLOCK_SZX_1024_BERT);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_more(block), true);
    
    unicoap_block_info_t info;
    unicoap_block_get_info(block, &info);
    
    TEST_ASSERT_EQUAL_INT(info.size, 1024);
    TEST_ASSERT_EQUAL_INT(info.offset, 9 * 1024);
    TEST_ASSERT_EQUAL_INT(info.number, 9);
    TEST_ASSERT_EQUAL_INT(info.bert, true);
    TEST_ASSERT_EQUAL_INT(info.more_available, true);
    
    block = unicoap_block_from_size(256, 8, false);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_number(block), 8);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), UNICOAP_BLOCK_SZX_256);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_more(block), false);
    
    block = unicoap_block_from_info(&info);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_number(block), 9);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_szx(block), UNICOAP_BLOCK_SZX_1024_BERT);
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_more(block), true);
    
    block = UNICOAP_BLOCK_OPTION_NONE;
    TEST_ASSERT_EQUAL_INT(unicoap_block_get_more(block), false);
}

static void test_blockwise_block1(void) {
    printf("TEST: %s\n", __func__);
    int res = 0;
    unicoap_blockwise_iterator_t slicer; /* client */
    ssize_t chunk_size = 0; /* client/server (network) */
    uint8_t* chunk; /* client/server (network) */
    unicoap_block_option_t block; /* client/server (network) */
    unicoap_blockwise_iterator_t collector; /* client */
    
    /* client */
    static char* body = BLOCK1_REPRESENTATION;
        
    unicoap_blockwise_iterator_init(&slicer, UNICOAP_BLOCK_SZX_32, (uint8_t*)body, strlen(body));
    TEST_ASSERT_EQUAL_INT(slicer.offset, 0);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 0, 32, true);
    TEST_ASSERT_EQUAL_INT(slicer.block_option, (UNICOAP_BLOCK_SZX_32 | _UNICOAP_BLOCK_MORE_FLAG));
    
    chunk_size = unicoap_blockwise_slice_block1(&slicer, UNICOAP_BLOCK_OPTION_NONE, &chunk);
    TEST_ASSERT_EQUAL_INT(slicer.offset, 32);
    TEST_ASSERT_EQUAL_INT(chunk_size, 32);
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 0);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 0, 32, true);

    
    block = slicer.block_option;
    /* >>>>> request >>>>> */
    
    
    /* server */
    char body_buffer[_sizeof_string(BLOCK1_REPRESENTATION)];
    unicoap_blockwise_iterator_init(&collector, UNICOAP_BLOCK_SZX_16, (uint8_t*)body_buffer, sizeof(body_buffer));
    
    res = unicoap_blockwise_collect_block1(&collector, block, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 1);
    _TEST_ASSERT_EQUAL_BYTES(body_buffer + 0, body + 0, chunk_size);
    _TEST_ASSERT_EQUAL_BLOCK(collector.block_option, 0, 16, true);
    
    
    block = collector.block_option;
    /* <<<<< response <<<<< */
    
    
    /* client */
    chunk_size = unicoap_blockwise_slice_block1(&slicer, block, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, 16);
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 32);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 2, 16, true);

    
    block = slicer.block_option;
    /* >>>>> request >>>>> */
    
    
    /* server */
    res = unicoap_blockwise_collect_block1(&collector, block, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 1);
    _TEST_ASSERT_EQUAL_BYTES(body_buffer + 32, body + 32, chunk_size);
    _TEST_ASSERT_EQUAL_BLOCK(collector.block_option, 2, 16, true);
    
    
    block = collector.block_option;
    /* <<<<< response <<<<< */
    
    
    /* client */
    chunk_size = unicoap_blockwise_slice_block1(&slicer, block, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, _sizeof_string(BLOCK_REMAINDER));
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 32 + 16);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 3, 16, false);
    
    
    block = slicer.block_option;
    /* >>>>> request >>>>> */
    
    
    /* server */
    res = unicoap_blockwise_collect_block1(&collector, block, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 0);
    _TEST_ASSERT_EQUAL_BYTES(body_buffer + 32 + 16, body + 32 + 16, chunk_size);
    _TEST_ASSERT_EQUAL_BLOCK(collector.block_option, 3, 16, false);
    
    
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)collector.body, (uint8_t*)body_buffer);
    TEST_ASSERT_EQUAL_INT(collector.body_size, strlen(body));
    
    
    /* <<<<< response <<<<< */
    /* ... */
}

static void test_blockwise_block1_single_block_no_copy(void) {
    printf("TEST: %s\n", __func__);

    int res = 0;
    unicoap_blockwise_iterator_t slicer; /* client */
    ssize_t chunk_size = 0; /* client/server (network) */
    uint8_t* chunk; /* client/server (network) */
    unicoap_block_option_t block; /* client/server (network) */
    unicoap_blockwise_iterator_t collector; /* client */
    
    /* client */
    static char* body = BLOCK32;
    unicoap_blockwise_iterator_init(&slicer, UNICOAP_BLOCK_SZX_32, (uint8_t*)body, strlen(body));
    
    chunk_size = unicoap_blockwise_slice_block1(&slicer, UNICOAP_BLOCK_OPTION_NONE, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, 32);
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 0);
    TEST_ASSERT_EQUAL_INT(slicer.offset, 32);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 0, 32, false);
    
    
    block = slicer.block_option;
    /* >>>>> request >>>>> */
    
    
    /* server */
    char body_buffer[1];
    /* buffer definitely too small, test no copy mechanism */
    unicoap_blockwise_iterator_init(&collector, UNICOAP_BLOCK_SZX_16, (uint8_t*)body_buffer, sizeof(body_buffer));
    
    res = unicoap_blockwise_collect_block1(&collector, block, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 0);
    _TEST_ASSERT_EQUAL_BLOCK(collector.block_option, 0, 16, false);
    
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)collector.body, (uint8_t*)body);
    TEST_ASSERT_EQUAL_INT(collector.body_size, 32);
    
    
    /* <<<<< response <<<<< */
    /* ... */
}

static void test_blockwise_block1_no_block(void) {
    printf("TEST: %s\n", __func__);

    int res = 0;
    unicoap_blockwise_iterator_t slicer; /* client */
    ssize_t chunk_size = 0; /* client/server (network) */
    uint8_t* chunk; /* client/server (network) */
    unicoap_blockwise_iterator_t collector; /* client */
    
    /* client */
    static char* body = BLOCK16;
    unicoap_blockwise_iterator_init(&slicer, UNICOAP_BLOCK_SZX_16, (uint8_t*)body, strlen(body));
    
    chunk_size = unicoap_blockwise_slice_block1(&slicer, UNICOAP_BLOCK_OPTION_NONE, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, 16);
    TEST_ASSERT_EQUAL_INT(slicer.offset, 16);
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 0);
    
    /* >>>>> request >>>>> */
    
    /* server */
    char body_buffer[15];
    unicoap_blockwise_iterator_init(&collector, UNICOAP_BLOCK_SZX_64, (uint8_t*)body_buffer, sizeof(body_buffer));
    
    res = unicoap_blockwise_collect_block1(&collector, UNICOAP_BLOCK_OPTION_NONE, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 0);
    TEST_ASSERT_EQUAL_INT(collector.offset, 16);
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)collector.body, (uint8_t*)body);
    TEST_ASSERT_EQUAL_INT(collector.body_size, 16);
    
    /* <<<<< response <<<<< */
    /* ... */
}

static void test_blockwise_block2(bool send_first_control_block) {
    printf("TEST: %s\n", __func__);

    int res = 0;
    unicoap_blockwise_iterator_t collector; /* client */
    ssize_t chunk_size = 0; /* client/server (network) */
    uint8_t* chunk; /* client/server (network) */
    unicoap_block_option_t block; /* client/server (network) */
    unicoap_blockwise_iterator_t slicer; /* server */
    
    /* client */
    char body_buffer[60];
    unicoap_blockwise_iterator_init(&collector, UNICOAP_BLOCK_SZX_32, (uint8_t*)body_buffer, sizeof(body_buffer));

    res = unicoap_blockwise_collect_block2(&collector, UNICOAP_BLOCK_OPTION_NONE, NULL, 0);
    TEST_ASSERT_EQUAL_INT(res, 1);
    _TEST_ASSERT_EQUAL_BLOCK(collector.block_option, 0, 32, false);
    
    
    block = collector.block_option;
    /* >>>>> request >>>>> */
    
    
    /* server */
    static char* body = BLOCK32 BLOCK_REMAINDER;
    unicoap_blockwise_iterator_init(&slicer, UNICOAP_BLOCK_SZX_16, (uint8_t*)body, strlen(body));

    chunk_size = unicoap_blockwise_slice_block2(&slicer, send_first_control_block ? block : UNICOAP_BLOCK_OPTION_NONE, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, 16);
    TEST_ASSERT_EQUAL_INT(slicer.offset, 16);
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 0);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 0, 16, true);
    
    
    block = slicer.block_option;
    /* <<<<< response <<<<< */
    
    
    /* client */
    res = unicoap_blockwise_collect_block2(&collector, block, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 1);
    TEST_ASSERT_EQUAL_INT(collector.offset, 16);
    _TEST_ASSERT_EQUAL_BYTES(body_buffer + 0, body + 0, chunk_size);
    _TEST_ASSERT_EQUAL_BLOCK(collector.block_option, 1, 16, false);
    
    
    block = collector.block_option;
    /* >>>>> request >>>>> */
    
    
    /* server */
    /* test retransmissions */
    for (unsigned int i = 0; i < 3; i += 1) {
        chunk_size = unicoap_blockwise_slice_block2(&slicer, block, &chunk);
        TEST_ASSERT_EQUAL_INT(chunk_size, 16);
        TEST_ASSERT_EQUAL_INT(slicer.offset, 32);
        _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 16);
        _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 1, 16, true);
    }
    
    /* test future block request */
    block = unicoap_block_from_size(16, 2, false);
    chunk_size = unicoap_blockwise_slice_block2(&slicer, block, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, _sizeof_string(BLOCK_REMAINDER));
    TEST_ASSERT_EQUAL_INT(slicer.offset, 32 + _sizeof_string(BLOCK_REMAINDER));
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 32);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 2, 16, false);
    
    /* test past block request */
    block = unicoap_block_from_size(16, 0, false);
    chunk_size = unicoap_blockwise_slice_block2(&slicer, block, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, 16);
    TEST_ASSERT_EQUAL_INT(slicer.offset, 16);
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 0);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 0, 16, true);
    
    /* test current block request */
    block = unicoap_block_from_size(16, 1, false);
    chunk_size = unicoap_blockwise_slice_block2(&slicer, block, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, 16);
    TEST_ASSERT_EQUAL_INT(slicer.offset, 32);
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 16);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 1, 16, true);
    
    
    block = slicer.block_option;
    /* <<<<< response <<<<< */
    
    
    /* client */
    res = unicoap_blockwise_collect_block2(&collector, block, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 1);
    TEST_ASSERT_EQUAL_INT(collector.offset, 32);
    _TEST_ASSERT_EQUAL_BYTES(body_buffer + 16, body + 16, chunk_size);
    _TEST_ASSERT_EQUAL_BLOCK(collector.block_option, 2, 16, false);
    
    
    block = collector.block_option;
    /* >>>>> request >>>>> */
    
    
    /* server */
    chunk_size = unicoap_blockwise_slice_block2(&slicer, block, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, _sizeof_string(BLOCK_REMAINDER));
    TEST_ASSERT_EQUAL_INT(slicer.offset, 32 + _sizeof_string(BLOCK_REMAINDER));
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 32);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 2, 16, false);
    
    block = slicer.block_option;
    /* <<<<< response <<<<< */
    
    
    /* client */
    res = unicoap_blockwise_collect_block2(&collector, block, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 0);
    TEST_ASSERT_EQUAL_INT(collector.offset, 32 + _sizeof_string(BLOCK_REMAINDER));
    _TEST_ASSERT_EQUAL_BYTES(body_buffer + 8, body + 8, chunk_size);
    
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)collector.body, (uint8_t*)body_buffer);
    TEST_ASSERT_EQUAL_INT(collector.body_size, strlen(body));
}

static void test_blockwise_block2_single_block_no_copy(void) {
    printf("TEST: %s\n", __func__);

    int res = 0;
    unicoap_blockwise_iterator_t collector; /* client */
    ssize_t chunk_size = 0; /* client/server (network) */
    uint8_t* chunk; /* client/server (network) */
    unicoap_block_option_t block; /* client/server (network) */
    unicoap_blockwise_iterator_t slicer; /* server */
    
    /* client */
    char body_buffer[15];
    unicoap_blockwise_iterator_init(&collector, UNICOAP_BLOCK_SZX_16, (uint8_t*)body_buffer, sizeof(body_buffer));

    res = unicoap_blockwise_collect_block2(&collector, UNICOAP_BLOCK_OPTION_NONE, NULL, 0);
    TEST_ASSERT_EQUAL_INT(res, 1);
    _TEST_ASSERT_EQUAL_BLOCK(collector.block_option, 0, 16, false);
    
    
    block = collector.block_option;
    /* >>>>> request >>>>> */
    
    
    /* server */
    static char* body = BLOCK_REMAINDER;
    unicoap_blockwise_iterator_init(&slicer, UNICOAP_BLOCK_SZX_128, (uint8_t*)body, strlen(body));

    chunk_size = unicoap_blockwise_slice_block2(&slicer, block, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, _sizeof_string(BLOCK_REMAINDER));
    TEST_ASSERT_EQUAL_INT(slicer.offset, _sizeof_string(BLOCK_REMAINDER));
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 0);
    _TEST_ASSERT_EQUAL_BLOCK(slicer.block_option, 0, 16, false);

    
    block = slicer.block_option;
    /* <<<<< response <<<<< */
    
    
    /* client */
    res = unicoap_blockwise_collect_block2(&collector, block, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 0);
    TEST_ASSERT_EQUAL_INT(collector.offset, _sizeof_string(BLOCK_REMAINDER));
    
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)collector.body, (uint8_t*)body);
    TEST_ASSERT_EQUAL_INT(collector.body_size, strlen(body));
}

static void test_blockwise_block2_no_block(void) {
    printf("TEST: %s\n", __func__);

    int res = 0;
    unicoap_blockwise_iterator_t collector; /* client */
    ssize_t chunk_size = 0; /* client/server (network) */
    uint8_t* chunk; /* client/server (network) */
    unicoap_blockwise_iterator_t slicer; /* server */
    
    /* client */
    char body_buffer[15];
    unicoap_blockwise_iterator_init(&collector, UNICOAP_BLOCK_SZX_16, (uint8_t*)body_buffer, sizeof(body_buffer));
    
    /* >>>>> request >>>>> */
    
    /* server */
    static char* body = BLOCK_REMAINDER;
    unicoap_blockwise_iterator_init(&slicer, 4, (uint8_t*)body, strlen(body));

    chunk_size = unicoap_blockwise_slice_block2(&slicer, UNICOAP_BLOCK_OPTION_NONE, &chunk);
    TEST_ASSERT_EQUAL_INT(chunk_size, _sizeof_string(BLOCK_REMAINDER));
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)chunk, (uint8_t*)body + 0);
    
    /* <<<<< response <<<<< */
    
    /* client */
    res = unicoap_blockwise_collect_block2(&collector, UNICOAP_BLOCK_OPTION_NONE, chunk, chunk_size);
    TEST_ASSERT_EQUAL_INT(res, 0);
    TEST_ASSERT_EQUAL_INT(collector.offset, _sizeof_string(BLOCK_REMAINDER));
    
    _TEST_ASSERT_EQUAL_POINTER((uint8_t*)collector.body, (uint8_t*)body);
    TEST_ASSERT_EQUAL_INT(collector.body_size, strlen(body));
}

static void test_blockwise_block2_with_initial_control_block(void) {
    test_blockwise_block2(true);
}

static void test_blockwise_block2_without_initial_control_block(void) {
    test_blockwise_block2(false);
}

Test* tests_unicoap_blockwise(void) {
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_block_option),
        new_TestFixture(test_blockwise_block1),
        new_TestFixture(test_blockwise_block1_single_block_no_copy),
        new_TestFixture(test_blockwise_block1_no_block),
        new_TestFixture(test_blockwise_block2_with_initial_control_block),
        new_TestFixture(test_blockwise_block2_without_initial_control_block),
        new_TestFixture(test_blockwise_block2_single_block_no_copy),
        new_TestFixture(test_blockwise_block2_no_block),
    };

    EMB_UNIT_TESTCALLER(tests_unicoap, NULL, NULL, fixtures);

    return (Test*)&tests_unicoap;
}
