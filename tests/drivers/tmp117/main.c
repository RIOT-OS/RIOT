/*
 * main.c
 *
 *  Created on: 19 mai 2026
 *      Author: leo
 */

#include <stdio.h>

#include "board.h"
#include "ztimer.h"

#include "tmp117.h"
#include "tmp117_params.h"


int main(void){

	tmp117_t tmp117;
	int raw_temp;

	ztimer_sleep(ZTIMER_MSEC, 1000);
	printf("################################\n");
	printf("####### TMP117 test app ########\n");
	printf("################################\n\n");

	printf("Initializing tmp117 sensor ...");

	if(tmp117_init(&tmp117,tmp117_params) != TMP117_OK){
		return -1;
	}

	printf("  done.\n\n");

	while(1){
		if(tmp117_read_temperature(&tmp117, &raw_temp) == TMP117_OK){
			printf("temperature : %02d.%02d°C\n", raw_temp/100, raw_temp%100);
		}else{
			printf("[tmp117] error : unable to read temperature\n");
			break;
		}

		ztimer_sleep(ZTIMER_MSEC, 1500);
	}


	return 0;
}




