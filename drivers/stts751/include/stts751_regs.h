/*
 * Copyright (C) 2020 Université Grenoble Alpes
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_stts751
 * @brief       Register definitions for the STTS751 I2C Temperature Sensor
 * @author      Pierre Millot
 * @file        stts751_regs.h
 * @{
 */

#ifndef STTS751_REGS_H
#define STTS751_REGS_H

#ifdef __cplusplus
extern "C"
{
#endif

#define STTS751_REG_TEMP_H      0x00
#define STTS751_REG_STATUS      0x01
#define STTS751_REG_TEMP_L      0x02
#define STTS751_REG_CONF        0x03
#define STTS751_CONF_RES_MASK   0x0C
#define STTS751_CONF_RES_SHIFT  2
#define STTS751_REG_RATE        0x04
#define STTS751_REG_HLIM_H      0x05
#define STTS751_REG_HLIM_L      0x06
#define STTS751_REG_LLIM_H      0x07
#define STTS751_REG_LLIM_L      0x08
#define STTS751_REG_TLIM        0x20
#define STTS751_REG_HYST        0x21
#define STTS751_REG_SMBUS_TO    0x22

#define STTS751_REG_PROD_ID     0xFD
#define STTS751_REG_MAN_ID      0xFE
#define STTS751_REG_REV_ID      0xFF

#define STTS751_0_PROD_ID       0x00
#define STTS751_1_PROD_ID       0x01

#ifdef __cplusplus
}
#endif

#endif /* STTS751_REGS_H */
/** @} */
