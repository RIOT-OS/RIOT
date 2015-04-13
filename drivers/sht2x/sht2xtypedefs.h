#ifndef TYPEDEFS_H
#define TYPEDEFS_H
//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT2x Sample Code (V1.2)
// File      :  typedefs.h
// Author    :  MST
// Controller:  NEC V850/SG3 (uPD70F3740)
// Compiler  :  IAR compiler for V850 (3.50A)
// Brief     :  Definitions of typedefs for good readability and portability
//==============================================================================

//---------- Defines -----------------------------------------------------------
//Processor endian system
//#define BIG ENDIAN   //e.g. Motorola (not tested at this time)
//#define LITTLE_ENDIAN  //e.g. PIC, 8051, NEC V850
//==============================================================================
// basic types: making the size of types clear
//==============================================================================
typedef unsigned char   u8t;      ///< range: 0 .. 255
typedef signed char     i8t;      ///< range: -128 .. +127
																    	
typedef unsigned short  u16t;     ///< range: 0 .. 65535
typedef signed short    i16t;     ///< range: -32768 .. +32767
																    	
typedef unsigned long   u32t;     ///< range: 0 .. 4'294'967'295
typedef signed long     i32t;     ///< range: -2'147'483'648 .. +2'147'483'647
																    	
typedef float           ft;       ///< range: +-1.18E-38 .. +-3.39E+38
typedef double          dt;      ///< range:            .. +-1.79E+308

//typedef bool            bt;       ///< values: 0, 1 (real bool used)

typedef union {
  u16t u16;               // element specifier for accessing whole u16
  i16t i16;               // element specifier for accessing whole i16
  struct {
    #ifdef LITTLE_ENDIAN  // Byte-order is little endian
    u8t u8L;              // element specifier for accessing low u8
    u8t u8H;              // element specifier for accessing high u8
    #else                 // Byte-order is big endian
    u8t u8H;              // element specifier for accessing low u8
    u8t u8L;              // element specifier for accessing high u8
    #endif
  } s16;                  // element spec. for acc. struct with low or high u8
} nt16;

typedef union {
  u32t u32;               // element specifier for accessing whole u32
  i32t i32;               // element specifier for accessing whole i32
 struct {
    #ifdef LITTLE_ENDIAN  // Byte-order is little endian
    u16t u16L;            // element specifier for accessing low u16
    u16t u16H;            // element specifier for accessing high u16
    #else                 // Byte-order is big endian
    u16t u16H;            // element specifier for accessing low u16
    u16t u16L;            // element specifier for accessing high u16
    #endif
  } s32;                  // element spec. for acc. struct with low or high u16
} nt32;
#endif