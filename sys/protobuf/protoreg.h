/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 *
 * @addtogroup      <module_name>
 * @{
 *
 * @file
 * @brief           
 *
 * @author          Attilio Dona'
 */

#ifndef RIOT_SYS_PROTOBUF_PROTOREG_H_
#define RIOT_SYS_PROTOBUF_PROTOREG_H_

#include "kernel.h"
#include "messages.pb-c.h"


typedef enum {
    M_PROTO_START  = 0x9000,
    M_MQTT_CONFIG,
    M_MQTT_COMMAND,
    M_PROTO_END

} proto_msg_t;

//#define MQTT_CONFIG  0x9001
//#define MQTT_COMMAND 0x9002


void proto_shell_run(void);

 /**
  * @brief
  *
  * @param[in] pid thread interested to receive messages
  *
  */
void subscribe(kernel_pid_t pid, proto_msg_t command_id);


/**
 * @brief parse a binary encoded message and submit to subscribers
 *
 *
 */
void submit(proto_msg_t command_id, uint8_t *buffer, size_t size);


#endif /* RIOT_SYS_PROTOBUF_PROTOREG_H_ */
/** @} */
