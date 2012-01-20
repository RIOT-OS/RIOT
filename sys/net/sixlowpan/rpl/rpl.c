#include <string.h>
#include <vtimer.h>
#include "rpl.h"

#include "sys/net/sixlowpan/sixlowmac.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowpan.h"

//global variables for own rpl status:
bool root = false;
ipv6_addr_t dodagid;
uint8_t rpl_instance_id;

//counters
uint8_t act_dodag_version = 240;
uint8_t dao_sequence = 240;
uint8_t path_sequence = 240;
//other global variables

//ipv6_addr_t parents[10];
//rpl_instance_t instances[5];
uint16_t packet_length;

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

void rpl_init(){
	//TODO: initialize trickle
	turn_on_rpl_handler();
	

}

void rpl_init_root(){
	rpl_instance_t *inst;
	rpl_dodag_t *dodag;

	inst = rpl_new_instance(RPL_DEFAULT_INSTANCE);
	inst->id = RPL_DEFAULT_INSTANCE;

	dodag = rpl_new_dodag(RPL_DEFAULT_INSTANCE);
	if(dodag != NULL) {
		ipv6_addr_t id;
		ipv6_addr_t mcast;
		ipv6_set_all_nds_mcast_addr(&mcast);
		ipv6_get_saddr(&id, &mcast);
		
		//TODO: dodag->of = 
		dodag->dodag_id = id;
		dodag->instance = inst;
		dodag->mop = RPL_DEFAULT_MOP;
		dodag->dtsn = 1;
		dodag->dio_interval_doubling = DEFAULT_DIO_INTERVAL_DOUBLINGS;
		dodag->dio_min = DEFAULT_DIO_INTERVAL_MIN;
		dodag->dio_redundancy = DEFAULT_DIO_REDUNDANCY_CONSTANT;
		dodag->maxrankincrease = 0;
		dodag->minhoprankincrease = DEFAULT_MIN_HOP_RANK_INCREASE;
		dodag->default_lifetime = RPL_DEFAULT_LIFETIME;
		dodag->lifetime_unit = RPL_LIFETIME_UNIT;
		dodag->version = 0;
		dodag->grounded = RPL_GROUNDED;
		dodag->my_rank = RPL_ROOT_RANK;
		dodag->joined = 1;
		dodag->my_preferred_parent = NULL;
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
	rpl_dio_buf->g_mop_prf = mydodag->grounded | (mydodag->mop << RPL_MOP_SHIFT);
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
	//ipv6_buf->length = ICMPV6_HDR_LEN + DIO_BASE_LEN;// + RPL_OPT_LEN + RPL_OPT_DODAG_CONF_LEN; 
	lowpan_init((ieee_802154_long_t*)&(ipv6_buf->destaddr.uint16[4]),(uint8_t*)ipv6_buf);
	printf("sent DIO\n");
}

void rpl_icmp_handler(uint8_t code){
	//pakettypen unterscheiden
	switch(code) {
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
			return;
	}
}


void recv_rpl_dio(void){
	ipv6_buf = get_ipv6_buf();
	
	rpl_dio_buf = get_rpl_dio_buf();
	
	int len = DIO_BASE_LEN;
	printf("Received DIO with DODAGID:\n");
	ipv6_print_addr(&rpl_dio_buf->dodagid);
	printf("Rank: %d \n", rpl_dio_buf->rank);
	
	//rpl_get_dodag();
	
	uint8_t mop = (rpl_dio_buf->g_mop_prf >> RPL_MOP_SHIFT ) | 0x7;
	if(mop != RPL_DEFAULT_MOP){
		//not supported yet
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

	

	//handle Packet...
	
	//bin ich schon in einem DODAG?

	//kein DODAG:
	//will ich dem DODAG beitreten?

	//im DODAG:
	//will ich das DODAG wechseln?
	//brauche ich für die entscheidung mehr Infos, die im DIO nicht enthalten waren?
		//DIS an entsprechenden Knoten senden!

	//wechseln:
		//war ich schon zuvor in dem DODAG?
			//ja:
			//Regeln beachten bei Wechsel

			//nein:

	//normal beitreten
	//-> nur in Parents eintragen
	
		
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
	//für mich?
		//ja: verarbeiten
		//nein: weiterleiten

}
