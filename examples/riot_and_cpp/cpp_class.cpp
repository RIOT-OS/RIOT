/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 * Copyright (C) 2014 Ho Chi Minh city University of Technology (HCMUT)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file        cpp_class.cpp
 * @brief       implementation of declared functions of object cpp_class
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 *
 * @}
 */

#include "cpp_class.hpp"

#define TRANSCEIVER_TYPE TRANSCEIVER_AT86RF231

sixlowpan::sixlowpan()
{
	printf("%s\n", "sixlowpan object spawned!");
}

sixlowpan::~sixlowpan()
{
	printf("%s\n", "sixlowpan object killed!");
}

extern "C" void sixlowpan::sixlowpan_init(int argc, char **argv)
{
	printf("%s\n", "sixlowpan initiating!");

	uint16_t r_addr;
    ipv6_addr_t std_addr;
    char* str = *argv;
    //size_t str_len = strlen(str);
    char *command = strtok(str, " ");

    if ((command = strtok(NULL, " ")) == NULL) {
        printUsage();
        return;
    }

    char *p;
    if (((p = strtok(NULL, " ")) == NULL) || ((r_addr = (uint16_t) strtol(p, NULL, 10)) == 0)) {
        printUsage();
        return;
    }

    ipv6_addr_init(&std_addr, 0xABCD, 0, 0, 0, 0x1034, 0x00FF, 0xFE00, r_addr);

    switch (command[0]) {
        case 'h':
            printf("INFO: Initialize as host on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }

            sixlowpan_lowpan_init();//TRANSCEIVER_TYPE, r_addr, 0);
            break;

        case 'r':
            printf("INFO: Initialize as router on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }

            sixlowpan_lowpan_init();//TRANSCEIVER_TYPE, r_addr, 0);
            ipv6_init_as_router();
            break;

        case 'a':
            printf("INFO: Initialize as adhoc router on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }

            //sixlowpan_lowpan_adhoc_init(TRANSCEIVER_TYPE, &std_addr, r_addr);
            sixlowpan_lowpan_init_adhoc_interface(TRANSCEIVER_TYPE, &std_addr);
            break;

        // case 'b':
        // {
        //     printf("INFO: Initialize as border router on radio address %hu\n", r_addr);

        //     if (r_addr > 255) {
        //         printf("ERROR: radio_address not an 8 bit integer\n");
        //         return;
        //     }

        //     //int res = sixlowpan_lowpan_border_init(TRANSCEIVER_TYPE, &std_addr);
        // 	int res = sixlowpan_lowpan_border_init(TRANSCEIVER_TYPE);
        //     switch (res) {
        //         case (SIXLOWERROR_SUCCESS):
        //             printf("INFO: Border router initialized.\n");
        //             break;

        //         case (SIXLOWERROR_ADDRESS):
        //             printf("ERROR: Illegal IP address: ");
        //             print_ipv6_addr(&std_addr);
        //             break;

        //         default:
        //             printf("ERROR: Unknown error (%d).\n", res);
        //             break;
        //     }

        //     break;
		//}
        default:
            printf("ERROR: Unknown command '%c'\n", command[0]);
            printUsage();
            break;
    }
}
	


void sixlowpan::printUsage()
{
	printf("Usage: init {h | r | a | e} radio_address\n");
    printf("\th\tinitialize as host\n");
    printf("\tr\tinitialize as router\n");
    printf("\ta\tinitialize as ad-hoc router\n");
    printf("\tb\tinitialize as border router\n\n");
    printf("\tradio_address must be an 8 bit integer\n");
}

void sixlowpan::print_ipv6_addr(const ipv6_addr_t *ipv6_addr)
{
    //char addr_str[IPV6_MAX_ADDR_STR_LEN];
    char *addr_str;
    printf("%s\n", ipv6_addr_to_str(addr_str,IPV6_MAX_ADDR_STR_LEN, ipv6_addr));
}

extern "C" void sixlowpan::bootstrapping(int argc, char **argv)
{
    sixlowpan_lowpan_bootstrapping();
}

extern "C" void sixlowpan::send_packet(int argc, char **argv)
{
    uint8_t test[2];
    test[0] = 30;
    test[1] = 98;

    ipv6_addr_t ipaddr;
    ipv6_addr_init(&ipaddr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, 0x0005);
    print_ipv6_addr(&ipaddr);

    for (int j = 0; j < 100; j++) {
        test[0] = j;

        for (int i = 0; i < 1000; i++) {
            ipv6_sendto(&ipaddr, IPV6_PROTO_NUM_NONE, test, 2);
        }

        //lib6lowpan_bootstrapping(&addr8);
    }
}

extern "C" void sixlowpan::ip(int argc, char **argv)
{
    //ipv6_iface_print_addrs();
    printf("TODO: sixlowpan class -> Member ip!\t");
}

extern "C" void sixlowpan::context(int argc, char **argv)
{
    uint8_t i;
    lowpan_context_t *context;

    for (i = 0; i < NDP_6LOWPAN_CONTEXT_MAX; i++) {
        context = lowpan_context_num_lookup(i);

        if (context != NULL) {
            printf("%2d\tLifetime: %5u\tLength: %3d\t", context->num, context->lifetime, context->length);
            print_ipv6_addr(&(context->prefix));
        }
    }
}





cpp_class::cpp_class()
{
    printf("Instanciating Object [constructor called]\n");
    greet();
}

cpp_class::~cpp_class()
{
    printf("Destroying Object [destructor called]\n");
    printf("Im shutting down!\n");
}

void cpp_class::say_hello(void)
{
    printf("Hello!\n");
}

void cpp_class::say_hello(int n)
{
    printf("Hello![int: %d]\n", n);
}

void cpp_class::say_hello(float f)
{
    printf("Hello![float: %f]\n", f);
}

void cpp_class::greet(void)
{
    printf("Im starting!\n");
}
