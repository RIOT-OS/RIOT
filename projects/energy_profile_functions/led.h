/*
 * led.h
 *
 * Provides some useful functions to generate different user signals with the
 * red LED.
 *
 *  Created on: 30.05.2012
 *  Author: julsch
 */

#ifndef LED_H_
#define LED_H_

#include <thread.h>

/**
 * Toggle the LED n times.
 */
void toggleLedNTimes(int times, long int toggleSpeed);

//void toggleLedNTimesInThread(int times, long int toggleSpeed);


#endif /* LED_H_ */




