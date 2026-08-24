/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup   drivers_ads131m02 ADS131M02 ADC device driver
 * @ingroup    drivers_sensors
 * @brief      SPI Analog-to-Digital Converter device driver
 *
 * @{
 *
 * @file
 * @brief      ads131m02 ADC device driver
 *
 * Implementation does not yet feature:
 * - CRC
 * - Global chop mode
 * - Current detect mode
 * - Calibration
 * - Channel phase shift
 * - Register locking
 * - any other word length than 24 bit
 * - turbo mode (OSR 64)
 *
 * @author     Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author     Fabian Hüßler <fabian.hussler@ml-pa.com>
 */


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Nominal voltage reference in nanovolts
 */
#define ADS131M02_VREF_NV           1200000000UL
/**
 * @brief   Number of ADC channels
 */
#define ADS131M02_CHANNELS_NUMOF    2
/**
 * @brief   Number of bytes per ADC word
 */
#define ADS131M02_WORD_LEN          3
/**
 * @brief   Number of words per ADC frame (response + CH0 + CH1 + CRC/zero)
 */
#define ADS131M02_FRAME_WORDS       4
/**
 * @brief   Number of frames the internal FIFO can hold
 */
#define ADS131M02_FIFO_LEN          2
/**
 * @brief   Safely convert a channel number to @ref ads131m02_channel_t
 */
#define ADS131M02_CHANNEL(ch)       ((ch) % ADS131M02_CHANNELS_NUMOF)
/**
 * @brief   Safely convert a channel number to @ref ads131m02_channel_mask_t
 */
#define ADS131M02_CHANNEL_MASK(ch)  (1u << (ADS131M02_CHANNEL(ch)))

/**
 * @brief   ADS131M02 ADC channel number
 */
typedef enum {
    /**
     * @brief ADC channel 0
     */
    ads131m02_channel_0 = ADS131M02_CHANNEL(0),
    /**
     * @brief ADC channel 1
     */
    ads131m02_channel_1 = ADS131M02_CHANNEL(1),
} ads131m02_channel_t;

/**
 * @brief    Channel bitmask values for @ref ads131m02_channel_mask_t
 */
typedef enum {
    /**
     * @brief Select ADC channel 0
     */
    ads131m02_channel_mask_0 = ADS131M02_CHANNEL_MASK(ads131m02_channel_0),
    /**
     * @brief Select ADC channel 1
     */
    ads131m02_channel_mask_1 = ADS131M02_CHANNEL_MASK(ads131m02_channel_1),
} ads131m02_channel_mask_t;

/**
 * @brief   ADS131M02 params
 */
typedef struct {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    gpio_t ready_pin;       /**< GPIO pin connected to DRDY */
    gpio_t sync_pin;        /**< GPIO pin connected to SYNC / RESET */
    bool ready_pin_pullup;  /**< Must be true if external 100-kOhm resistor is used
                                 to pull DRDY high when inactive */
} ads131m02_params_t;

/**
 * @brief   ADS131M02 device structure
 */
typedef struct ads131m02 ads131m02_t;

/**
 * @brief   ADS131M02 DRDY callback type
 */
typedef void (*ads131m02_drdy_cb_t)(ads131m02_t *dev);

/**
 * @brief   ADS131M02 device structure
 */
struct ads131m02 {
    const ads131m02_params_t *params;       /**< Device driver configuration */
    ads131m02_drdy_cb_t isr_drdy;           /**< Callback when DRDY is asserted,
                                                 called in gpio ISR */
    bool standby;                           /**< True if device is in standby mode */
    uint8_t gain[ADS131M02_CHANNELS_NUMOF]; /**< Current gain configuration */
};

typedef struct {
    ads131m02_drdy_cb_t isr_drdy;       /**< Callback when DRDY is asserted,
                                             called in gpio ISR */
} ads131m02_init_t;

/**
 * @brief   Calculate the register value for the given samples per second
 */
#define ADS131M02_SPS(sps)          (((32000 - 1) / (sps)))

/**
 * @brief   ADS131M02 samples per second
 */
typedef enum {
    ads131m02_sps_32000 = ADS131M02_SPS(32000),     /**< 32000 samples per second */
    ads131m02_sps_16000 = ADS131M02_SPS(16000),     /**< 16000 samples per second */
    ads131m02_sps_8000 = ADS131M02_SPS(8000),       /**< 8000 samples per second */
    ads131m02_sps_4000 = ADS131M02_SPS(4000),       /**< 4000 samples per second */
    ads131m02_sps_2000 = ADS131M02_SPS(2000),       /**< 2000 samples per second */
    ads131m02_sps_1000 = ADS131M02_SPS(1000),       /**< 1000 samples per second */
    ads131m02_sps_500 = ADS131M02_SPS(500),         /**< 500 samples per second */
    ads131m02_sps_250 = ADS131M02_SPS(250),         /**< 250 samples per second */
    ads131m02_sps_125 = ADS131M02_SPS(125),         /**< 125 samples per second */
} ads131m02_sps_t;

/**
 * @brief   ADS131M02 channel configuration
 */
typedef struct {
    /**
     * @brief   Absolute positive or negative voltage in mV to select the gain
     */
    uint16_t abs_mv;
} ads131m02_channel_config_t;

/**
 * @brief   ADS131M02 start sampling configuration
 */
typedef struct {
    /**
     * @brief   Samples per second required
     */
    ads131m02_sps_t sps;
    /**
     * @brief   Bitmask of channels to enable @ref ads131m02_channel_mask_t
     */
    unsigned ch_mask;
    /**
     * @brief  Channel configurations for each channel
     */
    ads131m02_channel_config_t ch[ADS131M02_CHANNELS_NUMOF]; /**< Channel configurations */
} ads131m02_start_t;

/**
 * @brief   ADS131M02 stop sampling configuration
 */
typedef struct {
    /**
     * @brief   Bitmask of channels to disable @ref ads131m02_channel_mask_t
     */
    unsigned ch_mask;
} ads131m02_stop_t;

/**
 * @brief   ADS131M02 resume sampling configuration
 */
typedef struct {
    /**
     * @brief   Bitmask of channels to enable @ref ads131m02_channel_mask_t
     */
    unsigned ch_mask;
} ads131m02_resume_t;

/**
 * @brief   Enable or disable the external reference clock (CLKIN)
 *
 * If the ADS131M02 is driven from an external reference clock rather
 * than its internal oscillator, boards providing that clock should
 * override this function to enable it before sampling and disable it
 * again once sampling is done. The default implementation does nothing.
 *
 * @param[in] dev       Device descriptor
 * @param[in] on        True to enable the clock, false to disable it
 */
void board_ads131m02_clk_ctrl(ads131m02_t *dev, bool on);

/**
 * @brief   Initialize an ADS131M02 ADC device
 *
 * @param[in,out] dev  Device descriptor
 * @param[in] params   Device hardware configuration
 * @param[in] init     Device initialization parameters
 *
 * @retval      0       Successful initialization
 * @retval      <0      Error code
 */
int ads131m02_init(ads131m02_t *dev, const ads131m02_params_t *params,
                   const ads131m02_init_t *init);

/**
 * @brief   Start sampling on the ADS131M02 ADC device
 *
 * @param[in] dev           Device descriptor
 * @param[in] config        Device sampling configuration
 * @param[in] f_clkin_hz    Externally provided clock frequency in Hz
 *
 * @retval    0         Successful initialization
 * @retval    <0        Error code
 * @retval   -EINVAL    Invalid configuration
 * @retval   -ECANCELED  Device is in standby mode
 */
int ads131m02_start(ads131m02_t *dev, const ads131m02_start_t *config, uint32_t f_clkin_hz);

/**
 * @brief   Stop sampling on the ADS131M02 ADC device
 *
 * @param[in] dev       Device descriptor
 * @param[in] stop      Device stop configuration
 *
 * @retval      0           Success
 * @retval      -ECANCELED  Device is in standby mode
 */
int ads131m02_stop(ads131m02_t *dev, const ads131m02_stop_t *stop);

/**
 * @brief   Resume sampling on the ADS131M02 ADC device
 *
 * @param[in] dev       Device descriptor
 * @param[in] resume    Device resume configuration
 *
 * @retval      0           Success
 * @retval      -ECANCELED  Device is in standby mode
 */
int ads131m02_resume(ads131m02_t *dev, const ads131m02_resume_t *resume);

/**
 * @brief   Sample data from the ADS131M02 ADC
 *
 * This should not be called from the DRDY ISR.
 * Use events to offload the sampling to a thread context.
 *
 * @param[in] dev           Device descriptor
 * @param[out] chan0        Buffer to store channel 0 samples or NULL if not enabled
 * @param[out] chan1        Buffer to store channel 1 samples or NULL if not enabled
 * @param[out] chan0_numof  Number of samples read for channel 0
 * @param[out] chan1_numof  Number of samples read for channel 1
 *
 * @retval      0           Success
 * @retval      <0          Error code
 * @retval      -EAGAIN     FIFO overflow, new samples produced while reading FIFO
 * @retval      -EINVAL     Invalid parameters
 * @retval      -ECANCELED  Device is in standby mode
 */
int ads131m02_sample(ads131m02_t *dev,
                     int32_t chan0[ADS131M02_FIFO_LEN + 1], int32_t chan1[ADS131M02_FIFO_LEN + 1],
                     unsigned *chan0_numof, unsigned *chan1_numof);

/**
 * @brief   Put device into low power standby mode, disabling ADC conversions and main clock
 *
 * The device must be woken up using @ref ads131m02_wakeup to resume sampling.
 *
 * @param[in] dev       Device descriptor
 *
 * @retval      0           Success
 * @retval      <0          Error code
 * @retval      -ECANCELED  Device is already in standby mode
 */
int ads131m02_standby(ads131m02_t *dev);

/**
 * @brief   Wake up device from standby mode
 *
 * ADC conversion will continue if it was running before standby.
 *
 * @param[in] dev       Device descriptor
 *
 * @retval      0           Success
 * @retval      <0          Error code
 * @retval      -ECANCELED  Device is not in standby mode
 */
int ads131m02_wakeup(ads131m02_t *dev);

/**
 * @brief   Switch ADC input to positive test signal.
 *
 * @param[in] dev       Device descriptor
 * @param[in] ch        Channel to configure
 *
 * @retval      0           Success
 * @retval      <0          Error code
 */
int ads131m02_mux_test_signal_positive(ads131m02_t *dev, ads131m02_channel_t ch);

/**
 * @brief   Switch ADC input to negative test signal.
 *
 * @param[in] dev       Device descriptor
 * @param[in] ch        Channel to configure
 *
 * @retval      0           Success
 * @retval      <0          Error code
 */
int ads131m02_mux_test_signal_negative(ads131m02_t *dev, ads131m02_channel_t ch);

/**
 * @brief   Switch ADC input to analog input (AINx).
 *
 * @param[in] dev       Device descriptor
 * @param[in] ch        Channel to configure
 *
 * @retval      0           Success
 * @retval      <0          Error code
 */
int ads131m02_mux_ain(ads131m02_t *dev, ads131m02_channel_t ch);

#ifdef __cplusplus
}
#endif

/** @} */
