#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>
#include "cpu.h"

#define __IO volatile

typedef struct {
    __IO uint32_t DIR;
    uint32_t _reserved[3];
    __IO uint32_t MASK;
    __IO uint32_t PIN;
    __IO uint32_t SET;
    __IO uint32_t CLR;
} FIO_PORT_t;

#define FIO_PORTS   ((FIO_PORT_t*)FIO_BASE_ADDR)
#define PINSEL      ((__IO uint32_t *)(PINSEL_BASE_ADDR))
#define PINMODE     ((__IO uint32_t *)(PINSEL_BASE_ADDR + 0x40))

int gpio_init_mux(unsigned pin, unsigned mux);
void gpio_init_states(void);

#define GPIO(port, pin) (port*32 + pin)

#define HAVE_GPIO_PP_T
typedef enum {
    GPIO_PULLUP = 0,        /**< enable internal pull-up resistor */
    GPIO_NOPULL = 2,        /**< do not use internal pull resistors */
    GPIO_PULLDOWN = 3       /**< enable internal pull-down resistor */
} gpio_pp_t;

#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 1,       /**< emit interrupt on falling flank */
    GPIO_RISING = 2,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTES
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */
