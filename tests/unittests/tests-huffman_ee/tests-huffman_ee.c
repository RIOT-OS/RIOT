/*
 * SPDX-FileCopyrightText: 2026 Harshita
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Unittests for the ``huffman_ee`` module
 *
 * The expected values of the small examples were worked out by hand; the
 * calculation is written down next to each test.
 *
 * @author      Harshita <bansalharshita3012@gmail.com>
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "huffman_ee/huffman_ee.h"
#include "tests-huffman_ee.h"

#define NUM_BASELINES   (15)    /* random-label codes averaged for a baseline */
#define STREAM_LEN      (1000)  /* symbols in the stream used for round trips */

/* A small alphabet that can be worked out by hand: symbol 1 is the most
 * frequent, symbol 3 the rarest. */
static const uint16_t small_symbols[] = { 1, 2, 3 };
static const uint32_t small_freqs[] = { 5, 3, 1 };
#define SMALL_NUM       (3)

/* Temperatures (degrees C) and counts from the Intel Berkeley Research Lab
 * data set, the first data set of the paper */
static const uint16_t ibrl_symbols[] = {
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28
};
static const uint32_t ibrl_freqs[] = {
    6915, 27693, 56889, 109890, 165197, 223977, 268664,
    303627, 226412, 147592, 92266, 69796, 46821, 36394
};
#define IBRL_NUM        (14)

/* weights of the headline result of the paper */
static const huffman_ee_params_t params_paper = { .alpha = 20, .beta = 1, .lambda = 1 };

/* the tables are big, so they are not put on the stack */
static huffman_ee_table_t table;
static huffman_ee_table_t table_plain;
static huffman_ee_table_t table_random;

static uint16_t stream[STREAM_LEN];
static uint16_t decoded[STREAM_LEN];
static uint8_t encoded[STREAM_LEN * 2];

/* ---------------------------------------------------------------------- */
/* Helpers                                                                */
/* ---------------------------------------------------------------------- */

/* true if the codeword of entry i is the start of the codeword of entry j */
static int _is_prefix(const huffman_ee_table_t *t, int i, int j)
{
    uint8_t len_i = t->code[i].length;
    uint8_t len_j = t->code[j].length;

    if (len_i > len_j) {
        return 0;
    }
    return (t->code[j].bits >> (len_j - len_i)) == t->code[i].bits;
}

/* true if no codeword of the table is the start of another one */
static int _is_prefix_free(const huffman_ee_table_t *t)
{
    for (uint16_t i = 0; i < t->num_symbols; i++) {
        for (uint16_t j = 0; j < t->num_symbols; j++) {
            if ((i != j) && _is_prefix(t, i, j)) {
                return 0;
            }
        }
    }
    return 1;
}

/* A prefix-free code that uses every possible path of the tree has
 * sum(2^-length) == 1. Here: sum(2^(32 - length)) == 2^32. */
static int _is_complete(const huffman_ee_table_t *t)
{
    uint64_t sum = 0;

    for (uint16_t i = 0; i < t->num_symbols; i++) {
        sum += UINT64_C(1) << (HUFFMAN_EE_MAX_CODE_LEN - t->code[i].length);
    }
    return sum == (UINT64_C(1) << HUFFMAN_EE_MAX_CODE_LEN);
}

/* true if entry i of the table has exactly the given codeword */
static int _has_code(const huffman_ee_table_t *t, int i, uint32_t bits, uint8_t length)
{
    return (t->code[i].bits == bits) && (t->code[i].length == length);
}

/* Cost of the codewords weighted with how often they occur (the quantity the
 * build function tries to keep low): sum of freq * (alpha*ones + beta*transitions) */
static uint64_t _weighted_cost(const huffman_ee_table_t *t, const huffman_ee_params_t *p)
{
    uint64_t cost = 0;

    for (uint16_t i = 0; i < t->num_symbols; i++) {
        uint32_t bits = t->code[i].bits;
        uint32_t ones = 0;
        uint32_t transitions = 0;

        for (int b = 0; b < t->code[i].length; b++) {
            ones += (bits >> b) & 1;
            if ((b > 0) && (((bits >> b) & 1) != ((bits >> (b - 1)) & 1))) {
                transitions++;
            }
        }
        cost += (uint64_t)t->freq[i] * (((uint64_t)p->alpha * ones) +
                                        ((uint64_t)p->beta * transitions));
    }
    return cost;
}

/* The quantity the lambda-aware exact build minimises, multiplied by the total
 * frequency so that it stays integral:
 *   E_total = T * (alpha*N1_freq + beta*Nt_freq) + lambda*(a*b + (T-a)*(T-b))
 * where a is the frequency of codewords ending in 0 and b the frequency of
 * codewords starting with 1 (independent-symbol assumption). */
static uint64_t _lambda_objective(const huffman_ee_table_t *t, const huffman_ee_params_t *p)
{
    uint64_t total = 0;
    uint64_t ones_w = 0;
    uint64_t trans_w = 0;
    uint64_t ending_zero = 0;
    uint64_t starting_one = 0;

    for (uint16_t i = 0; i < t->num_symbols; i++) {
        uint32_t bits = t->code[i].bits;
        uint8_t len = t->code[i].length;
        uint32_t ones = 0;
        uint32_t transitions = 0;

        for (int b = 0; b < len; b++) {
            ones += (bits >> b) & 1;
            if ((b > 0) && (((bits >> b) & 1) != ((bits >> (b - 1)) & 1))) {
                transitions++;
            }
        }
        total += t->freq[i];
        ones_w += (uint64_t)t->freq[i] * ones;
        trans_w += (uint64_t)t->freq[i] * transitions;
        if ((bits & 1) == 0) {
            ending_zero += t->freq[i];
        }
        if (((bits >> (len - 1)) & 1) == 1) {
            starting_one += t->freq[i];
        }
    }
    return total * (((uint64_t)p->alpha * ones_w) + ((uint64_t)p->beta * trans_w))
           + ((uint64_t)p->lambda *
              ((ending_zero * starting_one) +
               ((total - ending_zero) * (total - starting_one))));
}

/* fill stream[] with symbols drawn with the probabilities of ibrl_freqs */
static void _make_ibrl_stream(void)
{
    uint32_t state = 2463534242u;
    uint32_t total = 0;

    for (int i = 0; i < IBRL_NUM; i++) {
        total += ibrl_freqs[i];
    }
    for (int n = 0; n < STREAM_LEN; n++) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        uint32_t pick = state % total;
        int i = 0;
        while (pick >= ibrl_freqs[i]) {
            pick -= ibrl_freqs[i];
            i++;
        }
        stream[n] = ibrl_symbols[i];
    }
}

/* ---------------------------------------------------------------------- */
/* Building the code                                                      */
/* ---------------------------------------------------------------------- */

/*
 * Plain Huffman on frequencies 5, 3, 1 (symbols 1, 2, 3). The lightest two
 * subtrees are merged first, the first one gets 0:
 *   merge symbol 3 (1) and symbol 2 (3):  symbol 3 -> 0,  symbol 2 -> 1
 *   merge that (4) and symbol 1 (5):      that -> 0,      symbol 1 -> 1
 * Result: symbol 1 = 1, symbol 2 = 01, symbol 3 = 00
 */
static void test_build_plain_known_code(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table, small_symbols, small_freqs,
                                                    SMALL_NUM));
    TEST_ASSERT_EQUAL_INT(SMALL_NUM, table.num_symbols);
    TEST_ASSERT(_has_code(&table, 0, 0x1, 1));  /* symbol 1: 1  */
    TEST_ASSERT(_has_code(&table, 1, 0x1, 2));  /* symbol 2: 01 */
    TEST_ASSERT(_has_code(&table, 2, 0x0, 2));  /* symbol 3: 00 */
}

/*
 * Energy-aware code with alpha = 20, beta = 1 on the same frequencies.
 * First merge: a = symbol 3 (freq 1), b = symbol 2 (freq 3).
 *   option a=0,b=1: symbol 3 "0" costs 0, symbol 2 "1" costs 3 * 20 = 60  -> 60
 *   option a=1,b=0: symbol 3 "1" costs 1 * 20 = 20, symbol 2 "0" costs 0  -> 20
 *   the second option is cheaper, so symbol 3 = 1 and symbol 2 = 0 (so far)
 * Second merge: a = that subtree (freq 4), b = symbol 1 (freq 5).
 *   option a=0,b=1: symbol 1 "1" = 5 * 20 = 100; symbol 3 "01" = 1 * (20 + 1)
 *                   = 21; symbol 2 "00" = 0                                -> 121
 *   option a=1,b=0: symbol 1 "0" = 0; symbol 3 "11" = 1 * 40 = 40;
 *                   symbol 2 "10" = 3 * (20 + 1) = 63                      -> 103
 *   the second option is cheaper.
 * Result: symbol 1 = 0, symbol 2 = 10, symbol 3 = 11
 */
static void test_build_energy_known_code(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, small_symbols, small_freqs,
                                              SMALL_NUM, &params_paper));
    TEST_ASSERT(_has_code(&table, 0, 0x0, 1));  /* symbol 1: 0  */
    TEST_ASSERT(_has_code(&table, 1, 0x2, 2));  /* symbol 2: 10 */
    TEST_ASSERT(_has_code(&table, 2, 0x3, 2));  /* symbol 3: 11 */
}

/* With alpha = 0 and beta = 0 every labelling costs the same. The tie rule
 * (first subtree gets 0) then makes the result equal to plain Huffman. */
static void test_build_energy_with_zero_weights_equals_plain(void)
{
    huffman_ee_params_t zero = { 0, 0, 0 };

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &zero));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table_plain, ibrl_symbols, ibrl_freqs,
                                                    IBRL_NUM));
    TEST_ASSERT_EQUAL_INT(0, memcmp(table.code, table_plain.code,
                                    IBRL_NUM * sizeof(huffman_ee_code_t)));
}

static void test_build_single_symbol(void)
{
    uint16_t sym = 7;
    uint32_t freq = 10;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, &sym, &freq, 1, &params_paper));
    TEST_ASSERT(_has_code(&table, 0, 0x0, 1));  /* one bit, so it can be sent */
}

static void test_build_two_symbols(void)
{
    uint16_t syms[] = { 4, 9 };
    uint32_t freqs[] = { 3, 9 };

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, syms, freqs, 2, &params_paper));
    /* the frequent symbol gets the cheap bit 0, the rare one gets 1 */
    TEST_ASSERT(_has_code(&table, 1, 0x0, 1));
    TEST_ASSERT(_has_code(&table, 0, 0x1, 1));
}

static void test_build_prefix_free_and_complete(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &params_paper));
    TEST_ASSERT(_is_prefix_free(&table));
    TEST_ASSERT(_is_complete(&table));

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table, ibrl_symbols, ibrl_freqs,
                                                    IBRL_NUM));
    TEST_ASSERT(_is_prefix_free(&table));
    TEST_ASSERT(_is_complete(&table));

    for (uint32_t seed = 1; seed <= NUM_BASELINES; seed++) {
        TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_random(&table, ibrl_symbols, ibrl_freqs,
                                                         IBRL_NUM, seed));
        TEST_ASSERT(_is_prefix_free(&table));
        TEST_ASSERT(_is_complete(&table));
    }
}

/* The energy-aware code only chooses labels. The tree is the same as for
 * plain Huffman, so every symbol keeps its codeword length. */
static void test_build_keeps_huffman_lengths(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table_plain, ibrl_symbols, ibrl_freqs,
                                                    IBRL_NUM));
    for (int i = 0; i < IBRL_NUM; i++) {
        TEST_ASSERT_EQUAL_INT(table_plain.code[i].length, table.code[i].length);
    }
}

/* Same input, same output: the build must not depend on anything random */
static void test_build_is_deterministic(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table_plain, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, memcmp(table.code, table_plain.code,
                                    IBRL_NUM * sizeof(huffman_ee_code_t)));
}

/* The point of the module: the cost the algorithm minimises is lower than
 * for normal Huffman coding and lower than the average of several random
 * labellings (the baseline used in the paper). */
static void test_build_lowers_cost(void)
{
    uint64_t random_sum = 0;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &params_paper));
    uint64_t ours = _weighted_cost(&table, &params_paper);

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table_plain, ibrl_symbols, ibrl_freqs,
                                                    IBRL_NUM));
    TEST_ASSERT(ours < _weighted_cost(&table_plain, &params_paper));

    for (uint32_t seed = 1; seed <= NUM_BASELINES; seed++) {
        TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_random(&table_random, ibrl_symbols,
                                                         ibrl_freqs, IBRL_NUM, seed));
        random_sum += _weighted_cost(&table_random, &params_paper);
    }
    TEST_ASSERT(ours < (random_sum / NUM_BASELINES));
}

/*
 * Four symbols that all occur equally often. Which two are merged first is
 * decided by the tie rule: the subtree that was created first goes first.
 *   merge symbol 1 and 2 -> subtree A   (1 = 0, 2 = 1)
 *   merge symbol 3 and 4 -> subtree B   (3 = 0, 4 = 1)
 *   merge A and B (A is older, so it is first and gets 0)
 * Result: symbol 1 = 00, symbol 2 = 01, symbol 3 = 10, symbol 4 = 11
 */
static void test_build_equal_frequencies_follow_tie_rule(void)
{
    const uint16_t syms[] = { 1, 2, 3, 4 };
    const uint32_t freqs[] = { 1, 1, 1, 1 };

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table, syms, freqs, 4));
    TEST_ASSERT(_has_code(&table, 0, 0x0, 2));
    TEST_ASSERT(_has_code(&table, 1, 0x1, 2));
    TEST_ASSERT(_has_code(&table, 2, 0x2, 2));
    TEST_ASSERT(_has_code(&table, 3, 0x3, 2));
}

static void test_build_rejects_bad_input(void)
{
    uint16_t dup_symbols[] = { 1, 1 };
    uint32_t zero_freqs[] = { 0, 4 };
    uint32_t two_freqs[] = { 3, 9 };
    uint32_t huge_freqs[] = { 0xF0000000u, 0xF0000000u };
    huffman_ee_params_t too_big = { HUFFMAN_EE_MAX_WEIGHT + 1, 1, 1 };

    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(NULL, small_symbols, small_freqs,
                                                    SMALL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(&table, NULL, small_freqs,
                                                    SMALL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(&table, small_symbols, NULL,
                                                    SMALL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(&table, small_symbols, small_freqs,
                                                    0, &params_paper));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(&table, small_symbols, small_freqs,
                                                    SMALL_NUM, NULL));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(&table, small_symbols, small_freqs,
                                                    SMALL_NUM, &too_big));
    /* the same symbol twice could not be decoded */
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(&table, dup_symbols, two_freqs,
                                                    2, &params_paper));
    /* a symbol that never occurs */
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(&table, small_symbols, zero_freqs,
                                                    2, &params_paper));
    /* frequencies that do not add up inside 32 bits */
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build(&table, small_symbols, huge_freqs,
                                                    2, &params_paper));
    /* the baseline builders do not need params */
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table, small_symbols, small_freqs,
                                                    SMALL_NUM));
}

static void test_build_too_many_symbols(void)
{
    static uint16_t syms[CONFIG_HUFFMAN_EE_MAX_SYMBOLS + 1];
    static uint32_t freqs[CONFIG_HUFFMAN_EE_MAX_SYMBOLS + 1];

    for (int i = 0; i <= CONFIG_HUFFMAN_EE_MAX_SYMBOLS; i++) {
        syms[i] = i;
        freqs[i] = 1 + i;
    }
    TEST_ASSERT_EQUAL_INT(-E2BIG, huffman_ee_build(&table, syms, freqs,
                                                   CONFIG_HUFFMAN_EE_MAX_SYMBOLS + 1,
                                                   &params_paper));
    /* exactly the allowed number still works */
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, syms, freqs,
                                              CONFIG_HUFFMAN_EE_MAX_SYMBOLS,
                                              &params_paper));
}

/* Fibonacci numbers as frequencies give the longest codes possible: with n
 * symbols the rarest ones need n - 1 bits. */
static void test_build_code_too_long(void)
{
    static uint16_t syms[40];
    static uint32_t freqs[40];
    uint32_t a = 1;
    uint32_t b = 1;

    for (int i = 0; i < 40; i++) {
        syms[i] = i;
        freqs[i] = a;
        uint32_t next = a + b;
        a = b;
        b = next;
    }
    /* 33 symbols need 32 bits: just fits */
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, syms, freqs, 33, &params_paper));
    TEST_ASSERT(_is_prefix_free(&table));
    /* 35 symbols need 34 bits: reported instead of giving a broken code */
    TEST_ASSERT_EQUAL_INT(-E2BIG, huffman_ee_build(&table, syms, freqs, 35, &params_paper));
    TEST_ASSERT_EQUAL_INT(-E2BIG, huffman_ee_build_plain(&table, syms, freqs, 35));
}

/* ---------------------------------------------------------------------- */
/* Counting and energy                                                    */
/* ---------------------------------------------------------------------- */

/*
 * Stream 1 2 3 1 1 2 with the plain code (1 = "1", 2 = "01", 3 = "00"):
 *   bits sent:  1 | 01 | 00 | 1 | 1 | 01
 *   N1 (ones)                 = 1+1+0+1+1+1 = 5
 *   Nt (changes inside)       = 0+1+0+0+0+1 = 2
 *   Ns (changes between):  1|01 yes, 01|00 yes, 00|1 yes, 1|1 no, 1|01 yes = 4
 *   E = 1*5 + 1*2 + 1*4 = 11
 */
static void test_count_plain_code(void)
{
    const uint16_t input[] = { 1, 2, 3, 1, 1, 2 };
    huffman_ee_counts_t counts;
    huffman_ee_params_t ones = { 1, 1, 1 };

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table, small_symbols, small_freqs,
                                                    SMALL_NUM));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_count(&table, input, 6, &counts));
    TEST_ASSERT_EQUAL_INT(5, counts.ones);
    TEST_ASSERT_EQUAL_INT(2, counts.transitions);
    TEST_ASSERT_EQUAL_INT(4, counts.boundaries);
    TEST_ASSERT_EQUAL_INT(11, huffman_ee_energy(&counts, &ones));
}

/*
 * Same stream with the energy-aware code (1 = "0", 2 = "10", 3 = "11"):
 *   bits sent:  0 | 10 | 11 | 0 | 0 | 10
 *   N1 = 0+1+2+0+0+1 = 4
 *   Nt = 0+1+0+0+0+1 = 2
 *   Ns: 0|10 yes, 10|11 yes, 11|0 yes, 0|0 no, 0|10 yes = 4
 *   E(alpha 20, beta 1, lambda 1) = 20*4 + 2 + 4 = 86
 * The plain code needs 20*5 + 2 + 4 = 106 for the same stream.
 */
static void test_count_energy_code(void)
{
    const uint16_t input[] = { 1, 2, 3, 1, 1, 2 };
    huffman_ee_counts_t counts;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, small_symbols, small_freqs,
                                              SMALL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_count(&table, input, 6, &counts));
    TEST_ASSERT_EQUAL_INT(4, counts.ones);
    TEST_ASSERT_EQUAL_INT(2, counts.transitions);
    TEST_ASSERT_EQUAL_INT(4, counts.boundaries);
    TEST_ASSERT_EQUAL_INT(86, huffman_ee_energy(&counts, &params_paper));
}

static void test_count_empty_and_bad_input(void)
{
    const uint16_t unknown[] = { 1, 99 };
    huffman_ee_counts_t counts;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, small_symbols, small_freqs,
                                              SMALL_NUM, &params_paper));
    /* an empty stream costs nothing */
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_count(&table, NULL, 0, &counts));
    TEST_ASSERT_EQUAL_INT(0, counts.ones + counts.transitions + counts.boundaries);
    /* a symbol that is not in the table */
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_count(&table, unknown, 2, &counts));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_count(NULL, unknown, 2, &counts));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_count(&table, unknown, 2, NULL));
}

static void test_energy_formula(void)
{
    huffman_ee_counts_t counts = { .ones = 10, .transitions = 20, .boundaries = 30 };
    huffman_ee_params_t weights = { .alpha = 2, .beta = 3, .lambda = 5 };

    /* 2*10 + 3*20 + 5*30 = 230 */
    TEST_ASSERT_EQUAL_INT(230, huffman_ee_energy(&counts, &weights));
}

/* The goal of the paper on a whole stream: lower energy than normal Huffman
 * and than the average of random labellings, with the same weights. */
static void test_stream_energy_is_lower(void)
{
    huffman_ee_counts_t counts;
    uint64_t random_sum = 0;

    _make_ibrl_stream();

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_count(&table, stream, STREAM_LEN, &counts));
    uint64_t ours = huffman_ee_energy(&counts, &params_paper);

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table_plain, ibrl_symbols, ibrl_freqs,
                                                    IBRL_NUM));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_count(&table_plain, stream, STREAM_LEN, &counts));
    TEST_ASSERT(ours < huffman_ee_energy(&counts, &params_paper));

    for (uint32_t seed = 1; seed <= NUM_BASELINES; seed++) {
        TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_random(&table_random, ibrl_symbols,
                                                         ibrl_freqs, IBRL_NUM, seed));
        TEST_ASSERT_EQUAL_INT(0, huffman_ee_count(&table_random, stream, STREAM_LEN,
                                                  &counts));
        random_sum += huffman_ee_energy(&counts, &params_paper);
    }
    TEST_ASSERT(ours < (random_sum / NUM_BASELINES));
}

/* ---------------------------------------------------------------------- */
/* Encoding and decoding                                                  */
/* ---------------------------------------------------------------------- */

/*
 * Energy-aware code of the small alphabet (1 = "0", 2 = "10", 3 = "11").
 * Stream 1 2 3 1 gives the bits 0 10 11 0 = 010110 (6 bits). Packed from the
 * top of the first byte: 0101 1000 = 0x58.
 */
static void test_encode_known_bits(void)
{
    const uint16_t input[] = { 1, 2, 3, 1 };
    uint8_t out[2] = { 0xFF, 0xFF };
    size_t bits = 0;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, small_symbols, small_freqs,
                                              SMALL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_encode(&table, input, 4, out, sizeof(out), &bits));
    TEST_ASSERT_EQUAL_INT(6, bits);
    TEST_ASSERT_EQUAL_INT(0x58, out[0]);
    /* the rest of the buffer is cleared */
    TEST_ASSERT_EQUAL_INT(0x00, out[1]);
}

static void test_roundtrip(void)
{
    size_t bits = 0;
    size_t len = 0;
    size_t expected_bits = 0;

    _make_ibrl_stream();
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_encode(&table, stream, STREAM_LEN, encoded,
                                               sizeof(encoded), &bits));

    /* the number of bits is the sum of the codeword lengths */
    for (int n = 0; n < STREAM_LEN; n++) {
        for (int i = 0; i < IBRL_NUM; i++) {
            if (table.symbol[i] == stream[n]) {
                expected_bits += table.code[i].length;
            }
        }
    }
    TEST_ASSERT_EQUAL_INT(expected_bits, bits);

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_decode(&table, encoded, bits, decoded,
                                               STREAM_LEN, &len));
    TEST_ASSERT_EQUAL_INT(STREAM_LEN, len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(stream, decoded, sizeof(stream)));
}

static void test_roundtrip_single_symbol(void)
{
    uint16_t sym = 7;
    uint32_t freq = 10;
    const uint16_t input[] = { 7, 7, 7 };
    uint16_t out[3];
    uint8_t buf[1];
    size_t bits = 0;
    size_t len = 0;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, &sym, &freq, 1, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_encode(&table, input, 3, buf, sizeof(buf), &bits));
    TEST_ASSERT_EQUAL_INT(3, bits);
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_decode(&table, buf, bits, out, 3, &len));
    TEST_ASSERT_EQUAL_INT(3, len);
    TEST_ASSERT_EQUAL_INT(0, memcmp(input, out, sizeof(input)));
}

static void test_encode_errors(void)
{
    const uint16_t input[] = { 1, 2, 3, 1, 1, 2 };
    const uint16_t unknown[] = { 1, 99 };
    uint8_t one_byte[1];
    uint8_t buf[8];
    size_t bits = 0;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, small_symbols, small_freqs,
                                              SMALL_NUM, &params_paper));
    /* 9 bits do not fit in one byte */
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, huffman_ee_encode(&table, input, 6, one_byte,
                                                      sizeof(one_byte), &bits));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_encode(&table, unknown, 2, buf,
                                                     sizeof(buf), &bits));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_encode(NULL, input, 6, buf, sizeof(buf),
                                                     &bits));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_encode(&table, input, 6, NULL, sizeof(buf),
                                                     &bits));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_encode(&table, input, 6, buf, sizeof(buf),
                                                     NULL));
}

static void test_decode_errors(void)
{
    const uint16_t input[] = { 1, 2, 3, 1, 1, 2 };
    uint16_t out[8];
    uint8_t buf[8];
    size_t bits = 0;
    size_t len = 0;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table, small_symbols, small_freqs,
                                              SMALL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_encode(&table, input, 6, buf, sizeof(buf), &bits));

    /* output buffer too small for 6 symbols */
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, huffman_ee_decode(&table, buf, bits, out, 2, &len));

    /* data that stops in the middle of a codeword: the last codeword "10"
     * with its last bit cut off is just "1", which is no codeword */
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_decode(&table, buf, bits - 1, out, 8, &len));

    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_decode(NULL, buf, bits, out, 8, &len));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_decode(&table, buf, bits, out, 8, NULL));

    /* no data at all decodes to no symbols */
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_decode(&table, NULL, 0, out, 8, &len));
    TEST_ASSERT_EQUAL_INT(0, len);
}

/* ---------------------------------------------------------------------- */
/* Exact optimal labelling                                                */
/* ---------------------------------------------------------------------- */

/* On 5,3,1 with alpha=20, beta=1 the exact optimum happens to equal the greedy
 * result (symbol 1 = "0", symbol 2 = "10", symbol 3 = "11"). */
static void test_build_exact_known_code(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_exact(&table, small_symbols, small_freqs,
                                                    SMALL_NUM, &params_paper));
    TEST_ASSERT(_has_code(&table, 0, 0x0, 1));
    TEST_ASSERT(_has_code(&table, 1, 0x2, 2));
    TEST_ASSERT(_has_code(&table, 2, 0x3, 2));
}

/* The exact build is never worse than the greedy one: both minimise the same
 * cost over the same tree. */
static void test_build_exact_not_worse_than_greedy(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_exact(&table, ibrl_symbols, ibrl_freqs,
                                                    IBRL_NUM, &params_paper));
    uint64_t exact = _weighted_cost(&table, &params_paper);

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build(&table_plain, ibrl_symbols, ibrl_freqs,
                                              IBRL_NUM, &params_paper));
    TEST_ASSERT(exact <= _weighted_cost(&table_plain, &params_paper));
}

/* Same tree, so the codeword lengths are the Huffman lengths again. */
static void test_build_exact_prefix_free_and_lengths(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_exact(&table, ibrl_symbols, ibrl_freqs,
                                                    IBRL_NUM, &params_paper));
    TEST_ASSERT(_is_prefix_free(&table));
    TEST_ASSERT(_is_complete(&table));

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table_plain, ibrl_symbols, ibrl_freqs,
                                                    IBRL_NUM));
    for (int i = 0; i < IBRL_NUM; i++) {
        TEST_ASSERT_EQUAL_INT(table_plain.code[i].length, table.code[i].length);
    }
}

static void test_build_exact_single_symbol(void)
{
    uint16_t sym = 9;
    uint32_t freq = 4;

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_exact(&table, &sym, &freq, 1, &params_paper));
    TEST_ASSERT(_has_code(&table, 0, 0x0, 1));
}

static void test_build_exact_rejects_bad_input(void)
{
    uint16_t dup_symbols[] = { 1, 1 };
    uint32_t two_freqs[] = { 3, 9 };
    huffman_ee_params_t too_big = { HUFFMAN_EE_MAX_WEIGHT + 1, 1, 1 };

    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build_exact(NULL, small_symbols, small_freqs,
                                                          SMALL_NUM, &params_paper));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build_exact(&table, small_symbols, small_freqs,
                                                          SMALL_NUM, NULL));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build_exact(&table, small_symbols, small_freqs,
                                                          SMALL_NUM, &too_big));
    TEST_ASSERT_EQUAL_INT(-EINVAL, huffman_ee_build_exact(&table, dup_symbols, two_freqs,
                                                          2, &params_paper));
}

/* The lambda-aware build counts the boundary term too: its objective is at
* least as low as for plain Huffman and for the pure alpha/beta optimum. */
static void test_build_exact_lambda_improves_objective(void)
{
    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_exact_lambda(&table, small_symbols,
                                                           small_freqs, SMALL_NUM,
                                                           &params_paper));
    TEST_ASSERT(_is_prefix_free(&table));
    uint64_t ours = _lambda_objective(&table, &params_paper);

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_plain(&table_plain, small_symbols,
                                                    small_freqs, SMALL_NUM));
    TEST_ASSERT(ours <= _lambda_objective(&table_plain, &params_paper));

    TEST_ASSERT_EQUAL_INT(0, huffman_ee_build_exact(&table_random, small_symbols,
                                                    small_freqs, SMALL_NUM, &params_paper));
    TEST_ASSERT(ours <= _lambda_objective(&table_random, &params_paper));
}

/* The lambda-aware build scales the objective by the total frequency, so it
 * refuses inputs for which the scaled costs would overflow 64 bits. */
static void test_build_exact_lambda_total_limit(void)
{
    uint16_t syms[] = { 1, 2 };
    uint32_t freqs[] = { 1500000000u, 1500000000u };    /* total 3e9: too large */

    TEST_ASSERT_EQUAL_INT(-E2BIG, huffman_ee_build_exact_lambda(&table, syms, freqs, 2,
                                                                &params_paper));
}

Test *tests_huffman_ee_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_build_plain_known_code),
        new_TestFixture(test_build_energy_known_code),
        new_TestFixture(test_build_energy_with_zero_weights_equals_plain),
        new_TestFixture(test_build_single_symbol),
        new_TestFixture(test_build_two_symbols),
        new_TestFixture(test_build_prefix_free_and_complete),
        new_TestFixture(test_build_keeps_huffman_lengths),
        new_TestFixture(test_build_is_deterministic),
        new_TestFixture(test_build_lowers_cost),
        new_TestFixture(test_build_equal_frequencies_follow_tie_rule),
        new_TestFixture(test_build_rejects_bad_input),
        new_TestFixture(test_build_too_many_symbols),
        new_TestFixture(test_build_code_too_long),
        new_TestFixture(test_build_exact_known_code),
        new_TestFixture(test_build_exact_not_worse_than_greedy),
        new_TestFixture(test_build_exact_prefix_free_and_lengths),
        new_TestFixture(test_build_exact_single_symbol),
        new_TestFixture(test_build_exact_rejects_bad_input),
        new_TestFixture(test_build_exact_lambda_improves_objective),
        new_TestFixture(test_build_exact_lambda_total_limit),
        new_TestFixture(test_count_plain_code),
        new_TestFixture(test_count_energy_code),
        new_TestFixture(test_count_empty_and_bad_input),
        new_TestFixture(test_energy_formula),
        new_TestFixture(test_stream_energy_is_lower),
        new_TestFixture(test_encode_known_bits),
        new_TestFixture(test_roundtrip),
        new_TestFixture(test_roundtrip_single_symbol),
        new_TestFixture(test_encode_errors),
        new_TestFixture(test_decode_errors),
    };

    EMB_UNIT_TESTCALLER(huffman_ee_tests, NULL, NULL, fixtures);

    return (Test *)&huffman_ee_tests;
}

void tests_huffman_ee(void)
{
    TESTS_RUN(tests_huffman_ee_tests());
}

/** @} */
