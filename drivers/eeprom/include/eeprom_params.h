/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   drivers_eeprom
 * @{
 *
 * @file
 * @brief     Initialization parameters for the EEPROM drivers
 *
 * @author    Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef EEPROM_PARAMS_H
#define EEPROM_PARAMS_H

#include "board.h"
#include "eeprom_driver.h"

#ifdef MODULE_AT24CXXX
#include "at24cxxx_params.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_PERIPH_EEPROM
extern const eeprom_driver_t eeprom_driver_cpu;
#endif

#ifdef MODULE_AT24CXXX
extern const eeprom_driver_t eeprom_driver_at24cxxx;
#endif

#ifndef INTERNAL_EEPROM_DEVS
#ifdef MODULE_PERIPH_EEPROM
/**
 * @brief   Internal EEPROM device(s)
 */
#define INTERNAL_EEPROM_DEVS        { \
                                        .driver = &eeprom_driver_cpu, \
                                        .params = NULL, \
                                        .handle = NULL, \
                                    },
#endif /* defined(MODULE_PERIPH_EEPROM) */
#endif /* !defined(INTERNAL_EEPROM_DEVS) */

#ifndef AT24CXXX_EEPROM_DEVS
#ifdef MODULE_AT24CXXX
static at24cxxx_t _at24cxxx_handle;
/**
 * @brief   External AT24CXXX EEPROM device(s)
 */
#define AT24CXXX_EEPROM_DEVS        { \
                                        .driver = &eeprom_driver_at24cxxx, \
                                        .params = &at24cxxx_params[0], \
                                        .handle = &_at24cxxx_handle, \
                                    }
#endif /* defined(MODULE_AT24CXXX) */
#endif /* !defined(AT24CXXX_EEPROM_DEVS) */

/**
 * @brief   The EEPROM drivers
 */
static const eeprom_dev_t eeprom_devs[] = {
#ifdef INTERNAL_EEPROM_DEVS
    INTERNAL_EEPROM_DEVS,
#endif
#ifdef AT24CXXX_EEPROM_DEVS
    AT24CXXX_EEPROM_DEVS,
#endif
    /* dummy to get this compiled for now. TODO: Remove this */
    {
        .driver = NULL,
        .params = NULL,
        .handle = NULL,
    }
};

#define EEPROM_NUMOF        ARRAY_SIZE(eeprom_devs)

#ifdef __cplusplus
}
#endif
#endif /* EEPROM_PARAMS_H */
/** @} */
