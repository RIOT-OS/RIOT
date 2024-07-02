/*
 * Copyright (C) 2023 Stefan Schmidt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_w5500 W5500 ethernet driver
 * @ingroup     drivers_netdev
 * @brief       Driver for W5500 ethernet devices
 *
 * This device driver only exposes the MACRAW mode of W5500 devices, so it does
 * not offer any support for the on-chip IPv4, UDP, and TCP capabilities of
 * these chips. In connection with RIOT we are only interested in the RAW
 * Ethernet packets, which we can use through netdev with any software network
 * stack provided by RIOT (e.g. GNRC). This enables W5500 devices to communicate
 * via IPv6, enables unlimited connections, and more...
 *
 * @note        This driver can be used in polling or interrupt mode.
 *              On some shields the interrupt line is not enabled by default,
 *              you have to close the corresponding solder bridge to make the
 *              interrupt mode work...
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the W5500 device driver
 *
 * @author      Stefan Schmidt <stemschmidt@gmail.com>
 */

#ifndef W5500_H
#define W5500_H

#include <stdint.h>

#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/netdev.h"
#include "kernel_defines.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   W5500 device descriptor
 */
typedef struct {
    spi_t spi;                    /**< SPI bus used */
    spi_clk_t clk;                /**< clock speed used on the selected SPI bus */
    gpio_t cs;                    /**< pin connected to the chip select line */
    gpio_t irq;                   /**< pin connected to the INT line */
    uint32_t polling_interval_ms; /**< interval for polling, 0 if interrupt mode */
} w5500_params_t;

/**
 * @brief   Device descriptor for W5500 devices
 */
typedef struct w5500 {
    netdev_t netdev;           /**< extends the netdev structure */
    w5500_params_t p;          /**< device configuration parameters */
    uint16_t frame_size;       /**< size of the frame which has been send */
    bool link_up;              /**< used to prevent sending the same LINK event twice */
    bool frame_sent;           /**< indicates that the frame has been transmitted */
    ztimer_t timerInstance;    /**< stores the polling interval timer in polling mode */
} w5500_t;

/**
 * @brief   So the initial device setup
 *
 * This function pre-initializes the netdev structure, saves the configuration
 * parameters and finally initializes the SPI bus and the used GPIO pins.
 *
 * @param [out] dev     the handle of the device to initialize
 * @param [in]  params  parameters for device initialization
 * @param [in]  index   Index of @p params in a global parameter struct array.
 *                      If initialized manually, pass a unique identifier instead.
 */
void w5500_setup(w5500_t *dev, const w5500_params_t *params, uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* W5500_H */
/** @} */
