/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Setup of the LoRa device, in C as the parameters of the
 *              drivers
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "modules.h"
#include "net/netdev.h"

#if IS_USED(MODULE_SX127X)
#include "sx127x.h"
#include "sx127x_params.h"
#elif IS_USED(MODULE_SX126X)
#include "sx126x.h"
#include "sx126x_params.h"
#else
#error "This example requires a LoRa radio driver"
#endif

#include "lora.h"

#if IS_USED(MODULE_SX127X)
static sx127x_t _dev;
#elif IS_USED(MODULE_SX126X)
static sx126x_t _dev;
#endif

netdev_t *lora_setup(void)
{
#if IS_USED(MODULE_SX127X)
    sx127x_setup(&_dev, &sx127x_params[0], 0);
#elif IS_USED(MODULE_SX126X)
    sx126x_setup(&_dev, &sx126x_params[0], 0);
#endif

    return &_dev.netdev;
}
