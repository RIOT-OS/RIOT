/*
* RPL dodag implementation
*
* Copyright (C) 2014 Freie Universität Berlin
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

/**
 *
 * @ingroup rpl
 * @{
 * @file    rpl_of_manager.c
 * @brief   RPL Objective functions manager
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @}
 */



#include "rpl/rpl_of_manager.h"
#include "of0.h"
#include "of_mrhof.h"
#include "etx_beaconing.h"
#include "rpl/rpl_config.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

rpl_of_t *objective_functions[NUMBER_IMPLEMENTED_OFS];

void rpl_of_manager_init(ipv6_legacy_addr_t *my_address)
{
    /* insert new objective functions here */
    objective_functions[0] = rpl_get_of0();
    objective_functions[1] = rpl_get_of_mrhof();

    if (RPL_DEFAULT_OCP == 1) {
        DEBUG("%s, %d: INIT ETX BEACONING\n", __FILE__, __LINE__);
        etx_init_beaconing(my_address);
    }
}

/* find implemented OF via objective code point */
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp)
{
    for (uint16_t i = 0; i < NUMBER_IMPLEMENTED_OFS; i++) {
        if (objective_functions[i] == NULL) {
            /* fallback if something goes wrong */
            return rpl_get_of0();
        }
        else if (ocp == objective_functions[i]->ocp) {
            return objective_functions[i];
        }
    }

    return NULL;
}
