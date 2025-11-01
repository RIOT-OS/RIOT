/*
 * SPDX-FileCopyrightText: 2022 Dario Petrillo
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_ir_nec IR NEC Remote receiver
 * @ingroup     drivers_misc
 * @brief       Receives commands from NEC IR remotes
 *
 * @{
 *
 * @file
 *
 * @author      Dario Petrillo <dario.pk1@gmail.com>
 */

#include <stdint.h>

#include "isrpipe.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   NEC command structure
 */
typedef struct {
    uint8_t addr;   /**< Address (usually 0, depends on the remote) */
    uint8_t cmd;    /**< Command */
} ir_nec_cmd_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    gpio_t pin;     /**< Input pin */
} ir_nec_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    gpio_t pin;             /**< Input pin */
    isrpipe_t isrpipe;      /**< Internal pipe for commands */

    uint32_t last_rising;   /**< Timestamp of the last rising edge */
    uint32_t data;          /**< Data packet being received */
    uint8_t read_bits;      /**< Number of bits read so far. 0xff if not currently reading */

    uint8_t isrpipe_buf[2 * sizeof(ir_nec_cmd_t)];  /**< Buffer for the isrpipe */
} ir_nec_t;

/**
 * @brief   Initialize the given device
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[in]     params    Initialization parameters
 *
 * @return                  0 on success
 */
int ir_nec_init(ir_nec_t *dev, const ir_nec_params_t *params);

/**
 * @brief   Read a command packet. Blocks until one is received
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[out]    command   Structure to write the result into
 *
 * @retval       0          Success
 * @retval       -1         Isrpipe returned too few bytes. Should never happen
 */
int ir_nec_read(ir_nec_t *dev, ir_nec_cmd_t *command);

#ifdef __cplusplus
}
#endif

/** @} */
