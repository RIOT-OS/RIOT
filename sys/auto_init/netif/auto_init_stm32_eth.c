/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @brief       Auto initzialize stm32 ethernet driver
 *
 * @author      Robin Lösch <robin@chilio.net>
 */

#ifdef MODULE_STM32_ETH

#include "stm32_eth.h"
#include "net/gnrc/netif/ethernet.h"

static netdev_t stm32eth;
static char stack[THREAD_STACKSIZE_DEFAULT];

void auto_init_stm32_eth(void)
{
  /* setup netdev device */
  stm32_eth_netdev_setup(&stm32eth);
        /* initialize netdev <-> gnrc adapter state */
  gnrc_netif_ethernet_create(stack, THREAD_STACKSIZE_DEFAULT, GNRC_NETIF_PRIO, "stm32_eth",
                             &stm32eth);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_STM32_ETH */
/** @} */
