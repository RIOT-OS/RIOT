/*
 * Copyright (C) 2016 Leon George, Florent-Valéry Coen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x0_rfc_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 RFC register definitions
 *
 * @author          Leon George <leon@georgemail.eu>
 * @author          Florent-Valéry Coen <florent.coen@gmail.com>
 */

#ifndef CC26x0_RFC_H
#define CC26x0_RFC_H

#include "cc26x0.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define RFC_RAT_BASE           (0x40043000)
/** @} */


typedef struct {
    reg32_t __reserved0;
    reg32_t CNT;
    reg32_t __reserved1[30];
    reg32_t CH0VAL;
    reg32_t CH1VAL;
    reg32_t CH2VAL;
    reg32_t CH3VAL;
    reg32_t CH4VAL;
    reg32_t CH5VAL;
    reg32_t CH6VAL;
    reg32_t CH7VAL;
} cc26x0_rfc_rat_regs_t;

#define RFC_RAT ((cc26x0_rfc_rat_regs_t *)(RFC_RAT_BASE))



/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define RFC_DBELL_BASE           (0x40041000)
/** @} */


typedef struct {
    reg32_t CMDR;
    reg32_t CMDSTA;
    reg32_t RFHWIFG;
    reg32_t RFHWIEN;
    reg32_t RFCPEIFG;
    reg32_t RFCPEIEN;
    reg32_t RFCPEISL;
    reg32_t RFACKIFG;
    reg32_t SYSGPOCTL;
} cc26x0_rfc_dbell_regs_t;

#define RFC_DBELL ((cc26x0_rfc_dbell_regs_t *)(RFC_DBELL_BASE))

#define CMDR_TYPE_mask            0x3
#define CMDR_TYPE_ROP_IMM         0x0
/* pointer is 32-bit word aligned and the 2 LSB are not stored (implicit 0) */
#define CMDR_CMDPTR_pos           2 /* used for immediate and radio commands */
#define CMDR_CMDPTR_mask          0xFFFFFFFC
#define CMDR_TYPE_DIRECT          0x1
#define CMDR_OPTPAREXT_pos        2 /* used for direct commands */
#define CMDR_OPTPAREXT_mask       0xFC
#define CMDR_OPTPAR_pos           8
#define CMDR_OPTPAR_mask          0xFF00
#define CMDR_CMDID_pos            16
#define CMDR_CMDID_mask           0xFFFF0000
/**
 * @addtogroup cc2x0_rop_command_ids
 * @{
 */
/* radio operation */
#define CMDR_CMDID_NOP            0x0801
#define CMDR_CMDID_SETUP          0x0802
#define CMDR_CMDID_FS_POWERUP     0x080C
#define CMDR_CMDID_FS_POWERDOWN   0x080D
#define CMDR_CMDID_FS             0x0803
#define CMDR_CMDID_FS_OFF         0x0804
#define CMDR_CMDID_FS_RX_TEST     0x0807
#define CMDR_CMDID_FS_TX_TEST     0x0808
#define CMDR_CMDID_SYNC_STOP_RAT  0x0809
#define CMDR_CMDID_SYNC_START_RAT 0x080A
#define CMDR_CMDID_COUNT          0x080B
#define CMDR_CMDID_SCH_IMM        0x0810
#define CMDR_CMDID_COUNT_BRANCH   0x0812
#define CMDR_CMDID_PATTERN_CHECK  0x0813
/* immediate */
#define CMDR_CMDID_ABORT          0x0401
#define CMDR_CMDID_STOP           0x0402
#define CMDR_CMDID_GET_RSSI       0x0403
#define CMDR_CMDID_UPD_RADIO_SET  0x0001
#define CMDR_CMDID_TRIGGER        0x0404
#define CMDR_CMDID_GET_FW_INFO    0x0002
#define CMDR_CMDID_START_RAT      0x0405
#define CMDR_CMDID_PING           0x0406
#define CMDR_CMDID_READ_RF_REG    0x0601
#define CMDR_CMDID_SET_RAT_CMP    0x000A
#define CMDR_CMDID_SET_RAT_CPT    0x0603
#define CMDR_CMDID_DISABLE_RAT_CH 0x0408
#define CMDR_CMDID_SET_RAT_OUT    0x0604
#define CMDR_CMDID_SET_ARM_RAT_CH 0x0409
#define CMDR_CMDID_DISARM_RAT_CH  0x040A
#define CMDR_CMDID_SET_TX_POWER   0x0010
#define CMDR_CMDID_UPDATE_FS      0x0011
#define CMDR_CMDID_BUS_REQUEST    0x040E
/* immediate queue */
#define CMDR_CMDID_ADD_DATA_ENTRY 0x0005
#define CMDR_CMDID_REM_DATA_ENTRY 0x0006
#define CMDR_CMDID_FLUSH_QUEUE    0x0007
#define CMDR_CMDID_CLEAR_RX       0x0008
#define CMDR_CMDID_REM_PENDING_ENTRIES  0x0009 /* TODO format */
/**@}*/

#define CMDSTA_RESULT_mask    0xFF
#define CMDSTA_RET_VAL_pos    8 /* MSB first */
#define CMDSTA_RET_VAL_mask   0xFFFFFF00

#define CMDSTA_RESULT_PENDING         0x00
#define CMDSTA_RESULT_DONE            0x01
#define CMDSTA_RESULT_ILLEGALPOINTER  0x81
#define CMDSTA_RESULT_UNKNOWNCMD      0x82
#define CMDSTA_RESULT_UNKNOWNDIRCMD   0x83
#define CMDSTA_RESULT_CONTEXTERROR    0x85
#define CMDSTA_RESULT_SCHEDULINGERROR 0x86
#define CMDSTA_RESULT_PARERROR        0x87
#define CMDSTA_RESULT_QUEUEERROR      0x88
#define CMDSTA_RESULT_QUEUEBUSY       0x89



#define RFHWIFG_FSCA          0x00000002
#define RFHWIFG_MDMDONE       0x00000004
#define RFHWIFG_MDMIN         0x00000008
#define RFHWIFG_MDMOUT        0x00000010
#define RFHWIFG_MDMSOFT       0x00000020
#define RFHWIFG_TRCTK         0x00000040
#define RFHWIFG_RFEDONE       0x00000100
#define RFHWIFG_RFESOFT1      0x00000200
#define RFHWIFG_RFESOFT2      0x00000400
#define RFHWIFG_RFESOFT3      0x00000800
#define RFHWIFG_RATCH0        0x00001000
#define RFHWIFG_RATCH1        0x00002000
#define RFHWIFG_RATCH2        0x00004000
#define RFHWIFG_RATCH3        0x00008000
#define RFHWIFG_RATCH4        0x00010000
#define RFHWIFG_RATCH5        0x00020000
#define RFHWIFG_RATCH6        0x00040000
#define RFHWIFG_RATCH7        0x00080000

#define RFHWIEN_FSCA          0x00000002
#define RFHWIEN_MDMDONE       0x00000004
#define RFHWIEN_MDMIN         0x00000008
#define RFHWIEN_MDMOUT        0x00000010
#define RFHWIEN_MDMSOFT       0x00000020
#define RFHWIEN_TRCTK         0x00000040
#define RFHWIEN_RFEDONE       0x00000100
#define RFHWIEN_RFESOFT1      0x00000200
#define RFHWIEN_RFESOFT2      0x00000400
#define RFHWIEN_RFESOFT3      0x00000800
#define RFHWIEN_RATCH0        0x00001000
#define RFHWIEN_RATCH1        0x00002000
#define RFHWIEN_RATCH2        0x00004000
#define RFHWIEN_RATCH3        0x00008000
#define RFHWIEN_RATCH4        0x00010000
#define RFHWIEN_RATCH5        0x00020000
#define RFHWIEN_RATCH6        0x00040000
#define RFHWIEN_RATCH7        0x00080000

#define RFCPEIFG_COMMAND_DONE         0x00000001
#define RFCPEIFG_LAST_COMMAND_DONE    0x00000002
#define RFCPEIFG_FG_COMMAND_DONE      0x00000004
#define RFCPEIFG_FG_LAST_COMMAND_DONE 0x00000008
#define RFCPEIFG_TX_DONE              0x00000010
#define RFCPEIFG_TX_ACK               0x00000020
#define RFCPEIFG_TX_CTRL              0x00000030
#define RFCPEIFG_TX_CTRL_ACK          0x00000040
#define RFCPEIFG_TX_CTRL_ACK_ACK      0x00000100
#define RFCPEIFG_TX_RETRANS           0x00000200
#define RFCPEIFG_TX_ENTRY_DONE        0x00000400
#define RFCPEIFG_TX_BUFFER_CHANGED    0x00000800
#define RFCPEIFG_TX_BUFFER_IRQ12      0x00001000
#define RFCPEIFG_TX_BUFFER_IRQ13      0x00002000
#define RFCPEIFG_TX_BUFFER_IRQ14      0x00004000
#define RFCPEIFG_TX_BUFFER_IRQ15      0x00008000
#define RFCPEIFG_RX_OK                0x00010000
#define RFCPEIFG_RX_NOK               0x00020000
#define RFCPEIFG_RX_IGNORED           0x00040000
#define RFCPEIFG_RX_EMPTY             0x00080000
#define RFCPEIFG_RX_CTRL              0x00100000
#define RFCPEIFG_RX_CTRL_ACK          0x00200000
#define RFCPEIFG_RX_BUF_FULL          0x00400000
#define RFCPEIFG_RX_ENTRY_DONE        0x00800000
#define RFCPEIFG_RX_DATA_WRITTEN      0x01000000
#define RFCPEIFG_RX_N_DATA_WRITTEN    0x02000000
#define RFCPEIFG_RX_ABORTED           0x04000000
#define RFCPEIFG_IRQ27                0x08000000
#define RFCPEIFG_SYNTH_NO_LOCK        0x10000000
#define RFCPEIFG_MODULES_UNLOCKED     0x20000000
#define RFCPEIFG_BOOT_DONE            0x40000000
#define RFCPEIFG_INTERNAL_ERROR       0x80000000

#define RFCPEIEN_COMMAND_DONE         0x00000001
#define RFCPEIEN_LAST_COMMAND_DONE    0x00000002
#define RFCPEIEN_FG_COMMAND_DONE      0x00000004
#define RFCPEIEN_FG_LAST_COMMAND_DONE 0x00000008
#define RFCPEIEN_TX_DONE              0x00000010
#define RFCPEIEN_TX_ACK               0x00000020
#define RFCPEIEN_TX_CTRL              0x00000030
#define RFCPEIEN_TX_CTRL_ACK          0x00000040
#define RFCPEIEN_TX_CTRL_ACK_ACK      0x00000100
#define RFCPEIEN_TX_RETRANS           0x00000200
#define RFCPEIEN_TX_ENTRY_DONE        0x00000400
#define RFCPEIEN_TX_BUFFER_CHANGED    0x00000800
#define RFCPEIEN_TX_BUFFER_IRQ12      0x00001000
#define RFCPEIEN_TX_BUFFER_IRQ13      0x00002000
#define RFCPEIEN_TX_BUFFER_IRQ14      0x00004000
#define RFCPEIEN_TX_BUFFER_IRQ15      0x00008000
#define RFCPEIEN_RX_OK                0x00010000
#define RFCPEIEN_RX_NOK               0x00020000
#define RFCPEIEN_RX_IGNORED           0x00040000
#define RFCPEIEN_RX_EMPTY             0x00080000
#define RFCPEIEN_RX_CTRL              0x00100000
#define RFCPEIEN_RX_CTRL_ACK          0x00200000
#define RFCPEIEN_RX_BUF_FULL          0x00400000
#define RFCPEIEN_RX_ENTRY_DONE        0x00800000
#define RFCPEIEN_RX_DATA_WRITTEN      0x01000000
#define RFCPEIEN_RX_N_DATA_WRITTEN    0x02000000
#define RFCPEIEN_RX_ABORTED           0x04000000
#define RFCPEIEN_IRQ27                0x08000000
#define RFCPEIEN_SYNTH_NO_LOCK        0x10000000
#define RFCPEIEN_MODULES_UNLOCKED     0x20000000
#define RFCPEIEN_BOOT_DONE            0x40000000
#define RFCPEIEN_INTERNAL_ERROR       0x80000000

#define SYSGPOCTL_GPOCTL0_pos         0x0
#define SYSGPOCTL_GPOCTL0_len         4
#define SYSGPOCTL_GPOCTL1_pos         0x4
#define SYSGPOCTL_GPOCTL1_len         4
#define SYSGPOCTL_GPOCTL2_pos         0x8
#define SYSGPOCTL_GPOCTL2_len         4
#define SYSGPOCTL_GPOCTL3_pos         0xc
#define SYSGPOCTL_GPOCTL3_len         4

#define SYSGPOCTL_GPOCTL_CPE0         0x0
#define SYSGPOCTL_GPOCTL_CPE1         0x1
#define SYSGPOCTL_GPOCTL_CPE2         0x2
#define SYSGPOCTL_GPOCTL_CPE3         0x3
#define SYSGPOCTL_GPOCTL_MCE0         0x4
#define SYSGPOCTL_GPOCTL_MCE1         0x5
#define SYSGPOCTL_GPOCTL_MCE2         0x6
#define SYSGPOCTL_GPOCTL_MCE3         0x7
#define SYSGPOCTL_GPOCTL_RFE0         0x8
#define SYSGPOCTL_GPOCTL_RFE1         0x9
#define SYSGPOCTL_GPOCTL_RFE2         0xa
#define SYSGPOCTL_GPOCTL_RFE3         0xb
#define SYSGPOCTL_GPOCTL_RAT0         0xc
#define SYSGPOCTL_GPOCTL_RAT1         0xd
#define SYSGPOCTL_GPOCTL_RAT2         0xe
#define SYSGPOCTL_GPOCTL_RAT3         0xf




/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define RFC_PWR_BASE           (0x40040000)
/** @} */


typedef struct {
    reg32_t PWMCLKEN;
} cc26x0_rfc_pwr_regs_t;

#define RFC_PWR ((cc26x0_rfc_pwr_regs_t *)(RFC_PWR_BASE))

#define PWMCLKEN_RFC        0x00000001
#define PWMCLKEN_CPE        0x00000002
#define PWMCLKEN_CPERAM     0x00000004
#define PWMCLKEN_MDM        0x00000008
#define PWMCLKEN_MDMRAM     0x00000010
#define PWMCLKEN_RFE        0x00000020
#define PWMCLKEN_RFERAM     0x00000040
#define PWMCLKEN_RAT        0x00000080
#define PWMCLKEN_PHA        0x00000100
#define PWMCLKEN_FSCA       0x00000200
#define PWMCLKEN_RFCTRC     0x00000400









/*
 * R: system CPU can read a result back; the radio CPU does not read the field
 * W: system CPU writes a value, the radio CPU reads it and does not modify it
 * R/W: system CPU writes an initial value, the radio CPU may modify it
 */
typedef struct {
    uint16_t commandNo; /* W */
    uint16_t status; /* R/W */
    uint32_t pNextOp; /* W */
    uint32_t startTime; /* W */
    uint8_t startTrigger; /* missing from datashiet... */
    uint8_t condition; /* W */
} radio_op_command_t;

/**
 * @addtogroup cc26x0_rop_status_codes
 * @{
 */
#define R_OP_STATUS_IDLE                0x0000
#define R_OP_STATUS_PENDING             0x0001
#define R_OP_STATUS_ACTIVE              0x0002
#define R_OP_STATUS_SKIPPED             0x0003 /* TODO value */

#define R_OP_STATUS_DONE_OK             0x0400
#define R_OP_STATUS_DONE_CNTDWN         0x0401
#define R_OP_STATUS_DONE_RXERR          0x0402
#define R_OP_STATUS_DONE_TIMEOUT        0x0403
#define R_OP_STATUS_DONE_STOPPED        0x0404
#define R_OP_STATUS_DONE_ABORT          0x0405

#define R_OP_STATUS_ERROR_PAST_START    0x0800
#define R_OP_STATUS_ERROR_START_TRIG    0x0801
#define R_OP_STATUS_ERROR_CONDITION     0x0802
#define R_OP_STATUS_ERROR_PAR           0x0803
#define R_OP_STATUS_ERROR_POINTER       0x0804
#define R_OP_STATUS_ERROR_CMDID         0x0805
#define R_OP_STATUS_ERROR_NO_SETUP      0x0807
#define R_OP_STATUS_ERROR_NO_FS         0x0808
#define R_OP_STATUS_ERROR_SYNTH_PROG    0x0809
#define R_OP_STATUS_ERROR_TXUNF         0x080a
#define R_OP_STATUS_ERROR_RXOVF         0x080b
#define R_OP_STATUS_ERROR_NO_RX         0x080c

#define R_OP_STARTTRIG_TYPE_mask            0xF
#define R_OP_STARTTRIG_TYPE_TRIG_NOW        0x0
#define R_OP_STARTTRIG_TYPE_TRIG_NEVER      0x1
#define R_OP_STARTTRIG_TYPE_TRIG_ABSTIME    0x2
#define R_OP_STARTTRIG_TYPE_REL_SUBMIT      0x3
#define R_OP_STARTTRIG_TYPE_REL_START       0x4
#define R_OP_STARTTRIG_TYPE_REL_PREVSTART   0x5
#define R_OP_STARTTRIG_TYPE_REL_FIRSTSTART  0x6
#define R_OP_STARTTRIG_TYPE_REL_PREVEND     0x7
#define R_OP_STARTTRIG_TYPE_REL_EVT1        0x8
#define R_OP_STARTTRIG_TYPE_REL_EVT2        0x9
#define R_OP_STARTTRIG_TYPE_EXTERNAL        0xA
#define R_OP_STARTTRIG_ALT_TRIG             0x10
#define R_OP_STARTTRIG_TRIG_NO_mask         0x60
#define R_OP_STARTTRIG_TRIG_NO_pos          5
#define R_OP_STARTTRIG_PAST_TRIG            0x80

/* for R_OP_STARTTRIG_TYPE_EXTERNAL startTime describes the trigger this way: */
#define R_OP_STARTTIME_INPUT_RISING         0x0
#define R_OP_STARTTIME_INPUT_FALLING        0x4
#define R_OP_STARTTIME_INPUT_BOTH           0x8
#define R_OP_STARTTIME_SOURCE_GPI0          0x1200 /* TODO wtf does this mean for a 5-bit field? */
#define R_OP_STARTTIME_SOURCE_GPI1          0x1300 /* 22: RFC_GPI0, 23: RFC_GPI1 assuming octal - above is binary... */

#define R_OP_CONDITION_RULE_mask            0x0F
#define R_OP_CONDITION_RULE_ALWAYS          0x00
#define R_OP_CONDITION_RULE_NEVER           0x01
#define R_OP_CONDITION_RULE_STOP_ON_FALSE   0x02
#define R_OP_CONDITION_RULE_STOP_ON_TRUE    0x03
#define R_OP_CONDITION_RULE_SKIP_ON_FALSE   0x04
#define R_OP_CONDITION_RULE_SKIP_ON_TRUE    0x05
#define R_OP_CONDITION_NSKIP_pos            4
#define R_OP_CONDITION_NSKIP_mask           0xF0
/**@}*/


typedef struct {
    radio_op_command_t op;
    uint8_t mode;
    /*
     * supported values: 0 (equivalent to 2), 2, 5, 6, 10, 12, 15, and 30
     * 0 or 2 only supported for devices that support 2.4-GHz operation
     */
    uint8_t loDivider;
    uint16_t config;
    uint16_t txPower;
    uint32_t pRegOverride;
} radio_setup_t;

#define RADIO_SETUP_MODE_BLE                0x0
#define RADIO_SETUP_MODE_IEEE8021504        0x1
#define RADIO_SETUP_MODE_2MBPS_GFSK         0x2
#define RADIO_SETUP_MODE_5MBPS_CODED_8FSK   0x5
#define RADIO_SETUP_MODE_KEEP               0xFF // update overrides only

/*
 * http://processors.wiki.ti.com/images/4/45/CC26xx_HW_training_RF_Frontends_and_Antennas.pdf
*/
#define RADIO_SETUP_CONFIG_FEM_DIFF             0x0 /* front-end mode */
#define RADIO_SETUP_CONFIG_FEM_SINGLE_END_RFP   0x1
#define RADIO_SETUP_CONFIG_FEM_SINGLE_END_RFN   0x2
#define RADIO_SETUP_CONFIG_FEM_START_RFP        0x3
#define RADIO_SETUP_CONFIG_FEM_START_RFN        0x4
#define RADIO_SETUP_CONFIG_FEM_SINGLE_END_RFP_E 0x5 /* ext front end control on rf pins */
#define RADIO_SETUP_CONFIG_FEM_SINGLE_END_RFN_E 0x6




typedef struct {
    uint32_t pCurrEntry; /* R/W NULL -> empty*/
    uint32_t pLastEntry; /* R/W */
} data_entry_queue_t;

typedef struct {
    uint32_t pNextEntry; /* R/W */
    uint8_t status; /* R/W */
    uint8_t config; /* W */
    uint16_t length; /* W TODO find out what the difference between this and the value referenced by config.lensz is */
    /* maybe you can think of this struct being extended like this:
     * DATA_ENTRY_CONFIG_TYPE_GENERAL:
     * uint8_t data[length];
     * DATA_ENTRY_CONFIG_TYPE_MULTIRX:
     * uint16_t dataCnt; // R
     * uint16_t nextIndex; // R
     * uint16_t data[length]; // R
     * DATA_ENTRY_CONFIG_TYPE_PTR:
     * uint32_t pData;
     */
} data_entry_t;

typedef struct {
	uint16_t commandNo;
	uint16_t __reserved;
	uint32_t pQueue;
	uint32_t pEntry;
} data_queue_t;


#define DATA_ENTRY_STATUS_PENDING       0
#define DATA_ENTRY_STATUS_ACTIVE        1
#define DATA_ENTRY_STATUS_BUSY          2
#define DATA_ENTRY_STATUS_FINISHED      3

#define DATA_ENTRY_CONFIG_TYPE_mask     0x3
#define DATA_ENTRY_CONFIG_TYPE_GENERAL  0x0
#define DATA_ENTRY_CONFIG_TYPE_MULTIRX  0x1
#define DATA_ENTRY_CONFIG_TYPE_PTR      0x2
#define DATA_ENTRY_CONFIG_LENSZ_pos     4
#define DATA_ENTRY_CONFIG_LENSZ_mask    0xC
#define DATA_ENTRY_CONFIG_LENSZ_NOLEN   0x0
#define DATA_ENTRY_CONFIG_LENSZ_1BYTE   0x1
#define DATA_ENTRY_CONFIG_LENSZ_2BYTE   0x2





#include "cc26x0_rfc_ble.h"

#ifdef __cplusplus
}
#endif

#endif /* CC26x0_RFC_H */

/*@}*/
