/******************************************************************************
*  Filename:       setup.c
*  Revised:        2018-11-06 15:08:57 +0100 (Tue, 06 Nov 2018)
*  Revision:       53239
*
*  Description:    Setup file for CC13xx/CC26xx devices.
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

// Hardware headers
#include "../inc/hw_types.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_adi.h"
#include "../inc/hw_adi_2_refsys.h"
#include "../inc/hw_adi_3_refsys.h"
#include "../inc/hw_aon_ioc.h"
#include "../inc/hw_aon_sysctl.h"
#include "../inc/hw_aon_wuc.h"
#include "../inc/hw_aux_wuc.h"
#include "../inc/hw_ccfg.h"
#include "../inc/hw_fcfg1.h"
#include "../inc/hw_flash.h"
#include "../inc/hw_prcm.h"
#include "../inc/hw_vims.h"
// Driverlib headers
#include "aon_wuc.h"
#include "aux_wuc.h"
#include "chipinfo.h"
#include "setup.h"
#include "setup_rom.h"

//*****************************************************************************
//
// Handle support for DriverLib in ROM:
// This section will undo prototype renaming made in the header file
//
//*****************************************************************************
#if !defined(DOXYGEN)
    #undef  SetupTrimDevice
    #define SetupTrimDevice                 NOROM_SetupTrimDevice
#endif



//*****************************************************************************
//
// Defined CPU delay macro with microseconds as input
// Quick check shows: (To be further investigated)
// At 48 MHz RCOSC and VIMS.CONTROL.PREFETCH = 0, there is 5 cycles
// At 48 MHz RCOSC and VIMS.CONTROL.PREFETCH = 1, there is 4 cycles
// At 24 MHz RCOSC and VIMS.CONTROL.PREFETCH = 0, there is 3 cycles
//
//*****************************************************************************
#define CPU_DELAY_MICRO_SECONDS( x ) \
   CPUdelay(((uint32_t)((( x ) * 48.0 ) / 5.0 )) - 1 )


//*****************************************************************************
//
// Function declarations
//
//*****************************************************************************
static void     TrimAfterColdReset( void );
static void     TrimAfterColdResetWakeupFromShutDown( uint32_t ui32Fcfg1Revision );
static void     TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown( void );

//*****************************************************************************
//
// Perform the necessary trim of the device which is not done in boot code
//
// This function should only execute coming from ROM boot. The current
// implementation does not take soft reset into account. However, it does no
// damage to execute it again. It only consumes time.
//
//*****************************************************************************
void
SetupTrimDevice(void)
{
    uint32_t ui32Fcfg1Revision;
    uint32_t ui32AonSysResetctl;

    // Get layout revision of the factory configuration area
    // (Handle undefined revision as revision = 0)
    ui32Fcfg1Revision = HWREG(FCFG1_BASE + FCFG1_O_FCFG1_REVISION);
    if ( ui32Fcfg1Revision == 0xFFFFFFFF ) {
        ui32Fcfg1Revision = 0;
    }

    // This driverlib version and setup file is for CC26x0 PG2.2 and later
    // Halt if violated
    ThisLibraryIsFor_CC26x0_HwRev22AndLater_HaltIfViolated();

    // Enable standby in flash bank
    HWREGBITW( FLASH_BASE + FLASH_O_CFG, FLASH_CFG_DIS_STANDBY_BITN ) = 0;

    // Clock must always be enabled for the semaphore module (due to ADI/DDI HW workaround)
    HWREG( AUX_WUC_BASE + AUX_WUC_O_MODCLKEN1 ) = AUX_WUC_MODCLKEN1_SMPH;

    // Warm resets on CC13x0 and CC26x0 complicates software design because much of
    // our software expect that initialization is done from a full system reset.
    // This includes RTC setup, oscillator configuration and AUX setup.
    // To ensure a full reset of the device is done when customers get e.g. a Watchdog
    // reset, the following is set here:
    HWREGBITW( PRCM_BASE + PRCM_O_WARMRESET, PRCM_WARMRESET_WR_TO_PINRESET_BITN ) = 1;

    // Select correct CACHE mode and set correct CACHE configuration
#if ( CCFG_BASE == CCFG_BASE_DEFAULT )
    SetupSetCacheModeAccordingToCcfgSetting();
#else
    NOROM_SetupSetCacheModeAccordingToCcfgSetting();
#endif

    // 1. Check for powerdown
    // 2. Check for shutdown
    // 3. Assume cold reset if none of the above.
    //
    // It is always assumed that the application will freeze the latches in
    // AON_IOC when going to powerdown in order to retain the values on the IOs.
    //
    // NB. If this bit is not cleared before proceeding to powerdown, the IOs
    //     will all default to the reset configuration when restarting.
    if( ! ( HWREGBITW( AON_IOC_BASE + AON_IOC_O_IOCLATCH, AON_IOC_IOCLATCH_EN_BITN )))
    {
        // NB. This should be calling a ROM implementation of required trim and
        // compensation
        // e.g. TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown()
        TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown();
    }
    // Check for shutdown
    //
    // When device is going to shutdown the hardware will automatically clear
    // the SLEEPDIS bit in the SLEEP register in the AON_SYSCTL module.
    // It is left for the application to assert this bit when waking back up,
    // but not before the desired IO configuration has been re-established.
    else if( ! ( HWREGBITW( AON_SYSCTL_BASE + AON_SYSCTL_O_SLEEPCTL, AON_SYSCTL_SLEEPCTL_IO_PAD_SLEEP_DIS_BITN )))
    {
        // NB. This should be calling a ROM implementation of required trim and
        // compensation
        // e.g. TrimAfterColdResetWakeupFromShutDown()    -->
        //      TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown();
        TrimAfterColdResetWakeupFromShutDown(ui32Fcfg1Revision);
        TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown();
    }
    else
    {
        // Consider adding a check for soft reset to allow debugging to skip
        // this section!!!
        //
        // NB. This should be calling a ROM implementation of required trim and
        // compensation
        // e.g. TrimAfterColdReset()   -->
        //      TrimAfterColdResetWakeupFromShutDown()    -->
        //      TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown()
        TrimAfterColdReset();
        TrimAfterColdResetWakeupFromShutDown(ui32Fcfg1Revision);
        TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown();

    }

    // Set VIMS power domain control.
    // PDCTL1VIMS = 0 ==> VIMS power domain is only powered when CPU power domain is powered
    HWREG( PRCM_BASE + PRCM_O_PDCTL1VIMS ) = 0;

    // Configure optimal wait time for flash FSM in cases where flash pump
    // wakes up from sleep
    HWREG(FLASH_BASE + FLASH_O_FPAC1) = (HWREG(FLASH_BASE + FLASH_O_FPAC1) &
                                         ~FLASH_FPAC1_PSLEEPTDIS_M) |
                                        (0x139<<FLASH_FPAC1_PSLEEPTDIS_S);

    // And finally at the end of the flash boot process:
    // SET BOOT_DET bits in AON_SYSCTL to 3 if already found to be 1
    // Note: The BOOT_DET_x_CLR/SET bits must be manually cleared
    if ((( HWREG( AON_SYSCTL_BASE + AON_SYSCTL_O_RESETCTL ) &
        ( AON_SYSCTL_RESETCTL_BOOT_DET_1_M | AON_SYSCTL_RESETCTL_BOOT_DET_0_M )) >>
        AON_SYSCTL_RESETCTL_BOOT_DET_0_S ) == 1 )
    {
        ui32AonSysResetctl = ( HWREG( AON_SYSCTL_BASE + AON_SYSCTL_O_RESETCTL ) &
        ~( AON_SYSCTL_RESETCTL_BOOT_DET_1_CLR_M | AON_SYSCTL_RESETCTL_BOOT_DET_0_CLR_M |
           AON_SYSCTL_RESETCTL_BOOT_DET_1_SET_M | AON_SYSCTL_RESETCTL_BOOT_DET_0_SET_M ));
        HWREG( AON_SYSCTL_BASE + AON_SYSCTL_O_RESETCTL ) = ui32AonSysResetctl | AON_SYSCTL_RESETCTL_BOOT_DET_1_SET_M;
        HWREG( AON_SYSCTL_BASE + AON_SYSCTL_O_RESETCTL ) = ui32AonSysResetctl;
    }

    // Make sure there are no ongoing VIMS mode change when leaving SetupTrimDevice()
    // (There should typically be no wait time here, but need to be sure)
    while ( HWREGBITW( VIMS_BASE + VIMS_O_STAT, VIMS_STAT_MODE_CHANGING_BITN )) {
        // Do nothing - wait for an eventual ongoing mode change to complete.
    }
}

//*****************************************************************************
//
//! \brief Trims to be applied when coming from POWER_DOWN (also called when
//! coming from SHUTDOWN and PIN_RESET).
//!
//! \return None
//
//*****************************************************************************
static void
TrimAfterColdResetWakeupFromShutDownWakeupFromPowerDown( void )
{
    // Currently no specific trim for Powerdown
}

//*****************************************************************************
//
//! \brief Trims to be applied when coming from SHUTDOWN (also called when
//! coming from PIN_RESET).
//!
//! \param ui32Fcfg1Revision
//!
//! \return None
//
//*****************************************************************************
static void
TrimAfterColdResetWakeupFromShutDown(uint32_t ui32Fcfg1Revision)
{
    uint32_t   ccfg_ModeConfReg  ;
    uint32_t   mp1rev            ;

    // Force AUX on and enable clocks
    //
    // No need to save the current status of the power/clock registers.
    // At this point both AUX and AON should have been reset to 0x0.
    HWREG(AON_WUC_BASE + AON_WUC_O_AUXCTL) = AON_WUC_AUXCTL_AUX_FORCE_ON;

    // Wait for power on on the AUX domain
    while( ! ( HWREGBITW( AON_WUC_BASE + AON_WUC_O_PWRSTAT, AON_WUC_PWRSTAT_AUX_PD_ON_BITN )));

    // Enable the clocks for AUX_DDI0_OSC and AUX_ADI4
    HWREG(AUX_WUC_BASE + AUX_WUC_O_MODCLKEN0) = AUX_WUC_MODCLKEN0_AUX_DDI0_OSC |
                                                AUX_WUC_MODCLKEN0_AUX_ADI4;

    // It's found to be optimal to override the FCFG1..DCDC_IPEAK setting as follows:
    // if ( alternative DCDC setting in CCFG is enabled )  ADI3..IPEAK = CCFG..DCDC_IPEAK
    // else                                                ADI3..IPEAK = 2
    if (( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING ) == 0 ) {
        // ADI_3_REFSYS:DCDCCTL5[3]  (=DITHER_EN) = CCFG_MODE_CONF_1[19]   (=ALT_DCDC_DITHER_EN)
        // ADI_3_REFSYS:DCDCCTL5[2:0](=IPEAK    ) = CCFG_MODE_CONF_1[18:16](=ALT_DCDC_IPEAK    )
        // Using a single 4-bit masked write since layout is equal for both source and destination
        HWREGB( ADI3_BASE + ADI_O_MASK4B + ( ADI_3_REFSYS_O_DCDCCTL5 * 2 )) = ( 0xF0 |
            ( HWREG( CCFG_BASE + CCFG_O_MODE_CONF_1 ) >> CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_S ));

    } else {
        HWREGB( ADI3_BASE + ADI_O_MASK4B + ( ADI_3_REFSYS_O_DCDCCTL5 * 2 )) = 0x72;
    }

    //
    // Enable for JTAG to be powered down (will still be powered on if debugger is connected)
    AONWUCJtagPowerOff();

    // read the MODE_CONF register in CCFG
    ccfg_ModeConfReg = HWREG( CCFG_BASE + CCFG_O_MODE_CONF );

    // First part of trim done after cold reset and wakeup from shutdown:
    // -Adjust the VDDR_TRIM_SLEEP value.
    // -Configure DCDC.
    SetupAfterColdResetWakeupFromShutDownCfg1( ccfg_ModeConfReg );

    // Second part of trim done after cold reset and wakeup from shutdown:
    // -Configure XOSC.
#if ( CCFG_BASE == CCFG_BASE_DEFAULT )
    SetupAfterColdResetWakeupFromShutDownCfg2( ui32Fcfg1Revision, ccfg_ModeConfReg );
#else
    NOROM_SetupAfterColdResetWakeupFromShutDownCfg2( ui32Fcfg1Revision, ccfg_ModeConfReg );
#endif

    // Increased margin between digital supply voltage and VDD BOD during standby.
    // VTRIM_UDIG: signed 4 bits value to be incremented by 2 (max = 7)
    // VTRIM_BOD: unsigned 4 bits value to be decremented by 1 (min = 0)
    // This applies to chips with mp1rev < 542 for cc13x0 and for mp1rev < 527 for cc26x0
    mp1rev = (( HWREG( FCFG1_BASE + FCFG1_O_TRIM_CAL_REVISION ) & FCFG1_TRIM_CAL_REVISION_MP1_M ) >>
                                                                  FCFG1_TRIM_CAL_REVISION_MP1_S ) ;
    if ( mp1rev < 527 ) {
        uint32_t ldoTrimReg = HWREG( FCFG1_BASE + FCFG1_O_BAT_RC_LDO_TRIM );
        uint32_t vtrim_bod  = (( ldoTrimReg & FCFG1_BAT_RC_LDO_TRIM_VTRIM_BOD_M  ) >>
                                              FCFG1_BAT_RC_LDO_TRIM_VTRIM_BOD_S  ); // bit[27:24] unsigned
        uint32_t vtrim_udig = (( ldoTrimReg & FCFG1_BAT_RC_LDO_TRIM_VTRIM_UDIG_M ) >>
                                              FCFG1_BAT_RC_LDO_TRIM_VTRIM_UDIG_S ); // bit[19:16] signed but treated as unsigned
        if ( vtrim_bod > 0 ) {
            vtrim_bod -= 1;
        }
        if ( vtrim_udig != 7 ) {
            if ( vtrim_udig == 6 ) {
                vtrim_udig = 7;
            } else {
                vtrim_udig = (( vtrim_udig + 2 ) & 0xF );
            }
        }
        HWREGB( ADI2_BASE + ADI_2_REFSYS_O_SOCLDOCTL0 ) =
            ( vtrim_udig << ADI_2_REFSYS_SOCLDOCTL0_VTRIM_UDIG_S ) |
            ( vtrim_bod  << ADI_2_REFSYS_SOCLDOCTL0_VTRIM_BOD_S  ) ;
    }

    // Third part of trim done after cold reset and wakeup from shutdown:
    // -Configure HPOSC.
    // -Setup the LF clock.
#if ( CCFG_BASE == CCFG_BASE_DEFAULT )
    SetupAfterColdResetWakeupFromShutDownCfg3( ccfg_ModeConfReg );
#else
    NOROM_SetupAfterColdResetWakeupFromShutDownCfg3( ccfg_ModeConfReg );
#endif

    // Allow AUX to power down
    AUXWUCPowerCtrl( AUX_WUC_POWER_DOWN );

    // Leaving on AUX and clock for AUX_DDI0_OSC on but turn off clock for AUX_ADI4
    HWREG( AUX_WUC_BASE + AUX_WUC_O_MODCLKEN0 ) = AUX_WUC_MODCLKEN0_AUX_DDI0_OSC;

    // Disable EFUSE clock
    HWREGBITW( FLASH_BASE + FLASH_O_CFG, FLASH_CFG_DIS_EFUSECLK_BITN ) = 1;
}


//*****************************************************************************
//
//! \brief Trims to be applied when coming from PIN_RESET.
//!
//! \return None
//
//*****************************************************************************
static void
TrimAfterColdReset( void )
{
    // Currently no specific trim for Cold Reset
}
