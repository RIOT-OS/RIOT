#include "thread.h"
#include <stdio.h>
#include "xtimer.h"
#include "rgbled.h"

char rcv_thread_stack[THREAD_STACKSIZE_MAIN];


void *rcv_thread(void *arg)
{
	/*cycle through rgbled color */
	color_rgb_t my_color = {1,1,0};
	rgbled_t my_rgbled;
	pwm_t my_pwm = PWM_1;
	rgbled_init(&my_rgbled, my_pwm, 0, 1, 2);
	int8_t scale = 1;
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
			xtimer_usleep(50000);
		}
		while(my_color.g < 255 && my_color.g >0) {
			uint16_t new_green_value = my_color.g + scale*2;
			if(new_green_value >=255){
				new_green_value = 255;
			}else if(new_green_value < 0){
				new_green_value = 0;
			}
			my_color.g = new_green_value;
			rgbled_set(&my_rgbled, &my_color);
			xtimer_usleep(50000);
		}
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

int main(void)
{
	thread_create(rcv_thread_stack, sizeof(rcv_thread_stack),
			THREAD_PRIORITY_MAIN -1, 0,
			rcv_thread, NULL, "rcv_thread");
	return 0;
}
