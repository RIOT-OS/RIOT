//*****************************************************************************
//
// cmdline.c - Functions to help with processing command lines.
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

//*****************************************************************************
//
//! \addtogroup cmdline_api
//! @{
//
//*****************************************************************************

#include <string.h>
#include "utils/cmdline.h"

//*****************************************************************************
//
// Defines the maximum number of arguments that can be parsed.
//
//*****************************************************************************
#ifndef CMDLINE_MAX_ARGS
#define CMDLINE_MAX_ARGS        8
#endif

//*****************************************************************************
//
//! Process a command line string into arguments and execute the command.
//!
//! \param pcCmdLine points to a string that contains a command line that was
//! obtained by an application by some means.
//!
//! This function will take the supplied command line string and break it up
//! into individual arguments.  The first argument is treated as a command and
//! is searched for in the command table.  If the command is found, then the
//! command function is called and all of the command line arguments are passed
//! in the normal argc, argv form.
//!
//! The command table is contained in an array named <tt>g_sCmdTable</tt> which
//! must be provided by the application.
//!
//! \return Returns \b CMDLINE_BAD_CMD if the command is not found,
//! \b CMDLINE_TOO_MANY_ARGS if there are more arguments than can be parsed.
//! Otherwise it returns the code that was returned by the command function.
//
//*****************************************************************************
int
CmdLineProcess(char *pcCmdLine)
{
    static char *argv[CMDLINE_MAX_ARGS + 1];
    char *pcChar;
    int argc;
    int bFindArg = 1;
    tCmdLineEntry *pCmdEntry;

    //
    // Initialize the argument counter, and point to the beginning of the
    // command line string.
    //
    argc = 0;
    pcChar = pcCmdLine;

    //
    // Advance through the command line until a zero character is found.
    //
    while(*pcChar)
    {
        //
        // If there is a space, then replace it with a zero, and set the flag
        // to search for the next argument.
        //
        if(*pcChar == ' ')
        {
            *pcChar = 0;
            bFindArg = 1;
        }

        //
        // Otherwise it is not a space, so it must be a character that is part
        // of an argument.
        //
        else
        {
            //
            // If bFindArg is set, then that means we are looking for the start
            // of the next argument.
            //
            if(bFindArg)
            {
                //
                // As long as the maximum number of arguments has not been
                // reached, then save the pointer to the start of this new arg
                // in the argv array, and increment the count of args, argc.
                //
                if(argc < CMDLINE_MAX_ARGS)
                {
                    argv[argc] = pcChar;
                    argc++;
                    bFindArg = 0;
                }

                //
                // The maximum number of arguments has been reached so return
                // the error.
                //
                else
                {
                    return(CMDLINE_TOO_MANY_ARGS);
                }
            }
        }

        //
        // Advance to the next character in the command line.
        //
        pcChar++;
    }

    //
    // If one or more arguments was found, then process the command.
    //
    if(argc)
    {
        //
        // Start at the beginning of the command table, to look for a matching
        // command.
        //
        pCmdEntry = &g_sCmdTable[0];

        //
        // Search through the command table until a null command string is
        // found, which marks the end of the table.
        //
        while(pCmdEntry->pcCmd)
        {
            //
            // If this command entry command string matches argv[0], then call
            // the function for this command, passing the command line
            // arguments.
            //
            if(!strcmp(argv[0], pCmdEntry->pcCmd))
            {
                return(pCmdEntry->pfnCmd(argc, argv));
            }

            //
            // Not found, so advance to the next entry.
            //
            pCmdEntry++;
        }
    }

    //
    // Fall through to here means that no matching command was found, so return
    // an error.
    //
    return(CMDLINE_BAD_CMD);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
