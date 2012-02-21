#include <string.h>
#include <stdio.h>

#include "rpl_dodag.h"
#include "trickle.h"

rpl_instance_t instances[RPL_MAX_INSTANCES];
rpl_dodag_t dodags[RPL_MAX_DODAGS];
rpl_parent_t parents[RPL_MAX_PARENTS];

rpl_instance_t *rpl_new_instance(uint8_t instanceid){
	rpl_instance_t *inst;
	rpl_instance_t *end ;
	for(inst=&instances[0], end = inst+RPL_MAX_INSTANCES; inst < end;inst++){
		if(inst->used == 0){
			memset(inst, 0, sizeof(*inst));
			inst->used = 1;
			inst->id = instanceid;
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

rpl_dodag_t * rpl_new_dodag(uint8_t instanceid, ipv6_addr_t *dodagid){
	rpl_instance_t * inst;
	inst = rpl_get_instance(instanceid);
	if(inst == NULL){
		printf("Error - No instance found for id %d. This should not happen\n", instanceid);
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
			memcpy(&dodag->dodag_id,dodagid,sizeof(*dodagid));
			return dodag;
		}
	}
	return NULL;

}

rpl_dodag_t *rpl_get_dodag(ipv6_addr_t *id){
	for(int i=0;i<RPL_MAX_DODAGS;i++){
		if( dodags[i].used && (rpl_equal_id(&dodags[i].dodag_id, id))){
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
	memset(dodag, 0, sizeof(*dodag));
}

void rpl_leave_dodag(rpl_dodag_t * dodag){
	dodag->joined = 0;
	dodag->my_preferred_parent = NULL;
	//parents aus Liste löschen
	rpl_delete_all_parents();
	//TODO: Poison mit INFINITE_RANK
}

bool rpl_equal_id(ipv6_addr_t *id1, ipv6_addr_t *id2){
	for(uint8_t i=0;i<4;i++){
		if(id1->uint32[i] != id2->uint32[i]){
			return false;
		}
	}
	return true;

}

rpl_parent_t *rpl_new_parent(rpl_dodag_t *dodag, ipv6_addr_t *address, uint16_t rank){
	rpl_parent_t *parent;
	rpl_parent_t *end;

	for(parent= &parents[0], end=parents+RPL_MAX_PARENTS; parent < end; parent++){
		if(parent->used == 0){
			memset(parent, 0, sizeof(*parent));
			parent->used = 1;
			parent->addr = *address;
			parent->rank = rank;
			parent->dodag = dodag;
			return parent;
		}
	}
	rpl_delete_worst_parent();
	return rpl_new_parent(dodag, address, rank);
}

rpl_parent_t *rpl_find_parent(ipv6_addr_t *address){
	rpl_parent_t *parent;
	rpl_parent_t *end;

	for(parent= &parents[0], end=parents+RPL_MAX_PARENTS; parent < end; parent++){
		if( (parent->used) && (rpl_equal_id(address, &parent->addr)) ){
			return parent;
		}
	}
	return NULL;
}

void rpl_delete_parent(rpl_parent_t * parent){
	rpl_dodag_t * my_dodag = rpl_get_my_dodag();
	//check if this was the preferred parent, find new parent, if it was last parent leave dodag
	char new_preferred_parent = 0;
	if( (my_dodag != NULL) && rpl_equal_id(&my_dodag->my_preferred_parent->addr, &parent->addr) ){
		new_preferred_parent = 1;
		memset(parent,0,sizeof(*parent));
	}
	uint8_t best = 0xFF;
	uint16_t min_rank = 0xFFFF;
	if(new_preferred_parent){
		for(int i=0;i<RPL_MAX_PARENTS;i++){
			if(parents[i].rank < min_rank){
				best = i;
				min_rank = parents[i].rank;
			}
		}
		if(best == 0xFF){
			//we have no more parents for this dodag -> leave dodag;
			//TODO: Erst nach Ablauf eines Timers verlassen, siehe RPL draft 8.2.2.1 DODAG Version
			rpl_leave_dodag(my_dodag);
		}
		my_dodag->my_preferred_parent = &parents[best];
	}
}

void rpl_delete_worst_parent(void){
	uint8_t worst = 0xFF;
	uint16_t max_rank = 0x0000;
	for(int i=0;i<RPL_MAX_PARENTS;i++){
		if(parents[i].rank > max_rank){
			worst = i;
			max_rank = parents[i].rank;
		}
	}
	if(worst == 0xFF){
		//Fehler, keine parents -> sollte nicht passieren
		return;
	}
	rpl_delete_parent(&parents[worst]);

}

void rpl_delete_all_parents(void){
	for(int i=0;i<RPL_MAX_PARENTS;i++){
		memset(&parents[i],0,sizeof(parents[i]));
	}
}

void rpl_join_dodag(rpl_dodag_t *dodag, ipv6_addr_t *parent, uint16_t parent_rank){
	rpl_dodag_t *my_dodag;
	rpl_parent_t *preferred_parent;
	my_dodag = rpl_new_dodag(dodag->instance->id, &dodag->dodag_id);
	if(my_dodag == NULL){
		return;
	}
	preferred_parent = rpl_new_parent(my_dodag, parent, parent_rank);
	if(preferred_parent == NULL){
		rpl_del_dodag(my_dodag);
		return;	
	}
	my_dodag->instance->joined = 1;
	my_dodag->of = dodag->of;
	my_dodag->mop = dodag->mop;
	my_dodag->dtsn = dodag->dtsn;
	my_dodag->prf = dodag->prf;
	my_dodag->dio_interval_doubling = dodag->dio_interval_doubling;
	my_dodag->dio_min = dodag->dio_min;
	my_dodag->dio_redundancy = dodag->dio_redundancy;
	my_dodag->maxrankincrease = dodag->maxrankincrease;
	my_dodag->minhoprankincrease = dodag->minhoprankincrease;
	my_dodag->default_lifetime = dodag->default_lifetime;
	my_dodag->lifetime_unit = dodag->lifetime_unit;
	my_dodag->version = dodag->version;
	my_dodag->grounded = dodag->grounded;
	my_dodag->joined = 1;
	my_dodag->my_preferred_parent = preferred_parent;
	my_dodag->my_rank = dodag->of->calc_rank(preferred_parent, dodag->my_rank);
	my_dodag->dao_seq = RPL_COUNTER_INIT;
	my_dodag->min_rank = my_dodag->my_rank;
	
	start_trickle(my_dodag->dio_min, my_dodag->dio_interval_doubling, my_dodag->dio_redundancy);
	delay_dao();
}

void rpl_global_repair(rpl_dodag_t *dodag){
	rpl_dodag_t * my_dodag = rpl_get_my_dodag();
	if(my_dodag == NULL){
		printf("Error - no global repair possible, if not part of a DODAG\n");
		return;
	}
	//TODO: nachschauen - soll das wirklich so überschrieben werden? 
	my_dodag->version = dodag->version;
	my_dodag->dtsn = dodag->dtsn;
}

ipv6_addr_t *rpl_get_my_preferred_parent(){
	rpl_dodag_t * my_dodag = rpl_get_my_dodag();
	if(my_dodag == NULL){
		return NULL;
	}
	return &my_dodag->my_preferred_parent->addr;
}
