#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cc1100-interface.h>

#ifdef MODULE_CC110X

void _cc110x_get_set_address_handler(char *addr) {
    int16_t a;

    a = atoi(addr+5);
    if (strlen(addr) > 5) {
        printf("[cc110x] Setting address %i ... ", a);
        cc1100_set_address((radio_address_t)a);
        if (cc1100_get_address() == (radio_address_t)a) {
            puts("[OK]");
        } else {
            puts("Error!");
        }
    }
    else {
        printf("[cc1100] Got address: %i\n", cc1100_get_address());
    }
}

void _cc110x_get_set_channel_handler(char *addr) {
    int16_t a;

    a = atoi(addr+5);
    if (strlen(addr) > 5) {
        printf("[cc110x] Setting channel %i...", a);
        cc1100_set_channel(a);
        if (cc1100_get_channel() == a) {
            puts("OK");
        } else {
            puts("Error!");
        }
    }
    else {
        printf("[cc1100] Got address: %i\n", cc1100_get_channel());
    }
}
#endif
