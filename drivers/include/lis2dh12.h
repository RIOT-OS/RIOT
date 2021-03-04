/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lis2dh12 LIS2DH12 Accelerometer
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for the STM LIS2DH12 accelerometer
 *
 * This device driver provides a minimal interface to LIS2DH12 devices. As of
 * now, it only provides very basic access to the device. The driver configures
 * the device to continuously read the acceleration data with statically
 * defined scale and rate, and with a fixed 10-bit resolution. The LIS2DH12's
 * FIFO is bypassed, so the driver might not be sufficient for use cases where
 * the complete history of readings is of interest.
 *
 * Also, the current version of the driver supports only interfacing the sensor
 * via SPI. The driver is however written in a way, that adding I2C interface
 * support is quite simple, as all bus related functions (acquire, release,
 * read, write) are cleanly separated in the code.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 * @file
 * @brief       Interface definition for the STM LIS2DH12 accelerometer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LIS2DH12_H
#define LIS2DH12_H

#include <stdint.h>

#include "saul.h"
#include "lis2dh12_registers.h"

#include "periph/gpio.h"
#ifdef MODULE_LIS2DH12_SPI
#include "periph/spi.h"
#else
#include "periph/i2c.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MODULE_LIS2DH12) || defined(DOXYGEN)
/**
 * @brief   Default I2C slave address for LIS2DH12 devices
 */
#define LIS2DH12_ADDR_DEFAULT       (0x19)
#endif

/**
 * @brief   Available scale values
 */
typedef enum {
    LIS2DH12_SCALE_2G  = 0x00,      /**< +- 2g */
    LIS2DH12_SCALE_4G  = 0x10,      /**< +- 4g */
    LIS2DH12_SCALE_8G  = 0x20,      /**< +- 8g */
    LIS2DH12_SCALE_16G = 0x30,      /**< +- 16g */
} lis2dh12_scale_t;

/**
 * @brief   Available sampling rates
 *
 */
typedef enum {
    LIS2DH12_RATE_1HZ   = 0x1,      /**< sample with 1Hz @ all power modes */
    LIS2DH12_RATE_10HZ  = 0x2,      /**< sample with 10Hz @ all power modes */
    LIS2DH12_RATE_25HZ  = 0x3,      /**< sample with 25Hz @ all power modes */
    LIS2DH12_RATE_50HZ  = 0x4,      /**< sample with 50Hz @ all power modes */
    LIS2DH12_RATE_100HZ = 0x5,      /**< sample with 100Hz @ all power modes */
    LIS2DH12_RATE_200HZ = 0x6,      /**< sample with 200Hz @ all power modes */
    LIS2DH12_RATE_400HZ = 0x7,      /**< sample with 400Hz @ all power modes */
    LIS2DH12_RATE_1620HZ = 0x8,     /**< sample with 1620HZ @ Low Power*/
    LIS2DH12_RATE_VERYHIGH = 0x9,   /**< sample with 1344Hz @ High resolution or \
                                        5376Hz @ Low Power*/
} lis2dh12_rate_t;

/**
 * @brief   Available power modes
 *
 */
typedef enum {
    LIS2DH12_POWER_DOWN   = 0,  /**< power down the device */
    LIS2DH12_POWER_LOW    = 1,  /**< low power mode */
    LIS2DH12_POWER_NORMAL = 2,  /**< normal mode */
    LIS2DH12_POWER_HIGH   = 3,  /**< high resolution */
} lis2dh12_powermode_t;

/**
 * @brief   LIS2DH12 configuration parameters
 */
typedef struct {
#if MODULE_LIS2DH12_SPI || DOXYGEN
    spi_t spi;                      /**< SPI bus the device is connected to */
    gpio_t cs;                      /**< connected chip select pin */
#else
    i2c_t i2c;                      /**< I2C bus the device is connected to */
    uint8_t addr;                   /**< device address on the I2C bus */
#endif
#if MODULE_LIS2DH12_INT || DOXYGEN
    gpio_t int1_pin;                /**< first interrupt pin */
    gpio_t int2_pin;                /**< second interrupt pin */
#endif
    lis2dh12_scale_t scale;         /**< sampling sensitivity used */
    lis2dh12_rate_t rate;           /**< sampling rate used */
    lis2dh12_powermode_t powermode; /**< power mode used*/
} lis2dh12_params_t;

/**
 * @brief   LIS2DH12 high pass modes
 */
typedef enum {
    LIS2DH12_HP_MODE_NORMAL    = 0x0, /**< normal mode, reset by reading REG_REFERENCE */
    LIS2DH12_HP_MODE_REFERENCE = 0x1, /**< uses the reference signal for filtering */
    LIS2DH12_HP_MODE_AUTORESET = 0x3, /**< automatically resets on interrupt generation */
} lis2dh12_hp_mode_t;

/**
 * @brief   LIS2DH12 high pass cutoff frequency
 */
typedef enum {
    LIS2DH12_HP_FREQ_DIV50  = 0, /**< cutoff freq is ODR divided by 50 */
    LIS2DH12_HP_FREQ_DIV100 = 1, /**< cutoff freq is ODR divided by 100 */
    LIS2DH12_HP_FREQ_DIV200 = 2, /**< cutoff freq is ODR divided by 200 */
    LIS2DH12_HP_FREQ_DIV400 = 3, /**< cutoff freq is ODR divided by 400 */
} lis2dh12_hp_freq_t;

/**
 * @brief   LIS2DH12 high pass config values
 */
typedef struct {
    lis2dh12_hp_mode_t Highpass_mode; /**< set the High pass mode */
    lis2dh12_hp_freq_t Highpass_freq; /**< set the High pass cutoff frequency \
                                            related to device rate */
    bool CLICK_enable;    /**< enables filter for click data */
    bool INT1_enable;     /**< enables filter for AOI on interrupt 1 */
    bool INT2_enable;     /**< enables filter for AOI on interrupt 2 */
    bool DATA_OUT_enable; /**< enables filter for data output */
} lis2dh12_highpass_t;

/**
 * @brief   LIS2DH12 click config values
 */
typedef struct {
    bool enable_DOUBLE;     /**< otherwise single click for given axis are enabled */
    bool enable_X_CLICK;    /**< enable double pr single click for X axes */
    bool enable_Y_CLICK;    /**< enable double pr single click for Y axes */
    bool enable_Z_CLICK;    /**< enable double pr single click for Z axes */
    bool noINT_latency;     /**< if "0" interrupt stays high for TIME_latency setting \
                                if "1" interrupt stays high until CLICK_SRC is read */
    uint8_t CLICK_thold:7;  /**< set click threshold */
    uint8_t TIME_limit:7;   /**< set number of ODR cycles for time limit over threshold value */
    uint8_t TIME_latency;   /**< set number of ODR cycles for latency after a click */
    uint8_t TIME_window;    /**< set number of ODR cycles for window between clicks */
} lis2dh12_click_t;

/**
 * @brief   LIS2DH12 device descriptor
 */
typedef struct {
    const lis2dh12_params_t *p;     /**< device configuration */
    uint8_t comp;                   /**< scale compensation factor */
} lis2dh12_t;

/**
 * @brief   Status and error return codes
 */
enum {
    LIS2DH12_OK    =  0,            /**< everything was fine */
    LIS2DH12_NOBUS = -1,            /**< bus interface error */
    LIS2DH12_NODEV = -2,            /**< unable to talk to device */
    LIS2DH12_NOINT = -3,            /**< wrong interrupt line (has to be LIS2DH12_INT1
                                        or LIS2DH12_INT2) */
    LIS2DH12_NODATA= -4,            /**< no data available */
};

#if MODULE_LIS2DH12_INT || DOXYGEN
/*
 * @brief Interrupt lines
 */
enum {
    LIS2DH12_INT1 = 1,              /**< first interrupt line */
    LIS2DH12_INT2 = 2,              /**< second interrupt line */
};

/**
 * @brief    Parameter for interrupt configuration
 */
typedef struct {
    uint8_t int_config;             /**< values for configuration */
    uint8_t int_threshold:7;        /**< the threshold for triggering interrupt,
                                        threshold in range 0-127 */
    uint8_t int_duration:7;         /**< time between two interrupts ODR section in CTRL_REG1,
                                        duration in range 0-127 */
    uint8_t int_type;               /**< values for type of interrupts */
    gpio_cb_t cb;                   /**< the callback to execute */
    void *arg;                      /**< the callback argument */
} lis2dh12_int_params_t;
#endif /* MODULE_LIS2DH12_INT */

/**
 * @brief   LIS2DH12 FIFO data struct
 */
typedef struct {
    int16_t X_AXIS;    /**< X raw data in FIFO */
    int16_t Y_AXIS;    /**< Y raw data in FIFO */
    int16_t Z_AXIS;    /**< Z raw data in FIFO */
} lis2dh12_fifo_data_t;

/**
 * @brief   LIS2DH12 FIFO modes
 */
typedef enum {
    LIS2DH12_FIFO_MODE_BYPASS = 0,      /**< default mode, FIFO is bypassed */
    LIS2DH12_FIFO_MODE_FIFOMODE,        /**< normal FIFO mode, stops if FIFO is full */
    LIS2DH12_FIFO_MODE_STREAM,          /**< Stream mode, oldest values get overwritten */
    LIS2DH12_FIFO_MODE_STREAMtoFIFO,    /**< Stream mode and on interrupt jumps to FIFO mode */
} lis2dh12_fifo_mode_t;

/**
 * @brief   LIS2DH12 FIFO config values
 */
typedef struct {
    lis2dh12_fifo_mode_t FIFO_mode; /**< set FIFO mode */
    uint8_t FIFO_watermark:5;       /**< set the FIFO watermark level */
    bool FIFO_set_INT2;             /**< sets the FIFO interrupt to INT2, otherwise INT1 */
} lis2dh12_fifo_t;

/**
 * @brief   Export the SAUL interface for this driver
 */
extern const saul_driver_t lis2dh12_saul_driver;

#if MODULE_LIS2DH12_INT || DOXYGEN
/**
 * @brief   Set the interrupt values in LIS2DH12 sensor device
 *
 * @param[in] dev      device descriptor
 * @param[in] params   device interrupt configuration
 * @param[in] int_line number of interrupt line (LIS2DH12_INT1 or LIS2DH12_INT2)
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus errors
 */
int lis2dh12_set_int(const lis2dh12_t *dev, const lis2dh12_int_params_t *params, uint8_t int_line);

/**
 * @brief   Read an interrupt event on LIS2DH12 sensor device
 *
 * @param[in] dev      device descriptor
 * @param[out] data    device interrupt data
 * @param[in] int_line number of interrupt line (LIS2DH12_INT1 or LIS2DH12_INT2)
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus errors
 */
int lis2dh12_read_int_src(const lis2dh12_t *dev, uint8_t *data, uint8_t int_line);
#endif /* MODULE_LIS2DH12_INT */

/**
 * @brief   Set the FIFO configuration
 *
 * @param[in] dev       device descriptor
 * @param[in] config    device FIFO configuration
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_set_fifo(const lis2dh12_t *dev, const lis2dh12_fifo_t *config);

/**
 * @brief   Restart the FIFO mode
 *          this sets the FIFO mode in BYPASS mode and then back to previous mode
 *          Note: The LIS module disables the FIFO after interrupt automatically,
 *          it is recommended to set the FIFO in BYPASS mode and then back to old
 *          FIFO mode to enable the FIFO again.
 *
 * @param[in] dev       device descriptor
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_restart_fifo(const lis2dh12_t *dev);

/**
 * @brief   Read the FIFO source register
 *
 * @param[in] dev       device descriptor
 * @param[out] data     LIS2DH12_FIFO_SRC_REG_t content, allocate one byte
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_read_fifo_src(const lis2dh12_t *dev, LIS2DH12_FIFO_SRC_REG_t *data);

/**
 * @brief   This function will read a given number of data from FIFO
 *          reads amount of data that is available in FIFO
 *
 * @param[in] dev           device descriptor
 * @param[out] fifo_data    FIFO data, must have space for number of data
 * @param[in] number        amount of FIFO data to be read
 *
 * @return  number of valid data read from FIFO
 */
uint8_t lis2dh12_read_fifo_data(const lis2dh12_t *dev, lis2dh12_fifo_data_t *fifo_data,
                                uint8_t number);

/**
 * @brief   Initialize the given LIS2DH12 sensor device
 *
 * @param[out] dev      device descriptor
 * @param[in]  params   static device configuration
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus errors
 * @return  LIS2DH12_NODEV if no LIS2DH12 device was found on the bus
 */
int lis2dh12_init(lis2dh12_t *dev, const lis2dh12_params_t *params);

/**
 * @brief   Read acceleration data from the given device
 *
 * @param[in]  dev      device descriptor
 * @param[out] data     acceleration data in mili-g, **MUST** hold 3 values
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus error
 */
int lis2dh12_read(const lis2dh12_t *dev, int16_t *data);

/**
 * @brief   Clear the LIS2DH12 memory, clears all sampled data
 *
 * @param[in] dev       device descriptor
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_clear_data(const lis2dh12_t *dev);

/**
 * @brief   Change device scale value
 *
 * @param[in] dev       device descriptor
 * @param[in] scale     change to given scale value
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_set_scale(lis2dh12_t *dev, lis2dh12_scale_t scale);

/**
 * @brief   Change device sampling rate
 *
 * @param[in] dev       device descriptor
 * @param[in] rate      change to given sampling rate
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_set_datarate(const lis2dh12_t *dev, lis2dh12_rate_t rate);

/**
 * @brief   Change device power mode
 *
 * @param[in] dev           device descriptor
 * @param[in] powermode     change to given power mode
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_set_powermode(const lis2dh12_t *dev, lis2dh12_powermode_t powermode);

/**
 * @brief   Configures the high pass filter
 *
 * @param[in] dev       device descriptor
 * @param[in] config    device high pass configuration
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_set_highpass(const lis2dh12_t *dev, const lis2dh12_highpass_t *config);

/**
 * @brief   Set the reference value to control the high-pass reference.
 *          In LIS2DH12_HP_MODE_REFERENCE the reference value is used to filter data
 *          on all axis. Subtracts reference value from acceleration.
 *          Note: LSB changes according to LIS2DH12_SCALE
 *
 * @param[in] dev           device descriptor
 * @param[in] reference     reference value [8 Bit]
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_set_reference(const lis2dh12_t *dev, uint8_t reference);

/**
 * @brief   Read the reference value
 *
 * @param[in] dev       device descriptor
 * @param[out] data     reference value read from device
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_read_reference(const lis2dh12_t *dev, uint8_t *data);

/**
 * @brief   Set click configuration
 *
 * @param[in] dev       device descriptor
 * @param[in] config    device click configuration
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_set_click(const lis2dh12_t *dev, const lis2dh12_click_t *config);

/**
 * @brief   Read click source register
 *
 * @param[in] dev       device descriptor
 * @param[out] data     LIS2DH12_CLICK_SRC_t content, allocate one byte
 *
 * @return  LIS2DH12_OK on success
 */
int lis2dh12_read_click_src(const lis2dh12_t *dev, LIS2DH12_CLICK_SRC_t *data);

/**
 * @brief   Power on the given device and resets power mode and sampling rate
 *          to default values in the device descriptor parameters
 *
 * @param[in] dev       device descriptor
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus error
 */
int lis2dh12_poweron(const lis2dh12_t *dev);

/**
 * @brief   Power off the given device
 *
 * @param[in] dev       device descriptor
 *
 * @return  LIS2DH12_OK on success
 * @return  LIS2DH12_NOBUS on bus error
 */
int lis2dh12_poweroff(const lis2dh12_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* LIS2DH12_H */
/** @} */
