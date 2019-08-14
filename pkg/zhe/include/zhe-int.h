#ifndef ZHE_INT_H
#define ZHE_INT_H

#include "zhe.h"
#include "zhe-config-deriv.h"
#include "zhe-mscout.h"

#define SUFFIX_WITH_SIZE1(name, size) name##size
#define SUFFIX_WITH_SIZE(name, size) SUFFIX_WITH_SIZE1(name, size)

#define INFIX_WITH_SIZE1(name, size, suf) name##size##suf
#define INFIX_WITH_SIZE(name, size, suf) INFIX_WITH_SIZE1(name, size, suf)

struct out_conduit;
struct out_mconduit;
struct in_conduit;

struct peerid {
    uint8_t id[PEERID_SIZE];
    zhe_paysize_t len;
};

bool zhe_established_peer(peeridx_t peeridx);
int zhe_compare_peer_ids_for_peeridx(peeridx_t a, peeridx_t b);
int zhe_xmitw_hasspace(const struct out_conduit *c, zhe_paysize_t sz);
void zhe_pack_reserve(zhe_address_t *dst, struct out_conduit *oc, zhe_paysize_t cnt, zhe_time_t tnow);
void zhe_pack1(uint8_t x);
void zhe_pack2(uint8_t x, uint8_t y);
void zhe_pack_vec(zhe_paysize_t n, const void *buf);
uint16_t zhe_pack_locs_calcsize(void);
void zhe_pack_locs(void);
void zhe_oc_hit_full_window(struct out_conduit *c, zhe_time_t tnow);
int zhe_oc_am_draining_window(const struct out_conduit *c);
bool zhe_out_conduit_is_connected(cid_t cid);
void zhe_pack_msend(zhe_time_t tnow);
zhe_msgsize_t zhe_oc_pack_payload_msgprep(seq_t *s, struct out_conduit *c, int relflag, zhe_paysize_t sz, zhe_time_t tnow);
void zhe_oc_pack_copyrel(struct out_conduit *c, zhe_msgsize_t from);
void zhe_oc_pack_payload(struct out_conduit *c, int relflag, zhe_paysize_t sz, const void *vdata);
void zhe_oc_pack_payload_done(struct out_conduit *c, int relflag, zhe_time_t tnow);
int zhe_seq_lt(seq_t a, seq_t b);
int zhe_seq_le(seq_t a, seq_t b);
struct out_conduit *zhe_out_conduit_from_cid(cid_t cid);

#endif
