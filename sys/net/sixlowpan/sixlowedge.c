#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mutex.h>
#include <thread.h>
#include <msg.h>

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

#define READER_STACK_SIZE   512

char reader_stack[READER_STACK_SIZE];
uint16_t reader_pid;

abr_cache_t *abr_info;
uint16_t abro_version;
uint8_t edge_out_buf[EDGE_BUFFER_SIZE];
uint8_t edge_in_buf[EDGE_BUFFER_SIZE];

slwin_stat_t slwin_stat;
sem_t connection_established;
int16_t synack_seqnum = -1;
int readpacket(uint8_t *packet_buf, int size);
uint16_t get_next_abro_version();
void init_edge_router_info(ipv6_addr_t *abr_addr);
uint8_t abr_info_add_context(lowpan_context_t *context);

void edge_process_uart(void);
void edge_receive_from_uart(edge_packet_t *packet, int len);

void slwin_init() {
    int i;
    
    slwin_stat.send_win_not_full = sem_init(EDGE_SWS);
    for(i = 0; i < EDGE_SWS; i++) {
        slwin_stat.send_win[i].frame_len = 0;
    }
    memset(&slwin_stat.send_win,0, sizeof(struct send_slot) * EDGE_SWS);
        
    for(i = 0; i < EDGE_RWS; i++) {
        slwin_stat.recv_win[i].received = 0;
        slwin_stat.recv_win[i].frame_len = 0;
    }
    memset(&slwin_stat.recv_win,0, sizeof(struct recv_slot) * EDGE_RWS);
}

uint16_t get_next_abro_version() {
    abro_version = serial_add16(abro_version, 1);
    return abro_version;
}

ipv6_addr_t init_threeway_handshake() {
    edge_syn_packet_t *syn;
    msg_t m;
    m.content.ptr = NULL;
    msg_send(&m,reader_pid,1);
    while(1) {
        msg_receive(&m);
        printf("INFO: SYN received.\n");
        
        syn = (edge_syn_packet_t *)m.content.ptr;
        edge_conf_header_t *synack = (edge_conf_header_t *)edge_out_buf;
        ipv6_addr_t addr;
        memcpy(&addr, &(syn->addr), sizeof (ipv6_addr_t));
        
        slwin_stat.next_exp = syn->next_seq_num;
        slwin_stat.last_frame = syn->next_exp - 1;
        slwin_stat.last_ack = slwin_stat.last_frame;
        
        synack->reserved = 0;
        synack->type = EDGE_PACKET_CONF_TYPE;
        synack->conftype = EDGE_CONF_SYNACK;
        
        printf("Send SYNACK.\n");
        edge_send_over_uart((edge_packet_t *)synack,sizeof (edge_conf_header_t));
        
        synack_seqnum = synack->seq_num;
        
        return addr;
    }
}

uint8_t edge_initialize(transceiver_type_t trans,ipv6_addr_t *edge_router_addr) {
    ipv6_addr_t addr;
    
    slwin_init();
    
    reader_pid = thread_create(
            reader_stack, READER_STACK_SIZE,
            PRIORITY_MAIN-1, CREATE_STACKTEST,
            edge_process_uart, "edge_process_uart");
    
    if (edge_router_addr == NULL) {
        edge_router_addr = &addr;
        
        addr = init_threeway_handshake();
    }
    
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
    
    sixlowpan_init(trans,edge_router_addr->uint8[15],1);
    
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

int writepacket(uint8_t *packet_buf, size_t size) {
    uint8_t *byte_ptr = packet_buf;
    
    while ((byte_ptr - packet_buf) < size) {
        if ((byte_ptr - packet_buf) > EDGE_BUFFER_SIZE) {
            return -1;
        }
        printf("%02x ",*byte_ptr);
        switch (*byte_ptr) {
            case(DC3):{
                *byte_ptr = DC3_ESC;
                goto write_esc;
            }
            case(END):{
                *byte_ptr = END_ESC;
                goto write_esc;
            }
            case(ESC):{
                *byte_ptr = ESC_ESC;
                goto write_esc;
            }
            default:{
                goto write_byte;
            }
        }
        write_esc:
            uart0_putc(ESC);
        write_byte:
            uart0_putc(*byte_ptr);
        byte_ptr++;
    }
    
    printf("\n");
    uart0_putc(END);
    
    return (byte_ptr - packet_buf);
}

void edge_process_uart(void) {
    int main_pid = 0;
    int bytes;
    msg_t m;
    edge_packet_t *uart_buf;
    
    posix_open(uart0_handler_pid, 0);
    
    msg_receive(&m);
    main_pid = m.sender_pid;
    
    while(1) {
        posix_open(uart0_handler_pid, 0);
        bytes = readpacket(edge_in_buf, EDGE_BUFFER_SIZE);
        if (bytes < 0) {
            switch (bytes) {
                case (-SIXLOWERROR_ARRAYFULL):{
                    printf("ERROR: Array was full\n");
                    break;
                }
                default:{
                    printf("ERROR: unknown\n");
                    break;
                }
            }
            continue;
        }
        
        uart_buf = (edge_packet_t*)edge_in_buf;
        if (uart_buf->reserved == 0) {
            if (uart_buf->type == EDGE_PACKET_CONF_TYPE) {
                edge_conf_header_t *conf_packet = (edge_conf_header_t*)edge_in_buf;
                if (conf_packet->conftype == EDGE_CONF_SYN) {
                    m.content.ptr = (char *)conf_packet;
                    msg_send(&m, main_pid, 1);
                    continue;
                }
            }
            edge_receive_from_uart(uart_buf, bytes);
        }
    }
}

void timeout_callback (void *args) {
    uint8_t seq_num = *((uint8_t *)args);
    struct send_slot *slot = &(slwin_stat.send_win[seq_num % EDGE_SWS]);
    
    if (seq_num == ((edge_packet_t *)(slot->frame))->seq_num) {
        writepacket(slot->frame,slot->frame_len);
        
        if (set_timeout(&slot->timeout, EDGE_SL_TIMEOUT, args) != 0) {
            printf("ERROR: Error invoking timeout timer\n");
            return;
        }
    } else {
        return;
    }
}

int set_timeout(vtimer_t *timeout, long useconds, void *args) {
    timex_t interval;
    interval.seconds = useconds / 1000000;
    interval.nanoseconds = (useconds % 1000000) * 1000;
    
    return vtimer_set_cb(timeout, interval, timeout_callback, args);
}

static int in_window(uint8_t seq_num, uint8_t min, uint8_t max) {
    uint8_t pos = seq_num - min;
    uint8_t maxpos = max - min + 1;
    return pos < maxpos;
}

void edge_demultiplex(edge_packet_t *packet, int len) {
    switch (packet->type) {
        case (EDGE_PACKET_RAW_TYPE):{
            fputs(((char *)packet) + sizeof (edge_packet_t), stdin);
            break;
        }
        case (EDGE_PACKET_L3_TYPE):{
            edge_l3_header_t *l3_header_buf = (edge_l3_header_t *)packet;
            switch (l3_header_buf->ethertype) {
                case (EDGE_ETHERTYPE_IPV6):{
                    printf("INFO: IPv6-Packet received\n");
                    struct ipv6_hdr_t *ipv6_buf = (struct ipv6_hdr_t *)(((unsigned char *)packet) + sizeof (edge_l3_header_t));
                    edge_send_ipv6_over_lowpan(ipv6_buf, 1, 1);
                    break;
                }
                default:
                    printf("INFO: Unknown ethertype %04x\n", l3_header_buf->ethertype);
                    break;
            }
            break;
        }
        case (EDGE_PACKET_CONF_TYPE):{
            edge_conf_header_t *conf_header_buf = (edge_conf_header_t *)packet;
            switch (conf_header_buf->conftype) {
                default:
                    printf("INFO: Unknown conftype %02x\n", conf_header_buf->conftype);
                    break;
            }
            break;
        }
        default:
            printf("INFO: Unknown edge packet type %02x\n", packet->type);
            break;
    }
}

void edge_send_ack(uint8_t seq_num) {
    edge_packet_t *packet = (edge_packet_t *)edge_out_buf;
    packet->reserved = 0;
    packet->type = EDGE_PACKET_ACK_TYPE;
    packet->seq_num = seq_num;
    writepacket((uint8_t *)packet, sizeof (edge_packet_t));
}

void edge_receive_from_uart(edge_packet_t *packet, int len) {
    if (packet->type == EDGE_PACKET_ACK_TYPE) {
        printf("INFO: ACK %d received\n", packet->seq_num);
        if (in_window(packet->seq_num, slwin_stat.last_ack+1, slwin_stat.last_frame)) {
            if (synack_seqnum == packet->seq_num) {
                synack_seqnum = -1;
                sem_signal(&connection_established);
            }
            do {
                struct send_slot *slot;
                slot = &(slwin_stat.send_win[++slwin_stat.last_ack % EDGE_SWS]);
                vtimer_remove(&slot->timeout);
                memset(&slot->frame,0,EDGE_BUFFER_SIZE);
                sem_signal(&slwin_stat.send_win_not_full);
            } while (slwin_stat.last_ack != packet->seq_num);
        }
    } else {
        struct recv_slot *slot;
        
        slot = &(slwin_stat.recv_win[packet->seq_num % EDGE_RWS]);
        if (    !in_window(packet->seq_num, 
                slwin_stat.next_exp, 
                slwin_stat.next_exp + EDGE_RWS - 1)) {
            return;
        }
        
        memcpy(slot->frame, (uint8_t *)packet, len);
        slot->received = 1;
        
        if (packet->seq_num == slwin_stat.next_exp) {
            while (slot->received) {
                edge_demultiplex((edge_packet_t *)slot->frame, slot->frame_len);
                memset(&slot->frame,0,EDGE_BUFFER_SIZE);
                slot->received = 0;
                slot = &slwin_stat.recv_win[++(slwin_stat.next_exp) % EDGE_RWS];
            }
        }
        
        edge_send_ack(slwin_stat.next_exp - 1);
    }
}

void edge_send_over_uart(edge_packet_t *packet, int len) {
    struct send_slot *slot;
    uint8_t args[] = {packet->seq_num};
    
    sem_wait(&(slwin_stat.send_win_not_full));
    packet->seq_num = ++slwin_stat.last_frame;
    slot = &(slwin_stat.send_win[packet->seq_num % EDGE_SWS]);
    memcpy(slot->frame, (uint8_t *)packet, len);
    slot->frame_len = len;
    if (set_timeout(&slot->timeout, EDGE_SL_TIMEOUT, (void *)args) != 0) {
        printf("ERROR: Error invoking timeout timer\n");
        return;
    }
    writepacket((uint8_t *)packet, len);
}

void edge_send_ipv6_over_uart(struct ipv6_hdr_t *packet) {
    edge_l3_header_t *serial_buf;
    
    serial_buf = (edge_l3_header_t *)edge_out_buf;
    serial_buf->reserved = 0;
    serial_buf->type = EDGE_PACKET_L3_TYPE;
    serial_buf->ethertype = EDGE_ETHERTYPE_IPV6;
    memcpy(edge_out_buf+sizeof (edge_l3_header_t), packet, IPV6_HDR_LEN + packet->length);
    writepacket(edge_out_buf, sizeof (edge_l3_header_t) + IPV6_HDR_LEN + packet->length);
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

void edge_process_lowpan(void) {
    msg_t m;
    struct ipv6_hdr_t *ipv6_buf;
    
    while(1) {
        msg_receive(&m);
        ipv6_buf = (struct ipv6_hdr_t *)m.content.ptr;
        // TODO: Bei ICMPv6-Paketen entsprechende LoWPAN-Optionen verarbeiten und entfernen
        edge_send_ipv6_over_uart(ipv6_buf);
    }
}

abr_cache_t *get_edge_router_info() {
    return abr_info;
}
