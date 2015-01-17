/* Memory Spaces Definitions */
#include "k20_memory.h"

MEMORY
{
    rom (rx)    : ORIGIN = CPU_APPLICATION_ROM_START, LENGTH = CPU_APPLICATION_ROM_SIZE
    ram (rwx)   : ORIGIN = CPU_SRAM_START, LENGTH = CPU_SRAM_SIZE
}

#include "k20_base.ld"
