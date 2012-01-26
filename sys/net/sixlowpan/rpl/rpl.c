#include <string.h>
#include <vtimer.h>
#include <thread.h>
#include "msg.h"
#include "rpl.h"
#include "of0.h"

#include "sys/net/sixlowpan/sixlowmac.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowpan.h"

char rpl_process_buf[RPL_PROCESS_STACKSIZE];
//counters
uint8_t act_dodag_version = 240;
uint8_t dao_sequence = 240;
uint8_t path_sequence = 240;
//other global variables

rpl_of_t *objective_functions[NUMBER_IMPLEMENTED_OFS];
unsigned int rpl_process_pid;
uint16_t packet_length;

msg_t msg_queue[RPL_PKT_RECV_BUF_SIZE];

//Unbenutzte buffer auskommentiert, damit keine warnings erscheinen
static struct ipv6_hdr_t* ipv6_buf;
static struct icmpv6_hdr_t* icmp_buf;
static struct rpl_dio_t *rpl_dio_buf;
//static struct rpl_dis_t *rpl_dis_buf;
static struct rpl_dao_t *rpl_dao_buf;
//static struct rpl_dao_ack_t * rpl_dao_ack_buf;
static struct rpl_opt_t *rpl_opt_buf;
static struct rpl_opt_dodag_conf_t * rpl_opt_dodag_conf_buf;


static struct rpl_dio_t* get_rpl_dio_buf(){
	return ((struct rpl_dio_t*)&(buffer[LLHDR_ICMPV6HDR_LEN]));
}

static struct rpl_dao_t* get_rpl_dao_buf(){
	return ((struct rpl_dao_t*)&(buffer[LLHDR_ICMPV6HDR_LEN]));
}
/*static struct rpl_dao_ack_t* get_rpl_dao_ack_buf(){
	return ((struct rpl_dao_ack_t*)&(buffer[LLHDR_ICMPV6HDR_LEN]));
}
static struct rpl_dis_t* get_rpl_dis_buf(){
	return ((struct rpl_dis_t*)&(buffer[LLHDR_ICMPV6HDR_LEN]));
}*/
static struct rpl_opt_t* get_rpl_opt_buf(uint8_t rpl_msg_len){
	return ((struct rpl_opt_t*)&(buffer[LLHDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

static struct rpl_opt_dodag_conf_t* get_rpl_opt_dodag_conf_buf(uint8_t rpl_msg_len){
	return ((struct rpl_opt_dodag_conf_t*)&(buffer[LLHDR_ICMPV6HDR_LEN + rpl_msg_len]));
}

rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp){
	for(uint8_t i; i < NUMBER_IMPLEMENTED_OFS; i++){
		if(ocp == objective_functions[i]->ocp){
			return objective_functions[i];
		}
	}
	return NULL;
}

void rpl_init(){

	//TODO: initialize trickle
	rpl_process_pid = thread_create(rpl_process_buf, RPL_PROCESS_STACKSIZE,
									PRIORITY_MAIN-1, CREATE_STACKTEST,
									rpl_process, "rpl_process");
	objective_functions[0] = rpl_get_of0();
	set_rpl_process_pid(rpl_process_pid);

}

void rpl_init_root(){
	rpl_instance_t *inst;
	rpl_dodag_t *dodag;

	inst = rpl_new_instance(RPL_DEFAULT_INSTANCE);
	if(inst == NULL){
		printf("Error - No memory for another RPL instance\n");
		return;
	}
	inst->id = RPL_DEFAULT_INSTANCE;
	inst->joined = 1;

	ipv6_addr_t id;
	ipv6_addr_t mcast;
	ipv6_set_all_nds_mcast_addr(&mcast);
	ipv6_get_saddr(&id, &mcast);

	dodag = rpl_new_dodag(RPL_DEFAULT_INSTANCE, &id);
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
		dodag->default_lifetime = (uint16_t)RPL_DEFAULT_LIFETIME;
		dodag->lifetime_unit = RPL_LIFETIME_UNIT;
		dodag->version = 0;
		dodag->grounded = RPL_GROUNDED;
		dodag->my_rank = RPL_ROOT_RANK;
		dodag->joined = 1;
		dodag->my_preferred_parent = NULL;
	}	
	else{
		printf("Error - could not generate DODAG\n");
		return;
	}

	send_DIO();

}


void send_DIO(){
	rpl_dodag_t * mydodag;
	ipv6_buf = get_ipv6_buf();
	icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
	packet_length = 0;

	mydodag = rpl_get_my_dodag();
	if(mydodag == NULL){
		printf("Error, trying to send DIO without being part of a dodag. This should not happen\n");
		return;
	}

	icmp_buf->type = ICMP_RPL_CONTROL;
	icmp_buf->code = ICMP_CODE_DIO; 
	ipv6_buf->version_trafficclass = IPV6_VER;
	ipv6_buf->trafficclass_flowlabel = 0;
	ipv6_buf->flowlabel = 0;
	ipv6_buf->nextheader = PROTO_NUM_ICMPV6;
	ipv6_buf->hoplimit = 1;

	ipv6_set_all_nds_mcast_addr(&ipv6_buf->destaddr);
	ipv6_get_saddr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));

	rpl_dio_buf = get_rpl_dio_buf();
	memset(rpl_dio_buf, 0, sizeof(*rpl_dio_buf));
	rpl_dio_buf->rpl_instanceid = mydodag->instance->id;
	rpl_dio_buf->version_number = mydodag->version;
	rpl_dio_buf->rank = mydodag->my_rank;
	rpl_dio_buf->g_mop_prf = (mydodag->grounded << RPL_GROUNDED_SHIFT) | (mydodag->mop << RPL_MOP_SHIFT) | mydodag->prf;
	rpl_dio_buf->dtsn = mydodag->dtsn;
	rpl_dio_buf->flags = 0;
	rpl_dio_buf->reserved = 0;
	rpl_dio_buf->dodagid = mydodag->dodag_id;
	printf("Send DIO with DODAGID: \n");
	ipv6_print_addr(&rpl_dio_buf->dodagid);

	int opt_hdr_len = 0; 
	//DODAG Configuration Option!
	rpl_opt_dodag_conf_buf = get_rpl_opt_dodag_conf_buf(DIO_BASE_LEN);
	rpl_opt_dodag_conf_buf->type = RPL_OPT_DODAG_CONF;
	rpl_opt_dodag_conf_buf->length = RPL_OPT_DODAG_CONF_LEN;
	rpl_opt_dodag_conf_buf->flags_a_pcs = 0;
	rpl_opt_dodag_conf_buf->DIOIntDoubl = mydodag->dio_interval_doubling;
	rpl_opt_dodag_conf_buf->DIOIntMin = mydodag->dio_min;
	rpl_opt_dodag_conf_buf->DIORedun = mydodag->dio_redundancy;
	rpl_opt_dodag_conf_buf->MaxRankIncrease = mydodag->maxrankincrease;
	rpl_opt_dodag_conf_buf->MinHopRankIncrease = mydodag->minhoprankincrease;
	rpl_opt_dodag_conf_buf->ocp = mydodag->of->ocp;
	rpl_opt_dodag_conf_buf->reserved = 0;
	rpl_opt_dodag_conf_buf->default_lifetime = mydodag->default_lifetime;
	rpl_opt_dodag_conf_buf->lifetime_unit = mydodag->lifetime_unit;
	

	opt_hdr_len += RPL_OPT_LEN + RPL_OPT_DODAG_CONF_LEN;
	
	
	ipv6_buf->length = ICMPV6_HDR_LEN + DIO_BASE_LEN + opt_hdr_len;
	lowpan_init((ieee_802154_long_t*)&(ipv6_buf->destaddr.uint16[4]),(uint8_t*)ipv6_buf);
}

void rpl_process(void){

	msg_t m_recv;
	msg_init_queue(msg_queue, RPL_PKT_RECV_BUF_SIZE);

	while(1){
		msg_receive(&m_recv);
		uint8_t *code;
		code = ((uint8_t*)m_recv.content.ptr);
		//pakettypen unterscheiden
		switch(*code) {
			case(ICMP_CODE_DIS):{
				break;
			}
			case(ICMP_CODE_DIO):{
				recv_rpl_dio();
				break;
			}
			case(ICMP_CODE_DAO):{
				recv_rpl_dao();
				break;
			}
			 case(ICMP_CODE_DAO_ACK):{
				break;
			}
			default:
				break;
		}
	}
}


void recv_rpl_dio(void){
	ipv6_buf = get_ipv6_buf();
	
	rpl_dio_buf = get_rpl_dio_buf();
	int len = DIO_BASE_LEN;

	//printf("Rank: %d \n", rpl_dio_buf->rank);
	printf("Try to find instance with id: %d \n", rpl_dio_buf->rpl_instanceid);
	rpl_instance_t * dio_inst = rpl_get_instance(rpl_dio_buf->rpl_instanceid);
	if(dio_inst == NULL){
		dio_inst = rpl_new_instance(rpl_dio_buf->rpl_instanceid);
		if(dio_inst == NULL){
			return;
		}
		printf("Created new instance\n");
	}
	printf("Try to find dodag with id:\n");
	ipv6_print_addr(&rpl_dio_buf->dodagid);
	rpl_dodag_t * dio_dodag = rpl_get_dodag(&rpl_dio_buf->dodagid);
	if(dio_dodag == NULL){
		dio_dodag = rpl_new_dodag(dio_inst->id, &rpl_dio_buf->dodagid);
		if(dio_dodag == NULL){
			return;
		}
		printf("Created new dodag\n");
		dio_dodag->dtsn = rpl_dio_buf->dtsn;
		dio_dodag->mop = ((rpl_dio_buf->g_mop_prf >> RPL_MOP_SHIFT ) & RPL_SHIFTED_MOP_MASK);
		dio_dodag->grounded = rpl_dio_buf->g_mop_prf >> RPL_GROUNDED_SHIFT;
		dio_dodag->prf = (rpl_dio_buf->g_mop_prf & RPL_PRF_MASK);
		dio_dodag->version = rpl_dio_buf->version_number;
	}
	else{
		if(rpl_dio_buf->version_number > dio_dodag->version){
			printf("New Version of dodag\n");
		}
	}
	

	while(len < (ipv6_buf->length - LLHDR_ICMPV6HDR_LEN) ){
		rpl_opt_buf = get_rpl_opt_buf(len);
		switch(rpl_opt_buf->type){

			case(RPL_OPT_PAD1):{
				len += 1;
				break;
			}
			case(RPL_OPT_PADN):{
				len += rpl_opt_buf->length +2;
				break;
			}
			case(RPL_OPT_DAG_METRIC_CONTAINER):{
				len += rpl_opt_buf->length +2;
				break;
			}
			case(RPL_OPT_ROUTE_INFO):{
				len += rpl_opt_buf->length +2;
				break;
			}
			case(RPL_OPT_DODAG_CONF):{
				if(rpl_opt_buf->length != RPL_OPT_DODAG_CONF_LEN){
					//error malformed
				}
				len += RPL_OPT_DODAG_CONF_LEN +2;
				rpl_opt_dodag_conf_buf = get_rpl_opt_dodag_conf_buf(len);	
				
				break;
			}
			case(RPL_OPT_PREFIX_INFO):{
				if(rpl_opt_buf->length != RPL_OPT_PREFIX_INFO_LEN){
					//error malformed
				}
				len += RPL_OPT_PREFIX_INFO_LEN +2;
				break;
			}
			default:
				//unsupported option -> error
				break;
		}
	}

	

	if(dio_dodag->mop != RPL_DEFAULT_MOP){
		printf("Error - required MOP not supported yet\n");
		return;
	}
	//handle Packet...
}

void recv_rpl_dao(void){
	ipv6_buf = get_ipv6_buf();
	
	rpl_dao_buf = get_rpl_dao_buf();
	int len = DAO_BASE_LEN;
	while(len < (ipv6_buf->length - LLHDR_ICMPV6HDR_LEN) ){
		rpl_opt_buf = get_rpl_opt_buf(len);
		switch(rpl_opt_buf->type){

			case(RPL_OPT_PAD1):{
				len += 1;
				break;
			}
			case(RPL_OPT_PADN):{
				len += rpl_opt_buf->length +2;
				break;
			}
			case(RPL_OPT_DAG_METRIC_CONTAINER):{
				len += rpl_opt_buf->length +2;
				break;
			}
			case(RPL_OPT_TARGET):{
				len += rpl_opt_buf->length +2;
				break;
			}
			case(RPL_OPT_TRANSIT):{
				len += rpl_opt_buf->length +2;
				break;
			}
			case(RPL_OPT_TARGET_DESC):{
				len += rpl_opt_buf->length +2;
				break;
			}

			default:
				break;
		}
	}
}
