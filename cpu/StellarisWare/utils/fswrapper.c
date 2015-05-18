//*****************************************************************************
//
// fswrapper.c - File System Processing for lwIP Web Server Apps.
//
// Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 9453 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#include <string.h>
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "httpserver_raw/fs.h"
#include "httpserver_raw/fsdata.h"
#include "fatfs/src/ff.h"
#include "fatfs/src/diskio.h"
#include "utils/fswrapper.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"

//*****************************************************************************
//
//! \addtogroup fswrapper_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Static file system images for use with this module may be created using
// the makefsfile.exe utility.  Both position-independent (built using the -b
// command line option to makefsfile) and position dependent file system images
// may be used.
//
//*****************************************************************************
typedef struct
{
    //
    // The index of the file system containing this file.
    //
    unsigned long ulMountIndex;

    //
    // The FatFs file structure allocated if the target file is in the FAT
    // file system.
    //
    FIL *pFATFile;
}
fs_wrapper_data;

//*****************************************************************************
//
// This marker, "FIMG", is placed at the beginning of a position-independent
// file system image to differentiate it from a position-dependent image.
//
//*****************************************************************************
#define FILE_SYSTEM_MARKER ((const struct fsdata_file *)0x474D4946)

//*****************************************************************************
//
// A marker used to indicate that a passed filename cannot be mapped to any of
// the configured mount points.
//
//*****************************************************************************
#define BAD_MOUNT_INDEX 0xFFFFFFFF

//*****************************************************************************
//
// This macro is used to extract pointers from the file descriptors.  We support
// files systems linked into the image as well as external, position independent
// file system images and this macro allows us to use the same code to extract
// pointers from file descriptors in each case.
//
//*****************************************************************************
#define FS_POINTER(ptTree, ptValue, bPosInd) \
 (char *)((bPosInd) ? ((char *)(ptTree) + (unsigned long)(ptValue)) : \
 (char *)(ptValue))

//*****************************************************************************
//
// The pointer to the mount point table and the number of entries in the
// table.
//
//*****************************************************************************
static fs_mount_data *g_psMountPoints = NULL;
static unsigned long g_ulNumMountPoints = 0;
static unsigned long g_ulDefaultMountIndex = BAD_MOUNT_INDEX;
static tBoolean g_bFatFsEnabled = false;

//*****************************************************************************
//
// Given a filename, this function determine which of the configured mount
// points it resides under.  It returns the index of the mount point in the
// g_psMountPoints array and also a pointer to the first character of the
// filename with the mount point name (directory) stripped from it.
//
//*****************************************************************************
static unsigned long
fs_find_mount_index(const char *pcName, char **ppcFSFilename)
{
    unsigned long ulLoop;
    int iLenDirName;
    int iLenMountName;
    char *pcSlash;

    //
    // First extract the top level directory name which we need to match
    // with the mount point name.  For this to exist, the pcName string
    // must start with a '/' character and must contain at least one more
    // '/'.
    //
    if(pcName[0] == '/')
    {
        //
        // The string starts with a '/'.  Does it contain a second one?
        //
        pcSlash = strchr(pcName + 1, '/');

        //
        // Did we find another forward slash character?
        //
        if(pcSlash)
        {
            //
            // Yes - the mount point name is between the start of the
            // string and the slash we just found.  How long is this string?
            //
            iLenDirName = (int)(pcSlash - (pcName + 1));
        }
        else
        {
            //
            // The mount point name is the whole string.
            //
            iLenDirName = strlen(pcName + 1);
            pcSlash = (char *)pcName + 1 + iLenDirName;
        }

        //
        // Now figure out which, if any, of the mount points this matches.
        //
        for(ulLoop = 0; ulLoop < g_ulNumMountPoints; ulLoop++)
        {
            //
            // Skip the default mount point if found.
            //
            if(!g_psMountPoints[ulLoop].pcNamePrefix)
            {
                continue;
            }

            //
            // How long is the name of this mount point?
            //
            iLenMountName = strlen(g_psMountPoints[ulLoop].pcNamePrefix);

            //
            // Does the mount point name match the directory name extracted
            // from the passed pcName?
            //
            if(iLenMountName == iLenDirName)
            {
                //
                // The lengths match but are the strings the same?
                //
                if(!strncmp(g_psMountPoints[ulLoop].pcNamePrefix,
                            pcName + 1, iLenDirName))
                {
                    //
                    // Yes - we have a match. Set the stripped filename to
                    // the second '/' and return the mount point index.
                    //
                    *ppcFSFilename = pcSlash;
                    return(ulLoop);
                }
            }
        }
    }

    //
    // If we drop out of the loop, we didn't find a specific mount point for
    // this file so just return the filename passed and the default mount
    // point.
    //
    *ppcFSFilename = (char *)pcName;

    return(g_ulDefaultMountIndex);
}

//*****************************************************************************
//
//! Initializes the file system wrapper.
//!
//! \param psMountPoints points to an array of fs_mount_data structures. Each
//! element in the array maps a top level directory name to a particular
//! file system image or to the FAT file system and a logical drive number.
//! \param ulNumMountPoints provides the number of populated elements in the
//! \e psMountPoints array.
//!
//! This function should be called to initialize the file system wrapper and
//! provide it with the information required to access the files in multiple
//! file system images via a single filename space.
//!
//! Each entry in \e psMountPoints describes a top level directory in the
//! unified namespace and indicates to fswrapper where the files for that
//! directory can be found.  Each entry can describe either a file system
//! image in system memory or a logical disk handled via the FatFs file system
//! driver.
//!
//! For example, consider the following 3 entry mount point table:
//!
//! \verbatim
//! {{ "internal", &g_pcFSImage,   0, NULL,         NULL },
//!  { "sdcard",   NULL,           0, SDCardEnable, SDCardDisable },
//!  { NULL,       &g_pcFSDefault, 0, NULL,         NULL}}
//! \endverbatim
//!
//! Requests to open file ``/internal/index.html'' will be handled by attempting
//! to open ``/index.html'' in the internal file system pointed to by
//! \e g_pcFSImage.  Similarly, opening ``/sdcard/images/logo.gif'' will result
//! in a call to the FAT f_open function requesting ``0:/images/logo.gif''.  If
//! a request to open ``index.htm'' is received, this is handled by attempting
//! to open ``index.htm'' in the default internal file system image, \e
//! g_pcFSDefault.
//!
//! \return Returns \b true on success or \b false on failure.
//
//*****************************************************************************
tBoolean
fs_init(fs_mount_data *psMountPoints, unsigned long ulNumMountPoints)
{
    unsigned long ulLoop;
    //
    // Check for non-zero parameters in debug builds.
    //
    ASSERT(psMountPoints);
    ASSERT(ulNumMountPoints);

    //
    // Remember the mount point information we have been given.
    //
    if(psMountPoints && ulNumMountPoints)
    {
        //
        // Remember the information passed.
        //
        g_psMountPoints = psMountPoints;
        g_ulNumMountPoints = ulNumMountPoints;

        //
        // Check to determine if any of the mount points refer to FAT file
        // system drivers.  We also hijack this loop to determine what the
        // default mount point (if any) is.
        //
        g_bFatFsEnabled = false;
        for(ulLoop = 0; ulLoop < g_ulNumMountPoints; ulLoop++)
        {
            //
            // If the pcFSImage field of a mount point structure is NULL, this
            // implies that we are using the FAT file system for that node.
            //
            if(!g_psMountPoints[ulLoop].pcFSImage)
            {
                g_bFatFsEnabled = true;
            }

            //
            // Does this entry describe the default mount point?
            //
            if(g_psMountPoints[ulLoop].pcNamePrefix == NULL)
            {
                g_ulDefaultMountIndex = ulLoop;
            }
        }

        return(true);
    }
    else
    {
        //
        // Return an error due to being passed a bad parameter.
        //
        return(false);
    }
}

//*****************************************************************************
//
//! Provides a periodic tick for the file system.
//!
//! \param ulTickMS is the number of milliseconds which have elapsed since the
//! last time this function was called.
//!
//! Applications making use of the file system wrapper with underlying FatFs
//! drives must call this function at least once every 10 milliseconds to
//! provide a time reference for use by the file system.  It is typically
//! called in the context of the application's SysTick interrupt handler or
//! from the handler of some other timer interrupt.
//!
//! If only binary file system images are in use, this function need not be
//! called.
//!
//! \return None
//
//
//*****************************************************************************
void
fs_tick(unsigned long ulTickMS)
{
    static unsigned long ulTickCounter = 0;

    //
    // Check if the file system has been enabled yet.
    //
    if(!g_bFatFsEnabled)
    {
        return;
    }

    //
    // Increment the tick counter.
    //
    ulTickCounter += ulTickMS;

    //
    // Check to see if the FAT FS tick needs to run.
    //
    if(ulTickCounter >= 10)
    {
        ulTickCounter = 0;
        disk_timerproc();
    }
}

//*****************************************************************************
//
//! Opens a file.
//!
//! \param pcName points to a NULL terminated string containing the path and
//! file name to open.
//!
//! This function opens a file and returns a handle allowing it to be read.
//!
//! \return Returns a valid file handle on success or NULL on failure.
//
//*****************************************************************************
struct fs_file *
fs_open(char *pcName)
{
    const struct fsdata_file *ptTree;
    const struct fsdata_file *ptEnd;
    struct fs_file *ptFile = NULL;
    fs_wrapper_data *pWrapper;
    FRESULT fresult = FR_OK;
    tBoolean bPosInd = false;
    char *pcFSFilename;
    char *pcFilename;
    unsigned long ulLength;

    //
    // Allocate memory for the file system structure.
    //
    ptFile = mem_malloc(sizeof(struct fs_file));
    if(NULL == ptFile)
    {
        return(NULL);
    }

    //
    // Allocate memory for our internal control structure.
    //
    ptFile->pextension = mem_malloc(sizeof(fs_wrapper_data));
    pWrapper = (fs_wrapper_data *)ptFile->pextension;

    if(NULL == pWrapper)
    {
        return(NULL);
    }

    //
    // Find which mount point we need to use to satisfy this file open request.
    //
    pWrapper->ulMountIndex = fs_find_mount_index(pcName, &pcFSFilename);
    if(pWrapper->ulMountIndex == BAD_MOUNT_INDEX)
    {
        //
        // We can't map the mount index so return an error.
        //
        mem_free(pWrapper);
        mem_free(ptFile);
        return(NULL);
    }

    //
    // Enable access to the physical medium if we have been provided with
    // a callback for this.
    //
    if(g_psMountPoints[pWrapper->ulMountIndex].pfnEnable)
    {
        g_psMountPoints[pWrapper->ulMountIndex].pfnEnable(
                                                pWrapper->ulMountIndex);
    }

    //
    // Are we opening a file on an internal file system image?
    //
    if(g_psMountPoints[pWrapper->ulMountIndex].pcFSImage)
    {
        //
        // Initialize the file system tree pointer to the root of the linked
        // list for this mount point's file system image.
        //
        ptTree = (const struct fsdata_file *)
                  g_psMountPoints[pWrapper->ulMountIndex].pcFSImage;

        //
        // Which type of file system are we dealing with?
        //
        if(ptTree->next == FILE_SYSTEM_MARKER)
        {
            //
            // If we found the marker, this is a position independent file
            // system image.  Remember this and fix up the pointer to the
            // first descriptor by skipping over the 4 byte marker and the
            // 4 byte image size entry.  We also keep track of where the file
            // system image ends since this allows us to do a bit more error
            // checking later.
            //
            bPosInd = true;
            ulLength = *(unsigned long *)((unsigned char *)ptTree + 4);
            ptTree = (struct fsdata_file *)((char *)ptTree + 8);
            ptEnd = (struct fsdata_file *)((char *)ptTree + ulLength);
        }

        //
        // Begin processing the linked list, looking for the requested file
        // name.
        //
        while(NULL != ptTree)
        {
            //
            // Compare the requested file "name" to the file name in the
            // current node.
            //
            if(strncmp(pcFSFilename, FS_POINTER(ptTree, ptTree->name, bPosInd),
                       ptTree->len) == 0)
            {
                //
                // Fill in the data pointer and length values from the
                // linked list node.
                //
                ptFile->data = FS_POINTER(ptTree, ptTree->data, bPosInd);
                ptFile->len = ptTree->len;

                //
                // For now, we setup the read index to the end of the file,
                // indicating that all data has been read.  This indicates that
                // all the data is currently available in a contiguous block
                // of memory (which is always the case with an internal file
                // system image).
                //
                ptFile->index = ptTree->len;

                //
                // We are not using a FAT file system file and don't need to
                // remap the filename so set these pointers to NULL.
                //
                pWrapper->pFATFile = NULL;

                //
                // Exit the loop and return the file system pointer.
                //
                break;
            }

            //
            // If we get here, we did not find the file at this node of the
            // linked list.  Get the next element in the list.  We can't just
            // assign ptTree from ptTree->next since this will give us the
            // wrong pointer for a position independent image (where the values
            // in the structure are offsets from the start of the file
            // descriptor, not absolute pointers) but we do know that a 0 in
            // the "next" field does indicate that this is the last file so we
            // can use that info to force the loop to exit at the end.
            //
            if(ptTree->next == 0)
            {
                ptTree = NULL;
            }
            else
            {
                ptTree = (struct fsdata_file *)FS_POINTER(ptTree, ptTree->next,
                                                          bPosInd);

                //
                // If this is a position independent file system image, we can
                // also check that the new node is within the image. If it
                // isn't, the image is corrupted to stop the search.
                //
                if(bPosInd && (ptTree >= ptEnd))
                {
                    ptTree = NULL;
                }
            }
        }

        //
        // If we didn't find the file, ptTee will be NULL.  Make sure we
        // return a NULL pointer if this happens.
        //
        if(NULL == ptTree)
        {
            mem_free(ptFile->pextension);
            mem_free(ptFile);
            ptFile = NULL;
        }
    }
    else
    {
        //
        // This file is on the FAT file system.
        //

        //
        // Allocate memory for the Fat File system handle.
        //
        pWrapper->pFATFile = mem_malloc(sizeof(FIL));
        if(NULL == pWrapper->pFATFile)
        {
            mem_free(ptFile->pextension);
            mem_free(ptFile);
            ptFile = NULL;
        }
        else
        {
            //
            // Reformat the filename to start with the FAT logical drive number.
            //
            pcFilename = mem_malloc(strlen(pcFSFilename) + 16);
            if(!pcFilename)
            {
                //
                // Can't allocate temporary storage for the reformatted
                // filename!
                //
                mem_free(pWrapper->pFATFile);
                mem_free(ptFile->pextension);
                mem_free(ptFile);
                ptFile = NULL;
            }
            else
            {
                usprintf(pcFilename, "%d:%s",
                         g_psMountPoints[pWrapper->ulMountIndex].ulDriveNum,
                         pcFSFilename);
                //
                // Attempt to open the file on the Fat File System.
                //
                fresult = f_open(pWrapper->pFATFile, pcFilename, FA_READ);

                //
                // Free the filename storage
                //
                mem_free(pcFilename);

                //
                // Did we open the file correctly?
                //
                if(FR_OK == fresult)
                {
                    //
                    // Yes - fill in the file structure to indicate that a
                    // FAT file is in use.
                    //
                    ptFile->data = NULL;
                    ptFile->len = NULL;
                    ptFile->index = NULL;
                }
                else
                {
                    //
                    // If we get here, we failed to find the file on the FAT
                    // file system so free up the FAT handle/object.
                    //
                    mem_free(pWrapper->pFATFile);
                    mem_free(pWrapper);
                    mem_free(ptFile);
                    ptFile = NULL;
                }
            }
        }
    }

    //
    // Disable access to the physical medium if we have been provided with
    // a callback for this.
    //
    if(g_psMountPoints[pWrapper->ulMountIndex].pfnDisable)
    {
        g_psMountPoints[pWrapper->ulMountIndex].pfnDisable(
                                                       pWrapper->ulMountIndex);
    }

    return(ptFile);
}

//*****************************************************************************
//
//! Closes a file.
//!
//! \param phFile is the handle of the file that is to be closed.  This will
//! have been returned by an earlier call to fs_open().
//!
//! This function closes the file identified by \e phFile and frees all
//! resources associated with the file handle.
//!
//! \return None.
//
//*****************************************************************************
void
fs_close(struct fs_file *phFile)
{
    fs_wrapper_data *pWrapper;

    pWrapper = (fs_wrapper_data *)phFile->pextension;

    //
    // If a Fat file was opened, free its object.
    //
    if(pWrapper->pFATFile)
    {
        //
        // Close the file.
        //
        f_close(pWrapper->pFATFile);

        //
        // Free the file object.
        //
        mem_free(pWrapper->pFATFile);
    }

    //
    // Free our file wrapper control structure.
    //
    mem_free(phFile->pextension);

    //
    // Free the main file system object.
    //
    mem_free(phFile);
}

//*****************************************************************************
//
//!Reads data from an open file.
//!
//! \param phFile is the handle of the file which is to be read.  This will
//! have been returned by a previous call to fs_open().
//! \param pcBuffer points to the first byte of the buffer into which the
//! data read from the file will be copied.  This buffer must be large enough
//! to hold \e iCount bytes.
//! \param iCount is the maximum number of bytes of data that are to be read
//! from the file.
//!
//! This function reads the next block of data from the given file into a
//! buffer and returns the number of bytes read or -1 if the end of the file
//! has been reached.
//!
//! \return Returns the number of bytes read from the file or -1 if the end of
//! the file has been reached and no more data is available.
//
//*****************************************************************************
int
fs_read(struct fs_file *phFile, char *pcBuffer, int iCount)
{
    int iAvailable, iRetcode;
    fs_wrapper_data *pWrapper;

    pWrapper = (fs_wrapper_data *)phFile->pextension;

    //
    // Call the application's enable function for this physical medium (if
    // an enable function has been provided).
    //
    if(g_psMountPoints[pWrapper->ulMountIndex].pfnEnable)
    {
        g_psMountPoints[pWrapper->ulMountIndex].pfnEnable(
                                                       pWrapper->ulMountIndex);
    }

    //
    // Check to see if a Fat File was opened and process it.
    //
    if(pWrapper->pFATFile)
    {
        unsigned short usBytesRead;
        FRESULT fresult;

        //
        // Read the data.
        //
        fresult = f_read(pWrapper->pFATFile, pcBuffer, iCount, &usBytesRead);
        if((fresult != FR_OK) || (usBytesRead == 0))
        {
            iRetcode = -1;
        }
        else
        {
            iRetcode = (int)usBytesRead;
        }
    }
    else
    {
        //
        // We are reading a file from a file system image. Check to see if more
        // data is available.
        //
        if(phFile->len == phFile->index)
        {
            //
            // There is no remaining data.  Return a -1 for EOF indication.
            //
            return(-1);
        }

        //
        // Determine how much data we can copy.  The minimum of the 'iCount'
        // parameter or the available data in the file system buffer.
        //
        iAvailable = phFile->len - phFile->index;
        if(iAvailable > iCount)
        {
            iAvailable = iCount;
        }

        //
        // Copy the data.
        //
        memcpy(pcBuffer, phFile->data + phFile->index, iAvailable);
        phFile->index += iAvailable;

        //
        // Return the count of data that we copied.
        //
        iRetcode = iAvailable;
    }

    //
    // Call the application's disable function now that we have finished
    // accessing the file.
    //
    if(g_psMountPoints[pWrapper->ulMountIndex].pfnDisable)
    {
        g_psMountPoints[pWrapper->ulMountIndex].pfnDisable(
                                                       pWrapper->ulMountIndex);
    }

    //
    // Return the number of bytes read.
    //
    return(iRetcode);
}

//*****************************************************************************
//
//! Maps a path string containing mount point names to a path suitable for
//! use in calls to the FatFs APIs.
//!
//! \param pcPath points to a string containing a path in the namespace
//! defined by the mount information passed to fs_init().
//! \param pcMapped points to a buffer into which the mapped path string will
//! be written.
//! \param iLen is the size, in bytes, of the buffer pointed to by pcMapped.
//!
//! This function may be used by applications which want to make use of FatFs
//! functions which are not directly mapped by the fswrapper layer.  A path
//! in the namespace defined by the mount points passed to function fs_init()
//! is translated to an equivalent path in the FatFs namespace and this may
//! then be used in a direct call to functions such as f_opendir() or
//! f_getfree().
//!
//! \return Returns \b true on success or \b false if fs_init() has not
//! been called, if the path provided maps to an internal file system image
//! rather than a FatFs logical drive or if the buffer pointed to by
//! \e pcMapped is too small to fit the output string.
//
//*****************************************************************************
tBoolean
fs_map_path(const char *pcPath, char *pcMapped, int iLen)
{
    char *pcFSFilename;
    unsigned long ulMountIndex;
    int iCount;

    //
    // If no mount points have been defined, return an error.
    //
    if(!g_psMountPoints)
    {
        return(false);
    }

    //
    // Find which mount point we need to use to satisfy this file open request.
    //
    ulMountIndex = fs_find_mount_index(pcPath, &pcFSFilename);

    //
    // If we got a bad mount index or the index returned represents a mount
    // point that is not in the FAT file system, return an error.
    //
    if((ulMountIndex == BAD_MOUNT_INDEX) ||
       (g_psMountPoints[ulMountIndex].pcFSImage))
    {
        //
        // We can't map the mount index so return an error.
        //
        return(false);
    }

    //
    // Now we can generate the FatFs namespace path string.
    //
    iCount = usnprintf(pcMapped, iLen, "%d:%s",
                       g_psMountPoints[ulMountIndex].ulDriveNum, pcFSFilename);

    //
    // Tell the user how we got on.  The count returned by usnprintf is the
    // number of characters that should have been written, excluding the
    // terminating NULL so we use this to check for overflow of the output
    // buffer.
    //
    return((iLen >= (iCount + 1)) ? true : false);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
