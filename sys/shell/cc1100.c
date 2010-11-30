#include <stdio.h>
#include <cc1100-interface.h>

void _cc1100_get_address_handler(char *str) {
    radio_address_t addr = cc1100_get_address();
    printf("cc1100 address: %i\n", addr);
}

void _cc1100_set_address_handler(char *str) {
    int addr;
    int res = sscanf(str, "cc1100_set_address %i", &addr);
    if (res == 1) {
        cc1100_set_address((radio_address_t)addr);
        printf("Setting cc1100 address to %i: ", addr);
        if (cc1100_get_address() == (radio_address_t)addr) {
            puts("OK");
        } else {
            puts("Error!");
        }
    } else {
        puts("usage: cc1100_set_address <address>");
    }
}

