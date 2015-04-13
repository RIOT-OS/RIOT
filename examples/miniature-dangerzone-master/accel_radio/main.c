#include <stdio.h>

#include "thread.h"
#include "posix_io.h"
#include "shell.h"
#include "shell_commands.h"
#include "board_uart0.h"
#include "transceiver.h"
#include "debug.h"

#include "sense.h"

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)
#define RADIO_STACK_SIZE    (KERNEL_CONF_STACKSIZE_DEFAULT)

const shell_command_t shell_commands[] = {
    {NULL, NULL, NULL}
};

char radio_stack_buffer[RADIO_STACK_SIZE];
msg_t msg_q[RCV_BUFFER_SIZE];

void radio(void)
{
    msg_t m;
    radio_packet_t *p;
    radio_packet_length_t i;

    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t *) m.content.ptr;
            printf("Got radio packet:\n");
            printf("\tLength:\t%u\n", p->length);
            printf("\tSrc:\t%u\n", p->src);
            printf("\tDst:\t%u\n", p->dst);
            printf("\tLQI:\t%u\n", p->lqi);
            printf("\tRSSI:\t%u\n", p->rssi);

            for (i = 0; i < p->length; i++) {
                printf("%02X ", p->data[i]);
            }

            p->processing--;
            puts("\n");
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            puts("Unknown packet received");
        }
    }
}


void init_transceiver(void)
{
    int radio_pid = thread_create(
                        radio_stack_buffer,
                        RADIO_STACK_SIZE,
                        PRIORITY_MAIN - 2,
                        CREATE_STACKTEST,
                        radio,
                        "radio");

    uint16_t transceivers = TRANSCEIVER_DEFAULT;

    transceiver_init(transceivers);
    (void) transceiver_start();
    transceiver_register(transceivers, radio_pid);
}

int main(void)
{
    transceiver_command_t tcmd;
    msg_t m;
    uint8_t chan = 12;

    puts("Accel v0.1");

    /* start shell */
    posix_open(uart0_handler_pid, 0);

    sense_init(); 
    init_transceiver();
    LED_GREEN_OFF;
    LED_RED_ON;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &chan;
    m.type = SET_CHANNEL;
    m.content.ptr = (void *) &tcmd;
    msg_send_receive(&m, &m, transceiver_pid);
    printf("Channel set to %u\n", *m.content.ptr);

    shell_t shell;
    shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);

    shell_run(&shell);
    return 0;
}
