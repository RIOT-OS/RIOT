#include <string.h>
#include "of_mrhof.h"

rpl_of_t rpl_of_mrhof = {
    0x1,
    calc_rank,
    which_parent,
    which_dodag,
    reset,
    NULL
};

rpl_of_t *rpl_get_of_mrhof(){
    return &rpl_of_mrhof;
}

void reset(rpl_dodag_t *dodag){
    if(PARENT_SET_SIZE > 3){
            //do stuff
    }
    //todo implement if necessary
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
