/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "irq_arch.h"

#include "esp_err.h"
#include "rom/ets_sys.h"
#include "soc/dport_reg.h"
#include "xtensa/xtensa_api.h"

#define ENABLE_DEBUG 0
#include "debug.h"

struct _irq_alloc_table_t {
    int      src;   /* peripheral interrupt source */
    uint32_t intr;  /* interrupt number */
};

static const struct _irq_alloc_table_t _irq_alloc_table[] = {
    { ETS_FROM_CPU_INTR0_SOURCE, CPU_INUM_SOFTWARE },
    { ETS_TG0_WDT_LEVEL_INTR_SOURCE, CPU_INUM_WDT },
    { ETS_TG0_T0_LEVEL_INTR_SOURCE, CPU_INUM_RTC },
    { ETS_TG0_T1_LEVEL_INTR_SOURCE, CPU_INUM_TIMER },
    { ETS_TG1_T0_LEVEL_INTR_SOURCE, CPU_INUM_TIMER },
    { ETS_TG1_T1_LEVEL_INTR_SOURCE, CPU_INUM_TIMER },
    { ETS_UART0_INTR_SOURCE, CPU_INUM_UART },
    { ETS_UART1_INTR_SOURCE, CPU_INUM_UART },
    { ETS_UART2_INTR_SOURCE, CPU_INUM_UART },
    { ETS_GPIO_INTR_SOURCE, CPU_INUM_GPIO },
    { DPORT_PRO_RTC_CORE_INTR_MAP_REG, CPU_INUM_RTC },
    { ETS_I2C_EXT0_INTR_SOURCE, CPU_INUM_I2C },
    { ETS_I2C_EXT1_INTR_SOURCE, CPU_INUM_I2C },
    { ETS_ETH_MAC_INTR_SOURCE, CPU_INUM_ETH },
    { ETS_CAN_INTR_SOURCE, CPU_INUM_CAN }
};

typedef void (*intr_handler_t)(void *arg);

#define IRQ_ALLOC_TABLE_SIZE ARRAY_SIZE(_irq_alloc_table)
#define ESP_INTR_FLAG_INTRDISABLED    (1<<11)

/**
 * @brief Emulates the according ESP-IDF function
 *
 * This function provides a compatible interface to the ESP-IDF function for
 * source code compatibility. In difference to ESP-IDF it uses a fix interrupt
 * assignment scheme based on a very limited number peripheral interrupt
 * signals.
 *
 * TODO free allocation scheme as in ESP-IDF
 */
esp_err_t esp_intr_alloc(int source, int flags, intr_handler_t handler,
                         void *arg, void *ret_handle)
{
    unsigned i;
    for (i = 0; i < IRQ_ALLOC_TABLE_SIZE; i++) {
        if (_irq_alloc_table[i].src == source) {
            break;
        }
    }

    if (i == IRQ_ALLOC_TABLE_SIZE) {
        return ESP_ERR_NOT_FOUND;
    }

    /* route the interrupt source to the CPU interrupt */
    intr_matrix_set(PRO_CPU_NUM, _irq_alloc_table[i].src, _irq_alloc_table[i].intr);

    /* set the interrupt handler */
    xt_set_interrupt_handler(_irq_alloc_table[i].intr, handler, arg);

    /* enable the interrupt if ESP_INTR_FLAG_INTRDISABLED is not set */
    if ((flags & ESP_INTR_FLAG_INTRDISABLED) == 0) {
        xt_ints_on(BIT(_irq_alloc_table[i].intr));
    }

    return ESP_OK;
}
