#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include <thread.h>

#include <posix_io.h>
#include <shell.h>
#include <shell_commands.h>
#include <board_uart0.h>

#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowpan.h"
#include "sys/net/sixlowpan/sixlowerror.h"
#include "sys/net/sixlowpan/rpl/rpl.h"
#include "sys/net/sixlowpan/rpl/rpl_dodag.h"

#define TR_WD_STACKSIZE     (256)

char tr_wd_stack[TR_WD_STACKSIZE];

void wakeup_thread(void) {
    while (1) {
        if (thread_getstatus(transceiver_pid) == STATUS_SLEEPING) {
            vtimer_usleep(100 * 1000);
            if (thread_getstatus(transceiver_pid) == STATUS_SLEEPING) {
                puts("WAKEUP!");
                thread_wakeup(transceiver_pid);
            }
        }
        vtimer_usleep(1000 * 1000);

    }
}

void init(char *str){
    transceiver_command_t tcmd;
    msg_t m;
    uint8_t chan = 10;

    char command;
    uint16_t r_addr;

    int res = sscanf(str, "init %c %hu", &command, &r_addr);

    if(res < 1){
        printf("Usage: init address\n");
        printf("\tr\tinitialize as root\n");
        printf("\tn\tinitialize as node router\n");
        printf("\taddress must be an 8 bit integer\n");
    }

	uint8_t state;
    switch (command) {
        case 'r':
            printf("INFO: Initialize as root on address %d\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: address not an 8 bit integer\n");
                return;
            }
			state = rpl_init(TRANSCEIVER_CC1100, r_addr);
			if(state != SUCCESS){
				printf("Error initializing RPL\n");
			}
			rpl_init_root();
            break;
        case 'n':
            printf("INFO: Initialize as node on address %d\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: address not an 8 bit integer\n");
                return;
            }
            state = rpl_init(TRANSCEIVER_CC1100, r_addr);
            if(state != SUCCESS){
                printf("Error initializing RPL\n");
            }
            break;
        default:
            printf("ERROR: Unknown command '%c'\n", command);
            return;
    }

    /* set channel to 10 */
    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &chan;
    m.type = SET_CHANNEL;
    m.content.ptr = (void*) &tcmd;

    msg_send_receive(&m, &m, transceiver_pid);

    /* start transceiver watchdog */
    thread_create(tr_wd_stack, TR_WD_STACKSIZE, PRIORITY_MAIN-3, CREATE_STACKTEST, wakeup_thread, "TX/RX WD");

 }

void table(char *str){
	rpl_routing_entry_t * rtable;
	rtable = rpl_get_routing_table();
	printf("---------------------------\n");
	printf("OUTPUT\n");
	printf("---------------------------\n");
	for(int i=0;i<RPL_MAX_ROUTING_ENTRIES;i++){
		if(rtable[i].used){
			ipv6_print_addr(&rtable[i].address);
			printf("--------------\n");
		}
	}
}

void dodag(char *str){
	printf("---------------------------\n");
	rpl_dodag_t * mydodag = rpl_get_my_dodag();
	if(mydodag == NULL){
		printf("Not part of a dodag\n");
	printf("---------------------------\n");
		return;
	}
	printf("Part of Dodag:\n");
	ipv6_print_addr(&mydodag->dodag_id);
	printf("my rank: %d\n", mydodag->my_rank);
	printf("my preferred parent:\n");
	ipv6_print_addr(&mydodag->my_preferred_parent->addr);
	printf("---------------------------\n");
}

extern void cc1100_print_config(void);
extern void cc1100_print_statistic(void);

void cc1100_cfg(char* unused) {
    cc1100_print_config();
    puts("=============================================");
    cc1100_print_statistic();
}


void wakeup(char* unused) {
    thread_wakeup(10);
}

const shell_command_t shell_commands[] = {
    {"init", "", init},
    {"table", "", table},
    {"dodag", "", dodag},
    {"cc1100", "", cc1100_cfg},
    {"wakeup", "", wakeup},
	{NULL, NULL, NULL}
};

int main(void) {
    printf("RPL Test Application\n");
    

    posix_open(uart0_handler_pid, 0);

    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}



int old_main(void)
{
	timex_t mytime = timex_set(10,0);
    while(1){
		
		rpl_routing_entry_t * rtable;
		rtable = rpl_get_routing_table();
		printf("---------------------------\n");
		printf("OUTPUT\n");
		printf("---------------------------\n");
		for(int i=0;i<RPL_MAX_ROUTING_ENTRIES;i++){
			if(rtable[i].used){
				ipv6_print_addr(&rtable[i].address);
				printf("--------------\n");
			}
		}
		
		vtimer_sleep(mytime);	
	};
}

