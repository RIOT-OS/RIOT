/*
 * SPDX-FileCopyrightText: 2026 Harshita
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Demo of the huffman_ee module: with and without the module
 *
 * On the same data stream, shows what a sensor node gets from the module:
 *
 *  - raw         : the sensor value sent as a plain fixed-width field. This is
 *                  what a node does when it has no entropy coder; the
 *                  compressors that ship with RIOT (heatshrink, lz4, uzlib)
 *                  optimise the size for repetitive data, not the bit energy
 *                  of the radio, so none of them is used here.
 *  - Huffman     : the plain Huffman code of the module
 *  - energy-aware: the module's energy-aware labelling (the point of it all)
 *
 * All rows are measured with the same energy model
 * alpha*N1 + beta*Nt + lambda*Ns, so the numbers can be compared directly.
 * The Huffman rows use the same tree, so they have exactly the same size:
 * only the energy differs.
 *
 * @author      Harshita <bansalharshita3012@gmail.com>
 * @}
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "huffman_ee/huffman_ee.h"

/* Number of symbols in the test streams */
#define STREAM_LEN      (5000)

#define NELEMS(a)       (sizeof(a) / sizeof((a)[0]))

/* Skewed data: temperatures (degrees C) and how often each was seen in the
 * Intel Berkeley Research Lab data set. A few values dominate, most are rare,
 * which is what sensor readings usually look like. */
static const uint16_t ibrl_sym[] = {
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28
};
static const uint32_t ibrl_freq[] = {
    6915, 27693, 56889, 109890, 165197, 223977, 268664,
    303627, 226412, 147592, 92266, 69796, 46821, 36394
};

/* Flat data: eight values, all equally likely. Every complete code needs all
 * eight 3-bit words, so no labelling can lower the energy here. */
static const uint16_t flat_sym[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
static const uint32_t flat_freq[] = { 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000 };

/* big, so they live outside the stack */
static huffman_ee_table_t table_plain;
static huffman_ee_table_t table_energy;
static huffman_ee_table_t table_exact;
static uint16_t stream[STREAM_LEN];
static uint8_t raw_bytes[STREAM_LEN * 2];

static uint32_t rng_state = 12345;

static uint32_t _rand(void)
{
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return rng_state;
}

static unsigned _popc(uint32_t x)
{
    unsigned c = 0;

    while (x) {
        c += x & 1u; x >>= 1;
    }
    return c;
}

static unsigned _trans(uint32_t x, unsigned w)
{
    if (w < 2) {
        return 0;
    }
    return _popc((x ^ (x >> 1)) & ((1u << (w - 1)) - 1u));
}

/* Fill stream[] with symbols drawn with the probabilities given by freq */
static void _make_stream(const uint16_t *sym, const uint32_t *freq, size_t num)
{
    uint32_t total = 0;

    for (size_t i = 0; i < num; i++) {
        total += freq[i];
    }
    for (size_t n = 0; n < STREAM_LEN; n++) {
        uint32_t pick = _rand() % total;
        size_t i = 0;
        while (pick >= freq[i]) {
            pick -= freq[i];
            i++;
        }
        stream[n] = sym[i];
    }
}

/* Pack `values` as fixed `width`-bit fields, most significant bit first */
static size_t _pack(const uint16_t *values, size_t n, unsigned width, uint8_t *out)
{
    size_t pos = 0;

    memset(out, 0, (n * width + 7) / 8);
    for (size_t i = 0; i < n; i++) {
        for (int b = (int)width - 1; b >= 0; b--) {
            if ((values[i] >> b) & 1u) {
                out[pos >> 3] |= (uint8_t)(0x80u >> (pos & 7));
            }
            pos++;
        }
    }
    return pos;
}

/* Count N1 / Nt / Ns of a bit buffer that is a sequence of `width`-bit fields */
static void _count_fields(const uint8_t *buf, size_t nbits, unsigned width,
                          huffman_ee_counts_t *c)
{
    bool have = false;
    uint32_t prev_last = 0;

    memset(c, 0, sizeof(*c));
    for (size_t i = 0; i + width <= nbits; i += width) {
        uint32_t v = 0;
        for (unsigned b = 0; b < width; b++) {
            size_t p = i + b;
            v = (v << 1) | ((buf[p >> 3] >> (7 - (p & 7))) & 1u);
        }
        c->ones += _popc(v);
        c->transitions += _trans(v, width);
        uint32_t first = (v >> (width - 1)) & 1u;
        if (have && (prev_last != first)) {
            c->boundaries++;
        }
        prev_last = v & 1u;
        have = true;
    }
}

/* Number of bits the stream needs with the given Huffman code */
static size_t _huffman_bits(const huffman_ee_table_t *table, const uint16_t *input,
                            size_t n)
{
    size_t bits = 0;

    for (size_t i = 0; i < n; i++) {
        for (uint16_t j = 0; j < table->num_symbols; j++) {
            if (table->symbol[j] == input[i]) {
                bits += table->code[j].length;
                break;
            }
        }
    }
    return bits;
}

/* Print one row of the table */
static void _row(const char *name, size_t bits, const huffman_ee_counts_t *c,
                 uint64_t energy, uint64_t baseline)
{
    int64_t bp = 0;
    const char *sign = "+";

    if (baseline > 0) {
        bp = ((int64_t)baseline - (int64_t)energy) * 10000 / (int64_t)baseline;
    }
    if (bp < 0) {
        sign = "-";
        bp = -bp;
    }
    printf("%-22s %7u %6" PRIu32 " %6" PRIu32 " %6" PRIu32 " %10" PRIu64
           "  %s%lld.%02lld %%\n",
           name, (unsigned)bits, c->ones, c->transitions, c->boundaries, energy,
           sign, (long long)(bp / 100), (long long)(bp % 100));
}

static void _run_case(const char *title, const uint16_t *sym, const uint32_t *freq,
                      size_t num, const huffman_ee_params_t *p)
{
    huffman_ee_counts_t c;
    unsigned width = 1;

    /* number of bits a plain fixed-width field needs for the alphabet */
    for (size_t i = 0; i < num; i++) {
        while ((1u << width) <= sym[i]) {
            width++;
        }
    }

    printf("=== %s ===\nweights: alpha=%" PRIu32 " beta=%" PRIu32 " lambda=%" PRIu32
           "\n\n", title, p->alpha, p->beta, p->lambda);
    printf("%-22s %7s %6s %6s %6s %10s  %8s\n", "method", "bits", "N1", "Nt", "Ns",
           "energy", "saving");

    _make_stream(sym, freq, num);

    /* without the module: the sensor value goes out uncompressed */
    size_t raw_bits = _pack(stream, STREAM_LEN, width, raw_bytes);
    _count_fields(raw_bytes, raw_bits, width, &c);
    uint64_t e_raw = huffman_ee_energy(&c, p);
    _row("raw (no compression)", raw_bits, &c, e_raw, e_raw);

    /* with the module: plain Huffman ... */
    huffman_ee_build_plain(&table_plain, sym, freq, num);
    huffman_ee_count(&table_plain, stream, STREAM_LEN, &c);
    uint64_t e_plain = huffman_ee_energy(&c, p);
    _row("Huffman, plain", _huffman_bits(&table_plain, stream, STREAM_LEN), &c,
         e_plain, e_raw);

    /* ... and the energy-aware labelling (this is the module's contribution) */
    huffman_ee_build(&table_energy, sym, freq, num, p);
    huffman_ee_count(&table_energy, stream, STREAM_LEN, &c);
    _row("Huffman, energy-aware", _huffman_bits(&table_energy, stream, STREAM_LEN), &c,
         huffman_ee_energy(&c, p), e_raw);

    huffman_ee_build_exact(&table_exact, sym, freq, num, p);
    huffman_ee_count(&table_exact, stream, STREAM_LEN, &c);
    _row("Huffman, exact optimal", _huffman_bits(&table_exact, stream, STREAM_LEN), &c,
         huffman_ee_energy(&c, p), e_raw);

    puts("\n(without the module the value is sent raw; the Huffman rows all have"
         " the same size and differ only in energy)\n");
}

int main(void)
{
    const huffman_ee_params_t p_paper = { .alpha = 20, .beta = 1, .lambda = 1 };
    const huffman_ee_params_t p_ones = { .alpha = 1, .beta = 1, .lambda = 1 };

    puts("huffman_ee demo: with and without the module\n");

    _run_case("skewed sensor data (IBRL temperatures)",
              ibrl_sym, ibrl_freq, NELEMS(ibrl_sym), &p_paper);
    _run_case("same data, alpha=1 (1 bits and transitions equal)",
              ibrl_sym, ibrl_freq, NELEMS(ibrl_sym), &p_ones);
    _run_case("flat data (worst case: all values equally likely)",
              flat_sym, flat_freq, NELEMS(flat_sym), &p_paper);
    return 0;
}
