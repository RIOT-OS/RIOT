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

static int _nomac_recv_cb(netdev_event_t event, pkt_t *pkt)       /* this is the signature I
                                                                     propose for netdev */
{
    switch (event) {
        case NETDEV_EVENT_RX_COMPLETE:
            return _netapi_receive(pkt_t *pkt);
            break;
        default:
            return -1;
    }
}

static int _netapi_receive(pkt_t *pkt)
{
    msg_t msg_pkt;
    netreg_t *who_to;
    netmod_t module;
    kernel_pid_t pid;

    /* build the NETAPI packet that is send to the upper layers */
    msg_pkt.type = NETAPI_MSG_TYPE;
    msg_pkt.content.ptr = (char *)pkt;      /* this is how I would imagine the NETAPI call
                                               to look like */

    /* find out who to send it to */
    module = pkt->headers->header_proto;    /* we always transfer the header of the current layer
                                               to the next upper layer... */
                                            /* TODO: type of header_proto in pkt_t: change to netmod_t */

    /* send out the NETAPI message(s) */
    pid = netreg_lookup(who_to, module);
    while (pid != KERNEL_PID_UNDEF) {
        msg_send(&msg_pkt, &msg_ack, pid);
        pid = netreg_getnext(who_to);

        /**
         * NOTE: no ACKing so far - as I see it right now, this is not really needed. But
         * my brain is a wreck, so there might be a good chance I am wrong...
         */
    }

    return 0;  /* all done and good */
}

static int _nomac_get_option(netdev_t *dev, netapi_conf_t *conf)
{
    int res;

    switch (conf->param) {          /* why use nomac specifics here? Just go with NETAPI conf options... */
        case NETAPI_CONF_PROTO:
        case NETAPI_CONF_CARRIER:
        case NETAPI_CONF_ADDRESS:
        case NETAPI_CONF_SUBNETS:
        case NETAPI_CONF_MAX_PACKET_SIZE:
        case NETDEV_OPT_ADDRESS_LONG:
            if ((res = dev->driver->get_option(dev, (netdev_opt_t)conf->param,
                                               conf->data, &(conf->data_len))
                ) == 0) {
                return (int)conf->data_len;
            }
            else {
                return res;
            }

        default:
            break;
    }

    return -ENOTSUP;
}

static int _nomac_set_option(netdev_t *dev, netapi_conf_t *conf)
{
    switch ((nomac_conf_type_t)(conf->param)) {             /* same here... */
        case NETAPI_CONF_PROTO:
        case NETAPI_CONF_CARRIER:
        case NETAPI_CONF_ADDRESS:
        case NETAPI_CONF_SUBNETS:
        case NETDEV_OPT_ADDRESS_LONG:
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

    /* register network device with the netreg */
    netreg_add_interface(thread_getpid());

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
                    ack->result = dev->driver->send_data(dev, pkt);     /* could it look like this? */
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
