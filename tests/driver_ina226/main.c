/*
 * Copyright (C) 2016
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the INA226 sensor driver
 *              All references to the INA226 data sheet are related to the document revision SBOS547A
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */
#include <stdio.h>

#include "xtimer.h"
#include "ina226.h"
#include "ina226-regs.h"
#include "phydat.h"
#include "debug.h"

#define ENABLE_DEBUG (0)

#define MILLIS_PER_INT   (1000)
#define MICROS_PER_INT   (1000 * MILLIS_PER_INT)
#define MICROS_PER_MILLI (1000)
#define NANOS_PER_MICRO  (1000)
#define NANOS_PER_MILLI  (1000 * NANOS_PER_MICRO)

//--------------------------------- GENERAL CONFIGURATION PARAMETERS -----vvvvv
#ifndef TEST_INA226_I2C
#define TEST_INA226_I2C (I2C_0)
#endif
#ifndef TEST_INA226_ADDR
#define TEST_INA226_ADDR (0b1000000) //slave address of INA226 for A0 = A1 = GND --> 0b1000000 = 0x40
#endif
#ifndef TEST_INA226_ALERT_PIN
#define TEST_INA226_ALERT_PIN (GPIO_PIN(0,6)) //PIN PA06
#endif

/**
 * This configuration enables continuous measurement of both shunt and bus voltage.
 * The adc conversion time for both is set to be 1.1 ms and the measurements are averaged over 256 individual samples -> so the interrupt will fire ~twice per second*/
#define TEST_INA226_CONFIG (INA226_OPERATING_MODE_CONTINUOUS_SHUNT_AND_BUS | INA226_VBUSCT_1_1_MS | INA226_VSHCT_1_1_MS | INA226_AVG_256)

#define TEST_INA226_SHUNT_RESISTOR_MILLI_OHM (750)

/** Calculated value for the calibration register for use with a 0.75 Ohm shunt resistor. See section 7.5 in INA226 data sheet */
#define TEST_INA226_CALIBRATION_VALUE (2048)

/** The maximum current that is expected to flow through the shunt resistor in microampere. See section 7.5 in INA226 data sheet */
#define TEST_INA226_MAX_EXPECTED_CURRENT_UA ((INA226_MAX_SHUNT_VOLTAGE_UV * MICROS_PER_MILLI) / TEST_INA226_SHUNT_RESISTOR_MILLI_OHM)

/** The value of one LSB of the current register in nanoampere. See section 7.5 in INA226 data sheet */
#define TEST_INA226_CURRENT_LSB_NA ((TEST_INA226_MAX_EXPECTED_CURRENT_UA * NANOS_PER_MICRO) / INA226_MAX_CURRENT_TO_LSB_RATIO)
//--------------------------------- GENERAL CONFIGURATION PARAMETERS -----^^^^^


//--------------------- REFERENCE MEASUREMENT CALIBRATION PARAMETERS -----vvvvv
/** Current [uA] that was measured with external ampere meter. This value was measured under the same conditions as when the INA226
 *  (configured with the original calculated TEST_INA226_CALIBRATION_VALUE) reported TEST_INA226_CAL_REPORTED_CURRENT*/
#define TEST_INA226_CAL_AMM_MEAS_CURRENT (6593)

/** Current [uA] that was reported by INA226 corresponding to the value of TEST_INA226_CAL_AMM_MEAS_CURRENT above. */
#define TEST_INA226_CAL_REPORTED_CURRENT (7002)

/** This value is derived by the Equation shown in INA226 datasheet section 7.5.2 */
#define TEST_CORRECTED_FULL_SCALE_CAL ((TEST_INA226_CALIBRATION_VALUE * TEST_INA226_CAL_AMM_MEAS_CURRENT) / TEST_INA226_CAL_REPORTED_CURRENT)
//--------------------- REFERENCE MEASUREMENT CALIBRATION PARAMETERS -----^^^^^

/** callback that gets fired whenever a new measurement is available (the alert pin of the INA226 gets triggered). */
static void test_ina226_callback(void *arg);

int main(void)
{
	ina226_t ina226_dev;
	uint16_t ina_226_die_id;
	uint16_t ina_226_manufac_id;

	xtimer_init();

	printf("--- RIOT: INA226 test application ---\n");	

	printf("initializing I2C... ");
	if (i2c_init_master(TEST_INA226_I2C, I2C_SPEED_FAST) == 0) {
		printf("i2c_init_master [OK]\n");
	}
	else {
		printf("i2c_init_master [FAILED]\n");
		return -1;
	}

	printf("Initializing INA226 sensor at I2C_%" PRIu16 ", address 0x%02" PRIx16 "... ",TEST_INA226_I2C, TEST_INA226_ADDR);
	if (ina226_init(&ina226_dev, TEST_INA226_I2C, TEST_INA226_ADDR) == 0) {
		printf("ina226_init [OK]\n");
	}
	else {
		printf("ina226_init [FAILED]\n");
		return -1;
	}

	printf("reading die-id of INA226 sensor... ");
	if (ina226_read_die_id(&ina226_dev, &ina_226_die_id) == 0) {
		printf("ina226_read_die_id [OK]: ina_226_die_id = 0x%0" PRIx16 "\n", ina_226_die_id);
	}
	else {
		printf("ina226_read_die_id [FAILED]\n");
		return -1;
	}

	printf("reading manufacturer-id of INA226 sensor... ");
	if (ina226_read_manufacturer_id(&ina226_dev, &ina_226_manufac_id) == 0) {
		printf("ina226_read_manufacturer_id [OK]: ina_226_manufac_id = 0x%0" PRIx16 "\n", ina_226_manufac_id);
	}
	else {
		printf("ina226_read_manufacturer_id [FAILED]\n");
		return -1;
	}

	printf("setting configuration register of INA226 sensor... ");
	if (ina226_write_config(&ina226_dev, TEST_INA226_CONFIG) == 0) {
		printf("ina226_set_config [OK]\n");
	}
	else {
		printf("ina226_set_config [FAILED]\n");
		return -1;
	}

	printf("setting calibration register to %" PRIu16 "... ", TEST_CORRECTED_FULL_SCALE_CAL);
	if (ina226_write_calibration(&ina226_dev, TEST_CORRECTED_FULL_SCALE_CAL) == 0) {
		printf("ina226_set_calibration [OK]\n");
	}
	else {
		printf("ina226_set_calibration [FAILED!]\n");
		return -1;
	}

	printf("enabling interrupt by alert pin... ");
	if (ina226_activate_int(&ina226_dev, INA226_MASK_ENABLE_CNVR, TEST_INA226_ALERT_PIN,test_ina226_callback) == 0) {
		printf("ina226_activate_int [OK]\n");
	}
	else {
		printf("ina226_activate_int [FAILED]\n");
		return -1;
	}

	printf("INA226 initialized successfully -> now waiting for interrupts...\n");

	while(true){
		xtimer_usleep(3000000);

	}
}

static void test_ina226_callback(void *arg)
{
	DEBUG("test_ina226_callback called\n");

	ina226_t *dev = (ina226_t*) arg;

    //--- raw values of INA226 registers ---vvv
    uint16_t mask_enable_reg;
    int16_t bus_voltage_reg;
    int16_t  shunt_voltage_reg;
    int16_t  current_reg;
    int16_t  power_reg;
    //--- raw values of INA226 registers ---^^^

    //--- derived values for pretty printing afterwards ---vvv
    int32_t current_ua;
    int32_t power_uw;

    uint32_t bus_voltage_int_v;   //integer part of bus voltage [V]
    uint32_t bus_voltage_frac_mv; //fractional part of bus voltage [mV]

    int32_t shunt_voltage_int_mv;  //integer part of shunt voltage [mV]
    int32_t shunt_voltage_frac_uv; //fractional part of shunt voltage [uV]

    int32_t current_int_ma;  //integer part of current [mA]
    int32_t current_frac_ua; //fractional part of current [uA]

    int32_t power_int_mw;  //integer part of power [mW]
    int32_t power_frac_uw; //fractional part of power [uW]
    //--- derived values for pretty printing afterwards ---^^^

    //reading the mask enable register is needed to re-enable interrupt generation by ina226
    if (ina226_read_mask_enable(dev, &mask_enable_reg) == 0) {
    	DEBUG("ina226_read_mask_enable [OK]\n");
    }
    else {
    	DEBUG("ina226_read_mask_enable [FAILED]\n");
    }

	if (ina226_read_bus(dev, &bus_voltage_reg) == 0) {
		DEBUG("ina226_read_bus [OK]: bus_voltage_reg = %"PRIu16"\n", bus_voltage_reg);
		bus_voltage_int_v  = (bus_voltage_reg * INA226_BUS_VOLTAGE_LSB_UV) / MICROS_PER_INT;
		bus_voltage_frac_mv = ( (bus_voltage_reg * INA226_BUS_VOLTAGE_LSB_UV) / MICROS_PER_MILLI) % MILLIS_PER_INT;
	}
	else {
		DEBUG("ina226_read_bus [FAILED]\n");
		return;
	}

	if (ina226_read_shunt(dev, &shunt_voltage_reg) == 0) {
		DEBUG("ina226_read_shunt [OK]: shunt_voltage_reg = %"PRIi16"\n", shunt_voltage_reg);
		shunt_voltage_int_mv  = (shunt_voltage_reg * INA226_SHUNT_VOLTAGE_LSB_NV) / NANOS_PER_MILLI;
	    if (shunt_voltage_reg < 0) {
			shunt_voltage_frac_uv = ( (-1 * shunt_voltage_reg * INA226_SHUNT_VOLTAGE_LSB_NV) / NANOS_PER_MICRO ) % MICROS_PER_MILLI;
		}
		else {
			shunt_voltage_frac_uv = ( (shunt_voltage_reg* INA226_SHUNT_VOLTAGE_LSB_NV) / NANOS_PER_MICRO ) % MICROS_PER_MILLI;
		}
	}
	else {
		DEBUG("ina226_read_shunt [FAILED]\n");
		return;
	}

	if (ina226_read_current(dev, &current_reg) == 0) {
		DEBUG("ina226_read_current [OK]: current_reg = %"PRIi16"\n", current_reg);
		current_ua = (current_reg * TEST_INA226_CURRENT_LSB_NA) / NANOS_PER_MICRO;
		current_int_ma = current_ua / MICROS_PER_MILLI;

	    if (current_ua < 0) {
			current_frac_ua = (-1 * current_ua) % MICROS_PER_MILLI;
		}
		else{
			current_frac_ua = current_ua % MICROS_PER_MILLI;
		}
    }
	else {
		DEBUG("ina226_read_current [FAILED]\n");
		return;
	}

	if (ina226_read_power(dev,&power_reg) == 0) {
		DEBUG("ina226_read_power [OK]: power_reg = %"PRIi16"\n", power_reg);
		power_uw = (INA226_POWER_LSB_CURRENT_LSB_RATIO * TEST_INA226_CURRENT_LSB_NA * power_reg) / NANOS_PER_MICRO;
		power_int_mw = power_uw / MICROS_PER_MILLI;
		power_frac_uw = power_uw % MICROS_PER_MILLI;
	}
	else {
		DEBUG("ina226_read_power [FAILED]\n");
		return;
	}

	printf("| bus: %c%" PRIu32 ",%03" PRIu32 " V ", ((bus_voltage_int_v == 0 && bus_voltage_reg < 0) ? '-' : ' '), bus_voltage_int_v, bus_voltage_frac_mv);
	printf("| shunt: %c%" PRId32 ",%03" PRId32 " mV ", ((shunt_voltage_int_mv == 0 && shunt_voltage_reg < 0) ? '-' : ' '), shunt_voltage_int_mv, shunt_voltage_frac_uv);
	printf("| current: %c%" PRId32 ",%03" PRId32 " mA ", ((current_int_ma == 0 && current_ua < 0) ? '-' : ' '), current_int_ma, current_frac_ua);
	printf("| power:  %" PRId32 ",%03" PRId32 " mW |\n", power_int_mw, power_frac_uw);
}