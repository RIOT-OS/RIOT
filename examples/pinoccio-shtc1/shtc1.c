#define ENABLE_DEBUG    (0)

#include "board.h"
#include "periph_conf.h"
#include "debug.h"
#include "shtc1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int shtc1_cmd(int argc, char **argv)
{
	DEBUG("shtc1_cmd called \n");
	if (argc != 2) {
	        printf("usage: %s  [temp/humidity/all]", argv[0]);
	        return 1;
	}	
	int8_t error = shtc1_init(I2C_0);
	shtc1_values_t my_values;
	if(!error){
		error = shtc1_measure(I2C_0, CRC_ENABLED, &my_values);
	}else {
		printf("can't initialize the sensor");
		return 1;
	}
	if(!error) {
		if (!strcmp(argv[1], "temp")) {
			/*print temp value*/
			printf("Temperature: %.2f C", my_values.temp);
			return 0;
		}else if (!strcmp(argv[1], "humidity")) {
			/*print temp value*/
			printf("Humidity: %.2f%%", my_values.rel_humidity);
			return 0;
		}else if (!strcmp(argv[1], "all")) {
			/*print temp value*/
			printf("Temperature: %.2f C \n Humidity: %.2f%%", my_values.temp, my_values.rel_humidity);
			return 0;
		}else{
			printf("usage: %s  [temp/humidity/all]", argv[0]);
			return 1;
		}
	}
	return 0;
}
