#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include <thread.h>

#include <posix_io.h>
#include <shell.h>
#include <board_uart0.h>

#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowpan.h"
#include "sys/net/sixlowpan/sixlowerror.h"
#include "sys/net/sixlowpan/rpl/rpl.h"

void init(char *str){
    char command;
    uint16_t r_addr;
    ipv6_addr_t std_addr;

    int res = sscanf(str, "init %c %hu", &command, &r_addr);

    if(res < 1){
        printf("Usage: init address\n");
        printf("\tr\tinitialize as root\n");
        printf("\tn\tinitialize as node router\n");
        printf("\taddress must be an 8 bit integer\n");
    }

	ipv6_init_address(&std_addr, 0xABCD,0,0,0,0x1234,0xFFFF,0xFEDC,r_addr);
	uint8_t state;
    switch (command) {
        case 'r':
            printf("INFO: Initialize as root on address \n");
			ipv6_print_addr(&std_addr);
            if (r_addr > 255) {
                printf("ERROR: address not an 8 bit integer\n");
                return;
            }
			state = rpl_init(TRANSCEIVER_CC1100, &std_addr);
			if(state != SUCCESS){
				printf("Error initializing RPL\n");
			}
			rpl_init_root();
            break;
        case 'n':
            printf("INFO: Initialize as node on address \n");
			ipv6_print_addr(&std_addr);
            if (r_addr > 255) {
                printf("ERROR: address not an 8 bit integer\n");
                return;
            }
            state = rpl_init(TRANSCEIVER_CC1100, &std_addr);
            if(state != SUCCESS){
                printf("Error initializing RPL\n");
            }
            break;
        default:
            printf("ERROR: Unknown command '%c'\n", command);
            break;
    }
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

const shell_command_t shell_commands[] = {
    {"init", "", init},
    {"table", "", table},
	{NULL, NULL, NULL}
};

int main(void) {
    printf("RPL Test Application\n");
    vtimer_init();

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

