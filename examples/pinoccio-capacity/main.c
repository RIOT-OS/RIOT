/*
 * Copyright (C) 2016 RWTH Aachen, Steffen Robertz
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
 * @brief       Test Measurement of a capacity
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @}
 */
#include "capacity.h"
#include "capacity_settings.h"
#include "periph/timer.h"
#include <stdio.h>

void accb(void* arg, uint8_t dev) {
	PORTF ^= (1<<PF0);
	printf("hit");
}

int main(void)
{
	capacity_init(TIMER_DEV(2), AC_0);
	capacity_result_t my_result;
	start_measuring(10, &my_result);
	printf("Timestamp %u \n", my_result.timestamp);
	printf("Frequency %u \n", my_result.frequency);
	printf("Capacity %.6f \n", my_result.capacity*1000000000000);

}
