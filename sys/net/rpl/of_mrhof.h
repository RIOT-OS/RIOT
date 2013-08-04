#include "rpl_structs.h"

/*
 * Disallow links with greater than 4 expected
 * transmission counts on the selected path.
 */
#define MAX_LINK_METRIC (512)

/*
 * Disallow paths with greater than 256
 * expected transmission counts.
 */
#define MAX_PATH_COST (0x8000)

/*
 * Switch to a new path only if it is
 * expected to require at least 1.5 fewer transmissions than the
 * current path.
 */
#define PARENT_SWITCH_THRESHOLD (192)

/*
 *  Do not allow a node to become a floating root.
 *  (Currently unused, since the RPL-implementation does not allow for floating
 *  roots).
 */
#define ALLOW_FLOATING_ROOT (0)

/*
 *  While assigning Rank when using ETX, use the representation of ETX described
 *  in [RFC6551], i.e., assign Rank equal to ETX * 128.
 */
#define ETX_RANK_MULTIPLIER (0x80)

rpl_of_t *rpl_get_of_mrhof(void);
