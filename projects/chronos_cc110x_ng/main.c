#include <stdio.h>
#include <string.h>

#include <display.h>

#include <thread.h>
#include <board.h>
#include <hwtimer.h>
#include <swtimer.h>
#include <msg.h>
#include <transceiver.h>
#include <cc1100_ng.h>

#define RADIO_STACK_SIZE    (1024)
#define SEND_SIZE   CC1100_MAX_DATA_LENGTH

#define SND_BUFFER_SIZE     (3)
#define RCV_BUFFER_SIZE     (4)

#define SENDING_DELAY       (5 * 1000)

char radio_stack_buffer[RADIO_STACK_SIZE];

uint8_t snd_buffer[SND_BUFFER_SIZE][SEND_SIZE];

msg msg_q[RCV_BUFFER_SIZE];

static msg mesg;
static transceiver_command_t tcmd;
static radio_packet_t p;

void sender(char *count);

void sender(char *count) {
    unsigned int c = 3;
    unsigned int i;

    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;

    p.length = CC1100_MAX_DATA_LENGTH;
    p.dst = 0;

    for (i = 0; i < c; i++) {
        display_chars(LCD_SEG_L1_3_0, ".....", SEG_OFF);
        display_chars(LCD_SEG_L1_3_0, (char*) itoa(i, 1, 0), SEG_ON);
        p.data = snd_buffer[i % SND_BUFFER_SIZE];
        msg_send(&mesg, transceiver_pid, 1);
        hwtimer_wait(SENDING_DELAY);
    }
}


void radio(void) {
    msg m;
    radio_packet_t *p;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);
        if (m.type == PKT_PENDING) {
            p = (radio_packet_t*) m.content.ptr;
            display_chars(LCD_SEG_L2_5_0, (char*) itoa(p->length, 2, 0), SEG_ON);
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
    uint8_t addr = 43;
    uint8_t i;
    for (i = 0; i < SND_BUFFER_SIZE; i++) { 
        memset(snd_buffer[i], i, SEND_SIZE);
    }
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

    sender(NULL);

   while (1) {
   }
}
