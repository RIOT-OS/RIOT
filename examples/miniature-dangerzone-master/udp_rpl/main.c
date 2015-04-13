#include <stdio.h>
#include <string.h>

#include "sixlowpan.h"
#include "ipv6.h"

#define PANID       (0x3412)
#define EUT         (9)
#define TARGET {{0x00,0x07}};
#define ICMP_ID     (0xf2fc)
#define ICMP_DATA   "abcd"

void link_local_from_short_addr(ipv6_addr_t *dest, ieee_802154_short_t *src) {
    ieee_802154_long_t laddr;
    laddr.uint16[0] = PANID ^ 0x0200;
    laddr.uint16[1] = 0x00FF;
    laddr.uint16[2] = 0xFE00;
    laddr.uint16[3] = src->uint16[0];
    ipv6_addr_set_link_local_prefix(dest);
    memcpy(&dest->uint32[2], &laddr, 8);
}

void ping(void) {
    ipv6_addr_t dest_addr;
    char addr_str[IPV6_MAX_ADDR_STR_LEN];
    ieee_802154_short_t dest_short = TARGET;

    link_local_from_short_addr(&dest_addr, &dest_short);

    printf("ping %s\n", ipv6_addr_to_str(addr_str, &dest_addr));
    icmpv6_send_echo_request(&dest_addr, ICMP_ID, 1, ICMP_DATA, sizeof(ICMP_DATA));
    return;
}

int main(void) {
    puts("UDP over RPL application");
    printf("Initializes 6LoWPAN with short address %u, HC enabled\n", EUT);

    sixlowpan_lowpan_set_iphc_status(LOWPAN_IPHC_ENABLE);

    sixlowpan_lowpan_init(TRANSCEIVER_NATIVE, EUT, 0);

    ipv6_iface_print_addrs();

    ping();
    while (1);
    return 0;
}
