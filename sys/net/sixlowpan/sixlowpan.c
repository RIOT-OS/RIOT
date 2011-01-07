#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "thread.h"
#include "msg.h"
#include "sixlowmac.h"
#include "sixlowpan.h"
#include "transceiver.h"
#include "ieee802154_frame.h"

uint16_t packet_length;
uint16_t tag;
uint8_t packet_size = 0;
uint8_t header_size = 0;
uint8_t max_frame = 0;
uint8_t max_frag_initial = 0;
uint8_t position;
uint8_t max_frag;

uint8_t frag_size;
uint8_t *reas_buf;
uint8_t byte_offset;
uint8_t first_frag = 0;

static struct ipv6_hdr_t *ipv6_buf;
unsigned int ip_process_pid;

iface_t iface;

char ip_process_buf[IP_PROCESS_STACKSIZE];

/* deliver packet to mac*/
void output(uint8_t *addr, uint8_t *data){
    packet_size = (uint8_t)packet_length;
    /* check if packet needs to be fragmented */
    if(packet_size + header_size > PAYLOAD_SIZE - IEEE_802154_MAX_HDR_LEN){
        printf("INFO: packet is to large, fragmentation started. size:"
               " %hu byte\n", packet_length);

        uint8_t *ptr = data;
        uint8_t fragbuf[packet_size + header_size];
        uint8_t remaining;
        uint8_t i = 2;
        /* first fragment */

        max_frame = PAYLOAD_SIZE - IEEE_802154_MAX_HDR_LEN;
        max_frag_initial = ((max_frame - 4 - header_size) / 8) * 8;
       
        memcpy(fragbuf + 4, ptr, max_frag_initial); 
        
        fragbuf[0] = (((0xc0 << 8) | packet_length) >> 8) & 0xff;
        fragbuf[1] = ((0xc0 << 8) | packet_length) & 0xff;
        fragbuf[2] = (tag >> 8) & 0xff;
        fragbuf[3] = tag & 0xff;  

        send_ieee802154_frame(addr,(uint8_t*)&fragbuf, max_frag_initial + header_size + 4);
        printf("      frag 1 size: %d byte\n", max_frag_initial);
        /* subsequent fragments */
        position = max_frag_initial;
        max_frag = ((max_frame - 5) / 8) * 8;

        ptr += position;

        while(packet_size - position > max_frame - 5){
            memset(&fragbuf,0,packet_size + header_size);
            memcpy(fragbuf + 5, ptr, max_frag);
        
            fragbuf[0] = (((0xe0 << 8) | packet_length) >> 8) & 0xff;
            fragbuf[1] = ((0xe0 << 8) | packet_length) & 0xff;
            fragbuf[2] = (tag >> 8) & 0xff; 
            fragbuf[3] = tag & 0xff;
            fragbuf[4] = position / 8;
            
            send_ieee802154_frame(addr,(uint8_t*)&fragbuf, max_frag + 5);
            printf("      frag %d size: %d byte\n",i , max_frag);
            ptr += position;
            position += max_frag;
            i++;      
        }
       
        remaining = packet_size - position;
        memset(&fragbuf,0,packet_size + header_size);
        memcpy(fragbuf + 5, ptr, remaining); 
        
        fragbuf[0] = (((0xe0 << 8) | packet_length) >> 8) & 0xff;
        fragbuf[1] = ((0xe0 << 8) | packet_length) & 0xff;
        fragbuf[2] = (tag >> 8) & 0xff;
        fragbuf[3] = tag & 0xff;
        fragbuf[4] = position / 8;

        send_ieee802154_frame(addr, (uint8_t*)&fragbuf, remaining + 5);
        printf("      frag %d size: %d byte\n",i , remaining);   
    } else {
        send_ieee802154_frame(addr, data, packet_size);
    } 

    tag++;
}

void input(uint8_t *data, uint8_t length){
    /* check if packet is fragmented */
    msg m;
    uint8_t hdr_length = 0;
    uint8_t datagram_offset = 0;    
    uint16_t datagram_size = 0;
    uint16_t datagram_tag = 0;
    /* check first 5-bit*/
    switch(data[0] & 0xf8) {
        /* first fragment */
        case(0xc0):{
            first_frag = 1;
            datagram_offset = 0;
            /* get 11-bit from first 2 byte*/
            datagram_size = (((uint16_t)(data[0] << 8)) | data[1]) & 0x07ff;
            /* get 16-bit datagram tag */
            datagram_tag = (((uint16_t)(data[2] << 8)) | data[3]);
            /* discard fragment header */
            hdr_length += 4;
            /* set global variable frag_size to size of decompr. packet*/
            frag_size = datagram_size;
            /* create reassembly buffer */
            reas_buf = (uint8_t*)malloc(frag_size * sizeof(uint8_t));
            /* check dispatch byte */
            switch(data[hdr_length]) {
                case(DISPATCH_IPV6):{
                    break;
                }
                default:
                    break;
            }                
            memcpy(reas_buf, data + hdr_length, length - 4);
            break;
        }
        /* subsequent fragment */
        case(0xe0):{
            if(first_frag == 0){
                printf("ERROR: first fragment not received\n");
                break;
            }

            /* get 11-bit from first 2 byte*/
            datagram_size = (((uint16_t)(data[0] << 8)) | data[1]) & 0x07ff;
            /* get 16-bit datagram tag */
            datagram_tag = (((uint16_t)(data[2] << 8)) | data[3]);
            /* get 8-bit datagram offset */
            datagram_offset = data[4];
            /* discard framentation header */
            hdr_length += 5;
           
            frag_size = length - hdr_length;
            byte_offset = datagram_offset * 8;
            if((frag_size % 8) != 0){
                printf("ERROR: received invalid fragment\n");
                return;
            }     
            memcpy(reas_buf + byte_offset, data + hdr_length, byte_offset);
            if((byte_offset + frag_size) == datagram_size){
                m.content.ptr = (char*)reas_buf;
                msg_send(&m,ip_process_pid, 1);
                free(reas_buf);
            }
            break;
        }
        default:{
            m.content.ptr = (char*)data;
            msg_send(&m,ip_process_pid, 1);
            break;
        }
    }
}

void sixlowpan_init(transceiver_type_t trans){
    /* init mac-layer and radio transceiver */
    sixlowmac_init(trans);
    /* init interface addresses */
    memset(&iface,0,sizeof(iface_t));
    init_802154_short_addr(&(iface.saddr));
    init_802154_long_addr(&(iface.laddr));
    create_link_local_prefix(&(iface.ipaddr));
    memcpy(&(iface.ipaddr.uint8[8]), &iface.laddr, 8);
    
    ip_process_pid = thread_create(ip_process_buf, IP_PROCESS_STACKSIZE, 
                                       PRIORITY_MAIN-1, CREATE_STACKTEST,
                                       ip_process, "ip_process");
//    send_ieee802154_frame();
}
