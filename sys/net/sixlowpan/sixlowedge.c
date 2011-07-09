#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mutex.h>

#include <posix_io.h>
#include <board_uart0.h>
#include "ieee802154_frame.h"
#include "sixlowedge.h"
#include "sixlowip.h"
#include "sixlownd.h"
#include "serialnumber.h"
#include "sixlowerror.h"

#define DC3         0x0D
#define END         0xC0
#define ESC         0xDB
#define END_ESC     0xDC
#define ESC_ESC     0xDD
#define DC3_ESC     0xDE

abr_cache_t *abr_info;
uint16_t abro_version;
uint8_t edge_out_buf[EDGE_BUFFER_SIZE];
mutex_t edge_out_buf_mutex;

uint8_t edge_in_buf[EDGE_BUFFER_SIZE];
mutex_t edge_in_buf_mutex;

edge_packet_t *uart_buf;
edge_l3_header_t *l3_buf;

uint16_t get_next_abro_version();
void init_edge_router_info(ipv6_addr_t *abr_addr);
uint8_t abr_info_add_context(lowpan_context_t *context);

uint16_t get_next_abro_version() {
    abro_version = serial_add16(abro_version, 1);
    return abro_version;
}

uint8_t edge_initialize(transceiver_type_t trans,ipv6_addr_t *edge_router_addr) {
    /* only allow addresses generated accoding to 
     * RFC 4944 (Section 6) & RFC 2464 (Section 4) from short address 
     * -- for now
     */
    if (    edge_router_addr->uint16[4] != HTONS(IEEE_802154_PAN_ID ^ 0x0200) ||
            edge_router_addr->uint16[5] != HTONS(0x00FF) ||
            edge_router_addr->uint16[6] != HTONS(0xFE00)
        ) {
        return SIXLOWERROR_ADDRESS;
    }
    
    // radio-address is 8-bit so this must be tested extra
    if (edge_router_addr->uint8[14] != 0) {
        return SIXLOWERROR_ADDRESS;
    }
    
    posix_open(uart0_handler_pid, 0);
    
    mutex_init(&edge_in_buf_mutex);
    mutex_init(&edge_out_buf_mutex);
    
    sixlowpan_init(trans,edge_router_addr->uint8[15]);
    
    init_edge_router_info(edge_router_addr);
    
    ipv6_init_iface_as_router();
    
    return SUCCESS;
}

lowpan_context_t *edge_define_context(uint8_t cid, ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime) {
    lowpan_context_t *context;
    
    mutex_lock(&lowpan_context_mutex);
    context = lowpan_context_update(cid, prefix, prefix_len, OPT_6CO_FLAG_C_VALUE_SET, lifetime);
    abr_info_add_context(context);
    mutex_unlock(&lowpan_context_mutex,0);
    return context;
}

lowpan_context_t *edge_alloc_context(ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime) {
    lowpan_context_t *context = lowpan_context_lookup(prefix);
    
    if (context != NULL && context->length == prefix_len) {
        context = edge_define_context(context->num, prefix, prefix_len, lifetime);
    }
    
    context = NULL;
    for (int i = 0; i < LOWPAN_CONTEXT_MAX; i++) {
        if (lowpan_context_num_lookup(i) != NULL) {
            context = edge_define_context(i, prefix, prefix_len, lifetime);
        }
    }
    return context;
}

uint8_t abr_info_add_context(lowpan_context_t *context) {
    if (context == NULL) return SIXLOWERROR_NULLPTR;
    uint16_t abro_version = get_next_abro_version();
    int i;
    for (i = 0; i < abr_info->contexts_num; i++) {
        if (abr_info->contexts[i] == context->num) {
            abr_info->version = abro_version;
            return SUCCESS;
        }
    }
    
    if (abr_info->contexts_num == LOWPAN_CONTEXT_MAX) {
        return SIXLOWERROR_ARRAYFULL;
    }
    
    abr_info->contexts[abr_info->contexts_num++] = context->num;
    abr_info->version = abro_version;
    return SUCCESS;
}

void init_edge_router_info(ipv6_addr_t *abr_addr) {
    uint16_t abro_version = get_next_abro_version();
    ipv6_addr_t prefix;
    lowpan_context_t *context;
    
    if (abr_info == NULL)
        abr_info = abr_update_cache(abro_version,abr_addr,NULL,0);
    else
        abr_info = abr_update_cache(
                abro_version,
                abr_addr,
                abr_info->contexts,
                abr_info->contexts_num
            );
    
    ipv6_iface_add_addr(abr_addr,ADDR_STATE_PREFERRED,0,0,ADDR_TYPE_UNICAST);
    ipv6_set_prefix(&prefix, abr_addr);
    plist_add(&prefix, 64, OPT_PI_VLIFETIME_INFINITE,0,1,OPT_PI_FLAG_A);
    
    context = edge_define_context(0, &prefix, 64, 5);  // has to be reset some time later
}

int readpacket(uint8_t *packet_buf, int size) {
    uint8_t *line_buf_ptr = packet_buf;
    uint8_t byte = END+1;
    uint8_t esc = 0;
    
    while (byte != END) {
        byte = uart0_readc();
        
        if ( (line_buf_ptr - packet_buf) >= size-1) {
            return -SIXLOWERROR_ARRAYFULL;
        }

        if (byte == DC3) continue;
        
        if (esc) {
            switch (byte) {
                case(END_ESC):{
                    byte = END;
                    break;
                }
                case(ESC_ESC):{
                    byte = ESC;
                    break;
                }
                case(DC3_ESC):{
                    byte = DC3;
                    break;
                }
                default:
                    break;
            }
            
            esc = 0;
        }
                
        if (byte == ESC) {
            esc = 1;
            continue;
        }
        
        *line_buf_ptr++ = byte;
    }
    
    return (line_buf_ptr - packet_buf - 1);
}

void edge_process_uart(void) {
    int status;
    uint8_t packet_buf[EDGE_BUFFER_SIZE];
    while(1) {
        status = readpacket(packet_buf, EDGE_BUFFER_SIZE);
        if (status < 0) {
            switch (status) {
                case (-SIXLOWERROR_ARRAYFULL):{
                    printf("ERROR: (%s:%d) Array was full\n",__FILE__,__LINE__);
                    break;
                }
                default:{
                    printf("ERROR: unknown\n");
                    break;
                }
            }
            continue;
        }
        mutex_lock(&edge_in_buf_mutex);
        memcpy(edge_in_buf,packet_buf,status);
        uart_buf = (edge_packet_t*)edge_in_buf;
        if (uart_buf->reserved == 0) {
            switch (uart_buf->type) {
                case (EDGE_PACKET_ACK_TYPE):{
                    printf("INFO: ACK\n");
                    //edge_ack_package(uart_buf->seq_num);
                    break;
                }
                case (EDGE_PACKET_CONF_TYPE):{
                    printf("INFO: CONF\n");
                    // TODO
                    break;
                }
                case (EDGE_PACKET_L3_TYPE):{
                    l3_buf = (edge_l3_header_t*)uart_buf;
                    printf("INFO: L3-Packet\n");
                    // TODO
                    //edge_send_ack(l3_buf->seq_num);
                    
                    switch (l3_buf->ethertype) {
                        case(EDGE_ETHERTYPE_IPV6):{
                            printf("INFO: IPv6-Packet\n");
                            struct ipv6_hdr_t *ipv6_packet = (struct ipv6_hdr_t *)(edge_in_buf + sizeof (edge_l3_header_t));
                            edge_send_ipv6_over_lowpan(ipv6_packet,1,1);
                            break;
                        }
                        default:{
                        printf("ERROR: Unknown Layer-3-Type %04x\n",l3_buf->ethertype);
                            break;
                        }
                    }
                    break;
                }
                default:{
                    printf("ERROR: Unknown Packet-Type %d\n",uart_buf->type);
                    break;
                }
            }
        }
        mutex_unlock(&edge_in_buf_mutex,0);
    }
}

void edge_send_ipv6_over_lowpan(struct ipv6_hdr_t *packet, uint8_t aro_flag, uint8_t sixco_flag) {
    //abr_cache_t *msg_abr = NULL;
    uint16_t offset = IPV6_HDR_LEN+HTONS(packet->length);
    
    packet->flowlabel = HTONS(packet->flowlabel);
    packet->length = HTONS(packet->length);
    
    memset(buffer, 0, BUFFER_SIZE);
    memcpy(buffer+LL_HDR_LEN, packet, offset);
    
    /*
    if (ipv6_buf->nextheader == PROTO_NUM_ICMPV6) {
        icmp_buf = (icmpv6_hdr_t *)(ipv6_buf + IPV6_HDR_LEN);
        if (icmp_buf->type == ICMP_RTR_ADV) {
            if (abr_count > 0) {
                opt_abro_t *opt_abro_buf = (opt_abro_t *)(ipv6_buf + offset);
                msg_abr = abr_get_most_current();
                opt_abro_buf->type = OPT_ABRO_TYPE;
                opt_abro_buf->length = OPT_ABRO_LEN;
                opt_abro_buf->version = HTONS(msg_abr->version);
                opt_abro_buf->reserved = 0;
                memcpy(&(opt_abro_buf->addr), &(msg_abr->abr_addr), sizeof (ipv6_addr_t));
                ipv6->length += OPT_ABRO_HDR_LEN;
                offset += OPT_ARO_HDR_LEN;
            }
            
            if (sixco_flag) {
                // TODO: 6CO
            }
        }
        
        if (icmp_buf->type == ICMP_NBR_ADV && aro_flag) {
            if(aro == OPT_ARO){
                /* set aro option *//*
                opt_aro_t *opt_aro_buf = (opt_abro_t *)(ipv6_buf + offset);
                opt_aro_buf->type = OPT_ARO_TYPE;
                opt_aro_buf->length = OPT_ARO_LEN;
                opt_aro_buf->status = 0;    // TODO
                opt_aro_buf->reserved1 = 0;
                opt_aro_buf->reserved2 = 0;
                memcpy(&(opt_aro_buf->eui64), mac_get_eui(dst),8);
                
                ipv6->length += OPT_ARO_HDR_LEN;
                offset += OPT_ARO_HDR_LEN;
            }
        }
        
        // edge router should not send neighbor and router solicitations in lowpan
    }*/
    
    lowpan_init((ieee_802154_long_t*)&(packet->destaddr.uint16[4]), (uint8_t*)packet);
    
}

abr_cache_t *get_edge_router_info() {
    return abr_info;
}
