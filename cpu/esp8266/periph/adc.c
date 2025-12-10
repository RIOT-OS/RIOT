/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_adc
 * @{
 *
 * @file
 * @brief       Low-level ADC driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/adc.h"
#include "periph_conf.h"
#include "board.h"

#include "esp_common.h"
#include "sdk/sdk.h"

#define ENABLE_DEBUG 0
#include "debug.h"

extern uint16_t test_tout(void);

int adc_init(adc_t line)
{
    CHECK_PARAM_RET (line < ADC_NUMOF, -1)

    /* no special initialization needed */
    return 0;
}

int32_t adc_sample(adc_t line, adc_res_t res)
{
    CHECK_PARAM_RET (line < ADC_NUMOF, -1)
    CHECK_PARAM_RET (res == ADC_RES_10BIT, -1)

    return test_tout();
}

void adc_print_config(void)
{
    printf("\tADC\t\tpins=[ A0 ]\n");
}
