//*****************************************************************************
//
// grlib.h - Prototypes for the low level primitives provided by the graphics
//           library.
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
// This is part of revision 9453 of the Stellaris Graphics Library.
//
//*****************************************************************************

#ifndef __GRLIB_H__
#define __GRLIB_H__

//*****************************************************************************
//
//! \addtogroup primitives_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! This structure defines the extents of a rectangle.  All points greater than
//! or equal to the minimum and less than or equal to the maximum are part of
//! the rectangle.
//
//*****************************************************************************
typedef struct
{
    //
    //! The minimum X coordinate of the rectangle.
    //
    short sXMin;

    //
    //! The minimum Y coordinate of the rectangle.
    //
    short sYMin;

    //
    //! The maximum X coordinate of the rectangle.
    //
    short sXMax;

    //
    //! The maximum Y coordinate of the rectangle.
    //
    short sYMax;
}
tRectangle;

//*****************************************************************************
//
//! This structure defines the characteristics of a display driver.
//
//*****************************************************************************
typedef struct
{
    //
    //! The size of this structure.
    //
    long lSize;

    //
    //! A pointer to display driver-specific data.
    //
    void *pvDisplayData;

    //
    //! The width of this display.
    //
    unsigned short usWidth;

    //
    //! The height of this display.
    //
    unsigned short usHeight;

    //
    //! A pointer to the function to draw a pixel on this display.
    //
    void (*pfnPixelDraw)(void *pvDisplayData, long lX, long lY,
                         unsigned long ulValue);

    //
    //! A pointer to the function to draw multiple pixels on this display.
    //
    void (*pfnPixelDrawMultiple)(void *pvDisplayData, long lX, long lY,
                                 long lX0, long lCount, long lBPP,
                                 const unsigned char *pucData,
                                 const unsigned char *pucPalette);

    //
    //! A pointer to the function to draw a horizontal line on this display.
    //
    void (*pfnLineDrawH)(void *pvDisplayData, long lX1, long lX2, long lY,
                         unsigned long ulValue);

    //
    //! A pointer to the function to draw a vertical line on this display.
    //
    void (*pfnLineDrawV)(void *pvDisplayData, long lX, long lY1, long lY2,
                         unsigned long ulValue);

    //
    //! A pointer to the function to draw a filled rectangle on this display.
    //
    void (*pfnRectFill)(void *pvDisplayData, const tRectangle *pRect,
                        unsigned long ulValue);

    //
    //! A pointer to the function to translate 24-bit RGB colors to
    //! display-specific colors.
    //
    unsigned long (*pfnColorTranslate)(void *pvDisplayData,
                                       unsigned long ulValue);

    //
    //! A pointer to the function to flush any cached drawing operations on
    //! this display.
    //
    void (*pfnFlush)(void *pvDisplayData);
}
tDisplay;

//*****************************************************************************
//
//! This structure describes a font used for drawing text onto the screen.
//! Fonts in this format may encode ASCII characters with codepoints in the
//! range 0x20 - 0x7F.   More information on this and the other supported font
//! structures may be found in the ``Font Format'' section of the user's guide.
//
//*****************************************************************************
typedef struct
{
    //
    //! The format of the font.  Can be one of FONT_FMT_UNCOMPRESSED or
    //! FONT_FMT_PIXEL_RLE.
    //
    unsigned char ucFormat;

    //
    //! The maximum width of a character; this is the width of the widest
    //! character in the font, though any individual character may be narrower
    //! than this width.
    //
    unsigned char ucMaxWidth;

    //
    //! The height of the character cell; this may be taller than the font data
    //! for the characters (to provide inter-line spacing).
    //
    unsigned char ucHeight;

    //
    //! The offset between the top of the character cell and the baseline of
    //! the glyph.  The baseline is the bottom row of a capital letter, below
    //! which only the descenders of the lower case letters occur.
    //
    unsigned char ucBaseline;

    //
    //! The offset within pucData to the data for each character in the font.
    //
    unsigned short pusOffset[96];

    //
    //! A pointer to the data for the font.
    //
    const unsigned char *pucData;
}
tFont;

//*****************************************************************************
//
//! This is a newer version of the structure which describes a font used
//! for drawing text onto the screen.  This variant allows a font to contain an
//! arbitrary, contiguous block of codepoints from the 256 basic characters in
//! an ISO8859-n font and allows support for accented characters in Western
//! European languages and any left-to-right typeface supported by an ISO8859
//! variant. Fonts encoded in this format may be used interchangeably with the
//! original fonts merely by casting the structure pointer when calling any
//! function or macro which expects a font pointer as a parameter.  More
//! information on this and the other supported font structures may be found in
//! the ``Font Format'' section of the user's guide.
//
//*****************************************************************************
typedef struct
{
    //
    //! The format of the font.  Can be one of FONT_FMT_EX_UNCOMPRESSED or
    //! FONT_FMT_EX_PIXEL_RLE.
    //
    unsigned char ucFormat;

    //
    //! The maximum width of a character; this is the width of the widest
    //! character in the font, though any individual character may be narrower
    //! than this width.
    //
    unsigned char ucMaxWidth;

    //
    //! The height of the character cell; this may be taller than the font data
    //! for the characters (to provide inter-line spacing).
    //
    unsigned char ucHeight;

    //
    //! The offset between the top of the character cell and the baseline of
    //! the glyph.  The baseline is the bottom row of a capital letter, below
    //! which only the descenders of the lower case letters occur.
    //
    unsigned char ucBaseline;

    //
    //! The codepoint number representing the first character encoded in the
    //! font.
    //
    unsigned char ucFirst;

    //
    //! The codepoint number representing the last character encoded in the
    //! font.
    //
    unsigned char ucLast;

    //
    //! A pointer to a table containing the offset within pucData to the data
    //! for each character in the font.
    //
    const unsigned short *pusOffset;

    //
    //! A pointer to the data for the font.
    //
    const unsigned char *pucData;
}
tFontEx;

//*****************************************************************************
//
//! This variant of the font structure supports Unicode and other multi-byte
//! character sets.  It is intended for use when rendering such languages as
//! traditional and simplified Chinese, Korean and Japanese.  The font
//! supports multiple blocks of contiguous characters and includes a codepage
//! identifier to allow GrLib to correctly map source codepoints to font
//! glyphs in cases where the codepages may differ.  More information on
//! this and the other supported font structures may be found in the ``Font
//! Format'' section of the user's guide.
//
// For the benefit of those of you reading the source...

// Note that, unlike tFont and tFontEx where the character data and offset
// tables are referenced by pointer and may be discontiguous, a font described
// using tFontWide is assumed to comprise a single, contiguous block of data
// with a tFontWide structure as its header, a number of tFontBlock structures
// immediately following this, and a number of tFontOffsetTable and font glyph
// data entries following this.  This format ensures that the font is position-
// independent and allows use in external memory or from non-random-access
// storage such as SDCards or SSI flash.
//
// A complete font in this format would look as follows:
//
//      ------------------------------
//  0  | tFontWide header             |
//     | ulNumBlocks = n              |
//      ------------------------------
//     | tFontBlock[0]                | First block describes characters from
//     | ulStartCodepoint = S0        | codepoint S0 to (S0 + N0 - 1). Offset
//     | ulNumCodepoints  = N0        | to glyph table is OF0 bytes from the
//     | ulGlyphTableOffset = OF0     | start of the tFontWide header struct.
//      ------------------------------
//     |     ...                      |
//      ------------------------------
//     | tFontBlock[n-1]              | Final block describes characters from
//     | ulStartCodepoint = S(n-1)    | codepoint S(n-1) to (S(n-1) + N(n-1)
//     | ulNumCodepoints  = N(n-1)    | - 1).
//     | ulGlyphTableOffset = OF(n-1) |
//      ------------------------------
//      Start of Block 0 Offset Table
//      ------------------------------
// OF0 | ulGlyphOffset = GO0          | Offset from OF0 to glyph S0 data.
//     |                              |
//      ------------------------------
//     | ulGlyphOffset = GO1          | Offset from OF0 to glyph (S0 + 1) data.
//     |                              |
//      ------------------------------
//     |     ...                      |
//     |                              |
//      ------------------------------
//     | ulGlyphOffset = GO(N0 - 1)   | Offset from OF0 to glyph (S0 + N0 - 1)
//     |                              |
//      ------------------------------
//       Start of Block 0 Glyph Data
//      ------------------------------
// GO0 | Glyph data for codepoint     |
//     | S0 from block 0              |
//      ------------------------------
// GO1 | Glyph data for codepoint     |
//     | S0 + 1 from block 0          |
//      ------------------------------
//     |     ...                      |
//      ------------------------------
// GO  | Glyph data for codepoint     |
//(N-1)| (S0 + N0 - 1) from block 0   |
//      ------------------------------
//      Start of Block 1 Offset Table
//      ------------------------------
// OF1 | ulGlyphOffset = G10          | Offset from OF1 to glyph S1 data.
//     |                              |
//      ------------------------------
//     | ulGlyphOffset = G11          | Offset from OF1 to glyph (S1 + 1) data.
//     |                              |
//      ------------------------------
//     |     ...                      |
//     |                              |
//      ------------------------------
//     | ulGlyphOffset = G1(N1 - 1)   | Offset from OF1 to glyph (S1 + N1 - 1)
//     |                              |
//      ------------------------------
//       Start of Block 1 Glyph Data
//      ------------------------------
// G10 | Glyph data for codepoint     |
//     | S1 from block 1              |
//      ------------------------------
// G11 | Glyph data for codepoint     |
//     | S1 + 1 from block 1          |
//      ------------------------------
//     |     ...                      |
//      ------------------------------
// G1  | Glyph data for codepoint     |
//(N-1)| (S1 + N1 - 1) from block 1   |
//      ------------------------------
//! GO0 | Glyph data for codepoint     |
//     | S0 from block 0              |
//!      ------------------------------
//      Start of Block 2 Offset Table
//!      ------------------------------
//     | ...continued for remaining   |
//     | font blocks.                 |
//      ------------------------------
//
//*****************************************************************************
typedef struct
{
    //
    //! The format of the font.  Can be one of FONT_FMT_WIDE_UNCOMPRESSED or
    //! FONT_FMT_WIDE_PIXEL_RLE.
    //
    unsigned char ucFormat;

    //
    //! The maximum width of a character; this is the width of the widest
    //! character in the font, though any individual character may be narrower
    //! than this width.
    //
    unsigned char ucMaxWidth;

    //
    //! The height of the character cell; this may be taller than the font data
    //! for the characters (to provide inter-line spacing).
    //
    unsigned char ucHeight;

    //
    //! The offset between the top of the character cell and the baseline of
    //! the glyph.  The baseline is the bottom row of a capital letter, below
    //! which only the descenders of the lower case letters occur.
    //
    unsigned char ucBaseline;

    //
    //! The codepage that is used to find characters in this font.  This
    //! defines the codepoint-to-glyph mapping within this font.
    //
    unsigned short usCodepage;

    //
    //! The number of blocks of characters described by this font where a block
    //! contains a number of contiguous codepoints.
    //
    unsigned short usNumBlocks;
}
tFontWide;

typedef struct
{
    //
    //! The first codepoint in this block of characters.  The meaning of this
    //! value depends upon the codepage that the font is using, as defined in
    //! the eCodePage field of the associated tFontWide structure.
    //
    unsigned long ulStartCodepoint;

    //
    //! The number of characters encoded in this block.  The first character
    //! is given by ulStartCodepoint and the last is (ulStartCodepoint +
    //! ulNumBlockChars - 1).
    //
    unsigned long ulNumCodepoints;

    //
    //! The offset from the beginning of the tFontWide header to the glyph
    //! offset table for this block of characters.
    //
    unsigned long ulGlyphTableOffset;
}
tFontBlock;

typedef struct
{
    //
    //! The offset of each glyph in the block relative to the first entry in
    //! this table.  This structure is represented as an array of 1 entry but
    //! the actual number of entries is given in the usNumBlockChars entry of
    //! the tFontBlock that points to this structure.
    //!
    //! The value provided in ulGlyphOffset[n] is the byte offset from the
    //! start of the tFontBlock structure that this glyph belongs to to the
    //! first byte of the glyph data.
    //!
    //! To support fonts which contain large blocks of codepoints with
    //! small gaps, a ulGlyphOffset value of 0 indicates that the codepoint in
    //! question is not populated in the font.  Using this method, single
    //! characters may be skipped while avoiding the overhead of defining a new
    //! block.
    //
    unsigned long ulGlyphOffset[1];
}
tFontOffsetTable;

//*****************************************************************************
//
//! The jump table used to access a particular wrapped (offline) font.
//! This table exists for each type of wrapped font in use with the functions
//! dependent upon the storage medium holding the font.
//
//*****************************************************************************
typedef struct
{
    //
    //! A pointer to the function which will return information on the
    //! font.  This is used to support GrFontInfoGet.
    //
    void (*pfnFontInfoGet)(unsigned char *pucFontId, unsigned char *pucFormat,
                           unsigned char *pucWidth, unsigned char *pucHeight,
                           unsigned char *pucBaseline);

    //
    //! A pointer to the function used to retrieve data for a particular font
    //! glyph.  This function returns a pointer to the glyph data in linear,
    //! random access memory.  If a buffer is required to ensure this, that
    //! buffer must be owned and managed by the font wrapper function.  It is
    //! safe to assume that this function will not be called again until any
    //! previously requested glyph data has been used so a single character
    //! buffer should suffice.  This is used to support GrFontGlyphDataGet.
    //
    const unsigned char *(*pfnFontGlyphDataGet)(unsigned char *pucFontId,
                                                unsigned long ulCodePoint,
                                                unsigned char *pucWidth);

    //
    //! A pointer to the function used to determine the codepage supported by
    //! the font.
    //
    unsigned short (*pfnFontCodepageGet)(unsigned char *pucFontId);

    //
    //! A pointer to the function used to determine the number of blocks of
    //! codepoints supported by the font.
    //
    unsigned short (*pfnFontNumBlocksGet)(unsigned char *pucFontId);

    //
    //! A pointer to the function used to determine the codepoints in a given
    //! codepoints in a given font block.
    //
    unsigned long (*pfnFontBlockCodepointsGet)(unsigned char *pucFontId,
                                               unsigned short usBlockIndex,
                                               unsigned long *pulStart);
}
tFontAccessFuncs;

//*****************************************************************************
//
//! This is a wrapper used to support fonts which are stored in a file system
//! or other non-random access storage.  The font is accessed by means of
//! access functions whose pointers are described in this structure.  The
//! pucFontId field is written with a handle supplied to the application by
//! the font wrapper's FontLoad function and is passed to all access functions
//! to identify the font in use.  Wrapped fonts may be used by any GrLib
//! function that accepts a font pointer as a parameter merely by casting the
//! pointer appropriately.
//
//*****************************************************************************
typedef struct
{
    //
    //! The format of the font.  Will be FONT_FMT_WRAPPED.
    //
    unsigned char ucFormat;

    //
    //! A pointer to information required to allow the font access functions
    //! to find the font to be used.  This value is returned from a call to
    //! the FontLoad function for the particular font wrapper in use.
    //
    unsigned char *pucFontId;

    //
    //! Access functions for this font.
    //
    const tFontAccessFuncs *pFuncs;
}
tFontWrapper;

//*****************************************************************************
//
//! Indicates that the font data is stored in an uncompressed format.
//
//*****************************************************************************
#define FONT_FMT_UNCOMPRESSED   0x00

//*****************************************************************************
//
//! Indicates that the font data is stored using a pixel-based RLE format.
//
//*****************************************************************************
#define FONT_FMT_PIXEL_RLE      0x01

//*****************************************************************************
//
//! A marker used in the ucFormat field of a font to indicates that the font
//! data is stored using the new tFontEx structure.
//
//*****************************************************************************
#define FONT_EX_MARKER          0x80

//*****************************************************************************
//
//! Indicates that the font data is stored in an uncompressed format and uses
//! the tFontEx structure format.
//
//*****************************************************************************
#define FONT_FMT_EX_UNCOMPRESSED   (FONT_FMT_UNCOMPRESSED | FONT_EX_MARKER)

//*****************************************************************************
//
//! Indicates that the font data is stored using a pixel-based RLE format and
//! uses the tFontEx structure format.
//
//*****************************************************************************
#define FONT_FMT_EX_PIXEL_RLE      (FONT_FMT_PIXEL_RLE | FONT_EX_MARKER)

//*****************************************************************************
//
//! A marker used in the ucFormat field of a font to indicates that the font
//! data is stored using the new tFontWide structure.
//
//*****************************************************************************
#define FONT_WIDE_MARKER          0x40

//*****************************************************************************
//
//! Indicates that the font data is stored in an uncompressed format and uses
//! the tFontWide structure format.
//
//*****************************************************************************
#define FONT_FMT_WIDE_UNCOMPRESSED   (FONT_FMT_UNCOMPRESSED | FONT_WIDE_MARKER)

//*****************************************************************************
//
//! Indicates that the font data is stored using a pixel-based RLE format and
//! uses the tFontWide structure format.
//
//*****************************************************************************
#define FONT_FMT_WIDE_PIXEL_RLE      (FONT_FMT_PIXEL_RLE | FONT_WIDE_MARKER)

//*****************************************************************************
//
//! Indicates that the font data is stored in offline storage (file system,
//! serial memory device, etc) and must be accessed using wrapper functions.
//! Fonts using this format are described using a tFontWrapper structure.
//
//*****************************************************************************
#define FONT_FMT_WRAPPED        0x20

//*****************************************************************************
//
//! Indicates that the image data is not compressed and represents each pixel
//! with a single bit.
//
//*****************************************************************************
#define IMAGE_FMT_1BPP_UNCOMP   0x01

//*****************************************************************************
//
//! Indicates that the image data is not compressed and represents each pixel
//! with four bits.
//
//*****************************************************************************
#define IMAGE_FMT_4BPP_UNCOMP   0x04

//*****************************************************************************
//
//! Indicates that the image data is not compressed and represents each pixel
//! with eight bits.
//
//*****************************************************************************
#define IMAGE_FMT_8BPP_UNCOMP   0x08

//*****************************************************************************
//
//! Indicates that the image data is compressed and represents each pixel with
//! a single bit.
//
//*****************************************************************************
#define IMAGE_FMT_1BPP_COMP     0x81

//*****************************************************************************
//
//! Indicates that the image data is compressed and represents each pixel with
//! four bits.
//
//*****************************************************************************
#define IMAGE_FMT_4BPP_COMP     0x84

//*****************************************************************************
//
//! Indicates that the image data is compressed and represents each pixel with
//! eight bits.
//
//*****************************************************************************
#define IMAGE_FMT_8BPP_COMP     0x88

#ifndef GRLIB_REMOVE_WIDE_FONT_SUPPORT
//*****************************************************************************
//
//! Identifiers for codepages and source text encoding formats.
//
//*****************************************************************************
#define CODEPAGE_ISO8859_1     0x0000
#define CODEPAGE_ISO8859_2     0x0001
#define CODEPAGE_ISO8859_3     0x0002
#define CODEPAGE_ISO8859_4     0x0013
#define CODEPAGE_ISO8859_5     0x0003
#define CODEPAGE_ISO8859_6     0x0004
#define CODEPAGE_ISO8859_7     0x0005
#define CODEPAGE_ISO8859_8     0x0006
#define CODEPAGE_ISO8859_9     0x0007
#define CODEPAGE_ISO8859_10    0x0008
#define CODEPAGE_ISO8859_11    0x0009
#define CODEPAGE_ISO8859_13    0x000A
#define CODEPAGE_ISO8859_14    0x000B
#define CODEPAGE_ISO8859_15    0x000C
#define CODEPAGE_ISO8859_16    0x000D
#define CODEPAGE_UNICODE       0x000E
#define CODEPAGE_GB2312        0x000F
#define CODEPAGE_GB18030       0x0010
#define CODEPAGE_BIG5          0x0011
#define CODEPAGE_SHIFT_JIS     0x0012
#define CODEPAGE_WIN1250       0x0013
#define CODEPAGE_WIN1251       0x0014
#define CODEPAGE_WIN1252       0x0015
#define CODEPAGE_WIN1253       0x0016
#define CODEPAGE_WIN1254       0x0017
#define CODEPAGE_WIN1255       0x0018
#define CODEPAGE_WIN1256       0x0019
#define CODEPAGE_WIN1257       0x001A
#define CODEPAGE_WIN1258       0x001B

//
// UTF-8 and UTF-16 may be specified as the source text encoding but may not
// be used to describe the codepage in use in a font since they are simply
// different encoding methods for Unicode.
//
#define CODEPAGE_UTF_8         0x4000
#define CODEPAGE_UTF_16LE      0x4001
#define CODEPAGE_UTF_16BE      0x4002
#define CODEPAGE_UTF_16        CODEPAGE_UTF_16BE

//
// Applications wishing to use custom fonts with, for example, application-
// specific glyph mappings may use codepage identifiers above
// CODEPAGE_CUSTOM_BASE.
//
#define CODEPAGE_CUSTOM_BASE   0x8000

//*****************************************************************************
//
//! A structure used to define a mapping function that converts text in one
//! codepage to a different codepage.  Typically this is used to translate
//! text strings into the codepoints needed to retrieve the appropriate
//! glyphs from a font.
//
//*****************************************************************************
typedef struct
{
    //
    //! The codepage used to describe the source characters.
    //
    unsigned short usSrcCodepage;

    //
    //! The codepage into which source characters are to be mapped.
    //
    unsigned short usFontCodepage;

    //
    //! A pointer to the conversion function which will be used to translate
    //! input strings into codepoints in the output codepage.
    //
    unsigned long (*pfnMapChar)(const char *pcSrcChar,
                                unsigned long ulCount,
                                unsigned long *pulSkip);
}
tCodePointMap;

//*****************************************************************************
//
// Forward reference to the graphics context structure.
//
//*****************************************************************************
struct _tContext;

//*****************************************************************************
//
// A function pointer for a replacement text string rendering function.  The
// prototype for this function follows GrStringDraw.  Applications making use
// of scripts which require special handling for diacritics, ligatures or
// character composition/decomposition may replace the default GrLib string
// renderer with one of their own which understands these rules.
//
//*****************************************************************************
typedef void (*tStringRenderer)(const struct _tContext *, const char *, long, long,
                                long, unsigned long);

//*****************************************************************************
//
//! This structure contains default values that are set in any new context
//! initialized via a call to GrContextInit.  This structure is passed to the
//! graphics library using the GrLibInit function.
//
//*****************************************************************************
typedef struct
{
    //
    //! The default string rendering function to use.  This will normally be
    //! GrDefaultStringRenderer but may be replaced when supporting languages
    //! requiring mixed rendering directions such as Arabic or Hebrew.
    //
    void (*pfnStringRenderer)(const struct _tContext *, const char *, long,
                              long, long, unsigned long);

    //
    //! The default codepoint mapping function table.  This table contains
    //! information allowing GrLib to map text in the source codepage to the
    //! correct glyphs in the fonts to be used.  The field points to the first
    //! element of an array of ucNumCodePointMaps structures.
    //
    tCodePointMap *pCodePointMapTable;

    //
    //! The default source text codepage encoding in use by the application.
    //
    unsigned short usCodepage;

    //
    //! The number of entries in the pCodePointMapTable array.
    //
    unsigned char ucNumCodePointMaps;

    //
    //! Reserved for future expansion.
    //
    unsigned char ucReserved;
}
tGrLibDefaults;

#endif

//*****************************************************************************
//
//! This structure defines a drawing context to be used to draw onto the
//! screen.  Multiple drawing contexts may exist at any time.
//
//*****************************************************************************
#ifdef DOXYGEN
typedef struct
#else
typedef struct _tContext
#endif
{
    //
    //! The size of this structure.
    //
    long lSize;

    //
    //! The screen onto which drawing operations are performed.
    //
    const tDisplay *pDisplay;

    //
    //! The clipping region to be used when drawing onto the screen.
    //
    tRectangle sClipRegion;

    //
    //! The color used to draw primitives onto the screen.
    //
    unsigned long ulForeground;

    //
    //! The background color used to draw primitives onto the screen.
    //
    unsigned long ulBackground;

    //
    //! The font used to render text onto the screen.
    //
    const tFont *pFont;

#ifndef GRLIB_REMOVE_WIDE_FONT_SUPPORT
    //
    //! A pointer to a replacement string rendering function.  Applications
    //! can use this for language-specific string rendering support.  If
    //! set, this function is passed control whenever GrStringDraw is called.
    //
    void (*pfnStringRenderer)(const struct _tContext *, const char *, long,
                              long, long, unsigned long);

    //
    //! A table of functions used to map between the various supported
    //! source codepages and the codepages supported by fonts in use.
    //
    const tCodePointMap *pCodePointMapTable;

    //
    //! The currently selected source text codepage.
    //
    unsigned short usCodepage;

    //
    //! The number of entries in the pCodePointMapTable array.
    //
    unsigned char ucNumCodePointMaps;

    //
    //! The index of the codepoint map table entry which is currently in use
    //! based on the selected source codepage and the current font.
    //
    unsigned char ucCodePointMap;

    //
    //! Reserved for future expansion.
    //
    unsigned char ucReserved;
#endif
}
tContext;

//*****************************************************************************
//
//! Sets the background color to be used.
//!
//! \param pContext is a pointer to the drawing context to modify.
//! \param ulValue is the 24-bit RGB color to be used.
//!
//! This function sets the background color to be used for drawing operations
//! in the specified drawing context.
//!
//! \return None.
//
//*****************************************************************************
#define GrContextBackgroundSet(pContext, ulValue)                        \
        do                                                               \
        {                                                                \
            tContext *pC = pContext;                                     \
            pC->ulBackground = DpyColorTranslate(pC->pDisplay, ulValue); \
        }                                                                \
        while(0)

//*****************************************************************************
//
//! Sets the background color to be used.
//!
//! \param pContext is a pointer to the drawing context to modify.
//! \param ulValue is the display driver-specific color to be used.
//!
//! This function sets the background color to be used for drawing operations
//! in the specified drawing context, using a color that has been previously
//! translated to a driver-specific color (for example, via
//! DpyColorTranslate()).
//!
//! \return None.
//
//*****************************************************************************
#define GrContextBackgroundSetTranslated(pContext, ulValue) \
        do                                                  \
        {                                                   \
            tContext *pC = pContext;                        \
            pC->ulBackground = ulValue;                     \
        }                                                   \
        while(0)

//*****************************************************************************
//
//! Gets the width of the display being used by this drawing context.
//!
//! \param pContext is a pointer to the drawing context to query.
//!
//! This function returns the width of the display that is being used by this
//! drawing context.
//!
//! \return Returns the width of the display in pixels.
//
//*****************************************************************************
#define GrContextDpyWidthGet(pContext)      \
        (DpyWidthGet((pContext)->pDisplay))

//*****************************************************************************
//
//! Gets the height of the display being used by this drawing context.
//!
//! \param pContext is a pointer to the drawing context to query.
//!
//! This function returns the height of the display that is being used by this
//! drawing context.
//!
//! \return Returns the height of the display in pixels.
//
//*****************************************************************************
#define GrContextDpyHeightGet(pContext)      \
        (DpyHeightGet((pContext)->pDisplay))

#ifdef GRLIB_REMOVE_WIDE_FONT_SUPPORT
//*****************************************************************************
//
//! Sets the font to be used.
//!
//! \param pContext is a pointer to the drawing context to modify.
//! \param pFnt is a pointer to the font to be used.
//!
//! This function sets the font to be used for string drawing operations in the
//! specified drawing context.  If a tFontEx type font is to be used, cast its
//! pointer to a pFont pointer before passing it as the pFnt parameter.
//!
//! \return None.
//
//*****************************************************************************
#define GrContextFontSet(pContext, pFnt) \
        do                               \
        {                                \
            tContext *pC = pContext;     \
            const tFont *pF = pFnt;      \
            pC->pFont = pF;              \
        }                                \
        while(0)
#else
extern void GrContextFontSet(tContext *pContext, const tFont *pFnt);
#endif

//*****************************************************************************
//
//! Sets the foreground color to be used.
//!
//! \param pContext is a pointer to the drawing context to modify.
//! \param ulValue is the 24-bit RGB color to be used.
//!
//! This function sets the color to be used for drawing operations in the
//! specified drawing context.
//!
//! \return None.
//
//*****************************************************************************
#define GrContextForegroundSet(pContext, ulValue)                        \
        do                                                               \
        {                                                                \
            tContext *pC = pContext;                                     \
            pC->ulForeground = DpyColorTranslate(pC->pDisplay, ulValue); \
        }                                                                \
        while(0)

//*****************************************************************************
//
//! Sets the foreground color to be used.
//!
//! \param pContext is a pointer to the drawing context to modify.
//! \param ulValue is the display driver-specific color to be used.
//!
//! This function sets the foreground color to be used for drawing operations
//! in the specified drawing context, using a color that has been previously
//! translated to a driver-specific color (for example, via
//! DpyColorTranslate()).
//!
//! \return None.
//
//*****************************************************************************
#define GrContextForegroundSetTranslated(pContext, ulValue) \
        do                                                  \
        {                                                   \
            tContext *pC = pContext;                        \
            pC->ulForeground = ulValue;                     \
        }                                                   \
        while(0)

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pContext is a pointer to the drawing context to use.
//!
//! This function flushes any cached drawing operations.  For display drivers
//! that draw into a local frame buffer before writing to the actual display,
//! calling this function will cause the display to be updated to match the
//! contents of the local frame buffer.
//!
//! \return None.
//
//*****************************************************************************
#define GrFlush(pContext)                  \
        do                                 \
        {                                  \
            const tContext *pC = pContext; \
            DpyFlush(pC->pDisplay);        \
        }                                  \
        while(0)

#ifdef GRLIB_REMOVE_WIDE_FONT_SUPPORT
//*****************************************************************************
//
//! Gets the baseline of a font.
//!
//! \param pFont is a pointer to the font to query.
//!
//! This function determines the baseline position of a font.  The baseline is
//! the offset between the top of the font and the bottom of the capital
//! letters.  The only font data that exists below the baseline are the
//! descenders on some lower-case letters (such as ``y'').
//!
//! \return Returns the baseline of the font, in pixels.
//
//*****************************************************************************
#define GrFontBaselineGet(pFont) \
        ((pFont)->ucBaseline)

//*****************************************************************************
//
//! Gets the height of a font.
//!
//! \param pFont is a pointer to the font to query.
//!
//! This function determines the height of a font.  The height is the offset
//! between the top of the font and the bottom of the font, including any
//! ascenders and descenders.
//!
//! \return Returns the height of the font, in pixels.
//
//*****************************************************************************
#define GrFontHeightGet(pFont) \
        ((pFont)->ucHeight)

//*****************************************************************************
//
//! Gets the maximum width of a font.
//!
//! \param pFont is a pointer to the font to query.
//!
//! This function determines the maximum width of a font.  The maximum width is
//! the width of the widest individual character in the font.
//!
//! \return Returns the maximum width of the font, in pixels.
//
//*****************************************************************************
#define GrFontMaxWidthGet(pFont) \
        ((pFont)->ucMaxWidth)
#endif

//*****************************************************************************
//
//! Gets the number of colors in an image.
//!
//! \param pucImage is a pointer to the image to query.
//!
//! This function determines the number of colors in the palette of an image.
//! This is only valid for 4bpp and 8bpp images; 1bpp images do not contain a
//! palette.
//!
//! \return Returns the number of colors in the image.
//
//*****************************************************************************
#define GrImageColorsGet(pucImage)           \
        (((unsigned char *)pucImage)[5] + 1)

//*****************************************************************************
//
//! Gets the height of an image.
//!
//! \param pucImage is a pointer to the image to query.
//!
//! This function determines the height of an image in pixels.
//!
//! \return Returns the height of the image in pixels.
//
//*****************************************************************************
#define GrImageHeightGet(pucImage)          \
        (*(unsigned short *)(pucImage + 3))

//*****************************************************************************
//
//! Gets the width of an image.
//!
//! \param pucImage is a pointer to the image to query.
//!
//! This function determines the width of an image in pixels.
//!
//! \return Returns the width of the image in pixels.
//
//*****************************************************************************
#define GrImageWidthGet(pucImage)           \
        (*(unsigned short *)(pucImage + 1))

//*****************************************************************************
//
//! Determines the size of the buffer for a 1 BPP off-screen image.
//!
//! \param lWidth is the width of the image in pixels.
//! \param lHeight is the height of the image in pixels.
//!
//! This function determines the size of the memory buffer required to hold a
//! 1 BPP off-screen image of the specified geometry.
//!
//! \return Returns the number of bytes required by the image.
//
//*****************************************************************************
#define GrOffScreen1BPPSize(lWidth, lHeight) \
        (5 + (((lWidth + 7) / 8) * lHeight))

//*****************************************************************************
//
//! Determines the size of the buffer for a 4 BPP off-screen image.
//!
//! \param lWidth is the width of the image in pixels.
//! \param lHeight is the height of the image in pixels.
//!
//! This function determines the size of the memory buffer required to hold a
//! 4 BPP off-screen image of the specified geometry.
//!
//! \return Returns the number of bytes required by the image.
//
//*****************************************************************************
#define GrOffScreen4BPPSize(lWidth, lHeight)            \
        (6 + (16 * 3) + (((lWidth + 1) / 2) * lHeight))

//*****************************************************************************
//
//! Determines the size of the buffer for an 8 BPP off-screen image.
//!
//! \param lWidth is the width of the image in pixels.
//! \param lHeight is the height of the image in pixels.
//!
//! This function determines the size of the memory buffer required to hold an
//! 8 BPP off-screen image of the specified geometry.
//!
//! \return Returns the number of bytes required by the image.
//
//*****************************************************************************
#define GrOffScreen8BPPSize(lWidth, lHeight) \
        (6 + (256 * 3) + (lWidth * lHeight))

//*****************************************************************************
//
//! Draws a pixel.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//!
//! This function draws a pixel if it resides within the clipping region.
//!
//! \return None.
//
//*****************************************************************************
#define GrPixelDraw(pContext, lX, lY)                                 \
        do                                                            \
        {                                                             \
            const tContext *pC = pContext;                            \
            if((lX >= pC->sClipRegion.sXMin) &&                       \
               (lX <= pC->sClipRegion.sXMax) &&                       \
               (lY >= pC->sClipRegion.sYMin) &&                       \
               (lY <= pC->sClipRegion.sYMax))                         \
            {                                                         \
                DpyPixelDraw(pC->pDisplay, lX, lY, pC->ulForeground); \
            }                                                         \
        }                                                             \
        while(0)

//*****************************************************************************
//
//! Gets the baseline of a string.
//!
//! \param pContext is a pointer to the drawing context to query.
//!
//! This function determines the baseline position of a string.  The baseline
//! is the offset between the top of the string and the bottom of the capital
//! letters.  The only string data that exists below the baseline are the
//! descenders on some lower-case letters (such as ``y'').
//!
//! \return Returns the baseline of the string, in pixels.
//
//*****************************************************************************
#define GrStringBaselineGet(pContext)   \
        (GrFontBaselineGet((pContext)->pFont))

//*****************************************************************************
//
//! Draws a centered string.
//!
//! \param pContext is a pointer to the drawing context to use.
//! \param pcString is a pointer to the string to be drawn.
//! \param lLength is the number of characters from the string that should be
//! drawn on the screen.
//! \param lX is the X coordinate of the center of the string position on the
//! screen.
//! \param lY is the Y coordinate of the center of the string position on the
//! screen.
//! \param bOpaque is \b true if the background of each character should be
//! drawn and \b false if it should not (leaving the background as is).
//!
//! This function draws a string of test on the screen centered upon the
//! provided position.  The \e lLength parameter allows a portion of the
//! string to be examined without having to insert a NULL character at the
//! stopping point (which would not be possible if the string was located in
//! flash); specifying a length of -1 will cause the entire string to be
//! rendered (subject to clipping).
//!
//! \return None.
//
//*****************************************************************************
#define GrStringDrawCentered(pContext, pcString, lLength, lX, lY, bOpaque)  \
        do                                                                  \
        {                                                                   \
            const tContext *pC = pContext;                                  \
            const char *pcStr = pcString;                                   \
                                                                            \
            GrStringDraw(pC, pcStr, lLength,                                \
                         (lX) - (GrStringWidthGet(pC, pcStr, lLength) / 2), \
                         (lY) - (GrFontBaselineGet(pC->pFont) / 2), bOpaque); \
        }                                                                   \
        while(0)

//*****************************************************************************
//
//! Gets the height of a string.
//!
//! \param pContext is a pointer to the drawing context to query.
//!
//! This function determines the height of a string.  The height is the offset
//! between the top of the string and the bottom of the string, including any
//! ascenders and descenders.  Note that this will not account for the case
//! where the string in question does not have any characters that use
//! descenders but the font in the drawing context does contain characters with
//! descenders.
//!
//! \return Returns the height of the string, in pixels.
//
//*****************************************************************************
#define GrStringHeightGet(pContext)   \
        (GrFontHeightGet((pContext)->pFont))

//*****************************************************************************
//
//! Gets the maximum width of a character in a string.
//!
//! \param pContext is a pointer to the drawing context to query.
//!
//! This function determines the maximum width of a character in a string.  The
//! maximum width is the width of the widest individual character in the font
//! used to render the string, which may be wider than the widest character
//! that is used to render a particular string.
//!
//! \return Returns the maximum width of a character in a string, in pixels.
//
//*****************************************************************************
#define GrStringMaxWidthGet(pContext)   \
        (GrFontMaxWidthGet((pContext)->pFont))

//*****************************************************************************
//
// A set of color definitions.  This set is the subset of the X11 colors (from
// rgb.txt) that are supported by typical web browsers.
//
//*****************************************************************************
#define ClrAliceBlue            0x00F0F8FF
#define ClrAntiqueWhite         0x00FAEBD7
#define ClrAqua                 0x0000FFFF
#define ClrAquamarine           0x007FFFD4
#define ClrAzure                0x00F0FFFF
#define ClrBeige                0x00F5F5DC
#define ClrBisque               0x00FFE4C4
#define ClrBlack                0x00000000
#define ClrBlanchedAlmond       0x00FFEBCD
#define ClrBlue                 0x000000FF
#define ClrBlueViolet           0x008A2BE2
#define ClrBrown                0x00A52A2A
#define ClrBurlyWood            0x00DEB887
#define ClrCadetBlue            0x005F9EA0
#define ClrChartreuse           0x007FFF00
#define ClrChocolate            0x00D2691E
#define ClrCoral                0x00FF7F50
#define ClrCornflowerBlue       0x006495ED
#define ClrCornsilk             0x00FFF8DC
#define ClrCrimson              0x00DC143C
#define ClrCyan                 0x0000FFFF
#define ClrDarkBlue             0x0000008B
#define ClrDarkCyan             0x00008B8B
#define ClrDarkGoldenrod        0x00B8860B
#define ClrDarkGray             0x00A9A9A9
#define ClrDarkGreen            0x00006400
#define ClrDarkKhaki            0x00BDB76B
#define ClrDarkMagenta          0x008B008B
#define ClrDarkOliveGreen       0x00556B2F
#define ClrDarkOrange           0x00FF8C00
#define ClrDarkOrchid           0x009932CC
#define ClrDarkRed              0x008B0000
#define ClrDarkSalmon           0x00E9967A
#define ClrDarkSeaGreen         0x008FBC8F
#define ClrDarkSlateBlue        0x00483D8B
#define ClrDarkSlateGray        0x002F4F4F
#define ClrDarkTurquoise        0x0000CED1
#define ClrDarkViolet           0x009400D3
#define ClrDeepPink             0x00FF1493
#define ClrDeepSkyBlue          0x0000BFFF
#define ClrDimGray              0x00696969
#define ClrDodgerBlue           0x001E90FF
#define ClrFireBrick            0x00B22222
#define ClrFloralWhite          0x00FFFAF0
#define ClrForestGreen          0x00228B22
#define ClrFuchsia              0x00FF00FF
#define ClrGainsboro            0x00DCDCDC
#define ClrGhostWhite           0x00F8F8FF
#define ClrGold                 0x00FFD700
#define ClrGoldenrod            0x00DAA520
#define ClrGray                 0x00808080
#define ClrGreen                0x00008000
#define ClrGreenYellow          0x00ADFF2F
#define ClrHoneydew             0x00F0FFF0
#define ClrHotPink              0x00FF69B4
#define ClrIndianRed            0x00CD5C5C
#define ClrIndigo               0x004B0082
#define ClrIvory                0x00FFFFF0
#define ClrKhaki                0x00F0E68C
#define ClrLavender             0x00E6E6FA
#define ClrLavenderBlush        0x00FFF0F5
#define ClrLawnGreen            0x007CFC00
#define ClrLemonChiffon         0x00FFFACD
#define ClrLightBlue            0x00ADD8E6
#define ClrLightCoral           0x00F08080
#define ClrLightCyan            0x00E0FFFF
#define ClrLightGoldenrodYellow 0x00FAFAD2
#define ClrLightGreen           0x0090EE90
#define ClrLightGrey            0x00D3D3D3
#define ClrLightPink            0x00FFB6C1
#define ClrLightSalmon          0x00FFA07A
#define ClrLightSeaGreen        0x0020B2AA
#define ClrLightSkyBlue         0x0087CEFA
#define ClrLightSlateGray       0x00778899
#define ClrLightSteelBlue       0x00B0C4DE
#define ClrLightYellow          0x00FFFFE0
#define ClrLime                 0x0000FF00
#define ClrLimeGreen            0x0032CD32
#define ClrLinen                0x00FAF0E6
#define ClrMagenta              0x00FF00FF
#define ClrMaroon               0x00800000
#define ClrMediumAquamarine     0x0066CDAA
#define ClrMediumBlue           0x000000CD
#define ClrMediumOrchid         0x00BA55D3
#define ClrMediumPurple         0x009370DB
#define ClrMediumSeaGreen       0x003CB371
#define ClrMediumSlateBlue      0x007B68EE
#define ClrMediumSpringGreen    0x0000FA9A
#define ClrMediumTurquoise      0x0048D1CC
#define ClrMediumVioletRed      0x00C71585
#define ClrMidnightBlue         0x00191970
#define ClrMintCream            0x00F5FFFA
#define ClrMistyRose            0x00FFE4E1
#define ClrMoccasin             0x00FFE4B5
#define ClrNavajoWhite          0x00FFDEAD
#define ClrNavy                 0x00000080
#define ClrOldLace              0x00FDF5E6
#define ClrOlive                0x00808000
#define ClrOliveDrab            0x006B8E23
#define ClrOrange               0x00FFA500
#define ClrOrangeRed            0x00FF4500
#define ClrOrchid               0x00DA70D6
#define ClrPaleGoldenrod        0x00EEE8AA
#define ClrPaleGreen            0x0098FB98
#define ClrPaleTurquoise        0x00AFEEEE
#define ClrPaleVioletRed        0x00DB7093
#define ClrPapayaWhip           0x00FFEFD5
#define ClrPeachPuff            0x00FFDAB9
#define ClrPeru                 0x00CD853F
#define ClrPink                 0x00FFC0CB
#define ClrPlum                 0x00DDA0DD
#define ClrPowderBlue           0x00B0E0E6
#define ClrPurple               0x00800080
#define ClrRed                  0x00FF0000
#define ClrRosyBrown            0x00BC8F8F
#define ClrRoyalBlue            0x004169E1
#define ClrSaddleBrown          0x008B4513
#define ClrSalmon               0x00FA8072
#define ClrSandyBrown           0x00F4A460
#define ClrSeaGreen             0x002E8B57
#define ClrSeashell             0x00FFF5EE
#define ClrSienna               0x00A0522D
#define ClrSilver               0x00C0C0C0
#define ClrSkyBlue              0x0087CEEB
#define ClrSlateBlue            0x006A5ACD
#define ClrSlateGray            0x00708090
#define ClrSnow                 0x00FFFAFA
#define ClrSpringGreen          0x0000FF7F
#define ClrSteelBlue            0x004682B4
#define ClrTan                  0x00D2B48C
#define ClrTeal                 0x00008080
#define ClrThistle              0x00D8BFD8
#define ClrTomato               0x00FF6347
#define ClrTurquoise            0x0040E0D0
#define ClrViolet               0x00EE82EE
#define ClrWheat                0x00F5DEB3
#define ClrWhite                0x00FFFFFF
#define ClrWhiteSmoke           0x00F5F5F5
#define ClrYellow               0x00FFFF00
#define ClrYellowGreen          0x009ACD32

//*****************************************************************************
//
// Masks and shifts to aid in color format translation by drivers.
//
//*****************************************************************************
#define ClrRedMask              0x00FF0000
#define ClrRedShift             16
#define ClrGreenMask            0x0000FF00
#define ClrGreenShift           8
#define ClrBlueMask             0x000000FF
#define ClrBlueShift            0

//*****************************************************************************
//
// Prototypes for the predefined fonts in the graphics library.  ..Cm.. is the
// computer modern font, which is a serif font.  ..Cmsc.. is the computer
// modern small-caps font, which is also a serif font.  ..Cmss.. is the
// computer modern sans-serif font.
//
//*****************************************************************************
extern const tFont g_sFontCm12;
#define g_pFontCm12 (const tFont *)&g_sFontCm12
extern const tFont g_sFontCm12b;
#define g_pCm12b (const tFont *)&g_sFontCm12b
extern const tFont g_sFontCm12i;
#define g_pFontCm12i (const tFont *)&g_sFontCm12i
extern const tFont g_sFontCm14;
#define g_pFontCm14 (const tFont *)&g_sFontCm14
extern const tFont g_sFontCm14b;
#define g_pFontCm14b (const tFont *)&g_sFontCm14b
extern const tFont g_sFontCm14i;
#define g_pFontCm14i (const tFont *)&g_sFontCm14i
extern const tFont g_sFontCm16;
#define g_pFontCm16 (const tFont *)&g_sFontCm16
extern const tFont g_sFontCm16b;
#define g_pFontCm16b (const tFont *)&g_sFontCm16b
extern const tFont g_sFontCm16i;
#define g_pFontCm16i (const tFont *)&g_sFontCm16i
extern const tFont g_sFontCm18;
#define g_pFontCm18 (const tFont *)&g_sFontCm18
extern const tFont g_sFontCm18b;
#define g_pFontCm18b (const tFont *)&g_sFontCm18b
extern const tFont g_sFontCm18i;
#define g_pFontCm18i (const tFont *)&g_sFontCm18i
extern const tFont g_sFontCm20;
#define g_pFontCm20 (const tFont *)&g_sFontCm20
extern const tFont g_sFontCm20b;
#define g_pFontCm20b (const tFont *)&g_sFontCm20b
extern const tFont g_sFontCm20i;
#define g_pFontCm20i (const tFont *)&g_sFontCm20i
extern const tFont g_sFontCm22;
#define g_pFontCm22 (const tFont *)&g_sFontCm22
extern const tFont g_sFontCm22b;
#define g_pFontCm22b (const tFont *)&g_sFontCm22b
extern const tFont g_sFontCm22i;
#define g_pFontCm22i (const tFont *)&g_sFontCm22i
extern const tFont g_sFontCm24;
#define g_pFontCm24 (const tFont *)&g_sFontCm24
extern const tFont g_sFontCm24b;
#define g_pFontCm24b (const tFont *)&g_sFontCm24b
extern const tFont g_sFontCm24i;
#define g_pFontCm24i (const tFont *)&g_sFontCm24i
extern const tFont g_sFontCm26;
#define g_pFontCm26 (const tFont *)&g_sFontCm26
extern const tFont g_sFontCm26b;
#define g_pFontCm26b (const tFont *)&g_sFontCm26b
extern const tFont g_sFontCm26i;
#define g_pFontCm26i (const tFont *)&g_sFontCm26i
extern const tFont g_sFontCm28;
#define g_pFontCm28 (const tFont *)&g_sFontCm28
extern const tFont g_sFontCm28b;
#define g_pFontCm28b (const tFont *)&g_sFontCm28b
extern const tFont g_sFontCm28i;
#define g_pFontCm28i (const tFont *)&g_sFontCm28i
extern const tFont g_sFontCm30;
#define g_pFontCm30 (const tFont *)&g_sFontCm30
extern const tFont g_sFontCm30b;
#define g_pFontCm30b (const tFont *)&g_sFontCm30b
extern const tFont g_sFontCm30i;
#define g_pFontCm30i (const tFont *)&g_sFontCm30i
extern const tFont g_sFontCm32;
#define g_pFontCm32 (const tFont *)&g_sFontCm32
extern const tFont g_sFontCm32b;
#define g_pFontCm32b (const tFont *)&g_sFontCm32b
extern const tFont g_sFontCm32i;
#define g_pFontCm32i (const tFont *)&g_sFontCm32i
extern const tFont g_sFontCm34;
#define g_pFontCm34 (const tFont *)&g_sFontCm34
extern const tFont g_sFontCm34b;
#define g_pFontCm34b (const tFont *)&g_sFontCm34b
extern const tFont g_sFontCm34i;
#define g_pFontCm34i (const tFont *)&g_sFontCm34i
extern const tFont g_sFontCm36;
#define g_pFontCm36 (const tFont *)&g_sFontCm36
extern const tFont g_sFontCm36b;
#define g_pFontCm36b (const tFont *)&g_sFontCm36b
extern const tFont g_sFontCm36i;
#define g_pFontCm36i (const tFont *)&g_sFontCm36i
extern const tFont g_sFontCm38;
#define g_pFontCm38 (const tFont *)&g_sFontCm38
extern const tFont g_sFontCm38b;
#define g_pFontCm38b (const tFont *)&g_sFontCm38b
extern const tFont g_sFontCm38i;
#define g_pFontCm38i (const tFont *)&g_sFontCm38i
extern const tFont g_sFontCm40;
#define g_pFontCm40 (const tFont *)&g_sFontCm40
extern const tFont g_sFontCm40b;
#define g_pFontCm40b (const tFont *)&g_sFontCm40b
extern const tFont g_sFontCm40i;
#define g_pFontCm40i (const tFont *)&g_sFontCm40i
extern const tFont g_sFontCm42;
#define g_pFontCm42 (const tFont *)&g_sFontCm42
extern const tFont g_sFontCm42b;
#define g_pFontCm42b (const tFont *)&g_sFontCm42b
extern const tFont g_sFontCm42i;
#define g_pFontCm42i (const tFont *)&g_sFontCm42i
extern const tFont g_sFontCm44;
#define g_pFontCm44 (const tFont *)&g_sFontCm44
extern const tFont g_sFontCm44b;
#define g_pFontCm44b (const tFont *)&g_sFontCm44b
extern const tFont g_sFontCm44i;
#define g_pFontCm44i (const tFont *)&g_sFontCm44i
extern const tFont g_sFontCm46;
#define g_pFontCm46 (const tFont *)&g_sFontCm46
extern const tFont g_sFontCm46b;
#define g_pFontCm46b (const tFont *)&g_sFontCm46b
extern const tFont g_sFontCm46i;
#define g_pFontCm46i (const tFont *)&g_sFontCm46i
extern const tFont g_sFontCm48;
#define g_pFontCm48 (const tFont *)&g_sFontCm48
extern const tFont g_sFontCm48b;
#define g_pFontCm48b (const tFont *)&g_sFontCm48b
extern const tFont g_sFontCm48i;
#define g_pFontCm48i (const tFont *)&g_sFontCm48i
extern const tFont g_sFontCmsc12;
#define g_pFontCmsc12 (const tFont *)&g_sFontCmsc12
extern const tFont g_sFontCmsc14;
#define g_pFontCmsc14 (const tFont *)&g_sFontCmsc14
extern const tFont g_sFontCmsc16;
#define g_pFontCmsc16 (const tFont *)&g_sFontCmsc16
extern const tFont g_sFontCmsc18;
#define g_pFontCmsc18 (const tFont *)&g_sFontCmsc18
extern const tFont g_sFontCmsc20;
#define g_pFontCmsc20 (const tFont *)&g_sFontCmsc20
extern const tFont g_sFontCmsc22;
#define g_pFontCmsc22 (const tFont *)&g_sFontCmsc22
extern const tFont g_sFontCmsc24;
#define g_pFontCmsc24 (const tFont *)&g_sFontCmsc24
extern const tFont g_sFontCmsc26;
#define g_pFontCmsc26 (const tFont *)&g_sFontCmsc26
extern const tFont g_sFontCmsc28;
#define g_pFontCmsc28 (const tFont *)&g_sFontCmsc28
extern const tFont g_sFontCmsc30;
#define g_pFontCmsc30 (const tFont *)&g_sFontCmsc30
extern const tFont g_sFontCmsc32;
#define g_pFontCmsc32 (const tFont *)&g_sFontCmsc32
extern const tFont g_sFontCmsc34;
#define g_pFontCmsc34 (const tFont *)&g_sFontCmsc34
extern const tFont g_sFontCmsc36;
#define g_pFontCmsc36 (const tFont *)&g_sFontCmsc36
extern const tFont g_sFontCmsc38;
#define g_pFontCmsc38 (const tFont *)&g_sFontCmsc38
extern const tFont g_sFontCmsc40;
#define g_pFontCmsc40 (const tFont *)&g_sFontCmsc40
extern const tFont g_sFontCmsc42;
#define g_pFontCmsc42 (const tFont *)&g_sFontCmsc42
extern const tFont g_sFontCmsc44;
#define g_pFontCmsc44 (const tFont *)&g_sFontCmsc44
extern const tFont g_sFontCmsc46;
#define g_pFontCmsc46 (const tFont *)&g_sFontCmsc46
extern const tFont g_sFontCmsc48;
#define g_pFontCmsc48 (const tFont *)&g_sFontCmsc48
extern const tFont g_sFontCmss12;
#define g_pFontCmss12 (const tFont *)&g_sFontCmss12
extern const tFont g_sFontCmss12b;
#define g_pFontCmss12b (const tFont *)&g_sFontCmss12b
extern const tFont g_sFontCmss12i;
#define g_pFontCmss12i (const tFont *)&g_sFontCmss12i
extern const tFont g_sFontCmss14;
#define g_pFontCmss14 (const tFont *)&g_sFontCmss14
extern const tFont g_sFontCmss14b;
#define g_pFontCmss14b (const tFont *)&g_sFontCmss14b
extern const tFont g_sFontCmss14i;
#define g_pFontCmss14i (const tFont *)&g_sFontCmss14i
extern const tFont g_sFontCmss16;
#define g_pFontCmss16 (const tFont *)&g_sFontCmss16
extern const tFont g_sFontCmss16b;
#define g_pFontCmss16b (const tFont *)&g_sFontCmss16b
extern const tFont g_sFontCmss16i;
#define g_pFontCmss16i (const tFont *)&g_sFontCmss16i
extern const tFont g_sFontCmss18;
#define g_pFontCmss18 (const tFont *)&g_sFontCmss18
extern const tFont g_sFontCmss18b;
#define g_pFontCmss18b (const tFont *)&g_sFontCmss18b
extern const tFont g_sFontCmss18i;
#define g_pFontCmss18i (const tFont *)&g_sFontCmss18i
extern const tFont g_sFontCmss20;
#define g_pFontCmss20 (const tFont *)&g_sFontCmss20
extern const tFont g_sFontCmss20b;
#define g_pFontCmss20b (const tFont *)&g_sFontCmss20b
extern const tFont g_sFontCmss20i;
#define g_pFontCmss20i (const tFont *)&g_sFontCmss20i
extern const tFont g_sFontCmss22;
#define g_pFontCmss22 (const tFont *)&g_sFontCmss22
extern const tFont g_sFontCmss22b;
#define g_pFontCmss22b (const tFont *)&g_sFontCmss22b
extern const tFont g_sFontCmss22i;
#define g_pFontCmss22i (const tFont *)&g_sFontCmss22i
extern const tFont g_sFontCmss24;
#define g_pFontCmss24 (const tFont *)&g_sFontCmss24
extern const tFont g_sFontCmss24b;
#define g_pFontCmss24b (const tFont *)&g_sFontCmss24b
extern const tFont g_sFontCmss24i;
#define g_pFontCmss24i (const tFont *)&g_sFontCmss24i
extern const tFont g_sFontCmss26;
#define g_pFontCmss26 (const tFont *)&g_sFontCmss26
extern const tFont g_sFontCmss26b;
#define g_pFontCmss26b (const tFont *)&g_sFontCmss26b
extern const tFont g_sFontCmss26i;
#define g_pFontCmss26i (const tFont *)&g_sFontCmss26i
extern const tFont g_sFontCmss28;
#define g_pFontCmss28 (const tFont *)&g_sFontCmss28
extern const tFont g_sFontCmss28b;
#define g_pFontCmss28b (const tFont *)&g_sFontCmss28b
extern const tFont g_sFontCmss28i;
#define g_pFontCmss28i (const tFont *)&g_sFontCmss28i
extern const tFont g_sFontCmss30;
#define g_pFontCmss30 (const tFont *)&g_sFontCmss30
extern const tFont g_sFontCmss30b;
#define g_pFontCmss30b (const tFont *)&g_sFontCmss30b
extern const tFont g_sFontCmss30i;
#define g_pFontCmss30i (const tFont *)&g_sFontCmss30i
extern const tFont g_sFontCmss32;
#define g_pFontCmss32 (const tFont *)&g_sFontCmss32
extern const tFont g_sFontCmss32b;
#define g_pFontCmss32b (const tFont *)&g_sFontCmss32b
extern const tFont g_sFontCmss32i;
#define g_pFontCmss32i (const tFont *)&g_sFontCmss32i
extern const tFont g_sFontCmss34;
#define g_pFontCmss34 (const tFont *)&g_sFontCmss34
extern const tFont g_sFontCmss34b;
#define g_pFontCmss34b (const tFont *)&g_sFontCmss34b
extern const tFont g_sFontCmss34i;
#define g_pFontCmss34i (const tFont *)&g_sFontCmss34i
extern const tFont g_sFontCmss36;
#define g_pFontCmss36 (const tFont *)&g_sFontCmss36
extern const tFont g_sFontCmss36b;
#define g_pFontCmss36b (const tFont *)&g_sFontCmss36b
extern const tFont g_sFontCmss36i;
#define g_pFontCmss36i (const tFont *)&g_sFontCmss36i
extern const tFont g_sFontCmss38;
#define g_pFontCmss38 (const tFont *)&g_sFontCmss38
extern const tFont g_sFontCmss38b;
#define g_pFontCmss38b (const tFont *)&g_sFontCmss38b
extern const tFont g_sFontCmss38i;
#define g_pFontCmss38i (const tFont *)&g_sFontCmss38i
extern const tFont g_sFontCmss40;
#define g_pFontCmss40 (const tFont *)&g_sFontCmss40
extern const tFont g_sFontCmss40b;
#define g_pFontCmss40b (const tFont *)&g_sFontCmss40b
extern const tFont g_sFontCmss40i;
#define g_pFontCmss40i (const tFont *)&g_sFontCmss40i
extern const tFont g_sFontCmss42;
#define g_pFontCmss42 (const tFont *)&g_sFontCmss42
extern const tFont g_sFontCmss42b;
#define g_pFontCmss42b (const tFont *)&g_sFontCmss42b
extern const tFont g_sFontCmss42i;
#define g_pFontCmss42i (const tFont *)&g_sFontCmss42i
extern const tFont g_sFontCmss44;
#define g_pFontCmss44 (const tFont *)&g_sFontCmss44
extern const tFont g_sFontCmss44b;
#define g_pFontCmss44b (const tFont *)&g_sFontCmss44b
extern const tFont g_sFontCmss44i;
#define g_pFontCmss44i (const tFont *)&g_sFontCmss44i
extern const tFont g_sFontCmss46;
#define g_pFontCmss46 (const tFont *)&g_sFontCmss46
extern const tFont g_sFontCmss46b;
#define g_pFontCmss46b (const tFont *)&g_sFontCmss46b
extern const tFont g_sFontCmss46i;
#define g_pFontCmss46i (const tFont *)&g_sFontCmss46i
extern const tFont g_sFontCmss48;
#define g_pFontCmss48 (const tFont *)&g_sFontCmss48
extern const tFont g_sFontCmss48b;
#define g_pFontCmss48b (const tFont *)&g_sFontCmss48b
extern const tFont g_sFontCmss48i;
#define g_pFontCmss48i (const tFont *)&g_sFontCmss48i
extern const tFont g_sFontCmtt12;
#define g_pFontCmtt12 (const tFont *)&g_sFontCmtt12
extern const tFont g_sFontCmtt14;
#define g_pFontCmtt14 (const tFont *)&g_sFontCmtt14
extern const tFont g_sFontCmtt16;
#define g_pFontCmtt16 (const tFont *)&g_sFontCmtt16
extern const tFont g_sFontCmtt18;
#define g_pFontCmtt18 (const tFont *)&g_sFontCmtt18
extern const tFont g_sFontCmtt20;
#define g_pFontCmtt20 (const tFont *)&g_sFontCmtt20
extern const tFont g_sFontCmtt22;
#define g_pFontCmtt22 (const tFont *)&g_sFontCmtt22
extern const tFont g_sFontCmtt24;
#define g_pFontCmtt24 (const tFont *)&g_sFontCmtt24
extern const tFont g_sFontCmtt26;
#define g_pFontCmtt26 (const tFont *)&g_sFontCmtt26
extern const tFont g_sFontCmtt28;
#define g_pFontCmtt28 (const tFont *)&g_sFontCmtt28
extern const tFont g_sFontCmtt30;
#define g_pFontCmtt30 (const tFont *)&g_sFontCmtt30
extern const tFont g_sFontCmtt32;
#define g_pFontCmtt32 (const tFont *)&g_sFontCmtt32
extern const tFont g_sFontCmtt34;
#define g_pFontCmtt34 (const tFont *)&g_sFontCmtt34
extern const tFont g_sFontCmtt36;
#define g_pFontCmtt36 (const tFont *)&g_sFontCmtt36
extern const tFont g_sFontCmtt38;
#define g_pFontCmtt38 (const tFont *)&g_sFontCmtt38
extern const tFont g_sFontCmtt40;
#define g_pFontCmtt40 (const tFont *)&g_sFontCmtt40
extern const tFont g_sFontCmtt42;
#define g_pFontCmtt42 (const tFont *)&g_sFontCmtt42
extern const tFont g_sFontCmtt44;
#define g_pFontCmtt44 (const tFont *)&g_sFontCmtt44
extern const tFont g_sFontCmtt46;
#define g_pFontCmtt46 (const tFont *)&g_sFontCmtt46
extern const tFont g_sFontCmtt48;
#define g_pFontCmtt48 (const tFont *)&g_sFontCmtt48
extern const tFont g_sFontFixed6x8;
#define g_pFontFixed6x8 (const tFont *)&g_sFontFixed6x8

//*****************************************************************************
//
// Language identifiers supported by the string table processing functions.
//
//*****************************************************************************
#define GrLangZhPRC             0x0804      // Chinese (PRC)
#define GrLangZhTW              0x0404      // Chinese (Taiwan)
#define GrLangEnUS              0x0409      // English (United States)
#define GrLangEnUK              0x0809      // English (United Kingdom)
#define GrLangEnAUS             0x0C09      // English (Australia)
#define GrLangEnCA              0x1009      // English (Canada)
#define GrLangEnNZ              0x1409      // English (New Zealand)
#define GrLangFr                0x040C      // French (Standard)
#define GrLangDe                0x0407      // German (Standard)
#define GrLangHi                0x0439      // Hindi
#define GrLangIt                0x0410      // Italian (Standard)
#define GrLangJp                0x0411      // Japanese
#define GrLangKo                0x0412      // Korean
#define GrLangEsMX              0x080A      // Spanish (Mexico)
#define GrLangEsSP              0x0C0A      // Spanish (Spain)
#define GrLangSwKE              0x0441      // Swahili (Kenya)
#define GrLangUrIN              0x0820      // Urdu (India)
#define GrLangUrPK              0x0420      // Urdu (Pakistan)

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pDisplay is the pointer to the display driver structure for the
//! display to operate upon.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
#define DpyColorTranslate(pDisplay, ulValue)                                \
        ((pDisplay)->pfnColorTranslate((pDisplay)->pvDisplayData, ulValue))

//*****************************************************************************
//
//! Flushes cached drawing operations.
//!
//! \param pDisplay is the pointer to the display driver structure for the
//! display to operate upon.
//!
//! This function flushes any cached drawing operations on a display.
//!
//! \return None.
//
//*****************************************************************************
#define DpyFlush(pDisplay)                   \
        do                                   \
        {                                    \
            const tDisplay *pD = pDisplay;   \
            pD->pfnFlush(pD->pvDisplayData); \
        }                                    \
        while(0)

//*****************************************************************************
//
//! Gets the height of the display.
//!
//! \param pDisplay is a pointer to the display driver structure for the
//! display to query.
//!
//! This function determines the height of the display.
//!
//! \return Returns the height of the display in pixels.
//
//*****************************************************************************
#define DpyHeightGet(pDisplay) \
        ((pDisplay)->usHeight)

//*****************************************************************************
//
//! Draws a horizontal line on a display.
//!
//! \param pDisplay is the pointer to the display driver structure for the
//! display to operate upon.
//! \param lX1 is the starting X coordinate of the line.
//! \param lX2 is the ending X coordinate of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color to draw the line.
//!
//! This function draws a horizontal line on a display.  This assumes that
//! clipping has already been performed, and that both end points of the line
//! are within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
#define DpyLineDrawH(pDisplay, lX1, lX2, lY, ulValue)                   \
        do                                                              \
        {                                                               \
            const tDisplay *pD = pDisplay;                              \
            pD->pfnLineDrawH(pD->pvDisplayData, lX1, lX2, lY, ulValue); \
        }                                                               \
        while(0)

//*****************************************************************************
//
//! Draws a vertical line on a display.
//!
//! \param pDisplay is the pointer to the display driver structure for the
//! display to operate upon.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the starting Y coordinate of the line.
//! \param lY2 is the ending Y coordinate of the line.
//! \param ulValue is the color to draw the line.
//!
//! This function draws a vertical line on a display.  This assumes that
//! clipping has already been performed, and that both end points of the line
//! are within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
#define DpyLineDrawV(pDisplay, lX, lY1, lY2, ulValue)                   \
        do                                                              \
        {                                                               \
            const tDisplay *pD = pDisplay;                              \
            pD->pfnLineDrawV(pD->pvDisplayData, lX, lY1, lY2, ulValue); \
        }                                                               \
        while(0)

//*****************************************************************************
//
//! Draws a pixel on a display.
//!
//! \param pDisplay is the pointer to the display driver structure for the
//! display to operate upon.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color to draw the pixel.
//!
//! This function draws a pixel on a display.  This assumes that clipping has
//! already been performed.
//!
//! \return None.
//
//*****************************************************************************
#define DpyPixelDraw(pDisplay, lX, lY, ulValue)                   \
        do                                                        \
        {                                                         \
            const tDisplay *pD = pDisplay;                        \
            pD->pfnPixelDraw(pD->pvDisplayData, lX, lY, ulValue); \
        }                                                         \
        while(0)

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on a display.
//!
//! \param pDisplay is the pointer to the display driver structure for the
//! display to operate upon.
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on a display, using the
//! supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
#define DpyPixelDrawMultiple(pDisplay, lX, lY, lX0, lCount, lBPP, pucData,   \
                             pucPalette)                                     \
        do                                                                   \
        {                                                                    \
            const tDisplay *pD = pDisplay;                                   \
            pD->pfnPixelDrawMultiple(pD->pvDisplayData, lX, lY, lX0, lCount, \
                                     lBPP, pucData, pucPalette);             \
        }                                                                    \
        while(0)

//*****************************************************************************
//
//! Fills a rectangle on a display.
//!
//! \param pDisplay is the pointer to the display driver structure for the
//! display to operate upon.
//! \param pRect is a pointer to the structure describing the rectangle to
//! fill.
//! \param ulValue is the color to fill the rectangle.
//!
//! This function fills a rectangle on the display.  This assumes that clipping
//! has already been performed, and that all sides of the rectangle are within
//! the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
#define DpyRectFill(pDisplay, pRect, ulValue)                   \
        do                                                      \
        {                                                       \
            const tDisplay *pD = pDisplay;                      \
            pD->pfnRectFill(pD->pvDisplayData, pRect, ulValue); \
        }                                                       \
        while(0)

//*****************************************************************************
//
//! Gets the width of the display.
//!
//! \param pDisplay is a pointer to the display driver structure for the
//! display to query.
//!
//! This function determines the width of the display.
//!
//! \return Returns the width of the display in pixels.
//
//*****************************************************************************
#define DpyWidthGet(pDisplay) \
        ((pDisplay)->usWidth)

//*****************************************************************************
//
//! Determines if a point lies within a given rectangle.
//!
//! \param pRect is a pointer to the rectangle which the point is to be checked
//! against.
//! \param lX is the X coordinate of the point to be checked.
//! \param lY is the Y coordinate of the point to be checked.
//!
//! This function determines whether point (lX, lY) lies within the rectangle
//! described by \e pRect.
//!
//! \return Returns 1 if the point is within the rectangle or 0 otherwise.
//
//*****************************************************************************
#define GrRectContainsPoint(pRect, lX, lY)                                  \
            ((((lX) >= (pRect)->sXMin) && ((lX) <= (pRect)->sXMax) &&       \
              ((lY) >= (pRect)->sYMin) && ((lY) <= (pRect)->sYMax)) ? 1 : 0)

//*****************************************************************************
//
// Counts the number of zeros at the start of a word.
//
// \param x is the word whose leading zeros are to be counted.
//
// This macro uses compiler-specific constructs to perform an inline
// insertion of the "clz" instruction, which counts the leading zeros
// directly.
//
// \return Returns the number of leading 0 bits in the word provided.
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
// Prototypes for the graphics library functions.
//
//*****************************************************************************
#ifndef GRLIB_REMOVE_WIDE_FONT_SUPPORT
extern void GrLibInit(const tGrLibDefaults *pDefaults);
#endif
extern void GrCircleDraw(const tContext *pContext, long lX, long lY,
                         long lRadius);
extern void GrCircleFill(const tContext *pContext, long lX, long lY,
                         long lRadius);
extern void GrContextClipRegionSet(tContext *pContext, tRectangle *pRect);
extern void GrContextInit(tContext *pContext, const tDisplay *pDisplay);
extern void GrImageDraw(const tContext *pContext,
                        const unsigned char *pucImage, long lX, long lY);
extern void GrTransparentImageDraw(const tContext *pContext,
                                   const unsigned char *pucImage,
                                   long lX, long lY,
                                   unsigned long ulTransparent);
extern void GrLineDraw(const tContext *pContext, long lX1, long lY1, long lX2,
                       long lY2);
extern void GrLineDrawH(const tContext *pContext, long lX1, long lX2, long lY);
extern void GrLineDrawV(const tContext *pContext, long lX, long lY1, long lY2);
extern void GrOffScreen1BPPInit(tDisplay *pDisplay, unsigned char *pucImage,
                                long lWidth, long lHeight);
extern void GrOffScreen4BPPInit(tDisplay *pDisplay, unsigned char *pucImage,
                                long lWidth, long lHeight);
extern void GrOffScreen4BPPPaletteSet(tDisplay *pDisplay,
                                      unsigned long *pulPalette,
                                      unsigned long ulOffset,
                                      unsigned long ulCount);
extern void GrOffScreen8BPPInit(tDisplay *pDisplay, unsigned char *pucImage,
                                long lWidth, long lHeight);
extern void GrOffScreen8BPPPaletteSet(tDisplay *pDisplay,
                                      unsigned long *pulPalette,
                                      unsigned long ulOffset,
                                      unsigned long ulCount);
extern void GrRectDraw(const tContext *pContext, const tRectangle *pRect);
extern void GrRectFill(const tContext *pContext, const tRectangle *pRect);
extern void GrStringDraw(const tContext *pContext, const char *pcString,
                         long lLength, long lX, long lY,
                         unsigned long bOpaque);
extern long GrStringWidthGet(const tContext *pContext, const char *pcString,
                             long lLength);
extern void GrStringTableSet(const void *pvTable);
unsigned long GrStringLanguageSet(unsigned short usLangID);
unsigned long GrStringGet(long lIndex, char *pcData, unsigned long ulSize);
extern long GrRectOverlapCheck(tRectangle *psRect1, tRectangle *psRect2);
extern long GrRectIntersectGet(tRectangle *psRect1, tRectangle *psRect2,
                               tRectangle *psIntersect);

#ifndef GRLIB_REMOVE_WIDE_FONT_SUPPORT
//*****************************************************************************
//
// Font parsing functions.
//
//*****************************************************************************
extern void GrFontInfoGet(const tFont *pFont, unsigned char *pucFormat,
                          unsigned char *pucMaxWidth, unsigned char *pucHeight,
                          unsigned char *pucBaseline);
extern unsigned long GrFontMaxWidthGet(const tFont *pFont);
extern unsigned long GrFontHeightGet(const tFont *pFont);
extern unsigned long GrFontBaselineGet(const tFont *pFont);
extern const unsigned char *GrFontGlyphDataGet(const tFont *pFont,
                                               unsigned long ulCodePoint,
                                               unsigned char *pucWidth);
extern unsigned short GrFontCodepageGet(const tFont *pFont);
extern unsigned short GrFontNumBlocksGet(const tFont *pFont);
extern unsigned long GrFontBlockCodepointsGet(const tFont *pFont,
                                           unsigned short usBlockIndex,
                                           unsigned long *pulStart);

//*****************************************************************************
//
// Low level string and font glyph rendering function.
//
//*****************************************************************************
void GrFontGlyphRender(const tContext *pContext, const unsigned char *pucData,
                       long lX, long lY, unsigned long bCompressed,
                       unsigned long bOpaque);
void
GrDefaultStringRenderer(const tContext *pContext, const char *pcString,
                        long lLength, long lX, long lY, unsigned long bOpaque);

//*****************************************************************************
//
// Codepage translation functions.
//
//*****************************************************************************
extern void GrCodepageMapTableSet(tContext *pContext,
                                  tCodePointMap *pCodePointMapTable,
                                  unsigned char ucNumMaps);
extern long GrStringCodepageSet(tContext *pContext,
                                unsigned short usCodepage);
extern unsigned long GrStringRendererSet(tContext *pContext,
                                         tStringRenderer pfnRenderer);
extern unsigned long GrStringNumGlyphsGet(const tContext *pContext,
                                          const char *pcString);
extern unsigned long GrStringNextCharGet(const tContext *pContext,
                                         const char *pcString,
                                         unsigned long ulCount,
                                         unsigned long *pulSkip);

extern unsigned long GrMapUnicode_Unicode(const char *pcSrcChar,
                                          unsigned long ulCount,
                                          unsigned long *pulSkip);
extern unsigned long GrMapUTF8_Unicode(const char *pcSrcChar,
                                       unsigned long ulCount,
                                       unsigned long *pulSkip);
extern unsigned long GrMapUTF16LE_Unicode(const char *pcSrcChar,
                                          unsigned long ulCount,
                                          unsigned long *pulSkip);
extern unsigned long GrMapUTF16BE_Unicode(const char *pcSrcChar,
                                          unsigned long ulCount,
                                          unsigned long *pulSkip);
extern unsigned long GrMapISO8859_1_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_2_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_3_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_4_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_5_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_6_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_7_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_8_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_9_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_10_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_11_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_13_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_14_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_15_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapISO8859_16_Unicode(const char *pcSrcChar,
                                            unsigned long ulCount,
                                            unsigned long *pulSkip);
extern unsigned long GrMapWIN1250_Unicode(const char *pcSrcChar,
                                          unsigned long ulCount,
                                          unsigned long *pulSkip);
extern unsigned long GrMapWIN1251_Unicode(const char *pcSrcChar,
                                          unsigned long ulCount,
                                          unsigned long *pulSkip);
extern unsigned long GrMapWIN1252_Unicode(const char *pcSrcChar,
                                          unsigned long ulCount,
                                          unsigned long *pulSkip);
extern unsigned long GrMapWIN1253_Unicode(const char *pcSrcChar,
                                          unsigned long ulCount,
                                          unsigned long *pulSkip);
extern unsigned long GrMapWIN1254_Unicode(const char *pcSrcChar,
                                          unsigned long ulCount,
                                          unsigned long *pulSkip);

//*****************************************************************************
//
// A helpful #define that maps any 8 bit source codepage to itself.  This can
// be used for any 8 bit source encoding when the font being used is encoded
// using the same codepage, for example ISO8859-5 text with an ISO8859-5 font.
// It just so happens that the ISO8859-1 to Unicode mapping function provides
// exactly what is required here since there is a 1:1 mapping of ISO8859-1
// codepoints to the first 256 Unicode characters.
//
//*****************************************************************************
#define GrMap8BitIdentity GrMapISO8859_1_Unicode

#endif

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

#endif // __GRLIB_H__
