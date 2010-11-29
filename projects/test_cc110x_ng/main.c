#include <stdio.h>
#include <string.h>

#include <shell.h>
#include <board_uart0.h>
#include <posix_io.h>
#include <thread.h>
#include <board.h>
#include <hwtimer.h>
#include <swtimer.h>
#include <msg.h>
#include <transceiver.h>
#include <cc1100_ng.h>

#define SHELL_STACK_SIZE    (2048)
#define RADIO_STACK_SIZE    (2048)

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)

#define SENDING_DELAY       (1000)

char shell_stack_buffer[SHELL_STACK_SIZE];
char radio_stack_buffer[RADIO_STACK_SIZE];

uint8_t snd_buffer[SND_BUFFER_SIZE][CC1100_MAX_DATA_LENGTH];

msg msg_q[RCV_BUFFER_SIZE];

static msg mesg;
static transceiver_command_t tcmd;
static radio_packet_t p;

void sender(char *count);
void print_buffer(char *unused);

shell_t shell;
const shell_command_t sc[] = {
    {"snd", "", sender},
    {"buffer", "", print_buffer},
    {NULL, NULL, NULL}};

void shell_runner(void) {
    shell_init(&shell, sc, uart0_readc, uart0_putc);
    posix_open(uart0_handler_pid, 0);
    shell_run(&shell);
}

void sender(char *count) {
    unsigned int c, i;

    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;

    p.length = CC1100_MAX_DATA_LENGTH;
    p.dst = 0;

    sscanf(count, "snd %u", &c);
    for (i = 0; i < c; i++) {
        puts(".");
        p.data = snd_buffer[i % SND_BUFFER_SIZE];
        msg_send(&mesg, transceiver_pid, 1);
        swtimer_usleep(SENDING_DELAY);
    }
}

void print_buffer(char *unused) {
    uint8_t i;
    extern radio_packet_t transceiver_buffer[];
    for (i = 0; i < TRANSCEIVER_BUFFER_SIZE; i++) {
        printf("[%u] %u # %u # %u\n", i, transceiver_buffer[i].processing, transceiver_buffer[i].length, transceiver_buffer[i].data[i]);
    }
    extern rx_buffer_t cc1100_rx_buffer[];
    for (i = 0; i < TRANSCEIVER_BUFFER_SIZE; i++) {
        printf("[%u] %u # %u \n", i, cc1100_rx_buffer[i].packet.length, cc1100_rx_buffer[i].packet.data[i]);
    }
}

void radio(void) {
    msg m;
    radio_packet_t *p;
    uint8_t i;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);
        if (m.type == PKT_PENDING) {
            p = (radio_packet_t*) m.content.ptr;
            printf("Packet waiting, process %p...\n", p);
            printf("\tLength:\t%u\n", p->length);
            printf("\tSrc:\t%u\n", p->src);
            printf("\tDst:\t%u\n", p->dst);

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

int main(void) {
    int radio_pid;
    uint8_t i;
    for (i = 0; i < SND_BUFFER_SIZE; i++) { 
        memset(snd_buffer[i], i, CC1100_MAX_DATA_LENGTH);
    }
    thread_create(shell_stack_buffer, SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");
    radio_pid = thread_create(radio_stack_buffer, RADIO_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, radio, "radio");
    transceiver_init(TRANSCEIVER_CC1100);
    transceiver_start();
    transceiver_register(TRANSCEIVER_CC1100, radio_pid);
   
   while (1) {
//       LED_GREEN_TOGGLE;
       hwtimer_wait(1000 * 1000);
   }
}
