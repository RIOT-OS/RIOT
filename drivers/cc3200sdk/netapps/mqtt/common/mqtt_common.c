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
   mqtt_common.c

   This module implements routines that are common to both client and server
   components.
*/

#include "mqtt_common.h"

static void free_mqp(struct mqtt_packet *mqp)
{
        if((NULL != mqp->free) && (0 == --mqp->n_refs))
                mqp->free(mqp);

        return;
}

void mqp_free(struct mqtt_packet *mqp)
{
        free_mqp(mqp);
}

static void reset_mqp(struct mqtt_packet *mqp)
{
        /* Fields not handled here are meant to be left unaltered. */
        mqp->msg_type = 0x00;
        mqp->fh_byte1 = 0x00;
        mqp->msg_id   = 0;
        mqp->fh_len   = 0x00;
        mqp->vh_len   = 0;
        mqp->pl_len   = 0;
        mqp->private  = 0;
}

void mqp_reset(struct mqtt_packet *mqp)
{
        reset_mqp(mqp);
}

void mqp_init(struct mqtt_packet *mqp, u8 offset)
{
        reset_mqp(mqp);

        mqp->offset = offset;
        mqp->n_refs = 0x01;
        mqp->next   = NULL;
}

static i32 buf_wr_utf8(u8 *buf, const struct utf8_string *utf8)
{
        u8 *ref = buf;

        buf += buf_wr_nbo_2B(buf,      utf8->length);
        buf += buf_wr_nbytes(buf, (u8*)utf8->buffer, utf8->length);

        return buf - ref;
}

i32 mqp_buf_wr_utf8(u8 *buf, const struct utf8_string *utf8)
{
        return buf_wr_utf8(buf, utf8);
}

#define MAX_REMLEN_BYTES  (MAX_FH_LEN - 1)

static i32 buf_tail_wr_remlen(u8 *buf, u32 remlen)
{
        u8 val[MAX_REMLEN_BYTES], i = 0;

        do {
                val[i] = remlen & 0x7F; /* MOD 128 */
                remlen = remlen >> 7;   /* DIV 128 */

                if(remlen)
                        val[i] |= 0x80;

                i++;

        } while(remlen > 0);

        buf_wr_nbytes(buf + MAX_REMLEN_BYTES - i, val, i);

        return i;       /* # bytes written in buf */
}

i32 mqp_buf_tail_wr_remlen(u8 *buf, u32 remlen)
{
        return buf_tail_wr_remlen(buf, remlen);
}

static i32 buf_rd_remlen(u8 *buf, u32 *remlen)
{
        u32 val = 0, mul = 0;
        i32 i = 0;

        do {
                val += (buf[i] & 0x7F) << mul;
                mul += 7;

        } while((buf[i++] & 0x80)       &&
                (i < MAX_REMLEN_BYTES));

        *remlen = val;

        /* Return -1 if value was not found */
        return (buf[i - 1] & 0x80)? -1 : i;
}

i32 mqp_buf_rd_remlen(u8 *buf, u32 *remlen)
{
        return buf_rd_remlen(buf, remlen);
}

i32
mqp_pub_append_topic(struct mqtt_packet *mqp, const struct utf8_string *topic,
                     u16 msg_id)
{
        u8 *buf = MQP_VHEADER_BUF(mqp), *ref = buf;

        if(0 != mqp->vh_len) 
                return -1; /* Topic has been already added */

        if(MQP_FREEBUF_LEN(mqp) < (topic + msg_id? 2 : 0))
                return MQP_ERR_PKT_LEN; /* Can't WR topic */

        buf += buf_wr_utf8(buf, topic);

        if(0 != msg_id) {  /* MSG ID 0 indicates ==> QoS0 */
                mqp->msg_id = msg_id;
                buf += buf_wr_nbo_2B(buf, mqp->msg_id);
        }

        mqp->vh_len += buf - ref;

        return buf - ref;
}

i32 mqp_pub_append_data(struct mqtt_packet *mqp, const u8 *data_buf,
                        u32 data_len)
{
        u8 *buf = MQP_PAYLOAD_BUF(mqp) + mqp->pl_len, *ref = buf;

        if(0 == mqp->vh_len)
                return -1;    /* Must include topic first */

        if(MQP_FREEBUF_LEN(mqp) < data_len)
                return MQP_ERR_PKT_LEN; /* Can't WR  data */

        /* Including payload info for PUBLISH */
        buf += buf_wr_nbytes(buf, data_buf, data_len);
        mqp->pl_len += buf - ref;

        return buf - ref;
}

static bool proc_vh_msg_id_rx(struct mqtt_packet *mqp_raw)
{
        u8 *buf = MQP_VHEADER_BUF(mqp_raw);

        if(mqp_raw->pl_len < 2)
                return false;    /* Bytes for MSG ID not available */

        buf += buf_rd_nbo_2B(buf, &mqp_raw->msg_id);
        mqp_raw->vh_len += 2;
        mqp_raw->pl_len -= 2;

        return true;
}

bool mqp_proc_vh_msg_id_rx(struct mqtt_packet *mqp_raw)
{
        return proc_vh_msg_id_rx(mqp_raw);
}

bool mqp_proc_msg_id_ack_rx(struct mqtt_packet *mqp_raw, bool has_pl)
{
        if((false == proc_vh_msg_id_rx(mqp_raw)) ||
           (has_pl ^ (!!mqp_raw->pl_len)))
                return false;

        return true;
}

bool mqp_proc_pub_rx(struct mqtt_packet *mqp_raw)
{
        u8 *buf = MQP_VHEADER_BUF(mqp_raw), *ref = buf;
        u16 tmp = 0;

        if(mqp_raw->pl_len < (buf - ref + 2))    /* Length Check  */
                return false;      /* No space to hold Topic size */

        buf += buf_rd_nbo_2B(buf, &tmp);         /* Topic  Length */
        buf += tmp;                              /* Topic Content */

        if(MQTT_QOS0 != ENUM_QOS(mqp_raw->fh_byte1)) {
                if(mqp_raw->pl_len < (buf - ref + 2))
                        return false;  /* No space to hold MSG ID */

                buf += buf_rd_nbo_2B(buf, &mqp_raw->msg_id);
        }

        mqp_raw->vh_len += buf - ref;
        mqp_raw->pl_len -= buf - ref;

        return true;
}

bool mqp_ack_wlist_append(struct mqtt_ack_wlist *list,
                          struct mqtt_packet    *elem)
{
        elem->next = NULL;

        if(list->tail) {
                list->tail->next = elem;
                list->tail = elem;
        } else {
                list->tail = elem;
                list->head = elem;
        }

        return true;
}

struct mqtt_packet *mqp_ack_wlist_remove(struct mqtt_ack_wlist *list,
                                          u16 msg_id)
{
        struct mqtt_packet *elem = list->head, *prev = NULL;

        while(elem) {
                if(msg_id == elem->msg_id) {
                        if(prev)
                                prev->next = elem->next;
                        else
                                list->head = elem->next;

                        if(NULL == list->head)
                                list->tail = NULL;

                        if(list->tail == elem)
                                list->tail = prev;

                        break;
                }

                prev = elem;
                elem = elem->next;
        }

        return elem;
}

void mqp_ack_wlist_purge(struct mqtt_ack_wlist *list)
{
        struct mqtt_packet *elem = list->head;

        while(elem) {
                struct mqtt_packet *next = elem->next;
                free_mqp(elem);
                elem = next;
        }

        list->head = NULL;
        list->tail = NULL;

        return;
}

void secure_conn_struct_init(struct secure_conn *nw_security)
{
        nw_security->method = nw_security->cipher = NULL; 
        nw_security->files  = NULL;
        nw_security->n_file = 0;

        return;
}

i32 mqp_prep_fh(struct mqtt_packet *mqp, u8 flags)
{
        u32 remlen = mqp->vh_len + mqp->pl_len;
        u8 *buf    = mqp->buffer + mqp->offset;
        u8 *ref    = buf;

        buf -= buf_tail_wr_remlen(buf - MAX_REMLEN_BYTES, remlen);

        buf -= 1; /* Make space for FH Byte1 */        
        mqp->fh_byte1 = *buf = MAKE_FH_BYTE1(mqp->msg_type, flags);

        mqp->fh_len   = ref - buf;
        mqp->offset  -= ref - buf;

        return ref - buf;
}

/*
    Since, the network connection is a TCP socket stream, it is imperative that
    adequate checks are put in place to identify a MQTT packet and isolate it
    for further processing. The intent of following routine is to read just one
    packet from continuous stream and leave rest for the next iteration.
*/

#define RET_IF_ERR_IN_NET_RECV(net, buf, len, wait_secs, timed_out, ctx)\
        rv = net_ops->recv(net, buf, len, wait_secs, timed_out, ctx);   \
        if(rv < 1)                                                      \
                return MQP_ERR_NETWORK;

i32 mqp_recv(i32  net,     const struct device_net_services *net_ops,
             struct mqtt_packet *mqp, u32 wait_secs, bool *timed_out,
             void *ctx)
{
        u8 *buf = MQP_FHEADER_BUF(mqp), *ref = buf, fh_len = mqp->fh_len;
        u32 pl_len = mqp->pl_len, remlen = 0;
        i32 rv;

        while(2 > fh_len) {
                RET_IF_ERR_IN_NET_RECV(net, buf + fh_len, 1, wait_secs,
                                       timed_out, ctx);

                mqp->fh_len = ++fh_len;
        }

        while(buf[fh_len - 1] & 0x80) {
                if(fh_len > MAX_FH_LEN)
                        return MQP_ERR_NOT_DEF;/* Shouldn't happen */
                
                 RET_IF_ERR_IN_NET_RECV(net, buf + fh_len, 1, wait_secs,
                                        timed_out, ctx);

                 mqp->fh_len = ++fh_len;
        }

        mqp_buf_rd_remlen(buf + 1, &remlen);
        if(mqp->maxlen < (remlen + fh_len))
                return MQP_ERR_PKT_LEN;  /* Inadequate free buffer */

        buf += fh_len;     /* Try to read all data that follows FH */
        while(pl_len < remlen) {
               RET_IF_ERR_IN_NET_RECV(net, buf + pl_len, remlen - pl_len,
                                      wait_secs, timed_out, ctx);

               mqp->pl_len = pl_len += rv;
        }

        /* Set up MQTT Packet for received data from broker */
        buf_wr_nbytes(MQP_FHEADER_BUF(mqp), ref, fh_len);
        mqp->fh_byte1 = *ref;
        mqp->msg_type = MSG_TYPE(*ref);

        return fh_len + remlen;
}

/*----------------------------------------------------------------------------
 * QoS2 PUB RX Message handling mechanism and associated house-keeping
 *--------------------------------------------------------------------------*/
void qos2_pub_cq_reset(struct pub_qos2_cq *cq)
{
        //        memset(cq->id_vec, 0, sizeof(cq->id_vec));
        buf_set((u8*)cq->id_vec, 0, sizeof(cq->id_vec));
        cq->n_free = MAX_PUBREL_INFLT;
        cq->rd_idx = 0;
        cq->wr_idx = 0;

        return;
}

bool qos2_pub_cq_logup(struct pub_qos2_cq *cq, u16 msg_id)
{
        if(0 != cq->n_free) {
                cq->id_vec[cq->wr_idx++]  = msg_id;
                cq->wr_idx &= MAX_PUBREL_INFLT - 1;
                cq->n_free--;
                return true;
        }

        /* Must ensure through an out-of-band arrangement that a remote doesn't
           push more than MAX_PUBREL_INFLT in-flight QOS2 PUB packets to local.
           If it still happens that the local receives more in-flight messages,
           then it has no option but to close the network connection.

           Closing of network connection is left to invoker to of this service.
        */

        return false;
}

bool qos2_pub_cq_unlog(struct pub_qos2_cq *cq, u16 msg_id)
{
        /* Simple implementation. Should be good for embedded system */
        if(cq->n_free < MAX_PUBREL_INFLT) {
                /* Follow-up messages for QOS2 PUB must be transacted in the
                   same order as the initial sequence of QOS2 PUB dispatches.
                   Therefore, checking the first entry should be OK
                */
                if(msg_id == cq->id_vec[cq->rd_idx++]) {
                        cq->rd_idx &= MAX_PUBREL_INFLT - 1;
                        cq->n_free++;
                        return true;
                }
        }

        return false;
}

bool qos2_pub_cq_check(struct pub_qos2_cq *cq, u16 msg_id)
{
        u8 rd_idx = cq->rd_idx;
        u8 n_free = cq->n_free;
        u8 i = 0;

        /* Go through the vector to see if packet ID is still active */
        for(i = 0; i < (MAX_PUBREL_INFLT - n_free); i++) {
                    if(msg_id == cq->id_vec[rd_idx++]) 
                            return true;

                    rd_idx &= MAX_PUBREL_INFLT - 1;
        }

        return false;
}

/*----------------------------------------------------------------------------
 * Client Context related details and associated house-keeping
 *--------------------------------------------------------------------------*/
void cl_ctx_reset(struct client_ctx *cl_ctx)
{
        cl_ctx->usr       = NULL;
        cl_ctx->net       = -1;
        cl_ctx->ip_length = 16;

        cl_ctx->timeout   = 0;
        cl_ctx->ka_secs   = 0;

        cl_ctx->flags     = 0;

        return;
}

void cl_ctx_timeout_insert(struct client_ctx **head, struct client_ctx *elem)
{
        struct client_ctx *curr, *prev = NULL;

        if(NULL == *head) {
                *head = elem;
                return;
        }

        curr = *head;
        while(curr) {
                if(elem->timeout < curr->timeout) {
                        if(NULL == prev) {
                                elem->next = *head;
                                *head = elem;
                        } else {
                                prev->next = elem;
                                elem->next = curr;
                        }

                        break;
                }
                
                prev = curr;
                curr = curr->next;
        }

        if(NULL == curr)
                prev->next = elem;
}

void cl_ctx_remove(struct client_ctx **head, struct client_ctx *elem)
{
        struct client_ctx *curr = *head, *prev = NULL;

        while(curr) {
                if(curr == elem) {
                        if(NULL == prev) {
                                *head = elem->next;
                                prev  = *head;
                        } else {
                                prev->next = elem->next;
                        }

                        elem->next = NULL;
                        break;
                }

                prev = curr;
                curr = curr->next;
        }
}

void cl_ctx_timeout_update(struct client_ctx *cl_ctx, u32 now_secs)
{
        u32 timeout = KA_TIMEOUT_NONE;
        u16 ka_secs = cl_ctx->ka_secs;

        if((0 == ka_secs) && (KA_TIMEOUT_NONE == cl_ctx->timeout))
                return;

        if(0 != ka_secs)
                timeout = now_secs + ka_secs;

        cl_ctx->timeout = timeout;

        return;
}
