/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief   RPL Objective functions manager header
 *
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 */

#ifndef NET_GNRC_RPL_OF_MANAGER_H
#define NET_GNRC_RPL_OF_MANAGER_H

#include "structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialization of Manager and of-functions.
*/
void gnrc_rpl_of_manager_init(void);

/**
 * @brief Returns objective function with a given cope point
 * @param[in]   ocp Objective code point of objective function
 * @return      Pointer of corresponding objective function implementation
*/
gnrc_rpl_of_t *gnrc_rpl_get_of_for_ocp(uint16_t ocp);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_RPL_OF_MANAGER_H */
/** @} */
