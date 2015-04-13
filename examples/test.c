 
/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "posix_io.h"
#include "shell.h"
#include "board_uart0.h"
#include "vtimer.h"
#include "ltc4150.h"
#include "thread.h"
#include "cc110x_ng.h"
#include "transceiver.h"
#include "time.h"
#include "rtc.h"
#include "ipv6.h"
#include "sixlowpan.h"

#ifdef MODULE_NATIVENET
#define TRANSCEIVER_TYPE TRANSCEIVER_NATIVE
#else
#define TRANSCEIVER_TYPE TRANSCEIVER_CC1100
#endif

void print_ipv6_addr(const ipv6_addr_t *ipv6_addr)
{
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    printf("%s\n", ipv6_addr_to_str(addr_str, ipv6_addr));
}

void printUsage()
{
    printf("Usage: init {h | r | a | e} radio_address\n");
    printf("\th\tinitialize as host\n");
    printf("\tr\tinitialize as router\n");
    printf("\ta\tinitialize as ad-hoc router\n");
    printf("\tb\tinitialize as border router\n\n");
    printf("\tradio_address must be an 8 bit integer\n");
}

void init(char *str)
{
    uint16_t r_addr;
    ipv6_addr_t std_addr;
    size_t str_len = strlen(str);
    char *command = strtok(str, " ");

    if ((command = strtok(NULL, " ")) == NULL) {
        printUsage();
        return;
    }

    char *p;
    if (((p = strtok(NULL, " ")) == NULL)
            || ((r_addr = (uint16_t) strtol(p, NULL, 10)) == 0)) {
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

            sixlowpan_lowpan_init(TRANSCEIVER_TYPE, r_addr, 0);
            break;

        case 'r':
            printf("INFO: Initialize as router on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }

            sixlowpan_lowpan_init(TRANSCEIVER_TYPE, r_addr, 0);
            ipv6_init_iface_as_router();
            break;

        case 'a':
            printf("INFO: Initialize as adhoc router on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }

            sixlowpan_lowpan_adhoc_init(TRANSCEIVER_TYPE, &std_addr, r_addr);
            break;

        case 'b':
            printf("INFO: Initialize as border router on radio address %hu\n", r_addr);

            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }

            int res = sixlowpan_lowpan_border_init(TRANSCEIVER_TYPE, &std_addr);

            switch (res) {
                case (SIXLOWERROR_SUCCESS):
                    printf("INFO: Border router initialized.\n");
                    break;

                case (SIXLOWERROR_ADDRESS):
                    printf("ERROR: Illegal IP address: ");
                    print_ipv6_addr(&std_addr);
                    break;

                default:
                    printf("ERROR: Unknown error (%d).\n", res);
                    break;
            }

            break;

        default:
            printf("ERROR: Unknown command '%c'\n", command[0]);
            printUsage();
            break;
    }
}

void bootstrapping(char *str)
{
    sixlowpan_lowpan_bootstrapping();
}

void send_packet(char *str)
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

void ip(char *str)
{
    ipv6_iface_print_addrs();
}

void context(char *str)
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

const shell_command_t shell_commands[] = {
    {"send", "", send_packet},
    {"init", "", init},
    {"boot", "", bootstrapping},
    {"ip", "", ip},
    {"context", "", context},
    {NULL, NULL, NULL}
};

int main(void)
{
    printf("6LoWPAN\n");
    vtimer_init();

    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}

