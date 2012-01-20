#include <string.h>
#include "of0.h"

rpl_of_t rpl_of0 = {
	0,
	calc_rank,
	which_parent,
	which_dodag,
	reset,
	NULL
};

static void reset(rpl_dodag_t *dodag){
	//Nothing to do in OF0
}

static uint16_t calc_rank(rpl_parent_t * parent, uint16_t base_rank){
	if(base_rank == 0) {
		if(parent == NULL) {
			return INFINITE_RANK;
		}
		base_rank = parent->rank;
	}

	uint16_t add;
	if(parent != NULL){
		add = parent->dodag->minhoprankincrease;
	}
	else{
		add = DEFAULT_MIN_HOP_RANK_INCREASE;
	}
	if( base_rank + add < base_rank ){
		return INFINITE_RANK;
	}
	return base_rank + add;
}

static rpl_parent_t * which_parent(rpl_parent_t *p1, rpl_parent_t *p2){
	return p1;
}

static rpl_dodag_t * which_dodag(rpl_dodag_t *d1, rpl_dodag_t *d2){
	return d1;
}
