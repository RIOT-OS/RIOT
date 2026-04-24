#pragma once

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PERI_CLK            (50000000UL)

#if defined(CPU_CORE_CORTEX_M4F)
#define CLOCK_CORECLOCK     (50000000UL)
#endif

#define UART_NUMOF          ARRAY_SIZE(uart_config)
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
#define TIMER_0_MAX_VALUE   UINT32_MAX
#define GPIO_NUMOF          (1U)

#ifdef __cplusplus
}
#endif
