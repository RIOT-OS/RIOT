#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "cpu.h"
#include "RP2350.h"

#include "uart_conf.h"
#include "clock_conf.h"
#include "gpio_conf.h"
#include "helpers.h"

// Overwrite the default GPIO type to use uint32_t
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

#define LED0_PIN_ID 25
#define OSC_DEBUG_PIN_ID 15

#define RESET_PLL_SYS 1 << 14
#define RESET_PADS_BANK0 1 << 9
#define RESET_UART0 1 << 26
#define RESET_UART1 1 << 27
#define RESET_IO_BANK0 1 << 6

#define PADS_BANK0_GPIO0_IE_BITS 1<<6
#define PADS_BANK0_ISO_BITS 1<<8

#ifdef __cplusplus
extern "C" {
#endif

static inline volatile uint32_t calculate_gpio_pad_register_addr(gpio_t pin) {
    // Each pin has a 4 byte register, so we can calculate the address
    // by adding 4 bytes for each pin, starting at the base address of PADS_BANK0
    // and adding 4 bytes to skip VOLTAGE_SELECT
    return PADS_BANK0_BASE + 4 * (pin + 1);
}

static volatile uint32_t calculate_gpio_io_status_register_addr(gpio_t pin) {
    // Each status register is followed by a ctrl register,
    return IO_BANK0_BASE + 8 * pin;
}

static inline volatile uint32_t calculate_gpio_io_ctrl_register_addr(gpio_t pin) {
    // Each pin has a 8 byte register (4 Bytes of Status, 4 Bytes of CTRL),
    // so we can calculate the address by adding 8 bytes for each pin,
    // starting at the base address of IO_BANK0
    return calculate_gpio_io_status_register_addr(pin) + 4;
}

#ifdef __cplusplus
}
#endif

