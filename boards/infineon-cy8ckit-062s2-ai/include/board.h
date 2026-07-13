#pragma once

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STDIO_UART_DEV       (UART_DEV(0))
#define STDIO_UART_BAUDRATE (115200U)

#define LED0_PIN        GPIO_PIN(5, 3)
#define LED1_PIN        GPIO_PIN(5, 4)

#ifdef __cplusplus
}
#endif
