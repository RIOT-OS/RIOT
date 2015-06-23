/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file
 * @brief       Architecture dependent interface rebooting
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef REBOOT_ARCH_H
#define REBOOT_ARCH_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Reboot the system, must never return
 */
void reboot_arch(void);

#ifdef __cplusplus
}
#endif

#endif /* REBOOT_ARCH_H */
/** @} */
