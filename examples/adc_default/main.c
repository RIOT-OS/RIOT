/*
 * Copyright (C) 2008, 2009, 2010  Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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
 * @brief       Default adc application 
 *
 * @author      Rane Balslev <ranebalslev@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include "periph/adc.h"
#include "periph/gpio.h" 
#include "vtimer.h"
void handle_init_response( int );

int main(void)
{
	//printf(" :::: ADC Test Application v0.1 ::::\n" );
	adc_t dev;
	dev = ADC_0;
	adc_precision_t pre;
	pre = ADC_RES_16BIT; // 6BIT & 14BIT is not valid
	int result = 0;
	uint32_t resultAccum = 0;
    uint16_t count = 0;
    uint16_t accumulateNum = 500;

	int response = adc_init(dev, pre);
	handle_init_response(response);

  	gpio_init_out(GPIO_0, GPIO_NOPULL);
  	gpio_set(GPIO_0);
  	usleep(100000);
   
    while( count < accumulateNum/*+20*/)
    {
        result = adc_sample(dev, 0);
       // if(count > 20) // filter first 20 measurements out
        //{        
            resultAccum += result;
        //}
        count++;
    } 
    gpio_clear(GPIO_0);
    resultAccum = (uint32_t)(resultAccum/accumulateNum); 
    printf("%d\n", resultAccum); 

    while(1)
    {
    	//result = adc_sample(dev, 2); //pin 6
    	//printf("Result: " );
    	//printf("PIN6: %d\n", result);   

    	//result = adc_sample(dev, 0); // pin 5
    	//printf("Result: " );
    	//printf("%d\n", result);  	
    }
    
    return 0;
}

void handle_init_response( int response)
{
	if ( response == -1)
	{
		char c;
		printf("ADC initialization error\n");
		printf("Perhaps an incompatible resolution defined?\n");
		printf("\t To continue press 'c' \n");
		while(1)
		{                    
		    if (scanf("%c", &c) != 'c')
		        break;
		/* stay here! */
		}
	}	
}
