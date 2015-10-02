/*
 * OtaApp.c - OTA APP Implementation
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
#include "simplelink.h"
#include "OtaCommon.h"
#include "ota_api.h"
#include "OtaApp.h"
#include "OtaClient.h"
#include "CdnClient.h"
#include "LogClient.h"
#include "flc_api.h"

/* internals */
_i32 local_filter(OtaApp_t *pOtaApp, _u8 *file_path);
OtaApp_t g_OtaApp;
OtaApp_statistics_t g_OtaApp_statistics;
FlcCb_t g_FlcSflashCb;

_i32 _WriteStatFile(_u8 *buf, _i32 buf_len);
_i32 _ReadStatFile(_u8 *buf, _i32 buf_len);
_i32 _OtaCleanToIdle(OtaApp_t *pOtaApp);

void *sl_extLib_OtaInit(_i32 runMode, FlcCb_t *pFlcHostCb)
{
    OtaApp_t *pOtaApp = &g_OtaApp;
    FlcCb_t *pFlcSflashCb = &g_FlcSflashCb;

    memset(pOtaApp, 0, sizeof(OtaApp_t));
    pOtaApp->state = OTA_STATE_IDLE;
    pOtaApp->runMode = runMode;
    Report("sl_extLib_OtaInit: OTA lib version = %s\r\n", OTA_LIB_VERSION);

    /* init SFLASH file access callbacks */
    pFlcSflashCb->pOpenFile = sl_extlib_FlcOpenFile;
    pFlcSflashCb->pReadFile = sl_extlib_FlcReadFile;
    pFlcSflashCb->pWriteFile = sl_extlib_FlcWriteFile;
    pFlcSflashCb->pCloseFile = sl_extlib_FlcCloseFile;
    pFlcSflashCb->pAbortFile = sl_extlib_FlcAbortFile;

    pOtaApp->pvOtaClient = OtaClient_Init(pFlcSflashCb, pFlcHostCb);
    pOtaApp->pvCdnClient = CdnClient_Init(pFlcSflashCb, pFlcHostCb);
    pOtaApp->pvLogClient = LogClient_Init();

    /* init statistics */
    pOtaApp->pStatistics = &g_OtaApp_statistics;
    memset(pOtaApp->pStatistics, 0, sizeof(g_OtaApp_statistics));
    return (void *)pOtaApp;
}

_i32 sl_extLib_OtaSet(void *pvOtaApp, _i32 Option, _i32  OptionLen, _u8 *pOptionVal)
{
    OtaApp_t *pOtaApp = (OtaApp_t *)pvOtaApp;
    _i32 status = OTA_STATUS_OK;
    _i32 flcStatus;
    _i32 commitFlag;
    _i32 ImageTestFlags = 0;

    switch (Option)
    {
        case EXTLIB_OTA_SET_OPT_SERVER_INFO:
            pOtaApp->pOtaServerInfo = (OtaOptServerInfo_t *)pOptionVal;
            break;

        case EXTLIB_OTA_SET_OPT_VENDOR_ID:
            strcpy((char *)pOtaApp->vendorStr, (const char *)pOptionVal);
            break;

        case EXTLIB_OTA_SET_OPT_IMAGE_TEST:
            if (pOtaApp->resetMcu )
            {
                ImageTestFlags |= FLC_TEST_RESET_MCU;
            }

            if( pOtaApp->isMCUAppUpdate == 1 )
            {
            	ImageTestFlags |= FLC_TEST_RESET_MCU_WITH_APP;
            }

            if (pOtaApp->resetNwp)
            {
                ImageTestFlags |= FLC_TEST_RESET_NWP;
            }

            /* call File commit module */
            flcStatus = sl_extlib_FlcTest(ImageTestFlags);

            if ( flcStatus & FLC_TEST_RESET_MCU )
            {
                status |= OTA_ACTION_RESET_MCU;
            }
            if (flcStatus & FLC_TEST_RESET_NWP)
            {
                status |= OTA_ACTION_RESET_NWP;
            }

            /* close all connection from last OTA updates and back to IDLE */
            _OtaCleanToIdle(pOtaApp);

            break;

        case EXTLIB_OTA_SET_OPT_IMAGE_COMMIT:
            if (*(_i32 *)pOptionVal == OTA_ACTION_IMAGE_COMMITED)
            {
                commitFlag = FLC_COMMITED;
            }
            else
            {
                commitFlag = FLC_NOT_COMMITED;
            }

            /* call File commit module */
            flcStatus = sl_extlib_FlcCommit(commitFlag);

            if (flcStatus & FLC_TEST_RESET_MCU)
            {
                status |= OTA_ACTION_RESET_MCU;
            }
            if (flcStatus & FLC_TEST_RESET_NWP)
            {
                status |= OTA_ACTION_RESET_NWP;
            }

            if (pOtaApp)
            {
                _OtaCleanToIdle(pOtaApp); /* close all connection from last OTA updates and back to IDLE */
            }
            break;

        default:
            Report("sl_extLib_OtaSet: option %ld is not implemented\r\n", Option);
            break;
    }

    return status;
}

_i32 sl_extLib_OtaGet(void *pvOtaApp, _i32 Option, _i32 *OptionLen, _u8 *pOptionVal)
{
    OtaApp_t *pOtaApp = (OtaApp_t *)pvOtaApp;

#ifdef DEBUG_PRINT
    OtaApp_statistics_t  *pStatistics = pOtaApp->pStatistics;
#endif

    switch (Option)
    {
        case EXTLIB_OTA_GET_OPT_IS_ACTIVE:
            *(_i32 *)pOptionVal = (pOtaApp->state != OTA_STATE_IDLE);
            *OptionLen = sizeof(_i32);
            break;

        case EXTLIB_OTA_GET_OPT_IS_PENDING_COMMIT:
            *(_i32 *)pOptionVal = sl_extlib_FlcIsPendingCommit();
            *OptionLen = sizeof(_i32);
            break;

        case EXTLIB_OTA_GET_OPT_PRINT_STAT:
            Report("OtaApp stat: start counter=%ld, access errors=%ld ---------------\r\n", pStatistics->startCount, pStatistics->continuousAccessErrorCount);
            Report("OtaApp stat: connect server done=%ld, error=%ld, update=%d, metadata=%ld\r\n", pStatistics->ota_connect_done, pStatistics->ota_connect_error, pStatistics->ota_error_updatecheck, pStatistics->ota_error_metadata);
            Report("OtaApp stat: connect cdn    done=%ld, error=%ld\r\n", pStatistics->cdn_connect_done, pStatistics->cdn_connect_error);
            Report("OtaApp stat: download file  done=%ld, error=%ld, save_chunk=%ld, max_eagain=%ld\r\n", pStatistics->download_done, pStatistics->download_error, pStatistics->download_error_save_chunk, pStatistics->download_error_max_eagain);
            break;

        default:
            Report("sl_extLib_OtaGet: option %ld is not implemented\r\n", Option);
            break;
    }
    return OTA_STATUS_OK;
}

_i32 sl_extLib_OtaRun(void *pvOtaApp)
{
    OtaApp_t *pOtaApp = (OtaApp_t *)pvOtaApp;
    OtaClient_t *pOtaClient = (OtaClient_t *)pOtaApp->pvOtaClient;
    OtaApp_statistics_t  *pStatistics = pOtaApp->pStatistics;
    _i32 status;

    /*Report("sl_extLib_OtaRun: entry, state=%d\n", pOtaApp->state); */

    switch (pOtaApp->state)
    {
        case OTA_STATE_IDLE:
            /* serverInfo and vendorStr must be init before running OTA */
            if ((pOtaApp->pOtaServerInfo == NULL) || (pOtaApp->vendorStr[0] == 0))
            {
                return RUN_STAT_ERROR;
            }

            /* Should read in IDLE */
            _ReadStatFile((_u8 *)pOtaApp->pStatistics, sizeof(g_OtaApp_statistics));

            pOtaApp->state = OTA_STATE_CONNECT_SERVER;
            pStatistics->startCount++;
            Report("sl_extLib_OtaRun: call OtaClient_ConnectServer OTA server=%s\r\n", pOtaApp->pOtaServerInfo->server_domain);
            status = OtaClient_ConnectServer(pOtaClient, pOtaApp->pOtaServerInfo);
            if( status < 0)
            {
                _OtaCleanToIdle(pOtaApp);
                Report("sl_extLib_OtaRunERROR: OtaClient_ConnectServer, status=%ld\r\n", status);
                if (status == OTA_STATUS_ERROR_CONTINUOUS_ACCESS)
                {
                    pStatistics->continuousAccessErrorCount++;
                    return RUN_STAT_ERROR_CONTINUOUS_ACCESS_FAILURES;
                }
                pStatistics->ota_connect_error++;
                return RUN_STAT_ERROR_CONNECT_OTA_SERVER;
            }
            pStatistics->ota_connect_done++;
            break;

        case OTA_STATE_CONNECT_SERVER:
            pOtaApp->state = OTA_STATE_RESOURCE_LIST;
            Report("sl_extLib_OtaRun: OtaClient_UpdateCheck, vendorStr=%s\r\n", pOtaApp->vendorStr);
            pOtaApp->numUpdates = OtaClient_UpdateCheck(pOtaClient, pOtaApp->vendorStr);
            if( pOtaApp->numUpdates < 0)
            {
                _OtaCleanToIdle(pOtaApp);
                pStatistics->ota_error_updatecheck++;
                Report("sl_extLib_OtaRun ERROR: OtaClient_UpdateCheck\r\n");
                return RUN_STAT_ERROR_RESOURCE_LIST;
            }
            if( pOtaApp->numUpdates == 0)
            {
                _OtaCleanToIdle(pOtaApp);
                Report("sl_extLib_OtaRunERROR: OtaClient_UpdateCheck - no updates\r\n");
                return RUN_STAT_NO_UPDATES;
            }
            Report("sl_extLib_OtaRun: OtaClient_UpdateCheck, numUpdates=%ld\r\n", pOtaApp->numUpdates);
            break;

        case OTA_STATE_RESOURCE_LIST:

            pOtaApp->file_path = OtaClient_GetNextUpdate(pOtaClient, &pOtaApp->file_size);

            if( pOtaApp->file_path == NULL) /* end of updates */
            {
                OtaClient_CloseServer(pOtaApp->pvOtaClient);
                /* for reuse: close CDN server connection only after last update */
                CdnClient_CloseServer(pOtaApp->pvCdnClient);
                pOtaApp->state = OTA_STATE_WAIT_CONFIRM;
                Report("sl_extLib_OtaRun: -------- end of updates\r\n");

                _WriteStatFile((_u8 *)pOtaApp->pStatistics, sizeof(g_OtaApp_statistics));
                LogClient_ConnectAndPrint(pOtaApp->pvLogClient, pOtaApp->pOtaServerInfo, pOtaApp->vendorStr, pOtaApp->pStatistics);

                status = RUN_STAT_DOWNLOAD_DONE;
                return status;

            }

            pOtaApp->state = OTA_STATE_METADATA;
            Report("sl_extLib_OtaRun: OtaClient_GetNextUpdate: file=%s, size=%ld\r\n", pOtaApp->file_path, pOtaApp->file_size);

            status = local_filter(pOtaApp, pOtaApp->file_path);
            if (status == FALSE)
            {
                Report("sl_extLib_OtaRun: OTA App - local_filter=REJECT\r\n");
                pOtaApp->state = OTA_STATE_RESOURCE_LIST; /* skip to next resource */
                break;
            }

            status = OtaClient_ResourceMetadata(pOtaClient, pOtaApp->file_path, &pOtaApp->pResourceMetadata);
            if( status < 0)
            {
                pStatistics->ota_error_metadata++;
                Report("sl_extLib_OtaRun ERROR: OtaClient_ResourceMetadata\r\n");
                pOtaApp->state = OTA_STATE_RESOURCE_LIST; /* skip to next resource */
                return RUN_STAT_ERROR_METADATA;
            }

            pOtaApp->pResourceMetadata->sflash_file_size = pOtaApp->file_size; /* !!! the only parameter that pass from updateCheck to Metadata - take it from metadata !!!! */
            break;

        case OTA_STATE_METADATA:
            pOtaApp->state = OTA_STATE_CONNECT_CDN;
            Report("sl_extLib_OtaRun: ResourceMetadata CDN file URL = %s\r\n", pOtaApp->pResourceMetadata->cdn_url);
            status = CdnClient_ConnectByUrl(pOtaApp->pvCdnClient, pOtaApp->pResourceMetadata, pOtaApp->pOtaServerInfo->secured_connection);
            if( status < 0)
            {
                pOtaApp->state = OTA_STATE_RESOURCE_LIST;  /*skip this resource, ToDo - set another retry, use same cdnSockId !!! */
                Report("sl_extLib_OtaRun ERROR: Failed on CdnClient_ConnectByUrl\r\n");
                if (status == OTA_STATUS_ERROR_CONTINUOUS_ACCESS)
                {
                    pStatistics->continuousAccessErrorCount++;
                    return RUN_STAT_ERROR_CONTINUOUS_ACCESS_FAILURES;
                }
                pStatistics->cdn_connect_error++;
                return RUN_STAT_ERROR_CONNECT_CDN_SERVER;
            }
            pStatistics->cdn_connect_done++;
            break;

        case OTA_STATE_CONNECT_CDN:
            CdnClient_SetFileInfo(pOtaApp->pvCdnClient, pOtaApp->pResourceMetadata->sflash_file_size, pOtaApp->pResourceMetadata->p_file_name);
            pOtaApp->state = OTA_STATE_DOWNLOADING; /* handle next resource */
            status = CdnClient_Run(pOtaApp->pvCdnClient);
            if( status < 0)
            {
                pOtaApp->state = OTA_STATE_RESOURCE_LIST;
                pStatistics->download_error++;
                Report("sl_extLib_OtaRun ERROR: Failed on CdnClient_Run\r\n");
                return RUN_STAT_ERROR_DOWNLOAD_SAVE;
            }
            break;

        case OTA_STATE_DOWNLOADING:
            /*Report("sl_extLib_OtaRun: call CDN to continue download the file %s\n", pOtaApp->pResourceMetadata->p_file_name); */

            /* set mcu number and reset flag (even in error) */
            if (pOtaApp->pResourceMetadata->flags & METADATA_FLAGS_RESET_NWP)
            {
                pOtaApp->resetNwp = 1;
            }
            /* set reset flag if one of the files is with the reset flag */
            if (pOtaApp->pResourceMetadata->flags & METADATA_FLAGS_RESET_MCU)
            {
                pOtaApp->resetMcu = 1;
            }

            status = CdnClient_Run(pOtaApp->pvCdnClient);
            if( status < 0)
            {
                pOtaApp->state = OTA_STATE_RESOURCE_LIST;

                if (status == CDN_STATUS_ERROR_SAVE_CHUNK)
                    pStatistics->download_error_save_chunk++;
                else if (status == CDN_STATUS_ERROR_MAX_EAGAIN)
                    pStatistics->download_error_max_eagain++;
                else if (status == CDN_STATUS_ERROR_CONNECT_CDN)
                    pStatistics->download_error_connect_cdn++;
                else if (status == CDN_STATUS_ERROR_READ_HDRS)
                    pStatistics->download_error_read_hdrs++;
                else
                    pStatistics->download_error++;

                Report("sl_extLib_OtaRun ERROR: Failed on CdnClient_Run\r\n");
                return RUN_STAT_ERROR_DOWNLOAD_SAVE;
            }
            else if (status == CDN_STATUS_DOWNLOAD_DONE)
            {
                pStatistics->download_done++;
                Report("sl_extLib_OtaRun: ---- Download file completed %s\r\n", pOtaApp->pResourceMetadata->p_file_name);
                if(NULL != strstr((const char *)pOtaApp->file_path,"mcuimgA.bin"))
                {
                  pOtaApp->isMCUAppUpdate = 1;
                }
                pOtaApp->state = OTA_STATE_RESOURCE_LIST; /* handle next resource */
            }

            /* continue with same state */
            break;

        default:
            Report("sl_extLib_OtaRun ERROR: unexpected state %d\r\n", pOtaApp->state);
            return RUN_STAT_ERROR;

    }

    return RUN_STAT_CONTINUE;
}

_i32 local_filter(OtaApp_t *pOtaApp, _u8 *file_path)
{
#if 0
    /* reject files without . extention */
    if (strstr(file_path, ".") == NULL)
    {
        return FALSE;
    }
#endif
    return TRUE;
}

_i32 _OtaCleanToIdle(OtaApp_t *pOtaApp)
{
    Report("_OtaCleanToIdle: close OTA client and CDN client and back to IDLE\r\n");
    OtaClient_CloseServer(pOtaApp->pvOtaClient);
    CdnClient_CloseServer(pOtaApp->pvCdnClient);

    pOtaApp->state = OTA_STATE_IDLE;

    pOtaApp->resetMcu = 0;
    pOtaApp->resetNwp = 0;
    pOtaApp->isMCUAppUpdate = 0;

    return OTA_STATUS_OK;
}


_i32 _WriteStatFile(_u8 *buf, _i32 buf_len)
{
    _i32 lFileHandle;
    _u32 ulToken;
    _i32 status;

    status = sl_FsOpen((_u8 *)OTASTAT_FILENAME, FS_MODE_OPEN_CREATE(buf_len, 0), &ulToken, &lFileHandle);
    if (status < 0)
    {
        Report("_WriteStatFile: ERROR in sl_FsOpen, status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }

    status = sl_FsWrite(lFileHandle, 0, (_u8 *)buf, buf_len);
    if (status < 0)
    {
        sl_FsClose(lFileHandle, 0, 0, 0);
        Report("_WriteStatFile: ERROR in sl_FsRead, status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }

    sl_FsClose(lFileHandle, 0, 0, 0);
    /*Report("--------_WriteStatFile: success\n"); */
    return OTA_STATUS_OK;
}

_i32 _ReadStatFile(_u8 *buf, _i32 buf_len)
{
    _i32 lFileHandle;
    _u32 ulToken;
    _i32 status;

    status = sl_FsOpen((_u8 *)OTASTAT_FILENAME, FS_MODE_OPEN_READ, &ulToken, &lFileHandle);
    if (status < 0)
    {
        Report("_ReadStatFile: ERROR in sl_FsOpen, status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }

    status = sl_FsRead(lFileHandle, 0, (_u8 *)buf, buf_len);
    if (status < 0)
    {
        sl_FsClose(lFileHandle, 0, 0, 0);
        Report("_ReadStatFile: ERROR in sl_FsRead, status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }

    sl_FsClose(lFileHandle, 0, 0, 0);
    /*Report("_ReadStatFile: success\n"); */
    return OTA_STATUS_OK;
}

