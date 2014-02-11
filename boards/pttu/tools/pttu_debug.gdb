#winheight regs 11
set history save on
set history size 1000
target remote localhost:3333
monitor mt_internal_rc
load
compare-sections
monitor soft_reset_halt
set mem inaccessible-by-default off
monitor debug_level 0

break bootloader
continue
d b 1
