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
int radio_pid;
char shell_stack_buffer[SHELL_STACK_SIZE];
char radio_stack_buffer[RADIO_STACK_SIZE];

void trans_chan(char *chan);
void trans_register(char *unused);

msg mesg;
transceiver_command_t tcmd;

shell_t shell;
const shell_command_t sc[] = {
    {"tchan", "Set the channel for cc1100", trans_chan},
    {"treg", "Register application for CC1100", trans_register},
    {NULL, NULL, NULL}};

void trans_chan(char *chan) {
    unsigned int c;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &c;
    mesg.content.ptr = (char*) &tcmd;
    if (sscanf(chan, "tchan %u", &c)) {
        printf("Trying to set channel %u\n", c);
        mesg.type = SET_CHANNEL;
    }
    else {
        mesg.type = GET_CHANNEL;
    }
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    printf("Got channel: %hu\n", c);
}

void trans_register(char *unused) {
    transceiver_register(TRANSCEIVER_CC1100, radio_pid);
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
   thread_create(shell_stack_buffer, SHELL_STACK_SIZE, PRIORITY_MAIN-1, CREATE_STACKTEST, shell_runner, "shell");
    radio_pid = thread_create(radio_stack_buffer, RADIO_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, radio, "radio");
   transceiver_init(TRANSCEIVER_CC1100);
   transceiver_pid = transceiver_start();
   extern void cc1100_set_channel(uint8_t c);
   cc1100_set_channel(6);
   
   while (1) {
//       LED_GREEN_TOGGLE;
       hwtimer_wait(1000 * 1000);
   }
}
