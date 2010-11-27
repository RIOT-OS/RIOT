#include <stdio.h>
#include <string.h>

#include <shell.h>
#include <board_uart0.h>
#include <posix_io.h>
#include <thread.h>
#include <board.h>
#include <hwtimer.h>
#include <msg.h>
#include <transceiver.h>
#include <cc1100_ng.h>

#define SHELL_STACK_SIZE    (2048)
#define RADIO_STACK_SIZE    (2048)

int transceiver_pid;
char shell_stack_buffer[SHELL_STACK_SIZE];
char radio_stack_buffer[RADIO_STACK_SIZE];

void mon_handler(char *mode);

shell_t shell;
const shell_command_t sc[] = {
    {"mon", "", mon_handler},
    {NULL, NULL, NULL}};

void mon_handler(char *mode) {
    unsigned int m;

    sscanf(mode, "mon %u", &m);
    printf("Setting monitor mode: %u\n", m);
    cc1100_set_monitor(m);
}

void shell_runner(void) {
    shell_init(&shell, sc, uart0_readc, uart0_putc);
    posix_open(uart0_handler_pid, 0);
    shell_run(&shell);
}

void radio(void) {
    msg m;
    radio_packet_t *p;
    uint8_t i;

    while (1) {
        msg_receive(&m);
        printf("Received message of type %i:\n", m.type);
        if (m.type == PKT_PENDING) {
            p = (radio_packet_t*) m.content.ptr;
            printf("Packet waiting, process...\n");
            printf("\tLength:\t%u\n", p->length);
            printf("\tSrc:\t%u\n", p->src);
            printf("\tDst:\t%u\n", p->dst);

            for (i = 0; i < p->length; i++) {
                printf("%02X ", p->data[i]);
            }
            printf("\n");
        }
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
