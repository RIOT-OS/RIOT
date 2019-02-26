/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_extend_dac
 *
 * @{
 *
 * @file
 * @brief       DAC extension redirection functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#if MODULE_EXTEND_DAC

#include "periph/dac.h"
#include "extend/dac.h"
#include "dac_ext_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

dac_ext_t *dac_ext_entry(dac_t line)
{
    if (line == DAC_UNDEF) {
        return NULL;
    }

    dac_t devnum = dac_ext_dev(line);

    DEBUG("[%s] list entry is %u\n", __func__, devnum);

    /* device is greater than number of listed entries */
    if (devnum >= (sizeof(dac_ext_list) / sizeof(dac_ext_list[0]))) {
        return NULL;
    }

    /* Cast to discard const */
    return (dac_ext_t *)&dac_ext_list[devnum];
}

int8_t dac_init_redir(dac_t line)
{
    dac_ext_t *entry = dac_ext_entry(line);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for %X\n", __func__, line);
        return -1;
    }

    line = dac_ext_line(line);

    return entry->driver->init(entry->dev, line);
}

void dac_set_redir(dac_t line, uint16_t value)
{
    dac_ext_t *entry = dac_ext_entry(line);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for %X\n", __func__, line);
        return;
    }

    line = dac_ext_line(line);

    entry->driver->set(entry->dev, line, value);
}

void dac_poweron_redir(dac_t line)
{
    dac_ext_t *entry = dac_ext_entry(line);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for %X\n", __func__, line);
        return;
    }

    line = dac_ext_line(line);

    entry->driver->poweron(entry->dev, line);
}

void dac_poweroff_redir(dac_t line)
{
    dac_ext_t *entry = dac_ext_entry(line);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for %X\n", __func__, line);
        return;
    }

    line = dac_ext_line(line);

    entry->driver->poweroff(entry->dev, line);
}

unsigned int dac_channels_redir(dac_t line)
{
    dac_ext_t *entry = dac_ext_entry(line);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for %X\n", __func__, line);
        return 0;
    }

    return entry->driver->channels(entry->dev);
}

#endif /* MODULE_EXTEND_DAC */
