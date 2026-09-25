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
 * @brief   ADS131M02 frame type
 */
typedef struct ads131m02_frame {
    uint8_t f[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN];  /**< Frame data */
} ads131m02_frame_t;

/**
 * @brief   ADS131M02 frame buffer structure, coupling frame data with DMA descriptor
 */
typedef struct ads131m02_frame_buffer {
    ads131m02_frame_t *frames;  /**< Application provided frame buffers */
    void *desc;                 /**< DMA descriptor for the frame buffer */
} ads131m02_frame_buffer_t;

/**
 * @brief   ADS131M02 DRDY callback type
 */
typedef void (*ads131m02_drdy_cb_t)(ads131m02_t *dev, ads131m02_frame_buffer_t *frame_buf);

/**
 * @brief   ADS131M02 device structure
 */
struct ads131m02 {
    const ads131m02_params_t *params;           /**< Device driver configuration */
    ads131m02_drdy_cb_t drdy;                   /**< Data callback, called from ISR */
    ads131m02_frame_buffer_t *buf;              /**< Pointer to the current frame buffer */
    ads131m02_frame_buffer_t *frame_bufs;       /**< Frame buffers provided */
    unsigned frame_bufs_numof;                  /**< Number of frame buffers provided */
    bool standby;                               /**< True if device is in standby mode */
    bool active;                                /**< True if data acquisition is active */
#if MODULE_ADS131M02_EVENT
    event_channel_t event;                      /**< Event channel used for DMA transfers */
#endif
    uint8_t gain[ADS131M02_CHANNELS_NUMOF];     /**< Current gain configuration */
};

/**
 * @brief   Calculate the voltage range in millivolts for the given gain
 */
#define ADS131M02_ABS_MV(gain)      (ADS131M02_VREF_NV / 1000  / 1000 / (gain))

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
 * @brief   ADS131M02 initialization structure
 */
typedef struct ads131m02_init {
    ads131m02_frame_buffer_t *frame_bufs;    /**< Application provided frame buffers >= 2 */
    unsigned numof;                          /**< Number of frames buffers */
} ads131m02_init_t;

/**
 * @brief   ADS131M02 start sampling configuration
 */
typedef struct {
    /**
     * @brief   Samples per second required
     */
    unsigned sps;
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
} ads131m02_pause_t;

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
 * @param[in] init     Device runtime configuration
 *
 * @retval      0       Successful initialization
 * @retval      <0      Error code
 */
int ads131m02_init(ads131m02_t *dev, const ads131m02_params_t *params,
                   const ads131m02_init_t *init);

/**
 * @brief   Start sampling on the ADS131M02 ADC device
 *
 * @param[in]     dev           Device descriptor
 * @param[in,out] start         Device sampling configuration, adjusted with actual settings
 * @param[in]     f_clkin_hz    Externally provided clock frequency in Hz
 * @param[in]     drdy          DRDY callback function to be called upon transfer completion
 *
 * @retval    0         Successful initialization
 * @retval    <0        Error code
 * @retval   -EINVAL    Invalid configuration
 * @retval   -ECANCELED  Device is in standby mode
 */
int ads131m02_start(ads131m02_t *dev, ads131m02_start_t *start, uint32_t f_clkin_hz,
                    ads131m02_drdy_cb_t drdy);

/**
 * @brief   Pause sampling on the ADS131M02 ADC device
 *
 * @param[in] dev       Device descriptor
 * @param[in] stop      Device pause configuration
 *
 * @retval      0           Success
 * @retval      -ECANCELED  Device is in standby mode
 */
int ads131m02_pause(ads131m02_t *dev, const ads131m02_pause_t *stop);

/**
 * @brief   Stop sampling on the ADS131M02 ADC device
 *
 * @param[in] dev       Device descriptor
 *
 * @retval      0           Success
 * @retval      -ECANCELED  Device is in standby mode
 */
int ads131m02_stop(ads131m02_t *dev);

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
 * @brief   Sample data from the ADS131M02 ADC device
 *
 * This should not be called from the DRDY ISR.
 * Use events to offload the sampling to a thread context.
 *
 * @param[in] dev           Device descriptor
 * @param[out] chan         Buffers to store channel samples for every channel
 * @param[in]  nanovolt     True to scale the samples to nanovolts, false to keep raw values
 *
 * @retval      0           No samples
 * @retval      >0          Bitmask of channels with new samples
 * @retval      -ECANCELED  Device is in standby mode
 */
int ads131m02_sample(ads131m02_t *dev, int32_t chan[], bool nanovolt);

/**
 * @brief   Parse a frame of data from the ADS131M02 ADC
 *
 * @param[in] dev           Device descriptor
 * @param[out] chan         Buffers to store channel samples for every channel
 * @param[in]  nanovolt     True to scale the samples to nanovolts, false to keep raw values
 * @param[in]  frame        Frame containing the raw data from the ADC
 *
 * @retval      0           No samples
 * @retval      >0          Bitmask of channels with new samples
 */
ads131m02_channel_mask_t ads131m02_parse_frame(ads131m02_t *dev, int32_t chan[], bool nanovolt,
                                               ads131m02_frame_t *frame);

/**
 * @brief   Prepare a frame buffer for sampling
 *
 * @param[in] dev           Device descriptor
 * @param[in] frame_buf     Frame buffer to allocate
 */
void ads131m02_frame_buffer_alloc(ads131m02_t *dev, ads131m02_frame_buffer_t *frame_buf);

/**
 * @brief   Free a previously allocated frame buffer
 *
 * @param[in] dev           Device descriptor
 * @param[in] frame_buf     Frame buffer to free
 */
void ads131m02_frame_buffer_free(ads131m02_t *dev, ads131m02_frame_buffer_t *frame_buf);

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
