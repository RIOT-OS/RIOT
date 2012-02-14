#include "rpl_structs.h"
//#include "of0.h"

rpl_instance_t *rpl_new_instance(uint8_t instanceid);
rpl_instance_t *rpl_get_instance(uint8_t instanceid);
rpl_instance_t *rpl_get_my_instance();
rpl_dodag_t *rpl_new_dodag(uint8_t instanceid, ipv6_addr_t *id);
rpl_dodag_t *rpl_get_dodag(ipv6_addr_t *id);
rpl_dodag_t *rpl_get_my_dodag();
void rpl_join_dodag(rpl_dodag_t *dodag, ipv6_addr_t *parent, uint16_t parent_rank);
void rpl_del_dodag(rpl_dodag_t *dodag);
rpl_parent_t *rpl_new_parent(rpl_dodag_t *dodag, ipv6_addr_t *address, uint16_t rank);
rpl_parent_t *rpl_find_parent(ipv6_addr_t *address);
void rpl_leave_dodag(rpl_dodag_t * dodag);
bool rpl_equal_id(ipv6_addr_t *id1, ipv6_addr_t *id2);
void rpl_global_repair(rpl_dodag_t *dodag);
ipv6_addr_t *rpl_get_my_preferred_parent();
