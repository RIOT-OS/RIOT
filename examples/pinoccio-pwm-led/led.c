#include "board.h"
#include "rgbled.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rgb_cmd(int argc, char **argv)
{
	if (argc < 2) {
	        printf("usage: %s [red|green|blue|custom]\n", argv[0]);
	        return 1;
	}

	color_rgb_t my_color = {0,0,0};
	rgbled_t my_rgbled;
	pwm_t my_pwm = PWM_0;
	rgbled_init(&my_rgbled, my_pwm, 0, 1, 2);

	if (strcmp(argv[1], "red") == 0) {
		my_color.r = 255;
	}
	else if(strcmp(argv[1], "green") == 0){
		my_color.g = 255;
	}
	else if(strcmp(argv[1], "blue") == 0){
		my_color.b = 255;
	}
	else if(strcmp(argv[1], "custom") == 0){
		if(argc < 5) {
			printf("usage: %s %s <red> <green> <blue>\n", argv[0],argv[1]);
			puts("red|green|blue should be between 0-255");
			return 1;
		}
		uint16_t red = (uint16_t) atoi(argv[2]);
		uint16_t green = (uint16_t) atoi(argv[3]);
		uint16_t blue = (uint16_t) atoi(argv[4]);
		if(red >255 || green > 255 || blue >255 ||
				red < 0 || green < 0 || blue< 0) {
			puts("red|green|blue should be between 0-255");
			return 1;
		}
		my_color.r = red;
		my_color.g = green;
		my_color.b = blue;
	}
	else {
		printf("usage: %s [red|green|blue|custom]\n", argv[0]);
	}
	rgbled_set(&my_rgbled, &my_color);
	return 0;
}
