/*
 * testSHT11.h
 *
 *  Created on: 01.06.2012
 *      Author: julsch
 */

#ifndef TESTSHT11_H_
#define TESTSHT11_H_

#ifndef SHT11_H_
#include <sht11.h>
#endif

/**
 * Testing SHT11 functions.
 */
void testSHT11Functions(void);

/**
 *
 */
void getMeasurementInThread(void);

/**
 * Depending on parameter sht11_mode_t the procedure prints rel. humidity,
 * temperature and/or temperature compensated relative humidity.
 * @param sht11_mode_t is value HUMIDITY or/and TEMPERATURE
 */
void printMeasurement(sht11_mode_t mode);


#endif /* TESTSHT11_H_ */
