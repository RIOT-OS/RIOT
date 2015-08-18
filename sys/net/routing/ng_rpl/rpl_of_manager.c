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
 * @file
 * @brief   RPL Objective functions manager
 * @author  Fabian Brandt <fabianbr@zedat.fu-berlin.de>
 * @}
 */



#include "net/ng_rpl.h"
#include "net/ng_rpl/of_manager.h"
#include "of0.h"

#define ENABLE_DEBUG (0)
#include "debug.h"
/* !!! TODO: port etx/mrhof to the new network stack */

static ng_rpl_of_t *objective_functions[NG_RPL_IMPLEMENTED_OFS_NUMOF];

void ng_rpl_of_manager_init(void)
{
    /* insert new objective functions here */
    objective_functions[0] = ng_rpl_get_of0();
    /*objective_functions[1] = ng_rpl_get_of_mrhof(); */
}

/* find implemented OF via objective code point */
ng_rpl_of_t *ng_rpl_get_of_for_ocp(uint16_t ocp)
{
    for (uint16_t i = 0; i < NG_RPL_IMPLEMENTED_OFS_NUMOF; i++) {
        if (objective_functions[i] == NULL) {
            /* fallback if something goes wrong */
            return ng_rpl_get_of0();
        }
        else if (ocp == objective_functions[i]->ocp) {
            return objective_functions[i];
        }
    }

    return NULL;
}
