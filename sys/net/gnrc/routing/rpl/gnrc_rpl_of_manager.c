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

static struct of_list {
    struct of_list *next;
    gnrc_rpl_of_t *of;
} *of_list_head;

void gnrc_rpl_of_manager_init(void)
{
    if (IS_USED(MODULE_GNRC_RPL_OF0)) {
        static struct of_list of0;
        of0.of = gnrc_rpl_get_of0();
        of0.next = of_list_head;
        of_list_head = &of0;
    }
    if (IS_USED(MODULE_GNRC_RPL_MRHOF)) {
        static struct of_list of_mrhof;
        of_mrhof.of = gnrc_rpl_get_of_mrhof();
        of_mrhof.next = of_list_head;
        of_list_head = &of_mrhof;
    }

    /* insert new objective functions here */
}

/* find implemented OF via objective code point */
gnrc_rpl_of_t *gnrc_rpl_get_of_for_ocp(uint16_t ocp)
{
    for (struct of_list *of = of_list_head; of; of = of->next) {
        if (ocp == of->of->ocp) {
            return of->of;
        }
    }

    return NULL;
}
