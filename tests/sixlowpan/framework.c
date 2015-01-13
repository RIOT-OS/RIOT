/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup  tests
 * @{
 *
 * @file        framework.c
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "msg.h"
#include "netapi.h"
#include "pktbuf.h"
#include "sixlowpan.h"
#ifdef MODULE_SIXLOWPAN_IPHC_CBUF
#include "sixlowpan/iphc_cbuf.h"
#endif
#include "thread.h"

#include "framework.h"

#define _EXP_SIZE   (3)

kernel_pid_t sixlowpan_test_sixlowpan_pid = KERNEL_PID_UNDEF;
kernel_pid_t sixlowpan_test_receiver_pid = KERNEL_PID_UNDEF;
kernel_pid_t sixlowpan_test_mac_pid = KERNEL_PID_UNDEF;

static char _receiver_stack[KERNEL_CONF_STACKSIZE_DEFAULT];
static char _mac_stack[KERNEL_CONF_STACKSIZE_DEFAULT];
static char _sixlowpan_stack[SIXLOWPAN_CONTROL_STACKSIZE];

static inline netapi_conf_t *_snd_to_conf(netapi_pkt_t *snd)
{
    return (netapi_conf_t *)snd;
}

static void *_mac(void *args)
{
    kernel_pid_t exp_pids[_EXP_SIZE] = { KERNEL_PID_UNDEF, KERNEL_PID_UNDEF,
                                         KERNEL_PID_UNDEF
                                       };
    sixlowpan_test_exp_t *exp[_EXP_SIZE] = { NULL, NULL, NULL };
    uint32_t exp_status = 0;
    msg_t msg, msg_rply;
    size_t src_len = 2;
    network_uint16_t short_src = byteorder_htons(0xabcd);
    network_uint64_t long_src = byteorder_htonll(0xabcdef0123456789);

    (void)args;

    while (1) {
        msg_receive(&msg);

        if (msg.type == SIXLOWPAN_TEST_MSG_TYPE_EXP) {
            exp_status = SIXLOWPAN_TEST_ENO_EXP_BUF;

            for (int i = 0; i < _EXP_SIZE; i++) {
                if (exp[i] == NULL) {
                    exp[i] = (sixlowpan_test_exp_t *)(msg.content.ptr);
                    exp_pids[i] = msg.sender_pid;
                    exp_status = 0;
                    break;
                }
            }
        }

        else if (msg.type == NETAPI_MSG_TYPE) {
            msg_t msg_ack;
            netapi_pkt_t *snd = (netapi_pkt_t *)(msg.content.ptr);
            netapi_ack_t *ack = snd->ack;
            int exp_idx = -1;

            msg_ack.type = NETAPI_MSG_TYPE;
            msg_ack.content.ptr = (char *)ack;

            ack->type = NETAPI_CMD_ACK;
            ack->orig = snd->type;

            if (exp_status == SIXLOWPAN_TEST_ENO_EXP_BUF) {
                kernel_pid_t orig_pid = msg.sender_pid;
                msg_rply.content.value = exp_status;

                for (int i = 0; i < _EXP_SIZE; i++) {
                    exp_pids[i] = KERNEL_PID_UNDEF;
                    exp[i] = NULL;
                }

                ack->result = NETAPI_STATUS_OK;

                msg.sender_pid = sixlowpan_test_sixlowpan_pid;
                msg_reply(&msg, &msg_ack);

                msg_send(&msg_rply, orig_pid);
                continue;
            }

            switch (snd->type) {
                case NETAPI_CMD_GET:
                    switch ((netdev_opt_t)_snd_to_conf(snd)->param) {
                        case NETDEV_OPT_SRC_LEN:
                            if (_snd_to_conf(snd)->data_len < sizeof(size_t)) {
                                ack->result = -EOVERFLOW;
                                msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                                break;
                            }

                            memcpy(_snd_to_conf(snd)->data, &src_len, sizeof(size_t));
                            ack->result = sizeof(size_t);
                            break;

                        case NETDEV_OPT_ADDRESS:
                            if (_snd_to_conf(snd)->data_len < sizeof(network_uint16_t)) {
                                ack->result = -EOVERFLOW;
                                msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                                break;
                            }

                            memcpy(_snd_to_conf(snd)->data, &short_src, sizeof(network_uint16_t));
                            ack->result = sizeof(network_uint16_t);
                            break;

                        case NETDEV_OPT_ADDRESS_LONG:
                            if (_snd_to_conf(snd)->data_len < sizeof(network_uint64_t)) {
                                ack->result = -EOVERFLOW;
                                msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                                break;
                            }

                            memcpy(_snd_to_conf(snd)->data, &long_src, sizeof(network_uint64_t));
                            ack->result = sizeof(network_uint64_t);
                            break;

                        default:
                            ack->result = -ENOTSUP;
                            msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                            break;
                    }

                    break;

                case NETAPI_CMD_REG:    /* Needs to react to register for sixlowpan_init */
                    ack->result = NETAPI_STATUS_OK;
                    break;

                case NETAPI_CMD_SND:
                    msg_rply.content.value = SIXLOWPAN_TEST_EWRONG_TEST_RESULT;

                    for (int i = 0; i < _EXP_SIZE; i++) {
                        if (exp[i] != NULL &&
                            snd->pkt->payload_len == exp[i]->pkt.payload_len &&
                            snd->dest_len == exp[i]->dest_len &&
                            memcmp(snd->pkt->payload_data, exp[i]->pkt.payload_data,
                                   snd->pkt->payload_len) == 0 &&
                            memcmp(snd->dest, exp[i]->dest, snd->dest_len) == 0) {
                            int ulh_as_expected = 1;
                            pkt_hlist_t *ptr_snd = snd->pkt->headers;
                            pkt_hlist_t *ptr_exp = exp[i]->pkt.headers;

                            if (ptr_snd != NULL && ptr_exp != NULL) {
                                do {
                                    if (ptr_snd->header_len != ptr_exp->header_len ||
                                        ptr_snd->header_proto != ptr_exp->header_proto ||
                                        memcmp(ptr_snd->header_data, ptr_exp->header_data,
                                               ptr_snd->header_len) != 0) {
                                        ulh_as_expected = 0;
                                        break;
                                    }

                                    pkt_hlist_advance(&ptr_snd);
                                    pkt_hlist_advance(&ptr_exp);
                                }
                                while (ptr_snd != snd->pkt->headers);

                                /* compare until header list reaches its start again */

                                if (ptr_exp != exp[i]->pkt.headers) {
                                    /* if expected is not
                                     * at its start as well */
                                    ulh_as_expected = 0;    /* test failed */
                                }
                            }

                            if (ulh_as_expected) {
                                exp_idx = i;
                                msg_rply.content.value = 0;
                            }

                            break;
                        }
                    }

                    break;

                case NETAPI_CMD_SET:
                    ack->result = 0;

                    switch ((netdev_opt_t)_snd_to_conf(snd)->param) {
                        case NETDEV_OPT_SRC_LEN:
                            if (_snd_to_conf(snd)->data_len > sizeof(size_t)) {
                                ack->result = -EOVERFLOW;
                                msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                                break;
                            }

                            memcpy(&src_len, _snd_to_conf(snd)->data, sizeof(size_t));
                            break;

                        case NETDEV_OPT_ADDRESS:
                            if (_snd_to_conf(snd)->data_len > sizeof(network_uint16_t)) {
                                ack->result = -EOVERFLOW;
                                msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                                break;
                            }

                            memcpy(&short_src, _snd_to_conf(snd)->data, sizeof(network_uint16_t));
                            break;

                        case NETDEV_OPT_ADDRESS_LONG:
                            if (_snd_to_conf(snd)->data_len > sizeof(network_uint64_t)) {
                                ack->result = -EOVERFLOW;
                                msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                                break;
                            }

                            memcpy(&long_src, _snd_to_conf(snd)->data, sizeof(network_uint64_t));
                            break;

                        default:
                            ack->result = -ENOTSUP;
                            msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                            break;
                    }

                    break;

                default:
                    ack->result = -ENOTSUP;
                    msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
                    break;
            }

            msg_reply(&msg, &msg_ack);

            if (exp_idx != -1) {
                msg_send(&msg_rply, exp_pids[exp_idx]);
                exp_pids[exp_idx] = KERNEL_PID_UNDEF;
                exp[exp_idx] = NULL;
            }
        }
    }

    return NULL;
}

static void *_receiver(void *args)
{
    kernel_pid_t exp_pids[_EXP_SIZE] = { KERNEL_PID_UNDEF, KERNEL_PID_UNDEF };
    sixlowpan_test_exp_t *exp[_EXP_SIZE] = { NULL, NULL };
    uint32_t exp_status = 0;
    msg_t msg, msg_rply;

    (void)args;

    while (1) {
        msg_receive(&msg);

        if (msg.type == SIXLOWPAN_TEST_MSG_TYPE_EXP) {
            exp_status = SIXLOWPAN_TEST_ENO_EXP_BUF;

            for (int i = 0; i < _EXP_SIZE; i++) {
                if (exp[i] == NULL) {
                    exp[i] = (sixlowpan_test_exp_t *)(msg.content.ptr);
                    exp_pids[i] = msg.sender_pid;
                    exp_status = 0;
                    break;
                }
            }
        }

        else if (msg.type == NETAPI_MSG_TYPE) {
            msg_t msg_ack;
            netapi_pkt_t *rcv = (netapi_pkt_t *)(msg.content.ptr);
            netapi_ack_t *ack = rcv->ack;
            int exp_idx = -1;

            ack->type = NETAPI_CMD_ACK;
            ack->orig = rcv->type;

            if (exp_status == SIXLOWPAN_TEST_ENO_EXP_BUF) {
                kernel_pid_t orig_pid = msg.sender_pid;
                msg_rply.content.value = exp_status;

                for (int i = 0; i < _EXP_SIZE; i++) {
                    exp_pids[i] = KERNEL_PID_UNDEF;
                    exp[i] = NULL;
                }

                ack->result = NETAPI_STATUS_OK;

                msg.sender_pid = sixlowpan_test_sixlowpan_pid;
                msg_reply(&msg, &msg_ack);

                msg_send(&msg_rply, orig_pid);
                continue;
            }

            msg_ack.type = NETAPI_MSG_TYPE;
            msg_ack.content.ptr = (char *)ack;

            if (rcv->type != NETAPI_CMD_RCV) {
                msg_rply.content.value = SIXLOWPAN_TEST_EUNEXPECTED_COMMAND;
            }
            else {
                msg_rply.content.value = SIXLOWPAN_TEST_EWRONG_TEST_RESULT;

                for (int i = 0; i < _EXP_SIZE; i++) {
                    if (exp[i] != NULL &&
                        rcv->pkt->payload_len == exp[i]->pkt.payload_len &&
                        rcv->src_len == exp[i]->src_len &&
                        rcv->dest_len == exp[i]->dest_len &&
                        memcmp(rcv->pkt->payload_data, exp[i]->pkt.payload_data,
                               rcv->pkt->payload_len) == 0 &&
                        memcmp(rcv->src, exp[i]->src, rcv->src_len) == 0 &&
                        memcmp(rcv->dest, exp[i]->dest, rcv->dest_len) == 0) {
                        exp_idx = i;
                        msg_rply.content.value = 0;
                        break;
                    }
                }
            }

            ack->result = NETAPI_STATUS_OK;

            msg_reply(&msg, &msg_ack);

            if (exp_idx != -1) {
                msg_send(&msg_rply, exp_pids[exp_idx]);
                exp_pids[exp_idx] = KERNEL_PID_UNDEF;
                exp[exp_idx] = NULL;
            }
        }
    }

    return NULL;
}

uint32_t sixlowpan_test_send_test(netapi_pkt_t *snd, int num_snd,
                                  sixlowpan_test_exp_t *exp, int num_exp)
{
    msg_t msg;

    msg.type = SIXLOWPAN_TEST_MSG_TYPE_EXP;

    for (int i = 0; i < num_exp; i++) {
        msg.content.ptr = (char *)(&(exp[i]));
        msg_send(&msg, sixlowpan_test_mac_pid);
    }

    for (int i = 0; i < num_snd; i++) {
        int api_res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)(&(snd[i])));

        if (api_res < 0) {
            return -api_res;
        }
    }

    for (int i = 0; i < num_exp; i++) {
        msg_receive(&msg);

        if (msg.content.value > 0) {
            return msg.content.value;
        }
    }

    return 0;
}

uint32_t sixlowpan_test_receiver_test(netapi_pkt_t *rcv, int num_rcv,
                                      sixlowpan_test_exp_t *exp, int num_exp)
{
    msg_t msg;

    msg.type = SIXLOWPAN_TEST_MSG_TYPE_EXP;

    for (int i = 0; i < num_exp; i++) {
        msg.content.ptr = (char *)(&(exp[i]));
        msg_send(&msg, sixlowpan_test_receiver_pid);
    }

    for (int i = 0; i < num_rcv; i++) {
        int api_res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)(&(rcv[i])));

        if (api_res < 0) {
            return -api_res;
        }
    }

    for (int i = 0; i < num_exp; i++) {
        msg_receive(&msg);

        if (msg.content.value > 0) {
            return msg.content.value;
        }
    }

    return 0;
}

int sixlowpan_test_run(uint32_t exp, sixlowpan_test_t test, char *name)
{
    uint32_t res;

    pktbuf_reset();
    sixlowpan_reset();

#ifdef MODULE_SIXLOWPAN_IPHC_CBUF

    for (int i = 0; i < SIXLOWPAN_IPHC_CBUF_SIZE; i++) {
        sixlowpan_iphc_cbuf_remove_by_cid(i);
    }

#endif

    res = test();

    if (res != exp) {
        printf("\n\nTest '%s' failed with status ", name);

        switch (res) {
            case 0:
                puts("OK (everything is fine, but this was not expected)");
                break;

            case EADDRNOTAVAIL:
                puts("EADDRNOTAVAIL (needed context was not available)");
                break;

            case EAFNOSUPPORT:
                puts("EAFNOSUPPORT (Address family not supported)");
                break;

            case EDESTADDRREQ:
                puts("EDESTADDRREQ (compression bits for destination address are reserved)");
                break;

            case ENOBUFS:
                puts("ENOBUFS (Buffer is full)");
#ifdef DEVELHELP
                pktbuf_print();
#endif
                break;

            case ENOMEM:
                puts("ENOMEM (No space in selected buffer)");
                break;

            case EINVAL:
                puts("EINVAL (Data was invalid [unknown dispatch])");
                break;

            case EMSGSIZE:
                puts("EMSGSIZE (Length of data was invalid)");
                break;

            case EOVERFLOW:
                puts("EOVERFLOW (Length to get option to short)");
                break;

            case ENOMSG:
                puts("ENOMSG (something with your priorities might be wrong)");
                break;

            case ENOTSUP:
                puts("ENOTSUP (Command or option type not supported)");
                break;

            case SIXLOWPAN_TEST_SET_OPTION_ERROR:
                puts("setting option lead to an error");
                break;

            case SIXLOWPAN_TEST_SET_HC_STATUS_ERROR:
                puts("setting header compression status lead to an error");
                break;

            case SIXLOWPAN_TEST_EUNEXPECTED_COMMAND:
                puts("receive thread got something other than a receive command, please debug");
                break;

            case SIXLOWPAN_TEST_EWRONG_TEST_RESULT:
                puts("wrong test result (some outcoming values were not as expected. Please debug)");
                break;

            case SIXLOWPAN_TEST_EPKTBUF_LEAK:
                puts("pktbuf should be empty but it is not, please debug");
#ifdef DEVELHELP
                pktbuf_print();
#endif
                break;

            default:
                printf("UNKNOWN (errno = %" PRIu32 ", please check your libc's errno.h)\n", res);
                break;
        }

        return 0;
    }
    else {
        return 1;
    }
}

void sixlowpan_test_init(void)
{
    sixlowpan_test_receiver_pid = thread_create(_receiver_stack, KERNEL_CONF_STACKSIZE_DEFAULT,
                                  PRIORITY_MAIN - 1, CREATE_STACKTEST, _receiver,
                                  NULL, "receiver");
    sixlowpan_test_mac_pid = thread_create(_mac_stack, KERNEL_CONF_STACKSIZE_DEFAULT,
                                           PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                           _mac, NULL, "mac");
    sixlowpan_test_sixlowpan_pid = sixlowpan_init(sixlowpan_test_mac_pid, _sixlowpan_stack,
                                   SIXLOWPAN_CONTROL_STACKSIZE, PRIORITY_MAIN - 1,
                                   CREATE_STACKTEST, "sixlowpan");

    netapi_register(sixlowpan_test_sixlowpan_pid, sixlowpan_test_receiver_pid, 0);
}

/**
 * @}
 */
