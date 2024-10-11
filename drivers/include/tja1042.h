/*
 * Copyright (C) 2016-2018  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_tja1042 TJA1042
 * @ingroup     drivers_can
 * @ingroup     drivers_can_trx
 * @brief       tja1042 High Speed CAN transceiver driver
 *
 * @{
 *
 * @file
 * @brief       tja1042 generic CAN transceiver interface initialization
 *
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */
#ifndef TJA1042_H
#define TJA1042_H

#include <stdio.h>

#include "periph/gpio.h"
#include "can/can_trx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   tja1042 CAN trx descriptor
 */
typedef struct tja1042_trx {
    /**
     *  set mode interface
     */
    can_trx_t trx;

    /**
     *  Mode pin of tja1042 device
     */
    gpio_t stb_pin;

} tja1042_trx_t;

/**
 * @brief   Set mode interface
 *
 * @param[in] dev      Pointer to the tja1042 descriptor
 * @param[in] mode     mode to set
 *
 * @return  0 on success
 * @return -1 on error
 */
int tja1042_trx_set_mode(can_trx_t *dev, can_trx_mode_t mode);

/**
 * @brief   Initialize the given tja1042
 *
 * @param[in] dev      Pointer to the tja1042 descriptor
 *
 * @return  0 on success
 * @return -1 on error
 */
int tja1042_trx_init(can_trx_t *dev);

/**
 * @brief   tja1042 driver
 */
extern const trx_driver_t tja1042_driver;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TJA1042_H */
