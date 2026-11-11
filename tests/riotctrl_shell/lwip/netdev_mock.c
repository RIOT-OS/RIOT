/*
 * SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Mock for netdevs
 *
 * @author      Martine S. Lenders <mlenders@riot-os.org>
 *
 * @note        IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
 *              YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
 *              See https://github.com/RIOT-OS/Release-Specs/
 */

#include <stdio.h>

#include "lwip_init_devs.h"
#include "net/ethernet.h"
#include "net/loramac.h"
#include "net/netdev_test.h"
#include "net/netif.h"
#include "od.h"
#include "test_utils/expect.h"

typedef struct {
    uint8_t flags;
    netopt_state_t state;
} _netdev_test_state_t;

#define NETDEV_TEST_STATE_FLAG_UP       0x01

#define IEEE802154_MAX_FRAG_SIZE    102

#define ETH_LOCAL_MAC     { \
    0x02, 0x00, 0x00, 0x12, 0x34, 0x56 \
}
#define IEEE802154_LOCAL_EUI64     { \
    0x02, 0x00, 0x00, 0xFF, 0xFE, 0x12, 0x34, 0x57 \
}

static lwip_netif_t _eth_netif, _ieee802154_netif;
static netdev_test_t _eth_dev, _ieee802154_dev;
static _netdev_test_state_t _eth_dev_state, _ieee802154_dev_state;
static uint8_t _eth_local_mac[] = ETH_LOCAL_MAC;

static int _get_eth_device_type(netdev_t *netdev, void *value, size_t max_len)
{
    expect(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = NETDEV_TYPE_ETHERNET;
    return sizeof(uint16_t);
}

static int _get_eth_max_packet_size(netdev_t *netdev, void *value, size_t max_len)
{
    expect(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = ETHERNET_DATA_LEN;
    return sizeof(uint16_t);
}

static int _get_eth_src_len(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len == sizeof(uint16_t));
    *((uint16_t *)value) = sizeof(_eth_local_mac);
    return sizeof(uint16_t);
}

static int _get_eth_addr(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len >= sizeof(_eth_local_mac));
    memcpy(value, _eth_local_mac, sizeof(_eth_local_mac));
    return sizeof(_eth_local_mac);
}

static int _get_eth_link(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_test_t *dev = container_of(netdev_ieee802154, netdev_test_t, netdev);
    _netdev_test_state_t *state = dev->state;

    expect(max_len >= sizeof(netopt_enable_t));
    if (state->flags & NETDEV_TEST_STATE_FLAG_UP) {
        *((netopt_enable_t *)value) = NETOPT_ENABLE;
    }
    else {
        *((netopt_enable_t *)value) = NETOPT_DISABLE;
    }
    return sizeof(netopt_enable_t);
}

void netdev_mock_init_eth(void)
{
    char netif_name[CONFIG_NETIF_NAMELENMAX];

    netdev_test_setup(&_eth_dev, NULL);
    _eth_dev.state = &_eth_dev_state;

    netdev_test_set_get_cb(&_eth_dev, NETOPT_DEVICE_TYPE, _get_eth_device_type);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_MAX_PDU_SIZE, _get_eth_max_packet_size);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_SRC_LEN, _get_eth_src_len);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_ADDRESS, _get_eth_addr);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_LINK, _get_eth_link);

    if (lwip_add_ethernet(&_eth_netif, &_eth_dev.netdev.netdev) == NULL) {
        puts("Could not add Ethernet mock interface\n");
        return;
    }

    if (netif_get_name(&_eth_netif.common_netif, netif_name) <= 0) {
        puts("Unable to get network name");
    }
    else {
        printf("Started Ethernet interface %s.\n", netif_name);
    }
}

static int _get_ieee802154_device_type(netdev_t *netdev, void *value, size_t max_len)
{
    expect(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
    return sizeof(uint16_t);
}

static int _get_ieee802154_max_packet_size(netdev_t *netdev, void *value, size_t max_len)
{
    expect(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = IEEE802154_MAX_FRAG_SIZE;
    return sizeof(uint16_t);
}

static int _get_ieee802154_src_len(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(max_len == sizeof(uint16_t));
    if (netdev_ieee802154->flags & NETDEV_IEEE802154_SRC_MODE_LONG) {
        *((uint16_t *)value) = IEEE802154_LONG_ADDRESS_LEN;
    }
    else {
        *((uint16_t *)value) = IEEE802154_SHORT_ADDRESS_LEN;
    }
    return sizeof(uint16_t);
}

static int _get_ieee802154_addr(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(max_len >= IEEE802154_SHORT_ADDRESS_LEN);
    memcpy(value, netdev_ieee802154->short_addr, sizeof(netdev_ieee802154->short_addr));
    return IEEE802154_SHORT_ADDRESS_LEN;
}

static int _get_ieee802154_addr_long(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(max_len >= IEEE802154_LONG_ADDRESS_LEN);
    memcpy(value, netdev_ieee802154->long_addr, sizeof(netdev_ieee802154->long_addr));
    return IEEE802154_LONG_ADDRESS_LEN;
}

static int _get_ieee802154_pan_id(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(max_len >= sizeof(uint16_t));
    *((uint16_t *)value) = netdev_ieee802154->pan;
    return sizeof(uint16_t);
}

static int _get_ieee802154_state(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_test_t *dev = container_of(netdev_ieee802154, netdev_test_t, netdev);
    _netdev_test_state_t *state = dev->state;

    expect(max_len >= sizeof(netopt_enable_t));
    *((netopt_state_t *)value) = state->state;
    return sizeof(netopt_enable_t);
}

static int _set_ieee802154_src_len(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    uint16_t src_len;

    expect(value_len == sizeof(uint16_t));
    src_len = *((uint16_t *)value);

    if (src_len == IEEE802154_LONG_ADDRESS_LEN) {
        netdev_ieee802154->flags |= NETDEV_IEEE802154_SRC_MODE_LONG;
    }
    else if (src_len == IEEE802154_SHORT_ADDRESS_LEN) {
        netdev_ieee802154->flags &= ~NETDEV_IEEE802154_SRC_MODE_LONG;
    }
    else {
        return -EINVAL;
    }
    return sizeof(uint16_t);
}

void netdev_mock_init_ieee802154(void)
{
    char netif_name[CONFIG_NETIF_NAMELENMAX];

    const uint8_t _addr[] = IEEE802154_LOCAL_EUI64;
    netdev_ieee802154_t *netdev = &_ieee802154_dev.netdev;

    netdev_test_setup(&_ieee802154_dev, NULL);
    _ieee802154_dev.state = &_ieee802154_dev_state;

    memcpy(netdev->long_addr, _addr, sizeof(netdev->long_addr));
    memcpy(netdev->short_addr, &_addr[5], sizeof(netdev->short_addr));
    netdev->txpower = -12;
    _ieee802154_dev_state.state = NETOPT_STATE_IDLE;

    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_DEVICE_TYPE, _get_ieee802154_device_type);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_MAX_PDU_SIZE, _get_ieee802154_max_packet_size);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_SRC_LEN, _get_ieee802154_src_len);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_ADDRESS_LONG, _get_ieee802154_addr_long);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_ADDRESS, _get_ieee802154_addr);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_NID, _get_ieee802154_pan_id);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_STATE, _get_ieee802154_state);

    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_SRC_LEN, _set_ieee802154_src_len);

    if (lwip_add_6lowpan(&_ieee802154_netif, &netdev->netdev) == NULL) {
        puts("Could not add IEEE 802.15.4 mock interface\n");
        return;
    }

    if (netif_get_name(&_ieee802154_netif.common_netif, netif_name) <= 0) {
        puts("Unable to get network name");
    }
    else {
        printf("Started IEEE 802.15.4 interface %s.\n", netif_name);
    }
}

LWIP_INIT_ETH_NETIF(netdev_mock_init_eth);
LWIP_INIT_6LOWPAN_NETIF(netdev_mock_init_ieee802154);
