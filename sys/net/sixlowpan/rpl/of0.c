#include <string.h>
#include "of0.h"

rpl_of_t rpl_of0 = {
	0x0,
	calc_rank,
	which_parent,
	which_dodag,
	reset,
	NULL,
	NULL,
	NULL
};

rpl_of_t *rpl_get_of0(){
	return &rpl_of0;
}

void reset(rpl_dodag_t *dodag){
	//Nothing to do in OF0
}

uint16_t calc_rank(){
	rpl_parent_t *  parent  = rpl_find_preferred_parent();
	uint16_t        my_rank = rpl_get_my_dodag()->my_rank;

	if(i_am_root){
	    return rpl_get_my_dodag()->minhoprankincrease;
	}

    if(my_rank == 0) {
		if(parent == NULL) {
			return INFINITE_RANK;
		}
		my_rank = parent->rank;
	}

	uint16_t add;
	if(parent != NULL){
		add = parent->dodag->minhoprankincrease;
	}
	else{
		add = DEFAULT_MIN_HOP_RANK_INCREASE;
	}
	if( my_rank + add < my_rank ){
		return INFINITE_RANK;
	}
	return my_rank + add;
}

//We simply return the Parent with lower rank
rpl_parent_t * which_parent(rpl_parent_t *p1, rpl_parent_t *p2){
	if(p1->rank < p2->rank){
		return p1;
	}
	return p2;
}

//Not used yet, as the implementation only makes use of one dodag for now.
rpl_dodag_t * which_dodag(rpl_dodag_t *d1, rpl_dodag_t *d2){
	return d1;
}
