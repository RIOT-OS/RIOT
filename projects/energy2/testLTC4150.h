/*
 * testLTC4150.h
 *
 *  Created on: 05.06.2012
 *      Author: julsch
 */

#ifndef TESTLTC4150_H_
#define TESTLTC4150_H_

#include <swtimer.h>
#include "led.h"
#include <ltc4150.h>
#include <board.h>

void testLTCFunctions(void);

void startCurrentMeasurement(void);
void stopCurrentMeasurement(void);
void resetCurrentMeasurement(void);

double getAverageCurrent(void);
double getCurrent(void);

void printAverageCurrent(void);
void printCurrent(void);

void printCurrentWithLED(void);
void printCurrentWithoutLED(void);

#endif /* TESTLTC4150_H_ */
