#include "rpl_structs.h"

rpl_instance_t *rpl_new_instance(uint8_t instanceid);
rpl_instance_t *rpl_get_instance(uint8_t instanceid);
rpl_instance_t *rpl_get_my_instance();
rpl_dodag_t *rpl_new_dodag(uint8_t instanceid);
rpl_dodag_t *rpl_get_dodag(ipv6_addr_t *id);
rpl_dodag_t *rpl_get_my_dodag();
rpl_parent_t *rpl_new_parent(rpl_dodag_t * dodag, ipv6_addr_t address);
void rpl_del_dodag(rpl_dodag_t *dodag);
void rpl_leave_dodag(rpl_dodag_t * dodag);
void rpl_join_dodag();
bool rpl_equal_dodag_id(ipv6_addr_t *id1, ipv6_addr_t *id2);

