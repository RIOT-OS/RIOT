#ifndef DRIVER_GPIO_H
#define DRIVER_GPIO_H

#ifdef ESP_IDF_CODE

#include_next "driver/gpio.h"

#else

#include "hal/gpio_types.h"

#define GPIO_PIN_COUNT  (SOC_GPIO_PIN_COUNT)

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_GPIO_H */
