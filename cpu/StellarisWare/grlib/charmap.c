//*****************************************************************************
//
// charmap.c - Routines converting between different text codebases.
//
// Copyright (c) 2011-2012 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 9453 of the Stellaris Graphics Library.
//
//*****************************************************************************

#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "grlib/grlib.h"

//*****************************************************************************
//
//! \addtogroup primitives_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Counts the number of zeros at the start of a word.  This macro uses
// compiler-specific constructs to perform an inline insertion of the "clz"
// instruction, which counts the leading zeros directly.
//
//*****************************************************************************
#if defined(ewarm)
#include <intrinsics.h>
#define NumLeadingZeros(x)      __CLZ(x)
#endif
#if defined(codered) || defined(gcc) || defined(sourcerygxx)
#define NumLeadingZeros(x) __extension__                        \
        ({                                                      \
            register unsigned long __ret, __inp = x;            \
            __asm__("clz %0, %1" : "=r" (__ret) : "r" (__inp)); \
            __ret;                                              \
        })
#endif
#if defined(rvmdk) || defined(__ARMCC_VERSION)
#define NumLeadingZeros(x)      __clz(x)
#endif
#if defined(ccs)
//
// The CCS/TI compiler _norm intrinsic function will generate an inline CLZ
// instruction.
//
#define NumLeadingZeros(x)      _norm(x)
#endif

//*****************************************************************************
//
// An array containing Unicode mappings for ISO8859-2 codes from 0xA1 and above.
//
//*****************************************************************************
const unsigned short g_pusISO8859_2_Unicode[] =
{
    0x0104, 0x02D8, 0x0141, 0x00A4, 0x013D, 0x015A, 0x00A7, 0x00A8,
    0x0160, 0x015E, 0x0164, 0x0179, 0x00AD, 0x017D, 0x017B, 0x00B0,
    0x0105, 0x02DB, 0x0142, 0x00B4, 0x013E, 0x015B, 0x02C7, 0x00B8,
    0x0161, 0x015F, 0x0165, 0x017A, 0x02DD, 0x017E, 0x017C, 0x0154,
    0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7, 0x010C,
    0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E, 0x0110,
    0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7, 0x0158,
    0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF, 0x0155,
    0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7, 0x010D,
    0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F, 0x0111,
    0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7, 0x0159,
    0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
};

//*****************************************************************************
//
// An array containing Unicode mappings for ISO8859-3 codes from 0xA1 and above.
//
//*****************************************************************************
const unsigned short g_pusISO8859_3_Unicode[] =
{
    0x0126, 0x02D8, 0x00A3, 0x00A4, 0x0000, 0x0124, 0x00A7, 0x00A8,
    0x0130, 0x015E, 0x011E, 0x0134, 0x00AD, 0x0000, 0x017B, 0x00B0,
    0x0127, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x0125, 0x00B7, 0x00B8,
    0x0131, 0x015F, 0x011F, 0x0135, 0x00BD, 0x0000, 0x017C, 0x00C0,
    0x00C1, 0x00C2, 0x0000, 0x00C4, 0x010A, 0x0108, 0x00C7, 0x00C8,
    0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x0000,
    0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x0120, 0x00D6, 0x00D7, 0x011C,
    0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x016C, 0x015C, 0x00DF, 0x00E0,
    0x00E1, 0x00E2, 0x0000, 0x00E4, 0x010B, 0x0109, 0x00E7, 0x00E8,
    0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x0000,
    0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x0121, 0x00F6, 0x00F7, 0x011D,
    0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x016D, 0x015D, 0x02D9
};

//*****************************************************************************
//
// An array containing Unicode mappings for ISO8859-4 codes from 0xA1 and above.
//
//*****************************************************************************
const unsigned short g_pusISO8859_4_Unicode[] =
{
    0x0104, 0x0138, 0x0156, 0x00A4, 0x0128, 0x013B, 0x00A7, 0x00A8,
    0x0160, 0x0112, 0x0122, 0x0166, 0x00AD, 0x017D, 0x00AF, 0x00B0,
    0x0105, 0x02DB, 0x0157, 0x00B4, 0x0129, 0x013C, 0x02C7, 0x00B8,
    0x0161, 0x0113, 0x0123, 0x0167, 0x014A, 0x017E, 0x014B, 0x0100,
    0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E, 0x010C,
    0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x012A, 0x0110,
    0x0145, 0x014C, 0x0136, 0x00D4, 0x00D5, 0x00D6, 0x00D7, 0x00D8,
    0x0172, 0x00DA, 0x00DB, 0x00DC, 0x0168, 0x016A, 0x00DF, 0x0101,
    0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F, 0x010D,
    0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x012B, 0x0111,
    0x0146, 0x014D, 0x0137, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8,
    0x0173, 0x00FA, 0x00FB, 0x00FC, 0x0169, 0x016B, 0x02D9
};

//*****************************************************************************
//
// An array containing Unicode mappings for ISO8859-7 codes from 0xA1 to 0xBD
//
//*****************************************************************************
const unsigned short g_pusISO8859_7_Unicode[] =
{
    0x2018, 0x2019, 0x00A3, 0x20AC, 0x20AF, 0x00A6, 0x00A7, 0x00A8,
    0x00A9, 0x037A, 0x00AB, 0x00AC, 0x00AD, 0x0000, 0x2015, 0x00B0,
    0x00B1, 0x00B2, 0x00B3, 0x0384, 0x0385, 0x0386, 0x00B7, 0x0388,
    0x0389, 0x038A, 0x00BB, 0x038C, 0x00BD
};

//*****************************************************************************
//
// An array containing Unicode mappings for ISO8859-10 codes from 0xA1 and
// above.
//
//*****************************************************************************
const unsigned short g_pusISO8859_10_Unicode[] =
{
    0x0104, 0x0112, 0x0122, 0x012A, 0x0128, 0x0136, 0x00A7, 0x013B,
    0x0110, 0x0160, 0x0166, 0x017D, 0x00AD, 0x016A, 0x014A, 0x00B0,
    0x0105, 0x0113, 0x0123, 0x012B, 0x0129, 0x0137, 0x00B7, 0x013C,
    0x0111, 0x0161, 0x0167, 0x017E, 0x2015, 0x016B, 0x014B, 0x0100,
    0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x012E, 0x010C,
    0x00C9, 0x0118, 0x00CB, 0x0116, 0x00CD, 0x00CE, 0x00CF, 0x00D0,
    0x0145, 0x014C, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x0168, 0x00D8,
    0x0172, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF, 0x0101,
    0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x012F, 0x010D,
    0x00E9, 0x0119, 0x00EB, 0x0117, 0x00ED, 0x00EE, 0x00EF, 0x00F0,
    0x0146, 0x014D, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x0169, 0x00F8,
    0x0173, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x0138
};

//*****************************************************************************
//
// An array containing Unicode mappings for ISO8859-13 codes from 0xA1 and
// above.
//
//*****************************************************************************
const unsigned short g_pusISO8859_13_Unicode[] =
{
    0x201D, 0x00A2, 0x00A3, 0x00A4, 0x201E, 0x00A6, 0x00A7, 0x00D8,
    0x00A9, 0x0156, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00C6, 0x00B0,
    0x00B1, 0x00B2, 0x00B3, 0x201C, 0x00B5, 0x00B6, 0x00B7, 0x00F8,
    0x00B9, 0x0157, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00E6, 0x0104,
    0x012E, 0x0100, 0x0106, 0x00C4, 0x00C5, 0x0118, 0x0112, 0x010C,
    0x00C9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012A, 0x013B, 0x0160,
    0x0143, 0x0145, 0x00D3, 0x014C, 0x00D5, 0x00D6, 0x00D7, 0x0172,
    0x0141, 0x015A, 0x016A, 0x00DC, 0x017B, 0x017D, 0x00DF, 0x0105,
    0x012F, 0x0101, 0x0107, 0x00E4, 0x00E5, 0x0119, 0x0113, 0x010D,
    0x00E9, 0x017A, 0x0117, 0x0123, 0x0137, 0x012B, 0x013C, 0x0161,
    0x0144, 0x0146, 0x00F3, 0x014D, 0x00F5, 0x00F6, 0x00F7, 0x0173,
    0x0142, 0x015B, 0x016B, 0x00FC, 0x017C, 0x017E, 0x2019
};

//*****************************************************************************
//
// An array containing Unicode mappings for ISO8859-14 codes from 0xA1 to 0xBF.
//
//*****************************************************************************
const unsigned short g_pusISO8859_14_Unicode[] =
{
    0x1E02, 0x1E03, 0x00A3, 0x010A, 0x010B, 0x1E0A, 0x00A7, 0x1E80,
    0x00A9, 0x1E82, 0x1E0B, 0x1EF2, 0x00AD, 0x00AE, 0x0178, 0x1E1E,
    0x1E1F, 0x0120, 0x0121, 0x1E40, 0x1E41, 0x00B6, 0x1E56, 0x1E81,
    0x1E57, 0x1E83, 0x1E60, 0x1EF3, 0x1E84, 0x1E85, 0x1E61
};

//*****************************************************************************
//
// An array containing Unicode mappings for ISO8859-16 codes from 0xA1 and
// above.
//
//*****************************************************************************
const unsigned short g_pusISO8859_16_Unicode[] =
{
    0x0104, 0x0105, 0x0141, 0x20AC, 0x201E, 0x0160, 0x00A7, 0x0161,
    0x00A9, 0x0218, 0x00AB, 0x0179, 0x00AD, 0x017A, 0x017B, 0x00B0,
    0x00B1, 0x010C, 0x0142, 0x017D, 0x201D, 0x00B6, 0x00B7, 0x017E,
    0x010D, 0x0219, 0x00BB, 0x0152, 0x0153, 0x0178, 0x017C, 0x00C0,
    0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0106, 0x00C6, 0x00C7, 0x00C8,
    0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF, 0x0110,
    0x0143, 0x00D2, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x015A, 0x0170,
    0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x0118, 0x021A, 0x00DF, 0x00E0,
    0x00E1, 0x00E2, 0x0103, 0x00E4, 0x0107, 0x00E6, 0x00E7, 0x00E8,
    0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF, 0x0111,
    0x0144, 0x00F2, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x015B, 0x0171,
    0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x0119, 0x021B, 0x00FF
};

//*****************************************************************************
//
// An array containing Unicode mappings for Windows-1250 codes from 0x80 to
// 0xFF.
//
//*****************************************************************************
const unsigned short g_pusWIN1250_Unicode[] =
{
    0x20AC,      0, 0x201A,      0, 0x201E, 0x2026, 0x2020, 0x2021,
         0, 0x2030, 0x0160, 0x2039, 0x015A, 0x0164, 0x017D, 0x0179,
         0, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
         0, 0x2122, 0x0161, 0x203A, 0x015B, 0x0165, 0x017E, 0x017A,
    0x00A0, 0x02C7, 0x02D8, 0x0141, 0x00A4, 0x0104, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0x015E, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x017B,
    0x00B0, 0x00B1, 0x02DB, 0x0142, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
    0x00B8, 0x0105, 0x015F, 0x00BB, 0x013D, 0x02DD, 0x013E, 0x017C,
    0x0154, 0x00C1, 0x00C2, 0x0102, 0x00C4, 0x0139, 0x0106, 0x00C7,
    0x010C, 0x00C9, 0x0118, 0x00CB, 0x011A, 0x00CD, 0x00CE, 0x010E,
    0x0110, 0x0143, 0x0147, 0x00D3, 0x00D4, 0x0150, 0x00D6, 0x00D7,
    0x0158, 0x016E, 0x00DA, 0x0170, 0x00DC, 0x00DD, 0x0162, 0x00DF,
    0x0155, 0x00E1, 0x00E2, 0x0103, 0x00E4, 0x013A, 0x0107, 0x00E7,
    0x010D, 0x00E9, 0x0119, 0x00EB, 0x011B, 0x00ED, 0x00EE, 0x010F,
    0x0111, 0x0144, 0x0148, 0x00F3, 0x00F4, 0x0151, 0x00F6, 0x00F7,
    0x0159, 0x016F, 0x00FA, 0x0171, 0x00FC, 0x00FD, 0x0163, 0x02D9
};

//*****************************************************************************
//
// An array containing Unicode mappings for Windows-1251 codes from 0x80 to
// 0xBF.  Codes from 0xC0 to 0xFF calculate as code + 0x0350.
//
//*****************************************************************************
const unsigned short g_pusWIN1251_Unicode[] =
{
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
    0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x0020, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
    0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
    0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457
};

//*****************************************************************************
//
// An array containing Unicode mappings for Windows-1252 codes from 0x80 to
// 0x9F.  All codes outside this range map directly the same Unicode codepoint.
//
//*****************************************************************************
const unsigned short g_pusWIN1252_Unicode[] =
{
    0x20AC,      0, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
    0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0,      0x017D, 0,
    0,      0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0,      0x017E, 0x0178,
};

//*****************************************************************************
//
// An array containing Unicode mappings for Windows-1253 codes from 0x80 to
// 0xBF.  All codes below 0x80 map map directly the same Unicode codepoint.
// Codes from 0xC0 to 0xFF calculate as code + 0x02D0.
//
//*****************************************************************************
const unsigned short g_pusWIN1253_Unicode[] =
{
    0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397,
    0x0398, 0x0399, 0x039A, 0x039B, 0x039C, 0x039D, 0x039E, 0x039F,
    0x03A0, 0x03A1,      0, 0x03A3, 0x03A4, 0x03A5, 0x03A6, 0x03A7,
    0x03A8, 0x03A9, 0x03AA, 0x03AB, 0x03AC, 0x03AD, 0x03AE, 0x03AF,
    0x03B0, 0x03B1, 0x03B2, 0x03B3, 0x03B4, 0x03B5, 0x03B6, 0x03B7,
    0x03B8, 0x03B9, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BE, 0x03BF,
    0x03C0, 0x03C1, 0x03C2, 0x03C3, 0x03C4, 0x03C5, 0x03C6, 0x03C7,
    0x03C8, 0x03C9, 0x03CA, 0x03CB, 0x03CC, 0x03CD, 0x03CE,      0,
};

//*****************************************************************************
//
// An array containing Unicode mappings for Windows-1254 codes from 0x80 to
// 0x9F.  All codes outside this range are compatible with ISO8859-9.
//
//*****************************************************************************
const unsigned short g_pusWIN1254_Unicode[] =
{
    0x20AC,      0, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
    0x02C6, 0x2030, 0x0160, 0x2039, 0x0152,      0,      0,      0,
         0, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x02DC, 0x2122, 0x0161, 0x203A, 0x0153,      0,      0, 0x0178
};

//*****************************************************************************
//
//! Maps an ISO8859-1 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-1 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-1 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is trivial since
//! the bottom 256 characters in Unicode are the ISO8859-1 characters and since
//! ISO8859-1 is not a variable length encoding (every character is exactly 1
//! byte).
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-1.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_1_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // There is a 1:1 mapping of characters in ISO8859-1 to Unicode.
    //
    *pulSkip = 1;
    return((unsigned long)((unsigned char)*pcSrcChar));
}

//*****************************************************************************
//
//! Maps an ISO8859-2 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-2 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-2 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is straightforward
//! since character codes 0xA0 and below map directly to the same code in
//! Unicode and those from 0xA1 to 0xFF are converted using a global data
//! table.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-2.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_2_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)g_pusISO8859_2_Unicode[
                              (unsigned char)*pcSrcChar - 0xA1]);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-3 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-3 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-3 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is straightforward
//! since character codes 0xA0 and below map directly to the same code in
//! Unicode and those from 0xA1 to 0xFF are converted using a global data table.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-3.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_3_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)g_pusISO8859_3_Unicode[
               (unsigned char)*pcSrcChar - 0xA1]);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-4 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-4 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-4 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is straightforward
//! since character codes 0xA0 and below map directly to the same code in
//! Unicode and those from 0xA1 to 0xFF are converted using a global data table.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-4.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_4_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)g_pusISO8859_4_Unicode[
               (unsigned char)*pcSrcChar - 0xA1]);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-5 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-5 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-5 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is straightforward
//! since character codes 0xA0 and below map directly to the same code in
//! Unicode and those from 0xA1 to 0xFF map to the Unicode by adding 0x360 to
//! the ISO8859-5 code.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-5.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_5_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)((unsigned char)*pcSrcChar) + 0x360);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-6 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-6 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-6 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is straightforward
//! since character codes 0xA4 and below map directly to the same code in
//! Unicode and those from 0xA5 to 0xFF map to the Unicode by adding 0x560 to
//! the ISO8859-5 code.  The only odd-man-out is character 0xAD which maps to
//! Unicode 0xAD even though it is above 0xA4.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-6.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_6_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if(((unsigned char)*pcSrcChar <= 0xA4) ||
       ((unsigned char)*pcSrcChar == 0xAD))
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)((unsigned char)*pcSrcChar) + 0x560);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-7 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-7 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-7 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is straightforward
//! since character codes 0xA0 and below map directly to the same code in
//! Unicode, those from 0xBE to 0xFE map to the Unicode by adding 0x2D0 to
//! the ISO8859-7 code and only a few "unusual" mappings exist between
//! codepoints 0xA1 and 0xBD.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-7.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_7_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        //
        // This character is in the basic ASCII range so return it without
        // remapping.
        //
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else if((unsigned char)*pcSrcChar >= 0xBE)
    {
        //
        // This character can be mapped to Unicode by a simple addition.
        //
        return((unsigned long)((unsigned char)*pcSrcChar) + 0x2D0);
    }
    else
    {
        //
        // This is one of the characters in the range which we map using a
        // table.
        //
        return(g_pusISO8859_7_Unicode[(unsigned char)*pcSrcChar - 0xA1]);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-8 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-8 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-8 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is straightforward
//! since character codes 0xDE and below map directly to the same code in
//! Unicode, those from 0xE0 to 0xFA map to the Unicode by adding 0x4F0 to
//! the ISO8859-7 code and only a few "unusual" mappings exist at codepoints
//! 0xDF, 0xFD and 0xFE.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-8.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_8_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xDE)
    {
        //
        // This character is in the basic ASCII range so return it without
        // remapping.
        //
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else if((unsigned char)*pcSrcChar == 0xDF)
    {
        //
        // Return this special case.
        //
        return(0x2017);
    }
    else if((unsigned char)*pcSrcChar <= 0xFC)
    {
        //
        // Map this range using a simple addition.
        //
        return((unsigned char)*pcSrcChar + 0x4F0);
    }
    else
    {
        //
        // There are only 2 valid characters left at 0xFD and 0xFE.  Map these
        // to Unicode by adding 0x1F11 to map them to 0x200E and 0x200F.
        //
        return((unsigned char)*pcSrcChar + 0x1F11);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-9 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-9 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-9 format into 32 bit Unicode
//! typically used by wide character fonts.  This character set is almost
//! identical to ISO8859-1 but substitutes 6 characters to offer better
//! support for the Turkish language.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-9.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_9_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Deal with the special cases in this switch.
    //
    switch((unsigned char)*pcSrcChar)
    {
        //
        // Character 0xD0 in ISO8859-9 maps to U+011E
        //
        case 0xD0:
        {
            return(0x11E);
        }

        //
        // Character 0xDD in ISO8859-9 maps to U+0130
        //
        case 0xDD:
        {
            return(0x130);
        }

        //
        // Character 0xDE in ISO8859-9 maps to U+015E
        //
        case 0xDE:
        {
            return(0x15E);
        }

        //
        // Character 0xF0 in ISO8859-9 maps to U+011F
        //
        case 0xF0:
        {
            return(0x11F);
        }

        //
        // Character 0xFD in ISO8859-9 maps to U+0131
        //
        case 0xFD:
        {
            return(0x131);
        }

        //
        // Character 0xFE in ISO8859-9 maps to U+015F
        //
        case 0xFE:
        {
            return(0x15F);
        }

        //
        // All other characters map directly to Unicode using the same
        // codepoints.
        //
        default:
        {
            return((unsigned long)(unsigned char)*pcSrcChar);
        }
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-10 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-10 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-10 format into 32 bit Unicode
//! typically used by wide character fonts.  Character codes 0xA0 and below
//! map directly to the same code in Unicode and those from 0xA1 to 0xFF are
//! converted using a global data table.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-10.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_10_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)g_pusISO8859_10_Unicode[
               (unsigned char)*pcSrcChar - 0xA1]);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-11 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-11 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-11 format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion is straightforward
//! since character codes 0xA0 and below map directly to the same code in
//! Unicode and those from 0xA1 to 0xFF map to the Unicode by adding 0xD60 to
//! the ISO8859-11 code.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-11.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_11_Unicode(const char *pcSrcChar,
                        unsigned long ulCount,
                        unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)((unsigned char)*pcSrcChar) + 0xD60);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-13 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-13 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-13 format into 32 bit Unicode
//! typically used by wide character fonts.  Character codes 0xA0 and below
//! map directly to the same code in Unicode and those from 0xA1 to 0xFF are
//! converted using a global data table.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-13.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_13_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)g_pusISO8859_13_Unicode[
               (unsigned char)*pcSrcChar - 0xA1]);
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-14 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-14 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-9 format into 32 bit Unicode
//! typically used by wide character fonts.  This character set is similar to
//! ISO8859-1 but substitutes several characters to offer support for various
//! Celtic languages.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-14.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_14_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Deal with the special cases in this switch.
    //
    switch((unsigned char)*pcSrcChar)
    {
        //
        // Character 0xD0 in ISO8859-14 maps to U+0174
        //
        case 0xD0:
        {
            return(0x174);
        }

        //
        // Character 0xD7 in ISO8859-14 maps to U+1E6A
        //
        case 0xD7:
        {
            return(0x1E6A);
        }

        //
        // Character 0xDE in ISO8859-14 maps to U+0176
        //
        case 0xDE:
        {
            return(0x176);
        }

        //
        // Character 0xF0 in ISO8859-14 maps to U+0175
        //
        case 0xF0:
        {
            return(0x175);
        }

        //
        // Character 0xF7 in ISO8859-14 maps to U+1E6B
        //
        case 0xF7:
        {
            return(0x1E6B);
        }

        //
        // Character 0xFD in ISO8859-14 maps to U+0177
        //
        case 0xFD:
        {
            return(0x131);
        }

        //
        // Other codepoints above 0xC0 map directly to their Unicode
        // equivalents.  Mappings for characters from 0xA1 to 0xBF are handled
        // using a global table.
        //
        default:
        {
            if((unsigned char)*pcSrcChar >= 0xC0)
            {
                return((unsigned long)(unsigned char)*pcSrcChar);
            }
            else
            {
                return((unsigned long)g_pusISO8859_14_Unicode[
                       (unsigned char)*pcSrcChar - 0xA1]);
            }
        }
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-15 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-15 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-15 format into 32 bit Unicode
//! typically used by wide character fonts.  This character set is similar to
//! ISO8859-1 differing from that codepage in only 8 positions.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-15.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_15_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Deal with the special cases in this switch.
    //
    switch((unsigned char)*pcSrcChar)
    {
        //
        // Character 0xA4 in ISO8859-15 maps to U+20AC
        //
        case 0xA4:
        {
            return(0x20AC);
        }

        //
        // Character 0xA6 in ISO8859-15 maps to U+0160
        //
        case 0xA6:
        {
            return(0x160);
        }

        //
        // Character 0xA8 in ISO8859-15 maps to U+0161
        //
        case 0xA8:
        {
            return(0x161);
        }

        //
        // Character 0xB4 in ISO8859-15 maps to U+017D
        //
        case 0xB4:
        {
            return(0x17D);
        }

        //
        // Character 0xB8 in ISO8859-15 maps to U+017E
        //
        case 0xB8:
        {
            return(0x17E);
        }

        //
        // Character 0xBC in ISO8859-15 maps to U+0152
        //
        case 0xBC:
        {
            return(0x152);
        }

        //
        // Character 0xBD in ISO8859-15 maps to U+0153
        //
        case 0xBD:
        {
            return(0x153);
        }

        //
        // Character 0xBE in ISO8859-15 maps to U+0178
        //
        case 0xBE:
        {
            return(0x178);
        }

        //
        // All other codepoints map directly to their Unicode equivalents.
        //
        default:
        {
            return((unsigned long)(unsigned char)*pcSrcChar);
        }
    }
}

//*****************************************************************************
//
//! Maps an ISO8859-16 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing ISO8859-16 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in ISO8859-16 format into 32 bit Unicode
//! typically used by wide character fonts.  Character codes 0xA0 and below
//! map directly to the same code in Unicode and those from 0xA1 to 0xFF are
//! converted using a global data table.
//!
//! See http://unicode.org/Public/MAPPINGS/ISO8859/8859-16.TXT for more
//! information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapISO8859_16_Unicode(const char *pcSrcChar,
                       unsigned long ulCount,
                       unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // ISO8859 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0xA0)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)g_pusISO8859_16_Unicode[
               (unsigned char)*pcSrcChar - 0xA1]);
    }
}

//*****************************************************************************
//
//! Maps a UTF-8 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing UTF-8 encoded text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in UTF-8 format into 32 bit Unicode typically
//! used by wide character fonts.  This conversion will read bytes from the
//! buffer and decode the first full UTF-8 character found, returning the
//! Unicode code for that character and the number of bytes to advance
//! pcSrcChar by to point to the end of the decoded character.  If no valid
//! UTF-8 character is found, 0 is returned.
//!
//! See http://en.wikipedia.org/wiki/UTF-8 for more information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapUTF8_Unicode(const char *pcSrcChar, unsigned long ulCount,
                  unsigned long *pulSkip)
{
    unsigned long ulUnicode, ulToRead, ulIndex;

    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // Clear our our character code storage.
    //
    ulUnicode = 0;
    ulIndex = 0;

    //
    // While we have bytes left to examine in the buffer and the current byte
    // is a continuation code...
    //
    while(ulCount && ((unsigned char)pcSrcChar[ulIndex] & 0xC0) == 0x80)
    {
          //
           // Skip the byte.
           //
           ulIndex++;
           ulCount--;
    }

    //
    // We are now either at the end of the string or the start of a new
    // character unit.  Did we get to the end of the string?
    //
    if(!ulCount)
    {
        //
        // Yes - return 0.
        //
        return(0);
    }

    //
    // Is this a single byte encoded character of the form 0xxxxxxx?
    //
    if(!((unsigned char)(pcSrcChar[ulIndex]) & 0x80))
    {
        //
        // Yes - the Unicode equivalent is just the bottom 7 bits of the
        // UTF-8 byte.
        //
        ulUnicode = (unsigned char)(pcSrcChar[ulIndex++]) & 0x7F;
        ulToRead = 0;
    }
    else
    {
        //
        // This is a multi-byte encoded character. Determine how many bytes
        // it uses.
        //
        ulToRead = NumLeadingZeros(
                ~((unsigned long)pcSrcChar[ulIndex] << 24)) - 1;


        //
        // There can be a maximum of 5 more bytes to read.  If we found more
        // than this, then this is an illegal code.
        //
        if(ulToRead > 5)
        {
            ulUnicode = 0;
            ulToRead = 0;
            ulIndex++;
        }
        else
        {
            //
            // Extract the first bits of the Unicode value from this byte.
            //
            ulUnicode = (unsigned char)pcSrcChar[ulIndex++] &
                        ((1 << (6 - ulToRead)) - 1);

            //
            // Read the remaining bytes encoding this character.
            //
            while(ulCount && ulToRead)
            {
                //
                // Is this a continuation byte with form 10xxxxxx?
                //
                if(((unsigned char)pcSrcChar[ulIndex] & 0xC0) != 0x80)
                {
                    //
                    // No - this is an invalid character to return 0 and tell
                    // the caller to skip past it.
                    //
                    *pulSkip = ulIndex;
                    return(0);
                }
                else
                {
                    //
                    // Extract the bottom 6 bits from the continuation
                    // character and mask them into the Unicode code.
                    //
                    ulUnicode = ((ulUnicode << 6) |
                                 ((unsigned char)pcSrcChar[ulIndex++] & 0x3F));
                    ulToRead--;
                    ulCount--;
                }
            }

            //
            // If we get here and we didn't read the expected number of bytes,
            // this is an invalid character.
            //
            if(ulToRead)
            {
                //
                // We ran out of data before reading all the required
                // continuation bytes so tell the caller to skip the data and
                // return 0 to indicate an error.
                //
                *pulSkip = ulIndex;
                return(0);
            }
        }
    }

    //
    // Tell the caller how many bytes to skip to get to the next character in
    // the string.
    //
    *pulSkip = ulIndex;

    //
    // Return the Unicode character code.
    //
    return(ulUnicode);
}

//*****************************************************************************
//
// Maps a UTF-16LE or UTF-16BE encoded character to its Unicode equivalent.
//
// \param pcSrcChar is a pointer to a string containing UTF-16LE encoded text.
// \param ulCount is the number of bytes in the buffer pointed to by
//        pcSrcChar.
// \param pulSkip points to storage that will be written with the number of
//        bytes to skip in pcSrcChar to get to the next character in the
//        buffer.
// \param bLE is \b true if the \e pcSrcChar points to a UTF-16LE encoded
//        string or \b false if it points to a UTF-16BE string.
//
// This internal function is used to support GrMapUTF16LE_Unicode and
// GrMapUTF16BE_Unicode.
//
// \return Returns the Unicode character code for the first character in the
// \e pcSrcChar string passed.
//
//*****************************************************************************
static unsigned long
GrMapUTF16_Unicode(const char *pcSrcChar, unsigned long ulCount,
                   unsigned long *pulSkip, tBoolean bLE)
{
    unsigned short usChar, usCharToo;
    unsigned long ulCode;
    const char *pcChar;

    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // Walk the string until we find something valid that we can decode.
    //
    ulCode = 0;
    pcChar = pcSrcChar;

    while(ulCount >= 2)
    {
        //
        // Get a word from the string.
        //
        usChar = bLE ? (pcChar[0] + 256 * pcChar[1]) :
                       (pcChar[1] + 256 * pcChar[0]);

        //
        // Move to the next word and update our count.
        //
        ulCount -= 2;
        pcChar += 2;

        //
        // Is this a valid single-word encoding?
        //
        if((usChar <= 0xD7FF) || (usChar >= 0xE000))
        {
            //
            // Yes - return the value and tell the caller to skip the
            // appropriate number of bytes to get to the end of this character.
            //
            ulCode = (unsigned long)usChar;
            break;
        }
        else
        {
            //
            // Is this a valid first word for a high surrogate (first word in a
            // 2 word encoding)?
            //
            if((usChar >= 0xD800) && (usChar < 0xDC00))
            {
                //
                // Yes - this is valid. Now look ahead and see if the next
                // word is valid as a low surrogate, assuming there are at
                // least 2 more bytes for us to look at.
                //
                if(ulCount >= 2)
                {
                    //
                    // Get the next word in the string.
                    //
                    usCharToo = bLE ? (pcChar[0] + 256 * pcChar[1]) :
                                      (pcChar[1] + 256 * pcChar[0]);

                    //
                    // Is this a valid low surrogate?
                    //
                    if((usCharToo >= 0xDC00) && (usCharToo < 0xE000))
                    {
                        //
                        // Yes, it's valid. Update our skip count and parse
                        // the Unicode codepoint from the two words we've read.
                        //
                        pcChar += 2;
                        ulCount -= 2;
                        ulCode = 0x10000 +
                                 ((unsigned long)(usChar - 0xD800) << 10) +
                                 ((unsigned long)usCharToo - 0xDC00);
                        break;
                    }
                }
            }
        }
    }

    //
    // Set the number of bytes to skip in the string to get past the
    // character we just parsed and return the character codepoint if we
    // found one.
    //
    *pulSkip = (unsigned long)(pcChar - pcSrcChar);
    return(ulCode);
}

//*****************************************************************************
//
//! Maps a UTF-16LE encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing UTF-16LE encoded text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in UTF-16LE format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion will read bytes
//! from the buffer and decode the first full UTF-16LE character found,
//! returning the Unicode code for that character and the number of bytes to
//! advance \e pcSrcChar by to point to the end of the decoded character.  If
//! no valid UTF-16LE character is found, 0 is returned.
//!
//! See http://en.wikipedia.org/wiki/UTF-16 for more information.
//!
//! \return Returns the Unicode character code for the first character in the
//! \e pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapUTF16LE_Unicode(const char *pcSrcChar, unsigned long ulCount,
                     unsigned long *pulSkip)
{
    //
    // Call the low level UTF parsing function and return the result.
    //
    return(GrMapUTF16_Unicode(pcSrcChar, ulCount, pulSkip, true));
}

//*****************************************************************************
//
//! Maps a UTF-16BE encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing UTF-16BE encoded text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in UTF-16BE format into 32 bit Unicode
//! typically used by wide character fonts.  This conversion will read bytes
//! from the buffer and decode the first full UTF-16BE character found,
//! returning the Unicode code for that character and the number of bytes to
//! advance \e pcSrcChar by to point to the end of the decoded character.  If
//! no valid UTF-16BE character is found, 0 is returned.
//!
//! See http://en.wikipedia.org/wiki/UTF-16 for more information.
//!
//! \return Returns the Unicode character code for the first character in the
//! \e pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapUTF16BE_Unicode(const char *pcSrcChar, unsigned long ulCount,
                     unsigned long *pulSkip)
{
    //
    // Call the low level UTF parsing function and return the result.
    //
    return(GrMapUTF16_Unicode(pcSrcChar, ulCount, pulSkip, false));
}

//*****************************************************************************
//
//! Maps an 32 bit Unicode encoded character to itself.
//!
//! \param pcSrcChar is a pointer to a string containing 32 bit Unicode text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in Unicode (UTF32) format into 32 bit
//! Unicode typically used by wide character fonts.  This identity conversion is
//! trivial - we merely read 4 bytes at a time and return the 32 bit value they
//! contain. It is assumed that the text is encoded in little endian format.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapUnicode_Unicode(const char *pcSrcChar,
                     unsigned long ulCount,
                     unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // Do we have enough bytes to pull out a single 32 bit character code?
    //
    if(ulCount < 4)
    {
        //
        // Return 0 to indicate an error since we don't have enough data to
        // extract a single character code.
        //
        return(0);
    }
    else
    {
        //
        // We have at least 4 bytes so tell the caller to skip 4 bytes to get
        // the following character and return the Unicode codepoint represented
        // by the first 4 bytes in the buffer.
        //
        *pulSkip = 4;
        return(pcSrcChar[0] | (pcSrcChar[1] << 8) | (pcSrcChar[2] << 16) |
               (pcSrcChar[3] << 24));
    }
}

//*****************************************************************************
//
//! Maps a WIN1250 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing WIN1250 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in WIN1250 format into 32 bit Unicode
//! typically used by wide character fonts.  Windows-1250 is a codepage commonly
//! used in processing eastern European text.  This conversion is
//! straightforward since character codes 0x7F and below map directly to the
//! same code in Unicode, and those from 0x80 to 0xFF are converted using a
//! global data table.
//!
//! See http://en.wikipedia.org/wiki/Windows-1250 for more information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapWIN1250_Unicode(const char *pcSrcChar, unsigned long ulCount,
                     unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // WIN1250 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0x7F)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)g_pusWIN1250_Unicode[
                              (unsigned char)*pcSrcChar - 0x80]);
    }
}

//*****************************************************************************
//
//! Maps a WIN1251 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing WIN1251 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in WIN1251 format into 32 bit Unicode
//! typically used by wide character fonts.  Windows-1251 is a codepage commonly
//! used in processing Cyrillic text.  This conversion is straightforward
//! since character codes 0x7F and below map directly to the same code in
//! Unicode, those from 0x80 to 0xBF are converted using a global data table
//! and those from 0xC0 to 0xFF map to the Unicode by adding 0x350 to
//! the WIN1251 code.
//!
//! See http://en.wikipedia.org/wiki/Windows-1251 for more information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapWIN1251_Unicode(const char *pcSrcChar, unsigned long ulCount,
                     unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // WIN1251 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0x7F)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else if(((unsigned char)*pcSrcChar >= 0x80) &&
            ((unsigned char)*pcSrcChar <= 0xBF))
    {
        return((unsigned long)g_pusWIN1251_Unicode[
               (unsigned char)*pcSrcChar - 0x80]);
    }
    else
    {
         return((unsigned long)((unsigned char)*pcSrcChar) + 0x350);
    }
}

//*****************************************************************************
//
//! Maps a WIN1252 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing WIN1252 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in WIN1252 format into 32 bit Unicode
//! typically used by wide character fonts.  Windows-1252 is a codepage commonly
//! used in processing western European text.  This conversion is
//! straightforward since character codes 0x7F and below, and 0xA0 and above
//! map directly to the same code in Unicode, and those from 0x80 to 0x9F are
//! converted using a global data table.  This codepage can be thought of as a
//! superset of ISO8859-1 and text purported to be encoded in ISO8859-1 is
//! frequently processed using this codepage instead.
//!
//! See http://en.wikipedia.org/wiki/Windows-1252 for more information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapWIN1252_Unicode(const char *pcSrcChar, unsigned long ulCount,
                     unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // WIN1252 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if(((unsigned char)*pcSrcChar <= 0x7F) ||
       ((unsigned char)*pcSrcChar >= 0xA0))
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else
    {
        return((unsigned long)g_pusWIN1252_Unicode[
                              (unsigned char)*pcSrcChar - 0x80]);
    }
}

//*****************************************************************************
//
//! Maps a WIN1253 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing WIN1253 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in WIN1253 format into 32 bit Unicode
//! typically used by wide character fonts.  Windows-1253 is a codepage commonly
//! used in processing Greek text.  This conversion is straightforward
//! since character codes 0x7F and below map directly to the same code in
//! Unicode, those from 0x80 to 0xBF are converted using a global data table
//! and those from 0xC0 to 0xFF map to the Unicode by adding 0x2D0 to
//! the WIN1253 code.
//!
//! See http://en.wikipedia.org/wiki/Windows-1253 for more information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapWIN1253_Unicode(const char *pcSrcChar, unsigned long ulCount,
                     unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // WIN1253 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if((unsigned char)*pcSrcChar <= 0x7F)
    {
        return((unsigned long)(unsigned char)*pcSrcChar);
    }
    else if(((unsigned char)*pcSrcChar >= 0x80) &&
            ((unsigned char)*pcSrcChar <= 0xBF))
    {
        //
        // This is a character we remap via the global table.
        //
        return((unsigned long)g_pusWIN1253_Unicode[
               (unsigned char)*pcSrcChar - 0x80]);
    }
    else
    {
        //
        // This is a character we can calculate the Unicode character code for.
        //
        return((unsigned long)((unsigned char)*pcSrcChar) + 0x2D0);
    }
}

//*****************************************************************************
//
//! Maps a WIN1254 encoded character to its Unicode equivalent.
//!
//! \param pcSrcChar is a pointer to a string containing WIN1254 encoded
//!        text.
//! \param ulCount is the number of bytes in the buffer pointed to by
//!        pcSrcChar.
//! \param pulSkip points to storage that will be written with the number of
//!        bytes to skip in pcSrcChar to get to the next character in the
//!        buffer.
//!
//! This function may be passed to GrCodepageMapTableSet() in a tCodePointMap
//! structure to map source text in WIN1254 format into 32 bit Unicode
//! typically used by wide character fonts.  Windows-1254 is a codepage commonly
//! used in processing Turkish text.  It is compatible with ISO8859-9 but adds
//! several printable characters in the 0x80-0x9F range.
//!
//! See http://en.wikipedia.org/wiki/Windows-1254 for more information.
//!
//! \return Returns the Unicode character code for the first character in the
//! pcSrcChar string passed.
//
//*****************************************************************************
unsigned long
GrMapWIN1254_Unicode(const char *pcSrcChar, unsigned long ulCount,
                     unsigned long *pulSkip)
{
    //
    // Parameter sanity check.
    //
    ASSERT(pulSkip);
    ASSERT(pcSrcChar);

    //
    // WIN1254 mappings are all 8 bits so we skip a single byte for each
    // character.
    //
    *pulSkip = 1;

    //
    // Is this character one of those which maps directly to Unicode?
    //
    if(((unsigned char)*pcSrcChar <= 0x7F) ||
       ((unsigned char)*pcSrcChar >= 0xA0))
    {
        return(GrMapISO8859_9_Unicode(pcSrcChar, ulCount, pulSkip));
    }
    else
    {
        //
        // This is a character we remap via the global table.
        //
        return((unsigned long)g_pusWIN1254_Unicode[
               (unsigned char)*pcSrcChar - 0x80]);
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
