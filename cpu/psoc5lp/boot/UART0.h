/*******************************************************************************
* File Name: UART0.h
* Version 2.30
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_UART0_H)
#define CY_UART_UART0_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define UART0_RX_ENABLED                     (1u)
#define UART0_TX_ENABLED                     (1u)
#define UART0_HD_ENABLED                     (0u)
#define UART0_RX_INTERRUPT_ENABLED           (0u)
#define UART0_TX_INTERRUPT_ENABLED           (0u)
#define UART0_INTERNAL_CLOCK_USED            (0u)
#define UART0_RXHW_ADDRESS_ENABLED           (0u)
#define UART0_OVER_SAMPLE_COUNT              (8u)
#define UART0_PARITY_TYPE                    (0u)
#define UART0_PARITY_TYPE_SW                 (0u)
#define UART0_BREAK_DETECT                   (0u)
#define UART0_BREAK_BITS_TX                  (13u)
#define UART0_BREAK_BITS_RX                  (13u)
#define UART0_TXCLKGEN_DP                    (1u)
#define UART0_USE23POLLING                   (1u)
#define UART0_FLOW_CONTROL                   (0u)
#define UART0_CLK_FREQ                       (0u)
#define UART0_TXBUFFERSIZE                   (4u)
#define UART0_RXBUFFERSIZE                   (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_30 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#ifdef UART0_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define UART0_CONTROL_REG_REMOVED            (0u)
#else
    #define UART0_CONTROL_REG_REMOVED            (1u)
#endif /* End UART0_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct UART0_backupStruct_
{
    uint8 enableState;

    #if(UART0_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End UART0_CONTROL_REG_REMOVED */
    #if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )
        uint8 rx_period;
        #if (CY_UDB_V0)
            uint8 rx_mask;
            #if (UART0_RXHW_ADDRESS_ENABLED)
                uint8 rx_addr1;
                uint8 rx_addr2;
            #endif /* End UART0_RXHW_ADDRESS_ENABLED */
        #endif /* End CY_UDB_V0 */
    #endif  /* End (UART0_RX_ENABLED) || (UART0_HD_ENABLED)*/

    #if(UART0_TX_ENABLED)
        #if(UART0_TXCLKGEN_DP)
            uint8 tx_clk_ctr;
            #if (CY_UDB_V0)
                uint8 tx_clk_compl;
            #endif  /* End CY_UDB_V0 */
        #else
            uint8 tx_period;
        #endif /*End UART0_TXCLKGEN_DP */
        #if (CY_UDB_V0)
            uint8 tx_mask;
        #endif  /* End CY_UDB_V0 */
    #endif /*End UART0_TX_ENABLED */
} UART0_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void UART0_Start(void) ;
void UART0_Stop(void) ;
uint8 UART0_ReadControlRegister(void) ;
void UART0_WriteControlRegister(uint8 control) ;

void UART0_Init(void) ;
void UART0_Enable(void) ;
void UART0_SaveConfig(void) ;
void UART0_RestoreConfig(void) ;
void UART0_Sleep(void) ;
void UART0_Wakeup(void) ;

/* Only if RX is enabled */
#if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )

    #if(UART0_RX_INTERRUPT_ENABLED)
        void  UART0_EnableRxInt(void) ;
        void  UART0_DisableRxInt(void) ;
        CY_ISR_PROTO(UART0_RXISR);
    #endif /* UART0_RX_INTERRUPT_ENABLED */

    void UART0_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void UART0_SetRxAddress1(uint8 address) ;
    void UART0_SetRxAddress2(uint8 address) ;

    void  UART0_SetRxInterruptMode(uint8 intSrc) ;
    uint8 UART0_ReadRxData(void) ;
    uint8 UART0_ReadRxStatus(void) ;
    uint8 UART0_GetChar(void) ;
    uint16 UART0_GetByte(void) ;
    uint8 UART0_GetRxBufferSize(void)
                                                            ;
    void UART0_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define UART0_GetRxInterruptSource   UART0_ReadRxStatus

#endif /* End (UART0_RX_ENABLED) || (UART0_HD_ENABLED) */

/* Only if TX is enabled */
#if(UART0_TX_ENABLED || UART0_HD_ENABLED)

    #if(UART0_TX_INTERRUPT_ENABLED)
        void UART0_EnableTxInt(void) ;
        void UART0_DisableTxInt(void) ;
        CY_ISR_PROTO(UART0_TXISR);
    #endif /* UART0_TX_INTERRUPT_ENABLED */

    void UART0_SetTxInterruptMode(uint8 intSrc) ;
    void UART0_WriteTxData(uint8 txDataByte) ;
    uint8 UART0_ReadTxStatus(void) ;
    void UART0_PutChar(uint8 txDataByte) ;
    void UART0_PutString(const char8 string[]) ;
    void UART0_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void UART0_PutCRLF(uint8 txDataByte) ;
    void UART0_ClearTxBuffer(void) ;
    void UART0_SetTxAddressMode(uint8 addressMode) ;
    void UART0_SendBreak(uint8 retMode) ;
    uint8 UART0_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define UART0_PutStringConst         UART0_PutString
    #define UART0_PutArrayConst          UART0_PutArray
    #define UART0_GetTxInterruptSource   UART0_ReadTxStatus

#endif /* End UART0_TX_ENABLED || UART0_HD_ENABLED */

#if(UART0_HD_ENABLED)
    void UART0_LoadRxConfig(void) ;
    void UART0_LoadTxConfig(void) ;
#endif /* End UART0_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_UART0) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    UART0_CyBtldrCommStart(void) CYSMALL ;
    void    UART0_CyBtldrCommStop(void) CYSMALL ;
    void    UART0_CyBtldrCommReset(void) CYSMALL ;
    cystatus UART0_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus UART0_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_UART0)
        #define CyBtldrCommStart    UART0_CyBtldrCommStart
        #define CyBtldrCommStop     UART0_CyBtldrCommStop
        #define CyBtldrCommReset    UART0_CyBtldrCommReset
        #define CyBtldrCommWrite    UART0_CyBtldrCommWrite
        #define CyBtldrCommRead     UART0_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_UART0) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define UART0_BYTE2BYTE_TIME_OUT (25u)

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define UART0_SET_SPACE                              (0x00u)
#define UART0_SET_MARK                               (0x01u)

/* Status Register definitions */
#if( (UART0_TX_ENABLED) || (UART0_HD_ENABLED) )
    #if(UART0_TX_INTERRUPT_ENABLED)
        #define UART0_TX_VECT_NUM            (uint8)UART0_TXInternalInterrupt__INTC_NUMBER
        #define UART0_TX_PRIOR_NUM           (uint8)UART0_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* UART0_TX_INTERRUPT_ENABLED */
    #if(UART0_TX_ENABLED)
        #define UART0_TX_STS_COMPLETE_SHIFT          (0x00u)
        #define UART0_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
        #define UART0_TX_STS_FIFO_FULL_SHIFT         (0x02u)
        #define UART0_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #endif /* UART0_TX_ENABLED */
    #if(UART0_HD_ENABLED)
        #define UART0_TX_STS_COMPLETE_SHIFT          (0x00u)
        #define UART0_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
        #define UART0_TX_STS_FIFO_FULL_SHIFT         (0x05u)  /*needs MD=0*/
        #define UART0_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #endif /* UART0_HD_ENABLED */
    #define UART0_TX_STS_COMPLETE            (uint8)(0x01u << UART0_TX_STS_COMPLETE_SHIFT)
    #define UART0_TX_STS_FIFO_EMPTY          (uint8)(0x01u << UART0_TX_STS_FIFO_EMPTY_SHIFT)
    #define UART0_TX_STS_FIFO_FULL           (uint8)(0x01u << UART0_TX_STS_FIFO_FULL_SHIFT)
    #define UART0_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << UART0_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (UART0_TX_ENABLED) || (UART0_HD_ENABLED)*/

#if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )
    #if(UART0_RX_INTERRUPT_ENABLED)
        #define UART0_RX_VECT_NUM            (uint8)UART0_RXInternalInterrupt__INTC_NUMBER
        #define UART0_RX_PRIOR_NUM           (uint8)UART0_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* UART0_RX_INTERRUPT_ENABLED */
    #define UART0_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define UART0_RX_STS_BREAK_SHIFT             (0x01u)
    #define UART0_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define UART0_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define UART0_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define UART0_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define UART0_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define UART0_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define UART0_RX_STS_MRKSPC           (uint8)(0x01u << UART0_RX_STS_MRKSPC_SHIFT)
    #define UART0_RX_STS_BREAK            (uint8)(0x01u << UART0_RX_STS_BREAK_SHIFT)
    #define UART0_RX_STS_PAR_ERROR        (uint8)(0x01u << UART0_RX_STS_PAR_ERROR_SHIFT)
    #define UART0_RX_STS_STOP_ERROR       (uint8)(0x01u << UART0_RX_STS_STOP_ERROR_SHIFT)
    #define UART0_RX_STS_OVERRUN          (uint8)(0x01u << UART0_RX_STS_OVERRUN_SHIFT)
    #define UART0_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << UART0_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define UART0_RX_STS_ADDR_MATCH       (uint8)(0x01u << UART0_RX_STS_ADDR_MATCH_SHIFT)
    #define UART0_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << UART0_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define UART0_RX_HW_MASK                     (0x7Fu)
#endif /* End (UART0_RX_ENABLED) || (UART0_HD_ENABLED) */

/* Control Register definitions */
#define UART0_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define UART0_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define UART0_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define UART0_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define UART0_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define UART0_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define UART0_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define UART0_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define UART0_CTRL_HD_SEND               (uint8)(0x01u << UART0_CTRL_HD_SEND_SHIFT)
#define UART0_CTRL_HD_SEND_BREAK         (uint8)(0x01u << UART0_CTRL_HD_SEND_BREAK_SHIFT)
#define UART0_CTRL_MARK                  (uint8)(0x01u << UART0_CTRL_MARK_SHIFT)
#define UART0_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << UART0_CTRL_PARITY_TYPE0_SHIFT)
#define UART0_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << UART0_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define UART0_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define UART0_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define UART0_SEND_BREAK                         (0x00u)
#define UART0_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define UART0_REINIT                             (0x02u)
#define UART0_SEND_WAIT_REINIT                   (0x03u)

#define UART0_OVER_SAMPLE_8                      (8u)
#define UART0_OVER_SAMPLE_16                     (16u)

#define UART0_BIT_CENTER                         (UART0_OVER_SAMPLE_COUNT - 1u)

#define UART0_FIFO_LENGTH                        (4u)
#define UART0_NUMBER_OF_START_BIT                (1u)
#define UART0_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation*/
#define UART0_TXBITCTR_BREAKBITS8X   ((UART0_BREAK_BITS_TX * UART0_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation*/
#define UART0_TXBITCTR_BREAKBITS ((UART0_BREAK_BITS_TX * UART0_OVER_SAMPLE_COUNT) - 1u)

#define UART0_HALF_BIT_COUNT   \
                            (((UART0_OVER_SAMPLE_COUNT / 2u) + (UART0_USE23POLLING * 1u)) - 2u)
#if (UART0_OVER_SAMPLE_COUNT == UART0_OVER_SAMPLE_8)
    #define UART0_HD_TXBITCTR_INIT   (((UART0_BREAK_BITS_TX + \
                            UART0_NUMBER_OF_START_BIT) * UART0_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define UART0_RXBITCTR_INIT  ((((UART0_BREAK_BITS_RX + UART0_NUMBER_OF_START_BIT) \
                            * UART0_OVER_SAMPLE_COUNT) + UART0_HALF_BIT_COUNT) - 1u)


#else /* UART0_OVER_SAMPLE_COUNT == UART0_OVER_SAMPLE_16 */
    #define UART0_HD_TXBITCTR_INIT   ((8u * UART0_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount=16 */
    #define UART0_RXBITCTR_INIT      (((7u * UART0_OVER_SAMPLE_COUNT) - 1u) + \
                                                      UART0_HALF_BIT_COUNT)
#endif /* End UART0_OVER_SAMPLE_COUNT */
#define UART0_HD_RXBITCTR_INIT                   UART0_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 UART0_initVar;
#if( UART0_TX_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH))
    extern volatile uint8 UART0_txBuffer[UART0_TXBUFFERSIZE];
    extern volatile uint8 UART0_txBufferRead;
    extern uint8 UART0_txBufferWrite;
#endif /* End UART0_TX_ENABLED */
#if( ( UART0_RX_ENABLED || UART0_HD_ENABLED ) && \
     (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH) )
    extern volatile uint8 UART0_rxBuffer[UART0_RXBUFFERSIZE];
    extern volatile uint8 UART0_rxBufferRead;
    extern volatile uint8 UART0_rxBufferWrite;
    extern volatile uint8 UART0_rxBufferLoopDetect;
    extern volatile uint8 UART0_rxBufferOverflow;
    #if (UART0_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 UART0_rxAddressMode;
        extern volatile uint8 UART0_rxAddressDetected;
    #endif /* End EnableHWAddress */
#endif /* End UART0_RX_ENABLED */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define UART0__B_UART__AM_SW_BYTE_BYTE 1
#define UART0__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define UART0__B_UART__AM_HW_BYTE_BY_BYTE 3
#define UART0__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define UART0__B_UART__AM_NONE 0

#define UART0__B_UART__NONE_REVB 0
#define UART0__B_UART__EVEN_REVB 1
#define UART0__B_UART__ODD_REVB 2
#define UART0__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define UART0_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define UART0_NUMBER_OF_STOP_BITS    (1u)

#if (UART0_RXHW_ADDRESS_ENABLED)
    #define UART0_RXADDRESSMODE      (0u)
    #define UART0_RXHWADDRESS1       (0u)
    #define UART0_RXHWADDRESS2       (0u)
    /* Backward compatible define */
    #define UART0_RXAddressMode      UART0_RXADDRESSMODE
#endif /* End EnableHWAddress */

#define UART0_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << UART0_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << UART0_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << UART0_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << UART0_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << UART0_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << UART0_RX_STS_BREAK_SHIFT) \
                                        | (0 << UART0_RX_STS_OVERRUN_SHIFT))

#define UART0_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << UART0_TX_STS_COMPLETE_SHIFT) \
                                        | (1 << UART0_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << UART0_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << UART0_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef UART0_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define UART0_CONTROL_REG \
                            (* (reg8 *) UART0_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define UART0_CONTROL_PTR \
                            (  (reg8 *) UART0_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End UART0_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(UART0_TX_ENABLED)
    #define UART0_TXDATA_REG          (* (reg8 *) UART0_BUART_sTX_TxShifter_u0__F0_REG)
    #define UART0_TXDATA_PTR          (  (reg8 *) UART0_BUART_sTX_TxShifter_u0__F0_REG)
    #define UART0_TXDATA_AUX_CTL_REG  (* (reg8 *) UART0_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define UART0_TXDATA_AUX_CTL_PTR  (  (reg8 *) UART0_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define UART0_TXSTATUS_REG        (* (reg8 *) UART0_BUART_sTX_TxSts__STATUS_REG)
    #define UART0_TXSTATUS_PTR        (  (reg8 *) UART0_BUART_sTX_TxSts__STATUS_REG)
    #define UART0_TXSTATUS_MASK_REG   (* (reg8 *) UART0_BUART_sTX_TxSts__MASK_REG)
    #define UART0_TXSTATUS_MASK_PTR   (  (reg8 *) UART0_BUART_sTX_TxSts__MASK_REG)
    #define UART0_TXSTATUS_ACTL_REG   (* (reg8 *) UART0_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define UART0_TXSTATUS_ACTL_PTR   (  (reg8 *) UART0_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(UART0_TXCLKGEN_DP)
        #define UART0_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define UART0_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define UART0_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define UART0_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define UART0_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define UART0_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define UART0_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define UART0_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define UART0_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define UART0_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) UART0_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* UART0_TXCLKGEN_DP */

#endif /* End UART0_TX_ENABLED */

#if(UART0_HD_ENABLED)

    #define UART0_TXDATA_REG             (* (reg8 *) UART0_BUART_sRX_RxShifter_u0__F1_REG )
    #define UART0_TXDATA_PTR             (  (reg8 *) UART0_BUART_sRX_RxShifter_u0__F1_REG )
    #define UART0_TXDATA_AUX_CTL_REG     (* (reg8 *) UART0_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define UART0_TXDATA_AUX_CTL_PTR     (  (reg8 *) UART0_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define UART0_TXSTATUS_REG           (* (reg8 *) UART0_BUART_sRX_RxSts__STATUS_REG )
    #define UART0_TXSTATUS_PTR           (  (reg8 *) UART0_BUART_sRX_RxSts__STATUS_REG )
    #define UART0_TXSTATUS_MASK_REG      (* (reg8 *) UART0_BUART_sRX_RxSts__MASK_REG )
    #define UART0_TXSTATUS_MASK_PTR      (  (reg8 *) UART0_BUART_sRX_RxSts__MASK_REG )
    #define UART0_TXSTATUS_ACTL_REG      (* (reg8 *) UART0_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define UART0_TXSTATUS_ACTL_PTR      (  (reg8 *) UART0_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End UART0_HD_ENABLED */

#if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )
    #define UART0_RXDATA_REG             (* (reg8 *) UART0_BUART_sRX_RxShifter_u0__F0_REG )
    #define UART0_RXDATA_PTR             (  (reg8 *) UART0_BUART_sRX_RxShifter_u0__F0_REG )
    #define UART0_RXADDRESS1_REG         (* (reg8 *) UART0_BUART_sRX_RxShifter_u0__D0_REG )
    #define UART0_RXADDRESS1_PTR         (  (reg8 *) UART0_BUART_sRX_RxShifter_u0__D0_REG )
    #define UART0_RXADDRESS2_REG         (* (reg8 *) UART0_BUART_sRX_RxShifter_u0__D1_REG )
    #define UART0_RXADDRESS2_PTR         (  (reg8 *) UART0_BUART_sRX_RxShifter_u0__D1_REG )
    #define UART0_RXDATA_AUX_CTL_REG     (* (reg8 *) UART0_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define UART0_RXBITCTR_PERIOD_REG    (* (reg8 *) UART0_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define UART0_RXBITCTR_PERIOD_PTR    (  (reg8 *) UART0_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define UART0_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) UART0_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define UART0_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) UART0_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define UART0_RXBITCTR_COUNTER_REG   (* (reg8 *) UART0_BUART_sRX_RxBitCounter__COUNT_REG )
    #define UART0_RXBITCTR_COUNTER_PTR   (  (reg8 *) UART0_BUART_sRX_RxBitCounter__COUNT_REG )

    #define UART0_RXSTATUS_REG           (* (reg8 *) UART0_BUART_sRX_RxSts__STATUS_REG )
    #define UART0_RXSTATUS_PTR           (  (reg8 *) UART0_BUART_sRX_RxSts__STATUS_REG )
    #define UART0_RXSTATUS_MASK_REG      (* (reg8 *) UART0_BUART_sRX_RxSts__MASK_REG )
    #define UART0_RXSTATUS_MASK_PTR      (  (reg8 *) UART0_BUART_sRX_RxSts__MASK_REG )
    #define UART0_RXSTATUS_ACTL_REG      (* (reg8 *) UART0_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define UART0_RXSTATUS_ACTL_PTR      (  (reg8 *) UART0_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (UART0_RX_ENABLED) || (UART0_HD_ENABLED) */

#if(UART0_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define UART0_INTCLOCK_CLKEN_REG     (* (reg8 *) UART0_IntClock__PM_ACT_CFG)
    #define UART0_INTCLOCK_CLKEN_PTR     (  (reg8 *) UART0_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define UART0_INTCLOCK_CLKEN_MASK    UART0_IntClock__PM_ACT_MSK
#endif /* End UART0_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(UART0_TX_ENABLED)
    #define UART0_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End UART0_TX_ENABLED */

#if(UART0_HD_ENABLED)
    #define UART0_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End UART0_HD_ENABLED */

#if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )
    #define UART0_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (UART0_RX_ENABLED) || (UART0_HD_ENABLED) */


/***************************************
* Renamed global variables or defines
* for backward compatible
***************************************/

#define UART0_initvar                    UART0_initVar

#define UART0_RX_Enabled                 UART0_RX_ENABLED
#define UART0_TX_Enabled                 UART0_TX_ENABLED
#define UART0_HD_Enabled                 UART0_HD_ENABLED
#define UART0_RX_IntInterruptEnabled     UART0_RX_INTERRUPT_ENABLED
#define UART0_TX_IntInterruptEnabled     UART0_TX_INTERRUPT_ENABLED
#define UART0_InternalClockUsed          UART0_INTERNAL_CLOCK_USED
#define UART0_RXHW_Address_Enabled       UART0_RXHW_ADDRESS_ENABLED
#define UART0_OverSampleCount            UART0_OVER_SAMPLE_COUNT
#define UART0_ParityType                 UART0_PARITY_TYPE

#if( UART0_TX_ENABLED && (UART0_TXBUFFERSIZE > UART0_FIFO_LENGTH))
    #define UART0_TXBUFFER               UART0_txBuffer
    #define UART0_TXBUFFERREAD           UART0_txBufferRead
    #define UART0_TXBUFFERWRITE          UART0_txBufferWrite
#endif /* End UART0_TX_ENABLED */
#if( ( UART0_RX_ENABLED || UART0_HD_ENABLED ) && \
     (UART0_RXBUFFERSIZE > UART0_FIFO_LENGTH) )
    #define UART0_RXBUFFER               UART0_rxBuffer
    #define UART0_RXBUFFERREAD           UART0_rxBufferRead
    #define UART0_RXBUFFERWRITE          UART0_rxBufferWrite
    #define UART0_RXBUFFERLOOPDETECT     UART0_rxBufferLoopDetect
    #define UART0_RXBUFFER_OVERFLOW      UART0_rxBufferOverflow
#endif /* End UART0_RX_ENABLED */

#ifdef UART0_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define UART0_CONTROL                UART0_CONTROL_REG
#endif /* End UART0_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(UART0_TX_ENABLED)
    #define UART0_TXDATA                 UART0_TXDATA_REG
    #define UART0_TXSTATUS               UART0_TXSTATUS_REG
    #define UART0_TXSTATUS_MASK          UART0_TXSTATUS_MASK_REG
    #define UART0_TXSTATUS_ACTL          UART0_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(UART0_TXCLKGEN_DP)
        #define UART0_TXBITCLKGEN_CTR        UART0_TXBITCLKGEN_CTR_REG
        #define UART0_TXBITCLKTX_COMPLETE    UART0_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define UART0_TXBITCTR_PERIOD        UART0_TXBITCTR_PERIOD_REG
        #define UART0_TXBITCTR_CONTROL       UART0_TXBITCTR_CONTROL_REG
        #define UART0_TXBITCTR_COUNTER       UART0_TXBITCTR_COUNTER_REG
    #endif /* UART0_TXCLKGEN_DP */
#endif /* End UART0_TX_ENABLED */

#if(UART0_HD_ENABLED)
    #define UART0_TXDATA                 UART0_TXDATA_REG
    #define UART0_TXSTATUS               UART0_TXSTATUS_REG
    #define UART0_TXSTATUS_MASK          UART0_TXSTATUS_MASK_REG
    #define UART0_TXSTATUS_ACTL          UART0_TXSTATUS_ACTL_REG
#endif /* End UART0_HD_ENABLED */

#if( (UART0_RX_ENABLED) || (UART0_HD_ENABLED) )
    #define UART0_RXDATA                 UART0_RXDATA_REG
    #define UART0_RXADDRESS1             UART0_RXADDRESS1_REG
    #define UART0_RXADDRESS2             UART0_RXADDRESS2_REG
    #define UART0_RXBITCTR_PERIOD        UART0_RXBITCTR_PERIOD_REG
    #define UART0_RXBITCTR_CONTROL       UART0_RXBITCTR_CONTROL_REG
    #define UART0_RXBITCTR_COUNTER       UART0_RXBITCTR_COUNTER_REG
    #define UART0_RXSTATUS               UART0_RXSTATUS_REG
    #define UART0_RXSTATUS_MASK          UART0_RXSTATUS_MASK_REG
    #define UART0_RXSTATUS_ACTL          UART0_RXSTATUS_ACTL_REG
#endif /* End  (UART0_RX_ENABLED) || (UART0_HD_ENABLED) */

#if(UART0_INTERNAL_CLOCK_USED)
    #define UART0_INTCLOCK_CLKEN         UART0_INTCLOCK_CLKEN_REG
#endif /* End UART0_INTERNAL_CLOCK_USED */

#define UART0_WAIT_FOR_COMLETE_REINIT    UART0_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_UART0_H */


/* [] END OF FILE */
