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

#define SHELL_STACK (2048)
#define RADIO_STACK (2048)

#define SND_BUFFER_SIZE     (100)
#define RCV_BUFFER_SIZE     (64)

char stack_shell[SHELL_STACK];
char stack_radio[RADIO_STACK];

uint8_t snd_buffer[SND_BUFFER_SIZE][CC1100_MAX_DATA_LENGTH];

msg_t msg_q[RCV_BUFFER_SIZE];

transceiver_command_t tcmd;
msg_t mesg;

static int radio_pid;
static radio_packet_t p;

// Shell commands for this application
const shell_command_t shell_commands[] =
        { { "help", "Prints the help", help },
                { "init",
                        "Initializes this node with an address and a channel.",
                        init },
                { "ping", "Makes this node a pinging node", ping },
                { "stop", "Stops the current node's pings and prints a summary",
                        stop },
                { NULL, NULL, NULL }
        };

// pid of ongoing ping thread, 0 if not set
int pid_pingthread = 0;

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
            puts("turning on..");

            switch_to_rx();

            puts("done");
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
        puts("\tping [address] (time)");
        puts("\tstop");
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
    } else if (!strcasecmp("ping", command)) {
        puts(
                "ping: Sends ping-messages to another node and records statistics on the number");
        puts(
                " +    of sent messages/received messages as well as the RTT of those pings.");
        puts(" +");
        puts(
                " +---[address]: The radio-address that this node should send its pings to)");
        puts(" +              This argument is mandatory.");
        puts(" +");
        puts(
                " +---(time)   : The duration (in seconds) that these ping messages should ");
        puts("                be sent over");
        puts("                This argument is optional.");
        puts("                Sends infinite pings when no time is given.");
    } else if (!strcasecmp("stop", command)) {
        puts("stop: Stops any ongoing pings this node sends out.");
        puts(
                "      If this node is currently not sending any pings, this command does nothing.");
    } else {
        puts("The command given was not recognized. You gave:");
        puts(command);
        puts("Recognized commands are 'init','ping' and 'stop'");
    }
}

// see header for docs
void ping(char* arg) {
    uint16_t addr;

    int res = sscanf(arg, "ping %hu", &addr);

    if (res > 0) {
        if (addr < MAX_ADDR) {
            printf("Ready to send to address %d\n", addr);
            //todo do something that makes sense actually
            mesg.type = SND_PKT;
            mesg.content.ptr = (char*) &tcmd;

            tcmd.transceivers = TRANSCEIVER_CC1100;
            tcmd.data = &p;

            p.length = 4;
            p.dst = addr;

            puts("sending..");
            p.data = "ping";
            msg_send(&mesg, transceiver_pid, 1);
            puts("sent");
        } else {
            printf("ERROR: Please give an address which is in range %d to %d.",
                    MIN_ADDR, MAX_ADDR);
        }
    } else {
        puts("ERROR: Please give an address which you wish to ping.");
        puts("For more information on how to use ping, type 'help ping'.");
    }
}

void set_radio_address(uint8_t address) {
    uint16_t addr = (uint16_t) address;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &addr;
    mesg.content.ptr = (char*) &tcmd;
    mesg.type = SET_ADDRESS;
    msg_send_receive(&mesg, &mesg, transceiver_pid);
}

uint16_t get_radio_address(void) {
    uint16_t result;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &result;
    mesg.content.ptr = (char*) &tcmd;
    mesg.type = GET_ADDRESS;
    msg_send_receive(&mesg, &mesg, transceiver_pid);
    return result;
}

void set_radio_channel(uint8_t channel) {
    uint16_t chan = (uint16_t) channel;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &chan;
    mesg.content.ptr = (char*) &tcmd;
    mesg.type = SET_CHANNEL;
    msg_send_receive(&mesg, &mesg, transceiver_pid);
}

void switch_to_rx(void) {
    mesg.type = SWITCH_RX;
    mesg.content.ptr = (char*) &tcmd;
    tcmd.transceivers = TRANSCEIVER_CC1100;
    msg_send(&mesg, transceiver_pid, 1);
}

void powerdown(char *unused) {
    mesg.type = POWERDOWN;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    msg_send(&mesg, transceiver_pid, 1);
}

void stop(char* unused) {
    //TODO implement
    if (pid_pingthread) {
    }
}

void radio(void) {
    msg_t m;
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
            printf("\tLQI:\t%u\n", p->lqi);
            printf("\tRSSI:\t%u\n", p->rssi);

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
        vtimer_usleep(1000 * 1000);
    }

    return 0;
}

