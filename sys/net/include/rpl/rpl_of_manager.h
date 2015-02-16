/*
* RPL dodag implementation
*
* Copyright (C) 2014 Freie Universität Berlin
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*
* @ingroup rpl
* @{
* @file    rpl_of_manager.h
* @brief   RPL Objective functions manager header
* @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
* @}
*/

#ifndef __RPL_OFM_H
#define __RPL_OFM_H

#include "rpl_structs.h"
#include "rpl_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialization of Manager and of-functions.
 * @param[in]   my_address Own address for initialization of beaconing
*/
void rpl_of_manager_init(ipv6_addr_t *my_address);

/**
 * @brief Returns objective function with a given cope point
 * @param[in]   ocp Objective code point of objective function
 * @return      Pointer of corresponding objective function implementation
*/
rpl_of_t *rpl_get_of_for_ocp(network_uint16_t ocp);

#ifdef __cplusplus
}
#endif

#endif /* __RPL_OFM_H */
/** @} */
