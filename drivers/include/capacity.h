/*
 * Copyright 2017, RWTH-Aachen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CAPACITY_H_
#define CAPACITY_H_

/**
 * @defgroup    drivers_capacity  capacity
 * @ingroup     drivers_sensors
 * @brief       Driver to read a capacity value
 *
 * 				This is using a analog comparator and the input capture feature
 * 				in order to calculate the capacity of a Capacitor.
 * 				For further details on the circuit used check: http://www.ti.com/lit/an/slaa363a/slaa363a.pdf
 * 				The Output of the comparator is feed back using an interrupt and a gpio pin
 * @{
 *
 * @file
 * @brief       Driver to read a capacity value
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint16_t average;
	float frequency;
	float capacity;
	float timestamp;
}capacity_result_t;

/**
 * @brief initializes the ports
 */
void capacity_init(uint8_t timer_dev, uint8_t ac_dev);

/**
 * @brief starts a new measurement
 *
 * @param[in] cycle 			how many cycles should be used for the average?
 *
 * @return						0 on completed measurement / 1 on failure
 */

uint8_t start_measuring(uint8_t cycle, capacity_result_t *my_result);

/**
 * @brief stops the measurement
 */
//void stop_measuring();



#ifdef __cplusplus
}
#endif

/** @} */
#endif /*CAPACITY_H_*/
