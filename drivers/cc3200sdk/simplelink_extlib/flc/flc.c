/*
 * flc.c - APIs for reading/writing files to sFlash
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
#include "extlibs_common.h"
#include "flc_api.h"
#include "flc.h"

/* Internal functions */
static _i32 _McuImageGetNewIndex(void);
static _i32 _WriteBootInfo(sBootInfo_t *psBootInfo);
static _i32 _ReadBootInfo(sBootInfo_t *psBootInfo);

/* Save bootinfo on ImageCommit call */
static sBootInfo_t sBootInfo;


_i32 sl_extlib_FlcCommit(_i32 CommitFlag)
{
    _i32 ret_status = 0;

    /*
     * MCU Commit
     */
    _ReadBootInfo(&sBootInfo);

    /* Check only on status TESTING */
    if( IMG_STATUS_TESTING == sBootInfo.ulImgStatus )
    {
        /* commit TRUE: set status IMG_STATUS_NOTEST and switch to new image */
        if (CommitFlag == FLC_COMMITED)
        {
            Report("sl_extlib_FlcCommit: Booted in testing mode.\n\r");
            sBootInfo.ulImgStatus = IMG_STATUS_NOTEST;
            sBootInfo.ucActiveImg = (sBootInfo.ucActiveImg == IMG_ACT_USER1)?
                                    IMG_ACT_USER2:
                                    IMG_ACT_USER1;
            /* prepare switch image condition to the MCU boot loader */
            _WriteBootInfo(&sBootInfo);

            /* no need to reset the device */
        }
        else
        {
            /* commit FALSE: just reset the MCU to enable the bootloader to rollbacke to the old image */
            ret_status |= FLC_TEST_RESET_MCU;
        }
    }

    /*
     * NWP Commit
     */
    /* !!! not implemented yet */

    return ret_status;
}

_i32 sl_extlib_FlcTest(_i32 flags)
{
    _i32 ret_status = 0;

    if (flags & FLC_TEST_RESET_MCU)
    {
    	if(flags & FLC_TEST_RESET_MCU_WITH_APP )
    	{
            /* set status IMG_STATUS_TESTREADY to test the new image */
            Report("sl_extlib_FlcTest: change image status to IMG_STATUS_TESTREADY\n\r");
            _ReadBootInfo(&sBootInfo);
            sBootInfo.ulImgStatus = IMG_STATUS_TESTREADY;
            _WriteBootInfo(&sBootInfo);
    	}

        /* reboot, the MCU boot loader should test the new image */
         ret_status |= FLC_TEST_RESET_MCU;
    }

    if (flags & FLC_TEST_RESET_NWP)
    {
        /* Image test is not implemented yet, just reset the NWP */
        ret_status |= FLC_TEST_RESET_NWP;
    }

    return ret_status;
}

_i32 sl_extlib_FlcIsPendingCommit(void)
{
    _ReadBootInfo(&sBootInfo);
    return (IMG_STATUS_TESTING == sBootInfo.ulImgStatus);
}


_i32 _McuImageGetNewIndex(void)
{
    _i32 newImageIndex;

    /* Assume sBootInfo is alrteady filled in init time (by sl_extlib_FlcCommit) */
    switch(sBootInfo.ucActiveImg)
    {
        case IMG_ACT_USER1:
            newImageIndex = IMG_ACT_USER2;
            break;

        case IMG_ACT_USER2:
        default:
            newImageIndex = IMG_ACT_USER1;
            break;
    }
    Report("_McuImageGetNewIndex: active image is %d, return new image %ld \n\r", sBootInfo.ucActiveImg, newImageIndex);

    return newImageIndex;
}

static _i32 _WriteBootInfo(sBootInfo_t *psBootInfo)
{
    _i32 lFileHandle;
    _u32 ulToken;
    _i32 status = -1;

    if( 0 == sl_FsOpen((_u8 *)IMG_BOOT_INFO, FS_MODE_OPEN_WRITE, &ulToken, &lFileHandle) )
    {
        if( 0 < sl_FsWrite(lFileHandle, 0, (_u8 *)psBootInfo, sizeof(sBootInfo_t)) )
        {
            Report("WriteBootInfo: ucActiveImg=%d, ulImgStatus=0x%x\n\r", psBootInfo->ucActiveImg, psBootInfo->ulImgStatus);
            status = 0;
        }
        sl_FsClose(lFileHandle, 0, 0, 0);
    }

    return status;
}

static _i32 _ReadBootInfo(sBootInfo_t *psBootInfo)
{
    _i32 lFileHandle;
    _u32 ulToken;
    _i32 status = -1;

    if( 0 == sl_FsOpen((_u8 *)IMG_BOOT_INFO, FS_MODE_OPEN_READ, &ulToken, &lFileHandle) )
    {
        if( 0 < sl_FsRead(lFileHandle, 0, (_u8 *)psBootInfo, sizeof(sBootInfo_t)) )
        {
            status = 0;
            Report("ReadBootInfo: ucActiveImg=%d, ulImgStatus=0x%x\n\r", psBootInfo->ucActiveImg, psBootInfo->ulImgStatus);
        }
        sl_FsClose(lFileHandle, 0, 0, 0);
    }

    return status;
}

_i32 sl_extlib_FlcOpenFile(_u8 *file_name, _i32 file_size, _u32 *ulToken, _i32 *lFileHandle, _i32 open_flags)
{
    /* MCU image name should be changed */
    if (strstr((char *)file_name, "/sys/mcuimgA") != NULL)
    {
        file_name[11] = (_u8)_McuImageGetNewIndex() + '1'; /* mcuimg1 is for factory default, mcuimg2,3 are for OTA updates */
        Report("sl_extlib_FlcOpenFile: MCU image name converted to %s \r\n", file_name);
    }

    if (open_flags == _FS_MODE_OPEN_READ)
        return sl_FsOpen((_u8 *)file_name, _FS_MODE_OPEN_READ, ulToken, lFileHandle);
    else
        return sl_FsOpen((_u8 *)file_name, FS_MODE_OPEN_CREATE(file_size, open_flags), ulToken, lFileHandle);
}

_i32 sl_extlib_FlcWriteFile(_i32 fileHandle, _i32 offset, _u8 *buf, _i32 len)
{
    return sl_FsWrite(fileHandle, (_u32)(offset), (_u8 *)buf, len);
}

_i32 sl_extlib_FlcReadFile(_i32 fileHandle, _i32 offset, _u8 *buf, _i32 len)
{
    return sl_FsRead(fileHandle, (_u32)(offset), (_u8 *)buf, len);
}

_i32 sl_extlib_FlcCloseFile(_i32 fileHandle, _u8 *pCeritificateFileName,_u8 *pSignature ,_u32 SignatureLen)
{
    if (SignatureLen == 0)
    {
        pSignature = 0;
    }
    return sl_FsClose(fileHandle, pCeritificateFileName, pSignature, SignatureLen);
}

_i32 sl_extlib_FlcAbortFile(_i32 fileHandle)
{
    _u8 abortSig = 'A';
    /* Close the file with signature 'A' which is ABORT */
    return sl_extlib_FlcCloseFile(fileHandle, 0, &abortSig, 1);
}



