#include "esp-wifi/esp_wifi_netdev.h"

#include "lwip.h"
#include "lwip/netif.h"

#define ENABLE_DEBUG    0
#include "debug.h"

extern esp_wifi_netdev_t _esp_wifi_dev;
extern void esp_wifi_setup(esp_wifi_netdev_t *dev);

static struct netif netif;

void auto_init_esp_wifi(void)
{
    esp_wifi_setup(&_esp_wifi_dev);
    if (lwip_add_ethernet(&netif, &_esp_wifi_dev.netdev) == NULL) {
        DEBUG("Could not add esp_eth device\n");
    }
}
