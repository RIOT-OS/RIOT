#include <stdio.h>
#include <string.h>

#include <board_uart0.h>

#include "flowcontrol.h"
#include "sixlowpan.h"
#include "sixlownd.h"
#include "sixlowborder.h"
#include "sixlowerror.h"

#include "bordermultiplex.h"

#define DC3         0x0D
#define END         0xC0
#define ESC         0xDB
#define END_ESC     0xDC
#define ESC_ESC     0xDD
#define DC3_ESC     0xDE

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
                    mutex_unlock(&lowpan_context_mutex,0);
                    abr_add_context(context->context.version, &abr_addr, context->context.cid);
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

int readpacket(uint8_t *packet_buf, size_t size) {
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
    
    uart0_putc(END);
    
    return (byte_ptr - packet_buf);
}
