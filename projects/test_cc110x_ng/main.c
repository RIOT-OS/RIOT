#include <stdio.h>
#include <shell.h>
#include <board_uart0.h>
#include <posix_io.h>
#include <thread.h>
#include <board.h>
#include <hwtimer.h>
#include <msg.h>
#include <transceiver.h>

#define SHELL_STACK_SIZE    (4096)
#define RADIO_STACK_SIZE    (4096)

int transceiver_pid;
char shell_stack_buffer[SHELL_STACK_SIZE];
char radio_stack_buffer[RADIO_STACK_SIZE];

void trans_chan(char *chan);
void trans_addr(char *addr);
void trans_send(char *mesg);

msg mesg;
transceiver_command_t tcmd;

shell_t shell;
const shell_command_t sc[] = {
    {"tchan", "Set the channel for cc1100", trans_chan},
    {"taddr", "Set the address for cc1100", trans_addr},
    {"tsnd", "Sends a CC1100 packet", trans_send},
    {NULL, NULL, NULL}};

void trans_chan(char *chan) {
    int c;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &c;
    mesg.content.ptr = (char*) &tcmd;
    if (sscanf(chan, "tchan %i", &c) > 0) {
        printf("Trying to set channel %i\n", c);
        mesg.type = SET_CHANNEL;
    }
    else {
        mesg.type = GET_CHANNEL;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("Got channel: %i\n", c);
}

void trans_addr(char *addr) {
    int a;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &a;
    mesg.content.ptr = (char*) &tcmd;
    if (sscanf(addr, "taddr %i", &a) > 0) {
        printf("Trying to set address %i\n", a);
        mesg.type = SET_ADDRESS;
    }
    else {
        mesg.type = GET_ADDRESS;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("Got address: %i\n", a);
}

void trans_send(char *text) {
    radio_packet_t p;
    uint32_t response;
    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &p;

    p.length = 10;
    p.dst = 5;
    
    mesg.type = SND_PKT;
    mesg.content.ptr = (char*) &tcmd;
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    response = mesg.content.value;
    printf("Packet send: %lu\n", response);
}

void shell_runner(void) {
    shell_init(&shell, sc, uart0_readc, uart0_putc);
    posix_open(uart0_handler_pid, 0);
    shell_run(&shell);
}

void radio(void) {
    msg m;

    while (1) {
        msg_receive(&m);
        printf("Received message of type %i: %lX\n", m.type, m.content.value);
    }
}

int main(void) {
    int radio_pid;
    thread_create(shell_stack_buffer, SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");
    radio_pid = thread_create(radio_stack_buffer, RADIO_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, radio, "radio");
    transceiver_init(TRANSCEIVER_CC1100);
    transceiver_pid = transceiver_start();
    transceiver_register(TRANSCEIVER_CC1100, radio_pid);
   
   while (1) {
//       LED_GREEN_TOGGLE;
       hwtimer_wait(1000 * 1000);
   }
}
