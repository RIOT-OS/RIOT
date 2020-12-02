/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_IOCON_H_
#define _FSL_IOCON_H_

#include "fsl_common.h"

/*!
 * @addtogroup qn_iocon
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define IOCON SYSCON

/*! @name Driver version */
/*@{*/
/*! @brief IOCON driver version 2.0.0. */
#define LPC_IOCON_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))
/*@}*/

/**
 * @brief Array of IOCON pin definitions passed to IOCON_SetPinMuxing() must be in this format
 */
typedef struct _iocon_group
{
    uint32_t port : 8;      /* Pin port */
    uint32_t pin : 8;       /* Pin number */
    uint32_t modefunc : 16; /* Function and mode */
} iocon_group_t;

/**
 * @brief IOCON function, mode and drive selection definitions
 * @note See the User Manual for specific drive levels, modes and functions supported by the various pins.
 */
#define IOCON_FUNC0 0x0U /*!< Selects pin function 0 */
#define IOCON_FUNC1 0x1U /*!< Selects pin function 1 */
#define IOCON_FUNC2 0x2U /*!< Selects pin function 2 */
#define IOCON_FUNC3 0x3U /*!< Selects pin function 3 */
#define IOCON_FUNC4 0x4U /*!< Selects pin function 4 */
#define IOCON_FUNC5 0x5U /*!< Selects pin function 5 */
#define IOCON_FUNC6 0x6U /*!< Selects pin function 6 */
#define IOCON_FUNC7 0x7U /*!< Selects pin function 7 */

#define IOCON_MODE_HIGHZ (0x0U << 4U)    /*!< Selects High-Z function */
#define IOCON_MODE_PULLDOWN (0x1U << 4U) /*!< Selects pull-down function */
#define IOCON_MODE_PULLUP (0x2U << 4U)   /*!< Selects pull-up function */

#define IOCON_DRIVE_LOW (0x0U << 6U)   /*!< Enable low drive strength */
#define IOCON_DRIVE_HIGH (0x1U << 6U)  /*!< Enable high drive strength */
#define IOCON_DRIVE_EXTRA (0x1U << 7U) /*!< Enable extra drive, only valid for PA06/PA11/PA19/PA26/PA27 */

/**
 * @brief Pull mode
 */
typedef enum _iocon_pull_mode
{
    kIOCON_HighZ = 0U, /*!< High Z */
    kIOCON_PullDown,   /*!< Pull down */
    kIOCON_PullUp      /*!< Pull up */
} iocon_pull_mode_t;

/**
 * @brief Drive strength
 */
typedef enum _iocon_drive_strength
{
    kIOCON_LowDriveStrength = 0U,      /*!< Low-drive */
    kIOCON_HighDriveStrength,          /*!< High-drive */
    kIOCON_LowDriveWithExtraStrength,  /*!< Low-drive with extra */
    kIOCON_HighDriveWithExtraStrength, /*!< High-drive with extra */
} iocon_drive_strength_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sets I/O control pin mux
 * @param   base            The base of SYSCON peripheral on the chip
 * @param   port            GPIO port to mux (value from 0 ~ 1)
 * @param   pin             GPIO pin to mux (value from 0 ~ 31)
 * @param   modeFunc        OR'ed values of type IOCON_*
 * @return  Nothing
 */
__STATIC_INLINE void IOCON_PinMuxSet(SYSCON_Type *base, uint8_t port, uint8_t pin, uint32_t modeFunc)
{
    assert(((port == 0U) && (pin <= 31U)) || ((port == 1U) && (pin <= 2U)));

    uint8_t pinMuxIndex = (pin >> 3U);
    uint8_t pinMuxLocation = ((pin & 0x7U) << 2U);
    uint8_t pinPullIndex = (pin >> 4U);
    uint8_t pinPullLocation = ((pin & 0xFU) << 1U);

    if (port == 0U)
    {
        base->PIO_FUNC_CFG[pinMuxIndex] &= ~(0x07U << pinMuxLocation);
        base->PIO_FUNC_CFG[pinMuxIndex] |= (modeFunc & 0x07U) << pinMuxLocation;

        pinPullIndex = (pin >> 4U);
    }
    else if (port == 1U)
    {
        pinPullIndex = 2U;

        if ((pin == 0U) || (pin == 1U))
        {
            base->PIO_CFG_MISC &= ~(1U << pin);
            base->PIO_CFG_MISC |= (modeFunc & 0x01U) << pin;
        }
        else if (pin == 2U)
        {
            base->PIO_CFG_MISC &= ~(1U << 16U);
            base->PIO_CFG_MISC |= (modeFunc & 0x01U) << 16U;
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }

    base->PIO_PULL_CFG[pinPullIndex] &= ~(0x03U << pinPullLocation);
    base->PIO_PULL_CFG[pinPullIndex] |= ((modeFunc >> 4U) & 0x03U) << pinPullLocation;

    base->PIO_DRV_CFG[port] &= ~(1U << pin);
    base->PIO_DRV_CFG[port] |= (((modeFunc >> 6U) & 0x01U)) << pin;

    if ((port == 0U) && ((pin == 6U) || (pin == 11U) || (pin == 19U) || (pin == 26U) || (pin == 27U)))
    {
        base->PIO_DRV_CFG[2U] &= ~(1U << pin);
        base->PIO_DRV_CFG[2U] |= (((modeFunc >> 7U) & 0x01U)) << pin;
    }
}

/**
 * @brief   Set all I/O control pin muxing
 * @param   base            The base of SYSCON peripheral on the chip
 * @param   pinArray        Pointer to array of pin mux selections
 * @param   arrayLength     Number of entries in pinArray
 * @return  Nothing
 */
__STATIC_INLINE void IOCON_SetPinMuxing(SYSCON_Type *base, const iocon_group_t *pinArray, uint32_t arrayLength)
{
    uint32_t i;

    for (i = 0U; i < arrayLength; i++)
    {
        IOCON_PinMuxSet(base, pinArray[i].port, pinArray[i].pin, pinArray[i].modefunc);
    }
}

/**
 * @brief   Sets I/O control pin function
 * @param   base            The base of SYSCON peripheral on the chip
 * @param   port            GPIO port (value from 0 ~ 1)
 * @param   pin             GPIO pin (value from 0 ~ 31)
 * @param   func            Pin fucntion (value from 0 ~ 7)
 * @return  Nothing
 */
__STATIC_INLINE void IOCON_FuncSet(SYSCON_Type *base, uint8_t port, uint8_t pin, uint8_t func)
{
    assert(((port == 0U) && (pin <= 31U)) || ((port == 1U) && (pin <= 2U)));
    uint8_t index = (pin >> 3);
    uint8_t pinLocation = ((pin & 0x7U) << 2U);

    if (port == 0U)
    {
        base->PIO_FUNC_CFG[index] &= ~(0x07U << pinLocation);
        base->PIO_FUNC_CFG[index] |= (func & 0x07U) << pinLocation;
    }
    else if (port == 1U)
    {
        if ((pin == 0U) || (pin == 1U))
        {
            base->PIO_CFG_MISC &= ~(1U << pin);
            base->PIO_CFG_MISC |= (func & 0x01U) << pin;
        }
        else if (pin == 2U)
        {
            base->PIO_CFG_MISC &= ~(1U << 16U);
            base->PIO_CFG_MISC |= (func & 0x01U) << 16U;
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}

/**
 * @brief   Sets I/O control drive capability
 * @param   base            The base of SYSCON peripheral on the chip
 * @param   port            GPIO port (value from 0 ~ 1)
 * @param   pin             GPIO pin (value from 0 ~ 31)
 * @param   strength        Drive strength (Extra option is only valid for PA06/PA11/PA19/PA26/PA27)
 *        - kIOCON_LowDriveStrength = 0U - Low-drive strength is configured.
 *        - kIOCON_HighDriveStrength = 1U - High-drive strength is configured
 *        - kIOCON_LowDriveWithExtraStrength = 2U - Low-drive with extra strength is configured
 *        - kIOCON_HighDriveWithExtraStrength = 3U - High-drive with extra strength is configured
 * @return  Nothing
 */
__STATIC_INLINE void IOCON_DriveSet(SYSCON_Type *base, uint8_t port, uint8_t pin, uint8_t strength)
{
    assert(((port == 0U) && (pin <= 31U)) || ((port == 1U) && (pin <= 2U)));
    base->PIO_DRV_CFG[port] &= ~(1U << pin);
    base->PIO_DRV_CFG[port] |= (strength & 0x01U) << pin;
    if ((port == 0U) && ((pin == 6U) || (pin == 11U) || (pin == 19U) || (pin == 26U) || (pin == 27U)))
    {
        base->PIO_DRV_CFG[2U] &= ~(1U << pin);
        base->PIO_DRV_CFG[2U] |= (((strength >> 1U) & 0x01U)) << pin;
    }
}

/**
 * @brief   Sets I/O control pull configuration
 * @param   base            The base of SYSCON peripheral on the chip
 * @param   port            GPIO port (value from 0 ~ 1)
 * @param   pin             GPIO pin (value from 0 ~ 31)
 * @param   pullMode        Pull mode
 *        - kIOCON_HighZ = 0U - High Z is configured.
 *        - kIOCON_PullDown = 1U - Pull-down is configured
 *        - kIOCON_PullUp = 2U - Pull-up is configured
 * @return  Nothing
 */
__STATIC_INLINE void IOCON_PullSet(SYSCON_Type *base, uint8_t port, uint8_t pin, uint8_t pullMode)
{
    assert(((port == 0U) && (pin <= 31U)) || ((port == 1U) && (pin <= 2U)));
    uint8_t index = (port == 1U) ? 2U : (pin >> 4U);
    uint8_t pinLocation = ((pin & 0xFU) << 1U);

    base->PIO_PULL_CFG[index] &= ~(0x03U << pinLocation);
    base->PIO_PULL_CFG[index] |= (uint32_t)pullMode << pinLocation;
}

/* @} */

#ifdef __cplusplus
}
#endif

#endif /* _FSL_IOCON_H_ */
