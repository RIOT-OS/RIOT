//*****************************************************************************
//
// ustdlib.c - Simple standard library functions.
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
#include "driverlib/debug.h"
#include "utils/ustdlib.h"

//*****************************************************************************
//
//! \addtogroup ustdlib_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// A mapping from an integer between 0 and 15 to its ASCII character
// equivalent.
//
//*****************************************************************************
static const char * const g_pcHex = "0123456789abcdef";

//*****************************************************************************
//
//! Copies a certain number of characters from one string to another.
//!
//! \param pcDst is a pointer to the destination buffer into which characters
//!   are to be copied.
//! \param pcSrc is a pointer to the string from which characters are to be
//!   copied.
//! \param iNum is the number of characters to copy to the destination buffer.
//!
//! This function copies at most \e iNum characters from the string pointed to
//! by \e pcSrc into the buffer pointed to by \e pcDst.  If the end of \e
//! pcSrc is found before \e iNum characters have been copied, remaining
//! characters in \e pcDst will be padded with zeroes until \e iNum characters
//! have been written.  Note that the destination string will only be NULL
//! terminated if the number of characters to be copied is greater than the
//! length of \e pcSrc.
//!
//! \return Returns \e pcDst.
//
//*****************************************************************************
char *
ustrncpy (char *pcDst, const char *pcSrc, int iNum)
{
    int iCount;

    ASSERT(pcSrc);
    ASSERT(pcDst);

    //
    // Start at the beginning of the source string.
    //
    iCount = 0;

    //
    // Copy the source string until we run out of source characters or
    // destination space.
    //
    while(iNum && pcSrc[iCount])
    {
        pcDst[iCount] = pcSrc[iCount];
        iCount++;
        iNum--;
    }

    //
    // Pad the destination if we are not yet done.
    //
    while(iNum)
    {
        pcDst[iCount++] = (char)0;
        iNum--;
    }

    //
    // Pass the destination pointer back to the caller.
    //
    return(pcDst);
}

//*****************************************************************************
//
//! A simple vsnprintf function supporting \%c, \%d, \%p, \%s, \%u, \%x, and
//! \%X.
//!
//! \param pcBuf points to the buffer where the converted string is stored.
//! \param ulSize is the size of the buffer.
//! \param pcString is the format string.
//! \param vaArgP is the list of optional arguments, which depend on the
//! contents of the format string.
//!
//! This function is very similar to the C library <tt>vsnprintf()</tt>
//! function.  Only the following formatting characters are supported:
//!
//! - \%c to print a character
//! - \%d or \%i to print a decimal value
//! - \%s to print a string
//! - \%u to print an unsigned decimal value
//! - \%x to print a hexadecimal value using lower case letters
//! - \%X to print a hexadecimal value using lower case letters (not upper case
//! letters as would typically be used)
//! - \%p to print a pointer as a hexadecimal value
//! - \%\% to print out a \% character
//!
//! For \%d, \%i, \%p, \%s, \%u, \%x, and \%X, an optional number may reside
//! between the \% and the format character, which specifies the minimum number
//! of characters to use for that value; if preceded by a 0 then the extra
//! characters will be filled with zeros instead of spaces.  For example,
//! ``\%8d'' will use eight characters to print the decimal value with spaces
//! added to reach eight; ``\%08d'' will use eight characters as well but will
//! add zeroes instead of spaces.
//!
//! The type of the arguments after \e pcString must match the requirements of
//! the format string.  For example, if an integer was passed where a string
//! was expected, an error of some kind will most likely occur.
//!
//! The \e ulSize parameter limits the number of characters that will be stored
//! in the buffer pointed to by \e pcBuf to prevent the possibility of a buffer
//! overflow.  The buffer size should be large enough to hold the expected
//! converted output string, including the null termination character.
//!
//! The function will return the number of characters that would be converted
//! as if there were no limit on the buffer size.  Therefore it is possible for
//! the function to return a count that is greater than the specified buffer
//! size.  If this happens, it means that the output was truncated.
//!
//! \return Returns the number of characters that were to be stored, not
//! including the NULL termination character, regardless of space in the
//! buffer.
//
//*****************************************************************************
int
uvsnprintf(char *pcBuf, unsigned long ulSize, const char *pcString,
           va_list vaArgP)
{
    unsigned long ulIdx, ulValue, ulCount, ulBase, ulNeg;
    char *pcStr, cFill;
    int iConvertCount = 0;

    //
    // Check the arguments.
    //
    ASSERT(pcString != 0);
    ASSERT(pcBuf != 0);
    ASSERT(ulSize != 0);

    //
    // Adjust buffer size limit to allow one space for null termination.
    //
    if(ulSize)
    {
        ulSize--;
    }

    //
    // Initialize the count of characters converted.
    //
    iConvertCount = 0;

    //
    // Loop while there are more characters in the format string.
    //
    while(*pcString)
    {
        //
        // Find the first non-% character, or the end of the string.
        //
        for(ulIdx = 0; (pcString[ulIdx] != '%') && (pcString[ulIdx] != '\0');
            ulIdx++)
        {
        }

        //
        // Write this portion of the string to the output buffer.  If there are
        // more characters to write than there is space in the buffer, then
        // only write as much as will fit in the buffer.
        //
        if(ulIdx > ulSize)
        {
            ustrncpy(pcBuf, pcString, ulSize);
            pcBuf += ulSize;
            ulSize = 0;
        }
        else
        {
            ustrncpy(pcBuf, pcString, ulIdx);
            pcBuf += ulIdx;
            ulSize -= ulIdx;
        }

        //
        // Update the conversion count.  This will be the number of characters
        // that should have been written, even if there was not room in the
        // buffer.
        //
        iConvertCount += ulIdx;

        //
        // Skip the portion of the format string that was written.
        //
        pcString += ulIdx;

        //
        // See if the next character is a %.
        //
        if(*pcString == '%')
        {
            //
            // Skip the %.
            //
            pcString++;

            //
            // Set the digit count to zero, and the fill character to space
            // (that is, to the defaults).
            //
            ulCount = 0;
            cFill = ' ';

            //
            // It may be necessary to get back here to process more characters.
            // Goto's aren't pretty, but effective.  I feel extremely dirty for
            // using not one but two of the beasts.
            //
again:

            //
            // Determine how to handle the next character.
            //
            switch(*pcString++)
            {
                //
                // Handle the digit characters.
                //
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                {
                    //
                    // If this is a zero, and it is the first digit, then the
                    // fill character is a zero instead of a space.
                    //
                    if((pcString[-1] == '0') && (ulCount == 0))
                    {
                        cFill = '0';
                    }

                    //
                    // Update the digit count.
                    //
                    ulCount *= 10;
                    ulCount += pcString[-1] - '0';

                    //
                    // Get the next character.
                    //
                    goto again;
                }

                //
                // Handle the %c command.
                //
                case 'c':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Copy the character to the output buffer, if there is
                    // room.  Update the buffer size remaining.
                    //
                    if(ulSize != 0)
                    {
                        *pcBuf++ = (char)ulValue;
                        ulSize--;
                    }

                    //
                    // Update the conversion count.
                    //
                    iConvertCount++;

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %d and %i commands.
                //
                case 'd':
                case 'i':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // If the value is negative, make it positive and indicate
                    // that a minus sign is needed.
                    //
                    if((long)ulValue < 0)
                    {
                        //
                        // Make the value positive.
                        //
                        ulValue = -(long)ulValue;

                        //
                        // Indicate that the value is negative.
                        //
                        ulNeg = 1;
                    }
                    else
                    {
                        //
                        // Indicate that the value is positive so that a
                        // negative sign isn't inserted.
                        //
                        ulNeg = 0;
                    }

                    //
                    // Set the base to 10.
                    //
                    ulBase = 10;

                    //
                    // Convert the value to ASCII.
                    //
                    goto convert;
                }

                //
                // Handle the %s command.
                //
                case 's':
                {
                    //
                    // Get the string pointer from the varargs.
                    //
                    pcStr = va_arg(vaArgP, char *);

                    //
                    // Determine the length of the string.
                    //
                    for(ulIdx = 0; pcStr[ulIdx] != '\0'; ulIdx++)
                    {
                    }

                    //
                    // Update the convert count to include any padding that
                    // should be necessary (regardless of whether we have space
                    // to write it or not).
                    //
                    if(ulCount > ulIdx)
                    {
                        iConvertCount += (ulCount - ulIdx);
                    }

                    //
                    // Copy the string to the output buffer.  Only copy as much
                    // as will fit in the buffer.  Update the output buffer
                    // pointer and the space remaining.
                    //
                    if(ulIdx > ulSize)
                    {
                        ustrncpy(pcBuf, pcStr, ulSize);
                        pcBuf += ulSize;
                        ulSize = 0;
                    }
                    else
                    {
                        ustrncpy(pcBuf, pcStr, ulIdx);
                        pcBuf += ulIdx;
                        ulSize -= ulIdx;

                        //
                        // Write any required padding spaces assuming there is
                        // still space in the buffer.
                        //
                        if(ulCount > ulIdx)
                        {
                            ulCount -= ulIdx;
                            if(ulCount > ulSize)
                            {
                                ulCount = ulSize;
                            }
                            ulSize =- ulCount;

                            while(ulCount--)
                            {
                                *pcBuf++ = ' ';
                            }
                        }
                    }

                    //
                    // Update the conversion count.  This will be the number of
                    // characters that should have been written, even if there
                    // was not room in the buffer.
                    //
                    iConvertCount += ulIdx;

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %u command.
                //
                case 'u':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Set the base to 10.
                    //
                    ulBase = 10;

                    //
                    // Indicate that the value is positive so that a minus sign
                    // isn't inserted.
                    //
                    ulNeg = 0;

                    //
                    // Convert the value to ASCII.
                    //
                    goto convert;
                }

                //
                // Handle the %x and %X commands.  Note that they are treated
                // identically; that is, %X will use lower case letters for a-f
                // instead of the upper case letters is should use.  We also
                // alias %p to %x.
                //
                case 'x':
                case 'X':
                case 'p':
                {
                    //
                    // Get the value from the varargs.
                    //
                    ulValue = va_arg(vaArgP, unsigned long);

                    //
                    // Set the base to 16.
                    //
                    ulBase = 16;

                    //
                    // Indicate that the value is positive so that a minus sign
                    // isn't inserted.
                    //
                    ulNeg = 0;

                    //
                    // Determine the number of digits in the string version of
                    // the value.
                    //
convert:
                    for(ulIdx = 1;
                        (((ulIdx * ulBase) <= ulValue) &&
                         (((ulIdx * ulBase) / ulBase) == ulIdx));
                        ulIdx *= ulBase, ulCount--)
                    {
                    }

                    //
                    // If the value is negative, reduce the count of padding
                    // characters needed.
                    //
                    if(ulNeg)
                    {
                        ulCount--;
                    }

                    //
                    // If the value is negative and the value is padded with
                    // zeros, then place the minus sign before the padding.
                    //
                    if(ulNeg && (ulSize != 0) && (cFill == '0'))
                    {
                        //
                        // Place the minus sign in the output buffer.
                        //
                        *pcBuf++ = '-';
                        ulSize--;

                        //
                        // Update the conversion count.
                        //
                        iConvertCount++;

                        //
                        // The minus sign has been placed, so turn off the
                        // negative flag.
                        //
                        ulNeg = 0;
                    }

                    //
                    // See if there are more characters in the specified field
                    // width than there are in the conversion of this value.
                    //
                    if((ulCount > 1) && (ulCount < 65536))
                    {
                        //
                        // Loop through the required padding characters.
                        //
                        for(ulCount--; ulCount; ulCount--)
                        {
                            //
                            // Copy the character to the output buffer if there
                            // is room.
                            //
                            if(ulSize != 0)
                            {
                                *pcBuf++ = cFill;
                                ulSize--;
                            }

                            //
                            // Update the conversion count.
                            //
                            iConvertCount++;
                        }
                    }

                    //
                    // If the value is negative, then place the minus sign
                    // before the number.
                    //
                    if(ulNeg && (ulSize != 0))
                    {
                        //
                        // Place the minus sign in the output buffer.
                        //
                        *pcBuf++ = '-';
                        ulSize--;

                        //
                        // Update the conversion count.
                        //
                        iConvertCount++;
                    }

                    //
                    // Convert the value into a string.
                    //
                    for(; ulIdx; ulIdx /= ulBase)
                    {
                        //
                        // Copy the character to the output buffer if there is
                        // room.
                        //
                        if(ulSize != 0)
                        {
                            *pcBuf++ = g_pcHex[(ulValue / ulIdx) % ulBase];
                            ulSize--;
                        }

                        //
                        // Update the conversion count.
                        //
                        iConvertCount++;
                    }

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle the %% command.
                //
                case '%':
                {
                    //
                    // Simply write a single %.
                    //
                    if(ulSize != 0)
                    {
                        *pcBuf++ = pcString[-1];
                        ulSize--;
                    }

                    //
                    // Update the conversion count.
                    //
                    iConvertCount++;

                    //
                    // This command has been handled.
                    //
                    break;
                }

                //
                // Handle all other commands.
                //
                default:
                {
                    //
                    // Indicate an error.
                    //
                    if(ulSize >= 5)
                    {
                        ustrncpy(pcBuf, "ERROR", 5);
                        pcBuf += 5;
                        ulSize -= 5;
                    }
                    else
                    {
                        ustrncpy(pcBuf, "ERROR", ulSize);
                        pcBuf += ulSize;
                        ulSize = 0;
                    }

                    //
                    // Update the conversion count.
                    //
                    iConvertCount += 5;

                    //
                    // This command has been handled.
                    //
                    break;
                }
            }
        }
    }

    //
    // Null terminate the string in the buffer.
    //
    *pcBuf = 0;

    //
    // Return the number of characters in the full converted string.
    //
    return(iConvertCount);
}

//*****************************************************************************
//
//! A simple sprintf function supporting \%c, \%d, \%p, \%s, \%u, \%x, and \%X.
//!
//! \param pcBuf is the buffer where the converted string is stored.
//! \param pcString is the format string.
//! \param ... are the optional arguments, which depend on the contents of the
//! format string.
//!
//! This function is very similar to the C library <tt>sprintf()</tt> function.
//! Only the following formatting characters are supported:
//!
//! - \%c to print a character
//! - \%d or \%i to print a decimal value
//! - \%s to print a string
//! - \%u to print an unsigned decimal value
//! - \%x to print a hexadecimal value using lower case letters
//! - \%X to print a hexadecimal value using lower case letters (not upper case
//! letters as would typically be used)
//! - \%p to print a pointer as a hexadecimal value
//! - \%\% to print out a \% character
//!
//! For \%d, \%i, \%p, \%s, \%u, \%x, and \%X, an optional number may reside
//! between the \% and the format character, which specifies the minimum number
//! of characters to use for that value; if preceded by a 0 then the extra
//! characters will be filled with zeros instead of spaces.  For example,
//! ``\%8d'' will use eight characters to print the decimal value with spaces
//! added to reach eight; ``\%08d'' will use eight characters as well but will
//! add zeros instead of spaces.
//!
//! The type of the arguments after \e pcString must match the requirements of
//! the format string.  For example, if an integer was passed where a string
//! was expected, an error of some kind will most likely occur.
//!
//! The caller must ensure that the buffer \e pcBuf is large enough to hold the
//! entire converted string, including the null termination character.
//!
//! \return Returns the count of characters that were written to the output
//! buffer, not including the NULL termination character.
//
//*****************************************************************************
int
usprintf(char *pcBuf, const char *pcString, ...)
{
    va_list vaArgP;
    int iRet;

    //
    // Start the varargs processing.
    //
    va_start(vaArgP, pcString);

    //
    // Call vsnprintf to perform the conversion.  Use a large number for the
    // buffer size.
    //
    iRet = uvsnprintf(pcBuf, 0xffff, pcString, vaArgP);

    //
    // End the varargs processing.
    //
    va_end(vaArgP);

    //
    // Return the conversion count.
    //
    return(iRet);
}

//*****************************************************************************
//
//! A simple snprintf function supporting \%c, \%d, \%p, \%s, \%u, \%x, and
//! \%X.
//!
//! \param pcBuf is the buffer where the converted string is stored.
//! \param ulSize is the size of the buffer.
//! \param pcString is the format string.
//! \param ... are the optional arguments, which depend on the contents of the
//! format string.
//!
//! This function is very similar to the C library <tt>sprintf()</tt> function.
//! Only the following formatting characters are supported:
//!
//! - \%c to print a character
//! - \%d or \%i to print a decimal value
//! - \%s to print a string
//! - \%u to print an unsigned decimal value
//! - \%x to print a hexadecimal value using lower case letters
//! - \%X to print a hexadecimal value using lower case letters (not upper case
//! letters as would typically be used)
//! - \%p to print a pointer as a hexadecimal value
//! - \%\% to print out a \% character
//!
//! For \%d, \%i, \%p, \%s, \%u, \%x, and \%X, an optional number may reside
//! between the \% and the format character, which specifies the minimum number
//! of characters to use for that value; if preceded by a 0 then the extra
//! characters will be filled with zeros instead of spaces.  For example,
//! ``\%8d'' will use eight characters to print the decimal value with spaces
//! added to reach eight; ``\%08d'' will use eight characters as well but will
//! add zeros instead of spaces.
//!
//! The type of the arguments after \e pcString must match the requirements of
//! the format string.  For example, if an integer was passed where a string
//! was expected, an error of some kind will most likely occur.
//!
//! The function will copy at most \e ulSize - 1 characters into the buffer
//! \e pcBuf.  One space is reserved in the buffer for the null termination
//! character.
//!
//! The function will return the number of characters that would be converted
//! as if there were no limit on the buffer size.  Therefore it is possible for
//! the function to return a count that is greater than the specified buffer
//! size.  If this happens, it means that the output was truncated.
//!
//! \return Returns the number of characters that were to be stored, not
//! including the NULL termination character, regardless of space in the
//! buffer.
//
//*****************************************************************************
int
usnprintf(char *pcBuf, unsigned long ulSize, const char *pcString, ...)
{
    int iRet;
    va_list vaArgP;

    //
    // Start the varargs processing.
    //
    va_start(vaArgP, pcString);

    //
    // Call vsnprintf to perform the conversion.
    //
    iRet = uvsnprintf(pcBuf, ulSize, pcString, vaArgP);

    //
    // End the varargs processing.
    //
    va_end(vaArgP);

    //
    // Return the conversion count.
    //
    return(iRet);
}

//*****************************************************************************
//
// This array contains the number of days in a year at the beginning of each
// month of the year, in a non-leap year.
//
//*****************************************************************************
static const short g_psDaysToMonth[12] =
{
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

//*****************************************************************************
//
//! Converts from seconds to calendar date and time.
//!
//! \param ulTime is the number of seconds.
//! \param psTime is a pointer to the time structure that is filled in with the
//! broken down date and time.
//!
//! This function converts a number of seconds since midnight GMT on January 1,
//! 1970 (traditional Unix epoch) into the equivalent month, day, year, hours,
//! minutes, and seconds representation.
//!
//! \return None.
//
//*****************************************************************************
void
ulocaltime(unsigned long ulTime, tTime *psTime)
{
    unsigned long ulTemp, ulMonths;

    //
    // Extract the number of seconds, converting time to the number of minutes.
    //
    ulTemp = ulTime / 60;
    psTime->ucSec = ulTime - (ulTemp * 60);
    ulTime = ulTemp;

    //
    // Extract the number of minutes, converting time to the number of hours.
    //
    ulTemp = ulTime / 60;
    psTime->ucMin = ulTime - (ulTemp * 60);
    ulTime = ulTemp;

    //
    // Extract the number of hours, converting time to the number of days.
    //
    ulTemp = ulTime / 24;
    psTime->ucHour = ulTime - (ulTemp * 24);
    ulTime = ulTemp;

    //
    // Compute the day of the week.
    //
    psTime->ucWday = (ulTime + 4) % 7;

    //
    // Compute the number of leap years that have occurred since 1968, the
    // first leap year before 1970.  For the beginning of a leap year, cut the
    // month loop below at March so that the leap day is classified as February
    // 29 followed by March 1, instead of March 1 followed by another March 1.
    //
    ulTime += 366 + 365;
    ulTemp = ulTime / ((4 * 365) + 1);
    if((ulTime - (ulTemp * ((4 * 365) + 1))) > (31 + 28))
    {
        ulTemp++;
        ulMonths = 12;
    }
    else
    {
        ulMonths = 2;
    }

    //
    // Extract the year.
    //
    psTime->usYear = ((ulTime - ulTemp) / 365) + 1968;
    ulTime -= ((psTime->usYear - 1968) * 365) + ulTemp;

    //
    // Extract the month.
    //
    for(ulTemp = 0; ulTemp < ulMonths; ulTemp++)
    {
        if(g_psDaysToMonth[ulTemp] > ulTime)
        {
            break;
        }
    }
    psTime->ucMon = ulTemp - 1;

    //
    // Extract the day of the month.
    //
    psTime->ucMday = ulTime - g_psDaysToMonth[ulTemp - 1] + 1;
}

//*****************************************************************************
//
//! Compares two time structures and determines if one is greater than,
//! less than, or equal to the other.
//!
//! \param pTime1 is the first time structure to compare.
//! \param pTime2 is the second time structure to compare.
//!
//! This function compares two time structures and returns a signed number
//! to indicate the result of the comparison.  If the time represented by
//! \e pTime1 is greater than the time represented by \e pTime2 then a positive
//! number is returned.  Likewise if \e pTime1 is less than \e pTime2 then a
//! negative number is returned.  If the two times are equal then the function
//! returns 0.
//!
//! \return Returns 0 if the two times are equal, +1 if \e pTime1 is greater
//! than \e pTime2, and -1 if \e pTime1 is less than \e pTime2.
//
//*****************************************************************************
static int
ucmptime(tTime *pTime1, tTime *pTime2)
{
    //
    // Compare each field in descending signficance to determine if
    // greater than, less than, or equal.
    //
    if(pTime1->usYear > pTime2->usYear)
    {
        return(1);
    }
    else if(pTime1->usYear < pTime2->usYear)
    {
        return(-1);
    }
    else if(pTime1->ucMon > pTime2->ucMon)
    {
        return(1);
    }
    else if(pTime1->ucMon < pTime2->ucMon)
    {
        return(-1);
    }
    else if(pTime1->ucMday > pTime2->ucMday)
    {
        return(1);
    }
    else if(pTime1->ucMday < pTime2->ucMday)
    {
        return(-1);
    }
    else if(pTime1->ucHour > pTime2->ucHour)
    {
        return(1);
    }
    else if(pTime1->ucHour < pTime2->ucHour)
    {
        return(-1);
    }
    else if(pTime1->ucMin > pTime2->ucMin)
    {
        return(1);
    }
    else if(pTime1->ucMin < pTime2->ucMin)
    {
        return(-1);
    }
    else if(pTime1->ucSec > pTime2->ucSec)
    {
        return(1);
    }
    else if(pTime1->ucSec < pTime2->ucSec)
    {
        return(-1);
    }
    else
    {
        //
        // Reaching this branch of the conditional means that all of the
        // fields are equal, and thus the two times are equal.
        //
        return(0);
    }
}

//*****************************************************************************
//
//! Converts calendar date and time to seconds.
//!
//! \param psTime is a pointer to the time structure that is filled in with the
//! broken down date and time.
//!
//! This function converts the date and time represented by the \e psTime
//! structure pointer to the number of seconds since midnight GMT on January 1,
//! 1970 (traditional Unix epoch).
//!
//! \return Returns the calendar time and date as seconds.  If the conversion
//! was not possible then the function returns (unsigned long)(-1).
//
//*****************************************************************************
unsigned long
umktime(tTime *psTime)
{
    tTime sTimeGuess;
    unsigned long ulTimeGuess = 0x80000000;
    unsigned long ulAdjust = 0x40000000;
    int iSign;

    //
    // Seed the binary search with the first guess.
    //
    ulocaltime(ulTimeGuess, &sTimeGuess);
    iSign = ucmptime(psTime, &sTimeGuess);

    //
    // While the time is not yet found, execute a binary search.
    //
    while(iSign && ulAdjust)
    {
        //
        // Adjust the time guess up or down depending on the result of the
        // last compare.
        //
        ulTimeGuess = (iSign > 0) ? (ulTimeGuess + ulAdjust) :
                                    (ulTimeGuess - ulAdjust);
        ulAdjust /= 2;

        //
        // Compare the new time guess against the time pointed at by the
        // function parameters.
        //
        ulocaltime(ulTimeGuess, &sTimeGuess);
        iSign = ucmptime(psTime, &sTimeGuess);
    };

    //
    // If the above loop was exited with iSign == 0, that means that the
    // time in seconds was found, so return that value to the caller.
    //
    if(iSign == 0)
    {
        return(ulTimeGuess);
    }

    //
    // Otherwise the time could not be converted so return an error.
    //
    else
    {
        return((unsigned long)-1);
    }
}

//*****************************************************************************
//
//! Converts a string into its numeric equivalent.
//!
//! \param pcStr is a pointer to the string containing the integer.
//! \param ppcStrRet is a pointer that will be set to the first character past
//! the integer in the string.
//! \param iBase is the radix to use for the conversion; can be zero to
//! auto-select the radix or between 2 and 16 to explicitly specify the radix.
//!
//! This function is very similar to the C library <tt>strtoul()</tt> function.
//! It scans a string for the first token (that is, non-white space) and
//! converts the value at that location in the string into an integer value.
//!
//! \return Returns the result of the conversion.
//
//*****************************************************************************
unsigned long
ustrtoul(const char *pcStr, const char **ppcStrRet, int iBase)
{
    unsigned long ulRet, ulDigit, ulNeg, ulValid;
    const char *pcPtr;

    //
    // Check the arguments.
    //
    ASSERT(pcStr);
    ASSERT((iBase == 0) || ((iBase > 1) && (iBase <= 16)));

    //
    // Initially, the result is zero.
    //
    ulRet = 0;
    ulNeg = 0;
    ulValid = 0;

    //
    // Skip past any leading white space.
    //
    pcPtr = pcStr;
    while((*pcPtr == ' ') || (*pcPtr == '\t'))
    {
        pcPtr++;
    }

    //
    // Take a leading + or - from the value.
    //
    if(*pcPtr == '-')
    {
        ulNeg = 1;
        pcPtr++;
    }
    else if(*pcPtr == '+')
    {
        pcPtr++;
    }

    //
    // See if the radix was not specified, or is 16, and the value starts with
    // "0x" or "0X" (to indicate a hex value).
    //
    if(((iBase == 0) || (iBase == 16)) && (*pcPtr == '0') &&
       ((pcPtr[1] == 'x') || (pcPtr[1] == 'X')))
    {
        //
        // Skip the leading "0x".
        //
        pcPtr += 2;

        //
        // Set the radix to 16.
        //
        iBase = 16;
    }

    //
    // See if the radix was not specified.
    //
    if(iBase == 0)
    {
        //
        // See if the value starts with "0".
        //
        if(*pcPtr == '0')
        {
            //
            // Values that start with "0" are assumed to be radix 8.
            //
            iBase = 8;
        }
        else
        {
            //
            // Otherwise, the values are assumed to be radix 10.
            //
            iBase = 10;
        }
    }

    //
    // Loop while there are more valid digits to consume.
    //
    while(1)
    {
        //
        // See if this character is a number.
        //
        if((*pcPtr >= '0') && (*pcPtr <= '9'))
        {
            //
            // Convert the character to its integer equivalent.
            //
            ulDigit = *pcPtr++ - '0';
        }

        //
        // Otherwise, see if this character is an upper case letter.
        //
        else if((*pcPtr >= 'A') && (*pcPtr <= 'Z'))
        {
            //
            // Convert the character to its integer equivalent.
            //
            ulDigit = *pcPtr++ - 'A' + 10;
        }

        //
        // Otherwise, see if this character is a lower case letter.
        //
        else if((*pcPtr >= 'a') && (*pcPtr <= 'z'))
        {
            //
            // Convert the character to its integer equivalent.
            //
            ulDigit = *pcPtr++ - 'a' + 10;
        }

        //
        // Otherwise, this is not a valid character.
        //
        else
        {
            //
            // Stop converting this value.
            //
            break;
        }

        //
        // See if this digit is valid for the chosen radix.
        //
        if(ulDigit >= iBase)
        {
            //
            // Since this was not a valid digit, move the pointer back to the
            // character that therefore should not have been consumed.
            //
            pcPtr--;

            //
            // Stop converting this value.
            //
            break;
        }

        //
        // Add this digit to the converted value.
        //
        ulRet *= iBase;
        ulRet += ulDigit;

        //
        // Since a digit has been added, this is now a valid result.
        //
        ulValid = 1;
    }

    //
    // Set the return string pointer to the first character not consumed.
    //
    if(ppcStrRet)
    {
        *ppcStrRet = ulValid ? pcPtr : pcStr;
    }

    //
    // Return the converted value.
    //
    return(ulNeg ? (0 - ulRet) : ulRet);
}

//*****************************************************************************
//
//! Retruns the length of a null-terminated string.
//!
//! \param pcStr is a pointer to the string whose length is to be found.
//!
//! This function is very similar to the C library <tt>strlen()</tt> function.
//! It determines the length of the null-terminated string passed and returns
//! this to the caller.
//!
//! This implementation assumes that single byte character strings are passed
//! and will return incorrect values if passed some UTF-8 strings.
//!
//! \return Returns the length of the string pointed to by \e pcStr.
//
//*****************************************************************************
int
ustrlen(const char * pcStr)
{
    int iLen;

    ASSERT(pcStr);

    //
    // Initialize the length.
    //
    iLen = 0;

    //
    // Step throug the string looking for a zero character (marking its end).
    //
    while(pcStr[iLen])
    {
        //
        // Zero not found so move on to the next character.
        //
        iLen++;
    }

    return(iLen);
}

//*****************************************************************************
//
//! Finds a substring within a string.
//!
//! \param pcHaystack is a pointer to the string that will be searched.
//! \param pcNeedle is a pointer to the substring that is to be found within
//! \e pcHaystack.
//!
//! This function is very similar to the C library <tt>strstr()</tt> function.
//! It scans a string for the first instance of a given substring and returns
//! a pointer to that substring.  If the substring cannot be found, a NULL
//! pointer is returned.
//!
//! \return Returns a pointer to the first occurrence of \e pcNeedle within
//! \e pcHaystack or NULL if no match is found.
//
//*****************************************************************************
char *
ustrstr(const char *pcHaystack, const char *pcNeedle)
{
    unsigned long ulLength;

    //
    // Get the length of the string to be found.
    //
    ulLength = ustrlen(pcNeedle);

    //
    // Loop while we have not reached the end of the string.
    //
    while(*pcHaystack)
    {
        //
        // Check to see if the substring appears at this position.
        //
        if(ustrncmp(pcNeedle, pcHaystack, ulLength) == 0)
        {
            //
            // It does so return the pointer.
            //
            return((char *)pcHaystack);
        }

        //
        // Move to the next position in the string being searched.
        //
        pcHaystack++;
    }

    //
    // We reached the end of the string without finding the substring so
    // return NULL.
    //
    return((char *)0);
}

//*****************************************************************************
//
//! Compares two strings without regard to case.
//!
//! \param pcStr1 points to the first string to be compared.
//! \param pcStr2 points to the second string to be compared.
//! \param iCount is the maximum number of characters to compare.
//!
//! This function is very similar to the C library <tt>strnicmp()</tt> function.
//! It compares at most \e iCount characters of two strings without regard to
//! case.  The comparison ends if a terminating NULL character is found in
//! either string before \e iCount characters are compared.  In this case, the
//! shorter string is deemed the lesser.
//!
//! \return Returns 0 if the two strings are equal, -1 if \e pcStr1 is less
//! than \e pcStr2 and 1 if \e pcStr1 is greater than \e pcStr2.
//
//*****************************************************************************
int
ustrnicmp(const char *pcStr1, const char *pcStr2, int iCount)
{
    char cL1, cL2;

    while(iCount)
    {
        //
        // If we reached a NULL in both strings, they must be equal so
        // we end the comparison and return 0
        //
        if(!*pcStr1 && !*pcStr2)
        {
            return(0);
        }

        //
        // Lower case the characters at the current position before we compare.
        //
        cL1 = (((*pcStr1 >= 'A') && (*pcStr1 <= 'Z')) ?
                            (*pcStr1 + ('a' - 'A')) : *pcStr1);
        cL2 = (((*pcStr2 >= 'A') && (*pcStr2 <= 'Z')) ?
                            (*pcStr2  + ('a' - 'A')) : *pcStr2);
        //
        // Compare the two characters and, if different, return the relevant
        // return code.
        //
        if(cL2 < cL1)
        {
            return(1);
        }
        if(cL1 < cL2)
        {
            return(-1);
        }

        //
        // Move on to the next character.
        //
        pcStr1++;
        pcStr2++;
        iCount--;
    }

    //
    // If we fall out, the strings must be equal for at least the first iCount
    // characters so return 0 to indicate this.
    //
    return(0);
}

//*****************************************************************************
//
//! Compares two strings without regard to case.
//!
//! \param pcStr1 points to the first string to be compared.
//! \param pcStr2 points to the second string to be compared.
//!
//! This function is very similar to the C library <tt>strcasecmp()</tt>
//! function.  It compares two strings without regard to case.  The comparison
//! ends if a terminating NULL character is found in either string.  In this
//! case, the shorter string is deemed the lesser.
//!
//! \return Returns 0 if the two strings are equal, -1 if \e pcStr1 is less
//! than \e pcStr2 and 1 if \e pcStr1 is greater than \e pcStr2.
//
//*****************************************************************************
int
ustrcasecmp(const char *pcStr1, const char *pcStr2)
{
    //
    // Just let ustrnicmp() handle this.
    //
    return(ustrnicmp(pcStr1, pcStr2, -1));
}

//*****************************************************************************
//
//! Compares two strings.
//!
//! \param pcStr1 points to the first string to be compared.
//! \param pcStr2 points to the second string to be compared.
//! \param iCount is the maximum number of characters to compare.
//!
//! This function is very similar to the C library <tt>strncmp()</tt> function.
//! It compares at most \e iCount characters of two strings taking case into
//! account.  The comparison ends if a terminating NULL character is found in
//! either string before \e iCount characters are compared.  In this case, the
//! shorter string is deemed the lesser.
//!
//! \return Returns 0 if the two strings are equal, -1 if \e pcStr1 is less
//! than \e pcStr2 and 1 if \e pcStr1 is greater than \e pcStr2.
//
//*****************************************************************************
int
ustrncmp(const char *pcStr1, const char *pcStr2, int iCount)
{
    while(iCount)
    {
        //
        // If we reached a NULL in both strings, they must be equal so
        // we end the comparison and return 0
        //
        if(!*pcStr1 && !*pcStr2)
        {
            return(0);
        }

        //
        // Compare the two characters and, if different, return the relevant
        // return code.
        //
        if(*pcStr2 < *pcStr1)
        {
            return(1);
        }
        if(*pcStr1 < *pcStr2)
        {
            return(-1);
        }

        //
        // Move on to the next character.
        //
        pcStr1++;
        pcStr2++;
        iCount--;
    }

    //
    // If we fall out, the strings must be equal for at least the first iCount
    // characters so return 0 to indicate this.
    //
    return(0);

}

//*****************************************************************************
//
//! Compares two strings.
//!
//! \param pcStr1 points to the first string to be compared.
//! \param pcStr2 points to the second string to be compared.
//!
//! This function is very similar to the C library <tt>strcmp()</tt>
//! function.  It compares two strings, taking case into account.  The
//! comparison ends if a terminating NULL character is found in either string.
//! In this case, the shorter string is deemed the lesser.
//!
//! \return Returns 0 if the two strings are equal, -1 if \e pcStr1 is less
//! than \e pcStr2 and 1 if \e pcStr1 is greater than \e pcStr2.
//
//*****************************************************************************
int
ustrcmp(const char *pcStr1, const char *pcStr2)
{
    //
    // Pass this on to ustrncmp.
    //
    return(ustrncmp(pcStr1, pcStr2, -1));
}

//*****************************************************************************
//
// Random Number Generator Seed Value
//
//*****************************************************************************
static unsigned long g_ulRandomSeed = 1;

//*****************************************************************************
//
//! Set the random number generator seed.
//!
//! \param ulSeed is the new seed value to use for the random number generator.
//!
//! This function is very similar to the C library <tt>srand()</tt> function.
//! It will set the seed value used in the <tt>urand()</tt> function.
//!
//! \return None
//
//*****************************************************************************
void
usrand(unsigned long ulSeed)
{
    g_ulRandomSeed = ulSeed;
}

//*****************************************************************************
//
//! Generate a new (pseudo) random number
//!
//! This function is very similar to the C library <tt>rand()</tt> function.
//! It will generate a pseudo-random number sequence based on the seed value.
//!
//! \return A pseudo-random number will be returned.
//
//*****************************************************************************
int
urand(void)
{
    //
    // Generate a new pseudo-random number with a linear congruence random
    // number generator.  This new random number becomes the seed for the next
    // random number.
    //
    g_ulRandomSeed = (g_ulRandomSeed * 1664525) + 1013904223;

    //
    // Return the new random number.
    //
    return((int)g_ulRandomSeed);
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
