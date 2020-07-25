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
 * @ingroup net_gnrc_rpl
 * @{
 * @file
 * @brief   RPL Objective functions manager
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @}
 */

#include "net/gnrc/rpl.h"
#include "net/gnrc/rpl/of_manager.h"
#include "of0.h"
#include "mrhof.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static gnrc_rpl_of_t *objective_functions[GNRC_RPL_IMPLEMENTED_OFS_NUMOF];

void gnrc_rpl_of_manager_init(void)
{
    /* insert new objective functions here */
    objective_functions[0] = gnrc_rpl_get_of0();
    objective_functions[1] = gnrc_rpl_get_of_mrhof();
}

/* find implemented OF via objective code point */
gnrc_rpl_of_t *gnrc_rpl_get_of_for_ocp(uint16_t ocp)
{
    for (uint16_t i = 0; i < GNRC_RPL_IMPLEMENTED_OFS_NUMOF; i++) {
        if (objective_functions[i] == NULL) {
            /* fallback if something goes wrong */
            return gnrc_rpl_get_of0();
        }
        else if (ocp == objective_functions[i]->ocp) {
            return objective_functions[i];
        }
    }

    return NULL;
}
