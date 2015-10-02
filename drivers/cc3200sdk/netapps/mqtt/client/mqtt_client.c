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

/*
   mqtt_client.c

   The module provides implementation to the public interface of the MQTT
   Client Library.
*/

#include "mqtt_client.h"

static void  *mutex = NULL;
static void (*mutex_lockin)(void*) = NULL;
static void (*mutex_unlock)(void*) = NULL;

#define MUTEX_LOCKIN() if(mutex_lockin) mutex_lockin(mutex);
#define MUTEX_UNLOCK() if(mutex_unlock) mutex_unlock(mutex);

static bool aux_dbg_enbl = false;
i32 (*debug_printf)(const c8 *fmt, ...)   = NULL;

#define USR_INFO debug_printf
#define DBG_INFO(I, ...) if(aux_dbg_enbl) debug_printf(I, ##__VA_ARGS__)

static const struct device_net_services *net_ops = NULL;

static u16 msg_id = 0xFFFF;
static inline u16 assign_new_msg_id()
{
        return msg_id += 2;
}

/*-----------------------------------------------------------------------------
 * Data structure for managing the client and its nuances
 *---------------------------------------------------------------------------*/

/* Construct to manage TX for network that requires LIB to send a partial and
   incremental data to support restrictive TCP segments. Specifically, for the
   deployments, in which the network layer supports small segments, there can
   be only one in-flight message.
*/
struct tx_part_pkt {

        /* Refers to MQP, if TX for it is active, otherwise, set it to NULL */
        struct mqtt_packet *tx_mqp;

        union {
#define VH_MSG_LEN 4
                u8          vh_msg[VH_MSG_LEN];  /* VH msg for MQP = NULL */
                const u8    *buffer;             /* Refers to data in MQP */
        };

        u32                 length;              /* Length of entire data */
        u32                 offset;              /* Next data for sending */
};

static bool tx_part_setup(struct tx_part_pkt *tx_part, const u8 *buffer,
                          u32 length, struct mqtt_packet *tx_mqp)
{
        if(tx_mqp) {
                tx_part->buffer = buffer;
        } else {
                if(VH_MSG_LEN < length)
                        return false;

                buf_wr_nbytes(tx_part->vh_msg, buffer, length);
        }

        tx_part->length = length;
        tx_part->tx_mqp = tx_mqp;

        return true;
}

static void tx_part_reset(struct tx_part_pkt *tx_part)
{
        struct mqtt_packet *tx_mqp = tx_part->tx_mqp;
        if(tx_mqp)
                mqp_free(tx_mqp);

        tx_part->vh_msg[0] = 0x00;
        tx_part->tx_mqp    = NULL;
        tx_part->length    = 0;
        tx_part->offset    = 0;

        return;
}

static const u8 *tx_part_buf_p(struct tx_part_pkt *tx_part)
{
        struct mqtt_packet *tx_mqp = tx_part->tx_mqp;
        u32 offset = tx_part->offset;

        return  tx_mqp?
                tx_part->buffer + offset :
                tx_part->vh_msg + offset;
}

static void tx_part_addup(struct tx_part_pkt *tx_part, u32 offset)
{
        tx_part->offset += offset;
}

#define TX_PART_BUFFER(tx_part)  tx_part_buf_p(tx_part)
#define TX_PART_BUF_SZ(tx_part) (tx_part->length - tx_part->offset)
#define TX_PART_IN_USE(tx_part)  TX_PART_BUF_SZ(tx_part)

enum module_state {

        WAIT_INIT_STATE,
        INIT_DONE_STATE = 0x01,
};

static enum module_state cl_lib_state = WAIT_INIT_STATE;

static u16  loopb_portid = 0;
static bool grp_has_cbfn = false;

#define USE_PROTO_V31_FLAG         MQTT_CFG_PROTOCOL_V31
#define APP_RECV_TASK_FLAG         MQTT_CFG_APP_HAS_RTSK
#define GROUP_CONTEXT_FLAG         MQTT_CFG_MK_GROUP_CTX

#define CLEAN_SESSION_FLAG         0x00010000
#define CONNACK_AWAIT_FLAG         0x00020000
#define NOW_CONNECTED_FLAG         0x00040000
#define KA_PINGER_RSP_FLAG         0x00080000
#define USER_PING_RSP_FLAG         0x00100000
#define NETWORK_CLOSE_FLAG         0x00200000
#define DO_CONNACK_TO_FLAG         0x00400000

static struct client_ctx *free_ctxs  = NULL;  /* CTX construct available */
static struct client_ctx *used_ctxs  = NULL;  /* Relevant only for group */
static struct client_ctx *conn_ctxs  = NULL;  /* Relevant only for group */

static void cl_ctx_freeup(struct client_ctx *cl_ctx)
{
        cl_ctx->next = free_ctxs;
        free_ctxs = cl_ctx;

        return;
}

#define IS_PROTO_VER31(cl_ctx) (cl_ctx->flags & USE_PROTO_V31_FLAG)
#define AWAITS_CONNACK(cl_ctx) (cl_ctx->flags & CONNACK_AWAIT_FLAG)
#define HAS_CONNECTION(cl_ctx) (cl_ctx->flags & NOW_CONNECTED_FLAG)
#define AWAITS_KA_PING(cl_ctx) (cl_ctx->flags & KA_PINGER_RSP_FLAG)
#define AWAITS_PINGRSP(cl_ctx) (cl_ctx->flags & USER_PING_RSP_FLAG)
#define IS_CLN_SESSION(cl_ctx) (cl_ctx->flags & CLEAN_SESSION_FLAG)
#define RECV_TASK_AVBL(cl_ctx) (cl_ctx->flags & APP_RECV_TASK_FLAG)
#define A_GROUP_MEMBER(cl_ctx) (cl_ctx->flags & GROUP_CONTEXT_FLAG)
#define NEED_NET_CLOSE(cl_ctx) (cl_ctx->flags & NETWORK_CLOSE_FLAG)
#define CFG_CONNACK_TO(cl_ctx) (cl_ctx->flags & DO_CONNACK_TO_FLAG)

#ifndef CFG_CL_MQTT_CTXS
#define MAX_NWCONN 4
#else
#define MAX_NWCONN CFG_CL_MQTT_CTXS
#endif

static struct client_desc {

        /* ALERT:  "context" must be first elem in this structure, do not move */
        struct client_ctx           context; 

#define CLIENT(cl_ctx) ((struct client_desc*) cl_ctx)
#define CL_CTX(client) ((struct client_ctx*)  client)

        /* Order/Sequence: Client ID, Will Topic, Will Msg, Username, Password */
        const struct utf8_string    *conn_pl_utf8s[5]; /* Ref: CONNECT Payload */
        u8                           will_opts;

        /* Wait-List for Level 1 ACK(s), which are PUBACK, PUBREC, UN/SUBACK */
        struct mqtt_ack_wlist        qos_ack1_wl;

        /* Circular queue to track QOS2 PUBLISH packets from the server. They
           are tracked for the duration of PUBLISH-RX to PUBREL-RX.
        */
        struct pub_qos2_cq           qos2_rx_cq;

        /* Circular queue to track QOS2 PUBLISH packets from the client. They
           are tracked for the duration of PUBREC-RX to PUBOMP-RX.
        */
        struct pub_qos2_cq           qos2_tx_cq;

        struct mqtt_client_ctx_cbs   app_ctx_cbs; /* Callback funcs from app */
#define CTX_CBS_PTR(cl_ctx) &(CLIENT(cl_ctx)->app_ctx_cbs)

        struct tx_part_pkt           tx_part;/* Reference to partial TX PKT */
        struct mqtt_packet          *rx_mqp; /* Reference to partial RX PKT */
        void                        *app;

        u32                          nwconn_opts;
        c8                          *server_addr;
        u16                          port_number;
        struct secure_conn           nw_security;

} clients[MAX_NWCONN];

static void client_reset(struct client_desc *client)
{
        struct mqtt_client_ctx_cbs *ctx_cbs = &client->app_ctx_cbs;
        i32 i = 0;

        cl_ctx_reset(CL_CTX(client));

        for(i = 0; i < 5; i++)
                client->conn_pl_utf8s[i] = NULL;

        client->will_opts                = 0;

        mqp_ack_wlist_purge(&client->qos_ack1_wl);

        qos2_pub_cq_reset(&client->qos2_rx_cq);
        qos2_pub_cq_reset(&client->qos2_tx_cq);

        ctx_cbs->publish_rx              = NULL;
        ctx_cbs->ack_notify              = NULL;
        ctx_cbs->disconn_cb              = NULL;

        tx_part_reset(&client->tx_part);
        client->rx_mqp                   = NULL;
        client->app                      = NULL;

        client->nwconn_opts              = 0;
        client->server_addr              = NULL;
        client->port_number              = 0;

        secure_conn_struct_init(&client->nw_security);

        return;
}

static void client_desc_init(void)
{
        i32 i = 0;
        for(i = 0; i < MAX_NWCONN; i++) {
                struct client_desc   *client = clients + i;
                struct client_ctx *cl_ctx = CL_CTX(client);

                /* Initialize certain fields to defaults */
                client->qos_ack1_wl.head = NULL;
                client->qos_ack1_wl.tail = NULL;
                client->tx_part.tx_mqp   = NULL;

                client_reset(client); /* Reset remaining */

                cl_ctx->next = free_ctxs;
                free_ctxs    = cl_ctx;
        }

        return;
}

static void mqp_free_locked(struct mqtt_packet *mqp)
{
        MUTEX_LOCKIN();
        mqp_free(mqp);
        MUTEX_UNLOCK();
}

/*----------------------------------------------------------------------------
 * Fix-up to prevent certain good and non-callback MQP being reported to app
 *----------------------------------------------------------------------------
 */
/* cor --> clear on read. */
static bool mqp_do_not_report_cor(struct mqtt_packet *mqp)
{
        bool rv = (1 == mqp->private)? true : false;
        mqp->private = 0;
        return rv;
}

#define MQP_RX_DO_NOT_RPT_COR(mqp)     mqp_do_not_report_cor(mqp)

/* Only if MQP has good RX data i.e. this macro shouldn't be used for bad RX */
#define MQP_RX_DO_NOT_RPT_SET(mqp)    (mqp->private = 1)

#define MQP_TX_DONE_LEN_ADD(mqp, add) (mqp->private += add)
#define MQP_TX_DONE_LEN_GET(mqp)      (mqp->private)

/*---------------------------Fix-Up Ends ------------------------------------*/

static i32 loopb_net        = -1;
static const u8 LOOP_DATA[] = {0x00, 0x01};
#define LOOP_DLEN sizeof(LOOP_DATA)

static i32 loopb_trigger(void)
{
        u8 ip_addr[] = {127,0,0,1};

        return  (-1 != loopb_net)?
                net_ops->send_dest(loopb_net, LOOP_DATA, LOOP_DLEN,
                                   loopb_portid, ip_addr, 4) :
                MQP_ERR_LIBQUIT;
}

static void session_311fix(struct client_ctx *cl_ctx)
{
        struct mqtt_ack_wlist *wl = &CLIENT(cl_ctx)->qos_ack1_wl;
        struct mqtt_packet  *elem = wl->head;

        while(elem) {
                struct mqtt_packet *next = elem->next;
                if(MQTT_PUBLISH != elem->msg_type)
                        mqp_ack_wlist_remove(wl, elem->msg_id);

                elem = next;
        }

        return;
}

static void session_delete(struct client_ctx *cl_ctx)
{
        struct client_desc *client = CLIENT(cl_ctx);

        DBG_INFO("C: Cleaning session for net %d\n\r", cl_ctx->net);

        qos2_pub_cq_reset(&client->qos2_rx_cq);
        qos2_pub_cq_reset(&client->qos2_tx_cq);

        mqp_ack_wlist_purge(&client->qos_ack1_wl);

        return;
}

/*------------------------------------------------------------------------------
 * Routine to manage error conditions in client - close the network connection
 *----------------------------------------------------------------------------*/
static void do_net_close(struct client_ctx *cl_ctx)
{
        i32 net = cl_ctx->net;

        if(-1 == net)
                return;  /* network closed already, must not happen */

        if(IS_CLN_SESSION(cl_ctx)) {
                session_delete(cl_ctx);
        } else if(!IS_PROTO_VER31(cl_ctx)){
                /* Version 3.1.1 doesn't need SUB and UNSUB re-send */
                session_311fix(cl_ctx);
        }

        tx_part_reset(&CLIENT(cl_ctx)->tx_part); /* Part TX, if any */

        cl_ctx->flags &= ~(CONNACK_AWAIT_FLAG | NOW_CONNECTED_FLAG |
                           KA_PINGER_RSP_FLAG | USER_PING_RSP_FLAG |
                           NETWORK_CLOSE_FLAG | DO_CONNACK_TO_FLAG);

        cl_ctx->net = -1;
        net_ops->close(net);

        USR_INFO("C: Net %d now closed\n\r", net);

        return;
}

static void do_net_close_rx(struct client_ctx *cl_ctx, i32 cause)
{
        struct mqtt_client_ctx_cbs *ctx_cbs = CTX_CBS_PTR(cl_ctx);

        DBG_INFO("C: RX closing Net %d [%d]\n\r", cl_ctx->net, cause);

        do_net_close(cl_ctx);
        if(ctx_cbs->disconn_cb)
                ctx_cbs->disconn_cb(CLIENT(cl_ctx)->app, cause);

        if(A_GROUP_MEMBER(cl_ctx))
                cl_ctx_remove(&used_ctxs, cl_ctx);

        return;
}

static void do_net_close_tx(struct client_ctx *cl_ctx, c8 *cause)
{
        DBG_INFO("C: TX closing Net %d [%s]\n\r", cl_ctx->net, cause);

        if(RECV_TASK_AVBL(cl_ctx)) {
                cl_ctx->flags |= NETWORK_CLOSE_FLAG;
                if(A_GROUP_MEMBER(cl_ctx))
                        loopb_trigger();
        } else {
                struct mqtt_packet *rx_mqp = CLIENT(cl_ctx)->rx_mqp;

                do_net_close(cl_ctx);  /* No RX Task, close now */

                /* Release partial MQP, if any, for a CTX w/ CB */
                if((NULL != rx_mqp) && (NULL != rx_mqp->free))
                        mqp_free(rx_mqp);

                CLIENT(cl_ctx)->rx_mqp = NULL;
        }

        return;
}

/*----------------------------------------------------------------------------
 * QoS2 PUB RX Message handling mechanism and associated house-keeping
 *--------------------------------------------------------------------------*/
static bool qos2_pub_rx_logup(struct client_ctx *cl_ctx, u16 msg_id)
{
        return qos2_pub_cq_logup(&CLIENT(cl_ctx)->qos2_rx_cq, msg_id);
}

static bool ack2_msg_id_logup(struct client_ctx *cl_ctx, u16 msg_id)
{
        return qos2_pub_cq_logup(&CLIENT(cl_ctx)->qos2_tx_cq, msg_id);
}

static bool qos2_pub_rx_unlog(struct client_ctx *cl_ctx, u16 msg_id)
{
        return qos2_pub_cq_unlog(&CLIENT(cl_ctx)->qos2_rx_cq, msg_id);
}

static bool ack2_msg_id_unlog(struct client_ctx *cl_ctx, u16 msg_id)
{
        struct client_desc *client = CLIENT(cl_ctx);
        if(qos2_pub_cq_unlog(&client->qos2_tx_cq, msg_id)) {
                struct mqtt_client_ctx_cbs *ctx_cbs = CTX_CBS_PTR(cl_ctx);
                if(ctx_cbs->ack_notify)
                        ctx_cbs->ack_notify(client->app, MQTT_PUBCOMP,
                                            msg_id, NULL, 0);
                return true;
        }

        return false;
}

static bool qos2_pub_rx_is_done(struct client_ctx *cl_ctx, u16 msg_id)
{
        return qos2_pub_cq_check(&CLIENT(cl_ctx)->qos2_rx_cq, msg_id);
}

static bool awaits_pkts(struct client_ctx *cl_ctx)
{
        struct client_desc *client = CLIENT(cl_ctx);

        return  client->qos_ack1_wl.head                ||
                qos2_pub_cq_count(&client->qos2_rx_cq)  ||
                qos2_pub_cq_count(&client->qos2_tx_cq)?
                true : false;
}

static inline i32 len_err_free_mqp(struct mqtt_packet *mqp)
{
        mqp_free(mqp);
        return MQP_ERR_PKT_LEN;
}

static i32 is_valid_utf8_string(const struct utf8_string *utf8)
{
        /* Valid topic should be > 0 byte and must hosted in usable buffer */
        return ((utf8->length > 0) && (NULL != utf8->buffer))? true : false;
}

#define RET_IF_INVALID_UTF8(utf8)               \
        if(false == is_valid_utf8_string(utf8)) \
                return -1;

static bool is_connected(struct client_ctx *cl_ctx)
{
        return (HAS_CONNECTION(cl_ctx) && !NEED_NET_CLOSE(cl_ctx))?
                true : false;
}

u16 mqtt_client_new_msg_id()
{
        return assign_new_msg_id();
}

bool mqtt_client_is_connected(void *ctx)
{
        return is_connected(CL_CTX(ctx));
}

/*----------------------------------------------------------------------------
 * MQTT TX Routines
 *--------------------------------------------------------------------------*/
static void used_ctxs_TO_sort(struct client_ctx *cl_ctx_TO)
{
        cl_ctx_remove(&used_ctxs, cl_ctx_TO);
        cl_ctx_timeout_insert(&used_ctxs, cl_ctx_TO);
}

static inline i32 net_send(i32 net, const u8 *buf, u32 len, void *ctx)
{
        i32 rv = net_ops->send(net, buf, len, ctx);
        if(rv <= 0)
                rv = MQP_ERR_NETWORK;

        return rv;
}

#if 0
static i32 cl_ctx_send(struct client_ctx *cl_ctx, const u8 *buf, u32 len,
                       bool is_conn_msg)
{        
        i32 rv = MQP_ERR_NOTCONN; 

        /* For CONNECT msg, a client context mustn't be already connected.
           For others msgs, a client context must be conected to server */
        if(false == (is_conn_msg ^ is_connected(cl_ctx)))
                goto cl_ctx_send_exit1;

        rv = net_send(cl_ctx->net, buf, len);
        if(rv > 0) { /* A good send, do follow-up */
                cl_ctx_timeout_update(cl_ctx, net_ops->time());
                if(A_GROUP_MEMBER(cl_ctx) && HAS_CONNECTION(cl_ctx)) {
                        /* With update to timeout,
                           a sorting is impending */
                        used_ctxs_TO_sort(cl_ctx);
                }

                goto cl_ctx_send_exit1; /* A Good Send */
        }

        do_net_close_tx(cl_ctx, "snd-err");

 cl_ctx_send_exit1:
        USR_INFO("C: FH-B1 0x%02x, len %u bytes, to net %d: %s\n\r",
                 *buf, len, cl_ctx->net, (rv > 0)? "Sent" : "Fail");
        return rv;
}
#endif

static i32 cl_ctx_part_send(struct client_ctx *cl_ctx)
{
        struct tx_part_pkt *tx_part = &CLIENT(cl_ctx)->tx_part;
        const u8 *buf = TX_PART_BUFFER(tx_part);
        u32 len = TX_PART_BUF_SZ(tx_part);
        u32 ofs = tx_part->offset;
        u8  B1  = *buf;

        i32 rv = net_send(cl_ctx->net, buf, len, (void*)cl_ctx);
        if(rv > 0) { /* Follow-up for a good send */
                if(HAS_CONNECTION(cl_ctx)) {
                        /* Update TX timeout, if 'CTX' is connected */
                        cl_ctx_timeout_update(cl_ctx, net_ops->time());

                        /* After update, 'CTX'(s) sorting is a must */
                        if(A_GROUP_MEMBER(cl_ctx))
                                used_ctxs_TO_sort(cl_ctx);
                }

                if(rv != len)
                        /* Partial data was sent */
                        tx_part_addup(tx_part, rv);
                else
                        tx_part_reset(tx_part);

                goto cl_ctx_send_exit1; /* A Good Send */
        }

        do_net_close_tx(cl_ctx, "snd-err");

 cl_ctx_send_exit1:
        USR_INFO("C: %s 0x%02x to net %d, %s (%d Bytes) [@ %u]\n\r",
                 ofs? "PartN" : "FH-B1", B1, cl_ctx->net,
                 (rv > 0)? "Sent" : "Fail", rv, net_ops->time());

        return rv;
}

static i32 cl_ctx_seg1_send(struct client_ctx *cl_ctx, const u8 *buf, u32 len,
                            bool is_conn_msg, struct mqtt_packet *tx_mqp)
{
        struct tx_part_pkt *tx_part = &CLIENT(cl_ctx)->tx_part;

        /* For CONNECT msg, a client context mustn't be already connected.
           For others msgs, a client context must be conected to server */
        if(false == (is_conn_msg ^ is_connected(cl_ctx)))
                return MQP_ERR_NOTCONN;

        if(TX_PART_IN_USE(tx_part))
                return MQP_ERR_BADCALL;

        tx_part_setup(tx_part, buf, len, tx_mqp);

        return cl_ctx_part_send(cl_ctx);
}

i32 mqtt_client_send_progress(void *ctx)
{
        struct client_ctx *cl_ctx = CL_CTX(ctx);
        struct tx_part_pkt *tx_part = NULL;
        i32 rv = MQP_ERR_BADCALL;

        if(NULL == ctx)
                return MQP_ERR_FNPARAM;

        tx_part = &CLIENT(cl_ctx)->tx_part;

        if(!TX_PART_IN_USE(tx_part))
                return rv;

        rv = cl_ctx_part_send(cl_ctx);
        if(rv > 0)
                rv = TX_PART_BUF_SZ(tx_part);

        return rv;
}

static i32 wr_connect_pl(struct client_ctx *cl_ctx, u8 *buf,
                         u32 fsz, u8 *conn_opts)
{
        /* UTF8 usage selection order: Client, W-Topic, W-Msg, User-Name, Pwd */
        u8 utf8_sel[] = {0x00, WILL_CONFIG_VAL, 0x00,
                         USER_NAME_OPVAL, PASS_WORD_OPVAL};
        struct client_desc *client = CLIENT(cl_ctx);
        u8 *ref = buf;

        i32 i = 0; 

        for(i = 0; i < 5; i++) {                         /* TBD 5 --> macro */
                u16 len = 2;
                const struct utf8_string *utf8 = client->conn_pl_utf8s[i];
                if(NULL == utf8) {
                        /* UTF8 absent: Client ID (i = 0) and Will MSG (i = 2)
                           set zero byte length in the CONNECT message */
                        if(0 != i)
                                if(!((2 == i) && (*conn_opts & WILL_CONFIG_VAL)))
                                        continue;      /* Others, just pass */
                } else {
                        len += utf8->length;
                }
                
                if(fsz < (buf - ref + len))          /* TBD end = ref + fsz */
                        return MQP_ERR_PKT_LEN;   /* Payload: no space left */

                if(2 == len)
                        buf += buf_wr_nbo_2B(buf, 0);  /*  WR 0 byte length */
                else
                        buf += mqp_buf_wr_utf8(buf, utf8);

                *conn_opts |= utf8_sel[i];          /* Enable message flags */
        }

        return buf - ref;
}


/* Define protocol information for the supported versions */
static u8 mqtt310[] = {0x00, 0x06, 'M', 'Q', 'I', 's', 'd', 'p', 0x03};
static u8 mqtt311[] = {0x00, 0x04, 'M', 'Q', 'T', 'T', 0x04};

static inline u16 get_connect_vh_len(struct client_ctx *cl_ctx)
{
        return (IS_PROTO_VER31(cl_ctx)? sizeof(mqtt310) : sizeof(mqtt311))
                + 3;
}

static i32 wr_connect_vh(struct client_ctx *cl_ctx, u8 *buf,
                         u16 ka_secs, u8 conn_opts)
{
        u8  *ref = buf;

        if(IS_PROTO_VER31(cl_ctx)) 
                buf += buf_wr_nbytes(buf, mqtt310, sizeof(mqtt310));
        else
                buf += buf_wr_nbytes(buf, mqtt311, sizeof(mqtt311));

        *buf++ = conn_opts; 
        buf += buf_wr_nbo_2B(buf, ka_secs);

        return buf - ref;
}

static i32 net_connect(struct client_ctx *cl_ctx)
{
        struct client_desc *client = CLIENT(cl_ctx);

        if(NEED_NET_CLOSE(cl_ctx))
                return MQP_ERR_NOT_DEF;

        if(NULL == net_ops)
                return MQP_ERR_NET_OPS;

        cl_ctx->net = net_ops->open(client->nwconn_opts | DEV_NETCONN_OPT_TCP,
                                    client->server_addr,
                                    client->port_number,
                                    &client->nw_security);

        return (-1 == cl_ctx->net)? MQP_ERR_NETWORK : 0;
}

static
i32 cl_ctx_conn_state_try_locked(struct client_ctx *cl_ctx, const u8 *buf,
                                 u32 len, u16 ka_secs, bool clean_session,
                                 struct mqtt_packet *tx_mqp)
{
        i32 rv;

        MUTEX_LOCKIN();
        rv = net_connect(cl_ctx);
        if(rv < 0)
                goto cl_ctx_conn_state_try_locked_exit1;

        /* Ensure LIB is initialized & CTX isn't awaiting CONNACK */
        rv = MQP_ERR_BADCALL;
        if(false == ((INIT_DONE_STATE != cl_lib_state) ||
                     (AWAITS_CONNACK(cl_ctx))))
                rv = cl_ctx_seg1_send(cl_ctx, buf, len, true, tx_mqp);

        if(rv < 0)
                goto cl_ctx_conn_state_try_locked_exit1; /* Fail */

        /* Successfully sent CONNECT msg - let's do housekeeping */
        cl_ctx->timeout = net_ops->time();/* Fixup: CONN TX Time */
        cl_ctx->flags  |= DO_CONNACK_TO_FLAG | CONNACK_AWAIT_FLAG;
        cl_ctx->flags  |= clean_session? CLEAN_SESSION_FLAG : 0;

        cl_ctx->ka_secs = ka_secs;

        if(A_GROUP_MEMBER(cl_ctx)) {
                cl_ctx->next = conn_ctxs;
                conn_ctxs    = cl_ctx;

                /* First entry in 'conn_ctxs': schedule a move to
                   'used_conn' (for house-keeping and tracking) */
                if(NULL == cl_ctx->next)
                        rv = loopb_trigger();
        }

 cl_ctx_conn_state_try_locked_exit1:
        MUTEX_UNLOCK();

        return rv;
}

static
i32 connect_msg_send(struct client_ctx *cl_ctx, bool clean_session, u16 ka_secs)
{
        struct mqtt_packet *mqp = mqp_client_send_alloc(MQTT_CONNECT);
        u8 *buf, *ref, conn_opts = clean_session? CLEAN_START_VAL : 0;
        i32 rv = MQP_ERR_PKT_LEN; 
        u32 fsz; /* Free buffer size in PKT */ 
        u16 vhl = get_connect_vh_len(cl_ctx);

        if(NULL == mqp)
                return MQP_ERR_PKT_AVL;

        fsz = MQP_FREEBUF_LEN(mqp);
        if(fsz < vhl)
                goto connect_msg_send_exit1;           /* No space for VAR HDR */

        mqp->vh_len = vhl;               /* Reserve buffer for variable header */
        buf = ref = MQP_PAYLOAD_BUF(mqp);/* Get started to incorporate payload */

        rv = wr_connect_pl(cl_ctx, buf, fsz - vhl, &conn_opts);/* Payload data */
        if(rv < 0)
                goto connect_msg_send_exit1;              /* Payload WR failed */

        buf += rv;
        mqp->pl_len = buf - ref;

        wr_connect_vh(cl_ctx, ref - vhl, ka_secs,
                      CLIENT(cl_ctx)->will_opts | conn_opts);    /* Var Header */

        mqp_prep_fh(mqp, MAKE_FH_FLAGS(false, MQTT_QOS0, false));/* Fix Header */

        ref = MQP_FHEADER_BUF(mqp);

        /* Following routine frees up MQP - whether error or not */
        return cl_ctx_conn_state_try_locked(cl_ctx, ref, buf - ref,
                                            ka_secs, clean_session,
                                            mqp);
 connect_msg_send_exit1:
        if(mqp)
                mqp_free_locked(mqp);

        return rv;
}

i32 mqtt_connect_msg_send(void *ctx, bool clean_session, u16 ka_secs)
{
        return  ctx?
                connect_msg_send(CL_CTX(ctx), clean_session, ka_secs) : -1;
}

/*
   To be used for the following messages: PUBLISH, SUBSCRIBE, UNSUBSCRIBE
   Dispatches msg to broker over socket. Frees-up MQP, in case, MSG has QoS0 or
   if client-lib allocated MQP encounters an error in dispatch.
   Returns, on success, number of bytes transfered, otherwise -1 
*/
static i32 _msg_dispatch(struct client_ctx *cl_ctx, struct mqtt_packet *mqp,
                         enum mqtt_qos qos, bool retain)
{
        bool not_qos0 = (MQTT_QOS0 != qos)? true : false;
        u16 msg_id = mqp->msg_id;
        i32 rv = MQP_ERR_NETWORK;

        mqp_prep_fh(mqp, MAKE_FH_FLAGS(false, qos, retain));

        MUTEX_LOCKIN();

        if(not_qos0)
                mqp->n_refs++;   /* Need to enlist, do not free-up MQP */

        /* Tries to free-up MQP either on error or if full pkt is sent */
        rv = cl_ctx_seg1_send(cl_ctx, MQP_FHEADER_BUF(mqp),
                              MQP_CONTENT_LEN(mqp),  false,
                              mqp);

        /* At this point, error or not, QoS0 MQP would have been freed */

        if((rv <= 0) && not_qos0) {
                mqp_free(mqp); /* Err: Explicitly free-up non QoS0 MQP */

                goto _msg_dispatch_exit1;
        }

        rv = msg_id;   /* Make progress for a good send to the server  */

        if(not_qos0)   /* Enlist non QOS0 MQP to await ACK from server */
                mqp_ack_wlist_append(&CLIENT(cl_ctx)->qos_ack1_wl, mqp);

_msg_dispatch_exit1:

        MUTEX_UNLOCK();

        return rv;
}

static
i32 msg_dispatch_no_free(struct client_ctx *cl_ctx, struct mqtt_packet *mqp,
                         enum mqtt_qos qos, bool retain)
{
        if((NULL == mqp) || (NULL == cl_ctx))
                return MQP_ERR_FNPARAM;

        mqp->n_refs++; /* Ensures caller that MQP is not freed-up */

        return _msg_dispatch(cl_ctx, mqp, qos, retain);
}

i32 mqtt_client_pub_msg_send(void *ctx, const struct utf8_string *topic,
                             const u8 *data_buf, u32 data_len,
                             enum mqtt_qos qos, bool retain)
{
        struct mqtt_packet *mqp = NULL;

        if((NULL == ctx)   || 
           (NULL == topic) || 
           ((data_len > 0) && (NULL == data_buf)))
                return MQP_ERR_FNPARAM;

        if(false == is_valid_utf8_string(topic))
                return MQP_ERR_CONTENT;

        mqp = mqp_client_send_alloc(MQTT_PUBLISH);
        if(NULL == mqp)
                return MQP_ERR_PKT_AVL;

        if((0 > mqp_pub_append_topic(mqp, topic, qos? assign_new_msg_id(): 0)) ||
           (data_len && (0 > mqp_pub_append_data(mqp, data_buf, data_len)))) 
                return len_err_free_mqp(mqp);

        return _msg_dispatch(CL_CTX(ctx), mqp, qos, retain);
}

i32 mqtt_client_pub_dispatch(void *ctx, struct mqtt_packet *mqp,
                             enum mqtt_qos qos, bool retain)
{
        return msg_dispatch_no_free(CL_CTX(ctx), mqp, qos, retain);
}

static i32 tail_incorp_msg_id(struct mqtt_packet *mqp)
{
        u8 *buf = MQP_FHEADER_BUF(mqp) + mqp->vh_len;

        if(0 == mqp->msg_id) {
                mqp->msg_id  = assign_new_msg_id();
                buf += buf_wr_nbo_2B(buf, mqp->msg_id);
                mqp->vh_len += 2;

                return 2;
        }

        return 0;
}

static i32 buf_utf8_wr_try(u8 *buf, u32 fsz, const struct utf8_string *topic,
                           u8 qid)
{
        u8 *ref = buf;

        if(fsz < (topic->length + 2 + (QFL_VALUE == qid)? 0 : 1))
                return MQP_ERR_PKT_LEN; /* No buf */

        if(false == is_valid_utf8_string(topic))
                return MQP_ERR_CONTENT;/* Invalid */

        buf += mqp_buf_wr_utf8(buf, topic);
        if(QFL_VALUE != qid)
                *buf++ = qid;

        return buf - ref;
}

static i32 utf8_array_send(struct client_ctx           *cl_ctx,
                           const struct utf8_strqos *subsc_vec,
                           const struct utf8_string *unsub_vec,
                           u32 n_elem)
{
        struct mqtt_packet *mqp;
        u8 *ref, *buf, *end;        
        u32 i;

        if((NULL == cl_ctx) || !((!!subsc_vec) ^ (!!unsub_vec)) || (0 == n_elem))
                return MQP_ERR_FNPARAM;

        mqp = mqp_client_send_alloc(subsc_vec? 
                                    MQTT_SUBSCRIBE : MQTT_UNSUBSCRIBE);
        if(NULL == mqp)
                return MQP_ERR_PKT_AVL;

        buf = MQP_VHEADER_BUF(mqp);
        end = MQP_FREEBUF_LEN(mqp) + buf;  /* End of free buffer */
        if((end - buf) < 2)
                return len_err_free_mqp(mqp);/* MSG-ID: no space */

        buf += tail_incorp_msg_id(mqp);
        ref  = buf;

        for(i = 0; i < n_elem; i++) {
                const struct utf8_string *topic;
                struct utf8_string topreq;
                i32 rv;

                if(subsc_vec) {
                        topreq.length = subsc_vec[i].length;
                        topreq.buffer = subsc_vec[i].buffer;
                        topic = &topreq;
                } else
                        topic = unsub_vec + i;

                rv = buf_utf8_wr_try(buf, end - buf, topic, subsc_vec?
                                     (u8)subsc_vec[i].qosreq : QFL_VALUE);
                if(rv < 0) {
                        mqp_free(mqp);
                        return rv;
                }

                buf += rv;
        }

        mqp->pl_len = buf - ref; /* Total length of topics data */

        return _msg_dispatch(cl_ctx, mqp, MQTT_QOS1, false);
}

i32 mqtt_sub_msg_send(void *ctx, const struct utf8_strqos *qos_topics, u32 count)
{
        return utf8_array_send(CL_CTX(ctx), qos_topics, NULL, count);
}

i32 mqtt_sub_dispatch(void *ctx, struct mqtt_packet *mqp)
{
        return msg_dispatch_no_free(CL_CTX(ctx), mqp, MQTT_QOS1, false);
}

i32 mqtt_unsub_msg_send(void *ctx, const struct utf8_string *topics, u32 count)
{
        return utf8_array_send(CL_CTX(ctx), NULL, topics, count);
}

i32 mqtt_unsub_dispatch(void *ctx, struct mqtt_packet *mqp)
{
        return msg_dispatch_no_free(CL_CTX(ctx), mqp, MQTT_QOS1, false);
}

/* Note: in this revision of implementation, vh_msg_send() is being invoked
   from a locked RX context. Should this situation change, so should the 
   'locking' considerations in the routine. */
static i32 vh_msg_send(struct client_ctx *cl_ctx, u8 msg_type,
                       enum mqtt_qos qos, bool has_vh,
                       u16 vh_data)
{
        u8  buf[4];
        u32 len = 2;

        buf[0] = MAKE_FH_BYTE1(msg_type, MAKE_FH_FLAGS(false, qos, false));
        buf[1] = has_vh ? 2 : 0;

        if(has_vh)
                len += buf_wr_nbo_2B(buf + 2, vh_data);

        return cl_ctx_seg1_send(cl_ctx, buf, len, false, NULL);
}

static i32 pingreq_send(struct client_ctx *cl_ctx, u32 rsp_flag)
{
        i32 rv = 0;
        u8 buf[2];

        buf[0] = MAKE_FH_BYTE1(MQTT_PINGREQ,
                               MAKE_FH_FLAGS(false, MQTT_QOS0, false));
        buf[1] = 0;

        /* Note: in case of error in network send, cl_ctx_send() may
           try to terminate connection with server. */
        rv = cl_ctx_seg1_send(cl_ctx, buf, 2, false, NULL);
        if(rv > 0)
                cl_ctx->flags |= rsp_flag;

        return rv;
}

i32 mqtt_pingreq_send(void *ctx)
{
        i32 rv = 0;

        MUTEX_LOCKIN();
        rv = pingreq_send(CL_CTX(ctx), USER_PING_RSP_FLAG);
        MUTEX_UNLOCK();

        return rv;                
}

i32 mqtt_disconn_send(void *ctx)
{
        u8 buf[2];

        buf[0] = MAKE_FH_BYTE1(MQTT_DISCONNECT,
                               MAKE_FH_FLAGS(false, MQTT_QOS0, false));
        buf[1] = 0;

        MUTEX_LOCKIN();
        /* Note: in case of error in network send, cl_ctx_send() may
           try to terminate connection with server. */
        if(cl_ctx_seg1_send(CL_CTX(ctx), buf, 2, false, NULL) > 0)
                /* Terminate connection on application's request */
                do_net_close_tx(CL_CTX(ctx), "DISCONN");

        MUTEX_UNLOCK();

        return 0;
}

/*------------------------------------------------------------------------------
 *  MQTT RX Routines
 *------------------------------------------------------------------------------
 */
static bool ack1_wl_mqp_dispatch(struct client_ctx *cl_ctx)
{
        struct mqtt_ack_wlist *wlist = &CLIENT(cl_ctx)->qos_ack1_wl;
        struct mqtt_packet *mqp = NULL;
        bool rv = true;

        for(mqp = wlist->head; mqp && (true == rv); mqp = mqp->next) {
                u8 *buf = MQP_FHEADER_BUF(mqp);
                mqp->fh_byte1 = *buf |= DUP_FLAG_VAL(true);

                mqp->n_refs++; /* Ensures MQP is not freed by following */

                /* Error or not, following routine tries to free up MQP */
                if(cl_ctx_seg1_send(cl_ctx, buf, MQP_CONTENT_LEN(mqp),
                                    false, mqp) <= 0)
                        rv = false;
        }

        return rv;
}

/* TBD candidate for common */
static bool ack2_msg_id_dispatch(struct client_ctx *cl_ctx)
{
        struct pub_qos2_cq *tx_cq = &CLIENT(cl_ctx)->qos2_tx_cq;
        u8 rd_idx = tx_cq->rd_idx;
        u8 n_free = tx_cq->n_free;
        bool rv = true;
        u8 i = 0;

        for(i = rd_idx; i < (MAX_PUBREL_INFLT - n_free) && (true == rv); i++) {
                if(vh_msg_send(cl_ctx, MQTT_PUBREL, MQTT_QOS1,
                               true, tx_cq->id_vec[i]) <= 0)
                        rv = false;
        }

        return rv;
}

static void session_resume(struct client_ctx *cl_ctx)
{
        DBG_INFO("C: Re-send ACK awaited QoS1/2 msgs to net %d\n\r",
                 cl_ctx->net);

        if(ack1_wl_mqp_dispatch(cl_ctx))
                ack2_msg_id_dispatch(cl_ctx);
        
        return;
}

static bool ack1_wl_rmfree(struct mqtt_ack_wlist *wl, u16 msg_id)
{
        struct mqtt_packet *mqp = mqp_ack_wlist_remove(wl, msg_id);
        if(NULL != mqp) {
                mqp_free(mqp);
                return true;
        }

        USR_INFO("Err: Unexpected ACK w/ ID 0x%04x\n\r", msg_id);

        return false;
}

static bool _proc_pub_rec_rx(struct client_ctx *cl_ctx, u16 msg_id)
{
        /* Follow-up messages for QOS2 PUB must be transacted in the
           same order as the initial sequence of QOS2 PUB dispatches.
           Therefore, checking the first entry should be OK
        */
        struct mqtt_packet *mqp = CLIENT(cl_ctx)->qos_ack1_wl.head;

        if((msg_id == mqp->msg_id) && ack2_msg_id_logup(cl_ctx, msg_id)) {
                
                ack1_wl_rmfree(&CLIENT(cl_ctx)->qos_ack1_wl, msg_id);

                vh_msg_send(cl_ctx, MQTT_PUBREL, MQTT_QOS1,
                            true, msg_id);

                return true;              
        }

        return false; /* Unexpected PUBREC or QOS2 store exceeded */
}

static bool _proc_pub_rel_rx(struct client_ctx *cl_ctx, u16 msg_id)
{
        /* For a PUB-REL RX, send PUBCOMP to let server make progress */
        vh_msg_send(cl_ctx, MQTT_PUBCOMP, MQTT_QOS0, true, msg_id);

        if(qos2_pub_rx_is_done(cl_ctx, msg_id))
                qos2_pub_rx_unlog(cl_ctx, msg_id);  /* Expunge record */

        return true;
}

/*
   Process ACK Message from Broker.
   Returns true on success, otherwise false.
   Used for: PUBACK, SUBACK and UNSUBACK
*/
static
bool _proc_ack_msg_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        struct mqtt_client_ctx_cbs *ctx_cbs = CTX_CBS_PTR(cl_ctx);
        struct client_desc *client = CLIENT(cl_ctx);
        u16 msg_id = mqp_raw->msg_id;
        u32 len = mqp_raw->pl_len;

        /* Caters to SUB-ACK, UNSUB-ACK and PUB-ACK Messages */
        if(false == ack1_wl_rmfree(&client->qos_ack1_wl, msg_id))
                return false; /* Err: MSG_ID was not awaited */

        if(ctx_cbs->ack_notify)
                ctx_cbs->ack_notify(client->app, mqp_raw->msg_type, msg_id,
                                    len? MQP_PAYLOAD_BUF(mqp_raw): NULL,
                                    len);
        return true;
}

static
bool proc_ack_msg_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        u8  msg_type = mqp_raw->msg_type;
        bool rv = false;
        u16 msg_id = 0;

        if(false == mqp_proc_msg_id_ack_rx(mqp_raw, MQTT_SUBACK == msg_type))
                return rv; /* Problem in contents received from server */

        msg_id = mqp_raw->msg_id;

        if(MQTT_PUBREC == msg_type) {
                rv =  _proc_pub_rec_rx(cl_ctx, msg_id);
                if(rv)
                        MQP_RX_DO_NOT_RPT_SET(mqp_raw); /* Don't report to App */

        } else if(MQTT_PUBREL == msg_type) {
                rv =  _proc_pub_rel_rx(cl_ctx, msg_id);
                if(rv)
                        MQP_RX_DO_NOT_RPT_SET(mqp_raw); /* Don't report to App */
        } else if(MQTT_PUBCOMP == msg_type) {
                rv = ack2_msg_id_unlog(cl_ctx, msg_id);
        } else {
                rv = _proc_ack_msg_rx(cl_ctx, mqp_raw);
        }

        return rv;
}

static
bool proc_pub_msg_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        struct mqtt_client_ctx_cbs *ctx_cbs = CTX_CBS_PTR(cl_ctx);
        bool good_pub = mqp_proc_pub_rx(mqp_raw);
        u8 B = mqp_raw->fh_byte1;
        enum mqtt_qos qos = ENUM_QOS(B);
        u16 msg_id = 0;

        if(false == good_pub)
                return false; /* Didn't get nicely composed PUB Packet */

        msg_id = mqp_raw->msg_id;

        /* Irrespective of the result of the ACK through vh_msg_send(),
           the implementation has chosen to process the good PUB packet.
           Any error will be handled in next iteration of rx processing.
        */
        if(MQTT_QOS1 == qos)
                vh_msg_send(cl_ctx, MQTT_PUBACK, MQTT_QOS0, true, msg_id);

        if(MQTT_QOS2 == qos) {
                /* Ensuring "only once" philosophy for MQTT QoS2 PUBs */
                if(qos2_pub_rx_is_done(cl_ctx, msg_id)) {
                        /* Already delivered. Drop it & do not report */
                        MQP_RX_DO_NOT_RPT_SET(mqp_raw);
                        return true; /* No more follow-up; all's good */
                }

                if(false == qos2_pub_rx_logup(cl_ctx, msg_id))
                        return false;  /* Failed to record New RX PUB */

                vh_msg_send(cl_ctx, MQTT_PUBREC, MQTT_QOS0, true, msg_id);
        }

        /* QoS obliations completed, present PUBLISH RX packet to app */
        if(ctx_cbs->publish_rx) {
                /* App has chosen the callback method to  receive PKT */
                mqp_raw->n_refs++;   /* Make app owner of this packet */
                if(ctx_cbs->publish_rx(CLIENT(cl_ctx)->app, BOOL_DUP(B),
                                       qos, BOOL_RETAIN(B), mqp_raw)) {
                        /* App has no use of PKT any more, so free it */
                        mqp_raw->n_refs--;     /* Take back ownership */
                }
        }

        return true;
}

static
bool proc_connack_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        struct mqtt_client_ctx_cbs *ctx_cbs = CTX_CBS_PTR(cl_ctx);
        u8 *buf = MQP_VHEADER_BUF(mqp_raw);

        mqp_raw->vh_len += 2;
        mqp_raw->pl_len -= 2;

        if(0 != mqp_raw->pl_len)
                return false;      /* There is no payload in message */

        cl_ctx->flags &= ~(DO_CONNACK_TO_FLAG | CONNACK_AWAIT_FLAG);

        if(VHB_CONNACK_RC(buf))
                /* Server has refused the connection, inform the app */
                goto proc_connack_rx_exit1;

        cl_ctx->flags |=  NOW_CONNECTED_FLAG;
        cl_ctx_timeout_update(cl_ctx, net_ops->time());  /* start KA */

        if(IS_CLN_SESSION(cl_ctx))
                session_delete(cl_ctx);
        else
                session_resume(cl_ctx);

 proc_connack_rx_exit1:
        if(ctx_cbs->ack_notify)
                ctx_cbs->ack_notify(CLIENT(cl_ctx)->app, mqp_raw->msg_type,
                                    0, buf, 2);

        return true;
}

static
bool proc_pingrsp_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        struct mqtt_client_ctx_cbs *ctx_cbs = CTX_CBS_PTR(cl_ctx);

        if(0 != mqp_raw->pl_len)
                return false;

        if(AWAITS_KA_PING(cl_ctx)) {
                cl_ctx->flags &= ~KA_PINGER_RSP_FLAG;
                return true;
        }

        if(AWAITS_PINGRSP(cl_ctx)) {
                cl_ctx->flags &= ~USER_PING_RSP_FLAG;
                if(ctx_cbs->ack_notify)
                        ctx_cbs->ack_notify(CLIENT(cl_ctx)->app,
                                            mqp_raw->msg_type,
                                            0, NULL, 0);
                return true;
        }

        return false;
}

static
bool conn_sent_state_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        bool rv = false;

        switch(mqp_raw->msg_type) {

        case MQTT_CONNACK: 
                /* Changes client_ctx->flags to CONNECTED */
                rv = proc_connack_rx(cl_ctx, mqp_raw);
                break;

        default:
                break;
        }

        return rv;
}

static
bool connected_state_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        bool rv = false;

        switch(mqp_raw->msg_type) {

        case MQTT_SUBACK:
        case MQTT_PUBACK:
        case MQTT_PUBREC:
        case MQTT_PUBREL:
        case MQTT_PUBCOMP:
        case MQTT_UNSUBACK:
                rv = proc_ack_msg_rx(cl_ctx, mqp_raw);
                break;

        case MQTT_PINGRSP:
                rv = proc_pingrsp_rx(cl_ctx, mqp_raw);
                break;

        case MQTT_PUBLISH:
                rv = proc_pub_msg_rx(cl_ctx, mqp_raw);
                break;

        case MQTT_CONNACK: /* not expected */
        default:
                break;
        }
        
        return rv;
}

static bool process_recv(struct client_ctx   *cl_ctx,
                         struct mqtt_packet *mqp_raw)
{
        bool rv;

        USR_INFO("C: Rcvd msg Fix-Hdr (Byte1) 0x%02x from net %d [@ %u]\n\r",
                 mqp_raw->fh_byte1, cl_ctx->net, net_ops->time());

        /* Working Principle: Only RX processing errors should be
           reported as 'false'. Status of TX as a follow-up to RX
           messages need not be reported by the xyz_rx() routines.
           Error observed in TX is either dealt in next iteration
           of RX loop (in case, there is a dedicated RX task for
           the CTX) or in TX routine itself (in case, there is no
           dedicated RX task for the CTX).
        */
        rv =    AWAITS_CONNACK(cl_ctx)? 
                conn_sent_state_rx(cl_ctx, mqp_raw) :
                connected_state_rx(cl_ctx, mqp_raw);

        DBG_INFO("C: Msg w/ ID 0x%04x, processing status: %s\n\r",
                 mqp_raw->msg_id, rv? "Good" : "Fail");

        return rv;
}

static i32 net_recv(i32 net, struct mqtt_packet *mqp, u32 wait_secs, void *ctx)
{
        bool timed_out = false;
        i32 rv = mqp_recv(net, net_ops, mqp, wait_secs, &timed_out, ctx);
        if(rv <= 0) {
                USR_INFO("C: Net %d, Raw Error %d, Time Out: %c\n\r",
                         net, rv, timed_out? 'Y' : 'N');

                if(timed_out)
                        rv = MQP_ERR_TIMEOUT;
        }

        return rv;
}

/*
   MQTT 3.1.1 implementation
   -------------------------

   Keep Alive Time is maxmimum interval within which a client should send a
   packet to broker. If there are either no packets to be sent to broker or
   no retries left, then client is expected to a send a PINGREQ within Keep
   Alive Time. Broker should respond by sending PINGRSP with-in reasonable
   time of 'wait_secs'. If Keep Alive Time is set as 0, then client is not
   expected to be disconnected due to in-activity of MQTT messages. Value
   of 'wait_secs' is assumed to be quite smaller than (non-zero) 'ka_secs'.
*/
static void conn2used_ctxs(u32 wait_secs)
{
        while(conn_ctxs) {
                struct client_ctx *cl_ctx = conn_ctxs;
                conn_ctxs = conn_ctxs->next;

                cl_ctx_timeout_insert(&used_ctxs, cl_ctx);
        }
}

static i32 single_ctx_ka_sequence(struct client_ctx *cl_ctx, u32 wait_secs)
{
        u32 now_secs = net_ops->time();

        if(AWAITS_CONNACK(cl_ctx) && CFG_CONNACK_TO(cl_ctx)) {
                cl_ctx->timeout += wait_secs; /* Set CONNACK timeout value */
                cl_ctx->flags   &= ~DO_CONNACK_TO_FLAG;
        }

        if(cl_ctx->timeout > now_secs)
                return 1;  /* Still have time for next message transaction */

        if(is_connected(cl_ctx)) {
                /* Timeout has happened. Check for PINGRESP if PINGREQ done.
                   Otherwise, send PINGREQ (Are You there?) to the server. */
                if(AWAITS_KA_PING(cl_ctx))
                        goto single_ctx_ka_sequence_exit1; /* No PINGRESP  */

                return pingreq_send(cl_ctx, KA_PINGER_RSP_FLAG); /* Hello! */
        }

 single_ctx_ka_sequence_exit1:
        USR_INFO("C: Net %d, no RX MSG in reasonable time\n\r", cl_ctx->net);
        return -1;
}

static u32 single_ctx_adj_wait_secs_get(struct client_ctx *cl_ctx, u32 wait_secs)
{
        return (KA_TIMEOUT_NONE != cl_ctx->timeout)? 
                MIN(cl_ctx->timeout - net_ops->time(), wait_secs) : wait_secs;
}

static i32 single_ctx_rx_prep(struct client_ctx *cl_ctx, u32 *secs2wait)
{
        i32 rv;

        if(-1 == cl_ctx->net)
                return MQP_ERR_NOTCONN; /* Likely for a ctx w/o a receive task */

        if(NEED_NET_CLOSE(cl_ctx))
                rv = MQP_ERR_NOTCONN;
        else if(0 > single_ctx_ka_sequence(cl_ctx, *secs2wait))
                rv = MQP_ERR_NETWORK;
        else {
                *secs2wait = single_ctx_adj_wait_secs_get(cl_ctx, *secs2wait);
                return 1;
        }

        do_net_close_rx(cl_ctx, rv);
        return rv;
}

static
i32 proc_ctx_data_recv(struct client_ctx *cl_ctx, struct mqtt_packet *mqp,
                       u32 wait_secs, void **app)
{
        i32 rv  = MQP_ERR_NOTCONN;
        i32 net = cl_ctx->net;

        *app = cl_ctx->usr;

        rv = net_recv(net, mqp, wait_secs, (void*)cl_ctx);

        MUTEX_LOCKIN();

        if(rv > 0)
                if(false == process_recv(cl_ctx, mqp))
                        rv = MQP_ERR_CONTENT;

        /* RX: close the network connection to the server for this context, if
           (a) there is a processing / protocol error other than time-out
           (b) A good MQTT CONNACK has a return code - connection refused
        */
        if(((rv < 0) && (rv != MQP_ERR_TIMEOUT)) ||
           ((MQTT_CONNACK == mqp->msg_type) &&
            MQP_CONNACK_RC(mqp)))
                do_net_close_rx(cl_ctx, rv);

        MUTEX_UNLOCK();

        return rv;
}

static i32 mqp_setup_proc_ctx_data_recv(struct client_ctx *cl_ctx,
                                        struct mqtt_packet *mqp,
                                        u32 wait_secs, void **app)
{
        struct mqtt_packet *rx_mqp = CLIENT(cl_ctx)->rx_mqp;
        i32 rv;

        if(NULL != mqp) {
                /* Input MQP must be same as MQP for partial RX, if any */
                if(rx_mqp) {
                        if(mqp != rx_mqp)
                                return MQP_ERR_FNPARAM;
                } else
                        mqp_reset(mqp);
        }

        if(NULL == mqp) {
                mqp = rx_mqp? rx_mqp : mqp_client_recv_alloc(0);
                if(NULL == mqp)
                        return MQP_ERR_PKT_AVL;
        }

        rv = proc_ctx_data_recv(cl_ctx, mqp, wait_secs, app);
        if(rv == MQP_ERR_TIMEOUT) {
                CLIENT(cl_ctx)->rx_mqp = mqp;  /* Save partial RX MQP */
        } else {
                /* Control reaches here due to either an error in RX or the
                   completion of RX. In both the cases, the MQP needs to be
                   detached and processed. For completion of RX:
                   callback mode: Application has used up MQP data; free it
                   Non-callback mode: Application will now use complete MQP
                */
                CLIENT(cl_ctx)->rx_mqp = NULL;
                if(mqp->free)
                        mqp_free_locked(mqp);  /* For only callback mode */
        }

        return rv;
}

static i32 cl_ctx_recv(struct client_ctx *cl_ctx, struct mqtt_packet *mqp,
                       u32 wait_secs)
{
        void *app = NULL;
        i32 rv = 0;

        do {
                if(mqp && (NULL == CLIENT(cl_ctx)->rx_mqp))
                        mqp_reset(mqp);

                rv = single_ctx_rx_prep(cl_ctx, &wait_secs);
                if(rv > 0)
                        rv = mqp_setup_proc_ctx_data_recv(cl_ctx, mqp,
                                                          wait_secs,
                                                          &app);

                /* 'mqp' must be valid, if rv > 0 but including further
                   & additional check for sake of static cod eanalysis.*/
        } while((rv > 0) && mqp && MQP_RX_DO_NOT_RPT_COR(mqp));
        
        return rv;
}

i32 mqtt_client_ctx_await_msg(void *ctx, u8 msg_type, struct mqtt_packet *mqp,
                              u32 wait_secs)
{
        struct client_ctx *cl_ctx = CL_CTX(ctx);
        i32 rv = -1;

        if((NULL == cl_ctx) || (NULL == mqp))
                return MQP_ERR_FNPARAM;

        do {
                rv = cl_ctx_recv(cl_ctx, mqp, wait_secs);

        } while((rv > 0) && 
                (0 != msg_type) && (msg_type != mqp->msg_type));

        return rv;
}

i32 mqtt_client_ctx_run(void *ctx, u32 wait_secs)
{
        i32 rv;

        if(NULL == ctx)
                return MQP_ERR_FNPARAM;

        do {
                rv = cl_ctx_recv(CL_CTX(ctx), NULL, wait_secs);

        } while(rv > 0);

        return rv;
}

static struct client_ctx *group_ctxs_ka_sequence(u32 wait_secs)
{
        struct client_ctx *cl_ctx = used_ctxs;

        while(cl_ctx) {
                struct client_ctx *next = cl_ctx->next;
                if(single_ctx_rx_prep(cl_ctx, &wait_secs) < 0) {
                        /* 'CTX' no more eligible for operation
                           and has been removed from used_list */
                        if(false == grp_has_cbfn)
                                return cl_ctx;
                }

                cl_ctx = next;
        }

        return NULL;
}

#define IO_MON_NO_TIMEOUT (0xFFFFFFFF)

static u32 group_ctxs_adj_wait_secs_get(u32 wait_secs)
{
        return  used_ctxs?
                single_ctx_adj_wait_secs_get(used_ctxs, wait_secs) : wait_secs;
}

static i32 recv_hvec[MAX_NWCONN + 1 + 1]; /* GROUP LISTEN PORT + VEC END */
static i32 send_hvec  = -1;
static i32 rsvd_hvec  = -1;

/* Caller must ensure atomic enviroment for execution of this routine */
static void recv_hvec_load(i32 *hvec_recv, u32 size, struct client_ctx *list)
{
        i32 i = 0;

        for(i = 0; (i < size) && (NULL != list); i++, list = list->next)
                hvec_recv[i] = list->net;

        hvec_recv[i] = -1;

        return;
}

static i32 group_ctxs_rx_prep(u32 wait_secs, void **app)
{
        /* CHK 'used ctx'(s) have live connection w/ server. If not, drop it */
        struct client_ctx *ctx_kaTO = group_ctxs_ka_sequence(wait_secs);
        i32 n_hnds;

        if(ctx_kaTO) {
                *app = CLIENT(ctx_kaTO)->app;
                return MQP_ERR_NETWORK;
        }

        conn2used_ctxs(wait_secs);   /* Now, add new 'ctx'(s) to 'used ctxs' */

        recv_hvec[0] = loopb_net;
        recv_hvec_load(&recv_hvec[1], MAX_NWCONN + 1, used_ctxs);

        wait_secs = group_ctxs_adj_wait_secs_get(wait_secs);

        n_hnds = net_ops->io_mon(recv_hvec, &send_hvec,
                                 &rsvd_hvec, wait_secs);
        if(0 == n_hnds)
                n_hnds = MQP_ERR_TIMEOUT;
        else if(n_hnds < 0)
                n_hnds = MQP_ERR_LIBQUIT;

        return n_hnds;
}

static i32 proc_loopback_recv(i32 net)
{
        u8 buf[LOOP_DLEN];

        /* Thanks for waking-up thread, but ain't got much to do now */
        i32 rv = net_ops->recv_from(net, buf, LOOP_DLEN, NULL, NULL, 0);
        if(rv <= 0) {
                net_ops->close(net);
                return MQP_ERR_LIBQUIT;
        }

        return rv;
}

static struct client_ctx *net_cl_ctx_find(i32 net)
{
        struct client_ctx *cl_ctx = used_ctxs;

        while(cl_ctx && (net != cl_ctx->net))
                cl_ctx = cl_ctx->next;

        return cl_ctx;
}

static i32 proc_net_data_recv(i32 net, struct mqtt_packet *mqp, void **app)
{
        /* Note: used_ctxs are always managed by a single RX task */
        struct client_ctx  *cl_ctx = net_cl_ctx_find(net);
        i32 rv = MQP_ERR_NOTCONN;

        if(NULL == cl_ctx)
                return rv; /* TX removed it interim, mustn't happen */

        return mqp_setup_proc_ctx_data_recv(cl_ctx, mqp, 1, app);
}

static i32 cl_recv(struct mqtt_packet *mqp, u32 wait_secs, void **app)
{
        i32 rv = MQP_ERR_NETWORK;
        i32 n_hnds = 0, idx = 0;

        rv = group_ctxs_rx_prep(wait_secs, app);
        if(rv > 0)
                n_hnds = rv;

        for(idx = 0; (idx < n_hnds) && (rv > 0); idx++) {
                i32 net = recv_hvec[idx];
                if(loopb_net == net)
                        rv = proc_loopback_recv(net); /* UDP Packet */
                else {
                        rv = proc_net_data_recv(net, mqp, app);
                        if(false == grp_has_cbfn)
                                break; /* 'CTX': inform application */
                }
        }

        return rv;
}

static i32 grp_net_setup_create()
{
        if(0 == loopb_portid)
                return MQP_ERR_NOT_DEF;

        if(NULL == net_ops)
                return MQP_ERR_NET_OPS;  

        if(-1 == loopb_net) {
                loopb_net = net_ops->open(DEV_NETCONN_OPT_UDP, NULL,
                                          loopb_portid, NULL);
                if(-1 == loopb_net)
                        return MQP_ERR_LIBQUIT;
               
        }

        return 1;
}

i32 mqtt_client_await_msg(struct mqtt_packet *mqp, u32 wait_secs, void **app)
{
        i32 rv = MQP_ERR_NOTCONN;
        *app = NULL;

        if(NULL == mqp)
                return MQP_ERR_FNPARAM; /* Didn't get a valid MQP */

        if(true == grp_has_cbfn)
                return MQP_ERR_BADCALL; /* Err: LIB has CB config */

        rv = grp_net_setup_create();
        if(rv <= 0)
                return rv;

        do {
                rv = cl_recv(mqp, wait_secs, app);

        } while((rv > 0) && MQP_RX_DO_NOT_RPT_COR(mqp));

        return rv;
}

i32 mqtt_client_run(u32 wait_secs)
{
        void *app = NULL;
        i32 rv = -1;

        if(false == grp_has_cbfn)
                return MQP_ERR_BADCALL; /* Err: LIB has no CB config */

        rv = grp_net_setup_create();
        if(rv <= 0)
                return rv;

        do {
                rv = cl_recv(NULL, wait_secs, &app);

        } while((rv > 0) ||
                /* 'ctx' related errors are handled by the callbacks */
                ((rv != MQP_ERR_LIBQUIT) && (rv != MQP_ERR_TIMEOUT)));

        return rv;
}

/*------------------------------------------------------------------------------
 * Buffer Pool and management, other registrations and initialization.
 *------------------------------------------------------------------------------
 */
static struct mqtt_packet *free_list = NULL;

static struct mqtt_packet *mqp_alloc_atomic(void)
{
        struct mqtt_packet *mqp = NULL;

        MUTEX_LOCKIN();
        mqp = free_list;
        if(mqp)
                free_list = mqp->next;
        MUTEX_UNLOCK();

        return mqp;
}

struct mqtt_packet *mqp_client_alloc(u8 msg_type, u8 offset)
{
        struct mqtt_packet *mqp = mqp_alloc_atomic();
        if(NULL == mqp) {
                USR_INFO("MQP alloc failed - msg type 0x%02x\n\r", msg_type);
                return NULL;
        }

        mqp_init(mqp, offset);
        mqp->msg_type = msg_type;

        return mqp;
}

/* Do not use this routine with-in this file. */
static void free_mqp(struct mqtt_packet *mqp)
{
        /* Must be used in a locked state */
        mqp->next = free_list;
        free_list = mqp;
}

i32 mqtt_client_buffers_register(u32 num_mqp, struct mqtt_packet *mqp_vec,
                                 u32 buf_len, u8 *buf_vec)
{
        u32 i, j;

        if((0 == num_mqp) || (0 == buf_len) || free_list)
                return -1;

        for(i = 0, j = 0; i < num_mqp; i++, j += buf_len) {
                struct mqtt_packet *mqp = mqp_vec + i;

                mqp->buffer = buf_vec + j;
                mqp->maxlen = buf_len;

                mqp->free   = free_mqp;
                mqp->next   = free_list;
                free_list   = mqp;
        }

        return 0;
}

i32 mqtt_client_ctx_will_register(void *ctx,
                                  const struct utf8_string  *will_top,
                                  const struct utf8_string  *will_msg,
                                  enum mqtt_qos will_qos, bool retain)
{
        u8 B = 0;

        if((NULL == ctx) || ((NULL == will_top) && (NULL != will_msg)))
                return -1;  /* Bad Combo */

        if(NULL != will_top) {
                RET_IF_INVALID_UTF8(will_top);

                B = QOS_VALUE(will_qos) << 3;
                if(retain)
                        B |= WILL_RETAIN_VAL;

                if(NULL != will_msg)
                        RET_IF_INVALID_UTF8(will_msg);
        }

        CLIENT(ctx)->conn_pl_utf8s[1] = will_top;
        CLIENT(ctx)->conn_pl_utf8s[2] = will_msg;

        CLIENT(ctx)->will_opts = B;

        return 0;
}

i32 mqtt_client_ctx_info_register(void *ctx,
                                  const struct utf8_string *client_id,
                                  const struct utf8_string *user_name,
                                  const struct utf8_string *pass_word)
{
        const struct utf8_string *users_pwd = NULL;

        if(NULL == ctx)
                return -1;

        if(NULL != client_id)
                RET_IF_INVALID_UTF8(client_id);

        if(NULL != user_name) {
                RET_IF_INVALID_UTF8(user_name);

                if(NULL != pass_word)
                        RET_IF_INVALID_UTF8(pass_word);

                users_pwd = pass_word;
        }

        CLIENT(ctx)->conn_pl_utf8s[0] = client_id;
        CLIENT(ctx)->conn_pl_utf8s[3] = user_name;
        CLIENT(ctx)->conn_pl_utf8s[4] = users_pwd;

        return 0;
}

i32 mqtt_client_net_svc_register(const struct device_net_services *net)
{
        if(net && net->open && net->send && net->recv  &&
           net->send_dest && net->recv_from && net->close
           && net->io_mon && net->time) {
                net_ops = net;
                return 0;
        }

        return -1;
}

static void cl_ctx_setup(struct client_ctx *cl_ctx, /* WR Object */
                         const struct mqtt_client_ctx_cfg *ctx_cfg,
                         const struct mqtt_client_ctx_cbs *ctx_cbs,
                         void *app)
{
        struct client_desc *client = CLIENT(cl_ctx);

        cl_ctx->flags       = ctx_cfg->config_opts;

        client->nwconn_opts = ctx_cfg->nwconn_opts;
        client->server_addr = ctx_cfg->server_addr;
        client->port_number = ctx_cfg->port_number;

        client->app         = app;


        if(NULL != ctx_cfg->nw_security)
                buf_wr_nbytes((u8*)&client->nw_security,
                              (u8*)ctx_cfg->nw_security,
                              sizeof(struct secure_conn));

        if(NULL != ctx_cbs) {
                // set callback flag
                struct mqtt_client_ctx_cbs *cbs_ctx = CTX_CBS_PTR(client);
                cbs_ctx->publish_rx = ctx_cbs->publish_rx;
                cbs_ctx->ack_notify = ctx_cbs->ack_notify;
                cbs_ctx->disconn_cb = ctx_cbs->disconn_cb;
        }

        return;
}

i32 mqtt_client_ctx_create(const struct mqtt_client_ctx_cfg *ctx_cfg,
                           const struct mqtt_client_ctx_cbs *ctx_cbs,
                           void *app, void **ctx)
{
        struct client_ctx *cl_ctx = NULL;

        if((NULL == ctx_cfg)                 ||
           (NULL == ctx_cfg->server_addr)    ||
           (0    == ctx_cfg->port_number))
                return -1;

        if(ctx_cfg->config_opts & MQTT_CFG_MK_GROUP_CTX)
                if(grp_has_cbfn ^ (!!ctx_cbs))
                        return -1;

        MUTEX_LOCKIN();
        if(free_ctxs) {
                cl_ctx       = free_ctxs;
                free_ctxs    = cl_ctx->next;
                cl_ctx->next = NULL;
        }
        MUTEX_UNLOCK();

        if(cl_ctx) {
                cl_ctx_setup(cl_ctx, ctx_cfg, ctx_cbs, app);
                *ctx = (void*) cl_ctx;
                return 0;
        }

        return -1;
}

i32 mqtt_client_ctx_delete(void *ctx)
{
        struct client_ctx  *cl_ctx = (struct client_ctx*) ctx;
        i32 rv = -1; /* Not sure about deletion as yet */

        MUTEX_LOCKIN();
        if((NULL == cl_ctx)          ||
           (-1   != cl_ctx->net)     ||
           (awaits_pkts(cl_ctx)))
                goto mqtt_client_ctx_delete_exit1;

        rv = 0; /* OK to delete ctx */
        client_reset(CLIENT(cl_ctx));
        cl_ctx_freeup(cl_ctx);

 mqtt_client_ctx_delete_exit1:
        MUTEX_UNLOCK();
        return rv;
}

i32 mqtt_client_lib_init(const struct mqtt_client_lib_cfg *lib_cfg)
{
        if((NULL == lib_cfg) || (NULL == lib_cfg->debug_printf))
                return -1;

        debug_printf = lib_cfg->debug_printf; /* Facilitate debug */

        if(INIT_DONE_STATE == cl_lib_state) {
                USR_INFO("C: Error trying to re-initialize \n\r");
                return -1;
        }

        USR_INFO("Version: Client LIB %s, Common LIB %s.\n\r",
                 MQTT_CLIENT_VERSTR, MQTT_COMMON_VERSTR);

        client_desc_init();

        cl_lib_state = INIT_DONE_STATE;

        loopb_portid = lib_cfg->loopback_port;
        grp_has_cbfn = lib_cfg->grp_uses_cbfn;

        mutex        = lib_cfg->mutex;
        mutex_lockin = lib_cfg->mutex_lockin;
        mutex_unlock = lib_cfg->mutex_unlock;

        aux_dbg_enbl = lib_cfg->aux_debug_en;

        return 0;
}

i32 mqtt_client_lib_exit()
{
        struct client_ctx *cl_ctx = free_ctxs;
        i32 count = 0;

        while(cl_ctx) {
                cl_ctx = cl_ctx->next;
                count++;
        }

        if(MAX_NWCONN == count) {
                cl_lib_state = WAIT_INIT_STATE;
                free_ctxs    = NULL;
                return 0;
        }

        return -1;
}
