#include <string.h>
#include <math.h>
#include "of_mrhof.h"

static char is_initialised = 0;

uint16_t cur_min_path_cost = MAX_PATH_COST;
static mrhof_candidate_info_t candidate_info[RPL_MAX_CANDIDATE_NEIGHBORS];

rpl_of_t rpl_of_mrhof = {
        0x1,
        calc_rank,
        which_parent,
        which_dodag,
        reset,
        init,
        NULL
};

rpl_of_t *rpl_get_of_mrhof() {
    return &rpl_of_mrhof;
}

void init() {
    //TODO implement OF-specific init of timers/functions/structs/whatever
    if (!is_initialised) {
        //blabla init this node
    }
    is_initialised = 1;
}

void reset(rpl_dodag_t *dodag) {
    //todo implement if necessary
}

void calc_path_cost() {
    /*
     * Calculates the path cost through all candidate neighbours and sets
     * cur_min_path_cost to the lowest value?
     */

    if(i_am_root){
        //Since for now we only support ETX, the pastcost that computes to
        //a rank of minhoprankincrease IS minhoprankincrease
        cur_min_path_cost = rpl_get_my_dodag()->minhoprankincrease;
        return;
    }

    /*
     * This is the boolean describing if I need to do a parent selection.
     * This happens always when the past cost for a candidate neighbor OR a
     * parent changes.
     */
    char do_parent_selection = 0;
    uint16_t path_cost = 0;
    for (uint8_t i = 0; i < RPL_MAX_CANDIDATE_NEIGHBORS; i++) {
        if(candidates[i].used){
            if(candidate_info[i].cur_etx != 0){
                /* The RFC specifies this computation a bit ambiguously, this is
                 * what I understood:
                 *
                 * (ETX_for_link_to_neighbor * 128) + Rank_of_that_neighbor
                 *
                 * This means I get the rank of that neighbor (which is the etx
                 * of the whole path from him to the root node) plus my ETX to
                 * that neighbor*128, which would be the 'rank' of the single link
                 * from me to that neighbor
                 *
                 */
                path_cost = candidate_info[i].cur_etx*ETX_RANK_MULTIPLIER + (candidates[i].dodag->my_rank);
                if(candidate_info[i].cur_path_cost == path_cost){
                    //nothing changed
                    continue;
                }
                do_parent_selection = 1;
                candidate_info[i].cur_path_cost = path_cost;
            }else{
                //if it is 0, the value has not been computed yet, so we can't
                //compute a path cost
                if(candidate_info[i].cur_path_cost == MAX_PATH_COST){
                    //nothing changed
                    continue;
                }
                do_parent_selection = 1;
                candidate_info[i].cur_path_cost = MAX_PATH_COST;
            }
        }
    }
    if(do_parent_selection){
        parent_selection();
    }

}

uint16_t calc_rank() {
    /*
     * Return the rank for this node (calculated from path cost).
     * There will be 3 values calculated, the highest of those 3 values will be
     * used as the noderank.
     *
     * For now, there is no metric-selection or specification, so the rank com-
     * putation will always be assumed to be done for the ETX metric.
     */
    uint16_t result = INFINITE_RANK;

    //Holds the 3 computed values for the nodes rank from which the max value
    //will be the nodes actual rank
    uint16_t rankarray[3] = {0,0,0};

    //TODO finish this
    //rankarray[0] = (rpl_find_preferred_parent()->rank > ;

    if (i_am_root) {
        result = rpl_get_my_dodag()->minhoprankincrease;
        return result;
    }

    //Check which parent has the highest advertised rank
    for (uint8_t i = 0; i < RPL_MAX_PARENTS; i++) {
        if (parents[i].used) {
            if (parents[i].rank == INFINITE_RANK){
                // parent not useful, should be deleted //TODO check if deletion
                // should be triggered from here
                continue;
            }
            else if (parents[i].rank > rankarray[2]) {
                //store it away
                rankarray[2] = parents[i].rank;
            }
        }
    }

    //Compute 2nd rankvalue and store it in 2nd place in the array
    if(rpl_get_my_dodag()->minhoprankincrease * (1 + (rankarray[2]/rpl_get_my_dodag()->minhoprankincrease)) < rankarray[2]){
        //overflow
        rankarray[1] = INFINITE_RANK;
    }else{
        rankarray[1] = rpl_get_my_dodag()->minhoprankincrease * (1 + (rankarray[2]/rpl_get_my_dodag()->minhoprankincrease));
    }

    //Compute the 3rd rankvalue and store it in the 3rd place
    if(rankarray[2] - rpl_get_my_dodag()->maxrankincrease )


    return result;
}

void parent_selection(){
    /*
     * Select a candidate neighbor as preferred parent.
     *
     * Note:
     *      If we allow floating roots, a node may have no parents
     */

    /*
     * Might be faster to set the node directly?
     */
    uint8_t cur_min_path_neighbor_index = 0;
    uint16_t min_path_cost = MAX_PATH_COST;

    for (uint8_t i = 0; i < RPL_MAX_CANDIDATE_NEIGHBORS; i++) {
        /*
         * Go through all Candidate Neighbors, compare their path cost with cur-
         * rent minimum path cost and change cur_min_path_neighbor_index
         * to the index of the current neighbor if his path is 'shorter'
         */
        if(candidates[i].used && (candidate_info[i].cur_path_cost < MAX_LINK_METRIC)){
            if(candidate_info[i].cur_path_cost < min_path_cost){
                cur_min_path_neighbor_index = i;
            }
        }
    }

    //TODO finish this.
    //rpl_find_preferred_parent()
}

//Not used yet, as the implementation only makes use of one dodag for now.
rpl_dodag_t * which_dodag(rpl_dodag_t *d1, rpl_dodag_t *d2) {
    return d1;
}
