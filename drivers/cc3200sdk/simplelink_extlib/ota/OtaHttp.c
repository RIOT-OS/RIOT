/*
 * OtaHttp.c - Http request and response parser for OTA library
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
#include "OtaClient.h"
#include "OtaHttp.h"

static _u8 send_buf[HTTP_SEND_BUF_LEN];
static _u8 recv_buf[HTTP_RECV_BUF_LEN];

_u8 *http_send_buf(void) { return send_buf; }
_u8 *http_recv_buf(void) { return recv_buf; }

_i32 g_ConsecutiveNetAccessNetErrors = 0;

/* HTTP Get stcuture:
    GET <URI> HTTP/1.1
    Host: <Domain>
    Authorization: Bearer <Access Token>
*/
_i32 http_build_request(_u8 *pHttpReq, _u8 *method, _u8 *host_name, _u8 *req_uri_prefix, _u8 *req_uri, _u8 *hdr_name, _u8 *hdr_val)
{
    /* start with method GET/POST/PUT */
    strcpy((char *)pHttpReq, (const char *)method);

    /* fill uri_req_prefix, metadata/media */
    if (req_uri_prefix && strlen((const char *)req_uri_prefix))
    {
        strcat((char *)pHttpReq, (const char *)req_uri_prefix);
    }
    /* fill request URI */
    if (req_uri && strlen((const char *)req_uri))
    {
        strcat((char *)pHttpReq, (const char *)req_uri);
    }

    /* fill domain */
    strcat((char *)pHttpReq, " HTTP/1.1\r\nhost: ");
    strcat((char *)pHttpReq, (const char *)host_name);
    strcat((char *)pHttpReq, "\r\n");

    /* fill access_token */
    if (hdr_name && strlen((const char *)hdr_name))
    {
        strcat((char *)pHttpReq, (const char *)hdr_name);
        strcat((char *)pHttpReq, (const char *)hdr_val);
        strcat((char *)pHttpReq, "\r\n");
    }

    strcat((char *)pHttpReq, "\r\n");

    return OTA_STATUS_OK;
}

/*
    parse url into domain name and uri_name
    https://dl.cloud_storage_provider_1.com/1/view/3ntw3xgkgselev9/Apps/SL_OTA/IlanbSmallFile.txt
*/
_i32 http_extract_domain_by_url(_u8 *url_name, _u8 *domain_name, _u8 *req_uri)
{
    _u8 *pBuf;
    _i32 len;

    if (url_name == NULL)
    {
        Report("http_extract_domain_by_url: Error url name\r\n");
        return OTA_STATUS_ERROR;
    }

    /* extract domanin name, extract only the domain name between // to the first / */
    /* example: https://dl.cloud_storage_provider_1.com/1/view/3ntw3xgkgselev9/Apps/SL_OTA/IlanbSmallFile.txt */
    pBuf = url_name;
    pBuf = (_u8 *)strstr((const char *)pBuf, "//");
    if (pBuf == NULL)
        return OTA_STATUS_ERROR;

    pBuf += 2;
    len = strstr((const char *)pBuf, "/") - (char *)pBuf; /* ends with / */
    if (len <= 0)
        return OTA_STATUS_ERROR;
    strncpy ((char *)domain_name, (const char *)pBuf, len);
    domain_name[len] = 0;
    pBuf += len;

    /* ToDo add port number extraction  */

    /* extract domanin name */
    strcpy((char *)req_uri, (const char *)pBuf);

    return OTA_STATUS_OK;
}

_i32 sl_Recv_eagain(_i16 sockId, void *pBuf, _i32 Len, _i32 flags, _i32 max_eagain)
{
    _i32 len;

    do
    {
        len = sl_Recv(sockId, pBuf, (_u16)Len, (_i16)flags);
        if (len != SL_EAGAIN) /*Try Again */
            break;
    } while(--max_eagain);

    return len;
}

_i32 http_skip_headers(_i16 sockId)
{
    _i32 len;
    _u8 *pHeader_string;
    _u8 header_string[HTTP_HEADER_SIZE+1];
    _i32 retry=0;

    pHeader_string = &header_string[0];

    /* First thing - look for success code 200 */
    len = sl_Recv_eagain(sockId, pHeader_string, HTTP_HEADER_SIZE, 0, MAX_EAGAIN_RETRIES); /*Get beginning of server response http header */
    if (len < 0)
    {
        Report("http_skip_headers: ERROR sl_Recv_eagain, status=%ld\r\n", len);
        return OTA_STATUS_ERROR;
    }

    /* Second chance */
    if (len != HTTP_HEADER_SIZE)
    {
        /* second chance */
        len += sl_Recv_eagain(sockId, &pHeader_string[len], HTTP_HEADER_SIZE-len, 0, MAX_EAGAIN_RETRIES); /*Get beginning of server response http header */
        if (len < 0)
        {
            Report("http_skip_headers: ERROR sl_Recv_eagain, status=%ld\r\n", len);
            return OTA_STATUS_ERROR;
        }
        if (len != HTTP_HEADER_SIZE)
        {
            Report("http_skip_headers: fail on retry #2, len=%ld\r\n", len);
            return OTA_STATUS_ERROR;
        }
    }

    if (strncmp (HTTP_HEADER_OK, (const char *)&pHeader_string[9], 3)) /* search for HTTP/x.x 200 */
    {
        pHeader_string[HTTP_HEADER_SIZE]=0; /* just for printing */
        Report("http_skip_headers: http error code %s\r\n", pHeader_string);
        return OTA_STATUS_ERROR;
    }

    /* look for end of response header*/
    while (1)
    {
        len = sl_Recv_eagain(sockId, &header_string[0], 2, 0, MAX_EAGAIN_RETRIES);
        if (len < 0)
        {
            Report("http_skip_headers: ERROR sl_Recv_eagain, status=%ld\r\n", len);
            return OTA_STATUS_ERROR;
        }
        if (!(strncmp ((const char *)header_string, "\r\n", 2))) /*if an entire carriage return is found, search for another */
        {
            sl_Recv_eagain(sockId, &header_string[0], 2, 0, MAX_EAGAIN_RETRIES);
            if (!(strncmp ((const char *)header_string, "\r\n", 2)))
            {
                return OTA_STATUS_OK;
            }
        }
        else if (!(strncmp ((const char *)header_string, "\n\r", 2))) /*if two carriage returns are found in an offset */
        {
            sl_Recv_eagain(sockId, &header_string[0], 1, 0, MAX_EAGAIN_RETRIES); /*grab another byte to negate the offset */
            if (!(strncmp ((const char *)header_string, "\n", 1)))
            {
                return OTA_STATUS_OK;
            }
        }
        if (retry++ > HTTP_MAX_RETRIES)
        {
            Report("http_skip_headers: search end of headeer mac retries\r\n");
            return OTA_STATUS_ERROR;
        }
    }
}

/*
    example media url
    {
        "url": "https://cloud_storage_provider_1/1/view/3ntw3xgkgselev9/Apps/SL_OTA/IlanbSmallFile.txt",
        "expires": "Wed, 07 May 2014 15:54:25 +0000"
    }
*/
_i32 json_parse_dropbox_media_url(_u8 *media_response_buf, _u8 *media_url)
{
    _u8 *pBuf = media_response_buf;
    _i32 len;

    pBuf = (_u8 *)strstr((const char *)pBuf, "\r\n\r\n");  /* skip HTTP response header */
    if (pBuf == NULL)
        return OTA_STATUS_ERROR;

    pBuf = (_u8 *)strstr((const char *)pBuf, "url");
    if (pBuf == NULL)
        return OTA_STATUS_ERROR;

    pBuf += 7;
    len = strstr((const char *)pBuf, "\", ") - (char *)pBuf; /* ends with ", */
    if (len <= 0)
        return OTA_STATUS_ERROR;

    strncpy ((char *)media_url, (const char *)pBuf, len);
    media_url[len] = 0;

    return OTA_STATUS_OK;
}

/* example for metadata file:
        {
            "revision": 6,
            "rev": "623446f31",
            "thumb_exists": false,
            "bytes": 244048,
            "modified": "Sun, 04 May 2014 08:17:44 +0000",
            "client_mtime": "Sun, 04 May 2014 08:17:44 +0000",
            "path": "/Mac31041_Phy1533.ucf",
            "is_dir": false,
            "icon": "page_white",
            "root": "root_dir",
            "mime_type": "application/octet-stream",
            "size": "238.3 KB"
        },
*/
_i32 json_parse_dropbox_metadata(_i16 sockId, RsrcData_t *pRsrcData, _u8 *read_buf, _i32 size)
{
    RsrcData_t currentFile;
    _u8 *pBuf;
    _u8 *pRecord;
    _i32 len;
    _u8 fileNumber=0;
    _u8 size_str[64];

    pBuf = read_buf;
    pBuf = (_u8 *)strstr((const char *)pBuf, "["); /* start of directory files */

    //
    // No files
    //
    if( *(pBuf+1) == ']' )
      return OTA_STATUS_OK;

    if (pBuf == NULL)
        return OTA_STATUS_ERROR;

    while (1)
    {
        pBuf = (_u8 *)strstr((const char *)pBuf, "{"); /* start of next file */
        if (pBuf == NULL)
            return OTA_STATUS_ERROR;
        /* read more if no end of file record - between { } */
        if ((_u8 *)strstr((const char *)pBuf, "}") <= pBuf) /* file recored end not found */
        {
            _i32 left_len = read_buf+size-pBuf;
            if (left_len < 0)
            {
                /* ToDo: need to check it */
                return OTA_STATUS_ERROR;
            }
            strncpy((char *)read_buf, (const char *)pBuf, left_len); /* copy current file to start of read buffer */
            memset(&read_buf[left_len], 0, HTTP_RECV_BUF_LEN-left_len); /* copy current file to start of read buffer */
            size = sl_Recv_eagain(sockId, &read_buf[left_len], HTTP_RECV_BUF_LEN-left_len, 0, MAX_EAGAIN_RETRIES); /*Get media link */
            size += left_len;
            pBuf = read_buf;
            if ((_u8 *)strstr((const char *)read_buf, "}") <= pBuf) /* still not found, exit */
            {
                return OTA_STATUS_OK;
            }
        }
        /* at this point we have all next file record */

        pRecord = pBuf;

        /* extract file size */
        pBuf = (_u8 *)strstr((const char *)pRecord, "bytes"); /* start of directory files */
        if (pBuf == NULL)
            return OTA_STATUS_ERROR;

        pBuf += 8;
        len = strstr((const char *)pBuf, ",") - (char *)pBuf; /* end of size */
        if (len <= 0)
            return OTA_STATUS_ERROR;

        strncpy ((char *)size_str, (const char *)pBuf, len);
        size_str[len] = 0;
        pBuf += len;

        /* extract file path */
        pBuf = (_u8 *)strstr((const char *)pRecord, "path"); /* start of directory files */
        if (pBuf == NULL)
            return OTA_STATUS_ERROR;

        pBuf += 8;
        len = strstr((const char *)pBuf, "\",") - (char *)pBuf; /* end of size */
        if (len <= 0)
            return OTA_STATUS_ERROR;

        strncpy ((char *)currentFile.filename, (const char *)pBuf, len);
        currentFile.filename[len] = 0;
        pBuf += len;

        pBuf = (_u8 *)(1+strstr((const char *)pBuf, "}")); /* go to end of resource block */

        currentFile.size = atol((char const *)size_str);
        pRsrcData[fileNumber++] = currentFile;
        Report("    metadata file=%s, size=%ld\r\n", currentFile.filename, currentFile.size);

        /* end of file - last file doesn't have a , */
        if (pBuf[0] != ',')
            break;
    }

    /* The receive buffer should have the - "}" pattern, if not */
    /* we are yet to receive the last packet */
    if( strstr((const char *)pBuf, "}") == NULL )
    {
        /* Just receive this packet and discard it */
        sl_Recv_eagain(sockId, &read_buf[0], HTTP_RECV_BUF_LEN, 0,
                       MAX_EAGAIN_RETRIES);
    }

    return fileNumber;
}


/* example for metadata file:
{
    "resource_type_list":[
    {
        "resource_type": "RSRC_TYPE_FILE",
        "num_resources": 2,
        "resource_list":[
            {
                "resource_id":  "/Mac31041_Phy1533.ucf",
                "size":  244048
            },
            {
                "resource_id":  "/IlanbSmallFile.txt",
                "size":  4000
            }
        ],
        }
    ],
}
*/

_i32 json_parse_update_check_resp(_i16 sockId, RsrcData_t *pRsrcData, _u8 *read_buf, _i32 size)
{
    RsrcData_t rsrcData;
    _u8 size_str[64];
    _u8 *pBuf;
    _i32 len;
    _u8 rsrcNum=0;

    pBuf = read_buf;
    pBuf = (_u8 *)strstr((const char *)pBuf, "["); /* start of directory files */
    if (pBuf == NULL)
        return OTA_STATUS_ERROR;

    /* Skip to second list - ToDo parse full 2 bundle lists */
    pBuf = (_u8 *)strstr((const char *)pBuf, "["); /* start of directory files */
    if (pBuf == NULL)
        return OTA_STATUS_ERROR;

    while (1)
    {
        pBuf = (_u8 *)strstr((const char *)pBuf, "{"); /* start of next file */
        if (pBuf == NULL)
            return OTA_STATUS_ERROR;
        /* read more if no end of file record - between { } */
        if ((_u8 *)strstr((const char *)pBuf, "}") <= pBuf) /* file recored end not found */
        {
            _i32 left_len = read_buf+size-pBuf;
            strncpy((char *)read_buf, (const char *)pBuf, left_len); /* copy current file to start of read buffer */
            size = sl_Recv_eagain(sockId, &read_buf[left_len], HTTP_RECV_BUF_LEN-left_len, 0, MAX_EAGAIN_RETRIES); /*Get media link */
            pBuf = read_buf;
            if ((_u8 *)strstr((const char *)pBuf, "}") <= pBuf) /* still not found, exit */
            {
                return OTA_STATUS_OK;
            }
        }
        /* at this po_i32 we have all next file record */

        /* Todo Add check response after each strstr!!!!! */

        /* extract file size */
        pBuf = (_u8 *)strstr((const char *)pBuf, "size"); /* "size": 1000, */
        if (pBuf == NULL)
            return OTA_STATUS_ERROR;

        pBuf += sizeof("size")+3;
        len = strstr((const char *)pBuf, ",") - (char *)pBuf;
        if (len <= 0)
            return OTA_STATUS_ERROR;
        strncpy ((char *)size_str, (const char *)pBuf, len);
        size_str[len] = 0;
        pBuf += len;

        /* extract resource_id */
        pBuf = (_u8 *)strstr((const char *)pBuf, "resource_id");  /* "resource_id": "<filename>", */
        if (pBuf == NULL)
            return OTA_STATUS_ERROR;

        pBuf += sizeof("resource_id")+3;
        len = strstr((const char *)pBuf, "\"") - (char *)pBuf;
        if (len <= 0)
            return OTA_STATUS_ERROR;
        strncpy ((char *)rsrcData.filename, (const char *)pBuf, len);
        rsrcData.filename[len] = 0;
        pBuf += len;


        /* skip to end of list */
        pBuf = (_u8 *)(1+strstr((const char *)pBuf, "}"));

        /* copy resource data to the list */
        rsrcData.size = atol((char const *)size_str);
        pRsrcData[rsrcNum++] = rsrcData;
        Report("resource file=%s, size=%ld\r\n", rsrcData.filename, rsrcData.size);

        /* end of file - last file doesn't have a , */
        if (pBuf[0] != ',')
            break;
    }
    return rsrcNum;
}

/*
    example media url
    {
        "resource_type": "RSRC_TYPE_FILE",
        "name": "/Mac31041_Phy1533",
        "CDN_url": "https://<server_name>/api/1/ota/local_cdn/SL_OTA/Mac31041_Phy1533.ucf",
        "bytes": 244048,
    }
*/
_i32 json_parse_rsrc_metadata_url(_u8 *media_response_buf, _u8 *media_url)
{
    _u8 *pBuf = media_response_buf;
    _i32 len;

    pBuf = (_u8 *)strstr((const char *)pBuf, "\r\n\r\n");  /* skip HTTP response header */
    if (pBuf == NULL)
        return OTA_STATUS_ERROR;

    pBuf = (_u8 *)strstr((const char *)pBuf, "CDN_url");
    if (pBuf == NULL)
        return OTA_STATUS_ERROR;

    pBuf += 7;
    len = strstr((const char *)pBuf, "\", ") - (char *)pBuf; /* ends with ", */
    if (len <= 0)
        return OTA_STATUS_ERROR;
    strncpy ((char *)media_url, (const char *)pBuf, len);
    media_url[len] = 0;

    return OTA_STATUS_OK;
}


/* http_connect_server: create TCP socket to the server */
/* ----------------------------------------------------- */
_i16 http_connect_server(_u8 *ServerName, _i32 IpAddr, _i32 port, _i32 secured, _i32 nonBlocking)
{
    _i32 Status;
    SlSockAddrIn_t  Addr;
    _i32 AddrSize;
    _i16 SockID = 0;
    _u32 ServerIP;
    SlTimeval_t tTimeout;

    if (IpAddr == 0)
    {
        /* get host IP by name */
        Status = sl_NetAppDnsGetHostByName((_i8 *)ServerName, strlen((const char *)ServerName), &ServerIP, SL_AF_INET);
        if(0 > Status )
        {
            Report("http_connect_server: ERROR sl_NetAppDnsGetHostByName, status=%ld\r\n", Status);
            if (g_ConsecutiveNetAccessNetErrors++ >= MAX_CONSECUTIVE_NET_ACCESS_ERRORS)
            {
                g_ConsecutiveNetAccessNetErrors = 0;
                return OTA_STATUS_ERROR_CONTINUOUS_ACCESS;
            }
            return OTA_STATUS_ERROR;
        }
    }
    else
    {
        ServerIP = IpAddr;
    }

    g_ConsecutiveNetAccessNetErrors = 0;

    if (port == SOCKET_PORT_DEFAULT)
    {
        port = secured ? SOCKET_PORT_HTTPS : SOCKET_PORT_HTTP;
    }

    /* create socket */
    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons(((_u16)port));
    Addr.sin_addr.s_addr = sl_Htonl(ServerIP);

    AddrSize = sizeof(SlSockAddrIn_t);

    SockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, (secured? SL_SEC_SOCKET : 0));
    if( SockID < 0 )
    {
        Report("http_connect_server: ERROR Socket Open, status=%d\r\n", SockID);
        return OTA_STATUS_ERROR;
    }

    /* set recieve timeout */
    tTimeout.tv_sec=2;
    tTimeout.tv_usec=0;
    Status = sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_RCVTIMEO, &tTimeout, sizeof(SlTimeval_t));
    if( Status < 0 )
    {
        sl_Close(SockID);
        Report("http_connect_server: ERROR sl_SetSockOpt, status=%d\r\n", SockID);
        return OTA_STATUS_ERROR;
    }

    /* connect to the server */
    Status = sl_Connect(SockID, ( SlSockAddr_t *)&Addr, (_u16)AddrSize);
    if(( Status < 0 ) && (Status != (-453))) /*-453 - connected secure socket without server authentication */
    {
        sl_Close(SockID);
        Report("http_connect_server: ERROR Socket Connect, status=%ld\r\n", Status);
        return OTA_STATUS_ERROR;
    }

    /* handle non blocking */
    if (nonBlocking)
    {
        sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &nonBlocking, sizeof(nonBlocking));
    }

    return SockID;
}

