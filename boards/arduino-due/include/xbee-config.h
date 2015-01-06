/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_xbee XBee driver
 * @ingroup     drivers
 * @brief       XBee autonomous 802.15.4 modem driver
 * @{
 *
 * @file
 * @brief       Configuration values for the XBee 802.15.4 modem driver
 *
 * @author      Kévin Roussel <kevin.roussel@inria.fr>
 */

#ifndef __XBEE_CONFIG_H
#define __XBEE_CONFIG_H

#include "periph/uart.h"

/* identifier of the UART used to communicate with the XBee module;
   on the Arduino Due, this is UART 0 (the first one, also used for MCU
   programming) */
#define XBEE_UART_LINK   UART_0

/* identifier of the GPIO pin linked to the SLEEP_RQ pin (pin 9) of the XBee
   module (output that allows to put the modem online or offline) */
#define XBEE_SLEEP_RQ_GPIO    GPIO_UNDEFINED

/* identifier of the GPIO pin linked to the ON/SLEEP pin (pin 13) of the XBee
   module  (input that gives the online or sleeping status of the module) */
#define XBEE_ON_STATUS_GPIO   GPIO_UNDEFINED


#endif /* __XBEE_CONFIG_H */
