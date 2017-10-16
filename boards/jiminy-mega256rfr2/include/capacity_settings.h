/**
 * @brief contains the configuration for the capacity module
 */

#ifndef CAPACITY_SETTINGS_H_
#define CAPACITY_SETTINGS_H_

#include "periph/gpio.h"

#ifdef __cplusplus 
extern "C" {
#endif

/**
 * define the Analog Comparator to use
 */
#define MY_AC		AC_0

/**
 * define any other init features
 */
#define OTHER_INIT_FEATURES


typedef struct{
	gpio_t plus_pin;
	gpio_t minus_pin;
	gpio_t result_pin;
	gpio_t pxy_pin;
}capacity_conf_t;

static const capacity_conf_t capacity_conf = {
		.plus_pin = GPIO_PIN(PORT_E, 2),
		.minus_pin = GPIO_PIN(PORT_E, 3),
		.result_pin = GPIO_PIN(PORT_F, 0),
		.pxy_pin = GPIO_PIN(PORT_F, 1)
};

#ifdef __cplusplus
}
#endif

#endif
