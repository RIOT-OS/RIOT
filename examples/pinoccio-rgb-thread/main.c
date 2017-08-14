#define ENABLE_DEBUG    (1)
#include "thread.h"
#include "debug.h"
#include <stdio.h>
#include "xtimer.h"
#include "rgbled.h"

#include "shell.h"
#include "msg.h"

char rcv_thread_stack[THREAD_STACKSIZE_MAIN];
/* available times for xtimer
 *
 * clock is 250000Hz
 * minimum interval is 4µs
 * */

/* set interval to some seconds */
#define LED_INTERVAL (5U * US_PER_MS)

kernel_pid_t rcv_thread_pid;

void *rcv_thread(void *arg)
{
	/*cycle through rgbled color */
	color_rgb_t my_color = {1,1,0};
	rgbled_t my_rgbled;
	pwm_t my_pwm = PWM_1;
	rgbled_init(&my_rgbled, my_pwm, 0, 1, 2);
	int8_t scale = 1;

//	xtimer_ticks32_t last_wakeup = xtimer_now();

	/*only cycle through red and green since blue isnt woking on pinoccio board yet */
	while(1){
		while(my_color.r < 255 && my_color.r >0) {
			int16_t new_red_value = my_color.r + scale*5;
			if(new_red_value >=255){
				new_red_value = 255;
			}else if(new_red_value < 0){
				new_red_value = 0;
			}
			my_color.r = new_red_value;
			rgbled_set(&my_rgbled, &my_color);

			xtimer_usleep(LED_INTERVAL);

			// xtimer_periodic_wakeup(&last_wakeup, LED_INTERVAL);
		}

		printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()) );
		while(my_color.g < 255 && my_color.g >0) {
			uint16_t new_green_value = my_color.g + scale*2;
			if(new_green_value >=255){
				new_green_value = 255;
			}else if(new_green_value < 0){
				new_green_value = 0;
			}
			my_color.g = new_green_value;
			rgbled_set(&my_rgbled, &my_color);

			xtimer_usleep(LED_INTERVAL);
		}
		printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()) );

		/*change scale */
		if(scale == 1) {
			scale = -1;
		}else{
			scale = 1;
		}
		my_color.r += scale*5;
		my_color.g += scale*2;
	}
	return NULL;
}

/* set interval to some seconds */
#define MAIN_INTERVAL (1U * US_PER_SEC)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int time_cmd(int argc, char **argv){
	printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));
	return 0;
}
int run_thread_cmd(int argc, char **argv){

	thread_wakeup(rcv_thread_pid);
	return 0;
}
static const shell_command_t shell_commands[] = {
	{ "time", "reports time", time_cmd },
	{ "run", "runs led thread", run_thread_cmd },
    { NULL, NULL, NULL }
};

int main(void)
{
	/*cycle through rgbled color */
	rcv_thread_pid = thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
			THREAD_PRIORITY_MAIN - 1,
			THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
			rcv_thread, NULL, "led_thread");

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

//	xtimer_ticks32_t last_wakeup = xtimer_now();
//
//	while(1)
//	{
//		printf("slept until %" PRIu32 "\n", xtimer_usec_from_ticks(xtimer_now()));
//		xtimer_periodic_wakeup(&last_wakeup, MAIN_INTERVAL);
//	}
//	/* never reach here */
//	printf("ERROR: main end reached.\n");
	return 0;
}
