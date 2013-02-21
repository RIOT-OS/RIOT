#include "rpl_structs.h"

// Disallow links with greater than 4 expected
// transmission counts on the selected path.
#define MAX_LINK_METRIC (512)

// Disallow paths with greater than 256
// expected transmission counts.
#define MAX_PATH_COST (0x8000)

// Switch to a new path only if it is
// expected to require at least 1.5 fewer transmissions than the
// current path.
#define PARENT_SWITCH_THRESHOLD (192)

// If the preferred parent is not available, some more
// candidate parents are still available without triggering a new
// round of route discovery.
#define PARENT_SET_SIZE (RPL_MAX_PARENTS)

// Do not allow a node to become a floating root.
#define ALLOW_FLOATING_ROOT (0)

//  While assigning Rank when using ETX, use the representation of ETX described
//  in [RFC6551], i.e., assign Rank equal to ETX * 128.
#define ETX_RANK_MULTIPLIER (0x80)

rpl_of_t *rpl_get_of_mrhof();
uint16_t calc_rank();
uint16_t calc_path_cost();
uint16_t calc_etx_value();
rpl_parent_t *which_parent(rpl_parent_t *, rpl_parent_t *);
rpl_dodag_t *which_dodag(rpl_dodag_t *, rpl_dodag_t *);
void reset(rpl_dodag_t *);

//############################
// OF specific structs #######
//############################

//neighbour struct for etx path calculation
typedef struct mrhof_candidate_info_t {
    ipv6_addr_t addr;
    uint16_t    packet_snt;
    uint16_t    packet_rec;
    uint16_t    cur_path_cost;
    uint16_t    cur_etx;
} mrhof_candidate_info_t;
