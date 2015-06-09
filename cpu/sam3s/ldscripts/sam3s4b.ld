MEMORY
{
	rom (rx)  : ORIGIN = 0x00400000, LENGTH = 256K /* flash, 256K */
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 48K /* sram, 48K */
}

INCLUDE cortexm_base.ld
