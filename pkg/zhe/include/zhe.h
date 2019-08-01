#ifndef ZHE_H
#define ZHE_H

#include <stddef.h>
#include <stdbool.h>

#include "zhe-config.h"
#include "zhe-rid.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ZHE_MAX_PUBLICATIONS < 256
typedef uint8_t zhe_pubidx_inner_t;
#elif ZHE_MAX_PUBLICATIONS < 32768
typedef uint16_t zhe_pubidx_inner_t;
#else
#error "ZHE_MAX_PUBLICATIONS >= 32768 not implemented (cf bitset_findfirst)"
#endif

#if ZHE_MAX_SUBSCRIPTIONS < 256
typedef uint8_t zhe_subidx_inner_t;
#elif ZHE_MAX_SUBSCRIPTIONS < 32768
typedef uint16_t zhe_subidx_inner_t;
#else
#error "ZHE_MAX_SUBSCRIPTIONS >= 32768 not implemented (cf bitset_findfirst)"
#endif

typedef struct { zhe_pubidx_inner_t idx; } zhe_pubidx_t;
typedef struct { zhe_subidx_inner_t idx; } zhe_subidx_t;

typedef void (*zhe_subhandler_t)(zhe_rid_t rid, const void *payload, zhe_paysize_t size, void *arg);

struct zhe_address;
struct zhe_platform;

struct zhe_config {
    size_t idlen;
    const void *id;

    struct zhe_address *scoutaddr;

    size_t n_mcgroups_join;
    struct zhe_address *mcgroups_join;

    size_t n_mconduit_dstaddrs;
    struct zhe_address *mconduit_dstaddrs;
};

/* numerical values also appear on the wire (with the exception of PENDING, which is disallowed on the wire but rather generated locally) */
enum zhe_declstatus {
    ZHE_DECL_OK          = 0,    /* all declarations sent & accepted */
    ZHE_DECL_NOSPACE     = 1,    /* some peer rejected the set for a lack of memory (likely not temporary) */
    ZHE_DECL_CONFLICT    = 2,    /* some peer rejected the set because of conflicting definitions */
    ZHE_DECL_TXNSIZE     = 3,    /* some peer rejected the set because it couldn't a transaction this large */
    ZHE_DECL_AGAIN       = 4,    /* some peer rejected the set because of some (likely) temporary issue */
    ZHE_DECL_INVALID     = 5,    /* some peer rejected the set because it contains an declaration that is invalid by its rules */
    ZHE_DECL_UNSUPPORTED = 6,    /* some peer rejected the set because it contains an unsupported declaration */
    ZHE_DECL_OTHER       = 7,    /* some peer rejected the set for another reason */
    ZHE_DECL_PENDING     = 255,  /* some declarations pending (locally declared, not yet accepted by all, but also no rejects) */
};
#define ZHE_DRESULT_IS_VALID_DECLSTATUS(x) ((unsigned)(x) <= ZHE_DECL_OTHER)

int zhe_init(const struct zhe_config *config, struct zhe_platform *pf, zhe_time_t tnow);
void zhe_start(zhe_time_t tnow);
void zhe_housekeeping(zhe_time_t tnow);
int zhe_input(const void *buf, size_t sz, const struct zhe_address *src, zhe_time_t tnow);
void zhe_flush(zhe_time_t tnow);

bool zhe_declare_resource(zhe_rid_t rid, const char *uri);
zhe_pubidx_t zhe_publish(zhe_rid_t rid, unsigned cid, int reliable);
zhe_subidx_t zhe_subscribe(zhe_rid_t rid, zhe_paysize_t xmitneed, unsigned cid, zhe_subhandler_t handler, void *arg);
/* FIXME: should add zhe_declcommit(void) or something like that, rather than always auto-committing like it does now */
enum zhe_declstatus zhe_get_declstatus(zhe_rid_t *rid);

int zhe_write(zhe_pubidx_t pubidx, const void *data, zhe_paysize_t sz, zhe_time_t tnow);
int zhe_write_uri(const char *uri, const void *data, zhe_paysize_t sz, zhe_time_t tnow);

#ifdef __cplusplus
}
#endif

#endif
