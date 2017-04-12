#define ENABLE_DEBUG    (0)


#include "capacity.h"
#include "capacity_settings.h"
#include "periph/timer.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int capacity_cmd(int argc, char **argv)
{
	DEBUG("capacity_cmd called \n");
	if (argc >3 || argc == 1) {
		printf("usage: %s  [-c/-f/-t/-a] [#cycles=10] \n", argv[0]);
		return 1;
	}
	if(!strcmp("--help", argv[1]) || !strcmp("-h", argv[1])) {
		printf("usage: capacity  [-c/-f/-t] \n");
		printf("-c   Prints Capacity in Farad \n");
		printf("-f   Prints the Oscillating Frequency of the Capacity charge and discharge \n");
		printf("-t   Prints the time needed to charge and discharge the Capacity \n");
		printf("-a   Prints the average of cycles recorded by the timer. Only needed for debugging \n");
		printf("#cycles   Number of measurements used for the data     default:10 \n");
		return 1;
	}
	capacity_init(TIMER_DEV(2), AC_0);
	capacity_result_t my_result;
	uint8_t cycles = 10;

	if(argc == 3 && atoi(argv[2])) {
		uint8_t recieved_cycles = atoi(argv[2]);
		if(recieved_cycles >10){
			cycles = 10;
		}else{
		cycles = atoi(argv[2]);
		}
	}
	start_measuring(cycles, &my_result);
	DEBUG("Cycles %u \n", cycles);
	DEBUG("Capacity %.3f \n", my_result.capacity);
	DEBUG("Frequency %.2f \n", my_result.frequency);
	DEBUG("Timestamp %.3f \n", my_result.timestamp);
	DEBUG("Debug: Average %u \n", my_result.average);


	if(!strcmp("-c", argv[1])){
		printf("Capacity %.3fpF\n", my_result.capacity*1000000000000);
	}else if(!strcmp("-f", argv[1])) {
		printf("Frequency %.2f \n", my_result.frequency);
	}else if(!strcmp("-t", argv[1])) {
		printf("Timestamp %.3f us\n", my_result.timestamp*1000000);
	}else if(!strcmp("-a", argv[1])) {
		printf("Debug: Average %u \n", my_result.average);
	}else{
		printf("Unknown Command \n");
		printf("usage: %s  [-c/-f/-t/-a] [#cycles=10] \n", argv[0]);
		return 1;
	}
	DEBUG("capacity_cmd finished \n");

	return 0;

}
