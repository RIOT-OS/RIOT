/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <posix_io.h>
#include <shell.h>
#include <board_uart0.h>
#include <vtimer.h>
#include <ltc4150.h>
#include <thread.h>
#include <cc110x_ng.h>
#include <transceiver.h>
#include <time.h>
#include <rtc.h>
#include "sys/net/sixlowpan/sixlowmac.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowborder.h"
#include "sys/net/sixlowpan/sixlowpan.h"
#include "sys/net/sixlowpan/sixlowerror.h"

void init(char *str){
    char command;
    uint16_t r_addr;
    ipv6_addr_t std_addr;
    
    int res = sscanf(str, "init %c %hu", &command, &r_addr);
    
    if(res < 1){
        printf("Usage: init {h | r | a | e} radio_address\n");
        printf("\th\tinitialize as host\n");
        printf("\tr\tinitialize as router\n");
        printf("\ta\tinitialize as ad-hoc router\n");
        printf("\tb\tinitialize as border router\n\n");
        printf("\tradio_address must be an 8 bit integer\n");
    }
    
    ipv6_init_address(&std_addr,0xABCD,0,0,0,0x1034,0x00FF,0xFE00,r_addr);
    
    switch (command) {
        case 'h':
            printf("INFO: Initialize as host on radio address %hu\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            sixlowpan_init(TRANSCEIVER_CC1100,r_addr,0);
            break;
        case 'r':
            printf("INFO: Initialize as router on radio address %hu\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            sixlowpan_init(TRANSCEIVER_CC1100, r_addr,0);
            ipv6_init_iface_as_router();
            break;
        case 'a':
            printf("INFO: Initialize as adhoc router on radio address %hu\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            sixlowpan_adhoc_init(TRANSCEIVER_CC1100, &std_addr, r_addr);
            break;
        case 'b':
            printf("INFO: Initialize as border router on radio address %hu\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            res = border_initialize(TRANSCEIVER_CC1100, &std_addr);
            switch (res) {
                case (SUCCESS): printf("INFO: Border router initialized.\n"); break;
                case (SIXLOWERROR_ADDRESS): printf("ERROR: Illegal IP address: "); 
                        ipv6_print_addr(&std_addr); break;
                default: printf("ERROR: Unknown error (%d).\n", res); break;
            }
            break;
        default:
            printf("ERROR: Unknown command '%c'\n", command);
            break;
    }
}

void bootstrapping(char *str){
    sixlowpan_bootstrapping();
}

void send_packet(char *str){
    uint8_t test[2];
    test[0] = 30;
    test[1] = 98;

    ipv6_addr_t ipaddr;
    ipv6_init_address(&ipaddr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, 0x0005); 
    ipv6_print_addr(&ipaddr);
    
    for(int j=0;j<100;j++){
        test[0] = j;
        for(int i=0;i<1000;i++){
            sixlowpan_send(&ipaddr, test, 2, 0);
        }
        //lib6lowpan_bootstrapping(&addr8);
    }
}

void set_radio_chann(char *str){
    uint16_t chann;
    int res = sscanf(str, "set_chann %hu", &chann);
    if(res < 1){
        printf("Usage: set_chann [channel]\n");
    }
    
    cc110x_set_channel(chann);
}

void get_r_address(char *str){
    printf("radio: %hu\n", cc110x_get_address());
}

void ip(char *str){
    ipv6_iface_print_addrs();
}

void context(char *str){
    uint8_t i;
    lowpan_context_t *context;
    
    for(i = 0; i < LOWPAN_CONTEXT_MAX; i++){
        context = lowpan_context_num_lookup(i);
        if (context != NULL) {
            printf("%2d\tLifetime: %5u\tLength: %3d\t",context->num,context->lifetime,context->length);
            ipv6_print_addr(&(context->prefix));
        }
    }
}

const shell_command_t shell_commands[] = {
    {"send", "", send_packet},
    {"init", "", init},
    {"addr", "", get_r_address},
    {"set_chann", "", set_radio_chann},
    {"boot", "", bootstrapping},
    {"ip", "", ip},
    {"context", "", context},
    {NULL, NULL, NULL}
};

int main(void) {
    printf("6LoWPAN\n");
    vtimer_init();
    
    posix_open(uart0_handler_pid, 0);
    //struct tm now;
    //rtc_get_localtime(&now);
    
    //srand((unsigned int)now.tm_sec);
    //uint8_t random = rand() % 256;
    //printf("address: %d\n", random);

    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}


