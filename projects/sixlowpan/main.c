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
            sixlowpan_send(&ipaddr, &test, 2);
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


const shell_command_t shell_commands[] = {
    {"send", "", send_packet},
    {"addr", "", get_r_address},
    {"set_chann", "", set_radio_chann},
    {"boot", "", bootstrapping},
    {NULL, NULL, NULL}
};

int main(void) {
    //printf("Moin. build on %s %s SVN-Revision: %s\n", kernel_builddate, kernel_buildtime, kernel_svnrevision);
    printf("6LoWPAN\n");
    vtimer_init();
    
    posix_open(uart0_handler_pid, 0);
    struct tm now;
    //rtc_get_localtime(&now);
    
    //srand((unsigned int)now.tm_sec);
    //uint8_t random = rand() % 256;
    //printf("address: %d\n", random);
    sixlowpan_init(TRANSCEIVER_CC1100,5);

    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}


