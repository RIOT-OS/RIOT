#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#include "cpu.h"

#include "hardware/address_mapped.h"
#include "hardware/regs/clocks.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/xosc.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/uart.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"

#include "hardware/structs/uart.h"
#include "hardware/structs/clocks.h"
#include "hardware/structs/iobank0.h"
#include "hardware/structs/padsbank0.h"
#include "hardware/structs/xosc.h"
#include "hardware/structs/resets.h"
#include "hardware/structs/uart.h"
#include "hardware/structs/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PERIPH_TIMER_PROVIDES_SET


/**
 * @brief Available ports on the rp2040
 */
enum {
    GPIO_BANK_USER = 0,                 /**< GPIO User Bank */
    GPIO_BANK_QSPI = 1,                 /**< QSPI Bank */
};

/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

#define GPIO_PIN(x, y) ((gpio_t)((x << 8) | y))

/**
 * @brief   Definition of possible parity modes
 */
#define HAVE_UART_PARITY_T
typedef enum {
   UART_PARITY_NONE,   /**< no parity */
   UART_PARITY_EVEN,   /**< even parity */
   UART_PARITY_ODD,    /**< odd parity */
   UART_PARITY_MARK,   /**< mark parity */
   UART_PARITY_SPACE   /**< space parity */
} uart_parity_t;
#

/**
 * @brief   Definition of possible data bits lengths in a UART frame
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5,   /**< 5 data bits */
    UART_DATA_BITS_6,   /**< 6 data bits */
    UART_DATA_BITS_7,   /**< 7 data bits */
    UART_DATA_BITS_8,   /**< 8 data bits */
} uart_data_bits_t;

/**
 * @brief   Definition of possible stop bits lengths in a UART frame
 */
#define HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1,   /**< 1 stop bit */
   UART_STOP_BITS_2,   /**< 2 stop bits */
} uart_stop_bits_t;

/**
 * @brief UART configuration options
 */
typedef struct {
    gpio_t rx_pin;
    gpio_t tx_pin;
    uart_hw_t *dev;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
    uart_data_bits_t data_bits;
} uart_conf_t;

/**
 * @brief Timer configuration options
 */

typedef struct {
    bool is_relative;
    bool is_absolute;
    bool is_periodic;
} timer_channel_conf_t;

typedef struct {
    bool is_running;
    timer_channel_conf_t channel[4];
} timer_conf_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
