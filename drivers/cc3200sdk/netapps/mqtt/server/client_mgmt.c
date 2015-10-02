/******************************************************************************
*
*   Copyright (C) 2014 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/

#include "server_util.h"
#include "client_mgmt.h"
#include "server_pkts.h"

#ifndef CFG_SR_MAX_CL_ID_SIZE
#define MAX_CLIENT_ID_LEN 64
#else
#define MAX_CLIENT_ID_LEN CFG_SR_MAX_CL_ID_SIZE
#endif

#ifndef CFG_SR_MAX_NUM_CLIENT
#define MAX_CLIENTS       16 /* Must not exceed 32 */
#else
#define MAX_CLIENTS       CFG_SR_MAX_NUM_CLIENT

#if (CFG_SR_MAX_NUM_CLIENT > 32)
#error "CFG_SR_MAX_NUM_CLIENT must not exceed 32"
#endif
#endif

static struct client_usr {

        void  *ctx;  /* Client net */
        void  *app;  /* Client app */

        void  *will; /* Will topic */
        u32   index;
        u32   n_sub;

#define MQ_CONNECT_FLAG  0x00000001
#define CLEAN_SESS_FLAG  0x00000002
#define ASSIGNMENT_FLAG  0x00000004   /* No state or connected */

        u32   flags;

        c8    client_id[MAX_CLIENT_ID_LEN];

        struct pub_qos2_cq qos2_rx_cq;
        struct pub_qos2_cq qos2_tx_cq;

} users[MAX_CLIENTS];

// TBD consistency between inline and macro functions

static inline bool is_connected(struct client_usr *cl_usr)
{
        return (cl_usr->flags & MQ_CONNECT_FLAG)? true : false;
}

static inline bool has_clean_session(struct client_usr *cl_usr)
{
        return (cl_usr->flags & CLEAN_SESS_FLAG)? true : false;
}

static inline bool is_assigned(struct client_usr *cl_usr)
{
        return (cl_usr->flags & ASSIGNMENT_FLAG)? true : false;
}

static inline void set_connect_state(struct client_usr *cl_usr,
                                     bool connected)
{
        if(connected)
                cl_usr->flags |=  MQ_CONNECT_FLAG;
        else 
                cl_usr->flags &= ~MQ_CONNECT_FLAG;
}

static inline void set_clean_session(struct client_usr *cl_usr,
                                     bool clean_session)
{
        if(clean_session)
                cl_usr->flags |=  CLEAN_SESS_FLAG;
        else
                cl_usr->flags &= ~CLEAN_SESS_FLAG;
}

static inline void set_assigned(struct client_usr *cl_usr,
                                bool assignment)
{
        if(assignment)
                cl_usr->flags |=  ASSIGNMENT_FLAG;
        else
                cl_usr->flags &= ~ASSIGNMENT_FLAG;
}

static void cl_usr_reset(struct client_usr *cl_usr)
{
        cl_usr->ctx  = NULL;
        cl_usr->app  = NULL;
        cl_usr->will = NULL;

        cl_usr->n_sub = 0;
        cl_usr->flags = 0;
        
        cl_usr->client_id[0] = '\0';

        qos2_pub_cq_reset(&cl_usr->qos2_rx_cq);
        qos2_pub_cq_reset(&cl_usr->qos2_tx_cq);
}

static void cl_usr_init(void)
{
        i32 idx = 0;
        for(idx = 0; idx < MAX_CLIENTS; idx++) {
                struct client_usr *cl_usr = users + idx;
                cl_usr->index =  idx;

                cl_usr_reset(cl_usr);
        }
}

static void cl_usr_free(struct client_usr *cl_usr)
{
        cl_usr_reset(cl_usr);
        cl_usr->flags &= ~(MQ_CONNECT_FLAG | 
                           CLEAN_SESS_FLAG |
                           ASSIGNMENT_FLAG);
}

void cl_sub_count_add(void *usr_cl)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        if(is_connected(cl_usr)) {
                cl_usr->n_sub++;
                USR_INFO("%s has added a new sub, now total is %u\n\r",
                         cl_usr->client_id, cl_usr->n_sub);

        }

        return;
}

void cl_sub_count_del(void *usr_cl)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        if(is_connected(cl_usr)) {
                cl_usr->n_sub--;
                USR_INFO("%s has deleted a sub, now total is %u\n\r",
                         cl_usr->client_id, cl_usr->n_sub);
        }

        return;
}

/*----------------------------------------------------------------------------
 * QoS2 PUB RX Message handling mechanism and associated house-keeping
 *--------------------------------------------------------------------------*/
static inline bool qos2_pub_rx_logup(struct client_usr *cl_usr, u16 msg_id)
{
        return qos2_pub_cq_logup(&cl_usr->qos2_rx_cq, msg_id);
}

static inline bool ack2_msg_id_logup(struct client_usr *cl_usr, u16 msg_id)
{
        return qos2_pub_cq_logup(&cl_usr->qos2_tx_cq, msg_id);
}

static inline bool qos2_pub_rx_unlog(struct client_usr *cl_usr, u16 msg_id)
{
        return qos2_pub_cq_unlog(&cl_usr->qos2_rx_cq, msg_id);
}

static inline bool ack2_msg_id_unlog(struct client_usr *cl_usr, u16 msg_id)
{
        return qos2_pub_cq_unlog(&cl_usr->qos2_tx_cq, msg_id);
}

static inline bool qos2_pub_rx_is_done(struct client_usr *cl_usr, u16 msg_id)
{
        return qos2_pub_cq_check(&cl_usr->qos2_rx_cq, msg_id);
}

bool cl_mgmt_qos2_pub_rx_update(void *usr_cl, u16 msg_id)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        return cl_usr && (qos2_pub_rx_is_done(cl_usr, msg_id) ||
                          qos2_pub_rx_logup(cl_usr, msg_id));
}

static void ack2_msg_id_dispatch(struct client_usr *cl_usr)
{
        struct pub_qos2_cq *tx_cq = &cl_usr->qos2_tx_cq;
        u8 rd_idx = tx_cq->rd_idx;
        u8 n_free = tx_cq->n_free;
        u8 i = 0;

        for(i = rd_idx; i < (MAX_PUBREL_INFLT - n_free); i++) {
                if(mqtt_vh_msg_send(cl_usr->ctx, MQTT_PUBREL, MQTT_QOS1,
                                    true, tx_cq->id_vec[i]) <= 0)
                        break;
        }

        return;
}

static void ack2_msg_id_purge(struct client_usr *cl_usr)
{
        qos2_pub_cq_reset(&cl_usr->qos2_tx_cq);
        qos2_pub_cq_reset(&cl_usr->qos2_rx_cq);
}

/*

*/
static struct mqtt_ack_wlist  wl_qos_ack1 = {NULL, NULL};
static struct mqtt_ack_wlist *qos_ack1_wl = &wl_qos_ack1;

/*

*/
static struct mqtt_ack_wlist  wl_mqp_sess = {NULL, NULL};
static struct mqtt_ack_wlist *sess_mqp_wl = &wl_mqp_sess;

#define MQP_CL_MAP_GET(mqp)         (mqp->private)
#define MQP_CL_MAP_SET(mqp, cl_map) (mqp->private |=  cl_map)
#define MQP_CL_MAP_CLR(mqp, cl_map) (mqp->private &= ~cl_map)

#define CL_BMAP(cl_usr)             (1 << cl_usr->index)

static inline
i32 _pub_dispatch(struct client_usr *cl_usr, struct mqtt_packet *mqp,
                 bool dup)
{
        /* Error, if any, is handled in 'cl_net_close()' ....*/
        return mqtt_server_pub_dispatch(cl_usr->ctx, mqp, dup);
}

static void ack1_wl_mqp_dispatch(struct client_usr *cl_usr)
{
        struct mqtt_packet *mqp = NULL;

        for(mqp = qos_ack1_wl->head; NULL != mqp; mqp = mqp->next)
                if(MQP_CL_MAP_GET(mqp) & CL_BMAP(cl_usr))
                        _pub_dispatch(cl_usr, mqp, true);
}

#if  0
static struct client_usr *find_cl_usr(u32 index)
{
        struct client_usr *cl_usr = users + 0;

        i32 i = 0;
        for(i = 0; i < MAX_CLIENTS; i++, cl_usr++) {
                if(index == cl_usr->index)
                        break;
        }

        return (MAX_CLIENTS == i)? NULL : cl_usr;
}
#endif

#define IS_CL_USR_FREE(cl_usr) ((  0  == cl_usr->flags)        &&       \
                                ('\0' == cl_usr->client_id[0]))

#define IS_CL_INACTIVE(cl_usr) ((  0  == cl_usr->flags)        &&       \
                                ('\0' != cl_usr->client_id[0]))

#define NEED_TO_WAIT_LIST_PUBLISH(qos, cl_usr)                          \
        ((((MQTT_QOS1 == qos) && has_clean_session(cl_usr)) ||          \
          ((MQTT_QOS0 == qos)))?                                        \
         false : true)

static inline u32 _cl_bmap_get(void *usr_cl)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        return IS_CL_USR_FREE(cl_usr)? 0 : CL_BMAP(cl_usr);
}

u32 cl_bmap_get(void *usr_cl)
{
        return usr_cl? _cl_bmap_get(usr_cl) : 0;
}

void *cl_app_hndl_get(void *usr_cl)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        return (cl_usr && is_connected(usr_cl))? cl_usr->app : NULL;
}

void *cl_will_hndl_get(void *usr_cl)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        return (cl_usr && is_connected(usr_cl))? cl_usr->will : NULL;
}

static void pub_dispatch(u32 cl_map, struct mqtt_packet *mqp)
{
        //u32 n_bits = sizeof(u32) << 3;    /* Multiply by 8 */
        u32 sp_map = 0; /* client Map for sessions present */
        enum mqtt_qos qos = ENUM_QOS(mqp->fh_byte1);/* QOS */
        const u32 cl_ref = cl_map; /* Keep ref to original */

        i32 i = 0;
        for(i = 0; i < MAX_CLIENTS; i++) {
                if(cl_map & (1 << i)) {
                        struct client_usr *cl_usr = users + i; //find_cl_usr(i);
                        if(is_connected(cl_usr))
                                if((_pub_dispatch(cl_usr, mqp, false) > 0)  &&
                                   NEED_TO_WAIT_LIST_PUBLISH(qos, cl_usr))
                                        continue;/* Processing done; next CL */

                        /* CL: unconnected / PUB Err / QOS1 PKT (clean sess) */
                        cl_map &= ~(1 << i); 

                        if(IS_CL_INACTIVE(cl_usr)) 
                                sp_map |= (1 << i);  /* CL: Maintain session */
                }
        }

        if(sp_map) {
                struct mqtt_packet *cpy = mqp_server_copy(mqp); /* Make copy */
                if(cpy) {
                        MQP_CL_MAP_SET(cpy, sp_map);
                        mqp_ack_wlist_append(sess_mqp_wl, cpy);
                } else 
                        sp_map = 0;
        }

        if(cl_map) {                                    /* Wait List Publish */
                MQP_CL_MAP_SET(mqp, cl_map);
                mqp_ack_wlist_append(qos_ack1_wl, mqp);
        } else
                mqp_free(mqp); /* PUB MQP now has no more use; must be freed */

        USR_INFO("PUBLISH: CL Map(0x%08x): For Ack 0x%08x, Inactive 0x%08x\n\r",
                 cl_ref, cl_map, sp_map);

        return;
}

void cl_pub_dispatch(u32 cl_map, struct mqtt_packet *mqp)
{
        pub_dispatch(cl_map, mqp);
        return;
}

/* Move to mqtt_common.h and remove it from mqtt_client.h */
static inline i32 len_err_free_mqp(struct mqtt_packet *mqp)
{
        mqp_free(mqp);
        return MQP_ERR_PKT_LEN;
}

/* Move this to a common file */
i32 cl_pub_msg_send(void *usr_cl,
                    const struct utf8_string *topic, const u8 *data_buf,
                    u32 data_len, enum mqtt_qos qos, bool retain)
{
        struct mqtt_packet *mqp = NULL;

        if((NULL == topic)                        ||
           ((data_len > 0) && (NULL == data_buf)) ||
           (NULL == usr_cl))
                return MQP_ERR_FNPARAM;

        mqp = mqp_server_alloc(MQTT_PUBLISH, 2 + topic->length + 2 + data_len);
        if(NULL == mqp)
                return MQP_ERR_PKT_AVL;

        if((0 > mqp_pub_append_topic(mqp, topic, qos? mqp_new_id_server(): 0)) ||
           (data_len && (0 > mqp_pub_append_data(mqp, data_buf, data_len)))) 
                return len_err_free_mqp(mqp);
 
        mqp_prep_fh(mqp, MAKE_FH_FLAGS(false, qos, retain));

        pub_dispatch(_cl_bmap_get(usr_cl), mqp);

        return MQP_CONTENT_LEN(mqp);
}

static void wl_remove(struct mqtt_ack_wlist *list,
                      struct mqtt_packet *prev,
                      struct mqtt_packet *elem)
{
        if(prev)
                prev->next = elem->next;
        else
                list->head = elem->next;

        if(NULL == list->head)
                list->tail = NULL;

        if(list->tail == elem)
                list->tail = prev;

        return;
}

static void sess_wl_mqp_dispatch(struct client_usr *cl_usr)
{
        struct mqtt_packet *mqp = NULL, *prev = NULL, *next = NULL;
        u32 cl_map = CL_BMAP(cl_usr);

        for(mqp = sess_mqp_wl->head; NULL != mqp; prev = mqp, mqp = next) {
                struct mqtt_packet *cpy = NULL;

                if(0 == (MQP_CL_MAP_GET(mqp) & cl_map))
                        continue; /* MQP & CL: no association */

                /* MQP has associated client(s) -  process it */

                /* Dissociate this client from MQP */
                MQP_CL_MAP_CLR(mqp, cl_map);
                next = mqp->next; /* House keeping */

                if(0 == MQP_CL_MAP_GET(mqp)) {
                        /* MQP w/ no clients,  remove from WL */
                        wl_remove(sess_mqp_wl, prev, mqp);

                        /* Ensure correct arrangement for WL  */
                        cpy = mqp; mqp = prev;
                } else {
                        /* MQP is associated w/ other clients */
                        cpy = mqp_server_copy(mqp);
                        if(NULL == cpy)
                                continue;
                }

                /* Got packet from session, dispatch it to CL */
                pub_dispatch(cl_map, cpy);
        }

        return;
}

static
struct mqtt_packet *wl_mqp_unmap_find(struct mqtt_ack_wlist *wl,
                                      struct client_usr *cl_usr, u16 msg_id,
                                      struct mqtt_packet **prev)
{
        struct mqtt_packet *mqp = NULL;

        *prev = NULL;
        for(mqp = wl->head; NULL != mqp; *prev = mqp, mqp = mqp->next) {
                if(msg_id == mqp->msg_id) {
                        /* Found a MQP whose msg_id matches with input */
                        MQP_CL_MAP_CLR(mqp, CL_BMAP(cl_usr));
                        return mqp;
                }
        }

        return NULL;
}

static bool wl_rmfree_try(struct mqtt_ack_wlist *wl, struct mqtt_packet *prev,
                          struct mqtt_packet *mqp)
{
        if(0 == MQP_CL_MAP_GET(mqp)) {
                wl_remove(wl, prev, mqp);
                mqp_free(mqp);

                return true;
        }

        return false;
}

static bool ack1_unmap_rmfree_try(struct client_usr *cl_usr, u16 msg_id)
{
        struct mqtt_packet *prev = NULL, *mqp = NULL;

        if(false == has_clean_session(cl_usr)) {
                mqp = wl_mqp_unmap_find(qos_ack1_wl, cl_usr, msg_id, &prev);
                if(mqp)
                        wl_rmfree_try(qos_ack1_wl, prev, mqp);
        }

        return true;
}

static 
void wl_purge(struct mqtt_ack_wlist *wl, struct client_usr *cl_usr)
{
        struct mqtt_packet *mqp = NULL, *prev = NULL, *next = NULL;
        u32 bmap = CL_BMAP(cl_usr);

        for(mqp = wl->head; NULL != mqp; prev = mqp, mqp = next) {
                next = mqp->next;

                /* Ideally, check should be done to see if cl_usr and mqp are
                   associated. If yes, then the bit should be cleared. At the
                   moment, blind clearing of the bit has been implemented and
                   it has no side effects.
                */
                MQP_CL_MAP_CLR(mqp, bmap);

                /* MQP with no clients has no more use, so try deleting MQP */
                if(wl_rmfree_try(wl, prev, mqp))
                        mqp = prev; /* MQP deleted, prep for next iteration */
        }

        return;
}

static void ack1_wl_mqp_purge(struct client_usr *cl_usr)
{
        wl_purge(qos_ack1_wl, cl_usr);
}

static void sess_wl_mqp_purge(struct client_usr *cl_usr)
{
        wl_purge(sess_mqp_wl, cl_usr);
}

static void session_resume(struct client_usr *cl_usr)
{
        ack1_wl_mqp_dispatch(cl_usr);
        ack2_msg_id_dispatch(cl_usr);
        sess_wl_mqp_dispatch(cl_usr);
}

static void session_delete(struct client_usr *cl_usr)
{
        ack1_wl_mqp_purge(cl_usr);
        ack2_msg_id_purge(cl_usr);
        sess_wl_mqp_purge(cl_usr);
}

static bool has_session_data(struct client_usr *cl_usr)
{
        u32 map = CL_BMAP(cl_usr);
        struct mqtt_packet *elem;

        if(cl_usr->n_sub)
                return true;

        elem = qos_ack1_wl->head;
        while(elem) {
                if(MQP_CL_MAP_GET(elem) & map) 
                        return true;

                elem = elem->next;
        }

        elem = sess_mqp_wl->head;
        while(elem) {
                if(MQP_CL_MAP_GET(elem) & map) 
                        return true;

                elem = elem->next;
        }

        return false;
}

bool cl_can_session_delete(void *usr_cl)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        return cl_usr? (has_clean_session(cl_usr) ||
                        !has_session_data(cl_usr)) : false;
}

void cl_on_net_close(void *usr_cl)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        if(is_assigned(cl_usr)) {
                if(false == has_session_data(cl_usr))
                        cl_usr_free(cl_usr);
        } else if(has_clean_session(cl_usr)) {
                session_delete(cl_usr);
                cl_usr_free(cl_usr);
        } else {
                set_connect_state(cl_usr, false);
                cl_usr->ctx = NULL;
                cl_usr->app = NULL;
        }

        return;
}

static bool proc_pub_rel_rx(struct client_usr *cl_usr, u16 msg_id)
{
        mqtt_vh_msg_send(cl_usr->ctx, MQTT_PUBCOMP,
                         MQTT_QOS0, true, msg_id);

        if(qos2_pub_rx_is_done(cl_usr, msg_id))
                qos2_pub_rx_unlog(cl_usr, msg_id);

        return true;
}

static bool proc_pub_rec_rx(struct client_usr *cl_usr, u16 msg_id)
{
        struct mqtt_packet *prev = NULL, *mqp = NULL;

        mqp = wl_mqp_unmap_find(qos_ack1_wl, cl_usr, msg_id, &prev);
        if(mqp && ack2_msg_id_logup(cl_usr, msg_id)) {

                wl_rmfree_try(qos_ack1_wl, prev, mqp);

                mqtt_vh_msg_send(cl_usr->ctx, MQTT_PUBREL,
                                 MQTT_QOS1, true, msg_id);

                return true;
        }

        return false;
}

bool cl_notify_ack(void *usr_cl, u8 msg_type, u16 msg_id)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;
        bool rv = false;

        if(NULL == cl_usr)
                return rv;

        switch(msg_type) {

        case MQTT_PUBACK:
                rv = ack1_unmap_rmfree_try(cl_usr, msg_id);
                break;

        case MQTT_PUBREC:
                rv = proc_pub_rec_rx(cl_usr, msg_id);
                break;

        case MQTT_PUBREL:
                rv = proc_pub_rel_rx(cl_usr, msg_id);
                break;

        case MQTT_PUBCOMP:
                rv = ack2_msg_id_unlog(cl_usr, msg_id);
                break;

        default:
                break;
        }

        return rv;
}

static void assign_cl_index_as_id(struct client_usr *cl_usr)
{
        /* TBD: need a better implementation */
        c8 *client_id = cl_usr->client_id;

        client_id[0] = 'S';
        client_id[1] = 'e';
        client_id[2] = 'l';
        client_id[3] = 'f';
        client_id[4] = '-';
        client_id[5] = ((cl_usr->index & 0xf0) >> 4) + 0x30;
        client_id[6] = ((cl_usr->index & 0x0f))      + 0x30;
        client_id[7] = '\0';

        /* Make sure that above array size is with in MAX_CLIENT_ID_LEN */

        return;
}

static struct client_usr *assign_cl_usr(c8 *client_id, u8 *vh_buf)
{
        struct client_usr *cl_usr, *fr_usr = NULL;
        i32 idx = 0;
        for(idx = 0; idx < MAX_CLIENTS; idx++) {
                cl_usr = users + idx;
                if((NULL == fr_usr) && IS_CL_USR_FREE(cl_usr))
                        fr_usr = cl_usr;  /* Note 1st free cl_usr */

                if((NULL == client_id) && (NULL != fr_usr)) {
                        /* Free cl_usr is present to create CL-ID */
                        break;

                } else if((NULL != client_id) &&
                          (0 == strncmp(cl_usr->client_id, client_id,
                                        MAX_CLIENT_ID_LEN))) {
                        /* Found a client obj with exact ID match */
                        if(is_connected(cl_usr)) {
                                /* Error: CL-ID is already active */
                                vh_buf[1] = CONNACK_RC_CLI_REJECT;
                                cl_usr    = NULL;
                        }
                        break;
                }
        }

        if(idx == MAX_CLIENTS) {          /* Did not find a match */
                cl_usr   = fr_usr;
                if(NULL == cl_usr)
                        /* Server Resource unavailable */
                        vh_buf[1] = CONNACK_RC_SVR_UNAVBL;
        }

        if(NULL != cl_usr) {
                if(NULL == client_id)
                        assign_cl_index_as_id(cl_usr); /* Get ID */
                else if(IS_CL_USR_FREE(cl_usr))
                        strncpy(cl_usr->client_id, client_id,
                                MAX_CLIENT_ID_LEN);

                set_assigned(cl_usr, true);
        }

        USR_INFO("Assignment of cl_usr %s (detail: index %d and name %s) \n\r",
                 cl_usr? "Succeeded" : "Failed", cl_usr? (i32)cl_usr->index : -1,
                 cl_usr? cl_usr->client_id : "");

        return cl_usr;
}

u16 cl_connect_rx(void *ctx_cl, bool clean_session, c8 *client_id,
                  void *app_cl, void *will, void **usr_cl)
{
        u8 vh_buf[] = {0x00, CONNACK_RC_REQ_ACCEPT};
        struct client_usr *cl_usr;

        if(client_id && ('\0' == client_id[0]))
                /* Shouldn't happen: CLIENT ID with a NUL only string */
                return CONNACK_RC_CLI_REJECT;

        cl_usr = assign_cl_usr(client_id, vh_buf);
        if(NULL == cl_usr)
                return vh_buf[1]; /* Use vh_buf from assign_cl_usr() */

        if((false == clean_session) && has_session_data(cl_usr))
                vh_buf[0] = 0x01; /* Set Session Present Flag */

#if  0
        if(0x00 == vh_buf[1])
                *usr_cl = (void*)cl_usr;
#endif
        *usr_cl = (void*)cl_usr;

        cl_usr->ctx  = ctx_cl;
        cl_usr->app  = app_cl;
        cl_usr->will = will;

        return ((vh_buf[0] << 8) | vh_buf[1]);
}

void cl_on_connack_send(void *usr_cl, bool clean_session)
{
        struct client_usr *cl_usr = (struct client_usr*) usr_cl;

        if(false == is_assigned(cl_usr)) {
                USR_INFO("Fatal: CONNACK for unassigned cl_usr Id %u, abort\n\r",
                         cl_usr->index);
                return;
        }

        set_assigned(cl_usr, false);
        set_clean_session(cl_usr, clean_session);
        set_connect_state(cl_usr, true);

        if(clean_session) {
                session_delete(cl_usr);
        } else {
                session_resume(cl_usr);
        }

        DBG_INFO("Server is now connected to client %s\n\r", cl_usr->client_id);

        return;
}

i32 cl_mgmt_init(void)
{
        cl_usr_init();

        return 0;
}
