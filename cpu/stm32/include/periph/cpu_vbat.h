/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           CPU internal VBAT interface and definitions of the STM32 family
 *
 * @author          Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef PERIPH_CPU_VBAT_H
#define PERIPH_CPU_VBAT_H

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

#endif /* PERIPH_CPU_VBAT_H */
/** @} */
