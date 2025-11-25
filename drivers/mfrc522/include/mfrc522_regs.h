/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_mfrc522
 * @brief   Register definitions for the MFRC522 controller
 *
 * The documentation is using the following terms:
 *   PCD  = proximity coupling device
 *   PICC = proximity integrated circuit card
 *
 * @author  Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */

#ifndef MFRC522_REGS_H
#define MFRC522_REGS_H

#include "bitarithm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Register definitions
 * @{
 */
typedef enum {
    /* Command and status register definitions (page 36, table 20) */
    MFRC522_REG_COMMAND             = 0x01, /**< Starts and stops command execution */
    MFRC522_REG_COML_EN             = 0x02, /**< Enable and disable interrupt request control bits */
    MFRC522_REG_DIVL_EN             = 0x03, /**< Enable and disable interrupt request control bits */
    MFRC522_REG_COM_IRQ             = 0x04, /**< Interrupt request bits */
    MFRC522_REG_DIV_IRQ             = 0x05, /**< Interrupt request bits */
    MFRC522_REG_ERROR               = 0x06, /**< Error bits showing the error status of the last command executed */
    MFRC522_REG_STATUS_1            = 0x07, /**< Communication status bits */
    MFRC522_REG_STATUS_2            = 0x08, /**< Receiver and transmitter status bits */
    MFRC522_REG_FIFO_DATA           = 0x09, /**< Input and output of 64 byte FIFO buffer */
    MFRC522_REG_FIFO_LEVEL          = 0x0A, /**< Number of bytes stored in the FIFO buffer */
    MFRC522_REG_WATER_LEVEL         = 0x0B, /**< Level for FIFO underflow and overflow warning */
    MFRC522_REG_CONTROL             = 0x0C, /**< Miscellaneous control registers */
    MFRC522_REG_BIT_FRAMING         = 0x0D, /**< Adjustments for bit-oriented frames */
    MFRC522_REG_COLL                = 0x0E, /**< Bit position of the first bit-collision detected on the RF interface */

    /* Command register definitions (page 36, table 20) */
    MFRC522_REG_MODE                = 0x11, /**< Defines general modes for transmitting and receiving */
    MFRC522_REG_TX_MODE             = 0x12, /**< Defines transmission data rate and framing */
    MFRC522_REG_RX_MODE             = 0x13, /**< Defines reception data rate and framing */
    MFRC522_REG_TX_CONTROL          = 0x14, /**< Controls the logical behavior of the antenna driver pins TX1 and TX2 */
    MFRC522_REG_TX_ASK              = 0x15, /**< Controls the setting of the transmission modulation */
    MFRC522_REG_TX_SEL              = 0x16, /**< Selects the internal sources for the antenna driver */
    MFRC522_REG_RX_SEL              = 0x17, /**< Selects internal receiver settings */
    MFRC522_REG_RX_THRESHHOLD       = 0x18, /**< Selects thresholds for the bit decoder */
    MFRC522_REG_DEMOD               = 0x19, /**< Defines demodulator settings */
    MFRC522_REG_MF_TX               = 0x1C, /**< Controls some MIFARE communication transmit parameters */
    MFRC522_REG_MF_RX               = 0x1D, /**< Controls some MIFARE communication receive parameters */
    MFRC522_REG_SERIAL_SPEED        = 0x1F, /**< Selects the speed of the serial UART interface */

    /* Configuration register definitions (page 36-37, table 20) */
    MFRC522_REG_CRC_RESULT_MSB      = 0x21, /**< Shows the higher 8 bits of the CRC calculation */
    MFRC522_REG_CRC_RESULT_LSB      = 0x22, /**< Shows the lower 8 bits of the CRC calculation */
    MFRC522_REG_MOD_WIDTH           = 0x24, /**< Controls the ModWidth setting */
    MFRC522_REG_RF_CFG              = 0X26, /**< Configures the receiver gain */
    MFRC522_REG_GS_N                = 0x27, /**< Selects the conductance of the antenna driver pins TX1 and TX2 for modulation */
    MFRC522_REG_CW_GS_P             = 0x28, /**< Defines the conductance of the p-driver output during periods of no modulation */
    MFRC522_REG_MOD_GS_P            = 0x29, /**< Defines the conductance of the p-driver output during periods of modulation */
    MFRC522_REG_T_MODE              = 0x2A, /**< Defines settings for the internal timer */
    MFRC522_REG_T_PRESCALER         = 0x2B, /**< Defines settings for the internal timer */
    MFRC522_REG_T_RELOAD_MSB        = 0x2C, /**< Defines the higher 8 bits of 16-bit timer reload value */
    MFRC522_REG_T_RELOAD_LSB        = 0x2D, /**< Defines the lower 8 bits of 16-bit timer reload value */
    MFRC522_REG_T_COUNTER_VAL_MSB   = 0x2E, /**< Shows the higher 8 bits of 16-bit timer value */
    MFRC522_REG_T_COUNTER_VAL_LSB   = 0x2F, /**< Shows the lower 8 bits of 16-bit timer value */

    /* Test register definitions (page 37, table 20) */
    MFRC522_REG_TEST_SEL_1          = 0x31, /**< General test signal configuration */
    MFRC522_REG_TEST_SEL_2          = 0x32, /**< General test signal configuration and PRBS control */
    MFRC522_REG_TEST_PIN_EN         = 0x33, /**< Enables pin output driver on pins D1 to D7 */
    MFRC522_REG_TEST_PIN_VALUE      = 0x34, /**< Defines the values for D1 to D7 when it is used as an I/O bus */
    MFRC522_REG_TEST_BUS            = 0x35, /**< Shows the status of the internal test bus */
    MFRC522_REG_AUTO_TEST           = 0x36, /**< Controls the digital self test */
    MFRC522_REG_VERSION             = 0x37, /**< Shows the software version */
    MFRC522_REG_ANALOG_TEST         = 0x38, /**< Controls the pins AUX1 and AUX2 */
    MFRC522_REG_TEST_DAC_1          = 0x39, /**< Defines the test value for TestDAC1 */
    MFRC522_REG_TEST_DAC2           = 0x3A, /**< Defines the test value for TestDAC2 */
    MFRC522_REG_TEST_ADC            = 0x3B, /**< Shows the value of ADC I and Q channels */
} mfrc522_pcd_register_t;
/** @} */

/**
 * @brief Command definitions
 * @{
 */
typedef enum {
    /* Command codes (page 70, table 149) */
    MFRC522_CMD_IDLE                = 0x00, /**< No action, cancels current command execution */
    MFRC522_CMD_MEM                 = 0x01, /**< Stores 25 bytes into the internal buffer */
    MFRC522_CMD_GENERATE_RANDOM_ID  = 0x02, /**< Generates a 10-byte random ID number */
    MFRC522_CMD_CALC_CRC            = 0x03, /**< Activates the CRC coprocessor or performs a self test */
    MFRC522_CMD_TRANSMIT            = 0x04, /**< Transmits data from the FIFO buffer */
    MFRC522_CMD_NO_CMD_CHANGE       = 0x07, /**< No command change, can be used to modify the CommandReg register bits without affecting the command, for example, the PowerDown bit */
    MFRC522_CMD_RECEIVE             = 0x08, /**< Activates the receiver circuits */
    MFRC522_CMD_TRANSCEIVE          = 0x0C, /**< Transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission */
    MFRC522_CMD_MF_AUTHENT          = 0x0E, /**< Performs the MIFARE standard authentication as a reader */
    MFRC522_CMD_SOFT_RESET          = 0x0F, /**< Resets the MFRC522 */
} mfrc522_pcd_command_t;
/** @} */

/**
 * @brief Receiver gain definitions
 * @{
 */
typedef enum {
    /* Receiver gain definition (page 70, table 149) */
    MFRC522_RXGAIN_18_DB    = 0x00,                     /**< 18 dB, minimum */
    MFRC522_RXGAIN_23_DB    = 0x01,                     /**< 23 dB */
    MFRC522_RXGAIN_18_DB_2  = 0x02,                     /**< 18 dB, duplicate for MFRC522_RXGAIN_18_DB */
    MFRC522_RXGAIN_23_DB_2  = 0x03,                     /**< 23 dB, duplicate for MFRC522_RXGAIN_23_DB */
    MFRC522_RXGAIN_33_DB    = 0x04,                     /**< 33 dB, average, and typical default */
    MFRC522_RXGAIN_38_DB    = 0x05,                     /**< 38 dB */
    MFRC522_RXGAIN_43_DB    = 0x06,                     /**< 43 dB */
    MFRC522_RXGAIN_48_DB    = 0x07,                     /**< 48 dB, maximum */
    MFRC522_RXGAIN_MIN      = MFRC522_RXGAIN_18_DB,     /**< 18 dB, minimum, convenience for MFRC522_RXGAIN_18_DB */
    MFRC522_RXGAIN_AVG      = MFRC522_RXGAIN_33_DB,     /**< 33 dB, average, convenience for MFRC522_RXGAIN_33_DB */
    MFRC522_RXGAIN_MAX      = MFRC522_RXGAIN_48_DB,     /**< 48 dB, maximum, convenience for MFRC522_RXGAIN_48_DB */
} mfrc522_pcd_rx_gain_t;
/** @} */

/**
 * @brief PICC command definitions
 * @{
 */
typedef enum {
    /* Commands used by the PCD to manage communication with several PICCs (ISO 14443-3, Type A, section 6.4) */
    MFRC522_PICC_CMD_ISO_14443_REQA     = 0x26,     /**<  REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame. */
    MFRC522_PICC_CMD_ISO_14443_WUPA     = 0x52,     /**<  Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame. */
    MFRC522_PICC_CMD_ISO_14443_SEL_CL1  = 0x93,     /**<  Anti collision/Select, Cascade Level 1 */
    MFRC522_PICC_CMD_ISO_14443_SEL_CL2  = 0x95,     /**<  Anti collision/Select, Cascade Level 2 */
    MFRC522_PICC_CMD_ISO_14443_SEL_CL3  = 0x97,     /**<  Anti collision/Select, Cascade Level 3 */
    MFRC522_PICC_CMD_ISO_14443_HLTA     = 0x50,     /**<  HLTA command, Type A. Instructs an ACTIVE PICC to go to state HALT. */
    MFRC522_PICC_CMD_ISO_14443_RATS     = 0xE0,     /**<  Request command for Answer To Reset */

    /* Commands used for MIFARE Classic EV1 (https://www.nxp.com/docs/en/data-sheet/MF1S50YYX_V1.pdf, page 13, table 9) */
    MFRC522_PICC_CMD_MF_AUTH_KEY_A      = 0x60,     /**<  Authentication with Key A */
    MFRC522_PICC_CMD_MF_AUTH_KEY_B      = 0x61,     /**<  Authentication with Key B */
    MFRC522_PICC_CMD_MF_PERS_UID_USAGE  = 0x40,     /**<  Set anti-collision, selection and authentication behaviour */
    MFRC522_PICC_CMD_MF_SET_MOD_TYPE    = 0x43,     /**<  Set load modulation strength */
    MFRC522_PICC_CMD_MF_READ            = 0x30,     /**<  Read one block of 16 bytes */
    MFRC522_PICC_CMD_MF_WRITE           = 0xA0,     /**<  Write one block of 16 bytes */
    MFRC522_PICC_CMD_MF_DECREMENT       = 0xC0,     /**<  Decrement: Subtracts the operand from the value of the addressed block, and stores the result in the Transfer Buffer */
    MFRC522_PICC_CMD_MF_INCREMENT       = 0xC1,     /**<  Increment: Adds the operand to the value of the addressed block, and stores the result in the Transfer Buffer */
    MFRC522_PICC_CMD_MF_RESTORE         = 0xC2,     /**<  Restore: Copies the value of the addressed block into the Transfer Buffer.  */
    MFRC522_PICC_CMD_MF_TRANSFER        = 0xB0,     /**<  Write the value from the Transfer Buffer into destination block */

    /* Commands used for MIFARE Ultralight (https://www.nxp.com/docs/en/data-sheet/MF0ICU1.pdf, Section 7.6) */
    MFRC522_PICC_CMD_MF_UL_WRITE        = 0xA2,                         /**<  Transfers 16 bytes, but only writes least significant 4 bytes */
    MFRC522_PICC_CMD_MF_UL_READ         = MFRC522_PICC_CMD_MF_READ,     /**<  Read one block of 16 bytes */
    MFRC522_PICC_CMD_MF_UL_COMPAT_WRITE = MFRC522_PICC_CMD_MF_WRITE,    /**<  Write one block of 16 bytes */
} mfrc522_picc_command_t;
/** @} */

/**
 * @brief FIFO buffer size
 */
#define MFRC522_FIFO_BUF_SIZE   64

/**
 * @brief Cascade Tag, used during anti collision.
 */
# define MFRC522_PICC_CASCADE_TAG   0x88

/**
 * @brief 1 = Analog part of the receiver is switched off
 */
#define MFRC522_BIT_COMMAND_RCV_OFF         BIT5

/**
 * @brief 0 = MFRC522 starts the wake up procedure during which this bit is read
 *            as a logic 1; it is read as a logic 0 when the MFRC522 is ready;
 *            see Section 8.6.2 on page 33. Remark: The PowerDown bit cannot be
 *            set when the SoftReset command is activated
 *        1 = Soft power-down mode entered.
 *
 */
#define MFRC522_BIT_COMMAND_POWER_DOWN      BIT4

/**
 * @brief Activates a command based on the Command value; reading this register
 *        shows which command is executed; see Section 10.3 on page 70
 *
 *        MFRC522_REG_COMMAND[3:0]
 *
 */
#define MFRC522_BITMASK_COMMAND_POWER_DOWN  0x0F



/**
 * @brief 0 = Signal on pin IRQ is equal to the IRq bit; in combination with the
 *            DivIEnReg register’s IRqPushPull bit, the default value of logic 1
 *            ensures that the output level on pin IRQ is 3-state
 *        1 = Signal on pin IRQ is inverted with respect to the Status1Reg
 *            register’s IRq bit
 *
 */
#define MFRC522_BIT_COML_EN_IRQ_INV         BIT7

/**
 * @brief Allows the transmitter interrupt request (TxIRq bit) to be propagated
 *        to pin IRQ
 */
#define MFRC522_BIT_COML_EN_TX_I_EN         BIT6

/**
 * @brief Allows the receiver interrupt request (RxIRq bit) to be propagated to
 *        pin IRQ
 */
#define MFRC522_BIT_COML_EN_RX_I_EN         BIT5

/**
 * @brief Allows the idle interrupt request (IdleIRq bit) to be propagated to
 *        pin IRQ
 */
#define MFRC522_BIT_COML_EN_IDLE_I_EN       BIT4

/**
 * @brief Allows the high alert interrupt request (HiAlertIRq bit) to be
 *        propagated to pin IRQ
 */
#define MFRC522_BIT_COML_EN_HI_ALERT_I_EN   BIT3

/**
 * @brief Allows the low alert interrupt request (LoAlertIRq bit) to be
 *        propagated to pin IRQ
 */
#define MFRC522_BIT_COML_EN_LO_ALERT_I_EN   BIT2

/**
 * @brief Allows the error interrupt request (ErrIRq bit) to be propagated to
 *        pin IRQ
 */
#define MFRC522_BIT_COML_EN_ERR_I_EN        BIT1

/**
 * @brief Allows the timer interrupt request (TimerIRq bit) to be propagated to
 *        pin IRQ
 */
#define MFRC522_BIT_COML_EN_TIMER_I_EN      BIT0



/**
 * @brief 0 = Pin IRQ is an open-drain output pin
 *        1 = Pin IRQ is a standard CMOS output pin
 */
#define MFRC522_BIT_DIVL_EN_IRQ_PUSH_PULL   BIT7

/**
 * @brief Allows the MFIN active interrupt request to be propagated to pin IRQ
 */
#define MFRC522_BIT_DIVL_EN_MFIN_ACT_I_EN   BIT4

/**
 * @brief Allows the CRC interrupt request, indicated by the DivIrqReg
 *        register’s CRCIRq bit, to be propagated to pin IRQ
 */
#define MFRC522_BIT_DIVL_EN_CRC_I_EN        BIT2



/**
 * @brief 0 = Indicates that the marked bits in the ComIrqReg register are cleared
 *        1 = Indicates that the marked bits in the ComIrqReg register are set
 *
 */
#define MFRC522_BIT_COM_IRQ_SET_1           BIT7

/**
 * @brief 1 = Set immediately after the last bit of the transmitted data was
 *            sent out
 */
#define MFRC522_BIT_COM_IRQ_TX_IRQ          BIT6

/**
 * @brief 1 = Receiver has detected the end of a valid data stream if the
 *            RxModeReg register’s RxNoErr bit is set to logic 1, the RxIRq bit
 *            is only set to logic 1 when data bytes are available in the FIFO
 */
#define MFRC522_BIT_COM_IRQ_RX_IRQ          BIT5

/**
 * @brief 1 = If a command terminates, for example, when the CommandReg changes
 *            its value from any command to the Idle command (see Table 149 on
 *            page 70) if an unknown command is started, the CommandReg register
 *            Command[3:0] value changes to the idle state and the IdleIRq bit
 *            is set. The microcontroller starting the Idle command does not set
 *            the IdleIRq bit
 */
#define MFRC522_BIT_COM_IRQ_IDLE_IRQ        BIT4

/**
 * @brief 1 = The Status1Reg register’s HiAlert bit is set in opposition to the
 *            HiAlert bit, the HiAlertIRq bit stores this event and can only be
 *            reset as indicated by the Set1 bit in this register
 */
#define MFRC522_BIT_COM_IRQ_HI_ALERT_IRQ    BIT3

/**
 * @brief 1 = Status1Reg register’s LoAlert bit is set in opposition to the
 *            LoAlert bit, the LoAlertIRq bit stores this event and can only be
 *            reset as indicated by the Set1 bit in this register
 */
#define MFRC522_BIT_COM_IRQ_LO_ALERT_IRQ    BIT2

/**
 * @brief 1 = Any error bit in the ErrorReg register is set
 */
#define MFRC522_BIT_COM_IRQ_ERR_IRQ         BIT1

/**
 * @brief 1 = The timer decrements the timer value in register TCounterValReg to zero
 */
#define MFRC522_BIT_COM_IRQ_TIMER_IRQ       BIT0



/**
 * @brief 0 = Indicates that the marked bits in the DivIrqReg register are cleared
 *        1 = Indicates that the marked bits in the DivIrqReg register are set
 */
#define MFRC522_BIT_DIV_IRQ_SET_2           BIT7

/**
 * @brief 1 = MFIN is active; this interrupt is set when either a rising or
 *            falling signal edge is detected
 */
#define MFRC522_BIT_DIV_IRQ_MFIN_ACT_IRQ    BIT4

/**
 * @brief 1 = The CalcCRC command is active and all data is processed
 */
#define MFRC522_BIT_DIV_IRQ_CRC_IRQ         BIT2



/**
 * @brief 1 = Data is written into the FIFO buffer by the host during the
 *            MFAuthent command or if data is written into the FIFO buffer by
 *            the host during the time between sending the last bit on the RF
 *            interface and receiving the last bit on the RF interface
 */
#define MFRC522_BIT_ERROR_WR_ERR        BIT7

/**
 * @brief 1 = Internal temperature sensor detects overheating, in which case the
 *            antenna drivers are automatically switched off
 */
#define MFRC522_BIT_ERROR_TEMP_ERR      BIT6

/**
 * @brief 1 = The host or a MFRC522’s internal state machine (e.g. receiver)
 *            tries to write data to the FIFO buffer even though it is already full
 */
#define MFRC522_BIT_ERROR_BUFFER_OVFL   BIT4

/**
 * @brief 1 = A bit-collision is detected; cleared automatically at receiver
 *            start-up phase; only valid during the bitwise anticollision at
 *            106 kBd; always set to logic 0 during communication protocols at
 *            212 kBd, 424 kBd and 848 kBd
 */
#define MFRC522_BIT_ERROR_COLL_ERR      BIT3

/**
 * @brief 1 = The RxModeReg register’s RxCRCEn bit is set and the CRC
 *            calculation fails; automatically cleared to logic 0 during
 *            receiver start-up phase
 */
#define MFRC522_BIT_ERROR_CRC_ERR       BIT2

/**
 * @brief 1 = Parity check failed; automatically cleared during receiver
 *            start-up phase; only valid for ISO/IEC 14443 A/MIFARE
 *            communication at 106 kBd
 */
#define MFRC522_BIT_ERROR_PARITY_ERR    BIT1

/**
 * @brief 1 = Set to logic 1 if the SOF is incorrect; automatically cleared
 *            during receiver start-up phase; bit is only valid for 106 kBd;
 *            during the MFAuthent command, the ProtocolErr bit is set to
 *            logic 1 if the number of bytes received in one data stream is
 *            incorrect
 */
#define MFRC522_BIT_ERROR_PROTOCOL_ERR  BIT0



/**
 * @brief 1 = The CRC result is zero; for data transmission and reception, the
 *            CRCOk bit is undefined: use the ErrorReg register’s CRCErr bit;
 *            indicates the status of the CRC coprocessor, during calculation
 *            the value changes to logic 0, when the calculation is done
 *            correctly the value changes to logic 1
 */
#define MFRC522_BIT_STATUS_1_CRC_OK     BIT6

/**
 * @brief 1 = The CRC calculation has finished; only valid for the CRC
 *            coprocessor calculation using the CalcCRC command
 */
#define MFRC522_BIT_STATUS_1_CRC_READY  BIT5

/**
 * @brief Indicates if any interrupt source requests attention with respect to
 *        the setting of the interrupt enable bits: see the ComIEnReg and
 *        DivIEnReg registers
 */
#define MFRC522_BIT_STATUS_1_IRQ       BIT4

/**
 * @brief 1 = MFRC522’s timer unit is running, i.e. the timer will decrement the
 *            TCounterValReg register with the next timer clock; Remark: in
 *            gated mode, the TRunning bit is set to logic 1 when the timer is
 *            enabled by TModeReg register’s TGated[1:0] bits; this bit is not
 *            influenced by the gated signal
 */
#define MFRC522_BIT_STATUS_1_T_RUNNING  BIT3

/**
 * @brief 1 = The number of bytes stored in the FIFO buffer corresponds to equation:
 *            HiAlert = (64 - FIFOLength) <= WaterLevel
 *            example:
 *            FIFO length = 60, WaterLevel = 4 -> HiAlert = 1
 *            FIFO length = 59, WaterLevel = 4 -> HiAlert = 0
 */
#define MFRC522_BIT_STATUS_1_HI_ALERT   BIT1

/**
 * @brief 1 = The number of bytes stored in the FIFO buffer corresponds to equation:
 *            LoAlert = FIFOLength <= WaterLevel
 *            example:
 *            FIFO length = 4, WaterLevel = 4 -> LoAlert = 1
 *            FIFO length = 5, WaterLevel = 4 -> LoAlert = 0
 */
#define MFRC522_BIT_STATUS_1_LO_ALERT   BIT0



/**
 * @brief 1 = Clears the temperature error if the temperature is below the alarm
 *            limit of 125 °C
 */
#define MFRC522_BIT_STATUS_2_TEMP_SENS_CLEAR    BIT7

/**
 * @brief I2C-bus input filter settings:
 *        0 = the I2C-bus input filter is set to the I2C-bus protocol used
 *        1 = the I2C-bus input filter is set to the High-speed mode independent
 *            of the I2C-bus protocol
 */
#define MFRC522_BIT_STATUS_2_I2C_FORCE_HS       BIT6

/**
 * @brief Indicates that the MIFARE Crypto1 unit is switched on and therefore
 *        all data communication with the card is encrypted; can only be set to
 *        logic 1 by a successful execution of the MFAuthent command; only valid
 *        in Read/Write mode for MIFARE standard cards; this bit is cleared by
 *        software
 */
#define MFRC522_BIT_STATUS_2_MF_CRYPTO_1_ON     BIT3

/**
 * @brief Shows the state of the transmitter and receiver state machines:
 *
 *        000 idle
 *        001 wait for the BitFramingReg register’s StartSend bit
 *        010 TxWait: wait until RF field is present if the TModeReg register’s
 *            TxWaitRF bit is set to logic 1; the minimum time for TxWait is
 *            defined by the TxWaitReg register
 *        011 transmitting
 *        100 RxWait: wait until RF field is present if the TModeReg register’s
 *            TxWaitRF bit is set to logic 1; the minimum time for RxWait is
 *            defined by the RxWaitReg register
 *        101 wait for data
 *        110 receiving
 *
 *        MFRC522_REG_STATUS_2[2:0]
 */
#define MFRC522_BITMASK_STATUS_2_MODEM_STATE_2  0x07



/**
 * @brief Data input and output port for the internal 64-byte FIFO buffer;
 *        FIFO buffer acts as parallel in/parallel out converter for all serial
 *        data stream inputs and outputs
 *
 *        MFRC522_REG_FIFO_DATA[7:0]
 */
#define MFRC522_BITMASK_FIFO_DATA   0xFF



/**
 * @brief 1 = Immediately clears the internal FIFO buffer’s read and write
 *            pointer and ErrorReg register’s BufferOvfl bit; reading this bit
 *            always returns 0
 */
#define MFRC522_BIT_FIFO_LEVEL_FLUSH_BUFFER BIT7

/**
 * @brief Indicates the number of bytes stored in the FIFO buffer; writing to the
 *        FIFODataReg register increments and reading decrements the FIFOLevel
 *        value
 *
 *        MFRC522_REG_FIFO_LEVEL[6:0]
 */
#define MFRC522_BITMASK_FIFO_LEVEL_FIFO_LEVEL  0x7F



/**
 * @brief Defines a warning level to indicate a FIFO buffer overflow or underflow:
 *        Status1Reg register’s HiAlert bit is set to logic 1 if the remaining
 *        number of bytes in the FIFO buffer space is equal to, or less than the
 *        defined number of WaterLevel bytes.
 *        Status1Reg register’s LoAlert bit is set to logic 1 if equal to, or
 *        less than the WaterLevel bytes in the FIFO buffer.
 *        Remark: to calculate values for HiAlert and LoAlert see Section
 *        9.3.1.8 on page 42.
 *
 *        MFRC522_REG_WATER_LEVEL[5:0]
 */
#define MFRC522_BITMASK_WATER_LEVEL_WATER_LEVEL     0x3F



/**
 * @brief 1 = Timer stops immediately;
 *            reading this bit always returns it to logic 0
 */
#define MFRC522_BIT_CONTROL_T_STOP_NOW          BIT7

/**
 * @brief 1 = Timer starts immediately;
 *            reading this bit always returns it to logic 0
 */
#define MFRC522_BIT_CONTROL_T_START_NOW         BIT6

/**
 * @brief Indicates the number of valid bits in the last received byteif this
 *        value is 000b, the whole byte is valid
 *
 *        MFRC522_REG_CONTROL[2:0]
 */
#define MFRC522_BITMASK_CONTROL_RX_LAST_BITS    0x07



/**
 * @brief Starts the transmission of data; only valid in combination with the
 *        Transceive command
 */
#define MFRC522_BIT_BIT_FRAMING_START_SEND      BIT7

/**
 * @brief Used for reception of bit-oriented frames: defines the bit position
 *        for the first bit received to be stored in the FIFO buffer
 *        example:
 *        0 = LSB of the received bit is stored at bit position 0, the second
 *            received bit is stored at bit position 1
 *        1 = LSB of the received bit is stored at bit position 1, the second
 *            received bit is stored at bit position 2
 *        7 = LSB of the received bit is stored at bit position 7, the second
 *            received bit is stored in the next byte that follows at bit position 0;
 *
 *        These bits are only to be used for bitwise anticollision at 106 kBd,
 *        for all other modes they are set to 0
 *
 *        MFRC522_REG_BIT_FRAMING[6:4]
 */
#define MFRC522_BITMASK_BIT_FRAMING_RX_ALIGN    0x70

/**
 * @brief Used for transmission of bit oriented frames: defines the number of
 *        bits of the last byte that will be transmitted; 000b indicates that
 *        all bits of the last byte will be transmitted
 *
 *        BIT_FRAMING[2:0]
 */
#define MFRC522_BIT_BIT_FRAMING_TX_LAST_BITS    0x07



/**
 * @brief 0 = All received bits will be cleared after a collision; only used
 *            during bitwise anticollision at 106 kBd, otherwise it is set to logic 1
 */
#define MFRC522_BIT_COLL_VALUES_AFTER_COLL      BIT7

/**
 * @brief 1 = No collision detected or the position of the collision is out of
 *            the range of CollPos[4:0]
 */
#define MFRC522_BIT_COLL_COLL_POS_NOT_VALID     BIT5

/**
 * @brief Shows the bit position of the first detected collision in a received frame;
 *        only data bits are interpreted; example:
 *        00h = indicates a bit-collision in the 32nd bit
 *        01h = indicates a bit-collision in the 1st bit
 *        08h = indicates a bit-collision in the 8th bit
 *
 *        These bits will only be interpreted if the CollPosNotValid bit is set
 *        to logic 0
 *
 *        MFRC522_REG_COLL[4:0]
 */
#define MFRC522_BITMASK_COLL_COLL_POS           0x1F



/**
 * @brief 1 = CRC coprocessor calculates the CRC with MSB first; in the
 *            CRCResultReg register the values for the CRCResultMSB[7:0] bits
 *            and the CRCResultLSB[7:0] bits are bit reversed; Remark: during RF
 *            communication this bit is ignored
 */
#define MFRC522_BIT_MODE_MSB_FIRST          BIT7

/**
 * @brief 1 = Transmitter can only be started if an RF field is generated
 */
#define MFRC522_BIT_MODE_TX_WAIT_RF         BIT5

/**
 * @brief Defines the polarity of pin MFIN; Remark: the internal envelope signal
 *        is encoded active LOW, changing this bit generates a MFinActIRq event
 *
 *        0 = polarity of pin MFIN is active LOW
 *        1 = polarity of pin MFIN is active HIGH
 */
#define MFRC522_BIT_MODE_POL_MFIN           BIT3

/**
 * @brief Defines the preset value for the CRC coprocessor for the CalcCRC
 *        command; Remark: during any communication, the preset values are
 *        selected automatically according to the definition of bits in the
 *        RxModeReg and TxModeReg registers
 *
 *        00 = 0000h
 *        01 = 6363h
 *        10 = A671h
 *        11 = FFFFh
 *
 *        MFRC522_REG_MODE[1:0]
 */
#define MFRC522_BITMASK_MODE_CRC_PRESET     0x03



/**
 * @brief 1 = Enables CRC generation during data transmission; Remark: can only
 *            be set to logic 0 at 106 kBd
 */
#define MFRC522_BIT_TX_MODE_TX_CRC_EN       BIT7

/**
 * @brief Defines the bit rate during data transmission; the MFRC522 handles
 *        transfer speeds up to 848 kBd
 *
 *        000 = 106 kBd
 *        001 = 212 kBd
 *        010 = 424 kBd
 *        011 = 848 kBd
 *
 *        MFRC522_REG_TX_MODE[6:4]
 */
#define MFRC522_BITMASK_TX_MODE_TX_SPEED    0x70

/**
 * @brief 1 = Modulation of transmitted data is inverted
 */
#define MFRC522_BIT_TX_MODE_INV_MOD         BIT3



/**
 * @brief 1 = Enables the CRC calculation during reception; Remark: can only be
 *            set to logic 0 at 106 kBd
 */
#define MFRC522_BIT_RX_MODE_RX_CRC_EN       BIT7

/**
 * @brief Defines the bit rate while receiving data; the MFRC522 handles
 *        transfer speeds up to 848 kBd
 *
 *        000 = 106 kBd
 *        001 = 212 kBd
 *        010 = 424 kBd
 *        011 = 848 kBd
 *
 *        MFRC522_REG_RX_MODE[6:4]
 */
#define MFRC522_BITMASK_RX_MODE_RX_SPEED    0x70

/**
 * @brief 1 = An invalid received data stream (less than 4 bits received) will be
 *            ignored and the receiver remains active
 */
#define MFRC522_BIT_RX_MODE_RX_NO_ERR       BIT3

/**
 * @brief 0 = Receiver is deactivated after receiving a data frame
 *        1 = Able to receive more than one data frame; only valid for data
 *            rates above 106 kBd in order to handle the polling command; after
 *            setting this bit the Receive and Transceive commands will not
 *            terminate automatically. Multiple reception can only be
 *            deactivated by writing any command (except the Receive command) to
 *            the CommandReg register, or by the host clearing the bit; if set
 *            to logic 1, an error byte is added to the FIFO buffer at the end
 *            of a received data stream which is a copy of the ErrorReg register
 *            value. For the MFRC522 version 2.0 the CRC status is reflected in
 *            the signal CRCOk, which indicates the actual status of the CRC
 *            coprocessor. For the MFRC522 version 1.0 the CRC status is
 *            reflected in the signal CRCErr.
 */
#define MFRC522_BIT_RX_MODE_RX_MULTIPLE     BIT2



/**
 * @brief 1 = Output signal on pin TX2 inverted when driver TX2 is enabled
 */
#define MFRC522_BIT_TX_CONTROL_INV_TX2_RF_ON    BIT7

/**
 * @brief 1 = Output signal on pin TX1 inverted when driver TX1 is enabled
 */
#define MFRC522_BIT_TX_CONTROL_INV_TX1_RF_ON    BIT6

/**
 * @brief 1 = Output signal on pin TX2 inverted when driver TX2 is disabled
 */
#define MFRC522_BIT_TX_CONTROL_INV_TX2_RF_OFF   BIT5

/**
 * @brief 1 = Output signal on pin TX1 inverted when driver TX1 is disabled
 */
#define MFRC522_BIT_TX_CONTROL_INV_TX1_RF_OFF   BIT4

/**
 * @brief 0 = Tx2CW bit is enabled to modulate the 13.56 MHz energy carrier
 *        1 = Output signal on pin TX2 continuously delivers the unmodulated
 *            13.56 MHz energy carrier
 */
#define MFRC522_BIT_TX_CONTROL_TX2_CW           BIT3

/**
 * @brief 1 = Output signal on pin TX2 delivers the 13.56 MHz energy carrier
 *            modulated by the transmission data
 */
#define MFRC522_BIT_TX_CONTROL_TX2_RF_EN        BIT1

/**
 * @brief 1 = Output signal on pin TX1 delivers the 13.56 MHz energy carrier
 *            modulated by the transmission data
 */
#define MFRC522_BIT_TX_CONTROL_TX1_RF_EN        BIT0



/**
 * @brief 1 = Forces a 100 % ASK modulation independent of the ModGsPReg
 *            register setting
 */
#define MFRC522_BIT_TX_ASK_FORCE_100_ASK    BIT6



/**
 * @brief Selects the input of drivers TX1 and TX2
 *
 *        00 = 3-state; in soft power-down the drivers are only in 3-state mode
 *             if the DriverSel[1:0] value is set to 3-state mode
 *        01 = Modulation signal (envelope) from the internal encoder, Miller
 *             pulse encoded
 *        10 = Modulation signal (envelope) from pin MFIN
 *        11 = HIGH; the HIGH level depends on the setting of bits
 *             InvTx1RFOn/InvTx1RFOff and InvTx2RFOn/InvTx2RFOff
 *
 *        MFRC522_REG_TX_SEL[5:4]
 */
#define MFRC522_BITMASK_TX_SEL_DRIVER_SEL   0x30

/**
 * @brief Selects the input for pin MFOUT
 *
 *        0000 = 3-state
 *        0001 = LOW
 *        0010 = HIGH
 *        0011 = Test bus signal as defined by the TestSel1Reg register’s
 *               TstBusBitSel[2:0] value
 *        0100 = Modulation signal (envelope) from the internal encoder, Miller
 *               pulse encoded
 *        0101 = Serial data stream to be transmitted, data stream before Miller
 *               encoder
 *        0111 = Serial data stream received, data stream after Manchester
 *               decoder
 *
 *        MFRC522_REG_TX_SEL[3:0]
 */
#define MFRC522_BITMASK_TX_SEL_MF_OUT_SEL   0x0F



/**
 * @brief Selects the input of the contactless UART
 *
 *        00 = Constant LOW
 *        01 = Manchester with subcarrier from pin MFIN
 *        10 = Modulated signal from the internal analog module, default
 *        11 = NRZ coding without subcarrier from pin MFIN which is only valid
 *             for transfer speeds above 106 kBd
 *
 *        MFRC522_REG_RX_SEL[7:6]
 */
#define MFRC522_BITMASK_RX_SEL_UART_SEL     0xC0

/**
 * @brief After data transmission the activation of the receiver is delayed for
 *        RxWait bit-clocks, during this ‘frame guard time’ any signal on pin RX
 *        is ignored; this parameter is ignored by the Receive command; all
 *        other commands, such as Transceive, MFAuthent use this parameter; the
 *        counter starts immediately after the external RF field is switched on
 *
 *        MFRC522_REG_RX_SEL[5:0]
 */
#define MFRC522_BITMASK_RX_SEL_RX_WAIT      0x3F



/**
 * @brief Defines the minimum signal strength at the decoder input that will be
 *        accepted; if the signal strength is below this level it is not
 *        evaluated
 *
 *        MFRC522_REG_RX_THRESHHOLD[7:4]
 */
#define MFRC522_BITMASK_RX_THRESHHOLD_MIN_LEVEL     0xF0

/**
 * @brief Defines the minimum signal strength at the decoder input that must be
 *        reached by the weaker half-bit of the Manchester encoded signal to
 *        generate a bit-collision relative to the amplitude of the stronger
 *        half-bit
 *
 *        MFRC522_REG_RX_THRESHHOLD[2:0]
 */
#define MFRC522_BITMASK_RX_THRESHHOLD_COLL_LEVEL    0x07



/**
 * @brief Defines the use of I and Q channel during reception; Remark: the FixIQ
 *        bit must be set to logic 0 to enable the following settings:
 *
 *        00 = Selects the stronger channel
 *        01 = Selects the stronger channel and freezes the selected channel
 *             during communication
 *
 *        MFRC522_REG_DEMOD[7:6]
 */
#define MFRC522_BITMASK_DEMOD_ADD_IQ        0xC0

/**
 * @brief 1 = If AddIQ[1:0] are set to X0b, the reception is fixed to I channel
 *            If AddIQ[1:0] are set to X1b, the reception is fixed to Q channel
 */
#define MFRC522_BIT_DEMOD_FIX_IQ            BIT5

/**
 * @brief Available on RC522 version 1.0 and version 2.0:
 *        If set to logic 0 the following formula is used to calculate the timer
 *        frequency of the prescaler:
 *              f_timer = 13.56 MHz / (2 * TPreScaler + 1)

 *        Only available on version 2.0:
 *        If set to logic 1 the following formula is used to calculate the timer
 *        frequency of the prescaler:
 *              f_timer = 13.56 MHz / (2 * TPreScaler + 2)
 *        Default TPrescalEven bit is logic 0, find more information on the
 *        prescaler in Section 8.5.
 */
#define MFRC522_BIT_DEMOD_T_PRESCAL_EVEN    BIT4

/**
 * @brief Changes the time-constant of the internal PLL during data reception
 *        Remark: if set to 00b the PLL is frozen during data reception
 *
 *        MFRC522_REG_DEMOD[3:2]
 */
#define MFRC522_BITMASK_DEMOD_TAU_RCV       0x0C

/**
 * @brief Changes the time-constant of the internal PLL during burst
 *
 *        MFRC522_REG_DEMOD[1:0]
 */
#define MFRC522_BITMASK_DEMOD_TAU_SYNC      0x03



/**
 * @brief Defines the additional response time; 7 bits are added to the value of
 *        the register bit by default
 *
 *        MFRC522_REG_MF_TX[1:0]
 */
#define MFRC522_BITMASK_MF_TX_TX_WAIT   0x03



/**
 * @brief 1 = Generation of the parity bit for transmission and the parity check
 *            for receiving is switched off; the received parity bit is handled
 *            like a data bit
 */
#define MFRC522_BIT_MF_RX_PARITY_DISABLE    BIT4



/**
 * @brief Factor BR_T0 adjusts the transfer speed: for description, see Section
 *        8.1.3.2 on page 12
 *
 *        MFRC522_REG_SERIAL_SPEED[7:5]
 */
#define MFRC522_BITMASK_SERIAL_SPEED_BR_T0  0xE0

/**
 * @brief Factor BR_T1 adjusts the transfer speed: for description, see Section
 *        8.1.3.2 on page 12
 *
 *        MFRC522_REG_SERIAL_SPEED[4:0]
 */
#define MFRC522_BITMASK_SERIAL_SPEED_BR_T1  0x1F



/**
 * @brief Shows the value of the CRCResultReg register’s most significant byte;
 *        only valid if Status1Reg register’s CRCReady bit is set to logic 1
 *
 *        MFRC522_REG_CRC_RESULT_MSB[7:0]
 */
#define MFRC522_BITMASK_CRC_RESULT_MSB_CRC_RESULT_MSB   0xFF

/**
 * @brief Shows the value of the least significant byte of the CRCResultReg
 *        register; only valid if Status1Reg register’s CRCReady bit is set to
 *        logic 1
 *
 *        MFRC522_REG_CRC_RESULT_LSB[7:0]
 */
#define MFRC522_BITMASK_CRC_RESULT_LSB_CRC_RESULT_LSB   0xFF



/**
 * @brief Defines the width of the Miller modulation as multiples of the carrier
 *        frequency (ModWidth + 1 / f_clk); the maximum value is half the bit
 *        period
 *
 *        MFRC522_REG_MOD_WIDTH[7:0]
 */
#define MFRC522_BITMASK_MOD_WIDTH   0xFF



/**
 * @brief Defines the receiver’s signal voltage gain factor:
 *
 *        000 = 18 dB
 *        001 = 23 dB
 *        010 = 18 dB
 *        011 = 23 dB
 *        100 = 33 dB
 *        101 = 38 dB
 *        110 = 43 dB
 *        111 = 48 dB
 *
 *        MFRC522_REG_RF_CFG[6:4]
 *
 * @see mfrc522_pcd_rx_gain
 */
#define MFRC522_BITMASK_RF_CFG_RX_GAIN  0x70


/**
 * @brief Defines the conductance of the output n-driver during periods without
 *        modulation which can be used to regulate the output power and
 *        subsequently current consumption and operating distance; Remark: the
 *        conductance value is binary-weighted; during soft Power-down mode the
 *        highest bit is forced to logic 1; value is only used if driver TX1 or
 *        TX2 is switched on
 *
 *        MFRC522_REG_GS_N[7:4]
 */
#define MFRC522_BITMASK_GS_N_CW_GS_N    0xF0

/**
 * @brief Defines the conductance of the output n-driver during periods without
 *        modulation which can be used to regulate the modulation index;
 *        Remark: the conductance value is binary weighted; during soft
 *        Power-down mode the highest bit is forced to logic 1; value is only
 *        used if driver TX1 or TX2 is switched on
 *
 *        MFRC522_REG_GS_N[3:0]
 */
#define MFRC522_BITMASK_GS_N_MOD_GS_N   0x0F



/**
 * @brief Defines the conductance of the p-driver output which can be used to
 *        regulate the output power and subsequently current consumption and
 *        operating distance; Remark: the conductance value is binary weighted;
 *        during soft Power-down mode the highest bit is forced to logic 1
 *
 *        MFRC522_REG_CW_GS_P[5:0]
 */
#define MFRC522_BITMASK_CW_GS_P_CW_GS_P     0x3F



/**
 * @brief Defines the conductance of the p-driver output during modulation which
 *        can be used to regulate the modulation index; Remark: the conductance
 *        value is binary weighted; during soft Power-down mode the highest bit
 *        is forced to logic 1; if the TxASKReg register’s Force100ASK bit is
 *        set to logic 1 the value of ModGsP has no effect
 *
 *        MFRC522_REG_MOD_GS_P[5:0]
 */
#define MFRC522_BITMASK_MOD_GS_P_MOD_GS_P   0x3F



/**
 * @brief 0 = Indicates that the timer is not influenced by the protocol
 *        1 = Timer starts automatically at the end of the transmission in all
 *            communication modes at all speeds; if the RxModeReg register’s
 *            RxMultiple bit is not set, the timer stops immediately after
 *            receiving the 5th bit (1 startbit, 4 data bits); if the RxMultiple
 *            bit is set to logic 1 the timer never stops, in which case the
 *            timer can be stopped by setting the ControlReg register’s TStopNow
 *            bit to logic 1
 */
#define MFRC522_BIT_T_MODE_T_AUTO                   BIT7

/**
 * @brief Internal timer is running in gated mode; Remark: in gated mode, the
 *        Status1Reg register’s TRunning bit is logic 1 when the timer is
 *        enabled by the TModeReg register’s TGated[1:0] bits; this bit does not
 *        influence the gating signal
 *
 *        00 = non-gated mode
 *        01 = gated by pin MFIN
 *        10 = gated by pin AUX1
 *
 *        MFRC522_REG_T_MODE[6:5]
 */
#define MFRC522_BITMASK_T_MODE_T_GATED              0x60

/**
 * @brief 0 = Timer decrements to 0 and the ComIrqReg register’s TimerIRq bit is
 *            set to logic 1
 *        1 = Timer automatically restarts its count-down from the 16-bit timer
 *            reload value instead of counting down to zero
 */
#define MFRC522_BIT_T_MODE_T_AUTO_RESTART           BIT4

/**
 * @brief Defines the higher 4 bits of the TPrescaler value;
 *
 *        The following formula is used to calculate the timer frequency if the
 *        DemodReg register’s TPrescalEven bit in Demot Regis set to logic 0:
 *              f_timer = 13.56 MHz / (2 * TPreScaler + 1)
 *        Where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo] (TPrescaler value
 *        on 12 bits) (Default TPrescalEven bit is logic 0);
 *
 *        The following formula is used to calculate the timer frequency if the
 *        DemodReg register’s TPrescalEven bit is set to logic 1:
 *              f_timer = 13.56 MHz / (2 * TPreScaler + 2)
 *         See Section 8.5 “Timer unit”.
 *
 *        MFRC522_REG_T_MODE[3:0]
 */
#define MFRC522_BITMASK_T_MODE_T_PRESCALER_HI       0x0F

/**
 * @brief Defines the lower 8 bits of the TPrescaler value
 *        The following formula is used to calculate the timer frequency if the
 *        DemodReg register’s TPrescalEven bit is set to logic 0:
 *              f_timer = 13.56 MHz / (2 * TPreScaler + 1)
 *        Where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo] (TPrescaler value on
 *        12 bits) (Default TPrescalEven bit is logic 0)
 *
 *        The following formula is used to calculate the timer frequency if the
 *        DemodReg register’s TPrescalEven bit inDemoReg is set to logic 1:
 *              f_timer = 13.56 MHz / (2 * TPreScaler + 2)
 *        See Section 8.5 “Timer unit”.
 *
 *        MFRC522_REG_T_PRESCALER[7:0]
 */
#define MFRC522_BITMASK_T_PRESCALER_T_PRESCALER_LO  0xFF



/**
 * @brief Defines the higher 8 bits of the 16-bit timer reload value; on a start
 *        event, the timer loads the timer reload value; changing this register
 *        affects the timer only at the next start event
 *
 *        MFRC522_REG_T_RELOAD_MSB[7:0]
 */
#define MFRC522_BITMASK_T_RELOAD_MSB_T_RELOAD_VAL_HI    0xFF

/**
 * @brief Defines the lower 8 bits of the 16-bit timer reload value; on a start
 *        event, the timer loads the timer reload value; changing this register
 *        affects the timer only at the next start event
 *
 *        MFRC522_REG_T_RELOAD_LSB[7:0]
 */
#define MFRC522_BITMASK_T_RELOAD_LSB_T_RELOAD_VAL_LO    0xFF



/**
 * @brief Timer value higher 8 bits
 *
 *        MFRC522_REG_T_COUNTER_VAL_MSB[7:0]
 */
#define MFRC522_BITMASK_T_COUNTER_VAL_MSB_T_COUNTER_VAL_HI  0xFF

/**
 * @brief Timer value lower 8 bits
 *
 *        MFRC522_REG_T_COUNTER_VAL_LSB[7:0]
 */
#define MFRC522_BITMASK_T_COUNTER_VAL_LSB_T_COUNTER_VAL_LO  0xFF

#ifdef __cplusplus
}
#endif

#endif /* MFRC522_REGS_H */
/** @} */
