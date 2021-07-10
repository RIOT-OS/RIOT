/*
 * Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */
/**
 * @ingroup     sys_auto_init_storage
 * @{
 *
 * @file
 * @brief       Auto initialization of at24cxxx devices
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#include <assert.h>
#include "log.h"
#include "kernel_defines.h"
#include "at24cxxx.h"
#include "at24cxxx_params.h"
#include "mtd_at24cxxx.h"
#include "mtd_at24cxxx_params.h"

#define AT24CXXX_NUMOF  ARRAY_SIZE(at24cxxx_params)

static at24cxxx_t _at24cxxx_devs[AT24CXXX_NUMOF];

#if MTD_AT24CXXX_NUMOF > 0
static mtd_at24cxxx_t _mtd_at24cxxx_devs[MTD_AT24CXXX_NUMOF];
#else
#define _mtd_at24cxxx_devs          ((mtd_at24cxxx_t *)NULL)
#endif

at24cxxx_t *at24cxxx_devs(void) {
    return _at24cxxx_devs;
}
at24cxxx_id_t at24cxxx_numof(void) {
    return AT24CXXX_NUMOF;
}

#if IS_USED(MODULE_MTD_AT24CXXX) || defined(DOXYGEN)
mtd_at24cxxx_t *mtd_at24cxxx_devs(void) {
    return _mtd_at24cxxx_devs;
}
at24cxxx_id_t mtd_at24cxxx_numof(void) {
    return MTD_AT24CXXX_NUMOF;
}

static void _auto_init_mtd_at24cxxx(mtd_at24cxxx_t *mtd,
                                    at24cxxx_t *devs,
                                    const at24cxxx_params_t *params,
                                    at24cxxx_id_t numof)
{
    for (at24cxxx_id_t i = 0; i < numof; i++) {
        mtd[i].base.driver = &mtd_at24cxxx_driver;
        mtd[i].at24cxxx_eeprom = &devs[i];
        mtd[i].params = &params[i];
    }
}

/**
 * @internal    Additional setup at24cxxx MTD,
 *              e.g. to be implemented by the board
 *
 * @note This function has __attribute__((weak)) so it can be overridden
 *
 * @param[in] at24cxxx      Pointer to at24cxxx device
 * @param[in] numof         Number of at24cxxx devices
 */
void __attribute__((weak)) auto_init_mtd_at24cxxx(mtd_at24cxxx_t *at24cxxx,
                                                  at24cxxx_id_t numof)
{
    (void)at24cxxx;
    (void)numof;
}
#else
#define _auto_init_mtd_at24cxxx(...)
#define auto_init_mtd_at24cxxx(...)
#endif

static void _auto_init_at24cxxx(at24cxxx_t *devs,
                                const at24cxxx_params_t *params,
                                at24cxxx_id_t numof)
{
    for (at24cxxx_id_t i = 0; i < numof; i++) {
        int init;
        if (AT24CXXX_OK != (init = at24cxxx_init(&devs[i], &params[i]))) {
            LOG_DEBUG("[auto_init_at24cxxx]: "
                      "failed to initialize device %d due to reason %d",
                      devs - _at24cxxx_devs, init);
        }
    }
}

void auto_init_at24cxxx(void)
{
    assert(AT24CXXX_ID_MAX >= AT24CXXX_NUMOF);
    assert(MTD_AT24CXXX_NUMOF <= AT24CXXX_NUMOF);

    const at24cxxx_params_t *params = at24cxxx_params;
    at24cxxx_t *devs = _at24cxxx_devs;

    _auto_init_mtd_at24cxxx(_mtd_at24cxxx_devs, devs, params, MTD_AT24CXXX_NUMOF);
    _auto_init_at24cxxx(devs + MTD_AT24CXXX_NUMOF,
                        params + MTD_AT24CXXX_NUMOF,
                        AT24CXXX_NUMOF - MTD_AT24CXXX_NUMOF);

    auto_init_mtd_at24cxxx(_mtd_at24cxxx_devs, MTD_AT24CXXX_NUMOF);
}
