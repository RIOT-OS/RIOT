#include <stdio.h>
#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "byteorder.h"
#include "net/ng_tcp.h"
#include "net/ng_nettype.h"
#include "net/ng_netreg.h"
#include "net/ng_netapi.h"
#include "net/ng_pktbuf.h"
#include "net/ng_pkt.h"
#include "net/ng_ipv6.h"
#include "net/ng_ipv6/addr.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define PORT_SRV 1000

kernel_pid_t main_pid = KERNEL_PID_UNDEF;

static char _stack_srv[THREAD_STACKSIZE_MAIN];
static char _stack_cli[THREAD_STACKSIZE_MAIN];
static char _stack_fake_ip6[THREAD_STACKSIZE_MAIN];

void *cli(__attribute__((unused))void *arg)
{
    msg_t msg;
    ng_tcp_tcb_t tcb;
    ng_ipv6_addr_t peer_addr;

    /* Setup Peer Addr */
    ng_ipv6_addr_from_string(&peer_addr, "2000::1000");

    /* Initialize TCB and connect to peer */
    ng_tcp_tcb_init(&tcb);
    ng_tcp_open(&tcb, 0, (uint8_t *) &peer_addr, sizeof(peer_addr), PORT_SRV, 0);

    if(tcb.state == ESTABLISHED){
        printf("Cli: Connection Established\n");
    }

    // Destroy Transmission Control Block
    ng_tcp_tcb_destroy(&tcb);

    // Connection should be Established successfully.
    msg_send(&msg, main_pid);

    return NULL;
}

void *srv(__attribute__((unused))void *arg)
{
    msg_t msg;
    ng_tcp_tcb_t tcb;
    ng_ipv6_addr_t peer_addr = NG_IPV6_ADDR_UNSPECIFIED;

    /* Initiailze TCB and wait till peer connects */
    ng_tcp_tcb_init(&tcb);
    ng_tcp_open(&tcb, PORT_SRV, (uint8_t *) &peer_addr, sizeof(peer_addr), 0, AI_PASSIVE);

    if(tcb.state == CLOSED){
        printf("Srv: Connection Closed\n");
    }

    if(tcb.state == ESTABLISHED){
        printf("Srv: Connection Established\n");
    }

    // Destroy Transmission Control Block
    ng_tcp_tcb_destroy(&tcb);

    // Connection should be Established successfully.
    msg_send(&msg, main_pid);

    return NULL;
}

void *fake_ip6(__attribute__((unused))void *arg)
{
    // Lookup TCP Thread Entry
    ng_netreg_entry_t* tcp = NULL;
    if( (tcp = ng_netreg_lookup(NG_NETTYPE_TCP, NG_NETREG_DEMUX_CTX_ALL)) == NULL ){
        DEBUG("TCP is not running! Stopping \"fake_ip6\"\n");
        return NULL;
    }

    // Register as IP-Layer in netreg
    ng_netreg_entry_t reg;
    reg.demux_ctx = NG_NETREG_DEMUX_CTX_ALL;
    reg.pid = thread_getpid();
    ng_netreg_register(NG_NETTYPE_IPV6, &reg);

    // Variables
    msg_t msg;
    ng_tcp_hdr_t  tcp_hdr;
    ng_ipv6_hdr_t ip6_hdr;
    ng_pktsnip_t *old_pkt = NULL;
    ng_pktsnip_t *new_pkt = NULL;
    uint8_t buf[600];
    ng_ipv6_addr_t src_addr = NG_IPV6_ADDR_LOOPBACK;
    size_t ip6_len = 0;
    size_t tcp_len = 0;
    size_t pay_len = 0;

    while(1){
        msg_receive(&msg);
        switch(msg.type){
            case NG_NETAPI_MSG_TYPE_SND:

                old_pkt = (ng_pktsnip_t *) msg.content.ptr;

                // Set Loopback as src in IP - Layer
                memcpy(&(((ng_ipv6_hdr_t*) old_pkt->data)->src), &src_addr, sizeof(src_addr));

                // Calculate Checksum for TCP
                ng_tcp_calc_csum(old_pkt->next, old_pkt);

                // Copy Headers
                ip6_len = old_pkt->size;
                tcp_len = old_pkt->next->size;
                memcpy(&ip6_hdr, old_pkt->data,       ip6_len);
                memcpy(&tcp_hdr, old_pkt->next->data, tcp_len);

                if(old_pkt->next->next != NULL){
                    pay_len = old_pkt->next->next->size;
                    memcpy(buf, old_pkt->next->next->data, pay_len);
                }else{
                    pay_len = 0;
                }

                // Remove old Paket
                ng_pktbuf_release(old_pkt);

                // Dump Paket
                printf("--- Fake-IP-Layer: Begin pkt Dump ---\n");
                /*
                printf("\n--- IPv6-Header --\n");
                ng_ipv6_hdr_print(&ip6_hdr);
                */
                printf("\n--- TCP-Header --\n");
                ng_tcp_hdr_print(&tcp_hdr);
                /*
                printf("--- Payload --\n");
                if(pay_len){
                    printf("Dump Payload in Hex(%d bytes): ", pay_len);
                    for(size_t i=0; i<pay_len; i++){
                        if(i%4 == 0){
                            printf("\n");
                        }
                        printf("%02x ",buf[i]);
                    }
                }else{
                    printf("There was no Payload\n");
                }
                
                printf("\n--- Fake-IP-Layer: End pkt Dump ---\n\n");
                */
                // Build new Paket, reverse Order
                new_pkt = NULL;
                new_pkt = ng_pktbuf_add(new_pkt, &ip6_hdr, ip6_len, NG_NETTYPE_IPV6);
                new_pkt = ng_pktbuf_add(new_pkt, &tcp_hdr, tcp_len, NG_NETTYPE_TCP);

                if(pay_len){
                    new_pkt = ng_pktbuf_add(new_pkt, buf, pay_len, NG_NETTYPE_UNDEF);
                }

                // Pass Message up the Network Stack
                ng_netapi_receive(tcp->pid, new_pkt);
                break;
                
            default:
                DEBUG("Unexpected Message Type, discading Message.\n");
                break;
        }
    }
}


int main(void)
{
    msg_t msg;
    main_pid = thread_getpid();
    /*
    // Startup "Fake IP6"-Thread
    thread_create( _stack_fake_ip6
                 , sizeof(_stack_fake_ip6)
                 , THREAD_PRIORITY_MAIN -2
                 , CREATE_STACKTEST
                 , fake_ip6
                 , NULL
                 , "fake_ip6"
                 );

    // Startup "Server"-Thread
    thread_create( _stack_srv
                 , sizeof(_stack_srv)
                 , THREAD_PRIORITY_MAIN -2
                 , CREATE_STACKTEST
                 , srv
                 , NULL
                 , "srv"
                 );
    */
    // Startup "Client"-Thread
    thread_create( _stack_cli
                 , sizeof(_stack_cli)
                 , THREAD_PRIORITY_MAIN -2
                 , CREATE_STACKTEST
                 , cli
                 , NULL
                 , "cli"
                 );

    // Wait for both Threads to terminate(send messages)
    msg_receive(&msg);
    //msg_receive(&msg);
    
    while(1){
    }

    exit(0);
}
