/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     candev_stm32
 * @{
 *
 * @file
 * @brief       STM32 CAN controller driver (bxCAN) default parameters
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifndef CAN_PARAMS_H
#define CAN_PARAMS_H

#include "can/device.h"
#include "periph/can.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Default STM32 CAN devices config */
static const can_conf_t candev_conf[] = {
    {
#if defined(CPU_FAM_STM32F0)
        .can = CAN,
        .rcc_mask = RCC_APB1ENR_CANEN,
        .rx_pin = GPIO_PIN(PORT_A, 11),
        .tx_pin = GPIO_PIN(PORT_A, 12),
        .af = GPIO_AF4,
        .irqn = CEC_CAN_IRQn,
#else
        .can = CAN1,
#if defined(CPU_FAM_STM32L4)
        .rcc_mask = RCC_APB1ENR1_CAN1EN,
#else
        .rcc_mask = RCC_APB1ENR_CAN1EN,
        .can_master = CAN1,
        .master_rcc_mask = RCC_APB1ENR_CAN1EN,
        .first_filter = 0,
        .nb_filters = 14,
#endif
#if  defined(CPU_FAM_STM32F1)
        .rx_pin = GPIO_PIN(PORT_A, 11),
        .tx_pin = GPIO_PIN(PORT_A, 12),
#elif defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32F4)
        .rx_pin = GPIO_PIN(PORT_B, 8),
        .tx_pin = GPIO_PIN(PORT_B, 9),
        .af = GPIO_AF9,
#else
        .rx_pin = GPIO_PIN(PORT_D, 0),
        .tx_pin = GPIO_PIN(PORT_D, 1),
        .af = GPIO_AF9,
#endif
        .tx_irqn = CAN1_TX_IRQn,
        .rx0_irqn = CAN1_RX0_IRQn,
        .rx1_irqn = CAN1_RX1_IRQn,
        .sce_irqn = CAN1_SCE_IRQn,
#endif
        .en_deep_sleep_wake_up = true,
        .ttcm = 0,
        .abom = 1,
        .awum = 1,
        .nart = 0,
        .rflm = 0,
        .txfp = 0,
    },
#if (CANDEV_STM32_CHAN_NUMOF >= 2) && (CAN_DLL_NUMOF >= 2)
    {
        .can = CAN2,
        .rcc_mask = RCC_APB1ENR_CAN2EN,
        .can_master = CAN1,
        .master_rcc_mask = RCC_APB1ENR_CAN1EN,
        .first_filter = 14,
        .nb_filters = 14,
        .rx_pin = GPIO_PIN(PORT_B, 5),
        .tx_pin = GPIO_PIN(PORT_B, 6),
#ifndef CPU_FAM_STM32F1
        .af = GPIO_AF9,
#endif
        .en_deep_sleep_wake_up = true,
        .tx_irqn = CAN2_TX_IRQn,
        .rx0_irqn = CAN2_RX0_IRQn,
        .rx1_irqn = CAN2_RX1_IRQn,
        .sce_irqn = CAN2_SCE_IRQn,
        .ttcm = 0,
        .abom = 1,
        .awum = 1,
        .nart = 0,
        .rflm = 0,
        .txfp = 0,
    },
#endif
#if (CANDEV_STM32_CHAN_NUMOF >= 3) && (CAN_DLL_NUMOF >= 3)
    {
        .can = CAN3,
        .rcc_mask = RCC_APB1ENR_CAN3EN,
        .can_master = CAN3,
        .master_rcc_mask = RCC_APB1ENR_CAN3EN,
        .first_filter = 0,
        .nb_filters = 14,
        .rx_pin = GPIO_PIN(PORT_B, 3),
        .tx_pin = GPIO_PIN(PORT_B, 4),
        .af = GPIO_AF11,
        .en_deep_sleep_wake_up = true,
        .tx_irqn = CAN3_TX_IRQn,
        .rx0_irqn = CAN3_RX0_IRQn,
        .rx1_irqn = CAN3_RX1_IRQn,
        .sce_irqn = CAN3_SCE_IRQn,
        .ttcm = 0,
        .abom = 1,
        .awum = 1,
        .nart = 0,
        .rflm = 0,
        .txfp = 0,
    },
#endif
};

/** Default STM32 CAN devices parameters */
static const candev_params_t candev_params[] = {
    {
        .name = "can_stm32_0",
    },
#if (CANDEV_STM32_CHAN_NUMOF >= 2) && (CAN_DLL_NUMOF >= 2)
    {
        .name = "can_stm32_1",
    },
#endif
#if (CANDEV_STM32_CHAN_NUMOF >= 3) && (CAN_DLL_NUMOF >= 3)
    {
        .name = "can_stm32_2",
    },
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* CAN_PARAMS_H */
