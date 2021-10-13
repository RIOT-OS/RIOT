/***************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M0+ System Layer for EFM32ZG devices.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdint.h>
#include "em_device.h"

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

/** LFRCO frequency, tuned to below frequency during manufacturing. */
#define EFM32_LFRCO_FREQ  (32768UL)
/** ULFRCO frequency. */
#define EFM32_ULFRCO_FREQ (1000UL)

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/

/* System oscillator frequencies. These frequencies are normally constant */
/* for a target, but they are made configurable in order to allow run-time */
/* handling of different boards. The crystal oscillator clocks can be set */
/* compile time to a non-default value by defining respective EFM32_nFXO_FREQ */
/* values according to board design. By defining the EFM32_nFXO_FREQ to 0, */
/* one indicates that the oscillator is not present, in order to save some */
/* SW footprint. */

#ifndef EFM32_HFXO_FREQ
/** HFXO frequency. */
#define EFM32_HFXO_FREQ (24000000UL)
#endif

/** Maximum HFRCO frequency. */
#define EFM32_HFRCO_MAX_FREQ    (21000000UL)

/* Do not define variable if HF crystal oscillator not present */
#if (EFM32_HFXO_FREQ > 0U)
/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/** System HFXO clock. */
static uint32_t SystemHFXOClock = EFM32_HFXO_FREQ;
/** @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
#endif

#ifndef EFM32_LFXO_FREQ
/** LFXO frequency. */
#define EFM32_LFXO_FREQ (EFM32_LFRCO_FREQ)
#endif

/* Do not define variable if LF crystal oscillator not present */
#if (EFM32_LFXO_FREQ > 0U)
/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/** System LFXO clock. */
static uint32_t SystemLFXOClock = EFM32_LFXO_FREQ;
/** @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
#endif

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

/**
 * @brief
 *   System System Clock Frequency (Core Clock).
 *
 * @details
 *   Required CMSIS global variable that must be kept up-to-date.
 */
uint32_t SystemCoreClock = 14000000UL;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Get the current core clock frequency.
 *
 * @details
 *   Calculate and get the current core clock frequency based on the current
 *   configuration. Assuming that the SystemCoreClock global variable is
 *   maintained, the core clock frequency is stored in that variable as well.
 *   This function will however calculate the core clock based on actual HW
 *   configuration. It will also update the SystemCoreClock global variable.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   The current core clock frequency in Hz.
 ******************************************************************************/
uint32_t SystemCoreClockGet(void)
{
  uint32_t ret;

  ret = SystemHFClockGet();
  ret >>= (CMU->HFCORECLKDIV & _CMU_HFCORECLKDIV_HFCORECLKDIV_MASK)
          >> _CMU_HFCORECLKDIV_HFCORECLKDIV_SHIFT;

  /* Keep CMSIS variable up-to-date just in case */
  SystemCoreClock = ret;

  return ret;
}

/***************************************************************************//**
 * @brief
 *   Get the maximum core clock frequency.
 *
 * @note
 *   This is an EFR32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   The maximum core clock frequency in Hz.
 ******************************************************************************/
uint32_t SystemMaxCoreClockGet(void)
{
#if (EFM32_HFRCO_MAX_FREQ > EFM32_HFXO_FREQ)
  return EFM32_HFRCO_MAX_FREQ;
#else
  return EFM32_HFXO_FREQ;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get the current HFCLK frequency.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   The current HFCLK frequency in Hz.
 ******************************************************************************/
uint32_t SystemHFClockGet(void)
{
  uint32_t ret;

  switch (CMU->STATUS & (CMU_STATUS_HFRCOSEL | CMU_STATUS_HFXOSEL
                         | CMU_STATUS_LFRCOSEL | CMU_STATUS_LFXOSEL)) {
    case CMU_STATUS_LFXOSEL:
#if (EFM32_LFXO_FREQ > 0U)
      ret = SystemLFXOClock;
#else
      /* We should not get here, since core should not be clocked. May */
      /* be caused by a misconfiguration though. */
      ret = 0U;
#endif
      break;

    case CMU_STATUS_LFRCOSEL:
      ret = EFM32_LFRCO_FREQ;
      break;

    case CMU_STATUS_HFXOSEL:
#if (EFM32_HFXO_FREQ > 0U)
      ret = SystemHFXOClock;
#else
      /* We should not get here, since core should not be clocked. May */
      /* be caused by a misconfiguration though. */
      ret = 0U;
#endif
      break;

    default: /* CMU_STATUS_HFRCOSEL */
      switch (CMU->HFRCOCTRL & _CMU_HFRCOCTRL_BAND_MASK) {
        case CMU_HFRCOCTRL_BAND_21MHZ:
          ret = 21000000U;
          break;

        case CMU_HFRCOCTRL_BAND_14MHZ:
          ret = 14000000U;
          break;

        case CMU_HFRCOCTRL_BAND_11MHZ:
          ret = 11000000U;
          break;

        case CMU_HFRCOCTRL_BAND_7MHZ:
          ret = 6600000U;
          break;

        case CMU_HFRCOCTRL_BAND_1MHZ:
          ret = 1200000U;
          break;

        default:
          ret = 0U;
          break;
      }
      break;
  }

  return ret;
}

/***************************************************************************//**
 * @brief
 *   Get high frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   HFXO frequency in Hz.
 ******************************************************************************/
uint32_t SystemHFXOClockGet(void)
{
  /* External crystal oscillator present? */
#if (EFM32_HFXO_FREQ > 0U)
  return SystemHFXOClock;
#else
  return 0U;
#endif
}

/***************************************************************************//**
 * @brief
 *   Set high frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This function is mainly provided for being able to handle target systems
 *   with different HF crystal oscillator frequencies run-time. If used, it
 *   should probably only be used once during system startup.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @param[in] freq
 *   HFXO frequency in Hz used for target.
 ******************************************************************************/
void SystemHFXOClockSet(uint32_t freq)
{
  /* External crystal oscillator present? */
#if (EFM32_HFXO_FREQ > 0U)
  SystemHFXOClock = freq;

  /* Update core clock frequency if HFXO is used to clock core */
  if ((CMU->STATUS & CMU_STATUS_HFXOSEL) != 0U) {
    /* The function will update the global variable */
    (void)SystemCoreClockGet();
  }
#else
  (void)freq; /* Unused parameter */
#endif
}

/***************************************************************************//**
 * @brief
 *   Initialize the system.
 *
 * @details
 *   Do required generic HW system init.
 *
 * @note
 *   This function is invoked during system init, before the main() routine
 *   and any data has been initialized. For this reason, it cannot do any
 *   initialization of variables etc.
 ******************************************************************************/
void SystemInit(void)
{
#if defined(__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t)&__Vectors;
#endif
}

/***************************************************************************//**
 * @brief
 *   Get low frequency RC oscillator clock frequency for target system.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   LFRCO frequency in Hz.
 ******************************************************************************/
uint32_t SystemLFRCOClockGet(void)
{
  /* Currently we assume that this frequency is properly tuned during */
  /* manufacturing and is not changed after reset. If future requirements */
  /* for re-tuning by user, we can add support for that. */
  return EFM32_LFRCO_FREQ;
}

/***************************************************************************//**
 * @brief
 *   Get ultra low frequency RC oscillator clock frequency for target system.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   ULFRCO frequency in Hz.
 ******************************************************************************/
uint32_t SystemULFRCOClockGet(void)
{
  /* The ULFRCO frequency is not tuned, and can be very inaccurate */
  return EFM32_ULFRCO_FREQ;
}

/***************************************************************************//**
 * @brief
 *   Get low frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   LFXO frequency in Hz.
 ******************************************************************************/
uint32_t SystemLFXOClockGet(void)
{
  /* External crystal oscillator present? */
#if (EFM32_LFXO_FREQ > 0U)
  return SystemLFXOClock;
#else
  return 0U;
#endif
}

/***************************************************************************//**
 * @brief
 *   Set low frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This function is mainly provided for being able to handle target systems
 *   with different HF crystal oscillator frequencies run-time. If used, it
 *   should probably only be used once during system startup.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @param[in] freq
 *   LFXO frequency in Hz used for target.
 ******************************************************************************/
void SystemLFXOClockSet(uint32_t freq)
{
  /* External crystal oscillator present? */
#if (EFM32_LFXO_FREQ > 0U)
  SystemLFXOClock = freq;

  /* Update core clock frequency if LFXO is used to clock core */
  if ((CMU->STATUS & CMU_STATUS_LFXOSEL) != 0U) {
    /* The function will update the global variable */
    (void)SystemCoreClockGet();
  }
#else
  (void)freq; /* Unused parameter */
#endif
}
