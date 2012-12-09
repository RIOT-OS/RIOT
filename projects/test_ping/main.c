#include <main.h>

#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include <thread.h>

#include <posix_io.h>
#include <shell.h>
#include <shell_commands.h>
#include <board_uart0.h>
#include <transceiver.h>

#define STACKSIZE KERNEL_CONF_STACKSIZE_DEFAULT

char stack[STACKSIZE];

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
    transceiver_command_t tcmd;
    msg_t m;
    uint16_t addr = 0;
    uint16_t chan = 10;

    printf("transceiver pid is %d.\n", transceiver_pid);

    int res = sscanf(arg, "init %hu %hu", &addr, &chan);
    if (res == 1) {
        puts("Setting address, keeping default channel");

        if (addr >= MIN_ADDR && addr <= MAX_ADDR
                && chan >= MIN_CHAN && chan <= MAX_CHAN) {
            //addr. given, channel should be default, which is 10 for us
            //check for validity in any case, just to be sure


            puts("Initializing 6lowpan..");
            sixlowpan_init(TRANSCEIVER_CC1100,addr,0);
            puts("Address set, setting channel");

            tcmd.data = &chan;
            m.type = SET_CHANNEL;
            m.content.ptr = (void*) &tcmd;

            puts("waiting for transceiver..");
            msg_send_receive(&m, &m, transceiver_pid);
            puts("Channel set.");

        } else {
            printf("ERROR: The address for the node has to be in %d to %d.\n",
                    MIN_ADDR, MAX_ADDR);
        }

    } else if (res == 2) {
        puts("Setting address and channel");

        if (addr >= MIN_ADDR && addr <= MAX_ADDR
                && chan >= MIN_CHAN && chan <= MAX_CHAN) {
            //addr. & channel are given, check validity

            puts("Initializing 6lowpan..");
            sixlowpan_init(TRANSCEIVER_CC1100,addr,0);
            puts("address set, setting channel");

            tcmd.data = &chan;
            m.type = SET_CHANNEL;
            m.content.ptr = (void*) &tcmd;

            puts("waiting for transceiver..");
            msg_send_receive(&m, &m, transceiver_pid);
            puts("channel set");
        } else {
            printf("ERROR: The address for the node has to be in %d to %d.\n",
                    MIN_ADDR, MAX_ADDR);
            printf("ERROR: The channel for the node has to be in %d to %d.\n",
                    MIN_CHAN, MAX_CHAN);
        }
        //addr and channel given
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
void ping() {
    //TODO implement
}

void stop(char* unused) {
    //TODO implement
    if (pid_pingthread) {
    }
}

void hellothread(){
    while(true){
        puts("Hello!");
        vtimer_usleep(1000*2000); //2secs
    }
}

/**
 * The main function for this application, starts the shell and waits for
 * commands.
 */
int main(void) {
    puts("Ping Test Application\n");
    puts("For commands type 'help'!\n");

    thread_create(stack, STACKSIZE, PRIORITY_MAIN -3,8, hellothread,"hello");

    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}

