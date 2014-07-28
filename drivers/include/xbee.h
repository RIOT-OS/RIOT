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
 * @brief       High-level driver for the XBee 802.15.4 modem
 * @{
 *
 * @file
 * @brief       High-level driver for the XBee 802.15.4 modem
 *
 * @author      Kévin Roussel <kevin.roussel@inria.fr>
 */

#ifndef __XBEE_H
#define __XBEE_H

#include "radio_driver.h"

#define XBEE_MAX_PKT_DATA_LENGTH  100


/**
 * XBee low-level radio driver definition.
 */
extern const ieee802154_radio_driver_t xbee_radio_driver;

#endif /* __XBEE_H */
/** @} */
