#include <string.h>
#include "of0.h"

//Function Prototypes
static uint16_t calc_rank(rpl_parent_t *, uint16_t);
static rpl_parent_t *which_parent(rpl_parent_t *, rpl_parent_t *);
static rpl_dodag_t *which_dodag(rpl_dodag_t *, rpl_dodag_t *);
static void reset(rpl_dodag_t *);

rpl_of_t rpl_of0 = {
	0x0,
	calc_rank,
	which_parent,
	which_dodag,
	reset,
	NULL
};

rpl_of_t *rpl_get_of0(){
	return &rpl_of0;
}

void reset(rpl_dodag_t *dodag){
	//Nothing to do in OF0
}

uint16_t calc_rank(rpl_parent_t * parent, uint16_t base_rank){
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
