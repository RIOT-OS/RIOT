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

#include "log.h"
#include "tools.h"

#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>

#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc.h"
#include "net/ethernet.h"
#include "net/netdev/eth.h"

#include "od.h"
#include "timex.h"

#include "esp_common.h"
#include "esp_attr.h"
#include "irq_arch.h"

#include "esp_eth_params.h"
#include "esp_eth_netdev.h"

#include "esp_eth.h"
#include "esp_eth_phy.h"
#include "esp_eth_mac.h"
#include "esp_eth_mac_esp.h"
#include "esp_event.h"
#include "esp_event_legacy.h"

#include "board.h"

#define ENABLE_DEBUG_HEXDUMP    0
#define ENABLE_DEBUG            0
#include "debug.h"

#if !defined(EMAC_PHY_SMI_MDC_PIN) || !defined(EMAC_PHY_SMI_MDIO_PIN)
#error Board definition does not provide EMAC configuration
#endif

#define SYSTEM_EVENT_ETH_RX_DONE    (SYSTEM_EVENT_MAX + 1)
#define SYSTEM_EVENT_ETH_TX_DONE    (SYSTEM_EVENT_MAX + 2)

#define EMAC_PHY_CLOCK_DELAY_MS     (500)

#if defined(EMAC_PHY_DP83848)
#define esp_eth_phy_new_xxxxx(cfg)  esp_eth_phy_new_dp83848(cfg)
#elif defined(EMAC_PHY_IP101G)
#define esp_eth_phy_new_xxxxx(cfg)  esp_eth_phy_new_ip101(cfg)
#elif defined(EMAC_PHY_LAN8720)
#define esp_eth_phy_new_xxxxx(cfg)  esp_eth_phy_new_lan87xx(cfg)
#elif defined(EMAC_PHY_KSZ8041)
#define esp_eth_phy_new_xxxxx(cfg)  esp_eth_phy_new_ksz8041(cfg)
#elif defined(EMAC_PHY_KSZ8081)
#define esp_eth_phy_new_xxxxx(cfg)  esp_eth_phy_new_ksz8081(cfg)
#elif defined(EMAC_PHY_RTL8201)
#define esp_eth_phy_new_xxxxx(cfg)  esp_eth_phy_new_rtl8201(cfg)
#elif defined(EMAC_PHY_GENERIC)
#define esp_eth_phy_new_xxxxx(cfg)  esp_eth_phy_new_generic(cfg)
#else
#error "Physiscal layer transceiver not defined"
#endif

/* for source code compatibility of board definitions from ESP-IDF 3.1 */
#define ETH_CLOCK_GPIO0_IN          0
#define ETH_CLOCK_GPIO0_OUT         1
#define ETH_CLOCK_GPIO16_OUT        2
#define ETH_CLOCK_GPIO17_OUT        3

/**
 * There is only one ESP-ETH device. We define it as static device variable
 * to have access to the device inside ESP-ETH interrupt routines which do
 * not provide an argument that could be used as pointer to the ESP-ETH
 * device which triggers the interrupt.
 */
esp_eth_netdev_t _esp_eth_dev;

static esp_err_t IRAM_ATTR _eth_input_callback(esp_eth_handle_t hdl,
                                               uint8_t *buffer, uint32_t len,
                                               void *priv)
{
    DEBUG("%s: buf=%p len=%"PRIu32" priv=%p\n", __func__, buffer, len, priv);

    assert(buffer != NULL);
    assert(len <= ETHERNET_MAX_LEN);

    mutex_lock(&_esp_eth_dev.dev_lock);

    memcpy(_esp_eth_dev.rx_buf, buffer, len);
    /* buffer is allocated by the `emac_esp32_rx_task` upon receipt of a
     * MAC frame and is forwarded to the consumer who responsible for
     * freeing the buffer. */
    free(buffer);
    _esp_eth_dev.rx_len = len;
    _esp_eth_dev.event = SYSTEM_EVENT_ETH_RX_DONE;
    netdev_trigger_event_isr(&_esp_eth_dev.netdev);

    mutex_unlock(&_esp_eth_dev.dev_lock);

    return ESP_OK;
}

/** Event handler for Ethernet events */
static void _esp_eth_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    (void)arg;
    (void)event_data;

    switch (event_id) {
        case ETHERNET_EVENT_START:
            DEBUG("%s: Ethernet started\n", __func__);
            break;
        case ETHERNET_EVENT_STOP:
            DEBUG("%s: Ethernet stopped\n", __func__);
            break;
        case ETHERNET_EVENT_CONNECTED:
            DEBUG("%s Ethernet link up\n", __func__);
            _esp_eth_dev.link_up = true;
            if (SYSTEM_EVENT_MAX) {
                _esp_eth_dev.event = SYSTEM_EVENT_ETH_CONNECTED;
                netdev_trigger_event_isr(&_esp_eth_dev.netdev);
            }
            break;
        case ETHERNET_EVENT_DISCONNECTED:
            DEBUG("%s: Ethernet link down\n", __func__);
            _esp_eth_dev.link_up = false;
            if (SYSTEM_EVENT_MAX) {
                _esp_eth_dev.event = SYSTEM_EVENT_ETH_DISCONNECTED;
                netdev_trigger_event_isr(&_esp_eth_dev.netdev);
            }
            break;
        default:
            DEBUG("%s: event=%"PRIi32"\n", __func__, event_id);
            break;
    }
}

static int _esp_eth_init(netdev_t *netdev)
{
    DEBUG("%s: netdev=%p\n", __func__, netdev);

    esp_eth_netdev_t* dev = container_of(netdev, esp_eth_netdev_t, netdev);

    mutex_lock(&dev->dev_lock);

    /* set PHY configuration */
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = EMAC_PHY_ADDRESS;
    phy_config.reset_gpio_num = EMAC_PHY_POWER_PIN;

    /* set EMAC configuration */
    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();

    mac_config.sw_reset_timeout_ms = 500;
    mac_config.rx_task_prio = 20;

    /* set EMAC configuration */
    eth_esp32_emac_config_t emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();

    emac_config.smi_gpio.mdc_num = EMAC_PHY_SMI_MDC_PIN;
    emac_config.smi_gpio.mdio_num = EMAC_PHY_SMI_MDIO_PIN;

    if (EMAC_PHY_CLOCK_MODE == ETH_CLOCK_GPIO0_IN) {
        emac_config.clock_config.rmii.clock_mode = EMAC_CLK_EXT_IN;
        emac_config.clock_config.rmii.clock_gpio = EMAC_CLK_IN_GPIO;
    }
    else if (EMAC_PHY_CLOCK_MODE == ETH_CLOCK_GPIO0_OUT) {
        emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_config.clock_config.rmii.clock_gpio = EMAC_APPL_CLK_OUT_GPIO;
    }
    else if (EMAC_PHY_CLOCK_MODE == ETH_CLOCK_GPIO16_OUT) {
        emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_config.clock_config.rmii.clock_gpio = EMAC_CLK_OUT_GPIO;
    }
    else if (EMAC_PHY_CLOCK_MODE == ETH_CLOCK_GPIO17_OUT) {
        emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
        emac_config.clock_config.rmii.clock_gpio = EMAC_CLK_OUT_180_GPIO;
    }

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&emac_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_xxxxx(&phy_config);

    /* generate Ethernet driver configuration */
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);

    /* install Ethernet driver */
    if (esp_eth_driver_install(&config, &_esp_eth_dev.eth_driver) != ESP_OK) {
        LOG_TAG_ERROR("esp_eth", "driver installation failed");
        return -ENODEV;
    }

    esp_event_handler_instance_register(ETH_EVENT, ESP_EVENT_ANY_ID,
                                        &_esp_eth_event_handler, NULL, NULL);

    /* start Ethernet driver state machine */
    if (esp_eth_start(_esp_eth_dev.eth_driver) != ESP_OK) {
        LOG_TAG_ERROR("esp_eth", "driver installation failed");
        return -ENODEV;
    }

    esp_eth_update_input_path(_esp_eth_dev.eth_driver,
                              _eth_input_callback, &_esp_eth_dev);

    mutex_unlock(&dev->dev_lock);

    return 0;
}

static int _esp_eth_send(netdev_t *netdev, const iolist_t *iolist)
{
    DEBUG("%s: netdev=%p iolist=%p\n", __func__, netdev, iolist);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (iolist != NULL, -EINVAL);

    esp_eth_netdev_t* dev = container_of(netdev, esp_eth_netdev_t, netdev);

    if (!_esp_eth_dev.link_up) {
        DEBUG("%s: link is down\n", __func__);
        return -ENODEV;
    }

    mutex_lock(&dev->dev_lock);

    dev->tx_len = 0;

    /* load packet data into TX buffer */
    for (const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        if (dev->tx_len + iol->iol_len > ETHERNET_MAX_LEN) {
            mutex_unlock(&dev->dev_lock);
            return -EOVERFLOW;
        }
        if (iol->iol_len) {
            memcpy (dev->tx_buf + dev->tx_len, iol->iol_base, iol->iol_len);
            dev->tx_len += iol->iol_len;
        }
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf ("%s: send %d byte\n", __func__, dev->tx_len);
        if (IS_ACTIVE(ENABLE_DEBUG_HEXDUMP) && IS_USED(MODULE_OD)) {
            od_hex_dump(dev->tx_buf, dev->tx_len, OD_WIDTH_DEFAULT);
        }
    }

    int ret = 0;

    /* send the packet to the peer(s) mac address */
    if (esp_eth_transmit(dev->eth_driver, dev->tx_buf, dev->tx_len) == ESP_OK) {
        netdev->event_callback(netdev, NETDEV_EVENT_TX_COMPLETE);
    }
    else {
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

    esp_eth_netdev_t* dev = container_of(netdev, esp_eth_netdev_t, netdev);

    mutex_lock(&dev->dev_lock);

    int size = dev->rx_len;

    if (!buf) {
        /* get the size of the frame; if len > 0 then also drop the frame */
        if (len > 0) {
            /* drop frame requested */
            dev->rx_len = 0;
        }
        mutex_unlock(&dev->dev_lock);
        return size;
    }

    if (dev->rx_len > len) {
        /* buffer is smaller than the number of received bytes */
        DEBUG("%s: Not enough space in receive buffer for %d bytes\n",
              __func__, dev->rx_len);
        mutex_unlock(&dev->dev_lock);
        return -ENOBUFS;
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf ("%s: received %d byte\n", __func__, dev->rx_len);
        if (IS_ACTIVE(ENABLE_DEBUG_HEXDUMP) && IS_USED(MODULE_OD)) {
            od_hex_dump(dev->rx_buf, dev->rx_len, OD_WIDTH_DEFAULT);
        }
    }

    /* copy received date and reset the receive length */
    memcpy(buf, dev->rx_buf, dev->rx_len);
    dev->rx_len = 0;

    mutex_unlock(&dev->dev_lock);
    return size;
}

static int _esp_eth_get(netdev_t *netdev, netopt_t opt, void *val, size_t max_len)
{
    DEBUG("%s: netdev=%p opt=%s val=%p len=%u\n",
          __func__, netdev, netopt2str(opt), val, max_len);

    CHECK_PARAM_RET (netdev != NULL, -ENODEV);
    CHECK_PARAM_RET (val != NULL, -EINVAL);

    esp_eth_netdev_t* dev = container_of(netdev, esp_eth_netdev_t, netdev);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len >= ETHERNET_ADDR_LEN);
            esp_eth_ioctl(dev->eth_driver, ETH_CMD_G_MAC_ADDR, (uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        case NETOPT_LINK:
            assert(max_len == sizeof(netopt_enable_t));
            *((netopt_enable_t *)val) = (dev->link_up) ? NETOPT_ENABLE
                                                       : NETOPT_DISABLE;
            return sizeof(netopt_enable_t);
        default:
            return netdev_eth_get(netdev, opt, val, max_len);
    }
}

static int _esp_eth_set(netdev_t *netdev, netopt_t opt, const void *val, size_t max_len)
{
    DEBUG("%s: netdev=%p opt=%s val=%p len=%u\n",
          __func__, netdev, netopt2str(opt), val, max_len);

    CHECK_PARAM_RET(netdev != NULL, -ENODEV);
    CHECK_PARAM_RET(val != NULL, -EINVAL);

    esp_eth_netdev_t* dev = container_of(netdev, esp_eth_netdev_t, netdev);

    switch (opt) {
        case NETOPT_ADDRESS:
            assert(max_len == ETHERNET_ADDR_LEN);
            esp_eth_ioctl(dev->eth_driver, ETH_CMD_S_MAC_ADDR, (uint8_t *)val);
            return ETHERNET_ADDR_LEN;
        default:
            return netdev_eth_set(netdev, opt, val, max_len);
    }
}

static void _esp_eth_isr(netdev_t *netdev)
{
    DEBUG("%s: netdev=%p\n", __func__, netdev);

    CHECK_PARAM(netdev != NULL);

    esp_eth_netdev_t* dev = container_of(netdev, esp_eth_netdev_t, netdev);

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
    switch (event->event_id) {
        case SYSTEM_EVENT_ETH_START:
            DEBUG("%s: Ethernet started\n", __func__);
            break;
        case SYSTEM_EVENT_ETH_STOP:
            DEBUG("%s: Ethernet stopped\n", __func__);
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            DEBUG("%s Ethernet link up\n", __func__);
            _esp_eth_dev.link_up = true;
            if (SYSTEM_EVENT_MAX) {
                _esp_eth_dev.event = SYSTEM_EVENT_ETH_CONNECTED;
                netdev_trigger_event_isr(&_esp_eth_dev.netdev);
            }
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            DEBUG("%s: Ethernet link down\n", __func__);
            _esp_eth_dev.link_up = false;
            if (SYSTEM_EVENT_MAX) {
                _esp_eth_dev.event = SYSTEM_EVENT_ETH_DISCONNECTED;
                netdev_trigger_event_isr(&_esp_eth_dev.netdev);
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

void esp_eth_setup(esp_eth_netdev_t* dev)
{
    (void)dev;

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

    netdev_register(&dev->netdev, NETDEV_ESP_ETH, 0);
}

#endif /* MODULE_ESP_ETH */
/**@}*/
