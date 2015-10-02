/*
 * flc_api.h - Header file for File commit lib APIs
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
#ifndef __FLC_API_H__
#define __FLC_API_H__

#ifdef    __cplusplus
extern "C" {
#endif

#define FLC_LIB_VERSION    "1.00"

#define FLC_COMMITED      1
#define FLC_NOT_COMMITED  0

#define FLC_TEST_RESET_MCU		    	0x1
#define FLC_TEST_RESET_NWP    			0x2
#define FLC_TEST_RESET_MCU_WITH_APP		0x4

/*****************************************************************************

    API Prototypes

 *****************************************************************************/

/*!

    \addtogroup Flc
    @{

*/

/*!
    \brief Commits the newly downloaded images (MCU and NWP)
    
    \param[in] CommitFlag     Commit flag 
					
	This function Commits the newly downloaded images for MCU and NWP. After 
	downloading new images, the system is booted in test mode with newly 
	downloaded images. The image(s) should self test and call this function 
	with appropriate \e CommitFlag to commit or reject the update.

	The parameter \e CommitFlag can be one of the following: \
	
    \b FLC_COMMITED \n
    \b FLC_NOT_COMMITED \n
	
	\return  Bitmap of reset recommendation to the host: \b FLC_TEST_RESET_MCU
	or \b FLC_TEST_RESET_NWP
	
    \sa
    \note
    \warning
    \par                 Example:
    \code
    For example: commit after success NWP start

    status = slAccess_start_and_connect(OPEN_LINK_SSID);
    if( 0 > status )
    {
        status = sl_extlib_FlcCommit(FLC_NOT_COMMITED);
        if (status == FLC_TEST_RESET_MCU)
        {
            Report("main: Rebooting...");
            Platform_Reset();
        }
    }

    status = sl_extlib_FlcCommit(FLC_COMMITED);
    if (status == FLC_TEST_RESET_MCU)
    {
        Report("main: Rebooting...");
        Platform_Reset();
    }

    \endcode
*/
_i32 sl_extlib_FlcCommit(_i32 CommitFlag);

/*!
    \brief Set the system to boot in test mode for new downloaded images

    \param[in] flags     Test flags
	
	This function sets the system to boot in test mode during next boot. The 
	parameter \e flags can be logical OR of one or more of the following:
	\b FLC_TEST_RESET_MCU - There is a new image for testing require a restart
	\b FLC_TEST_RESET_MCU_WITH_APP - There is a new MCU for testing.
    \b FLC_TEST_RESET_NWP - There is a new NWP for testing.
	
	Use FLC_TEST_RESET_MCU when there is a new image/file but no update to  
	MCU application image is required, use FLC_TEST_RESET_MCU_WITH_APP 
	otherwise.

    \return	Bitmap of reset recommendation to the host: \b FLC_TEST_RESET_MCU 
	or \b FLC_TEST_RESET_NWP

    \sa
    \note
    \warning
    \par                 Example:
    \code
    For example: test images after OTA download done

    status = sl_extLib_OtaRun(pvOtaApp);
    if ((status & RUN_STAT_DOWNLOAD_DONE))
    {
        if (status & RUN_STAT_DOWNLOAD_DONE_RESET_MCU) ImageTestFlags |= FLC_TEST_RESET_MCU;
        if (status & RUN_STAT_DOWNLOAD_DONE_RESET_MCU) ImageTestFlags |= FLC_TEST_RESET_NWP;

        status = sl_extlib_FlcTest(ImageTestFlags);
        if (status & FLC_TEST_RESET_MCU)
        {
            Report("proc_ota_run_step: Rebooting...");
            Platform_Reset();
        }
        if (status & FLC_TEST_RESET_NWP)
        {
            Report("\nproc_ota_run_step: reset the NWP to test the new image ---\n");
            status = slAccess_stop_start(OPEN_LINK_SSID);
        }
    }

    \endcode
*/
_i32 sl_extlib_FlcTest(_i32 flags);

/*!
    \brief Check on bootloader flags if the state is IMG_STATUS_TESTING

    \return                   TRUE/FLASE
*/
_i32 sl_extlib_FlcIsPendingCommit(void);

/*!
    \brief Open file for write
	
    \param[in]  file_name	 File name
	\param[in]  file_size	 Max size to be allocated for this file
	\param[in]  ulToken		 Reserved for future use. Use NULL
	\param[out] lFileHandle  File handle
	\param[in]  open_flags   File open flags
	
		
    Opens a file for write. In case of OTA MCU (mcuimgA.bin), this function 
	takes care of renaming it to alternate image (mcuimg2.bin or mcuimg3.bin ) 
	name base on the boot info file.
	
	See SL_HOST sl_FsOpen documentation for more details on flags

    \return  Returns 0 on success, negative error number otherwise.
*/
_i32 sl_extlib_FlcOpenFile(_u8 *file_name, _i32 file_size, _u32 *ulToken, _i32 *lFileHandle, _i32 open_flags);

/*!
    \brief Read specified number of bytes from a file

    \param[in] fileHandle	Handle to file opened for reading
	\param[in] offset		Offset to specific block to be read
	\param[in] buf			Pointer the data to be read
	\param[in] len			Length of the data in bytes
	
	This function reads \e len bytes into \e buf from the file specified by the`
	\e fileHandle.

    \return  Returns the number of bytes read or negative error number.
*/
_i32 sl_extlib_FlcReadFile(_i32 fileHandle, _i32 offset, _u8 *buf, _i32 len);

/*!
    \brief Write specified number of bytes to a file

    \param[in] fileHandle	Handle to file opened for reading
	\param[in] offset		Offset to specific block to be read
	\param[in] buf			Pointer the data to be written
	\param[in] len			Length of the data in bytes

	This function writes \e len bytes from \e buf to the file specified by the`
	\e fileHandle.
	
   \return  Returns the number of bytes written or negative error number.
*/
_i32 sl_extlib_FlcWriteFile(_i32 fileHandle, _i32 offset, _u8 *buf, _i32 len);

/*!
    \brief Close a previously opened file

    \param[in] fileHandle              File Handle to previously opned file.
    \param[in] pCeritificateFileName   Reserved for future use. Use NULL.
    \param[in] pSignature              Reserved for future use. Use NULL.
    \param[in] SignatureLen            Reserved for future use. Use 0.

    \return  Returns 0 on success or negative error number.
*/
_i32 sl_extlib_FlcCloseFile(_i32 fileHandle, _u8 *pCeritificateFileName,_u8 *pSignature ,_u32 SignatureLen);

/*!
    \brief Abort a file update

    \param[in]      fileHandle   Pointer to the file (assigned from sl_FsOpen)
	 
	Abort a file update by closing the file with signature 'A', rolling back to 
	the old file (mirror).
	 
    \return  Returns 0 on success or negative error number.
*/
_i32 sl_extlib_FlcAbortFile(_i32 fileHandle);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /*  __FLC_API_H__ */

