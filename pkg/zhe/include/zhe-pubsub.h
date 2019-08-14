#ifndef PUBSUB_H
#define PUBSUB_H

#if ZHE_MAX_RID <= 127
#define WC_RID_SIZE 1
#elif ZHE_MAX_RID <= 16383
#define WC_RID_SIZE 2
#elif ZHE_MAX_RID <= 2097151
#define WC_RID_SIZE 3
#elif ZHE_MAX_RID <= 268435455
#define WC_RID_SIZE 4
#elif ZHE_MAX_RID <= 34359738367
#define WC_RID_SIZE 5
#elif ZHE_MAX_RID <= 4398046511103
#define WC_RID_SIZE 6
#elif ZHE_MAX_RID <= 562949953421311
#define WC_RID_SIZE 7
#elif ZHE_MAX_RID <= 72057594037927936
#define WC_RID_SIZE 8
#else
#define WC_RID_SIZE 9
#endif

#define WC_DRESULT_SIZE     (3 + WC_RID_SIZE) /* worst-case result size: header, commitid, status, rid */
#define WC_DCOMMIT_SIZE     (2) /* commit: header, commitid */
#define WC_DPUB_SIZE        (1 + WC_RID_SIZE) /* pub: header, rid (not using properties) */
#define WC_DSUB_SIZE        (2 + WC_RID_SIZE) /* sub: header, rid, mode (neither properties nor periodic modes) */

void zhe_decl_note_error_curpkt(enum zhe_declstatus status, zhe_rid_t rid);
void zhe_decl_note_error_somepeer(peeridx_t peeridx, enum zhe_declstatus status, zhe_rid_t rid);
int zhe_handle_msdata_deliver(zhe_rid_t prid, zhe_paysize_t paysz, const void *pay);
#if ZHE_MAX_URISPACE > 0
int zhe_handle_mwdata_deliver(zhe_paysize_t urisz, const uint8_t *uri, zhe_paysize_t paysz, const void *pay);
#endif

void zhe_pubsub_init(void);

void zhe_rsub_register(peeridx_t peeridx, zhe_rid_t rid, uint8_t submode, bool tentative);
uint8_t zhe_rsub_precommit_status_for_Cflag(peeridx_t peeridx, zhe_rid_t *err_rid);
uint8_t zhe_rsub_precommit(peeridx_t peeridx, zhe_rid_t *err_rid);
void zhe_rsub_commit(peeridx_t peeridx);
void zhe_rsub_precommit_curpkt_abort(peeridx_t peeridx);
void zhe_reset_peer_rsubs(peeridx_t peeridx);
void zhe_rsub_precommit_curpkt_done(peeridx_t peeridx);

void zhe_send_declares(zhe_time_t tnow);
void zhe_note_declstatus(peeridx_t peeridx, uint8_t status, zhe_rid_t rid);
void zhe_reset_peer_declstatus(peeridx_t peeridx);

void zhe_accept_peer_sched_hist_decls(peeridx_t peeridx);
void zhe_reset_peer_unsched_hist_decls(peeridx_t peeridx);
bool zhe_rid_in_use_anonymously(zhe_rid_t rid);
void zhe_update_subs_for_resource_decl(zhe_rid_t rid);

#endif
