/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Tests for general network device interface
 *
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#ifdef MODULE_NATIVENET
#include "nativenet.h"
#endif

#include "net_help.h"
#include "netdev/base.h"
#include "netdev/default.h"

#ifdef NETDEV_DEFAULT
#define SHELL_BUFSIZE   (UART0_BUFSIZE)

static size_t dev_address_len = 0;
static netdev_t *dev = NULL;

/***********************************
 * test channel                    *
 ***********************************/
static int init_channel(void)
{
    uint8_t channel = NETDEV_TEST_CHANNEL;
    int res = dev->driver->set_option(dev, NETDEV_OPT_CHANNEL,
                                      &channel, sizeof(uint8_t));

    switch (res) {
        case 0:
            printf("Channel was set to %" PRIu8 "\n", channel);
            return 1;

        case -EINVAL:
            printf("Channel %" PRIu8 " not supported\n", channel);
            break;

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support setting the channel");
            return 2;

        case -EOVERFLOW:
            puts("1 byte overflows o_o?!!?");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

static int check_channel(void)
{
    uint8_t channel[sizeof(uint64_t)];
    size_t channel_len = sizeof(uint64_t);
    int res = dev->driver->get_option(dev, NETDEV_OPT_CHANNEL,
                                      &channel, &channel_len);

    switch (res) {
        case 0:
            switch (channel_len) {
                case 1:
                    printf("Got channel of value %" PRIu8 " of length %u\n",
                           channel[0], channel_len);
                    return (channel[0] == NETDEV_TEST_CHANNEL) ? 1 : 0;

                case 2:
                    do {
                        uint16_t *c = (uint16_t *)channel;
                        printf("Got channel of value %" PRIu16 " of length %u\n",
                               *c, channel_len);
                        return (*c == NETDEV_TEST_CHANNEL) ? 1 : 0;
                    } while (0);

                    break;

                case 4:
                    do {
                        uint32_t *c = (uint32_t *)channel;
                        printf("Got channel of value %" PRIu32 " of length %u\n",
                               *c, channel_len);
                        return (*c == NETDEV_TEST_CHANNEL) ? 1 : 0;
                    } while (0);

                    break;

                case 8:
                    do {
                        uint64_t *c = (uint64_t *)channel;
                        printf("Got channel of value %" PRIu64 " of length %u\n",
                               *c, channel_len);
                        return (*c == NETDEV_TEST_CHANNEL) ? 1 : 0;
                    } while (0);

                    break;

                default:
                    printf("Unexpected channel type of length %u:", channel_len);

                    for (size_t i = 0; i < channel_len; i++) {
                        printf(" %02x", channel[i]);
                    }

                    printf("\n");
                    return 0;
            }

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support getting of channel");
            return 2;

        case -EOVERFLOW:
            puts("Channel type bigger then 64 bit.");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

/***********************************
 * test address                    *
 ***********************************/
static int init_address(void)
{
    uint32_t address = NETDEV_TEST_ADDRESS;
    int res = dev->driver->set_option(dev, NETDEV_OPT_ADDRESS,
                                      &address, sizeof(uint32_t));

    switch (res) {
        case 0:
            printf("Address was set to %" PRIu32 "\n", address);
            return 1;

        case -EINVAL:
            printf("Address %" PRIu32 " not supported\n", address);
            break;

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support setting the address");
            return 2;

        case -EOVERFLOW:
            puts("1 byte overflows o_o?!!?");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

static int check_address(void)
{
    uint8_t address[sizeof(uint64_t)];
    size_t address_len = sizeof(uint64_t);
    int res = dev->driver->get_option(dev, NETDEV_OPT_ADDRESS,
                                      &address, &address_len);

    switch (res) {
        case 0:
            dev_address_len = address_len;

            switch (address_len) {
                case 1:
                    printf("Got address of value %" PRIu8 " of length %u\n",
                           address[0], address_len);
                    return (address[0] == NETDEV_TEST_ADDRESS) ? 1 : 0;

                case 2:
                    do {
                        uint16_t *a = (uint16_t *)address;
                        printf("Got address of value %" PRIu16 " of length %u\n",
                               *a, address_len);
                        return (*a == NETDEV_TEST_ADDRESS) ? 1 : 0;
                    } while (0);

                    break;

                case 4:
                    do {
                        uint32_t *a = (uint32_t *)address;
                        printf("Got address of value %" PRIu32 " of length %u\n",
                               *a, address_len);
                        return (*a == NETDEV_TEST_ADDRESS) ? 1 : 0;
                    } while (0);

                    break;

                case 6:
                    printf("Got address of length %u of value: ", address_len);

                    for (size_t i = 0; i < address_len; i++) {
                        printf(" %02x", address[i]);
                    }

                    printf("\n");
                    return 1;

                case 8:
                    do {
                        uint64_t *a = (uint64_t *)address;
                        printf("Got address of value %" PRIu64 " of length %u\n",
                               *a, address_len);
                        return (*a == NETDEV_TEST_ADDRESS) ? 1 : 0;
                    } while (0);

                    break;

                default:
                    printf("Unexpected address type of length %u:", address_len);

                    for (size_t i = 0; i < address_len; i++) {
                        printf(" %02x", address[i]);
                    }

                    printf("\n");
                    return 0;
            }

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support getting the address");
            return 2;

        case -EOVERFLOW:
            puts("Address type bigger then 64 bit.");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

/***********************************
 * test long address               *
 ***********************************/
static int init_long_address(void)
{
    uint8_t address = NETDEV_TEST_ADDRESS;
    int res = dev->driver->set_option(dev, NETDEV_OPT_ADDRESS_LONG,
                                      &address, sizeof(uint8_t));

    switch (res) {
        case 0:
            printf("Long address was set to %" PRIu8 "\n", address);
            return 1;

        case -EINVAL:
            printf("Long address %" PRIu8 " not supported\n", address);
            break;

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support setting the long address");
            return 2;

        case -EOVERFLOW:
            puts("1 byte overflows o_o?!!?");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

static int check_long_address(void)
{
    uint8_t address[sizeof(uint64_t)];
    size_t address_len = sizeof(uint64_t);
    int res = dev->driver->get_option(dev, NETDEV_OPT_ADDRESS_LONG,
                                      &address, &address_len);

    switch (res) {
        case 0:
            switch (address_len) {
                case 1:
                    printf("Got long address of value %" PRIu8 " of length %u\n",
                           address[0], address_len);
                    return (address[0] == NETDEV_TEST_ADDRESS) ? 1 : 0;

                case 2:
                    do {
                        uint16_t *a = (uint16_t *)address;
                        printf("Got long address of value %" PRIu16 " of length %u\n",
                               *a, address_len);
                        return (*a == NETDEV_TEST_ADDRESS) ? 1 : 0;
                    } while (0);

                    break;

                case 4:
                    do {
                        uint32_t *a = (uint32_t *)address;
                        printf("Got long address of value %" PRIu32 " of length %u\n",
                               *a, address_len);
                        return (*a == NETDEV_TEST_ADDRESS) ? 1 : 0;
                    } while (0);

                    break;

                case 6:
                    printf("Got long address of length %u of value: ", address_len);

                    for (size_t i = 0; i < address_len; i++) {
                        printf(" %02x", address[i]);
                    }

                    printf("\n");
                    return 1;

                case 8:
                    do {
                        uint64_t *a = (uint64_t *)address;
                        printf("Got long address of value 0x%" PRIu64 " of length %u\n",
                               *a, address_len);
                        return (*a == NETDEV_TEST_ADDRESS) ? 1 : 0;
                    } while (0);

                default:
                    printf("Unexpected long address type of length %u:", address_len);

                    for (size_t i = 0; i < address_len; i++) {
                        printf(" %02x", address[i]);
                    }

                    printf("\n");
                    return 0;
            }

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support getting the long address");
            return 2;

        case -EOVERFLOW:
            puts("Long address type bigger then 64 bit.");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

/***********************************
 * test network ID                 *
 ***********************************/
static int init_nid(void)
{
    uint8_t nid = NETDEV_TEST_NID;
    int res = dev->driver->set_option(dev, NETDEV_OPT_NID,
                                      &nid, sizeof(uint8_t));

    switch (res) {
        case 0:
            printf("Network ID was set to %" PRIu8 "\n", nid);
            return 1;

        case -EINVAL:
            printf("Network ID %" PRIu8 " not supported\n", nid);
            break;

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support setting the network ID");
            return 2;

        case -EOVERFLOW:
            puts("1 byte overflows o_o?!!?");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

static int check_nid(void)
{
    uint8_t nid[sizeof(uint64_t)];
    size_t nid_len = sizeof(uint64_t);
    int res = dev->driver->get_option(dev, NETDEV_OPT_NID,
                                      &nid, &nid_len);

    switch (res) {
        case 0:
            switch (nid_len) {
                case 1:
                    printf("Got network ID of value %" PRIu8 " of length %u\n",
                           nid[0], nid_len);
                    return (nid[0] == NETDEV_TEST_NID) ? 1 : 0;

                    break;

                case 2:
                    do {
                        uint16_t *n = (uint16_t *)nid;
                        printf("Got network ID of value %" PRIu16 " of length %u\n",
                               *n, nid_len);
                        return (*n == NETDEV_TEST_NID) ? 1 : 0;
                    } while (0);

                    break;

                case 4:
                    do {
                        uint32_t *n = (uint32_t *)nid;
                        printf("Got network ID of value %" PRIu32 " of length %u\n",
                               *n, nid_len);
                        return (*n == NETDEV_TEST_NID) ? 1 : 0;
                    } while (0);

                    break;

                case 8:
                    do {
                        uint64_t *n = (uint64_t *)nid;
                        printf("Got network ID of value %" PRIu64 " of length %u\n",
                               *n, nid_len);
                        return (*n == NETDEV_TEST_NID) ? 1 : 0;
                    } while (0);

                    break;

                default:
                    printf("Unexpected network ID type of length %u:", nid_len);

                    for (size_t i = 0; i < nid_len; i++) {
                        printf(" %02x", nid[i]);
                    }

                    printf("\n");
                    return 0;
            }

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support getting the network ID");
            return 2;

        case -EOVERFLOW:
            puts("Network ID type bigger then 64 bit.");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

/***********************************
 * test maximum packet size        *
 ***********************************/
static int check_max_packet_size(void)
{
    uint8_t mps[sizeof(uint64_t)];
    size_t mps_len = sizeof(uint64_t);
    int res = dev->driver->get_option(dev, NETDEV_OPT_MAX_PACKET_SIZE,
                                      &mps, &mps_len);

    switch (res) {
        case 0:
            switch (mps_len) {
                case 1:
                    printf("Got maximum packet size of value %" PRIu8 " of length %u\n",
                           mps[0], mps_len);
                    return 1;

                case 2:
                    do {
                        uint16_t *size = (uint16_t *)mps;
                        printf("Got maximum packet size of value %" PRIu16 " of length %u\n",
                               *size, mps_len);
                        return 1;
                    } while (0);

                    break;

                case 4:
                    do {
                        uint32_t *size = (uint32_t *)mps;
                        printf("Got maximum packet size of value %" PRIu32 " of length %u\n",
                               *size, mps_len);
                        return 1;
                    } while (0);

                    break;

                case 8:
                    do {
                        uint64_t *size = (uint64_t *)mps;
                        printf("Got maximum packet size of value %" PRIu64 " of length %u\n",
                               *size, mps_len);
                        return 1;
                    } while (0);

                    break;

                default:
                    printf("Unexpected maximum packet size type of length %u:", mps_len);

                    for (size_t i = 0; i < mps_len; i++) {
                        printf(" %02x", mps[i]);
                    }

                    printf("\n");
                    return 0;
            }

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support getting the maximum packet size");
            return 2;

        case -EOVERFLOW:
            puts("Maximum packet size type bigger then 64 bit.");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

/***********************************
 * test protocol                   *
 ***********************************/
static int check_protocol(void)
{
    netdev_proto_t protocol;
    size_t protocol_len = sizeof(netdev_proto_t);
    int res = dev->driver->get_option(dev, NETDEV_OPT_PROTO,
                                      &protocol, &protocol_len);

    switch (res) {
        case 0:
            if (protocol_len != sizeof(netdev_proto_t)) {
                puts("Protocol type is not of length sizeof(netdev_proto_t)");
                return 0;
            }

            switch (protocol) {
                case NETDEV_PROTO_UNKNOWN:
                    puts("Got protocol: Unknown communication type");
                    return 1;

                case NETDEV_PROTO_RADIO:
                    puts("Got protocol: Radio");
                    return 1;

                case NETDEV_PROTO_802154:
                    puts("Got protocol: IEEE 802.15.4");
                    return 1;

                case NETDEV_PROTO_6LOWPAN:
                    puts("Got protocol: 6LoWPAN");
                    return 1;

                case NETDEV_PROTO_IPV6:
                    puts("Got protocol: IPv6");
                    return 1;

                case NETDEV_PROTO_UDP:
                    puts("Got protocol: UDP");
                    return 1;

                case NETDEV_PROTO_TCP:
                    puts("Got protocol: TCP");
                    return 1;

                case NETDEV_PROTO_CCNL:
                    puts("Got protocol: CCN lite");
                    return 1;

                case NETDEV_PROTO_NRF24L01X:
                    puts("Got protocol: NRF24L01X transceiver");
                    return 1;

                default:
                    puts("You probably have to update this switch-case.");
                    return 0;
            }

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Does not support getting the protocol");
            return 2;

        case -EOVERFLOW:
            puts("Protocol type bigger then 64 bit.");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

/*************************************
 * state tests                       *
 *************************************/
static int init_state(void)
{
    int res = dev->driver->set_state(dev, NETDEV_STATE_RX_MODE);

    switch (res) {
        case 0:
            puts("Device was set to RX mode");
            return 1;

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOTSUP:
            puts("Device does not have a RX mode");
            return 2;

        case -ETIME:
            puts("Device timed out setting to RX mode");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

static int check_state(void)
{
    netdev_state_t state;
    int res = dev->driver->get_state(dev, &state);

    switch (res) {
        case 0:
            switch (state) {
                case NETDEV_STATE_POWER_OFF:
                    puts("Device is powered off");
                    break;

                case NETDEV_STATE_POWER_SLEEP:
                    puts("Device is sleeping");
                    break;

                case NETDEV_STATE_POWER_IDLE:
                    puts("Device is idle");
                    break;

                case NETDEV_STATE_RX_MODE:
                    puts("Device is in RX mode");
                    break;

                case NETDEV_STATE_PROMISCUOUS_MODE:
                    puts("Device is in promiscous mode");
                    break;

                default:
                    puts("Device is in unknown state");
                    return 0;
            }

            return 1;

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ETIME:
            puts("Device timed out getting to RX mode");
            break;

        default:
            puts("unknown error");
            break;
    }

    return 0;
}

#ifdef SENDER
static int send_packet(void)
{
    netdev_hlist_t *hlist = NULL;
    netdev_hlist_t header1 = {NULL, NULL, "header 1,", 9};
    netdev_hlist_t header2 = {NULL, NULL, "header 2,", 9};
    char payload[] = "payload";

    netdev_hlist_add(&hlist, &header2);
    netdev_hlist_add(&hlist, &header1);

    printf("Sending \"%s%s%s\" to", (char *)header1.header, (char *)header2.header,
           payload);

    switch (dev_address_len) {
        case 1:
            do {
                uint8_t dest = NETDEV_TEST_RECEIVER;

                printf(" %" PRIu8 "\n", dest);
                dev->driver->send_data(dev, &dest, dev_address_len,
                                       hlist, payload, strlen(payload));
            }
            while (0);

            break;

        case 2:
            do {
                uint16_t dest = NETDEV_TEST_RECEIVER;

                printf(" %" PRIu16 "\n", dest);
                dev->driver->send_data(dev, &dest, dev_address_len,
                                       hlist, payload, strlen(payload));
            }
            while (0);

            break;

        case 3:
            do {
                uint16_t dest_int = HTONS(NETDEV_TEST_RECEIVER);
                uint8_t dest[dev_address_len];

                memset(dest, 0, dev_address_len - sizeof(uint16_t));
                memcpy(&(dest[dev_address_len - sizeof(uint16_t)]), &dest_int,
                       sizeof(uint16_t));

                for (size_t i = 0; i < dev_address_len; i++) {
                    printf(" %02x", dest[i]);
                }

                puts("");

                dev->driver->send_data(dev, &dest, dev_address_len,
                                       hlist, payload, strlen(payload));
            }
            while (0);

        case 4:
            do {
                uint32_t dest = NETDEV_TEST_RECEIVER;

                printf(" %" PRIu32 "\n", dest);
                dev->driver->send_data(dev, &dest, dev_address_len, hlist,
                                       payload, strlen(payload));
            }
            while (0);

            break;

        case 8:
            do {
                uint64_t dest = NETDEV_TEST_RECEIVER;

                printf(" %" PRIu64 "\n", dest);
                dev->driver->send_data(dev, &dest, dev_address_len,
                                       hlist, payload, strlen(payload));
            }
            while (0);

            break;

        default:
            do {
                uint32_t dest_int = HTONL(NETDEV_TEST_RECEIVER);
                uint8_t dest[dev_address_len];

                memset(dest, 0, dev_address_len - sizeof(uint32_t));
                memcpy(&(dest[dev_address_len - sizeof(uint32_t)]), &dest_int,
                       sizeof(uint32_t));

                for (size_t i = 0; i < dev_address_len; i++) {
                    printf(" %02x", dest[i]);
                }

                puts("");

                dev->driver->send_data(dev, &dest, dev_address_len,
                                       hlist, payload, strlen(payload));
            }
            while (0);

            break;
    }

    return 1;
}
#endif

#ifdef RECEIVER
static int test_callback(netdev_t *rcv_dev, void *src, size_t src_len,
                         void *dest, size_t dest_len, void *payload,
                         size_t payload_len)
{
    uint8_t exp_src[dev_address_len];
    uint8_t exp_dest[dev_address_len];

    if (rcv_dev != dev) {
        printf("cb: Device is not the expected one: %p != %p\n",
               (void *)rcv_dev, (void *)dev);
        return -EINVAL;
    }

    switch (dev_address_len) {
        case 1:
            exp_src[0] = NETDEV_TEST_SENDER;
            exp_dest[0] = NETDEV_TEST_RECEIVER;
            break;

        case 2:
            do {
                uint16_t *exp_src_ptr = (uint16_t *)exp_src;
                uint16_t *exp_dest_ptr = (uint16_t *)exp_dest;
                *exp_src_ptr = NETDEV_TEST_SENDER;
                *exp_dest_ptr = NETDEV_TEST_RECEIVER;
            } while (0);

            break;

        case 3:
            do {
                uint16_t exp_src_int = HTONS(NETDEV_TEST_SENDER);
                uint16_t exp_dest_int = HTONS(NETDEV_TEST_RECEIVER);
                memset(exp_dest, 0, dev_address_len - sizeof(uint16_t));
                memset(exp_src, 0, dev_address_len - sizeof(uint16_t));
                memcpy(&(exp_dest[dev_address_len - sizeof(uint16_t)]),
                       &exp_dest_int, sizeof(uint16_t));
                memcpy(&(exp_src[dev_address_len - sizeof(uint16_t)]),
                       &exp_src_int, sizeof(uint16_t));
            }
            while (0);

            break;

        case 4:
            do {
                uint32_t *exp_src_ptr = (uint32_t *)exp_src;
                uint32_t *exp_dest_ptr = (uint32_t *)exp_dest;
                *exp_src_ptr = NETDEV_TEST_SENDER;
                *exp_dest_ptr = NETDEV_TEST_RECEIVER;
            } while (0);

            break;

        case 8:
            do {
                uint64_t *exp_src_ptr = (uint64_t *)exp_src;
                uint64_t *exp_dest_ptr = (uint64_t *)exp_dest;
                *exp_src_ptr = NETDEV_TEST_SENDER;
                *exp_dest_ptr = NETDEV_TEST_RECEIVER;
            } while (0);

            break;

        default:
            do {
                uint32_t exp_src_int = HTONL(NETDEV_TEST_SENDER);
                uint32_t exp_dest_int = HTONL(NETDEV_TEST_RECEIVER);
                memset(exp_dest, 0, dev_address_len - sizeof(uint32_t));
                memset(exp_src, 0, dev_address_len - sizeof(uint32_t));
                memcpy(&(exp_dest[dev_address_len - sizeof(uint32_t)]),
                       &exp_dest_int, sizeof(uint32_t));
                memcpy(&(exp_src[dev_address_len - sizeof(uint32_t)]),
                       &exp_src_int, sizeof(uint32_t));
            }
            while (0);

            break;
    }
#ifndef MODULE_NRF24L01P
    if (src_len != dev_address_len || memcmp(exp_src, src, src_len) != 0) {
        printf("cb: src is not from sender %d\n", NETDEV_TEST_SENDER);
        return -EINVAL;
    }

    if (dest_len != dev_address_len || memcmp(exp_dest, dest, dest_len) != 0) {
        printf("cb: dest is not for receiver %d\n", NETDEV_TEST_SENDER);
        return -EINVAL;
    }

    if (strcmp(payload, "header 1,header 2,payload") != 0) {
        puts("cb: payload is not as expected (\"header 1,header 2,payload\")");
        return -EINVAL;
    }
#endif
    printf("Received \"");

    for (size_t i = 0; i < payload_len; i++) {
        printf("%c", ((char *)payload)[i]);
    }

    printf("\"\n");

    printf("Sender was");

    if (src_len !=0) {
        for (size_t i = 0; i < src_len; i++) {
            printf(" %02x", ((char *)src)[i]);
        }
    }
    else {
            printf(" not known");
    }

    printf("\n");

    return payload_len;
}

static int init_receiver_callback(void)
{
    int res = dev->driver->add_receive_data_callback(dev, test_callback);

    switch (res) {
        case 0:
            puts("Added receive callback successfully.");
            return 1;

        case -ENODEV:
            puts("Device was not initialized");
            break;

        case -ENOBUFS:
            puts("Can not add callback: no slot left");
            break;

        default:
            puts("Unknown error");
            break;
    }

    return 0;
}
#endif

int main(void)
{
    int init_res;

    puts("\nRIOT nrf24l01p_netdev test");
    dev = NETDEV_DEFAULT;

    if (dev == NULL) {
        puts("Default device was NULL");
        return 1;
    }

    printf("Initialized dev ");

    switch (dev->type) {
        case NETDEV_TYPE_UNKNOWN:
            printf("of unknown type\n");
            break;

        case NETDEV_TYPE_BASE:
            printf("as basic device\n");
            break;

        default:
            printf("of undefined type\n");
            break;
    }

    if (dev->driver == NULL) {
        puts("Default driver is defined as NULL!");
        return 1;
    }

    dev->driver->init(dev);

    if (!(init_res = init_channel())) {
        return 1;
    }

    if (check_channel() == 0) {
        printf("Channel is not as expected. ");

        if (init_res == 2) {
            printf("But initialization is not supported. Continuing.\n");
        }
        else {
            printf("Aborting\n");
            return 1;
        }
    }

    if (!(init_res = init_address())) {
        return 1;
    }

    if (check_address() == 0) {
        printf("Address is not as expected. ");

        if (init_res == 2) {
            printf("But initialization is not supported. Continuing.\n");
        }
        else {
            printf("Aborting\n");
            return 1;
        }
    }

    if (!(init_res = init_long_address())) {
        return 1;
    }

    if (check_long_address() == 0) {
        printf("Long address is not as expected. ");

        if (init_res == 2) {
            printf("But initialization is not supported. Continuing.\n");
        }
        else {
            printf("Aborting\n");
            return 1;
        }
    }

    if (!(init_res = init_nid())) {
        return 1;
    }

    if (check_nid() == 0) {
        printf("Network ID is not as expected. ");

        if (init_res == 2) {
            printf("But initialization is not supported. Continuing.\n");
        }
        else {
            printf("Aborting\n");
            return 1;
        }
    }

    if (check_max_packet_size() == 0) {
        return 1;
    }

    if (check_protocol() == 0) {
        return 1;
    }

    if (!init_state()) {
        return 1;
    }

    if (!check_state()) {
        return 1;
    }

#ifdef SENDER

    if (!send_packet()) {
        return 1;
    }

#elif RECEIVER

    if (!init_receiver_callback()) {
        return 1;
    }

#endif

    return 0;
}

#else

int main(void)
{
    puts("\nRIOT nrf24l01p_netdev test");
    puts("Default netdev type and driver unknown!");

    return 0;
}

#endif /* !NETDEV_DEFAULT */
