#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#include "cpu.h"

#include "hardware/regs/clocks.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/xosc.h"
#include "hardware/regs/resets.h"

#include "hardware/structs/uart.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/padsbank0.h"
#include "hardware/structs/xosc.h"
#include "hardware/structs/resets.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Available ports on the rp2040
 */
enum {
    GPIO_BANK_USER = IO_BANK0_BASE,                 /**< GPIO User Bank */
};

#define GPIO_PIN(x, y) ((unsigned int)(x + y))

/**
 * @brief UART module configuration options
 */
typedef struct {
    bool parity;
    unsigned int rx_pin;
    unsigned int tx_pin;
    uart_hw_t *dev;
    uint8_t stop_bits;
    uint8_t data_bits;
} uart_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
