/*
 * OtaApp.h - Header OTA APP Implementation
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
#ifndef __OTA_APP_H__
#define __OTA_APP_H__

#ifdef    __cplusplus
extern "C" {
#endif

typedef enum
{
    OTA_STATE_IDLE = 0,
    OTA_STATE_CONNECT_SERVER,
    OTA_STATE_RESOURCE_LIST,
    OTA_STATE_METADATA,
    OTA_STATE_CONNECT_CDN,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_WAIT_CONFIRM,
    OTA_STATE_NUMBER_OF_STATES
} OtaState_e;

typedef struct
{
    _u8 vendorStr[20];
    _i32 startCount;
    _i32 continuousAccessErrorCount;

    _i32 ota_connect_done;
    _i32 ota_connect_error;
    _i32 ota_error_updatecheck;
    _i32 ota_error_metadata;

    _i32 cdn_connect_error;
    _i32 cdn_connect_done;
    _i32 download_error;
    _i32 download_error_connect_cdn;
    _i32 download_error_read_hdrs;
    _i32 download_error_save_chunk;
    _i32 download_error_max_eagain;
    _i32 download_done;
} OtaApp_statistics_t;

typedef struct
{
    OtaState_e state;
    _i32 runMode;
    _u8 vendorStr[20];
    OtaOptServerInfo_t *pOtaServerInfo;
    OtaFileMetadata_t  *pResourceMetadata;
    OtaApp_statistics_t  *pStatistics;
    _i32 resetNwp;
    _i32 resetMcu;
    _u8 isMCUAppUpdate;

    void *pvOtaClient;
    void *pvCdnClient;
    void *pvLogClient;

    /* updateCheck info */
    _i32  numUpdates;
    _u8 *file_path;
    _i32  file_size;
} OtaApp_t;

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __OTA_APP_H__ */
