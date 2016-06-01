/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup gnrc_rpl
 * @{
 *
 * @file
 * @brief   Auto initialization for gnrc_rpl
 *
 * @author  Cenk Gündoğan <mail@cgundogan.de>
 */

#ifdef MODULE_AUTO_INIT_GNRC_RPL

#include "net/gnrc.h"
#include "net/gnrc/rpl.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void auto_init_gnrc_rpl(void)
{
#if (GNRC_NETIF_NUMOF == 1)
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    gnrc_netif_get(ifs);
    DEBUG("auto_init_gnrc_rpl: initializing RPL on interface %" PRIkernel_pid "\n", ifs[0]);
    gnrc_rpl_init(ifs[0]);
    return;
#elif defined(GNRC_RPL_DEFAULT_NETIF)
    if (gnrc_netif_exist(GNRC_RPL_DEFAULT_NETIF)) {
        DEBUG("auto_init_gnrc_rpl: initializing RPL on interface %" PRIkernel_pid "\n",
              GNRC_RPL_DEFAULT_NETIF);
        gnrc_rpl_init(GNRC_RPL_DEFAULT_NETIF);
        return;
    }
    DEBUG("auto_init_gnrc_rpl: could not initialize RPL on interface %" PRIkernel_pid" - "
          "interface does not exist\n", GNRC_RPL_DEFAULT_NETIF);
    return;
#else
    DEBUG("auto_init_gnrc_rpl: please specify an interface by setting GNRC_RPL_DEFAULT_NETIF\n");
#endif
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_AUTO_INIT_GNRC_RPL */

/** @} */
