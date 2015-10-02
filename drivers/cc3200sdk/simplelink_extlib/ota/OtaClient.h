/*
 * OtaClient.h - Header file OTA Client implementation
 *
 * Copyright (C) 2014 Texas Instruments Incorporated
 *
 * All rights reserved. Property of Texas Instruments Incorporated.
 * Restricted rights to use, duplicate or disclose this code are
 * granted through contract.
 * The program may not be used without the written permission of
 * Texas Instruments Incorporated or against the terms and conditions
 * stipulated in the agreement under which this program has been supplied,
 * and under no circumstances can it be used with non-TI connectivity device.
 *
*/
#ifndef __OTA_CLIENT_H__
#define __OTA_CLIENT_H__

#ifdef    __cplusplus
extern "C" {
#endif

typedef struct  
{
    _u8 filename[64];  
    _i32  size;
} RsrcData_t; 

typedef struct 
{
    _i32  status;
    _i32  num_updates;
    _i32  vendor_id;
    RsrcData_t rsrcList[MAX_RESOURCES];
} OtaCheckUpdateResponse_t; 

#define METADATA_FLAGS_SECURED              0x00000001
#define METADATA_FLAGS_SIGNATURE            0x00000002
#define METADATA_FLAGS_CERTIFICATE          0x00000004
#define METADATA_FLAGS_NOT_CONVERT_SYS_DIR  0x00000008
#define METADATA_FLAGS_NOT_SFLASH_STORAGE   0x00000010
#define METADATA_FLAGS_RESEREVED_1          0x00000020
#define METADATA_FLAGS_RESET_NWP            0x00000040
#define METADATA_FLAGS_RESET_MCU            0x00000080

typedef struct 
{
    OtaOptServerInfo_t       *pOtaServerInfo;
    OtaCheckUpdateResponse_t *pOtaCheckUpdateResponse;
    OtaFileMetadata_t        *pOtaMetadataResponse;
    FlcCb_t           *pFlcSflashCb;
    FlcCb_t           *pFlcHostCb;
    _i16  serverSockId;
    _i32  currUpdateIndex;
    _i32  numUpdates;
    _u8 *pVendorStr;
} OtaClient_t; 

void *OtaClient_Init(FlcCb_t *pFlcSflashCb, FlcCb_t *pFlcHostCb);
_i32   OtaClient_ConnectServer(void *pvOtaClient, OtaOptServerInfo_t *pOtaServerInfo);
_i32   OtaClient_UpdateCheck(void *pvOtaClient, _u8 *pVendorStr);
_u8 *OtaClient_GetNextUpdate(void *pvOtaClient, _i32 *size);
_i32   OtaClient_ResourceMetadata(void *pvOtaClient, _u8 *resource_file_name, OtaFileMetadata_t **pResourceMetadata);
void  OtaClient_CloseServer(void *pvOtaClient);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __OTA_CLIENT_H__ */
