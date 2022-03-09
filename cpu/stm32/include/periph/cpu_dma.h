/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           DMA CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_DMA_H
#define PERIPH_CPU_DMA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   DMA configuration
 */
typedef struct {
    /** DMA stream on stm32f2/4/7, channel on others
     * STM32F2/4/7:
     *  - 0: DMA1 / Stream0
     *  - 1: DMA1 / Stream1
     *  - ...
     *  - 7: DMA1 / Stream7
     *  - 8: DAM2 / Stream0
     *  - ...
     *  - 15: DMA2 / Stream7
     * STM32F0/1/L0/1/4:
     *  - 0: DMA1 / Channel1
     *  - ...
     *  - 4: DMA1 / Channel5
     *  - ...
     *  - 6: DMA1 / Channel7
     *  - 7: Reserved
     *  - 8: DMA2 / Channel1
     *  - ...
     *  - 12: DMA2 / Channel5
     *  - ...
     *  - 14: DMA2 / Channel7
     */
    int stream;
} dma_conf_t;

/**
 * @brief   DMA type
 */
typedef unsigned dma_t;

/**
 * @brief   DMA modes
 */
typedef enum {
    DMA_PERIPH_TO_MEM = 0,     /**< Peripheral to memory */
    DMA_MEM_TO_PERIPH = 1,     /**< Memory to peripheral */
    DMA_MEM_TO_MEM = 2,        /**< Memory to memory */
} dma_mode_t;

/**
 * @brief   DMA channel/trigger configuration for DMA peripherals without
 *          channel/trigger filtering such as the stm32f1 and stm32f3.
 */
#define DMA_CHAN_CONFIG_UNSUPPORTED  (UINT8_MAX)

/**
 * @name    DMA Increment modes
 * @{
 */
#define DMA_INC_SRC_ADDR  (0x04)    /**< DMA increment source address */
#define DMA_INC_DST_ADDR  (0x08)    /**< DMA increment destination address */
#define DMA_INC_BOTH_ADDR (DMA_INC_SRC_ADDR | DMA_INC_DST_ADDR) /**< DMA increment source + destination address */
/** @} */

/**
 * @name    DMA data width
 * @{
 */
#define DMA_DATA_WIDTH_BYTE      (0x00) /**< Byte width */
#define DMA_DATA_WIDTH_HALF_WORD (0x01) /**< Half word width */
#define DMA_DATA_WIDTH_WORD      (0x02) /**< Word width */
#define DMA_DATA_WIDTH_MASK      (0x03) /**< Width mask */
#define DMA_DATA_WIDTH_SHIFT     (0)    /**< Width position */
/** @} */

#ifdef MODULE_PERIPH_DMA
/**
 * @brief   DMA stream not defined
 */
#define DMA_STREAM_UNDEF (UINT_MAX)

/**
 * @brief   Initialize DMA
 */
void dma_init(void);

/**
 * @brief   Execute a DMA transfer
 *
 * This function blocks until the transfer is completed. This is a convenience
 * function which configure, start, wait and stop a DMA transfer.
 *
 * @param[in]  dma     logical DMA stream
 * @param[in]  chan    DMA channel (on stm32f2/4/7, CxS or unused on others)
 * @param[in]  src     source buffer
 * @param[out] dst     destination buffer
 * @param[in]  len     length to transfer
 * @param[in]  mode    DMA mode
 * @param[in]  flags   DMA configuration
 *
 * @return < 0 on error, the number of transferred bytes otherwise
 */
int dma_transfer(dma_t dma, int chan, const volatile void *src, volatile void *dst, size_t len,
                 dma_mode_t mode, uint8_t flags);

/**
 * @brief   Acquire a DMA stream
 *
 * @param[in] dma     logical DMA stream
 */
void dma_acquire(dma_t dma);

/**
 * @brief   Release a DMA stream
 *
 * @param[in] dma     logical DMA stream
 */
void dma_release(dma_t dma);

/**
 * @brief   Start a DMA transfer on a stream
 *
 * Start a DMA transfer on a given stream. The stream must be configured first
 * by a @p dma_configure call.
 *
 * @param[in] dma     logical DMA stream
 */
void dma_start(dma_t dma);

/**
 * @brief   Suspend a DMA transfer on a stream
 *
 * @param[in] dma     logical DMA stream
 *
 * @return the remaining number of bytes to transfer
 */
uint16_t dma_suspend(dma_t dma);

/**
 * @brief   Resume a suspended DMA transfer on a stream
 *
 * @param[in] dma         logical DMA stream
 * @param[in] reamaining  the remaining number of bytes to transfer
 */
void dma_resume(dma_t dma, uint16_t remaining);

/**
 * @brief   Stop a DMA transfer on a stream
 *
 * @param[in] dma     logical DMA stream
 */
void dma_stop(dma_t dma);

/**
 * @brief   Wait for the end of a transfer
 *
 * @param[in] dma     logical DMA stream
 */
void dma_wait(dma_t dma);

/**
 * @brief   Configure a DMA stream for a new transfer
 *
 * @param[in]  dma     logical DMA stream
 * @param[in]  chan    DMA channel (on stm32f2/4/7, CxS or unused on others)
 * @param[in]  src     source buffer
 * @param[out] dst     destination buffer
 * @param[in]  len     length to transfer
 * @param[in]  mode    DMA mode
 * @param[in]  flags   DMA configuration
 *
 * @return < 0 on error, 0 on success
 */
int dma_configure(dma_t dma, int chan, const volatile void *src, volatile void *dst, size_t len,
                  dma_mode_t mode, uint8_t flags);

/**
 * @brief   Low level initial DMA stream configuration
 *
 * This function is supposed to be used together with @ref dma_prepare. This
 * function sets up the one-time configuration of a stream and @ref dma_prepare
 * configures the per-transfer registers.
 *
 * @param[in]   dma         Logical DMA stream
 * @param[in]   chan        DMA channel (on stm32f2/4/7, CxS or unused on others)
 * @param[in]   periph_addr Peripheral register address
 * @param[in]   mode        DMA direction mode
 * @param[in]   width       DMA transfer width
 * @param[in]   inc_periph  Increment peripheral address after read/write
 */
void dma_setup(dma_t dma, int chan, void *periph_addr, dma_mode_t mode,
               uint8_t width, bool inc_periph);

/**
 * @brief   Low level DMA transfer configuration
 *
 * @param[in]   dma         Logical DMA stream
 * @param[in]   mem         Memory address
 * @param[in]   len         Transfer length
 * @param[in]   inc_mem     Increment the memory address after read/write
 */
void dma_prepare(dma_t dma, void *mem, size_t len, bool incr_mem);

#endif /* MODULE_PERIPH_DMA */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_DMA_H */
/** @} */
