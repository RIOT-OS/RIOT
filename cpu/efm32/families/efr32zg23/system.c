/***************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M33 system support for EFR32ZG23 devices.
 ******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories, Inc. www.silabs.com</b>
 ******************************************************************************
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
 *****************************************************************************/

#include <stdint.h>
#include "em_device.h"

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

/* System oscillator frequencies. These frequencies are normally constant */
/* for a target, but they are made configurable in order to allow run-time */
/* handling of different boards. The crystal oscillator clocks can be set */
/* compile time to a non-default value by defining respective nFXO_FREQ */
/* values according to board design. By defining the nFXO_FREQ to 0, */
/* one indicates that the oscillator is not present, in order to save some */
/* SW footprint. */

#if !defined(FSRCO_FREQ)
/* FSRCO frequency */
#define FSRCO_FREQ    (20000000UL)
#endif

#if !defined(HFXO_FREQ)
/* HFXO frequency */
#define HFXO_FREQ    (39000000UL)
#endif

#if !defined(HFRCODPLL_STARTUP_FREQ)
/* HFRCODPLL startup frequency */
#define HFRCODPLL_STARTUP_FREQ    (19000000UL)
#endif

#if !defined(HFRCODPLL_MAX_FREQ)
/* Maximum HFRCODPLL frequency */
#define HFRCODPLL_MAX_FREQ    (80000000UL)
#endif

/* CLKIN0 input */
#if !defined(CLKIN0_FREQ)
#define CLKIN0_FREQ    (0UL)
#endif

#if !defined(LFRCO_MAX_FREQ)
/* LFRCO frequency, tuned to below frequency during manufacturing. */
#define LFRCO_FREQ    (32768UL)
#endif

#if !defined(ULFRCO_FREQ)
/* ULFRCO frequency */
#define ULFRCO_FREQ    (1000UL)
#endif

#if !defined(LFXO_FREQ)
/* LFXO frequency */
#define LFXO_FREQ    (LFRCO_FREQ)
#endif

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/

#if (HFXO_FREQ > 0) && !defined(SYSTEM_NO_STATIC_MEMORY)
/* NOTE: Gecko bootloaders can't have static variable allocation. */
/* System HFXO clock frequency */
static uint32_t SystemHFXOClock = HFXO_FREQ;
#endif

#if (LFXO_FREQ > 0) && !defined(SYSTEM_NO_STATIC_MEMORY)
/* System LFXO clock frequency */
static uint32_t SystemLFXOClock = LFXO_FREQ;
#endif

#if !defined(SYSTEM_NO_STATIC_MEMORY)
/* System HFRCODPLL clock frequency */
static uint32_t SystemHFRCODPLLClock = HFRCODPLL_STARTUP_FREQ;
#endif

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

#if !defined(SYSTEM_NO_STATIC_MEMORY)

/**
 * @brief
 *   System System Clock Frequency (Core Clock).
 *
 * @details
 *   Required CMSIS global variable that must be kept up-to-date.
 */
uint32_t SystemCoreClock = HFRCODPLL_STARTUP_FREQ;

#endif

/*---------------------------------------------------------------------------
 * Exception / Interrupt Vector table
 *---------------------------------------------------------------------------*/
extern const tVectorEntry __VECTOR_TABLE[16 + EXT_IRQ_COUNT];

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/**************************************************************************//**
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
 *****************************************************************************/
void SystemInit(void)
{
#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t) (&__VECTOR_TABLE[0]);
#endif

#if defined(UNALIGNED_SUPPORT_DISABLE)
  SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif

#if (__FPU_PRESENT == 1)
  SCB->CPACR |= ((3U << 10U * 2U)           /* set CP10 Full Access */
                 | (3U << 11U * 2U));       /* set CP11 Full Access */
#endif

/* Secure app takes care of moving between the security states.
 * SL_TRUSTZONE_SECURE MACRO is for secure access.
 * SL_TRUSTZONE_NONSECURE MACRO is for non-secure access.
 * When both the MACROS are not defined, during start-up below code makes sure
 * that all the peripherals are accessed from non-secure address except SMU,
 * as SMU is used to configure the trustzone state of the system. */
#if !defined(SL_TRUSTZONE_SECURE) && !defined(SL_TRUSTZONE_NONSECURE) \
  && defined(__TZ_PRESENT)

#if (_SILICON_LABS_32B_SERIES_2_CONFIG >= 2)
  CMU->CLKEN1_SET = CMU_CLKEN1_SMU;
#endif

  /* config SMU to Secure and other peripherals to Non-Secure. */
  SMU->PPUSATD0_CLR = _SMU_PPUSATD0_MASK;
#if defined (SEMAILBOX_PRESENT)
  SMU->PPUSATD1_CLR = (_SMU_PPUSATD1_MASK & (~SMU_PPUSATD1_SMU & ~SMU_PPUSATD1_SEMAILBOX));
#else
  SMU->PPUSATD1_CLR = (_SMU_PPUSATD1_MASK & ~SMU_PPUSATD1_SMU);
#endif

  /* SAU treats all accesses as non-secure */
#if defined(__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  SAU->CTRL = SAU_CTRL_ALLNS_Msk;
  __DSB();
  __ISB();
#else
  #error "The startup code requires access to the CMSE toolchain extension to set proper SAU settings."
#endif /* __ARM_FEATURE_CMSE */

/* Clear and Enable the SMU PPUSEC and BMPUSEC interrupt. */
  NVIC_ClearPendingIRQ(SMU_SECURE_IRQn);
  SMU->IF_CLR = SMU_IF_PPUSEC | SMU_IF_BMPUSEC;
  NVIC_EnableIRQ(SMU_SECURE_IRQn);
  SMU->IEN = SMU_IEN_PPUSEC | SMU_IEN_BMPUSEC;
#endif /*SL_TRUSTZONE_SECURE */
}

/**************************************************************************//**
 * @brief
 *   Get current HFRCODPLL frequency.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   HFRCODPLL frequency in Hz.
 *****************************************************************************/
uint32_t SystemHFRCODPLLClockGet(void)
{
#if !defined(SYSTEM_NO_STATIC_MEMORY)
  return SystemHFRCODPLLClock;
#else
  uint32_t ret = 0UL;

  /* Get oscillator frequency band */
  switch ((HFRCO0->CAL & _HFRCO_CAL_FREQRANGE_MASK)
          >> _HFRCO_CAL_FREQRANGE_SHIFT) {
    case 0:
      switch (HFRCO0->CAL & _HFRCO_CAL_CLKDIV_MASK) {
        case HFRCO_CAL_CLKDIV_DIV1:
          ret = 4000000UL;
          break;

        case HFRCO_CAL_CLKDIV_DIV2:
          ret = 2000000UL;
          break;

        case HFRCO_CAL_CLKDIV_DIV4:
          ret = 1000000UL;
          break;

        default:
          ret = 0UL;
          break;
      }
      break;

    case 3:
      ret = 7000000UL;
      break;

    case 6:
      ret = 13000000UL;
      break;

    case 7:
      ret = 16000000UL;
      break;

    case 8:
      ret = 19000000UL;
      break;

    case 10:
      ret = 26000000UL;
      break;

    case 11:
      ret = 32000000UL;
      break;

    case 12:
      ret = 38000000UL;
      break;

    case 13:
      ret = 48000000UL;
      break;

    case 14:
      ret = 56000000UL;
      break;

    case 15:
      ret = 64000000UL;
      break;

    case 16:
      ret = 80000000UL;
      break;

    default:
      break;
  }
  return ret;
#endif
}

/**************************************************************************//**
 * @brief
 *   Set HFRCODPLL frequency value.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @param[in] freq
 *   HFRCODPLL frequency in Hz.
 *****************************************************************************/
void SystemHFRCODPLLClockSet(uint32_t freq)
{
#if !defined(SYSTEM_NO_STATIC_MEMORY)
  SystemHFRCODPLLClock = freq;
#else
  (void) freq; /* Unused parameter */
#endif
}

/***************************************************************************//**
 * @brief
 *   Get the current system clock frequency (SYSCLK).
 *
 * @details
 *   Calculate and get the current core clock frequency based on the current
 *   hardware configuration.
 *
 * @note
 *   This is an EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   Current system clock (SYSCLK) frequency in Hz.
 ******************************************************************************/
uint32_t SystemSYSCLKGet(void)
{
  uint32_t ret = 0U;

  /* Find clock source */
  switch (CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_CLKSEL_MASK) {
    case _CMU_SYSCLKCTRL_CLKSEL_HFRCODPLL:
      ret = SystemHFRCODPLLClockGet();
      break;

#if (HFXO_FREQ > 0U)
    case _CMU_SYSCLKCTRL_CLKSEL_HFXO:
#if defined(SYSTEM_NO_STATIC_MEMORY)
      ret = HFXO_FREQ;
#else
      ret = SystemHFXOClock;
#endif
      break;
#endif

#if (CLKIN0_FREQ > 0U)
    case _CMU_SYSCLKCTRL_CLKSEL_CLKIN0:
      ret = CLKIN0_FREQ;
      break;
#endif

    case _CMU_SYSCLKCTRL_CLKSEL_FSRCO:
      ret = FSRCO_FREQ;
      break;

    default:
      /* Unknown clock source. */
      while (1) {
      }
  }
  return ret;
}

/***************************************************************************//**
 * @brief
 *   Get the current system core clock frequency (HCLK).
 *
 * @details
 *   Calculate and get the current core clock frequency based on the current
 *   configuration. Assuming that the SystemCoreClock global variable is
 *   maintained, the core clock frequency is stored in that variable as well.
 *   This function will however calculate the core clock based on actual HW
 *   configuration. It will also update the SystemCoreClock global variable.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   The current core clock (HCLK) frequency in Hz.
 ******************************************************************************/
uint32_t SystemHCLKGet(void)
{
  uint32_t presc, ret;

  ret = SystemSYSCLKGet();

  presc = (CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_HCLKPRESC_MASK)
          >> _CMU_SYSCLKCTRL_HCLKPRESC_SHIFT;

  ret /= presc + 1U;

#if !defined(SYSTEM_NO_STATIC_MEMORY)
  /* Keep CMSIS system clock variable up-to-date */
  SystemCoreClock = ret;
#endif

  return ret;
}

/***************************************************************************//**
 * @brief
 *   Get the maximum core clock frequency.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   The maximum core clock frequency in Hz.
 ******************************************************************************/
uint32_t SystemMaxCoreClockGet(void)
{
  return(HFRCODPLL_MAX_FREQ > HFXO_FREQ \
         ? HFRCODPLL_MAX_FREQ : HFXO_FREQ);
}

/**************************************************************************//**
 * @brief
 *   Get high frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   HFXO frequency in Hz. 0 if the external crystal oscillator is not present.
 *****************************************************************************/
uint32_t SystemHFXOClockGet(void)
{
  /* The external crystal oscillator is not present if HFXO_FREQ==0 */
#if (HFXO_FREQ > 0U)
#if defined(SYSTEM_NO_STATIC_MEMORY)
  return HFXO_FREQ;
#else
  return SystemHFXOClock;
#endif
#else
  return 0U;
#endif
}

/**************************************************************************//**
 * @brief
 *   Set high frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This function is mainly provided for being able to handle target systems
 *   with different HF crystal oscillator frequencies run-time. If used, it
 *   should probably only be used once during system startup.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @param[in] freq
 *   HFXO frequency in Hz used for target.
 *****************************************************************************/
void SystemHFXOClockSet(uint32_t freq)
{
  /* External crystal oscillator present? */
#if (HFXO_FREQ > 0) && !defined(SYSTEM_NO_STATIC_MEMORY)
  SystemHFXOClock = freq;

  /* Update core clock frequency if HFXO is used to clock core */
  if ((CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_CLKSEL_MASK)
      == _CMU_SYSCLKCTRL_CLKSEL_HFXO) {
    /* This function will update the global variable */
    SystemHCLKGet();
  }
#else
  (void) freq; /* Unused parameter */
#endif
}

/**************************************************************************//**
 * @brief
 *   Get current CLKIN0 frequency.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   CLKIN0 frequency in Hz.
 *****************************************************************************/
uint32_t SystemCLKIN0Get(void)
{
  return CLKIN0_FREQ;
}

/**************************************************************************//**
 * @brief
 *   Get FSRCO frequency.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   FSRCO frequency in Hz.
 *****************************************************************************/
uint32_t SystemFSRCOClockGet(void)
{
  return FSRCO_FREQ;
}

/**************************************************************************//**
 * @brief
 *   Get current HFRCOEM23 frequency.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   HFRCOEM23 frequency in Hz.
 *****************************************************************************/
uint32_t SystemHFRCOEM23ClockGet(void)
{
  uint32_t ret = 0UL;

  /* Get oscillator frequency band */
  switch ((HFRCOEM23->CAL & _HFRCO_CAL_FREQRANGE_MASK)
          >> _HFRCO_CAL_FREQRANGE_SHIFT) {
    case 0:
      switch (HFRCOEM23->CAL & _HFRCO_CAL_CLKDIV_MASK) {
        case HFRCO_CAL_CLKDIV_DIV1:
          ret = 4000000UL;
          break;

        case HFRCO_CAL_CLKDIV_DIV2:
          ret = 2000000UL;
          break;

        case HFRCO_CAL_CLKDIV_DIV4:
          ret = 1000000UL;
          break;

        default:
          ret = 0UL;
          break;
      }
      break;

    case 6:
      ret = 13000000UL;
      break;

    case 7:
      ret = 16000000UL;
      break;

    case 8:
      ret = 19000000UL;
      break;

    case 10:
      ret = 26000000UL;
      break;

    case 11:
      ret = 32000000UL;
      break;

    case 12:
      ret = 40000000UL;
      break;

    default:
      break;
  }
  return ret;
}

/**************************************************************************//**
 * @brief
 *   Get low frequency RC oscillator clock frequency for target system.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   LFRCO frequency in Hz.
 *****************************************************************************/
uint32_t SystemLFRCOClockGet(void)
{
  return LFRCO_FREQ;
}

/**************************************************************************//**
 * @brief
 *   Get ultra low frequency RC oscillator clock frequency for target system.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   ULFRCO frequency in Hz.
 *****************************************************************************/
uint32_t SystemULFRCOClockGet(void)
{
  /* The ULFRCO frequency is not tuned, and can be very inaccurate */
  return ULFRCO_FREQ;
}

/**************************************************************************//**
 * @brief
 *   Get low frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @return
 *   LFXO frequency in Hz.
 *****************************************************************************/
uint32_t SystemLFXOClockGet(void)
{
  /* External crystal present? */
#if (LFXO_FREQ > 0U)
#if defined(SYSTEM_NO_STATIC_MEMORY)
  return LFXO_FREQ;
#else
  return SystemLFXOClock;
#endif
#else
  return 0U;
#endif
}

/**************************************************************************//**
 * @brief
 *   Set low frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This function is mainly provided for being able to handle target systems
 *   with different HF crystal oscillator frequencies run-time. If used, it
 *   should probably only be used once during system startup.
 *
 * @note
 *   This is a EFR32ZG23 specific function, not part of the
 *   CMSIS definition.
 *
 * @param[in] freq
 *   LFXO frequency in Hz used for target.
 *****************************************************************************/
void SystemLFXOClockSet(uint32_t freq)
{
  /* External crystal oscillator present? */
#if (LFXO_FREQ > 0U) && !defined(SYSTEM_NO_STATIC_MEMORY)
  SystemLFXOClock = freq;
#else
  (void) freq; /* Unused parameter */
#endif
}
