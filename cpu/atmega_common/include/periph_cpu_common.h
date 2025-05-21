/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 Freie Universität Berlin
 *               2016 INRIA
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_atmega_common
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          René Herthel <rene-herthel@outlook.de>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 * @author          Hugues Larrive <hugues.larrive@pm.me>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Length of the CPU_ID in octets
 * @{
 */
#define CPUID_LEN           (4U)
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
 * @{
 */
#define HAVE_GPIO_T
typedef uint8_t gpio_t;
/** @} */
#endif

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)          ((x << 4) | y)

/**
 * @brief   Base of the GPIO registers as memory address
 *
 * Must be identical to the address of `PINA` provided by avr/io.h
 */
#if (defined(OCF1A) && defined(OCF1B) && (OCF1A > OCF1B)) \
    || (defined(PUD) && (PUD != 4)) || (defined(INT0) && (INT0 == 6))
    /* match with 65 devices against 61 for (PORTB == _SFR_IO8(0x18)) which
     * did not work here anyway */
#define GPIO_PORT_DESCENDENT
#endif

#ifdef GPIO_PORT_DESCENDENT
#ifdef      _AVR_ATTINY1634_H_INCLUDED
/*          the only one that requires particular treatment! */
#define     ATMEGA_GPIO_BASE_A  0x2F
#else
/*          all other port descendent, including :
             - _AVR_IO8534_ (only have port A but with 0x1B address) ;
             - _AVR_IOAT94K_H_ (only have ports D and E) ;
             - _AVR_IOTN28_H_ (only have ports A and D). */
#define     ATMEGA_GPIO_BASE_A  0x39
#endif /*   _AVR_ATTINY1634_H_INCLUDED */
#else /* !GPIO_PORT_DESCENDENT */
#define     ATMEGA_GPIO_BASE_A  0x20
#endif /* GPIO_PORT_DESCENDENT */
/**
 * @brief   Base of the GPIO port G register as memory address
 *
 * Must be identical to the address of `PING` provided by avr/io.h
 */
#define ATMEGA_GPIO_BASE_G      (ATMEGA_GPIO_BASE_A + ATMEGA_GPIO_SIZE * ('G' - 'A'))
/**
 * @brief   Base of the GPIO registers of the second memory region (port >= H)
 *
 * Must be identical to the address of `PINH` provided by avr/io.h
 */
#define ATMEGA_GPIO_BASE_H      (0x100)
/**
 * @brief   sizeof(atmega_gpio_port_t), but preprocessor friendly
 */
#define ATMEGA_GPIO_SIZE        (0x03)

#if defined(DOXYGEN)
/**
 * @brief     Number of external interrupt vectors
 */
#define GPIO_EXT_INT_NUMOF      <CPU_SPECIFIC>
#elif defined(INT7_vect)
#define GPIO_EXT_INT_NUMOF      (8U)
#elif defined(INT6_vect)
#define GPIO_EXT_INT_NUMOF      (7U)
#elif defined(INT5_vect)
#define GPIO_EXT_INT_NUMOF      (6U)
#elif defined(INT4_vect)
#define GPIO_EXT_INT_NUMOF      (5U)
#elif defined(INT3_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT2_vect)
#define GPIO_EXT_INT_NUMOF      (3U)
#else
#define GPIO_EXT_INT_NUMOF      (2U)
#endif

/**
 * @brief   Structure describing the memory layout of the registers of a
 *          GPIO port on ATmega MCUs.
 */
typedef struct {
    /**
     * @brief   Toggle bits in the port register
     * @note    The bits in the port register will be also toggled for inputs.
     *          This can be both a footgun as well as an efficient way to toggle
     *          the pull up resistor on inputs
     *
     * Referred to as "Input Pins Address" in the datasheet.
     */
    volatile uint8_t pin;
    /**
     * @brief   Configure pins as output (1) or input (0) using the Data
     *          Direction Register
     */
    volatile uint8_t ddr;
    /**
     * @brief   Read/write the state of GPIO pins using the Port Data Register
     *
     * @note    When in input mode (see @ref atmega_gpio_port_t::ddr) writing a
     *          1 will enable the pull up resistor, writing a 0 will put the
     *          pin in floating mode.
     */
    volatile uint8_t port;
} atmega_gpio_port_t;

/**
 * @brief       Get the GPIO PORT registers of the given GPIO PORT
 * @param[in]   port_num    Number of the port to get the registers of
 * @return      Pointer to the registers controlling the given GPIO PORT
 */
static inline atmega_gpio_port_t *atmega_gpio_port(uint8_t port_num)
{
    static const uintptr_t base_addr = (uintptr_t)ATMEGA_GPIO_BASE_A;
#ifdef GPIO_PORT_DESCENDENT
    uintptr_t res = base_addr - port_num * sizeof(atmega_gpio_port_t);
#else
    uintptr_t res = base_addr + port_num * sizeof(atmega_gpio_port_t);
#endif
    /* GPIO ports up to (including) G are mapped in the I/O address space,
     * port H and higher (if present) are mapped in a different contiguous
     * region afterwards (e.g. 0x100 for ATmega2560). */
#ifdef PORTH
    if (port_num > 'G'-'A') {
        static const uintptr_t offset = ATMEGA_GPIO_BASE_H - ATMEGA_GPIO_BASE_G;
        res += offset;
    }
#endif

    return (atmega_gpio_port_t *)res;
}

#ifndef DOXYGEN
/**
 * @brief   Override the GPIO flanks
 *
 * This device has an additional mode in which the interrupt is triggered
 * when the pin is low.
 *
 * Enumeration order is important, do not modify.
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_LOW,          /**< emit interrupt when pin low */
    GPIO_BOTH,         /**< emit interrupt on both flanks */
    GPIO_FALLING,      /**< emit interrupt on falling flank */
    GPIO_RISING,       /**< emit interrupt on rising flank */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

#ifndef DOXYGEN /* BEGIN: GPIO LL overwrites */
#define HAVE_GPIO_SLEW_T
typedef enum {
    GPIO_SLEW_SLOWEST = 0,
    GPIO_SLEW_SLOW = 0,
    GPIO_SLEW_FAST = 0,
    GPIO_SLEW_FASTEST = 0,
} gpio_slew_t;

#define HAVE_GPIO_PULL_STRENGTH_T
typedef enum {
    GPIO_PULL_WEAKEST = 0,
    GPIO_PULL_WEAK = 0,
    GPIO_PULL_STRONG = 0,
    GPIO_PULL_STRONGEST = 0
} gpio_pull_strength_t;

#define HAVE_GPIO_DRIVE_STRENGTH_T
typedef enum {
    GPIO_DRIVE_WEAKEST = 0,
    GPIO_DRIVE_WEAK = 0,
    GPIO_DRIVE_STRONG = 0,
    GPIO_DRIVE_STRONGEST = 0
} gpio_drive_strength_t;

#define HAVE_GPIO_IRQ_TRIG_T
typedef enum {
    GPIO_TRIGGER_LEVEL_LOW      = 0x00,
    GPIO_TRIGGER_EDGE_BOTH      = 0x01,
    GPIO_TRIGGER_EDGE_FALLING   = 0x02,
    GPIO_TRIGGER_EDGE_RISING    = 0x03,
    GPIO_TRIGGER_LEVEL_HIGH     = 0xff, /**< not supported */
} gpio_irq_trig_t;

#define HAVE_GPIO_STATE_T
typedef enum {
    GPIO_INPUT,
    GPIO_OUTPUT_PUSH_PULL,
    GPIO_OUTPUT_OPEN_DRAIN,     /**< not supported */
    GPIO_OUTPUT_OPEN_SOURCE,    /**< not supported */
    GPIO_USED_BY_PERIPHERAL,    /**< not supported */
    GPIO_DISCONNECT             = GPIO_INPUT,
} gpio_state_t;

#define HAVE_GPIO_LL_PREPARE_WRITE_ALL_PINS
#define HAVE_GPIO_LL_PREPARE_WRITE

#endif /* END: GPIO LL overwrites */

/**
 * @brief   Use some common SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   SPI mode select macro
 *
 * The polarity is determined by bit 3 in the configuration register, the phase
 * by bit 2.
 */
#define SPI_MODE_SEL(pol, pha)          ((pol << 3) | (pha << 2))

/**
 * @name   Override the SPI mode values
 *
 * As the mode is set in bit 3 and 2 of the configuration register, we put the
 * correct configuration there
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = SPI_MODE_SEL(0, 0),    /**< mode 0 */
    SPI_MODE_1 = SPI_MODE_SEL(0, 1),    /**< mode 1 */
    SPI_MODE_2 = SPI_MODE_SEL(1, 0),    /**< mode 2 */
    SPI_MODE_3 = SPI_MODE_SEL(1, 1)     /**< mode 3 */
} spi_mode_t;
/** @} */

/**
 * @brief   SPI speed selection macro
 *
 * We encode the speed in bits 2, 1, and 0, where bit0 and bit1 hold the SPCR
 * prescaler bits, while bit2 holds the SPI2X bit.
 */
#define SPI_CLK_SEL(s2x, pr1, pr0)    ((s2x << 2) | (pr1 << 1) | pr0)

/**
 * @name   Override SPI speed values
 *
 * We assume a master clock speed of 16MHz here.
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = SPI_CLK_SEL(0, 1, 1),      /**< 16/128 -> 125KHz */
    SPI_CLK_400KHZ = SPI_CLK_SEL(1, 1, 0),      /**< 16/32  -> 500KHz */
    SPI_CLK_1MHZ   = SPI_CLK_SEL(0, 0, 1),      /**< 16/16  -> 1MHz */
    SPI_CLK_5MHZ   = SPI_CLK_SEL(0, 0, 0),      /**< 16/4   -> 4MHz */
    SPI_CLK_10MHZ  = SPI_CLK_SEL(1, 0, 0)       /**< 16/2   -> 8MHz */
} spi_clk_t;
/** @} */
#endif /* ifndef DOXYGEN */

/**
 * @name  Bitmasks indicating which are the possible dividers for a timer
 * @{
 */
typedef enum {
    TIMER_DIV1_8_64_128_1024 = 0x549,           /**< 1/{1,8,64,128,1024} */
    TIMER_DIV1_8_32_64_128_256_1024 = 0x5E9,    /**< 1/{1,8,32,64,128,256,1024} */
} timer_div_t;
/** @} */

/**
 * @name   PWM configuration
 * @{
 */
typedef struct {
    mini_timer_t *dev;                  /**< Timer used */
    gpio_t pin_ch[2];                   /**< Output Pins */
    timer_div_t div;                    /**< Timer divider mask */
} pwm_conf_t;
/** @} */

/**
 * @brief   A low-level timer_set() implementation is provided
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   EEPROM clear byte
 */
#define EEPROM_CLEAR_BYTE              (0xff)

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
#define NWDT_TIME_LOWER_LIMIT           (1)
#define NWDT_TIME_UPPER_LIMIT           (8192U)
/** @} */

/**
 * @brief   WDT can be stopped on AVR
 */
#define WDT_HAS_STOP                    (1)

/**
 * @name RTT configuration
 * @{
 */
#if defined(SCCR0) && !defined(RTT_BACKEND_SC)
#define RTT_BACKEND_SC   (1)
#endif

#if RTT_BACKEND_SC
/* For MCU with MAC symbol counter */
#ifndef RTT_MAX_VALUE
#define RTT_MAX_VALUE    (0xFFFFFFFFUL)  /* 32-bit timer */
#endif

#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY    (62500UL)       /* in Hz. */
#endif

#else
/* For MCU without MAC symbol counter */
#ifndef RTT_MAX_VALUE
#define RTT_MAX_VALUE    (0x00FFFFFF)    /* 24-bit timer */
#endif
/* possible values: 32, 128, 256, 512, 1024, 4096, 32768 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY    (1024U)         /* in Hz. */
#endif
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
