/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @brief       Auto initzialize stm32 ethernet driver
 *
 * @author      Robin Lösch <robin@chilio.net>
 */

#include "stm32_eth.h"
#include "net/gnrc/netif/ethernet.h"

static netdev_t stm32eth;
static char stack[THREAD_STACKSIZE_DEFAULT];
static gnrc_netif_t _netif;

void auto_init_stm32_eth(void)
{
  /* setup netdev device */
  stm32_eth_netdev_setup(&stm32eth);
        /* initialize netdev <-> gnrc adapter state */
  gnrc_netif_ethernet_create(&_netif, stack, THREAD_STACKSIZE_DEFAULT, GNRC_NETIF_PRIO, "stm32_eth",
                             &stm32eth);
}
/** @} */
