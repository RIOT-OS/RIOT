#ifndef NODE_H_
#define NODE_H_

#include "common/avl.h"
#include "common/netaddr.h"

#include "util.h"
#include "debug.h"

#ifdef ENABLE_NAME
extern char* local_name;
#endif

/* if a connection is lost, the loss will be reported LOST_ITER_MAX times in HELLO and TC messages. */
#define LOST_ITER_MAX	(1 << 3)

enum {
	NODE_TYPE_OLSR,
	NODE_TYPE_NHDP
};

/* simple list to store alternative routes */
struct alt_route {
	struct alt_route* next;

	struct netaddr* last_addr;
	time_t expires;
};

struct olsr_node {
	struct avl_node node;		/* for routing table Information Base */

	struct netaddr* addr;		/* node address */
	struct netaddr* next_addr;	/* neighbor addr to send packets to for this node*/
	struct netaddr* last_addr;	/* node that announced this node */
	struct alt_route* other_routes; /* other possible last_addrs */

	time_t expires;				/* time when this tuple is invalid */
	uint16_t seq_no;			/* last seq_no from last_addr */
	uint8_t distance;			/* hops between us and the node */

	uint8_t type		: 1;	/* node type */
	uint8_t mpr_selector: 2;	/* whether the node selected us as a MPR - only 1-hop */
	uint8_t pending		: 3;	/* whether the link can already be used - only 1-hop */
	uint8_t lost		: 4;	/* [4 bit] if set, the node will be annouced as lost - only 1-hop */

#ifdef ENABLE_NAME
	char* name;					/* node name from graph.gv */
#endif
};

struct nhdp_node {
	struct olsr_node super;

	/* number of 2-hop neighbors reached through this node aka if this value is > 0, it's a MPR */
	uint8_t mpr_neigh;

	/* average packet loss, decides if it should be used as 1-hop neigh */
	float link_quality;
};

static inline struct olsr_node* h1_super(struct nhdp_node* n)		{ return (struct olsr_node*) n; }
static inline struct nhdp_node* h1_deriv(struct olsr_node* n) {
	if (n == NULL)
		return 0;

	if (n->type != NODE_TYPE_NHDP)
		return 0;

	return (struct nhdp_node*) n;
}

void node_init(void);
struct netaddr* get_local_addr(void);
struct avl_tree* get_olsr_head(void);
int olsr_node_cmp(struct olsr_node* a, struct olsr_node* b);
struct olsr_node* get_node(struct netaddr* addr);

void add_other_route(struct olsr_node* node, struct netaddr* last_addr, uint8_t vtime);
void remove_other_route(struct olsr_node* node, struct netaddr* last_addr);
void remove_default_node(struct olsr_node* node);
void push_default_route(struct olsr_node* node);
void pop_other_route(struct olsr_node* node, struct netaddr* last_addr);

#endif /* NODE_H_ */
