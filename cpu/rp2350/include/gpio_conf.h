#pragma once

/**
 * @brief   Possible function values for @ref gpio_io_ctrl_t::function_select
 */
typedef enum {
    FUNCTION_SELECT_SPI     = 1,    /**< connect pin to the SPI peripheral
                                     *   (MISO/MOSI/SCK depends on pin) */
    FUNCTION_SELECT_UART    = 2,    /**< connect pin to the UART peripheral
                                     *   (TXD/RXD depends on pin) */
    FUNCTION_SELECT_I2C     = 3,    /**< connect pin to the I2C peripheral
                                     *   (SCL/SDA depends on pin) */
    FUNCTION_SELECT_PWM     = 4,    /**< connect pin to the timer for PWM
                                     *   (channel depends on pin) */
    FUNCTION_SELECT_SIO     = 5,    /**< use pin as vanilla GPIO */
    FUNCTION_SELECT_PIO0    = 6,    /**< connect pin to the first PIO peripheral */
    FUNCTION_SELECT_PIO1    = 7,    /**< connect pin to the second PIO peripheral */
    FUNCTION_SELECT_CLOCK   = 8,    /**< connect pin to the timer (depending on pin: external clock,
                                     *   clock output, or not supported) */
    FUNCTION_SELECT_USB     = 9,    /**< connect pin to the USB peripheral
                                     *   (function depends on pin) */
    FUNCTION_SELECT_NONE    = 31,   /**< Reset value, pin unconnected */
} gpio_function_select_t;
