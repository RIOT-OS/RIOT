/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_eth
 * @{
 *
 * @file
 * @brief       Netdev interface for the ESP32 Ethernet MAC (EMAC) module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifdef MODULE_ESP_ETH

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "log.h"
#include "tools.h"

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"

#include "xtimer.h"

#include "esp_common.h"
#include "esp_attr.h"
#include "irq_arch.h"

#include "esp_eth_params.h"
#include "esp_eth_netdev.h"

#include "esp32/esp_event_loop.h"

#if !defined(EMAC_PHY_SMI_MDC_PIN) || !defined(EMAC_PHY_SMI_MDIO_PIN)
#error Board definition does not provide EMAC configuration
#endif

#define SYSTEM_EVENT_ETH_RX_DONE    (SYSTEM_EVENT_MAX + 1)
#define SYSTEM_EVENT_ETH_TX_DONE    (SYSTEM_EVENT_MAX + 2)

#define EMAC_PHY_CLOCK_DELAY        (500 * USEC_PER_MSEC) /* given in us */

#ifdef EMAC_PHY_LAN8720
#include "eth_phy/phy_lan8720.h"
#define EMAC_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config
#endif
#ifdef EMAC_PHY_TLK110
#include "eth_phy/phy_tlk110.h"
#define EMAC_ETHERNET_PHY_CONFIG phy_tlk110_default_ethernet_config
#endif

/**
 * There is only one ESP-ETH device. We define it as static device variable
 * to have accesss to the device inside ESP-ETH interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP-ETH
 * device which triggers the interrupt.
 */
static esp_eth_netdev_t _esp_eth_dev;

/* device thread stack */
static char _esp_eth_stack[ESP_ETH_STACKSIZE];

static void _eth_gpio_config_rmii(void)
{
    DEBUG("%s\n", __func__);
    /*
     * RMII data pins are fixed:
     * TXD0  = GPIO19
     * TXD1  = GPIO22
     * TX_EN = GPIO21
     * RXD0  = GPIO25
     * RXD1  = GPIO26
     * CLK   = GPIO0
    */
    phy_rmii_configure_data_interface_pins();

    /* SMI pins can be configured in board definition */
    phy_rmii_smi_configure_pins(EMAC_PHY_SMI_MDC_PIN, EMAC_PHY_SMI_MDIO_PIN);
}

static esp_err_t IRAM_ATTR _eth_input_callback(void *buffer, uint16_t len, void *eb)
{
    DEBUG("%s: buf=%p len=%d eb=%p\n", __func__, buffer, len, eb);

    CHECK_PARAM_RET (buffer != NULL, -EINVAL);
    CHECK_PARAM_RET (len <= ETHERNET_DATA_LEN, -EINVAL);

    mutex_lock(&_esp_eth_dev.dev_lock);

    memcpy(_esp_eth_dev.rx_buf, buffer, len);
    _esp_eth_dev.rx_len = len;
    _esp_eth_dev.event = SYSTEM_EVENT_ETH_RX_DONE;
    _esp_eth_dev.netdev.event_callback(&_esp_eth_dev.netdev, NETDEV_EVENT_ISR);

    mutex_unlock(&_esp_eth_dev.dev_lock);

    return ESP_OK;
}

#if EMAC_PHY_POWER_PIN != GPIO_UNDEF
static void _esp_eth_phy_power_enable_gpio(bool enable)
{
    assert(EMAC_ETHERNET_PHY_CONFIG.phy_power_enable);

    if (!enable) {
        EMAC_ETHERNET_PHY_CONFIG.phy_power_enable(false);
    }

    gpio_init(EMAC_PHY_POWER_PIN, GPIO_OUT);
    gpio_write(EMAC_PHY_POWER_PIN, enable);

    xtimer_usleep (USEC_PER_MSEC);

    if (enable) {
        EMAC_ETHERNET_PHY_CONFIG.phy_power_enable(true);
    }
}
#endif

static int _esp_eth_init(netdev_t *netdev)
{
    DEBUG("%s: netdev=%p\n", __func__, netdev);

    esp_eth_netdev_t* dev = (esp_eth_netdev_t*)netdev;

    mutex_lock(&dev->dev_lock);

    esp_err_t ret = ESP_OK;
    eth_config_t config = EMAC_ETHERNET_PHY_CONFIG;

    /* Set configuration */
    config.phy_addr = EMAC_PHY_ADDRESS;
    config.clock_mode = EMAC_PHY_CLOCK_MODE;
    config.gpio_config = _eth_gpio_config_rmii;
    config.tcpip_input = _eth_input_callback;

    #if EMAC_PHY_POWER_PIN != GPIO_UNDEF
    /*
     * Replace the default 'power enable' function with an example-specific
     * one that toggles a power GPIO.
     */
    config.phy_power_enable = phy_device_power_enable_via_gpio;
    #endif

    /* initialize EMAC with config */
    if (ret == ESP_OK && (ret = esp_eth_init(&config)) != ESP_OK) {
        LOG_TAG_ERROR("esp_eth", "initialization failed");
    }

    /*
     * after activating clock logic it can take some time before we can
     * enable EMAC
     */
    xtimer_usleep (EMAC_PHY_CLOCK_DELAY);

    if (ret == ESP_OK && (ret = esp_eth_enable()) != ESP_OK) {
        LOG_TAG_ERROR("esp_eth", "enable failed");
    }

    #ifdef MODULE_NETSTATS_L2
    memset(&netdev->stats, 0, sizeof(netstats_t));
    #endif

    mutex_unlock(&dev->dev_lock);

    return (ret == ESP_OK) ? 0 : -1;
}

static int _esp_eth_send(netdev_t *netdev, const iolist_t *iolist)
{
    DEBUG("%s: netdev=%p iolist=%p\n", __func__, netdev, iolist);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (iolist != NULL, -EINVAL);

    esp_eth_netdev_t* dev = (esp_eth_netdev_t*)netdev;

    if (!_esp_eth_dev.link_up) {
        DEBUG("%s: link is down\n", __func__);
        return -ENODEV;
    }

    mutex_lock(&dev->dev_lock);

    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ETHERNET_DATA_LEN) {
            mutex_unlock(&dev->dev_lock);
            return -EOVERFLOW;
        }
        memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
        dev->tx_len += iol->iol_len;
    }

    #if ENABLE_DEBUG
    printf ("%s: send %d byte\n", __func__, dev->tx_len);
    /* esp_hexdump (dev->tx_buf, dev->tx_len, 'b', 16); */
    #endif

    int ret = 0;

    /* send the the packet to the peer(s) mac address */
    if (esp_eth_tx(dev->tx_buf, dev->tx_len) == ESP_OK) {
        #ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_success++;
        netdev->stats.tx_bytes += dev->tx_len;
        #endif
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    else {
        #ifdef MODULE_NETSTATS_L2
        netdev->stats.tx_failed++;
        #endif
        ret = -EIO;
    }

    mutex_unlock(&dev->dev_lock);
    return ret;
}

static int _esp_eth_recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    DEBUG("%s: netdev=%p buf=%p len=%u info=%p\n",
          __func__, netdev, buf, len, info);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);

    esp_eth_netdev_t* dev = (esp_eth_netdev_t*)netdev;

    mutex_lock(&dev->dev_lock);

    uint8_t size = dev->rx_len;

    if (!buf && !len) {
        /* return the size without dropping received data */
        mutex_unlock(&dev->dev_lock);
        return size;
    }

    if (!buf && len) {
        /* return the size and drop received data */
        mutex_unlock(&dev->dev_lock);
        dev->rx_len = 0;
        return size;
    }

    if (buf && len && dev->rx_len) {
        /* return the packet */

        if (dev->rx_len > len) {
            LOG_TAG_ERROR("esp_eth", "Not enough space in receive buffer "
                          "for %d byte\n", dev->rx_len);
            mutex_unlock(&dev->dev_lock);
            return -ENOBUFS;
        }

        #if ENABLE_DEBUG
        /* esp_hexdump (dev->rx_buf, dev->rx_len, 'b', 16); */
        #endif

        /* copy received date and reset the receive length */
        memcpy(buf, dev->rx_buf, dev->rx_len);
        dev->rx_len = 0;

        #ifdef MODULE_NETSTATS_L2
        netdev->stats.rx_count++;
        netdev->stats.rx_bytes += size;
        #endif

        mutex_unlock(&dev->dev_lock);
        return size;
    }

    mutex_unlock(&dev->dev_lock);
    return -EINVAL;
}

static int _esp_eth_get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    DEBUG("%s: netdev=%p opt=%s val=%p len=%u\n",
          __func__, netdev, netopt2str(opt), val, max_len);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (val != NULL, -EINVAL);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            esp_eth_get_mac((uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_get(netdev, opt, val, max_len);
    }
}

static int _esp_eth_set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len)
{
    DEBUG("%s: netdev=%p opt=%s val=%p len=%u\n",
          __func__, netdev, netopt2str(opt), val, max_len);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (val != NULL, -EINVAL);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            esp_eth_set_mac((uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, val, max_len);
    }
}

static void _esp_eth_isr(netdev_t *netdev)
{
    DEBUG("%s: netdev=%p\n", __func__, netdev);

    CHECK_PARAM (netdev != NULL);

    esp_eth_netdev_t *dev = (esp_eth_netdev_t *) netdev;

    switch (dev->event) {
        case SYSTEM_EVENT_ETH_RX_DONE:
            /* if data were received */
            if (dev->rx_len) {
                dev->netdev.event_callback(netdev, NETDEV_EVENT_RX_COMPLETE);
            }
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_UP);
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            dev->netdev.event_callback(netdev, NETDEV_EVENT_LINK_DOWN);
            break;
        default:
            break;
    }
    _esp_eth_dev.event = SYSTEM_EVENT_MAX; /* no event */

    return;
}

static esp_err_t IRAM_ATTR _esp_system_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
        case SYSTEM_EVENT_ETH_START:
            DEBUG("%s: Ethernet started\n", __func__);
            break;
        case SYSTEM_EVENT_ETH_STOP:
            DEBUG("%s: Ethernet started\n", __func__);
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            DEBUG("%s Ethernet link up\n", __func__);
            _esp_eth_dev.link_up = true;
            if (SYSTEM_EVENT_MAX) {
                _esp_eth_dev.event = SYSTEM_EVENT_ETH_CONNECTED;
                _esp_eth_dev.netdev.event_callback(&_esp_eth_dev.netdev, NETDEV_EVENT_ISR);
            }
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            DEBUG("%s: Ethernet link up\n", __func__);
            _esp_eth_dev.link_up = false;
            if (SYSTEM_EVENT_MAX) {
                _esp_eth_dev.event = SYSTEM_EVENT_ETH_DISCONNECTED;
                _esp_eth_dev.netdev.event_callback(&_esp_eth_dev.netdev, NETDEV_EVENT_ISR);
            }
            break;
        default:
            DEBUG("%s: event=%d\n", __func__, event->event_id);
            break;
    }
    return ESP_OK;
}

static const netdev_driver_t _esp_eth_driver =
{
    .send = _esp_eth_send,
    .recv = _esp_eth_recv,
    .init = _esp_eth_init,
    .isr = _esp_eth_isr,
    .get = _esp_eth_get,
    .set = _esp_eth_set,
};

extern esp_err_t esp_system_event_add_handler (system_event_cb_t handler,
                                               void *arg);

void auto_init_esp_eth (void)
{
    LOG_TAG_INFO("esp_eth", "initializing ESP32 Ethernet MAC (EMAC) device\n");

    /* initialize locking */
    mutex_init(&_esp_eth_dev.dev_lock);

    /* init esp system event loop */
    esp_system_event_add_handler(_esp_system_event_handler, NULL);

    /* set the netdev driver */
    _esp_eth_dev.netdev.driver = &_esp_eth_driver;

    /* initialize netdev data structure */
    _esp_eth_dev.event = SYSTEM_EVENT_MAX; /* no event */
    _esp_eth_dev.link_up = false;
    _esp_eth_dev.rx_len = 0;
    _esp_eth_dev.tx_len = 0;
    _esp_eth_dev.netif = gnrc_netif_ethernet_create(_esp_eth_stack,
                                                   ESP_ETH_STACKSIZE,
                                                   ESP_ETH_PRIO,
                                                   "netdev-esp-eth",
                                                   (netdev_t *)&_esp_eth_dev);
}

#endif /* MODULE_ESP_ETH */
/**@}*/
