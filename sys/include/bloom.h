#ifndef _BLOOM_FILTER_H
#define _BLOOM_FILTER_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef unsigned int (*hashfp_t)(const char *);

struct bloom_t {
    size_t m;
    size_t k;
    unsigned char *a;
    hashfp_t *hash;
};

struct bloom_t *bloom_new(size_t size, size_t num_hashes, ...);
void bloom_del(struct bloom_t *bloom);
void bloom_add(struct bloom_t *bloom, const char *s);
bool bloom_check(struct bloom_t *bloom, const char *s);

#endif
