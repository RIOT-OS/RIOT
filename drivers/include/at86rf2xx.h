/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_at86rf2xx AT86RF2xx based drivers
 * @ingroup     drivers_netdev
 *
 * This module contains drivers for radio devices in Atmel's AT86RF2xx series.
 * The driver is aimed to work with all devices of this series.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for AT86RF2xx based drivers
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 */

#ifndef AT86RF2XX_H
#define AT86RF2XX_H

#include <stdint.h>

#include "kernel_defines.h"
#include "board.h"
#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/gnrc/nettype.h"

#if IS_USED(MODULE_AT86RF2XX_SPI)
#include "periph/gpio.h"
#include "periph/spi.h"
#endif

#if IS_USED(MODULE_AT86RF212B)
#include "at86rf212b.h"
#endif
#if IS_USED(MODULE_AT86RF231)
#include "at86rf231.h"
#endif
#if IS_USED(MODULE_AT86RF232)
#include "at86rf232.h"
#endif
#if IS_USED(MODULE_AT86RF233)
#include "at86rf233.h"
#endif
#if IS_USED(MODULE_AT86RFA1)
#include "at86rfa1.h"
#endif
#if IS_USED(MODULE_AT86RFR2)
#include "at86rfr2.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum possible packet size in byte
 */
#define AT86RF2XX_MAX_PKT_LENGTH        (IEEE802154_FRAME_LEN_MAX)

/**
 * @name    Flags for device internal states (see datasheet)
 * @{
 */
#define AT86RF2XX_STATE_P_ON           (0x00)     /**< initial power on */
#define AT86RF2XX_STATE_BUSY_RX        (0x01)     /**< busy receiving data (basic mode) */
#define AT86RF2XX_STATE_BUSY_TX        (0x02)     /**< busy transmitting data (basic mode) */
#define AT86RF2XX_STATE_FORCE_TRX_OFF  (0x03)     /**< force transition to idle */
#define AT86RF2XX_STATE_RX_ON          (0x06)     /**< listen mode (basic mode) */
#define AT86RF2XX_STATE_TRX_OFF        (0x08)     /**< idle */
#define AT86RF2XX_STATE_PLL_ON         (0x09)     /**< ready to transmit */
#define AT86RF2XX_STATE_SLEEP          (0x0f)     /**< sleep mode */
#define AT86RF2XX_STATE_BUSY_RX_AACK   (0x11)     /**< busy receiving data (extended mode) */
#define AT86RF2XX_STATE_BUSY_TX_ARET   (0x12)     /**< busy transmitting data (extended mode) */
#define AT86RF2XX_STATE_RX_AACK_ON     (0x16)     /**< wait for incoming data */
#define AT86RF2XX_STATE_TX_ARET_ON     (0x19)     /**< ready for sending data */
#define AT86RF2XX_STATE_IN_PROGRESS    (0x1f)     /**< ongoing state conversion */
#define AT86RF2XX_STATE_RESET          (0xff)     /**< trigger reset */
/** @} */

/**
 * @name    Internal device option flags
 * @{
 */
#define AT86RF2XX_OPT_TELL_TX_START  (0x0001)       /**< notify MAC layer on TX
                                                     *   start */
#define AT86RF2XX_OPT_TELL_TX_END    (0x0002)       /**< notify MAC layer on TX
                                                     *   finished */
#define AT86RF2XX_OPT_TELL_RX_START  (0x0004)       /**< notify MAC layer on RX
                                                     *   start */
#define AT86RF2XX_OPT_TELL_RX_END    (0x0008)       /**< notify MAC layer on RX
                                                     *   finished */
#define AT86RF2XX_OPT_CSMA           (0x0010)       /**< CSMA active */
#define AT86RF2XX_OPT_PROMISCUOUS    (0x0020)       /**< promiscuous mode
                                                     *   active */
#define AT86RF2XX_OPT_PRELOADING     (0x0040)       /**< preloading enabled */
#define AT86RF2XX_OPT_AUTOACK        (0x0080)       /**< Auto ACK active */
#define AT86RF2XX_OPT_ACK_PENDING    (0x0100)       /**< ACK frames with data
                                                     *   pending */
/** @} */

#if IS_USED(MODULE_AT86RF2XX_SPI) || defined(DOXYGEN)
/**
 * @brief   struct holding all params needed for device initialization
 */
typedef struct at86rf2xx_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t sleep_pin;       /**< GPIO pin connected to the sleep pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} at86rf2xx_params_t;
#else
typedef struct at86rf2xx_params {
    char dummy;
} at86rf2xx_params_t;
#endif

/**
 * @brief   Device descriptor for AT86RF2XX radio devices
 *
 * @extends netdev_ieee802154_t
 */
typedef struct at86rf2xx_base {
    netdev_ieee802154_t netdev;             /**< netdev parent struct */
    uint16_t flags;                         /**< Device specific flags */
    uint8_t state;                          /**< current state of the radio */
    uint8_t tx_frame_len;                   /**< length of the current TX frame */
    uint8_t idle_state;                     /**< state to return to after sending */
    uint8_t pending_tx;                     /**< keep track of pending TX calls
                                                 this is required to know when to
                                                 return to idle_state */
    int8_t dev_type;                        /**< AT86RF2XX device type */
} at86rf2xx_base_t;

/**
 * @brief A pointer to an instance of an actual AT86RF2XX
 * must safely be castable to a pointer of this type
 */
typedef struct at86rf2xx {
    at86rf2xx_base_t base;                  /**< AT86RF2xx base */
    at86rf2xx_params_t params;              /**< AT86RF2xx params */
} at86rf2xx_t;

/**
 * @brief AT86RF212B params
 */
struct at86rf212b_params {
    at86rf2xx_params_t base_params;         /**< AT86RF2xx base params */
};

/**
 * @brief AT86RF212B device
 */
struct at86rf212b {
    at86rf2xx_base_t base;                  /**< AT86RF2xx base */
    struct at86rf212b_params params;        /**< AT86RF212B params */
};

/**
 * @brief AT86RF231 params
 */
struct at86rf231_params {
    at86rf2xx_params_t base_params;         /**< AT86RF2xx base params */
};

/**
 * @brief AT86RF231 device
 */
struct at86rf231 {
    at86rf2xx_base_t base;                  /**< AT86RF2xx base */
    struct at86rf231_params params;         /**< AT86RF231 params */
};

/**
 * @brief AT86RF232 params
 */
struct at86rf232_params {
    at86rf2xx_params_t base_params;         /**< AT86RF2xx base params */
};

/**
 * @brief AT86RF232 device
 */
struct at86rf232 {
    at86rf2xx_base_t base;                  /**< AT86RF2xx base */
    struct at86rf232_params params;         /**< At86RF232 params */
    uint8_t tx_retries;                     /**< Number of last frame retransmissions */
};

/**
 * @brief AT86RF233 params
 */
struct at86rf233_params {
    struct at86rf2xx_params base_params;    /**< AT86RF2xx base params */
};

/**
 * @brief AT86RF233 device
 */
struct at86rf233 {
    at86rf2xx_base_t base;                  /**< AT86RF2xx base */
    struct at86rf233_params params;         /**< AT86RF233 params */
    uint8_t tx_retries;                     /**< Number of last frame retransmissions */
};

/**
 * @brief AT86RFA1 device
 */
struct at86rfa1 {
    at86rf2xx_base_t base;                  /**< AT86RF2xx base */
    /*
     *  irq_status = IRQ_STATUS
     */
    uint8_t irq_status;                     /**< save irq status */
};

/**
 * @brief AT86RFR2 device
 */
struct at86rfr2 {
    at86rf2xx_base_t base;                  /**< AT86RF2xx base */
    /*
     *  irq_status = IRQ_STATUS
     */
    uint8_t irq_status;                     /**< save irq status */
};

/**
 * @brief   Dynamic size look up for a AT86RF2XX device
 *
 * @param[in]   dev         device handle
 *
 * @return                  device structure size in bytes
 */
size_t at86rf2xx_get_size(const at86rf2xx_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_H */
/** @} */
