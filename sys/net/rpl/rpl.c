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

#include "sixlowpan/types.h"
#include "sixlowpan/icmp.h"



uint16_t global_tvo_counter = 1; // trail

//rpl_dodag_trail_t trail_temp_dodag; // trail
rpl_dodag_trail_t trail_parent_buffer[RPL_MAX_PARENTS]; //trail: buffer parents when receiving DIOs

uint8_t tvo_sequence_number = 0; // trail
//uint8_t tvo_pending = 1; // trail: defines if a verification is pending
//uint8_t tvo_parent_verified = 1; // trail: defines if a verification is pending
uint8_t do_trail = 0; // trail: enables / disables trail on startup
uint8_t attacker = 0; // trail: enables / disables attacker mode on startup
uint16_t attacker_rank = 0; // trail: rank of the attacker -> is constant
uint16_t ignore_root_addr = 0; //trail: ignore the root: just for SAFEST demo


struct rpl_tvo_local_t tvo_local_buffer[TVO_LOCAL_BUFFER_LEN]; //trail
uint8_t tvo_local_flags[TVO_LOCAL_BUFFER_LEN]; //trail


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

static struct rpl_tvo_t *rpl_send_tvo_buf; //trail send buffer
static struct rpl_tvo_ack_t *rpl_send_tvo_ack_buf; //trail send tvo-ack buffer

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

static struct rpl_tvo_t *rpl_tvo_buf; // trail tvo buffer
static rpl_tvo_signature_t *rpl_send_tvo_signature_buf; // trail tvo buffer
static rpl_tvo_signature_t *rpl_tvo_signature_buf; // trail tvo buffer
static struct rpl_tvo_ack_t *rpl_tvo_ack_buf; // trail tvo-ack buffer

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

//trail get send tvo buffer
static struct rpl_tvo_t* get_rpl_send_tvo_buf(void){
	return ((struct rpl_tvo_t*)&(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}

//trail get send tvo signature buffer
static rpl_tvo_signature_t *get_rpl_send_tvo_signature_buf(uint8_t rpl_msg_len)
{
    return ((rpl_tvo_signature_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

//trail get send tvo tvo-ack buffer
static struct rpl_tvo_ack_t *get_rpl_send_tvo_ack_buf(void) {
    return ((struct rpl_tvo_ack_t *) &(rpl_send_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
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

//trail get tvo buf
static struct rpl_tvo_t* get_rpl_tvo_buf(){
	return ((struct rpl_tvo_t*)&(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN]));
}
//trail get tvo signature buf
static rpl_tvo_signature_t *get_tvo_signature_buf(uint8_t rpl_msg_len)
{
    return ((rpl_tvo_signature_t *) &(rpl_buffer[IPV6_HDR_LEN + ICMPV6_HDR_LEN + rpl_msg_len]));
}

static struct rpl_tvo_ack_t *get_rpl_tvo_ack_buf(void) {
    return ((struct rpl_tvo_ack_t *) &(buffer[(LL_HDR_LEN + IPV6_HDR_LEN + ICMPV6_HDR_LEN)]));
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

    return 0;
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

void change_rank(uint16_t new_rank){
	 rpl_dodag_t *mydodag;
	 mydodag = rpl_get_my_dodag();

	 mydodag->my_rank = new_rank;
	 mydodag->min_rank = new_rank;

	 printf("Calculated rank to %u (manually reset rank)\n" , mydodag->my_rank);

}

void ignore_root(uint16_t my_id,uint16_t root_addr){
	printf("i: ID %u ignores ID %u\n", my_id ,root_addr);
	ignore_root_addr = root_addr;
}


//trail tvo init
struct rpl_tvo_t * rpl_tvo_manual_init(struct rpl_tvo_t * tvo){

	ipv6_addr_t ll_address;
	ipv6_addr_t my_address;
	ipv6_addr_set_link_local_prefix(&ll_address);
	ipv6_iface_get_best_src_addr(&my_address, &ll_address);
    rpl_dodag_t * my_dodag = rpl_get_my_dodag();
    timex_t now;
    vtimer_now(&now);
	tvo->nonce = now.microseconds;
	tvo->rank = 0;
	tvo->rpl_instanceid = 99;//my_dodag->instance->id;
	tvo->s_flag = 0;
	tvo->src_addr = my_address;
	//TODO uncomment for source routing
//	tvo->srh_list[0].addr = my_address;
//	tvo->srh_list_size = 1;
	tvo->tvo_seq = global_tvo_counter + 100;
	global_tvo_counter++;
	tvo->version_number = 66;//my_dodag->version;
	return &tvo;
}

//trail tvo init
struct rpl_tvo_t * rpl_tvo_auto_init(struct rpl_tvo_t * tvo, uint8_t instance, uint8_t version_number){

	ipv6_addr_t ll_address;
	ipv6_addr_t my_address;
	ipv6_addr_set_link_local_prefix(&ll_address);
	ipv6_iface_get_best_src_addr(&my_address, &ll_address);

    timex_t now;
    vtimer_now(&now);
	tvo->nonce = now.microseconds;

	tvo->rank = 0;
	tvo->rpl_instanceid = instance;

	tvo->s_flag = 0;
	tvo->src_addr = my_address;

	tvo->tvo_seq = tvo_sequence_number;
	tvo_sequence_number++;
	tvo->version_number = version_number;
	return &tvo;
}


// trail: enable TRAIL at startup
void start_with_trail(void){
	do_trail = 1;
	printf("TRAIL enabled\n");
}

// trail: disable TRAIL during runtime
void disable_trail(void){
	do_trail = 0;
	printf("TRAIL disabled\n");
}


// trail: enable attacker at startup- only use the chosen rank
void start_as_attacker(uint16_t rank){
	attacker = 1;
	attacker_rank = rank;
	rpl_set_attacker(rank);
	printf("Attacker enabled with rank %u\n",rank);
}

void disable_attacker(void){
	attacker = 0;
	attacker_rank = 0;
	rpl_disable_attacker();
}

// trail: enable attacker during runtime - only use the chosen rank
void configure_as_attacker(uint16_t rank){
	attacker = 1;
	attacker_rank = rank;
	rpl_set_attacker(rank);
	rpl_dodag_t *my_dodag = rpl_get_my_dodag();
	if(my_dodag != NULL){
		my_dodag->my_rank = rank;
		my_dodag->min_rank = rank;
	}
	printf("Attacker enabled with rank %u\n",rank);
	 printf("r: ID %u selected rank %u\n",my_address.uint8[15],rank);
}

// trail: enable attacker at startup- only use the chosen rank
void reset_rpl(void){
	rpl_dodag_t *my_dodag = rpl_get_my_dodag();
	if(my_dodag != NULL){
		//my_dodag->my_rank = INFINITE_RANK;
		//my_dodag->min_rank = INFINITE_RANK;
		rpl_delete_all_parents();
		reset_trail_buffers();
		rpl_del_dodag(my_dodag);
		my_dodag = NULL;
	}
}

void reset_trail_buffers(){
	rpl_dodag_trail_t trail_parent_buffer[RPL_MAX_PARENTS]; //trail: buffer parents when receiving DIOs
	uint8_t i;
	for(i=0;i<RPL_MAX_PARENTS;i++){
		memset(&trail_parent_buffer, 0, sizeof(rpl_dodag_trail_t));
	}

	for(i=0;i<TVO_LOCAL_BUFFER_LEN;i++){
		memset(&tvo_local_buffer, 0, sizeof(struct rpl_tvo_local_t));
		tvo_local_flags[i] = 0;
	}

}

/*
// trail auto send
void enable_tvo_auto_send(void){

	set_tvo_auto_send();
	if(tvo_auto_send == false){
		tvo_auto_send = true;
	}else{
		tvo_auto_send = false;
	}
}
*/

//trail
void tvo_ack_has_been_received(ipv6_addr_t * source, uint8_t tvo_seq){
	uint16_t i;

	for(i=0;i<TVO_LOCAL_BUFFER_LEN;i++)
	{
		if(tvo_local_buffer[i].tvo_seq == tvo_seq && tvo_local_buffer[i].dst_addr.uint8[15] == source->uint8[15]){
			printf("freeing tvo buffer at %u\n",i);
			tvo_local_flags[i] = 0; //free array index
		}
	//	if(tvo_local_buffer[i].his_tvo_seq == tvo_seq && tvo_local_buffer[i].dst_addr.uint8[15] == source->uint8[15]){
	//		printf("freeing tvo buffer at %u\n",i);
	//		tvo_local_flags[i] = 0; //free array index
	//	}
	}
}

//trail
struct rpl_tvo_local_t * has_tvo_been_received(ipv6_addr_t * source, uint8_t tvo_seq){

	uint16_t i;

	for(i=0;i<TVO_LOCAL_BUFFER_LEN;i++)
	{
		if(tvo_local_buffer[i].his_tvo_seq == tvo_seq && tvo_local_buffer[i].prev_hop_addr.uint8[15] == source->uint8[15]){
			return &tvo_local_buffer[i];
		}
	}
	return NULL;
}

//trail
void save_tvo_locally(struct rpl_tvo_local_t * tvo_copy){

	uint8_t found_spot = 0;
	uint32_t temp_ts = UINT32_MAX;
	uint32_t i;
	uint32_t j = 0;

	for(i=0;i<TVO_LOCAL_BUFFER_LEN;i++){
		if(tvo_local_flags[i] == 0){
			found_spot = 1;
			//tvo_local_buffer[i] = tvo_copy;
			memcpy(&tvo_local_buffer[i], tvo_copy, sizeof(*tvo_copy));
			printf("blocking tvo buffer at %u for TVO (seq: %u)\n",i, tvo_copy->tvo_seq);
			tvo_local_flags[i] = 1;
			break;
		}
		if(tvo_local_buffer[i].timestamp_received < temp_ts){
			temp_ts = tvo_local_buffer[i].timestamp_received;
			j = i;
		}
	}
	if(found_spot == 0){
		memcpy(&tvo_local_buffer[j], tvo_copy, sizeof(*tvo_copy));
		tvo_local_flags[j] = 1;
		printf("blocking tvo buffer at %u\n",j);
		//tvo_local_buffer[j] = tvo_copy;
	}


}

//trail
void reset_tvo_timer(){
	printf("**Node %u: Resetting all TRAIL parents -> accepting all incoming DIOs\n**",my_address.uint8[15]);
	uint8_t i;
	for(i=0;i<RPL_MAX_PARENTS;i++){
		memset(&trail_parent_buffer[i], 0, sizeof(rpl_dodag_trail_t));
	}

}

//trail
void resend_tvos(){
	printf("\n   ########### CHECKING FOR TVO RESENDS ########\n\n");

	uint32_t i;
	timex_t now;
	vtimer_now(&now);

	uint8_t resend = 0;

	for(i=0;i<TVO_LOCAL_BUFFER_LEN;i++){
		if(tvo_local_flags[i] == 1){ //only resend for which no ack has been received
			resend++;
		//	printf("%u: ((%u - %u) > %u) && (%u < %u) --> %u \n",i, now.microseconds, tvo_local_buffer[i].timestamp_received, (DEFAULT_WAIT_FOR_TVO_ACK*1000000), tvo_local_buffer[i].number_resend, TVO_SEND_RETRIES, (now.microseconds - tvo_local_buffer[i].timestamp_received));
			if(((now.microseconds - tvo_local_buffer[i].timestamp_received) > (DEFAULT_WAIT_FOR_TVO_ACK * 1000000)) && (tvo_local_buffer[i].number_resend < TVO_SEND_RETRIES)){
				//resend tvo
				struct rpl_tvo_t tvo;
				memcpy(&tvo, &tvo_local_buffer[i], sizeof(tvo));
			//	tvo.tvo_seq = tvo_local_buffer[i].his_tvo_seq;
				printf("*RE*");

				//test: send via multicast
	//			ipv6_addr_t next_hop;
//				memcpy(&next_hop, &(ipv6_buf->srcaddr), sizeof(next_hop));

			//	ipv6_addr_set_all_nodes_addr(&next_hop);
			//	send_TVO(&next_hop, &tvo, NULL);

				send_TVO(&tvo_local_buffer[i].dst_addr, &tvo, NULL);

				tvo_local_buffer[i].number_resend++;
				//usleep(500000);
				timex_t time = timex_set(0, 250000);
				vtimer_sleep(time);
			}
			if (tvo_local_buffer[i].number_resend >= TVO_SEND_RETRIES){
				printf("max number of resends reached: freeing tvo buffer at %u\n",i);
				tvo_local_flags[i] = 0; //max number of resends, free buffer
				resend--;
			}
		}
	}
	if(resend == 0){
		delay_tvo(TEST_WAIT_FOR_TVO_ACK);
	}
//	else{
//		printf("Number of TVOs for resend at later point: %u\n",resend);
//	}
}


uint8_t include_parent_into_trail_buffer(void){
	uint8_t i;

	for(i=0;i<sizeof(trail_parent_buffer);i++){
		if(trail_parent_buffer[i].in_progress != 1){
			memset(&trail_parent_buffer[i], 0, sizeof(rpl_dodag_trail_t));
			trail_parent_buffer[i].in_progress = 1;
			printf("Including parent into TRAIL buffer at %u\n",i);
			return i;
		}
	}
	return 255;
}

uint8_t get_parent_from_trail_buffer(ipv6_addr_t * src_addr){
	uint8_t i;

	for(i=0;i<RPL_MAX_PARENTS;i++){
		if(trail_parent_buffer[i].parent_addr.uint8[15] == src_addr->uint8[15] && trail_parent_buffer[i].in_progress == 1){
			//printf("Returning parent from TRAIL buffer at %u\n",i);
			return i;
		}
	}
	//printf("Parent not in TRAIL buffer\n");
	return 255;
}

uint8_t is_parent_verified(ipv6_addr_t * src_addr, uint16_t dio_rank){

	rpl_parent_t *parent;
	parent = rpl_find_parent(src_addr);

	if(parent == NULL){
		printf("Parent not yet in routing table - return false\n");
		return 0;
	}
	else if (parent->rank != dio_rank) {
		// TODO may have to delete parent first (?)
		rpl_delete_parent(parent);
		printf("parent must have changed his rank - delete parent and return false\n");
		return 0;
	}
	else{
		printf("parent is in routing table with same rank - return true\n");
		return 1;
	}
}

void join_dodag(rpl_dodag_t * dio_dodag, ipv6_addr_t * src_addr, uint16_t parent_rank, uint8_t parent_dtsn){

	rpl_dodag_t *my_dodag = rpl_get_my_dodag();
	if(my_dodag == NULL){
		DEBUG("Will join DODAG: ");

		char addr_str[IPV6_MAX_ADDR_STR_LEN];
		printf("Joining network with DODAG ID: %s\n",ipv6_addr_to_str(addr_str, &dio_dodag->dodag_id));

		#if ENABLE_DEBUG
			char addr_str[IPV6_MAX_ADDR_STR_LEN];
			printf("%s\n", ipv6_addr_to_str(addr_str, &dio_dodag.dodag_id));
		#endif
		rpl_join_dodag(dio_dodag, src_addr, parent_rank);
	}
	if (rpl_equal_id(&my_dodag->dodag_id, &dio_dodag->dodag_id)) {
	        /* "our" DODAG */
	        if (RPL_COUNTER_GREATER_THAN(dio_dodag->version, my_dodag->version)) {
	            if (my_dodag->my_rank == ROOT_RANK) {
	                DEBUG("[Warning] Inconsistent Dodag Version\n");
	                my_dodag->version = RPL_COUNTER_INCREMENT(dio_dodag->version);
	                reset_trickletimer();
	            }
	            else {
	                DEBUG("[Info] New Version of dodag %d\n", dio_dodag.version);
	                rpl_global_repair(dio_dodag, src_addr, parent_rank);
	            }

	            return;
	        }
	        else if (RPL_COUNTER_GREATER_THAN(my_dodag->version, dio_dodag->version)) {
	            /* ein Knoten hat noch eine kleinere Versionsnummer -> mehr DIOs senden */
	            reset_trickletimer();
	            return;
	        }
	    }

	    /* version matches, DODAG matches */
	    if (parent_rank == INFINITE_RANK) {
	        reset_trickletimer();
	    }

	    /* We are root, all done! */
	    if (my_dodag->my_rank == ROOT_RANK) {
	        if (parent_rank != INFINITE_RANK) {
	            trickle_increment_counter();
	        }

	        return;
	    }

	    /*********************  Parent Handling *********************/
	    rpl_parent_t *parent;
	    parent = rpl_find_parent(src_addr);

	    my_dodag = rpl_get_my_dodag();

	    if(attacker == 0 && parent != NULL && parent->rank >= my_dodag->my_rank){
	    	printf("RANK ERROR: parent has greater or equal rank (%u >= %u) -> ignore parent \n", parent->rank, my_dodag->my_rank);
	    	return;
	    }


	    if (parent == NULL) {
	        /* add new parent candidate */
	        parent = rpl_new_parent(my_dodag, src_addr, parent_rank);

	        if (parent == NULL) {
	            return;
	        }
	    }
	    else {
	        /* DIO OK */
	        trickle_increment_counter();
	    }

	    /* update parent rank */
	    printf("Update parent *ID %u* (rank %u)\n", parent->addr.uint8[15], parent->rank);
	    parent->rank = parent_rank;
	    rpl_parent_update(parent);

	    char addr_str[IPV6_MAX_ADDR_STR_LEN];
	    //printf("Update parent (rank %u): %s\n", parent->rank, ipv6_addr_to_str(addr_str, &(parent->addr)));
	   // printf("Update parent *ID %u* (rank %u)\n", parent->addr.uint8[15], parent->rank);

	    //trail: disable downward routes
	//    if (rpl_equal_id(&parent->addr, &my_dodag->my_preferred_parent->addr) && (parent->dtsn != parent_dtsn)) {
	    	//printf("\n*\n*\n ********** ignore DTSN and DAO update *********** \n*\n*\n");
	    	// delay_dao();
	//    }
	 //   printf(" ----- ignore this ");
	    parent->dtsn = parent_dtsn;
	 //   printf(" ----- \n");
}


void send_DIO(ipv6_addr_t *destination)
{

//	char addr_str[IPV6_MAX_ADDR_STR_LEN];
//	printf("send DIO to %s (IPv6: " ,ipv6_addr_to_str(addr_str, destination));
	//printf("\n\n %u %u %u %u %u %u %u %u\n\n", my_address.uint16[0], my_address.uint16[1], my_address.uint16[2], my_address.uint16[3],
		//	my_address.uint16[4], my_address.uint16[5], my_address.uint16[6], my_address.uint16[7]);

    mutex_lock(&rpl_send_mutex);
    rpl_dodag_t *mydodag;
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    mydodag = rpl_get_my_dodag();
    uint16_t myRank;
    uint8_t myInst;
    if(mydodag == NULL){
    	myRank = 0;
    	myInst = 123;

    }else{
    	myRank = mydodag->my_rank;
    	myInst = mydodag->instance->id;
    }

    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    //printf("send DIO (my rank: %u, inst: %u) to %s (IPv6: ", myRank , myInst ,ipv6_addr_to_str(addr_str, destination));
  //	printf("send DIO to MCAST (my rank: %u, inst: %u, ", myRank , myInst);
  	printf("m: ID %u send msg DIO to MCAST #color0\n", my_address.uint8[15], destination->uint8[15]);

    if (mydodag == NULL) {
        DEBUG("Error - trying to send DIO without being part of a dodag.\n");
        mutex_unlock(&rpl_send_mutex);
        return;
    }

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DIO;
    icmp_send_buf->checksum = ~icmpv6_csum(IPV6_PROTO_NUM_ICMPV6);

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
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex);
}

void send_DIS(ipv6_addr_t *destination)
{

//	printf("***** send DIS *****\n");

    mutex_lock(&rpl_send_mutex);
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DIS;
    icmp_send_buf->checksum = ~icmpv6_csum(IPV6_PROTO_NUM_ICMPV6);

    rpl_send_dis_buf = get_rpl_send_dis_buf();

    uint16_t plen = ICMPV6_HDR_LEN + DIS_BASE_LEN;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex);
}


void send_DAO(ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index)
{

	printf("\n ** CALLED FUNCTION send_DAO (DAOs DISABLED -> RETURN) ** \n\n");
	return;

    if (i_am_root) {
        return;
    }

    mutex_lock(&rpl_send_mutex);
    rpl_dodag_t *my_dodag;
    my_dodag = rpl_get_my_dodag();

    if (destination == NULL) {
        destination = &my_dodag->my_preferred_parent->addr;
    }
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("send DAO to %s (IPv6: ", ipv6_addr_to_str(addr_str, destination));


    if (default_lifetime) {
        lifetime = my_dodag->default_lifetime;
    }

    icmp_send_buf  = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DAO;
    icmp_send_buf->checksum = ~icmpv6_csum(IPV6_PROTO_NUM_ICMPV6);

    if (my_dodag == NULL) {
        mutex_unlock(&rpl_send_mutex);
        return;
    }

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
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex);

    if (continue_index > 1) {
        send_DAO(destination, lifetime, default_lifetime, continue_index);
    }
}

void send_DAO_ACK(ipv6_addr_t *destination)
{

	char addr_str[IPV6_MAX_ADDR_STR_LEN];
	printf("send DAO-ACK to %s (IPv6: ", ipv6_addr_to_str(addr_str, destination));

    #if ENABLE_DEBUG
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("%s\n", ipv6_addr_to_str(addr_str, destination));
    #endif    
    rpl_dodag_t *my_dodag;
    my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        return;
    }

    mutex_lock(&rpl_send_mutex);
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_DAO_ACK;
    icmp_send_buf->checksum = ~icmpv6_csum(IPV6_PROTO_NUM_ICMPV6);

    rpl_send_dao_ack_buf = get_rpl_send_dao_ack_buf();
    rpl_send_dao_ack_buf->rpl_instanceid = my_dodag->instance->id;
    rpl_send_dao_ack_buf->d_reserved = 0;
    rpl_send_dao_ack_buf->dao_sequence = my_dodag->dao_seq;
    rpl_send_dao_ack_buf->status = 0;

    uint16_t plen = ICMPV6_HDR_LEN + DAO_ACK_LEN;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex);
}

//trail send tvo
void send_TVO(ipv6_addr_t * destination, struct rpl_tvo_t * tvo, rpl_tvo_signature_t * signature){

//	memcpy(&tvo_resend, tvo, sizeof(*tvo));
//	memcpy(&tvo_resend.dst_addr, destination, sizeof(*destination));

//	ipv6_addr_t mcast;
//	ipv6_addr_set_all_nodes_addr(&mcast);
//	ipv6_addr_set_all_nodes_addr(destination);

	char addr_str[IPV6_MAX_ADDR_STR_LEN];
	//printf("send TVO (seq: %u) to %s (IPv6: ", tvo->tvo_seq , ipv6_addr_to_str(addr_str, destination));
	//printf("send TVO to *ID %u* (seq: %u) (", destination->uint8[15], tvo->tvo_seq);

	if(tvo->s_flag){ //send TVO downwards
		printf("m: ID %u send msg TVO to ID %u #color2 - Seq. %u\n", my_address.uint8[15], destination->uint8[15], tvo->tvo_seq);
	}
	else { //send TVO upwards
		printf("m: ID %u send msg TVO to ID %u #color1 - Seq. %u\n", my_address.uint8[15], destination->uint8[15], tvo->tvo_seq);
	}
	mutex_lock(&rpl_send_mutex);
	rpl_dodag_t * mydodag;

	icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

	mydodag = rpl_get_my_dodag();

	/*
	if(mydodag == NULL){
		printf("Error, trying to send TVO without being part of a dodag. This should not happen\n");
		mutex_unlock(&rpl_send_mutex);
		return;
	}
	*/
	//get size of signature
	//int size = (sizeof(tvo->signature.uint32)) ; //devided by 4: 32 bit, 4 byte
	//size = size / 4;

	icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
	icmp_send_buf->code = ICMP_CODE_TVO;
	icmp_send_buf->checksum = ~icmpv6_csum(IPV6_PROTO_NUM_ICMPV6);

	rpl_send_tvo_buf = get_rpl_send_tvo_buf();
	memset(rpl_send_tvo_buf, 0, sizeof(*rpl_send_tvo_buf));
	memcpy(rpl_send_tvo_buf, tvo, sizeof(*tvo));

	// calculate the base length of the tvo
	uint16_t tvo_base_length = sizeof(*tvo);
//	printf("(TEST DEBUG) ***** base length calculated: %u / counted %u\n\n",tvo_base_length, TVO_BASE_LEN);

	uint8_t size_signature = 0;

	if(signature != NULL && rpl_send_tvo_buf->s_flag){
		size_signature = sizeof(rpl_tvo_signature_t);// + RPL_OPT_LEN;
		rpl_send_tvo_signature_buf = get_rpl_send_tvo_signature_buf(TVO_BASE_LEN);
		memcpy(rpl_send_tvo_signature_buf, signature, size_signature);
//		printf("(TEST DEBUG) send TVO: signature=%u / rpl_send_tvo_signature_buf=%u / size=%u\n\n",signature->uint8[0], rpl_send_tvo_signature_buf->uint8[0], size_signature);
	}

//	timex_t now;
//	vtimer_now(&now);
//	uint32_t timestamp = now.microseconds;

	uint16_t plen = ICMPV6_HDR_LEN + size_signature + tvo_base_length;// + (rpl_send_tvo_buf->srh_list_size * sizeof(srh_list_t));
//	printf("\n(TEST DEBUG) %u = %u + %u + %u\n\n",plen, ICMPV6_HDR_LEN, size_signature, tvo_base_length);
//	printf("%u %u %u\n",timestamp, rpl_send_tvo_buf->tvo_seq, plen);
	rpl_send(destination,(uint8_t*)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6, NULL);

	mutex_unlock(&rpl_send_mutex);

}

void send_TVO_ACK(ipv6_addr_t *destination, uint8_t sequence_number)
{
	char addr_str[IPV6_MAX_ADDR_STR_LEN];
	//printf("send TVO-ACK (seq: %u) to %s (IPv6: ", sequence_number, ipv6_addr_to_str(addr_str, destination));
	//printf("send TVO-ACK to *ID %u* (seq: %u) (", destination->uint8[15], sequence_number);

	printf("m: ID %u send msg TVO_ACK to ID %u #color3 - Seq. %u\n", my_address.uint8[15],destination->uint8[15], sequence_number);

    rpl_dodag_t *my_dodag;
    my_dodag = rpl_get_my_dodag();

    if (my_dodag == NULL) {
        return;
    }

    mutex_lock(&rpl_send_mutex);
    icmp_send_buf = get_rpl_send_icmpv6_buf(ipv6_ext_hdr_len);

    icmp_send_buf->type = ICMPV6_TYPE_RPL_CONTROL;
    icmp_send_buf->code = ICMP_CODE_TVO_ACK;
    icmp_send_buf->checksum = ~icmpv6_csum(IPV6_PROTO_NUM_ICMPV6);

    rpl_send_tvo_ack_buf = get_rpl_send_tvo_ack_buf();
    rpl_send_tvo_ack_buf->rpl_instanceid = my_dodag->instance->id;
    rpl_send_tvo_ack_buf->tvo_seq = sequence_number;
    rpl_send_tvo_ack_buf->status = 0;

    uint16_t plen = ICMPV6_HDR_LEN + TVO_ACK_LEN;
    rpl_send(destination, (uint8_t *)icmp_send_buf, plen, IPV6_PROTO_NUM_ICMPV6, NULL);
    mutex_unlock(&rpl_send_mutex);
}


void rpl_process(void)
{

    msg_t m_recv;
    msg_init_queue(msg_queue, RPL_PKT_RECV_BUF_SIZE);

    while (1) {
        msg_receive(&m_recv);
        mutex_lock(&rpl_recv_mutex);
        uint8_t *code;
        code = ((uint8_t *)m_recv.content.ptr);
        /* differentiate packet types */
        ipv6_buf = ipv6_get_buf();

        // trail: for SAFEST DEMO ONLY - ignore root for "ideal" network (delete if-else)
       // printf("\n\nSrc.adr: %u ... ignore_root_addr: %u\n\n",ipv6_buf->srcaddr.uint8[15], (uint8_t)ignore_root_addr);
        if(ipv6_buf->srcaddr.uint8[15] == (uint8_t)ignore_root_addr){
        	//printf("Ignore node RPL message from node %u\n",ignore_root_addr);
        	mutex_unlock(&rpl_recv_mutex);
        	//return;
        }
        else {

			memcpy(&rpl_buffer, ipv6_buf, ipv6_buf->length + IPV6_HDR_LEN);

	//        mutex_lock(&rpl_recv_mutex);

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

				case (ICMP_CODE_TVO): {
					recv_rpl_tvo();
					mutex_unlock(&rpl_recv_mutex);
					break;
				 }

				case (ICMP_CODE_TVO_ACK): {
					recv_rpl_tvo_ack();
					mutex_unlock(&rpl_recv_mutex);
					break;
				 }

				default:
					mutex_unlock(&rpl_recv_mutex);
					break;
			}
        }
    }
}

// trail receive tvo
void recv_rpl_tvo(void){

	ipv6_addr_t ll_address;
	ipv6_addr_t my_address;
	ipv6_addr_set_link_local_prefix(&ll_address);
	ipv6_iface_get_best_src_addr(&my_address, &ll_address);

	ipv6_addr_t * next_hop;
	ipv6_buf = get_rpl_ipv6_buf();
	rpl_tvo_buf = get_rpl_tvo_buf();


	char addr_str[IPV6_MAX_ADDR_STR_LEN];
	//printf("received TVO (seq: %u) from %s (IPv6)\n", rpl_tvo_buf->tvo_seq, ipv6_addr_to_str(addr_str, &(ipv6_buf->srcaddr)));
	//printf("received TVO from *ID %u* (seq: %u)\n", ipv6_buf->srcaddr.uint8[15], rpl_tvo_buf->tvo_seq);
//	printf("m: ID %u received msg TVO(%u) from ID %u #color2\n", my_address.uint8[15], rpl_tvo_buf->tvo_seq, ipv6_buf->srcaddr.uint8[15]);

	//send_TVO_ACK(&(ipv6_buf->srcaddr), rpl_tvo_buf->tvo_seq);

	if (rpl_tvo_buf->s_flag) {
	        rpl_opt_buf = get_rpl_opt_buf(TVO_BASE_LEN);
	        rpl_tvo_signature_buf = get_tvo_signature_buf(TVO_BASE_LEN);
	 }

	 struct rpl_tvo_local_t * local_tvo = has_tvo_been_received(&ipv6_buf->srcaddr, rpl_tvo_buf->tvo_seq);


	 if(local_tvo != NULL) // already received
	 {
		 /**
		  * refactor: eigentlich wird nur tvo_seq benötigt. Zweiten array mit addr_suffix und seq number
		  * suche nach tupel
		  * sende ack oder sonst speicher zum forwarden
		  * --> tvo struct muss nur zum forwarden gespeichert werden
		  * --> zum acken könnten so wesentlich mehr vorenthalten werden
		  * --> acken und forwarden wäre entkoppelt -> "löschen, da ack erhalten" hat dann nichts mehr mit
		  *  "sollte die TVO noch acken" zu tun.
		  */
		// printf("\n Already received TVO (seq: %u) from %s\n", rpl_tvo_buf->tvo_seq, ipv6_addr_to_str(addr_str, &(ipv6_buf->srcaddr)));

		 printf("m: ID %u received msg TVO from ID %u #color10 - Seq. %u\n", my_address.uint8[15], ipv6_buf->srcaddr.uint8[15], rpl_tvo_buf->tvo_seq);
		 send_TVO_ACK(&(ipv6_buf->srcaddr), rpl_tvo_buf->tvo_seq);
		 return;
	 }

	rpl_dodag_t *my_dodag = rpl_get_my_dodag();
	if(my_dodag == NULL){
	}

	if(rpl_tvo_buf->s_flag){ //response

		printf("m: ID %u received msg TVO from ID %u #color9 - Seq. %u\n", my_address.uint8[15], ipv6_buf->srcaddr.uint8[15], rpl_tvo_buf->tvo_seq);

		if(rpl_equal_id(&rpl_tvo_buf->src_addr, &my_address)){

			//am I the source?
			printf("*TVO origin* checking signature ... ");

			uint8_t trail_index;
			trail_index = get_parent_from_trail_buffer(&(ipv6_buf->srcaddr));
			if(trail_index == 255){
				printf("parent is not in list -> already verified... \n");
				send_TVO_ACK(&(ipv6_buf->srcaddr), rpl_tvo_buf->tvo_seq);
				return;
			}

			if(rpl_tvo_signature_buf->uint8[0] != 0){ //TODO any signature-dummy is OK
				printf("**valid**\n");

				//finish joining/update
				rpl_dodag_t dio_dodag;
				memcpy(&dio_dodag, &trail_parent_buffer[trail_index], sizeof(dio_dodag));
				dio_dodag.instance = rpl_get_instance(trail_parent_buffer[trail_index].instance_id);
				if(dio_dodag.instance == NULL){
					printf("\n(TEST DEBUG) dio_dodag.instance is NULL! .. trail_buf id: %u\n", trail_parent_buffer[trail_index].instance_id);
				}

				my_dodag = rpl_get_my_dodag();

				if(my_dodag != NULL && my_dodag->my_rank <= trail_parent_buffer[trail_index].parent_rank){
					printf("IGNORING TVO DUE TO RANK: my rank %u , parent rank %u\n", my_dodag->my_rank, trail_parent_buffer[trail_index].parent_rank);
					trail_parent_buffer[trail_index].in_progress = 0; // free buffer
					send_TVO_ACK(&(ipv6_buf->srcaddr), rpl_tvo_buf->tvo_seq);
					return;
				}

				join_dodag(&dio_dodag, &trail_parent_buffer[trail_index].parent_addr, trail_parent_buffer[trail_index].parent_rank, trail_parent_buffer[trail_index].parent_dtsn);

			//	trail_parent_buffer[trail_index].verified = 1; //verified
			//	trail_parent_buffer[trail_index].pending = 0;
				trail_parent_buffer[trail_index].in_progress = 0; // free buffer

				send_TVO_ACK(&(ipv6_buf->srcaddr), rpl_tvo_buf->tvo_seq);
				return;
			}
			else{
				printf(" **invalid**\n");
			//	trail_parent_buffer[trail_index].verified = 0; // not verfied
			//	trail_parent_buffer[trail_index].pending = 0;
				trail_parent_buffer[trail_index].in_progress = 0; //free buffer

				send_TVO_ACK(&(ipv6_buf->srcaddr), rpl_tvo_buf->tvo_seq);
				return;
			}
		}
		else{
			/*
			 * Received tvo on way back
			 */
			next_hop = rpl_get_next_hop(&rpl_tvo_buf->src_addr);
		}
	}
	else{
		/*
		 * received tvo on way to root
		 */
		printf("m: ID %u received msg TVO from ID %u #color8 - Seq. %u\n", my_address.uint8[15], ipv6_buf->srcaddr.uint8[15], rpl_tvo_buf->tvo_seq);

		//TVO is a request: on the way to the root
		if(my_dodag == NULL){
			printf("** Not in network, yet - dropping TVO **\n");
			//send_TVO_ACK(&(ipv6_buf->srcaddr), rpl_tvo_buf->tvo_seq);
			return;
		}
		//am I tested? (rank == 0)
		else if(rpl_tvo_buf->rank == 0){
			printf("Include rank (%u) into TVO \n", my_dodag->my_rank);
			// tested: set to my rank
			rpl_tvo_buf->rank = my_dodag->my_rank;
		}
		// not tested -> is rank OK?
		else if(rpl_tvo_buf->rank <= my_dodag->my_rank) {
			// not OK -> DROP
			printf("** TVO contains invalid rank: %u **\n", rpl_tvo_buf->rank);
			send_TVO_ACK(&(ipv6_buf->srcaddr), rpl_tvo_buf->tvo_seq);
			return;
		}

		// delete first in case a better entry is available
		rpl_del_routing_entry(&rpl_tvo_buf->src_addr);
		rpl_del_routing_entry(&ipv6_buf->srcaddr);

		//add downward routing entry to send TVO back to source
		rpl_add_routing_entry(&rpl_tvo_buf->src_addr, &ipv6_buf->srcaddr, 1000);
		//add downward routing entry for next hop (one-hop neighbor)
		rpl_add_routing_entry(&ipv6_buf->srcaddr, &ipv6_buf->srcaddr, 1000);

		//rank OK, NOT tested -> continue
		// am I root?
		if(i_am_root){
			rpl_tvo_signature_buf = get_tvo_signature_buf(TVO_BASE_LEN);
			memset(rpl_tvo_signature_buf, 0, sizeof(*rpl_tvo_signature_buf));
			printf("Signing TVO ... ");
			rpl_tvo_signature_buf->uint8[0] = 123;
			printf("done\n");

			rpl_tvo_buf->s_flag = 1;
			next_hop = rpl_get_next_hop(&rpl_tvo_buf->src_addr);
		}
		else{
			//not root: forward to preferred parent
			next_hop = &my_dodag->my_preferred_parent->addr;
		}
	}

	tvo_sequence_number++;
	struct rpl_tvo_local_t tvo_inst;
	local_tvo = &tvo_inst;
	// copy tvo to local_tvo
	memset(local_tvo, 0, sizeof(*local_tvo));
	memcpy(local_tvo, rpl_tvo_buf, sizeof(*rpl_tvo_buf));
	// assign his_counter (tvo.seqnr) to local tvo.his_seq_num
	local_tvo->his_tvo_seq = local_tvo->tvo_seq;
	local_tvo->number_resend = 0;
	// assign tvo_seq_number to local tvo Seq number
	local_tvo->tvo_seq = tvo_sequence_number;
	rpl_tvo_buf->tvo_seq = tvo_sequence_number;
	// give local tvo a timestamp
	timex_t now;
	vtimer_now(&now);
	local_tvo->timestamp_received = now.microseconds;
	//save destination / source
	local_tvo->prev_hop_addr = ipv6_buf->srcaddr;

	memcpy(&(local_tvo->dst_addr), next_hop, sizeof(local_tvo->dst_addr));
	// save_tvo_locally(local_tvo);
	save_tvo_locally(local_tvo);
	// send ack?

	send_TVO_ACK(&(ipv6_buf->srcaddr), local_tvo->his_tvo_seq);

	send_TVO(next_hop, rpl_tvo_buf, rpl_tvo_signature_buf);
	delay_tvo(DEFAULT_WAIT_FOR_TVO_ACK);
}


void recv_rpl_tvo_ack(void)
{

    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    ipv6_buf = get_rpl_ipv6_buf();
    rpl_tvo_ack_buf = get_rpl_tvo_ack_buf();

    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    //printf("*** received TVO-ACK (seq: %u) from %s\n", rpl_tvo_ack_buf->tvo_seq ,ipv6_addr_to_str(addr_str, &(ipv6_buf->srcaddr)));
   // printf("*** received TVO-ACK from *ID %u* (seq: %u)\n", ipv6_buf->srcaddr.uint8[15], rpl_tvo_ack_buf->tvo_seq);
    printf("m: ID %u received msg TVO_ACK from ID %u #color11 - Seq. %u\n", my_address.uint8[15],  ipv6_buf->srcaddr.uint8[15], rpl_tvo_buf->tvo_seq);


    if (rpl_tvo_ack_buf->status != 0) {
        return;
    }

  //  received_tvo_ack();
    tvo_ack_has_been_received(&ipv6_buf->srcaddr, rpl_tvo_ack_buf->tvo_seq);

}


void recv_rpl_dio(void)
{

    ipv6_buf = get_rpl_ipv6_buf();
    rpl_dio_buf = get_rpl_dio_buf();
    int len = DIO_BASE_LEN;

    ////// TEST
    rpl_dodag_t *mydodag;
    mydodag = rpl_get_my_dodag();
    uint16_t myRank;
    char addr_str[IPV6_MAX_ADDR_STR_LEN];

      if (mydodag == NULL) {
    	  myRank = 0;
      }
      else{
    	  myRank = mydodag->my_rank;
    	  if(attacker == 0 && rpl_dio_buf->rank >= myRank){
    		  //printf("received DIO (rank: %u / my rank: %u / inst: %u) from %s (IPv6)\n", rpl_dio_buf->rank, myRank,rpl_dio_buf->rpl_instanceid ,ipv6_addr_to_str(addr_str, &(ipv6_buf->srcaddr)));
    		  //printf("received DIO from *ID %u* (rank: %u / my rank: %u / inst: %u)\n", ipv6_buf->srcaddr.uint8[15], rpl_dio_buf->rank, myRank,rpl_dio_buf->rpl_instanceid);
    		 // printf(" ---> ignoring DIO due to greater/equal rank \n");
    		  printf("m: ID %u received msg DIO from ID %u #color7 - Rank %u\n", my_address.uint8[15],ipv6_buf->srcaddr.uint8[15], rpl_dio_buf->rank);
    		  return;
    	  }
      }

    //printf("received DIO (rank: %u / my rank: %u / inst: %u) from %s (IPv6)\n", rpl_dio_buf->rank, myRank, rpl_dio_buf->rpl_instanceid,ipv6_addr_to_str(addr_str, &(ipv6_buf->srcaddr)));
     // printf("received DIO from *ID %u* (rank: %u / my rank: %u / inst: %u)\n", ipv6_buf->srcaddr.uint8[15], rpl_dio_buf->rank, myRank,rpl_dio_buf->rpl_instanceid);
    printf("m: ID %u received msg DIO from ID %u #color6  - Rank %u\n", my_address.uint8[15], ipv6_buf->srcaddr.uint8[15], rpl_dio_buf->rank);
    uint8_t trail_index;
    uint8_t flag_send_TVO = 0;
    if(do_trail){

		//check if node in Routing table with *that* rank
		if(is_parent_verified(&(ipv6_buf->srcaddr), rpl_dio_buf->rank)){
			//trail_index = get_parent_from_trail_buffer(&(ipv6_buf->srcaddr));
			//set parent "verified" in trail_parent_buffer
			//if(trail_index == 255){
			//	printf("ERROR: parent is not in list, but verified -> something is wrong... \n");
			//	return;
			//}
			//trail_parent_buffer[trail_index].verified = 1;
			//trail_parent_buffer[trail_index].pending = 0;
			flag_send_TVO = 0;
		} else {
			trail_index = get_parent_from_trail_buffer(&(ipv6_buf->srcaddr));
			if(trail_index == 255){
				//parent not in buffer
				// -> not waiting for TVO: send new TVO!
				trail_index = include_parent_into_trail_buffer();
				if(trail_index == 255){

					printf("ERROR: trail buffer is full ... \n");
					return;
				}
				flag_send_TVO = 1;
				//trail_parent_buffer[trail_index].verified = 0;
				//trail_parent_buffer[trail_index].pending = 1;
				//printf("set TRAIL buffer at %u to verified: %u and pending: %u\n",trail_index,trail_parent_buffer[trail_index].verified, trail_parent_buffer[trail_index].pending);
			}
			else {

				//parent possibly chose different rank
				if(rpl_dio_buf->rank != trail_parent_buffer[trail_index].parent_rank){
					//re-schedule TVO
					flag_send_TVO = 1;
				}
				else {
					// parent in buffer: waiting for tvo: don't send new one.
					printf("still waiting for verification... ignoring DIO \n");
					//send_tvo = 0;
					return;
				}

			}

		}
    }

    rpl_instance_t *dio_inst = rpl_get_instance(rpl_dio_buf->rpl_instanceid);
    rpl_instance_t *my_inst = rpl_get_my_instance();

    if (dio_inst == NULL) {
        if (my_inst != NULL) {
            /* already part of a DODAG -> impossible to join other instance */
        	printf("dio_inst == NULL && my_inst != NULL -> ALREADY IN DODDAG, RETURN\n");
            return;
        }


        dio_inst = rpl_new_instance(rpl_dio_buf->rpl_instanceid);//rpl_new_instance(rpl_dio_buf->rpl_instanceid);

        if (dio_inst == NULL) {
        	printf("dio_inst == NULL && my_inst == NULL -> dio_inst still NULL, RETURN\n");
            return;
        }
    }
    else if (my_inst != NULL && my_inst->id != dio_inst->id) {
        /* TODO: Add support support for several instances.  */

        /* At the moment, nodes can only join one instance, this is
        * the instance they join first.
        * Instances cannot be switched later on.  */

        DEBUG("Ignoring instance - we are %d and got %d\n", my_inst->id, dio_inst->id);
        printf("Ignoring instance - we are %d and got %d\n", my_inst->id, dio_inst->id);
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

    if(do_trail && flag_send_TVO){
		memcpy(&trail_parent_buffer[trail_index].dodag_id, &rpl_dio_buf->dodagid, sizeof(dio_dodag.dodag_id));
		trail_parent_buffer[trail_index].dtsn = rpl_dio_buf->dtsn;
		trail_parent_buffer[trail_index].mop = ((rpl_dio_buf->g_mop_prf >> RPL_MOP_SHIFT) & RPL_SHIFTED_MOP_MASK);
		trail_parent_buffer[trail_index].grounded = rpl_dio_buf->g_mop_prf >> RPL_GROUNDED_SHIFT;
		trail_parent_buffer[trail_index].prf = (rpl_dio_buf->g_mop_prf & RPL_PRF_MASK);
		trail_parent_buffer[trail_index].version = rpl_dio_buf->version_number;
		trail_parent_buffer[trail_index].instance = dio_inst;
		trail_parent_buffer[trail_index].instance_id = rpl_dio_buf->rpl_instanceid;
    }
    /* Parse until all options are consumed.
     * ipv6_buf->length contains the packet length minus ipv6 and
     * icmpv6 header, so only ICMPV6_HDR_LEN remains to be
     * subtracted.  */
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

            case (RPL_OPT_ROUTE_INFO): {
                len += rpl_opt_buf->length + 2;
                break;
            }

            case (RPL_OPT_DODAG_CONF): {
                has_dodag_conf_opt = 1;

                if (rpl_opt_buf->length != RPL_OPT_DODAG_CONF_LEN) {
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

                if(do_trail && flag_send_TVO){
					trail_parent_buffer[trail_index].dio_interval_doubling = rpl_opt_dodag_conf_buf->DIOIntDoubl;
					trail_parent_buffer[trail_index].dio_min = rpl_opt_dodag_conf_buf->DIOIntMin;
					trail_parent_buffer[trail_index].dio_redundancy = rpl_opt_dodag_conf_buf->DIORedun;
					trail_parent_buffer[trail_index].maxrankincrease =	rpl_opt_dodag_conf_buf->MaxRankIncrease;
					trail_parent_buffer[trail_index].minhoprankincrease = rpl_opt_dodag_conf_buf->MinHopRankIncrease;
					trail_parent_buffer[trail_index].default_lifetime = rpl_opt_dodag_conf_buf->default_lifetime;
					trail_parent_buffer[trail_index].lifetime_unit = rpl_opt_dodag_conf_buf->lifetime_unit;
					trail_parent_buffer[trail_index].of = (struct rpl_of_t *) rpl_get_of_for_ocp(rpl_opt_dodag_conf_buf->ocp);
                }
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
            return;
        }

        if (rpl_dio_buf->rank < ROOT_RANK) {
            DEBUG("DIO with Rank < ROOT_RANK\n");
            return;
        }

        if (dio_dodag.mop != RPL_DEFAULT_MOP) {
            DEBUG("Required MOP not supported\n");
            return;
        }

        if (dio_dodag.of == NULL) {
            DEBUG("Required objective function not supported\n");
            return;
        }


        if (rpl_dio_buf->rank != INFINITE_RANK) {

        }
        else {
            DEBUG("Cannot access DODAG because of DIO with infinite rank\n");
            return;
        }
    }

    //if(tvo_pending && do_trail){ //do_trail enables TRAIL
    //if(trail_parent_buffer[trail_index].verified == 0 && trail_parent_buffer[trail_index].pending == 1 && do_trail){
    if(do_trail && flag_send_TVO){

    //	trail_parent_buffer[trail_index].pending = 0;

		printf("Parent's rank %u unverified .. initializing TRAIL\n", rpl_dio_buf->rank);
		struct rpl_tvo_t tvo;
		printf("TVO created\n");
		rpl_tvo_auto_init(&tvo, rpl_dio_buf->rpl_instanceid, rpl_dio_buf->version_number);
		printf("TVO initialized\n");

		ipv6_addr_t next_hop;
		memcpy(&next_hop, &(ipv6_buf->srcaddr), sizeof(next_hop));
		rpl_add_routing_entry(&ipv6_buf->srcaddr, &ipv6_buf->srcaddr, 1000);
	//	ipv6_addr_set_all_nodes_addr(&next_hop);

		//////////
		struct rpl_tvo_local_t tvo_inst;
		// copy tvo to local_tvo
		memset(&tvo_inst, 0, sizeof(tvo_inst));
		memcpy(&tvo_inst, &tvo, sizeof(tvo_inst));
		// assign his_counter (tvo.seqnr) to local tvo.his_seq_num
		tvo_inst.his_tvo_seq = 0;
		tvo_inst.number_resend = 0;
		// give local tvo a timestamp
		timex_t now;
		vtimer_now(&now);
		tvo_inst.timestamp_received = now.microseconds;
		//save destination / source
		memcpy(&(tvo_inst.dst_addr), &ipv6_buf->srcaddr, sizeof(tvo_inst.dst_addr));
		// save_tvo_locally(local_tvo);
		save_tvo_locally(&tvo_inst);
		/////////

		trail_parent_buffer[trail_index].parent_addr = ipv6_buf->srcaddr;
		trail_parent_buffer[trail_index].parent_rank = rpl_dio_buf->rank;
		trail_parent_buffer[trail_index].parent_dtsn = rpl_dio_buf->dtsn;

		srand(now.microseconds);
		uint32_t random = rand() % 1000000;
		printf(" ** Delay TVO verification for %u ms\n",(random/1000));
		timex_t time = timex_set(0, random);
		vtimer_sleep(time);

		delay_tvo(DEFAULT_WAIT_FOR_TVO_ACK);
		send_TVO(&next_hop, &tvo, NULL);
		return;
    }
    //else if(trail_parent_buffer[trail_index].verified == 1 || !do_trail){
    //	join_dodag(&dio_dodag, &ipv6_buf->srcaddr, rpl_dio_buf->rank, rpl_dio_buf->dtsn);
    //}

    join_dodag(&dio_dodag, &ipv6_buf->srcaddr, rpl_dio_buf->rank, rpl_dio_buf->dtsn);
}



void recv_rpl_dis(void)
{
//	printf("***** receive DIS *****\n");
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

    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("received DAO from %s (IPv6)\n", ipv6_addr_to_str(addr_str, &(ipv6_buf->srcaddr)));

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
       // delay_dao();//trail disable downward routes
    }
}

void recv_rpl_dao_ack(void)
{

    rpl_dodag_t *my_dodag = rpl_get_my_dodag();

    ipv6_buf = get_rpl_ipv6_buf();

    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("received DAO-ACK from %s (IPv6)\n", ipv6_addr_to_str(addr_str, &(ipv6_buf->srcaddr)));

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

/* TODO: tcp_socket unused? */
void rpl_send(ipv6_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t next_header, void *tcp_socket)
{

	char addr_str[IPV6_MAX_ADDR_STR_LEN];

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

    /* The packet was "assembled" in rpl.c. Therefore rpl_send_buf was used.
     * Therefore memcpy is not needed because the payload is at the
     * right memory location already. */

    if (p_ptr != payload) {
        memcpy(p_ptr, payload, p_len);
    }

    packet_length = IPV6_HDR_LEN + p_len;

    if (ipv6_addr_is_multicast(&ipv6_send_buf->destaddr)) {
    	 //printf("(TEST DEBUG) send RPL packet (multicast) payload: %u / packet: %u to %x\n",p_len, packet_length, ipv6_send_buf->destaddr.uint16[4]);
    //	printf("multicast, %u bytes)\n",packet_length);
        sixlowpan_lowpan_sendto((ieee_802154_long_t *) &(ipv6_send_buf->destaddr.uint16[4]), 
                                (uint8_t *)ipv6_send_buf,
                                packet_length);
    }
    else {
        /* find appropriate next hop before sending v*/
        ipv6_addr_t *next_hop = rpl_get_next_hop(&ipv6_send_buf->destaddr);

        if (next_hop == NULL) {
            if (i_am_root) {
                DEBUG("[Error] destination unknown\n");
                printf("\n");
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

    //    printf("(TEST DEBUG) send RPL packet (unicast) payload: %u / packet: %u to %x\n",p_len, packet_length, next_hop->uint16[4]);
    //    printf("unicast, %u bytes)\n", packet_length);
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

    return NULL;
}

void rpl_add_routing_entry(ipv6_addr_t *addr, ipv6_addr_t *next_hop, uint16_t lifetime)
{
    rpl_routing_entry_t *entry = rpl_find_routing_entry(addr);
    char addr_str[IPV6_MAX_ADDR_STR_LEN];

    if (entry != NULL) {
 //   	printf("Update downward route to %s (IPv6)\n", ipv6_addr_to_str(addr_str, addr));
        entry->lifetime = lifetime;
        return;
    }

 //   printf("Include downward route to %s (IPv6)\n", ipv6_addr_to_str(addr_str, addr));

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
	char addr_str[IPV6_MAX_ADDR_STR_LEN];
    for (uint8_t i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (routing_table[i].used && rpl_equal_id(&routing_table[i].address, addr)) {
        	//printf("Delete downward route to %s (IPv6)\n", ipv6_addr_to_str(addr_str, addr));
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
