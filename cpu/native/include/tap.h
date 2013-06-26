#ifndef _TAP_H
#define _TAP_H

#include <net/ethernet.h>

/**
 * create and/or open tap device "name"
 *
 * if "name" is an empty string, the kernel chooses a name
 * if "name" is an existing device, that device is used
 * otherwise a device named "name" is created
 */
int tap_init(char *name);
int send_buf(void);
void _native_marshall_ethernet(uint8_t *framebuf, uint8_t *data, int data_len);
void _native_cc1100_handle_input(unsigned char *buf, int size);

extern int _native_tap_fd;
extern unsigned char _native_tap_mac[ETHER_ADDR_LEN];

union eth_frame {
    struct {
        struct ether_header header;
        unsigned char data[ETHERMTU];
    } field;
    unsigned char buffer[ETHER_MAX_LEN];
};

#endif /* _TAP_H */
