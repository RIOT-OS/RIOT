/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Atomic util benchmark
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>

#include "atomic_utils.h"
#include "macros/utils.h"
#include "xtimer.h"

/* On fast CPUs: 1.000.000 loops */
#if defined(CPU_CORE_CORTEX_M7) || defined(CPU_ESP32)
#define LOOPS 1000000
#else
/* Else 100.000 loops */
#define LOOPS 100000
#endif

enum {
    IMPL_VOLATILE,
    IMPL_ATOMIC_UTIL,
    IMPL_C11_ATOMIC,
    IMPL_NUMOF
};

#define BENCH_ATOMIC_STORE(name, type, c11type) \
    static void CONCAT(bench_atomic_store_, name)(uint32_t *result_us)         \
    {                                                                          \
        uint32_t start, stop;                                                  \
                                                                               \
        {                                                                      \
            volatile type val;                                                 \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                val = 42;                                                      \
            }                                                                  \
            (void)val;                                                         \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_VOLATILE] = stop - start;                           \
        }                                                                      \
                                                                               \
        {                                                                      \
            type val;                                                          \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                CONCAT(atomic_store_, name)(&val, 42);                         \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_ATOMIC_UTIL] = stop - start;                        \
        }                                                                      \
                                                                               \
        {                                                                      \
            c11type val;                                                       \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                atomic_store(&val, 42);                                        \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_C11_ATOMIC] = stop - start;                         \
        }                                                                      \
    }
BENCH_ATOMIC_STORE(u8, uint8_t, atomic_uint_least8_t)
BENCH_ATOMIC_STORE(u16, uint16_t, atomic_uint_least16_t)
BENCH_ATOMIC_STORE(u32, uint32_t, atomic_uint_least32_t)
BENCH_ATOMIC_STORE(u64, uint64_t, atomic_uint_least64_t)

#define BENCH_ATOMIC_LOAD(name, type, c11type) \
    static void CONCAT(bench_atomic_load_, name)(uint32_t *result_us)          \
    {                                                                          \
        uint32_t start, stop;                                                  \
                                                                               \
        {                                                                      \
            volatile type val = 0;                                             \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                type tmp = val;                                                \
                (void)tmp;                                                     \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_VOLATILE] = stop - start;                           \
        }                                                                      \
                                                                               \
        {                                                                      \
            type val = 0;                                                      \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                type tmp = CONCAT(atomic_load_, name)(&val);                   \
                (void)tmp;                                                     \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_ATOMIC_UTIL] = stop - start;                        \
        }                                                                      \
                                                                               \
        {                                                                      \
            c11type val = ATOMIC_VAR_INIT(0);                                  \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                type tmp = atomic_load(&val);                                  \
                (void)tmp;                                                     \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_C11_ATOMIC] = stop - start;                         \
        }                                                                      \
    }
BENCH_ATOMIC_LOAD(u8, uint8_t, atomic_uint_least8_t)
BENCH_ATOMIC_LOAD(u16, uint16_t, atomic_uint_least16_t)
BENCH_ATOMIC_LOAD(u32, uint32_t, atomic_uint_least32_t)
BENCH_ATOMIC_LOAD(u64, uint64_t, atomic_uint_least64_t)

#define BENCH_ATOMIC_FETCH_OP(opname, op, name, type, c11type) \
    static void CONCAT4(bench_atomic_fetch_, opname, _, name)(uint32_t *result_us) \
    {                                                                          \
        uint32_t start, stop;                                                  \
                                                                               \
        {                                                                      \
            volatile type val = 0;                                             \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                val = val op 1;                                                \
            }                                                                  \
            (void)val;                                                         \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_VOLATILE] = stop - start;                           \
        }                                                                      \
                                                                               \
        {                                                                      \
            type val = 0;                                                      \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                CONCAT4(atomic_fetch_, opname, _, name)(&val, 1);              \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_ATOMIC_UTIL] = stop - start;                        \
        }                                                                      \
                                                                               \
        {                                                                      \
            c11type val = ATOMIC_VAR_INIT(0);                                  \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                CONCAT(atomic_fetch_, opname)(&val, 1);                        \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_C11_ATOMIC] = stop - start;                         \
        }                                                                      \
    }
BENCH_ATOMIC_FETCH_OP(add, +, u8, uint8_t, atomic_uint_least8_t)
BENCH_ATOMIC_FETCH_OP(add, +, u16, uint16_t, atomic_uint_least16_t)
BENCH_ATOMIC_FETCH_OP(add, +, u32, uint32_t, atomic_uint_least32_t)
BENCH_ATOMIC_FETCH_OP(add, +, u64, uint64_t, atomic_uint_least64_t)
BENCH_ATOMIC_FETCH_OP(sub, -, u8, uint8_t, atomic_uint_least8_t)
BENCH_ATOMIC_FETCH_OP(sub, -, u16, uint16_t, atomic_uint_least16_t)
BENCH_ATOMIC_FETCH_OP(sub, -, u32, uint32_t, atomic_uint_least32_t)
BENCH_ATOMIC_FETCH_OP(sub, -, u64, uint64_t, atomic_uint_least64_t)
BENCH_ATOMIC_FETCH_OP(or, |, u8, uint8_t, atomic_uint_least8_t)
BENCH_ATOMIC_FETCH_OP(or, |, u16, uint16_t, atomic_uint_least16_t)
BENCH_ATOMIC_FETCH_OP(or, |, u32, uint32_t, atomic_uint_least32_t)
BENCH_ATOMIC_FETCH_OP(or, |, u64, uint64_t, atomic_uint_least64_t)
BENCH_ATOMIC_FETCH_OP(xor, ^, u8, uint8_t, atomic_uint_least8_t)
BENCH_ATOMIC_FETCH_OP(xor, ^, u16, uint16_t, atomic_uint_least16_t)
BENCH_ATOMIC_FETCH_OP(xor, ^, u32, uint32_t, atomic_uint_least32_t)
BENCH_ATOMIC_FETCH_OP(xor, ^, u64, uint64_t, atomic_uint_least64_t)
BENCH_ATOMIC_FETCH_OP(and, &, u8, uint8_t, atomic_uint_least8_t)
BENCH_ATOMIC_FETCH_OP(and, &, u16, uint16_t, atomic_uint_least16_t)
BENCH_ATOMIC_FETCH_OP(and, &, u32, uint32_t, atomic_uint_least32_t)
BENCH_ATOMIC_FETCH_OP(and, &, u64, uint64_t, atomic_uint_least64_t)

#define BENCH_ATOMIC_SET_CLEAR_BIT(name, type, c11type, opname, set_or_clear) \
    static void CONCAT4(bench_atomic_, opname, _bit_, name)(uint32_t *result_us) \
    {                                                                          \
        uint32_t start, stop;                                                  \
        static const uint8_t _bit = 5;                                         \
        type mask = ((type)1) << _bit;                                         \
                                                                               \
        {                                                                      \
            volatile type val = 0;                                             \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                if (set_or_clear) {                                            \
                    val |= mask;                                               \
                }                                                              \
                else {                                                         \
                    val &= ~(mask);                                            \
                }                                                              \
            }                                                                  \
            (void)val;                                                         \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_VOLATILE] = stop - start;                           \
        }                                                                      \
                                                                               \
        {                                                                      \
            static type val = 0;                                               \
            CONCAT3(atomic_bit_, name, _t) bit =                               \
                CONCAT(atomic_bit_, name)(&val, _bit);                         \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                CONCAT4(atomic_, opname, _bit_, name)(bit);                    \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_ATOMIC_UTIL] = stop - start;                        \
        }                                                                      \
                                                                               \
        {                                                                      \
            c11type val = ATOMIC_VAR_INIT(0);                                  \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                if (set_or_clear) {                                            \
                    atomic_fetch_or(&val, mask);                               \
                }                                                              \
                else {                                                         \
                    atomic_fetch_and(&val, ~(mask));                           \
                }                                                              \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_C11_ATOMIC] = stop - start;                         \
        }                                                                      \
    }
BENCH_ATOMIC_SET_CLEAR_BIT(u8, uint8_t, atomic_uint_least8_t, set, 1)
BENCH_ATOMIC_SET_CLEAR_BIT(u16, uint16_t, atomic_uint_least16_t, set, 1)
BENCH_ATOMIC_SET_CLEAR_BIT(u32, uint32_t, atomic_uint_least32_t, set, 1)
BENCH_ATOMIC_SET_CLEAR_BIT(u64, uint64_t, atomic_uint_least64_t, set, 1)
BENCH_ATOMIC_SET_CLEAR_BIT(u8, uint8_t, atomic_uint_least8_t, clear, 0)
BENCH_ATOMIC_SET_CLEAR_BIT(u16, uint16_t, atomic_uint_least16_t, clear, 0)
BENCH_ATOMIC_SET_CLEAR_BIT(u32, uint32_t, atomic_uint_least32_t, clear, 0)
BENCH_ATOMIC_SET_CLEAR_BIT(u64, uint64_t, atomic_uint_least64_t, clear, 0)

#define BENCH_SEMI_ATOMIC_FETCH_OP(opname, op, name, type, c11type) \
    static void CONCAT4(bench_semi_atomic_fetch_, opname, _, name)(uint32_t *result_us) \
    {                                                                          \
        uint32_t start, stop;                                                  \
                                                                               \
        {                                                                      \
            volatile type val = 0;                                             \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                val = val op 1;                                                \
            }                                                                  \
            (void)val;                                                         \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_VOLATILE] = stop - start;                           \
        }                                                                      \
                                                                               \
        {                                                                      \
            type val = 0;                                                      \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                CONCAT4(semi_atomic_fetch_, opname, _, name)(&val, 1);         \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_ATOMIC_UTIL] = stop - start;                        \
        }                                                                      \
                                                                               \
        {                                                                      \
            c11type val = ATOMIC_VAR_INIT(0);                                  \
            start = xtimer_now_usec();                                         \
            for (uint32_t i = 0; i < LOOPS; i++) {                             \
                CONCAT(atomic_fetch_, opname)(&val, 1);                        \
            }                                                                  \
            stop = xtimer_now_usec();                                          \
            result_us[IMPL_C11_ATOMIC] = stop - start;                         \
        }                                                                      \
    }
BENCH_SEMI_ATOMIC_FETCH_OP(add, +, u8, uint8_t, atomic_uint_least8_t)
BENCH_SEMI_ATOMIC_FETCH_OP(add, +, u16, uint16_t, atomic_uint_least16_t)
BENCH_SEMI_ATOMIC_FETCH_OP(add, +, u32, uint32_t, atomic_uint_least32_t)
BENCH_SEMI_ATOMIC_FETCH_OP(add, +, u64, uint64_t, atomic_uint_least64_t)
BENCH_SEMI_ATOMIC_FETCH_OP(sub, -, u8, uint8_t, atomic_uint_least8_t)
BENCH_SEMI_ATOMIC_FETCH_OP(sub, -, u16, uint16_t, atomic_uint_least16_t)
BENCH_SEMI_ATOMIC_FETCH_OP(sub, -, u32, uint32_t, atomic_uint_least32_t)
BENCH_SEMI_ATOMIC_FETCH_OP(sub, -, u64, uint64_t, atomic_uint_least64_t)
BENCH_SEMI_ATOMIC_FETCH_OP(or, |, u8, uint8_t, atomic_uint_least8_t)
BENCH_SEMI_ATOMIC_FETCH_OP(or, |, u16, uint16_t, atomic_uint_least16_t)
BENCH_SEMI_ATOMIC_FETCH_OP(or, |, u32, uint32_t, atomic_uint_least32_t)
BENCH_SEMI_ATOMIC_FETCH_OP(or, |, u64, uint64_t, atomic_uint_least64_t)
BENCH_SEMI_ATOMIC_FETCH_OP(xor, ^, u8, uint8_t, atomic_uint_least8_t)
BENCH_SEMI_ATOMIC_FETCH_OP(xor, ^, u16, uint16_t, atomic_uint_least16_t)
BENCH_SEMI_ATOMIC_FETCH_OP(xor, ^, u32, uint32_t, atomic_uint_least32_t)
BENCH_SEMI_ATOMIC_FETCH_OP(xor, ^, u64, uint64_t, atomic_uint_least64_t)
BENCH_SEMI_ATOMIC_FETCH_OP(and, &, u8, uint8_t, atomic_uint_least8_t)
BENCH_SEMI_ATOMIC_FETCH_OP(and, &, u16, uint16_t, atomic_uint_least16_t)
BENCH_SEMI_ATOMIC_FETCH_OP(and, &, u32, uint32_t, atomic_uint_least32_t)
BENCH_SEMI_ATOMIC_FETCH_OP(and, &, u64, uint64_t, atomic_uint_least64_t)

#define LINE "+------+----------+------+------------------+"                   \
             "------------------+------------------+"
#define FMT "| %4s | %8s | %4u | %13" PRIu32 " µs | %13" PRIu32 " µs | "       \
            "%13" PRIu32 " µs |\n"
int main(void)
{
    uint32_t results[IMPL_NUMOF];

    puts("Note: LOWER IS BETTER!\n");
    puts(LINE);
    printf("| %4s | %8s | %4s | %16s | %16s | %16s |\n",
           "mode", "op", "bits", "volatile", "atomic util", "c11 atomic");
    puts(LINE);

    bench_atomic_store_u8(results);
    printf(FMT, "atom", "store", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_store_u16(results);
    printf(FMT, "atom", "store", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_store_u32(results);
    printf(FMT, "atom", "store", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_store_u64(results);
    printf(FMT, "atom", "store", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_atomic_load_u8(results);
    printf(FMT, "atom", "load", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_load_u16(results);
    printf(FMT, "atom", "load", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_load_u32(results);
    printf(FMT, "atom", "load", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_load_u64(results);
    printf(FMT, "atom", "load", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    /* atomic read-modify-write operations */
    bench_atomic_fetch_add_u8(results);
    printf(FMT, "atom", "add", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_add_u16(results);
    printf(FMT, "atom", "add", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_add_u32(results);
    printf(FMT, "atom", "add", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_add_u64(results);
    printf(FMT, "atom", "add", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_atomic_fetch_sub_u8(results);
    printf(FMT, "atom", "sub", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_sub_u16(results);
    printf(FMT, "atom", "sub", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_sub_u32(results);
    printf(FMT, "atom", "sub", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_sub_u64(results);
    printf(FMT, "atom", "sub", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_atomic_fetch_or_u8(results);
    printf(FMT, "atom", "or", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_or_u16(results);
    printf(FMT, "atom", "or", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_or_u32(results);
    printf(FMT, "atom", "or", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_or_u64(results);
    printf(FMT, "atom", "or", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_atomic_fetch_xor_u8(results);
    printf(FMT, "atom", "xor", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_xor_u16(results);
    printf(FMT, "atom", "xor", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_xor_u32(results);
    printf(FMT, "atom", "xor", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_xor_u64(results);
    printf(FMT, "atom", "xor", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_atomic_fetch_and_u8(results);
    printf(FMT, "atom", "and", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_and_u16(results);
    printf(FMT, "atom", "and", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_and_u32(results);
    printf(FMT, "atom", "and", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_fetch_and_u64(results);
    printf(FMT, "atom", "and", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    /* atomic bit setting and clearing */
    bench_atomic_set_bit_u8(results);
    printf(FMT, "atom", "set", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_set_bit_u16(results);
    printf(FMT, "atom", "set", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_set_bit_u32(results);
    printf(FMT, "atom", "set", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_set_bit_u64(results);
    printf(FMT, "atom", "set", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_atomic_clear_bit_u8(results);
    printf(FMT, "atom", "clear", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_clear_bit_u16(results);
    printf(FMT, "atom", "clear", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_clear_bit_u32(results);
    printf(FMT, "atom", "clear", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_atomic_clear_bit_u64(results);
    printf(FMT, "atom", "clear", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    /* semi-atomic read-modify-write operations */
    bench_semi_atomic_fetch_add_u8(results);
    printf(FMT, "semi", "add", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_add_u16(results);
    printf(FMT, "semi", "add", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_add_u32(results);
    printf(FMT, "semi", "add", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_add_u64(results);
    printf(FMT, "semi", "add", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_semi_atomic_fetch_sub_u8(results);
    printf(FMT, "semi", "sub", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_sub_u16(results);
    printf(FMT, "semi", "sub", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_sub_u32(results);
    printf(FMT, "semi", "sub", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_sub_u64(results);
    printf(FMT, "semi", "sub", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_semi_atomic_fetch_or_u8(results);
    printf(FMT, "semi", "or", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_or_u16(results);
    printf(FMT, "semi", "or", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_or_u32(results);
    printf(FMT, "semi", "or", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_or_u64(results);
    printf(FMT, "semi", "or", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_semi_atomic_fetch_xor_u8(results);
    printf(FMT, "semi", "xor", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_xor_u16(results);
    printf(FMT, "semi", "xor", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_xor_u32(results);
    printf(FMT, "semi", "xor", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_xor_u64(results);
    printf(FMT, "semi", "xor", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);

    bench_semi_atomic_fetch_and_u8(results);
    printf(FMT, "semi", "and", 8, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_and_u16(results);
    printf(FMT, "semi", "and", 16, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_and_u32(results);
    printf(FMT, "semi", "and", 32, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    bench_semi_atomic_fetch_and_u64(results);
    printf(FMT, "semi", "and", 64, results[IMPL_VOLATILE],
           results[IMPL_ATOMIC_UTIL], results[IMPL_C11_ATOMIC]);
    puts(LINE);
    return 0;
}
