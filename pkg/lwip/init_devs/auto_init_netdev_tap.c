#include "netdev_tap.h"
#include "netdev_tap_params.h"

#include "lwip.h"
#include "lwip/netif.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define NETIF_TAP_NUMOF        (NETDEV_TAP_MAX)

static struct netif netif[NETIF_TAP_NUMOF];
static netdev_tap_t netdev_taps[NETIF_TAP_NUMOF];

void auto_init_netdev_tap(void)
{
    for (unsigned i = 0; i < NETIF_TAP_NUMOF; i++) {
        netdev_tap_setup(&netdev_taps[i], &netdev_tap_params[i]);
        if (lwip_add_ethernet(&netif[i], &netdev_taps[i].netdev) == NULL) {
            DEBUG("Could not add netdev_tap device\n");
            return;
        }
    }
}
