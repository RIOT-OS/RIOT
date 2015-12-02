/* Linker script for Silicon Labs EFM32WG devices */
/*                                                                  */
/* This file is subject to the license terms as defined in ARM's    */
/* CMSIS END USER LICENSE AGREEMENT.pdf, governing the use of       */
/* Example Code.                                                    */
/*                                                                  */
/* Silicon Laboratories, Inc. 2015                                  */
/*                                                                  */
/* Version 4.0.0 */
/*                                                                  */

MEMORY
{
  rom (rx) : ORIGIN = 0x00000000, LENGTH = 262144
  ram (rwx)  : ORIGIN = 0x20000000, LENGTH = 32768
}

INCLUDE cortexm_base.ld
