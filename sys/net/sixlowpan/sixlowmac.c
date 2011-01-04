/* 6LoWPAN MAC - layer 2 implementations */

#include <stdlib.h>
#include "sixlowmac.h"
#include "sixlowip.h"
#include "sixlownd.h"
#include "thread.h"
#include "msg.h"
#include "radio/radio.h"
#include "transceiver.h"
#include "ieee802154_frame.h"

char radio_stack_buffer[RADIO_STACK_SIZE];
msg msg_q[RADIO_RCV_BUF_SIZE];
uint8_t snd_buffer[RADIO_SND_BUF_SIZE][PAYLOAD_SIZE];

uint8_t buf[PAYLOAD_SIZE];

static uint8_t macdsn;
static uint8_t macbsn;

static radio_packet_t p;
static msg mesg;
int transceiver_type;
static transceiver_command_t tcmd;

uint8_t get_radio_address(void){
    uint16_t addr;

    tcmd.transceivers = transceiver_type;
    tcmd.data = &addr;
    mesg.content.ptr = (char*)&tcmd;
    mesg.type = GET_ADDRESS;
    msg_send_receive(&mesg,&mesg,transceiver_pid);
    
    return (uint8_t)addr;
}

void set_radio_channel(uint8_t channel){
    tcmd.transceivers = transceiver_type;
    tcmd.data = &channel;
    mesg.content.ptr = (char*)&tcmd;
    mesg.type = SET_CHANNEL;
    msg_send_receive(&mesg,&mesg,transceiver_pid);
}

void init_802154_short_addr(ieee_802154_short_t *saddr){
    saddr->uint8[0] = 0;
    saddr->uint8[1] = get_radio_address(); 
}

void init_802154_long_addr(ieee_802154_long_t *laddr){
    // 16bit Pan-ID:16-zero-bits:16-bit-short-addr = 48bit
    laddr->uint16[0] = IEEE_802154_PAN_ID;
    
    /* RFC 4944 Section 6 / RFC 2464 Section 4 */
    laddr->uint8[0] |= 0x02;
    laddr->uint8[2] = 0;
    laddr->uint8[3] = 0xFF;
    laddr->uint8[4] = 0xFE;
    laddr->uint8[5] = 0;
    laddr->uint8[6] = 0;
    laddr->uint8[7] = get_radio_address();
}

void recv_ieee802154_frame(void){
    msg m;
    radio_packet_t *p;
    uint8_t i, hdrlen;
    ieee802154_frame_t frame;
   
    msg_init_queue(msg_q, RADIO_RCV_BUF_SIZE);

    while (1) {
        msg_receive(&m);
        if (m.type == PKT_PENDING) {
            p = (radio_packet_t*) m.content.ptr;

            for (i = 0; i < p->length; i++) {
                printf("%02X ", p->data[i]);
            }

            //clear_802154_pkt();
            hdrlen = read_802154_frame(p->data, &frame, p->length);
            printf("hdrlen: %hu\n", hdrlen);
            printf("payload: %02x\n", frame.payload[0]);
    
            p->processing--;
            printf("\n");
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            puts("Unknown packet received");
        }
    }
}

void set_ieee802154_fcf_values(ieee802154_frame_t *frame){
    frame->fcf.frame_type = IEEE_802154_DATA_FRAME;
    frame->fcf.sec_enb = 0;
    frame->fcf.frame_pend = 0;
    frame->fcf.ack_req = 0;
    frame->fcf.panid_comp = 0;
    frame->fcf.frame_ver = 0;
    frame->fcf.src_addr_m = IEEE_802154_SHORT_ADDR_M;
    frame->fcf.dest_addr_m = IEEE_802154_SHORT_ADDR_M;
}

void set_ieee802154_frame_values(ieee802154_frame_t *frame){
    // TODO: addresse aus ip paket auslesen und in frame einfuegen
    frame->dest_pan_id = IEEE_802154_PAN_ID;
    frame->src_pan_id = IEEE_802154_PAN_ID;
    frame->seq_nr = macdsn;
}

void send_ieee802154_frame(uint16_t addr, uint8_t *buf){
    unsigned int c, i;
    
    set_radio_channel(13);
    
    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = transceiver_type;
    tcmd.data = &p;
    
    ieee802154_frame_t frame; 

    memset(&frame, 0, sizeof(frame));
    set_ieee802154_fcf_values(&frame);
    set_ieee802154_frame_values(&frame); 
    
    /* ONLY FOR TESTING */
    frame.dest_addr[0] = 0;
    frame.dest_addr[1] = 4;
    frame.src_addr[0] = 0;
    frame.src_addr[1] = 5;
 
    uint8_t test = 30;
    frame.payload = &test;
    frame.payload_len = 1;
    uint8_t hdrlen = get_802154_hdr_len(&frame);
    //uint8_t buf[hdrlen + frame.payload_len];
    printf("hdrlen: %02x\n",hdrlen); 
    //if(check_802154_hdr_len(hdrlen)){
    memset(&buf,0,PAYLOAD_SIZE);
    uint8_t index = init_802154_frame(&frame,&buf);
    memcpy(&buf[hdrlen],frame.payload,frame.payload_len);
        //p.length = get_802154_pkt_len(); 
        //p.data = get_802154_hdr_ptr(); 
    //}

    p.length = PAYLOAD_SIZE;
    p.dst = addr;
    // TODO: geeignete ring-bufferung nötig
    p.data = buf;

    //p.data = snd_buffer[i % RADIO_SND_BUF_SIZE];
    msg_send(&mesg, transceiver_pid, 1);
    //swtimer_usleep(RADIO_SENDING_DELAY);
}

uint8_t sixlowmac_init(transceiver_type_t type){
    int recv_pid = thread_create(radio_stack_buffer, RADIO_STACK_SIZE, 
                        PRIORITY_MAIN-1, CREATE_STACKTEST, recv_ieee802154_frame , "radio");
    transceiver_type = type;
    transceiver_init(transceiver_type);
    transceiver_start();
    transceiver_register(type, recv_pid);

    macdsn = rand() % 256;    
}
