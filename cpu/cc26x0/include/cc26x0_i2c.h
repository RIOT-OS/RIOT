/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_i2c_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 MCU I/O register definitions
 *
 * @author          Leon George <leon@georgemail.eu>
 */

#ifndef CC26X0_I2C_H
#define CC26X0_I2C_H

#include "cc26x0.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * I2C registers
 */
typedef struct {
    reg32_t SOAR; /**< slave own address */
    union {
        reg32_t SSTAT; /**< slave status */
        reg32_t SCTL; /**< slave control */
    };
    reg32_t SDR; /**< slave data */
    reg32_t SIMR; /**< slave interrupt mask */
    reg32_t SRIS; /**< slave raw interrupt status */
    reg32_t SMIS; /**< slave masked interrupt status */
    reg32_t SICR; /**< slave interrupt clear */
    reg32_t __reserved[0x1F9]; /**< meh */
    reg32_t MSA; /**< master slave address */
    union {
        reg32_t MSTAT; /**< master status */
        reg32_t MCTRL; /**< master control */
    };
    reg32_t MDR; /**< master data */
    reg32_t MTPR; /**< master timer period */
    reg32_t MIMR; /**< master interrupt mask */
    reg32_t MRIS; /**< master raw interrupt status */
    reg32_t MMIS; /**< master masked interrupt statues */
    reg32_t MICR; /**< master interrupt clear */
    reg32_t MCR; /**< master configuration */
} i2c_regs_t;

/** @ingroup cpu_specific_peripheral_memory_map
  * @{
  */
#define I2C_BASE           (PERIPH_BASE + 0x2000) /**< I2C base address */
/** @} */

#define I2C ((i2c_regs_t *) (I2C_BASE)) /**< I2C register bank */

/**
 * @brief   I2C register values
 * @{
 */
//*****************************************************************************
//
// Register: I2C_O_SOAR
//
//*****************************************************************************
// Field:   [6:0] OAR
//
// I2C slave own address
// This field specifies bits a6 through a0 of the slave address.
#define I2C_SOAR_OAR_M                                              0x0000007F
#define I2C_SOAR_OAR_S                                                       0

//*****************************************************************************
//
// Register: I2C_O_SSTAT
//
//*****************************************************************************
// Field:     [2] FBR
//
// First byte received
//
// 0: The first byte has not been received.
// 1: The first byte following the slave's own address has been received.
//
// This bit is only valid when the RREQ bit is set and is automatically cleared
// when data has been read from the SDR register.
// Note: This bit is not used for slave transmit operations.
#define I2C_SSTAT_FBR                                               0x00000004
#define I2C_SSTAT_FBR_BITN                                                   2
#define I2C_SSTAT_FBR_M                                             0x00000004
#define I2C_SSTAT_FBR_S                                                      2

// Field:     [1] TREQ
//
// Transmit request
//
// 0: No outstanding transmit request.
// 1: The I2C controller has been addressed as a slave transmitter and is using
// clock stretching to delay the master until data has been written to the SDR
// register.
#define I2C_SSTAT_TREQ                                              0x00000002
#define I2C_SSTAT_TREQ_BITN                                                  1
#define I2C_SSTAT_TREQ_M                                            0x00000002
#define I2C_SSTAT_TREQ_S                                                     1

// Field:     [0] RREQ
//
// Receive request
//
// 0: No outstanding receive data
// 1: The I2C controller has outstanding receive data from the I2C master and
// is using clock stretching to delay the master until data has been read from
// the SDR register.
#define I2C_SSTAT_RREQ                                              0x00000001
#define I2C_SSTAT_RREQ_BITN                                                  0
#define I2C_SSTAT_RREQ_M                                            0x00000001
#define I2C_SSTAT_RREQ_S                                                     0

//*****************************************************************************
//
// Register: I2C_O_SCTL
//
//*****************************************************************************
// Field:     [0] DA
//
// Device active
//
// 0: Disables the I2C slave operation
// 1: Enables the I2C slave operation
#define I2C_SCTL_DA                                                 0x00000001
#define I2C_SCTL_DA_BITN                                                     0
#define I2C_SCTL_DA_M                                               0x00000001
#define I2C_SCTL_DA_S                                                        0

//*****************************************************************************
//
// Register: I2C_O_SDR
//
//*****************************************************************************
// Field:   [7:0] DATA
//
// Data for transfer
// This field contains the data for transfer during a slave receive or transmit
// operation.  When written the register data is used as transmit data.  When
// read, this register returns the last data received.
// Data is stored until next update, either by a system write for transmit or
// by an external master for receive.
#define I2C_SDR_DATA_M                                              0x000000FF
#define I2C_SDR_DATA_S                                                       0

//*****************************************************************************
//
// Register: I2C_O_SIMR
//
//*****************************************************************************
// Field:     [2] STOPIM
//
// Stop condition interrupt mask
//
// 0: The SRIS.STOPRIS interrupt is suppressed and not sent to the interrupt
// controller.
// 1: The SRIS.STOPRIS interrupt is enabled and sent to the interrupt
// controller.
// ENUMs:
// EN                       Enable Interrupt
// DIS                      Disable Interrupt
#define I2C_SIMR_STOPIM                                             0x00000004
#define I2C_SIMR_STOPIM_BITN                                                 2
#define I2C_SIMR_STOPIM_M                                           0x00000004
#define I2C_SIMR_STOPIM_S                                                    2
#define I2C_SIMR_STOPIM_EN                                          0x00000004
#define I2C_SIMR_STOPIM_DIS                                         0x00000000

// Field:     [1] STARTIM
//
// Start condition interrupt mask
//
// 0: The SRIS.STARTRIS interrupt is suppressed and not sent to the interrupt
// controller.
// 1: The SRIS.STARTRIS interrupt is enabled and sent to the interrupt
// controller.
// ENUMs:
// EN                       Enable Interrupt
// DIS                      Disable Interrupt
#define I2C_SIMR_STARTIM                                            0x00000002
#define I2C_SIMR_STARTIM_BITN                                                1
#define I2C_SIMR_STARTIM_M                                          0x00000002
#define I2C_SIMR_STARTIM_S                                                   1
#define I2C_SIMR_STARTIM_EN                                         0x00000002
#define I2C_SIMR_STARTIM_DIS                                        0x00000000

// Field:     [0] DATAIM
//
// Data interrupt mask
//
// 0: The SRIS.DATARIS interrupt is suppressed and not sent to the interrupt
// controller.
// 1: The SRIS.DATARIS interrupt is enabled and sent to the interrupt
// controller.
#define I2C_SIMR_DATAIM                                             0x00000001
#define I2C_SIMR_DATAIM_BITN                                                 0
#define I2C_SIMR_DATAIM_M                                           0x00000001
#define I2C_SIMR_DATAIM_S                                                    0

//*****************************************************************************
//
// Register: I2C_O_SRIS
//
//*****************************************************************************
// Field:     [2] STOPRIS
//
// Stop condition raw interrupt status
//
// 0: No interrupt
// 1: A Stop condition interrupt is pending.
//
// This bit is cleared by writing a 1 to SICR.STOPIC.
#define I2C_SRIS_STOPRIS                                            0x00000004
#define I2C_SRIS_STOPRIS_BITN                                                2
#define I2C_SRIS_STOPRIS_M                                          0x00000004
#define I2C_SRIS_STOPRIS_S                                                   2

// Field:     [1] STARTRIS
//
// Start condition raw interrupt status
//
// 0: No interrupt
// 1: A Start condition interrupt is pending.
//
// This bit is cleared by writing a 1 to SICR.STARTIC.
#define I2C_SRIS_STARTRIS                                           0x00000002
#define I2C_SRIS_STARTRIS_BITN                                               1
#define I2C_SRIS_STARTRIS_M                                         0x00000002
#define I2C_SRIS_STARTRIS_S                                                  1

// Field:     [0] DATARIS
//
// Data raw interrupt status
//
// 0: No interrupt
// 1: A data received or data requested interrupt is pending.
//
// This bit is cleared by writing a 1 to the SICR.DATAIC.
#define I2C_SRIS_DATARIS                                            0x00000001
#define I2C_SRIS_DATARIS_BITN                                                0
#define I2C_SRIS_DATARIS_M                                          0x00000001
#define I2C_SRIS_DATARIS_S                                                   0

//*****************************************************************************
//
// Register: I2C_O_SMIS
//
//*****************************************************************************
// Field:     [2] STOPMIS
//
// Stop condition masked interrupt status
//
// 0: An interrupt has not occurred or is masked/disabled.
// 1: An unmasked Stop condition interrupt is pending.
//
// This bit is cleared by writing a 1 to the SICR.STOPIC.
#define I2C_SMIS_STOPMIS                                            0x00000004
#define I2C_SMIS_STOPMIS_BITN                                                2
#define I2C_SMIS_STOPMIS_M                                          0x00000004
#define I2C_SMIS_STOPMIS_S                                                   2

// Field:     [1] STARTMIS
//
// Start condition masked interrupt status
//
// 0: An interrupt has not occurred or is masked/disabled.
// 1: An unmasked Start condition interrupt is pending.
//
// This bit is cleared by writing a 1 to the SICR.STARTIC.
#define I2C_SMIS_STARTMIS                                           0x00000002
#define I2C_SMIS_STARTMIS_BITN                                               1
#define I2C_SMIS_STARTMIS_M                                         0x00000002
#define I2C_SMIS_STARTMIS_S                                                  1

// Field:     [0] DATAMIS
//
// Data masked interrupt status
//
// 0: An interrupt has not occurred or is masked/disabled.
// 1: An unmasked data received or data requested interrupt is pending.
//
// This bit is cleared by writing a 1 to the SICR.DATAIC.
#define I2C_SMIS_DATAMIS                                            0x00000001
#define I2C_SMIS_DATAMIS_BITN                                                0
#define I2C_SMIS_DATAMIS_M                                          0x00000001
#define I2C_SMIS_DATAMIS_S                                                   0

//*****************************************************************************
//
// Register: I2C_O_SICR
//
//*****************************************************************************
// Field:     [2] STOPIC
//
// Stop condition interrupt clear
//
// Writing 1 to this bit clears SRIS.STOPRIS and SMIS.STOPMIS.
#define I2C_SICR_STOPIC                                             0x00000004
#define I2C_SICR_STOPIC_BITN                                                 2
#define I2C_SICR_STOPIC_M                                           0x00000004
#define I2C_SICR_STOPIC_S                                                    2

// Field:     [1] STARTIC
//
// Start condition interrupt clear
//
// Writing 1 to this bit clears SRIS.STARTRIS SMIS.STARTMIS.
#define I2C_SICR_STARTIC                                            0x00000002
#define I2C_SICR_STARTIC_BITN                                                1
#define I2C_SICR_STARTIC_M                                          0x00000002
#define I2C_SICR_STARTIC_S                                                   1

// Field:     [0] DATAIC
//
// Data interrupt clear
//
// Writing 1 to this bit clears SRIS.DATARIS SMIS.DATAMIS.
#define I2C_SICR_DATAIC                                             0x00000001
#define I2C_SICR_DATAIC_BITN                                                 0
#define I2C_SICR_DATAIC_M                                           0x00000001
#define I2C_SICR_DATAIC_S                                                    0

//*****************************************************************************
//
// Register: I2C_O_MSA
//
//*****************************************************************************
// Field:   [7:1] SA
//
// I2C master slave address
// Defines which slave is addressed for the transaction in master mode
#define I2C_MSA_SA_M                                                0x000000FE
#define I2C_MSA_SA_S                                                         1

// Field:     [0] RS
//
// Receive or Send
// This bit-field specifies if the next operation is a receive (high) or a
// transmit/send (low) from the addressed slave SA.
// ENUMs:
// RX                       Receive data from slave
// TX                       Transmit/send data to slave
#define I2C_MSA_RS                                                  0x00000001
#define I2C_MSA_RS_BITN                                                      0
#define I2C_MSA_RS_M                                                0x00000001
#define I2C_MSA_RS_S                                                         0
#define I2C_MSA_RS_RX                                               0x00000001
#define I2C_MSA_RS_TX                                               0x00000000

//*****************************************************************************
//
// Register: I2C_O_MSTAT
//
//*****************************************************************************
// Field:     [6] BUSBSY
//
// Bus busy
//
// 0: The I2C bus is idle.
// 1: The I2C bus is busy.
//
// The bit changes based on the MCTRL.START and MCTRL.STOP conditions.
#define I2C_MSTAT_BUSBSY                                            0x00000040
#define I2C_MSTAT_BUSBSY_BITN                                                6
#define I2C_MSTAT_BUSBSY_M                                          0x00000040
#define I2C_MSTAT_BUSBSY_S                                                   6

// Field:     [5] IDLE
//
// I2C idle
//
// 0: The I2C controller is not idle.
// 1: The I2C controller is idle.
#define I2C_MSTAT_IDLE                                              0x00000020
#define I2C_MSTAT_IDLE_BITN                                                  5
#define I2C_MSTAT_IDLE_M                                            0x00000020
#define I2C_MSTAT_IDLE_S                                                     5

// Field:     [4] ARBLST
//
// Arbitration lost
//
// 0: The I2C controller won arbitration.
// 1: The I2C controller lost arbitration.
#define I2C_MSTAT_ARBLST                                            0x00000010
#define I2C_MSTAT_ARBLST_BITN                                                4
#define I2C_MSTAT_ARBLST_M                                          0x00000010
#define I2C_MSTAT_ARBLST_S                                                   4

// Field:     [3] DATACK_N
//
// Data Was Not Acknowledge
//
// 0: The transmitted data was acknowledged.
// 1: The transmitted data was not acknowledged.
#define I2C_MSTAT_DATACK_N                                          0x00000008
#define I2C_MSTAT_DATACK_N_BITN                                              3
#define I2C_MSTAT_DATACK_N_M                                        0x00000008
#define I2C_MSTAT_DATACK_N_S                                                 3

// Field:     [2] ADRACK_N
//
// Address Was Not Acknowledge
//
// 0: The transmitted address was acknowledged.
// 1: The transmitted address was not acknowledged.
#define I2C_MSTAT_ADRACK_N                                          0x00000004
#define I2C_MSTAT_ADRACK_N_BITN                                              2
#define I2C_MSTAT_ADRACK_N_M                                        0x00000004
#define I2C_MSTAT_ADRACK_N_S                                                 2

// Field:     [1] ERR
//
// Error
//
// 0: No error was detected on the last operation.
// 1: An error occurred on the last operation.
#define I2C_MSTAT_ERR                                               0x00000002
#define I2C_MSTAT_ERR_BITN                                                   1
#define I2C_MSTAT_ERR_M                                             0x00000002
#define I2C_MSTAT_ERR_S                                                      1

// Field:     [0] BUSY
//
// I2C busy
//
// 0: The controller is idle.
// 1: The controller is busy.
//
// When this bit-field is set, the other status bits are not valid.
//
// Note: The I2C controller requires four SYSBUS clock cycles to assert the
// BUSY status after I2C master operation has been initiated through MCTRL
// register.
// Hence after programming MCTRL register, application is requested to wait for
// four SYSBUS clock cycles before issuing a controller status inquiry through
// MSTAT register.
// Any prior inquiry would result in wrong status being reported.
#define I2C_MSTAT_BUSY                                              0x00000001
#define I2C_MSTAT_BUSY_BITN                                                  0
#define I2C_MSTAT_BUSY_M                                            0x00000001
#define I2C_MSTAT_BUSY_S                                                     0

//*****************************************************************************
//
// Register: I2C_O_MCTRL
//
//*****************************************************************************
// Field:     [3] ACK
//
// Data acknowledge enable
//
// 0: The received data byte is not acknowledged automatically by the master.
// 1: The received data byte is acknowledged automatically by the master.
//
// This bit-field must be cleared when the I2C bus controller requires no
// further data to be transmitted from the slave transmitter.
// ENUMs:
// EN                       Enable acknowledge
// DIS                      Disable acknowledge
#define I2C_MCTRL_ACK                                               0x00000008
#define I2C_MCTRL_ACK_BITN                                                   3
#define I2C_MCTRL_ACK_M                                             0x00000008
#define I2C_MCTRL_ACK_S                                                      3
#define I2C_MCTRL_ACK_EN                                            0x00000008
#define I2C_MCTRL_ACK_DIS                                           0x00000000

// Field:     [2] STOP
//
// This bit-field determines if the cycle stops at the end of the data cycle or
// continues on to a repeated START condition.
//
// 0: The controller does not generate the Stop condition.
// 1: The controller generates the Stop condition.
// ENUMs:
// EN                       Enable STOP
// DIS                      Disable STOP
#define I2C_MCTRL_STOP                                              0x00000004
#define I2C_MCTRL_STOP_BITN                                                  2
#define I2C_MCTRL_STOP_M                                            0x00000004
#define I2C_MCTRL_STOP_S                                                     2
#define I2C_MCTRL_STOP_EN                                           0x00000004
#define I2C_MCTRL_STOP_DIS                                          0x00000000

// Field:     [1] START
//
// This bit-field generates the Start or Repeated Start condition.
//
// 0: The controller does not generate the Start condition.
// 1: The controller generates the Start condition.
// ENUMs:
// EN                       Enable START
// DIS                      Disable START
#define I2C_MCTRL_START                                             0x00000002
#define I2C_MCTRL_START_BITN                                                 1
#define I2C_MCTRL_START_M                                           0x00000002
#define I2C_MCTRL_START_S                                                    1
#define I2C_MCTRL_START_EN                                          0x00000002
#define I2C_MCTRL_START_DIS                                         0x00000000

// Field:     [0] RUN
//
// I2C master enable
//
// 0: The master is disabled.
// 1: The master is enabled to transmit or receive data.
// ENUMs:
// EN                       Enable Master
// DIS                      Disable Master
#define I2C_MCTRL_RUN                                               0x00000001
#define I2C_MCTRL_RUN_BITN                                                   0
#define I2C_MCTRL_RUN_M                                             0x00000001
#define I2C_MCTRL_RUN_S                                                      0
#define I2C_MCTRL_RUN_EN                                            0x00000001
#define I2C_MCTRL_RUN_DIS                                           0x00000000

//*****************************************************************************
//
// Register: I2C_O_MDR
//
//*****************************************************************************
// Field:   [7:0] DATA
//
// When Read: Last RX Data is returned
// When Written: Data is transferred during TX  transaction
#define I2C_MDR_DATA_M                                              0x000000FF
#define I2C_MDR_DATA_S                                                       0

//*****************************************************************************
//
// Register: I2C_O_MTPR
//
//*****************************************************************************
// Field:     [7] TPR_7
//
// Must be set to 0 to set TPR. If set to 1, a write to TPR will be ignored.
#define I2C_MTPR_TPR_7                                              0x00000080
#define I2C_MTPR_TPR_7_BITN                                                  7
#define I2C_MTPR_TPR_7_M                                            0x00000080
#define I2C_MTPR_TPR_7_S                                                     7

// Field:   [6:0] TPR
//
// SCL clock period
// This field specifies the period of the SCL clock.
// SCL_PRD = 2*(1+TPR)*(SCL_LP + SCL_HP)*CLK_PRD
// where:
// SCL_PRD is the SCL line period (I2C clock).
// TPR is the timer period register value (range of 1 to 127)
// SCL_LP is the SCL low period (fixed at 6).
// SCL_HP is the SCL high period (fixed at 4).
// CLK_PRD is the system clock period in ns.
#define I2C_MTPR_TPR_M                                              0x0000007F
#define I2C_MTPR_TPR_S                                                       0

//*****************************************************************************
//
// Register: I2C_O_MIMR
//
//*****************************************************************************
// Field:     [0] IM
//
// Interrupt mask
//
// 0: The MRIS.RIS interrupt is suppressed and not sent to the interrupt
// controller.
// 1: The master interrupt is sent to the interrupt controller when the
// MRIS.RIS is set.
// ENUMs:
// EN                       Enable Interrupt
// DIS                      Disable Interrupt
#define I2C_MIMR_IM                                                 0x00000001
#define I2C_MIMR_IM_BITN                                                     0
#define I2C_MIMR_IM_M                                               0x00000001
#define I2C_MIMR_IM_S                                                        0
#define I2C_MIMR_IM_EN                                              0x00000001
#define I2C_MIMR_IM_DIS                                             0x00000000

//*****************************************************************************
//
// Register: I2C_O_MRIS
//
//*****************************************************************************
// Field:     [0] RIS
//
// Raw interrupt status
//
// 0: No interrupt
// 1: A master interrupt is pending.
//
// This bit is cleared by writing 1 to the MICR.IC bit .
#define I2C_MRIS_RIS                                                0x00000001
#define I2C_MRIS_RIS_BITN                                                    0
#define I2C_MRIS_RIS_M                                              0x00000001
#define I2C_MRIS_RIS_S                                                       0

//*****************************************************************************
//
// Register: I2C_O_MMIS
//
//*****************************************************************************
// Field:     [0] MIS
//
// Masked interrupt status
//
// 0: An interrupt has not occurred or is masked.
// 1: A master interrupt is pending.
//
// This bit is cleared by writing 1 to the MICR.IC bit .
#define I2C_MMIS_MIS                                                0x00000001
#define I2C_MMIS_MIS_BITN                                                    0
#define I2C_MMIS_MIS_M                                              0x00000001
#define I2C_MMIS_MIS_S                                                       0

//*****************************************************************************
//
// Register: I2C_O_MICR
//
//*****************************************************************************
// Field:     [0] IC
//
// Interrupt clear
// Writing 1 to this bit clears MRIS.RIS and  MMIS.MIS .
//
// Reading this register returns no meaningful data.
#define I2C_MICR_IC                                                 0x00000001
#define I2C_MICR_IC_BITN                                                     0
#define I2C_MICR_IC_M                                               0x00000001
#define I2C_MICR_IC_S                                                        0

//*****************************************************************************
//
// Register: I2C_O_MCR
//
//*****************************************************************************
// Field:     [5] SFE
//
// I2C slave function enable
// ENUMs:
// EN                       Slave mode is enabled.
// DIS                      Slave mode is disabled.
#define I2C_MCR_SFE                                                 0x00000020
#define I2C_MCR_SFE_BITN                                                     5
#define I2C_MCR_SFE_M                                               0x00000020
#define I2C_MCR_SFE_S                                                        5
#define I2C_MCR_SFE_EN                                              0x00000020
#define I2C_MCR_SFE_DIS                                             0x00000000

// Field:     [4] MFE
//
// I2C master function enable
// ENUMs:
// EN                       Master mode is enabled.
// DIS                      Master mode is disabled.
#define I2C_MCR_MFE                                                 0x00000010
#define I2C_MCR_MFE_BITN                                                     4
#define I2C_MCR_MFE_M                                               0x00000010
#define I2C_MCR_MFE_S                                                        4
#define I2C_MCR_MFE_EN                                              0x00000010
#define I2C_MCR_MFE_DIS                                             0x00000000

// Field:     [0] LPBK
//
// I2C loopback
//
// 0: Normal operation
// 1: Loopback operation (test mode)
// ENUMs:
// EN                       Enable Test Mode
// DIS                      Disable Test Mode
#define I2C_MCR_LPBK                                                0x00000001
#define I2C_MCR_LPBK_BITN                                                    0
#define I2C_MCR_LPBK_M                                              0x00000001
#define I2C_MCR_LPBK_S                                                       0
#define I2C_MCR_LPBK_EN                                             0x00000001
#define I2C_MCR_LPBK_DIS                                            0x00000000

//*****************************************************************************
//
// I2C Master commands
//
//*****************************************************************************
#define I2C_MASTER_CMD_SINGLE_SEND                                            \
                                0x00000007
#define I2C_MASTER_CMD_SINGLE_RECEIVE                                         \
                                0x00000007
#define I2C_MASTER_CMD_BURST_SEND_START                                       \
                                0x00000003
#define I2C_MASTER_CMD_BURST_SEND_CONT                                        \
                                0x00000001
#define I2C_MASTER_CMD_BURST_SEND_FINISH                                      \
                                0x00000005
#define I2C_MASTER_CMD_BURST_SEND_ERROR_STOP                                  \
                                0x00000004
#define I2C_MASTER_CMD_BURST_RECEIVE_START                                    \
                                0x0000000b
#define I2C_MASTER_CMD_BURST_RECEIVE_CONT                                     \
                                0x00000009
#define I2C_MASTER_CMD_BURST_RECEIVE_FINISH                                   \
                                0x00000005
#define I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP                               \
                                0x00000004

/** @} */



#ifdef __cplusplus
}
#endif

#endif /* CC26X0_I2C_H */

/*@}*/
