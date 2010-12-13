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

static uint8_t macdsn;
static uint8_t macbsn;

static radio_packet_t p;
static msg mesg;
int transceiver_type;
static transceiver_command_t tcmd;

uint16_t get_radio_address(ieee_802154_long_t *lla){
    return ((lla->uint8[6] << 8) + lla->uint8[7]);
}

void init_802154_short_addr(ieee_802154_short_t *saddr){
    saddr->uint8[0] = 0;
//    saddr->uint8[1] = get_address(tcmd.transceivers); 
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
//    laddr->uint8[7] = get_address(tcmd.transceivers);
}

void recv_ieee802154_frame(void){
    msg m;
    radio_packet_t *p;
    uint8_t i, hdrlen;
    ieee802154_frame_t *frame;

    msg_init_queue(msg_q, RADIO_RCV_BUF_SIZE);

    while (1) {
        msg_receive(&m);
        if (m.type == PKT_PENDING) {
            p = (radio_packet_t*) m.content.ptr;

            clear_802154_pkt();
            hdrlen = read_802154_frame(p->data, frame, p->length);

            for (i = 0; i < p->length; i++) {
                printf("%02X ", p->data[i]);
            }
    
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
}

void set_ieee802154_frame_values(ieee802154_frame_t *frame){
    // TODO: addresse aus ip paket auslesen und in frame einfuegen
    frame->dest_pan_id = IEEE_802154_PAN_ID;
    frame->src_pan_id = IEEE_802154_PAN_ID;
    frame->seq_nr = macdsn;
     
}

void send_ieee802154_frame(void){
    unsigned int c, i;

    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = transceiver_type;
    tcmd.data = &p;
    
    ieee802154_frame_t frame; 

    memset(&frame, 0, sizeof(frame));
    //init_ieee802154_fcf(&frame);
     
    

    char *hello = "helloworld";

    uint8_t *buf = (uint8_t*)hello;

    uint8_t hdrlen = get_802154_hdr_len(&frame);
    if(check_802154_hdr_len(hdrlen)){
        init_802154_frame(&frame,buf);
        p.length = get_802154_pkt_len(); 
        p.data = get_802154_hdr_ptr(); 
    }

    //p.length = PAYLOAD_SIZE;
    p.dst = 0;
    // TODO: geeignete ring-bufferung nötig
    i = 1;

    p.data = snd_buffer[i % RADIO_SND_BUF_SIZE];
    msg_send(&mesg, transceiver_pid, 1);
    swtimer_usleep(RADIO_SENDING_DELAY);
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
