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

#ifndef __CLIENT_MGMT_H__
#define __CLIENT_MGMT_H__

#include "mqtt_common.h"

u32 cl_bmap_get(void *usr_cl);

void *cl_app_hndl_get(void *usr_cl);

void *cl_will_hndl_get(void *usr_cl);

bool cl_can_session_delete(void *usr_cl);

void cl_sub_count_add(void *usr_cl);

void cl_sub_count_del(void *usr_cl);

bool cl_mgmt_qos2_pub_rx_update(void *usr_cl, u16 msg_id);

void cl_pub_dispatch(u32 cl_map, struct mqtt_packet *mqp);

i32 cl_pub_msg_send(void *usr_cl,
                    const struct utf8_string *topic, const u8 *data_buf,
                    u32 data_len, enum mqtt_qos qos, bool retain);

void cl_on_net_close(void *usr_cl);

bool cl_notify_ack(void *usr_cl, u8 msg_type, u16 msg_id);

/* u16 composition: MSB is CONNACK-Flags and LSB is CONNACK-RC. The place-holder
   '*usr_cl' has valid value, only if CONNACK-RC in return value is 0.
*/
u16 cl_connect_rx(void *ctx_cl, bool clean_session, c8 *client_id,
                  void *app_cl, void *will, void **usr_cl);

void cl_on_connack_send(void *usr_cl, bool clean_session);

i32 cl_mgmt_init(void);

#endif
