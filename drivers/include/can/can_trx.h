/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_can_trx CAN transceiver interface
 * @ingroup     drivers_can
 * @brief       CAN generic transceiver interface
 * @{
 *
 * @file
 * @brief       CAN generic transceiver interface
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * trx transceiver mode
 */
typedef enum {
    TRX_NORMAL_MODE = 0,
    TRX_SILENT_MODE,
    TRX_SLEEP_MODE,
    /* single wire can modes */
    TRX_HIGH_SPEED_MODE,
    TRX_HIGH_VOLTAGE_WAKE_UP_MODE
} can_trx_mode_t;

/**
 * @brief forward declaration of trx_driver
 */
typedef struct trx_driver trx_driver_t;

/**
 * @brief Generic transceiver descriptor
 */
typedef struct can_trx {
    const trx_driver_t *driver; /**< driver */
    can_trx_mode_t mode;            /**< current mode */
} can_trx_t;

/**
 * @brief Generic transceiver driver
 */
struct trx_driver {
    /**
     * @brief initialize the trx device
     *
     * @param[in] dev     Transceiver to initialize
     *
     * @return 0 on success
     * @return < 0 on error
     */
    int (*init)(can_trx_t *dev);

    /**
     * @brief set mode interface
     *
     * @param[in] dev      Transceiver to set
     * @param[in] mode     Mode to set
     *
     * @return  0 on success
     * @return  < 0 on error
     */
    int (*set_mode)(can_trx_t *dev, can_trx_mode_t mode);
};

/**
 * @brief initialize a transceiver
 *
 * @param[in] dev      Transceiver to initialize
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int can_trx_init(can_trx_t *dev);

/**
 * @brief transceiver set mode
 *
 * @param[in] dev      Transceiver to set
 * @param[in] mode     Mode to set
 *
 * @return  0 on success
 * @return  < 0 on error
 */
int can_trx_set_mode(can_trx_t *dev, can_trx_mode_t mode);

#ifdef __cplusplus
}
#endif

/** @} */
