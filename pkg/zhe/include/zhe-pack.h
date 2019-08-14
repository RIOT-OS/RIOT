#ifndef ZHE_PACK_H
#define ZHE_PACK_H

#include <stdbool.h>
#include "zhe-config-deriv.h"

struct out_conduit;
struct peerid;

void zhe_pack_vle8(uint8_t x);
zhe_paysize_t zhe_pack_vle8req(uint8_t x);
void zhe_pack_vle16(uint16_t x);
zhe_paysize_t zhe_pack_vle16req(uint16_t x);
void zhe_pack_vle32(uint32_t x);
zhe_paysize_t zhe_pack_vle32req(uint32_t x);
void zhe_pack_vle64(uint64_t x);
zhe_paysize_t zhe_pack_vle64req(uint64_t x);
void zhe_pack_seq(seq_t x);
zhe_paysize_t zhe_pack_seqreq(seq_t x);
void zhe_pack_rid(zhe_rid_t x);
zhe_paysize_t zhe_pack_ridreq(zhe_rid_t x);
void zhe_pack_mscout(zhe_address_t *dst, zhe_time_t tnow);
void zhe_pack_mhello(zhe_address_t *dst, zhe_time_t tnow);
void zhe_pack_mopen(zhe_address_t *dst, uint8_t seqnumlen, const struct peerid *ownid, zhe_timediff_t lease_dur, zhe_time_t tnow);
void zhe_pack_maccept(zhe_address_t *dst, const struct peerid *ownid, const struct peerid *peerid, zhe_timediff_t lease_dur, zhe_time_t tnow);
void zhe_pack_mclose(zhe_address_t *dst, uint8_t reason, const struct peerid *ownid, zhe_time_t tnow);
void zhe_pack_reserve_mconduit(zhe_address_t *dst, cid_t cid, bool track_cid, zhe_paysize_t cnt, zhe_time_t tnow);
void zhe_pack_reserve_mconduit_rawcid(zhe_address_t *dst, cid_t cid, zhe_paysize_t cnt, zhe_time_t tnow);
void zhe_pack_msynch(zhe_address_t *dst, uint8_t sflag, cid_t cid, seq_t seqbase, seq_t cnt, zhe_time_t tnow);
void zhe_pack_macknack(zhe_address_t *dst, cid_t cid, seq_t seq, uint32_t mask, zhe_time_t tnow);
void zhe_pack_mping(zhe_address_t *dst, uint16_t hash, zhe_time_t tnow);
void zhe_pack_mpong(zhe_address_t *dst, uint16_t hash, zhe_time_t tnow);
void zhe_pack_mkeepalive(zhe_address_t *dst, const struct peerid *ownid, zhe_time_t tnow);
int zhe_oc_pack_msdata(struct out_conduit *c, int relflag, zhe_rid_t rid, zhe_paysize_t payloadlen, zhe_time_t tnow);
void zhe_oc_pack_msdata_payload(struct out_conduit *c, int relflag, zhe_paysize_t sz, const void *vdata);
void zhe_oc_pack_msdata_done(struct out_conduit *c, int relflag, zhe_time_t tnow);
int zhe_oc_pack_mwdata(struct out_conduit *c, int relflag, zhe_paysize_t urisz, const void *uri, zhe_paysize_t payloadlen, zhe_time_t tnow);
void zhe_oc_pack_mwdata_payload(struct out_conduit *c, int relflag, zhe_paysize_t sz, const void *vdata);
void zhe_oc_pack_mwdata_done(struct out_conduit *c, int relflag, zhe_time_t tnow);
int zhe_oc_pack_mdeclare(struct out_conduit *c, bool committed, uint8_t ndecls, zhe_paysize_t decllen, zhe_msgsize_t *from, zhe_time_t tnow);
void zhe_oc_pack_mdeclare_done(struct out_conduit *c, zhe_msgsize_t from, zhe_time_t tnow);
void zhe_pack_dresource(zhe_rid_t rid, zhe_paysize_t urisz, const uint8_t *uri);
void zhe_pack_dpub(zhe_rid_t rid);
void zhe_pack_dsub(zhe_rid_t rid);
void zhe_pack_dcommit(uint8_t commitid);
void zhe_pack_dresult(uint8_t commitid, uint8_t status, zhe_rid_t rid);

#endif
