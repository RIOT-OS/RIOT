/**
 * RPL dodag implementation
 *
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup rpl
 * @{
 * @file    rpl_of_manager.h
 * @brief   RPL Objective functions manager header
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @}
 */

 #include "rpl_structs.h"
 #include "rpl_config.h"

void init_of_manager(ipv6_addr_t *my_address);

//Returns objective function with a given cope point
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp);