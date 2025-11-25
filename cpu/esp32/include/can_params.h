/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_can
 * @{
 *
 * @file
 * @brief       ESP32 CAN controller driver (esp_can) default parameters
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef CAN_PARAMS_H
#define CAN_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "can_esp.h"
#include "can/device.h"
#include "periph/can.h"

#ifndef CAN_TX
/** Default CAN tranveiver TX pin if not defined in board configuration */
#define CAN_TX  GPIO5
#endif

#ifndef CAN_RX
/** Default CAN transceiver RX pin if not defined in board configuration */
#define CAN_RX  GPIO35
#endif

#ifndef CAN_BITRATE
/** Default CAN Bitrate */
#define CAN_BITRATE (500000)
#endif

/** Default ESP CAN devices parameters */
static const candev_params_t candev_params[] = {
    {
        .name = "esp-can",
    },
};

/** Default ESP CAN devices config */
static const can_conf_t candev_conf[] = {
    {
        .bitrate = CAN_BITRATE,
        .tx_pin = CAN_TX,
        .rx_pin = CAN_RX,
#ifdef CAN_CLK_OUT
        .clk_out_pin = CAN_CLK_OUT,
#endif
#ifdef CAN_BUS_ON_OFF
        .bus_on_off_pin = CAN_BUS_ON_OFF,
#endif
    },
};

#ifdef __cplusplus
}
#endif

#endif /* CAN_PARAMS_H */
/** @} */
