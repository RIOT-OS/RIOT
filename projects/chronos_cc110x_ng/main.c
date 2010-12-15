#include <stdio.h>
#include <string.h>
#include <time.h>

#include <display.h>

#include <thread.h>
#include <board.h>
#include <hwtimer.h>
#include <msg.h>
#include <transceiver.h>
#include <cc110x_ng.h>

#define RADIO_STACK_SIZE    (512)
#define SEND_SIZE   CC1100_MAX_DATA_LENGTH

#define RCV_BUFFER_SIZE     (4)

#define SENDING_DELAY       (5 * 1000)

char radio_stack_buffer[RADIO_STACK_SIZE];

uint8_t snd_buffer[SEND_SIZE];

msg msg_q[RCV_BUFFER_SIZE];

static msg mesg;
static transceiver_command_t tcmd;
static radio_packet_t p;

void send(radio_address_t dst, uint8_t len, uint8_t *data);

void send(radio_address_t dst, uint8_t len, uint8_t *data) {
    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;

    p.length = len;
    p.dst = dst;
    display_chars(LCD_SEG_L2_5_0, "CC1100", SEG_OFF);
    display_chars(LCD_SEG_L2_5_0, (char*) itoa(p.dst, 6, 0), SEG_ON);

    p.data = data;
    msg_send(&mesg, transceiver_pid, 1);
}


void radio(void) {
    msg m;
    radio_packet_t *p;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);
        if (m.type == PKT_PENDING) {

            p = (radio_packet_t*) m.content.ptr;
            display_chars(LCD_SEG_L2_5_0, "CC1100", SEG_OFF);
            display_chars(LCD_SEG_L2_5_0, (char*) p->data, SEG_ON);
            send(p->src, p->length, p->data);
            

            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
        }
        else {
        }
    }
}

int main(void) {
    int radio_pid;

    radio_address_t addr = 43;
    memset(snd_buffer, 43, SEND_SIZE);
    radio_pid = thread_create(radio_stack_buffer, RADIO_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, radio, "radio");
    transceiver_init(TRANSCEIVER_CC1100);
    transceiver_start();
    transceiver_register(TRANSCEIVER_CC1100, radio_pid);

    lcd_init();
    clear_display_all();
    mesg.type = SET_ADDRESS;
    mesg.content.ptr = (char*) &tcmd;

    display_chars(LCD_SEG_L2_5_0, "CC1100", SEG_ON);
    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &addr;
    msg_send(&mesg, transceiver_pid, 1);

    send(0, SEND_SIZE, snd_buffer);

   while (1) {
        hwtimer_wait(SENDING_DELAY);
        display_chars(LCD_SEG_L1_3_0, itoa(hwtimer_now(), 4, 0), SEG_ON);
   }
}
