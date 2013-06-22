/**
 * RPL implementation 
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup rpl 
 * @{
 * @file    rpl.c 
 * @brief   basic RPL functions 
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <vtimer.h>
#include <thread.h>
#include <mutex.h>
#include "msg.h"
#include "rpl.h"
#include "of0.h"
#include "trickle.h"

#include "sys/net/sixlowpan/sixlowmac.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowpan.h"
#include "sys/net/sixlowpan/sixlownd.h"
#include "sys/net/sixlowpan/sixlowerror.h"

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
static struct ipv6_hdr_t *ipv6_send_buf;
static struct icmpv6_hdr_t *icmp_send_buf;
static struct rpl_dio_t *rpl_send_dio_buf;
static struct rpl_dis_t *rpl_send_dis_buf;
static struct rpl_dao_t *rpl_send_dao_buf;
static struct rpl_dao_ack_t *rpl_send_dao_ack_buf;
static struct rpl_opt_dodag_conf_t *rpl_send_opt_dodag_conf_buf;
/* static struct rpl_opt_solicited_t * rpl_send_opt_solicited_buf; */
static struct rpl_opt_target_t *rpl_send_opt_target_buf;
static struct rpl_opt_transit_t *rpl_send_opt_transit_buf;

/* RECEIVE BUFFERS */
static struct ipv6_hdr_t *ipv6_buf;
static struct rpl_dio_t *rpl_dio_buf;
static struct rpl_dis_t *rpl_dis_buf;
static struct rpl_dao_t *rpl_dao_buf;
static struct rpl_dao_ack_t *rpl_dao_ack_buf;
static struct rpl_opt_t *rpl_opt_buf;
static struct rpl_opt_dodag_conf_t *rpl_opt_dodag_conf_buf;
static struct rpl_opt_solicited_t *rpl_opt_solicited_buf;
static struct rpl_opt_target_t *rpl_opt_target_buf;
static struct rpl_opt_transit_t *rpl_opt_transit_buf;

/*  SEND BUFFERS */
static struct ipv6_hdr_t *get_rpl_send_ipv6_buf(void)
{
    return ((struct ipv6_hdr_t *)&(rpl_send_buffer[0]));
}

static uint8_t *get_rpl_send_payload_buf(uint8_t ext_len)
{
    return &(rpl_send_buffer[IPV6_HDR_LEN + ext_len]);
}

static struct icmpv6_hdr_t *get_rpl_send_icmpv6_buf(uint8_t ext_len)
{
    return ((struct icmpv6_hdr_t *)&(rpl_send_buffer[IPV6_HDR_LEN + ext_len]));
}

static struct rpl_dio_t *get_rpl_send_dio_buf(void)
{
    return ((struct rpl_dio_t *)&(rpl_send_buffer[IPV6HDR_ICMPV6HDR_LEN]));
}

static struct rpl_dao_t *get_rpl_send_dao_buf(void)
{
    return ((struct rpl_dao_t *)&(rpl_send_buffer[IPV6HDR_ICMPV6HDR_LEN]));
}

static struct rpl_dao_ack_t *get_rpl_send_dao_ack_buf(void)
{
    return ((struct rpl_dao_ack_t *)&(rpl_send_buffer[IPV6HDR_ICMPV6HDR_LEN]));
}

static struct rpl_dis_t *get_rpl_send_dis_buf(void)
{
    return ((struct rpl_dis_t *)&(rpl_send_buffer[IPV6HDR_ICMPV6HDR_LEN]));
}

static struct rpl_opt_dodag_conf_t *get_rpl_send_opt_dodag_conf_buf(uint8_t rpl_msg_len)
{
    return ((struct rpl_opt_dodag_conf_t *)&(rpl_send_buffer[IPV6HDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

static struct rpl_opt_target_t *get_rpl_send_opt_target_buf(uint8_t rpl_msg_len)
{
    return ((struct rpl_opt_target_t *)&(rpl_send_buffer[IPV6HDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

static struct rpl_opt_transit_t *get_rpl_send_opt_transit_buf(uint8_t rpl_msg_len)
{
    return ((struct rpl_opt_transit_t *)&(rpl_send_buffer[IPV6HDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

/* RECEIVE BUFFERS */
static struct ipv6_hdr_t *get_rpl_ipv6_buf(void)
{
    return ((struct ipv6_hdr_t *)&(rpl_buffer[0]));
}

static struct rpl_dio_t *get_rpl_dio_buf(void)
{
    return ((struct rpl_dio_t *)&(rpl_buffer[IPV6HDR_ICMPV6HDR_LEN]));
}

static struct rpl_dao_t *get_rpl_dao_buf(void)
{
    return ((struct rpl_dao_t *)&(rpl_buffer[IPV6HDR_ICMPV6HDR_LEN]));
}

static struct rpl_dao_ack_t *get_rpl_dao_ack_buf(void)
{
    return ((struct rpl_dao_ack_t *)&(buffer[LLHDR_ICMPV6HDR_LEN]));
}

static struct rpl_dis_t *get_rpl_dis_buf(void)
{
    return ((struct rpl_dis_t *)&(rpl_buffer[IPV6HDR_ICMPV6HDR_LEN]));
}

static struct rpl_opt_t *get_rpl_opt_buf(uint8_t rpl_msg_len)
{
    return ((struct rpl_opt_t *)&(rpl_buffer[IPV6HDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

static struct rpl_opt_dodag_conf_t *get_rpl_opt_dodag_conf_buf(uint8_t rpl_msg_len)
{
    return ((struct rpl_opt_dodag_conf_t *)&(rpl_buffer[IPV6HDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

static struct rpl_opt_solicited_t *get_rpl_opt_solicited_buf(uint8_t rpl_msg_len)
{
    return ((struct rpl_opt_solicited_t *)&(rpl_buffer[IPV6HDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

static struct rpl_opt_target_t *get_rpl_opt_target_buf(uint8_t rpl_msg_len)
{
    return ((struct rpl_opt_target_t *)&(rpl_buffer[IPV6HDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

static struct rpl_opt_transit_t *get_rpl_opt_transit_buf(uint8_t rpl_msg_len)
{
    return ((struct rpl_opt_transit_t *)&(rpl_buffer[IPV6HDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

/* Diese Funktion findet eine implementierte OF anhand des Objective Code Point */
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp)
{
    for(uint16_t i = 0; i < NUMBER_IMPLEMENTED_OFS; i++) {
        if(ocp == objective_functions[i]->ocp) {
            return objective_functions[i];
        }
    }

    return NULL;
}

uint8_t rpl_init(transceiver_type_t trans, uint16_t rpl_address)
{
    mutex_init(&rpl_send_mutex);
    mutex_init(&rpl_recv_mutex);

    if(rpl_address == 0) {
        return SIXLOWERROR_ADDRESS;
    }

    /* initialize routing table */
    rpl_clear_routing_table();
    init_trickle();
    rpl_process_pid = thread_create(rpl_process_buf, RPL_PROCESS_STACKSIZE,
                                    PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                    rpl_process, "rpl_process");

    /* INSERT NEW OBJECTIVE FUNCTIONS HERE */
    objective_functions[0] = rpl_get_of0();
    /* objective_functions[1] = rpl_get_of_ETX() */

    sixlowpan_init(trans, rpl_address, 0);
    /* Wir benötigen einen Link Local prefix, um unsere entsprechende Addresse im Netz abzufragen */
    ipv6_addr_t ll_address;
    ipv6_set_ll_prefix(&ll_address);
    ipv6_get_saddr(&my_address, &ll_address);
    set_rpl_process_pid(rpl_process_pid);

    return SUCCESS;

}

void rpl_init_root(void)
{
    rpl_instance_t *inst;
    rpl_dodag_t *dodag;

    inst = rpl_new_instance(RPL_DEFAULT_INSTANCE);

    if(inst == NULL) {
        printf("Error - No memory for another RPL instance\n");
        return;
    }

    inst->id = RPL_DEFAULT_INSTANCE;
    inst->joined = 1;

    dodag = rpl_new_dodag(RPL_DEFAULT_INSTANCE, &my_address);

    if(dodag != NULL) {
        dodag->of = rpl_get_of_for_ocp(RPL_DEFAULT_OCP);
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
        dodag->my_rank = RPL_ROOT_RANK;
        dodag->joined = 1;
        dodag->my_preferred_parent = NULL;
    }
    else {
        printf("Error - could not generate DODAG\n");
        return;
    }

    i_am_root = 1;
    start_trickle(dodag->dio_min, dodag->dio_interval_doubling, dodag->dio_redundancy);
    puts("ROOT INIT FINISHED");

}


void send_DIO(ipv6_addr_t *destination)
{
    mutex_lock(&rpl_send_mutex);
    rpl_dodag_t *mydodag;
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    mydodag = rpl_get_my_dodag();

    if(mydodag == NULL) {
        printf("Error, trying to send DIO without being part of a dodag. This should not happen\n");
        mutex_unlock(&rpl_send_mutex, 0);
        return;
    }

    icmp_send_buf->type = ICMP_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DIO;
    icmp_send_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);

    rpl_send_dio_buf = get_rpl_send_dio_buf();
    memset(rpl_send_dio_buf, 0, sizeof(*rpl_send_dio_buf));
    rpl_send_dio_buf->rpl_instanceid = mydodag->instance->id;
    rpl_send_dio_buf->version_number = mydodag->version;
    rpl_send_dio_buf->rank = mydodag->my_rank;
    rpl_send_dio_buf->g_mop_prf = (mydodag->grounded << RPL_GROUNDED_SHIFT) | (mydodag->mop << RPL_MOP_SHIFT) | mydodag->prf;
    rpl_send_dio_buf->dtsn = mydodag->dtsn;
    rpl_send_dio_buf->flags = 0;
    rpl_send_dio_buf->reserved = 0;
    rpl_send_dio_buf->dodagid = mydodag->dodag_id;

    int opt_hdr_len = 0;
    /* DODAG Configuration Option! */
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
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex, 0);
}

void send_DIS(ipv6_addr_t *destination)
{
    mutex_lock(&rpl_send_mutex);
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMP_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DIS;
    icmp_send_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);

    rpl_send_dis_buf = get_rpl_send_dis_buf();

    uint16_t plen = ICMPV6_HDR_LEN + DIS_BASE_LEN;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex, 0);
}


void send_DAO(ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index)
{
    if(i_am_root) {
        return;
    }

    mutex_lock(&rpl_send_mutex);
    rpl_dodag_t *my_dodag;
    my_dodag = rpl_get_my_dodag();

    if(destination == NULL) {
        destination = &my_dodag->my_preferred_parent->addr;
    }

    if(default_lifetime) {
        lifetime = my_dodag->default_lifetime;
    }

    icmp_send_buf  = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMP_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DAO;
    icmp_send_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);

    if(my_dodag == NULL) {
        mutex_unlock(&rpl_send_mutex, 0);
        return;
    }

    rpl_send_dao_buf = get_rpl_send_dao_buf();
    memset(rpl_send_dao_buf, 0, sizeof(*rpl_send_dao_buf));
    rpl_send_dao_buf->rpl_instanceid = my_dodag->instance->id;
    rpl_send_dao_buf->k_d_flags = 0x00;
    rpl_send_dao_buf->dao_sequence = my_dodag->dao_seq;
    uint16_t opt_len = 0;
    rpl_send_opt_target_buf = get_rpl_send_opt_target_buf(DAO_BASE_LEN);
    /* Alle Ziele aus der Routing Tabelle als Target eintragen */
    uint8_t entries = 0;
    uint8_t continue_index = 0;

    for(uint8_t i = start_index; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if(routing_table[i].used) {
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

        if(entries >= 5) {
            /* split DAO, so packages dont get too big. */
            /* The value 5 is based on experience */
            continue_index = i + 1;
            break;
        }
    }

    /* Add own address */
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
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex, 0);

    if(continue_index > 1) {
        send_DAO(destination, lifetime, default_lifetime, continue_index);
    }
}

void send_DAO_ACK(ipv6_addr_t *destination)
{
    ipv6_print_addr(destination);
    rpl_dodag_t *my_dodag;
    my_dodag = rpl_get_my_dodag();

    if(my_dodag == NULL) {
        return;
    }

    mutex_lock(&rpl_send_mutex);
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMP_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DAO_ACK;
    icmp_send_buf->checksum = ~icmpv6_csum(PROTO_NUM_ICMPV6);

    rpl_send_dao_ack_buf = get_rpl_send_dao_ack_buf();
    rpl_send_dao_ack_buf->rpl_instanceid = my_dodag->instance->id;
    rpl_send_dao_ack_buf->d_reserved = 0;
    rpl_send_dao_ack_buf->dao_sequence = my_dodag->dao_seq;
    rpl_send_dao_ack_buf->status = 0;

    uint16_t plen = ICMPV6_HDR_LEN + DIS_BASE_LEN;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex, 0);


}

void rpl_process(void)
{

    msg_t m_recv;
    msg_init_queue(msg_queue, RPL_PKT_RECV_BUF_SIZE);

    while(1) {
        msg_receive(&m_recv);
        uint8_t *code;
        code = ((uint8_t *)m_recv.content.ptr);
        /* pakettypen unterscheiden */
        ipv6_buf = get_ipv6_buf();
        memcpy(&rpl_buffer, ipv6_buf, ipv6_buf->length + IPV6_HDR_LEN);

        switch(*code) {
            case(ICMP_CODE_DIS): {
                recv_rpl_dis();
                mutex_unlock(&rpl_recv_mutex, 0);
                break;
            }

            case(ICMP_CODE_DIO): {
                recv_rpl_dio();
                mutex_unlock(&rpl_recv_mutex, 0);
                break;
            }

            case(ICMP_CODE_DAO): {
                recv_rpl_dao();
                mutex_unlock(&rpl_recv_mutex, 0);
                break;
            }

            case(ICMP_CODE_DAO_ACK): {
                recv_rpl_dao_ack();
                mutex_unlock(&rpl_recv_mutex, 0);
                break;
            }

            default:
                mutex_unlock(&rpl_recv_mutex, 0);
                puts("default unlock");
                break;
        }
    }
}


void recv_rpl_dio(void)
{
    ipv6_buf = get_rpl_ipv6_buf();

    rpl_dio_buf = get_rpl_dio_buf();
    int len = DIO_BASE_LEN;

    rpl_instance_t *dio_inst = rpl_get_instance(rpl_dio_buf->rpl_instanceid);
    rpl_instance_t *my_inst = rpl_get_my_instance();

    if(dio_inst == NULL) {
        if(my_inst != NULL) {
            /* Dieser Knoten ist schon Teil eines DODAGS -> kein beitritt zu anderer Instanz moeglich */
            return;
        }

        dio_inst = rpl_new_instance(rpl_dio_buf->rpl_instanceid);

        if(dio_inst == NULL) {
            return;
        }
    }
    else if(my_inst->id != dio_inst->id) {
        printf("Andere Instanz, wir haben %d es kam aber %d\n", my_inst->id, dio_inst->id);
        /* DIO von fremder Instanz ignorieren, Knoten können Momentan nur
         * einer Instanz beitreten und das wird die Instanz sein, der sie als
         * erstes beitreten. Danach kann die Instanz nicht mehr gewechselt
         * werden Unterstützung für mehrere Instanzen könnte in Zukunft
         * implementiert werden */
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
    /* So lange das Paket größer ist, als die DIO Größe + Größe der bisher
     * verarbeiteten Optionen, sind noch weitere Optionen zu bearbeiten */

    /* dabei müssen wir jedoch von der ipv6_buf->length die Größe des ICMP
     * Headers abziehen weil get_rpl_opt_buf die Paketlänge OHNE ipv6 und
     * icmpv6 header übergeben werden muss in ipv6_buf->length ist die
     * IPV6_HDR_LEN nicht enthalten, also muss nur noch die ICMPV6_HDR_LEN
     * abgezogen werden */

    while(len < (ipv6_buf->length - ICMPV6_HDR_LEN)) {
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch(rpl_opt_buf->type) {

            case(RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case(RPL_OPT_PADN): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case(RPL_OPT_DAG_METRIC_CONTAINER): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case(RPL_OPT_ROUTE_INFO): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case(RPL_OPT_DODAG_CONF): {
                has_dodag_conf_opt = 1;

                if(rpl_opt_buf->length != RPL_OPT_DODAG_CONF_LEN) {
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
                dio_dodag.of = rpl_get_of_for_ocp(rpl_opt_dodag_conf_buf->ocp);
                len += RPL_OPT_DODAG_CONF_LEN + 2;
                break;
            }

            case(RPL_OPT_PREFIX_INFO): {
                if(rpl_opt_buf->length != RPL_OPT_PREFIX_INFO_LEN) {
                    /* error malformed */
                    return;
                }

                len += RPL_OPT_PREFIX_INFO_LEN + 2;
                break;
            }

            default:
                printf("[Error] Unsupported DIO option\n");
                return;
        }
    }

    /* handle packet content... */
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if(my_dodag == NULL) {
        if(!has_dodag_conf_opt) {
            puts("send DIS");
            send_DIS(&ipv6_buf->srcaddr);
            return;
        }

        if(rpl_dio_buf->rank < ROOT_RANK) {
            printf("DIO with Rank < ROOT_RANK\n");
            return;
        }

        if(dio_dodag.mop != RPL_DEFAULT_MOP) {
            printf("Required MOP not supported\n");
            return;
        }

        if(dio_dodag.of == NULL) {
            printf("Required objective function not supported\n");
            return;
        }

        if(rpl_dio_buf->rank != INFINITE_RANK) {
            puts("Will join DODAG\n");
            ipv6_print_addr(&dio_dodag.dodag_id);
            rpl_join_dodag(&dio_dodag, &ipv6_buf->srcaddr, rpl_dio_buf->rank);
        }
        else {
            printf("Cannot access DODAG because of DIO with infinite rank\n");
            return;
        }
    }

    if(rpl_equal_id(&my_dodag->dodag_id, &dio_dodag.dodag_id)) {
        /* Mein DODAG */
        if(RPL_COUNTER_GREATER_THAN(dio_dodag.version, my_dodag->version)) {
            if(my_dodag->my_rank == ROOT_RANK) {
                /* Jemand hat ein DIO mit einer höheren Version als der richtigen gesendet */
                /* Wir erhöhen diese Version noch einmal, und machen sie zur neuen */
                printf("[Warning] Inconsistent Dodag Version\n");
                my_dodag->version = RPL_COUNTER_INCREMENT(dio_dodag.version);
                reset_trickletimer();
            }
            else {
                printf("[Info] New Version of dodag %d\n", dio_dodag.version);
                rpl_global_repair(&dio_dodag, &ipv6_buf->srcaddr, rpl_dio_buf->rank);
            }

            return;
        }
        else if(RPL_COUNTER_GREATER_THAN(my_dodag->version, dio_dodag.version)) {
            /* ein Knoten hat noch eine kleinere Versionsnummer -> mehr DIOs senden */
            reset_trickletimer();
            return;
        }
    }

    /* Version stimmt, DODAG stimmt */
    if(rpl_dio_buf->rank == INFINITE_RANK) {
        reset_trickletimer();
    }

    /* Wenn wir root sind, ist nichts weiter zu tun */
    if(my_dodag->my_rank == ROOT_RANK) {
        if(rpl_dio_buf->rank != INFINITE_RANK) {
            trickle_increment_counter();
        }

        return;
    }

    /*  */
    /*  Parent Handling */
    /*  */

    /* Ist Knoten bereits Parent? */
    rpl_parent_t *parent;
    parent = rpl_find_parent(&ipv6_buf->srcaddr);

    if(parent == NULL) {
        /* neuen möglichen Elternknoten hinzufuegen */
        parent = rpl_new_parent(my_dodag, &ipv6_buf->srcaddr, rpl_dio_buf->rank);

        if(parent == NULL) {
            return;
        }
    }
    else {
        /* DIO ok */
        trickle_increment_counter();
    }

    /* update parent rank */
    parent->rank = rpl_dio_buf->rank;
    rpl_parent_update(parent);

    if(rpl_equal_id(&parent->addr, &my_dodag->my_preferred_parent->addr) && (parent->dtsn != rpl_dio_buf->dtsn)) {
        delay_dao();
    }

    parent->dtsn = rpl_dio_buf->dtsn;

}

void recv_rpl_dis(void)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if(my_dodag == NULL) {
        return;
    }

    ipv6_buf = get_rpl_ipv6_buf();
    rpl_dis_buf = get_rpl_dis_buf();
    int len = DIS_BASE_LEN;

    while(len < (ipv6_buf->length - ICMPV6_HDR_LEN)) {
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch(rpl_opt_buf->type) {
            case(RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case(RPL_OPT_PADN): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case(RPL_OPT_SOLICITED_INFO): {
                len += RPL_OPT_SOLICITED_INFO_LEN + 2;

                /* extract + check */
                if(rpl_opt_buf->length != RPL_OPT_SOLICITED_INFO_LEN) {
                    /* error malformed */
                    return;
                }

                rpl_opt_solicited_buf = get_rpl_opt_solicited_buf(len);

                if(rpl_opt_solicited_buf->VID_Flags & RPL_DIS_I_MASK) {
                    if(my_dodag->instance->id != rpl_opt_solicited_buf->rplinstanceid) {
                        return;
                    }
                }

                if(rpl_opt_solicited_buf->VID_Flags & RPL_DIS_D_MASK) {
                    if(!rpl_equal_id(&my_dodag->dodag_id, &rpl_opt_solicited_buf->dodagid)) {
                        return;
                    }
                }

                if(rpl_opt_solicited_buf->VID_Flags & RPL_DIS_V_MASK) {
                    if(my_dodag->version != rpl_opt_solicited_buf->version) {
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

    if(my_dodag == NULL) {
        printf("[Error] got DAO without beeing part of a Dodag\n");
        return;
    }

    ipv6_buf = get_rpl_ipv6_buf();
    rpl_dao_buf = get_rpl_dao_buf();
    int len = DAO_BASE_LEN;
    uint8_t increment_seq = 0;

    while(len < (ipv6_buf->length - ICMPV6_HDR_LEN)) {
        rpl_opt_buf = get_rpl_opt_buf(len);

        switch(rpl_opt_buf->type) {

            case(RPL_OPT_PAD1): {
                len += 1;
                break;
            }

            case(RPL_OPT_PADN): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case(RPL_OPT_DAG_METRIC_CONTAINER): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case(RPL_OPT_TARGET): {
                rpl_opt_target_buf = get_rpl_opt_target_buf(len);

                if(rpl_opt_target_buf->prefix_length != RPL_DODAG_ID_LEN) {
                    printf("prefixes are not supported yet");
                    break;
                }

                len += rpl_opt_target_buf->length + 2;
                rpl_opt_transit_buf = get_rpl_opt_transit_buf(len);

                if(rpl_opt_transit_buf->type != RPL_OPT_TRANSIT) {
                    printf("[Error] - no Transit Inforamtion to Target Option, type = %d\n", rpl_opt_transit_buf->type);
                    break;
                }

                len += rpl_opt_transit_buf->length + 2;
                /* Die eigentliche Lebenszeit einer Route errechnet sich aus  (Lifetime aus DAO) * (Lifetime Unit) Sekunden */
                rpl_add_routing_entry(&rpl_opt_target_buf->target, &ipv6_buf->srcaddr, rpl_opt_transit_buf->path_lifetime * my_dodag->lifetime_unit);
                /* puts("Updated route \n"); */
                increment_seq = 1;
                break;
            }

            case(RPL_OPT_TRANSIT): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case(RPL_OPT_TARGET_DESC): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            default:
                return;
        }
    }

    send_DAO_ACK(&ipv6_buf->srcaddr);

    if(increment_seq) {
        RPL_COUNTER_INCREMENT(my_dodag->dao_seq);
        delay_dao();
    }
}

void recv_rpl_dao_ack(void)
{
    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    if(my_dodag == NULL) {
        return;
    }

    rpl_dao_ack_buf = get_rpl_dao_ack_buf();

    if(rpl_dao_ack_buf->rpl_instanceid != my_dodag->instance->id) {
        return;
    }

    if(rpl_dao_ack_buf->status != 0) {
        return;
    }

    /* puts("Received DAO ACK"); */
    dao_ack_received();

}

void rpl_send(ipv6_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t next_header, void *tcp_socket)
{
    uint8_t *p_ptr;
    ipv6_send_buf = get_rpl_send_ipv6_buf();
    p_ptr = get_rpl_send_payload_buf(ipv6_ext_hdr_len);
    packet_length = 0;

    ipv6_send_buf->version_trafficclass = IPV6_VER;
    ipv6_send_buf->trafficclass_flowlabel = 0;
    ipv6_send_buf->flowlabel = 0;
    ipv6_send_buf->nextheader = next_header;
    ipv6_send_buf->hoplimit = MULTIHOP_HOPLIMIT;
    ipv6_send_buf->length = p_len;

    memcpy(&(ipv6_send_buf->destaddr), destination, 16);
    ipv6_get_saddr(&(ipv6_send_buf->srcaddr), &(ipv6_send_buf->destaddr));

    /* Wenn das Paket in der rpl.c "zusammegebaut" wurde, wurde dafür ohnehin
     * der rpl_send_buf verwendet.  In diesem Fall muss also keine memcopy
     * Aktion durchgeführt werden, da sich der payload bereits im richtigen
     * Speicherbereich befindet. */
    if(p_ptr != payload) {
        memcpy(p_ptr, payload, p_len);
    }

    packet_length = IPV6_HDR_LEN + p_len;

    if(ipv6_prefix_mcast_match(&ipv6_send_buf->destaddr)) {
        lowpan_init((ieee_802154_long_t *)&(ipv6_send_buf->destaddr.uint16[4]), (uint8_t *)ipv6_send_buf);
    }
    else {
        /* find right next hop before sending */
        ipv6_addr_t *next_hop = rpl_get_next_hop(&ipv6_send_buf->destaddr);

        if(next_hop == NULL) {
            if(i_am_root) {
                /* oops... ich bin root und weiß nicht wohin mit dem paketn */
                printf("[Error] destination unknown\n");
                return;
            }
            else {
                next_hop = rpl_get_my_preferred_parent();

                if(next_hop == NULL) {
                    /* kein preferred parent eingetragen */
                    puts("[Error] no preferred parent, dropping package");
                    return;
                }
            }
        }

        lowpan_init((ieee_802154_long_t *)&(next_hop->uint16[4]), (uint8_t *)ipv6_send_buf);
    }

}

ipv6_addr_t *rpl_get_next_hop(ipv6_addr_t *addr)
{
    for(uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if(routing_table[i].used && rpl_equal_id(&routing_table[i].address, addr)) {
            return &routing_table[i].next_hop;
        }
    }

    return NULL;
}

void rpl_add_routing_entry(ipv6_addr_t *addr, ipv6_addr_t *next_hop, uint16_t lifetime)
{
    rpl_routing_entry_t *entry = rpl_find_routing_entry(addr);

    if(entry != NULL) {
        entry->lifetime = lifetime;
        return;
    }

    for(uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if(!routing_table[i].used) {
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
    for(uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if(routing_table[i].used && rpl_equal_id(&routing_table[i].address, addr)) {
            memset(&routing_table[i], 0, sizeof(routing_table[i]));
            return;
        }
    }
}

rpl_routing_entry_t *rpl_find_routing_entry(ipv6_addr_t *addr)
{
    for(uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if(routing_table[i].used && rpl_equal_id(&routing_table[i].address, addr)) {
            return &routing_table[i];
        }
    }

    return NULL;
}

void rpl_clear_routing_table(void)
{
    for(uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        memset(&routing_table[i], 0, sizeof(routing_table[i]));
    }

}

rpl_routing_entry_t *rpl_get_routing_table(void)
{
    return routing_table;
}
