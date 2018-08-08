/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Arithmetic code benchmark
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "periph/timer.h"

#ifndef TEST_BLOCKSIZE
#define TEST_BLOCKSIZE       128
#endif
#ifndef TEST_ITERATIONS
#define TEST_ITERATIONS      128
#endif

#ifndef TIM_REF_DEV
#define TIM_REF_DEV     TIMER_DEV(0)
#endif

#ifndef TIM_REF_FREQ
#define TIM_REF_FREQ    1000000ul
#endif

/* working area */
static uint64_t buf[TEST_BLOCKSIZE];

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

/* template for test function with variable operands */
#define TEST_FUNC_VARIABLE(func, T, op) \
    uint32_t test_ ## T ## _ ## func ## _var(T *buf, size_t nelem) \
    { \
        unsigned time_start = timer_read(TIM_REF_DEV); \
        for (unsigned k = 0; k < (nelem - 1); ++k) { \
            buf[k] op (buf[k + 1]); \
        } \
        unsigned time_end = timer_read(TIM_REF_DEV); \
        return (time_end - time_start); \
    }

/* template for test function with compile-time constant right operand */
#define TEST_FUNC_CCONST(func, T, op, constant) \
    uint32_t test_ ## T ## _ ## func ## _cconst(T *buf, size_t nelem) \
    { \
        unsigned time_start = timer_read(TIM_REF_DEV); \
        for (unsigned k = 0; k < (nelem - 1); ++k) { \
            buf[k] op constant; \
        } \
        unsigned time_end = timer_read(TIM_REF_DEV); \
        return (time_end - time_start); \
    }

/* template for test function with run-time constant right operand */
/* This test case leaves fewer possibilities for the compiler to optimize the
 * generated code than when using a compile time constant */
#define TEST_FUNC_RCONST(func, T, op) \
    uint32_t test_ ## T ## _ ## func ## _rconst(T *buf, size_t nelem, const T constant) \
    { \
        unsigned time_start = timer_read(TIM_REF_DEV); \
        for (unsigned k = 0; k < (nelem - 1); ++k) { \
            buf[k] op constant; \
        } \
        unsigned time_end = timer_read(TIM_REF_DEV); \
        return (time_end - time_start); \
    }

/* Instancing all three above variants with one macro */
#define TEST_FUNC_OP_INSTANCES(func, T, op, constant) \
    TEST_FUNC_VARIABLE(func, T, op) \
    TEST_FUNC_CCONST(func, T, op, constant) \
    TEST_FUNC_RCONST(func, T, op)

/* Further instancing all operations with one macro */
#define TEST_FUNC_INSTANCES(T, constant) \
    TEST_FUNC_OP_INSTANCES(set, T,   =, constant); \
    TEST_FUNC_OP_INSTANCES(add, T,  +=, constant); \
    TEST_FUNC_OP_INSTANCES(sub, T,  -=, constant); \
    TEST_FUNC_OP_INSTANCES(mul, T,  *=, constant); \
    TEST_FUNC_OP_INSTANCES(div, T,  /=, constant); \
    TEST_FUNC_OP_INSTANCES(lsh, T, <<=, (sizeof(T) * 8 - 1) & (constant)); \
    TEST_FUNC_OP_INSTANCES(rsh, T, >>=, (sizeof(T) * 8 - 1) & (constant)); \
    TEST_FUNC_OP_INSTANCES( or, T,  |=, constant); \
    TEST_FUNC_OP_INSTANCES(and, T,  &=, constant); \
    TEST_FUNC_OP_INSTANCES(xor, T,  ^=, constant); \

#define TEST_FUNC_FLOAT_INSTANCES(T, constant) \
    TEST_FUNC_OP_INSTANCES(set, T,   =, constant); \
    TEST_FUNC_OP_INSTANCES(add, T,  +=, constant); \
    TEST_FUNC_OP_INSTANCES(sub, T,  -=, constant); \
    TEST_FUNC_OP_INSTANCES(mul, T,  *=, constant); \
    TEST_FUNC_OP_INSTANCES(div, T,  /=, constant);

/* Run one (op,T) test */
#define RUN_TEST(func, T, op, rconst, fill_func) \
    time_var = 0; \
    time_cconst = 0; \
    time_rconst = 0; \
    for (unsigned j = 0; j < TEST_ITERATIONS; ++j) { \
        fill_func(buf, ARRAY_LEN(buf), seed); \
        time_var += test_ ## T ## _ ## func ## _var((T *)buf, ARRAY_LEN(buf)); \
    } \
    for (unsigned j = 0; j < TEST_ITERATIONS; ++j) { \
        fill_func(buf, ARRAY_LEN(buf), seed); \
        time_cconst += test_ ## T ## _ ## func ## _cconst((T *)buf, ARRAY_LEN(buf)); \
    } \
    for (unsigned j = 0; j < TEST_ITERATIONS; ++j) { \
        fill_func(buf, ARRAY_LEN(buf), seed); \
        time_rconst += test_ ## T ## _ ## func ## _rconst((T *)buf, ARRAY_LEN(buf), rconst); \
    } \
    printf("%8s, %3s: %7" PRIu32 " %7" PRIu32 " %7" PRIu32 "\n", #T, #op, time_var, time_cconst, time_rconst) \

/* Run all tests for the given type T */
#define RUN_TESTS(T, rconst) \
    RUN_TEST(set, T,   =, rconst, fill_buf); \
    RUN_TEST(add, T,  +=, rconst, fill_buf); \
    RUN_TEST(sub, T,  -=, rconst, fill_buf); \
    RUN_TEST(mul, T,  *=, rconst, fill_buf); \
    RUN_TEST(div, T,  /=, rconst, fill_buf); \
    RUN_TEST(lsh, T, <<=, (sizeof(T) * 8 - 1) & (rconst), fill_buf); \
    RUN_TEST(rsh, T, >>=, (sizeof(T) * 8 - 1) & (rconst), fill_buf); \
    RUN_TEST( or, T,  |=, rconst, fill_buf); \
    RUN_TEST(and, T,  &=, rconst, fill_buf); \
    RUN_TEST(xor, T,  ^=, rconst, fill_buf);

#define RUN_DOUBLE_TESTS(T, rconst) \
    RUN_TEST(set, T,   =, rconst, fill_buf_double); \
    RUN_TEST(add, T,  +=, rconst, fill_buf_double); \
    RUN_TEST(sub, T,  -=, rconst, fill_buf_double); \
    RUN_TEST(mul, T,  *=, rconst, fill_buf_double); \
    RUN_TEST(div, T,  /=, rconst, fill_buf_double);

#define RUN_FLOAT_TESTS(T, rconst) \
    RUN_TEST(set, T,   =, rconst, fill_buf_float); \
    RUN_TEST(add, T,  +=, rconst, fill_buf_float); \
    RUN_TEST(sub, T,  -=, rconst, fill_buf_float); \
    RUN_TEST(mul, T,  *=, rconst, fill_buf_float); \
    RUN_TEST(div, T,  /=, rconst, fill_buf_float);

#define RUN_ALL_TESTS() \
    RUN_TESTS( uint8_t,                  211u) \
    RUN_TESTS(uint16_t,                64951u) \
    RUN_TESTS(uint32_t,           3392920849ul) \
    RUN_TESTS(uint64_t, 16611429676448458297ull) \
    RUN_TESTS(  int8_t,                  113) \
    RUN_TESTS( int16_t,                30047) \
    RUN_TESTS( int32_t,           2048015227l) \
    RUN_TESTS( int64_t,  6576191449585486549ll) \
    RUN_FLOAT_TESTS( float, 4.81047738097f) \
    RUN_DOUBLE_TESTS(double, 1.31083249443)

TEST_FUNC_INSTANCES( uint8_t,                  211u) \
TEST_FUNC_INSTANCES(uint16_t,                64951u) \
TEST_FUNC_INSTANCES(uint32_t,           3392920849ul) \
TEST_FUNC_INSTANCES(uint64_t, 16611429676448458297ull) \
TEST_FUNC_INSTANCES(  int8_t,                  113) \
TEST_FUNC_INSTANCES( int16_t,                30047) \
TEST_FUNC_INSTANCES( int32_t,           2048015227l) \
TEST_FUNC_INSTANCES( int64_t,  6576191449585486549ll) \
TEST_FUNC_FLOAT_INSTANCES( float, 4.81047738097f) \
TEST_FUNC_FLOAT_INSTANCES(double, 1.31083249443)

/* Basic LCG */
void fill_buf(uint64_t *buf, size_t nelem, uint64_t seed)
{
    for (unsigned k = 0; k < nelem; ++k) {
        do {
            seed = 6364136223846793005ull * seed + 1;
        } while (seed == 0); /* to avoid division by zero later */
        buf[k] = seed;
    }
}

/* A simple floating point variation of the above filler function */
void fill_buf_double(uint64_t *uint_buf, size_t nelem, uint64_t seed)
{
    double *buf = (double *)uint_buf;
    for (unsigned k = 0; k < nelem; ++k) {
        do {
            seed = 6364136223846793005ull * seed + 1;
        } while (seed == 0); /* to avoid division by zero later */
        buf[k] = ((double) seed) / (1ul << 31);
    }
}

void fill_buf_float(uint64_t *uint_buf, size_t nelem, uint64_t seed)
{
    float *buf = (float *)uint_buf;
    for (unsigned k = 0; k < nelem; ++k) {
        do {
            seed = 6364136223846793005ull * seed + 1;
        } while (seed == 0); /* to avoid division by zero later */
        buf[k] = ((float) seed) / (1ul << 31);
    }
}

void timer_cb(void *arg, int chan)
{
    (void) arg;
    (void) chan;
    puts("Warning! spurious timer interrupt");
}

int main(void)
{
    puts("Arithmetic benchmark");

    puts("Init timer");
    printf("TIM_REF_DEV: %u\n", (unsigned)TIM_REF_DEV);
    printf("TIM_REF_FREQ: %lu\n", (unsigned long)TIM_REF_FREQ);
    int res = timer_init(TIM_REF_DEV, TIM_REF_FREQ, timer_cb, NULL);
    if (res < 0) {
        puts("Error initializing timer!");
        while(1) {}
    }
    uint64_t seed = 12345;
    uint32_t variation = 4321;
    while (1) {
        ++seed;
        uint32_t time_var = 0;
        uint32_t time_cconst = 0;
        uint32_t time_rconst = 0;
        printf("\n\nTime in 1/%lu seconds for applying operator op to %u blocks of size %u (%lu values)\n\n",
            TIM_REF_FREQ, TEST_ITERATIONS, TEST_BLOCKSIZE, (unsigned long) TEST_ITERATIONS * TEST_BLOCKSIZE);
        printf("%8s, %3s: %7s %7s %7s\n", "type", "op", "var", "cconst", "rconst");
        RUN_ALL_TESTS();
        ++variation;
    }
    return 0;
}
