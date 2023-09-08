/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lcd LCD display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for the LCD display
 *
 * The LCD is a generic display driver for small RGB displays. It communicates
 * with the device either via an
 *
 * - SPI serial interface (if module `lcd_spi` enabled) or an
 * - MCU 8080 8-/16-bit parallel interface (if module `lcd_parallel` or
 *   module `lcd_parallel_16` is enabled).
 *
 * Usually the device driver is used either for a single display with SPI serial
 * interface or for a display with parallel MCU 8080 8-/16-bit parallel
 * interface. However, the device driver can also be used simultaneously for
 * multiple displays with different interfaces if several of the `lcd_spi`,
 * `lcd_parallel` and `lcd_parallel_16bit` modules are enabled at the same time.
 * In this case, please refer to the notes in @ref lcd_params_t.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_LCD_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef LCD_H
#define LCD_H

#include "board.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert little endian colors to big endian.
 *
 * Compile time switch to change the driver to convert little endian
 * colors to big endian.
 */
#ifdef DOXYGEN
#define CONFIG_LCD_LE_MODE
#endif

/**
 * @name Memory access control bits
 * @{
 */
#define LCD_MADCTL_MY           0x80    /**< Row address order */
#define LCD_MADCTL_MX           0x40    /**< Column access order */
#define LCD_MADCTL_MV           0x20    /**< Row column exchange */
#define LCD_MADCTL_ML           0x10    /**< Vertical refresh order */
#define LCD_MADCTL_BGR          0x08    /**< Color selector switch control */
#define LCD_MADCTL_MH           0x04    /**< Horizontal refresh direction */
/** @} */

#if MODULE_LCD_PARALLEL || DOXYGEN
/**
 * @brief   Display interface modi
 *
 * This enumeration is only needed if the MCU 8080 8-/16-bit interfaces are
 * enabled by `lcd_parallel` or `lcd_parallel_16bit`. Otherwise the serial
 * SPI interface is implicitly assumed.
 */
typedef enum {
    LCD_IF_SPI,             /**< SPI serial interface mode */
    LCD_IF_PARALLEL_8BIT,   /**< MCU 8080 8-bit parallel interface mode */
    LCD_IF_PARALLEL_16BIT,  /**< MCU 8080 16-bit parallel interface mode */
} lcd_if_mode_t;
#endif

/**
 * @brief   Device initialization parameters
 *
 * @note The device driver can be used simultaneously for displays with
 *       SPI serial interface and parallel MCU 8080 8-/16-bit interfaces
 *       if the modules `lcd_spi` and `lcd_parallel` or `lcd_parallel_16bit`
 *       are enabled at the same time. In this case the interface parameters
 *       for the SPI serial interface and the MCU 8080 parallel 8-/16-bit
 *       interfaces are defined. @ref lcd_params_t::spi must then be set to
 *       @ref SPI_UNDEF for displays that use the MCU-8080-parallel-8-/16-bit
 *       interface, i.e. @ref lcd_params_t::spi is then used to detect the
 *       interface mode.
 */
typedef  struct {
#if MODULE_LCD_SPI || DOXYGEN
    /* Interface parameters used for serial interface */
    spi_t spi;                  /**< SPI device that the display is connected to */
    spi_clk_t spi_clk;          /**< SPI clock speed to use */
    spi_mode_t spi_mode;        /**< SPI mode */
#endif
#if MODULE_LCD_PARALLEL || DOXYGEN
    lcd_if_mode_t mode;         /**< LCD driver interface mode */
    /* Interface parameters used for MCU 8080 8-bit parallel interface */
    gpio_t wrx_pin;             /**< pin connected to the WRITE ENABLE line */
    gpio_t rdx_pin;             /**< pin connected to the READ ENABLE line */
    gpio_t d0_pin;              /**< pin connected to the D0 line */
    gpio_t d1_pin;              /**< pin connected to the D1 line */
    gpio_t d2_pin;              /**< pin connected to the D2 line */
    gpio_t d3_pin;              /**< pin connected to the D3 line */
    gpio_t d4_pin;              /**< pin connected to the D4 line */
    gpio_t d5_pin;              /**< pin connected to the D5 line */
    gpio_t d6_pin;              /**< pin connected to the D6 line */
    gpio_t d7_pin;              /**< pin connected to the D7 line */
#if MODULE_LCD_PARALLEL_16BIT || DOXYGEN
    /* Interface parameters used for MCU 8080 16-bit parallel interface */
    gpio_t d8_pin;              /**< pin connected to the D8 line */
    gpio_t d9_pin;              /**< pin connected to the D9 line */
    gpio_t d10_pin;             /**< pin connected to the D10 line */
    gpio_t d11_pin;             /**< pin connected to the D11 line */
    gpio_t d12_pin;             /**< pin connected to the D12 line */
    gpio_t d13_pin;             /**< pin connected to the D13 line */
    gpio_t d14_pin;             /**< pin connected to the D14 line */
    gpio_t d15_pin;             /**< pin connected to the D15 line */
#endif /* MODULE_LCD_PARALLEL_16BIT */
#endif /* MODULE_LCD_PARALLEL */
    /* Common interface parameters */
    gpio_t cs_pin;              /**< pin connected to the CHIP SELECT line */
    gpio_t dcx_pin;             /**< pin connected to the DC line */
    gpio_t rst_pin;             /**< pin connected to the RESET line */
    bool rgb;                   /**< True when display is connected in RGB mode\n
                                     False when display is connected in BGR mode */
    bool inverted;              /**< Display works in inverted color mode */
    uint16_t lines;             /**< Number of lines, from 16 to the number of
                                     lines supported by the driver IC in 8 line
                                     steps */
    uint16_t rgb_channels;      /**< Display rgb channels */
    uint8_t rotation;           /**< Display rotation mode */
    uint8_t offset_x;           /**< LCD offset to apply on x axis. */
    uint8_t offset_y;           /**< LCD offset to apply on y axis. */
#if MODULE_LCD_MULTI_CNTRL || DOXYGEN
    uint8_t cntrl;              /**< controller variant used, if the controller-
                                     specific driver supports multiple
                                     controller variants */
#endif
} lcd_params_t;

/**
 * @brief   LCD driver interface
 *
 * This define the functions to access a LCD.
 */
typedef struct lcd_driver lcd_driver_t;

/**
 * @brief   Device descriptor for a lcd
 */
typedef struct {
#if MODULE_DISP_DEV || DOXYGEN
    disp_dev_t *dev;                /**< Pointer to the generic display device */
#endif
    const lcd_driver_t *driver;     /**< LCD driver */
    const lcd_params_t *params;     /**< Device initialization parameters */
#if MODULE_LCD_PARALLEL || DOXYGEN
    mutex_t lock;                   /**< Mutex used to lock the device in
                                         MCU 8080 parallel interface mode */
#endif
#if MODULE_LCD_PARALLEL_16BIT || DOXYGEN
    bool word_access;               /**< indicates that a word access is active */
#endif
} lcd_t;

/**
 * @brief   LCD driver interface
 *
 * This defines the functions to access a LCD.
 */
struct lcd_driver {
    /**
     * @brief   Initialize LCD controller
     *
     * @param[in] dev  Pointer to the selected driver
     *
     * @returns 0 on success
     * @returns < 0 value in error
     */
    int (*init)(lcd_t *dev, const lcd_params_t *params);

    /**
     * @brief   Set the LCD work area
     *
     * This function pointer can be NULL if the controller specific driver
     * does not require anything special. In this case the default
     * implementation is used which sets the column addresses and the row
     * addresses of the area including the coordinates of the opposite corner.
     *
     * @param[in] dev  Pointer to the selected driver
     * @param[in] x1   x coordinate of the first corner
     * @param[in] x2   x coordinate of the opposite corner
     * @param[in] y1   y coordinate of the first corner
     * @param[in] y2   y coordinate of the opposite corner
     *
     */
    void (*set_area)(lcd_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                     uint16_t y2);
};

/**
 * @name    Low-level LCD API
 *
 * Low-level functions are used to acquire a device, write commands with data
 * to the device, or read data from the device and release it when it is no
 * longer needed. They are usually called by the high-level functions such
 * as @ref lcd_init, @ref lcd_fill, @ref lcd_pixmap, etc., but can also be
 * used by the application to implement low-level operations if needed.
 *
 * @{
 */
/**
 * @brief   Low-level function to acquire the device
 *
 * @param[out]  dev     device descriptor
 */
void lcd_ll_acquire(lcd_t *dev);

/**
 * @brief   Low-level function to release the device
 *
 * @param[out]  dev     device descriptor
 */
void lcd_ll_release(lcd_t *dev);

/**
 * @brief   Low-level function to write a command
 *
 * @pre The device must have already been acquired with @ref lcd_ll_acquire
 *      before this function can be called.
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command code
 * @param[in]   data    command data to the device or NULL for commands without data
 * @param[in]   len     length of the command data or 0 for commands without data
 */
void lcd_ll_write_cmd(lcd_t *dev, uint8_t cmd, const uint8_t *data,
                      size_t len);

/**
 * @brief   Low-level function for read command
 *
 * @note Very often the SPI MISO signal of the serial interface or the RDX
 *       signal of the MCU 8080 parallel interface are not connected to the
 *       display. In this case the read command does not provide valid data.
 *
 * @pre The device must have already been acquired with @ref lcd_ll_acquire
 *      before this function can be called.
 * @pre len > 0
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command
 * @param[out]  data    data from the device
 * @param[in]   len     length of the returned data
 */
void lcd_ll_read_cmd(lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len);

/**
 * @brief   Set the LCD work area
 *
 * @param[in] dev  Pointer to the selected driver
 * @param[in] x1   x coordinate of the first corner
 * @param[in] x2   x coordinate of the opposite corner
 * @param[in] y1   y coordinate of the first corner
 * @param[in] y2   y coordinate of the opposite corner
 *
 */
void lcd_ll_set_area(lcd_t *dev, uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2);
/** @} */

/**
 * @name    High-level LCD API
 *
 * The functions of the high-level LCD API are used by the application. They
 * use the low-level LCD API to implement more complex operations.
 *
 * @{
 */
/**
 * @brief   Setup an LCD display device
 *
 * @param[in]   dev     device descriptor
 * @param[in]   params  parameters for device initialization
 */
int lcd_init(lcd_t *dev, const lcd_params_t *params);

/**
 * @brief   Fill a rectangular area with a single pixel color
 *
 * the rectangular area is defined as x1 being the first column of pixels and
 * x2 being the last column of pixels to fill. similar to that, y1 is the first
 * row to fill and y2 is the last row to fill.
 *
 * @param[in]   dev     device descriptor
 * @param[in]   x1      x coordinate of the first corner
 * @param[in]   x2      x coordinate of the opposite corner
 * @param[in]   y1      y coordinate of the first corner
 * @param[in]   y2      y coordinate of the opposite corner
 * @param[in]   color   single color to fill the area with
 */
void lcd_fill(lcd_t *dev, uint16_t x1, uint16_t x2,
              uint16_t y1, uint16_t y2, uint16_t color);

/**
 * @brief   Fill a rectangular area with an array of pixels
 *
 * the rectangular area is defined as x1 being the first column of pixels and
 * x2 being the last column of pixels to fill. similar to that, y1 is the first
 * row to fill and y2 is the last row to fill.
 *
 * @note @p color must have a length equal to `(x2 - x1 + 1) * (y2 - y1 + 1)`
 *
 * @param[in]   dev     device descriptor
 * @param[in]   x1      x coordinate of the first corner
 * @param[in]   x2      x coordinate of the opposite corner
 * @param[in]   y1      y coordinate of the first corner
 * @param[in]   y2      y coordinate of the opposite corner
 * @param[in]   color   array of colors to fill the area with
 */
void lcd_pixmap(lcd_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                uint16_t y2, const uint16_t *color);

/**
 * @brief   Raw write command
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command code
 * @param[in]   data    command data to the device or NULL for commands without data
 * @param[in]   len     length of the command data or 0 for commands without data
 */
void lcd_write_cmd(lcd_t *dev, uint8_t cmd, const uint8_t *data,
                   size_t len);

/**
 * @brief   Raw write command sequence
 *
 * The LCD driver allows to write a sequence of commands to the display
 * controller defined as a sequence of CLP tuples (command, length, parameter).
 * This can be useful, for example
 * - to send an initialization sequence to a controller that is not supported
 *   by a controller-specific driver (see also @ref lcd_params_t::init_seq
 *   and @ref lcd_init) or
 * - to write specific configuration parameters that are not covered by the
 *   controller-specific driver.
 *
 * The format of a CLP tuple is:
 * - one byte the command index
 * - one byte the length of the following parameters, i.e. the number of bytes
 * representing the parameters for the command. If the command has no
 * parameters, the length is 0.
 * - n bytes the parameters of the command.
 *
 * If @ref LCD_DELAY is used as the command index, the command is not sent
 * to the display controller, but a delay is inserted, where the length
 * in the CLP tuple then defines the delay in milliseconds.
 *
 * The following example shows the initialization sequence for a display
 * with a ST7789:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * static const uint8_t st7789_default_init[] =  {
 *     LCD_CMD_SWRESET, 0,              // Soft Reset
 *     LCD_DELAY, 120,                  // Soft Reset needs 120 ms if in Sleep In mode
 *     LCD_CMD_SLPOUT, 0,               // Sleep Out leave Sleep In state after reset
 *     LCD_DELAY, 120,                  // Sleep Out needs 120 ms
 *     LCD_CMD_VCOMS, 1, 0x20,          // VCOM=0.9V
 *     LCD_CMD_VRHS, 1, 0x0b,           // VRH=4.1V
 *     LCD_CMD_VDVS, 1, 0x20,           // VDV=0V
 *     LCD_CMD_VCMOFSET, 1, 0x20,       // VCOMFS=0V
 *     LCD_CMD_PWCTRL1X, 2, 0xa4, 0xa1, // AVDD=6.8V, AVCL=4.8V, VDS=2.3
 *     LCD_CMD_PGAMCTRL, 14,            // Positive Voltage Gamma Control
 *         0xd0, 0x08, 0x11, 0x08, 0x0c, 0x15, 0x39,
 *         0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2d,
 *     LCD_CMD_NGAMCTRL, 14,            // Negative Voltage Gamma Control
 *         0xd0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39,
 *         0x44, 0x51, 0x0b, 0x16, 0x14, 0x2f, 0x32,
 *     LCD_CMD_COLMOD, 1, 0x055,        // 16 bit mode RGB & Control interface pixel format
 *     LCD_CMD_MADCTL, 1, LCD_MADCTL_MV | LCD_MADCTL_MX,
 *     LCD_CMD_DINVON, 0,               // enable Inversion, reset default is off
 *     LCD_CMD_SLPOUT, 0,               // Sleep out (turn off sleep mode)
 *     LCD_CMD_NORON, 0,                // Normal display mode on
 *     LCD_DELAY, 1,
 *     LCD_CMD_DISPON, 0,               // Display on
 * };
 *
 * lcd_write_cmd_sequence(&dev, st7789_default_init, sizeof(st7789_default_init));
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @param[in]   dev     device descriptor
 * @param[in]   seq     command sequence of CLP tuples as a sequence of bytes
 * @param[in]   seq_len length of the command sequence
 */
void lcd_write_cmd_sequence(const lcd_t *dev, const uint8_t *seq, size_t seq_len);

/**
 * @brief   Raw read command
 *
 * @note Very often the SPI MISO signal of the serial interface or the RDX
 *       signal of the MCU 8080 parallel interface are not connected to the
 *       display. In this case the read command does not provide valid data.
 *
 * @pre         len > 0
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command
 * @param[out]  data    data from the device
 * @param[in]   len     length of the returned data
 */
void lcd_read_cmd(lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len);

/**
 * @brief   Invert the display colors
 *
 * @param[in]   dev     device descriptor
 */
void lcd_invert_on(lcd_t *dev);

/**
 * @brief   Disable color inversion
 *
 * @param[in]   dev     device descriptor
 */
void lcd_invert_off(lcd_t *dev);
/** @} */

#if MODULE_LCD_PARALLEL || DOXYGEN
/**
 * @name    Low-level MCU 8080 8-/16-bit parallel interface
 *
 * The low-level MCU 8080 8-/16-bit parallel interface (low-level parallel
 * interface for short) is used when the LCD device is connected via a parallel
 * interface. Either the GPIO-driven low-level parallel interface provided by
 * this LCD driver or a low-level parallel interface implemented by the MCU,
 * such as the STM32 FMC peripheral, can be used. If the MCU provides its
 * own implementation of the low-level parallel interface, it can be used
 * by implementing the following low-level parallel interface driver functions,
 * enabling the `lcd_parallel_ll_mcu` module and defining the
 * @ref lcd_ll_par_driver variable of type @ref lcd_ll_par_driver_t.
 *
 * @{
 */

/**
 * @brief   Low-level MCU 8080 8-/16-bit parallel interface driver
 *
 * If the MCU-driven low-level parallel interface is enabled by
 * module `lcd_ll_parallel_mcu`, the implementation of the MCU low-level
 * parallel interface has to define a variable @ref lcd_ll_par_driver of this
 * type. All or a set of members have to point to the low-level parallel
 * interface functions implemented by the MCU. For functions that are not
 * implemented by the MCU, the members have to be set to the corresponding
 * GPIO-driven low-level parallel interface functions provided by the LCD
 * driver.
 */
typedef struct {
    /**
     * @brief   Initialize the MCU-driven low-level parallel interface
     *
     * @param[in]   dev     device descriptor
     */
    void (*init)(lcd_t *dev);

    /**
     * @brief   Set the data direction of the low-level parallel interface
     *
     * @param[in]   dev     device descriptor
     * @param[in]   output  set to output mode if true and to input mode otherwise
     */
    void (*set_data_dir)(lcd_t *dev, bool output);

    /**
     * @brief   Write command using the MCU-driven low-level parallel interface
     *
     * @param[in]   dev     device descriptor
     * @param[in]   cmd     command
     * @param[in]   cont    operation is continued
     */
    void (*cmd_start)(lcd_t *dev, uint8_t cmd, bool cont);

    /**
     * @brief   Write a byte using the MCU-driven low-level parallel interface
     *
     * @param[in]   dev     device descriptor
     * @param[in]   cont    operation is continued
     * @param[in]   out     byte to be written
     */
    void (*write_byte)(lcd_t *dev, bool cont, uint8_t out);

    /**
     * @brief   Read a byte using the MCU-driven low-level parallel interface
     *
     * @param[in]   dev     device descriptor
     * @param[in]   cont    operation is continued
     *
     * @return  byte read
     */
    uint8_t (*read_byte)(lcd_t *dev, bool cont);

#if MODULE_LCD_PARALLEL_16BIT || DOXYGEN
    /**
     * @brief   Write a word using the MCU-driven low-level parallel interface
     *
     * @param[in]   dev     device descriptor
     * @param[in]   cont    operation is continued
     * @param[in]   out     word to be written
     */
    void (*write_word)(lcd_t *dev, bool cont, uint16_t out);

    /**
     * @brief   Read a word using the MCU-driven low-level parallel interface
     *
     * @param[in]   dev     device descriptor
     * @param[in]   cont    operation is continued
     *
     * @return  word read
     */
    uint16_t (*read_word)(lcd_t *dev, bool cont);
#endif
} lcd_ll_par_driver_t;

/**
 * @brief   Low-level parallel interface driver instance
 */
extern const lcd_ll_par_driver_t lcd_ll_par_driver;

/** @} */
#endif

#ifdef __cplusplus
}
#endif
#endif /* LCD_H */
/** @} */
