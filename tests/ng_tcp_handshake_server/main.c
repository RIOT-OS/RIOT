#include <stdio.h>
#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "byteorder.h"
#include "net/ng_nettype.h"
#include "net/ng_netreg.h"
#include "net/ng_netapi.h"
#include "net/ng_pktbuf.h"
#include "net/ng_pkt.h"
#include "net/ng_ipv6.h"
#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/nc.h"
#include "net/ng_netbase.h"
#include "net/ng_netif.h"
#include "transceiver.h"
#include "net/ng_tcp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define LOCL_PORT 2000

int main(void)
{
    ng_tcp_tcb_t tcb;
    //ng_ipv6_addr_t locl_addr;
    ng_ipv6_addr_t peer_addr = NG_IPV6_ADDR_UNSPECIFIED;

    /* Print current server ipv6-addresses on interface 0 */
    printf("-- TCP Server Running on Port %d\n", LOCL_PORT);
    printf("-- Assigned IP-Addresses: \n");

    kernel_pid_t ifs[NG_NETIF_NUMOF];
    char ipv6_addr[NG_IPV6_ADDR_MAX_STR_LEN];
    ng_netif_get(ifs);

    ng_ipv6_netif_t *entry = ng_ipv6_netif_get(ifs[0]);
    for (int i = 0; i < NG_IPV6_NETIF_ADDR_NUMOF; i++) {
        if (!ng_ipv6_addr_is_unspecified(&entry->addrs[i].addr)) {
            printf("inet6 addr: ");
            if (ng_ipv6_addr_to_str(ipv6_addr, &entry->addrs[i].addr, NG_IPV6_ADDR_MAX_STR_LEN)) {
                printf("%s/%" PRIu8 "\n", ipv6_addr, entry->addrs[i].prefix_len);
            }
        }
    }

    /* Initiailze TCB and wait till peer connects */
    ng_tcp_tcb_init(&tcb);
    ng_tcp_open(&tcb, LOCL_PORT, (uint8_t *) &peer_addr, sizeof(peer_addr), 0, AI_PASSIVE);

    if(tcb.state == ESTABLISHED){
        printf("Srv: Connection Established\n");
    } else if(tcb.state == CLOSED){
        printf("Srv: Connection Closed\n");
    }

    // Destroy Transmission Control Block
    ng_tcp_tcb_destroy(&tcb);

    exit(0);
}
