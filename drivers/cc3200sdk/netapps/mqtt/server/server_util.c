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

static u16 msg_id = 0xFFFF;
static inline u16 assign_new_msg_id()
{
        return msg_id += 2;
}

u16 mqp_new_id_server(void)
{
        return assign_new_msg_id();
}

static void my_pkt_free(struct mqtt_packet *mqp)
{
        my_free((void*) mqp);
}

/*----------------------------------------------------------------------------
 * Try to prevent fragmentation by consistently allocating a fixed memory size
 *----------------------------------------------------------------------------
 */
#ifndef CFG_SR_MAX_MQP_TX_LEN
#define MQP_SERVER_TX_LEN        1024
#else
#define MQP_SERVER_TX_LEN        CFG_SR_MAX_MQP_TX_LEN
#endif

/*------------------ Fixed memory configuration ends -----------------------*/

static struct mqtt_packet *server_mqp_alloc(u8 msg_type, u32 buf_sz, u8 offset)
{
        u32 mqp_sz = sizeof(struct mqtt_packet);
        struct mqtt_packet *mqp = NULL;

        buf_sz += offset;

        if((mqp_sz + buf_sz) > MQP_SERVER_TX_LEN) {
                USR_INFO("S: fatal, buf alloc len > MQP_SERVER_TX_LEN\n\r");
                return NULL;
        }

        mqp = my_malloc(MQP_SERVER_TX_LEN);
        if(NULL != mqp) {
                mqp_init(mqp, offset);

                mqp->msg_type = msg_type;
                mqp->maxlen   = buf_sz;
                mqp->buffer   = (u8*)mqp + mqp_sz;

                mqp->free     = my_pkt_free;

        } else {
                USR_INFO("S: fatal, failed to alloc Server MQP\n\r");
        }

        return mqp;
}

struct mqtt_packet *mqp_server_alloc(u8 msg_type, u32 buf_sz)
{
        return server_mqp_alloc(msg_type, buf_sz, MAX_FH_LEN);
}

struct mqtt_packet *mqp_server_copy(const struct mqtt_packet *mqp)
{
        struct mqtt_packet *cpy = server_mqp_alloc(mqp->msg_type,
                                                   mqp->maxlen, 0);
        if(NULL != cpy) {
                u8 *buffer  = cpy->buffer;

                /* Copy to overwrite everything in 'cpy' from source 'mqp' */
                buf_wr_nbytes((u8*)cpy, (u8*)mqp, sizeof(struct mqtt_packet));

                cpy->buffer = buffer; /* Restore buffer and copy */
                buf_wr_nbytes(cpy->buffer, mqp->buffer, mqp->maxlen);
        }

        return cpy;
}

i32  (*util_dbg_prn)(const c8 *fmt, ...) = NULL;
bool   util_prn_aux = false;

static void (*lockin_mutex)(void*) = NULL;
static void (*unlock_mutex)(void*) = NULL;
static void  *mutex_hnd = NULL;

void util_mutex_lockin(void)
{
        if(lockin_mutex)
                lockin_mutex(mutex_hnd);

        return;
}

void util_mutex_unlock(void)
{
        if(unlock_mutex)
                unlock_mutex(mutex_hnd);

        return;
}

void util_params_set(i32  (*dbg_prn)(const c8 *fmt, ...),
                     void  *mutex,
                     void (*mutex_lockin)(void*),
                     void (*mutex_unlock)(void*))
{
        util_dbg_prn = dbg_prn;
        mutex_hnd    = mutex;
        lockin_mutex = mutex_lockin;
        unlock_mutex = mutex_unlock;

        return;
}
