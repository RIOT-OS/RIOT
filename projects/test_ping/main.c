#include <main.h>

//standard stuff
#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include <thread.h>

//shell stuff
#include <posix_io.h>
#include <shell.h>
#include <shell_commands.h>
#include <board_uart0.h>

//net stuff
#include <transceiver.h>
#include <cc1100.h>
#include <pingpong.h>

#define SHELL_STACK (4500)
#define RADIO_STACK (2048)

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)

char stack_shell[SHELL_STACK];
char stack_radio[RADIO_STACK];

uint8_t snd_buffer[SND_BUFFER_SIZE][CC1100_MAX_DATA_LENGTH];

msg_t msg_q[RCV_BUFFER_SIZE];

transceiver_command_t tcmd;
msg_t mesg;

static bool isinit = false;
static int radio_pid;

// Shell commands for this application
const shell_command_t shell_commands[] =
        { { "help", "Prints the help", help },
                { "init",
                        "Initializes this node with an address and a channel.",
                        init },
                { "broadcast", "Puts this node into broadcast mode", broadcast },
                { NULL, NULL, NULL }
        };

// see header for documentation
void init(char* arg) {
    uint16_t addr = 0;
    uint16_t chan = 10;

    int res = sscanf(arg, "init %hu %hu", &addr, &chan);
    if (res > 0) {
        puts("Setting address and channel");

        if (addr >= MIN_ADDR && addr <= MAX_ADDR
                && chan >= MIN_CHAN && chan <= MAX_CHAN) {
            // check validity of address and channel

            if (!radio_pid) {
                radio_pid = thread_create(stack_radio, RADIO_STACK,
                        PRIORITY_MAIN - 2, CREATE_STACKTEST, radio, "radio");
                printf("radio pid is %d.\n", radio_pid);
            }

            if (!transceiver_pid) {
                puts("starting up transceiver..");
                transceiver_init(TRANSCEIVER_CC1100);
                transceiver_start();
                transceiver_register(TRANSCEIVER_CC1100, radio_pid);
                puts("started and registered the transceiver.");
                printf("transceiver pid is %d.\n", transceiver_pid);
            }

            set_radio_address((uint8_t) addr);
            set_radio_channel((uint8_t) chan);

            printf("radio address is %d\n", get_radio_address());
            isinit = true;
        } else {
            printf("ERROR: The address for the node has to be in %d to %d.\n",
                    MIN_ADDR, MAX_ADDR);
            printf("ERROR: The channel for the node has to be in %d to %d.\n",
                    MIN_CHAN, MAX_CHAN);
        }
    } else {
        //no correct argument given, but radio address is mandatory
        puts("Usage: init [radioaddr] (radiochannel)");
        puts("Use 'help init' for more information on this command.");
    }
}

// see header for documentation
void help(char* cmdname) {
    char *command = cmdname;
    while (*command != '\0' && *command++ != ' ')
        ; //Skip the command by going till first space

    if (strlen(cmdname) == 4) {
        puts("These are the usable commands:");
        puts("\thelp (commandname)");
        puts("\tinit [address] (channel)");
        puts("");
        puts("[] = mandatory, () = optional");
    } else if (!strcasecmp("init", command)) {
        puts("init: Initializes a node for radio communication.");
        puts(" +");
        puts(
                " +---[address]: The radio-address that this node should assume (range: 0 to 255)");
        puts(" +              This argument is mandatory.");
        puts(" +");
        puts(" +---(channel): The radio-channel that this node should use");
        puts("                This argument is optional.");
        puts("                Uses a default channel if not given.");
    } else {
        puts("The command given was not recognized. You gave:");
        puts(command);
        puts("Recognized command is 'init'");
    }
}

// see header for documentation
void broadcast(char* arg) {
    uint16_t duration;

    if (!isinit) {
        // don't try to send without proper init
        puts("[ERROR] Cannot broadcast while radio is not initialized!");
        return;
    }

    int res = sscanf(arg, "broadcast %hu", &duration);

    if (res > 0) {
        if (duration < MAX_ADDR) {
            printf("Ready to broadcast for a duration of %d seconds\n",
                    duration);

            broadcast_without_ack(duration);

        } else {
            printf("ERROR: Please give a duration which is in range %d to %d.",
                    MIN_DURATION, MAX_DURATION);
        }
    } else {
        puts("ERROR: Please give a duration for which you wish to broadcast.");
        puts(
                "For more information on how to use broadcast, type 'help broadcast'.");
    }

}

// see header for documentation
void set_radio_address(uint8_t address) {
    uint16_t addr = (uint16_t) address;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &addr;
    mesg.content.ptr = (char*) &tcmd;
    mesg.type = SET_ADDRESS;
    msg_send_receive(&mesg, &mesg, transceiver_pid);
}

// see header for documentation
uint16_t get_radio_address(void) {
    uint16_t result;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &result;
    mesg.content.ptr = (char*) &tcmd;
    mesg.type = GET_ADDRESS;
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    return result;
}

// see header for documentation
void set_radio_channel(uint8_t channel) {
    uint16_t chan = (uint16_t) channel;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &chan;
    mesg.content.ptr = (char*) &tcmd;
    mesg.type = SET_CHANNEL;
    msg_send_receive(&mesg, &mesg, transceiver_pid);
}

// see header for documentation
void radio(void) {
    msg_t m;
    radio_packet_t *p;
    ping_packet_t *ping_pkt;
    msg_init_queue(msg_q, RCV_BUFFER_SIZE);

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t*) m.content.ptr;
            ping_pkt = (ping_packet_t*) p->data;

            printf("Packet waiting, process %p...\n", p);
            printf("\tLength:\t%u\n", p->length);
            printf("\tSrc:\t%u\n", p->src);
            printf("\tDst:\t%u\n", p->dst);
            printf("\tLQI:\t%u\n", p->lqi);
            printf("\tRSSI:\t%u\n", p->rssi);
            printf("Type: %d, ", ping_pkt->type);
            printf("Seq#: %d\n", ping_pkt->seq_nr);

            if (ping_pkt->type == PING_BCST) {
                broadcast_incoming();
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

//Runs the shell
void shell_runner(void) {
    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);
}

/**
 * The main function for this application, starts the shell and waits for
 * commands.
 */
int main(void) {
    puts("Ping Test Application\n");
    puts("For commands type 'help'!\n");

    thread_create(stack_shell, SHELL_STACK, PRIORITY_MAIN - 1,
            CREATE_STACKTEST, shell_runner, "shell");

    while (true) {
        //sleep a sec
        vtimer_usleep(1 * SECOND);
    }

    return 0;
}

