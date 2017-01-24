/***************************************************************************//**
 * @file em_emu.c
 * @brief Energy Management Unit (EMU) Peripheral API
 * @version 5.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include <limits.h>

#include "em_emu.h"
#if defined( EMU_PRESENT ) && ( EMU_COUNT > 0 )

#include "em_cmu.h"
#include "em_system.h"
#include "em_common.h"
#include "em_assert.h"

/***************************************************************************//**
 * @addtogroup emlib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup EMU
 * @brief Energy Management Unit (EMU) Peripheral API
 * @details
 *  This module contains functions to control the EMU peripheral of Silicon
 *  Labs 32-bit MCUs and SoCs. The EMU handles the different low energy modes
 *  in Silicon Labs microcontrollers.
 * @{
 ******************************************************************************/

/* Consistency check, since restoring assumes similar bitpositions in */
/* CMU OSCENCMD and STATUS regs */
#if (CMU_STATUS_AUXHFRCOENS != CMU_OSCENCMD_AUXHFRCOEN)
#error Conflict in AUXHFRCOENS and AUXHFRCOEN bitpositions
#endif
#if (CMU_STATUS_HFXOENS != CMU_OSCENCMD_HFXOEN)
#error Conflict in HFXOENS and HFXOEN bitpositions
#endif
#if (CMU_STATUS_LFRCOENS != CMU_OSCENCMD_LFRCOEN)
#error Conflict in LFRCOENS and LFRCOEN bitpositions
#endif
#if (CMU_STATUS_LFXOENS != CMU_OSCENCMD_LFXOEN)
#error Conflict in LFXOENS and LFXOEN bitpositions
#endif


/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/* Fix for errata EMU_E107 - non-WIC interrupt masks. */
#if defined( _EFM32_GECKO_FAMILY )
#define ERRATA_FIX_EMU_E107_EN
#define NON_WIC_INT_MASK_0    (~(0x0dfc0323U))
#define NON_WIC_INT_MASK_1    (~(0x0U))

#elif defined( _EFM32_TINY_FAMILY )
#define ERRATA_FIX_EMU_E107_EN
#define NON_WIC_INT_MASK_0    (~(0x001be323U))
#define NON_WIC_INT_MASK_1    (~(0x0U))

#elif defined( _EFM32_GIANT_FAMILY )
#define ERRATA_FIX_EMU_E107_EN
#define NON_WIC_INT_MASK_0    (~(0xff020e63U))
#define NON_WIC_INT_MASK_1    (~(0x00000046U))

#elif defined( _EFM32_WONDER_FAMILY )
#define ERRATA_FIX_EMU_E107_EN
#define NON_WIC_INT_MASK_0    (~(0xff020e63U))
#define NON_WIC_INT_MASK_1    (~(0x00000046U))

#else
/* Zero Gecko and future families are not affected by errata EMU_E107 */
#endif

/* Fix for errata EMU_E108 - High Current Consumption on EM4 Entry. */
#if defined( _EFM32_HAPPY_FAMILY )
#define ERRATA_FIX_EMU_E108_EN
#endif

/* Fix for errata EMU_E208 - Occasional Full Reset After Exiting EM4H */
#if defined( _SILICON_LABS_32B_PLATFORM_2_GEN_1 )
#define ERRATA_FIX_EMU_E208_EN
#endif

/* Enable FETCNT tuning errata fix */
#if defined( _EMU_DCDCCTRL_MASK ) && defined( _SILICON_LABS_32B_PLATFORM_2_GEN_1 )
#define ERRATA_FIX_DCDC_FETCNT_SET_EN
#endif

/* Enable LN handshake errata fix */
#if defined( _EMU_DCDCCTRL_MASK ) && ( _SILICON_LABS_32B_PLATFORM_2_GEN < 3 )
#define ERRATA_FIX_DCDC_LNHS_BLOCK_EN
#endif

/* Enable bypass current limiter enable timing fix */
#if defined( _SILICON_LABS_32B_PLATFORM_2_GEN_2 )
#define ERRATA_FIX_BYPLIMEN_TIMING_EN
#endif

#define EMU_DCDCCLIMCTRL                        (uint32_t *)(EMU_BASE + 0x054)
#if !defined(_EMU_DCDCCLIMCTRL_BYPLIMEN_SHIFT)
#define _EMU_DCDCCLIMCTRL_BYPLIMEN_SHIFT        13
#endif
#if !defined(_EMU_PWRCTRL_DVDDBODDIS_SHIFT)
#define EMU_PWRCTRL_DVDDBODDIS                  (1 << 12)
#endif

/** @endcond */


#if defined( _EMU_DCDCCTRL_MASK )
/* DCDCTODVDD output range min/max */
#if !defined(PWRCFG_DCDCTODVDD_VMIN)
#define PWRCFG_DCDCTODVDD_VMIN          1800
#endif
#if !defined(PWRCFG_DCDCTODVDD_VMAX)
#define PWRCFG_DCDCTODVDD_VMAX          3000
#endif

typedef enum
{
  errataFixDcdcHsInit,
  errataFixDcdcHsTrimSet,
  errataFixDcdcHsBypassLn,
  errataFixDcdcHsLnWaitDone
} errataFixDcdcHs_TypeDef;
errataFixDcdcHs_TypeDef errataFixDcdcHsState = errataFixDcdcHsInit;
#endif

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/**
 * CMU configured oscillator selection and oscillator enable status. When a
 * user configures oscillators, this varaiable shall shadow the configuration.
 * It is used by the EMU module in order to be able to restore the oscillator
 * config after having been in certain energy modes (since HW may automatically
 * alter config when going into an energy mode). It is the responsibility of
 * the CMU module to keep it up-to-date (or a user if not using the CMU API
 * for oscillator control).
 */
static uint32_t cmuStatus;
#if defined( _CMU_HFCLKSTATUS_RESETVALUE )
static uint16_t cmuHfclkStatus;
#endif
#if defined( _EMU_DCDCCTRL_MASK )
static uint16_t dcdcMaxCurrent_mA;
static uint16_t dcdcEm01LoadCurrent_mA;
static EMU_DcdcLnReverseCurrentControl_TypeDef dcdcReverseCurrentControl;
#endif

/** @endcond */


/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Restore oscillators and core clock after having been in EM2 or EM3.
 ******************************************************************************/
static void emuRestore(void)
{
  uint32_t oscEnCmd;
  uint32_t cmuLocked;

  /* Although we could use the CMU API for most of the below handling, we */
  /* would like this function to be as efficient as possible. */

  /* CMU registers may be locked */
  cmuLocked = CMU->LOCK & CMU_LOCK_LOCKKEY_LOCKED;
  CMU_Unlock();

  /* AUXHFRCO are automatically disabled (except if using debugger). */
  /* HFRCO, USHFRCO and HFXO are automatically disabled. */
  /* LFRCO/LFXO may be disabled by SW in EM3. */
  /* Restore according to status prior to entering energy mode. */
  oscEnCmd = 0;
  oscEnCmd |= ((cmuStatus & CMU_STATUS_HFRCOENS)    ? CMU_OSCENCMD_HFRCOEN : 0);
  oscEnCmd |= ((cmuStatus & CMU_STATUS_AUXHFRCOENS) ? CMU_OSCENCMD_AUXHFRCOEN : 0);
  oscEnCmd |= ((cmuStatus & CMU_STATUS_LFRCOENS)    ? CMU_OSCENCMD_LFRCOEN : 0);
  oscEnCmd |= ((cmuStatus & CMU_STATUS_HFXOENS)     ? CMU_OSCENCMD_HFXOEN : 0);
  oscEnCmd |= ((cmuStatus & CMU_STATUS_LFXOENS)     ? CMU_OSCENCMD_LFXOEN : 0);
#if defined( _CMU_STATUS_USHFRCOENS_MASK )
  oscEnCmd |= ((cmuStatus & CMU_STATUS_USHFRCOENS)  ? CMU_OSCENCMD_USHFRCOEN : 0);
#endif
  CMU->OSCENCMD = oscEnCmd;


#if defined( _CMU_HFCLKSTATUS_RESETVALUE )
  /* Restore oscillator used for clocking core */
  switch (cmuHfclkStatus & _CMU_HFCLKSTATUS_SELECTED_MASK)
  {
    case CMU_HFCLKSTATUS_SELECTED_LFRCO:
      /* HFRCO could only be selected if the autostart HFXO feature is not
       * enabled, otherwise the HFXO would be started and selected automatically.
       * Note: this error hook helps catching erroneous oscillator configurations,
       * when the AUTOSTARTSELEM0EM1 is set in CMU_HFXOCTRL. */
      if (!(CMU->HFXOCTRL & CMU_HFXOCTRL_AUTOSTARTSELEM0EM1))
      {
        /* Wait for LFRCO to stabilize */
        while (!(CMU->STATUS & CMU_STATUS_LFRCORDY))
          ;
        CMU->HFCLKSEL = CMU_HFCLKSEL_HF_LFRCO;
      }
      else
      {
        EFM_ASSERT(0);
      }
      break;

    case CMU_HFCLKSTATUS_SELECTED_LFXO:
      /* Wait for LFXO to stabilize */
      while (!(CMU->STATUS & CMU_STATUS_LFXORDY))
        ;
      CMU->HFCLKSEL = CMU_HFCLKSEL_HF_LFXO;
      break;

    case CMU_HFCLKSTATUS_SELECTED_HFXO:
      /* Wait for HFXO to stabilize */
      while (!(CMU->STATUS & CMU_STATUS_HFXORDY))
        ;
#if defined( _CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_MASK )
      if (BUS_RegMaskedRead(&CMU->HFXOCTRL,
                            _CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_MASK)
          == CMU_HFXOCTRL_PEAKDETSHUNTOPTMODE_AUTOCMD)
      {
        while (BUS_RegMaskedRead(&CMU->STATUS,
                                _CMU_STATUS_HFXOSHUNTOPTRDY_MASK
                                | _CMU_STATUS_HFXOPEAKDETRDY_MASK)
               != (CMU_STATUS_HFXOSHUNTOPTRDY | CMU_STATUS_HFXOPEAKDETRDY))
          ;
      }
#endif
      CMU->HFCLKSEL = CMU_HFCLKSEL_HF_HFXO;
      break;

    default: /* CMU_HFCLKSTATUS_SELECTED_HFRCO */
      /* If core clock was HFRCO core clock, it is automatically restored to */
      /* state prior to entering energy mode. No need for further action. */
      break;
  }
#else
  switch (cmuStatus & (CMU_STATUS_HFRCOSEL
                      | CMU_STATUS_HFXOSEL
                      | CMU_STATUS_LFRCOSEL
#if defined( CMU_STATUS_USHFRCODIV2SEL )
                      | CMU_STATUS_USHFRCODIV2SEL
#endif
                      | CMU_STATUS_LFXOSEL))
  {
    case CMU_STATUS_LFRCOSEL:
      /* Wait for LFRCO to stabilize */
      while (!(CMU->STATUS & CMU_STATUS_LFRCORDY))
        ;
      CMU->CMD = CMU_CMD_HFCLKSEL_LFRCO;
      break;

    case CMU_STATUS_LFXOSEL:
      /* Wait for LFXO to stabilize */
      while (!(CMU->STATUS & CMU_STATUS_LFXORDY))
        ;
      CMU->CMD = CMU_CMD_HFCLKSEL_LFXO;
      break;

    case CMU_STATUS_HFXOSEL:
      /* Wait for HFXO to stabilize */
      while (!(CMU->STATUS & CMU_STATUS_HFXORDY))
        ;
      CMU->CMD = CMU_CMD_HFCLKSEL_HFXO;
      break;

#if defined( CMU_STATUS_USHFRCODIV2SEL )
    case CMU_STATUS_USHFRCODIV2SEL:
      /* Wait for USHFRCO to stabilize */
      while (!(CMU->STATUS & CMU_STATUS_USHFRCORDY))
        ;
      CMU->CMD = _CMU_CMD_HFCLKSEL_USHFRCODIV2;
      break;
#endif

    default: /* CMU_STATUS_HFRCOSEL */
      /* If core clock was HFRCO core clock, it is automatically restored to */
      /* state prior to entering energy mode. No need for further action. */
      break;
  }

  /* If HFRCO was disabled before entering Energy Mode, turn it off again */
  /* as it is automatically enabled by wake up */
  if ( ! (cmuStatus & CMU_STATUS_HFRCOENS) )
  {
    CMU->OSCENCMD = CMU_OSCENCMD_HFRCODIS;
  }
#endif
  /* Restore CMU register locking */
  if (cmuLocked)
  {
    CMU_Lock();
  }
}


#if defined( ERRATA_FIX_EMU_E107_EN )
/* Get enable conditions for errata EMU_E107 fix. */
__STATIC_INLINE bool getErrataFixEmuE107En(void)
{
  /* SYSTEM_ChipRevisionGet could have been used here, but we would like a
   * faster implementation in this case.
   */
  uint16_t majorMinorRev;

  /* CHIP MAJOR bit [3:0] */
  majorMinorRev = ((ROMTABLE->PID0 & _ROMTABLE_PID0_REVMAJOR_MASK)
                   >> _ROMTABLE_PID0_REVMAJOR_SHIFT)
                  << 8;
  /* CHIP MINOR bit [7:4] */
  majorMinorRev |= ((ROMTABLE->PID2 & _ROMTABLE_PID2_REVMINORMSB_MASK)
                    >> _ROMTABLE_PID2_REVMINORMSB_SHIFT)
                   << 4;
  /* CHIP MINOR bit [3:0] */
  majorMinorRev |= (ROMTABLE->PID3 & _ROMTABLE_PID3_REVMINORLSB_MASK)
                   >> _ROMTABLE_PID3_REVMINORLSB_SHIFT;

#if defined( _EFM32_GECKO_FAMILY )
  return (majorMinorRev <= 0x0103);
#elif defined( _EFM32_TINY_FAMILY )
  return (majorMinorRev <= 0x0102);
#elif defined( _EFM32_GIANT_FAMILY )
  return (majorMinorRev <= 0x0103) || (majorMinorRev == 0x0204);
#elif defined( _EFM32_WONDER_FAMILY )
  return (majorMinorRev == 0x0100);
#else
  /* Zero Gecko and future families are not affected by errata EMU_E107 */
  return false;
#endif
}
#endif

/* LP prepare / LN restore P/NFET count */
#define DCDC_LP_PFET_CNT        7
#define DCDC_LP_NFET_CNT        7
#if defined( ERRATA_FIX_DCDC_FETCNT_SET_EN )
static void currentLimitersUpdate(void);
static void dcdcFetCntSet(bool lpModeSet)
{
  uint32_t tmp;
  static uint32_t emuDcdcMiscCtrlReg;

  if (lpModeSet)
  {
    emuDcdcMiscCtrlReg = EMU->DCDCMISCCTRL;
    tmp  = EMU->DCDCMISCCTRL
           & ~(_EMU_DCDCMISCCTRL_PFETCNT_MASK | _EMU_DCDCMISCCTRL_NFETCNT_MASK);
    tmp |= (DCDC_LP_PFET_CNT << _EMU_DCDCMISCCTRL_PFETCNT_SHIFT)
            | (DCDC_LP_NFET_CNT << _EMU_DCDCMISCCTRL_NFETCNT_SHIFT);
    EMU->DCDCMISCCTRL = tmp;
    currentLimitersUpdate();
  }
  else
  {
    EMU->DCDCMISCCTRL = emuDcdcMiscCtrlReg;
    currentLimitersUpdate();
  }
}
#endif

#if defined( ERRATA_FIX_DCDC_LNHS_BLOCK_EN )
static void dcdcHsFixLnBlock(void)
{
#define EMU_DCDCSTATUS  (* (volatile uint32_t *)(EMU_BASE + 0x7C))
  if ((errataFixDcdcHsState == errataFixDcdcHsTrimSet)
      || (errataFixDcdcHsState == errataFixDcdcHsBypassLn))
  {
    /* Wait for LNRUNNING */
    if ((EMU->DCDCCTRL & _EMU_DCDCCTRL_DCDCMODE_MASK) == EMU_DCDCCTRL_DCDCMODE_LOWNOISE)
    {
      while (!(EMU_DCDCSTATUS & (0x1 << 16)));
    }
    errataFixDcdcHsState = errataFixDcdcHsLnWaitDone;
  }
}
#endif



/** @endcond */


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Enter energy mode 2 (EM2).
 *
 * @details
 *   When entering EM2, the high frequency clocks are disabled, ie HFXO, HFRCO
 *   and AUXHFRCO (for AUXHFRCO, see exception note below). When re-entering
 *   EM0, HFRCO is re-enabled and the core will be clocked by the configured
 *   HFRCO band. This ensures a quick wakeup from EM2.
 *
 *   However, prior to entering EM2, the core may have been using another
 *   oscillator than HFRCO. The @p restore parameter gives the user the option
 *   to restore all HF oscillators according to state prior to entering EM2,
 *   as well as the clock used to clock the core. This restore procedure is
 *   handled by SW. However, since handled by SW, it will not be restored
 *   before completing the interrupt function(s) waking up the core!
 *
 * @note
 *   If restoring core clock to use the HFXO oscillator, which has been
 *   disabled during EM2 mode, this function will stall until the oscillator
 *   has stabilized. Stalling time can be reduced by adding interrupt
 *   support detecting stable oscillator, and an asynchronous switch to the
 *   original oscillator. See CMU documentation. Such a feature is however
 *   outside the scope of the implementation in this function.
 * @par
 *   If HFXO is re-enabled by this function, and NOT used to clock the core,
 *   this function will not wait for HFXO to stabilize. This must be considered
 *   by the application if trying to use features relying on that oscillator
 *   upon return.
 * @par
 *   If a debugger is attached, the AUXHFRCO will not be disabled if enabled
 *   upon entering EM2. It will thus remain enabled when returning to EM0
 *   regardless of the @p restore parameter.
 * @par
 *   If HFXO autostart and select is enabled by using CMU_HFXOAutostartEnable(),
 *   the starting and selecting of the core clocks will be identical to the user
 *   independently of the value of the @p restore parameter when waking up on
 *   the wakeup sources corresponding to the autostart and select setting.
 *
 * @param[in] restore
 *   @li true - restore oscillators and clocks, see function details.
 *   @li false - do not restore oscillators and clocks, see function details.
 * @par
 *   The @p restore option should only be used if all clock control is done
 *   via the CMU API.
 ******************************************************************************/
void EMU_EnterEM2(bool restore)
{
#if defined( ERRATA_FIX_EMU_E107_EN )
  bool errataFixEmuE107En;
  uint32_t nonWicIntEn[2];
#endif

  /* Auto-update CMU status just in case before entering energy mode. */
  /* This variable is normally kept up-to-date by the CMU API. */
  cmuStatus = CMU->STATUS;
#if defined( _CMU_HFCLKSTATUS_RESETVALUE )
  cmuHfclkStatus = (uint16_t)(CMU->HFCLKSTATUS);
#endif

  /* Enter Cortex deep sleep mode */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

  /* Fix for errata EMU_E107 - store non-WIC interrupt enable flags.
     Disable the enabled non-WIC interrupts. */
#if defined( ERRATA_FIX_EMU_E107_EN )
  errataFixEmuE107En = getErrataFixEmuE107En();
  if (errataFixEmuE107En)
  {
    nonWicIntEn[0] = NVIC->ISER[0] & NON_WIC_INT_MASK_0;
    NVIC->ICER[0] = nonWicIntEn[0];
#if (NON_WIC_INT_MASK_1 != (~(0x0U)))
    nonWicIntEn[1] = NVIC->ISER[1] & NON_WIC_INT_MASK_1;
    NVIC->ICER[1] = nonWicIntEn[1];
#endif
  }
#endif

#if defined( ERRATA_FIX_DCDC_FETCNT_SET_EN )
  dcdcFetCntSet(true);
#endif
#if defined( ERRATA_FIX_DCDC_LNHS_BLOCK_EN )
  dcdcHsFixLnBlock();
#endif
#if defined( ERRATA_FIX_BYPLIMEN_TIMING_EN )
  BUS_RegBitWrite(EMU_DCDCCLIMCTRL, _EMU_DCDCCLIMCTRL_BYPLIMEN_SHIFT, 0);
#endif

  __WFI();

#if defined( ERRATA_FIX_DCDC_FETCNT_SET_EN )
  dcdcFetCntSet(false);
#endif

  /* Fix for errata EMU_E107 - restore state of non-WIC interrupt enable flags. */
#if defined( ERRATA_FIX_EMU_E107_EN )
  if (errataFixEmuE107En)
  {
    NVIC->ISER[0] = nonWicIntEn[0];
#if (NON_WIC_INT_MASK_1 != (~(0x0U)))
    NVIC->ISER[1] = nonWicIntEn[1];
#endif
  }
#endif

  /* Restore oscillators/clocks if specified */
  if (restore)
  {
    emuRestore();
  }
  /* If not restoring, and original clock was not HFRCO, we have to */
  /* update CMSIS core clock variable since core clock has changed */
  /* to using HFRCO. */
#if defined( _CMU_HFCLKSTATUS_RESETVALUE )
  else if ((cmuHfclkStatus & _CMU_HFCLKSTATUS_SELECTED_MASK)
           != CMU_HFCLKSTATUS_SELECTED_HFRCO)
#else
  else if (!(cmuStatus & CMU_STATUS_HFRCOSEL))
#endif
  {
    SystemCoreClockUpdate();
  }
}


/***************************************************************************//**
 * @brief
 *   Enter energy mode 3 (EM3).
 *
 * @details
 *   When entering EM3, the high frequency clocks are disabled by HW, ie HFXO,
 *   HFRCO and AUXHFRCO (for AUXHFRCO, see exception note below). In addition,
 *   the low frequency clocks, ie LFXO and LFRCO are disabled by SW. When
 *   re-entering EM0, HFRCO is re-enabled and the core will be clocked by the
 *   configured HFRCO band. This ensures a quick wakeup from EM3.
 *
 *   However, prior to entering EM3, the core may have been using another
 *   oscillator than HFRCO. The @p restore parameter gives the user the option
 *   to restore all HF/LF oscillators according to state prior to entering EM3,
 *   as well as the clock used to clock the core. This restore procedure is
 *   handled by SW. However, since handled by SW, it will not be restored
 *   before completing the interrupt function(s) waking up the core!
 *
 * @note
 *   If restoring core clock to use an oscillator other than HFRCO, this
 *   function will stall until the oscillator has stabilized. Stalling time
 *   can be reduced by adding interrupt support detecting stable oscillator,
 *   and an asynchronous switch to the original oscillator. See CMU
 *   documentation. Such a feature is however outside the scope of the
 *   implementation in this function.
 * @par
 *   If HFXO/LFXO/LFRCO are re-enabled by this function, and NOT used to clock
 *   the core, this function will not wait for those oscillators to stabilize.
 *   This must be considered by the application if trying to use features
 *   relying on those oscillators upon return.
 * @par
 *   If a debugger is attached, the AUXHFRCO will not be disabled if enabled
 *   upon entering EM3. It will thus remain enabled when returning to EM0
 *   regardless of the @p restore parameter.
 *
 * @param[in] restore
 *   @li true - restore oscillators and clocks, see function details.
 *   @li false - do not restore oscillators and clocks, see function details.
 * @par
 *   The @p restore option should only be used if all clock control is done
 *   via the CMU API.
 ******************************************************************************/
void EMU_EnterEM3(bool restore)
{
  uint32_t cmuLocked;

#if defined( ERRATA_FIX_EMU_E107_EN )
  bool errataFixEmuE107En;
  uint32_t nonWicIntEn[2];
#endif

  /* Auto-update CMU status just in case before entering energy mode. */
  /* This variable is normally kept up-to-date by the CMU API. */
  cmuStatus = CMU->STATUS;
#if defined( _CMU_HFCLKSTATUS_RESETVALUE )
  cmuHfclkStatus = (uint16_t)(CMU->HFCLKSTATUS);
#endif

  /* CMU registers may be locked */
  cmuLocked = CMU->LOCK & CMU_LOCK_LOCKKEY_LOCKED;
  CMU_Unlock();

  /* Disable LF oscillators */
  CMU->OSCENCMD = CMU_OSCENCMD_LFXODIS | CMU_OSCENCMD_LFRCODIS;

  /* Restore CMU register locking */
  if (cmuLocked)
  {
    CMU_Lock();
  }

  /* Enter Cortex deep sleep mode */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

  /* Fix for errata EMU_E107 - store non-WIC interrupt enable flags.
     Disable the enabled non-WIC interrupts. */
#if defined( ERRATA_FIX_EMU_E107_EN )
  errataFixEmuE107En = getErrataFixEmuE107En();
  if (errataFixEmuE107En)
  {
    nonWicIntEn[0] = NVIC->ISER[0] & NON_WIC_INT_MASK_0;
    NVIC->ICER[0] = nonWicIntEn[0];
#if (NON_WIC_INT_MASK_1 != (~(0x0U)))
    nonWicIntEn[1] = NVIC->ISER[1] & NON_WIC_INT_MASK_1;
    NVIC->ICER[1] = nonWicIntEn[1];
#endif
  }
#endif

#if defined( ERRATA_FIX_DCDC_FETCNT_SET_EN )
  dcdcFetCntSet(true);
#endif
#if defined( ERRATA_FIX_DCDC_LNHS_BLOCK_EN )
  dcdcHsFixLnBlock();
#endif
#if defined( ERRATA_FIX_BYPLIMEN_TIMING_EN )
  BUS_RegBitWrite(EMU_DCDCCLIMCTRL, _EMU_DCDCCLIMCTRL_BYPLIMEN_SHIFT, 0);
#endif

  __WFI();

#if defined( ERRATA_FIX_DCDC_FETCNT_SET_EN )
  dcdcFetCntSet(false);
#endif

  /* Fix for errata EMU_E107 - restore state of non-WIC interrupt enable flags. */
#if defined( ERRATA_FIX_EMU_E107_EN )
  if (errataFixEmuE107En)
  {
    NVIC->ISER[0] = nonWicIntEn[0];
#if (NON_WIC_INT_MASK_1 != (~(0x0U)))
    NVIC->ISER[1] = nonWicIntEn[1];
#endif
  }
#endif

  /* Restore oscillators/clocks if specified */
  if (restore)
  {
    emuRestore();
  }
  /* If not restoring, and original clock was not HFRCO, we have to */
  /* update CMSIS core clock variable since core clock has changed */
  /* to using HFRCO. */
#if defined( _CMU_HFCLKSTATUS_RESETVALUE )
  else if ((cmuHfclkStatus & _CMU_HFCLKSTATUS_SELECTED_MASK)
           != CMU_HFCLKSTATUS_SELECTED_HFRCO)
#else
  else if (!(cmuStatus & CMU_STATUS_HFRCOSEL))
#endif
  {
    SystemCoreClockUpdate();
  }
}


/***************************************************************************//**
 * @brief
 *   Enter energy mode 4 (EM4).
 *
 * @note
 *   Only a power on reset or external reset pin can wake the device from EM4.
 ******************************************************************************/
void EMU_EnterEM4(void)
{
  int i;

#if defined( _EMU_EM4CTRL_EM4ENTRY_SHIFT )
  uint32_t em4seq2 = (EMU->EM4CTRL & ~_EMU_EM4CTRL_EM4ENTRY_MASK)
                     | (2 << _EMU_EM4CTRL_EM4ENTRY_SHIFT);
  uint32_t em4seq3 = (EMU->EM4CTRL & ~_EMU_EM4CTRL_EM4ENTRY_MASK)
                     | (3 << _EMU_EM4CTRL_EM4ENTRY_SHIFT);
#else
  uint32_t em4seq2 = (EMU->CTRL & ~_EMU_CTRL_EM4CTRL_MASK)
                     | (2 << _EMU_CTRL_EM4CTRL_SHIFT);
  uint32_t em4seq3 = (EMU->CTRL & ~_EMU_CTRL_EM4CTRL_MASK)
                     | (3 << _EMU_CTRL_EM4CTRL_SHIFT);
#endif

  /* Make sure register write lock is disabled */
  EMU_Unlock();

#if defined( _EMU_EM4CTRL_MASK )
  if ((EMU->EM4CTRL & _EMU_EM4CTRL_EM4STATE_MASK) == EMU_EM4CTRL_EM4STATE_EM4S)
  {
    uint32_t dcdcMode = EMU->DCDCCTRL & _EMU_DCDCCTRL_DCDCMODE_MASK;
    if (dcdcMode == EMU_DCDCCTRL_DCDCMODE_LOWNOISE
        || dcdcMode == EMU_DCDCCTRL_DCDCMODE_LOWPOWER)
    {
      /* DCDC is not supported in EM4S so we switch DCDC to bypass mode before
       * entering EM4S */
      EMU_DCDCModeSet(emuDcdcMode_Bypass);
    }
  }
#endif

#if defined( _EMU_EM4CTRL_MASK ) && defined( ERRATA_FIX_EMU_E208_EN )
  if (EMU->EM4CTRL & EMU_EM4CTRL_EM4STATE_EM4H)
  {
    /* Fix for errata EMU_E208 - Occasional Full Reset After Exiting EM4H.
     * Full description of errata fix can be found in the errata document. */
    __disable_irq();
    *(volatile uint32_t *)(EMU_BASE + 0x190)  = 0x0000ADE8UL;
    *(volatile uint32_t *)(EMU_BASE + 0x198) |= (0x1UL << 7);
    *(volatile uint32_t *)(EMU_BASE + 0x88)  |= (0x1UL << 8);
  }
#endif

#if defined( ERRATA_FIX_EMU_E108_EN )
  /* Fix for errata EMU_E108 - High Current Consumption on EM4 Entry. */
  __disable_irq();
  *(volatile uint32_t *)0x400C80E4 = 0;
#endif

#if defined( ERRATA_FIX_DCDC_FETCNT_SET_EN )
  dcdcFetCntSet(true);
#endif
#if defined( ERRATA_FIX_DCDC_LNHS_BLOCK_EN )
  dcdcHsFixLnBlock();
#endif
#if defined( ERRATA_FIX_BYPLIMEN_TIMING_EN )
  BUS_RegBitWrite(EMU_DCDCCLIMCTRL, _EMU_DCDCCLIMCTRL_BYPLIMEN_SHIFT, 0);
#endif

  for (i = 0; i < 4; i++)
  {
#if defined( _EMU_EM4CTRL_EM4ENTRY_SHIFT )
    EMU->EM4CTRL = em4seq2;
    EMU->EM4CTRL = em4seq3;
  }
  EMU->EM4CTRL = em4seq2;
#else
    EMU->CTRL = em4seq2;
    EMU->CTRL = em4seq3;
  }
  EMU->CTRL = em4seq2;
#endif
}

#if defined( _EMU_EM4CTRL_MASK )
/***************************************************************************//**
 * @brief
 *   Enter energy mode 4 hibernate (EM4H).
 *
 * @note
 *   Retention of clocks and GPIO in EM4 can be configured using
 *   @ref EMU_EM4Init before calling this function.
 ******************************************************************************/
void EMU_EnterEM4H(void)
{
  BUS_RegBitWrite(&EMU->EM4CTRL, _EMU_EM4CTRL_EM4STATE_SHIFT, 1);
  EMU_EnterEM4();
}

/***************************************************************************//**
 * @brief
 *   Enter energy mode 4 shutoff (EM4S).
 *
 * @note
 *   Retention of clocks and GPIO in EM4 can be configured using
 *   @ref EMU_EM4Init before calling this function.
 ******************************************************************************/
void EMU_EnterEM4S(void)
{
  BUS_RegBitWrite(&EMU->EM4CTRL, _EMU_EM4CTRL_EM4STATE_SHIFT, 0);
  EMU_EnterEM4();
}
#endif

/***************************************************************************//**
 * @brief
 *   Power down memory block.
 *
 * @param[in] blocks
 *   Specifies a logical OR of bits indicating memory blocks to power down.
 *   Bit 0 selects block 1, bit 1 selects block 2, etc. Memory block 0 cannot
 *   be disabled. Please refer to the reference manual for available
 *   memory blocks for a device.
 *
 * @note
 *   Only a reset can make the specified memory block(s) available for use
 *   after having been powered down. Function will be void for devices not
 *   supporting this feature.
 ******************************************************************************/
void EMU_MemPwrDown(uint32_t blocks)
{
#if defined( _EMU_MEMCTRL_POWERDOWN_MASK )
  EFM_ASSERT(blocks <= (_EMU_MEMCTRL_POWERDOWN_MASK
                        >> _EMU_MEMCTRL_POWERDOWN_SHIFT));
  EMU->MEMCTRL = blocks;

#elif defined( _EMU_MEMCTRL_RAMPOWERDOWN_MASK )       \
      && defined( _EMU_MEMCTRL_RAMHPOWERDOWN_MASK )   \
      && defined( _EMU_MEMCTRL_SEQRAMPOWERDOWN_MASK )
  EFM_ASSERT((blocks & (_EMU_MEMCTRL_RAMPOWERDOWN_MASK
                        | _EMU_MEMCTRL_RAMHPOWERDOWN_MASK
                        | _EMU_MEMCTRL_SEQRAMPOWERDOWN_MASK))
             == blocks);
  EMU->MEMCTRL = blocks;

#elif defined( _EMU_MEMCTRL_RAMPOWERDOWN_MASK )
  EFM_ASSERT((blocks & _EMU_MEMCTRL_RAMPOWERDOWN_MASK) == blocks);
  EMU->MEMCTRL = blocks;

#elif defined( _EMU_RAM0CTRL_RAMPOWERDOWN_MASK )
  EFM_ASSERT((blocks & _EMU_RAM0CTRL_RAMPOWERDOWN_MASK) == blocks);
  EMU->RAM0CTRL = blocks;

#else
  (void)blocks;
#endif
}


/***************************************************************************//**
 * @brief
 *   Update EMU module with CMU oscillator selection/enable status.
 *
 * @details
 *   When entering EM2 and EM3, the HW may change the core clock oscillator
 *   used, as well as disabling some oscillators. The user may optionally select
 *   to restore the oscillators after waking up from EM2 and EM3 through the
 *   SW API.
 *
 *   However, in order to support this in a safe way, the EMU module must
 *   be kept up-to-date on the actual selected configuration. The CMU
 *   module must keep the EMU module up-to-date.
 *
 *   This function is mainly intended for internal use by the CMU module,
 *   but if the applications changes oscillator configurations without
 *   using the CMU API, this function can be used to keep the EMU module
 *   up-to-date.
 ******************************************************************************/
void EMU_UpdateOscConfig(void)
{
  /* Fetch current configuration */
  cmuStatus = CMU->STATUS;
#if defined( _CMU_HFCLKSTATUS_RESETVALUE )
  cmuHfclkStatus = (uint16_t)(CMU->HFCLKSTATUS);
#endif
}


/***************************************************************************//**
 * @brief
 *   Update EMU module with Energy Mode 2 and 3 configuration
 *
 * @param[in] em23Init
 *    Energy Mode 2 and 3 configuration structure
 ******************************************************************************/
void EMU_EM23Init(EMU_EM23Init_TypeDef *em23Init)
{
#if defined( _EMU_CTRL_EMVREG_MASK )
  EMU->CTRL = em23Init->em23VregFullEn ? (EMU->CTRL | EMU_CTRL_EMVREG)
                                         : (EMU->CTRL & ~EMU_CTRL_EMVREG);
#elif defined( _EMU_CTRL_EM23VREG_MASK )
  EMU->CTRL = em23Init->em23VregFullEn ? (EMU->CTRL | EMU_CTRL_EM23VREG)
                                         : (EMU->CTRL & ~EMU_CTRL_EM23VREG);
#else
  (void)em23Init;
#endif
}


#if defined( _EMU_EM4CONF_MASK ) || defined( _EMU_EM4CTRL_MASK )
/***************************************************************************//**
 * @brief
 *   Update EMU module with Energy Mode 4 configuration
 *
 * @param[in] em4Init
 *    Energy Mode 4 configuration structure
 ******************************************************************************/
void EMU_EM4Init(EMU_EM4Init_TypeDef *em4Init)
{
#if defined( _EMU_EM4CONF_MASK )
  /* Init for platforms with EMU->EM4CONF register */
  uint32_t em4conf = EMU->EM4CONF;

  /* Clear fields that will be reconfigured */
  em4conf &= ~(_EMU_EM4CONF_LOCKCONF_MASK
               | _EMU_EM4CONF_OSC_MASK
               | _EMU_EM4CONF_BURTCWU_MASK
               | _EMU_EM4CONF_VREGEN_MASK);

  /* Configure new settings */
  em4conf |= (em4Init->lockConfig << _EMU_EM4CONF_LOCKCONF_SHIFT)
             | (em4Init->osc)
             | (em4Init->buRtcWakeup << _EMU_EM4CONF_BURTCWU_SHIFT)
             | (em4Init->vreg << _EMU_EM4CONF_VREGEN_SHIFT);

  /* Apply configuration. Note that lock can be set after this stage. */
  EMU->EM4CONF = em4conf;

#elif defined( _EMU_EM4CTRL_MASK )
  /* Init for platforms with EMU->EM4CTRL register */

  uint32_t em4ctrl = EMU->EM4CTRL;

  em4ctrl &= ~(_EMU_EM4CTRL_RETAINLFXO_MASK
               | _EMU_EM4CTRL_RETAINLFRCO_MASK
               | _EMU_EM4CTRL_RETAINULFRCO_MASK
               | _EMU_EM4CTRL_EM4STATE_MASK
               | _EMU_EM4CTRL_EM4IORETMODE_MASK);

  em4ctrl |= (em4Init->retainLfxo     ? EMU_EM4CTRL_RETAINLFXO : 0)
              | (em4Init->retainLfrco  ? EMU_EM4CTRL_RETAINLFRCO : 0)
              | (em4Init->retainUlfrco ? EMU_EM4CTRL_RETAINULFRCO : 0)
              | (em4Init->em4State     ? EMU_EM4CTRL_EM4STATE_EM4H : 0)
              | (em4Init->pinRetentionMode);

  EMU->EM4CTRL = em4ctrl;
#endif
}
#endif


#if defined( BU_PRESENT )
/***************************************************************************//**
 * @brief
 *   Configure Backup Power Domain settings
 *
 * @param[in] bupdInit
 *   Backup power domain initialization structure
 ******************************************************************************/
void EMU_BUPDInit(EMU_BUPDInit_TypeDef *bupdInit)
{
  uint32_t reg;

  /* Set power connection configuration */
  reg = EMU->PWRCONF & ~(_EMU_PWRCONF_PWRRES_MASK
                         | _EMU_PWRCONF_VOUTSTRONG_MASK
                         | _EMU_PWRCONF_VOUTMED_MASK
                         | _EMU_PWRCONF_VOUTWEAK_MASK);

  reg |= bupdInit->resistor
         | (bupdInit->voutStrong << _EMU_PWRCONF_VOUTSTRONG_SHIFT)
         | (bupdInit->voutMed    << _EMU_PWRCONF_VOUTMED_SHIFT)
         | (bupdInit->voutWeak   << _EMU_PWRCONF_VOUTWEAK_SHIFT);

  EMU->PWRCONF = reg;

  /* Set backup domain inactive mode configuration */
  reg = EMU->BUINACT & ~(_EMU_BUINACT_PWRCON_MASK);
  reg |= (bupdInit->inactivePower);
  EMU->BUINACT = reg;

  /* Set backup domain active mode configuration */
  reg = EMU->BUACT & ~(_EMU_BUACT_PWRCON_MASK);
  reg |= (bupdInit->activePower);
  EMU->BUACT = reg;

  /* Set power control configuration */
  reg = EMU->BUCTRL & ~(_EMU_BUCTRL_PROBE_MASK
                        | _EMU_BUCTRL_BODCAL_MASK
                        | _EMU_BUCTRL_STATEN_MASK
                        | _EMU_BUCTRL_EN_MASK);

  /* Note use of ->enable to both enable BUPD, use BU_VIN pin input and
     release reset */
  reg |= bupdInit->probe
         | (bupdInit->bodCal          << _EMU_BUCTRL_BODCAL_SHIFT)
         | (bupdInit->statusPinEnable << _EMU_BUCTRL_STATEN_SHIFT)
         | (bupdInit->enable          << _EMU_BUCTRL_EN_SHIFT);

  /* Enable configuration */
  EMU->BUCTRL = reg;

  /* If enable is true, enable BU_VIN input power pin, if not disable it  */
  EMU_BUPinEnable(bupdInit->enable);

  /* If enable is true, release BU reset, if not keep reset asserted */
  BUS_RegBitWrite(&(RMU->CTRL), _RMU_CTRL_BURSTEN_SHIFT, !bupdInit->enable);
}


/***************************************************************************//**
 * @brief
 *   Configure Backup Power Domain BOD Threshold value
 * @note
 *   These values are precalibrated
 * @param[in] mode Active or Inactive mode
 * @param[in] value
 ******************************************************************************/
void EMU_BUThresholdSet(EMU_BODMode_TypeDef mode, uint32_t value)
{
  EFM_ASSERT(value<8);
  EFM_ASSERT(value<=(_EMU_BUACT_BUEXTHRES_MASK>>_EMU_BUACT_BUEXTHRES_SHIFT));

  switch(mode)
  {
    case emuBODMode_Active:
      EMU->BUACT = (EMU->BUACT & ~_EMU_BUACT_BUEXTHRES_MASK)
                   | (value<<_EMU_BUACT_BUEXTHRES_SHIFT);
      break;
    case emuBODMode_Inactive:
      EMU->BUINACT = (EMU->BUINACT & ~_EMU_BUINACT_BUENTHRES_MASK)
                     | (value<<_EMU_BUINACT_BUENTHRES_SHIFT);
      break;
  }
}


/***************************************************************************//**
 * @brief
 *  Configure Backup Power Domain BOD Threshold Range
 * @note
 *  These values are precalibrated
 * @param[in] mode Active or Inactive mode
 * @param[in] value
 ******************************************************************************/
void EMU_BUThresRangeSet(EMU_BODMode_TypeDef mode, uint32_t value)
{
  EFM_ASSERT(value < 4);
  EFM_ASSERT(value<=(_EMU_BUACT_BUEXRANGE_MASK>>_EMU_BUACT_BUEXRANGE_SHIFT));

  switch(mode)
  {
    case emuBODMode_Active:
      EMU->BUACT = (EMU->BUACT & ~_EMU_BUACT_BUEXRANGE_MASK)
                   | (value<<_EMU_BUACT_BUEXRANGE_SHIFT);
      break;
    case emuBODMode_Inactive:
      EMU->BUINACT = (EMU->BUINACT & ~_EMU_BUINACT_BUENRANGE_MASK)
                     | (value<<_EMU_BUINACT_BUENRANGE_SHIFT);
      break;
  }
}
#endif


#if defined( _EMU_DCDCCTRL_MASK )

/* Translate fields with different names across platform generations to common names. */
#if defined( _EMU_DCDCMISCCTRL_LPCMPBIAS_MASK )
#define _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_MASK      _EMU_DCDCMISCCTRL_LPCMPBIAS_MASK
#define _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT     _EMU_DCDCMISCCTRL_LPCMPBIAS_SHIFT
#elif defined( _EMU_DCDCMISCCTRL_LPCMPBIASEM234H_MASK )
#define _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_MASK      _EMU_DCDCMISCCTRL_LPCMPBIASEM234H_MASK
#define _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT     _EMU_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT
#endif
#if defined( _EMU_DCDCLPCTRL_LPCMPHYSSEL_MASK )
#define _GENERIC_DCDCLPCTRL_LPCMPHYSSELEM234H_MASK      _EMU_DCDCLPCTRL_LPCMPHYSSEL_MASK
#define _GENERIC_DCDCLPCTRL_LPCMPHYSSELEM234H_SHIFT     _EMU_DCDCLPCTRL_LPCMPHYSSEL_SHIFT
#elif defined( _EMU_DCDCLPCTRL_LPCMPHYSSELEM234H_MASK )
#define _GENERIC_DCDCLPCTRL_LPCMPHYSSELEM234H_MASK      _EMU_DCDCLPCTRL_LPCMPHYSSELEM234H_MASK
#define _GENERIC_DCDCLPCTRL_LPCMPHYSSELEM234H_SHIFT     _EMU_DCDCLPCTRL_LPCMPHYSSELEM234H_SHIFT
#endif

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Load DCDC calibration constants from DI page. Const means calibration
 *   data that does not change depending on other configuration parameters.
 *
 * @return
 *   False if calibration registers are locked
 ******************************************************************************/
static bool ConstCalibrationLoad(void)
{
#if defined( _SILICON_LABS_32B_PLATFORM_2_GEN_1 )
  uint32_t val;
  volatile uint32_t *reg;

  /* DI calib data in flash */
  volatile uint32_t* const diCal_EMU_DCDCLNFREQCTRL =  (volatile uint32_t *)(0x0FE08038);
  volatile uint32_t* const diCal_EMU_DCDCLNVCTRL =     (volatile uint32_t *)(0x0FE08040);
  volatile uint32_t* const diCal_EMU_DCDCLPCTRL =      (volatile uint32_t *)(0x0FE08048);
  volatile uint32_t* const diCal_EMU_DCDCLPVCTRL =     (volatile uint32_t *)(0x0FE08050);
  volatile uint32_t* const diCal_EMU_DCDCTRIM0 =       (volatile uint32_t *)(0x0FE08058);
  volatile uint32_t* const diCal_EMU_DCDCTRIM1 =       (volatile uint32_t *)(0x0FE08060);

  if (DEVINFO->DCDCLPVCTRL0 != UINT_MAX)
  {
    val = *(diCal_EMU_DCDCLNFREQCTRL + 1);
    reg = (volatile uint32_t *)*diCal_EMU_DCDCLNFREQCTRL;
    *reg = val;

    val = *(diCal_EMU_DCDCLNVCTRL + 1);
    reg = (volatile uint32_t *)*diCal_EMU_DCDCLNVCTRL;
    *reg = val;

    val = *(diCal_EMU_DCDCLPCTRL + 1);
    reg = (volatile uint32_t *)*diCal_EMU_DCDCLPCTRL;
    *reg = val;

    val = *(diCal_EMU_DCDCLPVCTRL + 1);
    reg = (volatile uint32_t *)*diCal_EMU_DCDCLPVCTRL;
    *reg = val;

    val = *(diCal_EMU_DCDCTRIM0 + 1);
    reg = (volatile uint32_t *)*diCal_EMU_DCDCTRIM0;
    *reg = val;

    val = *(diCal_EMU_DCDCTRIM1 + 1);
    reg = (volatile uint32_t *)*diCal_EMU_DCDCTRIM1;
    *reg = val;

    return true;
  }
  EFM_ASSERT(false);
  /* Return when assertions are disabled */
  return false;

#elif defined( _SILICON_LABS_32B_PLATFORM_2_GEN_2 )
  return true;
#else
#error "Undefined platform 2 generation."
  return true;
#endif
}


/***************************************************************************//**
 * @brief
 *   Set recommended and validated current optimization and timing settings
 *
 ******************************************************************************/
static void ValidatedConfigSet(void)
{
/* Disable LP mode hysterysis in the state machine control */
#define EMU_DCDCMISCCTRL_LPCMPHYSDIS (0x1UL << 1)
/* Comparator threshold on the high side */
#define EMU_DCDCMISCCTRL_LPCMPHYSHI  (0x1UL << 2)
#define EMU_DCDCSMCTRL  (* (volatile uint32_t *)(EMU_BASE + 0x44))

#if defined( _SILICON_LABS_32B_PLATFORM_2_GEN_1 )
  uint32_t dcdcTiming;
  SYSTEM_PartFamily_TypeDef family;
  SYSTEM_ChipRevision_TypeDef rev;
#endif

  uint32_t lnForceCcm = BUS_RegBitRead(&EMU->DCDCMISCCTRL, _EMU_DCDCMISCCTRL_LNFORCECCM_SHIFT);

  /* Enable duty cycling of the bias */
  EMU->DCDCLPCTRL |= EMU_DCDCLPCTRL_LPVREFDUTYEN;

  /* Set low-noise RCO for LNFORCECCM configuration
   * LNFORCECCM is default 1 for EFR32
   * LNFORCECCM is default 0 for EFM32
   */
  if (lnForceCcm)
  {
    /* 7MHz is recommended for LNFORCECCM = 1 */
    EMU_DCDCLnRcoBandSet(emuDcdcLnRcoBand_7MHz);
  }
  else
  {
    /* 3MHz is recommended for LNFORCECCM = 0 */
    EMU_DCDCLnRcoBandSet(emuDcdcLnRcoBand_3MHz);
  }

#if defined( _SILICON_LABS_32B_PLATFORM_2_GEN_1 )
  EMU->DCDCTIMING &= ~_EMU_DCDCTIMING_DUTYSCALE_MASK;
  EMU->DCDCMISCCTRL |= EMU_DCDCMISCCTRL_LPCMPHYSDIS
                       | EMU_DCDCMISCCTRL_LPCMPHYSHI;

  family = SYSTEM_GetFamily();
  SYSTEM_ChipRevisionGet(&rev);
  if ((((family >= systemPartFamilyMighty1P)
         && (family <= systemPartFamilyFlex1V))
       || (family == systemPartFamilyEfm32Pearl1B)
       || (family == systemPartFamilyEfm32Jade1B))
      && ((rev.major == 1) && (rev.minor < 3))
      && (errataFixDcdcHsState == errataFixDcdcHsInit))
  {
    /* LPCMPWAITDIS = 1 */
    EMU_DCDCSMCTRL |= 1;

    dcdcTiming = EMU->DCDCTIMING;
    dcdcTiming &= ~(_EMU_DCDCTIMING_LPINITWAIT_MASK
                    |_EMU_DCDCTIMING_LNWAIT_MASK
                    |_EMU_DCDCTIMING_BYPWAIT_MASK);

    dcdcTiming |= ((180 << _EMU_DCDCTIMING_LPINITWAIT_SHIFT)
                   | (12 << _EMU_DCDCTIMING_LNWAIT_SHIFT)
                   | (180 << _EMU_DCDCTIMING_BYPWAIT_SHIFT));
    EMU->DCDCTIMING = dcdcTiming;

    errataFixDcdcHsState = errataFixDcdcHsTrimSet;
  }
#endif
}


/***************************************************************************//**
 * @brief
 *   Compute current limiters:
 *     LNCLIMILIMSEL: LN current limiter threshold
 *     LPCLIMILIMSEL: LP current limiter threshold
 *     DCDCZDETCTRL:  zero detector limiter threshold
 ******************************************************************************/
static void currentLimitersUpdate(void)
{
  uint32_t lncLimSel;
  uint32_t zdetLimSel;
  uint32_t pFetCnt;
  uint16_t maxReverseCurrent_mA;

    /* 80mA as recommended peak in Application Note AN0948.
       The peak current is the average current plus 50% of the current ripple.
       Hence, a 14mA average current is recommended in LP mode. Since LP PFETCNT is also
       a constant, we get lpcLimImSel = 1. The following calculation is provided
       for documentation only. */
  const uint32_t lpcLim = (((14 + 40) + ((14 + 40) / 2))
                           / (5 * (DCDC_LP_PFET_CNT + 1)))
                          - 1;
  const uint32_t lpcLimSel = lpcLim << _EMU_DCDCMISCCTRL_LPCLIMILIMSEL_SHIFT;

  /* Get enabled PFETs */
  pFetCnt = (EMU->DCDCMISCCTRL & _EMU_DCDCMISCCTRL_PFETCNT_MASK)
             >> _EMU_DCDCMISCCTRL_PFETCNT_SHIFT;

  /* Compute LN current limiter threshold from nominal user input current and
     LN PFETCNT as described in the register description for
     EMU_DCDCMISCCTRL_LNCLIMILIMSEL. */
  lncLimSel = (((dcdcMaxCurrent_mA + 40) + ((dcdcMaxCurrent_mA + 40) / 2))
               / (5 * (pFetCnt + 1)))
              - 1;

  /* Saturate the register field value */
  lncLimSel = SL_MIN(lncLimSel,
                     _EMU_DCDCMISCCTRL_LNCLIMILIMSEL_MASK
                      >> _EMU_DCDCMISCCTRL_LNCLIMILIMSEL_SHIFT);

  lncLimSel <<= _EMU_DCDCMISCCTRL_LNCLIMILIMSEL_SHIFT;

  /* Check for overflow */
  EFM_ASSERT((lncLimSel & ~_EMU_DCDCMISCCTRL_LNCLIMILIMSEL_MASK) == 0x0);
  EFM_ASSERT((lpcLimSel & ~_EMU_DCDCMISCCTRL_LPCLIMILIMSEL_MASK) == 0x0);

  EMU->DCDCMISCCTRL = (EMU->DCDCMISCCTRL & ~(_EMU_DCDCMISCCTRL_LNCLIMILIMSEL_MASK
                                             | _EMU_DCDCMISCCTRL_LPCLIMILIMSEL_MASK))
                       | (lncLimSel | lpcLimSel);


  /* Compute reverse current limit threshold for the zero detector from user input
     maximum reverse current and LN PFETCNT as described in the register description
     for EMU_DCDCZDETCTRL_ZDETILIMSEL. */
  if (dcdcReverseCurrentControl >= 0)
  {
    /* If dcdcReverseCurrentControl < 0, then EMU_DCDCZDETCTRL_ZDETILIMSEL is "don't care" */
    maxReverseCurrent_mA = (uint16_t)dcdcReverseCurrentControl;

    zdetLimSel = ( ((maxReverseCurrent_mA + 40) + ((maxReverseCurrent_mA + 40) / 2))
                    / ((2 * (pFetCnt + 1)) + ((pFetCnt + 1) / 2)) );
    /* Saturate the register field value */
    zdetLimSel = SL_MIN(zdetLimSel,
                        _EMU_DCDCZDETCTRL_ZDETILIMSEL_MASK
                         >> _EMU_DCDCZDETCTRL_ZDETILIMSEL_SHIFT);

    zdetLimSel <<= _EMU_DCDCZDETCTRL_ZDETILIMSEL_SHIFT;

    /* Check for overflow */
    EFM_ASSERT((zdetLimSel & ~_EMU_DCDCZDETCTRL_ZDETILIMSEL_MASK) == 0x0);

    EMU->DCDCZDETCTRL = (EMU->DCDCZDETCTRL & ~_EMU_DCDCZDETCTRL_ZDETILIMSEL_MASK)
                         | zdetLimSel;
  }
}


/***************************************************************************//**
 * @brief
 *   Set static variables that hold the user set maximum peak current
 *   and reverse current. Update limiters.
 *
 * @param[in] maxCurrent_mA
 *   Set the maximum peak current that the DCDC can draw from the power source.
 * @param[in] reverseCurrentControl
 *   Reverse current control as defined by
 *   @ref EMU_DcdcLnReverseCurrentControl_TypeDef. Positive values have unit mA.
 ******************************************************************************/
static void userCurrentLimitsSet(uint32_t maxCurrent_mA,
                                 EMU_DcdcLnReverseCurrentControl_TypeDef reverseCurrentControl)
{
  dcdcMaxCurrent_mA = maxCurrent_mA;
  dcdcReverseCurrentControl = reverseCurrentControl;
}


/***************************************************************************//**
 * @brief
 *   Set DCDC low noise compensator control register
 *
 * @param[in] comp
 *   Low-noise mode compensator trim setpoint
 ******************************************************************************/
static void compCtrlSet(EMU_DcdcLnCompCtrl_TypeDef comp)
{
#define EMU_DCDCLNCOMPCTRL      (*(volatile uint32_t *) (EMU_BASE + 0x58UL))

  switch (comp)
  {
    case emuDcdcLnCompCtrl_1u0F:
      EMU_DCDCLNCOMPCTRL = 0x57204077UL;
      break;

    case emuDcdcLnCompCtrl_4u7F:
      EMU_DCDCLNCOMPCTRL = 0xB7102137UL;
      break;

    default:
      EFM_ASSERT(false);
      break;
  }
}

/***************************************************************************//**
 * @brief
 *   Load EMU_DCDCLPCTRL_LPCMPHYSSEL depending on LP bias, LP feedback
 *   attenuation and DEVINFOREV.
 *
 * @param[in] attSet
 *   LP feedback attenuation.
 * @param[in] lpCmpBias
 *   lpCmpBias selection (unshifted)
 ******************************************************************************/
static bool LpCmpHystCalibrationLoad(bool lpAttenuation, uint32_t lpCmpBias)
{
  uint8_t devinfoRev;
  uint32_t lpcmpHystSel;

  /* Get calib data revision */
  devinfoRev = SYSTEM_GetDevinfoRev();

  /* Load LPATT indexed calibration data */
  if (devinfoRev < 4)
  {
    lpcmpHystSel = DEVINFO->DCDCLPCMPHYSSEL0;

    if (lpAttenuation)
    {
      lpcmpHystSel = (lpcmpHystSel & _DEVINFO_DCDCLPCMPHYSSEL0_LPCMPHYSSELLPATT1_MASK)
                      >> _DEVINFO_DCDCLPCMPHYSSEL0_LPCMPHYSSELLPATT1_SHIFT;
    }
    else
    {
      lpcmpHystSel = (lpcmpHystSel & _DEVINFO_DCDCLPCMPHYSSEL0_LPCMPHYSSELLPATT0_MASK)
                      >> _DEVINFO_DCDCLPCMPHYSSEL0_LPCMPHYSSELLPATT0_SHIFT;
    }
  }
  else
  {
    /* devinfoRev >= 4: load LPCMPBIAS indexed calibration data */
    lpcmpHystSel = DEVINFO->DCDCLPCMPHYSSEL1;
    switch (lpCmpBias)
    {
      case 0:
        lpcmpHystSel = (lpcmpHystSel & _DEVINFO_DCDCLPCMPHYSSEL1_LPCMPHYSSELLPCMPBIAS0_MASK)
                        >> _DEVINFO_DCDCLPCMPHYSSEL1_LPCMPHYSSELLPCMPBIAS0_SHIFT;
        break;

      case 1 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
        lpcmpHystSel = (lpcmpHystSel & _DEVINFO_DCDCLPCMPHYSSEL1_LPCMPHYSSELLPCMPBIAS1_MASK)
                        >> _DEVINFO_DCDCLPCMPHYSSEL1_LPCMPHYSSELLPCMPBIAS1_SHIFT;
        break;

      case 2 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
        lpcmpHystSel = (lpcmpHystSel & _DEVINFO_DCDCLPCMPHYSSEL1_LPCMPHYSSELLPCMPBIAS2_MASK)
                        >> _DEVINFO_DCDCLPCMPHYSSEL1_LPCMPHYSSELLPCMPBIAS2_SHIFT;
        break;

      case 3 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
        lpcmpHystSel = (lpcmpHystSel & _DEVINFO_DCDCLPCMPHYSSEL1_LPCMPHYSSELLPCMPBIAS3_MASK)
                        >> _DEVINFO_DCDCLPCMPHYSSEL1_LPCMPHYSSELLPCMPBIAS3_SHIFT;
        break;

      default:
        EFM_ASSERT(false);
        /* Return when assertions are disabled */
        return false;
    }
  }
  /* Make sure the sel value is within the field range. */
  lpcmpHystSel <<= _GENERIC_DCDCLPCTRL_LPCMPHYSSELEM234H_SHIFT;
  if (lpcmpHystSel & ~_GENERIC_DCDCLPCTRL_LPCMPHYSSELEM234H_MASK)
  {
    EFM_ASSERT(false);
    /* Return when assertions are disabled */
    return false;
  }
  EMU->DCDCLPCTRL = (EMU->DCDCLPCTRL & ~_GENERIC_DCDCLPCTRL_LPCMPHYSSELEM234H_MASK) | lpcmpHystSel;

  return true;
}

/** @endcond */

/***************************************************************************//**
 * @brief
 *   Set DCDC regulator operating mode
 *
 * @param[in] dcdcMode
 *   DCDC mode
 ******************************************************************************/
void EMU_DCDCModeSet(EMU_DcdcMode_TypeDef dcdcMode)
{
  uint32_t currentDcdcMode = (EMU->DCDCCTRL & _EMU_DCDCCTRL_DCDCMODE_MASK);

  if ((EMU_DcdcMode_TypeDef)currentDcdcMode == dcdcMode)
  {
    /* Mode already set - do nothing */
    return;
  }

#if defined(_SILICON_LABS_32B_PLATFORM_2_GEN_1)

  while(EMU->DCDCSYNC & EMU_DCDCSYNC_DCDCCTRLBUSY);
  /* Configure bypass current limiter */
  BUS_RegBitWrite(EMU_DCDCCLIMCTRL, _EMU_DCDCCLIMCTRL_BYPLIMEN_SHIFT, dcdcMode == emuDcdcMode_Bypass ? 0 : 1);

  /* Fix for errata DCDC_E203 */
  if (((EMU_DcdcMode_TypeDef)currentDcdcMode == emuDcdcMode_Bypass)
      && (dcdcMode == emuDcdcMode_LowNoise))
  {
    errataFixDcdcHsState = errataFixDcdcHsBypassLn;
  }

#elif defined(_SILICON_LABS_32B_PLATFORM_2_GEN_2)

  if (((currentDcdcMode == EMU_DCDCCTRL_DCDCMODE_OFF) || (currentDcdcMode == EMU_DCDCCTRL_DCDCMODE_BYPASS))
       && ((dcdcMode == emuDcdcMode_LowPower) || (dcdcMode == emuDcdcMode_LowNoise)))
  {
    /* Always start in LOWNOISE mode and then switch to LOWPOWER mode once LOWNOISE startup is complete. */
    EMU_IntClear(EMU_IFC_DCDCLNRUNNING);
    while(EMU->DCDCSYNC & EMU_DCDCSYNC_DCDCCTRLBUSY);
    EMU->DCDCCTRL = (EMU->DCDCCTRL & ~_EMU_DCDCCTRL_DCDCMODE_MASK) | EMU_DCDCCTRL_DCDCMODE_LOWNOISE;
    while(!(EMU_IntGet() & EMU_IF_DCDCLNRUNNING));
  }
  else if (dcdcMode == emuDcdcMode_Bypass)
  {
    /* Enable limiter to remove current peak. Disable again in EMU_EnterEM2/3/4 */
    while(EMU->DCDCSYNC & EMU_DCDCSYNC_DCDCCTRLBUSY);
    BUS_RegBitWrite(EMU_DCDCCLIMCTRL, _EMU_DCDCCLIMCTRL_BYPLIMEN_SHIFT, 1);
  }
#else
#error "DCDC mode handling is undefined for this family."
#endif

  /* Set user requested mode. */
  EMU->DCDCCTRL = (EMU->DCDCCTRL & ~_EMU_DCDCCTRL_DCDCMODE_MASK) | dcdcMode;
}


/***************************************************************************//**
 * @brief
 *   Configure DCDC regulator
 *
 * @note
 * If the power circuit is configured for NODCDC as described in Section
 * 11.3.4.3 of the Reference Manual, do not call this function. Instead call
 * EMU_DCDCPowerOff().
 *
 * @param[in] dcdcInit
 *   DCDC initialization structure
 *
 * @return
 *   True if initialization parameters are valid
 ******************************************************************************/
bool EMU_DCDCInit(EMU_DCDCInit_TypeDef *dcdcInit)
{
#if defined(_EMU_DCDCLPEM01CFG_LPCMPBIASEM01_MASK)
  uint32_t lpCmpBiasSelEM01;
#endif
  uint32_t lpCmpBiasSelEM234H;

  /* Set external power configuration. This enables writing to the other
     DCDC registers. */
  EMU->PWRCFG = dcdcInit->powerConfig;

  /* EMU->PWRCFG is write-once and POR reset only. Check that
     we could set the desired power configuration. */
  if ((EMU->PWRCFG & _EMU_PWRCFG_PWRCFG_MASK) != dcdcInit->powerConfig)
  {
    /* If this assert triggers unexpectedly, please power cycle the
       kit to reset the power configuration. */
    EFM_ASSERT(false);
    /* Return when assertions are disabled */
    return false;
  }

  /* Load DCDC calibration data from the DI page */
  ConstCalibrationLoad();

  /* Check current parameters */
  EFM_ASSERT(dcdcInit->maxCurrent_mA <= 200);
  EFM_ASSERT(dcdcInit->em01LoadCurrent_mA <= dcdcInit->maxCurrent_mA);
  EFM_ASSERT(dcdcInit->reverseCurrentControl <= 200);

  /* DCDC low-noise supports max 200mA */
  if (dcdcInit->dcdcMode == emuDcdcMode_LowNoise)
  {
    EFM_ASSERT(dcdcInit->em01LoadCurrent_mA <= 200);
  }
#if (_SILICON_LABS_32B_PLATFORM_2_GEN > 1)
  else if (dcdcInit->dcdcMode == emuDcdcMode_LowPower)
  {
    EFM_ASSERT(dcdcInit->em01LoadCurrent_mA <= 10);
  }
#endif

  /* EM2/3/4 current above 10mA is not supported */
  EFM_ASSERT(dcdcInit->em234LoadCurrent_uA <= 10000);

  /* Decode LP comparator bias for EM0/1 and EM2/3 */
#if defined(_EMU_DCDCLPEM01CFG_LPCMPBIASEM01_MASK)
  lpCmpBiasSelEM01 = EMU_DCDCLPEM01CFG_LPCMPBIASEM01_BIAS3;
  if (dcdcInit->dcdcMode == emuDcdcMode_LowPower)
  {
    if (dcdcInit->em01LoadCurrent_mA <= 1)
    {
      lpCmpBiasSelEM01 = EMU_DCDCLPEM01CFG_LPCMPBIASEM01_BIAS1;
    }
    else if (dcdcInit->em01LoadCurrent_mA <= 3)
    {
      lpCmpBiasSelEM01 = EMU_DCDCLPEM01CFG_LPCMPBIASEM01_BIAS2;
    }
  }
#endif

  if (dcdcInit->em234LoadCurrent_uA < 75)
  {
    lpCmpBiasSelEM234H  = 0;
  }
  else if (dcdcInit->em234LoadCurrent_uA < 500)
  {
    lpCmpBiasSelEM234H  = 1 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT;
  }
  else if (dcdcInit->em234LoadCurrent_uA < 2500)
  {
    lpCmpBiasSelEM234H  = 2 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT;
  }
  else
  {
    lpCmpBiasSelEM234H  = 3 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT;
  }

  /* ==== THESE NEXT STEPS ARE STRONGLY ORDER DEPENDENT ==== */

  /* Set DCDC low-power mode comparator bias selection */

  /* 1. Set DCDC low-power mode comparator bias selection and forced CCM
        => Updates DCDCMISCCTRL_LNFORCECCM */
  EMU->DCDCMISCCTRL = (EMU->DCDCMISCCTRL & ~(_GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_MASK
                                             | _EMU_DCDCMISCCTRL_LNFORCECCM_MASK))
                       | ((uint32_t)lpCmpBiasSelEM234H
                          | (dcdcInit->reverseCurrentControl >= 0 ?
                             EMU_DCDCMISCCTRL_LNFORCECCM : 0));
#if defined(_EMU_DCDCLPEM01CFG_LPCMPBIASEM01_MASK)
  EMU->DCDCLPEM01CFG = (EMU->DCDCLPEM01CFG & ~_EMU_DCDCLPEM01CFG_LPCMPBIASEM01_MASK)
                       | lpCmpBiasSelEM01;
#endif

  /* 2. Set recommended and validated current optimization settings
        <= Depends on LNFORCECCM
        => Updates DCDCLNFREQCTRL_RCOBAND */
  ValidatedConfigSet();

  /* 3. Updated static currents and limits user data.
        Limiters are updated in EMU_DCDCOptimizeSlice() */
  userCurrentLimitsSet(dcdcInit->maxCurrent_mA,
                       dcdcInit->reverseCurrentControl);
  dcdcEm01LoadCurrent_mA = dcdcInit->em01LoadCurrent_mA;

  /* 4. Optimize LN slice based on given user input load current
        <= Depends on DCDCMISCCTRL_LNFORCECCM and DCDCLNFREQCTRL_RCOBAND
        <= Depends on dcdcInit->maxCurrent_mA and dcdcInit->reverseCurrentControl
        => Updates DCDCMISCCTRL_P/NFETCNT
        => Updates DCDCMISCCTRL_LNCLIMILIMSEL and DCDCMISCCTRL_LPCLIMILIMSEL
        => Updates DCDCZDETCTRL_ZDETILIMSEL */
  EMU_DCDCOptimizeSlice(dcdcInit->em01LoadCurrent_mA);

  /* ======================================================= */

  /* Set DCDC low noise mode compensator control register. */
  compCtrlSet(dcdcInit->dcdcLnCompCtrl);

  /* Set DCDC output voltage */
  if (!EMU_DCDCOutputVoltageSet(dcdcInit->mVout, true, true))
  {
    EFM_ASSERT(false);
    /* Return when assertions are disabled */
    return false;
  }

#if ( _SILICON_LABS_32B_PLATFORM_2_GEN == 1 )
  /* Select analog peripheral power supply. This must be done before
     DCDC mode is set for GEN_1. */
  BUS_RegBitWrite(&EMU->PWRCTRL,
                  _EMU_PWRCTRL_ANASW_SHIFT,
                  dcdcInit->anaPeripheralPower ? 1 : 0);
#endif

  /* Set EM0 DCDC operating mode. Output voltage set in
     EMU_DCDCOutputVoltageSet() above takes effect if mode
     is changed from bypass/off mode. */
  EMU_DCDCModeSet(dcdcInit->dcdcMode);

  /* Select DVDD as input to the digital regulator */
#if defined(_EMU_PWRCTRL_REGPWRSEL_MASK)
  EMU->PWRCTRL |= EMU_PWRCTRL_REGPWRSEL_DVDD;
#endif

#if ( _SILICON_LABS_32B_PLATFORM_2_GEN > 1 )
  /* Select analog peripheral power supply. This must be done after
     DCDC mode is set for GEN > 1. */
  BUS_RegBitWrite(&EMU->PWRCTRL,
                  _EMU_PWRCTRL_ANASW_SHIFT,
                  dcdcInit->anaPeripheralPower ? 1 : 0);
#endif

  return true;
}


/***************************************************************************//**
 * @brief
 *   Set DCDC output voltage
 *
 * @param[in] mV
 *   Target DCDC output voltage in mV
 *
 * @return
 *   True if the mV parameter is valid
 ******************************************************************************/
bool EMU_DCDCOutputVoltageSet(uint32_t mV,
                              bool setLpVoltage,
                              bool setLnVoltage)
{
#if defined( _DEVINFO_DCDCLNVCTRL0_3V0LNATT1_MASK )

  bool validOutVoltage;
  uint8_t lnMode;
  bool attSet;
  uint32_t attMask;
  uint32_t vrefLow = 0;
  uint32_t vrefHigh = 0;
  uint32_t vrefVal = 0;
  uint32_t mVlow = 0;
  uint32_t mVhigh = 0;
  uint32_t vrefShift;
  uint32_t lpcmpBias;
  volatile uint32_t* ctrlReg;

  /* Check that the set voltage is within valid range.
     Voltages are obtained from the datasheet. */
  validOutVoltage = false;
  if ((EMU->PWRCFG & _EMU_PWRCFG_PWRCFG_MASK) == EMU_PWRCFG_PWRCFG_DCDCTODVDD)
  {
    validOutVoltage = ((mV >= PWRCFG_DCDCTODVDD_VMIN)
                       && (mV <= PWRCFG_DCDCTODVDD_VMAX));
  }

  if (!validOutVoltage)
  {
    EFM_ASSERT(false);
    /* Return when assertions are disabled */
    return false;
  }

  /* Populate both LP and LN registers, set control reg pointer and VREF shift. */
  for (lnMode = 0; lnMode <= 1; lnMode++)
  {
    if (((lnMode == 0) && !setLpVoltage)
        || ((lnMode == 1) && !setLnVoltage))
    {
      continue;
    }

    ctrlReg   = (lnMode ? &EMU->DCDCLNVCTRL : &EMU->DCDCLPVCTRL);
    vrefShift = (lnMode ? _EMU_DCDCLNVCTRL_LNVREF_SHIFT
                        : _EMU_DCDCLPVCTRL_LPVREF_SHIFT);

    /* Set attenuation to use */
    attSet = (mV > 1800);
    /* Always set mVlow different from mVhigh to avoid division by zero   */
    /* further down.                                                      */
    if (attSet)
    {
      mVlow = 1800;
      mVhigh = 3000;
      attMask = (lnMode ? EMU_DCDCLNVCTRL_LNATT : EMU_DCDCLPVCTRL_LPATT);
    }
    else
    {
      mVlow = 1200;
      mVhigh = 1800;
      attMask = 0;
    }

    /* Get 2-point calib data from DEVINFO, calculate trimming and set voltege */
    if (lnMode)
    {
      /* Set low-noise DCDC output voltage tuning */
      if (attSet)
      {
        vrefLow  = DEVINFO->DCDCLNVCTRL0;
        vrefHigh = (vrefLow & _DEVINFO_DCDCLNVCTRL0_3V0LNATT1_MASK)
                   >> _DEVINFO_DCDCLNVCTRL0_3V0LNATT1_SHIFT;
        vrefLow  = (vrefLow & _DEVINFO_DCDCLNVCTRL0_1V8LNATT1_MASK)
                   >> _DEVINFO_DCDCLNVCTRL0_1V8LNATT1_SHIFT;
      }
      else
      {
        vrefLow  = DEVINFO->DCDCLNVCTRL0;
        vrefHigh = (vrefLow & _DEVINFO_DCDCLNVCTRL0_1V8LNATT0_MASK)
                   >> _DEVINFO_DCDCLNVCTRL0_1V8LNATT0_SHIFT;
        vrefLow  = (vrefLow & _DEVINFO_DCDCLNVCTRL0_1V2LNATT0_MASK)
                   >> _DEVINFO_DCDCLNVCTRL0_1V2LNATT0_SHIFT;
      }
    }
    else
    {
      /* Set low-power DCDC output voltage tuning */

      /* Get LPCMPBIAS and make sure masks are not overlayed */
      lpcmpBias = EMU->DCDCMISCCTRL & _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_MASK;
      EFM_ASSERT(!(_GENERIC_DCDCLPCTRL_LPCMPHYSSELEM234H_MASK & attMask));
      switch (attMask | lpcmpBias)
      {
        case EMU_DCDCLPVCTRL_LPATT:
          vrefLow  = DEVINFO->DCDCLPVCTRL2;
          vrefHigh = (vrefLow & _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS0_MASK)
                     >> _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS0_SHIFT;
          vrefLow  = (vrefLow & _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS0_MASK)
                     >> _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS0_SHIFT;
          break;

        case EMU_DCDCLPVCTRL_LPATT | 1 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
          vrefLow  = DEVINFO->DCDCLPVCTRL2;
          vrefHigh = (vrefLow & _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS1_MASK)
                     >> _DEVINFO_DCDCLPVCTRL2_3V0LPATT1LPCMPBIAS1_SHIFT;
          vrefLow  = (vrefLow & _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS1_MASK)
                     >> _DEVINFO_DCDCLPVCTRL2_1V8LPATT1LPCMPBIAS1_SHIFT;
          break;

        case EMU_DCDCLPVCTRL_LPATT | 2 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
          vrefLow  = DEVINFO->DCDCLPVCTRL3;
          vrefHigh = (vrefLow & _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS2_MASK)
                     >> _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS2_SHIFT;
          vrefLow  = (vrefLow & _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS2_MASK)
                     >> _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS2_SHIFT;
          break;

        case EMU_DCDCLPVCTRL_LPATT | 3 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
          vrefLow  = DEVINFO->DCDCLPVCTRL3;
          vrefHigh = (vrefLow & _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS3_MASK)
                     >> _DEVINFO_DCDCLPVCTRL3_3V0LPATT1LPCMPBIAS3_SHIFT;
          vrefLow  = (vrefLow & _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS3_MASK)
                     >> _DEVINFO_DCDCLPVCTRL3_1V8LPATT1LPCMPBIAS3_SHIFT;
          break;

        case 0:
          vrefLow  = DEVINFO->DCDCLPVCTRL0;
          vrefHigh = (vrefLow & _DEVINFO_DCDCLPVCTRL0_1V8LPATT0LPCMPBIAS0_MASK)
                     >> _DEVINFO_DCDCLPVCTRL0_1V8LPATT0LPCMPBIAS0_SHIFT;
          vrefLow  = (vrefLow & _DEVINFO_DCDCLPVCTRL0_1V2LPATT0LPCMPBIAS0_MASK)
                     >> _DEVINFO_DCDCLPVCTRL0_1V2LPATT0LPCMPBIAS0_SHIFT;
          break;

        case 1 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
          vrefLow  = DEVINFO->DCDCLPVCTRL0;
          vrefHigh = (vrefLow & _DEVINFO_DCDCLPVCTRL0_1V8LPATT0LPCMPBIAS1_MASK)
                     >> _DEVINFO_DCDCLPVCTRL0_1V8LPATT0LPCMPBIAS1_SHIFT;
          vrefLow  = (vrefLow & _DEVINFO_DCDCLPVCTRL0_1V2LPATT0LPCMPBIAS1_MASK)
                     >> _DEVINFO_DCDCLPVCTRL0_1V2LPATT0LPCMPBIAS1_SHIFT;
          break;

        case 2 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
          vrefLow  = DEVINFO->DCDCLPVCTRL1;
          vrefHigh = (vrefLow & _DEVINFO_DCDCLPVCTRL1_1V8LPATT0LPCMPBIAS2_MASK)
                     >> _DEVINFO_DCDCLPVCTRL1_1V8LPATT0LPCMPBIAS2_SHIFT;
          vrefLow  = (vrefLow & _DEVINFO_DCDCLPVCTRL1_1V2LPATT0LPCMPBIAS2_MASK)
                     >> _DEVINFO_DCDCLPVCTRL1_1V2LPATT0LPCMPBIAS2_SHIFT;
          break;

        case 3 << _GENERIC_DCDCMISCCTRL_LPCMPBIASEM234H_SHIFT:
          vrefLow  = DEVINFO->DCDCLPVCTRL1;
          vrefHigh = (vrefLow & _DEVINFO_DCDCLPVCTRL1_1V8LPATT0LPCMPBIAS3_MASK)
                     >> _DEVINFO_DCDCLPVCTRL1_1V8LPATT0LPCMPBIAS3_SHIFT;
          vrefLow  = (vrefLow & _DEVINFO_DCDCLPVCTRL1_1V2LPATT0LPCMPBIAS3_MASK)
                     >> _DEVINFO_DCDCLPVCTRL1_1V2LPATT0LPCMPBIAS3_SHIFT;
          break;

        default:
          EFM_ASSERT(false);
          break;
      }

      /* Load LP comparator hysteresis calibration */
      if(!(LpCmpHystCalibrationLoad(attSet, lpcmpBias)))
      {
        EFM_ASSERT(false);
        /* Return when assertions are disabled */
        return false;
      }
    } /* Low-nise / low-power mode */


    /* Check for valid 2-point trim values */
    if (mVlow >= mVhigh)
    {
      EFM_ASSERT(false);
      /* Return when assertions are disabled */
      return false;
    }

    /* Calculate and set voltage trim */
    vrefVal = ((mV - mVlow) * (vrefHigh - vrefLow))  / (mVhigh - mVlow);
    vrefVal += vrefLow;

    /* Range check */
    if ((vrefVal > vrefHigh) || (vrefVal < vrefLow))
    {
      EFM_ASSERT(false);
      /* Return when assertions are disabled */
      return false;
    }

    /* Update DCDCLNVCTRL/DCDCLPVCTRL */
    *ctrlReg = (vrefVal << vrefShift) | attMask;
  }
#endif
  return true;
}


/***************************************************************************//**
 * @brief
 *   Optimize DCDC slice count based on the estimated average load current
 *   in EM0
 *
 * @param[in] em0LoadCurrent_mA
 *   Estimated average EM0 load current in mA.
 ******************************************************************************/
void EMU_DCDCOptimizeSlice(uint32_t em0LoadCurrent_mA)
{
  uint32_t sliceCount = 0;
  uint32_t rcoBand = (EMU->DCDCLNFREQCTRL & _EMU_DCDCLNFREQCTRL_RCOBAND_MASK)
                      >> _EMU_DCDCLNFREQCTRL_RCOBAND_SHIFT;

  /* Set recommended slice count */
  if ((EMU->DCDCMISCCTRL & _EMU_DCDCMISCCTRL_LNFORCECCM_MASK) && (rcoBand >= emuDcdcLnRcoBand_5MHz))
  {
    if (em0LoadCurrent_mA < 20)
    {
      sliceCount = 4;
    }
    else if ((em0LoadCurrent_mA >= 20) && (em0LoadCurrent_mA < 40))
    {
      sliceCount = 8;
    }
    else
    {
      sliceCount = 16;
    }
  }
  else if ((!(EMU->DCDCMISCCTRL & _EMU_DCDCMISCCTRL_LNFORCECCM_MASK)) && (rcoBand <= emuDcdcLnRcoBand_4MHz))
  {
    if (em0LoadCurrent_mA < 10)
    {
      sliceCount = 4;
    }
    else if ((em0LoadCurrent_mA >= 10) && (em0LoadCurrent_mA < 20))
    {
      sliceCount = 8;
    }
    else
    {
      sliceCount = 16;
    }
  }
  else if ((EMU->DCDCMISCCTRL & _EMU_DCDCMISCCTRL_LNFORCECCM_MASK) && (rcoBand <= emuDcdcLnRcoBand_4MHz))
  {
    if (em0LoadCurrent_mA < 40)
    {
      sliceCount = 8;
    }
    else
    {
      sliceCount = 16;
    }
  }
  else
  {
    /* This configuration is not recommended. EMU_DCDCInit() applies a recommended
       configuration. */
    EFM_ASSERT(false);
  }

  /* The selected slices are PSLICESEL + 1 */
  sliceCount--;

  /* Apply slice count to both N and P slice */
  sliceCount = (sliceCount << _EMU_DCDCMISCCTRL_PFETCNT_SHIFT
                | sliceCount << _EMU_DCDCMISCCTRL_NFETCNT_SHIFT);
  EMU->DCDCMISCCTRL = (EMU->DCDCMISCCTRL & ~(_EMU_DCDCMISCCTRL_PFETCNT_MASK
                                             | _EMU_DCDCMISCCTRL_NFETCNT_MASK))
                      | sliceCount;

  /* Update current limiters */
  currentLimitersUpdate();
}

/***************************************************************************//**
 * @brief
 *   Set DCDC Low-noise RCO band.
 *
 * @param[in] band
 *   RCO band to set.
 ******************************************************************************/
void EMU_DCDCLnRcoBandSet(EMU_DcdcLnRcoBand_TypeDef band)
{
  uint32_t forcedCcm;
  forcedCcm = BUS_RegBitRead(&EMU->DCDCMISCCTRL, _EMU_DCDCMISCCTRL_LNFORCECCM_SHIFT);

  /* DCM mode supports up to 4MHz LN RCO. */
  EFM_ASSERT((!forcedCcm && band <= emuDcdcLnRcoBand_4MHz) || forcedCcm);

  EMU->DCDCLNFREQCTRL = (EMU->DCDCLNFREQCTRL & ~_EMU_DCDCLNFREQCTRL_RCOBAND_MASK)
                         | (band << _EMU_DCDCLNFREQCTRL_RCOBAND_SHIFT);

  /* Update slice configuration as this depends on the RCO band. */
  EMU_DCDCOptimizeSlice(dcdcEm01LoadCurrent_mA);
}

/***************************************************************************//**
 * @brief
 *   Power off the DCDC regulator.
 *
 * @details
 *   This function powers off the DCDC controller. This function should only be
 *   used if the external power circuit is wired for no DCDC. If the external power
 *   circuit is wired for DCDC usage, then use EMU_DCDCInit() and set the
 *   DCDC in bypass mode to disable DCDC.
 *
 * @return
 *   Return false if the DCDC could not be disabled.
 ******************************************************************************/
bool EMU_DCDCPowerOff(void)
{
  bool dcdcModeSet;

  /* Set DCDCTODVDD only to enable write access to EMU->DCDCCTRL */
  EMU->PWRCFG = EMU_PWRCFG_PWRCFG_DCDCTODVDD;

  /* Select DVDD as input to the digital regulator */
#if defined(_EMU_PWRCTRL_REGPWRSEL_MASK)
  EMU->PWRCTRL |= EMU_PWRCTRL_REGPWRSEL_DVDD;
#endif

  /* Set DCDC to OFF and disable LP in EM2/3/4. Verify that the required
     mode could be set. */
  while(EMU->DCDCSYNC & EMU_DCDCSYNC_DCDCCTRLBUSY);
  EMU->DCDCCTRL = EMU_DCDCCTRL_DCDCMODE_OFF;

  dcdcModeSet = (EMU->DCDCCTRL == EMU_DCDCCTRL_DCDCMODE_OFF);
  EFM_ASSERT(dcdcModeSet);

  return dcdcModeSet;
}
#endif


#if defined( EMU_STATUS_VMONRDY )
/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
__STATIC_INLINE uint32_t vmonMilliVoltToCoarseThreshold(int mV)
{
  return (mV - 1200) / 200;
}

__STATIC_INLINE uint32_t vmonMilliVoltToFineThreshold(int mV, uint32_t coarseThreshold)
{
  return (mV - 1200 - (coarseThreshold * 200)) / 20;
}
/** @endcond */

/***************************************************************************//**
 * @brief
 *   Initialize VMON channel.
 *
 * @details
 *   Initialize a VMON channel without hysteresis. If the channel supports
 *   separate rise and fall triggers, both thresholds will be set to the same
 *   value.
 *
 * @param[in] vmonInit
 *   VMON initialization struct
 ******************************************************************************/
void EMU_VmonInit(EMU_VmonInit_TypeDef *vmonInit)
{
  uint32_t thresholdCoarse, thresholdFine;
  EFM_ASSERT((vmonInit->threshold >= 1200) && (vmonInit->threshold <= 3980));

  thresholdCoarse = vmonMilliVoltToCoarseThreshold(vmonInit->threshold);
  thresholdFine = vmonMilliVoltToFineThreshold(vmonInit->threshold, thresholdCoarse);

  switch(vmonInit->channel)
  {
  case emuVmonChannel_AVDD:
    EMU->VMONAVDDCTRL = (thresholdCoarse << _EMU_VMONAVDDCTRL_RISETHRESCOARSE_SHIFT)
                      | (thresholdFine << _EMU_VMONAVDDCTRL_RISETHRESFINE_SHIFT)
                      | (thresholdCoarse << _EMU_VMONAVDDCTRL_FALLTHRESCOARSE_SHIFT)
                      | (thresholdFine << _EMU_VMONAVDDCTRL_FALLTHRESFINE_SHIFT)
                      | (vmonInit->riseWakeup ? EMU_VMONAVDDCTRL_RISEWU : 0)
                      | (vmonInit->fallWakeup ? EMU_VMONAVDDCTRL_FALLWU : 0)
                      | (vmonInit->enable     ? EMU_VMONAVDDCTRL_EN     : 0);
    break;
  case emuVmonChannel_ALTAVDD:
    EMU->VMONALTAVDDCTRL = (thresholdCoarse << _EMU_VMONALTAVDDCTRL_THRESCOARSE_SHIFT)
                         | (thresholdFine << _EMU_VMONALTAVDDCTRL_THRESFINE_SHIFT)
                         | (vmonInit->riseWakeup ? EMU_VMONALTAVDDCTRL_RISEWU : 0)
                         | (vmonInit->fallWakeup ? EMU_VMONALTAVDDCTRL_FALLWU : 0)
                         | (vmonInit->enable     ? EMU_VMONALTAVDDCTRL_EN     : 0);
    break;
  case emuVmonChannel_DVDD:
    EMU->VMONDVDDCTRL = (thresholdCoarse << _EMU_VMONDVDDCTRL_THRESCOARSE_SHIFT)
                      | (thresholdFine << _EMU_VMONDVDDCTRL_THRESFINE_SHIFT)
                      | (vmonInit->riseWakeup ? EMU_VMONDVDDCTRL_RISEWU : 0)
                      | (vmonInit->fallWakeup ? EMU_VMONDVDDCTRL_FALLWU : 0)
                      | (vmonInit->enable     ? EMU_VMONDVDDCTRL_EN     : 0);
    break;
  case emuVmonChannel_IOVDD0:
    EMU->VMONIO0CTRL = (thresholdCoarse << _EMU_VMONIO0CTRL_THRESCOARSE_SHIFT)
                     | (thresholdFine << _EMU_VMONIO0CTRL_THRESFINE_SHIFT)
                     | (vmonInit->retDisable ? EMU_VMONIO0CTRL_RETDIS : 0)
                     | (vmonInit->riseWakeup ? EMU_VMONIO0CTRL_RISEWU : 0)
                     | (vmonInit->fallWakeup ? EMU_VMONIO0CTRL_FALLWU : 0)
                     | (vmonInit->enable     ? EMU_VMONIO0CTRL_EN     : 0);
    break;
  default:
    EFM_ASSERT(false);
    return;
  }
}

/***************************************************************************//**
 * @brief
 *   Initialize VMON channel with hysteresis (separate rise and fall triggers).
 *
 * @details
 *   Initialize a VMON channel which supports hysteresis. The AVDD channel is
 *   the only channel to support separate rise and fall triggers.
 *
 * @param[in] vmonInit
 *   VMON Hysteresis initialization struct
 ******************************************************************************/
void EMU_VmonHystInit(EMU_VmonHystInit_TypeDef *vmonInit)
{
  uint32_t riseThresholdCoarse, riseThresholdFine, fallThresholdCoarse, fallThresholdFine;
  /* VMON supports voltages between 1200 mV and 3980 mV (inclusive) in 20 mV increments */
  EFM_ASSERT((vmonInit->riseThreshold >= 1200) && (vmonInit->riseThreshold < 4000));
  EFM_ASSERT((vmonInit->fallThreshold >= 1200) && (vmonInit->fallThreshold < 4000));
  /* Fall threshold has to be lower than rise threshold */
  EFM_ASSERT(vmonInit->fallThreshold <= vmonInit->riseThreshold);

  riseThresholdCoarse = vmonMilliVoltToCoarseThreshold(vmonInit->riseThreshold);
  riseThresholdFine = vmonMilliVoltToFineThreshold(vmonInit->riseThreshold, riseThresholdCoarse);
  fallThresholdCoarse = vmonMilliVoltToCoarseThreshold(vmonInit->fallThreshold);
  fallThresholdFine = vmonMilliVoltToFineThreshold(vmonInit->fallThreshold, fallThresholdCoarse);

  switch(vmonInit->channel)
  {
  case emuVmonChannel_AVDD:
    EMU->VMONAVDDCTRL = (riseThresholdCoarse << _EMU_VMONAVDDCTRL_RISETHRESCOARSE_SHIFT)
                      | (riseThresholdFine << _EMU_VMONAVDDCTRL_RISETHRESFINE_SHIFT)
                      | (fallThresholdCoarse << _EMU_VMONAVDDCTRL_FALLTHRESCOARSE_SHIFT)
                      | (fallThresholdFine << _EMU_VMONAVDDCTRL_FALLTHRESFINE_SHIFT)
                      | (vmonInit->riseWakeup ? EMU_VMONAVDDCTRL_RISEWU : 0)
                      | (vmonInit->fallWakeup ? EMU_VMONAVDDCTRL_FALLWU : 0)
                      | (vmonInit->enable     ? EMU_VMONAVDDCTRL_EN     : 0);
    break;
  default:
    EFM_ASSERT(false);
    return;
  }
}

/***************************************************************************//**
 * @brief
 *   Enable or disable a VMON channel
 *
 * @param[in] channel
 *   VMON channel to enable/disable
 *
 * @param[in] enable
 *   Whether to enable or disable
 ******************************************************************************/
void EMU_VmonEnable(EMU_VmonChannel_TypeDef channel, bool enable)
{
  uint32_t volatile * reg;
  uint32_t bit;

  switch(channel)
  {
  case emuVmonChannel_AVDD:
    reg = &(EMU->VMONAVDDCTRL);
    bit = _EMU_VMONAVDDCTRL_EN_SHIFT;
    break;
  case emuVmonChannel_ALTAVDD:
    reg = &(EMU->VMONALTAVDDCTRL);
    bit = _EMU_VMONALTAVDDCTRL_EN_SHIFT;
    break;
  case emuVmonChannel_DVDD:
    reg = &(EMU->VMONDVDDCTRL);
    bit = _EMU_VMONDVDDCTRL_EN_SHIFT;
    break;
  case emuVmonChannel_IOVDD0:
    reg = &(EMU->VMONIO0CTRL);
    bit = _EMU_VMONIO0CTRL_EN_SHIFT;
    break;
  default:
    EFM_ASSERT(false);
    return;
  }

  BUS_RegBitWrite(reg, bit, enable);
}

/***************************************************************************//**
 * @brief
 *   Get the status of a voltage monitor channel.
 *
 * @param[in] channel
 *   VMON channel to get status for
 *
 * @return
 *   Status of the selected VMON channel. True if channel is triggered.
 ******************************************************************************/
bool EMU_VmonChannelStatusGet(EMU_VmonChannel_TypeDef channel)
{
  uint32_t bit;
  switch(channel)
  {
  case emuVmonChannel_AVDD:
    bit = _EMU_STATUS_VMONAVDD_SHIFT;
    break;
  case emuVmonChannel_ALTAVDD:
    bit = _EMU_STATUS_VMONALTAVDD_SHIFT;
    break;
  case emuVmonChannel_DVDD:
    bit = _EMU_STATUS_VMONDVDD_SHIFT;
    break;
  case emuVmonChannel_IOVDD0:
    bit = _EMU_STATUS_VMONIO0_SHIFT;
    break;
  default:
    EFM_ASSERT(false);
    bit = 0;
  }

  return BUS_RegBitRead(&EMU->STATUS, bit);
}
#endif /* EMU_STATUS_VMONRDY */

#if defined( _SILICON_LABS_32B_PLATFORM_2_GEN_1 )
/**
 * @brief
 *   Adjust the bias refresh rate
 *
 * @details
 *   This function is only meant to be used under high-temperature operation on
 *   the first generation EFR32, Pearl and Jade. Adjusting the bias mode will
 *   increase the typical current consumption. See application note 1027
 *   and errata documents for further details.
 *
 * @param [in] mode
 *   The new bias refresh rate
 */
void EMU_SetBiasMode(EMU_BiasMode_TypeDef mode)
{
#define EMU_TESTLOCK         (*(volatile uint32_t *) (EMU_BASE + 0x190))
#define EMU_BIASCONF         (*(volatile uint32_t *) (EMU_BASE + 0x164))
#define EMU_BIASTESTCTRL     (*(volatile uint32_t *) (EMU_BASE + 0x19C))
#define CMU_ULFRCOCTRL       (*(volatile uint32_t *) (CMU_BASE + 0x03C))

  uint32_t freq = 0x2;
  bool emuTestLocked = false;

  if (mode == emuBiasMode_1KHz)
  {
    freq = 0x0;
  }

  if (EMU_TESTLOCK == 0x1)
  {
    emuTestLocked = true;
    EMU_TESTLOCK = 0xADE8;
  }

  if (mode == emuBiasMode_Continuous)
  {
    EMU_BIASCONF &= ~0x74;
  }
  else
  {
    EMU_BIASCONF |= 0x74;
  }

  EMU_BIASTESTCTRL |= 0x8;
  CMU_ULFRCOCTRL    = (CMU_ULFRCOCTRL & ~0xC00)
                      | ((freq & 0x3) << 10);
  EMU_BIASTESTCTRL &= ~0x8;

  if (emuTestLocked)
  {
    EMU_TESTLOCK = 0;
  }
}
#endif

/** @} (end addtogroup EMU) */
/** @} (end addtogroup emlib) */
#endif /* __EM_EMU_H */
