#include "esp-eth/esp_eth_netdev.h"

#include "lwip.h"
#include "lwip/netif.h"

#define ENABLE_DEBUG    0
#include "debug.h"

extern esp_eth_netdev_t _esp_eth_dev;
extern void esp_eth_setup(esp_eth_netdev_t* dev);

static struct netif netif;

void auto_init_esp_eth(void)
{
    esp_eth_setup(&_esp_eth_dev);
    if (lwip_add_ethernet(&netif, &_esp_eth_dev.netdev) == NULL) {
        DEBUG("Could not add esp_eth device\n");
    }
}
