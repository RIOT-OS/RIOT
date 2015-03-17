/*
 * Copyright (C) 2013 - 2014  INRIA.
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     rpl
 * @{
 *
 * @brief       RPL control messages handling
 *
 * Implementation of RPL control messages handling
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#include "rpl.h"
#include "rpl/rpl_structs.h"
#include "rpl/rpl_config.h"
#include "msg.h"
#include "trickle.h"

#include "sixlowpan.h"
#include "net_help.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
static char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif
#include "debug.h"

/* in send buffer we need space for LL_HDR */
static uint8_t rpl_send_buffer[BUFFER_SIZE];

/* SEND BUFFERS */
static icmpv6_hdr_t *icmp_send_buf;
static rpl_dis_t *rpl_send_dis_buf;
#if RPL_DEFAULT_MOP != RPL_MOP_NON_STORING_MODE
static rpl_dao_ack_t *rpl_send_dao_ack_buf;
#endif
static ipv6_hdr_t *ipv6_send_buf;
static rpl_dio_t *rpl_send_dio_buf;
static rpl_dao_t *rpl_send_dao_buf;
static rpl_opt_dodag_conf_t *rpl_send_opt_dodag_conf_buf;
static rpl_opt_target_t *rpl_send_opt_target_buf;
static rpl_opt_transit_t *rpl_send_opt_transit_buf;
static rpl_opt_prefix_information_t *rpl_send_opt_prefix_information_buf;

/* RECEIVE BUFFERS */
static ipv6_hdr_t *ipv6_buf;
static rpl_dio_t *rpl_dio_buf;
static rpl_dao_t *rpl_dao_buf;
static rpl_dao_ack_t *rpl_dao_ack_buf;
static rpl_opt_dodag_conf_t *rpl_opt_dodag_conf_buf;
static rpl_opt_target_t *rpl_opt_target_buf;
static rpl_opt_transit_t *rpl_opt_transit_buf;
static rpl_dis_t *rpl_dis_buf;
static rpl_opt_t *rpl_opt_buf;
static rpl_opt_solicited_t *rpl_opt_solicited_buf;
static rpl_opt_prefix_information_t *rpl_opt_prefix_information_buf;

/*  SEND BUFFERS */
static icmpv6_hdr_t *get_rpl_send_icmpv6_buf(uint8_t ext_len)
{
    return ((icmpv6_hdr_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ext_len]));
}

#if RPL_DEFAULT_MOP != RPL_MOP_NON_STORING_MODE
static rpl_dao_ack_t *get_rpl_send_dao_ack_buf(void)
{
    return ((rpl_dao_ack_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}
#endif

static rpl_dis_t *get_rpl_send_dis_buf(void)
{
    return ((rpl_dis_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static ipv6_hdr_t *get_rpl_send_ipv6_buf(void)
{
    return ((ipv6_hdr_t *) & (rpl_send_buffer[0]));
}

static uint8_t *get_rpl_send_payload_buf(uint8_t ext_len)
{
    return &(rpl_send_buffer[IPV6_HDR_LEN + ext_len]);
}

static rpl_dio_t *get_rpl_send_dio_buf(void)
{
    return ((rpl_dio_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static rpl_dao_t *get_rpl_send_dao_buf(void)
{
    return ((rpl_dao_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static rpl_opt_dodag_conf_t *get_rpl_send_opt_dodag_conf_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_dodag_conf_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_target_t *get_rpl_send_opt_target_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_target_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_transit_t *get_rpl_send_opt_transit_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_transit_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_prefix_information_t *get_rpl_send_opt_prefix_information_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_prefix_information_t *) & (rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}


/* RECEIVE BUFFERS */
static ipv6_hdr_t *get_rpl_ipv6_buf(void)
{
    return ((ipv6_hdr_t *) & (rpl_buffer[0]));
}

static rpl_opt_target_t *get_rpl_opt_target_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_target_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_transit_t *get_rpl_opt_transit_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_transit_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_dodag_conf_t *get_rpl_opt_dodag_conf_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_dodag_conf_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_dio_t *get_rpl_dio_buf(void)
{
    return ((rpl_dio_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static rpl_dao_t *get_rpl_dao_buf(void)
{
    return ((rpl_dao_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static rpl_dao_ack_t *get_rpl_dao_ack_buf(void)
{
    return ((rpl_dao_ack_t *) & (sixlowpan_buffer[(LL_HDR_LEN + IPV6_HDR_LEN + ICMPV6_HDR_LEN)]));
}

static rpl_dis_t *get_rpl_dis_buf(void)
{
    return ((rpl_dis_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

static rpl_opt_t *get_rpl_opt_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_solicited_t *get_rpl_opt_solicited_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_solicited_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static rpl_opt_prefix_information_t *get_rpl_opt_prefix_information_buf(uint8_t rpl_msg_len)
{
    return ((rpl_opt_prefix_information_t *) & (rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

void rpl_init_root(rpl_options_t *rpl_opts)
{
#if (RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE)
#ifndef RPL_NODE_IS_ROOT
    puts("\n############################## ERROR ###############################");
    puts("This configuration has NO ROUTING TABLE available for the root node!");
    puts("The root will NOT be INITIALIZED.");
    puts("Please build the binary for root in non-storing MOP with:");
    puts("\t\t'make RPL_NODE_IS_ROOT=1'");
    puts("############################## ERROR ###############################\n");
    return;
#endif
#endif
    rpl_instance_t *inst;
    rpl_dodag_t *dodag;

    inst = rpl_new_instance(rpl_opts ? rpl_opts->instance_id : RPL_DEFAULT_INSTANCE);

    if (inst == NULL) {
        DEBUGF("Error - No memory for another RPL instance\n");
        return;
    }

    inst->joined = 1;

    dodag = rpl_new_dodag(inst, &my_address);

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
        if (rpl_opts) {
            dodag->prefix = rpl_opts->prefix;
            dodag->prefix_length = rpl_opts->prefix_len;
            dodag->prefix_flags = rpl_opts->prefix_flags;
            dodag->prefix_preferred_lifetime =
                rpl_opts->prefix_valid_lifetime ? rpl_opts->prefix_preferred_lifetime : 0xffffffff;
            dodag->prefix_valid_lifetime =
                rpl_opts->prefix_valid_lifetime ? rpl_opts->prefix_valid_lifetime : 0xffffffff;
        }
    }
    else {
        DEBUGF("Error - could not generate DODAG\n");
        return;
    }

    trickle_start(rpl_process_pid, &dodag->trickle, RPL_MSG_TYPE_TRICKLE_INTERVAL,
                  RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min), dodag->dio_interval_doubling,
                  dodag->dio_redundancy);
    DEBUGF("ROOT INIT FINISHED\n");

}

/* TODO: this function is used by the ip layer for source routing (non-storing mode)
 * to determine if the node is root and change the dodag direction (upwards -> downwards).
 * This is a hack and needs to be refactored when the new network stack is ready.
 */
uint8_t rpl_is_root(void)
{
    rpl_dodag_t *dodag, *end;
    for (dodag = rpl_dodags, end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
        if (dodag->node_status == ROOT_NODE) {
            return 1;
        }
    }
    return 0;
}

void rpl_send_DIO(rpl_dodag_t *mydodag, ipv6_addr_t *destination)
{
#if ENABLE_DEBUG

    if (destination) {
        DEBUGF("Send DIO to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

#endif
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    if (mydodag == NULL) {
        DEBUGF("Error - trying to send DIO without being part of a dodag.\n");
        return;
    }

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DIO;

    rpl_send_dio_buf = get_rpl_send_dio_buf();
    memset(rpl_send_dio_buf, 0, sizeof(*rpl_send_dio_buf));

    DEBUGF("Sending DIO with ");
    rpl_send_dio_buf->rpl_instanceid = mydodag->instance->id;
    DEBUG("instance %02X ", rpl_send_dio_buf->rpl_instanceid);
    rpl_send_dio_buf->version_number = mydodag->version;
    rpl_send_dio_buf->rank = byteorder_htons(mydodag->my_rank);
    DEBUG("rank %04X\n", byteorder_ntohs(rpl_send_dio_buf->rank));
    rpl_send_dio_buf->g_mop_prf = (mydodag->grounded << RPL_GROUNDED_SHIFT) |
                                  (mydodag->mop << RPL_MOP_SHIFT) | mydodag->prf;
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
    rpl_send_opt_dodag_conf_buf->MaxRankIncrease = byteorder_htons(mydodag->maxrankincrease);
    rpl_send_opt_dodag_conf_buf->MinHopRankIncrease = byteorder_htons(mydodag->minhoprankincrease);
    rpl_send_opt_dodag_conf_buf->ocp = byteorder_htons(mydodag->of->ocp);
    rpl_send_opt_dodag_conf_buf->reserved = 0;
    rpl_send_opt_dodag_conf_buf->default_lifetime = mydodag->default_lifetime;
    rpl_send_opt_dodag_conf_buf->lifetime_unit = byteorder_htons(mydodag->lifetime_unit);

    opt_hdr_len += RPL_OPT_DODAG_CONF_LEN_WITH_OPT_LEN;

    if (!ipv6_addr_is_unspecified(&mydodag->prefix)) {
        rpl_send_opt_prefix_information_buf = get_rpl_send_opt_prefix_information_buf(DIO_BASE_LEN + opt_hdr_len);
        rpl_send_opt_prefix_information_buf->type = RPL_OPT_PREFIX_INFO;
        rpl_send_opt_prefix_information_buf->length = RPL_OPT_PREFIX_INFO_LEN;
        rpl_send_opt_prefix_information_buf->flags = mydodag->prefix_flags;
        rpl_send_opt_prefix_information_buf->prefix = mydodag->prefix;
        rpl_send_opt_prefix_information_buf->prefix_length = mydodag->prefix_length;
        rpl_send_opt_prefix_information_buf->preferred_lifetime = byteorder_htonl(mydodag->prefix_preferred_lifetime);
        rpl_send_opt_prefix_information_buf->valid_lifetime = byteorder_htonl(mydodag->prefix_valid_lifetime);

        opt_hdr_len += RPL_OPT_PREFIX_INFO_LEN_WITH_OPT_LEN;
    }

    uint16_t plen = ICMPV6_HDR_LEN + DIO_BASE_LEN + opt_hdr_len;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
}

void rpl_send_DAO(rpl_dodag_t *my_dodag, ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime,
                  uint8_t start_index)
{
#if RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE
    (void) start_index;
#endif

#if ENABLE_DEBUG

    if (destination) {
        DEBUGF("Send DAO to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }
    else {
        DEBUGF("Send DAO to default destination\n");
    }

#endif

    if (my_dodag == NULL) {
        DEBUGF("send_DAO: I have no my_dodag\n");
        return;
    }

    if (my_dodag->node_status == ROOT_NODE) {
        return;
    }

    if (destination == NULL) {
#if RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE
        destination = &my_dodag->dodag_id;
#else

        if (my_dodag->my_preferred_parent == NULL) {
            DEBUGF("send_DAO: my_dodag has no my_preferred_parent\n");
            return;
        }

        destination = &my_dodag->my_preferred_parent->addr;
#endif
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
#if RPL_DEFAULT_MOP != RPL_MOP_NON_STORING_MODE
    /* add all targets from routing table as targets */
    uint8_t entries = 0;
    uint8_t continue_index = 0;

    for (uint8_t i = start_index; i < rpl_max_routing_entries; i++) {
        if (rpl_get_routing_table()[i].used) {
            rpl_send_opt_target_buf->type = RPL_OPT_TARGET;
            rpl_send_opt_target_buf->length = RPL_OPT_TARGET_LEN;
            rpl_send_opt_target_buf->flags = 0x00;
            rpl_send_opt_target_buf->prefix_length = RPL_DODAG_ID_LEN;
            memcpy(&rpl_send_opt_target_buf->target, &rpl_get_routing_table()[i].address,
                   sizeof(ipv6_addr_t));
            opt_len += RPL_OPT_TARGET_LEN_WITH_OPT_LEN;
            rpl_send_opt_transit_buf = get_rpl_send_opt_transit_buf(DAO_BASE_LEN + opt_len);
            rpl_send_opt_transit_buf->type = RPL_OPT_TRANSIT;
            rpl_send_opt_transit_buf->length = (RPL_OPT_TRANSIT_LEN - sizeof(ipv6_addr_t));
            rpl_send_opt_transit_buf->e_flags = 0x00;
            rpl_send_opt_transit_buf->path_control = 0x00; /* not used */
            rpl_send_opt_transit_buf->path_sequence = 0x00; /* not used */
            rpl_send_opt_transit_buf->path_lifetime = lifetime;
            opt_len += (RPL_OPT_TRANSIT_LEN_WITH_OPT_LEN - sizeof(ipv6_addr_t));
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

#endif
    /* add own address */
    rpl_send_opt_target_buf->type = RPL_OPT_TARGET;
    rpl_send_opt_target_buf->length = RPL_OPT_TARGET_LEN;
    rpl_send_opt_target_buf->flags = 0x00;
    rpl_send_opt_target_buf->prefix_length = RPL_DODAG_ID_LEN;
    if (!ipv6_addr_is_unspecified(&my_dodag->prefix)) {
        ipv6_addr_t tmp;
        ipv6_addr_set_by_eui64(&tmp, rpl_if_id, &my_dodag->prefix);
        memcpy(&rpl_send_opt_target_buf->target, &tmp, sizeof(ipv6_addr_t));
    }
    else {
        memcpy(&rpl_send_opt_target_buf->target, &my_address, sizeof(ipv6_addr_t));
    }
    opt_len += RPL_OPT_TARGET_LEN_WITH_OPT_LEN;

    rpl_send_opt_transit_buf = get_rpl_send_opt_transit_buf(DAO_BASE_LEN + opt_len);
    rpl_send_opt_transit_buf->type = RPL_OPT_TRANSIT;
    rpl_send_opt_transit_buf->e_flags = 0x00;
    rpl_send_opt_transit_buf->path_control = 0x00;
    rpl_send_opt_transit_buf->path_sequence = 0x00;
    rpl_send_opt_transit_buf->path_lifetime = lifetime;

#if RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE
    rpl_send_opt_transit_buf->length = RPL_OPT_TRANSIT_LEN;
    memcpy(&rpl_send_opt_transit_buf->parent, &my_dodag->my_preferred_parent->addr, sizeof(ipv6_addr_t));
    opt_len += RPL_OPT_TRANSIT_LEN_WITH_OPT_LEN;
#else
    rpl_send_opt_transit_buf->length = (RPL_OPT_TRANSIT_LEN - sizeof(ipv6_addr_t));
    opt_len += (RPL_OPT_TRANSIT_LEN_WITH_OPT_LEN - sizeof(ipv6_addr_t));
#endif

    uint16_t plen = ICMPV6_HDR_LEN + DAO_BASE_LEN + opt_len;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);

#if RPL_DEFAULT_MOP != RPL_MOP_NON_STORING_MODE

    if (continue_index > 1) {
        rpl_send_DAO(my_dodag, destination, lifetime, default_lifetime, continue_index);
    }

#endif
}

void rpl_send_DIS(ipv6_addr_t *destination)
{
#if ENABLE_DEBUG

    if (destination) {
        DEBUGF("Send DIS to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

#endif

    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DIS;

    rpl_send_dis_buf = get_rpl_send_dis_buf();

    uint16_t plen = ICMPV6_HDR_LEN + DIS_BASE_LEN;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6);
}

#if RPL_DEFAULT_MOP != RPL_MOP_NON_STORING_MODE
void rpl_send_DAO_ACK(rpl_dodag_t *my_dodag, ipv6_addr_t *destination)
{
#if ENABLE_DEBUG

    if (destination) {
        DEBUGF("Send DAO ACK to %s\n",
               ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, destination));
    }

#endif

    if (my_dodag == NULL) {
        return;
    }

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
}
#endif

void rpl_recv_DIO(void)
{
    ipv6_buf = get_rpl_ipv6_buf();

    rpl_dio_buf = get_rpl_dio_buf();
    DEBUGF("instance %04X ", rpl_dio_buf->rpl_instanceid);
    DEBUGF("rank %04X\n", byteorder_ntohs(rpl_dio_buf->rank));
    int len = DIO_BASE_LEN;

    rpl_instance_t *dio_inst = rpl_get_instance(rpl_dio_buf->rpl_instanceid);

    if (dio_inst == NULL) {
        dio_inst = rpl_new_instance(rpl_dio_buf->rpl_instanceid);

        if (dio_inst == NULL) {
            DEBUGF("Failed to create a new RPL instance!\n");
            return;
        }
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
    while (len < (NTOHS(ipv6_buf->length) - ICMPV6_HDR_LEN)) {
        DEBUGF("parsing DIO options\n");
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch (rpl_opt_buf->type) {

            case (RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case (RPL_OPT_PADN): {
                len += rpl_opt_buf->length;
                break;
            }

            case (RPL_OPT_DAG_METRIC_CONTAINER): {
                len += rpl_opt_buf->length;
                break;
            }

            case (RPL_OPT_ROUTE_INFO): {
                len += rpl_opt_buf->length;
                break;
            }

            case (RPL_OPT_DODAG_CONF): {
                has_dodag_conf_opt = 1;

                if (rpl_opt_buf->length != RPL_OPT_DODAG_CONF_LEN) {
                    DEBUGF("DODAG configuration is malformed.\n");
                    /* error malformed */
                    return;
                }

                rpl_opt_dodag_conf_buf = get_rpl_opt_dodag_conf_buf(len);
                dio_dodag.dio_interval_doubling = rpl_opt_dodag_conf_buf->DIOIntDoubl;
                dio_dodag.dio_min = rpl_opt_dodag_conf_buf->DIOIntMin;
                dio_dodag.dio_redundancy = rpl_opt_dodag_conf_buf->DIORedun;
                dio_dodag.maxrankincrease = byteorder_ntohs(rpl_opt_dodag_conf_buf->MaxRankIncrease);
                dio_dodag.minhoprankincrease = byteorder_ntohs(rpl_opt_dodag_conf_buf->MinHopRankIncrease);
                dio_dodag.default_lifetime = rpl_opt_dodag_conf_buf->default_lifetime;
                dio_dodag.lifetime_unit = byteorder_ntohs(rpl_opt_dodag_conf_buf->lifetime_unit);
                dio_dodag.of = (struct rpl_of_t *) rpl_get_of_for_ocp(byteorder_ntohs(rpl_opt_dodag_conf_buf->ocp));
                if (dio_dodag.of == NULL) {
                    DEBUGF("[Error] OCP from DIO is not supported! ocp: %x\n",
                    byteorder_ntohs(rpl_opt_dodag_conf_buf->ocp));
                    return;
                }

                len += RPL_OPT_DODAG_CONF_LEN_WITH_OPT_LEN;
                break;
            }

            case (RPL_OPT_PREFIX_INFO): {
                if (rpl_opt_buf->length != RPL_OPT_PREFIX_INFO_LEN) {
                    /* error malformed */
                    return;
                }

                rpl_opt_prefix_information_buf = get_rpl_opt_prefix_information_buf(len);

                /* autonomous address-configuration flag */
                if (rpl_opt_prefix_information_buf->flags & (1 << 6)) {
                    ipv6_addr_t tmp;
                    tmp = rpl_opt_prefix_information_buf->prefix;
                    if (!ipv6_addr_is_link_local(&tmp)) {
                        if (byteorder_ntohl(rpl_opt_prefix_information_buf->preferred_lifetime)
                                <= byteorder_ntohl(rpl_opt_prefix_information_buf->valid_lifetime)) {
                            ipv6_addr_set_by_eui64(&tmp, rpl_if_id, &tmp);
                            ipv6_net_if_add_addr(rpl_if_id, &tmp,
                                    NDP_ADDR_STATE_PREFERRED,
                                    byteorder_ntohl(rpl_opt_prefix_information_buf->valid_lifetime),
                                    byteorder_ntohl(rpl_opt_prefix_information_buf->preferred_lifetime),
                                    0);
                            dio_dodag.prefix = rpl_opt_prefix_information_buf->prefix;
                            dio_dodag.prefix_length = rpl_opt_prefix_information_buf->prefix_length;
                            dio_dodag.prefix_valid_lifetime =
                                byteorder_ntohl(rpl_opt_prefix_information_buf->valid_lifetime);
                            dio_dodag.prefix_preferred_lifetime =
                                byteorder_ntohl(rpl_opt_prefix_information_buf->preferred_lifetime);
                            dio_dodag.prefix_flags = rpl_opt_prefix_information_buf->flags;
                        }
                    }
                }

                len += RPL_OPT_PREFIX_INFO_LEN_WITH_OPT_LEN;
                break;
            }

            default:
                DEBUGF("[Error] Unsupported DIO option\n");
                return;
        }
    }

    /* handle packet content... */
    rpl_dodag_t *my_dodag = rpl_get_joined_dodag(dio_inst->id);

    if (my_dodag == NULL) {
        if (!has_dodag_conf_opt) {
            DEBUGF("send DIS\n");
            rpl_send_DIS(&ipv6_buf->srcaddr);
        }

        if (byteorder_ntohs(rpl_dio_buf->rank) < ROOT_RANK) {
            DEBUGF("DIO with Rank < ROOT_RANK\n");
        }

        if (dio_dodag.mop != RPL_DEFAULT_MOP) {
            DEBUGF("Required MOP not supported\n");
        }

        if (dio_dodag.of == NULL) {
            DEBUGF("Required objective function not supported\n");
        }

        if (byteorder_ntohs(rpl_dio_buf->rank) != INFINITE_RANK) {
            DEBUGF("Will join DODAG\n");
            rpl_join_dodag(&dio_dodag, &ipv6_buf->srcaddr, byteorder_ntohs(rpl_dio_buf->rank));
        }
        else {
            DEBUGF("Cannot access DODAG because of DIO with infinite rank\n");
        }

        return;
    }

    if (rpl_equal_id(&my_dodag->dodag_id, &dio_dodag.dodag_id)) {
        /* "our" DODAG */
        if (RPL_COUNTER_GREATER_THAN(dio_dodag.version, my_dodag->version)) {
            if (my_dodag->my_rank == ROOT_RANK) {
                DEBUGF("[Warning] Inconsistent Dodag Version\n");
                my_dodag->version = RPL_COUNTER_INCREMENT(dio_dodag.version);
                trickle_reset_timer(&my_dodag->trickle);
            }
            else {
                DEBUGF("my dodag has no preferred_parent yet - seems to be odd since I have a parent.\n");
                my_dodag->version = dio_dodag.version;
                rpl_global_repair(my_dodag, &ipv6_buf->srcaddr, byteorder_ntohs(rpl_dio_buf->rank));
            }

            return;
        }
        else if (RPL_COUNTER_GREATER_THAN(my_dodag->version, dio_dodag.version)) {
            /* lower version number detected -> send more DIOs */
            trickle_reset_timer(&my_dodag->trickle);
            return;
        }
    }

    /* version matches, DODAG matches */
    if (byteorder_ntohs(rpl_dio_buf->rank) == INFINITE_RANK) {
        trickle_reset_timer(&my_dodag->trickle);
    }

    /* We are root, all done!*/
    if (my_dodag->my_rank == ROOT_RANK) {
        if (byteorder_ntohs(rpl_dio_buf->rank) != INFINITE_RANK) {
            trickle_increment_counter(&my_dodag->trickle);
        }

        return;
    }

    /*********************  Parent Handling *********************/

    rpl_parent_t *parent;
    parent = rpl_find_parent(my_dodag, &ipv6_buf->srcaddr);

    if (parent == NULL) {
        /* add new parent candidate */
        parent = rpl_new_parent(my_dodag, &ipv6_buf->srcaddr, byteorder_ntohs(rpl_dio_buf->rank));

        if (parent == NULL) {
            return;
        }
    }
    else {
        /* DIO OK */
        trickle_increment_counter(&my_dodag->trickle);
    }

    /* update parent rank */
    parent->rank = byteorder_ntohs(rpl_dio_buf->rank);
    rpl_parent_update(my_dodag, parent);

    if (my_dodag->my_preferred_parent == NULL) {
        DEBUGF("My dodag has no preferred_parent yet - seems to be odd since I have a parent...\n");
    }
    else if (rpl_equal_id(&parent->addr, &my_dodag->my_preferred_parent->addr) &&
             (parent->dtsn != rpl_dio_buf->dtsn)) {
        rpl_delay_dao(my_dodag);
    }

    parent->dtsn = rpl_dio_buf->dtsn;

}

void rpl_recv_DAO(void)
{
#if RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE
    DEBUGF("[Error] something went wrong - got a DAO.\n");
    return;
#else
    ipv6_buf = get_rpl_ipv6_buf();
    rpl_dao_buf = get_rpl_dao_buf();
    DEBUG("instance %04X ", rpl_dao_buf->rpl_instanceid);
    DEBUG("sequence %04X\n", rpl_dao_buf->dao_sequence);

    rpl_dodag_t *my_dodag = rpl_get_joined_dodag(rpl_dao_buf->rpl_instanceid);

    if (my_dodag == NULL) {
        DEBUG("[Error] got DAO although not a DODAG\n");
        return;
    }

    int len = DAO_BASE_LEN;
    uint8_t increment_seq = 0;

    while (len < (NTOHS(ipv6_buf->length) - ICMPV6_HDR_LEN)) {
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch (rpl_opt_buf->type) {

            case (RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case (RPL_OPT_PADN): {
                len += (rpl_opt_buf->length + RPL_OPT_LEN);
                break;
            }

            case (RPL_OPT_DAG_METRIC_CONTAINER): {
                len += (rpl_opt_buf->length + RPL_OPT_LEN);
                break;
            }

            case (RPL_OPT_TARGET): {
                rpl_opt_target_buf = get_rpl_opt_target_buf(len);

                if (rpl_opt_target_buf->prefix_length != RPL_DODAG_ID_LEN) {
                    DEBUGF("prefixes are not supported yet\n");
                    break;
                }

                len += (rpl_opt_target_buf->length + RPL_OPT_LEN);
                rpl_opt_transit_buf = get_rpl_opt_transit_buf(len);

                if (rpl_opt_transit_buf->type != RPL_OPT_TRANSIT) {
                    DEBUGF("[Error] - no transit information for target option type = %d\n",
                           rpl_opt_transit_buf->type);
                    break;
                }

                len += (rpl_opt_transit_buf->length + RPL_OPT_LEN);
                /* route lifetime seconds = (DAO lifetime) * (Unit Lifetime) */

#if (RPL_DEFAULT_MOP == RPL_MOP_NON_STORING_MODE) && (RPL_MAX_ROUTING_ENTRIES != 0)
                rpl_add_srh_entry(&rpl_opt_target_buf->target, &rpl_opt_transit_buf->parent,
                                  rpl_opt_transit_buf->path_lifetime * my_dodag->lifetime_unit);
#elif (RPL_MAX_ROUTING_ENTRIES != 0)
                DEBUG("Adding routing information: Target: %s, Source: %s, Lifetime: %u\n",
                      ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &rpl_opt_target_buf->target),
                      ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &ipv6_buf->srcaddr),
                      (rpl_opt_transit_buf->path_lifetime * my_dodag->lifetime_unit));
                rpl_add_routing_entry(&rpl_opt_target_buf->target, &ipv6_buf->srcaddr,
                                      rpl_opt_transit_buf->path_lifetime * my_dodag->lifetime_unit);
#endif
                increment_seq = 1;
                break;
            }

            case (RPL_OPT_TRANSIT): {
                len += (rpl_opt_buf->length + RPL_OPT_LEN);
                break;
            }

            case (RPL_OPT_TARGET_DESC): {
                len += (rpl_opt_buf->length + RPL_OPT_LEN);
                break;
            }

            default:
                return;
        }
    }

#if RPL_DEFAULT_MOP != RPL_MOP_NON_STORING_MODE
    rpl_send_DAO_ACK(my_dodag, &ipv6_buf->srcaddr);
#endif

    if (increment_seq) {
        RPL_COUNTER_INCREMENT(my_dodag->dao_seq);
        rpl_delay_dao(my_dodag);
    }
#endif
}

void rpl_recv_DIS(void)
{
    ipv6_buf = get_rpl_ipv6_buf();
    rpl_dis_buf = get_rpl_dis_buf();
    int len = DIS_BASE_LEN;
    rpl_dodag_t *dodag, *end;
    uint8_t options_missing = 1;

    while (len < (NTOHS(ipv6_buf->length) - ICMPV6_HDR_LEN)) {
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch (rpl_opt_buf->type) {
            case (RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case (RPL_OPT_PADN): {
                len += rpl_opt_buf->length;
                break;
            }

            case (RPL_OPT_SOLICITED_INFO): {
                options_missing = 0;
                len += RPL_OPT_SOLICITED_INFO_LEN;

                /* extract and check */
                if (rpl_opt_buf->length != RPL_OPT_SOLICITED_INFO_LEN) {
                    /* error malformed */
                    return;
                }

                rpl_opt_solicited_buf = get_rpl_opt_solicited_buf(len);

                for (dodag = rpl_dodags, end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
                    if (dodag->joined) {
                        if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_I_MASK) {
                            if (dodag->instance->id != rpl_opt_solicited_buf->rplinstanceid) {
                                continue;
                            }
                        }

                        if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_D_MASK) {
                            if (!rpl_equal_id(&dodag->dodag_id, &rpl_opt_solicited_buf->dodagid)) {
                                continue;
                            }
                        }

                        if (rpl_opt_solicited_buf->VID_Flags & RPL_DIS_V_MASK) {
                            if (dodag->version != rpl_opt_solicited_buf->version) {
                                continue;
                            }
                        }

                        rpl_send_DIO(dodag, &ipv6_buf->srcaddr);
                        trickle_reset_timer(&dodag->trickle);
                    }
                }

                break;
            }

            default:
                return;
        }
    }

    if (options_missing) {
        for (dodag = rpl_dodags, end = dodag + RPL_MAX_DODAGS; dodag < end; dodag++) {
            if (dodag->joined) {
                rpl_send_DIO(dodag, &ipv6_buf->srcaddr);
                trickle_reset_timer(&dodag->trickle);
            }
        }
    }
}

void rpl_recv_DAO_ACK(void)
{
    rpl_dao_ack_buf = get_rpl_dao_ack_buf();

    rpl_dodag_t *my_dodag = rpl_get_joined_dodag(rpl_dao_ack_buf->rpl_instanceid);

    if (my_dodag == NULL) {
        return;
    }

    if (rpl_dao_ack_buf->rpl_instanceid != my_dodag->instance->id) {
        return;
    }

    if (rpl_dao_ack_buf->status != 0) {
        return;
    }

    rpl_dao_ack_received(my_dodag);

}

/* obligatory for each mode. normally not modified */
void rpl_send(ipv6_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t next_header)
{
    uint8_t *p_ptr;
    ipv6_send_buf = get_rpl_send_ipv6_buf();
    p_ptr = get_rpl_send_payload_buf(ipv6_ext_hdr_len);

    ipv6_send_buf->version_trafficclass = IPV6_VER;
    ipv6_send_buf->trafficclass_flowlabel = 0;
    ipv6_send_buf->flowlabel = 0;
    ipv6_send_buf->nextheader = next_header;
    ipv6_send_buf->hoplimit = MULTIHOP_HOPLIMIT;
    ipv6_send_buf->length = HTONS(p_len);

    memcpy(&(ipv6_send_buf->destaddr), destination, 16);
    ipv6_net_if_get_best_src_addr(&(ipv6_send_buf->srcaddr), &(ipv6_send_buf->destaddr));

    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);
    icmp_send_buf->checksum = icmpv6_csum(ipv6_send_buf, icmp_send_buf);

    /* The packet was "assembled" in rpl_%mode%.c. Therefore rpl_send_buf was used.
     * Therefore memcpy is not needed because the payload is at the
     * right memory location already. */

    if (p_ptr != payload) {
        memcpy(p_ptr, payload, p_len);
    }

    ipv6_send_packet(ipv6_send_buf, NULL);
}
