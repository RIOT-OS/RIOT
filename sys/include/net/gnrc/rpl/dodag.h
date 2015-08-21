/*
 * Copyright (C) 2013  INRIA.
 * Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       DODAG-related functions for RPL
 *
 * Header file, which defines all public known DODAG-related functions for RPL.
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 * @author      Cenk Gündoğan <cnkgndgn@gmail.com>
 */

#ifndef GNRC_RPL_DODAG_H_
#define GNRC_RPL_DODAG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ipv6/addr.h"
#include "trickle.h"
#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/structs.h"

/**
 * @brief   Number of RPL instances
 */
#ifndef GNRC_RPL_INSTANCES_NUMOF
#define GNRC_RPL_INSTANCES_NUMOF (2)
#endif

/**
 * @brief   Number of RPL dodags
 */
#ifndef GNRC_RPL_DODAGS_NUMOF
#define GNRC_RPL_DODAGS_NUMOF (4)
#endif

/**
 * @brief   Number of RPL parents
 */
#ifndef GNRC_RPL_PARENTS_NUMOF
#define GNRC_RPL_PARENTS_NUMOF (6)
#endif

/**
 * @brief   RPL instance table
 */
extern gnrc_rpl_instance_t gnrc_rpl_instances[GNRC_RPL_INSTANCES_NUMOF];

/**
 * @brief   RPL DODAG table
 */
extern gnrc_rpl_dodag_t gnrc_rpl_dodags[GNRC_RPL_DODAGS_NUMOF];

/**
 * @brief   RPL parent table
 */
extern gnrc_rpl_parent_t gnrc_rpl_parents[GNRC_RPL_PARENTS_NUMOF];

/**
 * @brief   Add a new RPL instance with the id @p instance_id.
 *
 * @param[in]   instance_id     The instance id of the new RPL instance.
 * @param[out]  inst            Pointer to an existing or new instance. Otherwise NULL.
 *
 * @return  true, if instance could be created.
 * @return  false, if instance could not be created or exists already.
 */
bool gnrc_rpl_instance_add(uint8_t instance_id, gnrc_rpl_instance_t **inst);

/**
 * @brief   Remove a RPL instance with the id @p instance_id.
 *
 * @param[in] instance_id     The instance id of the RPL instance to remove.
 *
 * @return  true, on success.
 * @return  false, otherwise.
 */
bool gnrc_rpl_instance_remove_by_id(uint8_t instance_id);

/**
 * @brief   Remove a RPL instance with the pointer @p inst.
 *
 * @param[in] inst     Pointer to the the RPL instance to remove.
 *
 * @return  true, on success.
 * @return  false, otherwise.
 */
bool gnrc_rpl_instance_remove(gnrc_rpl_instance_t *inst);

/**
 * @brief   Get the RPL instance with the id @p instance_id.
 *
 * @param[in] instance_id     The instance id of the RPL instance to get.
 *
 * @return  Pointer to the RPL instance, on success.
 * @return  NULL, otherwise.
 */
gnrc_rpl_instance_t *gnrc_rpl_instance_get(uint8_t instance_id);

/**
 * @brief   Add a new RPL DODAG with the id @p dodag_id to the instance @p instance.
 *
 * @param[in]   instance        Pointer to the instance to add the DODAG to
 * @param[in]   dodag_id        The DODAG-ID of the new DODAG
 * @param[out]  dodag           Pointer to an existing or new DODAG. Otherwise NULL.
 *
 * @return  true, if DODAG could be created.
 * @return  false, if DODAG could not be created or exists already.
 */
bool gnrc_rpl_dodag_add(gnrc_rpl_instance_t *instance, ipv6_addr_t *dodag_id,
                        gnrc_rpl_dodag_t **dodag);

/**
 * @brief   Remove the RPL DODAG @p dodag.
 *
 * @param[in] dodag     Pointer to the dodag.
 *
 * @return  true, on success.
 * @return  false, otherwise.
 */
bool gnrc_rpl_dodag_remove(gnrc_rpl_dodag_t *dodag);

/**
 * @brief   Remove all parents from the @p dodag.
 *
 * @param[in] dodag     Pointer to the dodag.
 */
void gnrc_rpl_dodag_remove_all_parents(gnrc_rpl_dodag_t *dodag);

/**
 * @brief   Get the RPL DODAG with the id @p dodag_id to the instance @p instance.
 *
 * @param[in] instance      Pointer to the instance of the RPL DODAG
 * @param[in] dodag_id      The DODAG-ID of the RPL DODAG
 *
 * @return  Pointer to the existing RPL DODAG, on success.
 * @return  NULL, otherwise.
 */
gnrc_rpl_dodag_t *gnrc_rpl_dodag_get(gnrc_rpl_instance_t *instance, ipv6_addr_t *dodag_id);

/**
 * @brief   Add a new parent with the IPv6 address @p addr to the @p dodag.
 *
 * @param[in]   dodag           Pointer to the DODAG
 * @param[in]   addr            IPV6 address of the parent
 * @param[out]  parent          Pointer to an existing or new parent. Otherwise NULL.
 *
 * @return  true. if parent could be created.
 * @return  false, if parent could not be created or exists already.
 */
bool gnrc_rpl_parent_add_by_addr(gnrc_rpl_dodag_t *dodag, ipv6_addr_t *addr,
                                 gnrc_rpl_parent_t **parent);

/**
 * @brief   Remove the @p parent from its DODAG.
 *
 * @param[in] parent     Pointer to the parent.
 *
 * @return  true, on success.
 * @return  false, otherwise.
 */
bool gnrc_rpl_parent_remove(gnrc_rpl_parent_t *parent);

/**
 * @brief   Get a parent with the IPv6 addr @p addr of the DODAG @p dodag.
 *
 * @param[in] dodag     Pointer to the DODAG
 * @param[in] addr      IPv6 address of the parent
 *
 * @return  Pointer to the existing parent, on success.
 * @return  NULL, otherwise.
 */
gnrc_rpl_parent_t *gnrc_rpl_parent_get(gnrc_rpl_dodag_t *dodag, ipv6_addr_t *addr);

/**
 * @brief   Update a @p parent of the @p dodag.
 *
 * @param[in] dodag     Pointer to the DODAG
 * @param[in] parent    Pointer to the parent
 */
void gnrc_rpl_parent_update(gnrc_rpl_dodag_t *dodag, gnrc_rpl_parent_t *parent);

/**
 * @brief   Start a local repair.
 *
 * @param[in] dodag     Pointer to the DODAG
 */
void gnrc_rpl_local_repair(gnrc_rpl_dodag_t *dodag);
#ifdef __cplusplus
}
#endif

#endif /* GNRC_RPL_DODAG_H_ */
/**
 * @}
 */
