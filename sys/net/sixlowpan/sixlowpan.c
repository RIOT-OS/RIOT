#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vtimer.h>
#include "thread.h"
#include <mutex.h>
#include "msg.h"
#include "sixlowmac.h"
#include "sixlowpan.h"
#include "sixlowip.h"
#include "sixlownd.h"
#include "transceiver.h"
#include "ieee802154_frame.h"

uint16_t packet_length;
uint8_t packet_dispatch;
uint16_t tag;
uint8_t header_size = 0;
uint8_t max_frame = 0;
uint8_t max_frag_initial = 0;
uint8_t position;
uint8_t max_frag;

struct ipv6_hdr_t *ipv6_buf;

/* length of compressed packet */
uint16_t comp_len;
uint8_t frag_size;
uint8_t reas_buf[512];
uint8_t comp_buf[512];
uint8_t byte_offset;
uint8_t first_frag = 0;

unsigned int ip_process_pid;
unsigned int nd_nbr_cache_rem_pid = 0;

iface_t iface;
ipv6_addr_t loaddr;
ieee_802154_long_t laddr;
mutex_t buf_mutex;

char ip_process_buf[IP_PROCESS_STACKSIZE];
char nc_buf[NC_STACKSIZE];
lowpan_context_t contexts[LOWPAN_CONTEXT_MAX];
uint8_t context_len = 0;

/* deliver packet to mac*/
void lowpan_init(ieee_802154_long_t *addr, uint8_t *data){
    ipv6_buf = get_ipv6_buf();
    uint8_t mcast = 0;

    memcpy(&laddr.uint8[0], &addr->uint8[0], 8);

    if(ipv6_prefix_mcast_match(&ipv6_buf->destaddr)){
        /* send broadcast */
        mcast = 1;
    } 

//#ifdef LOWPAN_IPHC
    lowpan_iphc_encoding(&laddr);
    data = &comp_buf[0];
    packet_length = comp_len;
    
//#endif
//#ifndef LOWPAN_IPHC
//    lowpan_ipv6_set_dispatch(data);
//#endif

    /* check if packet needs to be fragmented */
    if(packet_length + header_size > PAYLOAD_SIZE - IEEE_802154_MAX_HDR_LEN){
        uint8_t fragbuf[packet_length + header_size];
        uint8_t remaining;
        uint8_t i = 2;
        /* first fragment */
        max_frame = PAYLOAD_SIZE - IEEE_802154_MAX_HDR_LEN;
        max_frag_initial = ((max_frame - 4 - header_size) / 8) * 8;
       
        memcpy(fragbuf + 4, data, max_frag_initial); 
        
        fragbuf[0] = (((0xc0 << 8) | packet_length) >> 8) & 0xff;
        fragbuf[1] = ((0xc0 << 8) | packet_length) & 0xff;
        fragbuf[2] = (tag >> 8) & 0xff;
        fragbuf[3] = tag & 0xff;  

        send_ieee802154_frame(&laddr,(uint8_t*)&fragbuf, 
                              max_frag_initial + header_size + 4, mcast);
        /* subsequent fragments */
        position = max_frag_initial;
        max_frag = ((max_frame - 5) / 8) * 8;

        data += position;        

        while(packet_length - position > max_frame - 5){
            memset(&fragbuf,0,packet_length + header_size);
            memcpy(fragbuf + 5, data, max_frag);
        
            fragbuf[0] = (((0xe0 << 8) | packet_length) >> 8) & 0xff;
            fragbuf[1] = ((0xe0 << 8) | packet_length) & 0xff;
            fragbuf[2] = (tag >> 8) & 0xff; 
            fragbuf[3] = tag & 0xff;
            fragbuf[4] = position / 8;
            
            send_ieee802154_frame(&laddr,(uint8_t*)&fragbuf, max_frag + 5, 
                                  mcast);
            data += max_frag;
            position += max_frag;

            i++;      
        }
       
        remaining = packet_length - position;

        memset(&fragbuf,0,packet_length + header_size);
        memcpy(fragbuf + 5, data, remaining); 
        
        fragbuf[0] = (((0xe0 << 8) | packet_length) >> 8) & 0xff;
        fragbuf[1] = ((0xe0 << 8) | packet_length) & 0xff;
        fragbuf[2] = (tag >> 8) & 0xff;
        fragbuf[3] = tag & 0xff;
        fragbuf[4] = position / 8;

        send_ieee802154_frame(&laddr, (uint8_t*)&fragbuf, remaining + 5, mcast);
    } else {
        send_ieee802154_frame(&laddr, data, packet_length, mcast);
    } 

    tag++;
}

void lowpan_read(uint8_t *data, uint8_t length, ieee_802154_long_t *s_laddr,
           ieee_802154_long_t *d_laddr){
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
            /* check dispatch byte */
            switch(data[hdr_length]) {
                case(LOWPAN_IPV6_DISPATCH):{
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
                if((byte_offset + frag_size) != datagram_size){
                    printf("ERROR: received invalid fragment\n");
                    return;
                }
            }     
            memcpy(reas_buf + byte_offset, data + hdr_length, byte_offset);
            if((byte_offset + frag_size) == datagram_size){
                if(reas_buf[0] == LOWPAN_IPV6_DISPATCH) {
                    /* mutex lock here */
       //             mutex_lock(&buf_mutex);
                    ipv6_buf = get_ipv6_buf();
                    memcpy(ipv6_buf, reas_buf + 1, datagram_size - 1);
                    m.content.ptr = (char*) ipv6_buf;
                    packet_length = datagram_size - 1;
                    msg_send(&m,ip_process_pid, 1);
                } else if((reas_buf[0] & 0xe0) == LOWPAN_IPHC_DISPATCH) {
                    /* mutex lock */
     //               mutex_lock(&buf_mutex);
                    lowpan_iphc_decoding(reas_buf, datagram_size, s_laddr, d_laddr);
                    ipv6_buf = get_ipv6_buf();
                    m.content.ptr = (char*) ipv6_buf;
                    msg_send(&m,ip_process_pid, 1);
                } else {        
                    printf("ERROR: packet with unknown dispatch received\n");
                }
            }
            break;
        }
        default:{
            if(data[0] == LOWPAN_IPV6_DISPATCH){
                /* mutex lock here */
   //             mutex_lock(&buf_mutex);
                ipv6_buf = get_ipv6_buf();
                memcpy(ipv6_buf, data + 1, length - 1);
                m.content.ptr = (char*) ipv6_buf;
                packet_length = length - 1;
                msg_send(&m,ip_process_pid, 1);
                break;
            } else if((data[0] & 0xe0) == LOWPAN_IPHC_DISPATCH){
                /* mutex lock here */
 //               mutex_lock(&buf_mutex);
                lowpan_iphc_decoding(data, length, s_laddr, d_laddr);
                ipv6_buf = get_ipv6_buf();
                m.content.ptr = (char*) ipv6_buf;
                msg_send(&m,ip_process_pid, 1);
                break;
            } else {
                printf("ERROR: packet with unknown dispatch received\n");
                break;
            }
        }
    }
}

void lowpan_ipv6_set_dispatch(uint8_t *data){
    memmove(data + 1, data, packet_length);
    data[0] = LOWPAN_IPV6_DISPATCH;
    packet_length++;
}

/* draft-ietf-6lowpan-hc-13#section-3.1 */
void lowpan_iphc_encoding(ieee_802154_long_t *dest){
    ipv6_buf = get_ipv6_buf();
    uint16_t payload_length = ipv6_buf->length;
    uint8_t lowpan_iphc[2];
    uint8_t *ipv6_hdr_fields = &comp_buf[2];
    lowpan_context_t *con = NULL;
    uint16_t hdr_pos = 0;
    uint8_t tc;

    memset(&lowpan_iphc, 0, 2);

    /* set iphc dispatch */
    lowpan_iphc[0] = LOWPAN_IPHC_DISPATCH;
   
    /* TF: Traffic Class, Flow Label:
     * first we need to change DSCP and ECN because in 6lowpan-nd-13 these 
     * fields are reverse, the original order is DSCP/ECN (rfc 3168) */
    tc = (ipv6_buf->version_trafficclass << 4) | (ipv6_buf->trafficclass_flowlabel >> 4);
    tc = (tc >> 2) | (tc << 6);
    
    if((ipv6_buf->flowlabel == 0) && 
       (ipv6_buf->trafficclass_flowlabel & 0x0f) == 0){
        /* flowlabel is elided */
        lowpan_iphc[0] |= LOWPAN_IPHC_FL_C;
        if(((ipv6_buf->version_trafficclass & 0x0f) == 0) && 
           ((ipv6_buf->trafficclass_flowlabel & 0xf0) == 0)){
            /* traffic class is elided */
            lowpan_iphc[0] |= LOWPAN_IPHC_TC_C;        
        } else {
            /* ECN + DSCP (1 byte), Flow Label is elided */
            ipv6_hdr_fields[hdr_pos] = tc;
            hdr_pos++;
        } 
    } else {
        /* flowlabel not compressible */
        if(((ipv6_buf->version_trafficclass & 0x0f) == 0) && 
           ((ipv6_buf->trafficclass_flowlabel & 0xf0) == 0)){
            /* traffic class is elided */
            lowpan_iphc[0] |= LOWPAN_IPHC_TC_C;
            /* ECN + 2-bit Pad + Flow Label (3 bytes), DSCP is elided */
            ipv6_hdr_fields[hdr_pos] = ((tc & 0xc0) | 
                               (ipv6_buf->trafficclass_flowlabel & 0x0f));
            memcpy(&(ipv6_hdr_fields[hdr_pos]), &ipv6_buf->flowlabel , 2);
            hdr_pos += 3;   
        } else {
            /* ECN + DSCP + 4-bit Pad + Flow Label (4 bytes) */
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->version_trafficclass, 4);
            ipv6_hdr_fields[hdr_pos] = tc;
            hdr_pos += 4;
        }
    }

    /* NH: Next Header: 
     * TODO: NHC */
    ipv6_hdr_fields[hdr_pos] = ipv6_buf->nextheader;
    hdr_pos++;

    /* HLIM: Hop Limit: */
    switch(ipv6_buf->hoplimit){
        case(1):{
            /* 01: The Hop Limit field is compressed and the hop limit is 1. */
            lowpan_iphc[0] |= 0x01;
            break;
        }
        case(64):{
            /* 10: The Hop Limit field is compressed and the hop limit is 64. */
            lowpan_iphc[0] |= 0x02;
            break;
        }
        case(255):{
            /* 11: The Hop Limit field is compressed and the hop limit is 255. */
            lowpan_iphc[0] |= 0x03;
            break;
        }
        default:{
            ipv6_hdr_fields[hdr_pos] = ipv6_buf->hoplimit;
            hdr_pos++;
            break;
        }
    }

    /* CID: Context Identifier Extension: */
    if((lowpan_context_lookup(&ipv6_buf->srcaddr) != NULL ) || 
       (lowpan_context_lookup(&ipv6_buf->destaddr) != NULL)){
        lowpan_iphc[1] |= LOWPAN_IPHC_CID;
        memmove(&ipv6_hdr_fields[1],&ipv6_hdr_fields[0], hdr_pos);
        hdr_pos++;
    }

    /* SAC: Source Address Compression */
    if(ipv6_addr_unspec_match(&(ipv6_buf->srcaddr))){
        /* SAC = 1 and SAM = 00 */
        lowpan_iphc[1] |= LOWPAN_IPHC_SAC;
    } else if((con = lowpan_context_lookup(&ipv6_buf->srcaddr)) != NULL) {
        /* 1: Source address compression uses stateful, context-based
         *    compression. */
        lowpan_iphc[1] |= LOWPAN_IPHC_SAC;
        ipv6_hdr_fields[0] |= (con->num << 4);
        
        if(memcmp(&(ipv6_buf->srcaddr.uint8[8]),&(iface.laddr.uint8[0]), 8) == 0){
            /* 0 bits. The address is derived using context information
             * and possibly the link-layer addresses.*/
            lowpan_iphc[1] |= 0x30;
        } else if((ipv6_buf->srcaddr.uint16[4] == 0) && 
                  (ipv6_buf->srcaddr.uint16[5] == 0) &&
                  (ipv6_buf->srcaddr.uint16[6] == 0) &&
                  ((ipv6_buf->srcaddr.uint8[14]) & 0x80) == 0){
            /* 49-bit of interface identifier are 0, so we can compress
             * source address-iid to 16-bit */
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->srcaddr.uint16[7], 2);
            hdr_pos += 2;
            /* 16 bits. The address is derived using context information
             * and the 16 bits carried inline. */
            lowpan_iphc[1] |= 0x20;
        } else {
            memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->srcaddr.uint16[4]),8);
            hdr_pos += 8;
            /* 64 bits. The address is derived using context information
             * and the 64 bits carried inline. */            
            lowpan_iphc[1] |= 0x10;
        }         
    } else if(ipv6_prefix_ll_match(&ipv6_buf->srcaddr)){
        /* 0: Source address compression uses stateless compression.*/
        if(memcmp(&(ipv6_buf->srcaddr.uint8[8]),&(iface.laddr.uint8[0]), 8) == 0){
            /* 0 bits. The address is derived using context information
             * and possibly the link-layer addresses.*/
            lowpan_iphc[1] |= 0x30;
        } else if((ipv6_buf->srcaddr.uint16[4] == 0) && 
                  (ipv6_buf->srcaddr.uint16[5] == 0) &&
                  (ipv6_buf->srcaddr.uint16[6] == 0) &&
                  ((ipv6_buf->srcaddr.uint8[14]) & 0x80) == 0){
            /* 49-bit of interface identifier are 0, so we can compress
             * source address-iid to 16-bit */
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->srcaddr.uint16[7], 2);
            hdr_pos += 2;
            /* 16 bits. The address is derived using context information
             * and the 16 bits carried inline. */
            lowpan_iphc[1] |= 0x20;
        } else {
            memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->srcaddr.uint16[4]),8);
            hdr_pos += 8;
            /* 64 bits. The address is derived using context information
             * and the 64 bits carried inline. */            
            lowpan_iphc[1] |= 0x10;
        }          
    } else {
        /* full address carried inline */
        memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->srcaddr.uint8[0]), 16);
        hdr_pos += 16;
    }

    /* M: Multicast Compression */
    if(ipv6_prefix_mcast_match(&ipv6_buf->destaddr)){
        /* 1: Destination address is a multicast address. */
        lowpan_iphc[1] |= LOWPAN_IPHC_M;
        /* just another cool if condition */
        if((ipv6_buf->destaddr.uint8[1] == 2) && 
           (ipv6_buf->destaddr.uint16[1] == 0) &&
           (ipv6_buf->destaddr.uint16[2] == 0) &&
           (ipv6_buf->destaddr.uint16[3] == 0) &&
           (ipv6_buf->destaddr.uint16[4] == 0) &&
           (ipv6_buf->destaddr.uint16[5] == 0) &&
           (ipv6_buf->destaddr.uint16[6] == 0) &&
           (ipv6_buf->destaddr.uint8[14] == 0)){
            /* 11: 8 bits. The address takes the form FF02::00XX. */
            lowpan_iphc[1] |= 0x03;
            ipv6_hdr_fields[hdr_pos] = ipv6_buf->destaddr.uint8[15];
            hdr_pos++;
        } else if((ipv6_buf->destaddr.uint16[1] == 0) && 
                  (ipv6_buf->destaddr.uint16[2] == 0) &&
                  (ipv6_buf->destaddr.uint16[3] == 0) &&
                  (ipv6_buf->destaddr.uint16[4] == 0) &&
                  (ipv6_buf->destaddr.uint16[5] == 0) &&
                  (ipv6_buf->destaddr.uint8[12] == 0)){
            /* 10: 32 bits. The address takes the form FFXX::00XX:XXXX. */
            lowpan_iphc[1] |= 0x02;
            /* copy second and last 3 byte */
            ipv6_hdr_fields[hdr_pos] = ipv6_buf->destaddr.uint8[1];
            hdr_pos++;
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint8[13], 3);
            hdr_pos += 3;     
        } else if((ipv6_buf->destaddr.uint16[1] == 0) && 
                  (ipv6_buf->destaddr.uint16[2] == 0) &&
                  (ipv6_buf->destaddr.uint16[3] == 0) &&
                  (ipv6_buf->destaddr.uint16[4] == 0) &&
                  (ipv6_buf->destaddr.uint8[10] == 0)){
            /* 01: 48 bits.  The address takes the form FFXX::00XX:XXXX:XXXX */
            lowpan_iphc[1] |= 0x01;
            /* copy second and last 5 byte */
            ipv6_hdr_fields[hdr_pos] = ipv6_buf->destaddr.uint8[1];
            hdr_pos++;
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint8[11], 5);
            hdr_pos += 5;
        } else {
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint8[0], 16);
            hdr_pos += 16;
        } 
    } else {
        /* 0: Destination address is not a multicast address. */
        if((con = lowpan_context_lookup(&ipv6_buf->destaddr)) != NULL){
            /* 1: Destination address compression uses stateful, context-based
             * compression. */
            lowpan_iphc[1] |= LOWPAN_IPHC_DAC;
            ipv6_hdr_fields[0] = con->num;

            if(memcmp(&(ipv6_buf->destaddr.uint8[8]),&(dest->uint8[0]), 8) == 0){
                /* 0 bits. The address is derived using context information
                 * and possibly the link-layer addresses.*/
                lowpan_iphc[1] |= 0x03;
            } else if((ipv6_buf->destaddr.uint16[4] == 0) &&
                      (ipv6_buf->destaddr.uint16[5] == 0) &&
                      (ipv6_buf->destaddr.uint16[6] == 0) &&
                      ((ipv6_buf->destaddr.uint8[14]) & 0x80) == 0){
                /* 49-bit of interface identifier are 0, so we can compress
                 * source address-iid to 16-bit */
                memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint16[7], 2);
                hdr_pos += 2;
                /* 16 bits. The address is derived using context information
                 * and the 16 bits carried inline. */
                lowpan_iphc[1] |= 0x02;
            } else {
                memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->destaddr.uint16[4]),8);
                hdr_pos += 8;
                /* 64 bits. The address is derived using context information
                * and the 64 bits carried inline. */
                lowpan_iphc[1] |= 0x01;
            }
        } else if(ipv6_prefix_ll_match(&ipv6_buf->destaddr)){
            if(memcmp(&(ipv6_buf->destaddr.uint8[8]),&(dest->uint8[0]), 8) == 0){
                /* 0 bits. The address is derived using context information
                * and possibly the link-layer addresses.*/
                lowpan_iphc[1] |= 0x03;
            } else if((ipv6_buf->destaddr.uint16[4] == 0) &&
                      (ipv6_buf->destaddr.uint16[5] == 0) &&
                      (ipv6_buf->destaddr.uint16[6] == 0) &&
                      ((ipv6_buf->destaddr.uint8[14]) & 0x80) == 0){
                /* 49-bit of interface identifier are 0, so we can compress
                 * source address-iid to 16-bit */
                memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint16[7], 2);
                hdr_pos += 2;
                /* 16 bits. The address is derived using context information
                 * and the 16 bits carried inline. */
                lowpan_iphc[1] |= 0x02;
            } else {
                memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->destaddr.uint16[4]),8);
                hdr_pos += 8;
                /* 64 bits. The address is derived using context information
                 * and the 64 bits carried inline. */
                lowpan_iphc[1] |= 0x01;
            }
        } else {
            memcpy(&ipv6_hdr_fields[hdr_pos],&(ipv6_buf->destaddr.uint8[0]), 16);
            hdr_pos += 16;
        }
    }

    comp_buf[0] = lowpan_iphc[0];
    comp_buf[1] = lowpan_iphc[1];

    uint8_t *ptr = get_payload_buf(ipv6_ext_hdr_len);

    memcpy(&ipv6_hdr_fields[hdr_pos],ptr,ipv6_buf->length);

    comp_len = 2 + hdr_pos + payload_length;
}

void lowpan_iphc_decoding(uint8_t *data, uint8_t length, 
                          ieee_802154_long_t *s_laddr, 
                          ieee_802154_long_t *d_laddr){
    uint8_t hdr_pos = 0;
    uint8_t *ipv6_hdr_fields = data;
    uint8_t lowpan_iphc[2];
    uint8_t cid = 0;
    uint8_t sci = 0;
    uint8_t dci = 0;
    
 


    uint8_t ll_prefix[2] = {0xfe, 0x80};
    uint8_t m_prefix[2] = {0xff, 0x02};
    lowpan_context_t *con = NULL;
    
    ipv6_buf = get_ipv6_buf();

    lowpan_iphc[0] = ipv6_hdr_fields[0];
    lowpan_iphc[1] = ipv6_hdr_fields[1];
    hdr_pos += 2;

    /* first check if CID flag is set */
    if(lowpan_iphc[1] & LOWPAN_IPHC_CID){
        hdr_pos++;
        cid = 1;
    }

    /* TF: Traffic Class, Flow Label: */
    if(lowpan_iphc[0] & LOWPAN_IPHC_FL_C){
        /* flowlabel is elided */
        if(lowpan_iphc[0] & LOWPAN_IPHC_TC_C){
            /* traffic class is elided */
            ipv6_buf->version_trafficclass = 0x60;
            ipv6_buf->trafficclass_flowlabel = 0;
            ipv6_buf->flowlabel = 0;        
        } else {
            /* toogle ecn/dscp order */
            ipv6_buf->version_trafficclass = 0x60 | (0x0f & 
                                             (ipv6_hdr_fields[hdr_pos] >> 2));
            ipv6_buf->trafficclass_flowlabel = ((ipv6_hdr_fields[hdr_pos] >> 2) & 0x30) |
                                               ((ipv6_hdr_fields[hdr_pos] << 6) & 0xc0);
            ipv6_buf->flowlabel = 0;  
            hdr_pos += 3; 
        }
    } else {
        /* flowlabel carried inline */
        if(lowpan_iphc[0] & LOWPAN_IPHC_TC_C){
            /* traffic class is elided */
            ipv6_buf->version_trafficclass = 0x60;
            /* ecn + 4 bit flowlabel*/
            ipv6_buf->trafficclass_flowlabel = ((ipv6_hdr_fields[hdr_pos] >> 2) & 0x30) | 
                                                (ipv6_hdr_fields[hdr_pos] & 0x0f);
            hdr_pos++;
            /* copy 2byte flowlabel */
            memcpy(&ipv6_buf->flowlabel, &ipv6_hdr_fields[hdr_pos], 2);
            hdr_pos += 2;    
        } else {
            ipv6_buf->version_trafficclass = 0x60 | (0x0f &
                                             (ipv6_hdr_fields[hdr_pos] >> 2));
            ipv6_buf->trafficclass_flowlabel = ((ipv6_hdr_fields[hdr_pos] >> 2) & 0x30) | 
                                                (ipv6_hdr_fields[hdr_pos] & 0x0f) |
                                                (ipv6_hdr_fields[hdr_pos + 1] & 0x0f);
            hdr_pos += 2;
            memcpy(&ipv6_buf->trafficclass_flowlabel, 
                   &ipv6_hdr_fields[hdr_pos], 2);
            hdr_pos += 2; 
        }
    }
   
    /* NH: Next Header: */
    if(lowpan_iphc[0] & LOWPAN_IPHC_NH){
        // TODO: next header decompression
    } else {
        ipv6_buf->nextheader = ipv6_hdr_fields[hdr_pos];
        hdr_pos++;
    }

    /* HLIM: Hop Limit: */
    if(lowpan_iphc[0] & 0x03){
        switch(lowpan_iphc[0] & 0x03){
            case(0x01):{
                ipv6_buf->hoplimit = 1;
                break;
            }
            case(0x02):{
                ipv6_buf->hoplimit = 64;
                break;
            }
            case(0x03):{
                ipv6_buf->hoplimit = 255;
                break;
            }
            default:
                break;
        } 
    } else {
        ipv6_buf->hoplimit = ipv6_hdr_fields[hdr_pos];
        hdr_pos++;
    }

    /* CID: Context Identifier Extension: + SAC: Source Address Compression */
    if(lowpan_iphc[1] & LOWPAN_IPHC_SAC){
        /* 1: Source address compression uses stateful, context-based
         * compression.*/
        if(cid){
            sci = ipv6_hdr_fields[3] >> 4;
        }
        /* check context number */
        if(((lowpan_iphc[1] & LOWPAN_IPHC_SAM) >> 4) & 0x03){
            con = lowpan_context_num_lookup(sci); 
        }

        if(con == NULL){
            printf("ERROR: context not found\n");
            return;
        }
        switch(((lowpan_iphc[1] & LOWPAN_IPHC_SAM) >> 4) & 0x03){
            case(0x01):{
                /* 64-bits */
                memcpy(&(ipv6_buf->srcaddr.uint8[8]), &ipv6_hdr_fields[hdr_pos], 8);
                /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                hdr_pos += 8;
                break;
            }
            case(0x02):{
                /* 16-bits */
                memset(&(ipv6_buf->srcaddr.uint8[8]), 0, 6);
                memcpy(&(ipv6_buf->srcaddr.uint8[14]), &ipv6_hdr_fields[hdr_pos], 2);
                /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                hdr_pos += 2;
                break;
            }
            case(0x03):{
                /* 0-bits */
                memset(&(ipv6_buf->srcaddr.uint8[8]), 0, 8);
                memcpy(&(ipv6_buf->srcaddr.uint8[8]), &s_laddr->uint8[0], 8); 
                /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                break;
            }
            default:{
                /* unspecified address */
                memset(&(ipv6_buf->srcaddr.uint8[0]), 0, 16);
                break;
            }
        } 
    } else {
        switch(((lowpan_iphc[1] & LOWPAN_IPHC_SAM) >> 4) & 0x03){
            case(0x01):{
                /* 64-bits */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &ll_prefix[0], 2);
                memset(&(ipv6_buf->srcaddr.uint8[2]), 0, 6);
                memcpy(&(ipv6_buf->srcaddr.uint8[8]), &ipv6_hdr_fields[hdr_pos], 8);
                hdr_pos += 8;
                break;
            }
            case(0x02):{
                /* 16-bits */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &ll_prefix[0], 2);
                memset(&(ipv6_buf->srcaddr.uint8[2]), 0, 12);
                memcpy(&(ipv6_buf->srcaddr.uint8[14]), &ipv6_hdr_fields[hdr_pos], 2);
                hdr_pos += 2;
                break;
            }
            case(0x03):{
                /* 0-bits */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &ll_prefix[0], 2);
                memset(&(ipv6_buf->srcaddr.uint8[8]), 0, 14);
                memcpy(&(ipv6_buf->srcaddr.uint8[8]), &s_laddr->uint8[0], 8);
                break;
            }
            default:{
                /* full address carried inline */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), 
                       &ipv6_hdr_fields[hdr_pos], 16);
                hdr_pos += 16;
                break;
            }
        }
    }

    /* M: Multicast Compression + DAC: Destination Address Compression */
    if(lowpan_iphc[1] & LOWPAN_IPHC_M){
        /* 1: Destination address is a multicast address. */
        if(lowpan_iphc[1] & LOWPAN_IPHC_DAC){
            /* 1: Destination address compression uses stateful, context-based
             * compression. 
             * If M=1 and DAC=1: */ 
            if(cid){
                dci = ipv6_hdr_fields[3] & 0x0f;
            }
            if((lowpan_iphc[1] & LOWPAN_IPHC_DAM) & 0x03){
                con = lowpan_context_num_lookup(dci);    
            }
            if(con == NULL){
                printf("ERROR: context not found\n");
                return;
            }
            // TODO:  
        } else {
            /* If M=1 and DAC=0: */
            switch(lowpan_iphc[1] & 0x03){
                case(0x01):{
                    m_prefix[1] = ipv6_hdr_fields[hdr_pos];
                    hdr_pos++;
                    break;
                }
                case(0x02):{
                    m_prefix[1] = ipv6_hdr_fields[hdr_pos];
                    hdr_pos++;
                    break;
                }
                default:
                    break;
            }
            
            switch(lowpan_iphc[1] & 0x03){
                case(0x01):{
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &m_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 9);
                    memcpy(&(ipv6_buf->destaddr.uint8[11]), &ipv6_hdr_fields[hdr_pos], 5);
                    hdr_pos += 5;
                    break;
                }
                case(0x02):{
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &m_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 11);
                    memcpy(&(ipv6_buf->destaddr.uint8[13]), &ipv6_hdr_fields[hdr_pos], 3);
                    hdr_pos += 3;
                    break;
                }
                case(0x03):{
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &m_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 13);
                    memcpy(&(ipv6_buf->destaddr.uint8[15]), &ipv6_hdr_fields[hdr_pos], 1);
                    hdr_pos++;
                    break;
                }
                default:{
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &ipv6_hdr_fields[hdr_pos], 16);
                    break;
                }
            } 
        }
    } else {
        if(lowpan_iphc[1] & LOWPAN_IPHC_DAC){
            /* 1: Destination address compression uses stateful, context-based
             * compression. 
             * If M=1 and DAC=1: */
            if(cid){
                dci = ipv6_hdr_fields[3] & 0x0f;
            }
            if((lowpan_iphc[1] & LOWPAN_IPHC_DAM) & 0x03){
                con = lowpan_context_num_lookup(dci);
            }
            if(con == NULL){
                printf("ERROR: context not found\n");
                return;
            }
            
            switch((lowpan_iphc[1] & LOWPAN_IPHC_DAM) & 0x03){
                case(0x01):{
                    memcpy(&(ipv6_buf->destaddr.uint8[8]), &ipv6_hdr_fields[hdr_pos], 8);
                    /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                    hdr_pos += 8;
                    break;
                }
                case(0x02):{
                    memset(&(ipv6_buf->destaddr.uint8[8]), 0, 6);
                    memcpy(&(ipv6_buf->destaddr.uint8[14]), &ipv6_hdr_fields[hdr_pos], 2);
                    /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                    hdr_pos += 2;
                    break;
                }
                case(0x03):{
                    memset(&(ipv6_buf->destaddr.uint8[0]), 0, 8);
                    memcpy(&(ipv6_buf->destaddr.uint8[8]), &d_laddr->uint8[0], 8);
                    /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                    break;
                }
                default:
                    break;
            }
        } else {
            switch((lowpan_iphc[1] & LOWPAN_IPHC_DAM) & 0x03){
                case(0x01):{
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &ll_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 6);
                    memcpy(&(ipv6_buf->destaddr.uint8[8]), 
                           &ipv6_hdr_fields[hdr_pos], 8);
                    hdr_pos += 8;
                    break;
                }
                case(0x02):{
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &ll_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 12);
                    memcpy(&(ipv6_buf->destaddr.uint8[14]), 
                           &ipv6_hdr_fields[hdr_pos], 2);
                    hdr_pos += 2;
                    break;
                }
                case(0x03):{
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &ll_prefix, 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 14);
                    memcpy(&(ipv6_buf->destaddr.uint8[8]), &d_laddr->uint8[0], 8);
                    break;
                }
                default:{
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), 
                           &ipv6_hdr_fields[hdr_pos], 16);
                    hdr_pos += 16; 
                    break;
                }
            }    
        } 
    }

    uint8_t *ptr = get_payload_buf(ipv6_ext_hdr_len);

    memcpy(ptr, &ipv6_hdr_fields[hdr_pos], length - hdr_pos);

    /* ipv6 length */
    ipv6_buf->length = length - hdr_pos;
    packet_length = IPV6_HDR_LEN + ipv6_buf->length; 
}

uint8_t lowpan_context_len(){
    return context_len;
}

void lowpan_context_remove(uint8_t num) {
    int i,j;
    for(i = 0; i < LOWPAN_CONTEXT_MAX; i++){
        if(contexts[i].num == num){
            context_len--;
//            vtimer_remove(&(contexts[i].lifetime));   Not implemented yet?
        }
    }
    
    for(j = 0; j < LOWPAN_CONTEXT_MAX; j++) {
        contexts[j] = contexts[j+1];
    }
}

void lowpan_context_remove_cb(void* ptr) {
    uint8_t num = *((uint8_t*)ptr);
    lowpan_context_remove(num);
}

lowpan_context_t *lowpan_context_update(uint8_t num, const ipv6_addr_t *prefix, 
                        uint8_t length, uint8_t comp,
                        uint16_t lifetime){
    lowpan_context_t *context;
    
    timex_t lt;
    
    lt.nanoseconds = lifetime * 60 * 1000000;
    
    if (lifetime == 0){
        lowpan_context_remove(num);
        return NULL;
    }
    
    if (context_len == LOWPAN_CONTEXT_MAX)
        return NULL;
    
    context = lowpan_context_num_lookup(num);
    
    if (context == NULL) {
        context = &(contexts[context_len++]);
    }
    
    context->num = num;
    memset((void*)(&context->prefix),0,16);
    // length in bits
    memcpy((void*)(&context->prefix),(void*)prefix,length/8);
    context->length = length;
    context->comp = comp;
    vtimer_set_cb(&(context->lifetime), 
                  lt, 
                  lowpan_context_remove_cb, 
                  (void*)(&num));
    
    return context;
}

lowpan_context_t *lowpan_context_get() {
    return contexts;
}

lowpan_context_t * lowpan_context_lookup(ipv6_addr_t *addr){
    int i;
    for(i = 0; i < lowpan_context_len(); i++){
        if(contexts[i].length > 0 && memcmp((void*)addr,&(contexts[i].prefix),contexts[i].length) == 0){
            return &contexts[i];
        }
    }
    return NULL; 
}

lowpan_context_t * lowpan_context_num_lookup(uint8_t num){
    int i;
    for(i = 0; i < lowpan_context_len(); i++){
        if(contexts[i].num == num){
            return &contexts[i];
        }
    }
    return NULL;
}

void sixlowpan_init(transceiver_type_t trans, uint8_t r_addr){
    /* init mac-layer and radio transceiver */
    vtimer_init();
    sixlowmac_init(trans);
    /* init interface addresses */
    memset(&iface,0,sizeof(iface_t));
    set_radio_address(r_addr); 
    init_802154_short_addr(&(iface.saddr));
    init_802154_long_addr(&(iface.laddr));
    /* init global buffer mutex */
    mutex_init(&buf_mutex);

    /* init link-local address */
    ipv6_set_ll_prefix(&loaddr);
    
    memcpy(&(loaddr.uint8[8]), &(iface.laddr.uint8[0]), 8);
    ipv6_iface_add_addr(&loaddr, ADDR_STATE_PREFERRED, 0, 0, 
                        ADDR_CONFIGURED_AUTO);
    ip_process_pid = thread_create(ip_process_buf, IP_PROCESS_STACKSIZE, 
                                       PRIORITY_MAIN-1, CREATE_STACKTEST,
                                       ipv6_process, "ip_process");
    nd_nbr_cache_rem_pid = thread_create(nc_buf, NC_STACKSIZE,
                                         PRIORITY_MAIN-1, CREATE_STACKTEST,
                                         nbr_cache_auto_rem, "nbr_cache_rem"); 
}

void sixlowpan_adhoc_init(transceiver_type_t trans, ipv6_addr_t *prefix, uint8_t r_addr){
    /* init network prefix */
    ipv6_set_prefix(prefix, prefix);
    plist_add(prefix, 64, OPT_PI_VLIFETIME_INFINITE,0,1,OPT_PI_FLAG_A);
    ipv6_init_iface_as_router();
    sixlowpan_init(trans, r_addr);
}
