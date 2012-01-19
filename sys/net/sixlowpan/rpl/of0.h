#include "rpl_structs.h"

static uint16_t calc_rank(rpl_parent_t *, uint16_t);
static rpl_parent_t *which_parent(rpl_parent_t *, rpl_parent_t *);
static rpl_dodag_t *which_dodag(rpl_dodag_t *, rpl_dodag_t *);
static void reset(rpl_dodag_t *);
