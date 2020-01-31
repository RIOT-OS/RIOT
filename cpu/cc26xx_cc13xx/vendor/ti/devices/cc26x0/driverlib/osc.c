/******************************************************************************
*  Filename:       osc.c
*  Revised:        2019-02-14 09:35:31 +0100 (Thu, 14 Feb 2019)
*  Revision:       54539
*
*  Description:    Driver for setting up the system Oscillators
*
*  Copyright (c) 2015 - 2017, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#include "../inc/hw_types.h"
#include "../inc/hw_ccfg.h"
#include "../inc/hw_fcfg1.h"
#include "aon_batmon.h"
#include "aon_rtc.h"
#include "osc.h"
#include "setup_rom.h"

//*****************************************************************************
//
// Handle support for DriverLib in ROM:
// This section will undo prototype renaming made in the header file
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #undef  OSCClockSourceSet
    #define OSCClockSourceSet               NOROM_OSCClockSourceSet
    #undef  OSCClockSourceGet
    #define OSCClockSourceGet               NOROM_OSCClockSourceGet
    #undef  OSCHF_GetStartupTime
    #define OSCHF_GetStartupTime            NOROM_OSCHF_GetStartupTime
    #undef  OSCHF_TurnOnXosc
    #define OSCHF_TurnOnXosc                NOROM_OSCHF_TurnOnXosc
    #undef  OSCHF_AttemptToSwitchToXosc
    #define OSCHF_AttemptToSwitchToXosc     NOROM_OSCHF_AttemptToSwitchToXosc
    #undef  OSCHF_SwitchToRcOscTurnOffXosc
    #define OSCHF_SwitchToRcOscTurnOffXosc  NOROM_OSCHF_SwitchToRcOscTurnOffXosc
    #undef  OSCHF_DebugGetCrystalAmplitude
    #define OSCHF_DebugGetCrystalAmplitude  NOROM_OSCHF_DebugGetCrystalAmplitude
    #undef  OSCHF_DebugGetExpectedAverageCrystalAmplitude
    #define OSCHF_DebugGetExpectedAverageCrystalAmplitude NOROM_OSCHF_DebugGetExpectedAverageCrystalAmplitude
    #undef  OSC_HPOSCRelativeFrequencyOffsetGet
    #define OSC_HPOSCRelativeFrequencyOffsetGet NOROM_OSC_HPOSCRelativeFrequencyOffsetGet
    #undef  OSC_AdjustXoscHfCapArray
    #define OSC_AdjustXoscHfCapArray        NOROM_OSC_AdjustXoscHfCapArray
    #undef  OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert
    #define OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert NOROM_OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert
#endif


//*****************************************************************************
//
// OSCHF switch time calculator defines and globals
//
//*****************************************************************************

#define RTC_CV_TO_MS(x) ((    1000 * ( x )) >> 16 )
#define RTC_CV_TO_US(x) (( 1000000 * ( x )) >> 16 )

typedef struct {
   uint32_t    previousStartupTimeInUs ;
   uint32_t    timeXoscOff_CV          ;
   uint32_t    timeXoscOn_CV           ;
   uint32_t    timeXoscStable_CV       ;
   int32_t     tempXoscOff             ;
} OscHfGlobals_t;

static OscHfGlobals_t oscHfGlobals;

//*****************************************************************************
//
//  Configure the oscillator input to the a source clock.
//
//*****************************************************************************
void
OSCClockSourceSet(uint32_t ui32SrcClk, uint32_t ui32Osc)
{
    // Check the arguments.
    ASSERT((ui32SrcClk & OSC_SRC_CLK_LF) ||
           (ui32SrcClk & OSC_SRC_CLK_MF) ||
           (ui32SrcClk & OSC_SRC_CLK_HF));
    ASSERT((ui32Osc == OSC_RCOSC_HF) ||
           (ui32Osc == OSC_RCOSC_LF) ||
           (ui32Osc == OSC_XOSC_HF) ||
           (ui32Osc == OSC_XOSC_LF));

    // Request the high frequency source clock (using 24 MHz XTAL)
    if(ui32SrcClk & OSC_SRC_CLK_HF)
    {
        // Enable the HF XTAL as HF clock source
        DDI16BitfieldWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_CTL0,
                           DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_M,
                           DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_S,
                           ui32Osc);
    }

    // Configure the medium frequency source clock
    if(ui32SrcClk & OSC_SRC_CLK_MF)
    {
        DDI16BitfieldWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_CTL0,
                           DDI_0_OSC_CTL0_SCLK_MF_SRC_SEL_M,
                           DDI_0_OSC_CTL0_SCLK_MF_SRC_SEL_S,
                           ui32Osc);
    }

    // Configure the low frequency source clock.
    if(ui32SrcClk & OSC_SRC_CLK_LF)
    {
        // Change the clock source.
        DDI16BitfieldWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_CTL0,
                           DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_M,
                           DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_S,
                           ui32Osc);
    }
}

//*****************************************************************************
//
//  Get the source clock settings
//
//*****************************************************************************
uint32_t
OSCClockSourceGet(uint32_t ui32SrcClk)
{
    uint32_t ui32ClockSource;

    // Check the arguments.
    ASSERT((ui32SrcClk & OSC_SRC_CLK_LF) ||
           (ui32SrcClk & OSC_SRC_CLK_HF));

    // Return the source for the selected clock.
    if(ui32SrcClk == OSC_SRC_CLK_LF)
    {
        ui32ClockSource = DDI16BitfieldRead(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_STAT0,
                                            DDI_0_OSC_STAT0_SCLK_LF_SRC_M,
                                            DDI_0_OSC_STAT0_SCLK_LF_SRC_S);
    }
    else
    {
        ui32ClockSource = DDI16BitfieldRead(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_STAT0,
                                            DDI_0_OSC_STAT0_SCLK_HF_SRC_M,
                                            DDI_0_OSC_STAT0_SCLK_HF_SRC_S);
    }
    return (ui32ClockSource);
}

//*****************************************************************************
//
// Returns maximum startup time (in microseconds) of XOSC_HF
//
//*****************************************************************************
uint32_t
OSCHF_GetStartupTime( uint32_t timeUntilWakeupInMs )
{
   uint32_t deltaTimeSinceXoscOnInMs   ;
   int32_t  deltaTempSinceXoscOn       ;
   uint32_t newStartupTimeInUs         ;

   deltaTimeSinceXoscOnInMs = RTC_CV_TO_MS( AONRTCCurrentCompareValueGet() - oscHfGlobals.timeXoscOn_CV );
   deltaTempSinceXoscOn     = AONBatMonTemperatureGetDegC() - oscHfGlobals.tempXoscOff;

   if ( deltaTempSinceXoscOn < 0 ) {
      deltaTempSinceXoscOn = -deltaTempSinceXoscOn;
   }

   if (  (( timeUntilWakeupInMs + deltaTimeSinceXoscOnInMs )     > 3000 ) ||
         ( deltaTempSinceXoscOn                                  >    5 ) ||
         ( oscHfGlobals.timeXoscStable_CV < oscHfGlobals.timeXoscOn_CV  ) ||
         ( oscHfGlobals.previousStartupTimeInUs                  ==   0 )    )
   {
      newStartupTimeInUs = 2000;
      if (( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_M ) == 0 ) {
         newStartupTimeInUs = (( HWREG( CCFG_BASE + CCFG_O_MODE_CONF_1 ) &
            CCFG_MODE_CONF_1_XOSC_MAX_START_M ) >>
            CCFG_MODE_CONF_1_XOSC_MAX_START_S ) * 125;
            // Note: CCFG startup time is "in units of 100us" adding 25% margin results in *125
      }
   } else {
      newStartupTimeInUs = RTC_CV_TO_US( oscHfGlobals.timeXoscStable_CV - oscHfGlobals.timeXoscOn_CV );
      newStartupTimeInUs += ( newStartupTimeInUs >> 2 ); // Add 25 percent margin
      if ( newStartupTimeInUs < oscHfGlobals.previousStartupTimeInUs ) {
         newStartupTimeInUs = oscHfGlobals.previousStartupTimeInUs;
      }
   }

   if ( newStartupTimeInUs < 200 ) {
      newStartupTimeInUs = 200;
   }
   if ( newStartupTimeInUs > 4000 ) {
      newStartupTimeInUs = 4000;
   }
   return ( newStartupTimeInUs );
}


//*****************************************************************************
//
// Turns on XOSC_HF (but without switching to XOSC_HF)
//
//*****************************************************************************
void
OSCHF_TurnOnXosc( void )
{
#if ( defined( ROM_OSCClockSourceSet ))
   ROM_OSCClockSourceSet( OSC_SRC_CLK_HF | OSC_SRC_CLK_MF, OSC_XOSC_HF );
#else
   OSCClockSourceSet( OSC_SRC_CLK_HF | OSC_SRC_CLK_MF, OSC_XOSC_HF );
#endif
   oscHfGlobals.timeXoscOn_CV  = AONRTCCurrentCompareValueGet();
}


//*****************************************************************************
//
// Switch to XOSC_HF if XOSC_HF is ready.
//
//*****************************************************************************
bool
OSCHF_AttemptToSwitchToXosc( void )
{
   uint32_t startupTimeInUs;
   uint32_t prevLimmit25InUs;

#if ( defined( ROM_OSCClockSourceGet ))
   if ( ROM_OSCClockSourceGet( OSC_SRC_CLK_HF ) == OSC_XOSC_HF )
#else
   if ( OSCClockSourceGet( OSC_SRC_CLK_HF ) == OSC_XOSC_HF )
#endif
   {
      // Already on XOSC - nothing to do
      return ( 1 );
   }
   if ( OSCHfSourceReady()) {
      OSCHfSourceSwitch();

      // Store startup time, but limit to 25 percent reduction each time.
      oscHfGlobals.timeXoscStable_CV  = AONRTCCurrentCompareValueGet();
      startupTimeInUs   = RTC_CV_TO_US( oscHfGlobals.timeXoscStable_CV - oscHfGlobals.timeXoscOn_CV );
      prevLimmit25InUs  = oscHfGlobals.previousStartupTimeInUs;
      prevLimmit25InUs -= ( prevLimmit25InUs >> 2 ); // 25 percent margin
      oscHfGlobals.previousStartupTimeInUs = startupTimeInUs;
      if ( prevLimmit25InUs > startupTimeInUs ) {
         oscHfGlobals.previousStartupTimeInUs = prevLimmit25InUs;
      }
      return ( 1 );
   }
   return ( 0 );
}


//*****************************************************************************
//
// Switch to RCOSC_HF and turn off XOSC_HF
//
//*****************************************************************************
void
OSCHF_SwitchToRcOscTurnOffXosc( void )
{
   // Set SCLK_HF and SCLK_MF to RCOSC_HF without checking
   // Doing this anyway to keep HF and MF in sync
#if ( defined( ROM_OSCClockSourceSet ))
   ROM_OSCClockSourceSet( OSC_SRC_CLK_HF | OSC_SRC_CLK_MF, OSC_RCOSC_HF );
#else
   OSCClockSourceSet( OSC_SRC_CLK_HF | OSC_SRC_CLK_MF, OSC_RCOSC_HF );
#endif

   // Do the switching if not already running on RCOSC_HF
#if ( defined( ROM_OSCClockSourceGet ))
   if ( ROM_OSCClockSourceGet( OSC_SRC_CLK_HF ) != OSC_RCOSC_HF )
#else
   if ( OSCClockSourceGet( OSC_SRC_CLK_HF ) != OSC_RCOSC_HF )
#endif
   {
      OSCHfSourceSwitch();
   }

   oscHfGlobals.timeXoscOff_CV  = AONRTCCurrentCompareValueGet();
   oscHfGlobals.tempXoscOff     = AONBatMonTemperatureGetDegC();
}

//*****************************************************************************
//
// Adjust the XOSC HF cap array relative to the factory setting
//
//*****************************************************************************
void
OSC_AdjustXoscHfCapArray( int32_t capArrDelta )
{
   // read the MODE_CONF register in CCFG
   uint32_t ccfg_ModeConfReg = HWREG( CCFG_BASE + CCFG_O_MODE_CONF );
   // Clear CAP_MODE and the CAPARRAY_DELATA field
   ccfg_ModeConfReg &= ~( CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_M | CCFG_MODE_CONF_XOSC_CAP_MOD_M );
   // Insert new delta value
   ccfg_ModeConfReg |= ((((uint32_t)capArrDelta) << CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_S ) & CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_M );
   // Update the HW register with the new delta value
   DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_ANABYPASSVAL1, SetupGetTrimForAnabypassValue1( ccfg_ModeConfReg ));
}

//*****************************************************************************
//
// Calculate the temperature dependent relative frequency offset of HPOSC
//
//*****************************************************************************
int32_t
OSC_HPOSCRelativeFrequencyOffsetGet( int32_t tempDegC )
{
   // Estimate HPOSC frequency, using temperature and curve fitting parameters

   uint32_t fitParams = HWREG( FCFG1_BASE + FCFG1_O_FREQ_OFFSET );
   // Extract the P0,P1,P2 params, and sign extend them via shifting up/down
   int32_t paramP0 = (((int32_t)( fitParams  << ( 32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P0_W - FCFG1_FREQ_OFFSET_HPOSC_COMP_P0_S )))
                                             >> ( 32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P0_W ));
   int32_t paramP1 = (((int32_t)( fitParams  << ( 32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P1_W - FCFG1_FREQ_OFFSET_HPOSC_COMP_P1_S )))
                                             >> ( 32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P1_W ));
   int32_t paramP2 = (((int32_t)( fitParams  << ( 32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P2_W - FCFG1_FREQ_OFFSET_HPOSC_COMP_P2_S )))
                                             >> ( 32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P2_W ));

   uint32_t fitParP3  = HWREG( FCFG1_BASE + FCFG1_O_MISC_CONF_2 );
   // Extract the P3 param, and sign extend via shifting up/down
   int32_t paramP3 = (((int32_t)( fitParP3   << ( 32 - FCFG1_MISC_CONF_2_HPOSC_COMP_P3_W - FCFG1_MISC_CONF_2_HPOSC_COMP_P3_S )))
                                             >> ( 32 - FCFG1_MISC_CONF_2_HPOSC_COMP_P3_W ));

   // Now we can find the HPOSC freq offset, given as a signed variable d, expressed by:
   //
   //    F_HPOSC = F_nom * (1 + d/(2^22))    , where: F_HPOSC = HPOSC frequency
   //                                                 F_nom = nominal clock source frequency (e.g. 48.000 MHz)
   //                                                 d = describes relative freq offset

   // We can estimate the d variable, using temperature compensation parameters:
   //
   //    d = P0 + P1*(t - T0) + P2*(t - T0)^2 + P3*(t - T0)^3, where: P0,P1,P2,P3 are curve fitting parameters from FCFG1
   //                                                 t = current temperature (from temp sensor) in deg C
   //                                                 T0 = 27 deg C (fixed temperature constant)
   int32_t tempDelta = (tempDegC - 27);
   int32_t tempDeltaX2 = tempDelta * tempDelta;
   int32_t d = paramP0 + ((tempDelta*paramP1)>>3) + ((tempDeltaX2*paramP2)>>10) + ((tempDeltaX2*tempDelta*paramP3)>>18);

   return ( d );
}

//*****************************************************************************
//
// Converts the relative frequency offset of HPOSC to the RF Core parameter format.
//
//*****************************************************************************
int16_t
OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert( int32_t HPOSC_RelFreqOffset )
{
   // The input argument, hereby referred to simply as "d", describes the frequency offset
   // of the HPOSC relative to the nominal frequency in this way:
   //
   //    F_HPOSC = F_nom * (1 + d/(2^22))
   //
   // But for use by the radio, to compensate the frequency error, we need to find the
   // frequency offset "rfcFreqOffset" defined in the following format:
   //
   //    F_nom = F_HPOSC * (1 + rfCoreFreqOffset/(2^22))
   //
   // To derive "rfCoreFreqOffset" from "d" we combine the two above equations and get:
   //
   //    (1 + rfCoreFreqOffset/(2^22)) = (1 + d/(2^22))^-1
   //
   // Which can be rewritten into:
   //
   //    rfCoreFreqOffset = -d*(2^22) / ((2^22) + d)
   //
   //               = -d * [ 1 / (1 + d/(2^22)) ]
   //
   // To avoid doing a 64-bit division due to the (1 + d/(2^22))^-1 expression,
   // we can use Taylor series (Maclaurin series) to approximate it:
   //
   //       1 / (1 - x) ~= 1 + x + x^2 + x^3 + x^4 + ... etc      (Maclaurin series)
   //
   // In our case, we have x = - d/(2^22), and we only include up to the first
   // order term of the series, as the second order term ((d^2)/(2^44)) is very small:
   //
   //       freqError ~= -d + d^2/(2^22)   (+ small approximation error)
   //
   // The approximation error is negligible for our use.

   int32_t rfCoreFreqOffset = -HPOSC_RelFreqOffset + (( HPOSC_RelFreqOffset * HPOSC_RelFreqOffset ) >> 22 );

   return ( rfCoreFreqOffset );
}

//*****************************************************************************
//
// Get crystal amplitude (assuming crystal is running).
//
//*****************************************************************************
uint32_t
OSCHF_DebugGetCrystalAmplitude( void )
{
   uint32_t oscCfgRegCopy  ;
   uint32_t startTime      ;
   uint32_t deltaTime      ;
   uint32_t ampValue       ;

   // The specified method is as follows:
   // 1. Set minimum interval between oscillator amplitude calibrations.
   //    (Done by setting PER_M=0 and PER_E=1)
   // 2. Wait approximately 4 milliseconds in order to measure over a
   //    moderately large number of calibrations.
   // 3. Read out the crystal amplitude value from the peek detector.
   // 4. Restore original oscillator amplitude calibrations interval.
   // 5. Return crystal amplitude value converted to millivolt.
   oscCfgRegCopy = HWREG( AON_WUC_BASE + AON_WUC_O_OSCCFG );
   HWREG( AON_WUC_BASE + AON_WUC_O_OSCCFG ) = ( 1 << AON_WUC_OSCCFG_PER_E_S );
   startTime = AONRTCCurrentCompareValueGet();
   do {
      deltaTime = AONRTCCurrentCompareValueGet() - startTime;
   } while ( deltaTime < ((uint32_t)( 0.004 * FACTOR_SEC_TO_COMP_VAL_FORMAT )));
   ampValue = ( HWREG( AUX_DDI0_OSC_BASE + DDI_0_OSC_O_STAT1 ) &
      DDI_0_OSC_STAT1_HPM_UPDATE_AMP_M ) >>
      DDI_0_OSC_STAT1_HPM_UPDATE_AMP_S ;
   HWREG( AON_WUC_BASE + AON_WUC_O_OSCCFG ) = oscCfgRegCopy;

   return ( ampValue * 15 );
}

//*****************************************************************************
//
// Get the expected average crystal amplitude.
//
//*****************************************************************************
uint32_t
OSCHF_DebugGetExpectedAverageCrystalAmplitude( void )
{
   uint32_t ampCompTh1    ;
   uint32_t highThreshold ;
   uint32_t lowThreshold  ;

   ampCompTh1 = HWREG( AUX_DDI0_OSC_BASE + DDI_0_OSC_O_AMPCOMPTH1 );
   highThreshold = ( ampCompTh1 & DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_HTH_M ) >>
                                  DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_HTH_S ;
   lowThreshold  = ( ampCompTh1 & DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_LTH_M ) >>
                                  DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_LTH_S ;

   return ((( highThreshold + lowThreshold ) * 15 ) >> 1 );
}
