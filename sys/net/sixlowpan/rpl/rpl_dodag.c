#include <string.h>
#include <stdio.h>

#include "rpl_dodag.h"

rpl_instance_t instances[RPL_MAX_INSTANCES];
rpl_dodag_t dodags[RPL_MAX_DODAGS];
rpl_parent_t parents[RPL_MAX_PARENTS];

rpl_instance_t *rpl_new_instance(uint8_t instanceid){
	rpl_instance_t *inst;
	rpl_instance_t *end ;
	for(inst=&instances[0], end = inst+RPL_MAX_INSTANCES; inst < end;inst++){
		if(inst->used == 0){
			memset(inst, 0, sizeof(*inst));
			return inst;
		}	
	}
	return NULL;
}
rpl_instance_t *rpl_get_instance(uint8_t instanceid){
	for(int i=0;i<RPL_MAX_INSTANCES;i++){
		if( instances[i].used && (instances[i].id == instanceid)){
			return &instances[i];
		}
	}
	return NULL;
}

rpl_instance_t *rpl_get_my_instance(){
	for(int i=0;i<RPL_MAX_INSTANCES;i++){
		if(instances[i].joined){
			return &instances[i];
		}
	}
	return NULL;
}
rpl_dodag_t * rpl_new_dodag(uint8_t instanceid){
	rpl_instance_t * inst;
	inst = rpl_get_instance(instanceid);
	if(inst == NULL){
		printf("No instance found for id %d. This should not happen\n", instanceid);
		return NULL;
	}

	rpl_dodag_t *dodag;
	rpl_dodag_t *end;

	for(dodag= &dodags[0], end=dodag+RPL_MAX_DODAGS; dodag < end; dodag++){
		if( dodag->used == 0){
			memset(dodag, 0,sizeof(*dodag));
			dodag->instance = inst;
			dodag->my_rank = INFINITE_RANK;
			dodag->used = 1;
			return dodag;
		}
	}
	return NULL;

}

rpl_dodag_t *rpl_get_dodag(ipv6_addr_t *id){
	for(int i=0;i<RPL_MAX_DODAGS;i++){
		if( dodags[i].used && (rpl_equal_dodag_id(&dodags[i].dodag_id, id))){
			return &dodags[i];
		}
	}
	return NULL;
}
rpl_dodag_t *rpl_get_my_dodag(){
	for(int i=0;i<RPL_MAX_DODAGS;i++){
		if( dodags[i].joined){
			return &dodags[i];
		}
	}
	return NULL;
}
void rpl_del_dodag(rpl_dodag_t *dodag){

}

void rpl_leave_dodag(rpl_dodag_t * dodag){
}

void rpl_join_dodag(){
}

bool rpl_equal_dodag_id(ipv6_addr_t *id1, ipv6_addr_t *id2){
	for(uint8_t i=0;i<4;i++){
		if(id1->uint32[i] != id2->uint32[i]){
			return false;
		}
	}
	return true;
}
