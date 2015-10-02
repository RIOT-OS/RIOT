/******************************************************************************
*
*   Copyright (C) 2014 Texas Instruments Incorporated
*
*   All rights reserved. Property of Texas Instruments Incorporated.
*   Restricted rights to use, duplicate or disclose this code are
*   granted through contract.
*
*   The program may not be used without the written permission of
*   Texas Instruments Incorporated or against the terms and conditions
*   stipulated in the agreement under which this program has been supplied,
*   and under no circumstances can it be used with non-TI connectivity device.
*
******************************************************************************/

#include "client_mgmt.h"
#include "server_util.h"
#include "server_pkts.h"
#include "server_plug.h"
#include "server_core.h"

#ifndef CFG_SR_MAX_SUBTOP_LEN
#define MAX_SUBTOP_LEN 32
#else
#define MAX_SUBTOP_LEN CFG_SR_MAX_SUBTOP_LEN
#endif

#ifndef CFG_SR_MAX_TOPIC_NODE
#define MAX_TOP_NODES  32
#else
#define MAX_TOP_NODES  CFG_SR_MAX_TOPIC_NODE
#endif

/* A topic name (or string or tree or hierarchy) is handled as a series of nodes.
   A 'topic node' refers to a level in the topic tree or hierarchy and it stores
   the associated part of the topic string at that level. For example, a topic
   having a name "abc/x/1" will have three nodes for each of the subtopics, viz,
   "abc/", "x/" and "1". Further, the association between the nodes is indicated
   as following.

          -- down link hierarchy -->      -- down link hierarchy --> 
   "abc/"                            "x/"                            "1"
          <-- up   link hierarchy --      <-- up   link hierarchy --

   To extend this example, another topic having a name "abc/y/2" will be managed
   as following.
  
          -- down link hierarchy -->      -- down link hierarchy --> 
   "abc/"                            "x/"                            "1"
          <-- up   link hierarchy --      <-- up   link hierarchy --
                                    |   ^
                                    |   |
                  down link neighbour   up link neighbour
                                    |   |
                                    V   |
                                          -- down link hierarchy -->
                                     "y/"                            "2" 
                                          <-- up   link hierarchy --

   To reduce wasted in byte alignments, the structure has been hand packed.
*/
static struct topic_node {

        struct topic_node *dn_nhbr;   /* Down Link Neighbour node */
        struct topic_node *up_nhbr;   /* Up   Link Neighbour node */

        struct topic_node *dn_hier;   /* Down Link Hierarchy node */
        struct topic_node *up_hier;   /* Up   Link Hierarchy node */

        u32                cl_map[3]; /* Subscribers for each QOS */

        u8                *my_data;   /* Leaf node: retained data */
        u32                my_dlen;

        void              *will_cl;   /* CL OBJ of WILL Leaf node */

#define TNODE_PROP_RETAIN_DATA  0x04
        /* Bits 0,1 for QOS and rest are flags */
        u8                 my_prop;

        u8                 pg_map;    /* Map: application plugins */

        u16    toplen;                /* Length of node sub-topic */
        c8     subtop[MAX_SUBTOP_LEN];/* NUL terminated sub-topic */

} nodes[MAX_TOP_NODES];

/* 
    Housekeeping to manage subtopics (nodes) at run-time.
*/
static struct _node_stack {
        struct topic_node *node;
        u32                val1;
        u32                val2;

} node_stack[MAX_TOP_NODES];

static i32 stack_idx = 0;

static void stack_add(struct topic_node *node, u32 val1, u32 val2)
{
        node_stack[stack_idx].node = node;
        node_stack[stack_idx].val1 = val1;
        node_stack[stack_idx].val2 = val2;

        stack_idx++;
}

static inline struct _node_stack *stack_pop()
{
        if(0 == stack_idx)
                return NULL;

        return node_stack + (--stack_idx);
}

static inline bool is_node_retain(const struct topic_node *node)
{
        return (node->my_prop & TNODE_PROP_RETAIN_DATA)? true : false;
}

static inline void node_retain_set(struct topic_node *node, bool retain)
{
        if(retain)
                node->my_prop |=  TNODE_PROP_RETAIN_DATA;
        else
                node->my_prop &= ~TNODE_PROP_RETAIN_DATA;
}

static inline void node_qid_set(struct topic_node *node, u8 qid)
{
        node->my_prop &= ~QID_VMASK;
        node->my_prop |=  qid;
}

static inline u8 node_qid_get(struct topic_node *node)
{
        return node->my_prop & QID_VMASK;
}

static inline bool is_node_willed(const struct topic_node *node)
{
        return (NULL != node->will_cl)? true : false;
}

static inline bool enrolls_plugin(const struct topic_node *node)
{
        return (PG_MAP_ALL_DFLTS != node->pg_map)? true : false;
}

static struct topic_node *free_nodes = NULL;
static struct topic_node *root_node  = NULL;

static void node_reset(struct topic_node *node)
{
        node->dn_nhbr = NULL;
        node->up_nhbr = NULL;
        node->dn_hier = NULL;
        node->up_hier = NULL;

        node->cl_map[0] = node->cl_map[1] = node->cl_map[2] = 0;
        node->my_data = NULL;
        node->my_dlen = 0;
        node->will_cl = NULL;
        node->my_prop = QID_VMASK;

        node->pg_map  = PG_MAP_ALL_DFLTS;
        node->toplen  = 0;
}

static void topic_node_init(void)
{
        int i = 0;

        for(i = 0; i < MAX_TOP_NODES; i++) {
                struct topic_node *node  = nodes + i;

                node_reset(node);

                node->dn_nhbr = free_nodes;
                free_nodes  = node;
        }

        return;
}

static struct topic_node *alloc_topic_node(void)
{
        struct topic_node *node = free_nodes;
        if(NULL != node) {
                free_nodes  = node->dn_nhbr;

                node_reset(node);
        }

        return node;
}

static void free_node(struct topic_node *node)
{
        node_reset(node);
        
        node->dn_nhbr = free_nodes;
        free_nodes     = node;
}

static bool is_end_of_subtop(const c8 *topstr, c8 const **next_subtop)
{
        bool rv = false;

        if('\0' == *topstr) {
                *next_subtop = NULL; /* Reached end of topstr */
                rv = true;
        }
        
        if('/'  == *topstr) {
                /* Next sub-topic is NULL, if a '\0' follows '/' */
                *next_subtop = *(topstr + 1)? topstr + 1 : NULL;
                rv = true;
        }

        return rv;
}

static i32 subtop_read(const c8 *topstr, c8 *subtop_buf, u16 buf_length,
                       c8 const **next_subtop)
{
        i32 idx = 0, len = buf_length;

        *next_subtop = topstr;

        while(idx < (len - 1)) {
                subtop_buf[idx++] = *topstr;

                if(is_end_of_subtop(topstr, next_subtop))
                        break;

                topstr++;
        }

        if(idx == len) {
                USR_INFO("S: Fatal, insufficient buffer for sub-str\n\r");
                return -1; /* zero or insufficient buffer provided */
        }

        /* NUL terminated buffer: unless '\0' ended copy, make one */
        if('\0' == subtop_buf[idx - 1])
                idx--; /* A fix up */
        else
                subtop_buf[idx] = '\0';

        return idx;
}

static 
struct topic_node *alloc_node_subtop(const c8 *topstr, c8 const **next_subtop)
{
        u16 len = 0;
        struct topic_node *node = alloc_topic_node();
        if(NULL == node)
                return NULL;

        len = subtop_read(topstr, node->subtop, MAX_SUBTOP_LEN, next_subtop);
        if(len <= 0) {
                free_node(node);
                return NULL;
        }

        node->toplen = len;

        return node;
}

/* Returns true if string 'subtop' is part of string 'topstr' otherwise false.
   Additionally, on success, pointer to next subtopic in 'topstr' is provided.
*/
bool is_first_subtop(const c8 *subtop, const c8 *topstr, c8 const **next_subtop)
{
        while(*subtop == *topstr) {
                if(is_end_of_subtop(topstr, next_subtop))
                        return true;

                subtop++;
                topstr++;
        }

        return false;
}

/* Find a topic node in neighbour list that matches first subtopic in 'topstr'.
   Additionally, on success, pointer to next subtopic in 'topstr' is provided.
*/
struct topic_node *subtop_nhbr_node_find(const struct topic_node *root_nh,
                                         const c8 *topstr, 
                                         c8 const **next_subtop)
{
        /* This routine does not make use of 'next_subtop' */
        while(root_nh) {
                if(true == is_first_subtop(root_nh->subtop, topstr, next_subtop))
                        break;

                root_nh = root_nh->dn_nhbr;
        }

        return (struct topic_node*) root_nh;/* Bad: const from poiner removed */
}

/* Find a topic node in neighbour list that matches the given 'subtop' string */
struct topic_node *nhbr_node_find(const struct topic_node *root_nh,
                                  const c8 *subtop)
{
        const c8 *next_subtop = NULL;
        
        return subtop_nhbr_node_find(root_nh, subtop, &next_subtop);
}

/* Find leaf node of branch-combo that matches complete 'topstr'. 
   Modus Operandi:  For each sub topic in 'topstr', go across neighbour list,
   then for matching neighbour node, make its child node as root of neighbour
   list for another iteration for next sub topic.
*/
/* Complex */
static struct topic_node *lowest_node_find(const c8 *topstr, bool *flag_nh, 
                                           c8 const **next_subtop)
{
        struct topic_node *root_nh = root_node, *node = NULL;

        *next_subtop = topstr;
        *flag_nh = false;

        while(root_nh) {
                node = subtop_nhbr_node_find(root_nh, topstr, next_subtop);
                if(NULL == node) {         /* Partial or no match */
                        *flag_nh = true;
                        node = root_nh;
                        break;
                }

                /* NULL '*next_subtop' indicates a complete match */
                if(NULL == (*next_subtop))
                        break;

                root_nh = node->dn_hier;
                topstr  = *next_subtop;
        }

        return node;
}

struct topic_node *leaf_node_find(const c8 *topstr)
{
        const c8 *next_subtop;
        bool flag_nh;   
        struct topic_node *node = lowest_node_find(topstr, &flag_nh,
                                                   &next_subtop);

        return (NULL == next_subtop)? node : NULL;
}

static void try_node_delete(struct topic_node *node); /* Forward declaration */

/* Create 'child only' hierarchy of nodes to hold all sub topics in 'topstr'. 
   The routine returns a start node of hierarchy and also provides leaf node.
*/
static 
struct topic_node *hier_nodes_create(const c8 *topstr, struct topic_node **leaf)
{
        struct topic_node *base = NULL, *node = NULL, *prev = NULL;
        const c8 *next_subtop = NULL;

        do {
                node = alloc_node_subtop(topstr, &next_subtop);

                if(NULL == node) {
                        /* Allocation of a node failed, free up
                           the previous allocations, if any, in
                           the hierarchy that was being created */
                        if(prev)
                                try_node_delete(prev);
                        
                        base = NULL;
                        break;
                }

                if(NULL == prev) {
                        //                        prev = node;
                        base = node; /* First node of hierarchy */
                } else {
                        prev->dn_hier = node;
                        node->up_hier = prev;
                        //                        prev          = node;
                }

                prev   = node;
                topstr = next_subtop;

        } while(next_subtop);

        *leaf = node;

        DBG_INFO("S: Hierarchy of nodes created: Base: %s & Leaf: %s\n\r",
                 base? base->subtop : " ", (*leaf)? (*leaf)->subtop : " ");

        return base;
}

static void install_nhbr_node(struct topic_node *base, struct topic_node *node)
{
        while(base->dn_nhbr) {
                base = base->dn_nhbr;
        }

        base->dn_nhbr = node;
        node->up_nhbr = base;

        DBG_INFO("S: %s added as a neighbour to %s\n\r",
                 node->subtop, base->subtop);
}

static void set_up_hier_nodes(struct topic_node *up_hier, 
                              struct topic_node *dn_hier)
{
        up_hier->dn_hier = dn_hier;
        dn_hier->up_hier = up_hier;

        DBG_INFO("%s added as DN HIER to %s \n\r",
                 dn_hier->subtop, up_hier->subtop);
}

static inline void install_topic_root_node(struct topic_node *node)
{
        root_node = node;
}

/* Create or update tree to create branch-combo to refer to 'topstr'.
   Modus operandi: 
  
*/
struct topic_node *topic_node_create(const c8 *topstr)
{
        struct topic_node *base, *node, *leaf;
        const c8 *next_subtop = topstr;
        bool flag_nh;

        base = lowest_node_find(topstr, &flag_nh, &next_subtop);
        if(NULL == next_subtop)
                return base;   /* Found exact match */

        /* Control reaches here, if either no or partial branch-combo has been
           found for 'topstr'. Now, let's create remaining branches for string
           'next_subtop' and assign them to appropriately to topic tree.
        */
        DBG_INFO("S: Creating Hierarchy for %s\n\r", next_subtop);

        node = hier_nodes_create(next_subtop, &leaf);
        if(node) {
                if(NULL == base)
                        install_topic_root_node(node);
                else if(flag_nh)
                        install_nhbr_node(base, node);
                else 
                        set_up_hier_nodes(base, node);

                return leaf;
        }

        return NULL;
}

static bool can_delete_node(const struct topic_node *node)
{
        if(node->up_nhbr && node->up_hier) {
                USR_INFO("S: fatal, node w/ up-nhbr & up-hier.\n\r");
                return false;
        }

        if(node->dn_nhbr  ||
           node->dn_hier)
                return false;

        return true;
}

static struct topic_node *node_delete(struct topic_node *node)
{
        struct topic_node *next = NULL;

        if(false == can_delete_node(node))
                return NULL;

        if(node->up_nhbr) {
                node->up_nhbr->dn_nhbr = NULL;
                next = node->up_nhbr;
        }

        if(node->up_hier) {
                node->up_hier->dn_hier = NULL;
                next = node->up_hier;
        }

        if((NULL == node->up_nhbr) &&
           (NULL == node->up_hier))
                root_node = NULL;

        DBG_INFO("S: Deleted node %s\n\r", node->subtop);

        free_node(node);

        return next;
}

struct topbuf_desc {
        c8   *buffer;
        u16   maxlen;
        u16   offset;
};

static bool topbuf_add(struct topbuf_desc *buf_desc, const struct topic_node *node)
{
        const c8 *next_subtop;
        c8  *buf = buf_desc->buffer + buf_desc->offset;
        u16  len = buf_desc->maxlen - buf_desc->offset;

        i32 rv = subtop_read(node->subtop, buf, len, &next_subtop);
        if(rv < 0)
                return false;

        if(NULL != next_subtop) {
                USR_INFO("S: topstr_add fatal, bad subtop.\n\r");
                return false;
        }

        buf_desc->offset += rv;

        return true;
}

static bool topbuf_cpy(struct topbuf_desc *buf_desc, const c8 *subtop)
{
        const c8 *next_subtop;
        c8  *buf = buf_desc->buffer + buf_desc->offset;
        u16  len = buf_desc->maxlen - buf_desc->offset;

        i32 rv = subtop_read(subtop, buf, len, &next_subtop);
        if(rv < 0)
                return false;

        if(NULL != next_subtop) {
                USR_INFO("S: topstr_copy fatal, bad subtop.\n\r");
                return false;
        }

        buf_desc->offset += rv;

        return true;
}

static bool has_a_wildcard(const struct topic_node *node)
{
        const c8 *str = node->subtop;
        while('\0' != *str) {
                if(('+' == *str) || ('#' == *str))
                        return true;

                str++;
        }

        return false;
}        

static bool is_node_SUB_subtop(const struct topic_node *node, const c8 *subtop)
{
        if(false == has_a_wildcard(node))
                return ((0 == strcmp(node->subtop, subtop))                ||
                        (node->dn_hier  && (0 == strcmp("+/", subtop)))    ||
                        (!node->dn_hier && (0 == strcmp("+",  subtop))))?
                        true : false;

       return false;
}

/* Search node tree, created by PUB retention, for the branch combo, whose
   absolute sub-topic sequence 'matches', in entirety, the topic, which is
   being subscribed. The 'match' criteria is met either through the
   wildcard sub-topics or exact compare.

   The hierarchical search starts at the specified 'base' node and ends at
   the leaf node. If the sequence of 'base-to-leaf' sub-topics 'match' the
   'topSUB', then the leaf node is returned, otherwise a NULL is returned.

   As part of hierarchical search, neighbouring nodes, if any, are logged
   for subsequent iteration of the routine.
*/
static struct topic_node *pub_hier_search(const c8 *topSUB,
                                          const struct topic_node *base, 
                                          struct topbuf_desc *mk_pubtop)
{
        const struct topic_node *node = base, *prev = NULL;
        const c8 *next_subtop = topSUB;
        c8 subtop[MAX_SUBTOP_LEN];

        while(next_subtop && node) {
                if(subtop_read(topSUB, subtop, MAX_SUBTOP_LEN,
                               &next_subtop) <= 0)
                        break;

                if(node->dn_nhbr)
                        stack_add(node->dn_nhbr, (u32)topSUB, mk_pubtop->offset);

                if(false == is_node_SUB_subtop(node, subtop))
                        break;    /* Node doesn't form part of published topic */

                if(false == topbuf_add(mk_pubtop, node))
                        break;

                prev = node;
                node = node->dn_hier;

                topSUB = next_subtop;
        }

        if(NULL != next_subtop)
                node = NULL;
        else 
                node = prev;

        return (struct topic_node *)node;
}

static bool is_node_PUB_subtop(const struct topic_node *node,
                               const c8 *subtop, bool endtop)
{
        /* Assumes that subtop hasn't got any wildcard characater */
        return ((0 == strcmp(subtop, node->subtop))               ||
                (!endtop && (0 == strcmp("+/", node->subtop)))    ||
                (endtop  && (0 == strcmp("+",  node->subtop))))?
                true : false;
}

/* Search node tree, created by subscriptions, for the branch combo, whose
   sub-topic sequence 'matches', in entirety, the absolute topic, to which
   data has been published. The 'match' criteria is met either through the
   wildcard sub-topics or exact compare.

   The hierarchical search starts at the specified 'base' node and ends at
   the leaf node. If the sequence of 'base-to-leaf' sub-topics 'match' the
   'topPUB', then the leaf node is returned, otherwise a NULL is returned.

   As part of hierarchical search, neighbouring nodes, if any, are logged
   for subsequent iteration of the routine.
*/ 
static struct topic_node *SUB_leaf_search(const c8 *topPUB,
                                          const struct topic_node *base)
{
        const struct topic_node *node = base, *prev = NULL;
        const c8 *next_subtop = topPUB;
        c8 subtop[MAX_SUBTOP_LEN];

        while(next_subtop && node) {
                if(subtop_read(topPUB, subtop, MAX_SUBTOP_LEN,
                               &next_subtop) <= 0)
                        break;

                if(node->dn_nhbr)
                        stack_add(node->dn_nhbr, (u32)topPUB, 0);

                if(0 == strcmp("#", node->subtop))
                        goto SUB_leaf_search_exit1;

                if(false == is_node_PUB_subtop(node, subtop, !next_subtop))
                        break; /* Node doesn't form part of published topic */

                prev = node;
                node = node->dn_hier;

                topPUB = next_subtop;
        }

        if(NULL != next_subtop)
                node = NULL;
        else 
                node = prev;

SUB_leaf_search_exit1:
        return (struct topic_node*) node; // Bad
}


/*-------------------------------------------------------------------------
 * MQTT Routines
 *-------------------------------------------------------------------------
 */

#define WBUF_LEN   MQP_SERVER_RX_LEN /* Assignment to ease implementation */
static c8 work_buf[WBUF_LEN];

static void try_node_delete(struct topic_node *node)
{
        while(node) {

                if(is_node_retain(node) ||
                   is_node_willed(node) ||
                   enrolls_plugin(node) ||
                   node->cl_map[0]      ||
                   node->cl_map[1]      ||
                   node->cl_map[2])
                        break;

                node = node_delete(node);
        }
}

/* Move this to a common file */
static void pub_msg_send(const struct utf8_string *topic, const u8 *data_buf,
                         u32 data_len, u8 fh_flags, u32 cl_map)
{
        enum mqtt_qos qos = ENUM_QOS(fh_flags);
        struct mqtt_packet *mqp = NULL;

        mqp = mqp_server_alloc(MQTT_PUBLISH, 2 + topic->length + 2 + data_len);
        if(NULL == mqp)
                return;

        if((0 > mqp_pub_append_topic(mqp, topic, qos? mqp_new_id_server(): 0)) ||
           (data_len && (0 > mqp_pub_append_data(mqp, data_buf, data_len)))) {
                mqp_free(mqp);
                return;
        }

        mqp_prep_fh(mqp, fh_flags);

        if(cl_map)
                cl_pub_dispatch(cl_map, mqp);

        return;
}

static struct topic_node *SUB_node_create(const c8 *topSUB, u8 qid, void *usr_cl)
{
        struct topic_node *leaf = topic_node_create(topSUB);
        if(leaf) {
                u8    j = 0;
                u32 map = cl_bmap_get(usr_cl);

                for(j = 0; j < 3; j++)
                        /* Client: clear QOS of existing sub, if any */
                        leaf->cl_map[j] &= ~map;

                leaf->cl_map[qid] |= map;

                cl_sub_count_add(usr_cl); 
        }

        return leaf;
}

static u8 proc_pub_leaf(struct topic_node *leaf, const struct utf8_string *topic,
                        enum mqtt_qos qos, void *usr_cl)
{
        u8  qid = QOS_VALUE(qos);

        if(is_node_retain(leaf)) {
                /* If it is an earlier created topic w/ retained
                   data, then pick lower of the two QOS(s)  */
                qid = MIN(node_qid_get(leaf), qid);

                /* Publish the retained data to this client */
                pub_msg_send(topic, leaf->my_data, leaf->my_dlen,
                             MAKE_FH_FLAGS(false, qid, true),
                             cl_bmap_get(usr_cl));
        }

        return qid;
}

/* Multi-level wild-card subscription - search of all of the tree i.e.
   "no topic" ('no_top') in particular and publish it to client, if
   the hierarchy has no wild-card node. */
static
u8 proc_pub_hier_no_top(struct topic_node *base, struct topbuf_desc *mk_pubtop,
                        enum mqtt_qos qos, void *usr_cl)
{
        struct topic_node *node = base, *leaf = NULL;
        u8 ack = QOS_VALUE(qos);

        /* 1. Find the leaf node of a non wildcard branch-combo */
        while(node) {
                if(node->dn_nhbr)
                        stack_add(node->dn_nhbr, 0, mk_pubtop->offset);

                if(has_a_wildcard(node))
                        break;

                if(false == topbuf_add(mk_pubtop, node))
                        break;

                leaf = node;
                node = node->dn_hier;
        }

        /* A non NULL value of 'node' would indicate a hierarchy with a
           wildcard (sub-)topic (the 'node') - not suitable for PUB. */

        if(NULL == node) {
                /* 2. Send retained data, if any, to SUB Client */
                struct utf8_string topic = {mk_pubtop->buffer,
                                            mk_pubtop->offset};

                /* In this version, at this juncture, the 'leaf'
                   will not be NULL. Nevertheless a check (for
                   the sake of static analytical tools).........*/
                if(leaf)
                        ack = proc_pub_leaf(leaf, &topic, qos, usr_cl);
        }

        return ack;
}

/* Multi-level wild-card subscription - search of all of the tree i.e.
   "no topic" ('no_top') in particular and publish it to client, if
   a hierarchy in the tree has no wild-card node. */
static
u8 proc_pub_tree_no_top(struct topic_node *base, struct topbuf_desc *mk_pubtop,
                        enum mqtt_qos qos, void *usr_cl)
{
        u32 stack_ref = stack_idx;
        u8 min = QOS_VALUE(qos);

        if(base != NULL)
                stack_add(base, 0, mk_pubtop->offset);

        while(stack_ref < stack_idx) {
                struct _node_stack *stack = stack_pop();
                u8 ack;

                mk_pubtop->offset = (u16) stack->val2;

                ack = proc_pub_hier_no_top(stack->node, mk_pubtop, qos, usr_cl);
                if(ack < min)
                        min = ack;
        }

        return min;
}

static u8 proc_pub_hier_SUBtop(const c8 *topSUB,  const struct topic_node *base,
                               struct topbuf_desc *mk_pubtop, enum mqtt_qos qos,
                               void *usr_cl)
{
        struct topic_node *leaf = pub_hier_search(topSUB, base, mk_pubtop);
        u8 min = QOS_VALUE(qos);

        if(leaf) {
                struct utf8_string topic = {mk_pubtop->buffer,
                                            mk_pubtop->offset};

                min = proc_pub_leaf(leaf, &topic, qos, usr_cl);
        }

        return min;
}

/* used by sl or no wc */
static u8 proc_pub_tree_SUBtop(const c8 *topSUB, struct topic_node *base,
                               struct topbuf_desc *mk_pubtop,
                               enum mqtt_qos qos, void *usr_cl)
{
        u32 stack_ref = stack_idx;
        u8 min = QOS_VALUE(qos);

        if(NULL != base)
                stack_add(base, (u32)topSUB, mk_pubtop->offset);

        while(stack_ref < stack_idx) {
                struct _node_stack *stack = stack_pop();
                u8 ack;

                mk_pubtop->offset = stack->val2;
                ack = proc_pub_hier_SUBtop((c8*)stack->val1,  stack->node,
                                           mk_pubtop, qos, usr_cl);

                if(ack < min)
                        min = ack;
        }

        return min;
}

static 
u8 proc_sub_ml_wc_hier(const c8 *grandpa_topSUB, c8 *parent_subtop,
                       struct topic_node *base, struct topbuf_desc *mk_pubtop,
                       enum mqtt_qos qos, void *usr_cl)
{
        u8 min = QOS_VALUE(qos), ack = QFL_VALUE;
        c8 *subtop = NULL;

        /* 1. Search hier node for 'grandpa' and if found, get to parent level */
        if('\0' != grandpa_topSUB[0]) {
                struct topic_node *leaf = pub_hier_search(grandpa_topSUB, base,
                                                          mk_pubtop);
                if(NULL == leaf)
                        return min;

                base = leaf->dn_hier;            /* nhbr root at parent level */
        }

        /* 2. If present, process parent as a leaf and get its down hierarchy */
        subtop = parent_subtop;
        if(('\0' != subtop[0]) && ('+' != subtop[0]) && ('/' != subtop[0])) {
                u16 offset = mk_pubtop->offset;  /* Refer to grandpa's pubtop */
                u16 sublen = 0;

                while(subtop[sublen]){sublen++;}

                ack = proc_pub_tree_SUBtop(subtop, base, mk_pubtop, qos, usr_cl);
                mk_pubtop->offset = offset;      /* Restores grandpa's pubtop */

                subtop[sublen] = '/';            /* Make parent's hier subtop */

                base = nhbr_node_find(base, subtop);
                if(base)
                        base =  topbuf_cpy(mk_pubtop, subtop)?
                                base->dn_hier : NULL;
                subtop[sublen] =  '\0';          /* Get back, original subtop */
        }

        min = MIN(min, ack);        
        /* 3. Process '#' WC by walking thru entire sub-tree of parent 'base' */
        if(NULL != base)
                ack = proc_pub_tree_no_top(base, mk_pubtop, qos, usr_cl);

        return MIN(min, ack);
}

static u8 proc_sub_ml_wc_tree(c8 *grandpa_topSUB, c8 *parent_subtop,
                              struct topic_node *base,
                              enum mqtt_qos qos, void *usr_cl)
{      
        struct topbuf_desc mk_pubtop = {work_buf, WBUF_LEN, 0 /* offset */};
        u32 stack_ref = stack_idx;
        u8 min = QOS_VALUE(qos);

        if(NULL != base)
                stack_add(base, (u32)grandpa_topSUB, mk_pubtop.offset);

        while(stack_ref < stack_idx) {
                struct _node_stack *stack = stack_pop();
                u8 ack;

                mk_pubtop.offset = stack->val2;
                ack = proc_sub_ml_wc_hier((c8*)stack->val1, parent_subtop,
                                          stack->node, &mk_pubtop, qos, usr_cl);
                if(ack < min)
                        min = ack;
        }

        return min;
}

static u8 ml_wc_nodes_create(c8 *parent_topSUB, u16 toplen, u8 qid, void *usr_cl)
{
        struct topic_node *parent_leaf = NULL;

        if('\0' != parent_topSUB[0]) {
                parent_leaf = SUB_node_create(parent_topSUB, qid, usr_cl);
                if(NULL == parent_leaf)
                        return QFL_VALUE;
        }

        /* Get the topic SUB to it's original state */
        if(toplen > 1) parent_topSUB[toplen - 2] = '/';
        parent_topSUB[toplen - 1] = '#';

        if(NULL == SUB_node_create(parent_topSUB, qid, usr_cl)) {
                /* Failed to create WC topic, so delete parent as well.
                   In this revision, 'parent_leaf' will not be a 'NULL'
                   at this juncture, nevertheless a check (for tools) */
                if(parent_leaf)
                        node_delete(parent_leaf);

                return QFL_VALUE;
        }

        return qid;
}

/* Process Multi-level Wildcard Topic SUBSCRIBE */
static u8 proc_sub_ml_wildcard(c8 *topSUB, u16 toplen, enum mqtt_qos qos,
                               void *usr_cl)
{
        u16 len = 0, limit = MIN(toplen, MAX_SUBTOP_LEN);
        c8 subtop[MAX_SUBTOP_LEN], *ptr;
        u8 min = QOS_VALUE(qos);

        /* 'topSUB': Need to create grandpa topic and parent-subtopic */
        topSUB[toplen - 1] = '\0'; /* Remove '#' */
        if(toplen > 1)             /* Remove '/' */
                topSUB[toplen - 2] = '\0';

        do {    /* Do processing to get parent sub-topic into buffer */
                if('/' == topSUB[toplen - len - 1])
                        break;      /* found '/' */

                len++; /* Copy parent characters */
                subtop[MAX_SUBTOP_LEN - len] = topSUB[toplen - len];
        } while(len < limit);

        if((toplen > len) && ('/' != topSUB[toplen - len - 1]))
                return QFL_VALUE;  /* Bad Length */

        topSUB[toplen - len] = '\0'; /* End of grand-pa's topic name */
        ptr = subtop + MAX_SUBTOP_LEN - len; /* Parent's leaf subtop */
        min = proc_sub_ml_wc_tree(topSUB, ptr, root_node, qos, usr_cl);

        /* Make branch-combo to complete processing of parent' topic */
        strcpy(topSUB + toplen - len, ptr); // topSUB[toplen - len] = *ptr;

        /* Create nodes for multi-level wildcard topic & it's parent */
        min = ml_wc_nodes_create(topSUB, toplen, min, usr_cl);

        return min;
}

/* Process Single-level Wildcard or No Wild Card Topic SUBSCRIBE */
static 
u8 proc_sub_sl_or_no_wc(const c8 *topSUB, enum mqtt_qos qos, void *usr_cl)
{
        struct topbuf_desc mk_pubtop = {work_buf, WBUF_LEN, 0 /* offset */};
        u8 min = QOS_VALUE(qos);

        /* For single level wildcard or absolute topic, find PUB nodes */
        min = proc_pub_tree_SUBtop(topSUB, root_node, &mk_pubtop, qos, usr_cl);

        if(NULL == SUB_node_create(topSUB, min, usr_cl))
                min = QFL_VALUE;

        return min;
}

static u16 proc_forward_slash(c8 *buf, u16 len)
{
        u16 i, j;
        for(i = 1, j = 1; i < len; i++) {
                c8 curr = buf[i];
                if(('/'  == curr) && (buf[i - 1] == curr))
                        continue; /* Drop consecutive '/' */

                buf[j++] = curr;
        }

        if((1 != j) && ('/' == buf[j - 1]))
                j--;      /* Topic can not end with a '/' */

        buf[j] = '\0';

        return j;
}

static inline bool is_valid_char_order(c8 prev, c8 curr)
{
        return ((('/' != prev) && ('+' == curr))  ||
                (('+' == prev) && ('/' != curr))  ||
                (('/' != prev) && ('#' == curr))  ||
                (('#' == prev)))? false : true;
}


static bool is_valid_SUB_top(const c8 *buf, u16 len)
{
        c8 prev, curr;
        u16 i = 0;

        if((0 == len) || ('\0' == *buf))
                return false;

        curr = buf[0];
        for(i = 1; (i < len) && ('\0' != curr); i++) {
                prev = curr;
                curr = buf[i];

                if(false == is_valid_char_order(prev, curr))
                        break;
        }

        return (i == len)? true : false;
}

static bool proc_sub_msg_rx(void *usr_cl, const struct utf8_strqos *qos_topics,
                            u32 n_topics, u16 msg_id, u8 *ack)
{
        i32 i = 0;
        for(i = 0; i < n_topics; i++) {
                const struct utf8_strqos *qos_top = qos_topics + i;
                enum mqtt_qos qos = qos_top->qosreq;
                c8 *buf = (c8*)qos_top->buffer;
                u16 len = qos_top->length;

                /* Remove zero-topics and trailing '/' from SUB top */
                len = proc_forward_slash(buf, len);
                ack[i] = QFL_VALUE;
                if(false == is_valid_SUB_top(buf, len))
                        continue;

                buf[len] = '\0';  /* Dirty trick, cheeky one */

                ack[i] = ('#' == buf[len - 1])? 
                        proc_sub_ml_wildcard(buf, len, qos, usr_cl) : 
                        proc_sub_sl_or_no_wc(buf, qos, usr_cl);

                DBG_INFO("SUB Topic%-2d %s is ACK'ed w/ 0x%02x\n\r",
                         i + 1, buf, ack[i]);
        }

        return true; /* Send SUB-ACK and do not close network */
}

static
bool proc_sub_msg_rx_locked(void *usr_cl, const struct utf8_strqos *qos_topics,
                            u32 n_topics, u16 msg_id, u8 *ack)
{
        return proc_sub_msg_rx(usr_cl, qos_topics, n_topics, msg_id, ack);
}

static void leaf_un_sub(struct topic_node *leaf, void *usr_cl)
{
        u8  j   = 0;
        u32 map = cl_bmap_get(usr_cl);
        
        for(j = 0; j < 3; j++) {
                /* Client: clear QOS of existing sub, if any */
                if(0 == (leaf->cl_map[j] & map))
                        continue;

                leaf->cl_map[j]  &= ~map;
                cl_sub_count_del(usr_cl);

                try_node_delete(leaf);

                break;
        }
}

static bool proc_un_sub_msg(void *usr_cl, const struct utf8_string *topics,
                            u32 n_topics, u16 msg_id)
{
        u32 i = 0;

        for(i = 0; i < n_topics; i++) {
                const struct utf8_string *topic = topics + i;
                struct topic_node *leaf  = NULL;
                u16 len = topic->length;

                /* The maximum length of 'work_buf' is same as that of RX buffer
                   in the PKT-LIB. Therefore, the WBUF_LEN is not being checked
                   against the length of the topic (a constituent of RX buffer).
                */
                strncpy(work_buf, topic->buffer, topic->length);
                work_buf[len] = '\0';

                if('#' == work_buf[len - 1]) { /* Multi-level Wildcard */
                        work_buf[len - 1]         = '\0';
                        if(len > 1)
                                work_buf[len - 2] = '\0';

                        leaf = leaf_node_find(work_buf);
                        if(leaf)
                                leaf_un_sub(leaf, usr_cl);

                        if(len > 1)
                                work_buf[len - 2] = '/';
                        work_buf[len - 1]         = '#';
                }

                leaf = leaf_node_find(work_buf); 
                if(leaf)
                        leaf_un_sub(leaf, usr_cl);
        }

        return true; /* Do not close network */
}

static
bool proc_un_sub_msg_locked(void *usr_cl, const struct utf8_string *topics,
                            u32 n_topics, u16 msg_id)
{
        return proc_un_sub_msg(usr_cl, topics, n_topics, msg_id);
}

static void
leaf_msg_send(const struct topic_node *leaf, const struct utf8_string *topic,
              const u8 *data_buf, u32 data_len, bool dup, enum mqtt_qos qos,
              bool retain)
{
        u8 qid = 0, fh_fgs = 0;

        for(qid = 0; qid < 3; qid++) {
                u8 map = leaf->cl_map[qid];
                fh_fgs = MAKE_FH_FLAGS(dup, MIN(qid, QOS_VALUE(qos)), retain);

                if(map)
                        pub_msg_send(topic, data_buf, data_len, fh_fgs, map);
        }

        if(enrolls_plugin(leaf))
                plugin_publish(leaf->pg_map, topic, data_buf, data_len,
                               dup, qos, retain);

        return;
}

static void node_data_set(struct topic_node *node, u8 *data,
                          u32 dlen, u8 qid, bool retain)
{
        node->my_data  = data;
        node->my_dlen  = dlen;

        node_qid_set(node, qid);
        node_retain_set(node, retain);

        return;
}

static bool node_data_update(struct topic_node *node, bool drop_qid0,
                             const u8 *data_buf, u32 data_len, 
                             u8 qid, bool retain)
{
#define NODE_DATA_RESET_PARAMS  NULL, 0, 0, false

        /* Assumes that caller has provided either reset or valid params */

        u8 *data  = NULL;

        if(node->my_dlen)
                my_free(node->my_data);

        /* Watch out for assignment in 'if' statement - avoid such smarts */
        if((drop_qid0 && (0 == qid))                        ||
           (data_buf  && !(data = my_malloc(data_len)))) {
                node_data_set(node, NODE_DATA_RESET_PARAMS);
        } else {

                if(data)
                        buf_wr_nbytes(data, data_buf, data_len);

                node_data_set(node, data, data_len, qid, retain);
        }

        return ((!!data) ^ (!!data_len))? false : true; 
}

static inline bool is_wildcard_char(c8 c)
{
        return (('+' == c) || ('#' == c))? true : false;
}

static i32 pub_topic_read(const struct utf8_string *topic, c8 *buf, u32 len)
{
        u32 i = 0;
        u32 toplen = topic->length;  

        if(len < (toplen + 1))
                return -1;

        for(i = 0; i < toplen; i++) {
                c8 c = topic->buffer[i];
                if(is_wildcard_char(c))
                        return -1; /* Invalid: wildcard in PUB topic */

                if('\0' == c)
                        return -1; /* Invalid: NUL char in PUB topic */

                buf[i] = c;
        }

        buf[i] = '\0';

        return i;
}

static
void proc_sub_tree_topPUB(const c8 *topPUB,   const struct utf8_string *topic,
                          const u8 *data_buf, u32 data_len, enum mqtt_qos qos,
                          bool retain)
{
        struct topic_node *leaf = NULL;
        u32 stack_ref = stack_idx;

        if(NULL != root_node)
                stack_add(root_node, (u32)topPUB, 0 /* Not used */);

        while(stack_ref < stack_idx) {
                struct _node_stack *stack = stack_pop();

                /* Find leaf node of SUB that matches the PUB topic */
                leaf = SUB_leaf_search((c8*)stack->val1, stack->node); 
                if(leaf)
                        leaf_msg_send(leaf, topic, data_buf, data_len,
                                      false, qos, retain);
        }
}

static bool _proc_pub_msg_rx(void *usr_cl, const struct utf8_string *topic,
                             const u8 *data_buf, u32 data_len, u8 msg_id,
                             enum mqtt_qos qos, bool retain)
{
        i32 err = -1;

        /* Prior to msg processing, chk for topic or buffer errors */
        if((pub_topic_read(topic,  work_buf,  WBUF_LEN) <= 0) ||
           (proc_forward_slash(work_buf, topic->length) <= 0))
                goto _proc_pub_msg_rx_exit;

        /* If a valid MSG ID is specified for a QOS2 pkt, track it */
        err = -2;
        if((msg_id)           &&
           (MQTT_QOS2 == qos) &&
           (false == cl_mgmt_qos2_pub_rx_update(usr_cl, msg_id)))
                goto _proc_pub_msg_rx_exit;

        /* Forward data to all subscribers of PUB topic in  server */
        proc_sub_tree_topPUB(work_buf, topic, data_buf,
                             data_len, qos, false);

        err = 0;
        if(retain) {
                struct topic_node *leaf = topic_node_create(work_buf);
                if((NULL  == leaf) ||
                   (false == node_data_update(leaf, true, data_buf, data_len,
                                              QOS_VALUE(qos), retain)))
                        err = -3;   /* Resources no more available */

                if(leaf)
                        try_node_delete(leaf);
        }

 _proc_pub_msg_rx_exit:
        DBG_INFO("Processing of PUB message from %s (0x%08x) has %s (%d)\n\r",
                 usr_cl? "client" : "plugin", usr_cl? cl_bmap_get(usr_cl) : 0,
                 err? "failed" : "succeeded", err);

        return (err < 0)? false : true;
}

static
bool proc_pub_msg_rx_locked(void *usr_cl, const struct utf8_string *topic,
                            const u8 *data_buf, u32 data_len, u16 msg_id,
                            bool dup, enum mqtt_qos qos, bool retain)
{
        return _proc_pub_msg_rx(usr_cl, topic, data_buf, data_len,
                                msg_id, qos, retain);
}

static i32 utf8_str_rd(const struct utf8_string *utf8, c8 *buf, u32 len)
{
        if((NULL == utf8) || (utf8->length > (len - 1)))
                return -1;

        buf_wr_nbytes((u8*)buf, (u8*)utf8->buffer, utf8->length);
        buf[utf8->length] = '\0';

        return utf8->length;
}

#define CONN_FLAGS_WQID_GET(conn_flags)                         \
        ((conn_flags >> 3) & QID_VMASK)        /* WILL QOS VAL */

static u16 proc_connect_rx(void *ctx_cl, u8 conn_flags,
                           struct utf8_string * const *utf8_vec, void **usr_cl)
{
        struct topic_node  *leaf = NULL;
        struct utf8_string *utf8 = NULL;
        void *app_cl = NULL;
        u16 utf8_len = 0;
        u16 rv = plugin_connect(MQC_UTF8_CLIENTID(utf8_vec),
                                MQC_UTF8_USERNAME(utf8_vec),
                                MQC_UTF8_PASSWORD(utf8_vec),
                                &app_cl);
        if(rv)
                goto proc_connect_rx_exit1; /* Admin did not permit connection */

        rv = CONNACK_RC_SVR_UNAVBL; /* Server (resource) unavailable */

        utf8 = MQC_UTF8_WILL_TOP(utf8_vec);
        if(utf8 && utf8->length) {
                utf8_str_rd(utf8, work_buf, WBUF_LEN);

                leaf = topic_node_create(work_buf);
                if(NULL == leaf)
                        goto proc_connect_rx_exit2;

                if(false == node_data_update(leaf, false, 
                                             (u8*)MQC_WILL_MSG_BUF(utf8_vec),
                                             MQC_WILL_MSG_LEN(utf8_vec),
                                             CONN_FLAGS_WQID_GET(conn_flags),
                                             conn_flags & WILL_RETAIN_VAL))
                        goto proc_connect_rx_exit3;
        }

        utf8 = MQC_UTF8_CLIENTID(utf8_vec);
        if(utf8)
                utf8_len = utf8_str_rd(utf8, work_buf, WBUF_LEN);
                
        rv = cl_connect_rx(ctx_cl, (conn_flags & CLEAN_START_VAL)? true : false,
                           utf8_len? work_buf : NULL, app_cl, leaf, usr_cl);
        if(CONNACK_RC_REQ_ACCEPT == (rv & 0xFF)) {
                if(leaf)
                        leaf->will_cl = *usr_cl;

                return rv; /* Connection successful */
        }

        if(leaf)
                node_data_update(leaf, true, NODE_DATA_RESET_PARAMS);

 proc_connect_rx_exit3: try_node_delete(leaf);
 proc_connect_rx_exit2: plugin_disconn(app_cl, true);
 proc_connect_rx_exit1:
        return rv;
}

static
u16 proc_connect_rx_locked(void *ctx_cl, u8 conn_flags,
                           struct utf8_string * const *utf8_vec, void **usr_cl)
{
        return proc_connect_rx(ctx_cl, conn_flags, utf8_vec, usr_cl);
}

static void session_hier_delete(struct topic_node *node, void *usr_cl)
{
        struct topic_node *prev = NULL;
        u32 cl_map = cl_bmap_get(usr_cl);

        while(node) {
                i32 i = 0;
                for(i = 0; i < 3; i++) {
                        if(node->cl_map[i] & cl_map) {
                                node->cl_map[i] &= ~cl_map; 
                                cl_sub_count_del(usr_cl);
                                /* Client/Topic/QID 1-to-1 map */
                                break; 
                        }
                }

                if(node->dn_nhbr)
                        stack_add(node->dn_nhbr, 0, 0);

                prev = node;
                node = node->dn_hier;
        }

        if(prev)
                try_node_delete(prev);
}

void session_tree_delete(void *usr_cl)
{
        u32 stack_ref = stack_idx;
        
        if(NULL != root_node)
                stack_add(root_node, 0, 0);

        while(stack_ref < stack_idx) {
                struct _node_stack *stack = stack_pop();
                session_hier_delete(stack->node, usr_cl);
        }
}

static void proc_client_will(struct topic_node *leaf)
{
        u32 wbuf_len = WBUF_LEN - 1; /* Make space for '\0' in wbuf */
        u32 offset   = wbuf_len;
        struct utf8_string topic;
        struct topic_node  *node;

        work_buf[offset] = '\0';  /* Ensures wbuf is NUL terminated */
        node = leaf;

        /* Prepare a topic string by walking back from leaf to root */
        do {
                if(offset < node->toplen)
                        return;

                offset -= node->toplen;
                strncpy(work_buf + offset, node->subtop, node->toplen);

                while(node->up_nhbr)
                        node = node->up_nhbr;

                node = node->up_hier;

        } while(node);

        topic.buffer = work_buf + offset;
        topic.length = wbuf_len - offset;
        
#define MK_QOS_ENUM(qid) ((enum mqtt_qos)(qid & QID_VMASK))

        proc_sub_tree_topPUB((c8*)topic.buffer, 
                             &topic, leaf->my_data, leaf->my_dlen,
                             MK_QOS_ENUM(node_qid_get(leaf)),
                             is_node_retain(leaf));

}

static void on_cl_net_close(void *usr_cl, bool due2err)
{
        struct topic_node *leaf = NULL;
        void *app_cl = NULL;

        /* See if client has a WILL that it intends to broadcast */
        leaf = (struct topic_node*) cl_will_hndl_get(usr_cl);
        if(NULL != leaf) {
                if(usr_cl != leaf->will_cl)
                        return; /* Mismatch: should never happen */

                if(due2err)
                        proc_client_will(leaf); /* pls broadcast */

                /* Network is closing, so cleanup WILL msg store */
                node_data_update(leaf, true, NODE_DATA_RESET_PARAMS);
                leaf->will_cl =  NULL;
                try_node_delete(leaf);
        }

        /* If not needed for future, delete session info */
        if(cl_can_session_delete(usr_cl))
                session_tree_delete(usr_cl);

        /* Inform app that client has been disconnected  */
        app_cl = cl_app_hndl_get(usr_cl);
        plugin_disconn(app_cl, due2err);

        cl_on_net_close(usr_cl);
}

static
void on_cl_net_close_locked(void *usr_cl, bool due2err)
{
        on_cl_net_close(usr_cl, due2err);
        return;
}

static void on_connack_send(void *usr_cl, bool clean_session)
{
        /* If asserted, then need to start w/ clean state */
        if(clean_session)
                session_tree_delete(usr_cl);  

        cl_on_connack_send(usr_cl, clean_session);

        return;
}

static
void on_connack_send_locked(void *usr_cl, bool clean_session)
{
        on_connack_send(usr_cl, clean_session);
        return;
}

static
bool proc_notify_ack_locked(void *usr_cl, u8 msg_type, u16 msg_id)
{
        return cl_notify_ack(usr_cl, msg_type, msg_id);
}

static i32 proc_topic_enroll(u8 pg_id, const struct utf8_string *topic,
                             enum mqtt_qos qos)
{
        struct topic_node *leaf = NULL;
        u16 len = 0;

        if((NULL == topic) || (NULL == topic->buffer) || (0 == topic->length))
                return -1;

        if(WBUF_LEN < (topic->length + 1))
                return -2;

        len = topic->length;
        strncpy(work_buf, topic->buffer, len);
        work_buf[len] = '\0';

        leaf = topic_node_create(work_buf);
        if(NULL == leaf)
                return -3;

        PG_MAP_VAL_SETUP(leaf->pg_map, QOS_VALUE(qos), pg_id);

        return 0;
}

static
i32 proc_topic_enroll_locked(u8 pg_id, const struct utf8_string *topic,
                             enum mqtt_qos qos)
{
        i32 rv = 0;

        MUTEX_LOCKIN();
        rv = proc_topic_enroll(pg_id, topic, qos);
        MUTEX_UNLOCK();

        return rv;
}

static i32 proc_topic_cancel(u8 pg_id, const struct utf8_string *topic)
{
        struct topic_node *leaf = NULL;
        u16 len = 0;

        if(NULL == topic)
                return -1;

        if(WBUF_LEN < (topic->length + 1))
                return -2;

        len = topic->length;
        strncpy(work_buf, topic->buffer, len);
        work_buf[len] = '\0';

        leaf = leaf_node_find(work_buf);
        if(NULL == leaf)
                return -2;

        PG_MAP_VAL_RESET(leaf->pg_map, pg_id);

        try_node_delete(leaf);

        return 0;
}

static
i32 proc_topic_cancel_locked(u8 pg_id, const struct utf8_string *topic)
{
        i32 rv = 0;

        MUTEX_LOCKIN();
        rv = proc_topic_cancel(pg_id, topic);
        MUTEX_UNLOCK();

        return rv;
}

static
i32 proc_app_pub_send_locked(const struct utf8_string *topic, const u8 *data_buf,
                             u32 data_len, enum mqtt_qos qos, bool retain)
{
        bool rv;

        MUTEX_LOCKIN();
        /* Received from application, process topic for distribution */
        rv = _proc_pub_msg_rx(NULL, topic, data_buf, data_len,
                              0x00, qos, retain);
        MUTEX_UNLOCK();

        return rv? (i32)data_len : -1;
}

i32 mqtt_server_init(const struct mqtt_server_lib_cfg *lib_cfg,
                     const struct mqtt_server_app_cfg *app_cfg)
{
        /* If mutex is specified, then the following set of callbacks
           are invoked in the locked state - enumerated by 'locked' */
        struct mqtt_server_msg_cbs pkts_cbs = {proc_connect_rx_locked, 
                                               proc_sub_msg_rx_locked,
                                               proc_un_sub_msg_locked,
                                               proc_pub_msg_rx_locked,
                                               proc_notify_ack_locked,
                                               on_cl_net_close_locked,
                                               on_connack_send_locked};

        struct plugin_core_msg_cbs core_cbs = {proc_topic_enroll_locked,
                                               proc_topic_cancel_locked,
                                               proc_app_pub_send_locked};

        util_params_set(lib_cfg->debug_printf,
                        lib_cfg->mutex,
                        lib_cfg->mutex_lockin,
                        lib_cfg->mutex_unlock);

        USR_INFO("Version: Server LIB %s, Common LIB %s.\n\r",
                 MQTT_SERVER_VERSTR, MQTT_COMMON_VERSTR);

        topic_node_init();

        cl_mgmt_init();

        plugin_init(&core_cbs);

        mqtt_server_lib_init(lib_cfg, &pkts_cbs);

        return 0;
}
