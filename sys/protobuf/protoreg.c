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

#include "protoreg.h"

typedef struct reg_t {
    kernel_pid_t pid;
} reg_t;

static reg_t reg[M_PROTO_ERROR - M_PROTO_START];

void subscribe(kernel_pid_t pid, proto_msg_t command_id) {
    reg[command_id - M_PROTO_START].pid = pid;
}

void submit(proto_msg_t command_id, uint8_t *buffer, size_t size) {
    msg_t msg;

    void *obj = NULL;

    switch(command_id) {
        case M_MQTT_COMMAND: {
            //MqttCommand *obj;
            obj = mqtt_command__unpack(NULL, size, buffer);
            break;
        }

        default:
            break;
    }
    msg.type = command_id;
    msg.content.ptr = obj;
    msg_send(&msg, reg[command_id - M_PROTO_START].pid);
}
