Boot Loader

The boot loader is a small piece of code that can be programmed at the
beginning of flash to act as an application loader as well as an update
mechanism for an application running on a Stellaris microcontroller, utilizing
either UART0, I2C0, SSI0, Ethernet or USB.  The capabilities of the boot loader
are configured via the bl_config.h include file (which is located in the
application directory, not in the boot loader source directory).

-------------------------------------------------------------------------------

Copyright (c) 2007-2012 Texas Instruments Incorporated.  All rights reserved.
Software License Agreement

Texas Instruments (TI) is supplying this software for use solely and
exclusively on TI's microcontroller products. The software is owned by
TI and/or its suppliers, and is protected under applicable copyright
laws. You may not combine this software with "viral" open-source
software in order to form a larger program.

THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, FOR ANY REASON WHATSOEVER.

This is part of revision 9453 of the Stellaris Firmware Development Package.
