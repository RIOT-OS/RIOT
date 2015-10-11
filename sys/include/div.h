#ifndef DIV_H
#define DIV_H

#include <stdint.h>

static inline uint64_t div_u64_by_1000000(uint64_t val)
{
    return (uint64_t)(val * 0x431bde83) >> (0x12 + 32);
}

static inline uint32_t div_u32_by_512div15625(uint32_t val)
{
    return ((uint64_t)(val) * 0x431bde83ul) >> (12 + 32 - 9);
}

#endif /* DIV_H */
