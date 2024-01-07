/*
 * Copyright (C) 2022 Gunar Schorcht
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

#include "esp_attr.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "hal/interrupt_controller_types.h"
#include "hal/interrupt_controller_ll.h"
#include "rom/ets_sys.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef ETS_CAN_INTR_SOURCE
#define ETS_CAN_INTR_SOURCE     ETS_TWAI_INTR_SOURCE
#endif

typedef struct intr_handle_data_t {
    int     src;   /* peripheral interrupt source */
    uint8_t intr;  /* interrupt number */
    uint8_t level;
} intr_handle_data_t;

/* TODO change to a clearer approach */
static const struct intr_handle_data_t _irq_data_table[] = {
    { ETS_FROM_CPU_INTR0_SOURCE, CPU_INUM_SOFTWARE, 1 },
    { ETS_TG0_WDT_LEVEL_INTR_SOURCE, CPU_INUM_WDT, 1 },
    { ETS_TG0_T0_LEVEL_INTR_SOURCE, CPU_INUM_RTT, 1 },
#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2) || defined(CPU_FAM_ESP32S3)
    { ETS_TG0_T1_LEVEL_INTR_SOURCE, CPU_INUM_TIMER, 2 },
#endif
#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2)
    { ETS_TG0_LACT_LEVEL_INTR_SOURCE, CPU_INUM_TIMER, 2 },
#endif
#if !defined(CPU_FAM_ESP32C2)
    { ETS_TG1_T0_LEVEL_INTR_SOURCE, CPU_INUM_TIMER, 2 },
#endif
#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2) || defined(CPU_FAM_ESP32S3)
    { ETS_TG1_T1_LEVEL_INTR_SOURCE, CPU_INUM_TIMER, 2 },
#endif
    { ETS_UART0_INTR_SOURCE, CPU_INUM_UART, 1 },
    { ETS_UART1_INTR_SOURCE, CPU_INUM_UART, 1 },
#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2) || defined(CPU_FAM_ESP32S3)
    { ETS_UART2_INTR_SOURCE, CPU_INUM_UART, 1 },
#endif
    { ETS_GPIO_INTR_SOURCE, CPU_INUM_GPIO, 1 },
    { ETS_I2C_EXT0_INTR_SOURCE, CPU_INUM_I2C, 1 },
#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2) || defined(CPU_FAM_ESP32S3)
    { ETS_I2C_EXT1_INTR_SOURCE, CPU_INUM_I2C, 1 },
#endif
#if defined(CPU_FAM_ESP32)
    { ETS_ETH_MAC_INTR_SOURCE, CPU_INUM_ETH, 1 },
#endif
#if !defined(CPU_FAM_ESP32C2)
    { ETS_TWAI_INTR_SOURCE, CPU_INUM_CAN, 1 },
    { ETS_TIMER2_INTR_SOURCE, CPU_INUM_FRC2, 2 },
#endif
#if !defined(CPU_FAM_ESP32)
    { ETS_SYSTIMER_TARGET2_EDGE_INTR_SOURCE, CPU_INUM_SYSTIMER, 2 },
#endif
    { ETS_INTERNAL_SW1_INTR_SOURCE, CPU_INUM_BLE, 2 },
#if defined(CPU_FAM_ESP32S2) || defined(CPU_FAM_ESP32S3)
    { ETS_USB_INTR_SOURCE, CPU_INUM_USB, 1 },
#endif
#if defined(ETS_USB_SERIAL_JTAG_INTR_SOURCE)
    { ETS_USB_SERIAL_JTAG_INTR_SOURCE, CPU_INUM_SERIAL_JTAG, 1 },
#endif
    { ETS_RMT_INTR_SOURCE, CPU_INUM_RMT, 1 },
#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2)
    { ETS_I2S0_INTR_SOURCE, CPU_INUM_LCD, 1 },
#elif defined(CPU_FAM_ESP32S3)
    { ETS_LCD_CAM_INTR_SOURCE, CPU_INUM_LCD, 1 },
#endif
#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2) || defined(CPU_FAM_ESP32S3)
    { ETS_SDIO_HOST_INTR_SOURCE, CPU_INUM_SDMMC, 2 },
#endif
};

#define IRQ_DATA_TABLE_SIZE        ARRAY_SIZE(_irq_data_table)

#if defined(CPU_FAM_ESP32) && MODULE_ESP_LCD && MODULE_ESP_ETH
#error "esp_eth and esp_lcd can't be used at the same time because of an interrupt conflict"
#endif

void esp_irq_init(void)
{
#ifdef SOC_CPU_HAS_FLEXIBLE_INTC
    /* to avoid to do it in every component, we initialize levels here once */
    for (unsigned i = 0; i < IRQ_DATA_TABLE_SIZE; i++) {
        intr_cntrl_ll_set_int_level(_irq_data_table[i].intr, _irq_data_table[i].level);
    }
#endif
}

void esp_intr_enable_source(int inum)
{
    intr_cntrl_ll_enable_interrupts(BIT(inum));
}

void esp_intr_disable_source(int inum)
{
    intr_cntrl_ll_disable_interrupts(BIT(inum));
}

esp_err_t esp_intr_enable(intr_handle_t handle)
{
    assert(handle != NULL);
    esp_intr_enable_source(handle->intr);
    return ESP_OK;
}

esp_err_t esp_intr_disable(intr_handle_t handle)
{
    assert(handle != NULL);
    esp_intr_disable_source(handle->intr);
    return ESP_OK;
}

/**
 * @brief Emulates the according ESP-IDF function
 *
 * This function provides a compatible interface to the ESP-IDF function for
 * source code compatibility. In difference to ESP-IDF it uses a fix interrupt
 * assignment scheme based on a very limited number of peripheral interrupt
 * signals.
 *
 * TODO free allocation scheme as in ESP-IDF
 */
esp_err_t esp_intr_alloc(int source, int flags, intr_handler_t handler,
                         void *arg, intr_handle_t *ret_handle)
{
    unsigned i;
    for (i = 0; i < IRQ_DATA_TABLE_SIZE; i++) {
        if (_irq_data_table[i].src == source) {
            break;
        }
    }

    if (i == IRQ_DATA_TABLE_SIZE) {
        return ESP_ERR_NOT_FOUND;
    }

    /* route the interrupt source to the CPU interrupt */
    intr_matrix_set(PRO_CPU_NUM, _irq_data_table[i].src, _irq_data_table[i].intr);

    /* set the interrupt handler */
    intr_cntrl_ll_set_int_handler(_irq_data_table[i].intr, handler, arg);

#ifdef SOC_CPU_HAS_FLEXIBLE_INTC
    /* set interrupt level given by flags */
    intr_cntrl_ll_set_int_level(_irq_data_table[i].intr, esp_intr_flags_to_level(flags));
#endif

    /* enable the interrupt if ESP_INTR_FLAG_INTRDISABLED is not set */
    if ((flags & ESP_INTR_FLAG_INTRDISABLED) == 0) {
        intr_cntrl_ll_enable_interrupts(BIT(_irq_data_table[i].intr));
    }

    if (ret_handle) {
        *((intr_handle_t *)ret_handle) = (const intr_handle_t)&_irq_data_table[i];
    }

    return ESP_OK;
}

esp_err_t esp_intr_alloc_intrstatus(int source, int flags,
                                    uint32_t reg, uint32_t mask,
                                    intr_handler_t handler,
                                    void *arg, intr_handle_t *ret_handle)
{
    /* TODO status register and status mask handling for shared interrupts */
    (void)reg;
    (void)mask;
    return esp_intr_alloc(source, flags, handler, arg, ret_handle);
}

esp_err_t esp_intr_free(intr_handle_t handle)
{
    return esp_intr_disable(handle);
}

int esp_intr_get_cpu(intr_handle_t handle)
{
    return PRO_CPU_NUM;
}
