/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_spi DMA
 * @ingroup     drivers_periph
 * @brief       Low-level DMA peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level DMA peripheral driver interface definitions
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 */
#ifndef DMA_H
#define DMA_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef dma_t
#define dma_t                       uint32_t
#endif

/**
 * @brief Total number of stream.
 *
 * For simplifying DMA stream handling, we map the DMA channels transparently to
 * one integer number, such that DMA1 stream0 equals 0, DMA2 stream0 equals 8,
 * DMA2 stream 7 equals 15 and so on.
 *
 */
#define DMA_NUMOF                   (16)

/**
 * @brief Get mutually exclusive access to the given DMA configuration
 *
 * In case the DMA configuration is busy, this function will block until the
 * bus is free again.
 * this function should be called before any configuration.
 *
 * @param[in] stream_dev        DMA device to access
 *
 * @return                      0 on success
 * @return                      -1 on error
 */
int dma_conf_acquire(dma_t stream_dev);

/**
 * @brief Release the given DMA configuration to be used by others
 *
 * This function should be called at the end of transmission.
 *
 * @param[in] stream_dev        DMA device to release
 *
 * @return                      0 on success
 * @return                      -1 on error
 */
int dma_conf_release(dma_t stream_dev);

/**
 * @brief Get mutually exclusive access to the given DMA transmission
 *
 * This function will block until the end of transmission. End of transmission
 * is done by interruption internally of this driver.
 * This function should be called after configuration and DMA_run functions.
 *
 * @param[in] stream_dev        DMA device to access
 *
 * @return                      0 on success
 * @return                      -1 on error
 */
int dma_transmission_acquire(dma_t stream_dev);

/**
 * @brief Release the given DMA transmission to be used by others
 *
 * This function releases the thread to continue its work. This function is
 * called internally of this driver during interruption at the end of
 * transmission.
 * this function should be called by users.
 *
 * @param[in] stream_dev        DMA device to release
 *
 * @return                      0 on success
 * @return                      -1 on error
 */
int dma_transmission_release(dma_t stream_dev);

/**
 * @brief Initialize the given DMA device
 *
 * All registers are initialized to 0.
 * All flags are initialized.
 * Configuration and transmission mutex are initialized in this function.
 * The transmission mutex is locked in this function to block any thread want
 * to make a transmission and wait until the end of the transmission.
 *
 * This function should be called in init function to configure correctly the
 * right operation of the transmission mutex.
 *
 * @param[out] stream_dev       DMA device to initialize
 *
 */
void dma_stream_init(dma_t stream_dev);

/**
 * @brief Initialize the given DMA device
 *
 * This function should be called at any operation with DMA as long as the
 * stream and channel could be shared between resources.
 *
 * @param[out] stream_dev       DMA device to configure
 * @param[in] periph_addr_reg   register to send or read data from the corresponding device (SPI, UART ...)
 * @param[in] dma_config        configuration of DMA
 * @param[in/out] data          Byte to send out or read.
 * @param[in] length            Number of bytes to transfer
 *
 */
void dma_stream_config(dma_t stream_dev, uint32_t periph_addr_reg, uint32_t dma_config, void* data, uint16_t length);

/**
 * @brief enable the given DMA device
 *
 * After configuration, the DMA should be enabled to initiate the DMA
 * transmission.
 *
 * @param[out] stream_dev       DMA device to configure
 *
 */
void dma_enable(dma_t stream_dev);

/**
 * @brief disable the given DMA device
 *
 * At the end of transmission the DMA could be disabled.
 *
 * @param[out] stream_dev       DMA device to configure
 *
 */
void dma_disable(dma_t stream_dev);

/**
 * @brief clear the ifc flag to the giver DMA
 *
 * The ifc flag indicates when the transmission in completely done. If the flag
 * is not cleared, the interruption is still activated.
 * At the end of transmission, all corresponding flags are cleared in
 * interruption. no need to do it.
 *
 * @param[out] stream_dev       DMA device to configure
 *
 */
void dma_clear_ifc_flag(dma_t stream_dev);

#ifdef __cplusplus
}
#endif

#endif /* DMA_H */
/** @} */
