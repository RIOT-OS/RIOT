#pragma once

#include "periph_cpu.h"
#include "macros/units.h"
#include "RP2350.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BAUDRATE 115200
#define IBRD ((8*CPUFREQ + BAUDRATE)/(2*BAUDRATE))/64
#define FBRD ((8*CPUFREQ + BAUDRATE)/(2*BAUDRATE))%64


#define UART_UARTCR_UARTEN_BITS 1<<0
#define UART_UARTCR_RXE_BITS 1<<9
#define UART_UARTCR_TXE_BITS 1<<8
#define UART_UARTFR_RXFF_BITS 1<<6
#define UART_UARTFR_TXFE_BITS 1<<7

#ifdef __cplusplus
}
#endif
