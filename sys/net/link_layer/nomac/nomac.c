/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include <errno.h>
#include <string.h>

#include "netapi.h"
#include "msg.h"
#include "netdev/base.h"
#include "thread.h"

#include "nomac.h"

#ifdef MODULE_NETDEV_DUMMY
#include "netdev_dummy.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define NOMAC_MSG_QUEUE_SIZE    (16)

static struct {
    kernel_pid_t registrar_pid; /**< Thread recipient is registered to */
    kernel_pid_t recipient_pid; /**< Registered recipient thread */
} _nomac_registry[NOMAC_REGISTRY_SIZE];

static int _nomac_recv_cb(netdev_t *dev, void *src, size_t src_len, void *dest,
                          size_t dest_len, void *payload, size_t payload_len)
{
    (void)dev;
    kernel_pid_t current_pid = thread_getpid();
    size_t offset;
    netapi_rcv_pkt_t packet;
    netapi_ack_t ack_mem;
    msg_t msg_pkt, msg_ack;

    packet.type = NETAPI_CMD_RCV;
    packet.ack = &ack_mem;
    packet.src = src;
    packet.src_len = src_len;
    packet.dest = dest;
    packet.dest_len = dest_len;
    msg_pkt.type = NETAPI_MSG_TYPE;
    msg_pkt.content.ptr = (char *)(&packet);

    for (unsigned int i = 0; i < NOMAC_REGISTRY_SIZE; i++) {
        if (_nomac_registry[i].registrar_pid == current_pid) {
            offset = 0;

            while (offset < payload_len) {
                netapi_ack_t *ack;
                packet.data = (void *)(((char *)payload) + offset);
                packet.data_len = payload_len - offset;

                msg_send_receive(&msg_pkt, &msg_ack,
                                 _nomac_registry[i].recipient_pid);
                ack = (netapi_ack_t *)(msg_ack.content.ptr);

                if ((msg_ack.type == NETAPI_MSG_TYPE) &&
                    (ack->type == NETAPI_CMD_ACK) &&
                    (ack->orig == NETAPI_CMD_RCV)) {
                    if (ack->result > 0) {
                        offset += (ack->result);
                    }
                    else {
                        DEBUG("Error code received for registrar %" PRIkernel_pid
                              " with recipient %" PRIkernel_pid ": %d",
                              _nomac_registry[i].registrar_pid,
                              _nomac_registry[i].recipient_pid,
                              -(ack->result));

                        return ack->result;
                    }
                }
                else {
                    DEBUG("Unexpected msg instead of ACK. Abort for registrar "
                          "\"%s\" with recipient \"%s\": msg.type = %d, "
                          "ack->type = %d, ack->orig = %d",
                          thread_getname(_nomac_registry[i].registrar_pid),
                          thread_getname(_nomac_registry[i].recipient_pid),
                          msg_ack.type, ack->type, ack->orig);

                    return -ENOMSG;
                }

            }
        }
    }

    return payload_len;
}

static inline int _nomac_send(netdev_t *dev, netapi_snd_pkt_t *pkt)
{
    return dev->driver->send_data(dev, pkt->dest, pkt->dest_len, pkt->ulh,
                                  pkt->data, pkt->data_len);
}

static int _nomac_get_registry(netapi_conf_t *conf)
{
    kernel_pid_t current_pid = thread_getpid();
    kernel_pid_t registry[NOMAC_REGISTRY_SIZE];
    uint8_t size = 0;

    for (int i = 0; i < NOMAC_REGISTRY_SIZE; i++) {
        if ((size * sizeof(kernel_pid_t)) > (conf->data_len)) {
            return -EOVERFLOW;
        }

        if (_nomac_registry[i].registrar_pid == current_pid) {
            registry[size++] = _nomac_registry[i].recipient_pid;
        }
    }

    conf->data_len = size * sizeof(kernel_pid_t);
    memcpy(conf->data, registry, conf->data_len);

    return 0;
}

static int _nomac_get_option(netdev_t *dev, netapi_conf_t *conf)
{
    int res;

    switch ((nomac_conf_type_t)conf->param) {
        case NOMAC_PROTO:
        case NOMAC_CHANNEL:
        case NOMAC_ADDRESS:
        case NOMAC_NID:
        case NOMAC_MAX_PACKET_SIZE:
        case NOMAC_ADDRESS2:
            if ((res = dev->driver->get_option(dev, (netdev_opt_t)conf->param,
                                               conf->data, &(conf->data_len))
                ) == 0) {
                return (int)conf->data_len;
            }
            else {
                return res;
            }

        case NOMAC_REGISTRY:
            return _nomac_get_registry(conf);

        default:
            break;
    }

    return -ENOTSUP;
}

static int _nomac_set_option(netdev_t *dev, netapi_conf_t *conf)
{
    switch ((nomac_conf_type_t)(conf->param)) {
        case NOMAC_PROTO:
        case NOMAC_CHANNEL:
        case NOMAC_ADDRESS:
        case NOMAC_NID:
        case NOMAC_ADDRESS2:
            return dev->driver->set_option(dev, (netdev_opt_t)conf->param,
                                           conf->data, conf->data_len);

        default:
            break;
    }

    return -ENOTSUP;
}

static void *_nomac_runner(void *args)
{
    netdev_t *dev = (netdev_t *)args;
    msg_t msg_cmd, msg_ack, msg_queue[NOMAC_MSG_QUEUE_SIZE];

    netapi_cmd_t *cmd;
    netapi_ack_t *ack;

    msg_ack.type = NETAPI_MSG_TYPE;

    msg_init_queue(msg_queue, NOMAC_MSG_QUEUE_SIZE);

    dev->driver->init(dev);
    dev->driver->add_receive_data_callback(dev, _nomac_recv_cb);

    while (1) {
        msg_receive(&msg_cmd);

        if (msg_cmd.type == NETDEV_MSG_EVENT_TYPE) {
            dev->driver->event(dev, msg_cmd.content.value);
        }
        else if (msg_cmd.type == NETAPI_MSG_TYPE) {
            cmd = (netapi_cmd_t *)(msg_cmd.content.ptr);
            ack = cmd->ack;
            msg_ack.content.ptr = (char *)ack;

            switch (cmd->type) {
                case NETAPI_CMD_SND:
                    ack->result = _nomac_send(dev, (netapi_snd_pkt_t *)cmd);
                    break;

                case NETAPI_CMD_GET:
                    ack->result = _nomac_get_option(dev, (netapi_conf_t *)cmd);
                    break;

                case NETAPI_CMD_SET:
                    ack->result = _nomac_set_option(dev, (netapi_conf_t *)cmd);
                    break;

                case NETAPI_CMD_REG:
                    ack->result = -ENOBUFS;

                    for (int i = 0; i < NOMAC_REGISTRY_SIZE; i++) {
                        if (_nomac_registry[i].registrar_pid == KERNEL_PID_UNDEF) {
                            netapi_reg_t *reg = (netapi_reg_t *)cmd;

                            _nomac_registry[i].registrar_pid = thread_getpid();
                            _nomac_registry[i].recipient_pid = reg->reg_pid;
                            ack->result = NETAPI_STATUS_OK;

                            break;
                        }
                    }

                    break;

                case NETAPI_CMD_UNREG:
                    ack->result = NETAPI_STATUS_OK;

                    for (int i = 0; i < NOMAC_REGISTRY_SIZE; i++) {
                        netapi_reg_t *reg = (netapi_reg_t *)cmd;

                        if (_nomac_registry[i].registrar_pid == thread_getpid() &&
                            _nomac_registry[i].recipient_pid == reg->reg_pid) {
                            _nomac_registry[i].recipient_pid = KERNEL_PID_UNDEF;

                            break;
                        }

                    }

                    break;

#ifdef MODULE_NETDEV_DUMMY

                case NETAPI_CMD_FIRE_RCV:
                    ack->result = unittest_netdev_dummy_fire_rcv_event(dev,
                                  ((netapi_rcv_pkt_t *)cmd)->src,
                                  ((netapi_rcv_pkt_t *)cmd)->src_len,
                                  ((netapi_rcv_pkt_t *)cmd)->dest,
                                  ((netapi_rcv_pkt_t *)cmd)->dest_len,
                                  ((netapi_rcv_pkt_t *)cmd)->data,
                                  ((netapi_rcv_pkt_t *)cmd)->data_len);
                    break;
#endif

                default:
                    ack->result = -ENOTSUP;
                    break;
            }

            ack->type = NETAPI_CMD_ACK;
            ack->orig = cmd->type;
            msg_reply(&msg_cmd, &msg_ack);
        }
    }

    /* never reached */
    return NULL;
}

void nomac_init_module(void)
{
    for (int i = 0; i < NOMAC_REGISTRY_SIZE; i++) {
        _nomac_registry[i].registrar_pid = KERNEL_PID_UNDEF;
        _nomac_registry[i].recipient_pid = KERNEL_PID_UNDEF;
    }
}

kernel_pid_t nomac_init(char *stack, int stacksize, char priority,
                        const char *name, netdev_t *dev)
{
    return thread_create(stack, stacksize, priority, CREATE_STACKTEST,
                         _nomac_runner, (void *)dev, name);
}

#ifdef MODULE_NETDEV_DUMMY
void nomac_update_callback(netdev_t *dev)
{
    dev->driver->add_receive_data_callback(dev, _nomac_recv_cb);
}
#endif
