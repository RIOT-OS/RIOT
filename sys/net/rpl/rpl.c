/**
 * RPL implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup rpl
 * @{
 * @file    rpl.c
 * @brief   basic RPL functions
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <string.h>
#include <vtimer.h>
#include <thread.h>
#include <mutex.h>

#include "msg.h"
#include "rpl.h"
#include "etx_beaconing.h"
#include "of0.h"
#include "of_mrhof.h"
#include "trickle.h"

#include "sixlowpan.h"
#include "net_help.h"

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#undef TRICKLE_TIMER_STACKSIZE
#define TRICKLE_TIMER_STACKSIZE (KERNEL_CONF_STACKSIZE_MAIN)
#endif
#include "debug.h"

char rpl_process_buf[RPL_PROCESS_STACKSIZE];
/* global variables */
char i_am_root = 0;
rpl_of_t *objective_functions[NUMBER_IMPLEMENTED_OFS];
rpl_routing_entry_t routing_table[RPL_MAX_ROUTING_ENTRIES];
unsigned int rpl_process_pid;
ipv6_addr_t my_address;
mutex_t rpl_send_mutex;
mutex_t rpl_recv_mutex;
/* receive buffer without LL_HDR */
uint8_t rpl_buffer[BUFFER_SIZE - LL_HDR_LEN];
/* in send buffer we need space fpr LL_HDR */
uint8_t rpl_send_buffer[BUFFER_SIZE];

msg_t msg_queue[RPL_PKT_RECV_BUF_SIZE];
/* SEND BUFFERS */
static ipv6_hdr_t *ipv6_send_buf;
static icmpv6_hdr_t *icmp_send_buf;
static struct rpl_dio_t *rpl_send_dio_buf;
static struct rpl_dis_t *rpl_send_dis_buf;
static struct rpl_dao_t *rpl_send_dao_buf;
static struct rpl_dao_ack_t *rpl_send_dao_ack_buf;
static rpl_opt_dodag_conf_t *rpl_send_opt_dodag_conf_buf;
/* static struct rpl_opt_solicited_t * rpl_send_opt_solicited_buf; */
static rpl_opt_target_t *rpl_send_opt_target_buf;
static rpl_opt_transit_t *rpl_send_opt_transit_buf;

/* RECEIVE BUFFERS */
static ipv6_hdr_t *ipv6_buf;
static struct rpl_dio_t *rpl_dio_buf;
static struct rpl_dis_t *rpl_dis_buf;
static struct rpl_dao_t *rpl_dao_buf;
static struct rpl_dao_ack_t *rpl_dao_ack_buf;
static rpl_opt_t *rpl_opt_buf;
static rpl_opt_dodag_conf_t *rpl_opt_dodag_conf_buf;
static rpl_opt_solicited_t *rpl_opt_solicited_buf;
static rpl_opt_target_t *rpl_opt_target_buf;
static rpl_opt_transit_t *rpl_opt_transit_buf;

/*  SEND BUFFERS */
static ipv6_hdr_t *get_rpl_send_ipv6_buf(void)
{
    return ((ipv6_hdr_t *) &(rpl_send_buffer[0]));
}

static uint8_t *get_rpl_send_payload_buf(uint8_t ext_len)
{
    return &(rpl_send_buffer[IPV6_HDR_LEN + ext_len]);
}

static icmpv6_hdr_t *get_rpl_send_icmpv6_buf(uint8_t ext_len) {
    return ((icmpv6_hdr_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ext_len]));
}

static struct rpl_dio_t *get_rpl_send_dio_buf(void) {
    return ((struct rpl_dio_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static struct rpl_dao_t *get_rpl_send_dao_buf(void) {
    return ((struct rpl_dao_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static struct rpl_dao_ack_t *get_rpl_send_dao_ack_buf(void) {
    return ((struct rpl_dao_ack_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static struct rpl_dis_t *get_rpl_send_dis_buf(void) {
    return ((struct rpl_dis_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static rpl_opt_dodag_conf_t *get_rpl_send_opt_dodag_conf_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_dodag_conf_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_target_t *get_rpl_send_opt_target_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_target_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_transit_t *get_rpl_send_opt_transit_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_transit_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

/* RECEIVE BUFFERS */
static ipv6_hdr_t *get_rpl_ipv6_buf(void)
{
    return ((ipv6_hdr_t *) &(rpl_buffer[0]));
}

static struct rpl_dio_t *get_rpl_dio_buf(void) {
    return ((struct rpl_dio_t *) &(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static struct rpl_dao_t *get_rpl_dao_buf(void) {
    return ((struct rpl_dao_t *) &(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static struct rpl_dao_ack_t *get_rpl_dao_ack_buf(void) {
    return ((struct rpl_dao_ack_t *) &(buffer[(LL_HDR_LEN + IPV6_HDR_LEN + ICMPV6_HDR_LEN)]));
}

static struct rpl_dis_t *get_rpl_dis_buf(void) {
    return ((struct rpl_dis_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static rpl_opt_t *get_rpl_opt_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_t *) &(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_dodag_conf_t *get_rpl_opt_dodag_conf_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_dodag_conf_t *) &(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_solicited_t *get_rpl_opt_solicited_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_solicited_t *) &(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_target_t *get_rpl_opt_target_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_target_t *) &(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_transit_t *get_rpl_opt_transit_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_transit_t *) &(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

/* find implemented OF via objective code point */
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp)
{
    for (uint16_t i = 0; i < NUMBER_IMPLEMENTED_OFS; i++) {
        if (ocp == objective_functions[i]->ocp) {
            return objective_functions[i];
        }
    }

    return NULL;
}

uint8_t rpl_init(transceiver_type_t trans, uint16_t rpl_address)
{
    mutex_init(&rpl_send_mutex);
    mutex_init(&rpl_recv_mutex);

    if (rpl_address == 0) {
        return SIXLOWERROR_ADDRESS;
    }

    rpl_instances_init();

    /* initialize routing table */
    rpl_clear_routing_table();
    init_trickle();
    rpl_process_pid = thread_create(rpl_process_buf, RPL_PROCESS_STACKSIZE,
                                    PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                    rpl_process, "rpl_process");

    /* INSERT NEW OBJECTIVE FUNCTIONS HERE */
    objective_functions[0] = rpl_get_of0();
    /* objective_functions[1] = rpl_get_of_ETX() */

    sixlowpan_lowpan_init(trans, rpl_address, 0);
    /* need link local prefix to query _our_ corresponding address  */
    ipv6_addr_t ll_address;
    ipv6_addr_set_link_local_prefix(&ll_address);
    ipv6_iface_get_best_src_addr(&my_address, &ll_address);
    ipv6_register_rpl_handler(rpl_process_pid);

    /* initialize ETX-calculation if needed */
    if (RPL_DEFAULT_OCP == 1) {
        DEBUG("INIT ETX BEACONING\n");
        etx_init_beaconing(&my_address);
    }

    return SIXLOWERROR_SUCCESS;
}

void rpl_init_root(void)
{
    rpl_instance_t *inst;
    rpl_dodag_t *dodag;

    inst = rpl_new_instance(RPL_DEFAULT_INSTANCE);

    if (inst == NULL) {
        DEBUG("Error - No memory for another RPL instance\n");
        return;
    }

    inst->id = RPL_DEFAULT_INSTANCE;
    inst->joined = 1;

    dodag = rpl_new_dodag(RPL_DEFAULT_INSTANCE, &my_address);

    if (dodag != NULL) {
        dodag->of = (struct rpl_of_t *) rpl_get_of_for_ocp(RPL_DEFAULT_OCP);
        dodag->instance = inst;
        dodag->mop = RPL_DEFAULT_MOP;
        dodag->dtsn = 1;
        dodag->prf = 0;
        dodag->dio_interval_doubling = DEFAULT_DIO_INTERVAL_DOUBLINGS;
        dodag->dio_min = DEFAULT_DIO_INTERVAL_MIN;
        dodag->dio_redundancy = DEFAULT_DIO_REDUNDANCY_CONSTANT;
        dodag->maxrankincrease = 0;
        dodag->minhoprankincrease = (uint16_t)DEFAULT_MIN_HOP_RANK_INCREASE;
        dodag->default_lifetime = (uint8_t)RPL_DEFAULT_LIFETIME;
        dodag->lifetime_unit = RPL_LIFETIME_UNIT;
        dodag->version = RPL_COUNTER_INIT;
        dodag->grounded = RPL_GROUNDED;
        dodag->node_status = (uint8_t) ROOT_NODE;
        dodag->my_rank = RPL_ROOT_RANK;
        dodag->joined = 1;
        dodag->my_preferred_parent = NULL;
    }
    else {
        DEBUG("Error - could not generate DODAG\n");
        return;
    }

    i_am_root = 1;
    start_trickle(dodag->dio_min, dodag->dio_interval_doubling, dodag->dio_redundancy);
    DEBUG("ROOT INIT FINISHED\n");

}


void send_DIO(ipv6_addr_t *destination)
{
    DEBUG("Send DIO\n");
    mutex_lock(&rpl_send_mutex);
    rpl_dodag_t *mydodag;
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    mydodag = rpl_get_my_dodag();

    if (mydodag == NULL) {
        DEBUG("Error - trying to send DIO without being part of a dodag.\n");
        mutex_unlock(&rpl_send_mutex);
        return;
    }

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DIO;

    rpl_send_dio_buf = get_rpl_send_dio_buf();
    memset(rpl_send_dio_buf, 0, sizeof(*rpl_send_dio_buf));

    DEBUG("Sending DIO with ");
    rpl_send_dio_buf->rpl_instanceid = mydodag->instance->id;
    DEBUG("instance %02X ", rpl_send_dio_buf->rpl_instanceid);
    rpl_send_dio_buf->version_number = mydodag->version;
    rpl_send_dio_buf->rank = mydodag->my_rank;
    DEBUG("rank %04X\n", rpl_send_dio_buf->rank);
    rpl_send_dio_buf->g_mop_prf = (mydodag->grounded << RPL_GROUNDED_SHIFT) | (mydodag->mop << RPL_MOP_SHIFT) | mydodag->prf;
    rpl_send_dio_buf->dtsn = mydodag->dtsn;
    rpl_send_dio_buf->flags = 0;
    rpl_send_dio_buf->reserved = 0;
    rpl_send_dio_buf->dodagid = mydodag->dodag_id;

    int opt_hdr_len = 0;
    /* DODAG configuration option */
    rpl_send_opt_dodag_conf_buf = get_rpl_send_opt_dodag_conf_buf(DIO_BASE_LEN);
    rpl_send_opt_dodag_conf_buf->type = RPL_OPT_DODAG_CONF;
    rpl_send_opt_dodag_conf_buf->length = RPL_OPT_DODAG_CONF_LEN;
    rpl_send_opt_dodag_conf_buf->flags_a_pcs = 0;
    rpl_send_opt_dodag_conf_buf->DIOIntDoubl = mydodag->dio_interval_doubling;
    rpl_send_opt_dodag_conf_buf->DIOIntMin = mydodag->dio_min;
    rpl_send_opt_dodag_conf_buf->DIORedun = mydodag->dio_redundancy;
    rpl_send_opt_dodag_conf_buf->MaxRankIncrease = mydodag->maxrankincrease;
    rpl_send_opt_dodag_conf_buf->MinHopRankIncrease = mydodag->minhoprankincrease;
    rpl_send_opt_dodag_conf_buf->ocp = mydodag->of->ocp;
    rpl_send_opt_dodag_conf_buf->reserved = 0;
    rpl_send_opt_dodag_conf_buf->default_lifetime = mydodag->default_lifetime;
    rpl_send_opt_dodag_conf_buf->lifetime_unit = mydodag->lifetime_unit;

    opt_hdr_len += RPL_OPT_LEN + RPL_OPT_DODAG_CONF_LEN;

    uint16_t plen = ICMPV6_HDR_LEN + DIO_BASE_LEN + opt_hdr_len;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
    mutex_unlock(&rpl_send_mutex);
}

void send_DIS(ipv6_addr_t *destination)
{
    DEBUG("Send DIS\n");
    mutex_lock(&rpl_send_mutex);
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DIS;

    rpl_send_dis_buf = get_rpl_send_dis_buf();

    uint16_t plen = ICMPV6_HDR_LEN + DIS_BASE_LEN;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
    mutex_unlock(&rpl_send_mutex);
}


void send_DAO(ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index)
{
    DEBUG("Send DAO\n");
    if (i_am_root) {
        return;
    }

    mutex_lock(&rpl_send_mutex);
    rpl_dodag_t *my_dodag;

    if ((my_dodag = rpl_get_my_dodag()) == NULL) {
        DEBUG("send_DAO: I have no my_dodag\n");
        mutex_unlock(&rpl_send_mutex);
        return;
    }

    if (destination == NULL) {
        if (my_dodag->my_preferred_parent == NULL) {
            DEBUG("send_DAO: my_dodag has no my_preferred_parent\n");
            mutex_unlock(&rpl_send_mutex);
            return;
        }
        destination = &my_dodag->my_preferred_parent->addr;
    }

    if (default_lifetime) {
        lifetime = my_dodag->default_lifetime;
    }

    icmp_send_buf  = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DAO;

    rpl_send_dao_buf = get_rpl_send_dao_buf();
    memset(rpl_send_dao_buf, 0, sizeof(*rpl_send_dao_buf));
    rpl_send_dao_buf->rpl_instanceid = my_dodag->instance->id;
    rpl_send_dao_buf->k_d_flags = 0x00;
    rpl_send_dao_buf->dao_sequence = my_dodag->dao_seq;
    uint16_t opt_len = 0;
    rpl_send_opt_target_buf = get_rpl_send_opt_target_buf(DAO_BASE_LEN);
    /* add all targets from routing table as targets */
    uint8_t entries = 0;
    uint8_t continue_index = 0;

    for (uint8_t i = start_index; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (routing_table[i].used) {
            rpl_send_opt_target_buf->type = RPL_OPT_TARGET;
            rpl_send_opt_target_buf->length = RPL_OPT_TARGET_LEN;
            rpl_send_opt_target_buf->flags = 0x00;
            rpl_send_opt_target_buf->prefix_length = RPL_DODAG_ID_LEN;
            memcpy(&rpl_send_opt_target_buf->target, &routing_table[i].address, sizeof(ipv6_addr_t));
            opt_len += RPL_OPT_TARGET_LEN + 2;
            rpl_send_opt_transit_buf = get_rpl_send_opt_transit_buf(DAO_BASE_LEN + opt_len);
            rpl_send_opt_transit_buf->type = RPL_OPT_TRANSIT;
            rpl_send_opt_transit_buf->length = RPL_OPT_TRANSIT_LEN;
            rpl_send_opt_transit_buf->e_flags = 0x00;
            rpl_send_opt_transit_buf->path_control = 0x00; /* not used */
            rpl_send_opt_transit_buf->path_sequence = 0x00; /* not used */
            rpl_send_opt_transit_buf->path_lifetime = lifetime;
            opt_len += RPL_OPT_TRANSIT_LEN + 2;
            rpl_send_opt_target_buf = get_rpl_send_opt_target_buf(DAO_BASE_LEN + opt_len);
            entries++;
        }

        /* Split DAO, so packages don't get too big.
         * The value 5 is based on experience. */
        if (entries >= 5) {
            continue_index = i + 1;
            break;
        }
    }

    /* add own address */
    rpl_send_opt_target_buf->type = RPL_OPT_TARGET;
    rpl_send_opt_target_buf->length = RPL_OPT_TARGET_LEN;
    rpl_send_opt_target_buf->flags = 0x00;
    rpl_send_opt_target_buf->prefix_length = RPL_DODAG_ID_LEN;
    memcpy(&rpl_send_opt_target_buf->target, &my_address, sizeof(ipv6_addr_t));
    opt_len += RPL_OPT_TARGET_LEN + 2;

    rpl_send_opt_transit_buf = get_rpl_send_opt_transit_buf(DAO_BASE_LEN + opt_len);
    rpl_send_opt_transit_buf->type = RPL_OPT_TRANSIT;
    rpl_send_opt_transit_buf->length = RPL_OPT_TRANSIT_LEN;
    rpl_send_opt_transit_buf->e_flags = 0x00;
    rpl_send_opt_transit_buf->path_control = 0x00;
    rpl_send_opt_transit_buf->path_sequence = 0x00;
    rpl_send_opt_transit_buf->path_lifetime = lifetime;
    opt_len += RPL_OPT_TRANSIT_LEN + 2;

    uint16_t plen = ICMPV6_HDR_LEN + DAO_BASE_LEN + opt_len;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
    mutex_unlock(&rpl_send_mutex);

    if (continue_index > 1) {
        send_DAO(destination, lifetime, default_lifetime, continue_index);
    }
}

void send_DAO_ACK(ipv6_addr_t *destination)
{
    DEBUG("Send DAO ACK\n");
    rpl_dodag_t *my_dodag;
    my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        return;
    }

    mutex_lock(&rpl_send_mutex);
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DAO_ACK;

    rpl_send_dao_ack_buf = get_rpl_send_dao_ack_buf();
    rpl_send_dao_ack_buf->rpl_instanceid = my_dodag->instance->id;
    rpl_send_dao_ack_buf->d_reserved = 0;
    rpl_send_dao_ack_buf->dao_sequence = my_dodag->dao_seq;
    rpl_send_dao_ack_buf->status = 0;

    uint16_t plen = ICMPV6_HDR_LEN + DAO_ACK_LEN;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
    mutex_unlock(&rpl_send_mutex);
}

void rpl_process(void)
{
    msg_t m_recv;
    msg_init_queue(msg_queue, RPL_PKT_RECV_BUF_SIZE);

    while (1) {
        msg_receive(&m_recv);
        uint8_t *code;
        code = ((uint8_t *)m_recv.content.ptr);
        /* differentiate packet types */
        ipv6_buf = ipv6_get_buf();
        memcpy(&rpl_buffer, ipv6_buf, ipv6_buf->length + IPV6_HDR_LEN);

        switch (*code) {
            case (ICMP_CODE_DIS): {
                recv_rpl_dis();
                mutex_unlock(&rpl_recv_mutex);
                break;
            }

            case (ICMP_CODE_DIO): {
                recv_rpl_dio();
                mutex_unlock(&rpl_recv_mutex);
                break;
            }

            case (ICMP_CODE_DAO): {
                recv_rpl_dao();
                mutex_unlock(&rpl_recv_mutex);
                break;
            }

            case (ICMP_CODE_DAO_ACK): {
                recv_rpl_dao_ack();
                mutex_unlock(&rpl_recv_mutex);
                break;
            }

            default:
                mutex_unlock(&rpl_recv_mutex);
                break;
        }
    }
}


void recv_rpl_dio(void)
{
    DEBUG("Received DIO with ");
    ipv6_buf = get_rpl_ipv6_buf();

    rpl_dio_buf = get_rpl_dio_buf();
    DEBUG("instance %04X ", rpl_dio_buf->rpl_instanceid);
    DEBUG("rank %04X\n", rpl_dio_buf->rank);
    int len = DIO_BASE_LEN;

    rpl_instance_t *dio_inst = rpl_get_instance(rpl_dio_buf->rpl_instanceid);
    rpl_instance_t *my_inst = rpl_get_my_instance();

    if (dio_inst == NULL) {
        if (my_inst != NULL) {
            /* already part of a DODAG -> impossible to join other instance */
            DEBUG("Not joining another DODAG!\n");
            return;
        }

        dio_inst = rpl_new_instance(rpl_dio_buf->rpl_instanceid);

        if (dio_inst == NULL) {
            DEBUG("Failed to create a new RPL instance!\n");
            return;
        }
    }
    else if (my_inst == NULL) {
        DEBUG("Not joined an instance yet\n");
    }
    else if (my_inst->id != dio_inst->id) {
        /* TODO: Add support support for several instances.  */

        /* At the moment, nodes can only join one instance, this is
        * the instance they join first.
        * Instances cannot be switched later on.  */

        DEBUG("Ignoring instance - we are %d and got %d\n", my_inst->id, dio_inst->id);
        return;
    }

    rpl_dodag_t dio_dodag;
    memset(&dio_dodag, 0, sizeof(dio_dodag));

    memcpy(&dio_dodag.dodag_id, &rpl_dio_buf->dodagid, sizeof(dio_dodag.dodag_id));
    dio_dodag.dtsn = rpl_dio_buf->dtsn;
    dio_dodag.mop = ((rpl_dio_buf->g_mop_prf >> RPL_MOP_SHIFT) & RPL_SHIFTED_MOP_MASK);
    dio_dodag.grounded = rpl_dio_buf->g_mop_prf >> RPL_GROUNDED_SHIFT;
    dio_dodag.prf = (rpl_dio_buf->g_mop_prf & RPL_PRF_MASK);
    dio_dodag.version = rpl_dio_buf->version_number;
    dio_dodag.instance = dio_inst;

    uint8_t has_dodag_conf_opt = 0;

    /* Parse until all options are consumed.
     * ipv6_buf->length contains the packet length minus ipv6 and
     * icmpv6 header, so only ICMPV6_HDR_LEN remains to be
     * subtracted.  */
    while (len < (ipv6_buf->length - ICMPV6_HDR_LEN)) {
        DEBUG("parsing DIO options\n");
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch (rpl_opt_buf->type) {

            case (RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case (RPL_OPT_PADN): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case (RPL_OPT_DAG_METRIC_CONTAINER): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case (RPL_OPT_ROUTE_INFO): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case (RPL_OPT_DODAG_CONF): {
                has_dodag_conf_opt = 1;

                if (rpl_opt_buf->length != RPL_OPT_DODAG_CONF_LEN) {
                    DEBUG("DODAG configuration is malformed.\n");
                    /* error malformed */
                    return;
                }

                rpl_opt_dodag_conf_buf = get_rpl_opt_dodag_conf_buf(len);
                dio_dodag.dio_interval_doubling = rpl_opt_dodag_conf_buf->DIOIntDoubl;
                dio_dodag.dio_min = rpl_opt_dodag_conf_buf->DIOIntMin;
                dio_dodag.dio_redundancy = rpl_opt_dodag_conf_buf->DIORedun;
                dio_dodag.maxrankincrease =	rpl_opt_dodag_conf_buf->MaxRankIncrease;
                dio_dodag.minhoprankincrease = rpl_opt_dodag_conf_buf->MinHopRankIncrease;
                dio_dodag.default_lifetime = rpl_opt_dodag_conf_buf->default_lifetime;
                dio_dodag.lifetime_unit = rpl_opt_dodag_conf_buf->lifetime_unit;
                dio_dodag.of = (struct rpl_of_t *) rpl_get_of_for_ocp(rpl_opt_dodag_conf_buf->ocp);
                len += RPL_OPT_DODAG_CONF_LEN + 2;
                break;
            }

            case (RPL_OPT_PREFIX_INFO): {
                if (rpl_opt_buf->length != RPL_OPT_PREFIX_INFO_LEN) {
                    /* error malformed */
                    return;
                }

                len += RPL_OPT_PREFIX_INFO_LEN + 2;
                break;
            }

            default:
                DEBUG("[Error] Unsupported DIO option\n");
                return;
        }
    }

    /* handle packet content... */
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        if (!has_dodag_conf_opt) {
            DEBUG("send DIS\n");
            send_DIS(&ipv6_buf->srcaddr);
        }

        if (rpl_dio_buf->rank < ROOT_RANK) {
            DEBUG("DIO with Rank < ROOT_RANK\n");
        }

        if (dio_dodag.mop != RPL_DEFAULT_MOP) {
            DEBUG("Required MOP not supported\n");
        }

        if (dio_dodag.of == NULL) {
            DEBUG("Required objective function not supported\n");
        }

        if (rpl_dio_buf->rank != INFINITE_RANK) {
            DEBUG("Will join DODAG\n");
            rpl_join_dodag(&dio_dodag, &ipv6_buf->srcaddr, rpl_dio_buf->rank);
        }
        else {
            DEBUG("Cannot access DODAG because of DIO with infinite rank\n");
        }
        return;
    }

    if (rpl_equal_id(&my_dodag->dodag_id, &dio_dodag.dodag_id)) {
        /* "our" DODAG */
        if (RPL_COUNTER_GREATER_THAN(dio_dodag.version, my_dodag->version)) {
            if (my_dodag->my_rank == ROOT_RANK) {
                DEBUG("[Warning] Inconsistent Dodag Version\n");
                my_dodag->version = RPL_COUNTER_INCREMENT(dio_dodag.version);
                reset_trickletimer();
            }
            else {
                DEBUG("[Info] New Version of dodag %d\n", dio_dodag.version);
                rpl_global_repair(&dio_dodag, &ipv6_buf->srcaddr, rpl_dio_buf->rank);
            }

            return;
        }
        else if (RPL_COUNTER_GREATER_THAN(my_dodag->version, dio_dodag.version)) {
            /* ein Knoten hat noch eine kleinere Versionsnummer -> mehr DIOs senden */
            reset_trickletimer();
            return;
        }
    }

    /* version matches, DODAG matches */
    if (rpl_dio_buf->rank == INFINITE_RANK) {
        reset_trickletimer();
    }

    /* We are root, all done! */
    if (my_dodag->my_rank == ROOT_RANK) {
        if (rpl_dio_buf->rank != INFINITE_RANK) {
            trickle_increment_counter();
        }

        return;
    }

    /*********************  Parent Handling *********************/

    rpl_parent_t *parent;
    parent = rpl_find_parent(&ipv6_buf->srcaddr);

    if (parent == NULL) {
        /* add new parent candidate */
        parent = rpl_new_parent(my_dodag, &ipv6_buf->srcaddr, rpl_dio_buf->rank);

        if (parent == NULL) {
            return;
        }
    }
    else {
        /* DIO OK */
        trickle_increment_counter();
    }

    /* update parent rank */
    parent->rank = rpl_dio_buf->rank;
    rpl_parent_update(parent);

    if (my_dodag->my_preferred_parent == NULL) {
        DEBUG("my dodag has no preferred_parent yet - seems to be odd since I have a parent...\n");
    }
    else if (rpl_equal_id(&parent->addr, &my_dodag->my_preferred_parent->addr) && (parent->dtsn != rpl_dio_buf->dtsn)) {
        delay_dao();
    }

    parent->dtsn = rpl_dio_buf->dtsn;

}

void recv_rpl_dis(void)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        return;
    }

    ipv6_buf = get_rpl_ipv6_buf();
    rpl_dis_buf = get_rpl_dis_buf();
    int len = DIS_BASE_LEN;

    while (len < (ipv6_buf->length - ICMPV6_HDR_LEN)) {
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch (rpl_opt_buf->type) {
            case (RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case (RPL_OPT_PADN): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case (RPL_OPT_SOLICITED_INFO): {
                len += RPL_OPT_SOLICITED_INFO_LEN + 2;

                /* extract and check */
                if (rpl_opt_buf->length != RPL_OPT_SOLICITED_INFO_LEN) {
                    /* error malformed */
                    return;
                }

                rpl_opt_solicited_buf = get_rpl_opt_solicited_buf(len);

                if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_I_MASK) {
                    if (my_dodag->instance->id != rpl_opt_solicited_buf->rplinstanceid) {
                        return;
                    }
                }

                if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_D_MASK) {
                    if (!rpl_equal_id(&my_dodag->dodag_id, &rpl_opt_solicited_buf->dodagid)) {
                        return;
                    }
                }

                if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_V_MASK) {
                    if (my_dodag->version != rpl_opt_solicited_buf->version) {
                        return;
                    }
                }

                break;
            }

            default:
                return;
        }
    }

    send_DIO(&ipv6_buf->srcaddr);

}

void recv_rpl_dao(void)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        DEBUG("[Error] got DAO although not a DODAG\n");
        return;
    }

    ipv6_buf = get_rpl_ipv6_buf();
    rpl_dao_buf = get_rpl_dao_buf();
    int len = DAO_BASE_LEN;
    uint8_t increment_seq = 0;

    while (len < (ipv6_buf->length - ICMPV6_HDR_LEN)) {
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch (rpl_opt_buf->type) {

            case (RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case (RPL_OPT_PADN): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case (RPL_OPT_DAG_METRIC_CONTAINER): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case (RPL_OPT_TARGET): {
                rpl_opt_target_buf = get_rpl_opt_target_buf(len);

                if (rpl_opt_target_buf->prefix_length != RPL_DODAG_ID_LEN) {
                    DEBUG("prefixes are not supported yet");
                    break;
                }

                len += rpl_opt_target_buf->length + 2;
                rpl_opt_transit_buf = get_rpl_opt_transit_buf(len);

                if (rpl_opt_transit_buf->type != RPL_OPT_TRANSIT) {
                    DEBUG("[Error] - no transit information for target option type = %d\n", rpl_opt_transit_buf->type);
                    break;
                }

                len += rpl_opt_transit_buf->length + 2;
                /* route lifetime seconds = (DAO lifetime) * (Unit Lifetime) */
                rpl_add_routing_entry(&rpl_opt_target_buf->target, &ipv6_buf->srcaddr, rpl_opt_transit_buf->path_lifetime * my_dodag->lifetime_unit);
                increment_seq = 1;
                break;
            }

            case (RPL_OPT_TRANSIT): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case (RPL_OPT_TARGET_DESC): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            default:
                return;
        }
    }

    send_DAO_ACK(&ipv6_buf->srcaddr);

    if (increment_seq) {
        RPL_COUNTER_INCREMENT(my_dodag->dao_seq);
        delay_dao();
    }
}

void recv_rpl_dao_ack(void)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        return;
    }

    rpl_dao_ack_buf = get_rpl_dao_ack_buf();

    if (rpl_dao_ack_buf->rpl_instanceid != my_dodag->instance->id) {
        return;
    }

    if (rpl_dao_ack_buf->status != 0) {
        return;
    }

    dao_ack_received();

}

void rpl_send(ipv6_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t next_header)
{
    uint8_t *p_ptr;
    ipv6_send_buf = get_rpl_send_ipv6_buf();
    p_ptr = get_rpl_send_payload_buf(ipv6_ext_hdr_len);
    uint16_t packet_length = 0;

    ipv6_send_buf->version_trafficclass = IPV6_VER;
    ipv6_send_buf->trafficclass_flowlabel = 0;
    ipv6_send_buf->flowlabel = 0;
    ipv6_send_buf->nextheader = next_header;
    ipv6_send_buf->hoplimit = MULTIHOP_HOPLIMIT;
    ipv6_send_buf->length = p_len;

    memcpy(&(ipv6_send_buf->destaddr), destination, 16);
    ipv6_iface_get_best_src_addr(&(ipv6_send_buf->srcaddr), &(ipv6_send_buf->destaddr));

    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_send_buf->checksum = 0;
    icmp_send_buf->checksum = ~ipv6_csum(ipv6_send_buf, (uint8_t*) icmp_send_buf, ipv6_send_buf->length, IPV6_PROTO_NUM_ICMPV6);

    /* The packet was "assembled" in rpl.c. Therefore rpl_send_buf was used.
     * Therefore memcpy is not needed because the payload is at the
     * right memory location already. */

    if (p_ptr != payload) {
        memcpy(p_ptr, payload, p_len);
    }

    packet_length = IPV6_HDR_LEN + p_len;

    if (ipv6_addr_is_multicast(&ipv6_send_buf->destaddr)) {
        sixlowpan_lowpan_sendto((ieee_802154_long_t *) &(ipv6_send_buf->destaddr.uint16[4]), 
                                (uint8_t *)ipv6_send_buf,
                                packet_length);
    }
    else {
        /* find appropriate next hop before sending */
        ipv6_addr_t *next_hop = rpl_get_next_hop(&ipv6_send_buf->destaddr);

        if (next_hop == NULL) {
            if (i_am_root) {
                DEBUG("[Error] destination unknown\n");
                return;
            }
            else {
                next_hop = rpl_get_my_preferred_parent();

                if (next_hop == NULL) {
                    DEBUG("[Error] no preferred parent, dropping package\n");
                    return;
                }
            }
        }

        sixlowpan_lowpan_sendto((ieee_802154_long_t *) &(next_hop->uint16[4]), 
                                (uint8_t *)ipv6_send_buf,
                                packet_length);
    }

}

ipv6_addr_t *rpl_get_next_hop(ipv6_addr_t *addr)
{
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (routing_table[i].used && rpl_equal_id(&routing_table[i].address, addr)) {
            return &routing_table[i].next_hop;
        }
    }

    return (rpl_get_my_preferred_parent());
}

void rpl_add_routing_entry(ipv6_addr_t *addr, ipv6_addr_t *next_hop, uint16_t lifetime)
{
    rpl_routing_entry_t *entry = rpl_find_routing_entry(addr);

    if (entry != NULL) {
        entry->lifetime = lifetime;
        return;
    }

    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (!routing_table[i].used) {
            routing_table[i].address = *addr;
            routing_table[i].next_hop = *next_hop;
            routing_table[i].lifetime = lifetime;
            routing_table[i].used = 1;
            break;
        }
    }
}

void rpl_del_routing_entry(ipv6_addr_t *addr)
{
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (routing_table[i].used && rpl_equal_id(&routing_table[i].address, addr)) {
            memset(&routing_table[i], 0, sizeof(routing_table[i]));
            return;
        }
    }
}

rpl_routing_entry_t *rpl_find_routing_entry(ipv6_addr_t *addr)
{
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (routing_table[i].used && rpl_equal_id(&routing_table[i].address, addr)) {
            return &routing_table[i];
        }
    }

    return NULL;
}

void rpl_clear_routing_table(void)
{
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        memset(&routing_table[i], 0, sizeof(routing_table[i]));
    }

}

rpl_routing_entry_t *rpl_get_routing_table(void)
{
    return routing_table;
}
