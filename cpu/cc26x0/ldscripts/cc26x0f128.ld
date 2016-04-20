/**
 * @addtogroup      cpu_cc26x0
 * @{
 *
 * @file
 * @brief           linker script for cc26x0f128 MCUs
 *
 * @}
 */

/* Memory Space Definitions: */
MEMORY
{
    rom  (rx ) : ORIGIN = 0x00000000, LENGTH = 0x00020000 - 88 /* technically, it's 128K */
    gpram  (rwx) : ORIGIN = 0x11000000, LENGTH = 8K /* configurable as cache. 20K here, 8K there, and 2K in the ld-script of cc26x0ware */
    ram    (rwx) : ORIGIN = 0x20000000, LENGTH = 20K /* sram */
}

INCLUDE cortexm_base.ld
