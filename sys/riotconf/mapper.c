/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotconf_mapper
 * @{
 *
 * @file
 * @brief       RIOT configuration mapper
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "board.h"
#include "container.h"
#include "configuration_backend_riotconf.h"
#include "mtd.h"
#include "mtd_default.h"
#include "mtd_mapper.h"
#include "riotconf/storage.h"
#include "riotconf/slot.h"

#ifndef CONFIG_RIOTCONF_MAPPER_SEC_OFFSET
/**
 * @brief   Reserve a number of sectors for any other application usage
 *
 * This is an even number so that the remaining sectors can be divided evenly
 */
#define CONFIG_RIOTCONF_MAPPER_SEC_OFFSET                   0
#endif

#ifndef CONFIG_RIOTCONF_MAPPER_MTD
/**
 * @brief   MTD device to use for the mapper
 */
#define CONFIG_RIOTCONF_MAPPER_MTD                          MTD_0
#endif

#ifndef CONFIG_RIOTCONF_MAPPER_MTD_SECTOR_SIZE
/**
 * @brief   Sector size to use for the mapper, must be a divisor of the MTD sector size
 */
#define CONFIG_RIOTCONF_MAPPER_MTD_SECTOR_SIZE              512
#endif

static mtd_mapper_parent_t _mtd_mapper_parent_config;
static mtd_mapper_region_t _mtd_mapper_regions_config[RIOTCONF_SLOT_NUMOF];
static mtd_mapper_region_t _mtd_mapper_region_data;
static uint8_t _mtd_config_sec_buf[RIOTCONF_SLOT_NUMOF][CONFIG_RIOTCONF_MAPPER_MTD_SECTOR_SIZE] RIOTCONF_BUF_ALIGN;

void riotconf_mapper_init(void)
{
    mtd_init(CONFIG_RIOTCONF_MAPPER_MTD);
    _mtd_mapper_parent_config = (mtd_mapper_parent_t)MTD_PARENT_INIT(CONFIG_RIOTCONF_MAPPER_MTD);

    /* initialize sectors for common data storage */
    if (CONFIG_RIOTCONF_MAPPER_SEC_OFFSET) {
        _mtd_mapper_region_data.mtd.driver = &mtd_mapper_driver;
        _mtd_mapper_region_data.parent = &_mtd_mapper_parent_config;
        _mtd_mapper_region_data.sector = 0;
        _mtd_mapper_region_data.mtd.sector_count = CONFIG_RIOTCONF_MAPPER_SEC_OFFSET;
        mtd_init(&_mtd_mapper_region_data.mtd);
    }

    for (unsigned i = 0; i < ARRAY_SIZE(_mtd_mapper_regions_config); i++) {
        _mtd_mapper_regions_config[i].mtd.driver = &mtd_mapper_driver;

        unsigned sectors = (_mtd_mapper_parent_config.mtd->sector_count - CONFIG_RIOTCONF_MAPPER_SEC_OFFSET) /
                            ARRAY_SIZE(_mtd_mapper_regions_config);
        assert(sectors);
        _mtd_mapper_regions_config[i].parent = &_mtd_mapper_parent_config;
        _mtd_mapper_regions_config[i].sector = i * sectors + CONFIG_RIOTCONF_MAPPER_SEC_OFFSET;
        _mtd_mapper_regions_config[i].mtd.sector_count = sectors;
        mtd_init(&_mtd_mapper_regions_config[i].mtd);
        riotconf_storage_init(i, &_mtd_mapper_regions_config[i].mtd, 0,
                              _mtd_config_sec_buf[i], sizeof(_mtd_config_sec_buf[i]));
    }
}

void configuration_backend_riotconf_storage_init(void)
{
    riotconf_mapper_init();
}
