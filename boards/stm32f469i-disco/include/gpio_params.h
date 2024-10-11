#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LEDs and BTN configuration
 * @{
 */
static const saul_gpio_params_t saul_gpio_params[] =
    {
        {
            .name = "LED1",
            .pin = LED0_PIN,
            .mode = GPIO_OUT,
            .flags = SAUL_GPIO_INIT_SET,
        },
        {
            .name = "LED2",
            .pin = LED1_PIN,
            .mode = GPIO_OUT,
            .flags = SAUL_GPIO_INIT_SET,
        },
        {
            .name = "LED3",
            .pin = LED2_PIN,
            .mode = GPIO_OUT,
            .flags = SAUL_GPIO_INIT_SET,
        },
        {
            .name = "LED4",
            .pin = LED3_PIN,
            .mode = GPIO_OUT,
            .flags = SAUL_GPIO_INIT_SET,
        },
        {
            .name = "BTN USER",
            .pin = BTN0_PIN,
            .mode = BTN0_MODE,
        },
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
