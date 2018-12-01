/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_extend_adc
 *
 * @{
 *
 * @file
 * @brief       ADC extension redirection functions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#if MODULE_EXTEND_ADC

#include "periph/adc.h"
#include "extend/adc.h"
#include "adc_ext_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

adc_ext_t *adc_ext_entry(adc_t line)
{
    if (line == ADC_UNDEF) {
        return NULL;
    }

    adc_t devnum = adc_ext_dev(line);

    DEBUG("[%s] list entry is %u\n", __func__, devnum);

    /* device is greater than number of listed entries */
    if (devnum >= (sizeof(adc_ext_list) / sizeof(adc_ext_list[0]))) {
        return NULL;
    }

    /* Cast to discard const */
    return (adc_ext_t *)&adc_ext_list[devnum];
}

int adc_init_redir(adc_t line)
{
    adc_ext_t *entry = adc_ext_entry(line);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for %X\n", __func__, line);
        return -1;
    }

    line = adc_ext_line(line);

    return entry->driver->init(entry->dev, line);
}

int adc_sample_redir(adc_t line, adc_res_t res)
{
    adc_ext_t *entry = adc_ext_entry(line);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for %X\n", __func__, line);
        return -1;
    }

    line = adc_ext_line(line);

    return entry->driver->sample(entry->dev, line, res);
}

unsigned int adc_channels_redir(adc_t line)
{
    adc_ext_t *entry = adc_ext_entry(line);

    if (entry == NULL) {
        DEBUG("[%s] ext entry doesn't exist for %X\n", __func__, line);
        return 0;
    }

    return entry->driver->channels(entry->dev);
}

#endif /* MODULE_EXTEND_ADC */
