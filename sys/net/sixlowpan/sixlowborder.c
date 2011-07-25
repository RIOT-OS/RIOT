#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <mutex.h>
#include <thread.h>
#include <msg.h>

#include <posix_io.h>
#include <board_uart0.h>
#include "ieee802154_frame.h"
#include "flowcontrol.h"
#include "sixlowborder.h"
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

uint8_t serial_out_buf[BORDER_BUFFER_SIZE];
uint8_t serial_in_buf[BORDER_BUFFER_SIZE];

void serial_reader_f(void);

uint8_t border_initialize(transceiver_type_t trans,ipv6_addr_t *border_router_addr) {
    ipv6_addr_t addr;
    
    serial_reader_pid = thread_create(
            serial_reader_stack, READER_STACK_SIZE,
            PRIORITY_MAIN-1, CREATE_STACKTEST,
            serial_reader_f, "serial_reader");
    
    if (border_router_addr == NULL) {
        border_router_addr = &addr;
        
        addr = flowcontrol_init();
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
    
    ipv6_init_iface_as_router();
    
    
    return SUCCESS;
}

uint16_t border_get_serial_reader() {
    return serial_reader_pid;
}

uint8_t *get_serial_out_buffer(int offset) {
    if (offset > BUFFER_SIZE) {
        return NULL;
    }
    
    return &(serial_out_buf[offset]);
}

uint8_t *get_serial_in_buffer(int offset) {
    if (offset > BUFFER_SIZE) {
        return NULL;
    }
    
    return &(serial_in_buf[offset]);
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
                uart0_putc(ESC);
                break;
            }
            case(END):{
                *byte_ptr = END_ESC;
                uart0_putc(ESC);
                break;
            }
            case(ESC):{
                *byte_ptr = ESC_ESC;
                uart0_putc(ESC);
                break;
            }
            default:{
                break;
            }
        }
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
        bytes = readpacket(get_serial_in_buffer(0), BORDER_BUFFER_SIZE);
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
        
        uart_buf = (border_packet_t*)get_serial_in_buffer(0);
        if (uart_buf->reserved == 0) {
            if (uart_buf->type == BORDER_PACKET_CONF_TYPE) {
                border_conf_header_t *conf_packet = (border_conf_header_t*)uart_buf;
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
                    struct ipv6_hdr_t *ipv6_buf = (struct ipv6_hdr_t *)(((unsigned char *)packet) + sizeof (border_l3_header_t));
                    border_send_ipv6_over_lowpan(ipv6_buf, 1, 1);
                    break;
                }
                default:
                    printf("ERROR: Unknown ethertype %02x\n", l3_header_buf->ethertype);
                    break;
            }
            break;
        }
        case (BORDER_PACKET_CONF_TYPE):{
            border_conf_header_t *conf_header_buf = (border_conf_header_t *)packet;
            switch (conf_header_buf->conftype) {
                case (BORDER_CONF_CONTEXT):{
                    border_context_packet_t *context = (border_context_packet_t *)packet;
                    ipv6_addr_t target_addr;
                    ipv6_set_all_nds_mcast_addr(&target_addr);
                    mutex_lock(&lowpan_context_mutex);
                    lowpan_context_update(
                            context->context.cid, 
                            &context->context.prefix, 
                            context->context.length, 
                            context->context.comp,
                            context->context.lifetime
                        );
                    // abr stuff
                    mutex_unlock(&lowpan_context_mutex,0);
                    // Send router advertisement
                    break;
                }
                case (BORDER_CONF_IPADDR):{
                    //border_addr_packet_t *addr_packet = (border_addr_packet_t *)packet;
                    // add address
                    break;
                }
                default:
                    printf("ERROR: Unknown conftype %02x\n", conf_header_buf->conftype);
                    break;
            }
            break;
        }
        default:
            printf("ERROR: Unknown border packet type %02x\n", packet->type);
            break;
    }
}

void multiplex_send_ipv6_over_uart(struct ipv6_hdr_t *packet) {
    border_l3_header_t *serial_buf;
    
    serial_buf = (border_l3_header_t *)get_serial_out_buffer(0);
    serial_buf->reserved = 0;
    serial_buf->type = BORDER_PACKET_L3_TYPE;
    serial_buf->ethertype = BORDER_ETHERTYPE_IPV6;
    memcpy(get_serial_in_buffer(0)+sizeof (border_l3_header_t), packet, IPV6_HDR_LEN + packet->length);
    
    flowcontrol_send_over_uart(
            (border_packet_t *) serial_buf,
            sizeof (border_l3_header_t)
        );
}

void multiplex_send_addr_over_uart(ipv6_addr_t *addr) {
    border_addr_packet_t *serial_buf;
    
    serial_buf = (border_addr_packet_t *)get_serial_in_buffer(0);
    serial_buf->reserved = 0;
    serial_buf->type = BORDER_PACKET_CONF_TYPE;
    serial_buf->conftype = BORDER_CONF_IPADDR;
    memcpy(
            &serial_buf->addr,
            addr, 
            sizeof (ipv6_addr_t)
        );
    
    flowcontrol_send_over_uart(
            (border_packet_t *) serial_buf,
            sizeof (border_addr_packet_t)
        );
}

void border_send_ipv6_over_lowpan(struct ipv6_hdr_t *packet, uint8_t aro_flag, uint8_t sixco_flag) {
    uint16_t offset = IPV6_HDR_LEN+HTONS(packet->length);
    
    packet->flowlabel = HTONS(packet->flowlabel);
    packet->length = HTONS(packet->length);
    
    memset(buffer, 0, BUFFER_SIZE);
    memcpy(buffer+LL_HDR_LEN, packet, offset);
    
    if (packet->nextheader == PROTO_NUM_ICMPV6) {
        struct icmpv6_hdr_t *icmp_buf = (struct icmpv6_hdr_t *)(packet + IPV6_HDR_LEN);
        if (icmp_buf->type == ICMP_REDIRECT) {
            return;
        }
        // Here, other ICMPv6 message types for ND may follow.
    }
    
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
