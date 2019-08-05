#ifndef ZHE_ICGCB_H
#define ZHE_ICGCB_H

#include <stdint.h>

/* Need configuration to scale uripos_t */
#include "zhe-config-deriv.h"

#if ZHE_NEED_ICGCB

#if ZHE_MAX_URISPACE >= UINT16_MAX || ZHE_MAX_RESOURCES >= UINT16_MAX
typedef uint32_t uripos_t;
#define URIPOS_MAX UINT32_MAX
#elif ZHE_MAX_URISPACE >= UINT8_MAX || ZHE_MAX_RESOURCES >= UINT8_MAX
typedef uint16_t uripos_t;
#define URIPOS_MAX UINT16_MAX
#else
typedef uint8_t uripos_t;
#define URIPOS_MAX UINT8_MAX
#endif

struct icgcb_hdr {
    uripos_t size;      /* includes header, multiple of UNIT unless allocated; sentinel is exception and has size = 0 */
    uripos_t ref;       /* URIPOS_INVALID if free, else whatever the user puts in */
};

struct icgcb {
    uripos_t size;
    uripos_t pad;
    uripos_t freespace; /* total free space, that is, buf size - allocated (incl. headers) - 1 header */
    uripos_t firstfree; /* index in e[], first free block, GC starts here */
    uripos_t openspace; /* index in e[], nothing to the end of the buffer, allocations happen here */
    uripos_t sentinel;  /* index in e[] of sentinel at the end of memory */
    struct icgcb_hdr e[];
};

enum icgcb_alloc_result {
    IAR_OK,
    IAR_AGAIN,
    IAR_NOSPACE
};

void zhe_icgcb_init(struct icgcb * const b, uripos_t size);
void zhe_icgcb_free(struct icgcb * const b, void * const ptr);
enum icgcb_alloc_result zhe_icgcb_alloc(void ** const ptr, struct icgcb * const b, uripos_t size, uripos_t ref);
void zhe_icgcb_gc(struct icgcb * const b, void (*move_cb)(uripos_t ref, void *newptr, void *arg), void *arg);
uripos_t zhe_icgcb_getsize(struct icgcb const * const b, const void *ptr);

#endif /* ZHE_NEED_ICGCB */
#endif
