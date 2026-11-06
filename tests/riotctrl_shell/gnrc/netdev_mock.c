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

#include "net/ethernet.h"
#include "net/gnrc/netif/ethernet.h"
#include "net/gnrc/netif/ieee802154.h"
#include "net/l2filter.h"
#include "net/loramac.h"
#include "net/netdev_test.h"
#include "od.h"
#include "test_utils/expect.h"

typedef struct {
    uint8_t flags;
    netopt_state_t state;
} _netdev_test_state_t;

#define NETDEV_TEST_STATE_FLAG_UP       0x01
#define NETDEV_TEST_STATE_FLAG_OTAA     0x02

#define IEEE802154_MAX_FRAG_SIZE    102

#define ETH_LOCAL_MAC     { \
    0x02, 0x00, 0x00, 0x12, 0x34, 0x56 \
}
#define IEEE802154_LOCAL_EUI64     { \
    0x02, 0x00, 0x00, 0xFF, 0xFE, 0x12, 0x34, 0x57 \
}

static gnrc_netif_t _eth_netif, _ieee802154_netif;
static char _eth_netif_stack[THREAD_STACKSIZE_DEFAULT];
static char _ieee802154_netif_stack[THREAD_STACKSIZE_DEFAULT];
static netdev_test_t _eth_dev, _ieee802154_dev;
static _netdev_test_state_t _eth_dev_state, _ieee802154_dev_state;
static uint8_t _eth_local_mac[] = ETH_LOCAL_MAC;
static uint8_t _ieee802154_encrypt_key[64U];
static uint8_t _lora_app_key[LORAMAC_APPSKEY_LEN];
static uint32_t _ieee802154_freq = -2;
static uint8_t _ieee802154_csma_retries = 5;
static uint8_t _ieee802154_encrypt_key_len = 0;
static uint8_t _ieee802154_retrans = 3;
static uint8_t _lora_app_key_len = 0;
static int8_t _ieee802154_cca_threshold = -2;

static int _send_eth(netdev_t *netdev, const iolist_t *iolist)
{
    int res = 0;

    printf("Sending on interface %d\n", _eth_netif.pid);

    (void)netdev;
    for (const iolist_t *ptr = iolist; ptr != NULL; ptr = ptr->iol_next) {
        od_hex_dump_ext(ptr->iol_base, ptr->iol_len, OD_WIDTH_DEFAULT, res);

        res += ptr->iol_len;
    }
    return res;
}

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

static int _get_eth_l2filter(netdev_t *netdev, void *value, size_t max_len)
{
    assert(max_len >= sizeof(l2filter_t **));
    *((l2filter_t **)value) = netdev->filter;
    return sizeof(l2filter_t **);
}

static int _set_eth_addr(netdev_t *netdev, const void *value, size_t value_len)
{
    (void)netdev;
    expect(value_len <= sizeof(_eth_local_mac));
    memcpy(_eth_local_mac, value, sizeof(_eth_local_mac));
    return sizeof(_eth_local_mac);
}

static int _set_eth_link(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_test_t *dev = container_of(netdev_ieee802154, netdev_test_t, netdev);
    _netdev_test_state_t *state = dev->state;

    expect(value_len <= sizeof(netopt_enable_t));
    if (*((netopt_enable_t *)value) == NETOPT_ENABLE) {
        state->flags |= NETDEV_TEST_STATE_FLAG_UP;
    }
    else {
        state->flags &= ~NETDEV_TEST_STATE_FLAG_UP;
    }
    return sizeof(netopt_enable_t);
}

static int _set_eth_l2filter_add(netdev_t *netdev, const void *value, size_t value_len)
{
    return l2filter_add(netdev->filter, value, value_len);
}

static int _set_eth_l2filter_rm(netdev_t *netdev, const void *value, size_t value_len)
{
    return l2filter_rm(netdev->filter, value, value_len);
}

void netdev_mock_init_eth(void)
{
    netdev_test_setup(&_eth_dev, NULL);
    _eth_dev.state = &_eth_dev_state;

    netdev_test_set_send_cb(&_eth_dev, _send_eth);

    netdev_test_set_get_cb(&_eth_dev, NETOPT_DEVICE_TYPE, _get_eth_device_type);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_MAX_PDU_SIZE, _get_eth_max_packet_size);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_SRC_LEN, _get_eth_src_len);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_ADDRESS, _get_eth_addr);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_LINK, _get_eth_link);
    netdev_test_set_get_cb(&_eth_dev, NETOPT_L2FILTER, _get_eth_l2filter);

    netdev_test_set_set_cb(&_eth_dev, NETOPT_ADDRESS, _set_eth_addr);
    netdev_test_set_set_cb(&_eth_dev, NETOPT_LINK, _set_eth_link);
    netdev_test_set_set_cb(&_eth_dev, NETOPT_L2FILTER, _set_eth_l2filter_add);
    netdev_test_set_set_cb(&_eth_dev, NETOPT_L2FILTER_RM, _set_eth_l2filter_rm);

    if (gnrc_netif_ethernet_create(&_eth_netif, _eth_netif_stack, THREAD_STACKSIZE_DEFAULT,
                                   GNRC_NETIF_PRIO, "dummy_eth", &_eth_dev.netdev.netdev) == 0) {
        printf("Started Ethernet interface on PID %d.\n", _eth_netif.pid);
    }
}

static int _send_ieee802154(netdev_t *netdev, const iolist_t *iolist)
{
    int res = 0;

    printf("Sending on interface %d\n", _ieee802154_netif.pid);

    (void)netdev;
    for (const iolist_t *ptr = iolist; ptr != NULL; ptr = ptr->iol_next) {
        od_hex_dump_ext(ptr->iol_base, ptr->iol_len, OD_WIDTH_DEFAULT, res);

        res += ptr->iol_len;
    }
    return res;
}

static int _get_ieee802154_device_type(netdev_t *netdev, void *value, size_t max_len)
{
    expect(max_len == sizeof(uint16_t));
    (void)netdev;

    *((uint16_t *)value) = NETDEV_TYPE_IEEE802154;
    return sizeof(uint16_t);
}

static int _get_ieee802154_proto(netdev_t *netdev, void *value, size_t max_len)
{
    expect(max_len == sizeof(gnrc_nettype_t));
    (void)netdev;

    *((gnrc_nettype_t *)value) = GNRC_NETTYPE_SIXLOWPAN;
    return sizeof(gnrc_nettype_t);
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

static int _get_ieee802154_cca_threshold(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len >= sizeof(int8_t));
    *((int8_t *)value) = _ieee802154_cca_threshold;
    return sizeof(int8_t);
}

static int _get_ieee802154_freq(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len >= sizeof(uint32_t));
    *((uint32_t *)value) = _ieee802154_freq;
    return sizeof(uint32_t);
}

static int _get_ieee802154_chan(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(max_len >= sizeof(uint16_t));
    *((uint16_t *)value) = netdev_ieee802154->chan;
    return sizeof(uint16_t);
}

static int _get_ieee802154_csma_retries(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len >= sizeof(uint8_t));
    *((uint8_t *)value) = _ieee802154_csma_retries;
    return sizeof(uint8_t);
}

static int _get_ieee802154_encrypt_key(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len >= sizeof(_ieee802154_encrypt_key));
    memcpy(value, _ieee802154_encrypt_key, _ieee802154_encrypt_key_len);
    return _ieee802154_encrypt_key_len;
}

static int _get_ieee802154_pan_id(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(max_len >= sizeof(uint16_t));
    *((uint16_t *)value) = netdev_ieee802154->pan;
    return sizeof(uint16_t);
}

static int _get_ieee802154_tx_power(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(max_len >= sizeof(int16_t));
    *((int16_t *)value) = netdev_ieee802154->txpower;
    return sizeof(int16_t);
}

static int _get_ieee802154_retrans(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len >= sizeof(uint8_t));
    *((uint8_t *)value) = _ieee802154_retrans;
    return sizeof(uint8_t);
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

static int _get_lora_app_key(netdev_t *netdev, void *value, size_t max_len)
{
    (void)netdev;
    expect(max_len >= sizeof(_lora_app_key));
    memcpy(value, _lora_app_key, _lora_app_key_len);
    return _lora_app_key_len;
}

static int _get_lora_otaa(netdev_t *netdev, void *value, size_t max_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_test_t *dev = container_of(netdev_ieee802154, netdev_test_t, netdev);
    _netdev_test_state_t *state = dev->state;

    expect(max_len >= sizeof(netopt_enable_t));
    if (state->flags & NETDEV_TEST_STATE_FLAG_OTAA) {
        *((netopt_enable_t *)value) = NETOPT_ENABLE;
    }
    else {
        *((netopt_enable_t *)value) = NETOPT_DISABLE;
    }
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

static int _set_ieee802154_addr(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(value_len == IEEE802154_SHORT_ADDRESS_LEN);
    memcpy(netdev_ieee802154->short_addr, value, sizeof(netdev_ieee802154->short_addr));
    return IEEE802154_SHORT_ADDRESS_LEN;
}

static int _set_ieee802154_addr_long(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(value_len == IEEE802154_LONG_ADDRESS_LEN);
    memcpy(netdev_ieee802154->long_addr, value, sizeof(netdev_ieee802154->long_addr));
    return IEEE802154_LONG_ADDRESS_LEN;
}

static int _set_ieee802154_cca_threshold(netdev_t *netdev, const void *value, size_t value_len)
{
    (void)netdev;
    expect(value_len <= sizeof(int8_t));
    _ieee802154_cca_threshold = *((int8_t *)value);
    return sizeof(int8_t);
}

static int _set_ieee802154_freq(netdev_t *netdev, const void *value, size_t value_len)
{
    (void)netdev;
    expect(value_len <= sizeof(uint32_t));
    _ieee802154_freq = *((uint32_t *)value);
    return sizeof(uint32_t);
}

static int _set_ieee802154_chan(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    uint16_t tmp;

    expect(value_len <= sizeof(uint16_t));
    tmp = *((uint16_t *)value);
    if (tmp > 255) {
        return -EINVAL;
    }
    netdev_ieee802154->chan = (uint8_t)tmp;
    return sizeof(uint16_t);
}

static int _set_ieee802154_csma_retries(netdev_t *netdev, const void *value, size_t value_len)
{
    (void)netdev;
    expect(value_len <= sizeof(uint8_t));
    _ieee802154_csma_retries = *((uint8_t *)value);
    return sizeof(uint8_t);
}

static int _set_ieee802154_encrypt_key(netdev_t *netdev, const void *value, size_t value_len)
{
    (void)netdev;
    expect(value_len <= sizeof(_ieee802154_encrypt_key));
    memcpy(_ieee802154_encrypt_key, value, value_len);
    _ieee802154_encrypt_key_len = value_len;
    return value_len;
}

static int _set_ieee802154_pan_id(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(value_len <= sizeof(uint16_t));
    netdev_ieee802154->pan = *((uint16_t *)value);
    return sizeof(uint16_t);
}

static int _set_ieee802154_tx_power(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);

    expect(value_len <= sizeof(int16_t));
    netdev_ieee802154->txpower = *((int16_t *)value);
    return sizeof(int16_t);
}

static int _set_ieee802154_retrans(netdev_t *netdev, const void *value, size_t value_len)
{
    (void)netdev;
    expect(value_len <= sizeof(uint8_t));
    _ieee802154_retrans = *((uint8_t *)value);
    return sizeof(uint8_t);
}

static int _set_ieee802154_state(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_test_t *dev = container_of(netdev_ieee802154, netdev_test_t, netdev);
    _netdev_test_state_t *state = dev->state;

    expect(value_len <= sizeof(netopt_state_t));
    state->state = *((netopt_state_t *)value);
    return sizeof(netopt_state_t);
}

static int _set_lora_app_key(netdev_t *netdev, const void *value, size_t value_len)
{
    (void)netdev;
    expect(value_len <= sizeof(_lora_app_key));
    memcpy(_lora_app_key, value, value_len);
    _lora_app_key_len = value_len;
    return value_len;
}

static int _set_lora_otaa(netdev_t *netdev, const void *value, size_t value_len)
{
    netdev_ieee802154_t *netdev_ieee802154 = container_of(netdev, netdev_ieee802154_t, netdev);
    netdev_test_t *dev = container_of(netdev_ieee802154, netdev_test_t, netdev);
    _netdev_test_state_t *state = dev->state;

    expect(value_len <= sizeof(netopt_enable_t));
    if (*((netopt_enable_t *)value) == NETOPT_ENABLE) {
        state->flags |= NETDEV_TEST_STATE_FLAG_OTAA;
    }
    else {
        state->flags &= ~NETDEV_TEST_STATE_FLAG_OTAA;
    }
    return sizeof(netopt_enable_t);
}

void netdev_mock_init_ieee802154(void)
{
    const uint8_t _addr[] = IEEE802154_LOCAL_EUI64;
    netdev_ieee802154_t *netdev = &_ieee802154_dev.netdev;

    netdev_test_setup(&_ieee802154_dev, NULL);
    _ieee802154_dev.state = &_ieee802154_dev_state;

    memcpy(netdev->long_addr, _addr, sizeof(netdev->long_addr));
    memcpy(netdev->short_addr, &_addr[5], sizeof(netdev->short_addr));
    netdev->txpower = -12;
    _ieee802154_dev_state.state = NETOPT_STATE_IDLE;

    netdev_test_set_send_cb(&_ieee802154_dev, _send_ieee802154);

    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_DEVICE_TYPE, _get_ieee802154_device_type);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_PROTO, _get_ieee802154_proto);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_MAX_PDU_SIZE, _get_ieee802154_max_packet_size);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_SRC_LEN, _get_ieee802154_src_len);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_ADDRESS_LONG, _get_ieee802154_addr_long);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_ADDRESS, _get_ieee802154_addr);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_CCA_THRESHOLD, _get_ieee802154_cca_threshold);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_CHANNEL_FREQUENCY, _get_ieee802154_freq);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_CHANNEL, _get_ieee802154_chan);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_CSMA_RETRIES, _get_ieee802154_csma_retries);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_ENCRYPTION_KEY, _get_ieee802154_encrypt_key);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_NID, _get_ieee802154_pan_id);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_TX_POWER, _get_ieee802154_tx_power);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_RETRANS, _get_ieee802154_retrans);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_STATE, _get_ieee802154_state);

    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_LORAWAN_APPKEY, _get_lora_app_key);
    netdev_test_set_get_cb(&_ieee802154_dev, NETOPT_OTAA, _get_lora_otaa);

    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_SRC_LEN, _set_ieee802154_src_len);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_ADDRESS_LONG, _set_ieee802154_addr_long);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_ADDRESS, _set_ieee802154_addr);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_CCA_THRESHOLD, _set_ieee802154_cca_threshold);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_CHANNEL_FREQUENCY, _set_ieee802154_freq);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_CHANNEL, _set_ieee802154_chan);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_CSMA_RETRIES, _set_ieee802154_csma_retries);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_ENCRYPTION_KEY, _set_ieee802154_encrypt_key);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_NID, _set_ieee802154_pan_id);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_TX_POWER, _set_ieee802154_tx_power);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_RETRANS, _set_ieee802154_retrans);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_STATE, _set_ieee802154_state);

    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_LORAWAN_APPKEY, _set_lora_app_key);
    netdev_test_set_set_cb(&_ieee802154_dev, NETOPT_OTAA, _set_lora_otaa);

    if (gnrc_netif_ieee802154_create(&_ieee802154_netif, _ieee802154_netif_stack,
                                     THREAD_STACKSIZE_DEFAULT, GNRC_NETIF_PRIO, "dummy_ieee802154",
                                     &_ieee802154_dev.netdev.netdev) == 0) {
        printf("Started IEEE 802.15.4 interface on PID %d.\n", _ieee802154_netif.pid);
    }
}
