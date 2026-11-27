/*
 * SPDX-FileCopyrightText: 2026 Harshita
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_huffman_ee
 * @{
 *
 * @file
 * @brief       Building the energy-aware Huffman code (Phases 1-3 of
 *              Algorithm 1 in the paper)
 *
 * How the code is built, in short:
 *
 *  1. Every symbol starts as a subtree of its own with an empty codeword.
 *  2. Repeatedly take the two subtrees with the lowest total frequency and
 *     merge them. Merging means that every codeword in one subtree gets a 0
 *     put in front of it and every codeword in the other subtree gets a 1.
 *  3. There are two ways to do that (A gets 0 and B gets 1, or the other way
 *     round). Normal Huffman coding does not care. We work out the cost of
 *     both ways and use the cheaper one.
 *  4. Continue until one subtree is left. Every symbol now has its codeword.
 *
 * The code is kept as a codeword per symbol, not as a pointer tree. Which
 * symbols belong to a subtree is remembered in a linked list of table
 * indices. This needs no dynamic memory.
 *
 * @author      Harshita <bansalharshita3012@gmail.com>
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "huffman_ee/huffman_ee.h"
#include "huffman_ee_internal.h"

/* Table indices are stored in 16 bit values and 0xFFFF means "none" */
_Static_assert(CONFIG_HUFFMAN_EE_MAX_SYMBOLS <= 0x7FFF,
               "CONFIG_HUFFMAN_EE_MAX_SYMBOLS must be at most 32767");

/* Marks the end of a list of leaves */
#define NO_LEAF         (0xFFFFU)

/* Seed used when the caller passes 0 (the random generator cannot start at 0) */
#define DEFAULT_SEED    (UINT32_C(0x9E3779B9))

/* The merge tree of the exact build has num_symbols leaves and num_symbols - 1
 * internal nodes */
#define EX_NODES        (2 * CONFIG_HUFFMAN_EE_MAX_SYMBOLS)

/* How the 0/1 labels are handed out when two subtrees are merged */
typedef enum {
    LABEL_PLAIN,    /* the first subtree always gets 0 (normal Huffman) */
    LABEL_ENERGY,   /* try both ways, keep the cheaper (the paper's method) */
    LABEL_RANDOM,   /* toss a coin (used for baselines) */
} label_mode_t;

/* One subtree that still waits to be merged */
typedef struct {
    uint32_t freq;          /* total frequency of all symbols in the subtree */
    uint32_t order;         /* creation counter, decides ties in frequency */
    uint16_t first_leaf;    /* first symbol (table index) in this subtree */
    uint16_t last_leaf;     /* last symbol, makes joining two lists quick */
    uint8_t height;         /* longest codeword inside this subtree, in bits */
} subtree_t;

/* Scratch memory needed while building a code */
typedef struct {
    subtree_t sub[CONFIG_HUFFMAN_EE_MAX_SYMBOLS];       /* the subtrees */
    uint16_t next_leaf[CONFIG_HUFFMAN_EE_MAX_SYMBOLS];  /* leaf lists */
    uint16_t heap[CONFIG_HUFFMAN_EE_MAX_SYMBOLS];       /* min-heap of subtrees */
    uint16_t heap_len;                                  /* entries in the heap */
    uint32_t next_order;                                /* next creation counter */
    uint32_t rng;                                       /* random generator state */
} build_ctx_t;

/* ---------------------------------------------------------------------- */
/* Min-heap: always gives back the subtree with the lowest frequency      */
/* ---------------------------------------------------------------------- */

/**
 * @brief   Compare two subtrees in the heap
 *
 * @param[in] ctx   Build context with the subtrees
 * @param[in] a     Index of the first subtree
 * @param[in] b     Index of the second subtree
 *
 * @return  true if @p a must be taken out of the heap before @p b
 */
static bool _heap_less(const build_ctx_t *ctx, uint16_t a, uint16_t b)
{
    const subtree_t *sa = &ctx->sub[a];
    const subtree_t *sb = &ctx->sub[b];

    if (sa->freq != sb->freq) {
        return sa->freq < sb->freq;
    }
    /* Equal frequency: the older subtree goes first. Without a fixed rule the
     * result would depend on the heap's internal layout. */
    return sa->order < sb->order;
}

/**
 * @brief   Put a subtree into the heap
 *
 * @param[in] ctx   Build context with the heap
 * @param[in] item  Index of the subtree to add
 */
static void _heap_push(build_ctx_t *ctx, uint16_t item)
{
    uint16_t pos = ctx->heap_len++;

    ctx->heap[pos] = item;
    /* move the new entry up until its parent is smaller */
    while (pos > 0) {
        uint16_t parent = (pos - 1) / 2;
        if (!_heap_less(ctx, ctx->heap[pos], ctx->heap[parent])) {
            break;
        }
        uint16_t tmp = ctx->heap[pos];
        ctx->heap[pos] = ctx->heap[parent];
        ctx->heap[parent] = tmp;
        pos = parent;
    }
}

/**
 * @brief   Take the lightest subtree out of the heap
 *
 * @param[in] ctx   Build context with the heap
 *
 * @return  index of the lightest subtree
 */
static uint16_t _heap_pop(build_ctx_t *ctx)
{
    uint16_t top = ctx->heap[0];

    /* move the last entry to the top, then let it sink down */
    ctx->heap_len--;
    ctx->heap[0] = ctx->heap[ctx->heap_len];

    uint16_t pos = 0;
    while (true) {
        uint16_t left = (2 * pos) + 1;
        uint16_t right = left + 1;
        uint16_t smallest = pos;

        if (left < ctx->heap_len && _heap_less(ctx, ctx->heap[left], ctx->heap[smallest])) {
            smallest = left;
        }
        if (right < ctx->heap_len && _heap_less(ctx, ctx->heap[right], ctx->heap[smallest])) {
            smallest = right;
        }
        if (smallest == pos) {
            break;
        }
        uint16_t tmp = ctx->heap[pos];
        ctx->heap[pos] = ctx->heap[smallest];
        ctx->heap[smallest] = tmp;
        pos = smallest;
    }
    return top;
}

/* ---------------------------------------------------------------------- */
/* Choosing the labels                                                    */
/* ---------------------------------------------------------------------- */

/**
 * @brief   Draw the next pseudo random number (xorshift32)
 *
 * A tiny generator, good enough for the coin tosses of the random baselines.
 *
 * @param[in,out] state     State of the generator
 *
 * @return  the next random number
 */
static uint32_t _rng_next(uint32_t *state)
{
    uint32_t x = *state;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

/**
 * @brief   Cost of one subtree with a given leading bit
 *
 * Cost of all codewords of one subtree if the bit @p bit is put in front of
 * them. Every codeword counts with its frequency:
 *
 *     frequency * (alpha * ones + beta * transitions)
 *
 * The caller must make sure that no codeword in the subtree is already
 * HUFFMAN_EE_MAX_CODE_LEN bits long.
 *
 * @param[in] table         Code table under construction
 * @param[in] ctx           Build context with the leaves of the subtree
 * @param[in] first_leaf    First symbol of the subtree
 * @param[in] bit           Bit to put in front of every codeword
 * @param[in] params        Weights of the energy model
 *
 * @return  the cost of the subtree
 */
static uint64_t _subtree_cost(const huffman_ee_table_t *table,
                              const build_ctx_t *ctx, uint16_t first_leaf,
                              uint32_t bit, const huffman_ee_params_t *params)
{
    uint64_t cost = 0;

    for (uint16_t leaf = first_leaf; leaf != NO_LEAF; leaf = ctx->next_leaf[leaf]) {
        const huffman_ee_code_t *code = &table->code[leaf];
        uint32_t bits = code->bits | (bit << code->length);
        uint8_t length = code->length + 1;
        uint64_t per_symbol = ((uint64_t)params->alpha * _count_ones(bits)) +
                              ((uint64_t)params->beta * _count_transitions(bits, length));

        cost += (uint64_t)table->freq[leaf] * per_symbol;
    }
    return cost;
}

/**
 * @brief   Decide which label the first subtree gets
 *
 * The second subtree always gets the opposite label.
 *
 * @param[in] table     Code table under construction
 * @param[in] ctx       Build context (its random state is used for LABEL_RANDOM)
 * @param[in] a         First subtree
 * @param[in] b         Second subtree
 * @param[in] mode      How the label is picked
 * @param[in] params    Weights of the energy model
 *
 * @return  0 if the first subtree gets 0, 1 if it gets 1
 */
static uint32_t _choose_label(const huffman_ee_table_t *table, build_ctx_t *ctx,
                              const subtree_t *a, const subtree_t *b,
                              label_mode_t mode, const huffman_ee_params_t *params)
{
    switch (mode) {
    case LABEL_ENERGY: {
        /* option 1: a gets 0 and b gets 1 */
        uint64_t cost_a0 = _subtree_cost(table, ctx, a->first_leaf, 0, params) +
                           _subtree_cost(table, ctx, b->first_leaf, 1, params);
        /* option 2: a gets 1 and b gets 0 */
        uint64_t cost_a1 = _subtree_cost(table, ctx, a->first_leaf, 1, params) +
                           _subtree_cost(table, ctx, b->first_leaf, 0, params);

        return (cost_a0 <= cost_a1) ? 0 : 1;
    }
    case LABEL_RANDOM:
        return (_rng_next(&ctx->rng) >> 16) & 1;
    case LABEL_PLAIN:
    default:
        return 0;
    }
}

/**
 * @brief   Put a bit in front of the codeword of every symbol of a subtree
 *
 * @param[in,out] table     Code table under construction
 * @param[in] ctx           Build context with the leaves of the subtree
 * @param[in] first_leaf    First symbol of the subtree
 * @param[in] bit           Bit to put in front of every codeword
 */
static void _prepend_bit(huffman_ee_table_t *table, const build_ctx_t *ctx,
                         uint16_t first_leaf, uint32_t bit)
{
    for (uint16_t leaf = first_leaf; leaf != NO_LEAF; leaf = ctx->next_leaf[leaf]) {
        huffman_ee_code_t *code = &table->code[leaf];

        /* the existing bits sit in the low positions, the new first bit goes
         * just above them */
        code->bits |= bit << code->length;
        code->length++;
    }
}

/* ---------------------------------------------------------------------- */
/* Building the code                                                      */
/* ---------------------------------------------------------------------- */

/**
 * @brief   Merge the two lightest subtrees into one
 *
 * @param[in,out] table     Code table under construction
 * @param[in,out] ctx       Build context with the heap
 * @param[in] mode          How the labels are picked
 * @param[in] params        Weights of the energy model
 *
 * @return  0 on success, -E2BIG if a codeword would become too long
 */
static int _merge_lightest(huffman_ee_table_t *table, build_ctx_t *ctx,
                           label_mode_t mode, const huffman_ee_params_t *params)
{
    uint16_t ia = _heap_pop(ctx);
    uint16_t ib = _heap_pop(ctx);
    subtree_t *a = &ctx->sub[ia];
    subtree_t *b = &ctx->sub[ib];
    uint8_t tallest = (a->height > b->height) ? a->height : b->height;

    /* one more bit will be added to the longest codeword */
    if (tallest >= HUFFMAN_EE_MAX_CODE_LEN) {
        return -E2BIG;
    }

    uint32_t label_a = _choose_label(table, ctx, a, b, mode, params);

    _prepend_bit(table, ctx, a->first_leaf, label_a);
    _prepend_bit(table, ctx, b->first_leaf, label_a ^ 1);

    /* The merged subtree reuses the slot of a. Its list of symbols is the
     * list of a followed by the list of b. */
    ctx->next_leaf[a->last_leaf] = b->first_leaf;
    a->last_leaf = b->last_leaf;
    a->freq += b->freq;
    a->height = tallest + 1;
    a->order = ctx->next_order++;
    _heap_push(ctx, ia);
    return 0;
}

/**
 * @brief   Check the arguments shared by all build functions
 *
 * @param[in] table         Code table to fill in
 * @param[in] symbols       Symbols of the alphabet
 * @param[in] freqs         How often each symbol occurs
 * @param[in] num_symbols   Size of the alphabet
 *
 * @return  0 if the arguments are usable, a negative errno otherwise
 */
static int _check_input(const huffman_ee_table_t *table, const uint16_t *symbols,
                        const uint32_t *freqs, size_t num_symbols)
{
    if ((table == NULL) || (symbols == NULL) || (freqs == NULL) || (num_symbols == 0)) {
        return -EINVAL;
    }
    if (num_symbols > CONFIG_HUFFMAN_EE_MAX_SYMBOLS) {
        return -E2BIG;
    }

    uint64_t total = 0;
    for (size_t i = 0; i < num_symbols; i++) {
        /* a symbol that never occurs does not need a codeword */
        if (freqs[i] == 0) {
            return -EINVAL;
        }
        /* two entries for the same symbol could never be decoded */
        for (size_t j = 0; j < i; j++) {
            if (symbols[j] == symbols[i]) {
                return -EINVAL;
            }
        }
        total += freqs[i];
    }
    /* the frequencies of the merged subtrees are added up in 32 bits */
    if (total > UINT32_MAX) {
        return -EINVAL;
    }
    return 0;
}

/**
 * @brief   Build a code with the given label rule
 *
 * @param[in,out] table     Code table to fill in
 * @param[in] symbols       Symbols of the alphabet
 * @param[in] freqs         How often each symbol occurs
 * @param[in] num_symbols   Size of the alphabet
 * @param[in] mode          How the labels are picked
 * @param[in] params        Weights of the energy model (NULL for LABEL_PLAIN)
 * @param[in] seed          Seed of the random generator (0 for the default)
 *
 * @return  0 on success, a negative errno otherwise
 */
static int _build(huffman_ee_table_t *table, const uint16_t *symbols,
                  const uint32_t *freqs, size_t num_symbols, label_mode_t mode,
                  const huffman_ee_params_t *params, uint32_t seed)
{
    int res = _check_input(table, symbols, freqs, num_symbols);

    if (res < 0) {
        return res;
    }
    if (mode == LABEL_ENERGY) {
        if ((params == NULL) || (params->alpha > HUFFMAN_EE_MAX_WEIGHT) ||
            (params->beta > HUFFMAN_EE_MAX_WEIGHT) ||
            (params->lambda > HUFFMAN_EE_MAX_WEIGHT)) {
            return -EINVAL;
        }
    }

    /* Phase 1: every symbol is a subtree of its own with an empty codeword */
    build_ctx_t ctx;

    ctx.heap_len = 0;
    ctx.next_order = num_symbols;
    ctx.rng = (seed != 0) ? seed : DEFAULT_SEED;
    table->num_symbols = num_symbols;
    for (size_t i = 0; i < num_symbols; i++) {
        table->symbol[i] = symbols[i];
        table->freq[i] = freqs[i];
        table->code[i].bits = 0;
        table->code[i].length = 0;

        ctx.sub[i].freq = freqs[i];
        ctx.sub[i].order = i;
        ctx.sub[i].first_leaf = i;
        ctx.sub[i].last_leaf = i;
        ctx.sub[i].height = 0;
        ctx.next_leaf[i] = NO_LEAF;
        _heap_push(&ctx, i);
    }

    /* A single symbol never gets merged. It still needs at least one bit. */
    if (num_symbols == 1) {
        table->code[0].bits = 0;
        table->code[0].length = 1;
        return 0;
    }

    /* Phase 2 and 3: merge until one subtree is left; the codewords of all
     * symbols are complete at that point */
    while (ctx.heap_len > 1) {
        res = _merge_lightest(table, &ctx, mode, params);
        if (res < 0) {
            return res;
        }
    }
    return 0;
}

int huffman_ee_build(huffman_ee_table_t *table, const uint16_t *symbols,
                     const uint32_t *freqs, size_t num_symbols,
                     const huffman_ee_params_t *params)
{
    return _build(table, symbols, freqs, num_symbols, LABEL_ENERGY, params, 0);
}

int huffman_ee_build_plain(huffman_ee_table_t *table, const uint16_t *symbols,
                           const uint32_t *freqs, size_t num_symbols)
{
    return _build(table, symbols, freqs, num_symbols, LABEL_PLAIN, NULL, 0);
}

int huffman_ee_build_random(huffman_ee_table_t *table, const uint16_t *symbols,
                            const uint32_t *freqs, size_t num_symbols,
                            uint32_t seed)
{
    return _build(table, symbols, freqs, num_symbols, LABEL_RANDOM, NULL, seed);
}

/* ====================================================================== */
/* Exact optimal labelling (dynamic program over the fixed Huffman tree)  */
/* ====================================================================== */

/**
 * @brief   Compare two subtrees of the exact build
 *
 * @param[in] nfreq     Frequencies of the nodes of the fixed tree
 * @param[in] a         Index of the first node
 * @param[in] b         Index of the second node
 *
 * @return  true if @p a comes before @p b (lowest frequency, then oldest)
 */
static bool _ex_less(const uint32_t *nfreq, uint16_t a, uint16_t b)
{
    if (nfreq[a] != nfreq[b]) {
        return nfreq[a] < nfreq[b];
    }
    return a < b;
}

/**
 * @brief   Put a node into the heap of the exact build
 *
 * @param[in] heap      Heap
 * @param[in,out] len   Number of entries in the heap
 * @param[in] nfreq     Frequencies of the nodes
 * @param[in] item      Node to add
 */
static void _ex_push(uint16_t *heap, uint16_t *len, const uint32_t *nfreq, uint16_t item)
{
    uint16_t pos = (*len)++;

    heap[pos] = item;
    while (pos > 0) {
        uint16_t parent = (pos - 1) / 2;
        if (!_ex_less(nfreq, heap[pos], heap[parent])) {
            break;
        }
        uint16_t tmp = heap[pos];
        heap[pos] = heap[parent];
        heap[parent] = tmp;
        pos = parent;
    }
}

/**
 * @brief   Take the lightest node out of the heap of the exact build
 *
 * @param[in] heap      Heap
 * @param[in,out] len   Number of entries in the heap
 * @param[in] nfreq     Frequencies of the nodes
 *
 * @return  index of the lightest node
 */
static uint16_t _ex_pop(uint16_t *heap, uint16_t *len, const uint32_t *nfreq)
{
    uint16_t top = heap[0];

    (*len)--;
    heap[0] = heap[*len];

    uint16_t pos = 0;
    while (true) {
        uint16_t left = (2 * pos) + 1;
        uint16_t right = left + 1;
        uint16_t smallest = pos;

        if ((left < *len) && _ex_less(nfreq, heap[left], heap[smallest])) {
            smallest = left;
        }
        if ((right < *len) && _ex_less(nfreq, heap[right], heap[smallest])) {
            smallest = right;
        }
        if (smallest == pos) {
            break;
        }
        uint16_t tmp = heap[pos];
        heap[pos] = heap[smallest];
        heap[smallest] = tmp;
        pos = smallest;
    }
    return top;
}

/**
 * @brief   One pass of the dynamic program over the fixed tree
 *
 * The incoming label of the root is fixed implicitly; the caller handles the
 * root. Fills g0/g1 (best cost of each subtree as a function of the label of
 * the edge leading into it) and, when the choices are not NULL, the label
 * chosen for the left child of every internal node.
 *
 * @param[in] nodes         Number of nodes of the fixed tree
 * @param[in] nlo           Left child of each node (NO_LEAF for a leaf)
 * @param[in] nhi           Right child of each node (NO_LEAF for a leaf)
 * @param[in] nfreq         Frequency of each node
 * @param[in] alpha_eff     Weight of a 1 bit (possibly scaled)
 * @param[in] beta_eff      Weight of a transition inside a codeword (possibly scaled)
 * @param[in] extra0        Per-frequency penalty of a leaf with incoming label 0
 * @param[out] g0           Best cost per subtree for an incoming 0
 * @param[out] g1           Best cost per subtree for an incoming 1
 * @param[out] c0           Label of the left child for an incoming 0 (may be NULL)
 * @param[out] c1           Label of the left child for an incoming 1 (may be NULL)
 */
static void _ex_dp(uint16_t nodes, const uint16_t *nlo, const uint16_t *nhi,
                   const uint32_t *nfreq, int64_t alpha_eff, int64_t beta_eff,
                   int64_t extra0, int64_t *g0, int64_t *g1,
                   uint8_t *c0, uint8_t *c1)
{
    for (uint16_t v = 0; v < nodes; v++) {
        if (nlo[v] == NO_LEAF) {                     /* leaf */
            g0[v] = extra0 * (int64_t)nfreq[v];
            g1[v] = 0;
            if (c0) {
                c0[v] = 0; c1[v] = 0;
            }
            continue;
        }
        uint16_t a = nlo[v];
        uint16_t b = nhi[v];
        int64_t fa = nfreq[a];
        int64_t fb = nfreq[b];

        for (int in = 0; in < 2; in++) {
            int64_t best = INT64_MAX;
            uint8_t best_l = 0;

            for (int l = 0; l < 2; l++) {
                int64_t cost = 0;
                if (l) {
                    cost += alpha_eff * fa;
                }
                if (in != l) {
                    cost += beta_eff  * fa;
                }
                cost += (l ? g1[a] : g0[a]);
                if (!l) {
                    cost += alpha_eff * fb;
                }
                if (in != (1 - l)) {
                    cost += beta_eff  * fb;
                }
                cost += ((1 - l) ? g1[b] : g0[b]);

                if (cost < best) {
                    best = cost; best_l = (uint8_t)l;
                }
            }
            if (in == 0) {
                g0[v] = best; if (c0) {
                    c0[v] = best_l;
                }
            }
            else {
                g1[v] = best; if (c1) {
                    c1[v] = best_l;
                }
            }
        }
    }
}

/**
 * @brief   Cost of the whole tree for one root orientation
 *
 * Only the alpha part applies at the root: the root has no incoming edge, so
 * no transition happens there.
 *
 * @param[in] nlo           Left child of each node
 * @param[in] nhi           Right child of each node
 * @param[in] nfreq         Frequency of each node
 * @param[in] alpha_eff     Weight of a 1 bit (possibly scaled)
 * @param[in] g0            Best cost per subtree for an incoming 0
 * @param[in] g1            Best cost per subtree for an incoming 1
 * @param[in] root          Index of the root
 * @param[in] l             Label of the left child of the root
 * @param[out] label_out    Label of the left child that was used (may be NULL)
 *
 * @return  the cost of the whole tree
 */
static int64_t _ex_root_cost(const uint16_t *nlo, const uint16_t *nhi,
                             const uint32_t *nfreq, int64_t alpha_eff,
                             const int64_t *g0, const int64_t *g1, uint16_t root,
                             int l, uint8_t *label_out)
{
    uint16_t a = nlo[root];
    uint16_t b = nhi[root];
    int64_t cost = 0;

    if (l) {
        cost += alpha_eff * nfreq[a];
    }
    cost += (l ? g1[a] : g0[a]);
    if (!l) {
        cost += alpha_eff * nfreq[b];
    }
    cost += ((1 - l) ? g1[b] : g0[b]);

    if (label_out) {
        *label_out = (uint8_t)l;
    }
    return cost;
}

/**
 * @brief   Hand out the codewords of the fixed tree
 *
 * Walks the tree from the root and uses the choices stored in c0/c1.
 *
 * @param[in,out] table     Code table to fill in
 * @param[in] nodes         Number of nodes of the fixed tree (unused)
 * @param[in] root          Index of the root
 * @param[in] nlo           Left child of each node
 * @param[in] nhi           Right child of each node
 * @param[in] c0            Label of the left child for an incoming 0
 * @param[in] c1            Label of the left child for an incoming 1
 * @param[in] root_label    Label of the left child of the root
 *
 * @return  0 on success, -E2BIG if a codeword would become too long
 */
static int _ex_assign(huffman_ee_table_t *table, uint16_t nodes, uint16_t root,
                      const uint16_t *nlo, const uint16_t *nhi,
                      const uint8_t *c0, const uint8_t *c1, uint8_t root_label)
{
    struct { uint16_t node; uint32_t bits; uint8_t len; } stack[2 * HUFFMAN_EE_MAX_CODE_LEN];
    uint16_t top = 0;

    /* the two children of the root */
    stack[top].node = nlo[root];
    stack[top].bits = root_label;
    stack[top].len = 1;
    top++;
    stack[top].node = nhi[root];
    stack[top].bits = (uint32_t)(root_label ^ 1);
    stack[top].len = 1;
    top++;

    while (top > 0) {
        top--;
        uint16_t v = stack[top].node;
        uint32_t bits = stack[top].bits;
        uint8_t len = stack[top].len;

        if (len > HUFFMAN_EE_MAX_CODE_LEN) {
            return -E2BIG;
        }
        if (nlo[v] == NO_LEAF) {                 /* leaf: v is a symbol index */
            table->code[v].bits = bits;
            table->code[v].length = len;
            continue;
        }
        uint8_t in = (uint8_t)(bits & 1);           /* label of the edge into v */
        uint8_t l = (in ? c1[v] : c0[v]);           /* label of v's left child */

        stack[top].node = nlo[v];
        stack[top].bits = (bits << 1) | l;
        stack[top].len = len + 1;
        top++;
        stack[top].node = nhi[v];
        stack[top].bits = (bits << 1) | (l ^ 1);
        stack[top].len = len + 1;
        top++;
    }
    (void)nodes;
    return 0;
}

/**
 * @brief   Build the exact optimal labelling of the fixed Huffman tree
 *
 * @param[in,out] table     Code table to fill in
 * @param[in] symbols       Symbols of the alphabet
 * @param[in] freqs         How often each symbol occurs
 * @param[in] num_symbols   Size of the alphabet
 * @param[in] params        Weights of the energy model
 * @param[in] use_lambda    Also put the boundary term into the decisions
 *
 * @return  0 on success, a negative errno otherwise
 */
static int _exact(huffman_ee_table_t *table, const uint16_t *symbols,
                  const uint32_t *freqs, size_t num_symbols,
                  const huffman_ee_params_t *params, bool use_lambda)
{
    if ((params == NULL) ||
        (params->alpha  > HUFFMAN_EE_MAX_WEIGHT) ||
        (params->beta   > HUFFMAN_EE_MAX_WEIGHT) ||
        (params->lambda > HUFFMAN_EE_MAX_WEIGHT)) {
        return -EINVAL;
    }

    uint16_t k = (uint16_t)num_symbols;

    uint32_t nfreq[EX_NODES];
    uint16_t nlo[EX_NODES];
    uint16_t nhi[EX_NODES];
    uint8_t nhgt[EX_NODES];
    uint16_t heap[EX_NODES];
    int64_t g0[EX_NODES];
    int64_t g1[EX_NODES];
    uint8_t c0[EX_NODES];
    uint8_t c1[EX_NODES];

    uint64_t total = 0;
    for (uint16_t i = 0; i < k; i++) {
        table->symbol[i] = symbols[i];
        table->freq[i] = freqs[i];
        table->code[i].bits = 0;
        table->code[i].length = 0;
        nfreq[i] = freqs[i];
        nlo[i] = NO_LEAF;
        nhi[i] = NO_LEAF;
        nhgt[i] = 0;
        total += freqs[i];
    }
    table->num_symbols = k;

    if (k == 1) {
        table->code[0].bits = 0;
        table->code[0].length = 1;
        return 0;
    }
    if (use_lambda) {
        /* The lambda-aware build scales the objective by the total frequency
         * and by up to the longest codeword (32 bits), so the total must stay
         * small enough for every cost to fit in 64 bits. The bound below keeps
         * 32 * alpha * total^2 + lambda * total^2 below 9e18. */
        uint64_t factor = (32 * (uint64_t)params->alpha) + (uint64_t)params->lambda;

        if (factor == 0) {
            factor = 1;
        }
        if ((total > UINT64_C(3000000000)) ||
            ((total * total) > (UINT64_C(9000000000000000000) / factor))) {
            return -E2BIG;
        }
    }

    /* Phase A: build the tree shape (same merges as plain Huffman) */
    uint16_t hlen = 0;
    for (uint16_t i = 0; i < k; i++) {
        _ex_push(heap, &hlen, nfreq, i);
    }
    uint16_t next = k;
    while (hlen > 1) {
        uint16_t a = _ex_pop(heap, &hlen, nfreq);
        uint16_t b = _ex_pop(heap, &hlen, nfreq);
        uint16_t v = next++;
        uint8_t h = (nhgt[a] > nhgt[b]) ? nhgt[a] : nhgt[b];

        if (h >= HUFFMAN_EE_MAX_CODE_LEN) {
            return -E2BIG;
        }
        nlo[v] = a;
        nhi[v] = b;
        nfreq[v] = nfreq[a] + nfreq[b];
        nhgt[v] = (uint8_t)(h + 1);
        _ex_push(heap, &hlen, nfreq, v);
    }
    uint16_t root = (uint16_t)(next - 1);

    /* Phase B: choose the labels */
    int64_t alpha_eff = params->alpha;
    int64_t beta_eff = params->beta;
    uint8_t root_label = 0;
    int64_t best_cost = INT64_MAX;

    if (!use_lambda) {
        _ex_dp(next, nlo, nhi, nfreq, alpha_eff, beta_eff, 0, g0, g1, c0, c1);
        for (int l = 0; l < 2; l++) {
            uint8_t rl = 0;
            int64_t c = _ex_root_cost(nlo, nhi, nfreq, alpha_eff, g0, g1, root,
                                      l, &rl);
            if (c < best_cost) {
                best_cost = c;
                root_label = rl;
            }
        }
    }
    else {
        int64_t T = (int64_t)total;
        alpha_eff = (int64_t)params->alpha * T;
        beta_eff = (int64_t)params->beta * T;

        int best_rc = 0;

        for (int rc = 0; rc < 2; rc++) {
            /* rc is the label of the left child of the root: it both fixes b'
             * and is the label we are evaluating, so there is nothing to try
             * inside the loop. */
            int64_t bprime = (rc == 0) ? (int64_t)nfreq[nhi[root]]
                                       : (int64_t)nfreq[nlo[root]];
            int64_t extra0 = (int64_t)params->lambda * (2 * bprime - T);
            int64_t constant = (int64_t)params->lambda * T * (T - bprime);

            _ex_dp(next, nlo, nhi, nfreq, alpha_eff, beta_eff, extra0, g0, g1,
                   NULL, NULL);

            int64_t c = _ex_root_cost(nlo, nhi, nfreq, alpha_eff, g0, g1,
                                      root, rc, NULL) + constant;

            if (c < best_cost) {
                best_cost = c;
                best_rc = rc;
                root_label = (uint8_t)rc;
            }
        }
        /* Redo the DP for the winning root orientation so that the stored
         * choices c0/c1 are the ones of the labelling we just picked. */
        {
            int64_t bprime = (best_rc == 0) ? (int64_t)nfreq[nhi[root]]
                                            : (int64_t)nfreq[nlo[root]];
            int64_t extra0 = (int64_t)params->lambda * (2 * bprime - T);

            _ex_dp(next, nlo, nhi, nfreq, alpha_eff, beta_eff, extra0, g0, g1,
                   c0, c1);
        }
    }

    return _ex_assign(table, next, root, nlo, nhi, c0, c1, root_label);
}

int huffman_ee_build_exact(huffman_ee_table_t *table, const uint16_t *symbols,
                           const uint32_t *freqs, size_t num_symbols,
                           const huffman_ee_params_t *params)
{
    int res = _check_input(table, symbols, freqs, num_symbols);

    if (res < 0) {
        return res;
    }
    return _exact(table, symbols, freqs, num_symbols, params, false);
}

int huffman_ee_build_exact_lambda(huffman_ee_table_t *table,
                                  const uint16_t *symbols, const uint32_t *freqs,
                                  size_t num_symbols,
                                  const huffman_ee_params_t *params)
{
    int res = _check_input(table, symbols, freqs, num_symbols);

    if (res < 0) {
        return res;
    }
    return _exact(table, symbols, freqs, num_symbols, params, true);
}

/** @} */
