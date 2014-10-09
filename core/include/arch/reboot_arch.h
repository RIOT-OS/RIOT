/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file        reboot_arch.h
 * @brief       Architecture dependent interface rebooting
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef __REBOOT_ARCH_H
#define __REBOOT_ARCH_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Reboot the system
 *
 * @param[in] mode  the argument is ignored and only used for conformity
 *                  with existing reboot implementations for now.
 *
 * @return          this call never returns when successful. -1 is returned otherwise.
 */
int reboot_arch(int mode);

#ifdef __cplusplus
}
#endif

#endif /* __REBOOT_ARCH_H */
/** @} */
