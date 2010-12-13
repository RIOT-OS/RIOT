#include "ieee802154_frame.h"

uint8_t mac_buf[IEEE_802154_HDR_LEN + IEEE_802154_PAYLOAD_LEN];

uint8_t ieee802154_hdr_ptr;
uint8_t ieee802154_payload_ptr;
uint16_t ieee802154_payload_len;

uint8_t init_802154_frame(ieee802154_frame_t *frame, uint8_t *buf){
    /* Frame Control Field - 802.15.4 - 2006 - 7.2.1.1  */
    uint8_t index = 0;
    
    buf[index] = ((frame->fcf.frame_type << 5) |
             (frame->fcf.sec_enb << 4) |
             (frame->fcf.frame_pend << 3) |
             (frame->fcf.ack_req << 2) | 
             (frame->fcf.panid_comp << 1));
    buf[index++] = ((frame->fcf.dest_addr_m << 4) |
             (frame->fcf.frame_ver << 2) |
             (frame->fcf.src_addr_m));
    
    /* Sequence Number - 802.15.4 - 2006 - 7.2.1.2 */
    buf[index++] = frame->seq_nr;
    
    /* Destination PAN Identifier - 802.15.4 - 2006 - 7.2.1.3 */
    if(frame->fcf.dest_addr_m == 0x02 || frame->fcf.dest_addr_m == 0x03){
        buf[index++] = ((frame->dest_pan_id >> 8) & 0xff);
        buf[index++] = (frame->dest_pan_id & 0xff); 
    }

    /* Destination Address - 802.15.4 - 2006 - 7.2.1.4 */
    if(frame->fcf.dest_addr_m == 0x02){
        buf[index++] = frame->dest_addr[0];
        buf[index++] = frame->dest_addr[1];
    } else if(frame->fcf.dest_addr_m == 0x03){
        buf[index++] = frame->dest_addr[0];
        buf[index++] = frame->dest_addr[1];
        buf[index++] = frame->dest_addr[2];
        buf[index++] = frame->dest_addr[3];       
        buf[index++] = frame->dest_addr[4];
        buf[index++] = frame->dest_addr[5];
        buf[index++] = frame->dest_addr[6];  
        buf[index++] = frame->dest_addr[7];
    }

    /* Source PAN Identifier - 802.15.4 - 2006 - 7.2.1.5 */
    if(!(frame->fcf.panid_comp & 0x01)){
        if(frame->fcf.src_addr_m == 0x02 || frame->fcf.src_addr_m == 0x03){
            buf[index++] = ((frame->src_pan_id >> 8) & 0xff);
            buf[index++] = (frame->src_pan_id & 0xff);
        }
    }

    /* Source Address field - 802.15.4 - 2006 - 7.2.1.6 */
    if(frame->fcf.src_addr_m == 0x02){
        buf[index++] = frame->src_addr[0];
        buf[index++] = frame->src_addr[1];      
    } else if(frame->fcf.src_addr_m == 0x03){    
        buf[index++] = frame->src_addr[0];
        buf[index++] = frame->src_addr[1];
        buf[index++] = frame->src_addr[2];
        buf[index++] = frame->src_addr[3];
        buf[index++] = frame->src_addr[4];
        buf[index++] = frame->src_addr[5];
        buf[index++] = frame->src_addr[6];
        buf[index++] = frame->src_addr[7];
    }

    return index; 
}

/**     2      1      2     VAR    2      VAR
  * -------------------------------------------
  * |  FCF  | DSN | DPID  | DAD | SPID  | SAD |
  * -------------------------------------------
  */
uint8_t get_802154_hdr_len(ieee802154_frame_t *frame){
    uint8_t len = 0;
    uint8_t comp;

     
    if(frame->fcf.dest_addr_m == 0x02){
        len += 2;
    } else if(frame->fcf.dest_addr_m == 0x03){
        len += 8;
    }

    if(frame->fcf.src_addr_m == 0x02){
        len += 2;
    } else if(frame->fcf.src_addr_m == 0x03){
        len += 8;
    }

    if((frame->fcf.dest_addr_m == 0x02) || (frame->fcf.dest_addr_m == 0x03)){
        len += 2;
    }

    if((frame->fcf.src_addr_m == 0x02) || (frame->fcf.src_addr_m == 0x03)){
        len += 2;
    } 

    /* if src pan id == dest pan id set compression bit */
    if(frame->src_pan_id == frame->dest_pan_id){
        frame->fcf.panid_comp = 1;
        len -= 2;
    } 

    /* (DPID + DAD + SPID + SAD) + (FCF + DSN) */
    return (len + 3);
}

uint8_t read_802154_frame(uint8_t *buf, ieee802154_frame_t *frame, uint8_t len){
    uint8_t index = 0;
    uint8_t hdrlen;

    frame->fcf.frame_type = (buf[index] >> 5) & 0x07;
    frame->fcf.sec_enb = (buf[index] >> 4) & 0x01;
    frame->fcf.frame_pend = (buf[index] >> 3) & 0x01;
    frame->fcf.ack_req = (buf[index] >> 2) & 0x01;
    frame->fcf.panid_comp = (buf[index] >> 1) & 0x01;

    index++;

    frame->fcf.dest_addr_m = (buf[index] >> 4) & 0x03;
    frame->fcf.frame_ver = (buf[index] >> 2) & 0x03;
    frame->fcf.src_addr_m = buf[index] & 0x03;

    index++;

    frame->seq_nr = buf[index];

    index++;

    frame->dest_pan_id = (((uint16_t)buf[index]) << 8) | buf[index+1];
    
    index += 2;

    switch(frame->fcf.dest_addr_m){
        case(0):{
            printf("fcf.dest_addr_m: pan identifier/address fields empty");
        }
        case(2):{
            frame->dest_addr[0] = buf[index];
            frame->dest_addr[1] = buf[index+1];
            index += 2;
        }
        case(3):{
            frame->dest_addr[0] = buf[index];
            frame->dest_addr[1] = buf[index+1];
            frame->dest_addr[2] = buf[index+2];
            frame->dest_addr[3] = buf[index+3];
            frame->dest_addr[4] = buf[index+4];
            frame->dest_addr[5] = buf[index+5];
            frame->dest_addr[6] = buf[index+6];
            frame->dest_addr[7] = buf[index+7];
            index += 8;
        }
    }
   
    if(!(frame->fcf.panid_comp == 1)){
        frame->src_pan_id = (((uint16_t)buf[index]) << 8) | buf[index+1];
        index += 2;
    }
 
    switch(frame->fcf.src_addr_m){
        case(0):{
            printf("fcf.src_addr_m: pan identifier/address fields empty");
        }
        case(2):{
            frame->src_addr[0] = buf[index];
            frame->src_addr[1] = buf[index+1];
            index += 2;
        }
        case(3):{
            frame->src_addr[0] = buf[index];
            frame->src_addr[1] = buf[index+1];
            frame->src_addr[2] = buf[index+2];
            frame->src_addr[3] = buf[index+3];
            frame->src_addr[4] = buf[index+4];
            frame->src_addr[5] = buf[index+5];
            frame->src_addr[6] = buf[index+6];
            frame->src_addr[7] = buf[index+7];
            index += 8;
        }
    }

    frame->payload = (buf + index);
    hdrlen = index;
    frame->payload_len = (len - hdrlen);
        
    return hdrlen; 
}

uint8_t get_802154_pkt_len(void){
    return ((IEEE_802154_HDR_LEN - ieee802154_hdr_ptr) + 
            ieee802154_payload_len);
}

uint8_t check_802154_hdr_len(uint8_t len){
    if((ieee802154_hdr_ptr >= len) && 
        ((get_802154_pkt_len() + len) <= IEEE_802154_PAYLOAD_LEN)){
        ieee802154_hdr_ptr -= len;
        return 1;
    }
    return 0;  
}

uint8_t * get_802154_hdr_ptr(void){
    return &mac_buf[ieee802154_hdr_ptr];
}

uint8_t * get_802154_payload_ptr(void){
    return &mac_buf[ieee802154_payload_ptr + IEEE_802154_HDR_LEN];
}

void clear_802154_pkt(void){
    ieee802154_payload_ptr = 0;
    ieee802154_payload_len = 0;
    ieee802154_hdr_ptr = IEEE_802154_HDR_LEN;
}
