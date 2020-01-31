/******************************************************************************
*  Filename:       rf_ble_cmd.h
*  Revised:        2017-11-10 10:42:47 +0100 (Fri, 10 Nov 2017)
*  Revision:       18052
*
*  Description:    CC13x0 API for Bluetooth Low Energy commands
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

#ifndef __BLE_CMD_H
#define __BLE_CMD_H

#ifndef __RFC_STRUCT
#define __RFC_STRUCT
#endif

#ifndef __RFC_STRUCT_ATTR
#if defined(__GNUC__)
#define __RFC_STRUCT_ATTR __attribute__ ((aligned (4)))
#elif defined(__TI_ARM__)
#define __RFC_STRUCT_ATTR __attribute__ ((__packed__,aligned (4)))
#else
#define __RFC_STRUCT_ATTR
#endif
#endif

//! \addtogroup rfc
//! @{

//! \addtogroup ble_cmd
//! @{

#include <stdint.h>
#include "rf_mailbox.h"
#include "rf_common_cmd.h"

typedef struct __RFC_STRUCT rfc_bleRadioOp_s rfc_bleRadioOp_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_SLAVE_s rfc_CMD_BLE_SLAVE_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_MASTER_s rfc_CMD_BLE_MASTER_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_ADV_s rfc_CMD_BLE_ADV_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_ADV_DIR_s rfc_CMD_BLE_ADV_DIR_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_ADV_NC_s rfc_CMD_BLE_ADV_NC_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_ADV_SCAN_s rfc_CMD_BLE_ADV_SCAN_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_SCANNER_s rfc_CMD_BLE_SCANNER_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_INITIATOR_s rfc_CMD_BLE_INITIATOR_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_GENERIC_RX_s rfc_CMD_BLE_GENERIC_RX_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_TX_TEST_s rfc_CMD_BLE_TX_TEST_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE_ADV_PAYLOAD_s rfc_CMD_BLE_ADV_PAYLOAD_t;
typedef struct __RFC_STRUCT rfc_CMD_BLE5_RADIO_SETUP_s rfc_CMD_BLE5_RADIO_SETUP_t;
typedef struct __RFC_STRUCT rfc_bleMasterSlavePar_s rfc_bleMasterSlavePar_t;
typedef struct __RFC_STRUCT rfc_bleMasterPar_s rfc_bleMasterPar_t;
typedef struct __RFC_STRUCT rfc_bleSlavePar_s rfc_bleSlavePar_t;
typedef struct __RFC_STRUCT rfc_bleAdvPar_s rfc_bleAdvPar_t;
typedef struct __RFC_STRUCT rfc_bleScannerPar_s rfc_bleScannerPar_t;
typedef struct __RFC_STRUCT rfc_bleInitiatorPar_s rfc_bleInitiatorPar_t;
typedef struct __RFC_STRUCT rfc_bleGenericRxPar_s rfc_bleGenericRxPar_t;
typedef struct __RFC_STRUCT rfc_bleTxTestPar_s rfc_bleTxTestPar_t;
typedef struct __RFC_STRUCT rfc_bleMasterSlaveOutput_s rfc_bleMasterSlaveOutput_t;
typedef struct __RFC_STRUCT rfc_bleAdvOutput_s rfc_bleAdvOutput_t;
typedef struct __RFC_STRUCT rfc_bleScannerOutput_s rfc_bleScannerOutput_t;
typedef struct __RFC_STRUCT rfc_bleInitiatorOutput_s rfc_bleInitiatorOutput_t;
typedef struct __RFC_STRUCT rfc_bleGenericRxOutput_s rfc_bleGenericRxOutput_t;
typedef struct __RFC_STRUCT rfc_bleTxTestOutput_s rfc_bleTxTestOutput_t;
typedef struct __RFC_STRUCT rfc_bleWhiteListEntry_s rfc_bleWhiteListEntry_t;
typedef struct __RFC_STRUCT rfc_bleRxStatus_s rfc_bleRxStatus_t;

//! \addtogroup bleRadioOp
//! @{
struct __RFC_STRUCT rfc_bleRadioOp_s {
   uint16_t commandNo;                  //!<        The command ID number
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   uint8_t* pParams;                    //!<        Pointer to command specific parameter structure
   uint8_t* pOutput;                    //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_SLAVE
//! @{
#define CMD_BLE_SLAVE                                           0x1801
//! BLE Slave Command
struct __RFC_STRUCT rfc_CMD_BLE_SLAVE_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1801
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleSlavePar_t *pParams;          //!<        Pointer to command specific parameter structure
   rfc_bleMasterSlaveOutput_t *pOutput; //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_MASTER
//! @{
#define CMD_BLE_MASTER                                          0x1802
//! BLE Master Command
struct __RFC_STRUCT rfc_CMD_BLE_MASTER_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1802
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleMasterPar_t *pParams;         //!<        Pointer to command specific parameter structure
   rfc_bleMasterSlaveOutput_t *pOutput; //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_ADV
//! @{
#define CMD_BLE_ADV                                             0x1803
//! BLE Connectable Undirected Advertiser Command
struct __RFC_STRUCT rfc_CMD_BLE_ADV_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1803
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleAdvPar_t *pParams;            //!<        Pointer to command specific parameter structure
   rfc_bleAdvOutput_t *pOutput;         //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_ADV_DIR
//! @{
#define CMD_BLE_ADV_DIR                                         0x1804
//! BLE Connectable Directed Advertiser Command
struct __RFC_STRUCT rfc_CMD_BLE_ADV_DIR_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1804
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleAdvPar_t *pParams;            //!<        Pointer to command specific parameter structure
   rfc_bleAdvOutput_t *pOutput;         //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_ADV_NC
//! @{
#define CMD_BLE_ADV_NC                                          0x1805
//! BLE Non-Connectable Advertiser Command
struct __RFC_STRUCT rfc_CMD_BLE_ADV_NC_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1805
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleAdvPar_t *pParams;            //!<        Pointer to command specific parameter structure
   rfc_bleAdvOutput_t *pOutput;         //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_ADV_SCAN
//! @{
#define CMD_BLE_ADV_SCAN                                        0x1806
//! BLE Scannable Undirected Advertiser Command
struct __RFC_STRUCT rfc_CMD_BLE_ADV_SCAN_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1806
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleAdvPar_t *pParams;            //!<        Pointer to command specific parameter structure
   rfc_bleAdvOutput_t *pOutput;         //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_SCANNER
//! @{
#define CMD_BLE_SCANNER                                         0x1807
//! BLE Scanner Command
struct __RFC_STRUCT rfc_CMD_BLE_SCANNER_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1807
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleScannerPar_t *pParams;        //!<        Pointer to command specific parameter structure
   rfc_bleScannerOutput_t *pOutput;     //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_INITIATOR
//! @{
#define CMD_BLE_INITIATOR                                       0x1808
//! BLE Initiator Command
struct __RFC_STRUCT rfc_CMD_BLE_INITIATOR_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1808
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleInitiatorPar_t *pParams;      //!<        Pointer to command specific parameter structure
   rfc_bleInitiatorOutput_t *pOutput;   //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_GENERIC_RX
//! @{
#define CMD_BLE_GENERIC_RX                                      0x1809
//! BLE Generic Receiver Command
struct __RFC_STRUCT rfc_CMD_BLE_GENERIC_RX_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1809
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleGenericRxPar_t *pParams;      //!<        Pointer to command specific parameter structure
   rfc_bleGenericRxOutput_t *pOutput;   //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_TX_TEST
//! @{
#define CMD_BLE_TX_TEST                                         0x180A
//! BLE PHY Test Transmitter Command
struct __RFC_STRUCT rfc_CMD_BLE_TX_TEST_s {
   uint16_t commandNo;                  //!<        The command ID number 0x180A
   uint16_t status;                     //!< \brief An integer telling the status of the command. This value is
                                        //!<        updated by the radio CPU during operation and may be read by the
                                        //!<        system CPU at any time.
   rfc_radioOp_t *pNextOp;              //!<        Pointer to the next operation to run after this operation is done
   ratmr_t startTime;                   //!<        Absolute or relative start time (depending on the value of <code>startTrigger</code>)
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } startTrigger;                      //!<        Identification of the trigger that starts the operation
   struct {
      uint8_t rule:4;                   //!<        Condition for running next command: Rule for how to proceed
      uint8_t nSkip:4;                  //!<        Number of skips + 1 if the rule involves skipping. 0: same, 1: next, 2: skip next, ...
   } condition;
   uint8_t channel;                     //!< \brief Channel to use<br>
                                        //!<        0--39: BLE advertising/data channel number<br>
                                        //!<        60--207: Custom frequency; (2300 + <code>channel</code>) MHz<br>
                                        //!<        255: Use existing frequency<br>
                                        //!<        Others: <i>Reserved</i>
   struct {
      uint8_t init:7;                   //!< \brief If <code>bOverride</code> = 1 or custom frequency is used:<br>
                                        //!<        0: Do not use whitening<br>
                                        //!<        Other value: Initialization for 7-bit LFSR whitener
      uint8_t bOverride:1;              //!< \brief 0: Use default whitening for BLE advertising/data channels<br>
                                        //!<        1: Override whitening initialization with value of init
   } whitening;
   rfc_bleTxTestPar_t *pParams;         //!<        Pointer to command specific parameter structure
   rfc_bleTxTestOutput_t *pOutput;      //!<        Pointer to command specific output structure
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE_ADV_PAYLOAD
//! @{
#define CMD_BLE_ADV_PAYLOAD                                     0x1001
//! BLE Update Advertising Payload Command
struct __RFC_STRUCT rfc_CMD_BLE_ADV_PAYLOAD_s {
   uint16_t commandNo;                  //!<        The command ID number 0x1001
   uint8_t payloadType;                 //!< \brief 0: Advertising data<br>
                                        //!<        1: Scan response data
   uint8_t newLen;                      //!<        Length of the new payload
   uint8_t* pNewData;                   //!<        Pointer to the buffer containing the new data
   rfc_bleAdvPar_t *pParams;            //!<        Pointer to the parameter structure to update
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup CMD_BLE5_RADIO_SETUP
//! @{
#define CMD_BLE5_RADIO_SETUP                                    0x1820
//! Define only for compatibility with CC26XXR2F family. Command will result in error if sent.
struct __RFC_STRUCT rfc_CMD_BLE5_RADIO_SETUP_s {
   uint8_t dummy0;
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleMasterSlavePar
//! @{
struct __RFC_STRUCT rfc_bleMasterSlavePar_s {
   dataQueue_t* pRxQ;                   //!<        Pointer to receive queue
   dataQueue_t* pTxQ;                   //!<        Pointer to transmit queue
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets from Rx queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bAutoFlushEmpty:1;        //!<        If 1, automatically remove empty packets from Rx queue
      uint8_t bIncludeLenByte:1;        //!<        If 1, include the received length byte in the stored packet; otherwise discard it
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConfig;                          //!<        Configuration bits for the receive queue entries
   struct {
      uint8_t lastRxSn:1;               //!<        The SN bit of the header of the last packet received with CRC OK
      uint8_t lastTxSn:1;               //!<        The SN bit of the header of the last transmitted packet
      uint8_t nextTxSn:1;               //!<        The SN bit of the header of the next packet to transmit
      uint8_t bFirstPkt:1;              //!<        For slave: 0 if a packet has been transmitted on the connection, 1 otherwise
      uint8_t bAutoEmpty:1;             //!<        1 if the last transmitted packet was an auto-empty packet
      uint8_t bLlCtrlTx:1;              //!<        1 if the last transmitted packet was an LL control packet (LLID = 11)
      uint8_t bLlCtrlAckRx:1;           //!<        1 if the last received packet was the ACK of an LL control packet
      uint8_t bLlCtrlAckPending:1;      //!<        1 if the last successfully received packet was an LL control packet which has not yet been ACK'ed
   } seqStat;
   uint8_t maxNack;                     //!<        Maximum number of NACKs received before operation ends. 0: No limit
   uint8_t maxPkt;                      //!<        Maximum number of packets transmitted in the operation before it ends. 0: No limit
   uint32_t accessAddress;              //!<        Access address used on the connection
   uint8_t crcInit0;                    //!<        CRC initialization value used on the connection -- least significant byte
   uint8_t crcInit1;                    //!<        CRC initialization value used on the connection -- middle byte
   uint8_t crcInit2;                    //!<        CRC initialization value used on the connection -- most significant byte
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleMasterPar
//! @{
//! Parameter structure for master (CMD_BLE_MASTER)

struct __RFC_STRUCT rfc_bleMasterPar_s {
   dataQueue_t* pRxQ;                   //!<        Pointer to receive queue
   dataQueue_t* pTxQ;                   //!<        Pointer to transmit queue
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets from Rx queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bAutoFlushEmpty:1;        //!<        If 1, automatically remove empty packets from Rx queue
      uint8_t bIncludeLenByte:1;        //!<        If 1, include the received length byte in the stored packet; otherwise discard it
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConfig;                          //!<        Configuration bits for the receive queue entries
   struct {
      uint8_t lastRxSn:1;               //!<        The SN bit of the header of the last packet received with CRC OK
      uint8_t lastTxSn:1;               //!<        The SN bit of the header of the last transmitted packet
      uint8_t nextTxSn:1;               //!<        The SN bit of the header of the next packet to transmit
      uint8_t bFirstPkt:1;              //!<        For slave: 0 if a packet has been transmitted on the connection, 1 otherwise
      uint8_t bAutoEmpty:1;             //!<        1 if the last transmitted packet was an auto-empty packet
      uint8_t bLlCtrlTx:1;              //!<        1 if the last transmitted packet was an LL control packet (LLID = 11)
      uint8_t bLlCtrlAckRx:1;           //!<        1 if the last received packet was the ACK of an LL control packet
      uint8_t bLlCtrlAckPending:1;      //!<        1 if the last successfully received packet was an LL control packet which has not yet been ACK'ed
   } seqStat;
   uint8_t maxNack;                     //!<        Maximum number of NACKs received before operation ends. 0: No limit
   uint8_t maxPkt;                      //!<        Maximum number of packets transmitted in the operation before it ends. 0: No limit
   uint32_t accessAddress;              //!<        Access address used on the connection
   uint8_t crcInit0;                    //!<        CRC initialization value used on the connection -- least significant byte
   uint8_t crcInit1;                    //!<        CRC initialization value used on the connection -- middle byte
   uint8_t crcInit2;                    //!<        CRC initialization value used on the connection -- most significant byte
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } endTrigger;                        //!<        Trigger that causes the device to end the connection event as soon as allowed
   ratmr_t endTime;                     //!< \brief Time used together with <code>endTrigger</code> that causes the device to end the
                                        //!<        connection event as soon as allowed
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleSlavePar
//! @{
//! Parameter structure for slave (CMD_BLE_SLAVE)

struct __RFC_STRUCT rfc_bleSlavePar_s {
   dataQueue_t* pRxQ;                   //!<        Pointer to receive queue
   dataQueue_t* pTxQ;                   //!<        Pointer to transmit queue
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets from Rx queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bAutoFlushEmpty:1;        //!<        If 1, automatically remove empty packets from Rx queue
      uint8_t bIncludeLenByte:1;        //!<        If 1, include the received length byte in the stored packet; otherwise discard it
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConfig;                          //!<        Configuration bits for the receive queue entries
   struct {
      uint8_t lastRxSn:1;               //!<        The SN bit of the header of the last packet received with CRC OK
      uint8_t lastTxSn:1;               //!<        The SN bit of the header of the last transmitted packet
      uint8_t nextTxSn:1;               //!<        The SN bit of the header of the next packet to transmit
      uint8_t bFirstPkt:1;              //!<        For slave: 0 if a packet has been transmitted on the connection, 1 otherwise
      uint8_t bAutoEmpty:1;             //!<        1 if the last transmitted packet was an auto-empty packet
      uint8_t bLlCtrlTx:1;              //!<        1 if the last transmitted packet was an LL control packet (LLID = 11)
      uint8_t bLlCtrlAckRx:1;           //!<        1 if the last received packet was the ACK of an LL control packet
      uint8_t bLlCtrlAckPending:1;      //!<        1 if the last successfully received packet was an LL control packet which has not yet been ACK'ed
   } seqStat;
   uint8_t maxNack;                     //!<        Maximum number of NACKs received before operation ends. 0: No limit
   uint8_t maxPkt;                      //!<        Maximum number of packets transmitted in the operation before it ends. 0: No limit
   uint32_t accessAddress;              //!<        Access address used on the connection
   uint8_t crcInit0;                    //!<        CRC initialization value used on the connection -- least significant byte
   uint8_t crcInit1;                    //!<        CRC initialization value used on the connection -- middle byte
   uint8_t crcInit2;                    //!<        CRC initialization value used on the connection -- most significant byte
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } timeoutTrigger;                    //!<        Trigger that defines timeout of the first receive operation
   ratmr_t timeoutTime;                 //!< \brief Time used together with <code>timeoutTrigger</code> that defines timeout of the first
                                        //!<        receive operation
   uint16_t __dummy0;
   uint8_t __dummy1;
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } endTrigger;                        //!<        Trigger that causes the device to end the connection event as soon as allowed
   ratmr_t endTime;                     //!< \brief Time used together with <code>endTrigger</code> that causes the device to end the
                                        //!<        connection event as soon as allowed
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleAdvPar
//! @{
//! Parameter structure for advertiser (CMD_BLE_ADV*)

struct __RFC_STRUCT rfc_bleAdvPar_s {
   dataQueue_t* pRxQ;                   //!<        Pointer to receive queue
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets from Rx queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bAutoFlushEmpty:1;        //!<        If 1, automatically remove empty packets from Rx queue
      uint8_t bIncludeLenByte:1;        //!<        If 1, include the received length byte in the stored packet; otherwise discard it
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConfig;                          //!<        Configuration bits for the receive queue entries
   struct {
      uint8_t advFilterPolicy:2;        //!< \brief Advertiser filter policy<br>
                                        //!<        0: Process scan and connect requests from all devices<br>
                                        //!<        1: Process connect requests from all devices and only scan requests from
                                        //!<        devices that are in the white list<br>
                                        //!<        2: Process scan requests from all devices and only connect requests from
                                        //!<        devices that are in the white list<br>
                                        //!<        3: Process scan and connect requests only from devices in the white list
      uint8_t deviceAddrType:1;         //!<        The type of the device address -- public (0) or random (1)
      uint8_t peerAddrType:1;           //!<        Directed advertiser: The type of the peer address -- public (0) or random (1)
      uint8_t bStrictLenFilter:1;       //!< \brief 0: Accept any packet with a valid advertising packet length<br>
                                        //!<        1: Discard messages with illegal length for the given packet type
      uint8_t :2;
      uint8_t rpaMode:1;                //!< \brief Resolvable private address mode<br>
                                        //!<        0: Normal operation<br>
                                        //!<        1: Use white list for a received RPA regardless of filter policy
   } advConfig;
   uint8_t advLen;                      //!<        Size of advertiser data
   uint8_t scanRspLen;                  //!<        Size of scan response data
   uint8_t* pAdvData;                   //!<        Pointer to buffer containing ADV*_IND data
   uint8_t* pScanRspData;               //!<        Pointer to buffer containing SCAN_RSP data
   uint16_t* pDeviceAddress;            //!<        Pointer to device address used for this device
   rfc_bleWhiteListEntry_t *pWhiteList; //!<        Pointer to white list or peer address (directed advertiser)
   uint16_t __dummy0;
   uint8_t __dummy1;
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } endTrigger;                        //!<        Trigger that causes the device to end the advertiser event as soon as allowed
   ratmr_t endTime;                     //!< \brief Time used together with <code>endTrigger</code> that causes the device to end the
                                        //!<        advertiser event as soon as allowed
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleScannerPar
//! @{
//! Parameter structure for scanner (CMD_BLE_SCANNER)

struct __RFC_STRUCT rfc_bleScannerPar_s {
   dataQueue_t* pRxQ;                   //!<        Pointer to receive queue
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets from Rx queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bAutoFlushEmpty:1;        //!<        If 1, automatically remove empty packets from Rx queue
      uint8_t bIncludeLenByte:1;        //!<        If 1, include the received length byte in the stored packet; otherwise discard it
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConfig;                          //!<        Configuration bits for the receive queue entries
   struct {
      uint8_t scanFilterPolicy:1;       //!< \brief Scanning filter policy<br>
                                        //!<        0: Accept all advertisement packets<br>
                                        //!<        1: Accept only advertisement packets from devices where the advertiser's address
                                        //!<        is in the White list.
      uint8_t bActiveScan:1;            //!< \brief 0: Passive scan<br>
                                        //!<        1: Active scan
      uint8_t deviceAddrType:1;         //!<        The type of the device address -- public (0) or random (1)
      uint8_t :1;
      uint8_t bStrictLenFilter:1;       //!< \brief 0: Accept any packet with a valid advertising packet length<br>
                                        //!<        1: Discard messages with illegal length for the given packet type
      uint8_t bAutoWlIgnore:1;          //!<        1: Automatically set ignore bit in white list
      uint8_t bEndOnRpt:1;              //!< \brief 0: Continue scanner operation after each reporting ADV*_IND or sending SCAN_RSP<br>
                                        //!<        1: End scanner operation after each reported ADV*_IND and potentially SCAN_RSP
      uint8_t rpaMode:1;                //!< \brief Resolvable private address mode<br>
                                        //!<        0: Normal operation<br>
                                        //!<        1: Use white list for a received RPA regardless of filter policy
   } scanConfig;
   uint16_t randomState;                //!<        State for pseudo-random number generation used in backoff procedure
   uint16_t backoffCount;               //!<        Parameter <i>backoffCount</i> used in backoff procedure, cf. Bluetooth 4.0 spec
   struct {
      uint8_t logUpperLimit:4;          //!<        Binary logarithm of parameter upperLimit used in scanner backoff procedure
      uint8_t bLastSucceeded:1;         //!< \brief 1 if the last SCAN_RSP was successfully received and <code>upperLimit</code>
                                        //!<        not changed
      uint8_t bLastFailed:1;            //!< \brief 1 if reception of the last SCAN_RSP failed and <code>upperLimit</code> was not
                                        //!<        changed
   } backoffPar;
   uint8_t scanReqLen;                  //!<        Size of scan request data
   uint8_t* pScanReqData;               //!<        Pointer to buffer containing SCAN_REQ data
   uint16_t* pDeviceAddress;            //!<        Pointer to device address used for this device
   rfc_bleWhiteListEntry_t *pWhiteList; //!<        Pointer to white list
   uint16_t __dummy0;
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } timeoutTrigger;                    //!<        Trigger that causes the device to stop receiving as soon as allowed
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } endTrigger;                        //!<        Trigger that causes the device to stop receiving as soon as allowed
   ratmr_t timeoutTime;                 //!< \brief Time used together with <code>timeoutTrigger</code> that causes the device to stop
                                        //!<        receiving as soon as allowed, ending with BLE_DONE_RXTIMEOUT
   ratmr_t endTime;                     //!< \brief Time used together with <code>endTrigger</code> that causes the device to stop
                                        //!<        receiving as soon as allowed, ending with BLE_DONE_ENDED
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleInitiatorPar
//! @{
//! Parameter structure for initiator (CMD_BLE_INITIATOR)

struct __RFC_STRUCT rfc_bleInitiatorPar_s {
   dataQueue_t* pRxQ;                   //!<        Pointer to receive queue
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets from Rx queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bAutoFlushEmpty:1;        //!<        If 1, automatically remove empty packets from Rx queue
      uint8_t bIncludeLenByte:1;        //!<        If 1, include the received length byte in the stored packet; otherwise discard it
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConfig;                          //!<        Configuration bits for the receive queue entries
   struct {
      uint8_t bUseWhiteList:1;          //!< \brief Initiator filter policy<br>
                                        //!<        0: Use specific peer address<br>
                                        //!<        1: Use white list
      uint8_t bDynamicWinOffset:1;      //!< \brief 0: No dynamic WinOffset insertion<br>
                                        //!<        1: Use dynamic WinOffset insertion
      uint8_t deviceAddrType:1;         //!<        The type of the device address -- public (0) or random (1)
      uint8_t peerAddrType:1;           //!<        The type of the peer address -- public (0) or random (1)
      uint8_t bStrictLenFilter:1;       //!< \brief 0: Accept any packet with a valid advertising packet length<br>
                                        //!<        1: Discard messages with illegal length for the given packet type
   } initConfig;
   uint8_t __dummy0;
   uint8_t connectReqLen;               //!<        Size of connect request data
   uint8_t* pConnectReqData;            //!<        Pointer to buffer containing LLData to go in the CONNECT_REQ
   uint16_t* pDeviceAddress;            //!<        Pointer to device address used for this device
   rfc_bleWhiteListEntry_t *pWhiteList; //!<        Pointer to white list or peer address
   ratmr_t connectTime;                 //!< \brief Indication of timer value of the first possible start time of the first connection event.
                                        //!<        Set to the calculated value if a connection is made and to the next possible connection
                                        //!<        time if not.
   uint16_t __dummy1;
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } timeoutTrigger;                    //!<        Trigger that causes the device to stop receiving as soon as allowed
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } endTrigger;                        //!<        Trigger that causes the device to stop receiving as soon as allowed
   ratmr_t timeoutTime;                 //!< \brief Time used together with <code>timeoutTrigger</code> that causes the device to stop
                                        //!<        receiving as soon as allowed, ending with BLE_DONE_RXTIMEOUT
   ratmr_t endTime;                     //!< \brief Time used together with <code>endTrigger</code> that causes the device to stop
                                        //!<        receiving as soon as allowed, ending with BLE_DONE_ENDED
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleGenericRxPar
//! @{
//! Parameter structure for generic Rx (CMD_BLE_GENERIC_RX)

struct __RFC_STRUCT rfc_bleGenericRxPar_s {
   dataQueue_t* pRxQ;                   //!<        Pointer to receive queue. May be NULL; if so, received packets are not stored
   struct {
      uint8_t bAutoFlushIgnored:1;      //!<        If 1, automatically remove ignored packets from Rx queue
      uint8_t bAutoFlushCrcErr:1;       //!<        If 1, automatically remove packets with CRC error from Rx queue
      uint8_t bAutoFlushEmpty:1;        //!<        If 1, automatically remove empty packets from Rx queue
      uint8_t bIncludeLenByte:1;        //!<        If 1, include the received length byte in the stored packet; otherwise discard it
      uint8_t bIncludeCrc:1;            //!<        If 1, include the received CRC field in the stored packet; otherwise discard it
      uint8_t bAppendRssi:1;            //!<        If 1, append an RSSI byte to the packet in the Rx queue
      uint8_t bAppendStatus:1;          //!<        If 1, append a status byte to the packet in the Rx queue
      uint8_t bAppendTimestamp:1;       //!<        If 1, append a timestamp to the packet in the Rx queue
   } rxConfig;                          //!<        Configuration bits for the receive queue entries
   uint8_t bRepeat;                     //!< \brief 0: End operation after receiving a packet<br>
                                        //!<        1: Restart receiver after receiving a packet
   uint16_t __dummy0;
   uint32_t accessAddress;              //!<        Access address used on the connection
   uint8_t crcInit0;                    //!<        CRC initialization value used on the connection -- least significant byte
   uint8_t crcInit1;                    //!<        CRC initialization value used on the connection -- middle byte
   uint8_t crcInit2;                    //!<        CRC initialization value used on the connection -- most significant byte
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } endTrigger;                        //!<        Trigger that causes the device to end the Rx operation
   ratmr_t endTime;                     //!< \brief Time used together with <code>endTrigger</code> that causes the device to end the
                                        //!<        Rx operation
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleTxTestPar
//! @{
//! Parameter structure for Tx test (CMD_BLE_TX_TEST)

struct __RFC_STRUCT rfc_bleTxTestPar_s {
   uint16_t numPackets;                 //!< \brief Number of packets to transmit<br>
                                        //!<        0: Transmit unlimited number of packets
   uint8_t payloadLength;               //!<        The number of payload bytes in each packet.
   uint8_t packetType;                  //!< \brief The packet type to be used, encoded according to the Bluetooth 4.0 spec, Volume 2, Part E,
                                        //!<        Section 7.8.29
   ratmr_t period;                      //!<        Number of radio timer cycles between the start of each packet
   struct {
      uint8_t bOverrideDefault:1;       //!< \brief 0: Use default packet encoding<br>
                                        //!<        1: Override packet contents
      uint8_t bUsePrbs9:1;              //!< \brief If <code>bOverride</code> is 1:<br>
                                        //!<        1: Use PRBS9 encoding of packet
      uint8_t bUsePrbs15:1;             //!< \brief If <code>bOverride</code> is 1:<br>
                                        //!<        1: Use PRBS15 encoding of packet
   } config;
   uint8_t byteVal;                     //!<        If <code>config.bOverride</code> is 1, value of each byte to be sent
   uint8_t __dummy0;
   struct {
      uint8_t triggerType:4;            //!<        The type of trigger
      uint8_t bEnaCmd:1;                //!< \brief 0: No alternative trigger command<br>
                                        //!<        1: CMD_TRIGGER can be used as an alternative trigger
      uint8_t triggerNo:2;              //!<        The trigger number of the CMD_TRIGGER command that triggers this action
      uint8_t pastTrig:1;               //!< \brief 0: A trigger in the past is never triggered, or for start of commands, give an error<br>
                                        //!<        1: A trigger in the past is triggered as soon as possible
   } endTrigger;                        //!<        Trigger that causes the device to end the Test Tx operation
   ratmr_t endTime;                     //!< \brief Time used together with <code>endTrigger</code> that causes the device to end the
                                        //!<        Test Tx operation
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleMasterSlaveOutput
//! @{
//! Output structure for master and slave (CMD_BLE_MASTER and CMD_BLE_SLAVE)

struct __RFC_STRUCT rfc_bleMasterSlaveOutput_s {
   uint8_t nTx;                         //!< \brief Total number of packets (including auto-empty and retransmissions) that have been
                                        //!<        transmitted
   uint8_t nTxAck;                      //!<        Total number of transmitted packets (including auto-empty) that have been ACK'ed
   uint8_t nTxCtrl;                     //!<        Number of unique LL control packets from the Tx queue that have been transmitted
   uint8_t nTxCtrlAck;                  //!<        Number of LL control packets from the Tx queue that have been finished (ACK'ed)
   uint8_t nTxCtrlAckAck;               //!< \brief Number of LL control packets that have been ACK'ed and where an ACK has been sent in
                                        //!<        response
   uint8_t nTxRetrans;                  //!<        Number of retransmissions that has been done
   uint8_t nTxEntryDone;                //!<        Number of packets from the Tx queue that have been finished (ACK'ed)
   uint8_t nRxOk;                       //!<        Number of packets that have been received with payload, CRC OK and not ignored
   uint8_t nRxCtrl;                     //!<        Number of LL control packets that have been received with CRC OK and not ignored
   uint8_t nRxCtrlAck;                  //!< \brief Number of LL control packets that have been received with CRC OK and not ignored, and
                                        //!<        then ACK'ed
   uint8_t nRxNok;                      //!<        Number of packets that have been received with CRC error
   uint8_t nRxIgnored;                  //!< \brief Number of packets that have been received with CRC OK and ignored due to repeated
                                        //!<        sequence number
   uint8_t nRxEmpty;                    //!<        Number of packets that have been received with CRC OK and no payload
   uint8_t nRxBufFull;                  //!<        Number of packets that have been received and discarded due to lack of buffer space
   int8_t lastRssi;                     //!<        RSSI of last received packet
   struct {
      uint8_t bTimeStampValid:1;        //!<        1 if a valid time stamp has been written to timeStamp; 0 otherwise
      uint8_t bLastCrcErr:1;            //!<        1 if the last received packet had CRC error; 0 otherwise
      uint8_t bLastIgnored:1;           //!<        1 if the last received packet with CRC OK was ignored; 0 otherwise
      uint8_t bLastEmpty:1;             //!<        1 if the last received packet with CRC OK was empty; 0 otherwise
      uint8_t bLastCtrl:1;              //!<        1 if the last received packet with CRC OK was empty; 0 otherwise
      uint8_t bLastMd:1;                //!<        1 if the last received packet with CRC OK had MD = 1; 0 otherwise
      uint8_t bLastAck:1;               //!< \brief 1 if the last received packet with CRC OK was an ACK of a transmitted packet;
                                        //!<        0 otherwise
   } pktStatus;
   ratmr_t timeStamp;                   //!<        Slave operation: Time stamp of first received packet
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleAdvOutput
//! @{
//! Output structure for advertiser (CMD_BLE_ADV*)

struct __RFC_STRUCT rfc_bleAdvOutput_s {
   uint16_t nTxAdvInd;                  //!<        Number of ADV*_IND packets completely transmitted
   uint8_t nTxScanRsp;                  //!<        Number of  SCAN_RSP packets transmitted
   uint8_t nRxScanReq;                  //!<        Number of SCAN_REQ packets received OK and not ignored
   uint8_t nRxConnectReq;               //!<        Number of CONNECT_REQ packets received OK and not ignored
   uint8_t __dummy0;
   uint16_t nRxNok;                     //!<        Number of packets received with CRC error
   uint16_t nRxIgnored;                 //!<        Number of packets received with CRC OK, but ignored
   uint8_t nRxBufFull;                  //!<        Number of packets received that did not fit in Rx queue
   int8_t lastRssi;                     //!<        The RSSI of the last received packet
   ratmr_t timeStamp;                   //!<        Time stamp of the last received packet
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleScannerOutput
//! @{
//! Output structure for scanner (CMD_BLE_SCANNER)

struct __RFC_STRUCT rfc_bleScannerOutput_s {
   uint16_t nTxScanReq;                 //!<        Number of transmitted SCAN_REQ packets
   uint16_t nBackedOffScanReq;          //!<        Number of SCAN_REQ packets not sent due to backoff procedure
   uint16_t nRxAdvOk;                   //!<        Number of ADV*_IND packets received with CRC OK and not ignored
   uint16_t nRxAdvIgnored;              //!<        Number of ADV*_IND packets received with CRC OK, but ignored
   uint16_t nRxAdvNok;                  //!<        Number of ADV*_IND packets received with CRC error
   uint16_t nRxScanRspOk;               //!<        Number of SCAN_RSP packets received with CRC OK and not ignored
   uint16_t nRxScanRspIgnored;          //!<        Number of SCAN_RSP packets received with CRC OK, but ignored
   uint16_t nRxScanRspNok;              //!<        Number of SCAN_RSP packets received with CRC error
   uint8_t nRxAdvBufFull;               //!<        Number of ADV*_IND packets received that did not fit in Rx queue
   uint8_t nRxScanRspBufFull;           //!<        Number of SCAN_RSP packets received that did not fit in Rx queue
   int8_t lastRssi;                     //!<        The RSSI of the last received packet
   uint8_t __dummy0;
   ratmr_t timeStamp;                   //!<        Time stamp of the last successfully received ADV*_IND packet that was not ignored
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleInitiatorOutput
//! @{
//! Output structure for initiator (CMD_BLE_INITIATOR)

struct __RFC_STRUCT rfc_bleInitiatorOutput_s {
   uint8_t nTxConnectReq;               //!<        Number of transmitted CONNECT_REQ packets
   uint8_t nRxAdvOk;                    //!<        Number of ADV*_IND packets received with CRC OK and not ignored
   uint16_t nRxAdvIgnored;              //!<        Number of ADV*_IND packets received with CRC OK, but ignored
   uint16_t nRxAdvNok;                  //!<        Number of ADV*_IND packets received with CRC error
   uint8_t nRxAdvBufFull;               //!<        Number of ADV*_IND packets received that did not fit in Rx queue
   int8_t lastRssi;                     //!<        The RSSI of the last received packet
   ratmr_t timeStamp;                   //!<        Time stamp of the received ADV*_IND packet that caused transmission of CONNECT_REQ
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleGenericRxOutput
//! @{
//! Output structure for generic Rx (CMD_BLE_GENERIC_RX)

struct __RFC_STRUCT rfc_bleGenericRxOutput_s {
   uint16_t nRxOk;                      //!<        Number of packets received with CRC OK
   uint16_t nRxNok;                     //!<        Number of packets received with CRC error
   uint16_t nRxBufFull;                 //!<        Number of packets that have been received and discarded due to lack of buffer space
   int8_t lastRssi;                     //!<        The RSSI of the last received packet
   uint8_t __dummy0;
   ratmr_t timeStamp;                   //!<        Time stamp of the last received packet
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleTxTestOutput
//! @{
//! Output structure for Tx test (CMD_BLE_TX_TEST)

struct __RFC_STRUCT rfc_bleTxTestOutput_s {
   uint16_t nTx;                        //!<        Number of packets transmitted
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleWhiteListEntry
//! @{
//! White list entry structure

struct __RFC_STRUCT rfc_bleWhiteListEntry_s {
   uint8_t size;                        //!<        Number of while list entries. Used in the first entry of the list only
   struct {
      uint8_t bEnable:1;                //!<        1 if the entry is in use, 0 if the entry is not in use
      uint8_t addrType:1;               //!<        The type address in the entry -- public (0) or random (1)
      uint8_t bWlIgn:1;                 //!< \brief 1 if the entry is to be ignored by a scanner, 0 otherwise. Used to mask out
                                        //!<        entries that have already been scanned and reported.
      uint8_t :1;
      uint8_t bIrkValid:1;              //!< \brief 1 if a valid IRK exists, so that the entry is to be ignored by an initiator,
                                        //!<        0 otherwise
   } conf;
   uint16_t address;                    //!<        Least significant 16 bits of the address contained in the entry
   uint32_t addressHi;                  //!<        Most significant 32 bits of the address contained in the entry
} __RFC_STRUCT_ATTR;

//! @}

//! \addtogroup bleRxStatus
//! @{
//! Receive status byte that may be appended to message in receive buffer

struct __RFC_STRUCT rfc_bleRxStatus_s {
   struct {
      uint8_t channel:6;                //!< \brief The channel on which the packet was received, provided channel is in the range
                                        //!<        0--39; otherwise 0x3F
      uint8_t bIgnore:1;                //!<        1 if the packet is marked as ignored, 0 otherwise
      uint8_t bCrcErr:1;                //!<        1 if the packet was received with CRC error, 0 otherwise
   } status;
} __RFC_STRUCT_ATTR;

//! @}

//! @}
//! @}
#endif
