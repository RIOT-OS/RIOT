/*
 * ota_api.h - Header file for OTA library APIs
 *
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
#ifndef __OTA_APP_EXT_H__
#define __OTA_APP_EXT_H__

#ifdef    __cplusplus
extern "C" {
#endif

#define OTA_LIB_VERSION    "1.00"
#define OTA_VENDOR_TEMPLATE_STR "Vid00_Pid00_Ver00"
#define OTASTAT_FILENAME "/sys/otastat.txt"

#define MAX_EAGAIN_RETRIES    10
#define MAX_RESOURCES         16
#define MAX_CONSECUTIVE_NET_ACCESS_ERRORS    10

#define MAX_SERVER_NAME     32
#define MAX_PATH_PREFIX     48
#define MAX_REST_HDRS_SIZE  96
#define MAX_USER_NAME_STR   8
#define MAX_USER_PASS_STR   8
#define MAX_CERT_STR        32
#define MAX_KEY_STR         32

/******************************************************************************
 * Server info Structure
******************************************************************************/
typedef struct
{
    _i32    ip_address; /* 0x0 – use server name */
    _i32    secured_connection;
    _u8   server_domain[MAX_SERVER_NAME];
    _u8   rest_update_chk[MAX_PATH_PREFIX];
    _u8   rest_rsrc_metadata[MAX_PATH_PREFIX];
    _u8   rest_hdr[MAX_REST_HDRS_SIZE];
    _u8   rest_hdr_val[MAX_REST_HDRS_SIZE];
    _u8   user_name[MAX_USER_NAME_STR];
    _u8   user_pass[MAX_USER_PASS_STR];
    _u8   ca_cert_filename[MAX_CERT_STR];
    _u8   client_cert_filename[MAX_CERT_STR];
    _u8   privatekey_filename[MAX_KEY_STR];
    /* logger */
    _u8   log_server_name[MAX_SERVER_NAME];
    _u8   rest_files_put[MAX_REST_HDRS_SIZE];
    _u8   log_mac_address[6];
} OtaOptServerInfo_t;

/******************************************************************************
 * typedefs
******************************************************************************/
typedef _i32 (*OpenFileCB)  (_u8 *file_name, _i32 file_size, _u32 *ulToken, _i32
									*lFileHandle, _i32 open_flags);
typedef _i32 (*WriteFileCB) (_i32 fileHandle, _i32 offset, _u8 *buf, _i32 len);
typedef _i32 (*ReadFileCB)  (_i32 fileHandle, _i32 offset, _u8 *buf, _i32 len);
typedef _i32 (*CloseFileCB) (_i32 fileHandle, _u8 *pCeritificateFileName,
								_u8 *pSignature ,_u32 SignatureLen);
typedef _i32 (*AbortFileCB)(_i32 fileHandle);
typedef _i32 (*CheckConvertFileCB)(_u8* file_name);

/******************************************************************************
 * FLC structure
******************************************************************************/
typedef struct
{
    OpenFileCB   pOpenFile;
    WriteFileCB  pWriteFile;
    ReadFileCB   pReadFile;
    CloseFileCB  pCloseFile;
    AbortFileCB pAbortFile;
} FlcCb_t;

/******************************************************************************
 * File metadata Structure
******************************************************************************/
typedef struct
{
    /* files server name */
    _u8 cdn_url[256];

    /* file flags */
    _i32  flags;

    /* file name */
    _u8 rsrc_file_name[256];
    _u8 *p_file_name;
    _i32  sflash_file_size;

    /* certificate file name */
    _u8 cert1_filename[64];
    _u8 cert2_filename[64];
    _u8 *p_cert_filename;

    /* signiture file name */
    _u8 signature_filename[64];
    _u8 *p_signature;
    _u8 signature[256];
    _i32  signature_len;
} OtaFileMetadata_t;

/******************************************************************************
 * Enumerations
******************************************************************************/
typedef enum
{
    EXTLIB_OTA_SET_OPT_SERVER_INFO = 0,  /* see OtaOptServerInfo_t   */
    EXTLIB_OTA_SET_OPT_VENDOR_ID,
    EXTLIB_OTA_SET_OPT_IMAGE_TEST,
    EXTLIB_OTA_SET_OPT_IMAGE_COMMIT
} OtaSetOpt_e;

typedef enum
{
    EXTLIB_OTA_GET_OPT_IS_ACTIVE,
    EXTLIB_OTA_GET_OPT_IS_PENDING_COMMIT,
    EXTLIB_OTA_GET_OPT_PRINT_STAT
} OtaGetOpt_e;

/******************************************************************************
 * Macros
******************************************************************************/
/* RunMode bitmap */
#define RUN_MODE_OS              (0 << 0)        /* bit 0: 1-NoneOs, 0-Os */
#define RUN_MODE_NONE_OS         (1 << 0)
#define RUN_MODE_BLOCKING        (0 << 1)       /* bit 1: 1-NoneBlocking, 0-Blocking */
#define RUN_MODE_NONE_BLOCKING   (1 << 1)

/* RunStatus */
#define RUN_STAT_DOWNLOAD_DONE                   0x0002
#define RUN_STAT_NO_UPDATES                      0x0001
#define RUN_STAT_OK                     0
#define RUN_STAT_CONTINUE               0
#define RUN_STAT_ERROR                          -1
#define RUN_STAT_ERROR_TIMEOUT                  -2
#define RUN_STAT_ERROR_CONNECT_OTA_SERVER       -3
#define RUN_STAT_ERROR_RESOURCE_LIST            -4
#define RUN_STAT_ERROR_METADATA                 -5
#define RUN_STAT_ERROR_CONNECT_CDN_SERVER       -6
#define RUN_STAT_ERROR_DOWNLOAD_SAVE            -7
#define RUN_STAT_ERROR_CONTINUOUS_ACCESS_FAILURES -1000

/* commit process decisions */
#define OTA_ACTION_RESET_MCU    0x1
#define OTA_ACTION_RESET_NWP    0x2

#define OTA_ACTION_IMAGE_COMMITED      1
#define OTA_ACTION_IMAGE_NOT_COMMITED  0

/*****************************************************************************
 *   API Prototypes
 *****************************************************************************/

/*!

    \addtogroup OTA
    @{

*/

/*!
    \brief Initialize OTA application

    \param[in] RunMode			Is the run mode for the application
    \param[in] pFlcHostCb		Pointer to file commit call back functions

	This function initializes the OTA application and modules. The parameter
	\e RunMode configure the OTA lib to run with OS or noneOS, use blocking or
	noneBlocking calls. It can be logical OR combination of following:

	-\b RUN_MODE_OS \n
	-\b RUN_MODE_NONE_OS \n
	-\b RUN_MODE_BLOCKING \n
	-\b RUN_MODE_NONE_BLOCKING \n

	The parameter \e pFlcHostCb is a pointer to FlcCb_t structure containing
	callback function to download files to storage other than SFLASH.

    \return                OTA control block pointer

    \sa
    \note  Currently OTA supports only NON-OS Blocking mode and pFlcHostCb
	should be set to NULL.
    \warning
    \par                 Example:
    \code
    For example: To initialize OTA from host invoke

    pvOtaApp = sl_extLib_OtaInit(RUN_MODE_NONE_OS | RUN_MODE_BLOCKING, NULL);

    \endcode
*/
void *sl_extLib_OtaInit(_i32 runMode, FlcCb_t *pFlcHostCb);

/*!
    \brief Run the OTA App state machine

    \param[in] pvOtaApp     Pointer to OTA application pointer


    Run one step from the OTA application state machine. Host should repeat
	calling this function and examine the return value in order to check if OTA
	completed or got error or just need to be continued. This pattern is useful
	in host with NON-OS. In host with OS, host should start OTA task and
	continuously calling this function till OTA completed.

    \return Return zero or +ve bitmap number on success, -ve otherwise.

    -\b RUN_STAT_CONTINUE    - Host should continue calling sl_extLib_OtaRun \n
    -\b RUN_STAT_NO_UPDATES  - No updates for now, host can retry \n
    -\b RUN_STAT_DOWNLOAD_DONE - Current OTA update completed,
	host should test bit 2,3 to reset the MCU/NWP \n
    -\b RUN_STAT_ERROR_CONTINUOUS_ACCESS_FAILURES - Fatal access error,
	OTA try more than 10 times to reach the OTA server and failed, it is
	recommended to reset the NWP. \n\n

	Other negative value - OTA error, OTA can recover from this failue, host
	should continue calling sl_extLib_OtaRun.

    \sa
    \note
    \warning
    \par                 Example:
    \code
    For example: Run OTA from host

    pvOtaApp = sl_extLib_OtaRun(pvOtaApp);

    \endcode
*/
_i32 sl_extLib_OtaRun(void *pvOtaApp);

/*!
    \brief Set OTA command/parameter

    \param[in] pvOtaApp     OTA control block pointer
    \param[in] Option       Select the option
    \param[in] OptionLen    Option structure length
    \param[in] pOptionVal   pointer to the option structure

	This function sets OTA command/parameter. The parameter \e Option can be
	one of the following:

	\b EXTLIB_OTA_SET_OPT_SERVER_INFO - Set the Server information \n
	\b EXTLIB_OTA_SET_OPT_VENDOR_ID   - Set the Vendor ID string \n
	\b EXTLIB_OTA_SET_OPT_COMMITED - Commit the last OTA update & move to idle \n

    \return         On success, zero is returned. On error, -1 is returned

    \sa
    \note
    \warning
    \par                 Example:
    \code
    For example: Set OTA server info from host

    sl_extLib_OtaSet(pvOtaApp, EXTLIB_OTA_SET_OPT_SERVER_INFO,sizeof(g_otaOptServerInfo), (_u8 *)&g_otaOptServerInfo);

    \endcode
*/
_i32 sl_extLib_OtaSet(void *pvOtaApp, _i32 Option, _i32  OptionLen, _u8 *pOptionVal);

/*!
    \brief Get the current OTA status


    \param[in] pvOtaApp     OTA control block pointer
    \param[in] Option       Selects the option
    \param[in] OptionLen    option structure length
    \param[in] pOptionVal   pointer to the option structure

	This function gets the current OTA of active or Idle.
	The parameter \e option, could be one of the following:

    \b EXTLIB_OTA_GET_OPT_IS_ACTIVE - Check if OTA process is active or idle \n

	Value at \e pOptionVal will be set to 1 if OTA process is active, 0 if Idle

    \b EXTLIB_OTA_GET_OPT_PRINT_STAT - Prints statistics

    \return         Return 0 on success, -1 otherwise.

    \sa
    \note
    \warning
    \par                 Example:
    \code
    For example: Get OTA running status

    sl_extLib_OtaGet(pvOtaApp, EXTLIB_OTA_GET_OPT_IS_ACTIVE, &ProcActiveLen, (_u8 *)&ProcActive);

    \endcode
*/
_i32 sl_extLib_OtaGet(void *pvOtaApp, _i32 Option, _i32 *OptionLen, _u8 *pOptionVal);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* __OTA_APP_EXT_H__ */
