/*
 * Copyright (C) 2013, 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup rpl
 * @{
 *
 * @file        rpl_dodag.h
 * @brief       RPL DODAG header
 *
 * Header file, which defines all public known DODAG-related functions for RPL.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include "ipv6.h"
#include "rpl_structs.h"
#include "rpl_config.h"

#ifdef __cplusplus
extern "C" {
#endif

void rpl_dao_ack_received(rpl_dodag_t *dodag);
void rpl_delay_dao(rpl_dodag_t *dodag);
void rpl_instances_init(void);
rpl_instance_t *rpl_new_instance(uint8_t instanceid);
rpl_instance_t *rpl_get_instance(uint8_t instanceid);
rpl_instance_t *rpl_get_my_instance(void);
rpl_dodag_t *rpl_new_dodag(rpl_instance_t *inst, ipv6_addr_t *id);
rpl_dodag_t *rpl_get_joined_dodag(uint8_t instanceid);
rpl_dodag_t *rpl_get_my_dodag(void);
void rpl_join_dodag(rpl_dodag_t *dodag, ipv6_addr_t *parent, uint16_t parent_rank);
void rpl_del_dodag(rpl_dodag_t *dodag);
rpl_parent_t *rpl_new_parent(rpl_dodag_t *dodag, ipv6_addr_t *address, uint16_t rank);
rpl_parent_t *rpl_find_parent(ipv6_addr_t *address);
void rpl_leave_dodag(rpl_dodag_t *dodag);
bool rpl_equal_id(ipv6_addr_t *id1, ipv6_addr_t *id2);
ipv6_addr_t *rpl_get_my_preferred_parent(void);
void rpl_delete_parent(rpl_parent_t *parent);
void rpl_delete_worst_parent(void);
void rpl_delete_all_parents(void);
rpl_parent_t *rpl_find_preferred_parent(void);
void rpl_parent_update(rpl_parent_t *parent);
void rpl_global_repair(rpl_dodag_t *dodag, ipv6_addr_t *p_addr, uint16_t rank);
void rpl_local_repair(void);
uint16_t rpl_calc_rank(uint16_t abs_rank, uint16_t minhoprankincrease);

#ifdef __cplusplus
}
#endif
