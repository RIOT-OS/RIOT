/*******************************************************************************
* File Name: CharacterLCD.h
* Version 2.10
*
* Description:
*  This header file contains registers and constants associated with the
*  Character LCD component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CHARLCD_CharacterLCD_H)
#define CY_CHARLCD_CharacterLCD_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define CharacterLCD_CONVERSION_ROUTINES     (1u)
#define CharacterLCD_CUSTOM_CHAR_SET         (0u)

/* Custom character set types */
#define CharacterLCD_NONE                     (0u)    /* No Custom Fonts      */
#define CharacterLCD_HORIZONTAL_BG            (1u)    /* Horizontal Bar Graph */
#define CharacterLCD_VERTICAL_BG              (2u)    /* Vertical Bar Graph   */
#define CharacterLCD_USER_DEFINED             (3u)    /* User Defined Fonts   */


/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
} CharacterLCD_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void CharacterLCD_Init(void) ;
void CharacterLCD_Enable(void) ;
void CharacterLCD_Start(void) ;
void CharacterLCD_Stop(void) ;
void CharacterLCD_WriteControl(uint8 cByte) ;
void CharacterLCD_WriteData(uint8 dByte) ;
void CharacterLCD_PrintString(char8 const string[]) ;
void CharacterLCD_Position(uint8 row, uint8 column) ;
void CharacterLCD_PutChar(char8 character) ;
void CharacterLCD_IsReady(void) ;
void CharacterLCD_SaveConfig(void) ;
void CharacterLCD_RestoreConfig(void) ;
void CharacterLCD_Sleep(void) ;
void CharacterLCD_Wakeup(void) ;

#if((CharacterLCD_CUSTOM_CHAR_SET == CharacterLCD_VERTICAL_BG) || \
                (CharacterLCD_CUSTOM_CHAR_SET == CharacterLCD_HORIZONTAL_BG))

    void  CharacterLCD_LoadCustomFonts(uint8 const customData[])
                        ;

    void  CharacterLCD_DrawHorizontalBG(uint8 row, uint8 column, uint8 maxCharacters, uint8 value)
                         ;

    void CharacterLCD_DrawVerticalBG(uint8 row, uint8 column, uint8 maxCharacters, uint8 value)
                        ;

#endif /* ((CharacterLCD_CUSTOM_CHAR_SET == CharacterLCD_VERTICAL_BG) */

#if(CharacterLCD_CUSTOM_CHAR_SET == CharacterLCD_USER_DEFINED)

    void CharacterLCD_LoadCustomFonts(uint8 const customData[])
                            ;

#endif /* ((CharacterLCD_CUSTOM_CHAR_SET == CharacterLCD_USER_DEFINED) */

#if(CharacterLCD_CONVERSION_ROUTINES == 1u)

    /* ASCII Conversion Routines */
    void CharacterLCD_PrintInt8(uint8 value) ;
    void CharacterLCD_PrintInt16(uint16 value) ;
    void CharacterLCD_PrintInt32(uint32 value) ;
    void CharacterLCD_PrintNumber(uint16 value) ; 
    void CharacterLCD_PrintU32Number(uint32 value) ;
    
#endif /* CharacterLCD_CONVERSION_ROUTINES == 1u */

/* Clear Macro */
#define CharacterLCD_ClearDisplay() CharacterLCD_WriteControl(CharacterLCD_CLEAR_DISPLAY)

/* Off Macro */
#define CharacterLCD_DisplayOff() CharacterLCD_WriteControl(CharacterLCD_DISPLAY_CURSOR_OFF)

/* On Macro */
#define CharacterLCD_DisplayOn() CharacterLCD_WriteControl(CharacterLCD_DISPLAY_ON_CURSOR_OFF)

#define CharacterLCD_PrintNumber(value) CharacterLCD_PrintU32Number((uint16) (value))


/***************************************
*           Global Variables
***************************************/

extern uint8 CharacterLCD_initVar;
extern uint8 CharacterLCD_enableState;
extern uint8 const CYCODE CharacterLCD_customFonts[64u];


/***************************************
*           API Constants
***************************************/

/* Full Byte Commands Sent as Two Nibbles */
#define CharacterLCD_DISPLAY_8_BIT_INIT       (0x03u)
#define CharacterLCD_DISPLAY_4_BIT_INIT       (0x02u)
#define CharacterLCD_DISPLAY_CURSOR_OFF       (0x08u)
#define CharacterLCD_CLEAR_DISPLAY            (0x01u)
#define CharacterLCD_CURSOR_AUTO_INCR_ON      (0x06u)
#define CharacterLCD_DISPLAY_CURSOR_ON        (0x0Eu)
#define CharacterLCD_DISPLAY_2_LINES_5x10     (0x2Cu)
#define CharacterLCD_DISPLAY_ON_CURSOR_OFF    (0x0Cu)

#define CharacterLCD_RESET_CURSOR_POSITION    (0x03u)
#define CharacterLCD_CURSOR_WINK              (0x0Du)
#define CharacterLCD_CURSOR_BLINK             (0x0Fu)
#define CharacterLCD_CURSOR_SH_LEFT           (0x10u)
#define CharacterLCD_CURSOR_SH_RIGHT          (0x14u)
#define CharacterLCD_DISPLAY_SCRL_LEFT        (0x18u)
#define CharacterLCD_DISPLAY_SCRL_RIGHT       (0x1Eu)
#define CharacterLCD_CURSOR_HOME              (0x02u)
#define CharacterLCD_CURSOR_LEFT              (0x04u)
#define CharacterLCD_CURSOR_RIGHT             (0x06u)

/* Point to Character Generator Ram 0 */
#define CharacterLCD_CGRAM_0                  (0x40u)

/* Point to Display Data Ram 0 */
#define CharacterLCD_DDRAM_0                  (0x80u)

/* LCD Characteristics */
#define CharacterLCD_CHARACTER_WIDTH          (0x05u)
#define CharacterLCD_CHARACTER_HEIGHT         (0x08u)

#if(CharacterLCD_CONVERSION_ROUTINES == 1u)
    #define CharacterLCD_NUMBER_OF_REMAINDERS_U32 (0x0Au)
    #define CharacterLCD_TEN                      (0x0Au)
    #define CharacterLCD_8_BIT_SHIFT              (8u)
    #define CharacterLCD_32_BIT_SHIFT             (32u)
    #define CharacterLCD_ZERO_CHAR_ASCII          (48u)
#endif /* CharacterLCD_CONVERSION_ROUTINES == 1u */

/* Nibble Offset and Mask */
#define CharacterLCD_NIBBLE_SHIFT             (0x04u)
#define CharacterLCD_NIBBLE_MASK              (0x0Fu)

/* LCD Module Address Constants */
#define CharacterLCD_ROW_0_START              (0x80u)
#define CharacterLCD_ROW_1_START              (0xC0u)
#define CharacterLCD_ROW_2_START              (0x94u)
#define CharacterLCD_ROW_3_START              (0xD4u)

/* Custom Character References */
#define CharacterLCD_CUSTOM_0                 (0x00u)
#define CharacterLCD_CUSTOM_1                 (0x01u)
#define CharacterLCD_CUSTOM_2                 (0x02u)
#define CharacterLCD_CUSTOM_3                 (0x03u)
#define CharacterLCD_CUSTOM_4                 (0x04u)
#define CharacterLCD_CUSTOM_5                 (0x05u)
#define CharacterLCD_CUSTOM_6                 (0x06u)
#define CharacterLCD_CUSTOM_7                 (0x07u)

/* Other constants */
#define CharacterLCD_BYTE_UPPER_NIBBLE_SHIFT  (0x04u)
#define CharacterLCD_BYTE_LOWER_NIBBLE_MASK   (0x0Fu)
#define CharacterLCD_U16_UPPER_BYTE_SHIFT     (0x08u)
#define CharacterLCD_U16_LOWER_BYTE_MASK      (0xFFu)
#define CharacterLCD_CUSTOM_CHAR_SET_LEN      (0x40u)

#define CharacterLCD_LONGEST_CMD_US           (0x651u)
#define CharacterLCD_WAIT_CYCLE               (0x10u)
#define CharacterLCD_READY_DELAY              ((CharacterLCD_LONGEST_CMD_US * 4u)/(CharacterLCD_WAIT_CYCLE))


/***************************************
*             Registers
***************************************/

/* Device specific registers */
#if (CY_PSOC4)

    #define CharacterLCD_PORT_DR_REG           (*(reg32 *) CharacterLCD_LCDPort__DR)  /* Data Output Register */
    #define CharacterLCD_PORT_DR_PTR           ( (reg32 *) CharacterLCD_LCDPort__DR)
    #define CharacterLCD_PORT_PS_REG           (*(reg32 *) CharacterLCD_LCDPort__PS)  /* Pin State Register */
    #define CharacterLCD_PORT_PS_PTR           ( (reg32 *) CharacterLCD_LCDPort__PS)
    
    #define CharacterLCD_PORT_PC_REG           (*(reg32 *) CharacterLCD_LCDPort__PC)
    #define CharacterLCD_PORT_PC_PTR           (*(reg32 *) CharacterLCD_LCDPort__PC)
    
#else

    #define CharacterLCD_PORT_DR_REG           (*(reg8 *) CharacterLCD_LCDPort__DR)  /* Data Output Register */
    #define CharacterLCD_PORT_DR_PTR           ( (reg8 *) CharacterLCD_LCDPort__DR)
    #define CharacterLCD_PORT_PS_REG           (*(reg8 *) CharacterLCD_LCDPort__PS)  /* Pin State Register */
    #define CharacterLCD_PORT_PS_PTR           ( (reg8 *) CharacterLCD_LCDPort__PS)

    #define CharacterLCD_PORT_DM0_REG          (*(reg8 *) CharacterLCD_LCDPort__DM0) /* Port Drive Mode 0 */
    #define CharacterLCD_PORT_DM0_PTR          ( (reg8 *) CharacterLCD_LCDPort__DM0)
    #define CharacterLCD_PORT_DM1_REG          (*(reg8 *) CharacterLCD_LCDPort__DM1) /* Port Drive Mode 1 */
    #define CharacterLCD_PORT_DM1_PTR          ( (reg8 *) CharacterLCD_LCDPort__DM1)
    #define CharacterLCD_PORT_DM2_REG          (*(reg8 *) CharacterLCD_LCDPort__DM2) /* Port Drive Mode 2 */
    #define CharacterLCD_PORT_DM2_PTR          ( (reg8 *) CharacterLCD_LCDPort__DM2)

#endif /* CY_PSOC4 */


/***************************************
*       Register Constants
***************************************/

/* SHIFT must be 1 or 0 */
#if (0 == CharacterLCD_LCDPort__SHIFT)
    #define CharacterLCD_PORT_SHIFT               (0x00u)
#else
    #define CharacterLCD_PORT_SHIFT               (0x01u)
#endif /* (0 == CharacterLCD_LCDPort__SHIFT) */

#define CharacterLCD_PORT_MASK                ((uint8) (CharacterLCD_LCDPort__MASK))

#if (CY_PSOC4)

    #define CharacterLCD_DM_PIN_STEP              (3u)
    /* Hi-Z Digital is defined by the value of "001b" and this should be set for
    * four data pins, in this way we get - 0x00000249. A similar Strong drive
    * is defined with "110b" so we get 0x00000DB6.
    */
    #define CharacterLCD_HIGH_Z_DATA_DM           ((0x00000249ul) << (CharacterLCD_PORT_SHIFT *\
                                                                          CharacterLCD_DM_PIN_STEP))
    #define CharacterLCD_STRONG_DATA_DM           ((0x00000DB6ul) << (CharacterLCD_PORT_SHIFT *\
                                                                          CharacterLCD_DM_PIN_STEP))
    #define CharacterLCD_DATA_PINS_MASK           (0x00000FFFul)
    #define CharacterLCD_DM_DATA_MASK             ((uint32) (CharacterLCD_DATA_PINS_MASK << \
                                                      (CharacterLCD_PORT_SHIFT * CharacterLCD_DM_PIN_STEP)))

#else

    /* Drive Mode register values for High Z */
    #define CharacterLCD_HIGH_Z_DM0               (0xFFu)
    #define CharacterLCD_HIGH_Z_DM1               (0x00u)
    #define CharacterLCD_HIGH_Z_DM2               (0x00u)

    /* Drive Mode register values for High Z Analog */
    #define CharacterLCD_HIGH_Z_A_DM0             (0x00u)
    #define CharacterLCD_HIGH_Z_A_DM1             (0x00u)
    #define CharacterLCD_HIGH_Z_A_DM2             (0x00u)

    /* Drive Mode register values for Strong */
    #define CharacterLCD_STRONG_DM0               (0x00u)
    #define CharacterLCD_STRONG_DM1               (0xFFu)
    #define CharacterLCD_STRONG_DM2               (0xFFu)

#endif /* CY_PSOC4 */

/* Pin Masks */
#define CharacterLCD_RS                     ((uint8) \
                                                (((uint8) 0x20u) << CharacterLCD_LCDPort__SHIFT))
#define CharacterLCD_RW                     ((uint8) \
                                                (((uint8) 0x40u) << CharacterLCD_LCDPort__SHIFT))
#define CharacterLCD_E                      ((uint8) \
                                                (((uint8) 0x10u) << CharacterLCD_LCDPort__SHIFT))
#define CharacterLCD_READY_BIT              ((uint8) \
                                                (((uint8) 0x08u) << CharacterLCD_LCDPort__SHIFT))
#define CharacterLCD_DATA_MASK              ((uint8) \
                                                (((uint8) 0x0Fu) << CharacterLCD_LCDPort__SHIFT))

/* These names are obsolete and will be removed in future revisions */
#define CharacterLCD_PORT_DR                  CharacterLCD_PORT_DR_REG
#define CharacterLCD_PORT_PS                  CharacterLCD_PORT_PS_REG
#define CharacterLCD_PORT_DM0                 CharacterLCD_PORT_DM0_REG
#define CharacterLCD_PORT_DM1                 CharacterLCD_PORT_DM1_REG
#define CharacterLCD_PORT_DM2                 CharacterLCD_PORT_DM2_REG


/***************************************
*       Obsolete function names
***************************************/
#if(CharacterLCD_CONVERSION_ROUTINES == 1u)
    /* This function names are obsolete and will be removed in future 
    * revisions of the component.
    */
    #define CharacterLCD_PrintDecUint16(x)   CharacterLCD_PrintNumber(x)  
    #define CharacterLCD_PrintHexUint8(x)    CharacterLCD_PrintInt8(x)
    #define CharacterLCD_PrintHexUint16(x)   CharacterLCD_PrintInt16(x)        

#endif /* CharacterLCD_CONVERSION_ROUTINES == 1u */

#endif /* CY_CHARLCD_CharacterLCD_H */


/* [] END OF FILE */
