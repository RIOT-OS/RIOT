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

#include "server_pkts.h"

/*-----------------------------------------------------------------------------
 * Note: Do not create additional dependency of this file on any header other
 * than server_pkts.h. Specifically, server_pkts.[hc] in conjunction with the
 * mqtt_common.[hc] files must be facilitated to create a stand-alone library.
 *-----------------------------------------------------------------------------
 */

static void  *mutex = NULL;
static void (*mutex_lockin)(void*) = NULL;
static void (*mutex_unlock)(void*) = NULL;

#define MUTEX_LOCKIN() if(mutex_lockin) mutex_lockin(mutex);
#define MUTEX_UNLOCK() if(mutex_unlock) mutex_unlock(mutex);

static bool aux_dbg_enbl = false;
static i32 (*debug_printf)(const c8 *fmt, ...) = NULL;

#define USR_INFO debug_printf
#define DBG_INFO(I, ...) if(aux_dbg_enbl) debug_printf(I, ##__VA_ARGS__)

static i32 mqp_buf_rd_utf8(const u8 *buf, const u8 *end,
                           struct utf8_string *utf8)
{
        const u8 *ref = buf; /* Reference */
        u16 len = 0;         /* UTF8 Size */

        if(end - buf < 2)
                return -1; /* No valid buffer to hold UTF8 size */

        buf += buf_rd_nbo_2B(buf, &len);
        if(end - buf < len)
                return -1; /* No valid buffer to hold UTF8 name */

        utf8->length = len;
        utf8->buffer = len? (c8*)buf : NULL;

        return buf + len - ref;
}

static struct mqtt_server_msg_cbs usr_obj, *usr_cbs = NULL;
static const struct device_net_services *net_ops = NULL;

#ifndef CFG_SR_MQTT_CTXS
#define MAX_NWCONN 6
#else
#define MAX_NWCONN CFG_SR_MQTT_CTXS
#endif

static struct client_ctx contexts[MAX_NWCONN];

static struct client_ctx *used_ctxs = NULL;
static struct client_ctx *free_ctxs = NULL;


#define NETWORK_CLOSE_FLAG         0x00200000
#define NW_CONN_ERROR_FLAG         0x00400000
#define RCVD_CONN_MSG_FLAG         0x00800000

#define NEED_NET_CLOSE(cl_ctx) (cl_ctx->flags & NETWORK_CLOSE_FLAG)

static void cl_ctx_init(void)
{
        i32 i = 0;
        for(i = 0; i < MAX_NWCONN; i++) {
                struct client_ctx *cl_ctx = contexts + i;

                cl_ctx_reset(cl_ctx);

                cl_ctx->next = free_ctxs;
                free_ctxs = cl_ctx;
        }
}

static void cl_ctx_free(struct client_ctx *cl_ctx)
{
        cl_ctx_reset(cl_ctx);

        cl_ctx->next = free_ctxs;
        free_ctxs = cl_ctx;

        return;
}

static struct client_ctx *cl_ctx_alloc(void)
{
        struct client_ctx *cl_ctx = free_ctxs;
        if(cl_ctx) {
                free_ctxs = cl_ctx->next;
                cl_ctx->next = NULL;
        } else
                USR_INFO("S: fatal, no free cl_ctx\n\r");

        return cl_ctx;
}

static inline bool had_rcvd_conn_msg(struct client_ctx *cl_ctx)
{
        return (cl_ctx->flags & RCVD_CONN_MSG_FLAG);
}

static inline void set_rcvd_conn_msg(struct client_ctx *cl_ctx)
{
        cl_ctx->flags |= RCVD_CONN_MSG_FLAG;
}

static void used_ctxs_insert(struct client_ctx *cl_ctx)
{
        cl_ctx_timeout_insert(&used_ctxs, cl_ctx);
}

static void used_ctxs_remove(struct client_ctx *cl_ctx)
{
        cl_ctx_remove(&used_ctxs, cl_ctx);
}

static i32 loopb_net        = -1;
static const u8 LOOP_DATA[] = {0x00, 0x01};
#define LOOP_DLEN sizeof(LOOP_DATA)
static u16  loopback_port   = 0;
static bool pending_trigs   = false;

static i32 loopb_trigger(void)
{
        u8 ip_addr[] = {127,0,0,1};
        i32 rv = 0;
        
        if((-1 != loopb_net) && (false == pending_trigs)) {
                rv = net_ops->send_dest(loopb_net, LOOP_DATA, LOOP_DLEN,
                                        loopback_port, ip_addr, 4);
                if(0 == rv)
                        pending_trigs = true;
        }

        return rv;
}

static void do_net_close_rx(struct client_ctx *cl_ctx, bool due2err)
{
        DBG_INFO("S: RX closing Net %d ...\n\r", (i32)cl_ctx->net);

        net_ops->close(cl_ctx->net);
        cl_ctx->net = -1;

        if(cl_ctx->usr)
                usr_cbs->on_cl_net_close(cl_ctx->usr, due2err);

        used_ctxs_remove(cl_ctx);
        cl_ctx_free(cl_ctx);
}

static void do_net_close_tx(struct client_ctx *cl_ctx, bool due2err)
{
        if(due2err)
                cl_ctx->flags |= NW_CONN_ERROR_FLAG;

        cl_ctx->flags |= NETWORK_CLOSE_FLAG;

        loopb_trigger();
}

static i32 cl_ctx_send(struct client_ctx *cl_ctx, u8 *buf, u32 len)
{
        i32 rv = net_ops->send(cl_ctx->net, buf, len, NULL);
        if(rv <= 0) {
                do_net_close_tx(cl_ctx, true);
                rv = MQP_ERR_NETWORK;
        }

        USR_INFO("S: FH-B1 0x%02x, len %u to net %d: %s\n\r",
                 *buf, len, cl_ctx->net, rv? "Sent" : "Fail");
        return rv;
}

static
i32 vh_msg_send(struct client_ctx *cl_ctx, u8 msg_type, enum mqtt_qos qos,
                bool has_vh, u16 vh_data)
{
        u8  buf[4];
        u32 len = 2;

        if(false == had_rcvd_conn_msg(cl_ctx))
                return MQP_ERR_NOTCONN;

        buf[0] = MAKE_FH_BYTE1(msg_type, MAKE_FH_FLAGS(false, qos, false));
        buf[1] = has_vh ? 2 : 0;

        if(has_vh)
                len += buf_wr_nbo_2B(buf + 2, vh_data);

        return cl_ctx_send(cl_ctx, buf, len);
}

static
i32 _mqtt_vh_msg_send(void *ctx_cl, u8 msg_type, enum mqtt_qos qos, bool has_vh,
                      u16 vh_data)
{
        struct client_ctx *cl_ctx = (struct client_ctx*) ctx_cl;

        return cl_ctx? vh_msg_send(ctx_cl, msg_type, qos,
                                   has_vh, vh_data) : -1;
}

i32  mqtt_vh_msg_send(void *ctx_cl, u8 msg_type, enum mqtt_qos qos, bool has_vh,
                      u16 vh_data)
{
        return _mqtt_vh_msg_send(ctx_cl, msg_type, qos, has_vh, vh_data);
}

i32 mqtt_vh_msg_send_locked(void *ctx_cl, u8 msg_type, enum mqtt_qos qos,
                            bool has_vh, u16 vh_data)
{
        i32 rv;

        MUTEX_LOCKIN();
        rv = _mqtt_vh_msg_send(ctx_cl, msg_type, qos, has_vh, vh_data);
        MUTEX_UNLOCK();

        return rv;
}

i32 mqtt_connack_send(void *ctx_cl, u8 *vh_buf)
{
        struct client_ctx *cl_ctx = (struct client_ctx *) ctx_cl;

        i32 rv = vh_msg_send(cl_ctx, MQTT_CONNACK, MQTT_QOS0,
                             true, (vh_buf[0] << 8) | vh_buf[1]);

        if((rv > 0) && (0x00 != vh_buf[1]))
                do_net_close_tx(cl_ctx, true);

        return rv;
}

static
i32 _mqtt_server_pub_dispatch(void *ctx_cl, struct mqtt_packet *mqp, bool dup)
{
        i32 rv = 0;
        u8 *buf = MQP_FHEADER_BUF(mqp);

        if(dup)
                *buf |= DUP_FLAG_VAL(true);

        rv = cl_ctx_send((struct client_ctx*)ctx_cl, buf, MQP_CONTENT_LEN(mqp));

        *buf &= ~DUP_FLAG_VAL(true);

        return rv;
}

i32 mqtt_server_pub_dispatch(void *ctx_cl, struct mqtt_packet *mqp, bool dup)
{
        return _mqtt_server_pub_dispatch(ctx_cl, mqp, dup);
}

i32 
mqtt_server_pub_dispatch_locked(void *ctx_cl, struct mqtt_packet *mqp, bool dup)
{
        i32 rv;

        MUTEX_LOCKIN();
        rv = _mqtt_server_pub_dispatch(ctx_cl, mqp, dup);
        MUTEX_UNLOCK();

        return rv;
}

#define MQP_MAX_TOPICS      16
#define MQP_SUBACK_PAY_OFS (MAX_FH_LEN + 2)

static i32 sub_ack_send(struct client_ctx *cl_ctx, u8 *buf, u8 pay_ofs,
                        u32 pay_len, u16 msg_id)
{
        u8 *ref = buf += MAX_FH_LEN;

        if(MQP_SUBACK_PAY_OFS != pay_ofs)
                return MQP_ERR_PKT_LEN;

        buf += buf_wr_nbo_2B(buf, msg_id);
        ref -= mqp_buf_tail_wr_remlen(ref - MAX_REMLEN_BYTES, 
                                      pay_len + buf - ref);

        ref -= 1;
        *ref = MAKE_FH_BYTE1(MQTT_SUBACK, 
                             MAKE_FH_FLAGS(false, MQTT_QOS0, false));

        return cl_ctx_send(cl_ctx, ref, pay_len + buf - ref);
}

static inline i32 unsub_ack_send(struct client_ctx *cl_ctx, u16 msg_id)
{
        return vh_msg_send(cl_ctx, MQTT_UNSUBACK, MQTT_QOS0, true, msg_id);
}

/*----------------------------------------------------------------------------
 * Receive Routines
 *----------------------------------------------------------------------------
 */

/* Candidate to be moved to mqtt_common.c file */
static bool mqp_proc_vh_msg_id_rx(struct mqtt_packet *mqp_raw)
{
        u8 *buf = MQP_VHEADER_BUF(mqp_raw);

        if(mqp_raw->pl_len < 2)
                return false;    /* Bytes for MSG ID not available */

        buf += buf_rd_nbo_2B(buf, &mqp_raw->msg_id);
        mqp_raw->vh_len += 2;
        mqp_raw->pl_len -= 2;

        return true;
}

#define BRK_IF_RD_ERR_UTF8(buf, end, utf8)      \
        if(rd_buf_utf8(buf, end, utf8) < 0)     \
                break;

static i32 buf_rd_utf8_qos(u8 *buf, u8 *end, struct utf8_strqos *utf8_qos)
{
        struct utf8_string utf8;
        u8 *ref = buf;

        buf += mqp_buf_rd_utf8(buf, end, &utf8);

        /* Assess that UTF8 has been read and QOS can be read */
        if((buf > ref) && (end > buf)) {
                utf8_qos->buffer = utf8.buffer;
                utf8_qos->length = utf8.length;
                utf8_qos->qosreq = (enum mqtt_qos)*buf++;

                return buf - ref;
        }

        return -1;
}

static bool _proc_sub_msg_rx(struct mqtt_packet *mqp_raw,
                             struct utf8_strqos *qos_topics, u32 *n_topics)
{
        u8 *buf, *end;
        u32 i = 0;

        if(false == mqp_proc_vh_msg_id_rx(mqp_raw))
                return false;   /* Problem in contents received from client */

        buf = MQP_PAYLOAD_BUF(mqp_raw);
        end = buf + mqp_raw->pl_len;

        for(i = 0; (i < *n_topics) && (buf < end); i++) {
                struct utf8_strqos *qos_top = qos_topics + i;
                i32 len = buf_rd_utf8_qos(buf, end, qos_top);
                if(len < 0)
                        break; /* Failed to read Topic */

                buf += len;
        }

        *n_topics = i;

        return ((0 == i) || (buf != end))? false : true;
}

static
bool proc_sub_msg_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        u32 n_topics = MQP_MAX_TOPICS;
        u16 msg_id;

        struct utf8_strqos qos_topics[MQP_MAX_TOPICS];
        u8 ack[MQP_MAX_TOPICS  +  MQP_SUBACK_PAY_OFS];

        if(false == _proc_sub_msg_rx(mqp_raw, qos_topics, &n_topics))
                return false;

        msg_id = mqp_raw->msg_id;

        /* All topics have been now put in array, pass-on info to upper layer */
        if(usr_cbs->sub_msg_rx(cl_ctx->usr, qos_topics, n_topics,
                               msg_id, ack + MQP_SUBACK_PAY_OFS)) {

                sub_ack_send(cl_ctx, ack, MQP_SUBACK_PAY_OFS, n_topics, msg_id);

                return true;
        }

        return false;
}


static bool _proc_unsub_msg_rx(struct mqtt_packet *mqp_raw,
                               struct utf8_string *topics, u32 *n_topics)
{
        u8 *buf, *end;
        u32 i = 0;
        
        if(false == mqp_proc_vh_msg_id_rx(mqp_raw))
                return false;   /* Problem in contents received from client */

        buf = MQP_PAYLOAD_BUF(mqp_raw);
        end = buf + mqp_raw->pl_len;

        for(i = 0; (i < *n_topics) && (buf < end); i++) {
                struct utf8_string *topic = topics + i;
                i32 len = mqp_buf_rd_utf8(buf, end, topic);
                if(len < 0)
                        break; /* Failed to read Topic */
                
                buf += len;
        }

        *n_topics = i;

        return ((0 == i) || (buf != end))? false : true;
}

static
bool proc_unsub_msg_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        u32 n_topics = MQP_MAX_TOPICS;
        u16 msg_id;

        struct utf8_string topics[MQP_MAX_TOPICS];

        if(false == _proc_unsub_msg_rx(mqp_raw, topics, &n_topics))
                return false;

        msg_id = mqp_raw->msg_id;

        /* All topics have been now put in array, pass-on info to upper layer */
        if(usr_cbs->un_sub_msg(cl_ctx->usr, topics, n_topics, msg_id)) {
                unsub_ack_send(cl_ctx, msg_id);
                return true;
        }

        return false;
}

static bool proc_pingreq_rx(struct client_ctx *cl_ctx)
{
        vh_msg_send(cl_ctx, MQTT_PINGRSP, MQTT_QOS0, false, 0x00);
        return true;
}

static bool proc_disconn_rx(struct client_ctx *cl_ctx)
{
        do_net_close_rx(cl_ctx, false);
        return true;
}

static
bool proc_pub_msg_rx(struct client_ctx  *cl_ctx, struct mqtt_packet *mqp_raw)
{
        bool rv = mqp_proc_pub_rx(mqp_raw);
        u8 B = mqp_raw->fh_byte1;
        enum mqtt_qos qos = ENUM_QOS(B);
        struct utf8_string topic;
        u16 msg_id = 0;

        if(false == rv)
                return rv; /* Didn't get a good PUB Packet */

        msg_id = mqp_raw->msg_id;

        topic.buffer = (c8*)MQP_PUB_TOP_BUF(mqp_raw);
        topic.length =      MQP_PUB_TOP_LEN(mqp_raw);

        rv = usr_cbs->pub_msg_rx(cl_ctx->usr, &topic, 
                                 MQP_PUB_PAY_BUF(mqp_raw),
                                 MQP_PUB_PAY_LEN(mqp_raw),
                                 msg_id, BOOL_DUP(B), qos, 
                                 BOOL_RETAIN(B));
        if(false == rv)
                return rv;

        if(MQTT_QOS1 == qos)
                vh_msg_send(cl_ctx, MQTT_PUBACK, MQTT_QOS0, true, msg_id);

        if(MQTT_QOS2 == qos)
                vh_msg_send(cl_ctx, MQTT_PUBREC, MQTT_QOS0, true, msg_id);

        return rv;
}

static 
bool proc_ack_msg_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        if((false == mqp_proc_msg_id_ack_rx(mqp_raw, false)) ||
           (false == usr_cbs->ack_notify(cl_ctx->usr,
                                         mqp_raw->msg_type,
                                         mqp_raw->msg_id)))
                return false;

        return true;
}

#define IO_MON_NO_TIMEOUT (0xFFFFFFFF) // TBD
//#define KA_TIMEOUT_NONE   (0xFFFFFFFF)

static void rx_timeout_update(struct client_ctx *cl_ctx)
{
        if(false == had_rcvd_conn_msg(cl_ctx))
                return;

        cl_ctx_timeout_update(cl_ctx, net_ops->time());

        used_ctxs_remove(cl_ctx);
        used_ctxs_insert(cl_ctx);

        return;
}

static bool proc_protocol_info(struct utf8_string *utf8, u8 ver)
{
        const c8 *buf = utf8->buffer;

        /* Check for protocol version 3.1.1 */
        if((4 == utf8->length)  &&
           (buf[0] == 'M')      &&
           (buf[1] == 'Q')      &&
           (buf[2] == 'T')      &&
           (buf[3] == 'T')      &&
           (0x04 == ver))
                return true;

        /* Check for protocol version 3.1   */
        if((6 == utf8->length)  &&
           (buf[0] == 'M')      &&
           (buf[1] == 'Q')      &&
           (buf[2] == 'I')      &&
           (buf[3] == 's')      &&
           (buf[4] == 'd')      &&
           (buf[5] == 'p')      &&
           (0x03 == ver))
                return true;

        return false;
}

static u16 proc_connect_vh_rx(struct mqtt_packet  *mqp_raw,
                              u8 *conn_flags, u16 *ka_secs)
{
        struct utf8_string utf8;
        u8 *buf = MQP_PAYLOAD_BUF(mqp_raw);
        u8 *end = buf + mqp_raw->pl_len;
        u8 *ref = buf;

        buf += mqp_buf_rd_utf8(buf, end, &utf8);
        if(end - buf < 1)
                return CONNACK_RC_BAD_PROTOV; /* No proto ver */

        if(false == proc_protocol_info(&utf8, *buf++))
                return CONNACK_RC_BAD_PROTOV;

        *conn_flags = *buf++;

        if(end - buf < 2)
                return 0xFF;        /* Bad packet composition */

        *ka_secs  = (buf[0] << 8) | buf[1];
        buf      += 2;
        *ka_secs += *ka_secs >> 1;

        mqp_raw->vh_len  = buf - ref;
        mqp_raw->pl_len -= buf - ref;

        return 0;
}

#define RET_IF_RD_CONN_ERROR(buf, end, utf8)                   \
        {                                                      \
                i32 len = mqp_buf_rd_utf8(buf, end, utf8);     \
                if(len < 0)                                    \
                        return 0x00FF;                         \
                                                               \
                buf += len;                                    \
        }

u16 proc_connect_pl_rx(const u8 *buf, const u8 *end, u8 conn_flags,
                       struct utf8_string *free_utf8s,
                       struct utf8_string **used_refs)
{
        struct utf8_string *utf8;

        utf8 = used_refs[0] = free_utf8s + 0;
        RET_IF_RD_CONN_ERROR(buf, end, utf8);

        if(conn_flags & WILL_CONFIG_VAL) {
                utf8 = used_refs[1] = free_utf8s + 1;
                RET_IF_RD_CONN_ERROR(buf, end, utf8);

                utf8 = used_refs[2] = free_utf8s + 2;
                RET_IF_RD_CONN_ERROR(buf, end, utf8);
        }

        if((0 == (conn_flags & USER_NAME_OPVAL))  &&
           (0 != (conn_flags & PASS_WORD_OPVAL)))
                return 0x00FF;  /* Bad combination */

        if(conn_flags & USER_NAME_OPVAL) {
                utf8 = used_refs[3] = free_utf8s + 3;
                RET_IF_RD_CONN_ERROR(buf, end, utf8);
        }

        if(conn_flags & PASS_WORD_OPVAL) {
                utf8 = used_refs[4] = free_utf8s + 4;
                RET_IF_RD_CONN_ERROR(buf, end, utf8);
        }

        return 0;
}

static
bool proc_connect_rx(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        struct utf8_string *used_refs[5] = {NULL, NULL, NULL, NULL, NULL};
        struct utf8_string free_utf8s[5];
        u8 conn_flags, *buf, *end;
        bool clean_session = false;
        u16 ack_vh16 = 0; /* Variable Header of CONNACK (response) Message */

        set_rcvd_conn_msg(cl_ctx);

        ack_vh16 = proc_connect_vh_rx(mqp_raw, &conn_flags, &cl_ctx->ka_secs);
        if(ack_vh16) 
                goto proc_connect_rx_exit1;

        buf = MQP_PAYLOAD_BUF(mqp_raw);
        end = buf + mqp_raw->pl_len;
        ack_vh16 = proc_connect_pl_rx(buf,  end, conn_flags,
                                      free_utf8s, used_refs);
        if(ack_vh16)
                goto proc_connect_rx_exit1;

        clean_session = (conn_flags & CLEAN_START_VAL)? true : false;
        ack_vh16 = (!used_refs[0]->length && !clean_session)?
                   CONNACK_RC_CLI_REJECT : 0;   /* Validate 0 byte Client ID */
        if(ack_vh16)
                goto proc_connect_rx_exit1;

        /* UTF8 Order: Client ID, Will Topic, Will Msg, User Name, Pass Word */
        ack_vh16 = usr_cbs->connect_rx(cl_ctx, conn_flags, &used_refs[0],
                                       &cl_ctx->usr);
 proc_connect_rx_exit1:

        DBG_INFO("S: CONNACK RC (16bits) is %u (%s)\n\r", ack_vh16,
                 ack_vh16 & 0xFF? "error" : "good");

        if(0xFF != (ack_vh16 & 0xFF))
                vh_msg_send(cl_ctx, MQTT_CONNACK, MQTT_QOS0, true, ack_vh16);

        if(CONNACK_RC_REQ_ACCEPT == (ack_vh16 & 0xFF)) {
                rx_timeout_update(cl_ctx);
                usr_cbs->on_connack_send(cl_ctx->usr, clean_session);
        } else {
                return false;
        }

        return true;
}


static void recv_hvec_load(i32 *recv_hvec, u32 size, struct client_ctx *list)
{
        i32 i = 0;

        for(i = 0; (i < size) && (NULL != list); i++, list = list->next)
                recv_hvec[i] = list->net;

        recv_hvec[i] = -1;

        return;
}

static bool process_recv(struct client_ctx *cl_ctx, struct mqtt_packet *mqp_raw)
{
        u8 msg_type = mqp_raw->msg_type;
        bool rv = false;

        USR_INFO("S: Rcvd msg Fix-Hdr (Byte1) 0x%02x from net %d\n\r",
                 mqp_raw->fh_byte1, cl_ctx->net);

        if((MQTT_CONNECT != msg_type) ^ had_rcvd_conn_msg(cl_ctx))
                goto process_recv_exit1; /* Proto Violation */

        rx_timeout_update(cl_ctx);

        switch(msg_type) {

        case MQTT_CONNECT:
                rv = proc_connect_rx(cl_ctx, mqp_raw);
                break;

        case MQTT_DISCONNECT:
                rv = proc_disconn_rx(cl_ctx);
                break;

        case MQTT_SUBSCRIBE:
                rv = proc_sub_msg_rx(cl_ctx, mqp_raw);
                break;

        case MQTT_UNSUBSCRIBE:
                rv = proc_unsub_msg_rx(cl_ctx, mqp_raw);
                break;

        case MQTT_PINGREQ:
                rv = proc_pingreq_rx(cl_ctx);
                break;

        case MQTT_PUBLISH:
                rv = proc_pub_msg_rx(cl_ctx, mqp_raw);
                break;

        case MQTT_PUBACK:
        case MQTT_PUBREC:
        case MQTT_PUBREL:
        case MQTT_PUBCOMP:
                rv = proc_ack_msg_rx(cl_ctx, mqp_raw);
                break;

        default:
                break;
        }

 process_recv_exit1:
        DBG_INFO("S: Processing of MSG ID 0x%02x: %s\n\r",
                 mqp_raw->msg_id, rv? "Good" : "Fail");

        return rv;
}


/* Terminate net connections which haven't received PKTs beyond expected time.
   Caller must ensure atomic enviroment for execution of this routine.
*/
static void ka_sequence(u32 *secs2wait)
{
        struct client_ctx *cl_ctx = used_ctxs; /* Sorted for timeout (ascend) */
        u32 now_secs = net_ops->time();

        while(NULL != cl_ctx) {
                struct client_ctx *next = cl_ctx->next;
                if(NEED_NET_CLOSE(cl_ctx) || !(cl_ctx->timeout > now_secs)) {
                        bool due2err = false;
                        if(cl_ctx->flags & NW_CONN_ERROR_FLAG)
                                due2err = true;

                        cl_ctx->flags &= ~(NW_CONN_ERROR_FLAG |
                                           NETWORK_CLOSE_FLAG);

                        /* Close network: Timeout or TX err */
                        do_net_close_rx(cl_ctx, due2err);
                }

                cl_ctx = next;
        }

        cl_ctx = used_ctxs;
        if(((NULL != cl_ctx) && (KA_TIMEOUT_NONE == cl_ctx->timeout)) ||
           ((NULL == cl_ctx)))
                *secs2wait = IO_MON_NO_TIMEOUT;
        else
                *secs2wait = cl_ctx->timeout - now_secs;

        return;
}

/* Put a new functiona name such as mk_new_ctx() or setup_ctx() and
   processing to restrict limit number of connections.

   Return value as well.
*/
static bool accept_ctx(i32 net, u32 wait_secs)
{
        struct client_ctx *cl_ctx = cl_ctx_alloc();
        if(NULL == cl_ctx)
                return false;

        cl_ctx->net = net_ops->accept(net, cl_ctx->remote_ip,
                                      &cl_ctx->ip_length);
        if(-1 == cl_ctx->net) {
                cl_ctx_free(cl_ctx);
                USR_INFO("S: failed to accept new NW connection\n\r");
                return false;
        }

        DBG_INFO("Accepted new connection (fd) %d\n\r", (i32)cl_ctx->net);

        /* Timeout to receive MQTT_CONNECT */
        cl_ctx->timeout = wait_secs + net_ops->time(); 

        used_ctxs_insert(cl_ctx);
        return true;
}

static struct client_ctx *net_cl_ctx_find(i32 net)
{
        struct client_ctx *cl_ctx = used_ctxs;
        while(cl_ctx) {
                if(net == cl_ctx->net)
                        break;

                cl_ctx = cl_ctx->next;
        }

        if(NULL == cl_ctx)
                USR_INFO("Did not find ctx for net %d\n\r", net); 

        return cl_ctx;
}

static i32 recv_hvec[MAX_NWCONN + 1 + 1 + 1]; /* LISTEN + LOOPBACK + VEC END */
static i32 send_hvec  = -1;
static i32 rsvd_hvec  = -1;
static i32 listen_net = -1;

static struct mqtt_packet rx_mqp;
static u8 rxb[MQP_SERVER_RX_LEN];
static u16 listener_port = 0;

static inline
i32 net_recv(i32 net, struct mqtt_packet *mqp, u32 wait_secs, bool *timed_out)
{
        i32 rv = mqp_recv(net, net_ops, mqp, wait_secs, timed_out, NULL);
        if(rv <= 0)
                rv = MQP_ERR_NETWORK;

        return rv;
}

static i32 proc_loopback_recv(i32 net)
{
        u8 buf[LOOP_DLEN];

        /* Thanks for waking-up thread and do nothing in this routine */
        i32 rv = net_ops->recv_from(net, buf, LOOP_DLEN, NULL, NULL, 0);
        pending_trigs = false;

        if(rv <= 0) {
                net_ops->close(net);
                return MQP_ERR_LIBQUIT;
        }

        return rv;
}

static void proc_net_data_recv(i32 net)
{
        struct client_ctx *cl_ctx = net_cl_ctx_find(net);
        bool dummy;
        i32 rv;

        mqp_reset(&rx_mqp); /* Start w/ a clean buffer */

        rv = net_recv(net, &rx_mqp, 0, &dummy);
        if(rv > 0)
                /* Working Principle: Only RX processing errors should be
                   reported as 'false'. Status of TX as a follow-up to RX
                   messages need not be reported by the xyz_rx() routines.
                   Error observed in TX is either dealt in next iteration
                   of RX loop.
                */
                if(false == process_recv(cl_ctx, &rx_mqp))
                        rv = MQP_ERR_CONTENT;

        if(rv < 0)
                do_net_close_rx(cl_ctx, rv);
}

static bool accept_and_recv_locked(i32 *recv_hnds, i32 n_hnds, u32 wait_secs)
{
        bool rv = true;
        i32 idx = 0;

        MUTEX_LOCKIN();

        for(idx = 0; (idx < n_hnds) && (rv == true); idx++) {
                i32 net = recv_hvec[idx];
                if(net == listen_net) {
                        rv = accept_ctx(listen_net, wait_secs);
                } else if(loopback_port && (net == loopb_net)) {
                        if(proc_loopback_recv(loopb_net) < 0)
                                rv = false;
                } else {
                        proc_net_data_recv(net);
                }
        }

        MUTEX_UNLOCK();

        return rv;
}

i32 mqtt_server_run(u32 wait_secs)   // TBD break into two functions
{
        u32 secs2wait = 0;
        i32 n_hnds = 0;

        USR_INFO("S: MQTT Server Run invoked ....\n\r");

        if(NULL == net_ops)
                return MQP_ERR_NET_OPS;

        if(loopback_port) {
                loopb_net  = net_ops->open(DEV_NETCONN_OPT_UDP, NULL, 
                                           loopback_port, NULL);
                if(-1 == loopb_net)
                        return MQP_ERR_LIBQUIT;
        }

        listen_net = net_ops->listen(0, listener_port, NULL);
        if(-1 == listen_net)
                return MQP_ERR_LIBQUIT;

        do {
                i32 *r_hvec = recv_hvec + 0;

                *r_hvec++ = listen_net;
                if(loopback_port)
                        *r_hvec++ = loopb_net;

                /* MQTT Timeouts: close expired conns; get time to next expiry */
                ka_sequence(&secs2wait); 

                /* Prepare array of net handles. Must've atleast listen handle */
                //                recv_hvec_load(&recv_hvec[2], MAX_NWCONN + 1,  used_ctxs);
                recv_hvec_load(r_hvec, MAX_NWCONN + 1,  used_ctxs);

                n_hnds = net_ops->io_mon(recv_hvec, &send_hvec,
                                         &rsvd_hvec, secs2wait);
                if(n_hnds < 0)
                        return MQP_ERR_LIBQUIT;

                if(false == accept_and_recv_locked(recv_hvec, n_hnds, wait_secs))
                        return MQP_ERR_LIBQUIT;

        } while(1);
}

i32 mqtt_server_register_net_svc(const struct device_net_services *net)
{
        if(net && net->io_mon && net->close && net->send &&
           net->recv && net->time && net->listen) {
                net_ops = net;
                return 0;
        }

        return -1;
}

i32 mqtt_server_lib_init(const struct mqtt_server_lib_cfg *lib_cfg,
                         const struct mqtt_server_msg_cbs *msg_cbs)
{
        cl_ctx_init();

        if((NULL == lib_cfg)                ||
           (0    == lib_cfg->listener_port) ||
           (NULL == lib_cfg->debug_printf))
                return -1;

        debug_printf = lib_cfg->debug_printf; /* Facilitate debug */

        loopback_port = lib_cfg->loopback_port;
        listener_port = lib_cfg->listener_port;

        mutex         = lib_cfg->mutex;
        mutex_lockin  = lib_cfg->mutex_lockin;
        mutex_unlock  = lib_cfg->mutex_unlock;

        aux_dbg_enbl  = lib_cfg->aux_debug_en;
        debug_printf  = lib_cfg->debug_printf;

        usr_cbs = &usr_obj;

        memcpy(usr_cbs, msg_cbs, sizeof(struct mqtt_server_msg_cbs));

        mqp_buffer_attach(&rx_mqp, rxb, MQP_SERVER_RX_LEN, 0);

        return 0;
}
