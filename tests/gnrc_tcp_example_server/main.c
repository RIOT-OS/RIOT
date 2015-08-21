#include <stdio.h>
#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "byteorder.h"
#include "xtimer.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/ipv6.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/tcp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define LOCL_ADDR "fe80::1"
#define LOCL_PORT 2000

int main(void)
{
    /* Allocate data structures */
    ipv6_addr_t locl_addr;
    ipv6_addr_t peer_addr = IPV6_ADDR_UNSPECIFIED;
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    gnrc_tcp_tcb_t tcb;

    /* Assign IPv6 adress to interface 0 */
    gnrc_netif_get(ifs);
    ipv6_addr_from_str(&locl_addr, LOCL_ADDR);
    gnrc_ipv6_netif_add_addr(ifs[0], &locl_addr, 64, false);

    /* Initiailze TCB */
    gnrc_tcp_tcb_init(&tcb);

    /* Open connection */
    gnrc_tcp_open(&tcb, LOCL_PORT, (uint8_t *) &peer_addr, sizeof(peer_addr), 0, AI_PASSIVE);

    /* Waste Time */
    xtimer_sleep(2);

    /* Close Connection */
    gnrc_tcp_close(&tcb);

    /* Destroy Transmission Control Block */
    gnrc_tcp_tcb_destroy(&tcb);

    exit(0);
}
