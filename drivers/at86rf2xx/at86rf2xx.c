/*
 * Copyright (C) 2013 Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * Copyright (C) 2015 Freie Universität Berlin
 *               2017 HAW Hamburg
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Implementation of public functions for AT86RF2xx drivers
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include "at86rf2xx.h"
#include "at86rf2xx_dev_types.h"
#include "at86rf2xx_registers.h"
#include "at86rf2xx_communication.h"
#include "at86rf2xx_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

size_t at86rf2xx_get_size(const at86rf2xx_t *dev)
{
    switch (dev->base.dev_type) {
        case AT86RF2XX_DEV_TYPE_AT86RF212B: {
            return sizeof(struct at86rf212b);
        }
        case AT86RF2XX_DEV_TYPE_AT86RF231: {
            return sizeof(struct at86rf231);
        }
        case AT86RF2XX_DEV_TYPE_AT86RF232: {
            return sizeof(struct at86rf232);
        }
        case AT86RF2XX_DEV_TYPE_AT86RF233: {
            return sizeof(struct at86rf233);
        }
        case AT86RF2XX_DEV_TYPE_AT86RFA1: {
            return sizeof(struct at86rfa1);
        }
        case AT86RF2XX_DEV_TYPE_AT86RFR2: {
            return sizeof(struct at86rfr2);
        }
        default:
            return sizeof(at86rf2xx_t);
    }
}
