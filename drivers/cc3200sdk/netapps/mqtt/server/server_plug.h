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

#ifndef __SERVER_PLUG_H__
#define __SERVER_PLUG_H__

#include "server_pkts.h"

/* Used by Server Core Logic */

#define PG_MAP_BITS_SIZE     2
#define PG_MAP_BITS_MASK     ((1 << PG_MAP_BITS_SIZE) - 1)
#define PG_MAP_MAX_ELEMS     4  /* should be able accomodate in 1 byte */
#define PG_MAP_ALL_DFLTS     ((1 << (PG_MAP_BITS_SIZE * PG_MAP_MAX_ELEMS)) - 1)

#define PG_MAP_HAS_VALUE(pg_map, index)                 \
        (((~pg_map) >> (index * PG_MAP_BITS_SIZE)) & PG_MAP_BITS_MASK)

#define PG_MAP_VAL_SETUP(pg_map, qid, index)                            \
        {                                                               \
                u32 ofst = index * PG_MAP_BITS_SIZE;                    \
                pg_map  &= ~(PG_MAP_BITS_MASK << ofst);                 \
                pg_map  |= qid << ofst;                                 \
        }

#define PG_MAP_VAL_RESET(pg_map, index)                                 \
        pg_map |= PG_MAP_BITS_MASK << (index * PG_MAP_BITS_SIZE);

#if (PG_MAP_BITS_MASK != QID_VMASK)
#error "PG_MAP_BITS_MASK must be same as 2bit QOS_VMASK"
#endif

#if ((PG_MAP_MAX_ELEMS * PG_MAP_BITS_SIZE) > 8)
#error "Make size-of pg_map greate than 1 byte"
#endif

struct plugin_core_msg_cbs {

        i32 (*topic_enroll)(u8 pg_id, const struct utf8_string *topic,
                            enum mqtt_qos qos);
        i32 (*topic_cancel)(u8 pg_id, const struct utf8_string *topic);
        i32 (*publish)(const struct utf8_string *topic,    const u8 *data_buf,
                       u32 data_len, enum mqtt_qos qos, bool retain);
};

i32 plugin_init(const struct plugin_core_msg_cbs *cbs);

/* u16 composition: MSB is CONNACK-Flags = 0, LSB is CONNACK-RC */
u16 plugin_connect(const struct utf8_string *clientId, 
                   const struct utf8_string *username,
                   const struct utf8_string *password,
                   void **app_usr);

i32 plugin_publish(u8 pg_map, const struct utf8_string *topic,
                   const u8 *payload, u32 pay_len,
                   bool dup, u8 qos, bool retain);

i32 plugin_disconn(const void *app_usr, bool due2err);

#endif

