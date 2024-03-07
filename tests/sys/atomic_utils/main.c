/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Atomic util benchmark
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

#include "atomic_utils.h"
#include "fmt.h"
#include "mutex.h"
#include "random.h"
#include "shell.h"
#include "thread.h"
#include "timex.h"
#include "ztimer.h"

#include "volatile_utils.h"

typedef enum {
    TEST_TYPE_TEARING,
    TEST_TYPE_LOST_UPDATE,
    TEST_TYPE_NUMOF
} test_type_t;

typedef enum {
    TEST_WIDTH_8_BIT,
    TEST_WIDTH_16_BIT,
    TEST_WIDTH_32_BIT,
    TEST_WIDTH_64_BIT,
    TEST_WIDTH_NUMOF
} test_width_t;

typedef uint8_t (*fetch_op_u8_t)(volatile uint8_t *dest, uint8_t val);
typedef uint16_t (*fetch_op_u16_t)(volatile uint16_t *dest, uint16_t val);
typedef uint32_t (*fetch_op_u32_t)(volatile uint32_t *dest, uint32_t val);
typedef uint64_t (*fetch_op_u64_t)(volatile uint64_t *dest, uint64_t val);

typedef struct {
    const char *name;
    fetch_op_u8_t op;
    uint8_t operand;
    uint8_t noop_operand;
    uint8_t init;
    uint8_t allowed[4];
    uint8_t allowed_numof;
    uint8_t reinit_every;
} fetch_op_test_u8_t;

typedef struct {
    const char *name;
    fetch_op_u16_t op;
    uint16_t operand;
    uint16_t noop_operand;
    uint16_t init;
    uint16_t allowed[4];
    uint8_t allowed_numof;
    uint8_t reinit_every;
} fetch_op_test_u16_t;

typedef struct {
    const char *name;
    fetch_op_u32_t op;
    uint32_t operand;
    uint32_t noop_operand;
    uint32_t init;
    uint32_t allowed[4];
    uint8_t allowed_numof;
    uint8_t reinit_every;
} fetch_op_test_u32_t;

typedef struct {
    const char *name;
    fetch_op_u64_t op;
    uint64_t operand;
    uint64_t noop_operand;
    uint64_t init;
    uint64_t allowed[4];
    uint8_t allowed_numof;
    uint8_t reinit_every;
} fetch_op_test_u64_t;

typedef struct {
    test_type_t type;
    test_width_t width;
    unsigned idx;
} test_conf_t;

typedef struct {
    test_conf_t conf;
    uint8_t counter;
} test_state_t;

static const fetch_op_test_u8_t fetch_op_tests_u8[] = {
    /* atomic_*() */
    {
        .name = "atomic_fetch_add_u8",
        .op = atomic_fetch_add_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = {
            0x00, 0x55,
            0xaa, 0xff
        },
        .allowed_numof = 4,
        .init = 0x00,
        .reinit_every = 3,
    },
    {
        .name = "atomic_fetch_sub_u8",
        .op = atomic_fetch_sub_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = {
            0x00, 0x55,
            0xaa, 0xff
        },
        .allowed_numof = 4,
        .init = 0xff,
        .reinit_every = 3,
    },
    {
        .name = "atomic_fetch_or_u8",
        .op = atomic_fetch_or_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = { 0x00, 0x55 },
        .allowed_numof = 2,
        .init = 0x00,
        .reinit_every = 1,
    },
    {
        .name = "atomic_fetch_xor_u8",
        .op = atomic_fetch_xor_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = { 0x00, 0x55 },
        .allowed_numof = 2,
        .init = 0x00,
    },
    {
        .name = "atomic_fetch_and_u8",
        .op = atomic_fetch_and_u8,
        .operand = 0x55,
        .noop_operand = 0xff,
        .allowed = { 0xff, 0x55 },
        .allowed_numof = 2,
        .init = 0xff,
        .reinit_every = 1,
    },
    /* semi_atomic_*() */
    {
        .name = "semi_atomic_fetch_add_u8",
        .op = semi_atomic_fetch_add_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = {
            0x00, 0x55,
            0xaa, 0xff
        },
        .allowed_numof = 4,
        .init = 0x00,
        .reinit_every = 3,
    },
    {
        .name = "semi_atomic_fetch_sub_u8",
        .op = semi_atomic_fetch_sub_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = {
            0x00, 0x55,
            0xaa, 0xff
        },
        .allowed_numof = 4,
        .init = 0xff,
        .reinit_every = 3,
    },
    {
        .name = "semi_atomic_fetch_or_u8",
        .op = semi_atomic_fetch_or_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = { 0x00, 0x55 },
        .allowed_numof = 2,
        .init = 0x00,
        .reinit_every = 1,
    },
    {
        .name = "semi_atomic_fetch_xor_u8",
        .op = semi_atomic_fetch_xor_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = { 0x00, 0x55 },
        .allowed_numof = 2,
        .init = 0x00,
    },
    {
        .name = "semi_atomic_fetch_and_u8",
        .op = semi_atomic_fetch_and_u8,
        .operand = 0x55,
        .noop_operand = 0xff,
        .allowed = { 0xff, 0x55 },
        .allowed_numof = 2,
        .init = 0xff,
        .reinit_every = 1,
    },
    /* volatile_*() */
    {
        .name = "volatile_fetch_add_u8",
        .op = volatile_fetch_add_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = {
            0x00, 0x55,
            0xaa, 0xff
        },
        .allowed_numof = 4,
        .init = 0x00,
        .reinit_every = 3,
    },
    {
        .name = "volatile_fetch_sub_u8",
        .op = volatile_fetch_sub_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = {
            0x00, 0x55,
            0xaa, 0xff
        },
        .allowed_numof = 4,
        .init = 0xff,
        .reinit_every = 3,
    },
    {
        .name = "volatile_fetch_or_u8",
        .op = volatile_fetch_or_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = { 0x00, 0x55 },
        .allowed_numof = 2,
        .init = 0x00,
        .reinit_every = 1,
    },
    {
        .name = "volatile_fetch_xor_u8",
        .op = volatile_fetch_xor_u8,
        .operand = 0x55,
        .noop_operand = 0,
        .allowed = { 0x00, 0x55 },
        .allowed_numof = 2,
        .init = 0x00,
    },
    {
        .name = "volatile_fetch_and_u8",
        .op = volatile_fetch_and_u8,
        .operand = 0x55,
        .noop_operand = 0xff,
        .allowed = { 0xff, 0x55 },
        .allowed_numof = 2,
        .init = 0xff,
        .reinit_every = 1,
    },
};

static const fetch_op_test_u16_t fetch_op_tests_u16[] = {
    /* atomic_*() */
    {
        .name = "atomic_fetch_add_u16",
        .op = atomic_fetch_add_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = {
            0x0000, 0x5555,
            0xaaaa, 0xffff
        },
        .allowed_numof = 4,
        .init = 0x0000,
        .reinit_every = 3,
    },
    {
        .name = "atomic_fetch_sub_u16",
        .op = atomic_fetch_sub_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = {
            0x0000, 0x5555,
            0xaaaa, 0xffff
        },
        .allowed_numof = 4,
        .init = 0xffff,
        .reinit_every = 3,
    },
    {
        .name = "atomic_fetch_or_u16",
        .op = atomic_fetch_or_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = { 0x0000, 0x5555 },
        .allowed_numof = 2,
        .init = 0x0000,
        .reinit_every = 1,
    },
    {
        .name = "atomic_fetch_xor_u16",
        .op = atomic_fetch_xor_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = { 0x0000, 0x5555 },
        .allowed_numof = 2,
        .init = 0x0000,
    },
    {
        .name = "atomic_fetch_and_u16",
        .op = atomic_fetch_and_u16,
        .operand = 0x5555,
        .noop_operand = 0xffff,
        .allowed = { 0xffff, 0x5555 },
        .allowed_numof = 2,
        .init = 0xffff,
        .reinit_every = 1,
    },
    /* semi_atomic_*() */
    {
        .name = "semi_atomic_fetch_add_u16",
        .op = semi_atomic_fetch_add_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = {
            0x0000, 0x5555,
            0xaaaa, 0xffff
        },
        .allowed_numof = 4,
        .init = 0x0000,
        .reinit_every = 3,
    },
    {
        .name = "semi_atomic_fetch_sub_u16",
        .op = semi_atomic_fetch_sub_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = {
            0x0000, 0x5555,
            0xaaaa, 0xffff
        },
        .allowed_numof = 4,
        .init = 0xffff,
        .reinit_every = 3,
    },
    {
        .name = "semi_atomic_fetch_or_u16",
        .op = semi_atomic_fetch_or_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = { 0x0000, 0x5555 },
        .allowed_numof = 2,
        .init = 0x0000,
        .reinit_every = 1,
    },
    {
        .name = "semi_atomic_fetch_xor_u16",
        .op = semi_atomic_fetch_xor_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = { 0x0000, 0x5555 },
        .allowed_numof = 2,
        .init = 0x0000,
    },
    {
        .name = "semi_atomic_fetch_and_u16",
        .op = semi_atomic_fetch_and_u16,
        .operand = 0x5555,
        .noop_operand = 0xffff,
        .allowed = { 0xffff, 0x5555 },
        .allowed_numof = 2,
        .init = 0xffff,
        .reinit_every = 1,
    },
    /* volatile_*() */
    {
        .name = "volatile_fetch_add_u16",
        .op = volatile_fetch_add_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = {
            0x0000, 0x5555,
            0xaaaa, 0xffff
        },
        .allowed_numof = 4,
        .init = 0x0000,
        .reinit_every = 3,
    },
    {
        .name = "volatile_fetch_sub_u16",
        .op = volatile_fetch_sub_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = {
            0x0000, 0x5555,
            0xaaaa, 0xffff
        },
        .allowed_numof = 4,
        .init = 0xffff,
        .reinit_every = 3,
    },
    {
        .name = "volatile_fetch_or_u16",
        .op = volatile_fetch_or_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = { 0x0000, 0x5555 },
        .allowed_numof = 2,
        .init = 0x0000,
        .reinit_every = 1,
    },
    {
        .name = "volatile_fetch_xor_u16",
        .op = volatile_fetch_xor_u16,
        .operand = 0x5555,
        .noop_operand = 0,
        .allowed = { 0x0000, 0x5555 },
        .allowed_numof = 2,
        .init = 0x0000,
    },
    {
        .name = "volatile_fetch_and_u16",
        .op = volatile_fetch_and_u16,
        .operand = 0x5555,
        .noop_operand = 0xffff,
        .allowed = { 0xffff, 0x5555 },
        .allowed_numof = 2,
        .init = 0xffff,
        .reinit_every = 1,
    },
};

static const fetch_op_test_u32_t fetch_op_tests_u32[] = {
    /* atomic_*() */
    {
        .name = "atomic_fetch_add_u32",
        .op = atomic_fetch_add_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = {
            0x00000000, 0x55555555,
            0xaaaaaaaa, 0xffffffff
        },
        .allowed_numof = 4,
        .init = 0x00000000,
        .reinit_every = 3,
    },
    {
        .name = "atomic_fetch_sub_u32",
        .op = atomic_fetch_sub_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = {
            0x00000000, 0x55555555,
            0xaaaaaaaa, 0xffffffff
        },
        .allowed_numof = 4,
        .init = 0xffffffff,
        .reinit_every = 3,
    },
    {
        .name = "atomic_fetch_or_u32",
        .op = atomic_fetch_or_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = { 0x00000000, 0x55555555 },
        .allowed_numof = 2,
        .init = 0x00000000,
        .reinit_every = 1,
    },
    {
        .name = "atomic_fetch_xor_u32",
        .op = atomic_fetch_xor_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = { 0x00000000, 0x55555555 },
        .allowed_numof = 2,
        .init = 0x00000000,
    },
    {
        .name = "atomic_fetch_and_u32",
        .op = atomic_fetch_and_u32,
        .operand = 0x55555555,
        .noop_operand = 0xffffffff,
        .allowed = { 0xffffffff, 0x55555555 },
        .allowed_numof = 2,
        .init = 0xffffffff,
        .reinit_every = 1,
    },
    /* semi_atomic_*() */
    {
        .name = "semi_atomic_fetch_add_u32",
        .op = semi_atomic_fetch_add_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = {
            0x00000000, 0x55555555,
            0xaaaaaaaa, 0xffffffff
        },
        .allowed_numof = 4,
        .init = 0x00000000,
        .reinit_every = 3,
    },
    {
        .name = "semi_atomic_fetch_sub_u32",
        .op = semi_atomic_fetch_sub_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = {
            0x00000000, 0x55555555,
            0xaaaaaaaa, 0xffffffff
        },
        .allowed_numof = 4,
        .init = 0xffffffff,
        .reinit_every = 3,
    },
    {
        .name = "semi_atomic_fetch_or_u32",
        .op = semi_atomic_fetch_or_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = { 0x00000000, 0x55555555 },
        .allowed_numof = 2,
        .init = 0x00000000,
        .reinit_every = 1,
    },
    {
        .name = "semi_atomic_fetch_xor_u32",
        .op = semi_atomic_fetch_xor_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = { 0x00000000, 0x55555555 },
        .allowed_numof = 2,
        .init = 0x00000000,
    },
    {
        .name = "semi_atomic_fetch_and_u32",
        .op = semi_atomic_fetch_and_u32,
        .operand = 0x55555555,
        .noop_operand = 0xffffffff,
        .allowed = { 0xffffffff, 0x55555555 },
        .allowed_numof = 2,
        .init = 0xffffffff,
        .reinit_every = 1,
    },
    /* volatile_*() */
    {
        .name = "volatile_fetch_add_u32",
        .op = volatile_fetch_add_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = {
            0x00000000, 0x55555555,
            0xaaaaaaaa, 0xffffffff
        },
        .allowed_numof = 4,
        .init = 0x00000000,
        .reinit_every = 3,
    },
    {
        .name = "volatile_fetch_sub_u32",
        .op = volatile_fetch_sub_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = {
            0x00000000, 0x55555555,
            0xaaaaaaaa, 0xffffffff
        },
        .allowed_numof = 4,
        .init = 0xffffffff,
        .reinit_every = 3,
    },
    {
        .name = "volatile_fetch_or_u32",
        .op = volatile_fetch_or_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = { 0x00000000, 0x55555555 },
        .allowed_numof = 2,
        .init = 0x00000000,
        .reinit_every = 1,
    },
    {
        .name = "volatile_fetch_xor_u32",
        .op = volatile_fetch_xor_u32,
        .operand = 0x55555555,
        .noop_operand = 0,
        .allowed = { 0x00000000, 0x55555555 },
        .allowed_numof = 2,
        .init = 0x00000000,
    },
    {
        .name = "volatile_fetch_and_u32",
        .op = volatile_fetch_and_u32,
        .operand = 0x55555555,
        .noop_operand = 0xffffffff,
        .allowed = { 0xffffffff, 0x55555555 },
        .allowed_numof = 2,
        .init = 0xffffffff,
        .reinit_every = 1,
    },
};

static const fetch_op_test_u64_t fetch_op_tests_u64[] = {
    /* atomic_*() */
    {
        .name = "atomic_fetch_add_u64",
        .op = atomic_fetch_add_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = {
            0x0000000000000000, 0x5555555555555555,
            0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff
        },
        .allowed_numof = 4,
        .init = 0x0000000000000000,
        .reinit_every = 3,
    },
    {
        .name = "atomic_fetch_sub_u64",
        .op = atomic_fetch_sub_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = {
            0x0000000000000000, 0x5555555555555555,
            0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff
        },
        .allowed_numof = 4,
        .init = 0xffffffffffffffff,
        .reinit_every = 3,
    },
    {
        .name = "atomic_fetch_or_u64",
        .op = atomic_fetch_or_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = { 0x0000000000000000, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0x0000000000000000,
        .reinit_every = 1,
    },
    {
        .name = "atomic_fetch_xor_u64",
        .op = atomic_fetch_xor_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = { 0x0000000000000000, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0x0000000000000000,
    },
    {
        .name = "atomic_fetch_and_u64",
        .op = atomic_fetch_and_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0xffffffffffffffff,
        .allowed = { 0xffffffffffffffff, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0xffffffffffffffff,
        .reinit_every = 1,
    },
    /* semi_atomic_*() */
    {
        .name = "semi_atomic_fetch_add_u64",
        .op = semi_atomic_fetch_add_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = {
            0x0000000000000000, 0x5555555555555555,
            0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff
        },
        .allowed_numof = 4,
        .init = 0x0000000000000000,
        .reinit_every = 3,
    },
    {
        .name = "semi_atomic_fetch_sub_u64",
        .op = semi_atomic_fetch_sub_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = {
            0x0000000000000000, 0x5555555555555555,
            0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff
        },
        .allowed_numof = 4,
        .init = 0xffffffffffffffff,
        .reinit_every = 3,
    },
    {
        .name = "semi_atomic_fetch_or_u64",
        .op = semi_atomic_fetch_or_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = { 0x0000000000000000, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0x0000000000000000,
        .reinit_every = 1,
    },
    {
        .name = "semi_atomic_fetch_xor_u64",
        .op = semi_atomic_fetch_xor_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = { 0x0000000000000000, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0x0000000000000000,
    },
    {
        .name = "semi_atomic_fetch_and_u64",
        .op = semi_atomic_fetch_and_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0xffffffffffffffff,
        .allowed = { 0xffffffffffffffff, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0xffffffffffffffff,
        .reinit_every = 1,
    },
    /* volatile_*() */
    {
        .name = "volatile_fetch_add_u64",
        .op = volatile_fetch_add_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = {
            0x0000000000000000, 0x5555555555555555,
            0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff
        },
        .allowed_numof = 4,
        .init = 0x0000000000000000,
        .reinit_every = 3,
    },
    {
        .name = "volatile_fetch_sub_u64",
        .op = volatile_fetch_sub_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = {
            0x0000000000000000, 0x5555555555555555,
            0xaaaaaaaaaaaaaaaa, 0xffffffffffffffff
        },
        .allowed_numof = 4,
        .init = 0xffffffffffffffff,
        .reinit_every = 3,
    },
    {
        .name = "volatile_fetch_or_u64",
        .op = volatile_fetch_or_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = { 0x0000000000000000, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0x0000000000000000,
        .reinit_every = 1,
    },
    {
        .name = "volatile_fetch_xor_u64",
        .op = volatile_fetch_xor_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0,
        .allowed = { 0x0000000000000000, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0x0000000000000000,
    },
    {
        .name = "volatile_fetch_and_u64",
        .op = volatile_fetch_and_u64,
        .operand = 0x5555555555555555,
        .noop_operand = 0xffffffffffffffff,
        .allowed = { 0xffffffffffffffff, 0x5555555555555555 },
        .allowed_numof = 2,
        .init = 0xffffffffffffffff,
        .reinit_every = 1,
    },
};

static char thread_worker_stack[THREAD_STACKSIZE_SMALL];
static char thread_checker_stack[THREAD_STACKSIZE_SMALL];
static char thread_timeout_stack[THREAD_STACKSIZE_SMALL];

static test_conf_t conf;
static mutex_t conf_mutex = MUTEX_INIT_LOCKED;
static mutex_t stop_mutex = MUTEX_INIT_LOCKED;

static int testing_active = 0;

/* Testing values to operate on */
static uint8_t val_u8;
static uint16_t val_u16;
static uint32_t val_u32;
static uint64_t val_u64;

static uint64_t stats_ops;
static uint64_t stats_tests;
static uint64_t stats_failures;

static int sc_tearing_test(int argc, char **argv);
static int sc_lost_update_test(int argc, char **argv);
static int sc_stats(int argc, char **argv);
static int sc_stop(int argc, char **argv);
static int sc_list(int argc, char **argv);
static const shell_command_t shell_commands[] = {
    { "tearing_test", "Run a store/load tearing test", sc_tearing_test },
    { "lost_update_test", "Run a lost update test", sc_lost_update_test },
    { "stats", "Show stats of current test", sc_stats },
    { "stop", "Stop running test", sc_stop },
    { "list", "List functions that can be tested", sc_list },
    { NULL, NULL, NULL }
};

static void tearing_test_worker(test_state_t *state)
{
    switch (state->conf.width) {
        default:
            break;
        case TEST_WIDTH_8_BIT:
            {
                const fetch_op_test_u8_t *t = &fetch_op_tests_u8[state->conf.idx];
                if (state->counter >= t->reinit_every) {
                    atomic_store_u8(&val_u8, t->init);
                    state->counter = 0;
                }
                t->op(&val_u8, t->operand);
                state->counter++;
            }
            break;
        case TEST_WIDTH_16_BIT:
            {
                const fetch_op_test_u16_t *t = &fetch_op_tests_u16[state->conf.idx];
                if (state->counter >= t->reinit_every) {
                    atomic_store_u16(&val_u16, t->init);
                    state->counter = 0;
                }
                t->op(&val_u16, t->operand);
                state->counter++;
            }
            break;
        case TEST_WIDTH_32_BIT:
            {
                const fetch_op_test_u32_t *t = &fetch_op_tests_u32[state->conf.idx];
                if (state->counter >= t->reinit_every) {
                    atomic_store_u32(&val_u32, t->init);
                    state->counter = 0;
                }
                t->op(&val_u32, t->operand);
                state->counter++;
            }
            break;
        case TEST_WIDTH_64_BIT:
            {
                const fetch_op_test_u64_t *t = &fetch_op_tests_u64[state->conf.idx];
                if (state->counter >= t->reinit_every) {
                    atomic_store_u64(&val_u64, t->init);
                    state->counter = 0;
                }
                t->op(&val_u64, t->operand);
                state->counter++;
            }
            break;
    }
}

static void tearing_test_checker(test_state_t *state)
{
    switch (state->conf.width) {
        default:
            break;
        case TEST_WIDTH_8_BIT:
            {
                const fetch_op_test_u8_t *t = &fetch_op_tests_u8[state->conf.idx];
                uint8_t val = atomic_load_u8(&val_u8);
                for (uint8_t i = 0; i < t->allowed_numof; i++) {
                    if (t->allowed[i] == val) {
                        return;
                    }
                }
                print_str(t->name);
                print_str(": Load/store tearing detected. (Value was ");
                print_u32_hex(val);
                print_str(")\n");
                stats_failures++;
            }
            break;
        case TEST_WIDTH_16_BIT:
            {
                const fetch_op_test_u16_t *t = &fetch_op_tests_u16[state->conf.idx];
                uint16_t val = atomic_load_u16(&val_u16);
                for (uint8_t i = 0; i < t->allowed_numof; i++) {
                    if (t->allowed[i] == val) {
                        return;
                    }
                }
                print_str(t->name);
                print_str(": Load/store tearing detected. (Value was ");
                print_u32_hex(val);
                print_str(")\n");
                stats_failures++;
            }
            break;
        case TEST_WIDTH_32_BIT:
            {
                const fetch_op_test_u32_t *t = &fetch_op_tests_u32[state->conf.idx];
                uint32_t val = atomic_load_u32(&val_u32);
                for (uint8_t i = 0; i < t->allowed_numof; i++) {
                    if (t->allowed[i] == val) {
                        return;
                    }
                }
                print_str(t->name);
                print_str(": Load/store tearing detected. (Value was ");
                print_u32_hex(val);
                print_str(")\n");
                stats_failures++;
            }
            break;
        case TEST_WIDTH_64_BIT:
            {
                const fetch_op_test_u64_t *t = &fetch_op_tests_u64[state->conf.idx];
                uint64_t val = atomic_load_u64(&val_u64);
                for (uint8_t i = 0; i < t->allowed_numof; i++) {
                    if (t->allowed[i] == val) {
                        return;
                    }
                }
                print_str(t->name);
                print_str(": Load/store tearing detected. (Value was ");
                print_u64_hex(val);
                print_str(")\n");
                stats_failures++;
            }
            break;
    }
}

static void lost_update_test_worker(test_state_t *state)
{
    switch (state->conf.width) {
        default:
            break;
        case TEST_WIDTH_8_BIT:
            {
                const fetch_op_test_u8_t *t = &fetch_op_tests_u8[state->conf.idx];
                t->op(&val_u8, t->noop_operand);
            }
            break;
        case TEST_WIDTH_16_BIT:
            {
                const fetch_op_test_u16_t *t = &fetch_op_tests_u16[state->conf.idx];
                t->op(&val_u16, t->noop_operand);
            }
            break;
        case TEST_WIDTH_32_BIT:
            {
                const fetch_op_test_u32_t *t = &fetch_op_tests_u32[state->conf.idx];
                t->op(&val_u32, t->noop_operand);
            }
            break;
        case TEST_WIDTH_64_BIT:
            {
                const fetch_op_test_u64_t *t = &fetch_op_tests_u64[state->conf.idx];
                t->op(&val_u64, t->noop_operand);
            }
            break;
    }
}

static void lost_update_test_checker(test_state_t *state)
{
    switch (state->conf.width) {
        default:
            break;
        case TEST_WIDTH_8_BIT:
            {
                const fetch_op_test_u8_t *t = &fetch_op_tests_u8[state->conf.idx];
                uint8_t val = atomic_load_u8(&val_u8);
                if (val != state->counter) {
                    print_str(t->name);
                    print_str(": Lost update detected.\n");
                    stats_failures++;
                }
                atomic_store_u8(&val_u8, ++state->counter);
            }
            break;
        case TEST_WIDTH_16_BIT:
            {
                const fetch_op_test_u16_t *t = &fetch_op_tests_u16[state->conf.idx];
                uint16_t val = atomic_load_u16(&val_u16);
                if (val != state->counter) {
                    print_str(t->name);
                    print_str(": Lost update detected.\n");
                    stats_failures++;
                }
                atomic_store_u16(&val_u16, ++state->counter);
            }
            break;
        case TEST_WIDTH_32_BIT:
            {
                const fetch_op_test_u32_t *t = &fetch_op_tests_u32[state->conf.idx];
                uint32_t val = atomic_load_u32(&val_u32);
                if (val != state->counter) {
                    print_str(t->name);
                    print_str(": Lost update detected.\n");
                    stats_failures++;
                }
                atomic_store_u32(&val_u32, ++state->counter);
            }
            break;
        case TEST_WIDTH_64_BIT:
            {
                const fetch_op_test_u64_t *t = &fetch_op_tests_u64[state->conf.idx];
                uint64_t val = atomic_load_u64(&val_u64);
                if (val != state->counter) {
                    print_str(t->name);
                    print_str(": Lost update detected.\n");
                    stats_failures++;
                }
                atomic_store_u64(&val_u64, ++state->counter);
            }
            break;
    }
}

static void *thread_worker_func(void *arg)
{
    (void)arg;
    static test_state_t state = { .conf = { .idx = UINT8_MAX } };

    while (1) {
        mutex_lock(&conf_mutex);
        test_conf_t c = conf;
        stats_ops++;
        mutex_unlock(&conf_mutex);
        if (memcmp(&c, &state.conf, sizeof(c))) {
            state.conf = c;
            state.counter = UINT8_MAX;
        }

        switch (state.conf.type) {
            default:
                break;
            case TEST_TYPE_TEARING:
                tearing_test_worker(&state);
                break;
            case TEST_TYPE_LOST_UPDATE:
                lost_update_test_worker(&state);
                break;
        }
    }

    return NULL;
}

static void *thread_checker_func(void *arg)
{
    (void)arg;
    static test_state_t state = { .conf = { .idx = 0 } };

    while (1) {
        mutex_lock(&conf_mutex);
        test_conf_t c = conf;
        stats_tests++;
        mutex_unlock(&conf_mutex);
        if (memcmp(&c, &state.conf, sizeof(c))) {
            state.conf = c;
            state.counter = 0;
        }

        switch (state.conf.type) {
            default:
                break;
            case TEST_TYPE_TEARING:
                tearing_test_checker(&state);
                break;
            case TEST_TYPE_LOST_UPDATE:
                lost_update_test_checker(&state);
                break;
        }

        ztimer_sleep(ZTIMER_USEC, (random_uint32() & 0x3ff));
    }

    return NULL;
}

static void *thread_timeout_func(void *arg)
{
    (void)arg;
    while (1) {
        mutex_lock(&stop_mutex);
        sc_stop(0, NULL);
    }
    return NULL;
}

static void test_timeout_callback(void *arg)
{
    (void)arg;
    mutex_unlock(&stop_mutex);
}

static int start_test(test_width_t width, size_t fn_index, int timeout)
{
    conf.width = width;
    conf.idx = fn_index;
    testing_active = 1;
    stats_ops = 0;
    stats_tests = 0;
    stats_failures = 0;

    /* Initialize values. Doing so for every width safes ROM and lines of code
     * but wastes a few CPU cycles */
    if (conf.type == TEST_TYPE_LOST_UPDATE) {
        atomic_store_u8(&val_u8, 0);
        atomic_store_u16(&val_u16, 0);
        atomic_store_u32(&val_u32, 0);
        atomic_store_u64(&val_u64, 0);
    }
    else {
        const fetch_op_test_u8_t *t8 = &fetch_op_tests_u8[fn_index];
        const fetch_op_test_u16_t *thread_worker6 = &fetch_op_tests_u16[fn_index];
        const fetch_op_test_u32_t *t32 = &fetch_op_tests_u32[fn_index];
        const fetch_op_test_u64_t *t64 = &fetch_op_tests_u64[fn_index];
        atomic_store_u8(&val_u8, t8->init);
        atomic_store_u16(&val_u16, thread_worker6->init);
        atomic_store_u32(&val_u32, t32->init);
        atomic_store_u64(&val_u64, t64->init);
    }

    if (timeout) {
        static ztimer_t xt = { .callback = test_timeout_callback };
        ztimer_set(ZTIMER_USEC, &xt, US_PER_SEC * timeout);
    }
    mutex_unlock(&conf_mutex);
    return 0;
}

static int select_func_and_start_test(const char *funcname, int timeout)
{
    size_t fn_len = strlen(funcname);

    /* Valid function names end with *_u8, *_u16, *_u32, or *_u64. Thus, the
     * last char is already sufficient to determine the width. We do not need
     * to search all test specs for the given name, but only those of
     * matching width
     */
    switch (funcname[fn_len - 1]) {
        case '8':
            for (size_t i = 0; i < ARRAY_SIZE(fetch_op_tests_u8); i++) {
                if (!strcmp(fetch_op_tests_u8[i].name, funcname)) {
                    return start_test(TEST_WIDTH_8_BIT, i, timeout);
                }
            }
            break;
        case '6':
            for (size_t i = 0; i < ARRAY_SIZE(fetch_op_tests_u16); i++) {
                if (!strcmp(fetch_op_tests_u16[i].name, funcname)) {
                    return start_test(TEST_WIDTH_16_BIT, i, timeout);
                }
            }
            break;
        case '2':
            for (size_t i = 0; i < ARRAY_SIZE(fetch_op_tests_u32); i++) {
                if (!strcmp(fetch_op_tests_u32[i].name, funcname)) {
                    return start_test(TEST_WIDTH_32_BIT, i, timeout);
                }
            }
            break;
        case '4':
            for (size_t i = 0; i < ARRAY_SIZE(fetch_op_tests_u64); i++) {
                if (!strcmp(fetch_op_tests_u64[i].name, funcname)) {
                    return start_test(TEST_WIDTH_64_BIT, i, timeout);
                }
            }
            break;
    }

    print_str("Function \"");
    print_str(funcname);
    print_str("\" not found\n");
    return 1;

}

static int sc_tearing_test(int argc, char **argv)
{
    if ((argc != 2) && (argc != 3)) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <FUNCTION_TO_TEST>\n");
        return 1;
    }

    int timeout = 0;
    if (argc == 3) {
        timeout = atoi(argv[2]);
        if (timeout <= 0) {
            print_str("Invalid timeout\n");
            return 1;
        }
    }

    if (testing_active) {
        mutex_lock(&conf_mutex);
        testing_active = 0;
    }

    conf.type = TEST_TYPE_TEARING;
    return select_func_and_start_test(argv[1], timeout);
}

static int sc_lost_update_test(int argc, char **argv)
{
    if ((argc != 2) && (argc != 3)) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <FUNCTION_TO_TEST> [TIMEOUT_IN_SECONDS]\n");
        return 1;
    }

    int timeout = 0;
    if (argc == 3) {
        timeout = atoi(argv[2]);
        if (timeout <= 0) {
            print_str("Invalid timeout\n");
            return 1;
        }
    }

    if (testing_active) {
        mutex_lock(&conf_mutex);
        testing_active = 0;
    }

    conf.type = TEST_TYPE_LOST_UPDATE;
    return select_func_and_start_test(argv[1], timeout);
}

static int sc_stats(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (!testing_active) {
        print_str("No test active\n");
        return 0;
    }
    mutex_lock(&conf_mutex);
    print_u64_dec(stats_ops);
    print_str(" operations performed\n");
    print_u64_dec(stats_tests);
    print_str(" tests performed\n");
    print_u64_dec(stats_failures);
    print_str(" corruptions detected\n");
    mutex_unlock(&conf_mutex);
    return 0;
}

static int sc_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (testing_active) {
        mutex_lock(&conf_mutex);
        testing_active = 0;
        if (stats_failures) {
            print_str("ERROR: Detected ");
            print_u64_dec(stats_failures);
            print_str(" corruptions\n");
        }
        else {
            print_str("OK\n");
        }
    }
    return 0;
}

static int sc_list(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (size_t i = 0; i < ARRAY_SIZE(fetch_op_tests_u8); i++) {
        print_str(fetch_op_tests_u8[i].name);
        print_str("\n");
    }

    for (size_t i = 0; i < ARRAY_SIZE(fetch_op_tests_u16); i++) {
        print_str(fetch_op_tests_u16[i].name);
        print_str("\n");
    }

    for (size_t i = 0; i < ARRAY_SIZE(fetch_op_tests_u32); i++) {
        print_str(fetch_op_tests_u32[i].name);
        print_str("\n");
    }

    for (size_t i = 0; i < ARRAY_SIZE(fetch_op_tests_u64); i++) {
        print_str(fetch_op_tests_u64[i].name);
        print_str("\n");
    }

    return 0;
}

int main(void)
{
    thread_create(thread_worker_stack, sizeof(thread_worker_stack),
                  THREAD_PRIORITY_MAIN + 2, 0,
                  thread_worker_func, NULL, "worker");
    thread_create(thread_checker_stack, sizeof(thread_checker_stack),
                  THREAD_PRIORITY_MAIN + 1, 0,
                  thread_checker_func, NULL, "checker");
    thread_create(thread_timeout_stack, sizeof(thread_timeout_stack),
                  THREAD_PRIORITY_MAIN - 1, 0,
                  thread_timeout_func, NULL, "timeout");

    print_str(
        "Test Application for sys/atomic_utils\n"
        "=====================================\n"
        "\n"
        "Use the shell commands \"tearing_test\" and \"lost_update_test\" to\n"
        "test the various <flavor>_fetch_<op>_u<width> functions for lost\n"
        "updates and store tearing. The \"list\" shell commands lists\n"
        "functions to test. See below which function families should\n"
        "pass which tests.\n"
        "\n"
        "The atomic_fetch_<op>_u<width> family must pass all tests.\n"
        "\n"
        "The semi_atomic_fetch_<op>_u<width> family must pass the tearing\n"
        "test, but may fail the lost update test.\n"
        "\n"
        "The volatile_fetch_<op>_u<width> family should fail the lost update\n"
        "tests for all platforms. On most platforms they should fail the\n"
        "tearing tests for widths greater than the word size. (One exception\n"
        "is the Cortex-M7 family, which can by using instruction fusion issue\n"
        "two 32 bit writes in a single CPU cycle.). The volatile family is\n"
        "provided to verify that the test actually can detect issues. Any\n"
        "failure here is not an indication of an issue, but indicates the.\n"
        "test is working as expected.\n"
    );
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
