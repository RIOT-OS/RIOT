/**
 * @addtogroup      cpu_cc13x2
 * @{
 *
 * @file
 * @brief           linker script for cc13x2 MCUs
 *
 * @}
 */

/* Memory Space Definitions: */
MEMORY
{
    rom (rx)   : ORIGIN = 0x00000000, LENGTH = 0x58000
    gpram      : ORIGIN = 0x11000000, LENGTH = 8K /* configurable as cache. 20K here, 8K there, and 2K in the ld-script of cc13x2ware */
    ram (w!rx) : ORIGIN = 0x20000000, LENGTH = 20K /* sram */
}

INCLUDE cortexm_base.ld
