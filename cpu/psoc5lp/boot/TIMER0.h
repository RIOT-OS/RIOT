/*******************************************************************************
* File Name: TIMER0.h
* Version 2.40
*
*  Description:
*   Contains the function prototypes and constants available to the counter
*   user module.
*
*   Note:
*    None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#if !defined(CY_COUNTER_TIMER0_H)
#define CY_COUNTER_TIMER0_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */

extern uint8 TIMER0_initVar;

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Counter_v2_40 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


/**************************************
*           Parameter Defaults
**************************************/

#define TIMER0_Resolution            32u
#define TIMER0_UsingFixedFunction    0u
#define TIMER0_ControlRegRemoved     0u
#define TIMER0_COMPARE_MODE_SOFTWARE 0u
#define TIMER0_CAPTURE_MODE_SOFTWARE 0u
#define TIMER0_RunModeUsed           0u


/***************************************
*       Type defines
***************************************/


/**************************************************************************
 * Sleep Mode API Support
 * Backup structure for Sleep Wake up operations
 *************************************************************************/
typedef struct
{
    uint8 CounterEnableState;
    uint32 CounterUdb;         /* Current Counter Value */

    #if (CY_UDB_V0)
        uint32 CounterPeriod;  /* Counter Period Value */
        uint32 CompareValue;   /* Counter Compare Value */
        uint8 InterruptMaskValue;                   /* Counter Interrupt Mask Value */
    #endif /* (CY_UDB_V0) */

    #if (!TIMER0_ControlRegRemoved)
        uint8 CounterControlRegister;               /* Counter Control Register */
    #endif /* (!TIMER0_ControlRegRemoved) */

}TIMER0_backupStruct;


/**************************************
 *  Function Prototypes
 *************************************/
void    TIMER0_Start(void) ;
void    TIMER0_Stop(void) ;
void    TIMER0_SetInterruptMode(uint8 interruptsMask) ;
uint8   TIMER0_ReadStatusRegister(void) ;
#define TIMER0_GetInterruptSource() TIMER0_ReadStatusRegister()
#if(!TIMER0_ControlRegRemoved)
    uint8   TIMER0_ReadControlRegister(void) ;
    void    TIMER0_WriteControlRegister(uint8 control) \
        ;
#endif /* (!TIMER0_ControlRegRemoved) */
#if (!(TIMER0_UsingFixedFunction && (CY_PSOC5A)))
    void    TIMER0_WriteCounter(uint32 counter) \
            ;
#endif /* (!(TIMER0_UsingFixedFunction && (CY_PSOC5A))) */
uint32  TIMER0_ReadCounter(void) ;
uint32  TIMER0_ReadCapture(void) ;
void    TIMER0_WritePeriod(uint32 period) \
    ;
uint32  TIMER0_ReadPeriod( void ) ;
#if (!TIMER0_UsingFixedFunction)
    void    TIMER0_WriteCompare(uint32 compare) \
        ;
    uint32  TIMER0_ReadCompare( void ) \
        ;
#endif /* (!TIMER0_UsingFixedFunction) */

#if (TIMER0_COMPARE_MODE_SOFTWARE)
    void    TIMER0_SetCompareMode(uint8 compareMode) ;
#endif /* (TIMER0_COMPARE_MODE_SOFTWARE) */
#if (TIMER0_CAPTURE_MODE_SOFTWARE)
    void    TIMER0_SetCaptureMode(uint8 captureMode) ;
#endif /* (TIMER0_CAPTURE_MODE_SOFTWARE) */
void TIMER0_ClearFIFO(void)     ;
void TIMER0_Init(void)          ;
void TIMER0_Enable(void)        ;
void TIMER0_SaveConfig(void)    ;
void TIMER0_RestoreConfig(void) ;
void TIMER0_Sleep(void)         ;
void TIMER0_Wakeup(void)        ;


/***************************************
*   Enumerated Types and Parameters
***************************************/

/* Enumerated Type B_Counter__CompareModes, Used in Compare Mode retained for backward compatibility of tests*/
#define TIMER0__B_COUNTER__LESS_THAN 1
#define TIMER0__B_COUNTER__LESS_THAN_OR_EQUAL 2
#define TIMER0__B_COUNTER__EQUAL 0
#define TIMER0__B_COUNTER__GREATER_THAN 3
#define TIMER0__B_COUNTER__GREATER_THAN_OR_EQUAL 4
#define TIMER0__B_COUNTER__SOFTWARE 5

/* Enumerated Type Counter_CompareModes */
#define TIMER0_CMP_MODE_LT 1u
#define TIMER0_CMP_MODE_LTE 2u
#define TIMER0_CMP_MODE_EQ 0u
#define TIMER0_CMP_MODE_GT 3u
#define TIMER0_CMP_MODE_GTE 4u
#define TIMER0_CMP_MODE_SOFTWARE_CONTROLLED 5u

/* Enumerated Type B_Counter__CaptureModes, Used in Capture Mode retained for backward compatibility of tests*/
#define TIMER0__B_COUNTER__NONE 0
#define TIMER0__B_COUNTER__RISING_EDGE 1
#define TIMER0__B_COUNTER__FALLING_EDGE 2
#define TIMER0__B_COUNTER__EITHER_EDGE 3
#define TIMER0__B_COUNTER__SOFTWARE_CONTROL 4

/* Enumerated Type Counter_CompareModes */
#define TIMER0_CAP_MODE_NONE 0u
#define TIMER0_CAP_MODE_RISE 1u
#define TIMER0_CAP_MODE_FALL 2u
#define TIMER0_CAP_MODE_BOTH 3u
#define TIMER0_CAP_MODE_SOFTWARE_CONTROLLED 4u


/***************************************
 *  Initialization Values
 **************************************/
#define TIMER0_CAPTURE_MODE_CONF       0u
#define TIMER0_INIT_PERIOD_VALUE       4294967295u
#define TIMER0_INIT_COUNTER_VALUE      0u
#if (TIMER0_UsingFixedFunction)
#define TIMER0_INIT_INTERRUPTS_MASK    ((uint8)((uint8)0u << TIMER0_STATUS_ZERO_INT_EN_MASK_SHIFT))
#else
#define TIMER0_INIT_COMPARE_VALUE      4294967295u
#define TIMER0_INIT_INTERRUPTS_MASK ((uint8)((uint8)0u << TIMER0_STATUS_ZERO_INT_EN_MASK_SHIFT) | \
        ((uint8)((uint8)0u << TIMER0_STATUS_CAPTURE_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)1u << TIMER0_STATUS_CMP_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)0u << TIMER0_STATUS_OVERFLOW_INT_EN_MASK_SHIFT)) | \
        ((uint8)((uint8)0u << TIMER0_STATUS_UNDERFLOW_INT_EN_MASK_SHIFT)))
#define TIMER0_DEFAULT_COMPARE_MODE    0u

#if( 0 != TIMER0_CAPTURE_MODE_CONF)
    #define TIMER0_DEFAULT_CAPTURE_MODE    ((uint8)((uint8)0u << TIMER0_CTRL_CAPMODE0_SHIFT))
#else
    #define TIMER0_DEFAULT_CAPTURE_MODE    (0u )
#endif /* ( 0 != TIMER0_CAPTURE_MODE_CONF) */

#endif /* (TIMER0_UsingFixedFunction) */


/**************************************
 *  Registers
 *************************************/
#if (TIMER0_UsingFixedFunction)
    #define TIMER0_STATICCOUNT_LSB     (*(reg16 *) TIMER0_CounterHW__CAP0 )
    #define TIMER0_STATICCOUNT_LSB_PTR ( (reg16 *) TIMER0_CounterHW__CAP0 )
    #define TIMER0_PERIOD_LSB          (*(reg16 *) TIMER0_CounterHW__PER0 )
    #define TIMER0_PERIOD_LSB_PTR      ( (reg16 *) TIMER0_CounterHW__PER0 )
    /* MODE must be set to 1 to set the compare value */
    #define TIMER0_COMPARE_LSB         (*(reg16 *) TIMER0_CounterHW__CNT_CMP0 )
    #define TIMER0_COMPARE_LSB_PTR     ( (reg16 *) TIMER0_CounterHW__CNT_CMP0 )
    /* MODE must be set to 0 to get the count */
    #define TIMER0_COUNTER_LSB         (*(reg16 *) TIMER0_CounterHW__CNT_CMP0 )
    #define TIMER0_COUNTER_LSB_PTR     ( (reg16 *) TIMER0_CounterHW__CNT_CMP0 )
    #define TIMER0_RT1                 (*(reg8 *) TIMER0_CounterHW__RT1)
    #define TIMER0_RT1_PTR             ( (reg8 *) TIMER0_CounterHW__RT1)
#else

    #if (TIMER0_Resolution <= 8u) /* 8-bit Counter */

        #define TIMER0_STATICCOUNT_LSB     (*(reg8 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__F0_REG )
        #define TIMER0_STATICCOUNT_LSB_PTR ( (reg8 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__F0_REG )
        #define TIMER0_PERIOD_LSB          (*(reg8 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__D0_REG )
        #define TIMER0_PERIOD_LSB_PTR      ( (reg8 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__D0_REG )
        #define TIMER0_COMPARE_LSB         (*(reg8 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__D1_REG )
        #define TIMER0_COMPARE_LSB_PTR     ( (reg8 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__D1_REG )
        #define TIMER0_COUNTER_LSB         (*(reg8 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )
        #define TIMER0_COUNTER_LSB_PTR     ( (reg8 *)\
            TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )

    #elif(TIMER0_Resolution <= 16u) /* 16-bit Counter */
        #if(CY_PSOC3) /* 8-bit address space */
            #define TIMER0_STATICCOUNT_LSB     (*(reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__F0_REG )
            #define TIMER0_STATICCOUNT_LSB_PTR ( (reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__F0_REG )
            #define TIMER0_PERIOD_LSB          (*(reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__D0_REG )
            #define TIMER0_PERIOD_LSB_PTR      ( (reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__D0_REG )
            #define TIMER0_COMPARE_LSB         (*(reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__D1_REG )
            #define TIMER0_COMPARE_LSB_PTR     ( (reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__D1_REG )
            #define TIMER0_COUNTER_LSB         (*(reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )
            #define TIMER0_COUNTER_LSB_PTR     ( (reg16 *)\
                TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )
        #else /* 16-bit address space */
            #define TIMER0_STATICCOUNT_LSB     (*(reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__16BIT_F0_REG )
            #define TIMER0_STATICCOUNT_LSB_PTR ( (reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__16BIT_F0_REG )
            #define TIMER0_PERIOD_LSB          (*(reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__16BIT_D0_REG )
            #define TIMER0_PERIOD_LSB_PTR      ( (reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__16BIT_D0_REG )
            #define TIMER0_COMPARE_LSB         (*(reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__16BIT_D1_REG )
            #define TIMER0_COMPARE_LSB_PTR     ( (reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__16BIT_D1_REG )
            #define TIMER0_COUNTER_LSB         (*(reg16 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__16BIT_A0_REG )
            #define TIMER0_COUNTER_LSB_PTR     ( (reg16 *)\
                TIMER0_CounterUDB_sC32_counterdp_u0__16BIT_A0_REG )
        #endif /* CY_PSOC3 */
    #elif(TIMER0_Resolution <= 24u) /* 24-bit Counter */

        #define TIMER0_STATICCOUNT_LSB     (*(reg32 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__F0_REG )
        #define TIMER0_STATICCOUNT_LSB_PTR ( (reg32 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__F0_REG )
        #define TIMER0_PERIOD_LSB          (*(reg32 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__D0_REG )
        #define TIMER0_PERIOD_LSB_PTR      ( (reg32 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__D0_REG )
        #define TIMER0_COMPARE_LSB         (*(reg32 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__D1_REG )
        #define TIMER0_COMPARE_LSB_PTR     ( (reg32 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__D1_REG )
        #define TIMER0_COUNTER_LSB         (*(reg32 *) \
            TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )
        #define TIMER0_COUNTER_LSB_PTR     ( (reg32 *)\
            TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )

    #else /* 32-bit Counter */
        #if(CY_PSOC3 || CY_PSOC5) /* 8-bit address space */
            #define TIMER0_STATICCOUNT_LSB     (*(reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__F0_REG )
            #define TIMER0_STATICCOUNT_LSB_PTR ( (reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__F0_REG )
            #define TIMER0_PERIOD_LSB          (*(reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__D0_REG )
            #define TIMER0_PERIOD_LSB_PTR      ( (reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__D0_REG )
            #define TIMER0_COMPARE_LSB         (*(reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__D1_REG )
            #define TIMER0_COMPARE_LSB_PTR     ( (reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__D1_REG )
            #define TIMER0_COUNTER_LSB         (*(reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )
            #define TIMER0_COUNTER_LSB_PTR     ( (reg32 *)\
                TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )
        #else /* 32-bit address space */
            #define TIMER0_STATICCOUNT_LSB     (*(reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__32BIT_F0_REG )
            #define TIMER0_STATICCOUNT_LSB_PTR ( (reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__32BIT_F0_REG )
            #define TIMER0_PERIOD_LSB          (*(reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__32BIT_D0_REG )
            #define TIMER0_PERIOD_LSB_PTR      ( (reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__32BIT_D0_REG )
            #define TIMER0_COMPARE_LSB         (*(reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__32BIT_D1_REG )
            #define TIMER0_COMPARE_LSB_PTR     ( (reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__32BIT_D1_REG )
            #define TIMER0_COUNTER_LSB         (*(reg32 *) \
                TIMER0_CounterUDB_sC32_counterdp_u0__32BIT_A0_REG )
            #define TIMER0_COUNTER_LSB_PTR     ( (reg32 *)\
                TIMER0_CounterUDB_sC32_counterdp_u0__32BIT_A0_REG )
        #endif /* CY_PSOC3 || CY_PSOC5 */
    #endif

	#define TIMER0_COUNTER_LSB_PTR_8BIT     ( (reg8 *)\
                TIMER0_CounterUDB_sC32_counterdp_u0__A0_REG )

    #define TIMER0_AUX_CONTROLDP0 \
        (*(reg8 *) TIMER0_CounterUDB_sC32_counterdp_u0__DP_AUX_CTL_REG)

    #define TIMER0_AUX_CONTROLDP0_PTR \
        ( (reg8 *) TIMER0_CounterUDB_sC32_counterdp_u0__DP_AUX_CTL_REG)

    #if (TIMER0_Resolution == 16 || TIMER0_Resolution == 24 || TIMER0_Resolution == 32)
       #define TIMER0_AUX_CONTROLDP1 \
           (*(reg8 *) TIMER0_CounterUDB_sC32_counterdp_u1__DP_AUX_CTL_REG)
       #define TIMER0_AUX_CONTROLDP1_PTR \
           ( (reg8 *) TIMER0_CounterUDB_sC32_counterdp_u1__DP_AUX_CTL_REG)
    #endif /* (TIMER0_Resolution == 16 || TIMER0_Resolution == 24 || TIMER0_Resolution == 32) */

    #if (TIMER0_Resolution == 24 || TIMER0_Resolution == 32)
       #define TIMER0_AUX_CONTROLDP2 \
           (*(reg8 *) TIMER0_CounterUDB_sC32_counterdp_u2__DP_AUX_CTL_REG)
       #define TIMER0_AUX_CONTROLDP2_PTR \
           ( (reg8 *) TIMER0_CounterUDB_sC32_counterdp_u2__DP_AUX_CTL_REG)
    #endif /* (TIMER0_Resolution == 24 || TIMER0_Resolution == 32) */

    #if (TIMER0_Resolution == 32)
       #define TIMER0_AUX_CONTROLDP3 \
           (*(reg8 *) TIMER0_CounterUDB_sC32_counterdp_u3__DP_AUX_CTL_REG)
       #define TIMER0_AUX_CONTROLDP3_PTR \
           ( (reg8 *) TIMER0_CounterUDB_sC32_counterdp_u3__DP_AUX_CTL_REG)
    #endif /* (TIMER0_Resolution == 32) */

#endif  /* (TIMER0_UsingFixedFunction) */

#if (TIMER0_UsingFixedFunction)
    #define TIMER0_STATUS         (*(reg8 *) TIMER0_CounterHW__SR0 )
    /* In Fixed Function Block Status and Mask are the same register */
    #define TIMER0_STATUS_MASK             (*(reg8 *) TIMER0_CounterHW__SR0 )
    #define TIMER0_STATUS_MASK_PTR         ( (reg8 *) TIMER0_CounterHW__SR0 )
    #define TIMER0_CONTROL                 (*(reg8 *) TIMER0_CounterHW__CFG0)
    #define TIMER0_CONTROL_PTR             ( (reg8 *) TIMER0_CounterHW__CFG0)
    #define TIMER0_CONTROL2                (*(reg8 *) TIMER0_CounterHW__CFG1)
    #define TIMER0_CONTROL2_PTR            ( (reg8 *) TIMER0_CounterHW__CFG1)
    #if (CY_PSOC3 || CY_PSOC5LP)
        #define TIMER0_CONTROL3       (*(reg8 *) TIMER0_CounterHW__CFG2)
        #define TIMER0_CONTROL3_PTR   ( (reg8 *) TIMER0_CounterHW__CFG2)
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    #define TIMER0_GLOBAL_ENABLE           (*(reg8 *) TIMER0_CounterHW__PM_ACT_CFG)
    #define TIMER0_GLOBAL_ENABLE_PTR       ( (reg8 *) TIMER0_CounterHW__PM_ACT_CFG)
    #define TIMER0_GLOBAL_STBY_ENABLE      (*(reg8 *) TIMER0_CounterHW__PM_STBY_CFG)
    #define TIMER0_GLOBAL_STBY_ENABLE_PTR  ( (reg8 *) TIMER0_CounterHW__PM_STBY_CFG)


    /********************************
    *    Constants
    ********************************/

    /* Fixed Function Block Chosen */
    #define TIMER0_BLOCK_EN_MASK          TIMER0_CounterHW__PM_ACT_MSK
    #define TIMER0_BLOCK_STBY_EN_MASK     TIMER0_CounterHW__PM_STBY_MSK

    /* Control Register Bit Locations */
    /* As defined in Register Map, part of TMRX_CFG0 register */
    #define TIMER0_CTRL_ENABLE_SHIFT      0x00u
    #define TIMER0_ONESHOT_SHIFT          0x02u
    /* Control Register Bit Masks */
    #define TIMER0_CTRL_ENABLE            ((uint8)((uint8)0x01u << TIMER0_CTRL_ENABLE_SHIFT))
    #define TIMER0_ONESHOT                ((uint8)((uint8)0x01u << TIMER0_ONESHOT_SHIFT))

    /* Control2 Register Bit Masks */
    /* Set the mask for run mode */
    #if (CY_PSOC5A)
        /* Use CFG1 Mode bits to set run mode */
        #define TIMER0_CTRL_MODE_SHIFT        0x01u
        #define TIMER0_CTRL_MODE_MASK         ((uint8)((uint8)0x07u << TIMER0_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC5A) */
    #if (CY_PSOC3 || CY_PSOC5LP)
        /* Use CFG2 Mode bits to set run mode */
        #define TIMER0_CTRL_MODE_SHIFT        0x00u
        #define TIMER0_CTRL_MODE_MASK         ((uint8)((uint8)0x03u << TIMER0_CTRL_MODE_SHIFT))
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    /* Set the mask for interrupt (raw/status register) */
    #define TIMER0_CTRL2_IRQ_SEL_SHIFT     0x00u
    #define TIMER0_CTRL2_IRQ_SEL          ((uint8)((uint8)0x01u << TIMER0_CTRL2_IRQ_SEL_SHIFT))

    /* Status Register Bit Locations */
    #define TIMER0_STATUS_ZERO_SHIFT      0x07u  /* As defined in Register Map, part of TMRX_SR0 register */

    /* Status Register Interrupt Enable Bit Locations */
    #define TIMER0_STATUS_ZERO_INT_EN_MASK_SHIFT      (TIMER0_STATUS_ZERO_SHIFT - 0x04u)

    /* Status Register Bit Masks */
    #define TIMER0_STATUS_ZERO            ((uint8)((uint8)0x01u << TIMER0_STATUS_ZERO_SHIFT))

    /* Status Register Interrupt Bit Masks*/
    #define TIMER0_STATUS_ZERO_INT_EN_MASK       ((uint8)((uint8)TIMER0_STATUS_ZERO >> 0x04u))

    /*RT1 Synch Constants: Applicable for PSoC3 and PSoC5LP */
    #define TIMER0_RT1_SHIFT            0x04u
    #define TIMER0_RT1_MASK             ((uint8)((uint8)0x03u << TIMER0_RT1_SHIFT))  /* Sync TC and CMP bit masks */
    #define TIMER0_SYNC                 ((uint8)((uint8)0x03u << TIMER0_RT1_SHIFT))
    #define TIMER0_SYNCDSI_SHIFT        0x00u
    #define TIMER0_SYNCDSI_MASK         ((uint8)((uint8)0x0Fu << TIMER0_SYNCDSI_SHIFT)) /* Sync all DSI inputs */
    #define TIMER0_SYNCDSI_EN           ((uint8)((uint8)0x0Fu << TIMER0_SYNCDSI_SHIFT)) /* Sync all DSI inputs */

#else /* !TIMER0_UsingFixedFunction */
    #define TIMER0_STATUS               (* (reg8 *) TIMER0_CounterUDB_sSTSReg_rstSts_stsreg__STATUS_REG )
    #define TIMER0_STATUS_PTR           (  (reg8 *) TIMER0_CounterUDB_sSTSReg_rstSts_stsreg__STATUS_REG )
    #define TIMER0_STATUS_MASK          (* (reg8 *) TIMER0_CounterUDB_sSTSReg_rstSts_stsreg__MASK_REG )
    #define TIMER0_STATUS_MASK_PTR      (  (reg8 *) TIMER0_CounterUDB_sSTSReg_rstSts_stsreg__MASK_REG )
    #define TIMER0_STATUS_AUX_CTRL      (*(reg8 *) TIMER0_CounterUDB_sSTSReg_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define TIMER0_STATUS_AUX_CTRL_PTR  ( (reg8 *) TIMER0_CounterUDB_sSTSReg_rstSts_stsreg__STATUS_AUX_CTL_REG)
    #define TIMER0_CONTROL              (* (reg8 *) TIMER0_CounterUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )
    #define TIMER0_CONTROL_PTR          (  (reg8 *) TIMER0_CounterUDB_sCTRLReg_SyncCtl_ctrlreg__CONTROL_REG )


    /********************************
    *    Constants
    ********************************/
    /* Control Register Bit Locations */
    #define TIMER0_CTRL_CAPMODE0_SHIFT    0x03u       /* As defined by Verilog Implementation */
    #define TIMER0_CTRL_RESET_SHIFT       0x06u       /* As defined by Verilog Implementation */
    #define TIMER0_CTRL_ENABLE_SHIFT      0x07u       /* As defined by Verilog Implementation */
    /* Control Register Bit Masks */
    #define TIMER0_CTRL_CMPMODE_MASK      0x07u
    #define TIMER0_CTRL_CAPMODE_MASK      0x03u
    #define TIMER0_CTRL_RESET             ((uint8)((uint8)0x01u << TIMER0_CTRL_RESET_SHIFT))
    #define TIMER0_CTRL_ENABLE            ((uint8)((uint8)0x01u << TIMER0_CTRL_ENABLE_SHIFT))

    /* Status Register Bit Locations */
    #define TIMER0_STATUS_CMP_SHIFT       0x00u       /* As defined by Verilog Implementation */
    #define TIMER0_STATUS_ZERO_SHIFT      0x01u       /* As defined by Verilog Implementation */
    #define TIMER0_STATUS_OVERFLOW_SHIFT  0x02u       /* As defined by Verilog Implementation */
    #define TIMER0_STATUS_UNDERFLOW_SHIFT 0x03u       /* As defined by Verilog Implementation */
    #define TIMER0_STATUS_CAPTURE_SHIFT   0x04u       /* As defined by Verilog Implementation */
    #define TIMER0_STATUS_FIFOFULL_SHIFT  0x05u       /* As defined by Verilog Implementation */
    #define TIMER0_STATUS_FIFONEMP_SHIFT  0x06u       /* As defined by Verilog Implementation */
    /* Status Register Interrupt Enable Bit Locations - UDB Status Interrupt Mask match Status Bit Locations*/
    #define TIMER0_STATUS_CMP_INT_EN_MASK_SHIFT       TIMER0_STATUS_CMP_SHIFT
    #define TIMER0_STATUS_ZERO_INT_EN_MASK_SHIFT      TIMER0_STATUS_ZERO_SHIFT
    #define TIMER0_STATUS_OVERFLOW_INT_EN_MASK_SHIFT  TIMER0_STATUS_OVERFLOW_SHIFT
    #define TIMER0_STATUS_UNDERFLOW_INT_EN_MASK_SHIFT TIMER0_STATUS_UNDERFLOW_SHIFT
    #define TIMER0_STATUS_CAPTURE_INT_EN_MASK_SHIFT   TIMER0_STATUS_CAPTURE_SHIFT
    #define TIMER0_STATUS_FIFOFULL_INT_EN_MASK_SHIFT  TIMER0_STATUS_FIFOFULL_SHIFT
    #define TIMER0_STATUS_FIFONEMP_INT_EN_MASK_SHIFT  TIMER0_STATUS_FIFONEMP_SHIFT
    /* Status Register Bit Masks */
    #define TIMER0_STATUS_CMP             ((uint8)((uint8)0x01u << TIMER0_STATUS_CMP_SHIFT))
    #define TIMER0_STATUS_ZERO            ((uint8)((uint8)0x01u << TIMER0_STATUS_ZERO_SHIFT))
    #define TIMER0_STATUS_OVERFLOW        ((uint8)((uint8)0x01u << TIMER0_STATUS_OVERFLOW_SHIFT))
    #define TIMER0_STATUS_UNDERFLOW       ((uint8)((uint8)0x01u << TIMER0_STATUS_UNDERFLOW_SHIFT))
    #define TIMER0_STATUS_CAPTURE         ((uint8)((uint8)0x01u << TIMER0_STATUS_CAPTURE_SHIFT))
    #define TIMER0_STATUS_FIFOFULL        ((uint8)((uint8)0x01u << TIMER0_STATUS_FIFOFULL_SHIFT))
    #define TIMER0_STATUS_FIFONEMP        ((uint8)((uint8)0x01u << TIMER0_STATUS_FIFONEMP_SHIFT))
    /* Status Register Interrupt Bit Masks  - UDB Status Interrupt Mask match Status Bit Locations */
    #define TIMER0_STATUS_CMP_INT_EN_MASK            TIMER0_STATUS_CMP
    #define TIMER0_STATUS_ZERO_INT_EN_MASK           TIMER0_STATUS_ZERO
    #define TIMER0_STATUS_OVERFLOW_INT_EN_MASK       TIMER0_STATUS_OVERFLOW
    #define TIMER0_STATUS_UNDERFLOW_INT_EN_MASK      TIMER0_STATUS_UNDERFLOW
    #define TIMER0_STATUS_CAPTURE_INT_EN_MASK        TIMER0_STATUS_CAPTURE
    #define TIMER0_STATUS_FIFOFULL_INT_EN_MASK       TIMER0_STATUS_FIFOFULL
    #define TIMER0_STATUS_FIFONEMP_INT_EN_MASK       TIMER0_STATUS_FIFONEMP


    /* StatusI Interrupt Enable bit Location in the Auxilliary Control Register */
    #define TIMER0_STATUS_ACTL_INT_EN     0x10u /* As defined for the ACTL Register */

    /* Datapath Auxillary Control Register definitions */
    #define TIMER0_AUX_CTRL_FIFO0_CLR         0x01u   /* As defined by Register map */
    #define TIMER0_AUX_CTRL_FIFO1_CLR         0x02u   /* As defined by Register map */
    #define TIMER0_AUX_CTRL_FIFO0_LVL         0x04u   /* As defined by Register map */
    #define TIMER0_AUX_CTRL_FIFO1_LVL         0x08u   /* As defined by Register map */
    #define TIMER0_STATUS_ACTL_INT_EN_MASK    0x10u   /* As defined for the ACTL Register */

#endif /* TIMER0_UsingFixedFunction */

#endif  /* CY_COUNTER_TIMER0_H */


/* [] END OF FILE */

