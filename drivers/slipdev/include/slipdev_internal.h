/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_slipdev
 * @{
 *
 * @file
 * @internal
 * @brief   Internal SLIP device definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef SLIPDEV_INTERNAL_H
#define SLIPDEV_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    SLIP marker bytes
 * @{
 */
#define SLIPDEV_END         (0xc0U)
#define SLIPDEV_ESC         (0xdbU)
#define SLIPDEV_END_ESC     (0xdcU)
#define SLIPDEV_ESC_ESC     (0xddU)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SLIPDEV_INTERNAL_H */
/** @} */
