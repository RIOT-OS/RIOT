#include "kernel_defines.h"

void lwip_netif_init_devs(void)
{
    if (IS_USED(MODULE_NETDEV_TAP)) {
        extern void auto_init_netdev_tap(void);
        auto_init_netdev_tap();
    }
}
