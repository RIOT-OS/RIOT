//*****************************************************************************
//
// ptpdlib.c - ptpd Library Abstraction Layer.
//
// Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
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
// Include the library source code header files next.
//
//*****************************************************************************
#include "utils/ptpdlib.h"

//*****************************************************************************
//
// Include ptpd library code.
//
//*****************************************************************************
#include "ptpd-1rc1/src/arith.c"
#include "ptpd-1rc1/src/bmc.c"
#include "ptpd-1rc1/src/protocol.c"

//*****************************************************************************
//
// Include ptpd porting layer code.
//
//*****************************************************************************
#include "ptpd-1rc1/src/dep-lmi/ptpd_timer.c"
#include "ptpd-1rc1/src/dep-lmi/ptpd_servo.c"
#include "ptpd-1rc1/src/dep-lmi/ptpd_msg.c"
#include "ptpd-1rc1/src/dep-lmi/ptpd_net.c"
