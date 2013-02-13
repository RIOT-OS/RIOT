#include "rpl_structs.h"

// Disallow links with greater than 4 expected
// transmission counts on the selected path.
#define MAX_LINK_METRIC (512)

// Disallow paths with greater than 256
// expected transmission counts.
#define MAX_PATH_COST (32768)

// Switch to a new path only if it is
// expected to require at least 1.5 fewer transmissions than the
// current path.
#define PARENT_SWITCH_THRESHOLD (192)

// If the preferred parent is not available, two
// candidate parents are still available without triggering a new
// round of route discovery.
#define PARENT_SET_SIZE (3)

// Do not allow a node to become a floating root.
#define ALLOW_FLOATING_ROOT (0)

rpl_of_t *rpl_get_of_mrhof();
uint16_t calc_rank(rpl_parent_t *, uint16_t);
rpl_parent_t *which_parent(rpl_parent_t *, rpl_parent_t *);
rpl_dodag_t *which_dodag(rpl_dodag_t *, rpl_dodag_t *);
void reset(rpl_dodag_t *);
