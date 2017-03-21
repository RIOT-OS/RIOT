#define ENABLE_DEBUG    (1)

#include "board.h"
#include "rgbled.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int rgb_cmd(int argc, char **argv)
{
	DEBUG("rgb_cmd called");
	if (argc < 2) {
	        printf("usage: %s [r/g/b] [on/off/value] \n %s [on/off/value value value]", argv[0], argv[0]);
	        return 1;
	}

	color_rgb_t my_color = {0,0,0};
	rgbled_t my_rgbled;
	pwm_t my_pwm = PWM_1;
	rgbled_init(&my_rgbled, my_pwm, 0, 1, 2);
	if(argc == 3) {
		if (!strcmp(argv[1], "r")) {
			if(!strcmp(argv[2], "on")) {
				my_color.r = 255;
			}else if(!strcmp(argv[2], "off")) {
				my_color.r = 0;
			}else
			{
				uint16_t value = atoi(argv[2]);
				if(value > 255 || value < 0) {
					puts("color values should be between 0-255");
					return 1;
				}
				my_color.r = value;
			}
		}
		else if(!strcmp(argv[1], "g")){
			if(!strcmp(argv[2], "on")) {
				my_color.g = 255;
			}else if(!strcmp(argv[2], "off")) {
				my_color.g = 0;
			}else{
				uint16_t value = atoi(argv[2]);
				if(value > 255 || value < 0) {
					puts("color values should be between 0-255");
					return 1;
				}
				my_color.g = value;
			}
		}
		else if(!strcmp(argv[1], "b")){
			if(!strcmp(argv[2], "on")) {
				my_color.b = 255;
			}else if(!strcmp(argv[2], "off")) {
				my_color.b = 0;
			}else{
				uint16_t value = atoi(argv[2]);
				if(value > 255 || value < 0) {
					puts("color values should be between 0-255");
					return 1;
				}
				my_color.b = value;
			}
		}
	}else if(argc == 2){
		if(!strcmp(argv[1], "on")){
			my_color.r = 255;
			my_color.g = 255;
			my_color.b = 255;
		}else if(!strcmp(argv[1], "off")) {
			my_color.r = 0;
			my_color.g = 0;
			my_color.b = 0;
		}else{
			printf("usage: %s [r/g/b] [on/off/value] \n %s [on/off/value value value]", argv[0], argv[0]);
		}
	}else if (argc == 4) {
		uint16_t red = (uint16_t) atoi(argv[1]);
		uint16_t green = (uint16_t) atoi(argv[2]);
		uint16_t blue = (uint16_t) atoi(argv[3]);
		if(red >255 || green > 255 || blue >255 ||
				red < 0 || green < 0 || blue< 0) {
			puts("values should be between 0-255");
			return 1;
		}
		my_color.r = red;
		my_color.g = green;
		my_color.b = blue;
	}else {
		 printf("usage: %s [r/g/b] [on/off/value] \n %s [on/off/value value value]", argv[0], argv[0]);
	}
	rgbled_set(&my_rgbled, &my_color);
	DEBUG("rgb_cmd finished");
	return 0;
}
