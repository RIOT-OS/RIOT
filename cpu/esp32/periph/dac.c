/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level DAC driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "board.h"
#include "periph/dac.h"

#include "esp_common.h"
#include "driver/dac_common.h"
#include "soc/dac_periph.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* forward declarations of internal functions */
static bool _dac_conf_check(void);
static bool _dac_module_initialized  = false;

/* RIOT DAC line to ESP-IDF channel map, filled during initialization */
dac_channel_t _dac_channels[DAC_NUMOF] = { DAC_CHANNEL_MAX };

int8_t dac_init(dac_t line)
{
    if (!_dac_module_initialized) {
        /* do some configuration checks and fill _dac_channel */
        if (!_dac_conf_check()) {
            return DAC_NOLINE;
        }
        _dac_module_initialized = true;
    }

    if ((line >= DAC_NUMOF) || (_dac_channels[line] == DAC_CHANNEL_MAX)) {
        LOG_TAG_ERROR("dac", "Line %u is an invalid DAC line\n", line);
        return DAC_NOLINE;
    }

    dac_poweron(line);
    dac_set(line, 0);

    return DAC_OK;
}

void dac_set(dac_t line, uint16_t value)
{
    assert(line < DAC_NUMOF);
    assert(_dac_channels[line] != DAC_CHANNEL_MAX);
    dac_output_voltage(_dac_channels[line], value >> (16 - SOC_DAC_RESOLUTION));
}

void dac_poweroff(dac_t line)
{
    assert(line < DAC_NUMOF);
    assert(_dac_channels[line] != DAC_CHANNEL_MAX);
    dac_output_disable(_dac_channels[line]);
}

void dac_poweron(dac_t line)
{
    assert(line < DAC_NUMOF);
    assert(_dac_channels[line] != DAC_CHANNEL_MAX);
    dac_output_enable(_dac_channels[line]);
}

static bool _dac_conf_check(void)
{
    for (unsigned i = 0; i < DAC_NUMOF; i++) {
        for (unsigned j = 0; i < SOC_DAC_PERIPH_NUM; j++) {
            if (dac_channels[i] == dac_periph_signal.dac_channel_io_num[j]) {
                _dac_channels[i] = j;
                break;
            }
        }
        if (_dac_channels[i] == DAC_CHANNEL_MAX) {
            LOG_TAG_ERROR("dac", "GPIO%d cannot be used as DAC line\n",
                          dac_channels[i]);
            return false;
        }
    }

    return true;
}

void dac_print_config(void)
{
    printf("\tDAC\t\tpins=[ ");
#if defined(DAC_GPIOS)
    for (unsigned i = 0; i < DAC_NUMOF; i++) {
        printf("%d ", dac_channels[i]);
    }
#endif /* defined(DAC_GPIOS) */
    printf("]\n");
}
