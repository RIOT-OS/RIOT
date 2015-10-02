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

#ifndef __SERVER_UTIL_H__
#define __SERVER_UTIL_H__

#include "mqtt_common.h"

#define my_malloc malloc
#define my_free   free

#define MQTT_SERVER_VERSTR "1.0.1"

#define MIN(a,b) ((a > b)? b : a)

u16 mqp_new_id_server(void);
struct mqtt_packet *mqp_server_alloc(u8 msg_type, u32 buf_sz);

struct mqtt_packet *mqp_server_copy(const struct mqtt_packet *mqp);

extern i32 (*util_dbg_prn)(const c8 *fmt, ...);
extern bool  util_prn_aux; 

void util_mutex_lockin(void);
void util_mutex_unlock(void);

#define MUTEX_LOCKIN()    util_mutex_lockin()
#define MUTEX_UNLOCK()    util_mutex_unlock()

#define USR_INFO(FMT, ...) if(util_dbg_prn) util_dbg_prn(FMT, ##__VA_ARGS__)

#define DBG_INFO(FMT, ...)                              \
        if(util_prn_aux && util_dbg_prn)                \
                util_dbg_prn(FMT, ##__VA_ARGS__)

void util_params_set(i32  (*dbg_prn)(const c8 *fmt, ...),
                     void  *mutex,
                     void (*mutex_lockin)(void*),
                     void (*mutex_unlock)(void*));

#endif
