/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#include "board.h"

#include "thread.h"
#include "xtimer.h"

#include "periph/pm.h"

#include "ps.h"

char ledblink_thread_stack[160];
/* set interval to 4 second */
#define INTERVAL (10U * US_PER_SEC)

void *ledblink_thread(void *arg)
{
	xtimer_ticks32_t last_wakeup = xtimer_now();

	while(1){
		// All LED OFF
		LED_PORT |= BLUE|GREEN|RED;
		/* Green LED ON, it will be shut off in pm_set_lowest*/
		LED_PORT &= ~GREEN;

		// ps();
		// puts("ledblink_thread timer expired. Text for long LED Illumination");
		 printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));
		xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

		// xtimer_usleep(4000000);// 4 seconds wait
	}
	return NULL;
}

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int udp_cmd(int argc, char **argv);

//int reboot_cmd(int argc, char **argv){
//	pm_reboot();
//	return 0;
//}

static const shell_command_t shell_commands[] = {
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
	//{ "reboot", "reboot node", reboot_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
	LED_PORT |= BLUE|GREEN|RED;
	LED_PORT ^= RED;

	thread_create(ledblink_thread_stack, sizeof(ledblink_thread_stack),
				THREAD_PRIORITY_MAIN +1,
				THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
				ledblink_thread, NULL, "ledblink_thread");

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    puts("Error: END of main().");
    /* should be never reached */
    while(1){}
    return 0;
}
