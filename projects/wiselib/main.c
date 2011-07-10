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
#include <cc110x_ng.h>

#define SHELL_STACK_SIZE    (2048)
#define RADIO_STACK_SIZE    (2048)

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)

#define SENDING_DELAY       (5 * 1000)

char shell_stack_buffer[SHELL_STACK_SIZE];
char radio_stack_buffer[RADIO_STACK_SIZE];

uint8_t snd_buffer[SND_BUFFER_SIZE][CC1100_MAX_DATA_LENGTH];

msg_t msg_q[RCV_BUFFER_SIZE];

static msg_t mesg;
static transceiver_command_t tcmd;
static radio_packet_t p;

static uint32_t sending_delay = SENDING_DELAY;

void sender(char *count);
void print_buffer(char *unused);
void switch2rx(char *unused);
void powerdown(char *unused);
void set_delay(char *delay);

int main(void){}
