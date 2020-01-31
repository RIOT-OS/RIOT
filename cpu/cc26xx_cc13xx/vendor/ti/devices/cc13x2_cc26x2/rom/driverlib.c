#include "../driverlib/adi.h"
#include "../driverlib/aes.h"
#include "../driverlib/aon_batmon.h"
#include "../driverlib/aon_batmon.h"
#include "../driverlib/aon_batmon.h"
#include "../driverlib/aon_event.h"
#include "../driverlib/aon_ioc.h"
#include "../driverlib/aon_rtc.h"
#include "../driverlib/aon_rtc.h"
#include "../driverlib/aux_adc.h"
#include "../driverlib/aux_ctrl.h"
#include "../driverlib/aux_tdc.h"
#include "../driverlib/chipinfo.h"
#include "../driverlib/cpu.h"
#include "../driverlib/cpu.h"
#include "../driverlib/crypto.h"
#include "../driverlib/ddi.h"
#include "../driverlib/ddi.h"
#include "../driverlib/event.h"
#include "../driverlib/flash.h"
#include "../driverlib/flash.h"
#include "../driverlib/gpio.h"
#include "../driverlib/i2c.h"
#include "../driverlib/i2s.h"
#include "../driverlib/interrupt.h"
#include "../driverlib/ioc.h"
#include "../driverlib/ioc.h"
#include "../driverlib/osc.h"
#include "../driverlib/osc.h"
#include "../driverlib/pka.h"
#include "../driverlib/prcm.h"
#include "../driverlib/pwr_ctrl.h"
#include "../driverlib/setup_rom.h"
#include "../driverlib/setup_rom.h"
#include "../driverlib/setup_rom.h"
#include "../driverlib/sha2.h"
#include "../driverlib/smph.h"
#include "../driverlib/ssi.h"
#include "../driverlib/sys_ctrl.h"
#include "../driverlib/sys_ctrl.h"
#include "../driverlib/timer.h"
#include "../driverlib/trng.h"
#include "../driverlib/uart.h"
#include "../driverlib/udma.h"
#include "../driverlib/vims.h"
#include "../inc/hw_adi.h"
#include "../inc/hw_adi_2_refsys.h"
#include "../inc/hw_adi_3_refsys.h"
#include "../inc/hw_adi_4_aux.h"
#include "../inc/hw_aon_batmon.h"
#include "../inc/hw_aux_sysif.h"
#include "../inc/hw_aux_sysif.h"
#include "../inc/hw_ccfg.h"
#include "../inc/hw_ccfg.h"
#include "../inc/hw_ccfg.h"
#include "../inc/hw_ccfg.h"
#include "../inc/hw_ddi_0_osc.h"
#include "../inc/hw_fcfg1.h"
#include "../inc/hw_fcfg1.h"
#include "../inc/hw_fcfg1.h"
#include "../inc/hw_fcfg1.h"
#include "../inc/hw_ioc.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_memmap.h"
#include "../inc/hw_types.h"
#include "../inc/hw_types.h"
#include "../inc/hw_types.h"
#include "../inc/hw_types.h"

//*****************************************************************************
//
// Disable all external interrupts
//
//*****************************************************************************
#if defined(DOXYGEN)
uint32_t
CPUcpsid(void)
{
    // This function is written in assembly. See cpu.c for compiler specific implementation.
}
#elif defined(__IAR_SYSTEMS_ICC__)
uint32_t
CPUcpsid(void)
{
    // Read PRIMASK and disable interrupts.
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsid   i\n");

    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
__asm uint32_t
CPUcpsid(void)
{
    // Read PRIMASK and disable interrupts.
    mrs     r0, PRIMASK;
    cpsid   i;
    bx      lr
}
#elif defined(__TI_COMPILER_VERSION__)
uint32_t
CPUcpsid(void)
{
    // Read PRIMASK and disable interrupts.
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsid   i\n"
          "    bx      lr\n");

    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    return(0);
}
#else
uint32_t __attribute__((naked))
CPUcpsid(void)
{
    uint32_t ui32Ret;

    // Read PRIMASK and disable interrupts
    __asm volatile ("    mrs     %0, PRIMASK\n"
                    "    cpsid   i\n"
                    "    bx      lr\n"
                    : "=r"(ui32Ret)
                   );

    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    return(ui32Ret);
}
#endif
//*****************************************************************************
//
// Enable all external interrupts
//
//*****************************************************************************
#if defined(DOXYGEN)
uint32_t
CPUcpsie(void)
{
    // This function is written in assembly. See cpu.c for compiler specific implementation.
}
#elif defined(__IAR_SYSTEMS_ICC__)
uint32_t
CPUcpsie(void)
{
    // Read PRIMASK and enable interrupts.
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsie   i\n");

    // "Warning[Pe940]: missing return statement at end of non-void function"
    // is suppressed here to avoid putting a "bx lr" in the inline assembly
    // above and a superfluous return statement here.
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
__asm uint32_t
CPUcpsie(void)
{
    // Read PRIMASK and enable interrupts.
    mrs     r0, PRIMASK;
    cpsie   i;
    bx      lr
}
#elif defined(__TI_COMPILER_VERSION__)
uint32_t
CPUcpsie(void)
{
    // Read PRIMASK and enable interrupts.
    __asm("    mrs     r0, PRIMASK\n"
          "    cpsie   i\n"
          "    bx      lr\n");

    // The following keeps the compiler happy, because it wants to see a
    // return value from this function.  It will generate code to return
    // a zero.  However, the real return is the "bx lr" above, so the
    // return(0) is never executed and the function returns with the value
    // you expect in R0.
    return(0);
}
#else
uint32_t __attribute__((naked))
CPUcpsie(void)
{
    uint32_t ui32Ret;

    // Read PRIMASK and enable interrupts.
    __asm volatile ("    mrs     %0, PRIMASK\n"
                    "    cpsie   i\n"
                    "    bx      lr\n"
                    : "=r"(ui32Ret)
                   );

    // The return is handled in the inline assembly, but the compiler will
    // still complain if there is not an explicit return here (despite the fact
    // that this does not result in any code being produced because of the
    // naked attribute).
    return(ui32Ret);
}
#endif
//*****************************************************************************
//
// Provide a small delay
//
//*****************************************************************************
#if defined(DOXYGEN)
void
CPUdelay(uint32_t ui32Count)
{
    // This function is written in assembly. See cpu.c for compiler specific implementation.
}
#elif defined(__IAR_SYSTEMS_ICC__)
void
CPUdelay(uint32_t ui32Count)
{
    // Loop the specified number of times
    __asm("CPUdelay:\n"
          "    subs    r0, #1\n"
          "    bne.n   CPUdelay\n"
          "    bx      lr");
#pragma diag_suppress=Pe940
}
#pragma diag_default=Pe940
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
__asm void
CPUdelay(uint32_t ui32Count)
{
    // Delay the specified number of times (3 cycles pr. loop)
CPUdel
    subs    r0, #1;
    bne     CPUdel;
    bx      lr;
}
#elif defined(__TI_COMPILER_VERSION__)
    // For CCS implement this function in pure assembly. This prevents the TI
    // compiler from doing funny things with the optimizer.

    // Loop the specified number of times
__asm("    .sect \".text:CPUdelay\"\n"
      "    .clink\n"
      "    .thumbfunc CPUdelay\n"
      "    .thumb\n"
      "    .global CPUdelay\n"
      "CPUdelay:\n"
      "    subs r0, #1\n"
      "    bne.n CPUdelay\n"
      "    bx lr\n");
#else
// GCC
void __attribute__((naked))
CPUdelay(uint32_t ui32Count)
{
    // Loop the specified number of times
    __asm volatile ("%=:  subs  %0, #1\n"
                    "     bne   %=b\n"
                    "     bx    lr\n"
                    : /* No output */
                    : "r" (ui32Count)
                   );
}
#endif

void AONEventMcuWakeUpSet(uint32_t ui32MCUWUEvent, uint32_t ui32EventSrc) {
    uint32_t ui32Shift  ;
    uint32_t ui32Mask   ;
    uint32_t ui32RegAdr ;

    // Check the arguments.
    ASSERT(( ui32MCUWUEvent >= AON_EVENT_MCU_WU0 ) && ( ui32MCUWUEvent <= AON_EVENT_MCU_WU7 ))
    ASSERT( ui32EventSrc <= AON_EVENT_NONE );

    ui32Shift  = (( ui32MCUWUEvent & 3 ) << 3            );
    ui32Mask   = ( 0x3F << ui32Shift                     );
    ui32RegAdr = ( AON_EVENT_BASE + AON_EVENT_O_MCUWUSEL );
    if ( ui32MCUWUEvent > 3 ) {
      ui32RegAdr += 4;
    }
    HWREG( ui32RegAdr ) = ( HWREG( ui32RegAdr ) & ( ~ui32Mask )) | ( ui32EventSrc << ui32Shift );
}

uint32_t AONEventMcuWakeUpGet(uint32_t ui32MCUWUEvent) {
    uint32_t ui32Shift  ;
    uint32_t ui32RegAdr ;

    // Check the arguments.
    ASSERT(( ui32MCUWUEvent >= AON_EVENT_MCU_WU0 ) && ( ui32MCUWUEvent <= AON_EVENT_MCU_WU7 ))

    ui32Shift  = (( ui32MCUWUEvent & 3 ) << 3            );
    ui32RegAdr = ( AON_EVENT_BASE + AON_EVENT_O_MCUWUSEL );
    if ( ui32MCUWUEvent > 3 ) {
      ui32RegAdr += 4;
    }
    return (( HWREG( ui32RegAdr ) >> ui32Shift ) & 0x3F );
}

void AONEventMcuSet(uint32_t ui32MCUEvent, uint32_t ui32EventSrc) {
    uint32_t ui32Ctrl;

    // Check the arguments.
    ASSERT((ui32MCUEvent == AON_EVENT_MCU_EVENT0) ||
           (ui32MCUEvent == AON_EVENT_MCU_EVENT1) ||
           (ui32MCUEvent == AON_EVENT_MCU_EVENT2));
    ASSERT(ui32EventSrc <= AON_EVENT_NONE);

    ui32Ctrl = HWREG(AON_EVENT_BASE + AON_EVENT_O_EVTOMCUSEL);

    if(ui32MCUEvent == AON_EVENT_MCU_EVENT0)
    {
        ui32Ctrl &= ~(AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_M);
        ui32Ctrl |= (ui32EventSrc & 0x3f) << AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_S;
    }
    else if(ui32MCUEvent == AON_EVENT_MCU_EVENT1)
    {
        ui32Ctrl &= ~(AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_M);
        ui32Ctrl |= (ui32EventSrc & 0x3f) << AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_S;
    }
    else if(ui32MCUEvent == AON_EVENT_MCU_EVENT2)
    {
        ui32Ctrl &= ~(AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_M);
        ui32Ctrl |= (ui32EventSrc & 0x3f) << AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_S;
    }

    HWREG(AON_EVENT_BASE + AON_EVENT_O_EVTOMCUSEL) = ui32Ctrl;
}

uint32_t AONEventMcuGet(uint32_t ui32MCUEvent) {
    uint32_t ui32EventSrc;

    // Check the arguments.
    ASSERT((ui32MCUEvent == AON_EVENT_MCU_EVENT0) ||
           (ui32MCUEvent == AON_EVENT_MCU_EVENT1) ||
           (ui32MCUEvent == AON_EVENT_MCU_EVENT2));

    ui32EventSrc = HWREG(AON_EVENT_BASE + AON_EVENT_O_EVTOMCUSEL);

    if(ui32MCUEvent == AON_EVENT_MCU_EVENT0)
    {
        return((ui32EventSrc & AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_M) >>
               AON_EVENT_EVTOMCUSEL_AON_PROG0_EV_S);
    }
    else if(ui32MCUEvent == AON_EVENT_MCU_EVENT1)
    {
        return((ui32EventSrc & AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_M) >>
               AON_EVENT_EVTOMCUSEL_AON_PROG1_EV_S);
    }
    else if(ui32MCUEvent == AON_EVENT_MCU_EVENT2)
    {
        return((ui32EventSrc & AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_M) >>
               AON_EVENT_EVTOMCUSEL_AON_PROG2_EV_S);
    }

    // Should never get to this statement, but suppress warning.
    ASSERT(0);
    return(0);
}

uint64_t AONRTCCurrent64BitValueGet( void ) {
    union {
        uint64_t  returnValue       ;
        uint32_t  secAndSubSec[ 2 ] ;
    } currentRtc                    ;
    uint32_t      ui32SecondSecRead ;

    // Reading SEC both before and after SUBSEC in order to detect if SEC incremented while reading SUBSEC
    // If SEC incremented, we can't be sure which SEC the SUBSEC belongs to, so repeating the sequence then.
    do {
        currentRtc.secAndSubSec[ 1 ] = HWREG( AON_RTC_BASE + AON_RTC_O_SEC    );
        currentRtc.secAndSubSec[ 0 ] = HWREG( AON_RTC_BASE + AON_RTC_O_SUBSEC );
        ui32SecondSecRead            = HWREG( AON_RTC_BASE + AON_RTC_O_SEC    );
    } while ( currentRtc.secAndSubSec[ 1 ] != ui32SecondSecRead );

    return ( currentRtc.returnValue );
}

void AUXCTRLImageLoad(uint16_t *pui16Image, uint32_t ui32StartAddr, uint32_t ui32Size) {
    uint16_t* pui16Src16;
    uint16_t* pui16Dst16;
    uint32_t ui32WordCnt;

    // Check the arguments.
    ASSERT(ui32StartAddr < 512);
    ASSERT(ui32Size <= 1024);
    ASSERT((ui32Size / 2 + ui32StartAddr) <= 512);

    // Copy image to AUX RAM.
    ui32WordCnt = (ui32Size >> 1);
    pui16Src16 = pui16Image;
    pui16Dst16 = (uint16_t*)(AUX_RAM_BASE + (ui32StartAddr << 1));

    while(ui32WordCnt--)
    {
        *pui16Dst16++ = *pui16Src16++;
    }
}

void AUXTDCConfigSet(uint32_t ui32Base, uint32_t ui32StartCondition, uint32_t ui32StopCondition) {
    // Check the arguments.
    ASSERT(AUXTDCBaseValid(ui32Base));

    // Make sure the AUX TDC is in the idle state before changing the
    // configuration.
    while(!((HWREG(ui32Base + AUX_TDC_O_STAT) & AUX_TDC_STAT_STATE_M) ==
            AUX_TDC_STAT_STATE_IDLE))
    {
    }

    // Clear previous results.
    HWREG(ui32Base + AUX_TDC_O_CTL) = 0x0;

    // Change the configuration.
    HWREG(ui32Base + AUX_TDC_O_TRIGSRC) = ui32StartCondition | ui32StopCondition;
}

uint32_t AUXTDCMeasurementDone(uint32_t ui32Base) {
    uint32_t ui32Reg;
    uint32_t ui32Status;

    // Check the arguments.
    ASSERT(AUXTDCBaseValid(ui32Base));

    // Check if the AUX TDC is done measuring.
    ui32Reg = HWREG(ui32Base + AUX_TDC_O_STAT);
    if(ui32Reg & AUX_TDC_STAT_DONE)
    {
        ui32Status = AUX_TDC_DONE;
    }
    else if(ui32Reg & AUX_TDC_STAT_SAT)
    {
        ui32Status = AUX_TDC_TIMEOUT;
    }
    else
    {
        ui32Status = AUX_TDC_BUSY;
    }

    // Return the status.
    return (ui32Status);
}

void DDI32RegWrite(uint32_t ui32Base, uint32_t ui32Reg, uint32_t ui32Val) {
    // Check the arguments.
    ASSERT(DDIBaseValid(ui32Base));
    ASSERT(ui32Reg < DDI_SLAVE_REGS);

    // Write the value to the register.
    HWREG(ui32Base + ui32Reg) = ui32Val;
}

void DDI16BitWrite(uint32_t ui32Base, uint32_t ui32Reg, uint32_t ui32Mask, uint32_t ui32WrData) {
    uint32_t ui32RegAddr;
    uint32_t ui32Data;

    // Check the arguments.
    ASSERT(DDIBaseValid(ui32Base));
    ASSERT(!((ui32Mask & 0xFFFF0000) ^ (ui32Mask & 0x0000FFFF)));
    ASSERT(!(ui32WrData & 0xFFFF0000));

    // DDI 16-bit target is on 32-bit boundary so double offset
    ui32RegAddr = ui32Base + (ui32Reg << 1) + DDI_O_MASK16B;

    // Adjust for target bit in high half of the word.
    if(ui32Mask & 0xFFFF0000)
    {
        ui32RegAddr += 4;
        ui32Mask >>= 16;
    }

    // Write mask if data is not zero (to set mask bit), else write '0'.
    ui32Data = ui32WrData ? ui32Mask : 0x0;

    // Update the register.
    HWREG(ui32RegAddr) = (ui32Mask << 16) | ui32Data;
}

void DDI16BitfieldWrite(uint32_t ui32Base, uint32_t ui32Reg, uint32_t ui32Mask, uint32_t ui32Shift, uint16_t ui32Data) {
    uint32_t ui32RegAddr;
    uint32_t ui32WrData;

    // Check the arguments.
    ASSERT(DDIBaseValid(ui32Base));

    // 16-bit target is on 32-bit boundary so double offset.
    ui32RegAddr = ui32Base + (ui32Reg << 1) + DDI_O_MASK16B;

    // Adjust for target bit in high half of the word.
    if(ui32Shift >= 16)
    {
        ui32Shift = ui32Shift - 16;
        ui32RegAddr += 4;
        ui32Mask = ui32Mask >> 16;
    }

    // Shift data in to position.
    ui32WrData = ui32Data << ui32Shift;

    // Write data.
    HWREG(ui32RegAddr) = (ui32Mask << 16) | ui32WrData;
}

uint16_t DDI16BitRead(uint32_t ui32Base, uint32_t ui32Reg, uint32_t ui32Mask) {
    uint32_t ui32RegAddr;
    uint16_t ui16Data;

    // Check the arguments.
    ASSERT(DDIBaseValid(ui32Base));

    // Calculate the address of the register.
    ui32RegAddr = ui32Base + ui32Reg + DDI_O_DIR;

    // Adjust for target bit in high half of the word.
    if(ui32Mask & 0xFFFF0000)
    {
        ui32RegAddr += 2;
        ui32Mask = ui32Mask >> 16;
    }

    // Read a halfword on the DDI interface.
    ui16Data = HWREGH(ui32RegAddr);

    // Mask data.
    ui16Data = ui16Data & ui32Mask;

    // Return masked data.
    return(ui16Data);
}

uint16_t DDI16BitfieldRead(uint32_t ui32Base, uint32_t ui32Reg, uint32_t ui32Mask, uint32_t ui32Shift) {
    uint32_t ui32RegAddr;
    uint16_t ui16Data;

    // Check the arguments.
    ASSERT(DDIBaseValid(ui32Base));

    // Calculate the register address.
    ui32RegAddr = ui32Base + ui32Reg + DDI_O_DIR;

    // Adjust for target bit in high half of the word.
    if(ui32Shift >= 16)
    {
        ui32Shift = ui32Shift - 16;
        ui32RegAddr += 2;
        ui32Mask = ui32Mask >> 16;
    }

    // Read the register.
    ui16Data = HWREGH(ui32RegAddr);

    // Mask data and shift into place.
    ui16Data &= ui32Mask;
    ui16Data >>= ui32Shift;

    // Return data.
    return(ui16Data);
}

//*****************************************************************************
//
// Defines for accesses to the security control in the customer configuration
// area in flash top sector.
//
//*****************************************************************************
#define CCFG_OFFSET_SECURITY   CCFG_O_BL_CONFIG
#define CCFG_OFFSET_SECT_PROT  CCFG_O_CCFG_PROT_31_0
#define CCFG_SIZE_SECURITY     0x00000014
#define CCFG_SIZE_SECT_PROT    0x00000004

//*****************************************************************************
//
// Default values for security control in customer configuration area in flash
// top sector.
//
//*****************************************************************************
const uint8_t g_pui8CcfgDefaultSec[] = {0xFF, 0xFF, 0xFF, 0xC5,
                                        0xFF, 0xFF, 0xFF, 0xFF,
                                        0xC5, 0xFF, 0xFF, 0xFF,
                                        0xC5, 0xC5, 0xC5, 0xFF,
                                        0xC5, 0xC5, 0xC5, 0xFF
                                       };

//*****************************************************************************
//
// Function prototypes for static functions
//
//*****************************************************************************
static void IssueFsmCommand(tFlashStateCommandsType eCommand);
static void EnableSectorsForWrite(void);
static uint32_t ScaleCycleValues(uint32_t ui32SpecifiedTiming,
                                 uint32_t ui32ScaleValue);
static void SetWriteMode(void);
static void TrimForWrite(void);
static void SetReadMode(void);

void FlashPowerModeSet(uint32_t ui32PowerMode, uint32_t ui32BankGracePeriod, uint32_t ui32PumpGracePeriod) {
    // Check the arguments.
    ASSERT(ui32PowerMode == FLASH_PWR_ACTIVE_MODE ||
           ui32PowerMode == FLASH_PWR_OFF_MODE    ||
           ui32PowerMode == FLASH_PWR_DEEP_STDBY_MODE);
    ASSERT(ui32BankGracePeriod <= 0xFF);
    ASSERT(ui32PumpGracePeriod <= 0xFFFF);

    switch(ui32PowerMode)
    {
    case FLASH_PWR_ACTIVE_MODE:
        // Set bank power mode to ACTIVE.
        HWREG(FLASH_BASE + FLASH_O_FBFALLBACK) =
            (HWREG(FLASH_BASE + FLASH_O_FBFALLBACK) &
             ~FLASH_FBFALLBACK_BANKPWR0_M) | FBFALLBACK_ACTIVE;

        // Set charge pump power mode to ACTIVE mode.
        HWREG(FLASH_BASE + FLASH_O_FPAC1) =
            (HWREG(FLASH_BASE + FLASH_O_FPAC1) & ~FLASH_FPAC1_PUMPPWR_M) | (1 << FLASH_FPAC1_PUMPPWR_S);
        break;

    case FLASH_PWR_OFF_MODE:
        // Set bank grace period.
        HWREG(FLASH_BASE + FLASH_O_FBAC) =
            (HWREG(FLASH_BASE + FLASH_O_FBAC) & (~FLASH_FBAC_BAGP_M)) |
            ((ui32BankGracePeriod << FLASH_FBAC_BAGP_S) & FLASH_FBAC_BAGP_M);

        // Set pump grace period.
        HWREG(FLASH_BASE + FLASH_O_FPAC2) =
            (HWREG(FLASH_BASE + FLASH_O_FPAC2) & (~FLASH_FPAC2_PAGP_M)) |
            ((ui32PumpGracePeriod << FLASH_FPAC2_PAGP_S) & FLASH_FPAC2_PAGP_M);

        // Set bank power mode to SLEEP.
        HWREG(FLASH_BASE + FLASH_O_FBFALLBACK) &= ~FLASH_FBFALLBACK_BANKPWR0_M;

        // Set charge pump power mode to SLEEP mode.
        HWREG(FLASH_BASE + FLASH_O_FPAC1) &= ~FLASH_FPAC1_PUMPPWR_M;
        break;

    case FLASH_PWR_DEEP_STDBY_MODE:
        // Set bank grace period.
        HWREG(FLASH_BASE + FLASH_O_FBAC) =
            (HWREG(FLASH_BASE + FLASH_O_FBAC) & (~FLASH_FBAC_BAGP_M)) |
            ((ui32BankGracePeriod << FLASH_FBAC_BAGP_S) & FLASH_FBAC_BAGP_M);

        // Set pump grace period.
        HWREG(FLASH_BASE + FLASH_O_FPAC2) =
            (HWREG(FLASH_BASE + FLASH_O_FPAC2) & (~FLASH_FPAC2_PAGP_M)) |
            ((ui32PumpGracePeriod << FLASH_FPAC2_PAGP_S) & FLASH_FPAC2_PAGP_M);

        // Set bank power mode to DEEP STANDBY mode.
        HWREG(FLASH_BASE + FLASH_O_FBFALLBACK) =
            (HWREG(FLASH_BASE + FLASH_O_FBFALLBACK) &
             ~FLASH_FBFALLBACK_BANKPWR0_M) | FBFALLBACK_DEEP_STDBY;

        // Set charge pump power mode to STANDBY mode.
        HWREG(FLASH_BASE + FLASH_O_FPAC1) |= FLASH_FPAC1_PUMPPWR_M;
        break;
    }
}

uint32_t FlashPowerModeGet(void) {
    uint32_t ui32PowerMode;
    uint32_t ui32BankPwrMode;

    ui32BankPwrMode = HWREG(FLASH_BASE + FLASH_O_FBFALLBACK) &
                      FLASH_FBFALLBACK_BANKPWR0_M;

    if(ui32BankPwrMode == FBFALLBACK_SLEEP)
    {
        ui32PowerMode = FLASH_PWR_OFF_MODE;
    }
    else if(ui32BankPwrMode == FBFALLBACK_DEEP_STDBY)
    {
        ui32PowerMode = FLASH_PWR_DEEP_STDBY_MODE;
    }
    else
    {
        ui32PowerMode = FLASH_PWR_ACTIVE_MODE;
    }

    // Return power mode.
    return(ui32PowerMode);
}

void FlashProtectionSet(uint32_t ui32SectorAddress, uint32_t ui32ProtectMode) {
    uint32_t ui32SectorNumber;

    // Check the arguments.
    ASSERT(ui32SectorAddress <= (FLASHMEM_BASE + FlashSizeGet() -
                                 FlashSectorSizeGet()));
    ASSERT((ui32SectorAddress & (FlashSectorSizeGet() - 1)) == 00);

    if(ui32ProtectMode == FLASH_WRITE_PROTECT)
    {
        ui32SectorNumber = (ui32SectorAddress - FLASHMEM_BASE) /
                           FlashSectorSizeGet();
        HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;

        if(ui32SectorNumber <= 31)
        {
            HWREG(FLASH_BASE + FLASH_O_FSM_BSLE0) |= (1 << ui32SectorNumber);
            HWREG(FLASH_BASE + FLASH_O_FSM_BSLP0) |= (1 << ui32SectorNumber);
        }
        else if(ui32SectorNumber <= 63)
        {
            HWREG(FLASH_BASE + FLASH_O_FSM_BSLE1) |=
                (1 << (ui32SectorNumber & 0x1F));
            HWREG(FLASH_BASE + FLASH_O_FSM_BSLP1) |=
                (1 << (ui32SectorNumber & 0x1F));
        }

        HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;
    }
}

uint32_t FlashProtectionGet(uint32_t ui32SectorAddress) {
    uint32_t ui32SectorProtect;
    uint32_t ui32SectorNumber;

    // Check the arguments.
    ASSERT(ui32SectorAddress <= (FLASHMEM_BASE + FlashSizeGet() -
                                 FlashSectorSizeGet()));
    ASSERT((ui32SectorAddress & (FlashSectorSizeGet() - 1)) == 00);

    ui32SectorProtect = FLASH_NO_PROTECT;
    ui32SectorNumber = (ui32SectorAddress - FLASHMEM_BASE) / FlashSectorSizeGet();

    if(ui32SectorNumber <= 31)
    {
        if((HWREG(FLASH_BASE + FLASH_O_FSM_BSLE0) & (1 << ui32SectorNumber)) &&
                (HWREG(FLASH_BASE + FLASH_O_FSM_BSLP0) & (1 << ui32SectorNumber)))
        {
            ui32SectorProtect = FLASH_WRITE_PROTECT;
        }
    }
    else if(ui32SectorNumber <= 63)
    {
        if((HWREG(FLASH_BASE + FLASH_O_FSM_BSLE1) &
                (1 << (ui32SectorNumber & 0x1F))) &&
                (HWREG(FLASH_BASE + FLASH_O_FSM_BSLP1) &
                 (1 << (ui32SectorNumber & 0x1F))))
        {
            ui32SectorProtect = FLASH_WRITE_PROTECT;
        }
    }

    return(ui32SectorProtect);
}

uint32_t FlashProtectionSave(uint32_t ui32SectorAddress) {
    uint32_t ui32ErrorReturn;
    uint32_t ui32SectorNumber;
    uint32_t ui32CcfgSectorAddr;
    uint32_t ui32ProgBuf;

    ui32ErrorReturn = FAPI_STATUS_SUCCESS;

    // Check the arguments.
    ASSERT(ui32SectorAddress <= (FLASHMEM_BASE + FlashSizeGet() -
                                 FlashSectorSizeGet()));
    ASSERT((ui32SectorAddress & (FlashSectorSizeGet() - 1)) == 00);

    if(FlashProtectionGet(ui32SectorAddress) == FLASH_WRITE_PROTECT)
    {
        // Find sector number for specified sector.
        ui32SectorNumber = (ui32SectorAddress - FLASHMEM_BASE) / FlashSectorSizeGet();
        ui32CcfgSectorAddr = FLASHMEM_BASE + FlashSizeGet() - FlashSectorSizeGet();

        // Adjust CCFG address to the 32-bit CCFG word holding the
        // protect-bit for the specified sector.
        ui32CcfgSectorAddr += (((ui32SectorNumber >> 5) * 4) + CCFG_OFFSET_SECT_PROT);

        // Find value to program by setting the protect-bit which
        // corresponds to specified sector number, to 0.
        // Leave other protect-bits unchanged.
        ui32ProgBuf = (~(1 << (ui32SectorNumber & 0x1F))) &
                                   *(uint32_t *)ui32CcfgSectorAddr;

        ui32ErrorReturn = FlashProgram((uint8_t*)&ui32ProgBuf, ui32CcfgSectorAddr,
                                       CCFG_SIZE_SECT_PROT);
    }

    // Return status.
    return(ui32ErrorReturn);
}

uint32_t FlashSectorErase(uint32_t ui32SectorAddress) {
    // The below code part constitutes the variant of the FlashSectorErase()
    // function that is located in ROM. The source code of this variant is not
    // visible in internal or external driverlib. The source code is
    // only compiled during a ROM build.
    // The two above code parts (seperated by compile switches) constitute wrapper
    // functions which both call this ROM variant of the function.
    // The ROM variant is called by referrencing it directly through the ROM API table.
    uint32_t ui32ErrorReturn;
    uint32_t ui32Error;
    uint32_t ui32SectorBit;
    uint32_t ui32SectorNumber;

    // Check the arguments.
    ASSERT(ui32SectorAddress <= (FLASHMEM_BASE + FlashSizeGet() -
                                 FlashSectorSizeGet()));
    ASSERT((ui32SectorAddress & (FlashSectorSizeGet() - 1)) == 00);

    // Enable all sectors for erase.
    EnableSectorsForWrite();

    // Check the arguments.
    if((ui32SectorAddress >
        (FLASHMEM_BASE + FlashSizeGet() - FlashSectorSizeGet())) ||
       ((ui32SectorAddress & (FlashSectorSizeGet() - 1)) != 00))
    {
        // Invalid arguments. Exit function!
        FlashDisableSectorsForWrite();
        return (FAPI_STATUS_INCORRECT_DATABUFFER_LENGTH);
    }

    // Clear the Status register.
    IssueFsmCommand(FAPI_CLEAR_STATUS);

    // Unprotect sector to be erased.
    ui32SectorNumber = (ui32SectorAddress - FLASHMEM_BASE) / FlashSectorSizeGet();
    ui32SectorBit = 1 << (ui32SectorNumber & 0x1F);
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    if(ui32SectorNumber < 0x20)
    {
        HWREG(FLASH_BASE + FLASH_O_FSM_SECTOR1) = ~ui32SectorBit;
    }
    else
    {
        HWREG(FLASH_BASE + FLASH_O_FSM_SECTOR2) = ~ui32SectorBit;
    }
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;

    // Write the address to the FSM.
    HWREG(FLASH_BASE + FLASH_O_FADDR) = ui32SectorAddress + ADDR_OFFSET;

    // Issue the sector erase command to the FSM.
    IssueFsmCommand(FAPI_ERASE_SECTOR);

    // Wait for erase to finish.
    while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
    {
    }

    // Update status.
    ui32ErrorReturn = FlashCheckFsmForError();

    // Disable sectors for erase.
    FlashDisableSectorsForWrite();

    // Check if flash top sector was erased.
    if(ui32SectorAddress == (FLASHMEM_BASE + FlashSizeGet() -
                             FlashSectorSizeGet()))
    {
        // Program security data to default values in the customer configuration
        // area within the flash top sector.
        ui32Error = FlashProgram((uint8_t *)g_pui8CcfgDefaultSec,
                                 (ui32SectorAddress + CCFG_OFFSET_SECURITY),
                                 CCFG_SIZE_SECURITY);

        if((ui32Error != FAPI_STATUS_SUCCESS) &&
                (ui32ErrorReturn == FAPI_STATUS_SUCCESS))
        {
            ui32ErrorReturn = ui32Error;
        }
}

    // Return status of operation.
    return(ui32ErrorReturn);
}

uint32_t FlashBankErase(bool bForcePrecondition) {
    uint32_t ui32ErrorReturn;
    uint32_t ui32SectorAddress;
    uint32_t ui32RegVal;

    // Enable all sectors for erase.
    EnableSectorsForWrite();

    // Clear the Status register.
    IssueFsmCommand(FAPI_CLEAR_STATUS);

    // Enable erase of all sectors and enable precondition if required.
    ui32RegVal = HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE);
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_SECTOR1) = 0x00000000;
    HWREG(FLASH_BASE + FLASH_O_FSM_SECTOR2) = 0x00000000;
    if(bForcePrecondition)
    {
        HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) |=
            FLASH_FSM_ST_MACHINE_DO_PRECOND;
    }
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;

    // Issue the bank erase command to the FSM.
    IssueFsmCommand(FAPI_ERASE_BANK);

    // Wait for erase to finish.
    while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
    {
    }

    // Update status.
    ui32ErrorReturn = FlashCheckFsmForError();

    // Disable sectors for erase.
    FlashDisableSectorsForWrite();

    // Set configured precondition mode since it may have been forced on.
    if(!(ui32RegVal & FLASH_FSM_ST_MACHINE_DO_PRECOND))
    {
        HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
        HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) &=
            ~FLASH_FSM_ST_MACHINE_DO_PRECOND;
        HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;
    }

    // Program security data to default values in the customer configuration
    // area within the flash top sector if erase was successful.
    if(ui32ErrorReturn == FAPI_STATUS_SUCCESS)
    {
        ui32SectorAddress = FLASHMEM_BASE + FlashSizeGet() - FlashSectorSizeGet();
        ui32ErrorReturn = FlashProgram((uint8_t *)g_pui8CcfgDefaultSec,
                                       (ui32SectorAddress + CCFG_OFFSET_SECURITY),
                                       CCFG_SIZE_SECURITY);
    }

    // Return status of operation.
    return(ui32ErrorReturn);
}

uint32_t FlashhOtpEngrErase(void) {
    uint32_t ui32ErrorReturn;
    uint32_t ui32RegVal;

    // Enable all sectors for erase.
    EnableSectorsForWrite();

    // Clear the Status register.
    IssueFsmCommand(FAPI_CLEAR_STATUS);

    // Disable OTP protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = FLASH_FBPROT_PROTL1DIS;
    HWREG(FLASH_BASE + FLASH_O_FBAC) |= FLASH_FBAC_OTPPROTDIS;
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = 0;

    // Enable test commands.
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;
    HWREG(FLASH_BASE + FLASH_O_FTCTL) |= FLASH_FTCTL_TEST_EN;
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;

    // Set address to OTP.
    HWREG(FLASH_BASE + FLASH_O_FADDR) = 0xF0000000;

    // Enable for FSM test commands and erase precondition.
    ui32RegVal = HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE);
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) |=
        (FLASH_FSM_ST_MACHINE_CMD_EN | FLASH_FSM_ST_MACHINE_DO_PRECOND);
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;

    // Issue the erase command to the FSM.
    IssueFsmCommand(FAPI_ERASE_OTP);

    // Wait for erase to finish.
    while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
    {
    }

    // Update status.
    ui32ErrorReturn = FlashCheckFsmForError();

    // Disable sectors for erase.
    FlashDisableSectorsForWrite();

    // Disable test commands.
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;
    HWREG(FLASH_BASE + FLASH_O_FTCTL) &= ~FLASH_FTCTL_TEST_EN;
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;

    // Renable OTP protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = FLASH_FBPROT_PROTL1DIS;
    HWREG(FLASH_BASE + FLASH_O_FBAC) &= ~FLASH_FBAC_OTPPROTDIS;
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = 0;

    // Disable FSM test command mode.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) &= ~FLASH_FSM_ST_MACHINE_CMD_EN;

    // Set configured precondition mode since it may have been changed.
    if(!(ui32RegVal & FLASH_FSM_ST_MACHINE_DO_PRECOND))
    {
        HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) &=
            ~FLASH_FSM_ST_MACHINE_DO_PRECOND;
    }
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;

    // Return status of operation.
    return(ui32ErrorReturn);
}

uint32_t FlashProgram(uint8_t *pui8DataBuffer, uint32_t ui32Address, uint32_t ui32Count) {
    // The below code part constitutes the variant of the FlashProgram() function
    // that is located in ROM. The source code of this variant is not visible in
    // internal or external driverlib. The source code is only compiled during a ROM
    // build.
    // The two above code parts (seperated by compile switches) constitute wrapper
    // functions which both call this ROM variant of the function.
    // The ROM variant is called by referrencing it directly through the ROM API table.
    uint32_t ui32StartIndex;
    uint32_t ui32StopIndex;
    uint32_t ui32Index;
    uint8_t ui8BankWidth;
    uint8_t ui8NoOfBytes;
    tFwpWriteByte *oFwpWriteByte;
    uint32_t ui32ErrorReturn;

    // Check the arguments.
    ASSERT((ui32Address + ui32Count) <= (FLASHMEM_BASE + FlashSizeGet()));

    // Enable sectors for programming.
    EnableSectorsForWrite();
    oFwpWriteByte = FWPWRITE_BYTE_ADDRESS;

    // Check the arguments.
    if((ui32Address + ui32Count) > (FLASHMEM_BASE + FlashSizeGet()))
    {
        // Invalid arguments. Exit function!
        FlashDisableSectorsForWrite();
        return (FAPI_STATUS_INCORRECT_DATABUFFER_LENGTH);
    }

    // Set the status to indicate success.
    ui32ErrorReturn = FAPI_STATUS_SUCCESS;

    // Find flash bank width in number of bytes.
    ui8BankWidth =
        (uint8_t)(((HWREG(FLASH_BASE + FLASH_O_FCFG_BANK) &
                    FLASH_FCFG_BANK_MAIN_BANK_WIDTH_M) >>
                   FLASH_FCFG_BANK_MAIN_BANK_WIDTH_S) >> 3);

    // Loop over the bytes to be programmed.
    while(ui32Count)
    {
        // Setup the start position within the write data registers.
        ui32StartIndex = ui32Address & (uint32_t)(ui8BankWidth - 1);

        // Setup number of bytes to program.
        ui8NoOfBytes = ui8BankWidth - ui32StartIndex;
        if(ui8NoOfBytes > ui32Count)
        {
            ui8NoOfBytes = ui32Count;
        }

        // Clear the Status register.
        IssueFsmCommand(FAPI_CLEAR_STATUS);

        // Write address to FADDR register.
        HWREG(FLASH_BASE + FLASH_O_FADDR) = ui32Address + ADDR_OFFSET;

        // Setup the stop position within the write data registers.
        ui32StopIndex = ui32StartIndex + (uint32_t)(ui8NoOfBytes - 1);

        // Write each byte to the FWPWrite registers.
        for(ui32Index = ui32StartIndex; ui32Index <= ui32StopIndex; ui32Index++)
        {
            oFwpWriteByte[ui32Index] = *(pui8DataBuffer++);
        }

        // Issue the Program command to the FSM.
        IssueFsmCommand(FAPI_PROGRAM_DATA);

        // Wait until the word has been programmed.
        while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
        {
        }

        // Exit if an access violation occurred.
        ui32ErrorReturn = FlashCheckFsmForError();
        if(ui32ErrorReturn != FAPI_STATUS_SUCCESS)
        {
            break;
        }

        // Prepare for next data burst.
        ui32Count   -= ((ui32StopIndex - ui32StartIndex) + 1);
        ui32Address += ((ui32StopIndex - ui32StartIndex) + 1);
    }

    // Disable sectors for programming.
    FlashDisableSectorsForWrite();

    // Return status of operation.
    return(ui32ErrorReturn);
}

uint32_t FlashProgramNowait(uint32_t ui32StartAddress, uint8_t *pui8DataBuffer, uint8_t ui8NoOfBytes) {
    uint32_t     ui32StartIndex;
    uint32_t     ui32StopIndex;
    uint32_t     ui32Index;
    uint32_t     ui32BankWidth;
    uint32_t     ui32ErrorReturn;
    tFwpWriteByte    *oFwpWriteByte;

    // Check the arguments.
    ASSERT((ui32StartAddress + ui8NoOfBytes) <= (FLASHMEM_BASE + FlashSizeGet()));

    // Enable sectors for programming.
    EnableSectorsForWrite();
    oFwpWriteByte = FWPWRITE_BYTE_ADDRESS;

    // Check the arguments.
    if((ui32StartAddress + ui8NoOfBytes) > (FLASHMEM_BASE + FlashSizeGet()))
    {
        // Invalid arguments. Exit function!
        FlashDisableSectorsForWrite();
        return (FAPI_STATUS_INCORRECT_DATABUFFER_LENGTH);
    }

    // Set status to indicate success
    ui32ErrorReturn  = FAPI_STATUS_SUCCESS;

    // Find flash bank width in number of bytes.
    ui32BankWidth = (((HWREG(FLASH_BASE + FLASH_O_FCFG_BANK) &
                       FLASH_FCFG_BANK_MAIN_BANK_WIDTH_M) >>
                      FLASH_FCFG_BANK_MAIN_BANK_WIDTH_S) >> 3);

    // Setup the start position within the write data registers.
    ui32StartIndex = ui32StartAddress & (ui32BankWidth - 1);

    // Check to see if there is more data in the buffer than the register.
    // width.
    if((ui8NoOfBytes == 0) || ((ui32StartIndex + ui8NoOfBytes) > ui32BankWidth))
    {
        ui32ErrorReturn = FAPI_STATUS_INCORRECT_DATABUFFER_LENGTH;
    }

    if(ui32ErrorReturn == FAPI_STATUS_SUCCESS)
    {
        // Clear the Status register.
        IssueFsmCommand(FAPI_CLEAR_STATUS);

        // Write address to FADDR register.
        HWREG(FLASH_BASE + FLASH_O_FADDR) = ui32StartAddress + ADDR_OFFSET;

        // Setup the stop position within the write data registers.
        ui32StopIndex  = ui32StartIndex + (uint32_t)(ui8NoOfBytes - 1);

        // Write each byte to the FWPWrite registers.
        for(ui32Index = ui32StartIndex; ui32Index <= ui32StopIndex; ui32Index++)
        {
            oFwpWriteByte[ui32Index] = *(pui8DataBuffer++);
}

        // Issue the Program command to the FSM.
        IssueFsmCommand(FAPI_PROGRAM_DATA);
    }

    // Return the function status.
    return(ui32ErrorReturn);
}

bool FlashEfuseReadRow(uint32_t *pui32EfuseData, uint32_t ui32RowAddress) {
    bool bStatus;

    // Make sure the clock for the efuse is enabled
    HWREG(FLASH_BASE + FLASH_O_CFG) &= ~FLASH_CFG_DIS_EFUSECLK;

    // Set timing for EFUSE read operations.
    HWREG(FLASH_BASE + FLASH_O_EFUSEREAD) |= ((5 << FLASH_EFUSEREAD_READCLOCK_S) &
            FLASH_EFUSEREAD_READCLOCK_M);

    // Clear status register.
    HWREG(FLASH_BASE + FLASH_O_EFUSEERROR) = 0;

    // Select the FuseROM block 0.
    HWREG(FLASH_BASE + FLASH_O_EFUSEADDR) = 0x00000000;

    // Start the read operation.
    HWREG(FLASH_BASE + FLASH_O_EFUSE) =
        (DUMPWORD_INSTR << FLASH_EFUSE_INSTRUCTION_S) |
        (ui32RowAddress & FLASH_EFUSE_DUMPWORD_M);

    // Wait for operation to finish.
    while(!(HWREG(FLASH_BASE + FLASH_O_EFUSEERROR) & FLASH_EFUSEERROR_DONE))
    {
    }

    // Check if error reported.
    if(HWREG(FLASH_BASE + FLASH_O_EFUSEERROR) & FLASH_EFUSEERROR_CODE_M)
    {
        // Set error status.
        bStatus = 1;

        // Clear data.
        *pui32EfuseData = 0;
    }
    else
    {
        // Set ok status.
        bStatus = 0;

        // No error. Get data from data register.
        *pui32EfuseData = HWREG(FLASH_BASE + FLASH_O_DATALOWER);
    }

    // Disable the efuse clock to conserve power
    HWREG(FLASH_BASE + FLASH_O_CFG) |= FLASH_CFG_DIS_EFUSECLK;

    // Return the data.
    return(bStatus);
}

uint32_t FlashProgramPattern(uint32_t ui32SectorAddress, uint32_t ui32DataPattern, bool bInvertData) {
    uint8_t ui8Index;
    uint8_t ui8BankWidth;
    tFwpWriteByte *oFwpWriteByte;
    uint32_t ui32ErrorReturn;

    // Check the arguments.
    ASSERT(ui32SectorAddress <= (FLASHMEM_BASE + FlashSizeGet() -
                                 FlashSectorSizeGet()));
    ASSERT((ui32SectorAddress & (FlashSectorSizeGet() - 1)) == 00);

    // Enable sectors for programming.
    EnableSectorsForWrite();
    oFwpWriteByte = FWPWRITE_BYTE_ADDRESS;

    // Check the arguments.
    if((ui32SectorAddress >
       (FLASHMEM_BASE + FlashSizeGet() - FlashSectorSizeGet())) ||
       ((ui32SectorAddress & (FlashSectorSizeGet() - 1)) != 00))
    {
        // Invalid arguments. Exit function!
        FlashDisableSectorsForWrite();
        return (FAPI_STATUS_INCORRECT_DATABUFFER_LENGTH);
    }

    // Find flash bank width in number of bytes.
    ui8BankWidth =
        (uint8_t)(((HWREG(FLASH_BASE + FLASH_O_FCFG_BANK) &
                    FLASH_FCFG_BANK_MAIN_BANK_WIDTH_M) >>
                   FLASH_FCFG_BANK_MAIN_BANK_WIDTH_S) >> 3);

    // Clear the Status register.
    IssueFsmCommand(FAPI_CLEAR_STATUS);

    // Write address to FADDR register.
    HWREG(FLASH_BASE + FLASH_O_FADDR) = ui32SectorAddress + ADDR_OFFSET;

    // Write each byte of the pattern to the FWPWrite registers.
    for(ui8Index = 0; ui8Index < ui8BankWidth; ui8Index++)
    {
        oFwpWriteByte[ui8Index] = ui32DataPattern >> ((ui8Index * 8) &
                                  (PATTERN_BITS - 1));
    }

    // Enable for FSM test command and enable the Invert Data option if
    // required.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) |= FLASH_FSM_ST_MACHINE_CMD_EN;
    if(bInvertData)
    {
        HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) |= FLASH_FSM_ST_MACHINE_INV_DATA;
    }
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;

    // Issue the Program command to the FSM.
    IssueFsmCommand(FAPI_PROGRAM_SECTOR);

    // Wait until the sector has been programmed.
    while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
    {
    }

    // Update status of the program operation.
    ui32ErrorReturn = FlashCheckFsmForError();

    // Disable sectors for programming.
    FlashDisableSectorsForWrite();

    // Disable FSM test command mode and the Invert Data option.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) &= ~FLASH_FSM_ST_MACHINE_CMD_EN;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) &= ~FLASH_FSM_ST_MACHINE_INV_DATA;
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;

    // Return status of operation.
    return(ui32ErrorReturn);
}

uint32_t FlashProgramEngr(uint8_t *pui8DataBuffer, uint32_t ui32AddressOffset, uint32_t ui32Count) {
    uint32_t ui32StartIndex;
    uint32_t ui32StopIndex;
    uint32_t ui32Index;
    uint8_t ui8BankWidth;
    uint8_t ui8NoOfBytes;
    tFwpWriteByte *oFwpWriteByte;
    uint32_t ui32ErrorReturn;

    // Check the arguments.
    ASSERT((ui32AddressOffset + ui32Count) <= 2048);
    // Enable sectors for programming.
    EnableSectorsForWrite();
    oFwpWriteByte = FWPWRITE_BYTE_ADDRESS;

    // Check the arguments.
    if((ui32AddressOffset + ui32Count) > 2048)
    {
        // Invalid arguments. Exit function!
        FlashDisableSectorsForWrite();
        return (FAPI_STATUS_INCORRECT_DATABUFFER_LENGTH);
    }

    // Set the status to indicate success.
    ui32ErrorReturn = FAPI_STATUS_SUCCESS;

    // Find flash bank width in number of bytes.
    ui8BankWidth =
        (uint8_t)(((HWREG(FLASH_BASE + FLASH_O_FCFG_BANK) &
                    FLASH_FCFG_BANK_MAIN_BANK_WIDTH_M) >>
                   FLASH_FCFG_BANK_MAIN_BANK_WIDTH_S) >> 3);

    // Disable OTP protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = FLASH_FBPROT_PROTL1DIS;
    HWREG(FLASH_BASE + FLASH_O_FBAC) |= FLASH_FBAC_OTPPROTDIS;
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = 0;

    // Enable test commands.
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;
    HWREG(FLASH_BASE + FLASH_O_FTCTL) |= FLASH_FTCTL_TEST_EN;
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;

    // Enable for FSM test command.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) |= FLASH_FSM_ST_MACHINE_CMD_EN;
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;

    // Loop over the bytes to be programmed.
    while(ui32Count)
    {
        // Setup the start position within the write data registers.
        ui32StartIndex = ui32AddressOffset & (uint32_t)(ui8BankWidth - 1);

        // Setup number of bytes to program.
        ui8NoOfBytes = ui8BankWidth - ui32StartIndex;
        if(ui8NoOfBytes > ui32Count)
        {
            ui8NoOfBytes = ui32Count;
        }

        // Clear the Status register.
        IssueFsmCommand(FAPI_CLEAR_STATUS);

        // Write address to FADDR register.
        HWREG(FLASH_BASE + FLASH_O_FADDR) = ui32AddressOffset + 0xF0080000;

        // Setup the stop position within the write data registers.
        ui32StopIndex = ui32StartIndex + (uint32_t)(ui8NoOfBytes - 1);

        // Write each byte to the FWPWrite registers.
        for(ui32Index = ui32StartIndex; ui32Index <= ui32StopIndex; ui32Index++)
        {
            oFwpWriteByte[ui32Index] = *(pui8DataBuffer++);
        }

        // Issue programming command.
        IssueFsmCommand(FAPI_PROGRAM_DATA);

        // Wait until the word has been programmed.
        while(FlashCheckFsmForReady() == FAPI_STATUS_FSM_BUSY)
        {
        }

        // Update error status and exit if an error occurred.
        ui32ErrorReturn = FlashCheckFsmForError();
        if(ui32ErrorReturn != FAPI_STATUS_SUCCESS)
        {
            break;
        }

        // Prepare for next data burst.
        ui32Count   -= ((ui32StopIndex - ui32StartIndex) + 1);
        ui32AddressOffset += ((ui32StopIndex - ui32StartIndex) + 1);
    }

    // Disable sectors for programming.
    FlashDisableSectorsForWrite();

    // Re-enable OTP protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = FLASH_FBPROT_PROTL1DIS;
    HWREG(FLASH_BASE + FLASH_O_FBAC) &= ~FLASH_FBAC_OTPPROTDIS;
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = 0;

    // Disable test commands.
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;
    HWREG(FLASH_BASE + FLASH_O_FTCTL) &= ~FLASH_FTCTL_TEST_EN;
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;

    // Disable FSM test command mode.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) &= ~FLASH_FSM_ST_MACHINE_CMD_EN;
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;

    // Return status of operation.
    return(ui32ErrorReturn);
}

void FlashOtpProgramEraseSetup(void) {
    // Disable OTP protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = FLASH_FBPROT_PROTL1DIS;
    HWREG(FLASH_BASE + FLASH_O_FBAC) |= FLASH_FBAC_OTPPROTDIS;
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = 0;

    // Enable test commands by performing the following steps:
    // - Enable SW Interface mode
    // - Enable for test commands
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x0000AAAA;
    HWREG(FLASH_BASE + FLASH_O_FTCTL) |= FLASH_FTCTL_TEST_EN;
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x000055AA;

    // Enable for FSM test commands.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) |= FLASH_FSM_ST_MACHINE_CMD_EN;
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;
}

void FlashOtpProgramEraseCleanup(void) {
    // Re-enable OTP protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = FLASH_FBPROT_PROTL1DIS;
    HWREG(FLASH_BASE + FLASH_O_FBAC) &= ~FLASH_FBAC_OTPPROTDIS;
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = 0;

    // Disable test commands and turn off SW interface mode.
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x0000AAAA;
    HWREG(FLASH_BASE + FLASH_O_FTCTL) &= ~FLASH_FTCTL_TEST_EN;
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;

    // Disable FSM test command mode.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) &= ~FLASH_FSM_ST_MACHINE_CMD_EN;
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;
}

void FlashDisableSectorsForWrite(void) {
    // Configure flash back to read mode
    SetReadMode();

    // Disable Level 1 Protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = FLASH_FBPROT_PROTL1DIS;

    // Disable all sectors for erase and programming.
    HWREG(FLASH_BASE + FLASH_O_FBSE) = 0x0000;

    // Enable Level 1 Protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = 0;

    // Protect sectors from sector erase.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;
    HWREG(FLASH_BASE + FLASH_O_FSM_SECTOR1) = 0xFFFFFFFF;
    HWREG(FLASH_BASE + FLASH_O_FSM_SECTOR2) = 0xFFFFFFFF;
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;
}
//*****************************************************************************
//
//! \internal
//! Issues a command to the Flash State Machine.
//!
//! \param eCommand specifies the FSM command.
//!
//! Issues a command to the Flash State Machine.
//!
//! \return None
//
//*****************************************************************************
static void
IssueFsmCommand(tFlashStateCommandsType eCommand)
{
    // Check the arguments.
    ASSERT(
        eCommand == FAPI_ERASE_SECTOR    || eCommand == FAPI_ERASE_BANK ||
        eCommand == FAPI_VALIDATE_SECTOR || eCommand == FAPI_CLEAR_STATUS ||
        eCommand == FAPI_PROGRAM_RESUME  || eCommand == FAPI_ERASE_RESUME ||
        eCommand == FAPI_CLEAR_MORE      || eCommand == FAPI_PROGRAM_SECTOR ||
        eCommand == FAPI_PROGRAM_DATA    || eCommand == FAPI_ERASE_OTP);

    // Enable write to FSM register.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;

    // Issue FSM command.
    HWREG(FLASH_BASE + FLASH_O_FSM_CMD) = eCommand;

    // Start command execute.
    HWREG(FLASH_BASE + FLASH_O_FSM_EXECUTE) = FLASH_CMD_EXEC;

    // Disable write to FSM register.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;
}

//*****************************************************************************
//
//! \internal
//! Enables all sectors for erase and programming on the active bank.
//!
//! This function disables the idle reading power reduction mode, selects the
//! flash bank and enables all sectors for erase and programming on the active
//! bank.
//! Sectors may be protected from programming depending on the value of the
//! FLASH_O_FSM_BSLPx registers.
//! Sectors may be protected from erase depending on the value of the
//! FLASH_O_FSM_BSLEx registers. Additional sector erase protection is set by
//! the FLASH_O_FSM_SECTOR1 register.
//!
//! \return None
//
//*****************************************************************************
static void
EnableSectorsForWrite(void)
{
    // Trim flash module for program/erase operation.
    TrimForWrite();

    // Configure flash to write mode
    SetWriteMode();

    // Select flash bank.
    HWREG(FLASH_BASE + FLASH_O_FMAC) = 0x00;

    // Disable Level 1 Protection.
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = FLASH_FBPROT_PROTL1DIS;

    // Enable all sectors for erase and programming.
    HWREG(FLASH_BASE + FLASH_O_FBSE) = 0xFFFF;

    // Enable Level 1 Protection
    HWREG(FLASH_BASE + FLASH_O_FBPROT) = 0;
}

//*****************************************************************************
//
//! \internal
//! Trims the Flash Bank and Flash Pump for program/erase functionality
//!
//! This trimming will make it possible to perform erase and program operations
//! of the flash. Trim values are loaded from factory configuration area
//! (referred to as FCGF1). The trimming done by this function is valid until
//! reset of the flash module.
//!
//! Some registers shall be written with a value that is a number of FCLK
//! cycles. The trim values controlling these registers have a value of
//! number of half us. FCLK = SysClk / ((RWAIT+1) x 2).
//!
//! In order to calculate the register value for these registers the
//! following calculation must be done:
//!
//!                                        OtpValue                   SysClkMHz
//!                                        -------- us      OtpValue x ---------
//!                                           2                       (RWAIT+1)
//!     RegValue_in_no_of_clk_cycles = ----------------- = ---------------------
//!                                           1                     4
//!                                     --------------
//!                                       SysClkMHz
//!                                      ------------
//!                                      (RWAIT+1)x 2
//!
//! This is equivalent to:
//!
//!                                               16 x SysClkMHz
//!                                    OtpValue x ---------------
//!                                                  (RWAIT+1)
//!     RegValue_in_no_of_clk_cycles = ----------------------------
//!                                               64
//!
//! A scaling factor is set equal to:
//!
//!                     16 x SysClkMHz
//!     ui32FclkScale = --------------
//!                       (RWAIT+1)
//!
//! which gives:
//!
//!                                     OtpValue x ui32FclkScale
//!     RegValue_in_no_of_clk_cycles = ------------------------
//!                                              64
//!
//! \return None.
//
//*****************************************************************************
static void
TrimForWrite(void)
{
    uint32_t ui32Value;
    uint32_t ui32TempVal;
    uint32_t ui32FclkScale;
    uint32_t ui32RWait;

    // Return if flash is already trimmed for program/erase operations.
    if(HWREG(FLASH_BASE + FLASH_O_FWFLAG) & FW_WRT_TRIMMED)
    {
        return;
    }

    //***********************************************************************//
    //                                                                       //
    //                 Configure the FSM registers                           //
    //                                                                       //
    //***********************************************************************//

    // Enable access to the FSM registers.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_ENABLE;

    // Determine the scaling value to be used on timing related trim values.
    // The scaling value is based on the flash module clock frequency and RWAIT
    ui32RWait     = (HWREG(FLASH_BASE + FLASH_O_FRDCTL) &
                     FLASH_FRDCTL_RWAIT_M) >> FLASH_FRDCTL_RWAIT_S;
    ui32FclkScale = (16 * FLASH_MODULE_CLK_FREQ) / (ui32RWait + 1);

    // Configure Program pulse width bits 15:0.
    // (FCFG1 offset 0x188 bits 15:0).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_PROG_EP) &
         FCFG1_FLASH_PROG_EP_PROGRAM_PW_M) >>
         FCFG1_FLASH_PROG_EP_PROGRAM_PW_S;

    ui32Value = ScaleCycleValues(ui32Value, ui32FclkScale);

    HWREG(FLASH_BASE + FLASH_O_FSM_PRG_PW) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_PRG_PW) &
         ~FLASH_FSM_PRG_PW_PROG_PUL_WIDTH_M) |
        ((ui32Value << FLASH_FSM_PRG_PW_PROG_PUL_WIDTH_S) &
         FLASH_FSM_PRG_PW_PROG_PUL_WIDTH_M);

    // Configure Erase pulse width bits 31:0.
    // (FCFG1 offset 0x18C bits 31:0).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_ERA_PW) &
         FCFG1_FLASH_ERA_PW_ERASE_PW_M) >>
         FCFG1_FLASH_ERA_PW_ERASE_PW_S;

    ui32Value = ScaleCycleValues(ui32Value, ui32FclkScale);

    HWREG(FLASH_BASE + FLASH_O_FSM_ERA_PW) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_ERA_PW) &
         ~FLASH_FSM_ERA_PW_FSM_ERA_PW_M) |
        ((ui32Value << FLASH_FSM_ERA_PW_FSM_ERA_PW_S) &
         FLASH_FSM_ERA_PW_FSM_ERA_PW_M);


    // Configure no of flash clock cycles from EXECUTEZ going low to the
    // verify data can be read in the program verify mode bits 7:0.
    // (FCFG1 offset 0x174 bits 23:16).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_C_E_P_R) &
         FCFG1_FLASH_C_E_P_R_PV_ACCESS_M) >>
         FCFG1_FLASH_C_E_P_R_PV_ACCESS_S;

    ui32Value = ScaleCycleValues(ui32Value, ui32FclkScale);

    HWREG(FLASH_BASE + FLASH_O_FSM_EX_VAL) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_EX_VAL) &
         ~FLASH_FSM_EX_VAL_EXE_VALD_M) |
        ((ui32Value << FLASH_FSM_EX_VAL_EXE_VALD_S) &
         FLASH_FSM_EX_VAL_EXE_VALD_M);

    // Configure the number of flash clocks from the start of the Read mode at
    // the end of the operations until the FSM clears the BUSY bit in FMSTAT.
    // (FCFG1 offset 0x178 bits 23:16).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_P_R_PV) &
         FCFG1_FLASH_P_R_PV_RH_M) >>
         FCFG1_FLASH_P_R_PV_RH_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_RD_H) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_RD_H) &
         ~FLASH_FSM_RD_H_RD_H_M) |
        ((ui32Value << FLASH_FSM_RD_H_RD_H_S) &
         FLASH_FSM_RD_H_RD_H_M);

    // Configure Program hold time
    // (FCFG1 offset 0x178 bits 31:24).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_P_R_PV) &
         FCFG1_FLASH_P_R_PV_PH_M) >>
         FCFG1_FLASH_P_R_PV_PH_S;

    ui32Value = ScaleCycleValues(ui32Value, ui32FclkScale);

    HWREG(FLASH_BASE + FLASH_O_FSM_P_OH) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_P_OH) &
         ~FLASH_FSM_P_OH_PGM_OH_M) |
        ((ui32Value << FLASH_FSM_P_OH_PGM_OH_S) &
         FLASH_FSM_P_OH_PGM_OH_M);

    // Configure Erase hold time
    // (FCFG1 offset 0x17C bits 31:24).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_EH_SEQ) &
         FCFG1_FLASH_EH_SEQ_EH_M) >>
         FCFG1_FLASH_EH_SEQ_EH_S;

    ui32Value = ScaleCycleValues(ui32Value, ui32FclkScale);

    HWREG(FLASH_BASE + FLASH_O_FSM_ERA_OH) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_ERA_OH) &
         ~FLASH_FSM_ERA_OH_ERA_OH_M) |
        ((ui32Value << FLASH_FSM_ERA_OH_ERA_OH_S) &
         FLASH_FSM_ERA_OH_ERA_OH_M);

    // Configure Program verify row switch time
    // (FCFG1 offset0x178 bits 15:8).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_P_R_PV) &
         FCFG1_FLASH_P_R_PV_PVH_M) >>
         FCFG1_FLASH_P_R_PV_PVH_S;

    ui32Value = ScaleCycleValues(ui32Value, ui32FclkScale);

    HWREG(FLASH_BASE + FLASH_O_FSM_PE_VH) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_PE_VH) &
         ~FLASH_FSM_PE_VH_PGM_VH_M) |
        ((ui32Value << FLASH_FSM_PE_VH_PGM_VH_S) &
         FLASH_FSM_PE_VH_PGM_VH_M);

    // Configure Program Operation Setup time
    // (FCFG1 offset 0x170 bits 31:24).
    ui32Value = (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_E_P) &
                 FCFG1_FLASH_E_P_PSU_M) >>
                 FCFG1_FLASH_E_P_PSU_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_PE_OSU) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_PE_OSU) &
         ~FLASH_FSM_PE_OSU_PGM_OSU_M) |
        ((ui32Value << FLASH_FSM_PE_OSU_PGM_OSU_S) &
         FLASH_FSM_PE_OSU_PGM_OSU_M);

    // Configure Erase Operation Setup time
    // (FCGF1 offset 0x170 bits 23:16).
    ui32Value = (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_E_P) &
                 FCFG1_FLASH_E_P_ESU_M) >>
                 FCFG1_FLASH_E_P_ESU_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_PE_OSU) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_PE_OSU) &
         ~FLASH_FSM_PE_OSU_ERA_OSU_M) |
        ((ui32Value << FLASH_FSM_PE_OSU_ERA_OSU_S) &
         FLASH_FSM_PE_OSU_ERA_OSU_M);

    // Configure Program Verify Setup time
    // (FCFG1 offset 0x170 bits 15:8).
    ui32Value = (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_E_P) &
                 FCFG1_FLASH_E_P_PVSU_M) >>
                 FCFG1_FLASH_E_P_PVSU_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_PE_VSU) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_PE_VSU) &
         ~FLASH_FSM_PE_VSU_PGM_VSU_M) |
        ((ui32Value << FLASH_FSM_PE_VSU_PGM_VSU_S) &
         FLASH_FSM_PE_VSU_PGM_VSU_M);

    // Configure Erase Verify Setup time
    // (FCFG1 offset 0x170 bits 7:0).
    ui32Value = (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_E_P) &
                 FCFG1_FLASH_E_P_EVSU_M) >>
                 FCFG1_FLASH_E_P_EVSU_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_PE_VSU) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_PE_VSU) &
         ~FLASH_FSM_PE_VSU_ERA_VSU_M) |
        ((ui32Value << FLASH_FSM_PE_VSU_ERA_VSU_S) &
         FLASH_FSM_PE_VSU_ERA_VSU_M);

    // Configure Addr to EXECUTEZ low setup time
    // (FCFG1 offset 0x174 bits 15:12).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_C_E_P_R) &
         FCFG1_FLASH_C_E_P_R_A_EXEZ_SETUP_M) >>
         FCFG1_FLASH_C_E_P_R_A_EXEZ_SETUP_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_CMP_VSU) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_CMP_VSU) &
         ~FLASH_FSM_CMP_VSU_ADD_EXZ_M) |
        ((ui32Value << FLASH_FSM_CMP_VSU_ADD_EXZ_S) &
         FLASH_FSM_CMP_VSU_ADD_EXZ_M);

    // Configure Voltage Status Count
    // (FCFG1 offset 0x17C bits 15:12).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_EH_SEQ) &
         FCFG1_FLASH_EH_SEQ_VSTAT_M) >>
         FCFG1_FLASH_EH_SEQ_VSTAT_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_VSTAT) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_VSTAT) &
         ~FLASH_FSM_VSTAT_VSTAT_CNT_M) |
        ((ui32Value << FLASH_FSM_VSTAT_VSTAT_CNT_S) &
         FLASH_FSM_VSTAT_VSTAT_CNT_M);

    // Configure Repeat Verify action setup
    // (FCFG1 offset 0x174 bits 31:24).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_C_E_P_R) &
         FCFG1_FLASH_C_E_P_R_RVSU_M) >>
         FCFG1_FLASH_C_E_P_R_RVSU_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_EX_VAL) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_EX_VAL) &
         ~FLASH_FSM_EX_VAL_REP_VSU_M) |
        ((ui32Value << FLASH_FSM_EX_VAL_REP_VSU_S) &
         FLASH_FSM_EX_VAL_REP_VSU_M);

    // Configure Maximum Programming Pulses
    // (FCFG1 offset 0x184 bits 15:0).
    ui32Value = (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_PP) &
                 FCFG1_FLASH_PP_MAX_PP_M) >>
                 FCFG1_FLASH_PP_MAX_PP_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_PRG_PUL) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_PRG_PUL) &
         ~FLASH_FSM_PRG_PUL_MAX_PRG_PUL_M) |
        ((ui32Value << FLASH_FSM_PRG_PUL_MAX_PRG_PUL_S) &
         FLASH_FSM_PRG_PUL_MAX_PRG_PUL_M);

    // Configure Beginning level for VHVCT used during erase modes
    // (FCFG1 offset 0x180 bits 31:16).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_VHV_E) &
         FCFG1_FLASH_VHV_E_VHV_E_START_M) >>
         FCFG1_FLASH_VHV_E_VHV_E_START_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_PRG_PUL) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_PRG_PUL) &
         ~FLASH_FSM_PRG_PUL_BEG_EC_LEVEL_M) |
        ((ui32Value << FLASH_FSM_PRG_PUL_BEG_EC_LEVEL_S) &
         FLASH_FSM_PRG_PUL_BEG_EC_LEVEL_M);

    // Configure Maximum EC Level
    // (FCFG1 offset 0x2B0 bits 21:18).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_OTP_DATA3) &
         FCFG1_FLASH_OTP_DATA3_MAX_EC_LEVEL_M) >>
         FCFG1_FLASH_OTP_DATA3_MAX_EC_LEVEL_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_ERA_PUL) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_ERA_PUL) &
         ~FLASH_FSM_ERA_PUL_MAX_EC_LEVEL_M) |
        ((ui32Value << FLASH_FSM_ERA_PUL_MAX_EC_LEVEL_S) &
         FLASH_FSM_ERA_PUL_MAX_EC_LEVEL_M);

    // Configure Maximum Erase Pulses
    // (FCFG1 offset 0x188 bits 31:16).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_PROG_EP) &
         FCFG1_FLASH_PROG_EP_MAX_EP_M) >>
         FCFG1_FLASH_PROG_EP_MAX_EP_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_ERA_PUL) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_ERA_PUL) &
         ~FLASH_FSM_ERA_PUL_MAX_ERA_PUL_M) |
        ((ui32Value << FLASH_FSM_ERA_PUL_MAX_ERA_PUL_S) &
         FLASH_FSM_ERA_PUL_MAX_ERA_PUL_M);

    // Configure the VHVCT Step Size. This is the number of erase pulses that
    // must be completed for each level before the FSM increments the
    // CUR_EC_LEVEL to the next higher level. Actual erase pulses per level
    // equals (EC_STEP_SIZE +1). The stepping is only needed for the VHVCT
    // voltage.
    // (FCFG1 offset 0x2B0 bits 31:23).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_OTP_DATA3) &
         FCFG1_FLASH_OTP_DATA3_EC_STEP_SIZE_M) >>
         FCFG1_FLASH_OTP_DATA3_EC_STEP_SIZE_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_STEP_SIZE) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_STEP_SIZE) &
         ~FLASH_FSM_STEP_SIZE_EC_STEP_SIZE_M) |
        ((ui32Value << FLASH_FSM_STEP_SIZE_EC_STEP_SIZE_S) &
         FLASH_FSM_STEP_SIZE_EC_STEP_SIZE_M);

    // Configure the hight of each EC step. This is the number of counts that
    // the CUR_EC_LEVEL will increment when going to a new level. Actual count
    // size equals (EC_STEP_HEIGHT + 1). The stepping applies only to the VHVCT
    // voltage.
    // The read trim value is decremented by 1 before written to the register
    // since actual counts equals (register value + 1).
    // (FCFG1 offset 0x180 bits 15:0).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_VHV_E) &
         FCFG1_FLASH_VHV_E_VHV_E_STEP_HIGHT_M) >>
         FCFG1_FLASH_VHV_E_VHV_E_STEP_HIGHT_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_EC_STEP_HEIGHT) = ((ui32Value - 1) &
            FLASH_FSM_EC_STEP_HEIGHT_EC_STEP_HEIGHT_M);

    // Configure Precondition used in erase operations
    // (FCFG1 offset 0x2B0 bit 22).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_OTP_DATA3) &
         FCFG1_FLASH_OTP_DATA3_DO_PRECOND_M) >>
         FCFG1_FLASH_OTP_DATA3_DO_PRECOND_S;

    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) =
        (HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) &
         ~FLASH_FSM_ST_MACHINE_DO_PRECOND_M) |
        ((ui32Value << FLASH_FSM_ST_MACHINE_DO_PRECOND_S) &
         FLASH_FSM_ST_MACHINE_DO_PRECOND_M);

    // Enable the recommended Good Time function.
    HWREG(FLASH_BASE + FLASH_O_FSM_ST_MACHINE) |=
        FLASH_FSM_ST_MACHINE_ONE_TIME_GOOD;

    // Disable write access to FSM registers.
    HWREG(FLASH_BASE + FLASH_O_FSM_WR_ENA) = FSM_REG_WRT_DISABLE;


    //***********************************************************************//
    //                                                                       //
    //                 Configure the voltage registers                       //
    //                                                                       //
    //***********************************************************************//

    // Unlock voltage registers (0x2080 - 0x2098).
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;

    // Configure voltage level for the specified pump voltage of high
    // voltage supply input during erase operation VHVCT_E and the TRIM13_E
    // (FCFG1 offset 0x190 bits[3:0] and bits[11:8]).
    ui32TempVal = HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_VHV);

    ui32Value  = ((ui32TempVal & FCFG1_FLASH_VHV_TRIM13_E_M)>>
                  FCFG1_FLASH_VHV_TRIM13_E_S) << FLASH_FVHVCT1_TRIM13_E_S;
    ui32Value |= ((ui32TempVal & FCFG1_FLASH_VHV_VHV_E_M)>>
                  FCFG1_FLASH_VHV_VHV_E_S) << FLASH_FVHVCT1_VHVCT_E_S;

    HWREG(FLASH_BASE + FLASH_O_FVHVCT1) = (HWREG(FLASH_BASE + FLASH_O_FVHVCT1) &
         ~(FLASH_FVHVCT1_TRIM13_E_M | FLASH_FVHVCT1_VHVCT_E_M)) | ui32Value;

    // Configure voltage level for the specified pump voltage of high voltage
    // supply input during program verify operation VHVCT_PV and the TRIM13_PV
    // (OTP offset 0x194 bits[19:16] and bits[27:24]).
    ui32TempVal =
             HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_VHV_PV);

    ui32Value  = ((ui32TempVal & FCFG1_FLASH_VHV_PV_TRIM13_PV_M)>>
                  FCFG1_FLASH_VHV_PV_TRIM13_PV_S) <<
                                                     FLASH_FVHVCT1_TRIM13_PV_S;
    ui32Value |= ((ui32TempVal & FCFG1_FLASH_VHV_PV_VHV_PV_M)>>
                  FCFG1_FLASH_VHV_PV_VHV_PV_S) <<
                                                     FLASH_FVHVCT1_VHVCT_PV_S;

    HWREG(FLASH_BASE + FLASH_O_FVHVCT1) = (HWREG(FLASH_BASE + FLASH_O_FVHVCT1) &
         ~(FLASH_FVHVCT1_TRIM13_PV_M | FLASH_FVHVCT1_VHVCT_PV_M)) | ui32Value;

    // Configure voltage level for the specified pump voltage of high voltage
    // supply input during program operation VHVCT_P and TRIM13_P
    // (FCFG1 offset 0x190 bits[19:16] and bits[27:24]).
    ui32TempVal =
                HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_VHV);

    ui32Value = ((ui32TempVal & FCFG1_FLASH_VHV_TRIM13_P_M)>>
                  FCFG1_FLASH_VHV_TRIM13_P_S) << FLASH_FVHVCT2_TRIM13_P_S;
    ui32Value |= ((ui32TempVal & FCFG1_FLASH_VHV_VHV_P_M)>>
                  FCFG1_FLASH_VHV_VHV_P_S) << FLASH_FVHVCT2_VHVCT_P_S;

    HWREG(FLASH_BASE + FLASH_O_FVHVCT2) =
        (HWREG(FLASH_BASE + FLASH_O_FVHVCT2) &
         ~(FLASH_FVHVCT2_TRIM13_P_M | FLASH_FVHVCT2_VHVCT_P_M)) | ui32Value;

    // Configure voltage level for the specified pump voltage of wordline power
    // supply for read mode
    // (FCFG1 offset 0x198 Bits 15:8).
    ui32Value = (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_V) &
                 FCFG1_FLASH_V_V_READ_M) >>
                 FCFG1_FLASH_V_V_READ_S;

    HWREG(FLASH_BASE + FLASH_O_FVREADCT) =
        (HWREG(FLASH_BASE + FLASH_O_FVREADCT) &
         ~FLASH_FVREADCT_VREADCT_M) |
        ((ui32Value << FLASH_FVREADCT_VREADCT_S) &
         FLASH_FVREADCT_VREADCT_M);

    // Configure the voltage level for the VCG 2.5 CT pump voltage
    // (FCFG1 offset 0x194 bits 15:8).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_VHV_PV) &
         FCFG1_FLASH_VHV_PV_VCG2P5_M) >>
         FCFG1_FLASH_VHV_PV_VCG2P5_S;

    HWREG(FLASH_BASE + FLASH_O_FVNVCT) =
        (HWREG(FLASH_BASE + FLASH_O_FVNVCT) &
         ~FLASH_FVNVCT_VCG2P5CT_M) |
        ((ui32Value << FLASH_FVNVCT_VCG2P5CT_S) &
         FLASH_FVNVCT_VCG2P5CT_M);

    // Configure the voltage level for the specified pump voltage of high
    // current power input during program operation
    // (FCFG1 offset 0x198 bits 31:24).
    ui32Value = (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_V) &
                 FCFG1_FLASH_V_VSL_P_M) >>
                 FCFG1_FLASH_V_VSL_P_S;

    HWREG(FLASH_BASE + FLASH_O_FVSLP) =
        (HWREG(FLASH_BASE + FLASH_O_FVSLP) &
         ~FLASH_FVSLP_VSL_P_M) |
        ((ui32Value << FLASH_FVSLP_VSL_P_S) &
         FLASH_FVSLP_VSL_P_M);

    // Configure the voltage level for the specified pump voltage of wordline
    // power supply during programming operations
    // (OTP offset 0x198 bits 23:16).
    ui32Value = (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_V) &
                 FCFG1_FLASH_V_VWL_P_M) >>
                 FCFG1_FLASH_V_VWL_P_S;

    HWREG(FLASH_BASE + FLASH_O_FVWLCT) =
        (HWREG(FLASH_BASE + FLASH_O_FVWLCT) &
         ~FLASH_FVWLCT_VWLCT_P_M) |
        ((ui32Value << FLASH_FVWLCT_VWLCT_P_S) &
         FLASH_FVWLCT_VWLCT_P_M);

    // Configure the pump's TRIM_1P7 port pins.
    // (FCFG1 offset 0x2B0 bits 17:16).
    ui32Value =
        (HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_OTP_DATA3) &
         FCFG1_FLASH_OTP_DATA3_TRIM_1P7_M) >>
         FCFG1_FLASH_OTP_DATA3_TRIM_1P7_S;

    HWREG(FLASH_BASE + FLASH_O_FSEQPMP) =
        (HWREG(FLASH_BASE + FLASH_O_FSEQPMP) &
         ~FLASH_FSEQPMP_TRIM_1P7_M) |
        ((ui32Value << FLASH_FSEQPMP_TRIM_1P7_S) &
         FLASH_FSEQPMP_TRIM_1P7_M);

    // Lock the voltage registers.
    HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;

    // Set trimmed flag.
    HWREG(FLASH_BASE + FLASH_O_FWLOCK) = 5;
    HWREG(FLASH_BASE + FLASH_O_FWFLAG) |= FW_WRT_TRIMMED;
    HWREG(FLASH_BASE + FLASH_O_FWLOCK) = 0;
}

//*****************************************************************************
//
//! \internal
//! Used to scale the TI OTP values based on the FClk scaling value.
//!
//! \param ui32SpecifiedTiming
//! \param ui32ScaleValue
//!
//! Used to scale the TI OTP values based on the FClk scaling value.
//!
//! \return Returns the scaled value
//
//*****************************************************************************
static uint32_t
ScaleCycleValues(uint32_t ui32SpecifiedTiming, uint32_t ui32ScaleValue)
{
    return((ui32SpecifiedTiming * ui32ScaleValue) >> 6);
}

//*****************************************************************************
//
//! \internal
//! Used to set flash in read mode.
//!
//! Flash is configured with values loaded from OTP dependent on the current
//! regulator mode.
//!
//! \return None.
//
//*****************************************************************************
static void
SetReadMode(void)
{
    uint32_t ui32TrimValue;
    uint32_t ui32Value;

    // Configure the STANDBY_MODE_SEL, STANDBY_PW_SEL, DIS_STANDBY, DIS_IDLE,
    // VIN_AT_X and VIN_BY_PASS for read mode
    if(HWREG(AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL) &
       AON_PMCTL_PWRCTL_EXT_REG_MODE)
    {
        // Select trim values for external regulator mode:
        // Configure STANDBY_MODE_SEL (OTP offset 0x308 bit 7)
        // COnfigure STANDBY_PW_SEL   (OTP offset 0x308 bit 6:5)
        // Must be done while the register bit field CONFIG.DIS_STANDBY = 1
        HWREG(FLASH_BASE + FLASH_O_CFG) |= FLASH_CFG_DIS_STANDBY;

        ui32TrimValue =
           HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_OTP_DATA4);

        ui32Value = ((ui32TrimValue &
                      FCFG1_FLASH_OTP_DATA4_STANDBY_MODE_SEL_EXT_RD_M) >>
                      FCFG1_FLASH_OTP_DATA4_STANDBY_MODE_SEL_EXT_RD_S) <<
                    FLASH_CFG_STANDBY_MODE_SEL_S;

        ui32Value |= ((ui32TrimValue &
                       FCFG1_FLASH_OTP_DATA4_STANDBY_PW_SEL_EXT_RD_M) >>
                       FCFG1_FLASH_OTP_DATA4_STANDBY_PW_SEL_EXT_RD_S) <<
                     FLASH_CFG_STANDBY_PW_SEL_S;

        // Configure DIS_STANDBY (OTP offset 0x308 bit 4).
        // Configure DIS_IDLE    (OTP offset 0x308 bit 3).
        ui32Value |= ((ui32TrimValue &
                       (FCFG1_FLASH_OTP_DATA4_DIS_STANDBY_EXT_RD_M |
                        FCFG1_FLASH_OTP_DATA4_DIS_IDLE_EXT_RD_M)) >>
                        FCFG1_FLASH_OTP_DATA4_DIS_IDLE_EXT_RD_S) <<
                        FLASH_CFG_DIS_IDLE_S;

        HWREG(FLASH_BASE + FLASH_O_CFG) = (HWREG(FLASH_BASE + FLASH_O_CFG) &
                                           ~(FLASH_CFG_STANDBY_MODE_SEL_M |
                                             FLASH_CFG_STANDBY_PW_SEL_M   |
                                             FLASH_CFG_DIS_STANDBY_M      |
                                             FLASH_CFG_DIS_IDLE_M)) | ui32Value;

        // Check if sample and hold functionality is disabled.
        if(HWREG(FLASH_BASE + FLASH_O_CFG) & FLASH_CFG_DIS_IDLE)
        {
            // Wait for disabled sample and hold functionality to be stable.
            while(!(HWREG(FLASH_BASE + FLASH_O_STAT) & FLASH_STAT_SAMHOLD_DIS))
            {
            }
        }

        // Configure VIN_AT_X (OTP offset 0x308 bits 2:0)
        ui32Value = ((ui32TrimValue &
                      FCFG1_FLASH_OTP_DATA4_VIN_AT_X_EXT_RD_M) >>
                      FCFG1_FLASH_OTP_DATA4_VIN_AT_X_EXT_RD_S) <<
                    FLASH_FSEQPMP_VIN_AT_X_S;

        // Configure VIN_BY_PASS which is dependent on the VIN_AT_X value.
        // If VIN_AT_X = 7 then VIN_BY_PASS should be 0 otherwise
        // VIN_BY_PASS should be 1
        if(((ui32Value & FLASH_FSEQPMP_VIN_AT_X_M) >>
            FLASH_FSEQPMP_VIN_AT_X_S) != 0x7)
        {
            ui32Value |= FLASH_FSEQPMP_VIN_BY_PASS;
        }

        HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;
        HWREG(FLASH_BASE + FLASH_O_FSEQPMP) =
                                    (HWREG(FLASH_BASE + FLASH_O_FSEQPMP) &
                                     ~(FLASH_FSEQPMP_VIN_BY_PASS_M |
                                       FLASH_FSEQPMP_VIN_AT_X_M))  | ui32Value;
        HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;
    }
    else
    {
        // Select trim values for internal regulator mode:
        // Configure STANDBY_MODE_SEL (OTP offset 0x308 bit 15)
        // COnfigure STANDBY_PW_SEL   (OTP offset 0x308 bit 14:13)
        // Must be done while the register bit field CONFIG.DIS_STANDBY = 1
        HWREG(FLASH_BASE + FLASH_O_CFG) |= FLASH_CFG_DIS_STANDBY;

        ui32TrimValue =
           HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_OTP_DATA4);

        ui32Value = ((ui32TrimValue &
                      FCFG1_FLASH_OTP_DATA4_STANDBY_MODE_SEL_INT_RD_M) >>
                      FCFG1_FLASH_OTP_DATA4_STANDBY_MODE_SEL_INT_RD_S) <<
                    FLASH_CFG_STANDBY_MODE_SEL_S;

        ui32Value |= ((ui32TrimValue &
                       FCFG1_FLASH_OTP_DATA4_STANDBY_PW_SEL_INT_RD_M) >>
                       FCFG1_FLASH_OTP_DATA4_STANDBY_PW_SEL_INT_RD_S) <<
                     FLASH_CFG_STANDBY_PW_SEL_S;

        // Configure DIS_STANDBY (OTP offset 0x308 bit 12).
        // Configure DIS_IDLE    (OTP offset 0x308 bit 11).
        ui32Value |= ((ui32TrimValue &
                       (FCFG1_FLASH_OTP_DATA4_DIS_STANDBY_INT_RD_M |
                        FCFG1_FLASH_OTP_DATA4_DIS_IDLE_INT_RD_M)) >>
                        FCFG1_FLASH_OTP_DATA4_DIS_IDLE_INT_RD_S) <<
                        FLASH_CFG_DIS_IDLE_S;

        HWREG(FLASH_BASE + FLASH_O_CFG) = (HWREG(FLASH_BASE + FLASH_O_CFG) &
                                           ~(FLASH_CFG_STANDBY_MODE_SEL_M |
                                             FLASH_CFG_STANDBY_PW_SEL_M   |
                                             FLASH_CFG_DIS_STANDBY_M      |
                                             FLASH_CFG_DIS_IDLE_M)) | ui32Value;

        // Check if sample and hold functionality is disabled.
        if(HWREG(FLASH_BASE + FLASH_O_CFG) & FLASH_CFG_DIS_IDLE)
        {
            // Wait for disabled sample and hold functionality to be stable.
            while(!(HWREG(FLASH_BASE + FLASH_O_STAT) & FLASH_STAT_SAMHOLD_DIS))
            {
            }
        }

        // Configure VIN_AT_X (OTP offset 0x308 bits 10:8)
        ui32Value = (((ui32TrimValue &
                       FCFG1_FLASH_OTP_DATA4_VIN_AT_X_INT_RD_M) >>
                       FCFG1_FLASH_OTP_DATA4_VIN_AT_X_INT_RD_S) <<
                       FLASH_FSEQPMP_VIN_AT_X_S);

        // Configure VIN_BY_PASS which is dependent on the VIN_AT_X value.
        // If VIN_AT_X = 7 then VIN_BY_PASS should be 0 otherwise
        // VIN_BY_PASS should be 1
        if(((ui32Value & FLASH_FSEQPMP_VIN_AT_X_M) >>
            FLASH_FSEQPMP_VIN_AT_X_S) != 0x7)
        {
            ui32Value |= FLASH_FSEQPMP_VIN_BY_PASS;
        }

        HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;
        HWREG(FLASH_BASE + FLASH_O_FSEQPMP) =
                                    (HWREG(FLASH_BASE + FLASH_O_FSEQPMP) &
                                     ~(FLASH_FSEQPMP_VIN_BY_PASS_M |
                                       FLASH_FSEQPMP_VIN_AT_X_M)) | ui32Value;
        HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;
    }
}

//*****************************************************************************
//
//! \internal
//! Used to set flash in write mode.
//!
//! Flash is configured with values loaded from OTP dependent on the current
//! regulator mode.
//!
//! \return None.
//
//*****************************************************************************
static void
SetWriteMode(void)
{
    uint32_t ui32TrimValue;
    uint32_t ui32Value;

    // Configure the STANDBY_MODE_SEL, STANDBY_PW_SEL, DIS_STANDBY, DIS_IDLE,
    // VIN_AT_X and VIN_BY_PASS for program/erase mode
    if(HWREG(AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL) &
       AON_PMCTL_PWRCTL_EXT_REG_MODE)
    {
        // Select trim values for external regulator mode:
        // Configure STANDBY_MODE_SEL (OTP offset 0x308 bit 23)
        // COnfigure STANDBY_PW_SEL   (OTP offset 0x308 bit 22:21)
        // Must be done while the register bit field CONFIG.DIS_STANDBY = 1
        HWREG(FLASH_BASE + FLASH_O_CFG) |= FLASH_CFG_DIS_STANDBY;

        ui32TrimValue =
           HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_OTP_DATA4);

        ui32Value = ((ui32TrimValue &
                      FCFG1_FLASH_OTP_DATA4_STANDBY_MODE_SEL_EXT_WRT_M) >>
                      FCFG1_FLASH_OTP_DATA4_STANDBY_MODE_SEL_EXT_WRT_S) <<
                      FLASH_CFG_STANDBY_MODE_SEL_S;

        ui32Value |= ((ui32TrimValue &
                       FCFG1_FLASH_OTP_DATA4_STANDBY_PW_SEL_EXT_WRT_M) >>
                       FCFG1_FLASH_OTP_DATA4_STANDBY_PW_SEL_EXT_WRT_S) <<
                       FLASH_CFG_STANDBY_PW_SEL_S;

        // Configure DIS_STANDBY (OTP offset 0x308 bit 20).
        // Configure DIS_IDLE    (OTP offset 0x308 bit 19).
        ui32Value |= ((ui32TrimValue &
                       (FCFG1_FLASH_OTP_DATA4_DIS_STANDBY_EXT_WRT_M |
                        FCFG1_FLASH_OTP_DATA4_DIS_IDLE_EXT_WRT_M)) >>
                        FCFG1_FLASH_OTP_DATA4_DIS_IDLE_EXT_WRT_S) <<
                        FLASH_CFG_DIS_IDLE_S;

        HWREG(FLASH_BASE + FLASH_O_CFG) = (HWREG(FLASH_BASE + FLASH_O_CFG) &
                                           ~(FLASH_CFG_STANDBY_MODE_SEL_M |
                                             FLASH_CFG_STANDBY_PW_SEL_M   |
                                             FLASH_CFG_DIS_STANDBY_M      |
                                             FLASH_CFG_DIS_IDLE_M)) | ui32Value;

        // Check if sample and hold functionality is disabled.
        if(HWREG(FLASH_BASE + FLASH_O_CFG) & FLASH_CFG_DIS_IDLE)
        {
            // Wait for disabled sample and hold functionality to be stable.
            while(!(HWREG(FLASH_BASE + FLASH_O_STAT) & FLASH_STAT_SAMHOLD_DIS))
            {
            }
        }

        // Configure VIN_AT_X (OTP offset 0x308 bits 18:16)
        ui32Value = ((ui32TrimValue &
                      FCFG1_FLASH_OTP_DATA4_VIN_AT_X_EXT_WRT_M) >>
                      FCFG1_FLASH_OTP_DATA4_VIN_AT_X_EXT_WRT_S) <<
                      FLASH_FSEQPMP_VIN_AT_X_S;

        // Configure VIN_BY_PASS which is dependent on the VIN_AT_X value.
        // If VIN_AT_X = 7 then VIN_BY_PASS should be 0 otherwise
        // VIN_BY_PASS should be 1
        if(((ui32Value & FLASH_FSEQPMP_VIN_AT_X_M) >>
            FLASH_FSEQPMP_VIN_AT_X_S) != 0x7)
        {
            ui32Value |= FLASH_FSEQPMP_VIN_BY_PASS;
        }

        HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;
        HWREG(FLASH_BASE + FLASH_O_FSEQPMP) =
                                    (HWREG(FLASH_BASE + FLASH_O_FSEQPMP) &
                                     ~(FLASH_FSEQPMP_VIN_BY_PASS_M |
                                       FLASH_FSEQPMP_VIN_AT_X_M))  | ui32Value;
        HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;
    }
    else
    {
        // Select trim values for internal regulator mode:
        // Configure STANDBY_MODE_SEL (OTP offset 0x308 bit 31)
        // COnfigure STANDBY_PW_SEL   (OTP offset 0x308 bit 30:29)
        // Must be done while the register bit field CONFIG.DIS_STANDBY = 1
        HWREG(FLASH_BASE + FLASH_O_CFG) |= FLASH_CFG_DIS_STANDBY;

        ui32TrimValue =
           HWREG(FLASH_CFG_BASE + FCFG1_OFFSET + FCFG1_O_FLASH_OTP_DATA4);

        ui32Value = ((ui32TrimValue &
                      FCFG1_FLASH_OTP_DATA4_STANDBY_MODE_SEL_INT_WRT_M) >>
                      FCFG1_FLASH_OTP_DATA4_STANDBY_MODE_SEL_INT_WRT_S) <<
                      FLASH_CFG_STANDBY_MODE_SEL_S;

        ui32Value |= ((ui32TrimValue &
                       FCFG1_FLASH_OTP_DATA4_STANDBY_PW_SEL_INT_WRT_M) >>
                       FCFG1_FLASH_OTP_DATA4_STANDBY_PW_SEL_INT_WRT_S) <<
                       FLASH_CFG_STANDBY_PW_SEL_S;

        // Configure DIS_STANDBY (OTP offset 0x308 bit 28).
        // Configure DIS_IDLE    (OTP offset 0x308 bit 27).
        ui32Value |= ((ui32TrimValue &
                      (FCFG1_FLASH_OTP_DATA4_DIS_STANDBY_INT_WRT_M |
                       FCFG1_FLASH_OTP_DATA4_DIS_IDLE_INT_WRT_M)) >>
                       FCFG1_FLASH_OTP_DATA4_DIS_IDLE_INT_WRT_S) <<
                       FLASH_CFG_DIS_IDLE_S;


        HWREG(FLASH_BASE + FLASH_O_CFG) = (HWREG(FLASH_BASE + FLASH_O_CFG) &
                                           ~(FLASH_CFG_STANDBY_MODE_SEL_M |
                                             FLASH_CFG_STANDBY_PW_SEL_M   |
                                             FLASH_CFG_DIS_STANDBY_M      |
                                             FLASH_CFG_DIS_IDLE_M)) | ui32Value;

        // Check if sample and hold functionality is disabled.
        if(HWREG(FLASH_BASE + FLASH_O_CFG) & FLASH_CFG_DIS_IDLE)
        {
            // Wait for disabled sample and hold functionality to be stable.
            while(!(HWREG(FLASH_BASE + FLASH_O_STAT) & FLASH_STAT_SAMHOLD_DIS))
            {
            }
        }

        // Configure VIN_AT_X (OTP offset 0x308 bits 26:24)
        ui32Value = ((ui32TrimValue &
                      FCFG1_FLASH_OTP_DATA4_VIN_AT_X_INT_WRT_M) >>
                      FCFG1_FLASH_OTP_DATA4_VIN_AT_X_INT_WRT_S) <<
                      FLASH_FSEQPMP_VIN_AT_X_S;

        // Configure VIN_BY_PASS which is dependent on the VIN_AT_X value.
        // If VIN_AT_X = 7 then VIN_BY_PASS should be 0 otherwise
        // VIN_BY_PASS should be 1
        if(((ui32Value & FLASH_FSEQPMP_VIN_AT_X_M) >>
            FLASH_FSEQPMP_VIN_AT_X_S) != 0x7)
        {
            ui32Value |= FLASH_FSEQPMP_VIN_BY_PASS;
        }

        HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0xAAAA;
        HWREG(FLASH_BASE + FLASH_O_FSEQPMP) =
                                    (HWREG(FLASH_BASE + FLASH_O_FSEQPMP) &
                                     ~(FLASH_FSEQPMP_VIN_BY_PASS_M |
                                       FLASH_FSEQPMP_VIN_AT_X_M)) | ui32Value;
        HWREG(FLASH_BASE + FLASH_O_FLOCK) = 0x55AA;
    }
}

void I2CMasterInitExpClk(uint32_t ui32Base, uint32_t ui32I2CClk, bool bFast) {
    uint32_t ui32SCLFreq;
    uint32_t ui32TPR;

    // Check the arguments.
    ASSERT(I2CBaseValid(ui32Base));

    // Must enable the device before doing anything else.
    I2CMasterEnable(I2C0_BASE);

    // Get the desired SCL speed.
    if(bFast == true)
    {
        ui32SCLFreq = 400000;
    }
    else
    {
        ui32SCLFreq = 100000;
    }

    // Compute the clock divider that achieves the fastest speed less than or
    // equal to the desired speed. The numerator is biased to favor a larger
    // clock divider so that the resulting clock is always less than or equal
    // to the desired clock, never greater.
    ui32TPR = ((ui32I2CClk + (2 * 10 * ui32SCLFreq) - 1) / (2 * 10 * ui32SCLFreq)) - 1;
    HWREG(I2C0_BASE + I2C_O_MTPR) = ui32TPR;
}

uint32_t I2CMasterErr(uint32_t ui32Base) {
    uint32_t ui32Err;

    // Check the arguments.
    ASSERT(I2CBaseValid(ui32Base));

    // Get the raw error state.
    ui32Err = HWREG(I2C0_BASE + I2C_O_MSTAT);

    // If the I2C master is busy, then all the other status bits are invalid,
    // and there is no error to report.
    if(ui32Err & I2C_MSTAT_BUSY)
    {
        return(I2C_MASTER_ERR_NONE);
    }

    // Check for errors.
    if(ui32Err & (I2C_MSTAT_ERR | I2C_MSTAT_ARBLST))
    {
        return(ui32Err & (I2C_MSTAT_ARBLST | I2C_MSTAT_DATACK_N | I2C_MSTAT_ADRACK_N));
    }
    else
    {
        return(I2C_MASTER_ERR_NONE);
    }
}
//*****************************************************************************
//
//! This is a mapping between priority grouping encodings and the number of
//! preemption priority bits.
//
//*****************************************************************************
static const uint32_t g_pui32Priority[] =
{
    NVIC_APINT_PRIGROUP_0_8, NVIC_APINT_PRIGROUP_1_7, NVIC_APINT_PRIGROUP_2_6,
    NVIC_APINT_PRIGROUP_3_5, NVIC_APINT_PRIGROUP_4_4, NVIC_APINT_PRIGROUP_5_3,
    NVIC_APINT_PRIGROUP_6_2, NVIC_APINT_PRIGROUP_7_1
};

//*****************************************************************************
//
//! This is a mapping between interrupt number and the register that contains
//! the priority encoding for that interrupt.
//
//*****************************************************************************
static const uint32_t g_pui32Regs[] =
{
    0, NVIC_SYS_PRI1, NVIC_SYS_PRI2, NVIC_SYS_PRI3, NVIC_PRI0, NVIC_PRI1,
    NVIC_PRI2, NVIC_PRI3, NVIC_PRI4, NVIC_PRI5, NVIC_PRI6, NVIC_PRI7,
    NVIC_PRI8, NVIC_PRI9, NVIC_PRI10, NVIC_PRI11, NVIC_PRI12, NVIC_PRI13
};

void IntPriorityGroupingSet(uint32_t ui32Bits) {
    // Check the arguments.
    ASSERT(ui32Bits < NUM_PRIORITY);

    // Set the priority grouping.
    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | g_pui32Priority[ui32Bits];
}

uint32_t IntPriorityGroupingGet(void) {
    uint32_t ui32Loop, ui32Value;

    // Read the priority grouping.
    ui32Value = HWREG(NVIC_APINT) & NVIC_APINT_PRIGROUP_M;

    // Loop through the priority grouping values.
    for(ui32Loop = 0; ui32Loop < NUM_PRIORITY; ui32Loop++)
    {
        // Stop looping if this value matches.
        if(ui32Value == g_pui32Priority[ui32Loop])
        {
            break;
        }
    }

    // Return the number of priority bits.
    return(ui32Loop);
}

void IntPrioritySet(uint32_t ui32Interrupt, uint8_t ui8Priority) {
    uint32_t ui32Temp;

    // Check the arguments.
    ASSERT((ui32Interrupt >= 4) && (ui32Interrupt < NUM_INTERRUPTS));
    ASSERT(ui8Priority <= INT_PRI_LEVEL7);

    // Set the interrupt priority.
    ui32Temp = HWREG(g_pui32Regs[ui32Interrupt >> 2]);
    ui32Temp &= ~(0xFF << (8 * (ui32Interrupt & 3)));
    ui32Temp |= ui8Priority << (8 * (ui32Interrupt & 3));
    HWREG(g_pui32Regs[ui32Interrupt >> 2]) = ui32Temp;
}

int32_t IntPriorityGet(uint32_t ui32Interrupt) {
    // Check the arguments.
    ASSERT((ui32Interrupt >= 4) && (ui32Interrupt < NUM_INTERRUPTS));

    // Return the interrupt priority.
    return((HWREG(g_pui32Regs[ui32Interrupt >> 2]) >> (8 * (ui32Interrupt & 3))) &
           0xFF);
}

void IntEnable(uint32_t ui32Interrupt) {
    // Check the arguments.
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    // Determine the interrupt to enable.
    if(ui32Interrupt == INT_MEMMANAGE_FAULT)
    {
        // Enable the MemManage interrupt.
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_MEM;
    }
    else if(ui32Interrupt == INT_BUS_FAULT)
    {
        // Enable the bus fault interrupt.
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_BUS;
    }
    else if(ui32Interrupt == INT_USAGE_FAULT)
    {
        // Enable the usage fault interrupt.
        HWREG(NVIC_SYS_HND_CTRL) |= NVIC_SYS_HND_CTRL_USAGE;
    }
    else if(ui32Interrupt == INT_SYSTICK)
    {
        // Enable the System Tick interrupt.
        HWREG(NVIC_ST_CTRL) |= NVIC_ST_CTRL_INTEN;
    }
    else if((ui32Interrupt >= 16) && (ui32Interrupt <= 47))
    {
        // Enable the general interrupt.
        HWREG(NVIC_EN0) = 1 << (ui32Interrupt - 16);
    }
    else if(ui32Interrupt >= 48)
    {
        // Enable the general interrupt.
        HWREG(NVIC_EN1) = 1 << (ui32Interrupt - 48);
    }
}

void IntDisable(uint32_t ui32Interrupt) {
    // Check the arguments.
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    // Determine the interrupt to disable.
    if(ui32Interrupt == INT_MEMMANAGE_FAULT)
    {
        // Disable the MemManage interrupt.
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_MEM);
    }
    else if(ui32Interrupt == INT_BUS_FAULT)
    {
        // Disable the bus fault interrupt.
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_BUS);
    }
    else if(ui32Interrupt == INT_USAGE_FAULT)
    {
        // Disable the usage fault interrupt.
        HWREG(NVIC_SYS_HND_CTRL) &= ~(NVIC_SYS_HND_CTRL_USAGE);
    }
    else if(ui32Interrupt == INT_SYSTICK)
    {
        // Disable the System Tick interrupt.
        HWREG(NVIC_ST_CTRL) &= ~(NVIC_ST_CTRL_INTEN);
    }
    else if((ui32Interrupt >= 16) && (ui32Interrupt <= 47))
    {
        // Disable the general interrupt.
        HWREG(NVIC_DIS0) = 1 << (ui32Interrupt - 16);
    }
    else if(ui32Interrupt >= 48)
    {
        // Disable the general interrupt.
        HWREG(NVIC_DIS1) = 1 << (ui32Interrupt - 48);
    }
}

void IntPendSet(uint32_t ui32Interrupt) {
    // Check the arguments.
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    // Determine the interrupt to pend.
    if(ui32Interrupt == INT_NMI_FAULT)
    {
        // Pend the NMI interrupt.
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_NMI_SET;
    }
    else if(ui32Interrupt == INT_PENDSV)
    {
        // Pend the PendSV interrupt.
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
    }
    else if(ui32Interrupt == INT_SYSTICK)
    {
        // Pend the SysTick interrupt.
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PENDSTSET;
    }
    else if((ui32Interrupt >= 16) && (ui32Interrupt <= 47))
    {
        // Pend the general interrupt.
        HWREG(NVIC_PEND0) = 1 << (ui32Interrupt - 16);
    }
    else if(ui32Interrupt >= 48)
    {
        // Pend the general interrupt.
        HWREG(NVIC_PEND1) = 1 << (ui32Interrupt - 48);
    }
}

bool IntPendGet(uint32_t ui32Interrupt) {
    uint32_t ui32IntPending;

    // Check the arguments.
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    // Assume no interrupts are pending.
    ui32IntPending = 0;

    // The lower 16 IRQ vectors are unsupported by this function
    if (ui32Interrupt < 16)
    {

       return 0;
    }

    // Subtract lower 16 irq vectors
    ui32Interrupt -= 16;

    // Check if the interrupt is pending
    ui32IntPending = HWREG(NVIC_PEND0 + (ui32Interrupt / 32));
    ui32IntPending &= (1 << (ui32Interrupt & 31));

    return ui32IntPending ? true : false;
}

void IntPendClear(uint32_t ui32Interrupt) {
    // Check the arguments.
    ASSERT(ui32Interrupt < NUM_INTERRUPTS);

    // Determine the interrupt to unpend.
    if(ui32Interrupt == INT_PENDSV)
    {
        // Unpend the PendSV interrupt.
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_UNPEND_SV;
    }
    else if(ui32Interrupt == INT_SYSTICK)
    {
        // Unpend the SysTick interrupt.
        HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PENDSTCLR;
    }
    else if((ui32Interrupt >= 16) && (ui32Interrupt <= 47))
    {
        // Unpend the general interrupt.
        HWREG(NVIC_UNPEND0) = 1 << (ui32Interrupt - 16);
    }
    else if(ui32Interrupt >= 48)
    {
        // Unpend the general interrupt.
        HWREG(NVIC_UNPEND1) = 1 << (ui32Interrupt - 48);
    }
}

void IOCPortConfigureSet(uint32_t ui32IOId, uint32_t ui32PortId, uint32_t ui32IOConfig) {
    uint32_t ui32Reg;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT(ui32PortId <= IOC_PORT_RFC_GPI1);

    // Get the register address.
    ui32Reg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the port.
    HWREG(ui32Reg) = ui32IOConfig | ui32PortId;
}

uint32_t IOCPortConfigureGet(uint32_t ui32IOId) {
    uint32_t ui32Reg;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);

    // Get the register address.
    ui32Reg = IOC_BASE + ( ui32IOId << 2 );

    // Return the IO configuration.
    return HWREG(ui32Reg);
}

void IOCIOShutdownSet(uint32_t ui32IOId, uint32_t ui32IOShutdown) {
    uint32_t ui32Reg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32IOShutdown == IOC_NO_WAKE_UP) ||
           (ui32IOShutdown == IOC_WAKE_ON_LOW) ||
           (ui32IOShutdown == IOC_WAKE_ON_HIGH));

    // Get the register address.
    ui32Reg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32Reg);
    ui32Config &= ~IOC_IOCFG0_WU_CFG_M;
    HWREG(ui32Reg) = ui32Config | ui32IOShutdown;
}

void IOCIOJTagSet(uint32_t ui32IOId, uint32_t ui32IOJTag) {
    uint32_t ui32Reg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32IOJTag == IOC_JTAG_TDO_ENABLE) ||
           (ui32IOJTag == IOC_JTAG_TDI_ENABLE) ||
           (ui32IOJTag == IOC_JTAG_DISABLE));

    // Get the register address.
    ui32Reg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32Reg);
    ui32Config &= ~(IOC_IOCFG0_TDI | IOC_IOCFG0_TDO);
    HWREG(ui32Reg) = ui32Config | ui32IOJTag;
}

void IOCIOModeSet(uint32_t ui32IOId, uint32_t ui32IOMode) {
    uint32_t ui32Reg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32IOMode == IOC_IOMODE_NORMAL) ||
           (ui32IOMode == IOC_IOMODE_INV) ||
           (ui32IOMode == IOC_IOMODE_OPEN_DRAIN_NORMAL) ||
           (ui32IOMode == IOC_IOMODE_OPEN_DRAIN_INV) ||
           (ui32IOMode == IOC_IOMODE_OPEN_SRC_NORMAL) ||
           (ui32IOMode == IOC_IOMODE_OPEN_SRC_INV));

    // Get the register address.
    ui32Reg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32Reg);
    ui32Config &= ~IOC_IOCFG0_IOMODE_M;
    HWREG(ui32Reg) = ui32Config | ui32IOMode;
}

void IOCIOIntSet(uint32_t ui32IOId, uint32_t ui32Int, uint32_t ui32EdgeDet) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32Int == IOC_INT_ENABLE) ||
           (ui32Int == IOC_INT_DISABLE));
    ASSERT((ui32EdgeDet == IOC_NO_EDGE) ||
           (ui32EdgeDet == IOC_FALLING_EDGE) ||
           (ui32EdgeDet == IOC_RISING_EDGE) ||
           (ui32EdgeDet == IOC_BOTH_EDGES));

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32IOReg);
    ui32Config &=  ~(IOC_IOCFG0_EDGE_IRQ_EN | IOC_IOCFG0_EDGE_DET_M);
    HWREG(ui32IOReg) = ui32Config | ((ui32Int ? IOC_IOCFG0_EDGE_IRQ_EN : 0) | ui32EdgeDet);
}

void IOCIOPortPullSet(uint32_t ui32IOId, uint32_t ui32Pull) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the argument.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32Pull == IOC_NO_IOPULL) ||
           (ui32Pull == IOC_IOPULL_UP) ||
           (ui32Pull == IOC_IOPULL_DOWN));

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32IOReg);
    ui32Config &= ~IOC_IOCFG0_PULL_CTL_M;
    HWREG(ui32IOReg) = ui32Config | ui32Pull;
}

void IOCIOHystSet(uint32_t ui32IOId, uint32_t ui32Hysteresis) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32Hysteresis == IOC_HYST_ENABLE) ||
           (ui32Hysteresis == IOC_HYST_DISABLE));

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32IOReg);
    ui32Config &= ~IOC_IOCFG0_HYST_EN;
    HWREG(ui32IOReg) = ui32Config | ui32Hysteresis;
}

void IOCIOInputSet(uint32_t ui32IOId, uint32_t ui32Input) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32Input == IOC_INPUT_ENABLE) ||
           (ui32Input == IOC_INPUT_DISABLE));

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32IOReg);
    ui32Config &= ~IOC_IOCFG0_IE;
    HWREG(ui32IOReg) = ui32Config | ui32Input;
}

void IOCIOSlewCtrlSet(uint32_t ui32IOId, uint32_t ui32SlewEnable) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32SlewEnable == IOC_SLEW_ENABLE) ||
           (ui32SlewEnable == IOC_SLEW_DISABLE));

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32IOReg);
    ui32Config &= ~IOC_IOCFG0_SLEW_RED;
    HWREG(ui32IOReg) = ui32Config | ui32SlewEnable;
}

void IOCIODrvStrengthSet(uint32_t ui32IOId, uint32_t ui32IOCurrent, uint32_t ui32DrvStrength) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT((ui32IOCurrent == IOC_CURRENT_2MA) ||
           (ui32IOCurrent == IOC_CURRENT_4MA) ||
           (ui32IOCurrent == IOC_CURRENT_8MA));
    ASSERT((ui32DrvStrength == IOC_STRENGTH_MIN) ||
           (ui32DrvStrength == IOC_STRENGTH_MAX) ||
           (ui32DrvStrength == IOC_STRENGTH_MED) ||
           (ui32DrvStrength == IOC_STRENGTH_AUTO));

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32IOReg);
    ui32Config &= ~(IOC_IOCFG0_IOCURR_M | IOC_IOCFG0_IOSTR_M);
    HWREG(ui32IOReg) = ui32Config | (ui32IOCurrent | ui32DrvStrength);
}

void IOCIOPortIdSet(uint32_t ui32IOId, uint32_t ui32PortId) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);
    ASSERT(ui32PortId <= IOC_PORT_RFC_GPI1);

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Configure the IO.
    ui32Config = HWREG(ui32IOReg);
    ui32Config &= ~IOC_IOCFG0_PORT_ID_M;
    HWREG(ui32IOReg) = ui32Config | ui32PortId;
}

void IOCIntEnable(uint32_t ui32IOId) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Enable the specified interrupt.
    ui32Config = HWREG(ui32IOReg);
    ui32Config |= IOC_IOCFG0_EDGE_IRQ_EN;
    HWREG(ui32IOReg) = ui32Config;
}

void IOCIntDisable(uint32_t ui32IOId) {
    uint32_t ui32IOReg;
    uint32_t ui32Config;

    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);

    // Get the register address.
    ui32IOReg = IOC_BASE + ( ui32IOId << 2 );

    // Disable the specified interrupt.
    ui32Config = HWREG(ui32IOReg);
    ui32Config &= ~IOC_IOCFG0_EDGE_IRQ_EN;
    HWREG(ui32IOReg) = ui32Config;
}

void IOCPinTypeGpioInput(uint32_t ui32IOId) {
    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);

    // Setup the IO for standard input.
    IOCPortConfigureSet(ui32IOId, IOC_PORT_GPIO, IOC_STD_INPUT);

    // Enable input mode in the GPIO module.
    GPIO_setOutputEnableDio(ui32IOId, GPIO_OUTPUT_DISABLE);
}

void IOCPinTypeGpioOutput(uint32_t ui32IOId) {
    // Check the arguments.
    ASSERT(ui32IOId <= IOID_31);

    // Setup the IO for standard input.
    IOCPortConfigureSet(ui32IOId, IOC_PORT_GPIO, IOC_STD_OUTPUT);

    // Enable output mode in the GPIO module.
    GPIO_setOutputEnableDio(ui32IOId, GPIO_OUTPUT_ENABLE);
}

void IOCPinTypeUart(uint32_t ui32Base, uint32_t ui32Rx, uint32_t ui32Tx, uint32_t ui32Cts, uint32_t ui32Rts) {
    // Check the arguments.
    ASSERT(ui32Base == UART0_BASE);
    ASSERT((ui32Rx <= IOID_31) || (ui32Rx == IOID_UNUSED));
    ASSERT((ui32Tx <= IOID_31) || (ui32Tx == IOID_UNUSED));
    ASSERT((ui32Cts <= IOID_31) || (ui32Cts == IOID_UNUSED));
    ASSERT((ui32Rts <= IOID_31) || (ui32Rts == IOID_UNUSED));

    // Setup the IOs in the desired configuration.
    if(ui32Rx != IOID_UNUSED)
    {
        IOCPortConfigureSet(ui32Rx, IOC_PORT_MCU_UART0_RX, IOC_STD_INPUT);
    }
    if(ui32Tx != IOID_UNUSED)
    {
        IOCPortConfigureSet(ui32Tx, IOC_PORT_MCU_UART0_TX, IOC_STD_OUTPUT);
    }
    if(ui32Cts != IOID_UNUSED)
    {
        IOCPortConfigureSet(ui32Cts, IOC_PORT_MCU_UART0_CTS, IOC_STD_INPUT);
    }
    if(ui32Rts != IOID_UNUSED)
    {
        IOCPortConfigureSet(ui32Rts, IOC_PORT_MCU_UART0_RTS, IOC_STD_OUTPUT);
    }
}

void IOCPinTypeSsiMaster(uint32_t ui32Base, uint32_t ui32Rx, uint32_t ui32Tx, uint32_t ui32Fss, uint32_t ui32Clk) {
    // Check the arguments.
    ASSERT((ui32Base == SSI0_BASE) || (ui32Base == SSI1_BASE));
    ASSERT((ui32Rx <= IOID_31) || (ui32Rx == IOID_UNUSED));
    ASSERT((ui32Tx <= IOID_31) || (ui32Tx == IOID_UNUSED));
    ASSERT((ui32Fss <= IOID_31) || (ui32Fss == IOID_UNUSED));
    ASSERT((ui32Clk <= IOID_31) || (ui32Clk == IOID_UNUSED));

    // Setup the IOs in the desired configuration.
    if(ui32Base == SSI0_BASE)
    {
        if(ui32Rx != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Rx, IOC_PORT_MCU_SSI0_RX, IOC_STD_INPUT);
        }
        if(ui32Tx != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Tx, IOC_PORT_MCU_SSI0_TX, IOC_STD_OUTPUT);
        }
        if(ui32Fss != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Fss, IOC_PORT_MCU_SSI0_FSS, IOC_STD_OUTPUT);
        }
        if(ui32Clk != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Clk, IOC_PORT_MCU_SSI0_CLK, IOC_STD_OUTPUT);
        }
    }
    else
    {
        if(ui32Rx != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Rx, IOC_PORT_MCU_SSI1_RX, IOC_STD_INPUT);
        }
        if(ui32Tx != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Tx, IOC_PORT_MCU_SSI1_TX, IOC_STD_OUTPUT);
        }
        if(ui32Fss != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Fss, IOC_PORT_MCU_SSI1_FSS, IOC_STD_OUTPUT);
        }
        if(ui32Clk != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Clk, IOC_PORT_MCU_SSI1_CLK, IOC_STD_OUTPUT);
        }
    }
}

void IOCPinTypeSsiSlave(uint32_t ui32Base, uint32_t ui32Rx, uint32_t ui32Tx, uint32_t ui32Fss, uint32_t ui32Clk) {
    // Check the arguments.
    ASSERT((ui32Base == SSI0_BASE) || (ui32Base == SSI1_BASE));
    ASSERT((ui32Rx <= IOID_31) || (ui32Rx == IOID_UNUSED));
    ASSERT((ui32Tx <= IOID_31) || (ui32Tx == IOID_UNUSED));
    ASSERT((ui32Fss <= IOID_31) || (ui32Fss == IOID_UNUSED));
    ASSERT((ui32Clk <= IOID_31) || (ui32Clk == IOID_UNUSED));

    // Setup the IOs in the desired configuration.
    if(ui32Base == SSI0_BASE)
    {
        if(ui32Rx != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Rx, IOC_PORT_MCU_SSI0_RX,  IOC_STD_INPUT);
        }
        if(ui32Tx != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Tx, IOC_PORT_MCU_SSI0_TX, IOC_STD_OUTPUT);
        }
        if(ui32Fss != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Fss, IOC_PORT_MCU_SSI0_FSS, IOC_STD_INPUT);
        }
        if(ui32Clk != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Clk, IOC_PORT_MCU_SSI0_CLK, IOC_STD_INPUT);
        }
    }
    else
    {
        if(ui32Rx != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Rx, IOC_PORT_MCU_SSI1_RX, IOC_STD_INPUT);
        }
        if(ui32Tx != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Tx, IOC_PORT_MCU_SSI1_TX, IOC_STD_OUTPUT);
        }
        if(ui32Fss != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Fss, IOC_PORT_MCU_SSI1_FSS, IOC_STD_INPUT);
        }
        if(ui32Clk != IOID_UNUSED)
        {
            IOCPortConfigureSet(ui32Clk, IOC_PORT_MCU_SSI1_CLK, IOC_STD_INPUT);
        }
    }
}

void IOCPinTypeI2c(uint32_t ui32Base, uint32_t ui32Data, uint32_t ui32Clk) {
    uint32_t ui32IOConfig;

    // Check the arguments.
    ASSERT((ui32Data <= IOID_31) || (ui32Data == IOID_UNUSED));
    ASSERT((ui32Clk <= IOID_31) || (ui32Clk == IOID_UNUSED));

    // Define the IO configuration parameters.
    ui32IOConfig = IOC_CURRENT_2MA | IOC_STRENGTH_AUTO | IOC_IOPULL_UP |
                   IOC_SLEW_DISABLE | IOC_HYST_DISABLE | IOC_NO_EDGE |
                   IOC_INT_DISABLE | IOC_IOMODE_OPEN_DRAIN_NORMAL |
                   IOC_NO_WAKE_UP | IOC_INPUT_ENABLE;

    // Setup the IOs in the desired configuration.
    IOCPortConfigureSet(ui32Data, IOC_PORT_MCU_I2C_MSSDA, ui32IOConfig);
    IOCPortConfigureSet(ui32Clk, IOC_PORT_MCU_I2C_MSSCL, ui32IOConfig);
}

void IOCPinTypeAux(uint32_t ui32IOId) {
    // Check the arguments.
    ASSERT((ui32IOId <= IOID_31) || (ui32IOId == IOID_UNUSED));

    // Setup the IO.
    IOCPortConfigureSet(ui32IOId, IOC_PORT_AUX_IO, IOC_STD_INPUT);
}

//*****************************************************************************
//
// Arrays that maps the "peripheral set" number (which is stored in
// bits[11:8] of the PRCM_PERIPH_* defines) to the PRCM register that
// contains the relevant bit for that peripheral.
//
//*****************************************************************************

// Run mode registers
static const uint32_t g_pui32RCGCRegs[] =
{
    PRCM_O_GPTCLKGR     , // Index 0
    PRCM_O_SSICLKGR     , // Index 1
    PRCM_O_UARTCLKGR    , // Index 2
    PRCM_O_I2CCLKGR     , // Index 3
    PRCM_O_SECDMACLKGR  , // Index 4
    PRCM_O_GPIOCLKGR    , // Index 5
    PRCM_O_I2SCLKGR       // Index 6
};

// Sleep mode registers
static const uint32_t g_pui32SCGCRegs[] =
{
    PRCM_O_GPTCLKGS     , // Index 0
    PRCM_O_SSICLKGS     , // Index 1
    PRCM_O_UARTCLKGS    , // Index 2
    PRCM_O_I2CCLKGS     , // Index 3
    PRCM_O_SECDMACLKGS  , // Index 4
    PRCM_O_GPIOCLKGS    , // Index 5
    PRCM_O_I2SCLKGS       // Index 6
};

// Deep sleep mode registers
static const uint32_t g_pui32DCGCRegs[] =
{
    PRCM_O_GPTCLKGDS    , // Index 0
    PRCM_O_SSICLKGDS    , // Index 1
    PRCM_O_UARTCLKGDS   , // Index 2
    PRCM_O_I2CCLKGDS    , // Index 3
    PRCM_O_SECDMACLKGDS , // Index 4
    PRCM_O_GPIOCLKGDS   , // Index 5
    PRCM_O_I2SCLKGDS      // Index 6
};

//*****************************************************************************
//
// This macro extracts the array index out of the peripheral number
//
//*****************************************************************************
#define PRCM_PERIPH_INDEX(a)  (((a) >> 8) & 0xf)

//*****************************************************************************
//
// This macro extracts the peripheral instance number and generates bit mask
//
//*****************************************************************************
#define PRCM_PERIPH_MASKBIT(a) (0x00000001 << ((a) & 0x1f))


void PRCMInfClockConfigureSet(uint32_t ui32ClkDiv, uint32_t ui32PowerMode) {
    uint32_t ui32Divisor;

    // Check the arguments.
    ASSERT((ui32ClkDiv == PRCM_CLOCK_DIV_1) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_2) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_8) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_32));
    ASSERT((ui32PowerMode == PRCM_RUN_MODE) ||
           (ui32PowerMode == PRCM_SLEEP_MODE) ||
           (ui32PowerMode == PRCM_DEEP_SLEEP_MODE));

    ui32Divisor = 0;

    // Find the correct division factor.
    if(ui32ClkDiv == PRCM_CLOCK_DIV_1)
    {
        ui32Divisor = 0x0;
    }
    else if(ui32ClkDiv == PRCM_CLOCK_DIV_2)
    {
        ui32Divisor = 0x1;
    }
    else if(ui32ClkDiv == PRCM_CLOCK_DIV_8)
    {
        ui32Divisor = 0x2;
    }
    else if(ui32ClkDiv == PRCM_CLOCK_DIV_32)
    {
        ui32Divisor = 0x3;
    }

    // Determine the correct power mode set the division factor accordingly.
    if(ui32PowerMode == PRCM_RUN_MODE)
    {
        HWREG(PRCM_BASE + PRCM_O_INFRCLKDIVR) = ui32Divisor;
    }
    else if(ui32PowerMode == PRCM_SLEEP_MODE)
    {
        HWREG(PRCM_BASE + PRCM_O_INFRCLKDIVS) = ui32Divisor;
    }
    else if(ui32PowerMode == PRCM_DEEP_SLEEP_MODE)
    {
        HWREG(PRCM_BASE + PRCM_O_INFRCLKDIVDS) = ui32Divisor;
    }
}

uint32_t PRCMInfClockConfigureGet(uint32_t ui32PowerMode) {
    uint32_t ui32ClkDiv;
    uint32_t ui32Divisor;

    // Check the arguments.
    ASSERT((ui32PowerMode == PRCM_RUN_MODE) ||
           (ui32PowerMode == PRCM_SLEEP_MODE) ||
           (ui32PowerMode == PRCM_DEEP_SLEEP_MODE));

    ui32ClkDiv = 0;
    ui32Divisor = 0;

    // Determine the correct power mode.
    if(ui32PowerMode == PRCM_RUN_MODE)
    {
        ui32ClkDiv = HWREG(PRCM_BASE + PRCM_O_INFRCLKDIVR);
    }
    else if(ui32PowerMode == PRCM_SLEEP_MODE)
    {
        ui32ClkDiv = HWREG(PRCM_BASE + PRCM_O_INFRCLKDIVS);
    }
    else if(ui32PowerMode == PRCM_DEEP_SLEEP_MODE)
    {
        ui32ClkDiv = HWREG(PRCM_BASE + PRCM_O_INFRCLKDIVDS);
    }

    // Find the correct division factor.
    if(ui32ClkDiv == 0x0)
    {
        ui32Divisor = PRCM_CLOCK_DIV_1;
    }
    else if(ui32ClkDiv == 0x1)
    {
        ui32Divisor = PRCM_CLOCK_DIV_2;
    }
    else if(ui32ClkDiv == 0x2)
    {
        ui32Divisor = PRCM_CLOCK_DIV_8;
    }
    else if(ui32ClkDiv == 0x3)
    {
        ui32Divisor = PRCM_CLOCK_DIV_32;
    }

    // Return the clock division factor.
    return ui32Divisor;
}

void PRCMClockConfigureSet(uint32_t ui32Domains, uint32_t ui32ClkDiv) {
    uint32_t ui32Reg;

    // Check the arguments.
    ASSERT((ui32Domains & PRCM_DOMAIN_SYSBUS) ||
           (ui32Domains & PRCM_DOMAIN_CPU) ||
           (ui32Domains & PRCM_DOMAIN_PERIPH) ||
           (ui32Domains & PRCM_DOMAIN_TIMER) ||
           (ui32Domains & PRCM_DOMAIN_SERIAL));
    ASSERT((ui32ClkDiv == PRCM_CLOCK_DIV_1) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_2) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_4) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_8) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_16) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_32) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_64) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_128) ||
           (ui32ClkDiv == PRCM_CLOCK_DIV_256));

    // Configure the selected clock dividers.
    if(ui32Domains & PRCM_DOMAIN_SYSBUS)
    {
        ui32Reg = PRCM_O_SYSBUSCLKDIV;
        HWREG(PRCM_BASE + ui32Reg) = ui32ClkDiv;
    }
    if(ui32Domains & PRCM_DOMAIN_CPU)
    {
        ui32Reg = PRCM_O_CPUCLKDIV;
        HWREG(PRCM_BASE + ui32Reg) = ui32ClkDiv;
    }
    if(ui32Domains & PRCM_DOMAIN_PERIPH)
    {
        ui32Reg = PRCM_O_PERBUSCPUCLKDIV;
        HWREG(PRCM_BASE + ui32Reg) = ui32ClkDiv;
    }
    if(ui32Domains & PRCM_DOMAIN_SERIAL)
    {
        ui32Reg = PRCM_O_PERDMACLKDIV;
        HWREG(PRCM_BASE + ui32Reg) = ui32ClkDiv;
    }
    if(ui32Domains & PRCM_DOMAIN_TIMER)
    {
        ui32Reg = PRCM_O_GPTCLKDIV;
        HWREG(PRCM_BASE + ui32Reg) = ui32ClkDiv;
    }
}

uint32_t PRCMClockConfigureGet(uint32_t ui32Domain) {
    uint32_t ui32ClkDiv;

    // Check the arguments.
    ASSERT((ui32Domain == PRCM_DOMAIN_SYSBUS) ||
           (ui32Domain == PRCM_DOMAIN_CPU) ||
           (ui32Domain == PRCM_DOMAIN_PERIPH) ||
           (ui32Domain == PRCM_DOMAIN_TIMER) ||
           (ui32Domain == PRCM_DOMAIN_SERIAL));

    ui32ClkDiv = 0;

    // Find the correct sub system.
    if(ui32Domain == PRCM_DOMAIN_SYSBUS)
    {
        ui32ClkDiv = HWREG(PRCM_BASE + PRCM_O_SYSBUSCLKDIV);
    }
    else if(ui32Domain == PRCM_DOMAIN_CPU)
    {
        ui32ClkDiv = HWREG(PRCM_BASE + PRCM_O_CPUCLKDIV);
    }
    else if(ui32Domain == PRCM_DOMAIN_PERIPH)
    {
        ui32ClkDiv = HWREG(PRCM_BASE + PRCM_O_PERBUSCPUCLKDIV);
    }
    else if(ui32Domain == PRCM_DOMAIN_SERIAL)
    {
        ui32ClkDiv = HWREG(PRCM_BASE + PRCM_O_PERDMACLKDIV);
    }
    else if(ui32Domain == PRCM_DOMAIN_TIMER)
    {
        ui32ClkDiv = HWREG(PRCM_BASE + PRCM_O_GPTCLKDIV);
    }

    // Return the clock configuration.
    return(ui32ClkDiv);
}

void PRCMAudioClockConfigSet(uint32_t ui32ClkConfig, uint32_t ui32SampleRate) {
    uint32_t ui32Reg;
    uint32_t ui32MstDiv;
    uint32_t ui32BitDiv;
    uint32_t ui32WordDiv;

    // Check the arguments.
    ASSERT(!(ui32ClkConfig & (PRCM_I2SCLKCTL_WCLK_PHASE_M | PRCM_I2SCLKCTL_SMPL_ON_POSEDGE_M)));
    ASSERT((ui32SampleRate == I2S_SAMPLE_RATE_16K) ||
           (ui32SampleRate == I2S_SAMPLE_RATE_24K) ||
           (ui32SampleRate == I2S_SAMPLE_RATE_32K) ||
           (ui32SampleRate == I2S_SAMPLE_RATE_48K));

    ui32MstDiv = 0;
    ui32BitDiv = 0;
    ui32WordDiv = 0;

    // Make sure the audio clock generation is disabled before reconfiguring.
    PRCMAudioClockDisable();

    // Define the clock division factors for the audio interface.
    switch(ui32SampleRate)
    {
    case I2S_SAMPLE_RATE_16K :
        ui32MstDiv = 6;
        ui32BitDiv = 60;
        ui32WordDiv = 25;
        break;
    case I2S_SAMPLE_RATE_24K :
        ui32MstDiv = 4;
        ui32BitDiv = 40;
        ui32WordDiv = 25;
        break;
    case I2S_SAMPLE_RATE_32K :
        ui32MstDiv = 3;
        ui32BitDiv = 30;
        ui32WordDiv = 25;
        break;
    case I2S_SAMPLE_RATE_48K :
        ui32MstDiv = 2;
        ui32BitDiv = 20;
        ui32WordDiv = 25;
        break;
    }

    // Make sure to compensate the Frame clock division factor if using single
    // phase format.
    if((ui32ClkConfig & PRCM_I2SCLKCTL_WCLK_PHASE_M) == PRCM_WCLK_SINGLE_PHASE)
    {
        ui32WordDiv -= 1;
    }

    // Write the clock division factors.
    HWREG(PRCM_BASE + PRCM_O_I2SMCLKDIV) = ui32MstDiv;
    HWREG(PRCM_BASE + PRCM_O_I2SBCLKDIV) = ui32BitDiv;
    HWREG(PRCM_BASE + PRCM_O_I2SWCLKDIV) = ui32WordDiv;

    // Configure the Word clock format and polarity.
    ui32Reg = HWREG(PRCM_BASE + PRCM_O_I2SCLKCTL) & ~(PRCM_I2SCLKCTL_WCLK_PHASE_M |
              PRCM_I2SCLKCTL_SMPL_ON_POSEDGE_M);
    HWREG(PRCM_BASE + PRCM_O_I2SCLKCTL) = ui32Reg | ui32ClkConfig;
}

void PRCMAudioClockConfigSetOverride(uint32_t ui32ClkConfig, uint32_t ui32MstDiv, uint32_t ui32BitDiv, uint32_t ui32WordDiv) {
    uint32_t ui32Reg;

    // Check the arguments.
    ASSERT(!(ui32ClkConfig & (PRCM_I2SCLKCTL_WCLK_PHASE_M | PRCM_I2SCLKCTL_SMPL_ON_POSEDGE_M)));

    // Make sure the audio clock generation is disabled before reconfiguring.
    PRCMAudioClockDisable();

    // Make sure to compensate the Frame clock division factor if using single
    // phase format.
    if((ui32ClkConfig & PRCM_I2SCLKCTL_WCLK_PHASE_M) == PRCM_WCLK_SINGLE_PHASE)
    {
        ui32WordDiv -= 1;
    }

    // Write the clock division factors.
    HWREG(PRCM_BASE + PRCM_O_I2SMCLKDIV) = ui32MstDiv;
    HWREG(PRCM_BASE + PRCM_O_I2SBCLKDIV) = ui32BitDiv;
    HWREG(PRCM_BASE + PRCM_O_I2SWCLKDIV) = ui32WordDiv;

    // Configure the Word clock format and polarity.
    ui32Reg = HWREG(PRCM_BASE + PRCM_O_I2SCLKCTL) & ~(PRCM_I2SCLKCTL_WCLK_PHASE_M |
              PRCM_I2SCLKCTL_SMPL_ON_POSEDGE_M);
    HWREG(PRCM_BASE + PRCM_O_I2SCLKCTL) = ui32Reg | ui32ClkConfig;
}

void PRCMPowerDomainOn(uint32_t ui32Domains) {
    // Check the arguments.
    ASSERT((ui32Domains & PRCM_DOMAIN_RFCORE) ||
           (ui32Domains & PRCM_DOMAIN_SERIAL) ||
           (ui32Domains & PRCM_DOMAIN_PERIPH) ||
           (ui32Domains & PRCM_DOMAIN_CPU) ||
           (ui32Domains & PRCM_DOMAIN_VIMS));

    // Assert the request to power on the right domains.
    if(ui32Domains & PRCM_DOMAIN_RFCORE)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL0RFC   ) = 1;
    }
    if(ui32Domains & PRCM_DOMAIN_SERIAL)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL0SERIAL) = 1;
    }
    if(ui32Domains & PRCM_DOMAIN_PERIPH)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL0PERIPH) = 1;
    }
    if(ui32Domains & PRCM_DOMAIN_VIMS)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL1VIMS  ) = 1;
    }
    if(ui32Domains & PRCM_DOMAIN_CPU)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL1CPU   ) = 1;
    }
}

void PRCMPowerDomainOff(uint32_t ui32Domains) {
    // Check the arguments.
    ASSERT((ui32Domains & PRCM_DOMAIN_RFCORE) ||
           (ui32Domains & PRCM_DOMAIN_SERIAL) ||
           (ui32Domains & PRCM_DOMAIN_PERIPH) ||
           (ui32Domains & PRCM_DOMAIN_CPU) ||
           (ui32Domains & PRCM_DOMAIN_VIMS));

    // Assert the request to power off the right domains.
    if(ui32Domains & PRCM_DOMAIN_RFCORE)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL0RFC   ) = 0;
    }
    if(ui32Domains & PRCM_DOMAIN_SERIAL)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL0SERIAL) = 0;
    }
    if(ui32Domains & PRCM_DOMAIN_PERIPH)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL0PERIPH) = 0;
    }
    if(ui32Domains & PRCM_DOMAIN_VIMS)
    {
        // Write bits ui32Domains[17:16] to the VIMS_MODE alias register.
        // PRCM_DOMAIN_VIMS sets VIMS_MODE=0b00, PRCM_DOMAIN_VIMS_OFF_NO_WAKEUP sets VIMS_MODE=0b10.
        ASSERT(!(ui32Domains & 0x00010000));
        HWREG(PRCM_BASE + PRCM_O_PDCTL1VIMS  ) = ( ui32Domains >> 16 ) & 3;
    }
    if(ui32Domains & PRCM_DOMAIN_CPU)
    {
        HWREG(PRCM_BASE + PRCM_O_PDCTL1CPU   ) = 0;
    }
}

void PRCMPeripheralRunEnable(uint32_t ui32Peripheral) {
    // Check the arguments.
    ASSERT(PRCMPeripheralValid(ui32Peripheral));

    // Enable module in Run Mode.
    HWREG(PRCM_BASE + g_pui32RCGCRegs[PRCM_PERIPH_INDEX(ui32Peripheral)]) |=
        PRCM_PERIPH_MASKBIT(ui32Peripheral);
}

void PRCMPeripheralRunDisable(uint32_t ui32Peripheral) {
    // Check the arguments.
    ASSERT(PRCMPeripheralValid(ui32Peripheral));

    // Disable module in Run Mode.
    HWREG(PRCM_BASE + g_pui32RCGCRegs[PRCM_PERIPH_INDEX(ui32Peripheral)]) &=
        ~PRCM_PERIPH_MASKBIT(ui32Peripheral);
}

void PRCMPeripheralSleepEnable(uint32_t ui32Peripheral) {
    // Check the arguments.
    ASSERT(PRCMPeripheralValid(ui32Peripheral));

    // Enable this peripheral in sleep mode.
    HWREG(PRCM_BASE + g_pui32SCGCRegs[PRCM_PERIPH_INDEX(ui32Peripheral)]) |=
        PRCM_PERIPH_MASKBIT(ui32Peripheral);
}

void PRCMPeripheralSleepDisable(uint32_t ui32Peripheral) {
    // Check the arguments.
    ASSERT(PRCMPeripheralValid(ui32Peripheral));

    // Disable this peripheral in sleep mode
    HWREG(PRCM_BASE + g_pui32SCGCRegs[PRCM_PERIPH_INDEX(ui32Peripheral)]) &=
        ~PRCM_PERIPH_MASKBIT(ui32Peripheral);
}

void PRCMPeripheralDeepSleepEnable(uint32_t ui32Peripheral) {
    // Check the arguments.
    ASSERT(PRCMPeripheralValid(ui32Peripheral));

    // Enable this peripheral in deep-sleep mode.
    HWREG(PRCM_BASE + g_pui32DCGCRegs[PRCM_PERIPH_INDEX(ui32Peripheral)]) |=
        PRCM_PERIPH_MASKBIT(ui32Peripheral);
}

void PRCMPeripheralDeepSleepDisable(uint32_t ui32Peripheral) {
    // Check the arguments.
    ASSERT(PRCMPeripheralValid(ui32Peripheral));

    // Disable this peripheral in Deep Sleep mode.
    HWREG(PRCM_BASE + g_pui32DCGCRegs[PRCM_PERIPH_INDEX(ui32Peripheral)]) &=
        ~PRCM_PERIPH_MASKBIT(ui32Peripheral);
}

uint32_t PRCMPowerDomainStatus(uint32_t ui32Domains) {
    bool bStatus;
    uint32_t ui32StatusRegister0;
    uint32_t ui32StatusRegister1;

    // Check the arguments.
    ASSERT((ui32Domains & (PRCM_DOMAIN_RFCORE |
                           PRCM_DOMAIN_SERIAL |
                           PRCM_DOMAIN_PERIPH)));

    bStatus = true;
    ui32StatusRegister0 = HWREG(PRCM_BASE + PRCM_O_PDSTAT0);
    ui32StatusRegister1 = HWREG(PRCM_BASE + PRCM_O_PDSTAT1);

    // Return the correct power status.
    if(ui32Domains & PRCM_DOMAIN_RFCORE)
    {
       bStatus = bStatus &&
                 ((ui32StatusRegister0 & PRCM_PDSTAT0_RFC_ON) ||
                  (ui32StatusRegister1 & PRCM_PDSTAT1_RFC_ON));
    }
    if(ui32Domains & PRCM_DOMAIN_SERIAL)
    {
        bStatus = bStatus && (ui32StatusRegister0 & PRCM_PDSTAT0_SERIAL_ON);
    }
    if(ui32Domains & PRCM_DOMAIN_PERIPH)
    {
        bStatus = bStatus && (ui32StatusRegister0 & PRCM_PDSTAT0_PERIPH_ON);
    }

    // Return the status.
    return (bStatus ? PRCM_DOMAIN_POWER_ON : PRCM_DOMAIN_POWER_OFF);
}

void PRCMDeepSleep(void) {
    // Enable deep-sleep.
    HWREG(NVIC_SYS_CTRL) |= NVIC_SYS_CTRL_SLEEPDEEP;

    // Wait for an interrupt.
    CPUwfi();

    // Disable deep-sleep so that a future sleep will work correctly.
    HWREG(NVIC_SYS_CTRL) &= ~(NVIC_SYS_CTRL_SLEEPDEEP);
}

void PRCMRetentionEnable(uint32_t ui32PowerDomain) {
    uint32_t ui32Retention;

    // Check the arguments.
    ASSERT(PRCM_DOMAIN_CPU & ui32PowerDomain);

    // Get the current register values.
    ui32Retention = HWREG(PRCM_BASE + PRCM_O_RAMRETEN);

    // Enable retention on RF core SRAM.
    if(PRCM_DOMAIN_RFCORE & ui32PowerDomain)
    {
        ui32Retention |= PRCM_RAMRETEN_RFC_M;
    }

    // Enable retention on VIMS cache.
    if(PRCM_DOMAIN_VIMS & ui32PowerDomain)
    {
        ui32Retention |= PRCM_RAMRETEN_VIMS_M;
    }

    // Enable retention on RFC ULL SRAM.
    if(PRCM_DOMAIN_RFCULL_SRAM & ui32PowerDomain)
    {
        ui32Retention |= PRCM_RAMRETEN_RFCULL_M;
    }

    // Reconfigure retention.
    HWREG(PRCM_BASE + PRCM_O_RAMRETEN) = ui32Retention;
}

void PRCMRetentionDisable(uint32_t ui32PowerDomain) {
    uint32_t ui32Retention;

    // Check the arguments.
    ASSERT(PRCM_DOMAIN_CPU & ui32PowerDomain);

    // Get the current register values
    ui32Retention = HWREG(PRCM_BASE + PRCM_O_RAMRETEN);

    // Disable retention on RF core SRAM
    if(PRCM_DOMAIN_RFCORE & ui32PowerDomain)
    {
        ui32Retention &= ~PRCM_RAMRETEN_RFC_M;
    }

    // Disable retention on VIMS cache
    if(PRCM_DOMAIN_VIMS & ui32PowerDomain)
    {
        ui32Retention &= ~PRCM_RAMRETEN_VIMS_M;
    }

    // Disable retention on RFC ULL SRAM.
    if(PRCM_DOMAIN_RFCULL_SRAM & ui32PowerDomain)
    {
        ui32Retention &= ~PRCM_RAMRETEN_RFCULL_M;
    }

    // Reconfigure retention.
    HWREG(PRCM_BASE + PRCM_O_RAMRETEN) = ui32Retention;
}

void SMPHAcquire(uint32_t ui32Semaphore) {
    // Check the arguments.
    ASSERT((ui32Semaphore == SMPH_0) ||
           (ui32Semaphore == SMPH_1) ||
           (ui32Semaphore == SMPH_2) ||
           (ui32Semaphore == SMPH_3) ||
           (ui32Semaphore == SMPH_4) ||
           (ui32Semaphore == SMPH_5) ||
           (ui32Semaphore == SMPH_6) ||
           (ui32Semaphore == SMPH_7) ||
           (ui32Semaphore == SMPH_8) ||
           (ui32Semaphore == SMPH_9) ||
           (ui32Semaphore == SMPH_10) ||
           (ui32Semaphore == SMPH_11) ||
           (ui32Semaphore == SMPH_12) ||
           (ui32Semaphore == SMPH_13) ||
           (ui32Semaphore == SMPH_14) ||
           (ui32Semaphore == SMPH_15) ||
           (ui32Semaphore == SMPH_16) ||
           (ui32Semaphore == SMPH_17) ||
           (ui32Semaphore == SMPH_18) ||
           (ui32Semaphore == SMPH_19) ||
           (ui32Semaphore == SMPH_20) ||
           (ui32Semaphore == SMPH_21) ||
           (ui32Semaphore == SMPH_22) ||
           (ui32Semaphore == SMPH_23) ||
           (ui32Semaphore == SMPH_24) ||
           (ui32Semaphore == SMPH_25) ||
           (ui32Semaphore == SMPH_26) ||
           (ui32Semaphore == SMPH_27) ||
           (ui32Semaphore == SMPH_28) ||
           (ui32Semaphore == SMPH_29) ||
           (ui32Semaphore == SMPH_30) ||
           (ui32Semaphore == SMPH_31));

    // Wait for semaphore to be release such that it can be claimed
    // Semaphore register reads 1 when lock was acquired otherwise 0
    // (i.e. SMPH_CLAIMED).
    while(HWREG(SMPH_BASE + SMPH_O_SMPH0 + 4 * ui32Semaphore) ==
            SMPH_CLAIMED)
    {
    }
}

void SSIConfigSetExpClk(uint32_t ui32Base, uint32_t ui32SSIClk, uint32_t ui32Protocol, uint32_t ui32Mode, uint32_t ui32BitRate, uint32_t ui32DataWidth) {
    uint32_t ui32MaxBitRate;
    uint32_t ui32RegVal;
    uint32_t ui32PreDiv;
    uint32_t ui32SCR;
    uint32_t ui32SPH_SPO;

    // Check the arguments.
    ASSERT(SSIBaseValid(ui32Base));
    ASSERT((ui32Protocol == SSI_FRF_MOTO_MODE_0) ||
           (ui32Protocol == SSI_FRF_MOTO_MODE_1) ||
           (ui32Protocol == SSI_FRF_MOTO_MODE_2) ||
           (ui32Protocol == SSI_FRF_MOTO_MODE_3) ||
           (ui32Protocol == SSI_FRF_TI) ||
           (ui32Protocol == SSI_FRF_NMW));
    ASSERT((ui32Mode == SSI_MODE_MASTER) ||
           (ui32Mode == SSI_MODE_SLAVE) ||
           (ui32Mode == SSI_MODE_SLAVE_OD));
    ASSERT(((ui32Mode == SSI_MODE_MASTER) && (ui32BitRate <= (ui32SSIClk / 2))) ||
           ((ui32Mode != SSI_MODE_MASTER) && (ui32BitRate <= (ui32SSIClk / 12))));
    ASSERT((ui32SSIClk / ui32BitRate) <= (254 * 256));
    ASSERT((ui32DataWidth >= 4) && (ui32DataWidth <= 16));

    // Set the mode.
    ui32RegVal = (ui32Mode == SSI_MODE_SLAVE_OD) ? SSI_CR1_SOD : 0;
    ui32RegVal |= (ui32Mode == SSI_MODE_MASTER) ? 0 : SSI_CR1_MS;
    HWREG(ui32Base + SSI_O_CR1) = ui32RegVal;

    // Set the clock predivider.
    ui32MaxBitRate = ui32SSIClk / ui32BitRate;
    ui32PreDiv = 0;
    do
    {
        ui32PreDiv += 2;
        ui32SCR = (ui32MaxBitRate / ui32PreDiv) - 1;
    }
    while(ui32SCR > 255);
    HWREG(ui32Base + SSI_O_CPSR) = ui32PreDiv;

    // Set protocol and clock rate.
    ui32SPH_SPO = (ui32Protocol & 3) << 6;
    ui32Protocol &= SSI_CR0_FRF_M;
    ui32RegVal = (ui32SCR << 8) | ui32SPH_SPO | ui32Protocol | (ui32DataWidth - 1);
    HWREG(ui32Base + SSI_O_CR0) = ui32RegVal;
}

int32_t SSIDataPutNonBlocking(uint32_t ui32Base, uint32_t ui32Data) {
    // Check the arguments.
    ASSERT(SSIBaseValid(ui32Base));
    ASSERT((ui32Data & (0xfffffffe << (HWREG(ui32Base + SSI_O_CR0) &
                                       SSI_CR0_DSS_M))) == 0);

    // Check for space to write.
    if(HWREG(ui32Base + SSI_O_SR) & SSI_SR_TNF)
    {
        HWREG(ui32Base + SSI_O_DR) = ui32Data;
        return(1);
    }
    else
    {
        return(0);
    }
}

void SSIDataPut(uint32_t ui32Base, uint32_t ui32Data) {
    // Check the arguments.
    ASSERT(SSIBaseValid(ui32Base));
    ASSERT((ui32Data & (0xfffffffe << (HWREG(ui32Base + SSI_O_CR0) &
                                       SSI_CR0_DSS_M))) == 0);

    // Wait until there is space.
    while(!(HWREG(ui32Base + SSI_O_SR) & SSI_SR_TNF))
    {
    }

    // Write the data to the SSI.
    HWREG(ui32Base + SSI_O_DR) = ui32Data;
}

void SSIDataGet(uint32_t ui32Base, uint32_t *pui32Data) {
    // Check the arguments.
    ASSERT(SSIBaseValid(ui32Base));

    // Wait until there is data to be read.
    while(!(HWREG(ui32Base + SSI_O_SR) & SSI_SR_RNE))
    {
    }

    // Read data from SSI.
    *pui32Data = HWREG(ui32Base + SSI_O_DR);
}

int32_t SSIDataGetNonBlocking(uint32_t ui32Base, uint32_t *pui32Data) {
    // Check the arguments.
    ASSERT(SSIBaseValid(ui32Base));

    // Check for data to read.
    if(HWREG(ui32Base + SSI_O_SR) & SSI_SR_RNE)
    {
        *pui32Data = HWREG(ui32Base + SSI_O_DR);
        return(1);
    }
    else
    {
        return(0);
    }
}

void TimerConfigure(uint32_t ui32Base, uint32_t ui32Config) {
    // Check the arguments.
    ASSERT(TimerBaseValid(ui32Base));
    ASSERT((ui32Config == TIMER_CFG_ONE_SHOT) ||
           (ui32Config == TIMER_CFG_ONE_SHOT_UP) ||
           (ui32Config == TIMER_CFG_PERIODIC) ||
           (ui32Config == TIMER_CFG_PERIODIC_UP) ||
           ((ui32Config & 0xFF000000) == TIMER_CFG_SPLIT_PAIR));
    ASSERT(((ui32Config & 0xFF000000) != TIMER_CFG_SPLIT_PAIR) ||
           ((((ui32Config & 0x000000FF) == TIMER_CFG_A_ONE_SHOT) ||
             ((ui32Config & 0x000000FF) == TIMER_CFG_A_ONE_SHOT_UP) ||
             ((ui32Config & 0x000000FF) == TIMER_CFG_A_PERIODIC) ||
             ((ui32Config & 0x000000FF) == TIMER_CFG_A_PERIODIC_UP) ||
             ((ui32Config & 0x000000FF) == TIMER_CFG_A_CAP_COUNT) ||
             ((ui32Config & 0x000000FF) == TIMER_CFG_A_CAP_COUNT_UP) ||
             ((ui32Config & 0x000000FF) == TIMER_CFG_A_CAP_TIME) ||
             ((ui32Config & 0x000000FF) == TIMER_CFG_A_CAP_TIME_UP) ||
             ((ui32Config & 0x000000FF) == TIMER_CFG_A_PWM)) &&
            (((ui32Config & 0x0000FF00) == TIMER_CFG_B_ONE_SHOT) ||
             ((ui32Config & 0x0000FF00) == TIMER_CFG_B_ONE_SHOT_UP) ||
             ((ui32Config & 0x0000FF00) == TIMER_CFG_B_PERIODIC) ||
             ((ui32Config & 0x0000FF00) == TIMER_CFG_B_PERIODIC_UP) ||
             ((ui32Config & 0x0000FF00) == TIMER_CFG_B_CAP_COUNT) ||
             ((ui32Config & 0x0000FF00) == TIMER_CFG_B_CAP_COUNT_UP) ||
             ((ui32Config & 0x0000FF00) == TIMER_CFG_B_CAP_TIME) ||
             ((ui32Config & 0x0000FF00) == TIMER_CFG_B_CAP_TIME_UP) ||
             ((ui32Config & 0x0000FF00) == TIMER_CFG_B_PWM))));

    // Disable the timers.
    HWREG(ui32Base + GPT_O_CTL) &= ~(GPT_CTL_TAEN | GPT_CTL_TBEN);

    // Set the global timer configuration.
    HWREG(ui32Base + GPT_O_CFG) = ui32Config >> 24;

    // Set the configuration of the A and B timers. Note that the B timer
    // configuration is ignored by the hardware in 32-bit modes.
    HWREG(ui32Base + GPT_O_TAMR) = (ui32Config & 0xFF) | GPT_TAMR_TAPWMIE;
    HWREG(ui32Base + GPT_O_TBMR) =
        ((ui32Config >> 8) & 0xFF) | GPT_TBMR_TBPWMIE;
}

void TimerLevelControl(uint32_t ui32Base, uint32_t ui32Timer, bool bInvert) {
    // Check the arguments.
    ASSERT(TimerBaseValid(ui32Base));
    ASSERT((ui32Timer == TIMER_A) || (ui32Timer == TIMER_B) ||
           (ui32Timer == TIMER_BOTH));

    // Set the output levels as requested.
    ui32Timer &= GPT_CTL_TAPWML | GPT_CTL_TBPWML;
    HWREG(ui32Base + GPT_O_CTL) = (bInvert ?
                                   (HWREG(ui32Base + GPT_O_CTL) | ui32Timer) :
                                   (HWREG(ui32Base + GPT_O_CTL) &
                                   ~(ui32Timer)));
}

void TimerStallControl(uint32_t ui32Base, uint32_t ui32Timer, bool bStall) {
    // Check the arguments.
    ASSERT(TimerBaseValid(ui32Base));
    ASSERT((ui32Timer == TIMER_A) || (ui32Timer == TIMER_B) ||
           (ui32Timer == TIMER_BOTH));

    // Set the stall mode.
    ui32Timer &= GPT_CTL_TASTALL | GPT_CTL_TBSTALL;
    HWREG(ui32Base + GPT_O_CTL) = (bStall ?
                                   (HWREG(ui32Base + GPT_O_CTL) | ui32Timer) :
                                   (HWREG(ui32Base + GPT_O_CTL) & ~(ui32Timer)));
}

void TimerWaitOnTriggerControl(uint32_t ui32Base, uint32_t ui32Timer, bool bWait) {
    // Check the arguments.
    ASSERT(TimerBaseValid(ui32Base));
    ASSERT((ui32Timer == TIMER_A) || (ui32Timer == TIMER_B) ||
           (ui32Timer == TIMER_BOTH));

    // Set the wait on trigger mode for timer A.
    if(ui32Timer & TIMER_A)
    {
        if(bWait)
        {
            HWREG(ui32Base + GPT_O_TAMR) |= GPT_TAMR_TAWOT;
        }
        else
        {
            HWREG(ui32Base + GPT_O_TAMR) &= ~(GPT_TAMR_TAWOT);
        }
    }

    // Set the wait on trigger mode for timer B.
    if(ui32Timer & TIMER_B)
    {
        if(bWait)
        {
            HWREG(ui32Base + GPT_O_TBMR) |= GPT_TBMR_TBWOT;
        }
        else
        {
            HWREG(ui32Base + GPT_O_TBMR) &= ~(GPT_TBMR_TBWOT);
        }
    }
}

void TimerMatchUpdateMode(uint32_t ui32Base, uint32_t ui32Timer, uint32_t ui32Mode) {
    // Check the arguments
    ASSERT(TimerBaseValid(ui32Base));
    ASSERT((ui32Timer == TIMER_A) || (ui32Timer == TIMER_B) || (ui32Timer == TIMER_BOTH));
    ASSERT((ui32Mode == TIMER_MATCHUPDATE_NEXTCYCLE) || (ui32Mode == TIMER_MATCHUPDATE_TIMEOUT));

    // Set mode for timer A
    if(ui32Timer & TIMER_A)
    {
        if(ui32Mode == TIMER_MATCHUPDATE_NEXTCYCLE)
        {
            HWREG(ui32Base + GPT_O_TAMR) &= ~(GPT_TAMR_TAMRSU);
        }
        else
        {
            HWREG(ui32Base + GPT_O_TAMR) |= GPT_TAMR_TAMRSU;
        }
    }

    // Set mode for timer B
    if(ui32Timer & TIMER_B)
    {
        if(ui32Mode == TIMER_MATCHUPDATE_NEXTCYCLE)
        {
            HWREG(ui32Base + GPT_O_TBMR) &= ~(GPT_TBMR_TBMRSU);
        }
        else
        {
            HWREG(ui32Base + GPT_O_TBMR) |= GPT_TBMR_TBMRSU;
        }
    }
}

void TimerIntervalLoadMode(uint32_t ui32Base, uint32_t ui32Timer, uint32_t ui32Mode) {
    // Check the arguments
    ASSERT(TimerBaseValid(ui32Base));
    ASSERT((ui32Timer == TIMER_A) || (ui32Timer == TIMER_B) || (ui32Timer == TIMER_BOTH));
    ASSERT((ui32Mode == TIMER_INTERVALLOAD_NEXTCYCLE) || (ui32Mode == TIMER_INTERVALLOAD_TIMEOUT));

    // Set mode for timer A
    if(ui32Timer & TIMER_A)
    {
        if(ui32Mode == TIMER_INTERVALLOAD_NEXTCYCLE)
        {
            HWREG(ui32Base + GPT_O_TAMR) &= ~(GPT_TAMR_TAILD);
        }
        else
        {
            HWREG(ui32Base + GPT_O_TAMR) |= GPT_TAMR_TAILD;
        }
    }

    // Set mode for timer B
    if(ui32Timer & TIMER_B)
    {
        if(ui32Mode == TIMER_INTERVALLOAD_NEXTCYCLE)
        {
            HWREG(ui32Base + GPT_O_TBMR) &= ~(GPT_TBMR_TBILD);
        }
        else
        {
            HWREG(ui32Base + GPT_O_TBMR) |= GPT_TBMR_TBILD;
        }
    }
}

void TRNGConfigure(uint32_t ui32MinSamplesPerCycle, uint32_t ui32MaxSamplesPerCycle, uint32_t ui32ClocksPerSample) {
    uint32_t ui32Val;

    // Make sure the TRNG is disabled.
    ui32Val = HWREG(TRNG_BASE + TRNG_O_CTL) & ~TRNG_CTL_TRNG_EN;
    HWREG(TRNG_BASE + TRNG_O_CTL) = ui32Val;

    // Configure the startup number of samples.
    ui32Val &= ~TRNG_CTL_STARTUP_CYCLES_M;
    ui32Val |= ((( ui32MaxSamplesPerCycle >> 8 ) << TRNG_CTL_STARTUP_CYCLES_S ) & TRNG_CTL_STARTUP_CYCLES_M );
    HWREG(TRNG_BASE + TRNG_O_CTL) = ui32Val;

    // Configure the minimum and maximum number of samples pr generated number
    // and the number of clocks per sample.
    HWREG(TRNG_BASE + TRNG_O_CFG0) = (
        ((( ui32MaxSamplesPerCycle >> 8 ) << TRNG_CFG0_MAX_REFILL_CYCLES_S ) & TRNG_CFG0_MAX_REFILL_CYCLES_M ) |
        ((( ui32ClocksPerSample         ) << TRNG_CFG0_SMPL_DIV_S          ) & TRNG_CFG0_SMPL_DIV_M          ) |
        ((( ui32MinSamplesPerCycle >> 6 ) << TRNG_CFG0_MIN_REFILL_CYCLES_S ) & TRNG_CFG0_MIN_REFILL_CYCLES_M )   );
}

uint32_t TRNGNumberGet(uint32_t ui32Word) {
    uint32_t ui32RandomNumber;

    // Check the arguments.
    ASSERT((ui32Word == TRNG_HI_WORD) ||
           (ui32Word == TRNG_LOW_WORD));

    // Return the right requested part of the generated number.
    if(ui32Word == TRNG_HI_WORD)
    {
        ui32RandomNumber = HWREG(TRNG_BASE + TRNG_O_OUT1);
    }
    else
    {
        ui32RandomNumber = HWREG(TRNG_BASE + TRNG_O_OUT0);
    }

    // Initiate generation of new number.
    HWREG(TRNG_BASE + TRNG_O_IRQFLAGCLR) = 0x1;

    // Return the random number.
    return ui32RandomNumber;
}

void UARTFIFOLevelGet(uint32_t ui32Base, uint32_t *pui32TxLevel, uint32_t *pui32RxLevel) {
    uint32_t ui32Temp;

    // Check the arguments.
    ASSERT(UARTBaseValid(ui32Base));

    // Read the FIFO level register.
    ui32Temp = HWREG(ui32Base + UART_O_IFLS);

    // Extract the transmit and receive FIFO levels.
    *pui32TxLevel = ui32Temp & UART_IFLS_TXSEL_M;
    *pui32RxLevel = ui32Temp & UART_IFLS_RXSEL_M;
}

void UARTConfigSetExpClk(uint32_t ui32Base, uint32_t ui32UARTClk, uint32_t ui32Baud, uint32_t ui32Config) {
    uint32_t ui32Div;

    // Check the arguments.
    ASSERT(UARTBaseValid(ui32Base));
    ASSERT(ui32Baud != 0);

    // Stop the UART.
    UARTDisable(ui32Base);

    // Compute the fractional baud rate divider.
    ui32Div = (((ui32UARTClk * 8) / ui32Baud) + 1) / 2;

    // Set the baud rate.
    HWREG(ui32Base + UART_O_IBRD) = ui32Div / 64;
    HWREG(ui32Base + UART_O_FBRD) = ui32Div % 64;

    // Set parity, data length, and number of stop bits.
    HWREG(ui32Base + UART_O_LCRH) = ui32Config;
}

void UARTConfigGetExpClk(uint32_t ui32Base, uint32_t ui32UARTClk, uint32_t *pui32Baud, uint32_t *pui32Config) {
    uint32_t ui32Int, ui32Frac;

    // Check the arguments.
    ASSERT(UARTBaseValid(ui32Base));

    // Compute the baud rate.
    ui32Int = HWREG(ui32Base + UART_O_IBRD);
    ui32Frac = HWREG(ui32Base + UART_O_FBRD);
    *pui32Baud = (ui32UARTClk * 4) / ((64 * ui32Int) + ui32Frac);

    // Get the parity, data length, and number of stop bits.
    *pui32Config = (HWREG(ui32Base + UART_O_LCRH) &
                    (UART_LCRH_SPS | UART_LCRH_WLEN_M | UART_LCRH_STP2 |
                     UART_LCRH_EPS | UART_LCRH_PEN));
}

void UARTDisable(uint32_t ui32Base) {

    // Check the arguments.
    ASSERT(UARTBaseValid(ui32Base));

    // Wait for end of TX.
    while(HWREG(ui32Base + UART_O_FR) & UART_FR_BUSY)
    {
    }

    // Disable the FIFO.
    HWREG(ui32Base + UART_O_LCRH) &= ~(UART_LCRH_FEN);

    // Disable the UART.
    HWREG(ui32Base + UART_O_CTL) &= ~(UART_CTL_UARTEN | UART_CTL_TXE |
                                      UART_CTL_RXE);
}

int32_t UARTCharGetNonBlocking(uint32_t ui32Base) {
    // Check the arguments.
    ASSERT(UARTBaseValid(ui32Base));

    // See if there are any characters in the receive FIFO.
    if(!(HWREG(ui32Base + UART_O_FR) & UART_FR_RXFE))
    {
        // Read and return the next character.
        return(HWREG(ui32Base + UART_O_DR));
    }
    else
    {
        // There are no characters, so return a failure.
        return(-1);
    }
}

int32_t UARTCharGet(uint32_t ui32Base) {
    // Check the arguments.
    ASSERT(UARTBaseValid(ui32Base));

    // Wait until a char is available.
    while(HWREG(ui32Base + UART_O_FR) & UART_FR_RXFE)
    {
    }

    // Now get the character.
    return(HWREG(ui32Base + UART_O_DR));
}

bool UARTCharPutNonBlocking(uint32_t ui32Base, uint8_t ui8Data) {
    // Check the arguments.
    ASSERT(UARTBaseValid(ui32Base));

    // See if there is space in the transmit FIFO.
    if(!(HWREG(ui32Base + UART_O_FR) & UART_FR_TXFF))
    {
        // Write this character to the transmit FIFO.
        HWREG(ui32Base + UART_O_DR) = ui8Data;

        // Success.
        return(true);
    }
    else
    {
        // There is no space in the transmit FIFO, so return a failure.
        return(false);
    }
}

void UARTCharPut(uint32_t ui32Base, uint8_t ui8Data) {
    // Check the arguments.
    ASSERT(UARTBaseValid(ui32Base));

    // Wait until space is available.
    while(HWREG(ui32Base + UART_O_FR) & UART_FR_TXFF)
    {
    }

    // Send the char.
    HWREG(ui32Base + UART_O_DR) = ui8Data;
}

void uDMAChannelAttributeEnable(uint32_t ui32Base, uint32_t ui32ChannelNum, uint32_t ui32Attr) {
    // Check the arguments.
    ASSERT(uDMABaseValid(ui32Base));
    ASSERT(ui32ChannelNum < UDMA_NUM_CHANNELS);
    ASSERT((ui32Attr & ~(UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT |
                         UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK)) == 0);

    // Set the useburst bit for this channel if set in ui32Attr.
    if(ui32Attr & UDMA_ATTR_USEBURST)
    {
        HWREG(ui32Base + UDMA_O_SETBURST) = 1 << ui32ChannelNum;
    }

    // Set the alternate control select bit for this channel,
    // if set in ui32Attr.
    if(ui32Attr & UDMA_ATTR_ALTSELECT)
    {
        HWREG(ui32Base + UDMA_O_SETCHNLPRIALT) = 1 << ui32ChannelNum;
    }

    // Set the high priority bit for this channel, if set in ui32Attr.
    if(ui32Attr & UDMA_ATTR_HIGH_PRIORITY)
    {
        HWREG(ui32Base + UDMA_O_SETCHNLPRIORITY) = 1 << ui32ChannelNum;
    }

    // Set the request mask bit for this channel, if set in ui32Attr.
    if(ui32Attr & UDMA_ATTR_REQMASK)
    {
        HWREG(ui32Base + UDMA_O_SETREQMASK) = 1 << ui32ChannelNum;
    }
}

void uDMAChannelAttributeDisable(uint32_t ui32Base, uint32_t ui32ChannelNum, uint32_t ui32Attr) {
    // Check the arguments.
    ASSERT(uDMABaseValid(ui32Base));
    ASSERT(ui32ChannelNum < UDMA_NUM_CHANNELS);
    ASSERT((ui32Attr & ~(UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT |
                         UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK)) == 0);

    // Clear the useburst bit for this channel if set in ui32Attr.
    if(ui32Attr & UDMA_ATTR_USEBURST)
    {
        HWREG(ui32Base + UDMA_O_CLEARBURST) = 1 << ui32ChannelNum;
    }

    // Clear the alternate control select bit for this channel, if set in
    // ululAttr.
    if(ui32Attr & UDMA_ATTR_ALTSELECT)
    {
        HWREG(ui32Base + UDMA_O_CLEARCHNLPRIALT) = 1 << ui32ChannelNum;
    }

    // Clear the high priority bit for this channel, if set in ui32Attr.
    if(ui32Attr & UDMA_ATTR_HIGH_PRIORITY)
    {
        HWREG(ui32Base + UDMA_O_CLEARCHNLPRIORITY) = 1 << ui32ChannelNum;
    }

    // Clear the request mask bit for this channel, if set in ui32Attr.
    if(ui32Attr & UDMA_ATTR_REQMASK)
    {
        HWREG(ui32Base + UDMA_O_CLEARREQMASK) = 1 << ui32ChannelNum;
    }
}

uint32_t uDMAChannelAttributeGet(uint32_t ui32Base, uint32_t ui32ChannelNum) {
    uint32_t ui32Attr = 0;

    // Check the arguments.
    ASSERT(uDMABaseValid(ui32Base));
    ASSERT(ui32ChannelNum < UDMA_NUM_CHANNELS);

    // Check to see if useburst bit is set for this channel.
    if(HWREG(ui32Base + UDMA_O_SETBURST) & (1 << ui32ChannelNum))
    {
        ui32Attr |= UDMA_ATTR_USEBURST;
    }

    // Check to see if the alternate control bit is set for this channel.
    if(HWREG(ui32Base + UDMA_O_SETCHNLPRIALT) & (1 << ui32ChannelNum))
    {
        ui32Attr |= UDMA_ATTR_ALTSELECT;
    }

    // Check to see if the high priority bit is set for this channel.
    if(HWREG(ui32Base + UDMA_O_SETCHNLPRIORITY) & (1 << ui32ChannelNum))
    {
        ui32Attr |= UDMA_ATTR_HIGH_PRIORITY;
    }

    // Check to see if the request mask bit is set for this channel.
    if(HWREG(ui32Base + UDMA_O_SETREQMASK) & (1 << ui32ChannelNum))
    {
        ui32Attr |= UDMA_ATTR_REQMASK;
    }

    // Return the configuration flags.
    return(ui32Attr);
}

void uDMAChannelControlSet(uint32_t ui32Base, uint32_t ui32ChannelStructIndex, uint32_t ui32Control) {
    tDMAControlTable *pControlTable;

    // Check the arguments.
    ASSERT(uDMABaseValid(ui32Base));
    ASSERT(ui32ChannelStructIndex < (UDMA_NUM_CHANNELS * 2));
    ASSERT(HWREG(ui32Base + UDMA_O_CTRL) != 0);

    // Get the base address of the control table.
    pControlTable = (tDMAControlTable *)HWREG(ui32Base + UDMA_O_CTRL);

    // Get the current control word value and mask off the fields to be
    // changed, then OR in the new settings.
    pControlTable[ui32ChannelStructIndex].ui32Control =
        ((pControlTable[ui32ChannelStructIndex].ui32Control &
          ~(UDMA_DST_INC_M |
            UDMA_SRC_INC_M |
            UDMA_SIZE_M |
            UDMA_ARB_M |
            UDMA_NEXT_USEBURST)) |
         ui32Control);
}

void uDMAChannelTransferSet(uint32_t ui32Base, uint32_t ui32ChannelStructIndex, uint32_t ui32Mode, void *pvSrcAddr, void *pvDstAddr, uint32_t ui32TransferSize) {
    tDMAControlTable *pControlTable;
    uint32_t ui32Control;
    uint32_t ui32Inc;
    uint32_t ui32BufferBytes;

    // Check the arguments.
    ASSERT(uDMABaseValid(ui32Base));
    ASSERT(ui32ChannelStructIndex < (UDMA_NUM_CHANNELS * 2));
    ASSERT(HWREG(ui32Base + UDMA_O_CTRL) != 0);
    ASSERT(ui32Mode <= UDMA_MODE_PER_SCATTER_GATHER);
    ASSERT((uint32_t)pvSrcAddr >= SRAM_BASE);
    ASSERT((uint32_t)pvDstAddr >= SRAM_BASE);
    ASSERT((ui32TransferSize != 0) && (ui32TransferSize <= UDMA_XFER_SIZE_MAX));

    // Get the base address of the control table.
    pControlTable = (tDMAControlTable *)HWREG(ui32Base + UDMA_O_CTRL);

    // Get the current control word value and mask off the mode and size
    // fields.
    ui32Control = (pControlTable[ui32ChannelStructIndex].ui32Control &
                   ~(UDMA_XFER_SIZE_M | UDMA_MODE_M));

    // Adjust the mode if the alt control structure is selected.
    if(ui32ChannelStructIndex & UDMA_ALT_SELECT)
    {
        if((ui32Mode == UDMA_MODE_MEM_SCATTER_GATHER) ||
           (ui32Mode == UDMA_MODE_PER_SCATTER_GATHER))
        {
            ui32Mode |= UDMA_MODE_ALT_SELECT;
        }
    }

    // Set the transfer size and mode in the control word (but don't write the
    // control word yet as it could kick off a transfer).
    ui32Control |= ui32Mode | ((ui32TransferSize - 1) << UDMA_XFER_SIZE_S);

    // Get the address increment value for the source, from the control word.
    ui32Inc = (ui32Control & UDMA_SRC_INC_M);

    // Compute the ending source address of the transfer.  If the source
    // increment is set to none, then the ending address is the same as the
    // beginning.
    if(ui32Inc != UDMA_SRC_INC_NONE)
    {
        ui32Inc = ui32Inc >> UDMA_SRC_INC_S;
        ui32BufferBytes = ui32TransferSize << ui32Inc;
        pvSrcAddr = (void *)((uint32_t)pvSrcAddr + ui32BufferBytes - (1 << ui32Inc));
    }

    // Load the source ending address into the control block.
    pControlTable[ui32ChannelStructIndex].pvSrcEndAddr = pvSrcAddr;

    // Get the address increment value for the destination, from the control
    // word.
    ui32Inc = ui32Control & UDMA_DST_INC_M;

    // Compute the ending destination address of the transfer.  If the
    // destination increment is set to none, then the ending address is the
    // same as the beginning.
    if(ui32Inc != UDMA_DST_INC_NONE)
    {
        // There is a special case if this is setting up a scatter-gather
        // transfer.  The destination pointer needs to point to the end of
        // the alternate structure for this channel instead of calculating
        // the end of the buffer in the normal way.
        if((ui32Mode == UDMA_MODE_MEM_SCATTER_GATHER) ||
           (ui32Mode == UDMA_MODE_PER_SCATTER_GATHER))
        {
            pvDstAddr =
                (void *)&pControlTable[ui32ChannelStructIndex |
                                       UDMA_ALT_SELECT].ui32Spare;
        }
        // Not a scatter-gather transfer, calculate end pointer normally.
        else
        {
            ui32Inc = ui32Inc >> UDMA_DST_INC_S;
            ui32BufferBytes = ui32TransferSize << ui32Inc;
            pvDstAddr = (void *)((uint32_t)pvDstAddr + ui32BufferBytes - 1);
        }
    }

    // Load the destination ending address into the control block.
    pControlTable[ui32ChannelStructIndex].pvDstEndAddr = pvDstAddr;

    // Write the new control word value.
    pControlTable[ui32ChannelStructIndex].ui32Control = ui32Control;
}

void uDMAChannelScatterGatherSet(uint32_t ui32Base, uint32_t ui32ChannelNum, uint32_t ui32TaskCount, void *pvTaskList, uint32_t ui32IsPeriphSG) {
    tDMAControlTable *pControlTable;
    tDMAControlTable *pTaskTable;

    // Check the parameters.
    ASSERT(uDMABaseValid(ui32Base));
    ASSERT(ui32ChannelNum < UDMA_NUM_CHANNELS);
    ASSERT(HWREG(ui32Base + UDMA_O_CTRL) != 0);
    ASSERT(pvTaskList != 0);
    ASSERT(ui32TaskCount <= UDMA_XFER_SIZE_MAX);
    ASSERT(ui32TaskCount != 0);

    // Get the base address of the control table.
    pControlTable = (tDMAControlTable *)HWREG(ui32Base + UDMA_O_CTRL);

    // Get a handy pointer to the task list.
    pTaskTable = (tDMAControlTable *)pvTaskList;

    // Compute the ending address for the source pointer. This will be the
    // last element of the last task in the task table.
    pControlTable[ui32ChannelNum].pvSrcEndAddr =
        &pTaskTable[ui32TaskCount - 1].ui32Spare;

    // Compute the ending address for the destination pointer. This will be
    // the end of the alternate structure for this channel.
    pControlTable[ui32ChannelNum].pvDstEndAddr =
        &pControlTable[ui32ChannelNum | UDMA_ALT_SELECT].ui32Spare;

    // Compute the control word.  Most configurable items are fixed for
    // scatter-gather. Item and increment sizes are all 32-bit and arb
    // size must be 4. The count is the number of items in the task list
    // times 4 (4 words per task).
    pControlTable[ui32ChannelNum].ui32Control =
        (UDMA_DST_INC_32 | UDMA_SRC_INC_32 |
         UDMA_SIZE_32 | UDMA_ARB_4 |
         (((ui32TaskCount * 4) - 1) << UDMA_XFER_SIZE_S) |
         (ui32IsPeriphSG ? UDMA_MODE_PER_SCATTER_GATHER :
          UDMA_MODE_MEM_SCATTER_GATHER));

    // Scatter-gather operations can leave the alt bit set.  So if doing
    // back to back scatter-gather transfers, the second attempt may not
    // work correctly because the alt bit is set.  Therefore, clear the
    // alt bit here to ensure that it is always cleared before a new SG
    // transfer is started.
    HWREG(ui32Base + UDMA_O_CLEARCHNLPRIALT) = 1 << ui32ChannelNum;

}

uint32_t uDMAChannelSizeGet(uint32_t ui32Base, uint32_t ui32ChannelStructIndex) {
    tDMAControlTable *pControlTable;
    uint32_t ui32Control;

    // Check the arguments.
    ASSERT(uDMABaseValid(ui32Base));
    ASSERT(ui32ChannelStructIndex < (UDMA_NUM_CHANNELS * 2));
    ASSERT(HWREG(ui32Base + UDMA_O_CTRL) != 0);

    // Get the base address of the control table.
    pControlTable = (tDMAControlTable *)HWREG(ui32Base + UDMA_O_CTRL);

    // Get the current control word value and mask off all but the size field
    // and the mode field.
    ui32Control = (pControlTable[ui32ChannelStructIndex].ui32Control &
                   (UDMA_XFER_SIZE_M | UDMA_MODE_M));

    // If the size field and mode field are 0 then the transfer is finished
    // and there are no more items to transfer.
    if(ui32Control == 0)
    {
        return(0);
    }

    // Otherwise, if either the size field or more field is non-zero, then
    // not all the items have been transferred.
    else
    {
        // Shift the size field and add one, then return to user.
        return((ui32Control >> UDMA_XFER_SIZE_S) + 1);
    }
}

uint32_t uDMAChannelModeGet(uint32_t ui32Base, uint32_t ui32ChannelStructIndex) {
    tDMAControlTable *pControlTable;
    uint32_t ui32Control;

    // Check the arguments.
    ASSERT(uDMABaseValid(ui32Base));
    ASSERT(ui32ChannelStructIndex < (UDMA_NUM_CHANNELS * 2));
    ASSERT(HWREG(ui32Base + UDMA_O_CTRL) != 0);

    // Get the base address of the control table.
    pControlTable = (tDMAControlTable *)HWREG(ui32Base + UDMA_O_CTRL);

    // Get the current control word value and mask off all but the mode field.
    ui32Control = (pControlTable[ui32ChannelStructIndex].ui32Control &
                   UDMA_MODE_M);

    // Check if scatter/gather mode, and if so, mask off the alt bit.
    if(((ui32Control & ~UDMA_MODE_ALT_SELECT) == UDMA_MODE_MEM_SCATTER_GATHER) ||
       ((ui32Control & ~UDMA_MODE_ALT_SELECT) == UDMA_MODE_PER_SCATTER_GATHER))
    {
        ui32Control &= ~UDMA_MODE_ALT_SELECT;
    }

    // Return the mode to the caller.
    return(ui32Control);
}

void VIMSConfigure(uint32_t ui32Base, bool bRoundRobin, bool bPrefetch) {
    uint32_t ui32Reg;

    // Check the arguments.
    ASSERT(VIMSBaseValid(ui32Base));

    ui32Reg = HWREG(ui32Base + VIMS_O_CTL);
    ui32Reg &= ~(VIMS_CTL_PREF_EN | VIMS_CTL_ARB_CFG);
    if(bRoundRobin)
    {
        ui32Reg |= VIMS_CTL_ARB_CFG;
    }
    if(bPrefetch)
    {
        ui32Reg |= VIMS_CTL_PREF_EN;
    }

    // Set the Arbitration and prefetch mode.
    HWREG(ui32Base + VIMS_O_CTL) = ui32Reg;
}

void VIMSModeSet(uint32_t ui32Base, uint32_t ui32Mode) {
    uint32_t ui32Reg;

    // Check the arguments.
    ASSERT(VIMSBaseValid(ui32Base));

    ASSERT((ui32Mode == VIMS_MODE_DISABLED)   ||
           (ui32Mode == VIMS_MODE_ENABLED)    ||
           (ui32Mode == VIMS_MODE_OFF));

    // Set the mode.
    ui32Reg = HWREG(ui32Base + VIMS_O_CTL);
    ui32Reg &= ~VIMS_CTL_MODE_M;
    ui32Reg |= (ui32Mode & VIMS_CTL_MODE_M);

    HWREG(ui32Base + VIMS_O_CTL) = ui32Reg;
}

uint32_t VIMSModeGet(uint32_t ui32Base) {
    uint32_t ui32Reg;

    // Check the arguments.
    ASSERT(VIMSBaseValid(ui32Base));

    ui32Reg = HWREG(ui32Base + VIMS_O_STAT);
    if(ui32Reg & VIMS_STAT_MODE_CHANGING)
    {
        return (VIMS_MODE_CHANGING);
    }
    else
    {
        return (ui32Reg & VIMS_STAT_MODE_M);
    }
}

void VIMSModeSafeSet( uint32_t ui32Base, uint32_t ui32NewMode, bool blocking ) {
    uint32_t currentMode;

    // Check the arguments.
    ASSERT(VIMSBaseValid(ui32Base));
    ASSERT((ui32NewMode == VIMS_MODE_DISABLED)   ||
           (ui32NewMode == VIMS_MODE_ENABLED)    ||
           (ui32NewMode == VIMS_MODE_OFF));

    // Make sure that only the mode bits are set in the input parameter
    // (done just for security since it is critical to the code flow)
    ui32NewMode &= VIMS_CTL_MODE_M;

    // Wait for any pending change to complete and get current VIMS mode
    // (This is a blocking point but will typically only be a blocking point
    // only if mode is changed multiple times with blocking=0)
    do {
        currentMode = VIMSModeGet( ui32Base );
    } while ( currentMode == VIMS_MODE_CHANGING );

    // First check that it actually is a mode change request
    if ( ui32NewMode != currentMode ) {
        // Set new mode
        VIMSModeSet( ui32Base, ui32NewMode );

        // Wait for final mode change to complete - if blocking is requested
        if ( blocking ) {
            while ( HWREGBITW( VIMS_BASE + VIMS_O_STAT, VIMS_STAT_MODE_CHANGING_BITN )) {
                // Do nothing - wait for change to complete.
            }
        }
    }
}

void OSCClockSourceSet(uint32_t ui32SrcClk, uint32_t ui32Osc) {
    // Check the arguments.
    ASSERT((ui32SrcClk & OSC_SRC_CLK_LF) ||
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

uint32_t OSCClockSourceGet(uint32_t ui32SrcClk) {
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

int32_t OSC_HPOSCRelativeFrequencyOffsetGet( int32_t tempDegC ) {
   // Estimate HPOSC frequency, using temperature and curve fitting parameters
   uint32_t fitParams = HWREG(FCFG1_BASE + FCFG1_O_FREQ_OFFSET);
   // Extract the P0,P1,P2 params, and sign extend them via shifting up/down
   int32_t paramP0 = ((((int32_t) fitParams) << (32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P0_W - FCFG1_FREQ_OFFSET_HPOSC_COMP_P0_S))
                                             >> (32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P0_W));
   int32_t paramP1 = ((((int32_t) fitParams) << (32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P1_W - FCFG1_FREQ_OFFSET_HPOSC_COMP_P1_S))
                                             >> (32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P1_W));
   int32_t paramP2 = ((((int32_t) fitParams) << (32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P2_W - FCFG1_FREQ_OFFSET_HPOSC_COMP_P2_S))
                                             >> (32 - FCFG1_FREQ_OFFSET_HPOSC_COMP_P2_W));
   int32_t paramP3 = ((((int32_t) HWREG(FCFG1_BASE + FCFG1_O_MISC_CONF_2))
                                             << (32 - FCFG1_MISC_CONF_2_HPOSC_COMP_P3_W - FCFG1_MISC_CONF_2_HPOSC_COMP_P3_S))
                                             >> (32 - FCFG1_MISC_CONF_2_HPOSC_COMP_P3_W));

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

int16_t OSC_HPOSCRelativeFrequencyOffsetToRFCoreFormatConvert( int32_t HPOSC_RelFreqOffset ) {
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

void AUXADCDisable(void) {
    // Disable the ADC reference
    ADI8BitsClear(AUX_ADI4_BASE, ADI_4_AUX_O_ADCREF0, ADI_4_AUX_ADCREF0_EN_M | ADI_4_AUX_ADCREF0_REF_ON_IDLE_M | ADI_4_AUX_ADCREF0_SRC_M);

    // Assert reset and disable the ADC
    ADI8BitsClear(AUX_ADI4_BASE, ADI_4_AUX_O_ADC0, ADI_4_AUX_ADC0_EN_M | ADI_4_AUX_ADC0_RESET_N_M | ADI_4_AUX_ADC0_SMPL_MODE_M | ADI_4_AUX_ADC0_SMPL_CYCLE_EXP_M);

    // Ensure that scaling is enabled by default before next use of the ADC
    ADI8BitsClear(AUX_ADI4_BASE, ADI_4_AUX_O_ADC1, ADI_4_AUX_ADC1_SCALE_DIS_M);

    // Flush the FIFO before disabling the clocks
    HWREGBITW(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL, 1) = 1; // CMD: EN(1) -> FLUSH(3)
    HWREGBITW(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL, 1) = 0; // CMD: FLUSH(3) -> EN(1)

    // Disable the ADC clock (no need to wait since IOB_WUC_ADCCLKCTL_ACK goes low immediately)
    HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_ADCCLKCTL) = 0;

    // Disable the ADC data interface
    HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL) = 0;
}

void AUXADCEnableAsync(uint32_t refSource, uint32_t trigger) {
    // Enable the ADC reference, with the following options:
    // - SRC: Set when using relative reference
    // - REF_ON_IDLE: Always cleared since there is no idle state in asynchronous operation
    ADI8BitsSet(AUX_ADI4_BASE, ADI_4_AUX_O_ADCREF0, refSource | ADI_4_AUX_ADCREF0_EN_M);

    // Enable the ADC clock
    HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_ADCCLKCTL) = AUX_SYSIF_ADCCLKCTL_REQ_M;
    while (!(HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_ADCCLKCTL) & AUX_SYSIF_ADCCLKCTL_ACK_M));

    // Enable the ADC data interface
    if (trigger == AUXADC_TRIGGER_MANUAL) {
        // Manual trigger: No need to configure event routing from GPT
        HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL) = AUX_ANAIF_ADCCTL_START_SRC_NO_EVENT | AUX_ANAIF_ADCCTL_CMD_EN;
    } else {
        // GPT trigger: Configure event routing via MCU_EV to the AUX domain
        HWREG(EVENT_BASE + EVENT_O_AUXSEL0) = trigger;
        HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL) = AUX_ANAIF_ADCCTL_START_SRC_MCU_EV | AUX_ANAIF_ADCCTL_CMD_EN;
    }

    // Configure the ADC
    ADI8BitsSet(AUX_ADI4_BASE, ADI_4_AUX_O_ADC0, ADI_4_AUX_ADC0_SMPL_MODE_M);

    // Release reset and enable the ADC
    ADI8BitsSet(AUX_ADI4_BASE, ADI_4_AUX_O_ADC0, ADI_4_AUX_ADC0_EN_M | ADI_4_AUX_ADC0_RESET_N_M);
}

void AUXADCEnableSync(uint32_t refSource, uint32_t sampleTime, uint32_t trigger) {
    // Enable the ADC reference, with the following options:
    // - SRC: Set when using relative reference
    // - REF_ON_IDLE: Set when using fixed reference and sample time < 21.3 us
    uint8_t adcref0 = refSource | ADI_4_AUX_ADCREF0_EN_M;
    if (!refSource && (sampleTime < AUXADC_SAMPLE_TIME_21P3_US)) {
        adcref0 |= ADI_4_AUX_ADCREF0_REF_ON_IDLE_M;
    }
    ADI8BitsSet(AUX_ADI4_BASE, ADI_4_AUX_O_ADCREF0, adcref0);

    // Enable the ADC clock
    HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_ADCCLKCTL) = AUX_SYSIF_ADCCLKCTL_REQ_M;
    while (!(HWREG(AUX_SYSIF_BASE + AUX_SYSIF_O_ADCCLKCTL) & AUX_SYSIF_ADCCLKCTL_ACK_M));

    // Enable the ADC data interface
    if (trigger == AUXADC_TRIGGER_MANUAL) {
        // Manual trigger: No need to configure event routing from GPT
        HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL) = AUX_ANAIF_ADCCTL_START_SRC_NO_EVENT | AUX_ANAIF_ADCCTL_CMD_EN;
    } else {
        // GPT trigger: Configure event routing via MCU_EV to the AUX domain
        HWREG(EVENT_BASE + EVENT_O_AUXSEL0) = trigger;
        HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL) = AUX_ANAIF_ADCCTL_START_SRC_MCU_EV | AUX_ANAIF_ADCCTL_CMD_EN;
    }

    // Configure the ADC
    ADI8BitsSet(AUX_ADI4_BASE, ADI_4_AUX_O_ADC0, sampleTime << ADI_4_AUX_ADC0_SMPL_CYCLE_EXP_S);

    // Release reset and enable the ADC
    ADI8BitsSet(AUX_ADI4_BASE, ADI_4_AUX_O_ADC0, ADI_4_AUX_ADC0_EN_M | ADI_4_AUX_ADC0_RESET_N_M);
}

void AUXADCDisableInputScaling(void) {
    ADI8BitsSet(AUX_ADI4_BASE, ADI_4_AUX_O_ADC1, ADI_4_AUX_ADC1_SCALE_DIS_M);
}

void AUXADCFlushFifo(void) {
    HWREGBITW(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL, 1) = 1; // CMD: EN(1) -> FLUSH(3)
    HWREGBITW(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCCTL, 1) = 0; // CMD: FLUSH(3) -> EN(1)
}

uint32_t AUXADCReadFifo(void) {

    // Wait until there is at least one sample in the FIFO
    while (HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCFIFOSTAT) & AUX_ANAIF_ADCFIFOSTAT_EMPTY_M);

    // Return the first sample from the FIFO
    return HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCFIFO);
}

uint32_t AUXADCPopFifo(void) {

    // Return the first sample from the FIFO. If the FIFO is empty, this
    // generates ADC FIFO underflow
    return HWREG(AUX_ANAIF_BASE + AUX_ANAIF_O_ADCFIFO);
}

int32_t AUXADCGetAdjustmentGain(uint32_t refSource) {
    int32_t gain;
    if (refSource == AUXADC_REF_FIXED) {
        // AUXADC_REF_FIXED ==> ABS_GAIN
        gain = (HWREG(FCFG1_BASE + FCFG1_O_SOC_ADC_ABS_GAIN) & FCFG1_SOC_ADC_ABS_GAIN_SOC_ADC_ABS_GAIN_TEMP1_M) >> FCFG1_SOC_ADC_ABS_GAIN_SOC_ADC_ABS_GAIN_TEMP1_S;
    } else {
      // AUXADC_REF_VDDS_REL ==> REL_GAIN
        gain = (HWREG(FCFG1_BASE + FCFG1_O_SOC_ADC_REL_GAIN) & FCFG1_SOC_ADC_REL_GAIN_SOC_ADC_REL_GAIN_TEMP1_M) >> FCFG1_SOC_ADC_REL_GAIN_SOC_ADC_REL_GAIN_TEMP1_S;
    }
    return gain;
}

int32_t AUXADCGetAdjustmentOffset(uint32_t refSource) {
    int8_t offset;
    if ( refSource == AUXADC_REF_FIXED ) {
        // AUXADC_REF_FIXED ==> ABS_OFFSET
        offset = HWREG(FCFG1_BASE + FCFG1_O_SOC_ADC_OFFSET_INT) >> FCFG1_SOC_ADC_OFFSET_INT_SOC_ADC_ABS_OFFSET_TEMP1_S;
    } else {
        // AUXADC_REF_VDDS_REL ==> REL_OFFSET
        offset = HWREG(FCFG1_BASE + FCFG1_O_SOC_ADC_OFFSET_INT) >> FCFG1_SOC_ADC_OFFSET_INT_SOC_ADC_REL_OFFSET_TEMP1_S;
    }
    return offset;
}

int32_t AUXADCValueToMicrovolts(int32_t fixedRefVoltage, int32_t adcValue) {
    // Chop off 4 bits during calculations to avoid 32-bit overflow
    fixedRefVoltage >>= 4;
    return (((adcValue * fixedRefVoltage) + 2047) / 4095) << 4;
}

int32_t AUXADCMicrovoltsToValue(int32_t fixedRefVoltage, int32_t microvolts) {
    // Chop off 4 bits during calculations to avoid 32-bit overflow
    fixedRefVoltage >>= 4;
    microvolts >>= 4;
    return ((microvolts * 4095) + (fixedRefVoltage / 2)) / fixedRefVoltage;
}

int32_t AUXADCAdjustValueForGainAndOffset(int32_t adcValue, int32_t gain, int32_t offset) {
    // Apply gain and offset adjustment
    adcValue = (((adcValue + offset) * gain) + 16384) / 32768;

    // Saturate
    if (adcValue < 0) {
        return 0;
    } else if (adcValue > 4095) {
        return 4095;
    } else {
        return adcValue;
    }
}

int32_t AUXADCUnadjustValueForGainAndOffset(int32_t adcValue, int32_t gain, int32_t offset) {
    // Apply inverse gain and offset adjustment
    adcValue = (((adcValue * 32768) + (gain / 2)) / gain) - offset;

    // Saturate
    if (adcValue < 0) {
        return 0;
    } else if (adcValue > 4095) {
        return 4095;
    } else {
        return adcValue;
    }
}

void SysCtrl_DCDC_VoltageConditionalControl( void ) {
   uint32_t batThreshold     ;  // Fractional format with 8 fractional bits.
   uint32_t aonBatmonBat     ;  // Fractional format with 8 fractional bits.
   uint32_t ccfg_ModeConfReg ;  // Holds a copy of the CCFG_O_MODE_CONF register.
   uint32_t aonPmctlPwrctl   ;  // Reflect whats read/written to the AON_PMCTL_O_PWRCTL register.

   // We could potentially call this function before any battery voltage measurement
   // is made/available. In that case we must make sure that we do not turn off the DCDC.
   // This can be done by doing nothing as long as the battery voltage is 0 (Since the
   // reset value of the battery voltage register is 0).
   aonBatmonBat = HWREG( AON_BATMON_BASE + AON_BATMON_O_BAT );
   if ( aonBatmonBat != 0 ) {
      // Check if Voltage Conditional Control is enabled
      // It is enabled if all the following are true:
      // - DCDC in use (either in active or recharge mode), (in use if one of the corresponding CCFG bits are zero).
      // - Alternative DCDC settings are enabled ( DIS_ALT_DCDC_SETTING == 0 )
      // - Not in external regulator mode ( EXT_REG_MODE == 0 )
      ccfg_ModeConfReg = HWREG( CCFG_BASE + CCFG_O_MODE_CONF );

      if (((( ccfg_ModeConfReg & CCFG_MODE_CONF_DCDC_RECHARGE_M ) == 0                                            ) ||
           (( ccfg_ModeConfReg & CCFG_MODE_CONF_DCDC_ACTIVE_M   ) == 0                                            )    ) &&
          (( HWREG( AON_PMCTL_BASE  + AON_PMCTL_O_PWRCTL  ) & AON_PMCTL_PWRCTL_EXT_REG_MODE  )               == 0      ) &&
          (( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING ) == 0      )    )
      {
         aonPmctlPwrctl = HWREG( AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL );
         batThreshold   = (((( HWREG( CCFG_BASE + CCFG_O_MODE_CONF_1 ) &
            CCFG_MODE_CONF_1_ALT_DCDC_VMIN_M ) >>
            CCFG_MODE_CONF_1_ALT_DCDC_VMIN_S ) + 28 ) << 4 );

         if ( aonPmctlPwrctl & ( AON_PMCTL_PWRCTL_DCDC_EN_M | AON_PMCTL_PWRCTL_DCDC_ACTIVE_M )) {
            // DCDC is ON, check if it should be switched off
            if ( aonBatmonBat < batThreshold ) {
               aonPmctlPwrctl &= ~( AON_PMCTL_PWRCTL_DCDC_EN_M | AON_PMCTL_PWRCTL_DCDC_ACTIVE_M );

               HWREG( AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL ) = aonPmctlPwrctl;
            }
         } else {
            // DCDC is OFF, check if it should be switched on
            if ( aonBatmonBat > batThreshold ) {
               if (( ccfg_ModeConfReg & CCFG_MODE_CONF_DCDC_RECHARGE_M ) == 0 ) aonPmctlPwrctl |= AON_PMCTL_PWRCTL_DCDC_EN_M     ;
               if (( ccfg_ModeConfReg & CCFG_MODE_CONF_DCDC_ACTIVE_M   ) == 0 ) aonPmctlPwrctl |= AON_PMCTL_PWRCTL_DCDC_ACTIVE_M ;

               HWREG( AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL ) = aonPmctlPwrctl;
            }
         }
      }
   }
}

uint32_t SysCtrlResetSourceGet( void ) {
   uint32_t aonPmctlResetCtl = HWREG( AON_PMCTL_BASE + AON_PMCTL_O_RESETCTL );

   if ( aonPmctlResetCtl & AON_PMCTL_RESETCTL_WU_FROM_SD_M ) {
      if ( aonPmctlResetCtl & AON_PMCTL_RESETCTL_GPIO_WU_FROM_SD_M ) {
         return ( RSTSRC_WAKEUP_FROM_SHUTDOWN );
      } else {
         return ( RSTSRC_WAKEUP_FROM_TCK_NOISE );
      }
   } else {
      return (( aonPmctlResetCtl & AON_PMCTL_RESETCTL_RESET_SRC_M ) >> AON_PMCTL_RESETCTL_RESET_SRC_S );
   }
}

int32_t AONBatMonTemperatureGetDegC( void ) {
   int32_t  signedTemp        ; // Signed extended temperature with 8 fractional bits
   int32_t  tempCorrection    ; // Voltage dependent temp correction with 8 fractional bits
   int8_t   voltageSlope      ; // Signed byte value representing the TEMP slope with battery voltage, in degrees C/V, with 4 fractional bits.

   // Shift left then right to sign extend the BATMON_TEMP field
   signedTemp = ((((int32_t)HWREG( AON_BATMON_BASE + AON_BATMON_O_TEMP ))
     << ( 32 - AON_BATMON_TEMP_INT_W - AON_BATMON_TEMP_INT_S ))
     >> ( 32 - AON_BATMON_TEMP_INT_W - AON_BATMON_TEMP_INT_S ));

   // Typecasting voltageSlope to int8_t prior to assignment in order to make sure sign extension works properly
   // Using byte read (HWREGB) in order to make more efficient code since voltageSlope is assigned to bits[7:0] of FCFG1_O_MISC_TRIM
   voltageSlope      = ((int8_t)HWREGB( FCFG1_BASE + FCFG1_O_MISC_TRIM ));
   tempCorrection    = (( voltageSlope * (((int32_t)HWREG( AON_BATMON_BASE + AON_BATMON_O_BAT )) - 0x300 )) >> 4 );

   return ((( signedTemp - tempCorrection ) + 0x80 ) >> 8 );
}

void SetupStepVddrTrimTo( uint32_t toCode ) {
    uint32_t    pmctlResetctl_reg   ;
    int32_t     targetTrim          ;
    int32_t     currentTrim         ;

    targetTrim  = SetupSignExtendVddrTrimValue( toCode & ( ADI_3_REFSYS_DCDCCTL0_VDDR_TRIM_M >> ADI_3_REFSYS_DCDCCTL0_VDDR_TRIM_S ));
    currentTrim = SetupSignExtendVddrTrimValue((
        HWREGB( ADI3_BASE + ADI_3_REFSYS_O_DCDCCTL0 ) &
        ADI_3_REFSYS_DCDCCTL0_VDDR_TRIM_M ) >>
        ADI_3_REFSYS_DCDCCTL0_VDDR_TRIM_S ) ;

    if ( targetTrim != currentTrim ) {
        pmctlResetctl_reg = ( HWREG( AON_PMCTL_BASE + AON_PMCTL_O_RESETCTL ) & ~AON_PMCTL_RESETCTL_MCU_WARM_RESET_M );
        if ( pmctlResetctl_reg & AON_PMCTL_RESETCTL_VDDR_LOSS_EN_M ) {
            HWREG( AON_PMCTL_BASE + AON_PMCTL_O_RESETCTL ) = ( pmctlResetctl_reg & ~AON_PMCTL_RESETCTL_VDDR_LOSS_EN_M );
            HWREG( AON_RTC_BASE + AON_RTC_O_SYNC );      // Wait for VDDR_LOSS_EN setting to propagate
        }

        while ( targetTrim != currentTrim ) {
            HWREG( AON_RTC_BASE + AON_RTC_O_SYNCLF );    // Wait for next edge on SCLK_LF (positive or negative)

            if ( targetTrim > currentTrim )  currentTrim++;
            else                             currentTrim--;

            HWREGB( ADI3_BASE + ADI_3_REFSYS_O_DCDCCTL0 ) = (
                ( HWREGB( ADI3_BASE + ADI_3_REFSYS_O_DCDCCTL0 ) & ~ADI_3_REFSYS_DCDCCTL0_VDDR_TRIM_M ) |
                ((((uint32_t)currentTrim) << ADI_3_REFSYS_DCDCCTL0_VDDR_TRIM_S ) &
                                             ADI_3_REFSYS_DCDCCTL0_VDDR_TRIM_M ) );
        }

        HWREG( AON_RTC_BASE + AON_RTC_O_SYNCLF );        // Wait for next edge on SCLK_LF (positive or negative)

        if ( pmctlResetctl_reg & AON_PMCTL_RESETCTL_VDDR_LOSS_EN_M ) {
            HWREG( AON_RTC_BASE + AON_RTC_O_SYNCLF );    // Wait for next edge on SCLK_LF (positive or negative)
            HWREG( AON_RTC_BASE + AON_RTC_O_SYNCLF );    // Wait for next edge on SCLK_LF (positive or negative)
            HWREG( AON_PMCTL_BASE + AON_PMCTL_O_RESETCTL ) = pmctlResetctl_reg;
            HWREG( AON_RTC_BASE + AON_RTC_O_SYNC );      // And finally wait for VDDR_LOSS_EN setting to propagate
        }
    }
}

void SetupAfterColdResetWakeupFromShutDownCfg1( uint32_t ccfg_ModeConfReg ) {
    // Check for CC1352 boost mode
    // The combination VDDR_EXT_LOAD=0 and VDDS_BOD_LEVEL=1 is defined to select boost mode
    if ((( ccfg_ModeConfReg & CCFG_MODE_CONF_VDDR_EXT_LOAD  ) == 0 ) &&
        (( ccfg_ModeConfReg & CCFG_MODE_CONF_VDDS_BOD_LEVEL ) != 0 )    )
    {
        // Set VDDS_BOD trim - using masked write {MASK8:DATA8}
        // - TRIM_VDDS_BOD is bits[7:3] of ADI3..REFSYSCTL1
        // - Needs a positive transition on BOD_BG_TRIM_EN (bit[7] of REFSYSCTL3) to
        //   latch new VDDS BOD. Set to 0 first to guarantee a positive transition.
        HWREGB( ADI3_BASE + ADI_O_CLR + ADI_3_REFSYS_O_REFSYSCTL3 ) = ADI_3_REFSYS_REFSYSCTL3_BOD_BG_TRIM_EN;
        //
        // VDDS_BOD_LEVEL = 1 means that boost mode is selected
        // - Max out the VDDS_BOD trim (=VDDS_BOD_POS_31)
        HWREGH( ADI3_BASE + ADI_O_MASK8B + ( ADI_3_REFSYS_O_REFSYSCTL1 * 2 )) =
            ( ADI_3_REFSYS_REFSYSCTL1_TRIM_VDDS_BOD_M << 8 ) |
            ( ADI_3_REFSYS_REFSYSCTL1_TRIM_VDDS_BOD_POS_31 ) ;
        HWREGB( ADI3_BASE + ADI_O_SET + ADI_3_REFSYS_O_REFSYSCTL3 ) = ADI_3_REFSYS_REFSYSCTL3_BOD_BG_TRIM_EN;

        SetupStepVddrTrimTo(( HWREG( FCFG1_BASE + FCFG1_O_VOLT_TRIM ) &
            FCFG1_VOLT_TRIM_VDDR_TRIM_HH_M ) >>
            FCFG1_VOLT_TRIM_VDDR_TRIM_HH_S ) ;
    }

    // 1.
    // Do not allow DCDC to be enabled if in external regulator mode.
    // Preventing this by setting both the RECHARGE and the ACTIVE bits bit in the CCFG_MODE_CONF copy register (ccfg_ModeConfReg).
    //
    // 2.
    // Adjusted battery monitor low limit in internal regulator mode.
    // This is done by setting AON_BATMON_FLASHPUMPP0_LOWLIM=0 in internal regulator mode.
    if ( HWREG( AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL ) & AON_PMCTL_PWRCTL_EXT_REG_MODE ) {
        ccfg_ModeConfReg |= ( CCFG_MODE_CONF_DCDC_RECHARGE_M | CCFG_MODE_CONF_DCDC_ACTIVE_M );
    } else {
        HWREGBITW( AON_BATMON_BASE + AON_BATMON_O_FLASHPUMPP0, AON_BATMON_FLASHPUMPP0_LOWLIM_BITN ) = 0;
    }

    // set the RECHARGE source based upon CCFG:MODE_CONF:DCDC_RECHARGE
    // Note: Inverse polarity
    HWREGBITW( AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL, AON_PMCTL_PWRCTL_DCDC_EN_BITN ) =
        ((( ccfg_ModeConfReg >> CCFG_MODE_CONF_DCDC_RECHARGE_S ) & 1 ) ^ 1 );

    // set the ACTIVE source based upon CCFG:MODE_CONF:DCDC_ACTIVE
    // Note: Inverse polarity
    HWREGBITW( AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL, AON_PMCTL_PWRCTL_DCDC_ACTIVE_BITN ) =
        ((( ccfg_ModeConfReg >> CCFG_MODE_CONF_DCDC_ACTIVE_S ) & 1 ) ^ 1 );
}

void SetupAfterColdResetWakeupFromShutDownCfg2( uint32_t ui32Fcfg1Revision, uint32_t ccfg_ModeConfReg ) {
    uint32_t   ui32Trim;

    // Following sequence is required for using XOSCHF, if not included
    // devices crashes when trying to switch to XOSCHF.
    //
    // Trim CAP settings. Get and set trim value for the ANABYPASS_VALUE1
    // register
    ui32Trim = SetupGetTrimForAnabypassValue1( ccfg_ModeConfReg );
    DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_ANABYPASSVAL1, ui32Trim);

    // Trim RCOSC_LF. Get and set trim values for the RCOSCLF_RTUNE_TRIM and
    // RCOSCLF_CTUNE_TRIM fields in the XOSCLF_RCOSCLF_CTRL register.
    ui32Trim = SetupGetTrimForRcOscLfRtuneCtuneTrim();
    DDI16BitfieldWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_LFOSCCTL,
                       (DDI_0_OSC_LFOSCCTL_RCOSCLF_CTUNE_TRIM_M |
                        DDI_0_OSC_LFOSCCTL_RCOSCLF_RTUNE_TRIM_M),
                       DDI_0_OSC_LFOSCCTL_RCOSCLF_CTUNE_TRIM_S,
                       ui32Trim);

    // Trim XOSCHF IBIAS THERM. Get and set trim value for the
    // XOSCHF IBIAS THERM bit field in the ANABYPASS_VALUE2 register. Other
    // register bit fields are set to 0.
    ui32Trim = SetupGetTrimForXoscHfIbiastherm();
    DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_ANABYPASSVAL2,
                  ui32Trim<<DDI_0_OSC_ANABYPASSVAL2_XOSC_HF_IBIASTHERM_S);

    // Trim AMPCOMP settings required before switch to XOSCHF
    ui32Trim = SetupGetTrimForAmpcompTh2();
    DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_AMPCOMPTH2, ui32Trim);
    ui32Trim = SetupGetTrimForAmpcompTh1();
    DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_AMPCOMPTH1, ui32Trim);
#if ( CCFG_BASE == CCFG_BASE_DEFAULT )
    ui32Trim = SetupGetTrimForAmpcompCtrl( ui32Fcfg1Revision );
#else
    ui32Trim = NOROM_SetupGetTrimForAmpcompCtrl( ui32Fcfg1Revision );
#endif
    DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_AMPCOMPCTL, ui32Trim);

    // Set trim for DDI_0_OSC_ADCDOUBLERNANOAMPCTL_ADC_SH_MODE_EN in accordance to FCFG1 setting
    // This is bit[5] in the DDI_0_OSC_O_ADCDOUBLERNANOAMPCTL register
    // Using MASK4 write + 1 => writing to bits[7:4]
    ui32Trim = SetupGetTrimForAdcShModeEn( ui32Fcfg1Revision );
    HWREGB( AUX_DDI0_OSC_BASE + DDI_O_MASK4B + ( DDI_0_OSC_O_ADCDOUBLERNANOAMPCTL * 2 ) + 1 ) =
      ( 0x20 | ( ui32Trim << 1 ));

    // Set trim for DDI_0_OSC_ADCDOUBLERNANOAMPCTL_ADC_SH_VBUF_EN in accordance to FCFG1 setting
    // This is bit[4] in the DDI_0_OSC_O_ADCDOUBLERNANOAMPCTL register
    // Using MASK4 write + 1 => writing to bits[7:4]
    ui32Trim = SetupGetTrimForAdcShVbufEn( ui32Fcfg1Revision );
    HWREGB( AUX_DDI0_OSC_BASE + DDI_O_MASK4B + ( DDI_0_OSC_O_ADCDOUBLERNANOAMPCTL * 2 ) + 1 ) =
      ( 0x10 | ( ui32Trim ));

    // Set trim for the PEAK_DET_ITRIM, HP_BUF_ITRIM and LP_BUF_ITRIM bit fields
    // in the DDI0_OSC_O_XOSCHFCTL register in accordance to FCFG1 setting.
    // Remaining register bit fields are set to their reset values of 0.
    ui32Trim = SetupGetTrimForXoscHfCtl(ui32Fcfg1Revision);
    DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_XOSCHFCTL, ui32Trim);

    // Set trim for DBLR_LOOP_FILTER_RESET_VOLTAGE in accordance to FCFG1 setting
    // (This is bits [18:17] in DDI_0_OSC_O_ADCDOUBLERNANOAMPCTL)
    // (Using MASK4 write + 4 => writing to bits[19:16] => (4*4))
    // (Assuming: DDI_0_OSC_ADCDOUBLERNANOAMPCTL_DBLR_LOOP_FILTER_RESET_VOLTAGE_S = 17 and
    //  that DDI_0_OSC_ADCDOUBLERNANOAMPCTL_DBLR_LOOP_FILTER_RESET_VOLTAGE_M = 0x00060000)
    ui32Trim = SetupGetTrimForDblrLoopFilterResetVoltage( ui32Fcfg1Revision );
    HWREGB( AUX_DDI0_OSC_BASE + DDI_O_MASK4B + ( DDI_0_OSC_O_ADCDOUBLERNANOAMPCTL * 2 ) + 4 ) =
      ( 0x60 | ( ui32Trim << 1 ));

    // Update DDI_0_OSC_ATESTCTL_ATESTLF_RCOSCLF_IBIAS_TRIM with data from
    // FCFG1_OSC_CONF_ATESTLF_RCOSCLF_IBIAS_TRIM
    // This is DDI_0_OSC_O_ATESTCTL bit[7]
    // ( DDI_0_OSC_O_ATESTCTL is currently hidden (but=0x00000020))
    // Using MASK4 write + 1 => writing to bits[7:4]
    ui32Trim = SetupGetTrimForRcOscLfIBiasTrim( ui32Fcfg1Revision );
    HWREGB( AUX_DDI0_OSC_BASE + DDI_O_MASK4B + ( 0x00000020 * 2 ) + 1 ) =
      ( 0x80 | ( ui32Trim << 3 ));

    // Update DDI_0_OSC_LFOSCCTL_XOSCLF_REGULATOR_TRIM and
    //        DDI_0_OSC_LFOSCCTL_XOSCLF_CMIRRWR_RATIO in one write
    // This can be simplified since the registers are packed together in the same
    // order both in FCFG1 and in the HW register.
    // This spans DDI_0_OSC_O_LFOSCCTL bits[23:18]
    // Using MASK8 write + 4 => writing to bits[23:16]
    ui32Trim = SetupGetTrimForXoscLfRegulatorAndCmirrwrRatio( ui32Fcfg1Revision );
    HWREGH( AUX_DDI0_OSC_BASE + DDI_O_MASK8B + ( DDI_0_OSC_O_LFOSCCTL * 2 ) + 4 ) =
      ( 0xFC00 | ( ui32Trim << 2 ));

    // Set trim the HPM_IBIAS_WAIT_CNT, LPM_IBIAS_WAIT_CNT and IDAC_STEP bit
    // fields in the DDI0_OSC_O_RADCEXTCFG register in accordance to FCFG1 setting.
    // Remaining register bit fields are set to their reset values of 0.
    ui32Trim = SetupGetTrimForRadcExtCfg(ui32Fcfg1Revision);
    DDI32RegWrite(AUX_DDI0_OSC_BASE, DDI_0_OSC_O_RADCEXTCFG, ui32Trim);

}

void SetupAfterColdResetWakeupFromShutDownCfg3( uint32_t ccfg_ModeConfReg ) {
    uint32_t   fcfg1OscConf;
    uint32_t   ui32Trim;
#if ( ! defined( SKIP_SCLK_LF_SRC_CHANGE ))
    uint32_t   currentHfClock;
    uint32_t   ccfgExtLfClk;
#endif

    // Examine the XOSC_FREQ field to select 0x1=HPOSC, 0x2=48MHz XOSC, 0x3=24MHz XOSC
    switch (( ccfg_ModeConfReg & CCFG_MODE_CONF_XOSC_FREQ_M ) >> CCFG_MODE_CONF_XOSC_FREQ_S ) {
    case 2 :
        // XOSC source is a 48 MHz crystal
        // Do nothing (since this is the reset setting)
        break;
    case 1 :
        // XOSC source is HPOSC (trim the HPOSC if this is a chip with HPOSC, otherwise skip trimming and default to 24 MHz XOSC)

        fcfg1OscConf = HWREG( FCFG1_BASE + FCFG1_O_OSC_CONF );

        if (( fcfg1OscConf & FCFG1_OSC_CONF_HPOSC_OPTION ) == 0 ) {
            // This is a HPOSC chip, apply HPOSC settings
            // Set bit DDI_0_OSC_CTL0_HPOSC_MODE_EN (this is bit 14 in DDI_0_OSC_O_CTL0)
            HWREG( AUX_DDI0_OSC_BASE + DDI_O_SET + DDI_0_OSC_O_CTL0 ) = DDI_0_OSC_CTL0_HPOSC_MODE_EN;

            // ADI_2_REFSYS_HPOSCCTL2_BIAS_HOLD_MODE_EN = FCFG1_OSC_CONF_HPOSC_BIAS_HOLD_MODE_EN   (1 bit)
            // ADI_2_REFSYS_HPOSCCTL2_CURRMIRR_RATIO    = FCFG1_OSC_CONF_HPOSC_CURRMIRR_RATIO      (4 bits)
            // ADI_2_REFSYS_HPOSCCTL1_BIAS_RES_SET      = FCFG1_OSC_CONF_HPOSC_BIAS_RES_SET        (4 bits)
            // ADI_2_REFSYS_HPOSCCTL0_FILTER_EN         = FCFG1_OSC_CONF_HPOSC_FILTER_EN           (1 bit)
            // ADI_2_REFSYS_HPOSCCTL0_BIAS_RECHARGE_DLY = FCFG1_OSC_CONF_HPOSC_BIAS_RECHARGE_DELAY (2 bits)
            // ADI_2_REFSYS_HPOSCCTL0_SERIES_CAP        = FCFG1_OSC_CONF_HPOSC_SERIES_CAP          (2 bits)
            // ADI_2_REFSYS_HPOSCCTL0_DIV3_BYPASS       = FCFG1_OSC_CONF_HPOSC_DIV3_BYPASS         (1 bit)

            HWREG( ADI2_BASE + ADI_2_REFSYS_O_HPOSCCTL2 ) = (( HWREG( ADI2_BASE + ADI_2_REFSYS_O_HPOSCCTL2 ) &
                  ~( ADI_2_REFSYS_HPOSCCTL2_BIAS_HOLD_MODE_EN_M | ADI_2_REFSYS_HPOSCCTL2_CURRMIRR_RATIO_M  )                                                                       ) |
                   ((( fcfg1OscConf & FCFG1_OSC_CONF_HPOSC_BIAS_HOLD_MODE_EN_M   ) >> FCFG1_OSC_CONF_HPOSC_BIAS_HOLD_MODE_EN_S   ) << ADI_2_REFSYS_HPOSCCTL2_BIAS_HOLD_MODE_EN_S   ) |
                   ((( fcfg1OscConf & FCFG1_OSC_CONF_HPOSC_CURRMIRR_RATIO_M      ) >> FCFG1_OSC_CONF_HPOSC_CURRMIRR_RATIO_S      ) << ADI_2_REFSYS_HPOSCCTL2_CURRMIRR_RATIO_S      )   );
            HWREG( ADI2_BASE + ADI_2_REFSYS_O_HPOSCCTL1 ) = (( HWREG( ADI2_BASE + ADI_2_REFSYS_O_HPOSCCTL1 ) & ~( ADI_2_REFSYS_HPOSCCTL1_BIAS_RES_SET_M )                          ) |
                   ((( fcfg1OscConf & FCFG1_OSC_CONF_HPOSC_BIAS_RES_SET_M        ) >> FCFG1_OSC_CONF_HPOSC_BIAS_RES_SET_S        ) << ADI_2_REFSYS_HPOSCCTL1_BIAS_RES_SET_S        )   );
            HWREG( ADI2_BASE + ADI_2_REFSYS_O_HPOSCCTL0 ) = (( HWREG( ADI2_BASE + ADI_2_REFSYS_O_HPOSCCTL0 ) &
                  ~( ADI_2_REFSYS_HPOSCCTL0_FILTER_EN_M | ADI_2_REFSYS_HPOSCCTL0_BIAS_RECHARGE_DLY_M | ADI_2_REFSYS_HPOSCCTL0_SERIES_CAP_M | ADI_2_REFSYS_HPOSCCTL0_DIV3_BYPASS_M )) |
                   ((( fcfg1OscConf & FCFG1_OSC_CONF_HPOSC_FILTER_EN_M           ) >> FCFG1_OSC_CONF_HPOSC_FILTER_EN_S           ) << ADI_2_REFSYS_HPOSCCTL0_FILTER_EN_S           ) |
                   ((( fcfg1OscConf & FCFG1_OSC_CONF_HPOSC_BIAS_RECHARGE_DELAY_M ) >> FCFG1_OSC_CONF_HPOSC_BIAS_RECHARGE_DELAY_S ) << ADI_2_REFSYS_HPOSCCTL0_BIAS_RECHARGE_DLY_S   ) |
                   ((( fcfg1OscConf & FCFG1_OSC_CONF_HPOSC_SERIES_CAP_M          ) >> FCFG1_OSC_CONF_HPOSC_SERIES_CAP_S          ) << ADI_2_REFSYS_HPOSCCTL0_SERIES_CAP_S          ) |
                   ((( fcfg1OscConf & FCFG1_OSC_CONF_HPOSC_DIV3_BYPASS_M         ) >> FCFG1_OSC_CONF_HPOSC_DIV3_BYPASS_S         ) << ADI_2_REFSYS_HPOSCCTL0_DIV3_BYPASS_S         )   );
            break;
        }
        // Not a HPOSC chip - fall through to default
    default :
        // XOSC source is a 24 MHz crystal (default)
        // Set bit DDI_0_OSC_CTL0_XTAL_IS_24M (this is bit 31 in DDI_0_OSC_O_CTL0)
        HWREG( AUX_DDI0_OSC_BASE + DDI_O_SET + DDI_0_OSC_O_CTL0 ) = DDI_0_OSC_CTL0_XTAL_IS_24M;
        break;
    }

    // Set XOSC_HF in bypass mode if CCFG is configured for external TCXO
    // Please note that it is up to the customer to make sure that the external clock source is up and running before XOSC_HF can be used.
    if (( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_TCXO ) == 0 ) {
        HWREG( AUX_DDI0_OSC_BASE + DDI_O_SET + DDI_0_OSC_O_XOSCHFCTL ) = DDI_0_OSC_XOSCHFCTL_BYPASS;
    }

    // Clear DDI_0_OSC_CTL0_CLK_LOSS_EN (ClockLossEventEnable()). This is bit 9 in DDI_0_OSC_O_CTL0.
    // This is typically already 0 except on Lizard where it is set in ROM-boot
    HWREG( AUX_DDI0_OSC_BASE + DDI_O_CLR + DDI_0_OSC_O_CTL0 ) = DDI_0_OSC_CTL0_CLK_LOSS_EN;

    // Setting DDI_0_OSC_CTL1_XOSC_HF_FAST_START according to value found in FCFG1
    ui32Trim = SetupGetTrimForXoscHfFastStart();
    HWREGB( AUX_DDI0_OSC_BASE + DDI_O_MASK4B + ( DDI_0_OSC_O_CTL1 * 2 )) = ( 0x30 | ui32Trim );

#if ( defined( SKIP_SCLK_LF_SRC_CHANGE ))
    // If SKIP_SCLK_LF_SRC_CHANGE is defined we will skip the switch-statement below, and
    // remain on RCOSCHFDLF as LF source (since RCOSCHFDLF is the reset setting).
    // (RCOSCHFDLF = Low frequency clock derived from RCOSC_HF).
    // Note1: Function SetupAfterColdResetWakeupFromShutDownCfg3() is a ROM function in some devices (R2 and Agama)
    //        It must be removed from rom.h on these devices in order to avoid that the ROM version is called.
    // Note2: It's not possible to enter STANDBY when running the LF clock from RCOSCHFDLF, however IDLE will work just fine.
    // RCOSCHFDLF (RCOSC_HF/1536) -> SCLK_LF (=31250 Hz)
    SetupSetAonRtcSubSecInc( 0x8637BD ); // RTC_INCREMENT = 2^38 / frequency
#else
    // setup the LF clock based upon CCFG:MODE_CONF:SCLK_LF_OPTION
    switch (( ccfg_ModeConfReg & CCFG_MODE_CONF_SCLK_LF_OPTION_M ) >> CCFG_MODE_CONF_SCLK_LF_OPTION_S ) {
    case 0 : // XOSC_HF_DLF (XOSCHF/1536) -> SCLK_LF (=31250 Hz)
        OSCClockSourceSet( OSC_SRC_CLK_LF, OSC_XOSC_HF );
        SetupSetAonRtcSubSecInc( 0x8637BD ); // RTC_INCREMENT = 2^38 / frequency
        break;
    case 1 : // EXTERNAL signal -> SCLK_LF (frequency=2^38/CCFG_EXT_LF_CLK_RTC_INCREMENT)
        // Set SCLK_LF to use the same source as SCLK_HF
        // Can be simplified a bit since possible return values for HF matches LF settings
        currentHfClock = OSCClockSourceGet( OSC_SRC_CLK_HF );
        OSCClockSourceSet( OSC_SRC_CLK_LF, currentHfClock );
        while( OSCClockSourceGet( OSC_SRC_CLK_LF ) != currentHfClock ) {
            // Wait until switched
        }
        ccfgExtLfClk = HWREG( CCFG_BASE + CCFG_O_EXT_LF_CLK );
        SetupSetAonRtcSubSecInc(( ccfgExtLfClk & CCFG_EXT_LF_CLK_RTC_INCREMENT_M ) >> CCFG_EXT_LF_CLK_RTC_INCREMENT_S );
        IOCPortConfigureSet(( ccfgExtLfClk & CCFG_EXT_LF_CLK_DIO_M ) >> CCFG_EXT_LF_CLK_DIO_S,
                              IOC_PORT_AON_CLK32K,
                              IOC_STD_INPUT | IOC_HYST_ENABLE );   // Route external clock to AON IOC w/hysteresis
                                                                   // Set XOSC_LF in bypass mode to allow external 32 kHz clock
        HWREG( AUX_DDI0_OSC_BASE + DDI_O_SET + DDI_0_OSC_O_CTL0 ) = DDI_0_OSC_CTL0_XOSC_LF_DIG_BYPASS;
        // Fall through to set XOSC_LF as SCLK_LF source
    case 2 : // XOSC_LF -> SLCK_LF (32768 Hz)
        OSCClockSourceSet( OSC_SRC_CLK_LF, OSC_XOSC_LF );
        break;
    default : // (=3) RCOSC_LF
        OSCClockSourceSet( OSC_SRC_CLK_LF, OSC_RCOSC_LF );
        break;
    }
#endif // ( ! defined( SKIP_SCLK_LF_SRC_CHANGE ))

    // Update ADI_4_AUX_ADCREF1_VTRIM with value from FCFG1
    HWREGB( AUX_ADI4_BASE + ADI_4_AUX_O_ADCREF1 ) =
      ((( HWREG( FCFG1_BASE + FCFG1_O_SOC_ADC_REF_TRIM_AND_OFFSET_EXT ) >>
      FCFG1_SOC_ADC_REF_TRIM_AND_OFFSET_EXT_SOC_ADC_REF_VOLTAGE_TRIM_TEMP1_S ) <<
      ADI_4_AUX_ADCREF1_VTRIM_S ) &
      ADI_4_AUX_ADCREF1_VTRIM_M );

    // Sync with AON
    SysCtrlAonSync();
}

uint32_t SetupGetTrimForAnabypassValue1( uint32_t ccfg_ModeConfReg ) {
    uint32_t ui32Fcfg1Value            ;
    uint32_t ui32XoscHfRow             ;
    uint32_t ui32XoscHfCol             ;
    uint32_t ui32TrimValue             ;

    // Use device specific trim values located in factory configuration
    // area for the XOSC_HF_COLUMN_Q12 and XOSC_HF_ROW_Q12 bit fields in
    // the ANABYPASS_VALUE1 register. Value for the other bit fields
    // are set to 0.

    ui32Fcfg1Value = HWREG(FCFG1_BASE + FCFG1_O_CONFIG_OSC_TOP);
    ui32XoscHfRow = (( ui32Fcfg1Value &
        FCFG1_CONFIG_OSC_TOP_XOSC_HF_ROW_Q12_M ) >>
        FCFG1_CONFIG_OSC_TOP_XOSC_HF_ROW_Q12_S );
    ui32XoscHfCol = (( ui32Fcfg1Value &
        FCFG1_CONFIG_OSC_TOP_XOSC_HF_COLUMN_Q12_M ) >>
        FCFG1_CONFIG_OSC_TOP_XOSC_HF_COLUMN_Q12_S );

    if (( ccfg_ModeConfReg & CCFG_MODE_CONF_XOSC_CAP_MOD ) == 0 ) {
        // XOSC_CAP_MOD = 0 means: CAP_ARRAY_DELTA is in use -> Apply compensation
        // XOSC_CAPARRAY_DELTA is located in bit[15:8] of ccfg_ModeConfReg
        // Note: HW_REV_DEPENDENT_IMPLEMENTATION. Field width is not given by
        // a define and sign extension must therefore be hard coded.
        // ( A small test program is created verifying the code lines below:
        //   Ref.: ..\test\small_standalone_test_programs\CapArrayDeltaAdjust_test.c)
        int32_t i32CustomerDeltaAdjust =
            (((int32_t)( ccfg_ModeConfReg << ( 32 - CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_W - CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_S )))
                                          >> ( 32 - CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_W ));

        while ( i32CustomerDeltaAdjust < 0 ) {
            ui32XoscHfCol >>= 1;                              // COL 1 step down
            if ( ui32XoscHfCol == 0 ) {                       // if COL below minimum
                ui32XoscHfCol = 0xFFFF;                       //   Set COL to maximum
                ui32XoscHfRow >>= 1;                          //   ROW 1 step down
                if ( ui32XoscHfRow == 0 ) {                   // if ROW below minimum
                   ui32XoscHfRow = 1;                         //   Set both ROW and COL
                   ui32XoscHfCol = 1;                         //   to minimum
                }
            }
            i32CustomerDeltaAdjust++;
        }
        while ( i32CustomerDeltaAdjust > 0 ) {
            ui32XoscHfCol = ( ui32XoscHfCol << 1 ) | 1;       // COL 1 step up
            if ( ui32XoscHfCol > 0xFFFF ) {                   // if COL above maximum
                ui32XoscHfCol = 1;                            //   Set COL to minimum
                ui32XoscHfRow = ( ui32XoscHfRow << 1 ) | 1;   //   ROW 1 step up
                if ( ui32XoscHfRow > 0xF ) {                  // if ROW above maximum
                   ui32XoscHfRow = 0xF;                       //   Set both ROW and COL
                   ui32XoscHfCol = 0xFFFF;                    //   to maximum
                }
            }
            i32CustomerDeltaAdjust--;
        }
    }

    ui32TrimValue = (( ui32XoscHfRow << DDI_0_OSC_ANABYPASSVAL1_XOSC_HF_ROW_Q12_S    ) |
                     ( ui32XoscHfCol << DDI_0_OSC_ANABYPASSVAL1_XOSC_HF_COLUMN_Q12_S )   );

    return (ui32TrimValue);
}

uint32_t SetupGetTrimForRcOscLfRtuneCtuneTrim( void ) {
    uint32_t ui32TrimValue;

    // Use device specific trim values located in factory configuration
    // area
    ui32TrimValue =
        ((HWREG(FCFG1_BASE + FCFG1_O_CONFIG_OSC_TOP) &
          FCFG1_CONFIG_OSC_TOP_RCOSCLF_CTUNE_TRIM_M)>>
          FCFG1_CONFIG_OSC_TOP_RCOSCLF_CTUNE_TRIM_S)<<
            DDI_0_OSC_LFOSCCTL_RCOSCLF_CTUNE_TRIM_S;

    ui32TrimValue |=
        ((HWREG(FCFG1_BASE + FCFG1_O_CONFIG_OSC_TOP) &
          FCFG1_CONFIG_OSC_TOP_RCOSCLF_RTUNE_TRIM_M)>>
          FCFG1_CONFIG_OSC_TOP_RCOSCLF_RTUNE_TRIM_S)<<
            DDI_0_OSC_LFOSCCTL_RCOSCLF_RTUNE_TRIM_S;

    return(ui32TrimValue);
}

uint32_t SetupGetTrimForXoscHfIbiastherm( void ) {
    uint32_t ui32TrimValue;

    // Use device specific trim value located in factory configuration
    // area
    ui32TrimValue =
        (HWREG(FCFG1_BASE + FCFG1_O_ANABYPASS_VALUE2) &
         FCFG1_ANABYPASS_VALUE2_XOSC_HF_IBIASTHERM_M)>>
         FCFG1_ANABYPASS_VALUE2_XOSC_HF_IBIASTHERM_S;

    return(ui32TrimValue);
}

uint32_t SetupGetTrimForAmpcompTh2( void ) {
    uint32_t ui32TrimValue;
    uint32_t ui32Fcfg1Value;

    // Use device specific trim value located in factory configuration
    // area. All defined register bit fields have corresponding trim
    // value in the factory configuration area
    ui32Fcfg1Value = HWREG(FCFG1_BASE + FCFG1_O_AMPCOMP_TH2);
    ui32TrimValue = ((ui32Fcfg1Value &
                      FCFG1_AMPCOMP_TH2_LPMUPDATE_LTH_M)>>
                      FCFG1_AMPCOMP_TH2_LPMUPDATE_LTH_S)<<
                   DDI_0_OSC_AMPCOMPTH2_LPMUPDATE_LTH_S;
    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_TH2_LPMUPDATE_HTM_M)>>
                        FCFG1_AMPCOMP_TH2_LPMUPDATE_HTM_S)<<
                     DDI_0_OSC_AMPCOMPTH2_LPMUPDATE_HTH_S);
    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_TH2_ADC_COMP_AMPTH_LPM_M)>>
                        FCFG1_AMPCOMP_TH2_ADC_COMP_AMPTH_LPM_S)<<
                     DDI_0_OSC_AMPCOMPTH2_ADC_COMP_AMPTH_LPM_S);
    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_TH2_ADC_COMP_AMPTH_HPM_M)>>
                        FCFG1_AMPCOMP_TH2_ADC_COMP_AMPTH_HPM_S)<<
                     DDI_0_OSC_AMPCOMPTH2_ADC_COMP_AMPTH_HPM_S);

    return(ui32TrimValue);
}

uint32_t SetupGetTrimForAmpcompTh1( void ) {
    uint32_t ui32TrimValue;
    uint32_t ui32Fcfg1Value;

    // Use device specific trim values located in factory configuration
    // area. All defined register bit fields have a corresponding trim
    // value in the factory configuration area
    ui32Fcfg1Value = HWREG(FCFG1_BASE + FCFG1_O_AMPCOMP_TH1);
    ui32TrimValue = (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_TH1_HPMRAMP3_LTH_M)>>
                        FCFG1_AMPCOMP_TH1_HPMRAMP3_LTH_S)<<
                     DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_LTH_S);
    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_TH1_HPMRAMP3_HTH_M)>>
                        FCFG1_AMPCOMP_TH1_HPMRAMP3_HTH_S)<<
                     DDI_0_OSC_AMPCOMPTH1_HPMRAMP3_HTH_S);
    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_TH1_IBIASCAP_LPTOHP_OL_CNT_M)>>
                        FCFG1_AMPCOMP_TH1_IBIASCAP_LPTOHP_OL_CNT_S)<<
                     DDI_0_OSC_AMPCOMPTH1_IBIASCAP_LPTOHP_OL_CNT_S);
    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_TH1_HPMRAMP1_TH_M)>>
                        FCFG1_AMPCOMP_TH1_HPMRAMP1_TH_S)<<
                     DDI_0_OSC_AMPCOMPTH1_HPMRAMP1_TH_S);

    return(ui32TrimValue);
}

uint32_t SetupGetTrimForAmpcompCtrl( uint32_t ui32Fcfg1Revision ) {
    uint32_t ui32TrimValue    ;
    uint32_t ui32Fcfg1Value   ;
    uint32_t ibiasOffset      ;
    uint32_t ibiasInit        ;
    uint32_t modeConf1        ;
    int32_t  deltaAdjust      ;

    // Use device specific trim values located in factory configuration
    // area. Register bit fields without trim values in the factory
    // configuration area will be set to the value of 0.
    ui32Fcfg1Value = HWREG( FCFG1_BASE + FCFG1_O_AMPCOMP_CTRL1 );

    ibiasOffset    = ( ui32Fcfg1Value &
                       FCFG1_AMPCOMP_CTRL1_IBIAS_OFFSET_M ) >>
                       FCFG1_AMPCOMP_CTRL1_IBIAS_OFFSET_S ;
    ibiasInit      = ( ui32Fcfg1Value &
                       FCFG1_AMPCOMP_CTRL1_IBIAS_INIT_M ) >>
                       FCFG1_AMPCOMP_CTRL1_IBIAS_INIT_S ;

    if (( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR_M ) == 0 ) {
        // Adjust with DELTA_IBIAS_OFFSET and DELTA_IBIAS_INIT from CCFG
        modeConf1   = HWREG( CCFG_BASE + CCFG_O_MODE_CONF_1 );

        // Both fields are signed 4-bit values. This is an assumption when doing the sign extension.
        deltaAdjust =
            (((int32_t)( modeConf1 << ( 32 - CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_W - CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_S )))
                                   >> ( 32 - CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_W ));
        deltaAdjust += (int32_t)ibiasOffset;
        if ( deltaAdjust < 0 ) {
            deltaAdjust  = 0;
        }
        if ( deltaAdjust > ( DDI_0_OSC_AMPCOMPCTL_IBIAS_OFFSET_M >> DDI_0_OSC_AMPCOMPCTL_IBIAS_OFFSET_S )) {
            deltaAdjust  = ( DDI_0_OSC_AMPCOMPCTL_IBIAS_OFFSET_M >> DDI_0_OSC_AMPCOMPCTL_IBIAS_OFFSET_S );
        }
        ibiasOffset = (uint32_t)deltaAdjust;

        deltaAdjust =
            (((int32_t)( modeConf1 << ( 32 - CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_W - CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_S )))
                                   >> ( 32 - CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_W ));
        deltaAdjust += (int32_t)ibiasInit;
        if ( deltaAdjust < 0 ) {
            deltaAdjust  = 0;
        }
        if ( deltaAdjust > ( DDI_0_OSC_AMPCOMPCTL_IBIAS_INIT_M >> DDI_0_OSC_AMPCOMPCTL_IBIAS_INIT_S )) {
            deltaAdjust  = ( DDI_0_OSC_AMPCOMPCTL_IBIAS_INIT_M >> DDI_0_OSC_AMPCOMPCTL_IBIAS_INIT_S );
        }
        ibiasInit = (uint32_t)deltaAdjust;
    }
    ui32TrimValue = ( ibiasOffset << DDI_0_OSC_AMPCOMPCTL_IBIAS_OFFSET_S ) |
                    ( ibiasInit   << DDI_0_OSC_AMPCOMPCTL_IBIAS_INIT_S   ) ;

    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_CTRL1_LPM_IBIAS_WAIT_CNT_FINAL_M)>>
                        FCFG1_AMPCOMP_CTRL1_LPM_IBIAS_WAIT_CNT_FINAL_S)<<
                       DDI_0_OSC_AMPCOMPCTL_LPM_IBIAS_WAIT_CNT_FINAL_S);
    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_CTRL1_CAP_STEP_M)>>
                        FCFG1_AMPCOMP_CTRL1_CAP_STEP_S)<<
                       DDI_0_OSC_AMPCOMPCTL_CAP_STEP_S);
    ui32TrimValue |= (((ui32Fcfg1Value &
                        FCFG1_AMPCOMP_CTRL1_IBIASCAP_HPTOLP_OL_CNT_M)>>
                        FCFG1_AMPCOMP_CTRL1_IBIASCAP_HPTOLP_OL_CNT_S)<<
                       DDI_0_OSC_AMPCOMPCTL_IBIASCAP_HPTOLP_OL_CNT_S);

    if ( ui32Fcfg1Revision >= 0x00000022 ) {
        ui32TrimValue |= ((( ui32Fcfg1Value &
            FCFG1_AMPCOMP_CTRL1_AMPCOMP_REQ_MODE_M ) >>
            FCFG1_AMPCOMP_CTRL1_AMPCOMP_REQ_MODE_S ) <<
           DDI_0_OSC_AMPCOMPCTL_AMPCOMP_REQ_MODE_S );
    }

    return(ui32TrimValue);
}

uint32_t SetupGetTrimForDblrLoopFilterResetVoltage( uint32_t ui32Fcfg1Revision ) {
   uint32_t dblrLoopFilterResetVoltageValue = 0; // Reset value

   if ( ui32Fcfg1Revision >= 0x00000020 ) {
      dblrLoopFilterResetVoltageValue = ( HWREG( FCFG1_BASE + FCFG1_O_MISC_OTP_DATA_1 ) &
         FCFG1_MISC_OTP_DATA_1_DBLR_LOOP_FILTER_RESET_VOLTAGE_M ) >>
         FCFG1_MISC_OTP_DATA_1_DBLR_LOOP_FILTER_RESET_VOLTAGE_S;
   }

   return ( dblrLoopFilterResetVoltageValue );
}

uint32_t SetupGetTrimForAdcShModeEn( uint32_t ui32Fcfg1Revision ) {
   uint32_t getTrimForAdcShModeEnValue = 1; // Recommended default setting

   if ( ui32Fcfg1Revision >= 0x00000022 ) {
      getTrimForAdcShModeEnValue = ( HWREG( FCFG1_BASE + FCFG1_O_OSC_CONF ) &
         FCFG1_OSC_CONF_ADC_SH_MODE_EN_M ) >>
         FCFG1_OSC_CONF_ADC_SH_MODE_EN_S;
   }

   return ( getTrimForAdcShModeEnValue );
}

uint32_t SetupGetTrimForAdcShVbufEn( uint32_t ui32Fcfg1Revision ) {
   uint32_t getTrimForAdcShVbufEnValue = 1; // Recommended default setting

   if ( ui32Fcfg1Revision >= 0x00000022 ) {
      getTrimForAdcShVbufEnValue = ( HWREG( FCFG1_BASE + FCFG1_O_OSC_CONF ) &
         FCFG1_OSC_CONF_ADC_SH_VBUF_EN_M ) >>
         FCFG1_OSC_CONF_ADC_SH_VBUF_EN_S;
   }

   return ( getTrimForAdcShVbufEnValue );
}

uint32_t SetupGetTrimForXoscHfCtl( uint32_t ui32Fcfg1Revision ) {
   uint32_t getTrimForXoschfCtlValue = 0; // Recommended default setting
   uint32_t fcfg1Data;

   if ( ui32Fcfg1Revision >= 0x00000020 ) {
      fcfg1Data = HWREG( FCFG1_BASE + FCFG1_O_MISC_OTP_DATA_1 );
      getTrimForXoschfCtlValue =
         ( ( ( fcfg1Data & FCFG1_MISC_OTP_DATA_1_PEAK_DET_ITRIM_M ) >>
             FCFG1_MISC_OTP_DATA_1_PEAK_DET_ITRIM_S ) <<
           DDI_0_OSC_XOSCHFCTL_PEAK_DET_ITRIM_S);

      getTrimForXoschfCtlValue |=
         ( ( ( fcfg1Data & FCFG1_MISC_OTP_DATA_1_HP_BUF_ITRIM_M ) >>
             FCFG1_MISC_OTP_DATA_1_HP_BUF_ITRIM_S ) <<
           DDI_0_OSC_XOSCHFCTL_HP_BUF_ITRIM_S);

      getTrimForXoschfCtlValue |=
         ( ( ( fcfg1Data & FCFG1_MISC_OTP_DATA_1_LP_BUF_ITRIM_M ) >>
             FCFG1_MISC_OTP_DATA_1_LP_BUF_ITRIM_S ) <<
           DDI_0_OSC_XOSCHFCTL_LP_BUF_ITRIM_S);
   }

   return ( getTrimForXoschfCtlValue );
}

uint32_t SetupGetTrimForXoscHfFastStart( void ) {
   uint32_t ui32XoscHfFastStartValue   ;

   // Get value from FCFG1
   ui32XoscHfFastStartValue = ( HWREG( FCFG1_BASE + FCFG1_O_OSC_CONF ) &
      FCFG1_OSC_CONF_XOSC_HF_FAST_START_M ) >>
      FCFG1_OSC_CONF_XOSC_HF_FAST_START_S;

   return ( ui32XoscHfFastStartValue );
}

uint32_t SetupGetTrimForRadcExtCfg( uint32_t ui32Fcfg1Revision ) {
   uint32_t getTrimForRadcExtCfgValue = 0x403F8000; // Recommended default setting
   uint32_t fcfg1Data;

   if ( ui32Fcfg1Revision >= 0x00000020 ) {
      fcfg1Data = HWREG( FCFG1_BASE + FCFG1_O_MISC_OTP_DATA_1 );
      getTrimForRadcExtCfgValue =
         ( ( ( fcfg1Data & FCFG1_MISC_OTP_DATA_1_HPM_IBIAS_WAIT_CNT_M ) >>
             FCFG1_MISC_OTP_DATA_1_HPM_IBIAS_WAIT_CNT_S ) <<
           DDI_0_OSC_RADCEXTCFG_HPM_IBIAS_WAIT_CNT_S);

      getTrimForRadcExtCfgValue |=
         ( ( ( fcfg1Data & FCFG1_MISC_OTP_DATA_1_LPM_IBIAS_WAIT_CNT_M ) >>
             FCFG1_MISC_OTP_DATA_1_LPM_IBIAS_WAIT_CNT_S ) <<
           DDI_0_OSC_RADCEXTCFG_LPM_IBIAS_WAIT_CNT_S);

      getTrimForRadcExtCfgValue |=
         ( ( ( fcfg1Data & FCFG1_MISC_OTP_DATA_1_IDAC_STEP_M ) >>
             FCFG1_MISC_OTP_DATA_1_IDAC_STEP_S ) <<
           DDI_0_OSC_RADCEXTCFG_IDAC_STEP_S);
   }

   return ( getTrimForRadcExtCfgValue );
}

uint32_t SetupGetTrimForRcOscLfIBiasTrim( uint32_t ui32Fcfg1Revision ) {
   uint32_t trimForRcOscLfIBiasTrimValue = 0; // Default value

   if ( ui32Fcfg1Revision >= 0x00000022 ) {
      trimForRcOscLfIBiasTrimValue = ( HWREG( FCFG1_BASE + FCFG1_O_OSC_CONF ) &
         FCFG1_OSC_CONF_ATESTLF_RCOSCLF_IBIAS_TRIM_M ) >>
         FCFG1_OSC_CONF_ATESTLF_RCOSCLF_IBIAS_TRIM_S ;
   }

   return ( trimForRcOscLfIBiasTrimValue );
}

uint32_t SetupGetTrimForXoscLfRegulatorAndCmirrwrRatio( uint32_t ui32Fcfg1Revision ) {
   uint32_t trimForXoscLfRegulatorAndCmirrwrRatioValue = 0; // Default value for both fields

   if ( ui32Fcfg1Revision >= 0x00000022 ) {
      trimForXoscLfRegulatorAndCmirrwrRatioValue = ( HWREG( FCFG1_BASE + FCFG1_O_OSC_CONF ) &
         ( FCFG1_OSC_CONF_XOSCLF_REGULATOR_TRIM_M |
           FCFG1_OSC_CONF_XOSCLF_CMIRRWR_RATIO_M  )) >>
           FCFG1_OSC_CONF_XOSCLF_CMIRRWR_RATIO_S  ;
   }

   return ( trimForXoscLfRegulatorAndCmirrwrRatioValue );
}

void SetupSetCacheModeAccordingToCcfgSetting( void ) {
    // - Make sure to enable aggressive VIMS clock gating for power optimization
    //   Only for PG2 devices.
    // - Enable cache prefetch enable as default setting
    //   (Slightly higher power consumption, but higher CPU performance)
    // - IF ( CCFG_..._DIS_GPRAM == 1 )
    //   then: Enable cache (set cache mode = 1), even if set by ROM boot code
    //         (This is done because it's not set by boot code when running inside
    //         a debugger supporting the Halt In Boot (HIB) functionality).
    //   else: Set MODE_GPRAM if not already set (see inline comments as well)
    uint32_t vimsCtlMode0 ;

    while ( HWREGBITW( VIMS_BASE + VIMS_O_STAT, VIMS_STAT_MODE_CHANGING_BITN )) {
        // Do nothing - wait for an eventual ongoing mode change to complete.
        // (There should typically be no wait time here, but need to be sure)
    }

    // Note that Mode=0 is equal to MODE_GPRAM
    vimsCtlMode0 = (( HWREG( VIMS_BASE + VIMS_O_CTL ) & ~VIMS_CTL_MODE_M ) | VIMS_CTL_DYN_CG_EN_M | VIMS_CTL_PREF_EN_M );

#if ( defined( DO_NOT_ENABLE_CACHE_IN_TRIM_DEVICE ))
 #if ( defined( CODE_IN_FLASH ))
    // Enable cache (and hence disable GPRAM)
    HWREG( VIMS_BASE + VIMS_O_CTL ) = ( vimsCtlMode0 | VIMS_CTL_MODE_CACHE );
 #else
    HWREG( VIMS_BASE + VIMS_O_CTL ) = vimsCtlMode0;
 #endif
#else

    if ( HWREG( CCFG_BASE + CCFG_O_SIZE_AND_DIS_FLAGS ) & CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM ) {
        // Enable cache (and hence disable GPRAM)
        HWREG( VIMS_BASE + VIMS_O_CTL ) = ( vimsCtlMode0 | VIMS_CTL_MODE_CACHE );
    } else if (( HWREG( VIMS_BASE + VIMS_O_STAT ) & VIMS_STAT_MODE_M ) != VIMS_STAT_MODE_GPRAM ) {
        // GPRAM is enabled in CCFG but not selected
        // Note: It is recommended to go via MODE_OFF when switching to MODE_GPRAM
        HWREG( VIMS_BASE + VIMS_O_CTL ) = ( vimsCtlMode0 | VIMS_CTL_MODE_OFF );
        while (( HWREG( VIMS_BASE + VIMS_O_STAT ) & VIMS_STAT_MODE_M ) != VIMS_STAT_MODE_OFF ) {
            // Do nothing - wait for an eventual mode change to complete (This goes fast).
        }
        HWREG( VIMS_BASE + VIMS_O_CTL ) = vimsCtlMode0;
    } else {
        // Correct mode, but make sure PREF_EN and DYN_CG_EN always are set
        HWREG( VIMS_BASE + VIMS_O_CTL ) = vimsCtlMode0;
    }
#endif
}

void SetupSetAonRtcSubSecInc( uint32_t subSecInc ) {
   // Loading a new RTCSUBSECINC value is done in 5 steps:
   // 1. Write bit[15:0] of new SUBSECINC value to AUX_SYSIF_O_RTCSUBSECINC0
   // 2. Write bit[23:16] of new SUBSECINC value to AUX_SYSIF_O_RTCSUBSECINC1
   // 3. Set AUX_SYSIF_RTCSUBSECINCCTL_UPD_REQ
   // 4. Wait for AUX_SYSIF_RTCSUBSECINCCTL_UPD_ACK
   // 5. Clear AUX_SYSIF_RTCSUBSECINCCTL_UPD_REQ
   HWREG( AUX_SYSIF_BASE + AUX_SYSIF_O_RTCSUBSECINC0 ) = (( subSecInc       ) & AUX_SYSIF_RTCSUBSECINC0_INC15_0_M  );
   HWREG( AUX_SYSIF_BASE + AUX_SYSIF_O_RTCSUBSECINC1 ) = (( subSecInc >> 16 ) & AUX_SYSIF_RTCSUBSECINC1_INC23_16_M );

   HWREG( AUX_SYSIF_BASE + AUX_SYSIF_O_RTCSUBSECINCCTL ) = AUX_SYSIF_RTCSUBSECINCCTL_UPD_REQ;
   while( ! ( HWREGBITW( AUX_SYSIF_BASE + AUX_SYSIF_O_RTCSUBSECINCCTL, AUX_SYSIF_RTCSUBSECINCCTL_UPD_ACK_BITN )));
   HWREG( AUX_SYSIF_BASE + AUX_SYSIF_O_RTCSUBSECINCCTL ) = 0;
}

void I2SPointerSet(uint32_t ui32Base, bool bInput, void * pNextPointer) {
    // Check the arguments.
    ASSERT(I2SBaseValid(ui32Base));

    // Update the next input/output pointer with the correct address.
    if(bInput == true)
    {
        HWREG(I2S0_BASE + I2S_O_AIFINPTRNEXT) = (uint32_t)pNextPointer;
    }
    else
    {
        HWREG(I2S0_BASE + I2S_O_AIFOUTPTRNEXT) = (uint32_t)pNextPointer;
    }
}

uint32_t I2SSampleStampGet(uint32_t ui32Base, uint32_t ui32Channel) {
    uint32_t ui32FrameClkCnt;
    uint32_t ui32SysClkCnt;
    uint32_t ui32PeriodSysClkCnt;
    uint32_t ui32SampleStamp;

    // Get the number of Frame clock counts since last stamp.
    ui32FrameClkCnt = HWREG(I2S0_BASE + I2S_O_STMPWCNTCAPT0);

    // Get the number of system clock ticks since last frame clock edge.
    ui32SysClkCnt = HWREG(I2S0_BASE + I2S_O_STMPXCNTCAPT0);

    // Get the number system clock ticks in the last frame clock period.
    ui32PeriodSysClkCnt = HWREG(I2S0_BASE + I2S_O_STMPXPER);

    // Calculate the sample stamp.
    ui32SampleStamp = (ui32SysClkCnt << 16) / ui32PeriodSysClkCnt;
    ui32SampleStamp = (ui32SampleStamp > I2S_STMP_SATURATION) ?
                      I2S_STMP_SATURATION : ui32SampleStamp;
    ui32SampleStamp |= (ui32FrameClkCnt << 16);

    return (ui32SampleStamp);
}

void PowerCtrlSourceSet(uint32_t ui32PowerConfig) {
    // Check the arguments.
    ASSERT((ui32PowerConfig == PWRCTRL_PWRSRC_DCDC) ||
           (ui32PowerConfig == PWRCTRL_PWRSRC_GLDO) ||
           (ui32PowerConfig == PWRCTRL_PWRSRC_ULDO));

    // Configure the power.
    if(ui32PowerConfig == PWRCTRL_PWRSRC_DCDC) {
        HWREG(AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL) |=
            (AON_PMCTL_PWRCTL_DCDC_EN | AON_PMCTL_PWRCTL_DCDC_ACTIVE);
    }
    else if (ui32PowerConfig == PWRCTRL_PWRSRC_GLDO)
    {
        HWREG(AON_PMCTL_BASE + AON_PMCTL_O_PWRCTL) &=
            ~(AON_PMCTL_PWRCTL_DCDC_EN | AON_PMCTL_PWRCTL_DCDC_ACTIVE);
    }
    else
    {
        PRCMMcuUldoConfigure(true);
    }
}

void AESSetInitializationVector(const uint32_t *initializationVector) {
    // Write initialization vector to the aes registers
    HWREG(CRYPTO_BASE + CRYPTO_O_AESIV0) = initializationVector[0];
    HWREG(CRYPTO_BASE + CRYPTO_O_AESIV1) = initializationVector[1];
    HWREG(CRYPTO_BASE + CRYPTO_O_AESIV2) = initializationVector[2];
    HWREG(CRYPTO_BASE + CRYPTO_O_AESIV3) = initializationVector[3];
}

void AESStartDMAOperation(const uint8_t *channel0Addr, uint32_t channel0Length,  uint8_t *channel1Addr, uint32_t channel1Length) {
    if (channel0Length && channel0Addr) {
        // We actually want to perform an operation. Clear any outstanding events.
        HWREG(CRYPTO_BASE + CRYPTO_O_IRQCLR) = CRYPTO_IRQCLR_RESULT_AVAIL_M | CRYPTO_IRQEN_DMA_IN_DONE_M; // This might need AES_IRQEN_DMA_IN_DONE as well

        while(HWREG(CRYPTO_BASE + CRYPTO_O_IRQSTAT) & (CRYPTO_IRQSTAT_DMA_IN_DONE_M | CRYPTO_IRQSTAT_RESULT_AVAIL_M));

        // Configure the DMA controller - enable both DMA channels.
        HWREGBITW(CRYPTO_BASE + CRYPTO_O_DMACH0CTL, CRYPTO_DMACH0CTL_EN_BITN) = 1;

        // Base address of the payload data in ext. memory.
        HWREG(CRYPTO_BASE + CRYPTO_O_DMACH0EXTADDR) = (uint32_t)channel0Addr;

        // Payload data length in bytes, equal to the cipher text length.
        HWREG(CRYPTO_BASE + CRYPTO_O_DMACH0LEN) = channel0Length;
    }

    if (channel1Length && channel1Addr) {
        // Enable DMA channel 1.
        HWREGBITW(CRYPTO_BASE + CRYPTO_O_DMACH1CTL, CRYPTO_DMACH1CTL_EN_BITN) = 1;

        // Base address of the output data buffer.
        HWREG(CRYPTO_BASE + CRYPTO_O_DMACH1EXTADDR) = (uint32_t)channel1Addr;

        // Output data length in bytes, equal to the cipher text length.
        HWREG(CRYPTO_BASE + CRYPTO_O_DMACH1LEN) = channel1Length;
    }
}

uint32_t AESWaitForIRQFlags(uint32_t irqFlags) {
    uint32_t irqTrigger = 0;
    // Wait for the DMA operation to complete. Add a delay to make sure we are
    // not flooding the bus with requests too much.
    do {
        CPUdelay(1);
    }
    while(!(HWREG(CRYPTO_BASE + CRYPTO_O_IRQSTAT) & irqFlags & (CRYPTO_IRQSTAT_DMA_IN_DONE_M |
                                                                CRYPTO_IRQSTAT_RESULT_AVAIL_M |
                                                                CRYPTO_IRQSTAT_DMA_BUS_ERR_M |
                                                                CRYPTO_IRQSTAT_KEY_ST_WR_ERR_M)));

    // Save the IRQ trigger source
    irqTrigger = HWREG(CRYPTO_BASE + CRYPTO_O_IRQSTAT) & irqFlags;

    // Clear IRQ flags
    HWREG(CRYPTO_BASE + CRYPTO_O_IRQCLR) = irqTrigger;

    return irqTrigger;
}

uint32_t AESWriteToKeyStore(const uint8_t *aesKey, uint32_t aesKeyLength, uint32_t keyStoreArea) {
    // Check the arguments.
    ASSERT((keyStoreArea == AES_KEY_AREA_0) ||
           (keyStoreArea == AES_KEY_AREA_1) ||
           (keyStoreArea == AES_KEY_AREA_2) ||
           (keyStoreArea == AES_KEY_AREA_3) ||
           (keyStoreArea == AES_KEY_AREA_4) ||
           (keyStoreArea == AES_KEY_AREA_5) ||
           (keyStoreArea == AES_KEY_AREA_6) ||
           (keyStoreArea == AES_KEY_AREA_7));

    ASSERT((aesKeyLength == AES_128_KEY_LENGTH_BYTES) ||
           (aesKeyLength == AES_192_KEY_LENGTH_BYTES) ||
           (aesKeyLength == AES_256_KEY_LENGTH_BYTES));

    uint32_t keySize = 0;

    switch (aesKeyLength) {
        case AES_128_KEY_LENGTH_BYTES:
            keySize = CRYPTO_KEYSIZE_SIZE_128_BIT;
            break;
        case AES_192_KEY_LENGTH_BYTES:
            keySize = CRYPTO_KEYSIZE_SIZE_192_BIT;
            break;
        case AES_256_KEY_LENGTH_BYTES:
            keySize = CRYPTO_KEYSIZE_SIZE_256_BIT;
            break;
    }

    // Clear any previously written key at the keyLocation
    AESInvalidateKey(keyStoreArea);

    // Disable the external interrupt to stop the interrupt form propagating
    // from the module to the System CPU.
    IntDisable(INT_CRYPTO_RESULT_AVAIL_IRQ);

    // Enable internal interrupts.
    HWREG(CRYPTO_BASE + CRYPTO_O_IRQTYPE) = CRYPTO_IRQTYPE_LEVEL_M;
    HWREG(CRYPTO_BASE + CRYPTO_O_IRQEN) = CRYPTO_IRQEN_DMA_IN_DONE_M | CRYPTO_IRQEN_RESULT_AVAIL_M;

    // Configure master control module.
    HWREG(CRYPTO_BASE + CRYPTO_O_ALGSEL) = CRYPTO_ALGSEL_KEY_STORE;

    // Clear any outstanding events.
    HWREG(CRYPTO_BASE + CRYPTO_O_IRQCLR) = (CRYPTO_IRQCLR_DMA_IN_DONE | CRYPTO_IRQCLR_RESULT_AVAIL);

    // Configure the size of keys contained within the key store
    // Do not write to the register if the correct key size is already set.
    // Writing to this register causes all current keys to be invalidated.
    uint32_t keyStoreKeySize = HWREG(CRYPTO_BASE + CRYPTO_O_KEYSIZE);
    if (keySize != keyStoreKeySize) {
        HWREG(CRYPTO_BASE + CRYPTO_O_KEYSIZE) = keySize;
    }

    // Enable key to write (e.g. Key 0).
    HWREG(CRYPTO_BASE + CRYPTO_O_KEYWRITEAREA) = 1 << keyStoreArea;

    // Total key length in bytes (16 for 1 x 128-bit key and 32 for 1 x 256-bit key).
    AESStartDMAOperation(aesKey, aesKeyLength, 0, 0);

    // Wait for the DMA operation to complete.
    uint32_t irqTrigger = AESWaitForIRQFlags(CRYPTO_IRQCLR_RESULT_AVAIL | CRYPTO_IRQCLR_DMA_IN_DONE | CRYPTO_IRQSTAT_DMA_BUS_ERR | CRYPTO_IRQSTAT_KEY_ST_WR_ERR);

    // Re-enable interrupts globally.
    IntPendClear(INT_CRYPTO_RESULT_AVAIL_IRQ);
    IntEnable(INT_CRYPTO_RESULT_AVAIL_IRQ);

    // If we had a bus error or the key is not in the CRYPTO_O_KEYWRITTENAREA, return an error.
    if ((irqTrigger & (CRYPTO_IRQSTAT_DMA_BUS_ERR_M | CRYPTO_IRQSTAT_KEY_ST_WR_ERR_M)) || !(HWREG(CRYPTO_BASE + CRYPTO_O_KEYWRITTENAREA) & (1 << keyStoreArea))) {
        // There was an error in writing to the keyStore.
        return AES_KEYSTORE_ERROR;
    }
    else {
        return AES_SUCCESS;
    }
}

uint32_t AESReadFromKeyStore(uint32_t keyStoreArea) {
    // Check the arguments.
    ASSERT((keyStoreArea == AES_KEY_AREA_0) ||
           (keyStoreArea == AES_KEY_AREA_1) ||
           (keyStoreArea == AES_KEY_AREA_2) ||
           (keyStoreArea == AES_KEY_AREA_3) ||
           (keyStoreArea == AES_KEY_AREA_4) ||
           (keyStoreArea == AES_KEY_AREA_5) ||
           (keyStoreArea == AES_KEY_AREA_6) ||
           (keyStoreArea == AES_KEY_AREA_7));

    // Check if there is a valid key in the specified keyStoreArea
    if (!(HWREG(CRYPTO_BASE + CRYPTO_O_KEYWRITTENAREA) & (1 << keyStoreArea))) {
        return AES_KEYSTORE_AREA_INVALID;
    }

    // Enable keys to read (e.g. Key 0).
    HWREG(CRYPTO_BASE + CRYPTO_O_KEYREADAREA) = keyStoreArea;

    // Wait until key is loaded to the AES module.
    // We cannot simply poll the IRQ status as only an error is communicated through
    // the IRQ status and not the completion of the transfer.
    do {
        CPUdelay(1);
    }
    while((HWREG(CRYPTO_BASE + CRYPTO_O_KEYREADAREA) & CRYPTO_KEYREADAREA_BUSY_M));

    // Check for keyStore read error.
    if((HWREG(CRYPTO_BASE + CRYPTO_O_IRQSTAT) & CRYPTO_IRQSTAT_KEY_ST_RD_ERR_M)) {
        return AES_KEYSTORE_ERROR;
    }
    else {
        return AES_SUCCESS;
    }
}

uint32_t AESReadTag(uint8_t *tag, uint32_t tagLength) {
    // The intermediate array is used instead of a caller-provided one
    // to enable a simple API with no unintuitive alignment or size requirements.
    // This is a trade-off of stack-depth vs ease-of-use that came out on the
    // ease-of-use side.
    uint32_t computedTag[AES_BLOCK_SIZE / sizeof(uint32_t)];

    // Wait until the computed tag is ready.
    while (!(HWREG(CRYPTO_BASE + CRYPTO_O_AESCTL) & CRYPTO_AESCTL_SAVED_CONTEXT_RDY_M));

    // Read computed tag out from the HW registers
    // Need to read out all 128 bits in four reads to correctly clear CRYPTO_AESCTL_SAVED_CONTEXT_RDY flag
    computedTag[0] = HWREG(CRYPTO_BASE + CRYPTO_O_AESTAGOUT0);
    computedTag[1] = HWREG(CRYPTO_BASE + CRYPTO_O_AESTAGOUT1);
    computedTag[2] = HWREG(CRYPTO_BASE + CRYPTO_O_AESTAGOUT2);
    computedTag[3] = HWREG(CRYPTO_BASE + CRYPTO_O_AESTAGOUT3);

    memcpy(tag, computedTag, tagLength);

    return AES_SUCCESS;
}

uint32_t AESVerifyTag(const uint8_t *tag, uint32_t tagLength) {
    uint32_t resultStatus;
    // The intermediate array is allocated on the stack to ensure users do not
    // point the tag they provide and the one computed at the same location.
    // That would cause memcmp to compare an array against itself. We could add
    // a check that verifies that the arrays are not the same. If we did that and
    // modified AESReadTag to just copy all 128 bits into a provided array,
    // we could save 16 bytes of stack space while making the API much more
    // complicated.
    uint8_t computedTag[AES_BLOCK_SIZE];

    resultStatus = AESReadTag(computedTag, tagLength);

    if (resultStatus != AES_SUCCESS) {
        return resultStatus;
    }

    resultStatus = memcmp(computedTag, tag, tagLength);

    if (resultStatus != 0) {
        return AES_TAG_VERIFICATION_FAILED;
    }

    return AES_SUCCESS;
}

void AESConfigureCCMCtrl(uint32_t nonceLength, uint32_t macLength, bool encrypt) {
    uint32_t ctrlVal = 0;

    ctrlVal = ((15 - nonceLength - 1) << CRYPTO_AESCTL_CCM_L_S);
    if ( macLength >= 2 ) {
        ctrlVal |= ((( macLength - 2 ) >> 1 ) << CRYPTO_AESCTL_CCM_M_S );
    }
    ctrlVal |= CRYPTO_AESCTL_CCM |
               CRYPTO_AESCTL_CTR |
               CRYPTO_AESCTL_SAVE_CONTEXT |
               CRYPTO_AESCTL_CTR_WIDTH_128_BIT;
    ctrlVal |= encrypt ? CRYPTO_AESCTL_DIR : 0;

    AESSetCtrl(ctrlVal);
}

void AESWriteCCMInitializationVector(const uint8_t *nonce, uint32_t nonceLength) {
    union {
        uint32_t word[4];
        uint8_t  byte[16];
    } initializationVector = {{0}};

    initializationVector.byte[0] = 15 - nonceLength - 1;

    memcpy(&(initializationVector.byte[1]), nonce, nonceLength);

    AESSetInitializationVector(initializationVector.word);
}


#define MAX(x,y)            (((x) > (y)) ?  (x) : (y))
#define MIN(x,y)            (((x) < (y)) ?  (x) : (y))
#define INRANGE(x,y,z)      ((x) > (y) && (x) < (z))


//*****************************************************************************
//
// Define for the maximum curve size supported by the PKA module in 32 bit
// word.
// \note PKA hardware module can support up to 384 bit curve size due to the
//       2K of PKA RAM.
//
//*****************************************************************************
#define PKA_MAX_CURVE_SIZE_32_BIT_WORD  12

//*****************************************************************************
//
// Define for the maximum length of the big number supported by the PKA module
// in 32 bit word.
//
//*****************************************************************************
#define PKA_MAX_LEN_IN_32_BIT_WORD  PKA_MAX_CURVE_SIZE_32_BIT_WORD

//*****************************************************************************
//
// Used in PKAWritePkaParam() and PKAWritePkaParamExtraOffset() to specify that
// the base address of the parameter should not be written to a NPTR register.
//
//*****************************************************************************
#define PKA_NO_POINTER_REG 0xFF

//*****************************************************************************
//
// NIST P224 constants in little endian format. byte[0] is the least
// significant byte and byte[NISTP224_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const PKA_EccPoint224 NISTP224_generator = {
    .x = {.byte = {0x21, 0x1D, 0x5C, 0x11, 0xD6, 0x80, 0x32, 0x34,
                   0x22, 0x11, 0xC2, 0x56, 0xD3, 0xC1, 0x03, 0x4A,
                   0xB9, 0x90, 0x13, 0x32, 0x7F, 0xBF, 0xB4, 0x6B,
                   0xBD, 0x0C, 0x0E, 0xB7, }},
    .y = {.byte = {0x34, 0x7E, 0x00, 0x85, 0x99, 0x81, 0xD5, 0x44,
                   0x64, 0x47, 0x07, 0x5A, 0xA0, 0x75, 0x43, 0xCD,
                   0xE6, 0xDF, 0x22, 0x4C, 0xFB, 0x23, 0xF7, 0xB5,
                   0x88, 0x63, 0x37, 0xBD, }},
};

const PKA_EccParam224 NISTP224_prime       = {.byte = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                       0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
                                                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                       0xFF, 0xFF, 0xFF, 0xFF}};

const PKA_EccParam224 NISTP224_a           = {.byte = {0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                       0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF,
                                                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                       0xFF, 0xFF, 0xFF, 0xFF}};

const PKA_EccParam224 NISTP224_b           = {.byte = {0xB4, 0xFF, 0x55, 0x23, 0x43, 0x39, 0x0B, 0x27,
                                                       0xBA, 0xD8, 0xBF, 0xD7, 0xB7, 0xB0, 0x44, 0x50,
                                                       0x56, 0x32, 0x41, 0xF5, 0xAB, 0xB3, 0x04, 0x0C,
                                                       0x85, 0x0A, 0x05, 0xB4}};

const PKA_EccParam224 NISTP224_order       = {.byte = {0x3D, 0x2A, 0x5C, 0x5C, 0x45, 0x29, 0xDD, 0x13,
                                                       0x3E, 0xF0, 0xB8, 0xE0, 0xA2, 0x16, 0xFF, 0xFF,
                                                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                       0xFF, 0xFF, 0xFF, 0xFF}};

//*****************************************************************************
//
// NIST P256 constants in little endian format. byte[0] is the least
// significant byte and byte[NISTP256_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const PKA_EccPoint256 NISTP256_generator = {
    .x = {.byte = {0x96, 0xc2, 0x98, 0xd8, 0x45, 0x39, 0xa1, 0xf4,
                   0xa0, 0x33, 0xeb, 0x2d, 0x81, 0x7d, 0x03, 0x77,
                   0xf2, 0x40, 0xa4, 0x63, 0xe5, 0xe6, 0xbc, 0xf8,
                   0x47, 0x42, 0x2c, 0xe1, 0xf2, 0xd1, 0x17, 0x6b}},
    .y = {.byte = {0xf5, 0x51, 0xbf, 0x37, 0x68, 0x40, 0xb6, 0xcb,
                   0xce, 0x5e, 0x31, 0x6b, 0x57, 0x33, 0xce, 0x2b,
                   0x16, 0x9e, 0x0f, 0x7c, 0x4a, 0xeb, 0xe7, 0x8e,
                   0x9b, 0x7f, 0x1a, 0xfe, 0xe2, 0x42, 0xe3, 0x4f}},
};

const PKA_EccParam256 NISTP256_prime       = {.byte = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                    0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff}};

const PKA_EccParam256 NISTP256_a           = {.byte = {0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                    0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff}};

const PKA_EccParam256 NISTP256_b           = {.byte = {0x4b, 0x60, 0xd2, 0x27, 0x3e, 0x3c, 0xce, 0x3b,
                                                    0xf6, 0xb0, 0x53, 0xcc, 0xb0, 0x06, 0x1d, 0x65,
                                                    0xbc, 0x86, 0x98, 0x76, 0x55, 0xbd, 0xeb, 0xb3,
                                                    0xe7, 0x93, 0x3a, 0xaa, 0xd8, 0x35, 0xc6, 0x5a}};

const PKA_EccParam256 NISTP256_order       = {.byte = {0x51, 0x25, 0x63, 0xfc, 0xc2, 0xca, 0xb9, 0xf3,
                                                    0x84, 0x9e, 0x17, 0xa7, 0xad, 0xfa, 0xe6, 0xbc,
                                                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff}};

//*****************************************************************************
//
// NIST P384 constants in little endian format. byte[0] is the least
// significant byte and byte[NISTP384_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const PKA_EccPoint384 NISTP384_generator = {
    .x = {.byte = {0xb7, 0x0a, 0x76, 0x72, 0x38, 0x5e, 0x54, 0x3a,
                   0x6c, 0x29, 0x55, 0xbf, 0x5d, 0xf2, 0x02, 0x55,
                   0x38, 0x2a, 0x54, 0x82, 0xe0, 0x41, 0xf7, 0x59,
                   0x98, 0x9b, 0xa7, 0x8b, 0x62, 0x3b, 0x1d, 0x6e,
                   0x74, 0xad, 0x20, 0xf3, 0x1e, 0xc7, 0xb1, 0x8e,
                   0x37, 0x05, 0x8b, 0xbe, 0x22, 0xca, 0x87, 0xaa}},
    .y = {.byte = {0x5f, 0x0e, 0xea, 0x90, 0x7c, 0x1d, 0x43, 0x7a,
                   0x9d, 0x81, 0x7e, 0x1d, 0xce, 0xb1, 0x60, 0x0a,
                   0xc0, 0xb8, 0xf0, 0xb5, 0x13, 0x31, 0xda, 0xe9,
                   0x7c, 0x14, 0x9a, 0x28, 0xbd, 0x1d, 0xf4, 0xf8,
                   0x29, 0xdc, 0x92, 0x92, 0xbf, 0x98, 0x9e, 0x5d,
                   0x6f, 0x2c, 0x26, 0x96, 0x4a, 0xde, 0x17, 0x36,}},
};

const PKA_EccParam384 NISTP384_prime       = {.byte = {0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                                       0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
                                                       0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

const PKA_EccParam384 NISTP384_a           = {.byte = {0xfc, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
                                                       0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
                                                       0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

const PKA_EccParam384 NISTP384_b           = {.byte = {0xef, 0x2a, 0xec, 0xd3, 0xed, 0xc8, 0x85, 0x2a,
                                                       0x9d, 0xd1, 0x2e, 0x8a, 0x8d, 0x39, 0x56, 0xc6,
                                                       0x5a, 0x87, 0x13, 0x50, 0x8f, 0x08, 0x14, 0x03,
                                                       0x12, 0x41, 0x81, 0xfe, 0x6e, 0x9c, 0x1d, 0x18,
                                                       0x19, 0x2d, 0xf8, 0xe3, 0x6b, 0x05, 0x8e, 0x98,
                                                       0xe4, 0xe7, 0x3e, 0xe2, 0xa7, 0x2f, 0x31, 0xb3}};

const PKA_EccParam384 NISTP384_order       = {.byte = {0x73, 0x29, 0xc5, 0xcc, 0x6a, 0x19, 0xec, 0xec,
                                                       0x7a, 0xa7, 0xb0, 0x48, 0xb2, 0x0d, 0x1a, 0x58,
                                                       0xdf, 0x2d, 0x37, 0xf4, 0x81, 0x4d, 0x63, 0xc7,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};


//*****************************************************************************
//
// NIST P521 constants in little endian format. byte[0] is the least
// significant byte and byte[NISTP521_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const PKA_EccPoint521 NISTP521_generator = {
    .x = {.byte = {0x66, 0xbd, 0xe5, 0xc2, 0x31, 0x7e, 0x7e, 0xf9,
                   0x9b, 0x42, 0x6a, 0x85, 0xc1, 0xb3, 0x48, 0x33,
                   0xde, 0xa8, 0xff, 0xa2, 0x27, 0xc1, 0x1d, 0xfe,
                   0x28, 0x59, 0xe7, 0xef, 0x77, 0x5e, 0x4b, 0xa1,
                   0xba, 0x3d, 0x4d, 0x6b, 0x60, 0xaf, 0x28, 0xf8,
                   0x21, 0xb5, 0x3f, 0x05, 0x39, 0x81, 0x64, 0x9c,
                   0x42, 0xb4, 0x95, 0x23, 0x66, 0xcb, 0x3e, 0x9e,
                   0xcd, 0xe9, 0x04, 0x04, 0xb7, 0x06, 0x8e, 0x85,
                   0xc6, 0x00}},
    .y = {.byte = {0x50, 0x66, 0xd1, 0x9f, 0x76, 0x94, 0xbe, 0x88,
                   0x40, 0xc2, 0x72, 0xa2, 0x86, 0x70, 0x3c, 0x35,
                   0x61, 0x07, 0xad, 0x3f, 0x01, 0xb9, 0x50, 0xc5,
                   0x40, 0x26, 0xf4, 0x5e, 0x99, 0x72, 0xee, 0x97,
                   0x2c, 0x66, 0x3e, 0x27, 0x17, 0xbd, 0xaf, 0x17,
                   0x68, 0x44, 0x9b, 0x57, 0x49, 0x44, 0xf5, 0x98,
                   0xd9, 0x1b, 0x7d, 0x2c, 0xb4, 0x5f, 0x8a, 0x5c,
                   0x04, 0xc0, 0x3b, 0x9a, 0x78, 0x6a, 0x29, 0x39,
                   0x18, 0x01}},
};

const PKA_EccParam521 NISTP521_prime       = {.byte = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0x01}};

const PKA_EccParam521 NISTP521_a           = {.byte = {0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0x01}};

const PKA_EccParam521 NISTP521_b           = {.byte = {0x00, 0x3f, 0x50, 0x6b, 0xd4, 0x1f, 0x45, 0xef,
                                                       0xf1, 0x34, 0x2c, 0x3d, 0x88, 0xdf, 0x73, 0x35,
                                                       0x07, 0xbf, 0xb1, 0x3b, 0xbd, 0xc0, 0x52, 0x16,
                                                       0x7b, 0x93, 0x7e, 0xec, 0x51, 0x39, 0x19, 0x56,
                                                       0xe1, 0x09, 0xf1, 0x8e, 0x91, 0x89, 0xb4, 0xb8,
                                                       0xf3, 0x15, 0xb3, 0x99, 0x5b, 0x72, 0xda, 0xa2,
                                                       0xee, 0x40, 0x85, 0xb6, 0xa0, 0x21, 0x9a, 0x92,
                                                       0x1f, 0x9a, 0x1c, 0x8e, 0x61, 0xb9, 0x3e, 0x95,
                                                       0x51, 0x00}};

const PKA_EccParam521 NISTP521_order       = {.byte = {0x09, 0x64, 0x38, 0x91, 0x1e, 0xb7, 0x6f, 0xbb,
                                                       0xae, 0x47, 0x9c, 0x89, 0xb8, 0xc9, 0xb5, 0x3b,
                                                       0xd0, 0xa5, 0x09, 0xf7, 0x48, 0x01, 0xcc, 0x7f,
                                                       0x6b, 0x96, 0x2f, 0xbf, 0x83, 0x87, 0x86, 0x51,
                                                       0xfa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                       0xff, 0x01}};


//*****************************************************************************
//
// Brainpool P256r1 constants in little endian format. byte[0] is the least
// significant byte and byte[BrainpoolP256R1_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const PKA_EccPoint256 BrainpoolP256R1_generator = {
    .x = {.byte = {0x62, 0x32, 0xCE, 0x9A, 0xBD, 0x53, 0x44, 0x3A,
                   0xC2, 0x23, 0xBD, 0xE3, 0xE1, 0x27, 0xDE, 0xB9,
                   0xAF, 0xB7, 0x81, 0xFC, 0x2F, 0x48, 0x4B, 0x2C,
                   0xCB, 0x57, 0x7E, 0xCB, 0xB9, 0xAE, 0xD2, 0x8B}},
    .y = {.byte = {0x97, 0x69, 0x04, 0x2F, 0xC7, 0x54, 0x1D, 0x5C,
                   0x54, 0x8E, 0xED, 0x2D, 0x13, 0x45, 0x77, 0xC2,
                   0xC9, 0x1D, 0x61, 0x14, 0x1A, 0x46, 0xF8, 0x97,
                   0xFD, 0xC4, 0xDA, 0xC3, 0x35, 0xF8, 0x7E, 0x54}},
};

const PKA_EccParam256 BrainpoolP256R1_prime       = {.byte = {0x77, 0x53, 0x6E, 0x1F, 0x1D, 0x48, 0x13, 0x20,
                                                              0x28, 0x20, 0x26, 0xD5, 0x23, 0xF6, 0x3B, 0x6E,
                                                              0x72, 0x8D, 0x83, 0x9D, 0x90, 0x0A, 0x66, 0x3E,
                                                              0xBC, 0xA9, 0xEE, 0xA1, 0xDB, 0x57, 0xFB, 0xA9}};

const PKA_EccParam256 BrainpoolP256R1_a           = {.byte = {0xD9, 0xB5, 0x30, 0xF3, 0x44, 0x4B, 0x4A, 0xE9,
                                                              0x6C, 0x5C, 0xDC, 0x26, 0xC1, 0x55, 0x80, 0xFB,
                                                              0xE7, 0xFF, 0x7A, 0x41, 0x30, 0x75, 0xF6, 0xEE,
                                                              0x57, 0x30, 0x2C, 0xFC, 0x75, 0x09, 0x5A, 0x7D}};

const PKA_EccParam256 BrainpoolP256R1_b           = {.byte = {0xB6, 0x07, 0x8C, 0xFF, 0x18, 0xDC, 0xCC, 0x6B,
                                                              0xCE, 0xE1, 0xF7, 0x5C, 0x29, 0x16, 0x84, 0x95,
                                                              0xBF, 0x7C, 0xD7, 0xBB, 0xD9, 0xB5, 0x30, 0xF3,
                                                              0x44, 0x4B, 0x4A, 0xE9, 0x6C, 0x5C, 0xDC, 0x26,}};

const PKA_EccParam256 BrainpoolP256R1_order       = {.byte = {0xA7, 0x56, 0x48, 0x97, 0x82, 0x0E, 0x1E, 0x90,
                                                              0xF7, 0xA6, 0x61, 0xB5, 0xA3, 0x7A, 0x39, 0x8C,
                                                              0x71, 0x8D, 0x83, 0x9D, 0x90, 0x0A, 0x66, 0x3E,
                                                              0xBC, 0xA9, 0xEE, 0xA1, 0xDB, 0x57, 0xFB, 0xA9}};

//*****************************************************************************
//
// Brainpool P384r1 constants in little endian format. byte[0] is the least
// significant byte and byte[BrainpoolP384R1_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const PKA_EccPoint384 BrainpoolP384R1_generator = {
    .x = {.byte = {0x1E, 0xAF, 0xD4, 0x47, 0xE2, 0xB2, 0x87, 0xEF,
                   0xAA, 0x46, 0xD6, 0x36, 0x34, 0xE0, 0x26, 0xE8,
                   0xE8, 0x10, 0xBD, 0x0C, 0xFE, 0xCA, 0x7F, 0xDB,
                   0xE3, 0x4F, 0xF1, 0x7E, 0xE7, 0xA3, 0x47, 0x88,
                   0x6B, 0x3F, 0xC1, 0xB7, 0x81, 0x3A, 0xA6, 0xA2,
                   0xFF, 0x45, 0xCF, 0x68, 0xF0, 0x64, 0x1C, 0x1D}},
    .y = {.byte = {0x15, 0x53, 0x3C, 0x26, 0x41, 0x03, 0x82, 0x42,
                   0x11, 0x81, 0x91, 0x77, 0x21, 0x46, 0x46, 0x0E,
                   0x28, 0x29, 0x91, 0xF9, 0x4F, 0x05, 0x9C, 0xE1,
                   0x64, 0x58, 0xEC, 0xFE, 0x29, 0x0B, 0xB7, 0x62,
                   0x52, 0xD5, 0xCF, 0x95, 0x8E, 0xEB, 0xB1, 0x5C,
                   0xA4, 0xC2, 0xF9, 0x20, 0x75, 0x1D, 0xBE, 0x8A}},
};

const PKA_EccParam384 BrainpoolP384R1_prime       = {.byte = {0x53, 0xEC, 0x07, 0x31, 0x13, 0x00, 0x47, 0x87,
                                                              0x71, 0x1A, 0x1D, 0x90, 0x29, 0xA7, 0xD3, 0xAC,
                                                              0x23, 0x11, 0xB7, 0x7F, 0x19, 0xDA, 0xB1, 0x12,
                                                              0xB4, 0x56, 0x54, 0xED, 0x09, 0x71, 0x2F, 0x15,
                                                              0xDF, 0x41, 0xE6, 0x50, 0x7E, 0x6F, 0x5D, 0x0F,
                                                              0x28, 0x6D, 0x38, 0xA3, 0x82, 0x1E, 0xB9, 0x8C}};

const PKA_EccParam384 BrainpoolP384R1_a           = {.byte = {0x26, 0x28, 0xCE, 0x22, 0xDD, 0xC7, 0xA8, 0x04,
                                                              0xEB, 0xD4, 0x3A, 0x50, 0x4A, 0x81, 0xA5, 0x8A,
                                                              0x0F, 0xF9, 0x91, 0xBA, 0xEF, 0x65, 0x91, 0x13,
                                                              0x87, 0x27, 0xB2, 0x4F, 0x8E, 0xA2, 0xBE, 0xC2,
                                                              0xA0, 0xAF, 0x05, 0xCE, 0x0A, 0x08, 0x72, 0x3C,
                                                              0x0C, 0x15, 0x8C, 0x3D, 0xC6, 0x82, 0xC3, 0x7B}};

const PKA_EccParam384 BrainpoolP384R1_b           = {.byte = {0x11, 0x4C, 0x50, 0xFA, 0x96, 0x86, 0xB7, 0x3A,
                                                              0x94, 0xC9, 0xDB, 0x95, 0x02, 0x39, 0xB4, 0x7C,
                                                              0xD5, 0x62, 0xEB, 0x3E, 0xA5, 0x0E, 0x88, 0x2E,
                                                              0xA6, 0xD2, 0xDC, 0x07, 0xE1, 0x7D, 0xB7, 0x2F,
                                                              0x7C, 0x44, 0xF0, 0x16, 0x54, 0xB5, 0x39, 0x8B,
                                                              0x26, 0x28, 0xCE, 0x22, 0xDD, 0xC7, 0xA8, 0x04}};

const PKA_EccParam384 BrainpoolP384R1_order       = {.byte = {0x65, 0x65, 0x04, 0xE9, 0x02, 0x32, 0x88, 0x3B,
                                                              0x10, 0xC3, 0x7F, 0x6B, 0xAF, 0xB6, 0x3A, 0xCF,
                                                              0xA7, 0x25, 0x04, 0xAC, 0x6C, 0x6E, 0x16, 0x1F,
                                                              0xB3, 0x56, 0x54, 0xED, 0x09, 0x71, 0x2F, 0x15,
                                                              0xDF, 0x41, 0xE6, 0x50, 0x7E, 0x6F, 0x5D, 0x0F,
                                                              0x28, 0x6D, 0x38, 0xA3, 0x82, 0x1E, 0xB9, 0x8C}};

//*****************************************************************************
//
// Brainpool P512r1 constants in little endian format. byte[0] is the least
// significant byte and byte[BrainpoolP512R1_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const PKA_EccPoint512 BrainpoolP512R1_generator = {
    .x = {.byte = {0x22, 0xF8, 0xB9, 0xBC, 0x09, 0x22, 0x35, 0x8B,
                   0x68, 0x5E, 0x6A, 0x40, 0x47, 0x50, 0x6D, 0x7C,
                   0x5F, 0x7D, 0xB9, 0x93, 0x7B, 0x68, 0xD1, 0x50,
                   0x8D, 0xD4, 0xD0, 0xE2, 0x78, 0x1F, 0x3B, 0xFF,
                   0x8E, 0x09, 0xD0, 0xF4, 0xEE, 0x62, 0x3B, 0xB4,
                   0xC1, 0x16, 0xD9, 0xB5, 0x70, 0x9F, 0xED, 0x85,
                   0x93, 0x6A, 0x4C, 0x9C, 0x2E, 0x32, 0x21, 0x5A,
                   0x64, 0xD9, 0x2E, 0xD8, 0xBD, 0xE4, 0xAE, 0x81}},
    .y = {.byte = {0x92, 0x08, 0xD8, 0x3A, 0x0F, 0x1E, 0xCD, 0x78,
                   0x06, 0x54, 0xF0, 0xA8, 0x2F, 0x2B, 0xCA, 0xD1,
                   0xAE, 0x63, 0x27, 0x8A, 0xD8, 0x4B, 0xCA, 0x5B,
                   0x5E, 0x48, 0x5F, 0x4A, 0x49, 0xDE, 0xDC, 0xB2,
                   0x11, 0x81, 0x1F, 0x88, 0x5B, 0xC5, 0x00, 0xA0,
                   0x1A, 0x7B, 0xA5, 0x24, 0x00, 0xF7, 0x09, 0xF2,
                   0xFD, 0x22, 0x78, 0xCF, 0xA9, 0xBF, 0xEA, 0xC0,
                   0xEC, 0x32, 0x63, 0x56, 0x5D, 0x38, 0xDE, 0x7D}},
};

const PKA_EccParam512 BrainpoolP512R1_prime       = {.byte = {0xF3, 0x48, 0x3A, 0x58, 0x56, 0x60, 0xAA, 0x28,
                                                              0x85, 0xC6, 0x82, 0x2D, 0x2F, 0xFF, 0x81, 0x28,
                                                              0xE6, 0x80, 0xA3, 0xE6, 0x2A, 0xA1, 0xCD, 0xAE,
                                                              0x42, 0x68, 0xC6, 0x9B, 0x00, 0x9B, 0x4D, 0x7D,
                                                              0x71, 0x08, 0x33, 0x70, 0xCA, 0x9C, 0x63, 0xD6,
                                                              0x0E, 0xD2, 0xC9, 0xB3, 0xB3, 0x8D, 0x30, 0xCB,
                                                              0x07, 0xFC, 0xC9, 0x33, 0xAE, 0xE6, 0xD4, 0x3F,
                                                              0x8B, 0xC4, 0xE9, 0xDB, 0xB8, 0x9D, 0xDD, 0xAA}};

const PKA_EccParam512 BrainpoolP512R1_a           = {.byte = {0xCA, 0x94, 0xFC, 0x77, 0x4D, 0xAC, 0xC1, 0xE7,
                                                              0xB9, 0xC7, 0xF2, 0x2B, 0xA7, 0x17, 0x11, 0x7F,
                                                              0xB5, 0xC8, 0x9A, 0x8B, 0xC9, 0xF1, 0x2E, 0x0A,
                                                              0xA1, 0x3A, 0x25, 0xA8, 0x5A, 0x5D, 0xED, 0x2D,
                                                              0xBC, 0x63, 0x98, 0xEA, 0xCA, 0x41, 0x34, 0xA8,
                                                              0x10, 0x16, 0xF9, 0x3D, 0x8D, 0xDD, 0xCB, 0x94,
                                                              0xC5, 0x4C, 0x23, 0xAC, 0x45, 0x71, 0x32, 0xE2,
                                                              0x89, 0x3B, 0x60, 0x8B, 0x31, 0xA3, 0x30, 0x78}};

const PKA_EccParam512 BrainpoolP512R1_b           = {.byte = {0x23, 0xF7, 0x16, 0x80, 0x63, 0xBD, 0x09, 0x28,
                                                              0xDD, 0xE5, 0xBA, 0x5E, 0xB7, 0x50, 0x40, 0x98,
                                                              0x67, 0x3E, 0x08, 0xDC, 0xCA, 0x94, 0xFC, 0x77,
                                                              0x4D, 0xAC, 0xC1, 0xE7, 0xB9, 0xC7, 0xF2, 0x2B,
                                                              0xA7, 0x17, 0x11, 0x7F, 0xB5, 0xC8, 0x9A, 0x8B,
                                                              0xC9, 0xF1, 0x2E, 0x0A, 0xA1, 0x3A, 0x25, 0xA8,
                                                              0x5A, 0x5D, 0xED, 0x2D, 0xBC, 0x63, 0x98, 0xEA,
                                                              0xCA, 0x41, 0x34, 0xA8, 0x10, 0x16, 0xF9, 0x3D}};

const PKA_EccParam512 BrainpoolP512R1_order       = {.byte = {0x69, 0x00, 0xA9, 0x9C, 0x82, 0x96, 0x87, 0xB5,
                                                              0xDD, 0xDA, 0x5D, 0x08, 0x81, 0xD3, 0xB1, 0x1D,
                                                              0x47, 0x10, 0xAC, 0x7F, 0x19, 0x61, 0x86, 0x41,
                                                              0x19, 0x26, 0xA9, 0x4C, 0x41, 0x5C, 0x3E, 0x55,
                                                              0x70, 0x08, 0x33, 0x70, 0xCA, 0x9C, 0x63, 0xD6,
                                                              0x0E, 0xD2, 0xC9, 0xB3, 0xB3, 0x8D, 0x30, 0xCB,
                                                              0x07, 0xFC, 0xC9, 0x33, 0xAE, 0xE6, 0xD4, 0x3F,
                                                              0x8B, 0xC4, 0xE9, 0xDB, 0xB8, 0x9D, 0xDD, 0xAA}};

//*****************************************************************************
//
// Curve25519 constants in little endian format. byte[0] is the least
// significant byte and byte[Curve25519_PARAM_SIZE_BYTES - 1] is the most
// significant.
//
//*****************************************************************************
const PKA_EccPoint256 Curve25519_generator = {
    .x = {.byte = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}},
    .y = {.byte = {0xd9, 0xd3, 0xce, 0x7e, 0xa2, 0xc5, 0xe9, 0x29,
                   0xb2, 0x61, 0x7c, 0x6d, 0x7e, 0x4d, 0x3d, 0x92,
                   0x4c, 0xd1, 0x48, 0x77, 0x2c, 0xdd, 0x1e, 0xe0,
                   0xb4, 0x86, 0xa0, 0xb8, 0xa1, 0x19, 0xae, 0x20}},
};

const PKA_EccParam256 Curve25519_prime       = {.byte = {0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f}};

const PKA_EccParam256 Curve25519_a           = {.byte = {0x06, 0x6d, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}};

const PKA_EccParam256 Curve25519_b           = {.byte = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}};

const PKA_EccParam256 Curve25519_order       = {.byte = {0xb9, 0xdc, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58,
                                                         0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14,
                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,}};

//*****************************************************************************
//
// Write a PKA parameter to the PKA module, set required registers, and add an offset.
//
//*****************************************************************************
static uint32_t PKAWritePkaParam(const uint8_t *param, uint32_t paramLength, uint32_t paramOffset, uint32_t ptrRegOffset)
{
    uint32_t i;
    uint32_t *paramWordAlias = (uint32_t *)param;
    // Take the floor of paramLength in 32-bit words
    uint32_t paramLengthInWords = paramLength / sizeof(uint32_t);

    // Only copy data if it is specified. We may wish to simply allocate another buffer and get
    // the required offset.
    if (param) {
        // Load the number in PKA RAM
        for (i = 0; i < paramLengthInWords; i++) {
            HWREG(PKA_RAM_BASE + paramOffset + sizeof(uint32_t) * i) = paramWordAlias[i];
        }

        // If the length is not a word-multiple, fill up a temporary word and copy that in
        // to avoid a bus error. The extra zeros at the end should not matter, as the large
        // number is little-endian and thus has no effect.
        // We could have correctly calculated ceiling(paramLength / sizeof(uint32_t)) above.
        // However, we would not have been able to zero-out the extra few most significant
        // bytes of the most significant word. That would have resulted in doing maths operations
        // on whatever follows param in RAM.
        if (paramLength % sizeof(uint32_t)) {
            uint32_t temp = 0;
            uint8_t j;

            // Load the entire word line of the param remainder
            temp = paramWordAlias[i];

            // Zero-out all bytes beyond the end of the param
            for (j = paramLength % sizeof(uint32_t); j < sizeof(uint32_t); j++) {
                ((uint8_t *)&temp)[j] = 0;
            }

            HWREG(PKA_RAM_BASE + paramOffset + sizeof(uint32_t) * i) = temp;

            // Increment paramLengthInWords since we take the ceiling of length / sizeof(uint32_t)
            paramLengthInWords++;
        }
    }

    // Update the A, B, C, or D pointer with the offset address of the PKA RAM location
    // where the number will be stored.
    switch (ptrRegOffset) {
        case PKA_O_APTR:
            HWREG(PKA_BASE + PKA_O_APTR) = paramOffset >> 2;
            HWREG(PKA_BASE + PKA_O_ALENGTH) = paramLengthInWords;
            break;
        case PKA_O_BPTR:
            HWREG(PKA_BASE + PKA_O_BPTR) = paramOffset >> 2;
            HWREG(PKA_BASE + PKA_O_BLENGTH) = paramLengthInWords;
            break;
        case PKA_O_CPTR:
            HWREG(PKA_BASE + PKA_O_CPTR) = paramOffset >> 2;
            break;
        case PKA_O_DPTR:
            HWREG(PKA_BASE + PKA_O_DPTR) = paramOffset >> 2;
            break;
    }

    // Ensure 8-byte alignment of next parameter.
    // Returns the offset for the next parameter.
    return paramOffset + sizeof(uint32_t) * (paramLengthInWords + (paramLengthInWords % 2));
}

//*****************************************************************************
//
// Write a PKA parameter to the PKA module but return a larger offset.
//
//*****************************************************************************
static uint32_t PKAWritePkaParamExtraOffset(const uint8_t *param, uint32_t paramLength, uint32_t paramOffset, uint32_t ptrRegOffset)
{
    // Ensure 16-byte alignment.
    return  (sizeof(uint32_t) * 2) + PKAWritePkaParam(param, paramLength, paramOffset, ptrRegOffset);
}

//*****************************************************************************
//
// Writes the result of a large number arithmetic operation to a provided buffer.
//
//*****************************************************************************
static uint32_t PKAGetBigNumResult(uint8_t *resultBuf, uint32_t *resultLength, uint32_t resultPKAMemAddr)
{
    uint32_t mswOffset;
    uint32_t lswOffset;
    uint32_t lengthInWords;
    uint32_t i;
    uint32_t *resultWordAlias = (uint32_t *)resultBuf;

    // Check the arguments.
    ASSERT(resultBuf);
    ASSERT((resultPKAMemAddr > PKA_RAM_BASE) &&
           (resultPKAMemAddr < (PKA_RAM_BASE + PKA_RAM_TOT_BYTE_SIZE)));

    // Verify that the operation is complete.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    // Get the MSW register value.
    mswOffset = HWREG(PKA_BASE + PKA_O_MSW);

    // If the result vector is zero, write back one zero byte so the caller does not need
    // to handle a special error for the perhaps valid result of zero.
    // They will only get the error status if they do not provide a buffer
    if (mswOffset & PKA_MSW_RESULT_IS_ZERO_M) {
        if (*resultLength){
            if(resultBuf){
                resultBuf[0] = 0;
            }

            *resultLength = 1;

            return PKA_STATUS_SUCCESS;
        }
        else {
            return PKA_STATUS_BUF_UNDERFLOW;
        }
    }

    // Get the length of the result
    mswOffset = ((mswOffset & PKA_MSW_MSW_ADDRESS_M) + 1);
    lswOffset = ((resultPKAMemAddr - PKA_RAM_BASE) >> 2);

    if (mswOffset >= lswOffset) {
        lengthInWords = mswOffset - lswOffset;
    }
    else {
        return PKA_STATUS_RESULT_ADDRESS_INCORRECT;
    }

    // Check if the provided buffer length is adequate to store the result data.
    if (*resultLength < lengthInWords * sizeof(uint32_t)) {
        return PKA_STATUS_BUF_UNDERFLOW;
    }

    // Copy the resultant length.
    *resultLength = lengthInWords * sizeof(uint32_t);


    if (resultBuf) {
        // Copy the result into the resultBuf.
        for (i = 0; i < lengthInWords; i++) {
            resultWordAlias[i]= HWREG(resultPKAMemAddr + sizeof(uint32_t) * i);
        }
    }

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Retrieve the result of a modulo operation or the remainder of a division.
//
//*****************************************************************************
static uint32_t PKAGetBigNumResultRemainder(uint8_t *resultBuf, uint32_t *resultLength, uint32_t resultPKAMemAddr)
{
    uint32_t regMSWVal;
    uint32_t lengthInWords;
    uint32_t i;
    uint32_t *resultWordAlias = (uint32_t *)resultBuf;

    // Check the arguments.
    ASSERT(resultBuf);
    ASSERT((resultPKAMemAddr > PKA_RAM_BASE) &&
           (resultPKAMemAddr < (PKA_RAM_BASE + PKA_RAM_TOT_BYTE_SIZE)));

    // Verify that the operation is complete.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    // Get the MSW register value.
    regMSWVal = HWREG(PKA_BASE + PKA_O_DIVMSW);

    // If the result vector is zero, write back one zero byte so the caller does not need
    // to handle a special error for the perhaps valid result of zero.
    // They will only get the error status if they do not provide a buffer
    if (regMSWVal & PKA_DIVMSW_RESULT_IS_ZERO_M) {
        if (*resultLength){
            if(resultBuf){
                resultBuf[0] = 0;
            }

            *resultLength = 1;

            return PKA_STATUS_SUCCESS;
        }
        else {
            return PKA_STATUS_BUF_UNDERFLOW;
        }
    }

    // Get the length of the result
    lengthInWords = ((regMSWVal & PKA_DIVMSW_MSW_ADDRESS_M) + 1) - ((resultPKAMemAddr - PKA_RAM_BASE) >> 2);

    // Check if the provided buffer length is adequate to store the result data.
    if (*resultLength < lengthInWords * sizeof(uint32_t)) {
        return PKA_STATUS_BUF_UNDERFLOW;
    }

    // Copy the resultant length.
    *resultLength = lengthInWords * sizeof(uint32_t);

    if (resultBuf) {
        // Copy the result into the resultBuf.
        for (i = 0; i < lengthInWords; i++) {
            resultWordAlias[i] = HWREG(resultPKAMemAddr + sizeof(uint32_t) * i);
        }
    }

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Writes the resultant curve point of an ECC operation to the provided buffer.
//
//*****************************************************************************
static uint32_t PKAGetECCResult(uint8_t *curvePointX, uint8_t *curvePointY, uint32_t resultPKAMemAddr, uint32_t length)
{
    uint32_t i = 0;
    uint32_t *xWordAlias = (uint32_t *)curvePointX;
    uint32_t *yWordAlias = (uint32_t *)curvePointY;
    uint32_t lengthInWordsCeiling = 0;

    // Check for the arguments.
    ASSERT(curvePointX);
    ASSERT(curvePointY);
    ASSERT((resultPKAMemAddr > PKA_RAM_BASE) &&
           (resultPKAMemAddr < (PKA_RAM_BASE + PKA_RAM_TOT_BYTE_SIZE)));

    // Verify that the operation is completed.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    if (HWREG(PKA_BASE + PKA_O_SHIFT)) {
         return PKA_STATUS_FAILURE;
    }

    // Check to make sure that the result vector is not the point at infinity.
    if (HWREG(PKA_BASE + PKA_O_MSW) & PKA_MSW_RESULT_IS_ZERO) {
        return PKA_STATUS_POINT_AT_INFINITY;
    }

    if (curvePointX != NULL) {
        // Copy the x co-ordinate value of the result from vector D into
        // the curvePoint.
        for (i = 0; i < (length / sizeof(uint32_t)); i++) {
            xWordAlias[i] = HWREG(resultPKAMemAddr + sizeof(uint32_t) * i);
        }

        // If the length is not a word-multiple, fill up a temporary word and copy that in
        // to avoid a bus error.
        if (length % sizeof(uint32_t)) {
            uint32_t temp = 0;
            uint8_t j;

            // Load the entire word line of the coordinate remainder
            temp = HWREG(resultPKAMemAddr + sizeof(uint32_t) * i);

            // Write all remaining bytes to the coordinate
            for (j = 0; j < length % sizeof(uint32_t); j++) {
                curvePointX[i * sizeof(uint32_t) + j] = ((uint8_t *)&temp)[j];
            }

        }
    }

    lengthInWordsCeiling = (length % sizeof(uint32_t)) ? length / sizeof(uint32_t) + 1 : length / sizeof(uint32_t);

    resultPKAMemAddr += sizeof(uint32_t) * (2 + lengthInWordsCeiling + (lengthInWordsCeiling % 2));

    if (curvePointY != NULL) {
        // Copy the y co-ordinate value of the result from vector D into
        // the curvePoint.
        for (i = 0; i < (length / sizeof(uint32_t)); i++) {
            yWordAlias[i] = HWREG(resultPKAMemAddr + sizeof(uint32_t) * i);
        }

        // If the length is not a word-multiple, fill up a temporary word and copy that in
        // to avoid a bus error.
        if (length % sizeof(uint32_t)) {
            uint32_t temp = 0;
            uint8_t j;

            // Load the entire word line of the coordinate remainder
            temp = HWREG(resultPKAMemAddr + sizeof(uint32_t) * i);

            // Write all remaining bytes to the coordinate
            for (j = 0; j < length % sizeof(uint32_t); j++) {
                curvePointY[i * sizeof(uint32_t) + j] = ((uint8_t *)&temp)[j];
            }
        }
    }


    return PKA_STATUS_SUCCESS;
}


//*****************************************************************************
//
// Provides the PKA operation status.
//
//*****************************************************************************
uint32_t PKAGetOpsStatus(void)
{
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN_M) {
        return PKA_STATUS_OPERATION_BUSY;
    }
    else {
        return PKA_STATUS_OPERATION_RDY;
    }
}

//*****************************************************************************
//
// Check if an array consists only of zeros.
//
//*****************************************************************************
bool PKAArrayAllZeros(const uint8_t *array, uint32_t arrayLength)
{
    uint32_t i;
    uint8_t arrayBits = 0;

    // We could speed things up by comparing word-wise rather than byte-wise.
    // However, this extra overhead is inconsequential compared to running an
    // actual PKA operation. Especially ECC operations.
    for (i = 0; i < arrayLength; i++) {
        arrayBits |= array[i];
    }

    if (arrayBits) {
        return false;
    }
    else {
        return true;
    }

}

//*****************************************************************************
//
// Fill an array with zeros
//
//*****************************************************************************
void PKAZeroOutArray(const uint8_t *array, uint32_t arrayLength)
{
    uint32_t i;
    // Take the floor of paramLength in 32-bit words
    uint32_t arrayLengthInWords = arrayLength / sizeof(uint32_t);

    // Zero-out the array word-wise until i >= arrayLength
    for (i = 0; i < arrayLengthInWords * sizeof(uint32_t); i += 4) {
        HWREG(array + i) = 0;
    }

    // If i != arrayLength, there are some remaining bytes to zero-out
    if (arrayLength % sizeof(uint32_t)) {
        // Subtract 4 from i, since i has already overshot the array
        for (i -= 4; i < arrayLength; i++) {
            HWREGB(array + i * sizeof(uint32_t));
        }
    }
}

//*****************************************************************************
//
// Start the big number modulus operation.
//
//*****************************************************************************
uint32_t PKABigNumModStart(const uint8_t *bigNum, uint32_t bigNumLength, const uint8_t *modulus, uint32_t modulusLength, uint32_t *resultPKAMemAddr)
{
    uint32_t offset = 0;

    // Check the arguments.
    ASSERT(bigNum);
    ASSERT(modulus);
    ASSERT(resultPKAMemAddr);

    // Make sure no operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(bigNum, bigNumLength, offset, PKA_O_APTR);

    offset = PKAWritePkaParamExtraOffset(modulus, modulusLength, offset, PKA_O_BPTR);

    // Copy the result vector address location.
    *resultPKAMemAddr = PKA_RAM_BASE + offset;

    // Load C pointer with the result location in PKA RAM
    HWREG(PKA_BASE + PKA_O_CPTR) = offset >> 2;

    // Start the PKCP modulo operation by setting the PKA Function register.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = (PKA_FUNCTION_RUN | PKA_FUNCTION_MODULO);

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Get the result of the big number modulus operation.
//
//*****************************************************************************
uint32_t PKABigNumModGetResult(uint8_t *resultBuf, uint32_t length, uint32_t resultPKAMemAddr)
{
    // Zero-out array in case modulo result is shorter than length
    PKAZeroOutArray(resultBuf, length);

    return PKAGetBigNumResultRemainder(resultBuf, &length, resultPKAMemAddr);
}

//*****************************************************************************
//
// Start the big number divide operation.
//
//*****************************************************************************
uint32_t PKABigNumDivideStart(const uint8_t *dividend, uint32_t dividendLength, const uint8_t *divisor, uint32_t divisorLength, uint32_t *resultQuotientMemAddr, uint32_t *resultRemainderMemAddr)
{
    uint32_t offset = 0;

    // Check the arguments.
    ASSERT(dividend);
    ASSERT(divisor);
    ASSERT(resultQuotientMemAddr);
    ASSERT(resultRemainderMemAddr);

    // Make sure no operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(dividend, dividendLength, offset, PKA_O_APTR);

    offset = PKAWritePkaParamExtraOffset(divisor, divisorLength, offset, PKA_O_BPTR);

    // Copy the remainder result vector address location.
    if (resultRemainderMemAddr) {
        *resultRemainderMemAddr = PKA_RAM_BASE + offset;
    }

    // The remainder cannot ever be larger than the divisor. It should fit inside
    // a buffer of that size.
    offset = PKAWritePkaParamExtraOffset(0, divisorLength, offset, PKA_O_CPTR);

    // Copy the remainder result vector address location.
    if (resultQuotientMemAddr) {
        *resultQuotientMemAddr = PKA_RAM_BASE + offset;
    }

    // Load D pointer with the quotient location in PKA RAM
    HWREG(PKA_BASE + PKA_O_DPTR) = offset >> 2;

    // Start the PKCP modulo operation by setting the PKA Function register.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = (PKA_FUNCTION_RUN | PKA_FUNCTION_DIVIDE);

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Get the quotient of the big number divide operation.
//
//*****************************************************************************
uint32_t PKABigNumDivideGetQuotient(uint8_t *resultBuf, uint32_t *length, uint32_t resultQuotientMemAddr)
{
    return PKAGetBigNumResult(resultBuf, length, resultQuotientMemAddr);
}

//*****************************************************************************
//
// Get the remainder of the big number divide operation.
//
//*****************************************************************************
uint32_t PKABigNumDivideGetRemainder(uint8_t *resultBuf, uint32_t *length, uint32_t resultQuotientMemAddr)
{
    return PKAGetBigNumResultRemainder(resultBuf, length, resultQuotientMemAddr);
}


//*****************************************************************************
//
// Start the comparison of two big numbers.
//
//*****************************************************************************
uint32_t PKABigNumCmpStart(const uint8_t *bigNum1, const uint8_t *bigNum2, uint32_t length)
{
    uint32_t offset = 0;

    // Check the arguments.
    ASSERT(bigNum1);
    ASSERT(bigNum2);

    // Make sure no operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(bigNum1, length, offset, PKA_O_APTR);

    offset = PKAWritePkaParam(bigNum2, length, offset, PKA_O_BPTR);

    // Set the PKA Function register for the Compare operation
    // and start the operation.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = (PKA_FUNCTION_RUN | PKA_FUNCTION_COMPARE);

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Get the result of the comparison operation of two big numbers.
//
//*****************************************************************************
uint32_t PKABigNumCmpGetResult(void)
{
    uint32_t  status;

    // verify that the operation is complete.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    // Check the COMPARE register.
    switch(HWREG(PKA_BASE + PKA_O_COMPARE)) {
        case PKA_COMPARE_A_EQUALS_B:
            status = PKA_STATUS_EQUAL;
            break;

        case PKA_COMPARE_A_GREATER_THAN_B:
            status = PKA_STATUS_A_GREATER_THAN_B;
            break;

        case PKA_COMPARE_A_LESS_THAN_B:
            status = PKA_STATUS_A_LESS_THAN_B;
            break;

        default:
            status = PKA_STATUS_FAILURE;
            break;
    }

    return status;
}

//*****************************************************************************
//
// Start the big number inverse modulo operation.
//
//*****************************************************************************
uint32_t PKABigNumInvModStart(const uint8_t *bigNum, uint32_t bigNumLength, const uint8_t *modulus, uint32_t modulusLength, uint32_t *resultPKAMemAddr)
{
    uint32_t offset = 0;

    // Check the arguments.
    ASSERT(bigNum);
    ASSERT(modulus);
    ASSERT(resultPKAMemAddr);

    // Make sure no operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(bigNum, bigNumLength, offset, PKA_O_APTR);

    offset = PKAWritePkaParam(modulus, modulusLength, offset, PKA_O_BPTR);

    // Copy the result vector address location.
    *resultPKAMemAddr = PKA_RAM_BASE + offset;

    // Load D pointer with the result location in PKA RAM.
    HWREG(PKA_BASE + PKA_O_DPTR) = offset >> 2;

    // set the PKA function to InvMod operation and the start the operation.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = 0x0000F000;

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Get the result of the big number inverse modulo operation.
//
//*****************************************************************************
uint32_t PKABigNumInvModGetResult(uint8_t *resultBuf, uint32_t length, uint32_t resultPKAMemAddr)
{
    // Zero-out array in case modulo result is shorter than length
    PKAZeroOutArray(resultBuf, length);

    return PKAGetBigNumResult(resultBuf, &length, resultPKAMemAddr);
}

//*****************************************************************************
//
// Start the big number multiplication.
//
//*****************************************************************************
uint32_t PKABigNumMultiplyStart(const uint8_t *multiplicand, uint32_t multiplicandLength, const uint8_t *multiplier, uint32_t multiplierLength, uint32_t *resultPKAMemAddr)
{
    uint32_t offset = 0;

    // Check for the arguments.
    ASSERT(multiplicand);
    ASSERT(multiplier);
    ASSERT(resultPKAMemAddr);

    // Make sure no operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(multiplicand, multiplicandLength, offset, PKA_O_APTR);

    offset = PKAWritePkaParam(multiplier, multiplierLength, offset, PKA_O_BPTR);


    // Copy the result vector address location.
    *resultPKAMemAddr = PKA_RAM_BASE + offset;

    // Load C pointer with the result location in PKA RAM.
    HWREG(PKA_BASE + PKA_O_CPTR) = offset >> 2;

    // Set the PKA function to the multiplication and start it.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = (PKA_FUNCTION_RUN | PKA_FUNCTION_MULTIPLY);

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Get the results of the big number multiplication.
//
//*****************************************************************************
uint32_t PKABigNumMultGetResult(uint8_t *resultBuf, uint32_t *resultLength, uint32_t resultPKAMemAddr)
{
     return PKAGetBigNumResult(resultBuf, resultLength, resultPKAMemAddr);
}

//*****************************************************************************
//
// Start the addition of two big number.
//
//*****************************************************************************
uint32_t PKABigNumAddStart(const uint8_t *bigNum1, uint32_t bigNum1Length, const uint8_t *bigNum2, uint32_t bigNum2Length, uint32_t *resultPKAMemAddr)
{
    uint32_t offset = 0;

    // Check for arguments.
    ASSERT(bigNum1);
    ASSERT(bigNum2);
    ASSERT(resultPKAMemAddr);

    // Make sure no operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(bigNum1, bigNum1Length, offset, PKA_O_APTR);

    offset = PKAWritePkaParam(bigNum2, bigNum2Length, offset, PKA_O_BPTR);

    // Copy the result vector address location.
    *resultPKAMemAddr = PKA_RAM_BASE + offset;

    // Load C pointer with the result location in PKA RAM.
    HWREG(PKA_BASE + PKA_O_CPTR) = offset >> 2;

    // Set the function for the add operation and start the operation.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = (PKA_FUNCTION_RUN | PKA_FUNCTION_ADD);

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Get the result of the addition operation on two big number.
//
//*****************************************************************************
uint32_t PKABigNumSubGetResult(uint8_t *resultBuf, uint32_t *resultLength, uint32_t resultPKAMemAddr)
{
    return PKAGetBigNumResult(resultBuf, resultLength, resultPKAMemAddr);
}

//*****************************************************************************
//
// Start the addition of two big number.
//
//*****************************************************************************
uint32_t PKABigNumSubStart(const uint8_t *minuend, uint32_t minuendLength, const uint8_t *subtrahend, uint32_t subtrahendLength, uint32_t *resultPKAMemAddr)
{
    uint32_t offset = 0;

    // Check for arguments.
    ASSERT(minuend);
    ASSERT(subtrahend);
    ASSERT(resultPKAMemAddr);


    // Make sure no operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(minuend, minuendLength, offset, PKA_O_APTR);

    offset = PKAWritePkaParam(subtrahend, subtrahendLength, offset, PKA_O_BPTR);

    // Copy the result vector address location.
    *resultPKAMemAddr = PKA_RAM_BASE + offset;

    // Load C pointer with the result location in PKA RAM.
    HWREG(PKA_BASE + PKA_O_CPTR) = offset >> 2;

    // Set the function for the add operation and start the operation.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = (PKA_FUNCTION_RUN | PKA_FUNCTION_SUBTRACT);

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Get the result of the addition operation on two big number.
//
//*****************************************************************************
uint32_t PKABigNumAddGetResult(uint8_t *resultBuf, uint32_t *resultLength, uint32_t resultPKAMemAddr)
{
    return PKAGetBigNumResult(resultBuf, resultLength, resultPKAMemAddr);
}


//*****************************************************************************
//
// Start ECC Multiplication.
//
//*****************************************************************************
uint32_t PKAEccMultiplyStart(const uint8_t *scalar, const uint8_t *curvePointX, const uint8_t *curvePointY, const uint8_t *prime, const uint8_t *a, const uint8_t *b, uint32_t length, uint32_t *resultPKAMemAddr)
{
    uint32_t offset = 0;

    // Check for the arguments.
    ASSERT(scalar);
    ASSERT(curvePointX);
    ASSERT(curvePointY);
    ASSERT(prime);
    ASSERT(a);
    ASSERT(b);
    ASSERT(length <= PKA_MAX_CURVE_SIZE_32_BIT_WORD * sizeof(uint32_t));
    ASSERT(resultPKAMemAddr);

    // Make sure no PKA operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(scalar, length, offset, PKA_O_APTR);

    offset = PKAWritePkaParamExtraOffset(prime, length, offset, PKA_O_BPTR);
    offset = PKAWritePkaParamExtraOffset(a, length, offset, PKA_NO_POINTER_REG);
    offset = PKAWritePkaParamExtraOffset(b, length, offset, PKA_NO_POINTER_REG);

    offset = PKAWritePkaParamExtraOffset(curvePointX, length, offset, PKA_O_CPTR);
    offset = PKAWritePkaParamExtraOffset(curvePointY, length, offset, PKA_NO_POINTER_REG);

    // Update the result location.
    // The resultPKAMemAddr may be 0 if we only want to check that we generated the point at infinity
    if (resultPKAMemAddr) {
        *resultPKAMemAddr =  PKA_RAM_BASE + offset;
    }

    // Load D pointer with the result location in PKA RAM.
    HWREG(PKA_BASE + PKA_O_DPTR) = offset >> 2;

    // Set the PKA function to ECC-MULT and start the operation.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = PKA_FUNCTION_RUN_M | (0x05 << PKA_FUNCTION_SEQUENCER_OPERATIONS_S);

    return PKA_STATUS_SUCCESS;
}


//*****************************************************************************
//
// Start ECC Montgomery Multiplication.
//
//*****************************************************************************
uint32_t PKAEccMontgomeryMultiplyStart(const uint8_t *scalar, const uint8_t *curvePointX, const uint8_t *prime, const uint8_t *a, uint32_t length, uint32_t *resultPKAMemAddr)
{
    uint32_t offset = 0;

    // Check for the arguments.
    ASSERT(scalar);
    ASSERT(curvePointX);
    ASSERT(prime);
    ASSERT(a);
    ASSERT(length <= PKA_MAX_CURVE_SIZE_32_BIT_WORD * sizeof(uint32_t));
    ASSERT(resultPKAMemAddr);

    // Make sure no PKA operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParam(scalar, length, offset, PKA_O_APTR);

    offset = PKAWritePkaParamExtraOffset(prime, length, offset, PKA_O_BPTR);
    offset = PKAWritePkaParamExtraOffset(a, length, offset, PKA_NO_POINTER_REG);

    offset = PKAWritePkaParamExtraOffset(curvePointX, length, offset, PKA_O_CPTR);

    // Update the result location.
    // The resultPKAMemAddr may be 0 if we only want to check that we generated the point at infinity
    if (resultPKAMemAddr) {
        *resultPKAMemAddr =  PKA_RAM_BASE + offset;
    }

    // Load D pointer with the result location in PKA RAM.
    HWREG(PKA_BASE + PKA_O_DPTR) = offset >> 2;

    // Set the PKA function to Montgomery ECC-MULT and start the operation.
    HWREG(PKA_BASE + PKA_O_FUNCTION) = PKA_FUNCTION_RUN_M | (0x02 << PKA_FUNCTION_SEQUENCER_OPERATIONS_S);

    return PKA_STATUS_SUCCESS;
}


//*****************************************************************************
//
// Get the result of ECC Multiplication
//
//*****************************************************************************
uint32_t PKAEccMultiplyGetResult(uint8_t *curvePointX, uint8_t *curvePointY, uint32_t resultPKAMemAddr, uint32_t length)
{
    return PKAGetECCResult(curvePointX, curvePointY, resultPKAMemAddr, length);
}

//*****************************************************************************
//
// Start the ECC Addition.
//
//*****************************************************************************
uint32_t PKAEccAddStart(const uint8_t *curvePoint1X, const uint8_t *curvePoint1Y, const uint8_t *curvePoint2X, const uint8_t *curvePoint2Y, const uint8_t *prime, const uint8_t *a, uint32_t length, uint32_t *resultPKAMemAddr)
{
    uint32_t offset = 0;

    // Check for the arguments.
    ASSERT(curvePoint1X);
    ASSERT(curvePoint1Y);
    ASSERT(curvePoint2X);
    ASSERT(curvePoint2Y);
    ASSERT(prime);
    ASSERT(a);
    ASSERT(resultPKAMemAddr);

    // Make sure no operation is in progress.
    if (HWREG(PKA_BASE + PKA_O_FUNCTION) & PKA_FUNCTION_RUN) {
        return PKA_STATUS_OPERATION_BUSY;
    }

    offset = PKAWritePkaParamExtraOffset(curvePoint1X, length, offset, PKA_O_APTR);
    offset = PKAWritePkaParamExtraOffset(curvePoint1Y, length, offset, PKA_NO_POINTER_REG);


    offset = PKAWritePkaParamExtraOffset(prime, length, offset, PKA_O_BPTR);
    offset = PKAWritePkaParamExtraOffset(a, length, offset, PKA_NO_POINTER_REG);

    offset = PKAWritePkaParamExtraOffset(curvePoint2X, length, offset, PKA_O_CPTR);
    offset = PKAWritePkaParamExtraOffset(curvePoint2Y, length, offset, PKA_NO_POINTER_REG);

    // Copy the result vector location.
    *resultPKAMemAddr = PKA_RAM_BASE + offset;

    // Load D pointer with the result location in PKA RAM.
    HWREG(PKA_BASE + PKA_O_DPTR) = offset >> 2;

    // Set the PKA Function to ECC-ADD and start the operation.
    HWREG(PKA_BASE + PKA_O_FUNCTION ) = PKA_FUNCTION_RUN_M | (0x03 << PKA_FUNCTION_SEQUENCER_OPERATIONS_S);

    return PKA_STATUS_SUCCESS;
}

//*****************************************************************************
//
// Get the result of the ECC Addition
//
//*****************************************************************************
uint32_t PKAEccAddGetResult(uint8_t *curvePointX, uint8_t *curvePointY, uint32_t resultPKAMemAddr, uint32_t length)
{
    return PKAGetECCResult(curvePointX, curvePointY, resultPKAMemAddr, length);
}

//*****************************************************************************
//
// Verify a public key against the supplied elliptic curve equation
//
//*****************************************************************************
uint32_t PKAEccVerifyPublicKeyWeierstrassStart(const uint8_t *curvePointX, const uint8_t *curvePointY, const uint8_t *prime, const uint8_t *a, const uint8_t *b, const uint8_t *order, uint32_t length)
{
    uint32_t pkaResult;
    uint32_t resultAddress;
    uint32_t resultLength;
    uint8_t *scratchBuffer = (uint8_t *)(PKA_RAM_BASE + PKA_RAM_TOT_BYTE_SIZE / 2);
    uint8_t *scratchBuffer2 = scratchBuffer + 512;


    // Verify X in range [0, prime - 1]
    PKABigNumCmpStart(curvePointX,
                      prime,
                      length);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    pkaResult = PKABigNumCmpGetResult();

    if (pkaResult != PKA_STATUS_A_LESS_THAN_B) {
        return PKA_STATUS_X_LARGER_THAN_PRIME;
    }

    // Verify Y in range [0, prime - 1]
    PKABigNumCmpStart(curvePointY,
                      prime,
                      length);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    pkaResult = PKABigNumCmpGetResult();

    if (pkaResult != PKA_STATUS_A_LESS_THAN_B) {
        return PKA_STATUS_Y_LARGER_THAN_PRIME;
    }

    // Verify point on curve
    // Short-Weierstrass equation: Y ^ 2 = X ^3 + a * X + b mod P
    // Reduced: Y ^ 2 = X * (X ^ 2 + a) + b

    // tmp = X ^ 2
    PKABigNumMultiplyStart(curvePointX, length, curvePointX, length, &resultAddress);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    resultLength = 200;
    pkaResult = PKABigNumMultGetResult(scratchBuffer, &resultLength, resultAddress);

    if (pkaResult != PKA_STATUS_SUCCESS) {
        return PKA_STATUS_FAILURE;
    }

    // tmp += a
    PKABigNumAddStart(scratchBuffer, resultLength, a, length, &resultAddress);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    resultLength = 200;
    pkaResult = PKABigNumAddGetResult(scratchBuffer, &resultLength, resultAddress);

    if (pkaResult != PKA_STATUS_SUCCESS) {
        return PKA_STATUS_FAILURE;
    }

    // tmp *= x
    PKABigNumMultiplyStart(scratchBuffer, resultLength, curvePointX, length, &resultAddress);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    resultLength = 200;
    pkaResult = PKABigNumMultGetResult(scratchBuffer, &resultLength, resultAddress);

    if (pkaResult != PKA_STATUS_SUCCESS) {
        return PKA_STATUS_FAILURE;
    }

    // tmp += b
     PKABigNumAddStart(scratchBuffer, resultLength, b, length, &resultAddress);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    resultLength = 200;
    pkaResult = PKABigNumAddGetResult(scratchBuffer, &resultLength, resultAddress);

    if (pkaResult != PKA_STATUS_SUCCESS) {
        return PKA_STATUS_FAILURE;
    }


    // tmp2 = tmp % prime to ensure we have no fraction in the division.
    // The number will only shrink from here on out.
    PKABigNumModStart(scratchBuffer, resultLength, prime, length, &resultAddress);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    // If the result is not a multiple of the word-length, the PKA HW will round up
    // because it deals in words only. That means that using 'length' directly
    // would cause and underflow, since length refers to the actual length in bytes of
    // the curve parameters while the PKA HW reports that rounded up to the next
    // word boundary.
    // Use 200 as the resultLength instead since we are copying to the scratch buffer
    // anyway.
    // Practically, this only happens with curves such as NIST-P521 that are not word
    // multiples.
    resultLength = 200;
    pkaResult = PKABigNumModGetResult(scratchBuffer2, resultLength, resultAddress);

    if (pkaResult != PKA_STATUS_SUCCESS) {
        return PKA_STATUS_FAILURE;
    }

    // tmp = y^2
    PKABigNumMultiplyStart(curvePointY, length, curvePointY, length, &resultAddress);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    resultLength = 200;
    pkaResult = PKABigNumMultGetResult(scratchBuffer, &resultLength, resultAddress);

    if (pkaResult != PKA_STATUS_SUCCESS) {
        return PKA_STATUS_FAILURE;
    }

    // tmp %= prime
    PKABigNumModStart(scratchBuffer, resultLength, prime, length, &resultAddress);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    // If the result is not a multiple of the word-length, the PKA HW will round up
    // because it deals in words only. That means that using 'length' directly
    // would cause and underflow, since length refers to the actual length in bytes of
    // the curve parameters while the PKA HW reports that rounded up to the next
    // word boundary.
    // Use 200 as the resultLength instead since we are copying to the scratch buffer
    // anyway.
    // Practically, this only happens with curves such as NIST-P521 that are not word
    // multiples.
    resultLength = 200;
    pkaResult = PKABigNumModGetResult(scratchBuffer, resultLength, resultAddress);

    if (pkaResult != PKA_STATUS_SUCCESS) {
        return PKA_STATUS_FAILURE;
    }

    // tmp ?= tmp2
    PKABigNumCmpStart(scratchBuffer,
                      scratchBuffer2,
                      length);

    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY);

    pkaResult = PKABigNumCmpGetResult();

    if (pkaResult != PKA_STATUS_EQUAL) {
        return PKA_STATUS_POINT_NOT_ON_CURVE;
    }
    else {
        return PKA_STATUS_SUCCESS;
    }
}


static uint32_t SHA2ExecuteHash(const uint8_t *message, uint8_t *resultDigest, uint32_t *intermediateDigest, uint32_t totalMsgLength, uint32_t messageLength, uint32_t hashAlgorithm, bool initialHash, bool finalHash);


void SHA2StartDMAOperation(uint8_t *channel0Addr, uint32_t channel0Length,  uint8_t *channel1Addr, uint32_t channel1Length) {

    // Clear any outstanding events.
    HWREG(CRYPTO_BASE + CRYPTO_O_IRQCLR) = CRYPTO_IRQCLR_RESULT_AVAIL_M | CRYPTO_IRQEN_DMA_IN_DONE_M;

    while(HWREG(CRYPTO_BASE + CRYPTO_O_IRQSTAT) & (CRYPTO_IRQSTAT_DMA_IN_DONE_M | CRYPTO_IRQSTAT_RESULT_AVAIL_M));

    if (channel0Addr) {
        // Configure the DMA controller - enable both DMA channels.
        HWREGBITW(CRYPTO_BASE + CRYPTO_O_DMACH0CTL, CRYPTO_DMACH0CTL_EN_BITN) = 1;

        // Base address of the payload data in ext. memory.
        HWREG(CRYPTO_BASE + CRYPTO_O_DMACH0EXTADDR) = (uint32_t)channel0Addr;

        // Payload data length in bytes, equal to the cipher text length.
        HWREG(CRYPTO_BASE + CRYPTO_O_DMACH0LEN) = channel0Length;
    }

    if (channel1Addr) {
        // Enable DMA channel 1.
        HWREGBITW(CRYPTO_BASE + CRYPTO_O_DMACH1CTL, CRYPTO_DMACH1CTL_EN_BITN) = 1;

        // Base address of the output data buffer.
        HWREG(CRYPTO_BASE + CRYPTO_O_DMACH1EXTADDR) = (uint32_t)channel1Addr;

        // Output data length in bytes, equal to the cipher text length.
        HWREG(CRYPTO_BASE + CRYPTO_O_DMACH1LEN) = channel1Length;
    }
}

uint32_t SHA2WaitForIRQFlags(uint32_t irqFlags) {
    uint32_t irqTrigger = 0;
    // Wait for the DMA operation to complete. Add a delay to make sure we are
    // not flooding the bus with requests too much.
    do {
        CPUdelay(1);
    }
    while(!(HWREG(CRYPTO_BASE + CRYPTO_O_IRQSTAT) & irqFlags & (CRYPTO_IRQSTAT_DMA_IN_DONE_M | CRYPTO_IRQSTAT_RESULT_AVAIL_M)));

    // Save the IRQ trigger source
    irqTrigger = HWREG(CRYPTO_BASE + CRYPTO_O_IRQSTAT);

    // Clear IRQ flags
    HWREG(CRYPTO_BASE + CRYPTO_O_IRQCLR) = irqFlags;

    while(HWREG(CRYPTO_BASE + CRYPTO_O_IRQSTAT) & irqFlags & (CRYPTO_IRQSTAT_DMA_IN_DONE_M | CRYPTO_IRQSTAT_RESULT_AVAIL_M));

    return irqTrigger;
}

uint32_t SHA2ComputeInitialHash(const uint8_t *message, uint32_t *intermediateDigest, uint32_t hashAlgorithm, uint32_t initialMessageLength) {
    ASSERT(message);
    ASSERT((hashAlgorithm == SHA2_MODE_SELECT_SHA224) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA256) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA384) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA512));
    ASSERT(!(intermediateDigest == NULL) && !((uint32_t)intermediateDigest & 0x03));

    return SHA2ExecuteHash(message, (uint8_t *)intermediateDigest, intermediateDigest, initialMessageLength, initialMessageLength, hashAlgorithm, true, false);
}

uint32_t SHA2ComputeIntermediateHash(const uint8_t *message, uint32_t *intermediateDigest, uint32_t hashAlgorithm, uint32_t intermediateMessageLength) {
    ASSERT(message);
    ASSERT(!(intermediateDigest == NULL) && !((uint32_t)intermediateDigest & 0x03));
    ASSERT((hashAlgorithm == SHA2_MODE_SELECT_SHA224) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA256) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA384) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA512));

    return SHA2ExecuteHash(message, (uint8_t *)intermediateDigest, intermediateDigest, 0, intermediateMessageLength, hashAlgorithm, false, false);
}

uint32_t SHA2ComputeFinalHash(const uint8_t *message, uint8_t *resultDigest, uint32_t *intermediateDigest, uint32_t totalMsgLength, uint32_t messageLength, uint32_t hashAlgorithm) {
    ASSERT(message);
    ASSERT(totalMsgLength);
    ASSERT(!(intermediateDigest == NULL) && !((uint32_t)intermediateDigest & 0x03));
    ASSERT(resultDigest);
    ASSERT((hashAlgorithm == SHA2_MODE_SELECT_SHA224) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA256) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA384) ||
           (hashAlgorithm == SHA2_MODE_SELECT_SHA512));

    return SHA2ExecuteHash(message, resultDigest, intermediateDigest, totalMsgLength, messageLength, hashAlgorithm, false, true);
}

uint32_t SHA2ComputeHash(const uint8_t *message, uint8_t *resultDigest, uint32_t totalMsgLength, uint32_t hashAlgorithm) {
    ASSERT(message);
    ASSERT(totalMsgLength);
    ASSERT(resultDigest);
    ASSERT((hashAlgorithm == SHA2_MODE_SELECT_SHA224) ||
       (hashAlgorithm == SHA2_MODE_SELECT_SHA256) ||
       (hashAlgorithm == SHA2_MODE_SELECT_SHA384) ||
       (hashAlgorithm == SHA2_MODE_SELECT_SHA512));

    return SHA2ExecuteHash(message, resultDigest, 0, totalMsgLength, totalMsgLength, hashAlgorithm, true, true);
}

static uint32_t SHA2ExecuteHash(const uint8_t *message, uint8_t *resultDigest, uint32_t *intermediateDigest, uint32_t totalMsgLength, uint32_t messageLength, uint32_t hashAlgorithm, bool initialHash, bool finalHash) {
    uint8_t digestLength = 0;
    uint32_t dmaAlgorithmSelect = 0;

    SHA2ClearDigestAvailableFlag();

    switch (hashAlgorithm) {
        case SHA2_MODE_SELECT_SHA224:
            digestLength = SHA2_SHA224_DIGEST_LENGTH_BYTES;
            dmaAlgorithmSelect = SHA2_ALGSEL_SHA256;
            break;
        case SHA2_MODE_SELECT_SHA256:
            digestLength = SHA2_SHA256_DIGEST_LENGTH_BYTES;
            dmaAlgorithmSelect = SHA2_ALGSEL_SHA256;
            break;
        case SHA2_MODE_SELECT_SHA384:
            digestLength = SHA2_SHA384_DIGEST_LENGTH_BYTES;
            dmaAlgorithmSelect = SHA2_ALGSEL_SHA512;
            break;
        case SHA2_MODE_SELECT_SHA512:
            digestLength = SHA2_SHA512_DIGEST_LENGTH_BYTES;
            dmaAlgorithmSelect = SHA2_ALGSEL_SHA512;
            break;
        default:
            return SHA2_INVALID_ALGORITHM;
    }

    if (initialHash && finalHash) {
        // The empty string is a perfectly valid message. It obviously has a length of 0. The DMA cannot
        // handle running with a transfer length of 0. This workaround depends on the hash engine adding the
        // trailing 1 bit and 0-padding bits after the DMAtransfer is complete and not in the DMA itself.
        // totalMsgLength is purposefully not altered as it is appended to the end of the message during finalization
        // and determines how many padding-bytes are added.
        // Altering totalMsgLength would alter the final hash digest.
        // Because totalMsgLength specifies that the message is of length 0, the content of the byte loaded
        // through the DMA is irrelevant. It is overwritten internally in the hash engine.
        messageLength = messageLength ? messageLength : 1;
    }

    // Setting the incorrect number of bits here leads to the calculation of the correct result
    // but a failure to read them out.
    // The tag bit is set to read out the digest via DMA rather than through the slave interface.
    SHA2SelectAlgorithm(dmaAlgorithmSelect | (resultDigest ? SHA2_ALGSEL_TAG : 0));
    SHA2IntClear(SHA2_DMA_IN_DONE | SHA2_RESULT_RDY);
    SHA2IntEnable(SHA2_DMA_IN_DONE | SHA2_RESULT_RDY);

    HWREG(CRYPTO_BASE + CRYPTO_O_HASHMODE) = hashAlgorithm | (initialHash ? CRYPTO_HASHMODE_NEW_HASH_M : 0);

    // Only load the intermediate digest if requested.
    if (intermediateDigest && !initialHash) {
        SHA2SetDigest(intermediateDigest, digestLength);
    }

    // If this is the final hash, finalization is required. This means appending a 1 bit, padding the message until this section
    // is 448 bytes long, and adding the 64 bit total length of the message in bits. Thankfully, this is all done in hardware.
    if (finalHash) {
        // This specific length must be specified in bits not bytes.
        SHA2SetMessageLength(totalMsgLength * 8);
        HWREG(CRYPTO_BASE + CRYPTO_O_HASHIOBUFCTRL) = CRYPTO_HASHIOBUFCTRL_PAD_DMA_MESSAGE_M;

    }

    // The cast is fine in this case. SHA2StartDMAOperation channel one serves as input and no one does
    // hash operations in-place.
    SHA2StartDMAOperation((uint8_t *)message, messageLength,  resultDigest, digestLength);

    return SHA2_SUCCESS;
}
