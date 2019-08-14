#ifndef ZHE_BITSET_H
#define ZHE_BITSET_H

#include <stdint.h>
#include <stdbool.h>

#define DECL_BITSET(name_, size_) uint8_t name_[((size_)+7)/8]

void zhe_bitset_set(uint8_t *s, unsigned idx);
void zhe_bitset_clear(uint8_t *s, unsigned idx);
void zhe_bitset_andnot(uint8_t *d, const uint8_t *s, unsigned size);
int zhe_bitset_test(const uint8_t *s, unsigned idx);
unsigned zhe_bitset_count(const uint8_t *s, unsigned size);
int zhe_bitset_findfirst(const uint8_t *s, unsigned size);

typedef struct bitset_iter {
    const uint8_t *s;
    unsigned size;
    unsigned cursor;
} bitset_iter_t;
bool zhe_bitset_iter_first(bitset_iter_t *it, const uint8_t *s, unsigned size, unsigned *idx);
bool zhe_bitset_iter_next(bitset_iter_t *it, unsigned *idx);

#endif /* BITSET_H */
