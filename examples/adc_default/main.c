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
//double ExponentialADC(double alpha, double now, double past);
uint32_t ExponentialADC(double alpha, double now, double past);

int main(void)
{
	//printf(" :::: ADC Test Application v0.1 ::::\n" );
	adc_t dev;
	dev = ADC_0;
	adc_precision_t pre;
	pre = ADC_RES_16BIT; // 6BIT & 14BIT is not valid
	int result0 = 0;
	uint32_t resultAccum = 0;
    uint32_t count = 0;
    uint16_t accumulateNum = 100;

	int response = adc_init(dev, pre);
	handle_init_response(response);

  	gpio_init_out(GPIO_0, GPIO_NOPULL);

  	gpio_set(GPIO_0);
  	usleep(100000);
    int result0 = 0;
    uint32_t resultAccum = 0;
    uint32_t count = 0;
    uint16_t accumulateNum = 100;
    while( count < accumulateNum)
    {
        result0 = adc_sample(dev, 0);      
        resultAccum += result0;
        count++;
    }
    gpio_clear(GPIO_0);
    resultAccum = (resultAccum/accumulateNum); 




    printf("\n\n\n\tFirst result: %d\n", resultAccum); 

    // while (count < 1000000)
    // {
    //     result = adc_sample(dev, 0);
    //     printf("%d\n", result); 
    //     count++;
    // }


    count = 0;
    double result = 0;
    double const alpha = 0.1;
    double past = 0.0;
    gpio_set(GPIO_0);
    usleep(100000);
    while(count < 100)
    {
        if(count == 0)
        {
            double temp = (double) adc_sample(dev, 0);
            result = ExponentialADC(alpha,temp, temp);
            past = result;
        }
        double now = (double) adc_sample(dev, 0);
        result = ExponentialADC(alpha, now, past);
        past = result;
        count++;
    }


    printf("\n\n\n\tSecond result: %d\n\n\n", (uint16_t)result);

    printf("\n\n\t\tDONE!\n\n\n"); 
    while(1)
    {}
    
    return 0;
}

uint32_t ExponentialADC(double alpha, double now, double past)
{
    return (uint32_t)((alpha*now)+(1-alpha)*past);

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
