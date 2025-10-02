/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           CPU internal VBAT interface and definitions of the STM32 family
 *
 * @author          Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable the VBAT sampling channel
 */
void vbat_enable(void);

/**
 * @brief   Disable the VBAT sampling channel
 */
void vbat_disable(void);

#ifdef __cplusplus
}
#endif

/** @} */
