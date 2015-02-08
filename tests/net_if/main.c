/*
 * Copyright (C) 2013 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "net_if.h"
#include "net_help.h"
#include "transceiver.h"

int initialize_tests(void);
int test_net_if_initialization(int iface);
int test_net_if_get_add_l3p_types(int iface);
int test_net_if_add_address(int iface, net_if_addr_t *addr1,
                            net_if_addr_t *addr2);
int test_net_if_del_address(int iface, net_if_addr_t *addr1,
                            net_if_addr_t *addr2);
int test_net_if_get_set_hardware_address(int iface, uint16_t addr);
int test_net_if_get_set_pan_id(int iface);
int test_net_if_get_set_eui64(int iface, net_if_eui64_t *eui64,
                              uint16_t addr);

int main(void)
{
    int iface;
    char *addr1_data = "abcdefgh", *addr2_data = "12345678";
    net_if_addr_t addr1 = {
        .addr_next = NULL,
        .addr_prev = NULL,
        .addr_protocol = NET_IF_L3P_IPV6_MULTICAST,
        .addr_data = (void *)addr1_data,
        .addr_len = (strlen(addr1_data) + 1) * 8
    };
    net_if_addr_t addr2 = {
        .addr_next = NULL,
        .addr_prev = NULL,
        .addr_protocol = NET_IF_L3P_IPV6_PREFIX,
        .addr_data = (void *)addr2_data,
        .addr_len = (strlen(addr2_data) + 1) * 8
    };
    uint16_t own = 1, target = 2;
    net_if_eui64_t eui64;

    iface = initialize_tests();

    if (!test_net_if_initialization(iface)) {
        printf("FAILED: test_net_if_initialization()\n");
        return -1;
    }

    if (!test_net_if_get_add_l3p_types(iface)) {
        printf("FAILED: test_net_if_get_add_l3p_types()\n");
        return -1;
    }

    if (!test_net_if_add_address(iface, &addr1, &addr2)) {
        printf("FAILED: test_net_if_add_address()\n");
        return -1;
    }

    if (!test_net_if_del_address(iface, &addr1, &addr2)) {
        printf("FAILED: test_net_if_del_address()\n");
        return -1;
    }

    if (!test_net_if_get_set_hardware_address(iface, own)) {
        printf("FAILED: test_net_if_get_set_hardware_address()\n");
        return -1;
    }

    if (!test_net_if_get_set_pan_id(iface)) {
        printf("FAILED: test_net_if_get_set_pan_id()\n");
        return -1;
    }

    if (!test_net_if_get_set_eui64(iface, &eui64, own)) {
        printf("FAILED: test_net_if_get_set_eui64()\n");
        return -1;
    }

    int count = net_if_send_packet(iface, target, "Test", 4);

    printf("Count was %i after net_if_send_packet()\n", count);

    printf("All test ran successfully.\n");

    return 0;
}

int initialize_tests(void)
{
    int iface;

#ifndef MODULE_AUTO_INIT
    transceiver_init(TRANSCEIVER_DEFAULT);
    transceiver_start();
    net_if_init();
    iface = net_if_init_interface(0, TRANSCEIVER_DEFAULT);
    return iface;
#else
    iface = -1;

    while ((iface = net_if_iter_interfaces(iface)) >= 0) {
        return iface;
    }

    return iface;
#endif
}

int test_net_if_initialization(int iface)
{
    net_if_addr_t *addr_ptr = NULL;

    if (net_if_get_l3p_types(iface)) {
        printf("FAILED: No L3 type expected on interface %d.\n", iface);
        return 0;
    }

    if (net_if_iter_addresses(iface + 1, &addr_ptr)) {
        printf("FAILED: Expected error on interface '%d'\n", iface + 1);
        return 0;
    }

    if (net_if_iter_addresses(iface, &addr_ptr)) {
        printf("FAILED: Expected error on interface '%d'\n", iface);
        return 0;
    }

    return 1;
}

int test_net_if_get_add_l3p_types(int iface)
{
    if (net_if_add_l3p_types(iface + 1, NET_IF_L3P_IPV6_UNICAST)) {
        printf("FAILED: expected net_if_add_l3p_types to fail for iface %d.\n",
               iface + 1);
        return 0;
    }

    if (!net_if_add_l3p_types(iface, 0)) {
        printf("FAILED: expected net_if_add_l3p_types to succeed for iface %d and no type\n",
               iface);
        return 0;
    }


    if (!net_if_add_l3p_types(iface, NET_IF_L3P_IPV6_UNICAST)) {
        printf("FAILED: expected net_if_add_l3p_types to succeed for iface %d and no type\n",
               iface);
        return 0;
    }

    if (!(net_if_get_l3p_types(iface) & NET_IF_L3P_IPV6_UNICAST)) {
        printf("FAILED: L3 type IPv6 unicast expected on interface %d.\n", iface);
        return 0;
    }

    if (net_if_get_l3p_types(iface) & ~NET_IF_L3P_IPV6_UNICAST) {
        printf("FAILED: L3 type other than IPv6 unicast not expected on interface %d.\n",
               iface);
        return 0;
    }

    if (net_if_del_l3p_types(iface + 1, NET_IF_L3P_IPV6_UNICAST)) {
        printf("FAILED: expected net_if_del_l3p_types to fail for iface %d.\n",
               iface + 1);
        return 0;
    }

    if (!net_if_del_l3p_types(iface, 0)) {
        printf("FAILED: expected net_if_del_l3p_types to succeed for iface %d and no type\n",
               iface);
        return 0;
    }

    if (!net_if_del_l3p_types(iface, NET_IF_L3P_IPV6_UNICAST)) {
        printf("FAILED: expected net_if_del_l3p_types to succeed for iface %d and no type\n",
               iface);
        return 0;
    }

    if (net_if_get_l3p_types(iface)) {
        printf("FAILED: No L3 type expected on interface %d.\n", iface);
        return 0;
    }

    return 1;
}

int test_net_if_add_address(int iface, net_if_addr_t *addr1,
                            net_if_addr_t *addr2)
{
    int count = 0;
    net_if_addr_t *addr_ptr = NULL;

    if (net_if_add_address(iface + 1, addr1)) {
        printf("FAILED: expected net_if_add_address(%d, %p) to fail.\n",
               iface + 1, (void *)addr1);
        return 0;
    }

    if (net_if_add_address(iface, NULL)) {
        printf("FAILED: expected net_if_add_address(%d, NULL) to fail.\n",
               iface);
        return 0;
    }

    if (!net_if_add_address(iface, addr1)) {
        printf("FAILED: Address addition failed\n");
        return 0;
    }

    if (!(net_if_get_l3p_types(iface) & NET_IF_L3P_IPV6_MULTICAST)) {
        printf("FAILED: L3 type IPv6 multicast expected on interface %d.\n", iface);
        return 0;
    }

    if (net_if_get_l3p_types(iface) & ~NET_IF_L3P_IPV6_MULTICAST) {
        printf("FAILED: L3 type other than IPv6 multicast not expected on interface %d.\n",
               iface);
        return 0;
    }

    if (!net_if_add_address(iface, addr2)) {
        printf("FAILED: Address addition failed\n");
        return 0;
    }

    if (!(net_if_get_l3p_types(iface) & NET_IF_L3P_IPV6_MULTICAST)) {
        printf("FAILED: L3 type IPv6 multcast expected on interface %d.\n", iface);
        return 0;
    }

    if (!(net_if_get_l3p_types(iface) & NET_IF_L3P_IPV6_PREFIX)) {
        printf("FAILED: L3 type IPv6 prefix expected on interface %d.\n", iface);
        return 0;
    }

    if (net_if_get_l3p_types(iface) & ~(NET_IF_L3P_IPV6_MULTICAST | NET_IF_L3P_IPV6_PREFIX)) {
        printf("FAILED: L3 type other than IPv6 multicast and IPv6 prefix not expected on interface %d.\n",
               iface);
        return 0;
    }

    while (net_if_iter_addresses(iface, &addr_ptr)) {
        if (addr_ptr == addr1  || addr_ptr == addr2) {
            count++;
        }
    }

    if (count != 2) {
        printf("FAILED: expected 2 addresses in iface's address list once respectively\n");
        printf("        missing '%d'\n", 2 - count);
        return 0;
    }

    return 1;
}

int test_net_if_del_address(int iface, net_if_addr_t *addr1,
                            net_if_addr_t *addr2)
{
    int count = 0;
    net_if_addr_t *addr_ptr = NULL;

    if (net_if_del_address(iface + 1, addr1)) {
        printf("FAILED: expected net_if_del_address(%d, %p) to fail.\n",
               iface + 1, (void *)addr1);
        return 0;
    }

    if (net_if_del_address(iface, NULL)) {
        printf("FAILED: expected net_if_del_address(%d, NULL) to fail.\n",
               iface);
        return 0;
    }

    if (!net_if_del_address(iface, addr1)) {
        printf("FAILED: Address deletion failed\n");
        return 0;
    }

    while (net_if_iter_addresses(iface, &addr_ptr)) {
        if (addr_ptr == addr1 || addr_ptr == addr2) {
            count++;
        }
    }

    if (count != 1) {
        printf("FAILED: expected 1 address in iface's address list\n");
        printf("        missing '%d'\n", 1 - count);
        return 0;
    }

    return 1;
}

int test_net_if_get_set_hardware_address(int iface, uint16_t addr)
{
    uint16_t tmp;

    if (net_if_set_hardware_address(iface + 1, addr)) {
        printf("FAILED: expected net_if_set_hardware_address(%d, %d) to fail.\n",
               iface + 1, addr);
        return 0;
    }

    if (net_if_set_hardware_address(iface, 0)) {
        printf("FAILED: expected net_if_set_hardware_address(%d, 0) to fail.\n",
               iface);
        return 0;
    }

    tmp = net_if_set_hardware_address(iface, addr);

    if (addr != tmp) {
        printf("FAILED: Expected '%d' as result of net_if_set_hardware_addr() "
               "(was '%d')\n", addr, tmp);
        return 0;
    }

    tmp = net_if_get_hardware_address(iface);

    if (addr != tmp) {
        printf("FAILED: Expected '%d' as result of net_if_get_hardware_addr() "
               "(was '%d')\n", addr, tmp);
        return 0;
    }

    return 1;
}

int test_net_if_get_set_pan_id(int iface)
{
    uint16_t pan_id = 0xabcd;

    if (net_if_get_pan_id(iface + 1) >= 0) {
        printf("FAILED: net_if_get_pan_id(%d) not failed\n", iface);
        return 0;
    }

    if (net_if_set_pan_id(iface, pan_id) < 0) {
        printf("FAILED: net_if_set_pan_id(%d, 0x%04x) failed\n", iface, pan_id);
        return 0;
    }

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
    int32_t res = net_if_get_pan_id(iface);
    if (res < 0) {
        printf("FAILED: net_if_get_pan_id(%d) failed\n", iface);
        return 0;
    }

    pan_id = (uint16_t) res;
#else
    pan_id = 0;
#endif

    return 1;
}

int test_net_if_get_set_eui64(int iface, net_if_eui64_t *eui64,
                              uint16_t addr)
{
    if (net_if_get_eui64(NULL, iface, 1)) {
        printf("FAILED: expected net_if_get_eui64(NULL, %d, 1) to fail\n",
               iface);
        return 0;
    }

    if (!net_if_get_eui64(eui64, iface, 1)) {
        printf("FAILED: Error getting EUI-64 on interface %d\n", iface);
        return 0;
    }

    if (eui64->uint16[0] != 0 || eui64->uint8[2] != 0 ||
        eui64->uint8[3] != 0xff || eui64->uint8[4] != 0xfe ||
        eui64->uint8[5] != 0 || (uint16_t)eui64->uint16[3] != HTONS(addr)) {
        printf("FAILED: Expected last 16 bit of EUI-64 to be 0x%04x (is 0x%04x)\n",
               addr, NTOHS(eui64->uint16[3]));
        return 0;
    }

    eui64->uint64 = 0;

    if (net_if_set_eui64(iface, NULL)) {
        printf("FAILED: expected error on net_if_set_eui64(%d, NULL)\n", iface);
        return 0;
    }

    if (net_if_set_eui64(iface, eui64)) {
        printf("FAILED: expected error trying to set EUI-64 to broadcast\n");
        return 0;
    }

#if MODULE_AT86RF231 || MODULE_CC2420 || MODULE_MC1322X
    eui64->uint8[0] = 0x11;
    eui64->uint8[1] = 0x22;
    eui64->uint8[2] = 0x33;
    eui64->uint8[3] = 0x44;
    eui64->uint8[4] = 0x55;
    eui64->uint8[5] = 0x66;
    eui64->uint8[6] = 0x77;
    eui64->uint8[7] = 0x88;

    if (!net_if_set_eui64(iface, eui64)) {
        printf("FAILED: Error setting EUI-64 on interface %d\n", iface);
        return 0;
    }

    eui64->uint64 = 0;

    if (!net_if_get_eui64(eui64, iface, 0)) {
        printf("FAILED: Error getting EUI-64 on interface %d\n", iface);
        return 0;
    }

    /* transceivers that do not support EUI-64 addresses convert automatically
     * so we have to test both cases */
    if (eui64->uint8[0] != 0x11 || eui64->uint8[1] != 0x22 ||
        eui64->uint8[2] != 0x33 || eui64->uint8[3] != 0x44 ||
        eui64->uint8[4] != 0x55 || eui64->uint8[5] != 0x66 ||
        eui64->uint8[6] != 0x77 || eui64->uint8[7] != 0x88) {
        printf("FAILED: EUI-64 to be 11-22-33-44-55-66-77-88 but is "
               "%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
               eui64->uint8[0], eui64->uint8[1], eui64->uint8[2],
               eui64->uint8[3], eui64->uint8[4], eui64->uint8[5],
               eui64->uint8[6], eui64->uint8[7]);
        return 0;
    }

#endif

    return 1;
}
