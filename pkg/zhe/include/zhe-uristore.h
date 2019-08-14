#ifndef ZHE_URISTORE_H
#define ZHE_URISTORE_H

#if ZHE_MAX_URISPACE > 0
#include <stdbool.h>
#include "zhe-icgcb.h"

#if ZHE_MAX_RESOURCES < UINT8_MAX
typedef uint8_t zhe_residx_t;
#define RESIDX_INVALID UINT8_MAX
#elif ZHE_MAX_RESOURCES < UINT16_MAX
typedef uint16_t zhe_residx_t;
#define RESIDX_INVALID UINT16_MAX
#elif ZHE_MAX_RESOURCES < UINT32_MAX
typedef uint32_t zhe_residx_t;
#define RESIDX_INVALID UINT32_MAX
#endif

enum uristore_result {
    USR_OK,       /* new (or new for this peer) and stored */
    USR_DUPLICATE,/* already known for this peer */
    USR_AGAIN,    /* not stored, but GC will eventually free up space (barring any intervening allocations) */
    USR_NOSPACE,  /* not stored, insufficient total free space */
    USR_MISMATCH, /* not stored, different URI known for this RID */
    USR_INVALID   /* URI is not invalid */
};

typedef struct uristore_iter {
    zhe_residx_t cursor;
} uristore_iter_t;

void zhe_uristore_init(void);
void zhe_uristore_gc(void);
zhe_residx_t zhe_uristore_nres(void);
#define URISTORE_PEERIDX_SELF MAX_PEERS
/* if tentative & result is OK, *loser is set to INVALID if all is well or to the peeridx of the peer that lost on doing a tentative definition, which may be peeridx itself */
enum uristore_result zhe_uristore_store(zhe_residx_t *res_idx, peeridx_t peeridx, zhe_rid_t rid, const uint8_t *uri, size_t urilen_in, bool tentative, peeridx_t *loser);
void zhe_uristore_drop(peeridx_t peeridx, zhe_rid_t rid);
void zhe_uristore_reset_peer(peeridx_t peeridx);
bool zhe_uristore_geturi_for_idx(zhe_residx_t idx, zhe_rid_t *rid, zhe_paysize_t *sz, const uint8_t **uri, bool *islocal);
bool zhe_uristore_geturi_for_rid(zhe_rid_t rid, zhe_paysize_t *sz, const uint8_t **uri);
bool zhe_uristore_getidx_for_rid(zhe_rid_t rid, zhe_residx_t *idx);

void zhe_uristore_abort_tentative(peeridx_t peeridx);
void zhe_uristore_commit_tentative(peeridx_t peeridx);

void zhe_uristore_iter_init(uristore_iter_t *it);
bool zhe_uristore_iter_next(uristore_iter_t *it, zhe_rid_t *rid, zhe_paysize_t *sz, const uint8_t **uri);
#endif /* ZHE_MAX_URISPACE > 0 */

#endif
