#ifndef ZHE_UNPACK_H
#define ZHE_UNPACK_H

#include "zhe-config-deriv.h"

typedef enum zhe_unpack_result {
    ZUR_OK,       /* unpack successful */
    ZUR_SHORT,    /* unpack failed because of incomplete input */
    ZUR_OVERFLOW, /* unpack failed because of an overflow / out-of-range input */
    ZUR_ABORT     /* totally messed up */
} zhe_unpack_result_t;

zhe_unpack_result_t zhe_unpack_skip(uint8_t const * const end, uint8_t const * * const data, zhe_msgsize_t n) ZHE_NONNULL_ALL;
zhe_unpack_result_t zhe_unpack_byte(uint8_t const * const end, uint8_t const * * const data, uint8_t * restrict u) ZHE_NONNULL_ALL;
zhe_unpack_result_t zhe_unpack_vle8(uint8_t const * const end, uint8_t const * * const data, uint8_t * restrict u) ZHE_NONNULL_ALL;
zhe_unpack_result_t zhe_unpack_vle16(uint8_t const * const end, uint8_t const * * const data, uint16_t * restrict u) ZHE_NONNULL_ALL;
zhe_unpack_result_t zhe_unpack_vle32(uint8_t const * const end, uint8_t const * * const data, uint32_t * restrict u) ZHE_NONNULL_ALL;
#if ZHE_RID_SIZE > 32 || SEQNUM_LEN > 28
zhe_unpack_result_t zhe_unpack_vle64(uint8_t const * const end, uint8_t const * * const data, uint64_t * restrict u) ZHE_NONNULL_ALL;
#endif
zhe_unpack_result_t zhe_unpack_seq(uint8_t const * const end, uint8_t const * * const data, seq_t * restrict u) ZHE_NONNULL_ALL;
const uint8_t *zhe_skip_validated_vle(const uint8_t *data);
zhe_unpack_result_t zhe_unpack_rid(uint8_t const * const end, uint8_t const * * const data, zhe_rid_t * restrict u) ZHE_NONNULL_ALL;
zhe_unpack_result_t zhe_unpack_vec(uint8_t const * const end, uint8_t const * * const data, size_t lim, zhe_paysize_t * restrict u, uint8_t * restrict v) ZHE_NONNULL_ALL;
zhe_unpack_result_t zhe_unpack_vecref(uint8_t const * const end, uint8_t const * * const data, zhe_paysize_t *u, uint8_t const **v) ZHE_NONNULL_ALL;

struct unpack_props_iter {
    uint16_t n;
    const uint8_t *data;
    const uint8_t *end;
};

zhe_unpack_result_t zhe_unpack_props(uint8_t const * const end, uint8_t const * * const data, struct unpack_props_iter *it) ZHE_NONNULL_ALL;
int zhe_unpack_props_iter(struct unpack_props_iter *it, uint8_t *propid, zhe_paysize_t *sz, const uint8_t **data) ZHE_NONNULL_ALL;

struct unpack_locs_iter {
    uint16_t n;
    const uint8_t *data;
    const uint8_t *end;
};

zhe_unpack_result_t zhe_unpack_locs(uint8_t const * const end, uint8_t const * * const data, struct unpack_locs_iter *it) ZHE_NONNULL_ALL;
int zhe_unpack_locs_iter(struct unpack_locs_iter *it, zhe_paysize_t *sz, const uint8_t **loc) ZHE_NONNULL_ALL;

#endif
