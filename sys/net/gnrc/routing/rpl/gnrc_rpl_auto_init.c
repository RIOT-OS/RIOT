/*
 * Copyright (C) 2016 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup net_gnrc_rpl
 * @{
 *
 * @file
 * @brief   Auto initialization for gnrc_rpl
 *
 * @author  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#ifdef MODULE_AUTO_INIT_GNRC_RPL

#include "log.h"
#include "net/gnrc.h"
#include "net/gnrc/rpl.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void auto_init_gnrc_rpl(void)
{
    if (gnrc_netif_highlander() || gnrc_netif_numof() == 1) {
        gnrc_netif_t *netif = gnrc_netif_iter(NULL);
        if (netif == NULL) {
            /* XXX this is just a work-around ideally this would happen with
             * an `up` event of the interface */
            LOG_INFO("Unable to auto-initialize RPL. No interfaces found.\n");
            return;
        }
        DEBUG("auto_init_gnrc_rpl: initializing RPL on interface %" PRIkernel_pid "\n",
              netif->pid);
        gnrc_rpl_init(netif->pid);
        return;
    }
    else if (CONFIG_GNRC_RPL_DEFAULT_NETIF != KERNEL_PID_UNDEF) {
        if (gnrc_netif_get_by_pid(CONFIG_GNRC_RPL_DEFAULT_NETIF) != NULL) {
            DEBUG("auto_init_gnrc_rpl: initializing RPL on interface %" PRIkernel_pid "\n",
                  (kernel_pid_t) CONFIG_GNRC_RPL_DEFAULT_NETIF);
            gnrc_rpl_init(CONFIG_GNRC_RPL_DEFAULT_NETIF);
            return;
        }
        /* XXX this is just a work-around ideally this would happen with
         * an `up` event of the CONFIG_GNRC_RPL_DEFAULT_NETIF */
        DEBUG("auto_init_gnrc_rpl: could not initialize RPL on interface %" PRIkernel_pid" - "
              "interface does not exist\n", (kernel_pid_t) CONFIG_GNRC_RPL_DEFAULT_NETIF);
        return;
    }
    else {
        /* XXX this is just a work-around ideally this should be defined in some
         * run-time interface configuration */
        DEBUG("auto_init_gnrc_rpl: please specify an interface "
               "by setting CONFIG_GNRC_RPL_DEFAULT_NETIF\n");
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_AUTO_INIT_GNRC_RPL */

/** @} */
