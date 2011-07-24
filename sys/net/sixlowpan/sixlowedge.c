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

char serial_reader_stack[READER_STACK_SIZE];
uint16_t serial_reader_pid;

abr_cache_t *abr_info;
uint16_t abro_version;
uint8_t border_out_buf[BORDER_BUFFER_SIZE];
uint8_t border_in_buf[BORDER_BUFFER_SIZE];

flowcontrol_stat_t slwin_stat;
sem_t connection_established;
int16_t synack_seqnum = -1;
int readpacket(uint8_t *packet_buf, int size);
uint16_t get_next_abro_version();
void init_border_router_info(ipv6_addr_t *abr_addr);
uint8_t abr_info_add_context(lowpan_context_t *context);

void serial_reader_f(void);
void flowcontrol_deliver_from_uart(border_packet_t *packet, int len);

void slwin_init() {
    int i;
    
    slwin_stat.send_win_not_full = sem_init(BORDER_SWS);
    for(i = 0; i < BORDER_SWS; i++) {
        slwin_stat.send_win[i].frame_len = 0;
    }
    memset(&slwin_stat.send_win,0, sizeof(struct send_slot) * BORDER_SWS);
        
    for(i = 0; i < BORDER_RWS; i++) {
        slwin_stat.recv_win[i].received = 0;
        slwin_stat.recv_win[i].frame_len = 0;
    }
    memset(&slwin_stat.recv_win,0, sizeof(struct recv_slot) * BORDER_RWS);
}

uint16_t get_next_abro_version() {
    abro_version = serial_add16(abro_version, 1);
    return abro_version;
}

ipv6_addr_t init_threeway_handshake() {
    border_syn_packet_t *syn;
    msg_t m;
    m.content.ptr = NULL;
    msg_send(&m,serial_reader_pid,1);
    while(1) {
        msg_receive(&m);
        printf("INFO: SYN received.\n");
        
        syn = (border_syn_packet_t *)m.content.ptr;
        border_conf_header_t *synack = (border_conf_header_t *)border_out_buf;
        ipv6_addr_t addr;
        memcpy(&addr, &(syn->addr), sizeof (ipv6_addr_t));
        
        slwin_stat.next_exp = syn->next_seq_num;
        slwin_stat.last_frame = syn->next_exp - 1;
        slwin_stat.last_ack = slwin_stat.last_frame;
        
        synack->reserved = 0;
        synack->type = BORDER_PACKET_CONF_TYPE;
        synack->conftype = BORDER_CONF_SYNACK;
        
        printf("Send SYNACK.\n");
        flowcontrol_send_over_uart((border_packet_t *)synack,sizeof (border_conf_header_t));
        
        synack_seqnum = synack->seq_num;
        
        return addr;
    }
}

uint8_t border_initialize(transceiver_type_t trans,ipv6_addr_t *border_router_addr) {
    ipv6_addr_t addr;
    
    slwin_init();
    
    serial_reader_pid = thread_create(
            serial_reader_stack, READER_STACK_SIZE,
            PRIORITY_MAIN-1, CREATE_STACKTEST,
            serial_reader_f, "serial_reader");
    
    if (border_router_addr == NULL) {
        border_router_addr = &addr;
        
        addr = init_threeway_handshake();
    }
    
    /* only allow addresses generated accoding to 
     * RFC 4944 (Section 6) & RFC 2464 (Section 4) from short address 
     * -- for now
     */
    if (    border_router_addr->uint16[4] != HTONS(IEEE_802154_PAN_ID ^ 0x0200) ||
            border_router_addr->uint16[5] != HTONS(0x00FF) ||
            border_router_addr->uint16[6] != HTONS(0xFE00)
        ) {
        return SIXLOWERROR_ADDRESS;
    }
    
    // radio-address is 8-bit so this must be tested extra
    if (border_router_addr->uint8[14] != 0) {
        return SIXLOWERROR_ADDRESS;
    }
    
    sixlowpan_init(trans,border_router_addr->uint8[15],1);
    
    init_border_router_info(border_router_addr);
    
    ipv6_init_iface_as_router();
    
    
    return SUCCESS;
}

lowpan_context_t *border_define_context(uint8_t cid, ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime) {
    lowpan_context_t *context;
    
    mutex_lock(&lowpan_context_mutex);
    context = lowpan_context_update(cid, prefix, prefix_len, OPT_6CO_FLAG_C_VALUE_SET, lifetime);
    abr_info_add_context(context);
    mutex_unlock(&lowpan_context_mutex,0);
    return context;
}

lowpan_context_t *border_alloc_context(ipv6_addr_t *prefix, uint8_t prefix_len, uint16_t lifetime) {
    lowpan_context_t *context = lowpan_context_lookup(prefix);
    
    if (context != NULL && context->length == prefix_len) {
        context = border_define_context(context->num, prefix, prefix_len, lifetime);
    }
    
    context = NULL;
    for (int i = 0; i < LOWPAN_CONTEXT_MAX; i++) {
        if (lowpan_context_num_lookup(i) != NULL) {
            context = border_define_context(i, prefix, prefix_len, lifetime);
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

void init_border_router_info(ipv6_addr_t *abr_addr) {
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
    
    context = border_define_context(0, &prefix, 64, 5);  // has to be reset some time later
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
        if ((byte_ptr - packet_buf) > BORDER_BUFFER_SIZE) {
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

void serial_reader_f(void) {
    int main_pid = 0;
    int bytes;
    msg_t m;
    border_packet_t *uart_buf;
    
    posix_open(uart0_handler_pid, 0);
    
    msg_receive(&m);
    main_pid = m.sender_pid;
    
    while(1) {
        posix_open(uart0_handler_pid, 0);
        bytes = readpacket(border_in_buf, BORDER_BUFFER_SIZE);
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
        
        uart_buf = (border_packet_t*)border_in_buf;
        if (uart_buf->reserved == 0) {
            if (uart_buf->type == BORDER_PACKET_CONF_TYPE) {
                border_conf_header_t *conf_packet = (border_conf_header_t*)border_in_buf;
                if (conf_packet->conftype == BORDER_CONF_SYN) {
                    m.content.ptr = (char *)conf_packet;
                    msg_send(&m, main_pid, 1);
                    continue;
                }
            }
            flowcontrol_deliver_from_uart(uart_buf, bytes);
        }
    }
}

int set_timeout(vtimer_t *timeout, long useconds, void *args);

void timeout_callback (void *args) {
    uint8_t seq_num = *((uint8_t *)args);
    struct send_slot *slot = &(slwin_stat.send_win[seq_num % BORDER_SWS]);
    
    if (seq_num == ((border_packet_t *)(slot->frame))->seq_num) {
        writepacket(slot->frame,slot->frame_len);
        
        if (set_timeout(&slot->timeout, BORDER_SL_TIMEOUT, args) != 0) {
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

void demultiplex(border_packet_t *packet, int len) {
    switch (packet->type) {
        case (BORDER_PACKET_RAW_TYPE):{
            fputs(((char *)packet) + sizeof (border_packet_t), stdin);
            break;
        }
        case (BORDER_PACKET_L3_TYPE):{
            border_l3_header_t *l3_header_buf = (border_l3_header_t *)packet;
            switch (l3_header_buf->ethertype) {
                case (BORDER_ETHERTYPE_IPV6):{
                    printf("INFO: IPv6-Packet received\n");
                    struct ipv6_hdr_t *ipv6_buf = (struct ipv6_hdr_t *)(((unsigned char *)packet) + sizeof (border_l3_header_t));
                    border_send_ipv6_over_lowpan(ipv6_buf, 1, 1);
                    break;
                }
                default:
                    printf("INFO: Unknown ethertype %04x\n", l3_header_buf->ethertype);
                    break;
            }
            break;
        }
        case (BORDER_PACKET_CONF_TYPE):{
            border_conf_header_t *conf_header_buf = (border_conf_header_t *)packet;
            switch (conf_header_buf->conftype) {
                case (BORDER_CONF_SYNACK):{
                    printf("INFO: SYNACK-Packet %d received, "
                            "but nothing is implemented yet for this case.\n",
                            conf_header_buf->seq_num);
                    break;
                }
                case (BORDER_CONF_CONTEXT):{
                    border_context_packet_t *context = (border_context_packet_t *)packet;
                    ipv6_addr_t target_addr;
                    printf("INFO: Context packet (%d) received\n",
                            conf_header_buf->seq_num);
                    ipv6_set_all_nds_mcast_addr(&target_addr);
                    lowpan_context_update(
                            context->context.cid, 
                            &context->context.prefix, 
                            context->context.length, 
                            context->context.comp,
                            context->context.lifetime
                        );
                    // Send router advertisement
                    break;
                }
                case (BORDER_CONF_IPADDR):{
                    border_addr_packet_t *addr_packet = (border_addr_packet_t *)packet;
                    
                    printf("INFO: Address packet (%d) received.\n",
                            conf_header_buf->seq_num);
                    // add address
                }
                default:
                    printf("INFO: Unknown conftype %02x\n", conf_header_buf->conftype);
                    break;
            }
            break;
        }
        default:
            printf("INFO: Unknown border packet type %02x\n", packet->type);
            break;
    }
}

void border_send_ack(uint8_t seq_num) {
    border_packet_t *packet = (border_packet_t *)border_out_buf;
    packet->reserved = 0;
    packet->type = BORDER_PACKET_ACK_TYPE;
    packet->seq_num = seq_num;
    writepacket((uint8_t *)packet, sizeof (border_packet_t));
}

void flowcontrol_deliver_from_uart(border_packet_t *packet, int len) {
    if (packet->type == BORDER_PACKET_ACK_TYPE) {
        printf("INFO: ACK %d received\n", packet->seq_num);
        if (in_window(packet->seq_num, slwin_stat.last_ack+1, slwin_stat.last_frame)) {
            if (synack_seqnum == packet->seq_num) {
                synack_seqnum = -1;
                sem_signal(&connection_established);
            }
            do {
                struct send_slot *slot;
                slot = &(slwin_stat.send_win[++slwin_stat.last_ack % BORDER_SWS]);
                vtimer_remove(&slot->timeout);
                memset(&slot->frame,0,BORDER_BUFFER_SIZE);
                sem_signal(&slwin_stat.send_win_not_full);
            } while (slwin_stat.last_ack != packet->seq_num);
        }
    } else {
        struct recv_slot *slot;
        
        slot = &(slwin_stat.recv_win[packet->seq_num % BORDER_RWS]);
        if (    !in_window(packet->seq_num, 
                slwin_stat.next_exp, 
                slwin_stat.next_exp + BORDER_RWS - 1)) {
            return;
        }
        
        memcpy(slot->frame, (uint8_t *)packet, len);
        slot->received = 1;
        
        if (packet->seq_num == slwin_stat.next_exp) {
            while (slot->received) {
                demultiplex((border_packet_t *)slot->frame, slot->frame_len);
                memset(&slot->frame,0,BORDER_BUFFER_SIZE);
                slot->received = 0;
                slot = &slwin_stat.recv_win[++(slwin_stat.next_exp) % BORDER_RWS];
            }
        }
        
        border_send_ack(slwin_stat.next_exp - 1);
    }
}

void flowcontrol_send_over_uart(border_packet_t *packet, int len) {
    struct send_slot *slot;
    uint8_t args[] = {packet->seq_num};
    
    sem_wait(&(slwin_stat.send_win_not_full));
    packet->seq_num = ++slwin_stat.last_frame;
    slot = &(slwin_stat.send_win[packet->seq_num % BORDER_SWS]);
    memcpy(slot->frame, (uint8_t *)packet, len);
    slot->frame_len = len;
    if (set_timeout(&slot->timeout, BORDER_SL_TIMEOUT, (void *)args) != 0) {
        printf("ERROR: Error invoking timeout timer\n");
        return;
    }
    writepacket((uint8_t *)packet, len);
}

void multiplex_send_ipv6_over_uart(struct ipv6_hdr_t *packet) {
    border_l3_header_t *serial_buf;
    
    serial_buf = (border_l3_header_t *)border_out_buf;
    serial_buf->reserved = 0;
    serial_buf->type = BORDER_PACKET_L3_TYPE;
    serial_buf->ethertype = BORDER_ETHERTYPE_IPV6;
    memcpy(border_out_buf+sizeof (border_l3_header_t), packet, IPV6_HDR_LEN + packet->length);
    writepacket(border_out_buf, sizeof (border_l3_header_t) + IPV6_HDR_LEN + packet->length);
}

void border_send_ipv6_over_lowpan(struct ipv6_hdr_t *packet, uint8_t aro_flag, uint8_t sixco_flag) {
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
        
        // border router should not send neighbor and router solicitations in lowpan
    }*/
    
    lowpan_init((ieee_802154_long_t*)&(packet->destaddr.uint16[4]), (uint8_t*)packet);
    
}

void border_process_lowpan(void) {
    msg_t m;
    struct ipv6_hdr_t *ipv6_buf;
    
    while(1) {
        msg_receive(&m);
        ipv6_buf = (struct ipv6_hdr_t *)m.content.ptr;
        // TODO: Bei ICMPv6-Paketen entsprechende LoWPAN-Optionen verarbeiten und entfernen
        multiplex_send_ipv6_over_uart(ipv6_buf);
    }
}

abr_cache_t *get_border_router_info() {
    return abr_info;
}
