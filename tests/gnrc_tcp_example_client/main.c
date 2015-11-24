#include <stdio.h>
#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "byteorder.h"
#include "xtimer.h"
#include "net/inet_csum.h"
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

#define LOCL_ADDR "fc00::2"
#define PEER_ADDR "fc00::1"
#define PEER_PORT 2000

#define REQUESTED_DATA 4031
#define BUFFER_SIZE 100

const char request_str[] = "Send me x bytes\n";
typedef struct {
    char msg[sizeof(request_str)];
    uint16_t nByte;
}request_t;


int main(void)
{
    /* Setup Request and Receive Buffer */
    request_t req;
    strncpy(req.msg, request_str, sizeof(request_str));
    req.nByte = REQUESTED_DATA;

    /* Use small buffer for incremental reading */
    uint8_t  buf[BUFFER_SIZE];
    uint16_t left = REQUESTED_DATA;
    ssize_t rcvd = 0;
    uint16_t csum = 0;

    /* Setup IP-Adress */
    ipv6_addr_t locl_addr;
    ipv6_addr_t peer_addr;
    ipv6_addr_from_str(&locl_addr, LOCL_ADDR);
    ipv6_addr_from_str(&peer_addr, PEER_ADDR);

    /* Assign IPv6 adress to interface 0 */
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    gnrc_netif_get(ifs);
    ipv6_addr_from_str(&locl_addr, LOCL_ADDR);
    gnrc_ipv6_netif_add_addr(ifs[0], &locl_addr, 64, false);

    /* Initialize TCB */
    gnrc_tcp_tcb_t tcb;
    gnrc_tcp_tcb_init(&tcb);

    /* Open Connection */
    gnrc_tcp_open(&tcb, 0, (uint8_t *) &peer_addr, sizeof(peer_addr), PEER_PORT, 0);

    /* Send request */
    gnrc_tcp_send(&tcb, (void *)&req, sizeof(req), 0, 0);

    //int i = 0;

    /* Read data in 100 byte pieces from the buffer */
    while(left){
        if(left >= sizeof(buf)){
            rcvd = gnrc_tcp_recv(&tcb, buf, sizeof(buf));

            if(rcvd < 0){
                printf("Something went wrong\n");
                return -1;
            }

            csum = inet_csum(csum, buf, rcvd);
        }else{
            rcvd = gnrc_tcp_recv(&tcb, buf, left);
            csum = inet_csum(csum, buf, rcvd);
        }
        left -= rcvd;
    }
    
    /* Calculate and print checksum of Payload for verification */
    printf("Checksum of recived data: %d\n", csum);

    /* Close Connection */
    gnrc_tcp_close(&tcb);

    /* Destroy Transmission Control Block */
    gnrc_tcp_tcb_destroy(&tcb);

    exit(0);
}
