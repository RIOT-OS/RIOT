/*
 * CdnClient.c - CDN Client Implementation
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
#include "OtaHttp.h"
#include "CdnClient.h"

/* Internals */
CdnClient_t g_CdnClient;

_i32 _OpenStorageFile(CdnClient_t *pCdnClient, _u8 *file_name, _i32 file_size, _u32 *ulToken, _i32 *lFileHandle);
_i32 _ReadFileHeaders(_i16 fileSockId, _u8 *domian_name, _u8 *file_name);
_i32 _RecvFileChunk(_i16 fileSockId, void *pBuf, _i32 Len, _i32 flags);
_i32 _readSignature(CdnClient_t *pCdnClient, _u8 *signature_filename, OtaFileMetadata_t *pMetadata);

void *CdnClient_Init(FlcCb_t *pFlcSflashCb, FlcCb_t *pFlcHostCb)
{
    CdnClient_t *pCdnClient = &g_CdnClient;

    memset(pCdnClient, 0, sizeof(CdnClient_t));
    pCdnClient->fileSockId = -1;
    pCdnClient->port_num = SOCKET_PORT_DEFAULT;
    pCdnClient->pFlcSflashCb = pFlcSflashCb;
    pCdnClient->pFlcHostCb = pFlcHostCb;
    pCdnClient->pFlcCb = pFlcSflashCb; /* default is SFLASH */
    pCdnClient->totalBytesReceived = 0;
    pCdnClient->state = CDN_STATE_IDLE;

    return (void *)pCdnClient;
}


_i32 CdnClient_ConnectByUrl(void *pvCdnClient, OtaFileMetadata_t *pResourceMetadata, _i32 secured_connection)
{
    CdnClient_t *pCdnClient = (CdnClient_t *)pvCdnClient;
    _u8 domain_name[64];
    _u8 req_uri[128];
    _u8 *cdn_url = pResourceMetadata->cdn_url;
    _i32 status;

    /* extract CDN server name and file name */
    status = http_extract_domain_by_url((_u8 *)cdn_url, domain_name, req_uri);
    if (status < 0)
    {
        Report("CdnClient_ConnectByUrl: ERROR, http_extract_domain_by_url, status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }

    /* Skip connect server if same CDN server */
    if (strcmp((const char *)pCdnClient->cdn_server_name, (const char *)domain_name) != 0)
    {
        /* close previous CDN server socket */
        CdnClient_CloseServer(pCdnClient);
        strcpy((char *)pCdnClient->cdn_server_name, (const char *)domain_name);
        /* connect to the CDN server */
        pCdnClient->port_num = SOCKET_PORT_DEFAULT;
        pCdnClient->fileSockId = http_connect_server(pCdnClient->cdn_server_name, 0, pCdnClient->port_num, secured_connection, SOCKET_BLOCKING);
        if (pCdnClient->fileSockId < 0)
        {
            Report("CdnClient_ConnectByUrl: ERROR on http_connect_server, status=%d\r\n", pCdnClient->fileSockId);
            CdnClient_CloseServer(pvCdnClient);
            if (pCdnClient->fileSockId == OTA_STATUS_ERROR_CONTINUOUS_ACCESS)
            {
                return OTA_STATUS_ERROR_CONTINUOUS_ACCESS;
            }
            return CDN_STATUS_ERROR_CONNECT_CDN;
        }
    }

    /* read file headers, skip all not relenant headers */
    status = _ReadFileHeaders(pCdnClient->fileSockId, pCdnClient->cdn_server_name, req_uri); 
    if (status < 0)
    {
        Report("CdnClient_ConnectByUrl: ERROR, _ReadFileHeaders, status=%ld\r\n", status);
        CdnClient_CloseServer(pvCdnClient);
        return CDN_STATUS_ERROR_READ_HDRS;
    }

    pCdnClient->totalBytesReceived = 0;
    pCdnClient->state = CDN_STATE_CDN_SERVER_CONNECTED;

    if (pResourceMetadata->flags & METADATA_FLAGS_NOT_SFLASH_STORAGE)
    {
        /* Host storage */
        pCdnClient->pFlcCb = pCdnClient->pFlcHostCb; /* switch (only for current file) to host storage  */
    }
    else
    {
        /* Sflash storage */
        pCdnClient->pFlcCb = pCdnClient->pFlcSflashCb; /* switch back to default SFLASH storage  */
    }

    /* save metadata */
    pCdnClient->pResourceMetadata = pResourceMetadata;

    return OTA_STATUS_OK;
}

_i32 _ReadFileHeaders(_i16 fileSockId, _u8 *domian_name, _u8 *file_name)
{
    _i32 status=0;
    _i32 len;
    _u8 *send_buf = http_send_buf();

    Report("_ReadFileHeaders: domain=%s, file=%s\r\n", domian_name, file_name);
    http_build_request (send_buf, "GET ", domian_name, NULL, file_name, NULL, NULL);  

    len = sl_Send(fileSockId, send_buf, (_i16)strlen((const char *)send_buf), 0);
    if (len <= 0)
    {
        Report("_ReadFileHeaders: ERROR, sl_Send, status=%ld\r\n", len);
        return OTA_STATUS_ERROR;
    }

    Report("_ReadFileHeaders: skip http headers\r\n");
    status = http_skip_headers(fileSockId);
    if (status < 0)
    {
        Report("_ReadFileHeaders: ERROR, http_skip_headers, status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }

    return OTA_STATUS_OK;
}

_i32 _RecvFileChunk(_i16 fileSockId, void *pBuf, _i32 Len, _i32 flags)
{
    _i32 len;

    len = sl_Recv_eagain(fileSockId, pBuf, Len, flags, MAX_EAGAIN_RETRIES);

    return len;
}

/* default file access is SFLASH, user should overwrite this functions in order to save on Host files */
_i32 _readSignature(CdnClient_t *pCdnClient, _u8 *signature_filename, OtaFileMetadata_t *pMetadata)
{
    FlcCb_t *pFlcCb = pCdnClient->pFlcCb;
    _i32 lFileHandle;
    _i32 status;
    _i32 len;

    status = pFlcCb->pOpenFile((_u8 *)signature_filename , 0, NULL, &lFileHandle, _FS_MODE_OPEN_READ);
    if(status < 0)
    {
        Report("_readSignature: Error pOpenFile\r\n");
        return OTA_STATUS_ERROR;
    }

    len = pFlcCb->pReadFile(lFileHandle , 0, pMetadata->signature, sizeof(pMetadata->signature));
    if(len < 0)
    {
        Report("_readSignature: Error pReadFile\r\n");
        return OTA_STATUS_ERROR;
    }
    pMetadata->p_signature = pMetadata->signature;
    pMetadata->signature_len = len;

    status = pFlcCb->pCloseFile(lFileHandle, NULL, NULL, 0);
    if(status < 0)
    {
        Report("_readSignature: Error pCloseFile\r\n");
        return OTA_STATUS_ERROR;
    }
    return OTA_STATUS_OK;
}

/* default file access is SFLASH, user should overwrite this functions in order to save on Host files */
_i32 _OpenStorageFile(CdnClient_t *pCdnClient, _u8 *file_name, _i32 file_size, _u32 *ulToken, _i32 *lFileHandle)
{
    OtaFileMetadata_t *pMetadata = pCdnClient->pResourceMetadata;
    _i32 fs_open_flags=0;
    _i32 status;

    if((_i32)(pCdnClient->pFlcCb) == 0)
    {
        pCdnClient->state = CDN_STATE_IDLE;
        Report("_OpenStorageFile: pCdnClient->pFlcCb is NULL !!!!\r\n");
        return OTA_STATUS_ERROR;
    }
    /*  create a user file with mirror */
    fs_open_flags  = _FS_FILE_OPEN_FLAG_COMMIT;

    /* set security flags */
    if (pMetadata->flags & METADATA_FLAGS_SECURED)
    {
        fs_open_flags |= _FS_FILE_PUBLIC_WRITE;
        fs_open_flags |= _FS_FILE_OPEN_FLAG_SECURE;
        if ((pMetadata->flags & METADATA_FLAGS_SIGNATURE) == 0)
        {
            fs_open_flags |= _FS_FILE_OPEN_FLAG_NO_SIGNATURE_TEST;
        }
        else
        {
            status = _readSignature(pCdnClient, pMetadata->signature_filename, pMetadata);
            if (status < 0)
            {
                Report( "_OpenStorageFile: error in _readSignature, status=%ld\r\n", status);
                return OTA_STATUS_ERROR;
            }
        }
    }

    status = pCdnClient->pFlcCb->pOpenFile((_u8 *)file_name, file_size, ulToken, lFileHandle, fs_open_flags);
    if(status < 0)
    {
        Report( "_OpenStorageFile: error in pOpenFile, status=%ld\r\n", status);
        return OTA_STATUS_ERROR;
    }
    return OTA_STATUS_OK;
}

void CdnClient_CloseServer(void *pvCdnClient)
{
    CdnClient_t *pCdnClient = (CdnClient_t *)pvCdnClient;

    pCdnClient->cdn_server_name[0] = 0;        
    if (pCdnClient->fileSockId >= 0)
    {
        sl_Close(pCdnClient->fileSockId);
        pCdnClient->fileSockId = -1;
    }
}

_i32 CdnClient_Run(void *pvCdnClient)
{
    CdnClient_t *pCdnClient = (CdnClient_t *)pvCdnClient;
    FlcCb_t *pFlcCb = pCdnClient->pFlcCb;
    _i32 status;
    _i32 bytesReceived;
    _u8 *endBuf;

    /*Report("sl_extLib_CdnRun: entry, state=%d, totalBytesReceived=%d\n", pCdnClient->state, pCdnClient->totalBytesReceived); */

    switch (pCdnClient->state)
    {
        case CDN_STATE_CDN_SERVER_CONNECTED:
            Report( "CdnClient_Run: Create/Open for write file %s\r\n", pCdnClient->p_file_name);
    
            /*  create a user file */
            status = _OpenStorageFile(pCdnClient, pCdnClient->p_file_name, pCdnClient->file_size, &pCdnClient->ulToken, &pCdnClient->lFileHandle);
            if(status < 0)
            {
                pCdnClient->state = CDN_STATE_IDLE;
                Report("CdnClient_Run ERROR: pCdnClient->pOpenFileCB\r\n");
                return CDN_STATUS_ERROR_OPEN_FILE;
            }

            pCdnClient->state = CDN_STATE_FILE_DOWNLOAD_AND_SAVE;
            Report( "CdnClient_Run: file opened\r\n");
            pCdnClient->recv_buf = http_recv_buf();
            memset(pCdnClient->recv_buf, 0, sizeof(HTTP_RECV_BUF_LEN));
            break;
            
        case CDN_STATE_FILE_DOWNLOAD_AND_SAVE:
            
            bytesReceived = _RecvFileChunk(pCdnClient->fileSockId, &pCdnClient->recv_buf[0], HTTP_RECV_BUF_LEN, 0);
            if (0 >= bytesReceived)
            {
                Report("CdnClient_Run: ERROR sl_Recv status=%ld\r\n", bytesReceived);
                /*!!!!Don't close the file do Abort or just reset without close */
                /*!!!!pFlcCb->pCloseFile(pCdnClient->lFileHandle, NULL, NULL, 0); */
                pFlcCb->pAbortFile(pCdnClient->lFileHandle);
                pCdnClient->state = CDN_STATE_IDLE;
                if (bytesReceived == SL_EAGAIN)
                    return CDN_STATUS_ERROR_MAX_EAGAIN;
                return CDN_STATUS_ERROR_RECV_CHUNK;

            }

            /* End of file, ignore "HTTP/1.1" ... */
            endBuf = pCdnClient->recv_buf;
            if ((pCdnClient->totalBytesReceived + bytesReceived) >= pCdnClient->file_size)
            {
                bytesReceived = pCdnClient->file_size - pCdnClient->totalBytesReceived;
                endBuf = pCdnClient->recv_buf+1; /* sign EOF  */
            }
            else
            {
                /* Force align length */
                if (bytesReceived & ALIGNMENT_READ_MASK)
                {
                    status = _RecvFileChunk(pCdnClient->fileSockId, &pCdnClient->recv_buf[bytesReceived], 
                                          (ALIGNMENT_READ_MASK + 1) - (bytesReceived & ALIGNMENT_READ_MASK), 0);
                    if (0 >= status)
                    {
                        Report("CdnClient_Run: ERROR sl_Recv on force align recv status=%ld\r\n", status);
                        /*!!!!Don't close the file do Abort or just reset without close */
                        /*!!!!pFlcCb->pCloseFile(pCdnClient->lFileHandle, NULL, NULL, 0); */
                        pFlcCb->pAbortFile(pCdnClient->lFileHandle);
                        pCdnClient->state = CDN_STATE_IDLE;
                        if (status == SL_EAGAIN)
                            return CDN_STATUS_ERROR_MAX_EAGAIN;
                        return CDN_STATUS_ERROR_RECV_CHUNK;

                    }
                    bytesReceived += status;
                }
            }


            /* Write the packet to the file */
            Report( "CdnClient_Run: Write size %ld ",bytesReceived);
            Report("to file %s ", pCdnClient->p_file_name);
            Report("total %ld.\r\n", pCdnClient->totalBytesReceived );
            status = pFlcCb->pWriteFile((_i32)pCdnClient->lFileHandle, pCdnClient->totalBytesReceived, pCdnClient->recv_buf, bytesReceived); 
            if (status < 0)
            {
                Report("CdnClient_Run: Error - SaveFileChunk - (%ld)\r\n", status);
                /*!!!!Don't close the file do Abort or just reset without close */
                /*!!!!pFlcCb->pCloseFile(pCdnClient->lFileHandle, NULL, NULL, 0); */
                pFlcCb->pAbortFile(pCdnClient->lFileHandle);
                pCdnClient->state = CDN_STATE_IDLE;
                return CDN_STATUS_ERROR_SAVE_CHUNK;
            }

            pCdnClient->totalBytesReceived += bytesReceived;

            /* check EOF */
            if (endBuf > pCdnClient->recv_buf)
            {
                Report("CdnClient_Run: End of file\r\n");
                status = pFlcCb->pCloseFile(pCdnClient->lFileHandle, (_u8 *)pCdnClient->pResourceMetadata->p_cert_filename , (_u8 *)pCdnClient->pResourceMetadata->p_signature, pCdnClient->pResourceMetadata->signature_len);
                if (status < 0)
                {
                    Report( "CdnClient_Run: error on pCloseFile, status=%ld\r\n", status);
                    /*!!!!Don't close the file do Abort or just reset without close */
                    /*!!!!pFlcCb->pCloseFile(pCdnClient->lFileHandle, NULL, NULL, 0); */
                    pFlcCb->pAbortFile(pCdnClient->lFileHandle);
                    pCdnClient->state = CDN_STATE_IDLE;
                    return OTA_STATUS_ERROR;
                }

               
                Report("CdnClient_Run: Downloading File Completed - Size=%ld\r\n", pCdnClient->totalBytesReceived);
                pCdnClient->state = CDN_STATE_IDLE;
                return CDN_STATUS_DOWNLOAD_DONE;

            }

            break;
        
        default:
            Report("CdnClient_Run ERROR: unexpected state %d\r\n", pCdnClient->state);
            pCdnClient->state = CDN_STATE_IDLE;
            return CDN_STATUS_ERROR;

    }

    return CDN_STATUS_CONTINUE;
}

_i32 CdnClient_SetFileInfo(void *pvCdnClient, _i32 file_size, _u8 *file_name)
{
    CdnClient_t *pCdnClient = (CdnClient_t *)pvCdnClient;
    pCdnClient->p_file_name = file_name;
    pCdnClient->file_size = file_size;
    
    return OTA_STATUS_OK;
}
